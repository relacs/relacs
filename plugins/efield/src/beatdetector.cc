/*
  efield/beatdetector.cc
  Detects beats in EODs of wave-type weakly electric fish

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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

#include <relacs/efield/beatdetector.h>
using namespace relacs;

namespace efield {


BeatDetector::BeatDetector( const string &ident, int mode )
  : Filter( ident, mode, MultipleEventDetector, 2,
	    "BeatDetector", "EField", 
	    "Jan Benda", "1.2", "Jun 17, 2009" )
{
  // parameter:
  Threshold = 6.0;
  MinThresh = 6.0;
  MaxThresh = 1000.0;
  Ratio = 0.5;
  Decay = 10.0;
  Delay = 0.0;
  ChirpDistance = 0.03;
  ChirpEvents = 0;

  // options:
  int strongstyle = OptWidget::ValueLarge + OptWidget::ValueBold + OptWidget::ValueGreen + OptWidget::ValueBackBlack;
  addNumber( "threshold", "Threshold", Threshold, 0.0, 1000.0, 0.001, "", "", "%.3f", 2+4+32 );
  addNumber( "minthresh", "Minimum threshold", MinThresh, 0.0, 1000.0, 0.001, "", "", "%.3f", 2+8+32 );
  addNumber( "delay", "Delay time", Delay, 0.0, 1000.0, 1.0, "sec", "sec", "", 8+32 );
  addNumber( "decay", "Decay time constant", Decay, 0.0, 1000.0, 1.0, "sec", "sec", "", 2+8+32 );
  addNumber( "ratio", "Ratio threshold / size", Ratio, 0.0, 1.0, 0.05, "1", "%", "%.0f", 2+8+32 ); 
  addNumber( "rate", "Rate", 0.0, 0.0, 100000.0, 0.1, "Hz", "Hz", "%.1f", 2+4, strongstyle );
  addNumber( "contrast", "Contrast", 0.0, 0.0, 10000.0, 0.1, "%", "%", "%.1f", 2+4, strongstyle );

  BDW.assign( ((Options*)this), 2, 4, true, 0, mutex() );
  BDW.setVerticalSpacing( 4 );
  BDW.setMargins( 4 );
  setWidget( &BDW );

  setDialogSelectMask( 8 );
  setDialogReadOnlyMask( 16 );
  setConfigSelectMask( -32 );
}


BeatDetector::~BeatDetector( void )
{
}


int BeatDetector::init( const EventList &inevents, EventList &outevents, 
			const EventList &other, const EventData &stimuli )
{
  for ( int k=0; k<2; k++ ) {
    outevents[k].setSizeScale( 1.0 );
    outevents[k].setSizeUnit( inevents[0].sizeUnit() );
    outevents[k].setSizeFormat( "%6.2f" );
  }

  adjust( inevents );

  D.init( EventSizeIterator( inevents[0].begin() ),
	  EventSizeIterator( inevents[0].end() ),
	  EventIterator( inevents[0].begin() ));

  ChirpEvents = &other[0];

  outevents[0].setMeanRatio( 0.3 );
  outevents[1].setMeanRatio( 0.3 );

  return 0;
}


void BeatDetector::notify( void )
{
  Threshold = number( "threshold" );
  MinThresh = number( "minthresh" );
  Delay = number( "delay" );
  Decay = number( "decay" );
  Ratio = number( "ratio" );
  BDW.updateValues( OptWidget::changedFlag() );
}


int BeatDetector::adjust( const EventList &events )
{
  const InData &dd = trace( eventInputTrace( events[0].ident() ) );
  MaxThresh = ceil10( dd.maxValue(), 0.1 );
  double min = floor10( 0.01 * MaxThresh );
  unsetNotify();
  setUnit( "threshold", dd.unit() );
  setMinMax( "threshold", min, MaxThresh, min );
  setUnit( "minthresh", dd.unit() );
  setMinMax( "minthresh", min, MaxThresh, min );
  setNotify();
  BDW.updateSettings();
  return 0;
}


int BeatDetector::detect( const EventList &inevents, EventList &outevents, 
			  const EventList &other, const EventData &stimuli )
{
  long lastsize0 = outevents[0].size();
  long lastsize1 = outevents[1].size();

  D.dynamicPeakTrough( EventSizeIterator( inevents[0].begin() ), 
		       EventSizeIterator( inevents[0].end() - 1 ),
		       outevents, Threshold, MinThresh, MaxThresh,
		       Delay, Decay, *this );

  if ( outevents[0].size() - lastsize0 <= 0 )
    outevents[0].updateMean( 1, inevents[0].meanSize() );
  if ( outevents[1].size() - lastsize1 <= 0 )
    outevents[1].updateMean( 1, inevents[0].meanSize() );

  double sum = outevents[0].meanSize() + outevents[1].meanSize();
  double diff = outevents[0].meanSize() - outevents[1].meanSize();
  if ( diff < 0.0 )
    diff = 0.0;
  unsetNotify();
  if ( sum > 1.0e-2 )
    setNumber( "contrast", 100.0*diff/sum );

  setNumber( "threshold", Threshold );
  setNumber( "rate", outevents[1].meanRate() );
  setNotify();
  BDW.updateValues( OptWidget::changedFlag() );

  return 0;
}


int BeatDetector::checkPeak( EventSizeIterator first, 
			     EventSizeIterator last,
			     EventSizeIterator event, 
			     EventIterator eventtime, 
			     EventSizeIterator index,
			     EventIterator indextime,
			     EventSizeIterator prevevent, 
			     EventIterator prevtime, 
			     EventList &outevents, 
			     double &threshold,
			     double &minthresh, double &maxthresh,
			     double &time, double &size, double &width )
{
  time = *eventtime;
  size = *event;

  // chirps:
  if ( ChirpEvents->within( time, ChirpDistance ) )
    return 0;

  return 1;
}


int BeatDetector::checkTrough( EventSizeIterator first, 
			       EventSizeIterator last,
			       EventSizeIterator event, 
			       EventIterator eventtime, 
			       EventSizeIterator index,
			       EventIterator indextime,
			       EventSizeIterator prevevent, 
			       EventIterator prevtime, 
			       EventList &outevents, 
			       double &threshold,
			       double &minthresh, double &maxthresh,
			       double &time, double &size, double &width )
{
  time = *eventtime;
  size = *event;

  // update threshold:
  double r = 0.2;
  double diff = fabs(outevents[0].backSize() - size);
  Threshold = (1.0-r)*Threshold + r*Ratio*diff;

  // chirps:
  if ( ChirpEvents->within( time, ChirpDistance ) )
    return 0;

  return 1;
}


addDetector( BeatDetector );

}; /* namespace efield */

#include "moc_beatdetector.cc"
