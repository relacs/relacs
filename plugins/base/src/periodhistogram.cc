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
    P( 2 )
{
  // parameter:
  Duration = 1.0;
  MaxJitter = 10.0;

  // add some options:
  addNumber( "duration", "Width of analysis window", Duration, 0.0, 100.0, 0.1, "s", "ms" );
  addNumber( "maxjitter", "Maximum jitter shown in histogram", MaxJitter, 0.0, 100.0, 0.1, "us" );

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
  MaxJitter = number( "maxjitter" );
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
  P[0].setLMarg( 5.0 );
  P[0].setTitle( "Interval Histogram @ " + Str( 0.001*intrace.sampleRate(), "%.0f" ) + "kHz" );
  P[0].setTitlePos( 0.0, Plot::Graph, 0.0, Plot::SecondAxis, Plot::Left );
  P[0].setXLabel( intrace.ident() + " [" + intrace.unit() + "]" );
  P[0].setYLabel( "log10(count)");
  P[0].setYLabelPos( 1.5, Plot::FirstMargin, 0.5, Plot::Graph, Plot::Center, -90.0 );
  P[0].setYRange( -0.5, ::ceil( ::log10( ndata ) ) );
  P[0].setYTics( 1.0 );

  P[1].setLMarg( 5.0 );
  P[1].setTitle( "Cumulative of Jitter" );
  P[1].setTitlePos( 0.0, Plot::Graph, 0.0, Plot::SecondAxis, Plot::Left );
  P[1].setXLabel( "log10(Jitter[us])" );
  P[1].setXRange( -3.0, 1.0 );
  P[1].setXTics( 1.0 );
  P[1].setYLabel( "log10(Fraction)");
  P[1].setYLabelPos( 1.5, Plot::FirstMargin, 0.5, Plot::Graph, Plot::Center, -90.0 );
  P[1].setYRange( -5.0, 0.0 );
  P[1].setYTics( 1.0 );
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

      // mean interval:
      double meaninterval = intervals.mean();
      double expectedinterval = 1e6*intrace.sampleInterval();

      // min/max:
      float cmin = 0.0;
      float cmax = 0.0;
      intervals.minMax( cmin, cmax );
    
      // histogram:
      SampleDataD hist( cmin, cmax, (cmax-cmin)/nbins );
      hist.hist( intervals );

      // plot range:
      if ( max < cmax )
	max = cmax;
      if ( min > cmin )
	min = cmin;
      // symmetric:
      if ( max - meaninterval > meaninterval - min )
	min = meaninterval - (max - meaninterval);
      else
	max = meaninterval + (meaninterval - min);
      // maximal:
      double hmin = meaninterval - MaxJitter;
      if ( hmin < 0.0 )
	hmin = 0.0;
      double hmax = (meaninterval + MaxJitter)*1.0001;
      if ( max > hmax )
	max = hmax;
      if ( min < hmin )
	min = hmin;
      // include expected interval:
      if ( expectedinterval < min )
	min = expectedinterval - 0.5;
      if ( expectedinterval > max )
	max = expectedinterval + 0.5;

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
      P[0].plot( log10(hist), 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Box, 0, Plot::Yellow, Plot::Yellow );
      P[0].plotVLine( 1e6*intrace.sampleInterval(), Plot::Red, 2, Plot::Solid );
      P[1].clear();
      P[1].plot( log10(negintervals), log10(negcumulative), Plot::Orange, 2, Plot::Solid );
      P[1].plot( log10( posintervals ), log10(1.0-poscumulative), Plot::Yellow, 2, Plot::Solid );
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
