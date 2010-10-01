/*
  ephys/windowmuspikedetector.cc
  Extracellular spike discrimination based on threshold windows.

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

#include <relacs/ephys/windowmuspikedetector.h>
using namespace relacs;

namespace ephys {


WindowMUSpikeDetector::WindowMUSpikeDetector( const string &ident, int mode )
  : Filter( ident, mode, MultipleAnalogDetector, 1,
	    "WindowMUSpikeDetector", "ephys",
	    "Jan Benda", "1.0", "Sep 30, 2010" )
{
  // add some options:
//  addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );

  // parameter:
  Threshold = 0.0001;
  MinThresh = 0.0;
  MaxThresh = 1000.0;
  /*
  ThreshRatio = 0.5;
  AdaptThresh = false;

  // options:
  addNumber( "threshold", "Threshold", Threshold, MinThresh, MaxThresh, 0.01*MaxThresh, "", "", "%g", 2+8+32 );
  addBoolean( "adapt", "Adapt threshold", AdaptThresh, 2+8 );
  addNumber( "ratio", "Ratio", ThreshRatio, 0.05, 1.0, 0.05, "", "%", "%g", 2+8 ).setActivation( "adapt", "true" );
  addNumber( "rate", "Rate", 0.0, 0.0, 100000.0, 0.1, "Hz", "Hz", "%.1f", 2+4 );
  addNumber( "size", "Size", 0.0, 0.0, 100000.0, 0.1, "", "", "%.3f", 2+4 );
  addStyle( OptWidget::ValueLarge + OptWidget::ValueBold + OptWidget::ValueGreen + OptWidget::ValueBackBlack, 4 );

  EDW.assign( ((Options*)this), 2, 4, true, 0, mutex() );
  EDW.setVerticalSpacing( 4 );
  EDW.setMargins( 4 );
  setWidget( &EDW );

  setDialogSelectMask( 8 );
  setDialogReadOnlyMask( 16 );
  setConfigSelectMask( -32 );
  */
}


int WindowMUSpikeDetector::init( const InList &data, EventList &outevents,
				 const EventList &other, const EventData &stimuli )
{
  /*
  outevents.setSizeScale( 1.0 );
  outevents.setSizeUnit( data[0].unit() );
  outevents.setSizeFormat( "%6.2f" );
  */
  Threshold = MinThresh;
  D.init( data[0].begin(), data[0].end(), data[0].timeBegin() );
  return 0;
}


int WindowMUSpikeDetector::detect( const InList &data, EventList &outevents,
				   const EventList &other, const EventData &stimuli )
{
  EventData peaks( 1000, true );
  D.peak( data[0].minBegin(), data[0].end(), peaks,
	  Threshold, MinThresh, MaxThresh, *this );
  // sort peaks according to threshold windows:
  return 0;
}


int WindowMUSpikeDetector::checkEvent( const InData::const_iterator &first, 
				       const InData::const_iterator &last,
				       InData::const_iterator &event,
				       InDataTimeIterator &eventtime,
				       InData::const_iterator &index,
				       InDataTimeIterator &indextime,
				       InData::const_iterator &prevevent,
				       InDataTimeIterator &prevtime,
				       EventData &outevents,
				       double &threshold,
				       double &minthresh, double &maxthresh,
				       double &time, double &size, double &width )
{ 
  if ( event+1 >= last ) {
    // resume:
    return -1;
  }
  if ( event-1 < first ) {
    // discard:
    return 0;
  }

  // accept:
  return 1; 
}


 addDetector( WindowMUSpikeDetector );

}; /* namespace ephys */

#include "moc_windowmuspikedetector.cc"
