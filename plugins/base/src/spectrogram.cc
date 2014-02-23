/*
  base/spectrogram.cc
  Displays a spectrogram of an input trace.

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

#include <relacs/base/spectrogram.h>
using namespace relacs;

namespace base {


Spectrogram::Spectrogram( void )
  : RePro( "Spectrogram", "base", "Jan Benda", "1.0", "Feb 10, 2013" )
{
  // add some options:
  addSelection( "intrace", "Input trace", "V-1" );
  addNumber( "width", "Width of powerspectrum  window", 0.1, 0.0, 100.0, 0.01, "s", "ms" );
  addNumber( "step", "Temporal increment for powerspectrum window", 0.0, 0.0, 100.0, 0.1, "s", "ms" );
  addNumber( "tmax", "Maximum time to plot in spectrogram", 10.0, 0.0, 100.0, 0.1, "s" );
  addNumber( "duration", "Total duration of spectrogram", 0.0, 0.0, 100000.0, 0.1, "s" );
  addSelection( "size", "Number of data points for FFT", "1024|64|128|256|512|1024|2048|4096|8192|16384|32768|65536|131072|262144|524288|1048576" );
  addBoolean( "overlap", "Overlap FFT windows within single powerspectrum", true );
  addSelection( "window", "FFT window function", "Hanning|Bartlett|Blackman|Blackman-Harris|Hamming|Hanning|Parzen|Square|Welch" );
  addBoolean( "powermax", "Power relative to maximum", true );
  addNumber( "fmax", "Maximum frequency", 2000.0, 0.0, 100000.0, 100.0, "Hz", "Hz" );
  addNumber( "pmax", "Maximum power", 0, -1000.0, 0.0, 10.0, "dB" );
  addNumber( "pmin", "Minimum power", -50, -1000.0, 0.0, 10.0, "dB" );

  // plot:
  P.lock();
  P.setXLabel( "Time [s]" );
  P.setXRange( 0.0, 10.0 );
  //  P.setXLabelPos( 1.0, Plot::Graph, -1.0, Plot::FirstAxis, Plot::Right, 0.0 );
  P.setYLabel( "Frequency [Hz]" );
  P.setYRange( 0.0, 1000.0 );
  //  p.setYLabelPos( -0.6, Plot::FirstAxis, 0.5, Plot::Graph, Plot::Center, -90.0 );
  P.unlock();

  setWidget( &P );
}


void Spectrogram::preConfig( void )
{
  setText( "intrace", traceNames() );
  setToDefault( "intrace" );
}


int Spectrogram::main( void )
{
  // get options:
  int intrace = index( "intrace" );
  double width = number( "width" );
  double step = number( "step" );
  double duration = number( "duration" );
  double tmax = number( "tmax" );
  int specsize = integer( "size" );
  bool overlap = boolean( "overlap" );
  int win = index( "window" );
  bool powermax = boolean( "powermax" );
  double fmax = number( "fmax" );
  double  pmax = number( "pmax" );
  double  pmin = number( "pmin" );

  double (*window)( int j, int n );
  switch ( win ) {
  case 0: window = bartlett; break;
  case 1: window = blackman; break;
  case 2: window = blackmanHarris; break;
  case 3: window = hamming; break;
  case 4: window = hanning; break;
  case 5: window = parzen; break;
  case 6: window = square; break;
  case 7: window = welch; break;
  default: window = hanning;
  }
  int n = trace( intrace ).indices( width );
  if ( n < 16 ) {
    printlog( "Not enough data points (n=" + Str( n ) + "<16) in width=" +
	      Str( width ) + "s !" );
    return Failed;
  }
  if ( step <= 0.0 )
    step = width;
  int repeats = duration <= 0 ? 0 : ::ceil( duration/step );

  // plot:
  P.lock();
  P.setXRange( 0.0, tmax );
  P.setYRange( 0.0, fmax );
  //  P.setXFallBackRange( 0.0, 10.0 );
  //  P.setYFallBackRange( 0.0, 1.0 );
  P.setZRange( 0.0, 1.0 );
  P.unlock();

  // data:
  const InData &data = trace( intrace );
  int lastindex = data.size();
  SampleData< SampleDataD > spectrogram( 0, 0.0, step );

  // don't print repro message:
  noMessage();

  // plot trace:
  tracePlotContinuous( width );

  for ( int count=0;
	( repeats <= 0 || count < repeats ) && softStop() == 0;
	count++ ) {
    // wait:
    sleep( step );
    if ( interrupt() )
      return Aborted;

    // get data:
    while ( lastindex + n < data.size() ) {
      SampleDataD d( n, 0.0, data.sampleInterval() );
      for ( int k=0; k<d.size(); k++ )
	d[k] = data[ lastindex+k ];
      d -= mean( d );
      spectrogram.push( SampleDataD( specsize ) );
      SampleDataD &spec = spectrogram.back();
      rPSD( d, spec, overlap, window );
      if ( powermax )
	spec.decibel();
      else
	spec.decibel( data.maxValue() * ::sqrt( 2.0 ) );
      for ( int k=0; k<spec.size(); k++ )
	spec[k] = ( spec[k] - pmin )/::fabs(pmax-pmin);
      lastindex += data.indices( step );
    }
    // clip data:
    while ( spectrogram.length() > tmax )
      spectrogram.erase( 0 );

    // plot:
    P.lock();
    P.clear();
    //    P.plot( spectrogram, 1.0, Plot::BlackBlueGreenRedWhiteGradient );
    P.plot( spectrogram, 1.0, Plot::BlackMagentaRedYellowWhiteGradient );
    P.draw();
    P.unlock();
  }

  return Completed;
}


addRePro( Spectrogram, base );

}; /* namespace base */

#include "moc_spectrogram.cc"
