/*
  base/spectrumanalyzer.cc
  Displays the spectrum of the voltage traces.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

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

#include <QDateTime>
#include <QVBoxLayout>
#include <relacs/sampledata.h>
#include <relacs/stats.h>
#include <relacs/spectrum.h>
#include <relacs/base/spectrumanalyzer.h>
using namespace relacs;

namespace base {


SpectrumAnalyzer::SpectrumAnalyzer( void )
  : Control( "SpectrumAnalyzer", "base", "Jan Benda", "1.4", "Jul 13, 2018" )
{
  // parameter:
  InTrace = 0;
  Origin = 0;
  Offset = 0.0;
  Duration = 1.0;
  SpecSize = 1024;
  Overlap = true;
  Window = hanning;
  Decibel = true;
  Peak = true;
  FMax = 500.0;
  PMin = -50.0;

  // options:
  addSelection( "intrace", "Input trace", "V-1" ).setFlags( 8 );
  addSelection( "origin", "Analysis window", "before end of data|before signal|after signal" );
  addNumber( "offset", "Offset of analysis window", Offset, -10000.0, 10000.0, 0.1, "s", "ms" );
  addNumber( "duration", "Width of analysis window", Duration, 0.0, 100.0, 0.1, "s", "ms" );
  addNumber( "resolution", "Frequency resolution of power spectrum", 10.0, 0.0, 1000.0, 1.0, "Hz" );
  addBoolean( "overlap", "Overlap FFT windows", Overlap );
  addSelection( "window", "FFT window function", "Hanning|Bartlett|Blackman|Blackman-Harris|Hamming|Hanning|Parzen|Square|Welch" );
  addNumber( "fmax", "Maximum frequency", FMax, 0.0, 100000.0, 100.0, "Hz", "Hz" );
  addBoolean( "decibel", "Plot decibel relative to maximum", Decibel );
  addBoolean( "peak", "Decibel relative to maximum peak", Peak ).setActivation( "decibel", "true" );
  addNumber( "pmin", "Minimum power", PMin, -1000.0, 0.0, 10.0, "dB" ).setActivation( "decibel", "true" );

  // layout:
  QVBoxLayout *vb = new QVBoxLayout;
  setLayout( vb );
  vb->setSpacing( 0 );
  SW.setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  vb->addWidget( &SW );

  // plot:
  P.lock();
  P.setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  P.setXLabel( "Frequency [Hz]" );
  P.setXRange( 0.0, FMax );
  P.setYLabel( "Power [dB]" );
  P.setYRange( PMin, 0.0 );
  P.setLabel( "", 0.1, Plot::Graph, 0.5, Plot::Graph, Plot::Left,
	      0.0, Plot::Red, 5.0 );
  P.unlock();
  vb->addWidget( &P );
}


SpectrumAnalyzer::~SpectrumAnalyzer( void )
{
}


void SpectrumAnalyzer::preConfig( void )
{
  Parameter &p = *find( "intrace" );
  p.setText( traceNames() );
  p.setToDefault();
  if ( p.size() <= 1 )
    p.addFlags( 16 );
  else
    p.delFlags( 16 );

  lock();
  P.lock();
  if ( Decibel )
    P.setYLabel( "Power [dB]" );
  else
    P.setYLabel( trace( InTrace ).ident() + " [" + trace( InTrace ).unit() + "]" );
  P.unlock();
  unlock();

  SW.assign( this, 8, 16, true, 0, mutex() );
  SW.setMargins( 0 );
}


void SpectrumAnalyzer::notify( void )
{
  SW.updateValues( OptWidget::changedFlag() );
  InTrace = index( "intrace" );
  Origin = index( "origin" );
  Offset = number( "offset" );
  Duration = number( "duration" );
  Resolution = number( "resolution" );
  Overlap = boolean( "overlap" );
  if ( InTrace >= 0 && InTrace < traces().size() ) {
    SpecSize = nextPowerOfTwo( trace( InTrace ).indices( 1.0/Resolution ) );
    if ( Overlap && Duration < 1.5*trace( InTrace ).interval( SpecSize ) )
      Duration = 1.5*trace( InTrace ).interval( SpecSize );
    else if ( ! Overlap && Duration < 2.0*trace( InTrace ).interval( SpecSize ) )
      Duration = 2.0*trace( InTrace ).interval( SpecSize );
  }
  else
    SpecSize = 0;
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
  Peak = boolean( "peak" );
  FMax = number( "fmax" );
  PMin = number( "pmin" );
  P.lock();
  P.setXRange( 0.0, FMax );
  if ( Decibel ) {
    P.setYLabel( "Power [dB]" );
    P.setYRange( PMin, 0.0 );
  }
  else {
    if ( SpecSize > 0 )  // valid InTrace
      P.setYLabel( trace( InTrace ).ident() + " [" + trace( InTrace ).unit() + "]" );
    P.setYRange( 0.0, Plot::AutoScale );
  }
  P.unlock();
}


void SpectrumAnalyzer::main( void )
{
  sleep( Duration );

  do {

    if ( InTrace < 0 || InTrace >= traces().size() ) {
      warning( "Trace does not exist!", 4.0 );
      return;
    }

    int n = trace( InTrace ).indices( Duration );
    int offsinx = 0;
    if ( Origin == 1 )
      offsinx = trace( InTrace ).index( signalTime() - Offset - Duration );
    else if ( Origin == 2 )
      offsinx = trace( InTrace ).index( signalTime() + Offset );
    else
      offsinx = trace( InTrace ).index( currentTime() - Offset - Duration );
    if ( offsinx < trace( InTrace ).minIndex() )
      offsinx = trace( InTrace ).minIndex();
    if ( offsinx + n > trace( InTrace ).currentIndex() )
      n = trace( InTrace ).currentIndex() - offsinx;

    if ( n < 64 ) {
      printlog( "Not enough data points (n=" + Str( n ) + ")!" );
      P.lock();
      P.clear();
      P.noXYGrid();
      P.setLabel( 0, "Not enough data points (n=" + Str( n ) + ")!" );
      P.draw();
      P.unlock();
      if ( Origin > 0 )
	waitOnReProSleep();
      else {
	QTime time;
	time.start();
	do {
	  waitOnData();
	} while ( 0.001*time.elapsed() < Duration && ! interrupt() );
      }
      if ( interrupt() )
	return;
      else
	continue;
    }

    SampleDataD d( n, 0.0, trace( InTrace ).sampleInterval() );
    for ( int k=0; k<d.size(); k++ )
      d[k] = trace( InTrace )[ offsinx+k ];
    d -= mean( d );

    SampleDataD spec( SpecSize );
    rPSD( d, spec, Overlap, Window );
    if ( Decibel )
      if ( Peak )
	spec.decibel();
      else
	spec.decibel( trace( InTrace ).maxValue() );
    else
      spec.sqrt();

    P.lock();
    P.clear();
    P.setXYGrid();
    P.setLabel( 0, "" );
    P.plot( spec, 1.0, Plot::Yellow, 2, Plot::Solid );
    P.draw();
    P.unlock();

    if ( Origin > 0 )
      waitOnReProSleep();
    else {
      QTime time;
      time.start();
      do {
	waitOnData();
      } while ( 0.001*time.elapsed() < Duration && ! interrupt() );
    }

  } while ( ! interrupt() );

}


addControl( SpectrumAnalyzer, base );

}; /* namespace base */

#include "moc_spectrumanalyzer.cc"
