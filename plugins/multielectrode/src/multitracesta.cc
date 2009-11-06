/*
  multielectrode/multitracesta.cc
  Computes spike-triggered averages from all recorded input traces

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <relacs/stats.h>
#include <relacs/multielectrode/multitracesta.h>
using namespace relacs;

namespace multielectrode {


MultiTraceSTA::MultiTraceSTA( void )
  : RePro( "MultiTraceSTA", "Multiple Traces STA", "Multi-electrode",
	   "Jan Benda", "0.1", "Jan 21, 2009" ),
    P( this )
{
  // add some options:
  addSelection( "inspikes", "Input spike train", "Spikes-1" );
  addNumber( "interval", "Averaging interval", 1.0, 0.001, 100000.0, 0.001, "sec" );
  addInteger( "repeats", "Repeats", 100, 0, 10000, 1 );
  addNumber( "stamint", "Minimum STA time", -0.1, -1000.0, 1000.0, 0.01, "sec", "ms" );
  addNumber( "stamaxt", "Maximum STA time", 0.01, -1000.0, 1000.0, 0.01, "sec", "ms" );
  addBoolean( "plotsnippets", "Plot the individual snippets", true );

  // plot:
  boxLayout()->addWidget( &P );
}


int MultiTraceSTA::main( void )
{
  // get options:
  int inspikes = index( "inspikes", 0 );
  double interval = number( "interval" );
  int repeats = integer( "repeats" );
  double stamint = number( "stamint" );
  double stamaxt = number( "stamaxt" );
  bool plotsnippets = boolean( "plotsnippets" );

  // init STAs:
  STAs.resize( traces().size() );
  for ( unsigned int k=0; k<STAs.size(); k++ ) {
    STAs[k].Snippets.clear();                               // we have no data yet
    STAs[k].Snippets.reserve( (int)floor( interval*500 ) ); // reserve enough memory (assumes 500Hz spike rate)
    // the STA goes from stamint to stamaxt with the same sample interval as the corresponding input trace:
    STAs[k].Average.resize( stamint, stamaxt, trace( SpikeTrace[k] ).sampleInterval(), 0.0 );
    STAs[k].StDev.resize( stamint, stamaxt, trace( SpikeTrace[k] ).sampleInterval(), 0.0 );
  }

  // init plots:
  P.lock();
  P.clear();
  P.resize( STAs.size(), 4, true );  // set the number of plots and arrange them in 4 columns
  for ( int k=0; k<P.size(); k++ ) {
    if ( SpikeTrace[inspikes] == k )
      P[k].setPlotColor( Plot::Gray );
    P[k].setLabel( trace( SpikeTrace[k] ).ident(), 0.05, Plot::Graph, 0.8, Plot::Graph );
    P[k].setXRange( 1000.0*stamint, 1000.0*stamaxt );
    /*
    P[k].setXLabel( "msec" );
    P[k].setXLabelPos( 1.0, Plot::FirstMargin, 0.0, Plot::FirstAxis, 
		       Plot::Left, 0.0 );
    */
    P[k].setBMarg( 2.5 );
  }
  P.unlock();

  for ( int count=0;
	( repeats <= 0 ||        // infinite repetitions
	  count < repeats ) &&   // exactly repeats repetions
	  softStop() == 0;       // press the space key for exiting this loop
	count++ ) {

    // write loop count to status bar:
    Str s = "Loop <b>" + Str( count+1 ) + "</b>";
    message( s );

    sleep( interval );
    if ( interrupt() ) {
      // user interruption (by starting a different RePro), we have to return:
      STAs.clear();
      return count > 2 ? Completed : Aborted;
    }

    analyze( events( SpikeEvents[inspikes] ), interval );

    plot( plotsnippets );

    // here you should save some data ...

  }

  STAs.clear();
  return Completed;
}


void MultiTraceSTA::analyze( const EventData &spiketrain, double interval )
{
  cerr << "spikes " << spiketrain.ident() << endl;

  // room to leave at the end of the input traces:
  double skip = 2.0*STAs[0].Average.rangeBack();

  // go through the traces:
  for ( unsigned int k=0; k<STAs.size(); k++ ) {
    STAs[k].Snippets.clear();
    // for each spike find corresponding snippet in the input trace:
    const InData &intrace = trace( SpikeTrace[k] );  // numerics/include/relacs/eventdata.h
    int n = spiketrain.next( intrace.currentTime() - skip - interval ); // index to first spike in interval
    int p = spiketrain.previous( intrace.currentTime() - skip ); // index to last spike in interval
    for ( int i=n; i<=p; i++ ) {
      STAs[k].Snippets.push_back( SampleDataF( STAs[k].Average.range(), 0.0 ) );
      SampleDataF &snippet = STAs[k].Snippets.back();
      // copy data from trace into snippet:
      for ( int j=0; j<snippet.size(); j++ )
	snippet[j] = intrace[ intrace.index( spiketrain[i] + snippet.pos( j ) ) ];
    }

    // compute the average (from numerics/include/relacs/stats.h):
    average( STAs[k].Average.array(), STAs[k].StDev.array(), STAs[k].Snippets );

  }
}


void MultiTraceSTA::plot( bool snippets )
{
  P.lock();
  for ( unsigned int k=0; k<STAs.size(); k++ ) {
    P[k].clear();
    P[k].plotVLine( 0.0, Plot::White, 2 );
    if ( snippets ) {
      for ( unsigned int j=0; j<STAs[k].Snippets.size(); j++ )
	P[k].plot( STAs[k].Snippets[j], 1000.0, Plot::Red, 1, Plot::Solid );
    }
    P[k].plot( STAs[k].Average, 1000.0, Plot::Blue, 4, Plot::Solid );
    P[k].plot( STAs[k].StDev, 1000.0, Plot::Cyan, 2, Plot::Solid );
  }
  P.unlock();
  P.draw();
}


void MultiTraceSTA::config( void )
{
  // collect names of all input spike trains ...
  string ss = text( "inspikes" );
  for ( int k=0; k<SpikeTraces; k++ ) {
    if ( ! ss.empty() )
      ss += '|';
    ss += events( SpikeEvents[k] ).ident();
  }
  // ... and store them in the inspikes option:
  setText( "inspikes", ss );
  setToDefault( "inspikes" );
}


addRePro( MultiTraceSTA );

}; /* namespace multielectrode */

#include "moc_multitracesta.cc"

