#ifdef __KERNEL__
#include <asm/uaccess.h>
#include <linux/vmalloc.h>
#include <linux/string.h>

#else
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#endif

#include "blocklist.h"


void moveWritePos( struct BlockDataT *bData, int *error )
{
  // check buffer-overflow:
  if( bData->writeBlock == bData->readBlock &&
      bData->writeBlockPos < bData->readBlockPos &&
      bData->writeBlockPos + 2*bData->chanN - 1 >= bData->readBlockPos ) 
    *error = E_OVERFLOW;
  else
    bData->writeBlockPos += bData->chanN;

  // switch to next data block?
  if( bData->writeBlockPos + bData->chanN - 1 >= DATABLOCKSIZE/bData->sampleSize ) {
    bData->writeBlock = bData->writeBlock->next;
    bData->writeBlockPos = 0;
    // check buffer-overflow again:
    if( bData->writeBlock == bData->readBlock &&
	bData->writeBlockPos + bData->chanN - 1 >= bData->readBlockPos )
      *error = E_OVERFLOW;
  }
}


void moveReadPos( struct BlockDataT *bData, int *error )
{

  // check buffer-underrun:
  if( bData->readBlock == bData->writeBlock &&
      bData->readBlockPos <= bData->writeBlockPos &&
      bData->readBlockPos + 2*bData->chanN - 1 >= bData->writeBlockPos )
    *error = E_UNDERRUN;
  else
    bData->readBlockPos += bData->chanN;
  
  // switch to next data block?
  if( bData->readBlockPos + bData->chanN - 1 >= DATABLOCKSIZE/bData->sampleSize ) {
    bData->readBlock = bData->readBlock->next;
    bData->readBlockPos = 0;
    // check buffer-underrun again:
    if( bData->readBlock == bData->writeBlock &&
	bData->readBlockPos + bData->chanN - 1 >= bData->writeBlockPos )
      *error = E_UNDERRUN;
  }
}

int popBlockToBuff( struct BlockDataT *bData, void *buff, ssize_t buffSize )
{
  long bytesToCopy;
  unsigned long bytesFailed;
  int diffReadWriteBytes;
  struct dataBlockListT *writeBlockBefore, *writeBlock;

  if( !bData || !bData->readBlock || !bData->readBlock->data )
    return -1;

  // get atomic data of current write-position inside dynamic clamp task:
  //spin_lock( &bData->spinlock );
  do { 
    writeBlockBefore = bData->writeBlock;
    diffReadWriteBytes = (bData->writeBlockPos - bData->readBlockPos) * 
                         bData->sampleSize;
    writeBlock = bData->writeBlock;
  } while( writeBlockBefore != writeBlock );

  // calculate bytes ready to be copied:
  bytesToCopy = DATABLOCKSIZE - bData->readBlockPos * bData->sampleSize;
  if( bData->readBlock == writeBlock && diffReadWriteBytes >= 0 )
    bytesToCopy = diffReadWriteBytes;
  if( bytesToCopy > buffSize )
    bytesToCopy = buffSize;
  // only copy full sets of channel data:
  bytesToCopy -= (bytesToCopy/bData->sampleSize % bData->chanN )*bData->sampleSize;
  
  if( bytesToCopy <= 0 ) {
    printk( "popBlockToBuff: bytesToCopy=%d diff=%d rbp=%d wpp=%d rb=%p wb=%p\n",
	    bytesToCopy, diffReadWriteBytes, bData->readBlockPos, bData->writeBlockPos,
	    bData->readBlock, writeBlock );
    spin_unlock( &bData->spinlock );
    return 0;
  }

  // copy block content to buffer:
  bytesFailed = (unsigned long)copy_to_buff( buff, 
					     bData->readBlock->data 
					     + bData->readBlockPos * bData->sampleSize,
					     bytesToCopy );
  printk( "popBlockToBuff: bytesToCopy=%d  bytesFailed=%d\n", bytesToCopy, bytesFailed );
#ifndef __KERNEL__
  // copy_from_buff==memcpy
  bytesFailed = 0;
#endif
  bytesToCopy -= bytesFailed;

  // adjust current read position:
  bData->readBlockPos += bytesToCopy / bData->sampleSize;
  if( bData->readBlockPos + bData->chanN - 1 >= DATABLOCKSIZE/bData->sampleSize ) {
    printk( "popBlockToBuff: readBlock=%p readBlockNext=%p\n", bData->readBlock, bData->readBlock->next );
    bData->readBlock = bData->readBlock->next;
    bData->readBlockPos = 0;
  }

  if( bytesFailed )
    printk( "popBlockToBuff: bytes Failed: %u\n", bytesFailed );
 printk( "popBlockToBuff: bytesToCopy=%d  bytesFailed=%d\n", bytesToCopy, bytesFailed );

  //spin_unlock( &bData->spinlock );

  return bytesToCopy;
}

