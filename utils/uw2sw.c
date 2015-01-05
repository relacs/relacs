/*****************************************************************************
 *
 * uw2sw.c
 * 
 *
 * RELACS
 * Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
 * Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * RELACS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

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
