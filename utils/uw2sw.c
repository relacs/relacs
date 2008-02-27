/*
gcc -o uw2sw uw2sw.c
 */

#include <stdio.h>

int main( int argc, char *argv[] )
{
  FILE *SF, *DF;
  #define N 1000
  unsigned short uwbuffer[N];
  signed short swbuffer[N];
  int usw;
  int n, k;

  if ( argc < 3 )
    {
      fprintf( stderr, "\nusage: uw2sw <unsigned word file> <signed word file>\n\n" );
      return 1;
    }
  SF = fopen( argv[1], "r" );
  if ( SF == NULL )
    {
      fprintf( stderr, "\ncan't open %s!\n", argv[1] );
      return 1;
    }
  DF = fopen( argv[2], "w" );
  if ( DF == NULL )
    {
      fprintf( stderr, "\ncan't open %s!\n", argv[2] );
      return 1;
    }

  do {
    n = fread( &uwbuffer, sizeof( short ), N, SF );
    for ( k=0; k<n; k++ )
      {
	usw = uwbuffer[k];
	usw -= 2048;
	swbuffer[k] = usw;
      }
    fwrite( &swbuffer, sizeof( short ), n, DF );
  } while ( n==N );
  fclose( SF );
  fclose( DF );
  return 0;
}
