/*
  checkao.cc
  

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  RELACS is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/*
gcc -Wall -o checkao checkao.cc -I hardware/ -I datahandling/ -I relacsmodules/ -I/usr/lib/qt-2.3.0/include/ ATT.o ATTParPort.o ATTSim.o DAQNIE.o DAQSim.o mDAQSim.o ReadWrite.o mReadWrite.o IOData.o HardWare.o SampleData.o -lqt -L/usr/lib/qt-2.3.0/lib/
*/
#include <cstdlib>
#include <cstdio>
#include <getopt.h>
#include <iostream>
#include <iomanip>

#include <relacs/ReadWrite.hh>
#include <relacs/IOData.hh>

int aochannel = 0;
double aocarrier = 5000.0;
double aointensity = 80.0;
int aichannel = 0;
int aigain = 0;
long airate = 100000;
int filec;


void RecordSignal( const char *aofile, const char *aifile )
{
  // load signal:  
  OData signal( 2, 1 );
  if ( signal.loadSignal( aofile, aofile, aocarrier ) )
    {
      fprintf( stderr, "can't open file %s!\n", aofile );
      //      return;
    }
  signal.setChannel( aochannel );
  signal.setIntensity( aointensity );

  ReadWrite RW( AIAOATT, airate, aichannel, aigain, 0, 5000000, false, true );

  RW.write( signal );

  RW.sender();

  RW.trace()->writeText( aifile, 6, 2, 0.000001 );
}

 
void WriteUsage()

{
  fprintf( stderr, "\nusage:\n" );
  fprintf( stderr, "\n" );
  fprintf( stderr, "checkao -c # -f # -i # -a # -g # -s # signal data\n" );
  fprintf( stderr, "\n" );
  fprintf( stderr, "play signal from ascii-file <signal> and store at the same\n" );
  fprintf( stderr, "time data from analog input into file <data>.\n" );
  fprintf( stderr, "-c : output channel of signal\n" );
  fprintf( stderr, "-f : carrier frequency of output signal in kHz (default 5)\n" );
  fprintf( stderr, "-i : output intensity in dB (default 70)\n" );
  fprintf( stderr, "-a : input channel of recording (default 0)\n" );
  fprintf( stderr, "-g : gain of input channel (default 0)\n" );
  fprintf( stderr, "-s : sampling rate of recording in kHz (default 100)\n" );
  fprintf( stderr, "\n" );
  exit( 1 );
}


void ReadArgs( int argc, char *argv[] )
{
  int c;
  double v;

  if ( argc <= 1 )
    WriteUsage();
  optind = 0;
  opterr = 0;
  while ( (c = getopt( argc, argv, "c:f:i:a:g:s:" )) >= 0 )
    switch ( c ) {
      case 'c': if ( optarg == NULL || sscanf( optarg, "%d", &aochannel ) == 0 )
		  aochannel = 0;
                break;
      case 'f': if ( optarg == NULL || sscanf( optarg, "%lf", &aocarrier ) == 0 )
		  aocarrier = 5.0;
   	        aocarrier *= 1000.0;
                break;
      case 'i': if ( optarg == NULL || sscanf( optarg, "%lf", &aointensity ) == 0 )
		  aointensity = 80.0;
                break;
      case 'a': if ( optarg == NULL || sscanf( optarg, "%d", &aichannel ) == 0 )
		  aichannel = 0;
                break;
      case 'g': if ( optarg == NULL || sscanf( optarg, "%d", &aigain ) == 0 )
		  aigain = 0;
                break;
      case 's': if ( optarg == NULL || sscanf( optarg, "%lf", &v ) == 0 )
		  v = 100.0;
   	        airate = long( 1000.0*v );
                break;
      default : WriteUsage();
    }
  if ( optind >= argc || argv[optind][0] == '?' )
    WriteUsage();
  filec = optind;
  if ( filec >= argc )
    WriteUsage();
}


int main( int argc, char *argv[] )
{
  ReadArgs( argc, argv );
  RecordSignal( argv[filec], filec+1 < argc ? argv[filec+1] : "" );
  return( 0 );
}
