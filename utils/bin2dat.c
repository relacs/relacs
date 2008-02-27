/*
gcc -o bin2dat bin2dat.c
 */


/*!
\page bin2dat bin2dat

bin2dat can be used to convert binary data as they are produced
by RELACS, and especially the IOData::writeBinary() functions,
into ascii data files.

\section bin2datusage USAGE
\c bin2dat \a <binfile> \a <datfile> \a <options>

The first argument \a <binfile> is the full specification of the file
containing the binary data. 
The second argument is the output file in which the ascii data are written.
The program tries to guess the data type of the binary file from its extension.
See IOData::binaryExtension() about the standrd extension.
The data type as well as the range of data can be specified by several \a options.

\section bin2datoptions OPTIONS
\arg \c -s 0 : The data are unsigned.
\arg \c -s 1 : The data are signed.
\arg \c -d \a x : The size of a single data element is \a x bytes.
               Only values of 1, 2, 4, 8 are allowed.
\arg \c -c \a x : Data from \a x channels are multiplexed in the file.
\arg \c -o \a x : Write data starting from byte offset \a x.
\arg \c -O \a x : Write data starting from byte offset \a x times the size of a single data element.
\arg \c -u \a x : Write data up to byte offset \a x.
\arg \c -U \a x : Write data up to byte offset \a x times the size of a single data element.
\arg \c -n \a x : Write \a x bytes of data.
\arg \c -N \a x : Write \a x data elements per channel.
\arg \c -T \a x : Write \a x divided by the time step (-t) lines of data.\n" );
\arg \c -t \a x : Add a time column to the ascii file with time step \a x.
\arg \c -v : Print settings to stderr.

\section bin2datexamples EXAMPLES

To convert the whole binary file \c data.sw1 into the ascii file \c signal.dat use
\code
bin2dat data.sw1 signal.dat
\endcode

To convert only data elements 5 to 10 use
\code
bin2dat data.sw1 signal.dat -O 5 -U 10
\endcode

Data element 5 of a \c sw1 -file (signed word) is at byte offset 10.
Data element 10 at 20.
So, alternatively 
\code
bin2dat data.sw1 signal.dat -o 10 -u 20
\endcode
can be used.

Write the first 100 bytes:
\code
bin2dat data.sw1 signal.dat -n 100
\endcode

Write 50 data elements starting at data element 20:
\code
bin2dat data.sw1 signal.dat -O 20 -N 50
\endcode

Write data elements 14 to 43 and add a time column with time step 0.1:
\code
bin2dat data.sw1 signal.dat -O 14 -U 43 -t 0.1
\endcode

Convert binary data file \c data.dat, which contains signed word (two bytes long)
data of three multiplexed channels into ascii-file \c signal.dat:
\code
bin2dat data.dat signal.dat -s 1 -d 2 -c 3
\endcode

 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <values.h>
#include <getopt.h>


char binfile[200] = "";
char datfile[200] = "signal.dat";
int datasize = 2;
int datasign = 1;
int datachannels = 1;
long offset = 0;
long ndata = LONG_MAX;
double deltat = 1.0;
int tcol = 0;


void extractData( void )
{
  FILE *BF;
  FILE *DF;
  char buffer[2048];
  long m, n, k, t;
  signed short *swp;
  /*  unsigned short *uwp;*/
  int c;

  BF = fopen( binfile, "r" );
  if ( BF == NULL )
    {
      fprintf( stderr, "can't open %s!\n", binfile );
      return;
    }
  fseek( BF, offset, SEEK_SET );
  DF = fopen( datfile, "w" );
  n = 0;
  t = 0;
  c = 0;
  do {
    m = fread( buffer, 1, 2048, BF );
    if ( n+m > ndata )
      m = ndata - n;
    if ( datasize == 2 && datasign == 1 )
      {
	swp = (signed short *)buffer;
	for ( k=0; k<m; k+=datasize )
	  {
	    if ( c>0 )
	      fprintf( DF, "  " );
	    else if ( tcol ) 
	      fprintf( DF, "%.7g  ", deltat*t );
	    fprintf( DF, "%5d", *swp );
	    c++;
	    if ( c >= datachannels ) 
	      {
		fprintf( DF, "\n" );
		c = 0;
		t++;
	      }
	    swp++;
	  }
      }
    else
      {
	fprintf( stderr, "sorry! Data format not supported.\n" );
	fprintf( stderr, "data sign: %s\n", datasign > 0 ? "signed" : "unsigned" );
	fprintf( stderr, "data size: %d\n", datasize );
	break;
      }
    n += m;
  } while ( m == 2048 && n<ndata);
  if ( c>0 )
    fprintf( DF, "\n" );
  fclose( DF );
  fclose( BF  );
}

 
void WriteUsage()

