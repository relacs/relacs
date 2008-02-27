/* 
gcc -ansi -o data data.c 
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>


void getStr( char *s, char *ident, char *dat )
{
  char *sp;

  if ( strstr( s, ident ) != NULL )
    {
      sp = strchr( s, ':' );
      if ( sp != NULL )
	{
	  sp+=2;
	  strcpy( dat, sp );
	  for ( sp = dat + strlen( dat ) -1; sp > dat && isspace( *sp ); sp-- );
	  *(++sp) = '\0';
	}
      else
	dat[0] = '\0';
    }
}


void extract( FILE *F, char *dir )
{
  char s[200];
  FILE *I;
  char file[100]="", date[100]="", cell[100]="";
  char species[100]="", scientist[100]="";

  sprintf( s, "%s/%s.inf", dir, dir );
  I = fopen( s, "r" );
  if ( I == NULL )
    return;

  while( fgets( s, 200, I ) != NULL ) 
    {
      getStr( s, "file", file );
      getStr( s, "date", date );
      getStr( s, "cell type", cell );
      getStr( s, "species", species );
      getStr( s, "scientist", scientist );
    }
  fclose( I );
  fprintf( F, "%-15s  %-10s  %-30s  %-30s  %-20s\n",
	   file, date, cell, species, scientist );
}


int main( int argc, char *argv[] )
{
  FILE *F;
  int k;

  F = fopen( "data.txt", "w" );
  for ( k=1; k<argc; k++ )
    extract( F, argv[k] );
  fclose( F );
  return 0;
}
