/*
  base/histogram.cc
  Displays histogram of a selected input trace.

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

#include <relacs/base/histogram.h>
using namespace relacs;

namespace base {


Histogram::Histogram( void )
  : Control( "Histogram", "base", "Jan Benda", "1.2", "Oct 14, 2015" )
{
  // parameter:
  InTrace = 0;
  Origin = 0;
  Offset = 0.0;
  Duration = 1.0;
  UpdateRange = 0;

  // add some options:
  addSelection( "intrace", "Input trace", "V-1" ).setFlags( 8 );
  addSelection( "origin", "Analysis window", "before end of data|before signal|after signal" );
  addNumber( "offset", "Offset of analysis window", Offset, -10000.0, 10000.0, 0.1, "s", "ms" );
  addNumber( "duration", "Width of analysis window", Duration, 0.0, 100.0, 0.1, "s", "ms" );
  addSelection( "updaterange", "Update x-range to", "current range|maximum range" );

  // layout:
  QVBoxLayout *vb = new QVBoxLayout;
  setLayout( vb );
  vb->setSpacing( 0 );
  SW.setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  vb->addWidget( &SW );

  // plot:
  P.lock();
  P.setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  P.setYRange( 0.0, Plot::AutoScale );
  P.unlock();
  vb->addWidget( &P );
}


void Histogram::preConfig( void )
{
  Parameter &p = *find( "intrace" );
  p.setText( traceNames() );
  p.setToDefault();
  if ( p.size() <= 1 )
    p.addFlags( 16 );
  else
    p.delFlags( 16 );

  SW.assign( this, 8, 16, true, 0, mutex() );
  SW.setMargins( 0 );
}


void Histogram::notify( void )
{
  SW.updateValues( OptWidget::changedFlag() );
  InTrace = index( "intrace" );
  Origin = index( "origin" );
  Offset = number( "offset" );
  Duration = number( "duration" );
  UpdateRange = index( "updaterange" );
  if ( InTrace >= 0 && InTrace < traces().size() ) {
    P.lock();
    P.setXLabel( trace( InTrace ).ident() + " [" + trace( InTrace ).unit() + "]" );
    P.unlock();
  }
}


void Histogram::main( void )
{
  sleep( Duration );
  if ( interrupt() )
    return;

  bool histinit = true;
  double histmin = 0.0;
  double histmax = 0.0;
  int previntrace = InTrace;

  do {

    if ( InTrace < 0 || InTrace >= traces().size() ) {
      warning( "Trace does not exist!", 4.0 );
      return;
    }

    if ( previntrace != InTrace ) {
      histinit = true;
      previntrace = InTrace;
      P.lock();
      P.resetRanges();
      P.unlock();
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

    float min = 0.0;
    float max = 0.0;
    trace( InTrace ).minMax( min, max, offsinx, offsinx+n );
    if ( ::fabs(max-min) < 1e-8 ) {
      min -= 1.0;
      max += 1.0;
    }
    
    int nbins = n/500;
    if ( n < 2 )
      n = 2;
    SampleDataD hist( min, max, (max-min)/nbins );
    trace( InTrace ).hist( hist, offsinx, offsinx+n );
    if ( histinit || UpdateRange == 0 ) {
      histinit = false;
      histmin = hist.rangeFront();
      histmax = hist.rangeBack();
    }
    else {
      if ( histmin > hist.rangeFront() )
	histmin = hist.rangeFront();
      if ( histmax < hist.rangeBack() )
	histmax = hist.rangeBack();
    }

    P.lock();
    P.clear();
    P.setXYGrid();
    P.setLabel( 0, "" );
    if ( ! P.zoomedXRange() )
      P.setXRange( histmin, histmax );
    P.plot( hist, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Box, 0, Plot::Yellow, Plot::Yellow );
    P.draw();
    P.unlock();

    if ( Origin > 0 )
      waitOnReProSleep();
    else
      sleep( Duration );

  } while ( ! interrupt() );
}


addControl( Histogram, base );

}; /* namespace base */

#include "moc_histogram.cc"
