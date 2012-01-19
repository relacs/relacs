/*
  checkao.cc
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#include <cstdlib>
#include <getopt.h>
#include <iostream>
#include <relacs/indata.h>
#include <relacs/outdata.h>
#include <relacs/acquire.h>
#include <relacs/comedi/comedianaloginput.h>
#include <relacs/comedi/comedianalogoutput.h>
using namespace relacs;
using namespace comedi;

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
  OutData signal;
  signal.load( aofile, aofile, aocarrier );
  if ( signal.empty() ) {
    cerr << "can't read file " << aofile << "!\n";
    return;
  }
  signal.setChannel( aochannel );
  signal.setIntensity( aointensity );

  // setup hardware:
  ComediAnalogInput CAI( "/dev/comedi0", Options() );
  ComediAnalogOutput CAO( "/dev/comedi0", Options() );
  Acquire AQ;
  AQ.addInput( &CAI );
  AQ.addOutput( &CAO );

  // prepare reading:
  InData trace( (int)::rint( signal.length()*airate ), 1.0/airate );
  trace.setChannel( aichannel );
  trace.setGainIndex( aigain );
  InList traces;
  traces.add( &trace );

  // acquire:
  AQ.read( traces );
  AQ.write( signal );
  // XXX loop and fill up write buffer and read trace buffer?
  AQ.readData();

  // save data:
  ofstream df( aifile );
  for ( int k=0; k<trace.size(); k++ )
    df << trace.pos( k ) << "\t" << trace[k] << '\n';
}

 
void WriteUsage()

{
  cerr << "usage:\n";
  cerr << "\n";
  cerr << "checkao -c # -f # -i # -a # -g # -s # signal data\n";
  cerr << "\n";
  cerr << "play signal from ascii-file <signal> and store at the same\n";
  cerr << "time data from analog input into file <data>.\n";
  cerr << "-c : output channel of signal\n";
  cerr << "-f : carrier frequency of output signal in kHz (default 5)\n";
  cerr << "-i : output intensity in dB (default 70)\n";
  cerr << "-a : input channel of recording (default 0)\n";
  cerr << "-g : gain of input channel (default 0)\n";
  cerr << "-s : sampling rate of recording in kHz (default 100)\n";
  cerr << "\n";
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
    case 'c':
      if ( optarg == NULL || sscanf( optarg, "%d", &aochannel ) == 0 )
	aochannel = 0;
      break;
    case 'f':
      if ( optarg == NULL || sscanf( optarg, "%lf", &aocarrier ) == 0 )
	aocarrier = 5.0;
      aocarrier *= 1000.0;
      break;
    case 'i':
      if ( optarg == NULL || sscanf( optarg, "%lf", &aointensity ) == 0 )
	aointensity = 80.0;
      break;
    case 'a':
      if ( optarg == NULL || sscanf( optarg, "%d", &aichannel ) == 0 )
	aichannel = 0;
      break;
    case 'g':
      if ( optarg == NULL || sscanf( optarg, "%d", &aigain ) == 0 )
	aigain = 0;
      break;
    case 's':
      if ( optarg == NULL || sscanf( optarg, "%lf", &v ) == 0 )
	v = 100.0;
      airate = long( 1000.0*v );
      break;
    default :
      WriteUsage();
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
