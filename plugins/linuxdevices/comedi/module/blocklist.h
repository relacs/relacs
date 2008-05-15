/* 
 *  BLOCK-LIST DATA STRUCTURE *

  - ring-list of Buffers with pointers to current read and write position
  - block size per buffer: DATABLOCKSIZE bytes
  - number of elements per buffer: DATABLOCKSIZE/sampleSize

*/


#ifdef __KERNEL__
#include <linux/spinlock.h>

#include "moduledef.h"
#define mem_alloc      vmalloc        //malloc
#define mem_free       vfree          //free
#define copy_to_buff   copy_to_user   //memcpy
#define copy_from_buff copy_from_user //memcpy

#else
#define DATABLOCKSIZE  65536
#define mem_alloc      malloc
#define mem_free       free
#define copy_to_buff   memcpy
#define copy_from_buff memcpy
#endif


#define E_OVERFLOW -11
#define E_UNDERRUN -12



struct dataBlockListT {
  void *data; // multiplexed channel data (float or lsampl_t)
  struct dataBlockListT *next;
};

struct BlockDataT {
  struct dataBlockListT *readBlock;
  struct dataBlockListT *writeBlock;
  unsigned int readBlockPos;
  unsigned int writeBlockPos;
  unsigned int sampleSize;
  unsigned int chanN;
  unsigned int blockN;
  spinlock_t spinlock;  
};



// TODO: MAKE THIS TYPE-INDEPENDENT
#define WRITEDATA( BDATA, CHAN  ) ((float*)(BDATA).writeBlock->data)[ \
					     (BDATA).writeBlockPos+CHAN]

#define READDATAERROR( BDATA ) \
   (  (BDATA).readBlock == (BDATA).writeBlock &&                            \
      (BDATA).readBlockPos <= (BDATA).writeBlockPos &&                      \
      (BDATA).readBlockPos + (BDATA).chanN - 1 >= (BDATA).writeBlockPos  )
  
#define WRITEDATAERROR( BDATA ) \
  (   (BDATA).writeBlock == (BDATA).readBlock &&			    \
      (BDATA).writeBlockPos < (BDATA).readBlockPos &&                       \
      (BDATA).writeBlockPos + (BDATA).chanN - 1 >= (BDATA).readBlockPos )

#define READDATA( BDATA, CHAN ) ((float*)(BDATA).readBlock->data)[ \
					     (BDATA).readBlockPos+CHAN]
/*
#define MOVE_INDATAPOS( BDATA, ERROR )  moveWritePos( &(BDATA), &(ERROR) )

#define MOVE_OUTDATAPOS( BDATA, ERROR ) moveReadPos( &(BDATA), &(ERROR) )
*/

#define MOVE_WRITEPOS( BDATA ) {  				    \
  (BDATA).writeBlockPos += (BDATA).chanN;			  	    \
  if( (BDATA).writeBlockPos + (BDATA).chanN - 1 >=                          \
      DATABLOCKSIZE/(BDATA).sampleSize ) {                                  \
    (BDATA).writeBlock = (BDATA).writeBlock->next;                          \
    (BDATA).writeBlockPos = 0;                                              \
  }                                                                         \
}

#define MOVE_READPOS( BDATA ) {				            \
  (BDATA).readBlockPos += (BDATA).chanN;			  	    \
  if( (BDATA).readBlockPos + (BDATA).chanN - 1 >=                           \
      DATABLOCKSIZE/(BDATA).sampleSize ) {                                  \
    (BDATA).readBlock = (BDATA).readBlock->next;                            \
    (BDATA).readBlockPos = 0;                                               \
  }         \
}


/* copy of MOVE_INDATAPOS as function */
void moveWritePos( struct BlockDataT *bData, int *error );

/* copy of MOVE_OUTDATAPOS as function */
void moveReadPos( struct BlockDataT *bData, int *error );


/* Move the content of the block of the current reading-position to buff, 
   but not more than buffSize bytes and only as much data as currently 
   available inside the block-list.
   If the block is emptied, the current reading block position moves one block
   further.
   return-value: number of bytes copied */
int popBlockToBuff( struct BlockDataT *bData, void *buff, ssize_t buffSize );

/* Move the content of buff to the block of the current writing-position,
   but not more than buffSize bytes and only as much data as still fit into
   the block-list.
   If the block is filled up, the current writing block position moves one block
   further.
   return-value: number of bytes which fitted into the current block */
int pushBuffToBlock( struct BlockDataT *bData, void *buff, ssize_t buffSize );


/* creates a ringl-list of blockN data blocks pointed to by bData
   return value: number of Blocks created */
int createBlockList( struct BlockDataT *bData, int blockN, 
		     unsigned int chanN, unsigned int sampleSize );

/* delete the list of blocks pointed to by bData */
void deleteBlockList( struct BlockDataT *bData );