int pushBuffToBlock( struct BlockDataT *bData, void *buff, ssize_t buffSize )
{
  long bytesToCopy;
  unsigned long bytesFailed;
  int diffReadWriteBytes, useCnt;
  struct dataBlockListT *readBlockBefore, *readBlock;

  if( !bData || !bData->writeBlock || !bData->writeBlock->data )
    return -1;

  // get atomic data of current read-position inside dynamic clamp task:
  spin_lock( &bData->spinlock );
  do { 
    readBlockBefore = bData->readBlock;
    diffReadWriteBytes = (bData->readBlockPos - bData->writeBlockPos) * 
                          bData->sampleSize;
    readBlock = bData->readBlock;
  } while( readBlockBefore != readBlock );

  
  // calculate bytes ready to be copied:
  bytesToCopy = DATABLOCKSIZE - bData->writeBlockPos * bData->sampleSize;
  if( bData->writeBlock == readBlock && diffReadWriteBytes > 0 ) {
    bytesToCopy = diffReadWriteBytes;
  }
  if( bytesToCopy > buffSize ) {
    bytesToCopy = buffSize;
  }
  // only copy full sets of channel data:
  bytesToCopy -= (bytesToCopy/bData->sampleSize % bData->chanN )*bData->sampleSize;


  // copy buffer content to block:
  bytesFailed = (unsigned long)copy_from_buff( bData->writeBlock->data 
					       + bData->writeBlockPos * bData->sampleSize,
					       buff, 
					       bytesToCopy );

#ifndef __KERNEL__
  // copy_from_buff==memcpy
  bytesFailed = 0;
#endif
  bytesToCopy -= bytesFailed;

  // atomic(?) adjustment of current write position:
  bData->writeBlockPos += bytesToCopy / bData->sampleSize;
  if( bData->writeBlockPos + bData->chanN - 1 >= DATABLOCKSIZE/bData->sampleSize ) {
    bData->writeBlock = bData->writeBlock->next;
    bData->writeBlockPos = 0;
  }

  if( bytesFailed )
    printk( "popBlockToBuff: bytes Failed: %u\n", bytesFailed );
 
  spin_unlock( &bData->spinlock );

  return bytesToCopy;
}


int createBlockList( struct BlockDataT *bData, int blockN,
		     unsigned int chanN, unsigned int sampleSize )
{
  struct dataBlockListT *bListTmp = NULL;
  int blockCnt = 0;

  do {
    bListTmp = mem_alloc( sizeof(struct dataBlockListT ) );
    if( !bListTmp )
      return blockCnt;
    bListTmp->data = mem_alloc( DATABLOCKSIZE );
    if( !bListTmp->data ) {
      mem_free( bListTmp );
      bListTmp = NULL;
      return blockCnt;
    }
    if( blockCnt ) {
      bListTmp->next = bData->readBlock->next;
      bData->readBlock->next = bListTmp;
    }
    else {
      bData->readBlock = bListTmp;
      bData->readBlock->next = bData->readBlock;
    }
    blockCnt++;
  } while( blockCnt < blockN );
  bData->writeBlock = bData->readBlock;
  bData->readBlockPos = 0;
  bData->writeBlockPos = 0;
  bData->blockN = blockCnt;
  bData->chanN = chanN;
  bData->sampleSize = sampleSize;

  bListTmp = bData->readBlock->next;
  printk( "createBlockList: BlockCnt=%d, blockN=%d\n", blockCnt, blockN );
  printk( "CBL: %p  ", bListTmp );
  while( bListTmp != bData->readBlock && blockCnt > 0 ) {
    bListTmp = bListTmp->next;
    printk( "CBL%d: %p  ", blockCnt, bListTmp  );
    blockCnt--;
  }
  printk( "\n"  );

  spin_lock_init( &bData->spinlock );

  return blockCnt;
}


void deleteBlockList(  struct BlockDataT *bData )
{
  struct dataBlockListT *bListTmp = NULL;
  if( !bData || !bData->readBlock )
    return;
  while( bData->readBlock->next != bData->readBlock ) {
    bListTmp = bData->readBlock->next;
    bData->readBlock->next = bData->readBlock->next->next;
    mem_free( bListTmp );
  }
  bListTmp = bData->readBlock;
  bData->readBlock = NULL;
  bData->writeBlock = NULL;
  mem_free( bListTmp );
}