{
  fprintf( stderr, "\nusage:\n" );
  fprintf( stderr, "\n" );
  fprintf( stderr, "bin2dat <binfile> <datfile> [-o|O ## -u|U ## -n|N ## -T ## -t ## -s ## -d ## -c ## -v] \n" );
  fprintf( stderr, "\n" );
  fprintf( stderr, "save binary data from file <binfile> as ascii data in file <datfile>.\n" );
  fprintf( stderr, "-o : save data starting from byte offset ##.\n" );
  fprintf( stderr, "-O : save data starting from byte offset ## times size of data type.\n" );
  fprintf( stderr, "-u : save data upto byte offset ##.\n" );
  fprintf( stderr, "-U : save data upto byte offset ## times size of data type.\n" );
  fprintf( stderr, "-n : save at maximum ## bytes.\n" );
  fprintf( stderr, "-N : save at maximum ## lines (i.e. ## times size of data type times number of channels bytes).\n" );
  fprintf( stderr, "-T : save at maximum ## divided by stepsize (-t) lines of data.\n" );
  fprintf( stderr, "-t : add a time column with stepsize ##.\n" );
  fprintf( stderr, "Usually the type of the data contained in the binary file\n" );
  fprintf( stderr, "is determined from its extension. The following options can be\n" );
  fprintf( stderr, "used to specify the data type directly.\n" );
  fprintf( stderr, "-s : specify sign of the binary data (0=unsigned, 1=signed, default=signed).\n" );
  fprintf( stderr, "-d : specify size of the binary data type in bytes (1, 2, 4, 8, default=2).\n" );
  fprintf( stderr, "-c : specify number of channels multiplexed in the binary data file (default=1).\n" );
  fprintf( stderr, "-v : print settings to stderr.\n" );
  fprintf( stderr, "\n" );
  exit( 1 );
}


void ReadArgs( int argc, char *argv[] )
{
  int c;
  long upto = 0;
  int offsd = 0;
  int uptod = 0;
  int ndatad = 0;
  double time = 0.0;
  int setsign = 1;
  int setsize = 1;
  int setcol = 1;
  int showvals = 0;
  char *sp;

  if ( argc <= 1 )
    WriteUsage();
  optind = 0;
  opterr = 0;
  while ( (c = getopt( argc, argv, "o:O:u:U:n:N:T:t:s:d:c:v" )) >= 0 )
    switch ( c ) {
      case 'o': if ( optarg == NULL || sscanf( optarg, "%ld", &offset ) == 0 )
            	  offset = 0;
                break;
      case 'O': if ( optarg == NULL || sscanf( optarg, "%ld", &offset ) == 0 )
            	  offset = 0;
                offsd = 1;
                break;
      case 'u': if ( optarg == NULL || sscanf( optarg, "%ld", &upto ) == 0 )
            	  upto = 0;
                break;
      case 'U': if ( optarg == NULL || sscanf( optarg, "%ld", &upto ) == 0 )
            	  upto = 0;
                uptod = 1;
                break;
      case 'n': if ( optarg == NULL || sscanf( optarg, "%ld", &ndata ) == 0 )
          	  ndata = 0;
                break;
      case 'N': if ( optarg == NULL || sscanf( optarg, "%ld", &ndata ) == 0 )
            	  ndata = 0;
                ndatad = 1;
                break;
      case 'T': if ( optarg == NULL || sscanf( optarg, "%lf", &time ) == 0 )
            	  time = 0.0;
                break;
      case 't': if ( optarg == NULL || sscanf( optarg, "%lf", &deltat ) == 0 )
            	  deltat = 1.0;
                tcol = 1;
                break;
      case 's': if ( optarg == NULL || sscanf( optarg, "%d", &datasign ) == 0 )
            	  datasign = 1;
                setsign = 0;
                break;
      case 'd': if ( optarg == NULL || sscanf( optarg, "%d", &datasize ) == 0 )
            	  datasize = 2;
                setsize = 0;
                break;
      case 'c': if ( optarg == NULL || sscanf( optarg, "%d", &datachannels ) == 0 )
            	  datachannels = 1;
                setcol = 0;
                break;
      case 'v': showvals=1;
                break;
      default : WriteUsage();
    }
  if ( optind >= argc-1 || argv[optind][0] == '?' )
    WriteUsage();

  strcpy( binfile, argv[optind] );
  strcpy( datfile, argv[optind+1] );

  sp = strrchr( binfile, '.' );
  if ( sp != NULL )
    {
      sp++;
      if ( setsign )
	datasign = ( *sp == 's' );
      sp++;
      if ( setsize )
	{
	  if ( *sp == 'b' )
	    datasize = 1;
	  else if ( *sp == 'w' )
	    datasize = 2;
	  else if ( *sp == 'd' )
	    datasize = 4;
	  else if ( *sp == 'q' )
	    datasize = 8;
	}
      sp++;
      if ( setcol )
	{
	  datachannels = atoi( sp );
	  if ( datachannels <= 0 )
	    datachannels = 1;
	}
    }

  if ( offsd )
    offset *= datasize;
  if ( time > 0.0 ) {
    ndata = (int)floor( time / deltat );
    ndata *= datasize * datachannels;
  }
  else if ( ndatad )
    ndata *= datasize * datachannels;
  if ( uptod )
    upto *= datasize;
  if ( upto > 0 )
    ndata = upto - offset;

  if ( showvals )
    {
      fprintf( stderr, "binary file: %s\n", binfile );
      fprintf( stderr, "data file: %s\n", datfile );
      fprintf( stderr, "offset: %ld bytes\n", offset );
      fprintf( stderr, "ndata: %ld bytes\n", ndata );
      fprintf( stderr, "data sign: %s\n", datasign > 0 ? "signed" : "unsigned" );
      fprintf( stderr, "data size: %d\n", datasize );
      fprintf( stderr, "data columns: %d\n", datachannels );
      fprintf( stderr, "time column: %s\n", tcol ? "yes" : "no" );
      fprintf( stderr, "time step: %g\n", deltat );
    }
}


int main( int argc, char *argv[] )
{
  ReadArgs( argc, argv );  
  extractData();
  return 0;
}
