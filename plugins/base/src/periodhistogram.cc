/*
  base/periodhistogram.cc
  Displays logarithmic histogram of dynamic clamp sampling intervals.

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

#include <relacs/base/periodhistogram.h>
using namespace relacs;

namespace base {


PeriodHistogram::PeriodHistogram( void )
  : Control( "PeriodHistogram", "base", "Jan Benda", "1.0", "Jun 12, 2018" ),
    P( 3 )
{
  // parameter:
  Duration = 1.0;

  // add some options:
  addNumber( "duration", "Width of analysis window", Duration, 0.0, 100.0, 0.1, "s", "ms" );

  // plot:
  P.lock();
  P.setMinimumWidth( 250 );
  P.setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Expanding );
  P.unlock();
  setWidget( &P );
}


void PeriodHistogram::notify( void )
{
  Duration = number( "duration" );
}


void PeriodHistogram::main( void )
{
  if ( traceIndex( "Interval" ) < 0 ) {
    warning( "Interval-trace does not exist!", 4.0 );
    return;
  }

  const InData &intrace = trace( "Interval" );

  int ndata = intrace.indices( Duration );
  double min = 0.99e6*intrace.sampleInterval();
  double max = 1.01e6*intrace.sampleInterval();
  int index = intrace.size();
  ArrayF intervals;
  ArrayD posintervals;
  ArrayD negintervals;
  ArrayD poscumulative;
  ArrayD negcumulative;

  P.lock();
  P[0].setOrigin( 0.0, 0.5 );
  P[0].setSize( 1.0, 0.5 );
  P[0].setLMarg( 5.0 );
  P[0].setTitle( "Intervals" );
  P[0].setTitlePos( 0.0, Plot::Graph, 0.0, Plot::SecondAxis, Plot::Left );
  P[0].setXLabel( intrace.ident() + " [" + intrace.unit() + "]" );
  P[0].setYLabel( "log10(count)");
  P[0].setYLabelPos( 1.5, Plot::FirstMargin, 0.5, Plot::Graph, Plot::Center, -90.0 );
  P[0].setYRange( -0.5, ::ceil( ::log10( ndata ) ) );
  P[0].setYTics( 1.0 );

  P[1].setOrigin( 0.0, 0.0 );
  P[1].setSize( 0.58, 0.5 );
  P[1].setLMarg( 5.0 );
  P[1].setBMarg( 4.5 );
  P[1].setTitle( "Shorter" );
  P[1].setTitlePos( 0.0, Plot::Graph, 0.0, Plot::SecondAxis, Plot::Left );
  P[1].setXRange( -2.0, 2.0 );
  P[1].setXTics( 1.0 );
  P[1].setYLabel( "log10(Fraction)");
  P[1].setYLabelPos( 1.5, Plot::FirstMargin, 0.5, Plot::Graph, Plot::Center, -90.0 );
  P[1].setYRange( 0.0, 100.0 );
  P[1].setYTics( 10.0 );
  P[1].setYRange( -4.0, 0.0 );
  P[1].setYTics( 1.0 );

  P[2].setOrigin( 0.58, 0.0 );
  P[2].setSize( 0.42, 0.5 );
  P[2].setLMarg( 0.0 );
  P[2].setBMarg( 4.5 );
  P[2].setTitle( "Longer" );
  P[2].setTitlePos( 0.0, Plot::Graph, 0.0, Plot::SecondAxis, Plot::Left );
  P[2].setXLabel( "log10(Jitter[us])" );
  P[2].setXRange( -2.0, 2.0 );
  P[2].setXTics( 1.0 );
  P[2].setYRange( 0.0, 100.0 );
  P[2].setYTics( 10.0 );
  P[2].setYRange( -4.0, 0.0 );
  P[2].setYTics( 1.0 );
  P[2].setFormatY( "" );
  P.unlock();

  sleep( Duration );
  if ( interrupt() )
    return;

  do {
    ndata = intrace.indices( Duration );
    if ( intrace.size() > index + ndata/4 ) {
      int nbins = ndata/500;
      if ( nbins < 2 )
	nbins = 2;

      // get interval data:
      intrace.copy( index, intrace.size(), intervals );

      // min/max:
      float cmin = 0.0;
      float cmax = 0.0;
      intervals.minMax( cmin, cmax );
      if ( max < cmax )
	max = cmax;
      if ( min > cmin )
	min = cmin;
    
      // histogram:
      SampleDataD hist( min, max, (max-min)/nbins );
      hist.hist( intervals );

      // cumulative differences:
      intervals -= 1e6*intrace.sampleInterval();
      sort( intervals.begin(), intervals.end() );
      posintervals.clear();
      negintervals.clear();
      poscumulative.clear();
      negcumulative.clear();
      for ( auto i = intervals.begin(); i != intervals.end(); ++i ) {
	if ( *i >= 0.0 ) {
	  posintervals.push( double( *i ) );
	  poscumulative.push( poscumulative.size()+1.0 );
	}
	else {
	  negintervals.push( - double( *i ) );
	  negcumulative.push( negcumulative.size()+1.0 );
	}
      }
      poscumulative *= 1.0/poscumulative.size();
      negcumulative *= 1.0/negcumulative.size();
      reverse( negintervals.begin(), negintervals.end() );
      reverse( negcumulative.begin(), negcumulative.end() );

      P.lock();
      P[0].clear();
      if ( ! P[0].zoomedXRange() )
	P[0].setXRange( min, max );
      P[0].setXRange( min, max );
      P[0].plot( log10(hist), 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Box, 0, Plot::Yellow, Plot::Yellow );
      P[0].plotVLine( 1e6*intrace.sampleInterval(), Plot::Red, 2, Plot::Solid );
      P[1].clear();
      P[1].plot( log10(negintervals), log10(negcumulative), Plot::Orange, 2, Plot::Solid );
      P[2].clear();
      P[2].plot( log10( posintervals ), log10(1.0-poscumulative), Plot::Orange, 2, Plot::Solid );
      P.draw();
      P.unlock();
    }

    index = intrace.size();

    sleep( Duration );

  } while ( ! interrupt() );
}


addControl( PeriodHistogram, base );

}; /* namespace base */

#include "moc_periodhistogram.cc"
