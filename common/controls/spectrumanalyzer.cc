/*
  spectrumanalyzer.cc
  Displays the spectrum of the voltage traces.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <relacs/sampledata.h>
#include <relacs/stats.h>
#include <relacs/spectrum.h>
#include <relacs/common/spectrumanalyzer.h>

using namespace numerics;


SpectrumAnalyzer::SpectrumAnalyzer( void )
  : Control( "SpectrumAnalyzer", "Spectrum", "Common",
	     "Jan Benda", "1.0", "Mar 1, 2005" ),
    P( this )
{
  // parameter:
  Trace = 0;
  Offset = -1.0;
  Duration = 1.0;
  SpecSize = 1024;
  Overlap = true;
  Window = numerics::hanning;
  Decibel = true;
  FMax = 500.0;
  PMin = -50.0;

  // options:
  addNumber( "trace", "Input trace number", Trace, 0, 1000 );
  addNumber( "offset", "Offset of analysis window relativ to signal", Offset, -1000.0, 1000.0, 0.1, "s", "ms" );
  addNumber( "duration", "Width of analysis window", Duration, 0.0, 100.0, 0.1, "s", "ms" );
  addSelection( "size", "Number of data points for FFT", "1024|64|128|256|512|1024|2048|4096|8192|16384|32768|65536|131072|262144|524288|1048576" );
  addBoolean( "overlap", "Overlap FFT windows", Overlap );
  addSelection( "window", "FFT window function", "Hanning|Bartlett|Blackman|Blackman-Harris|Hamming|Hanning|Parzen|Square|Welch" );
  addNumber( "fmax", "Maximum frequency", FMax, 0.0, 100000.0, 100.0, "Hz", "Hz" );
  addBoolean( "decibel", "Plot decibel relative to maximum", Decibel );
  addNumber( "pmin", "Minimum power", PMin, -1000.0, 0.0, 10.0, "dB" ).setActivation( "decibel", "true" );

  // plot:
  P.lock();
  P.setXLabel( "Frequency [Hz]" );
  P.setXRange( 0.0, FMax );
  P.setYLabel( "Power [dB]" );
  P.setYRange( PMin, 0.0 );
  P.setLabel( "", 0.1, Plot::Graph, 0.5, Plot::Graph, Plot::Left,
	      0.0, Plot::Red, 5.0 );
  P.unlock();

}


SpectrumAnalyzer::~SpectrumAnalyzer( void )
{
}


void SpectrumAnalyzer::config( void )
{
  lock();
  P.lock();
  if ( Decibel )
    P.setYLabel( "Power [dB]" );
  else
    P.setYLabel( trace( Trace ).ident() + " [" + trace( Trace ).unit() + "]" );
  P.unlock();
  unlock();
}


void SpectrumAnalyzer::notify( void )
{
  Trace = integer( "trace" );
  Offset = number( "offset" );
  Duration = number( "duration" );
  SpecSize = integer( "size" );
  Overlap = boolean( "overlap" );
  int win = index( "window" );
  switch ( win ) {
  case 0: Window = bartlett; break;
  case 1: Window = blackman; break;
  case 2: Window = blackmanHarris; break;
  case 3: Window = hamming; break;
  case 4: Window = hanning; break;
  case 5: Window = parzen; break;
  case 6: Window = square; break;
  case 7: Window = welch; break;
  default: Window = hanning;
  }
  Decibel = boolean( "decibel" );
  FMax = number( "fmax" );
  PMin = number( "pmin" );
  P.lock();
  P.setXRange( 0.0, FMax );
  if ( Decibel ) {
    P.setYLabel( "Power [dB]" );
    P.setYRange( PMin, 0.0 );
  }
  else {
    if ( traces().size() > 0 )
      P.setYLabel( trace( Trace ).ident() + " [" + trace( Trace ).unit() + "]" );
    else
      P.setYLabel( "Amplitude" );
    P.setYRange( 0.0, Plot::AutoScale );
  }
  P.unlock();
}


void SpectrumAnalyzer::main( void )
{
  double offs = Offset;
  sleep( -offs );

  do { 

    if ( Trace < 0 || Trace >= traces().size() ) {
      warning( "Trace does not exist!", 4.0 );
      return;
    }

    int offsinx = trace( Trace ).signalIndex() + trace( Trace ).indices( Offset );
    if ( offsinx < trace( Trace ).minIndex() )
      offsinx = trace( Trace ).minIndex();
    int maxinx = trace( Trace ).currentIndex() - offsinx - 1;
    int durinx = trace( Trace ).indices( Duration );
    int n = durinx < maxinx ? durinx : maxinx;

    if ( n < 64 ) {
      printlog( "Not enough data points (n=" + Str( n ) + ")!" );
      P.lock();
      P.clear();
      P.noXYGrid();
      P.setLabel( 0, "Not enough data points (n=" + Str( n ) + ")!" );
      P.unlock();
      P.draw();
      if ( waitOnReProSleep() )
	return;
      else
	continue;
    }

    SampleDataD d( n, 0.0, trace( Trace ).sampleInterval() );
    for ( int k=0; k<d.size(); k++ )
      d[k] = trace( Trace )[ offsinx+k ];
    d -= mean( d );

    SampleDataD spec( SpecSize );
    rPSD( d, spec, Overlap, Window );
    if ( Decibel )
      spec.decibel();
    else
      spec.sqrt();

    P.lock();
    P.clear();
    P.setXYGrid();
    P.setLabel( 0, "" );
    P.plot( spec, 1.0, Plot::Yellow, 2, Plot::Solid );
    P.unlock();
    P.draw();

    waitOnReProSleep();

  } while ( ! interrupt() );

}


addControl( SpectrumAnalyzer );

#include "moc_spectrumanalyzer.cc"
