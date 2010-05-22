/*
  efield/eoddetector.cc
  A detector for EOD cycles of weakly electric fish

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

#include <relacs/efield/eoddetector.h>
using namespace relacs;

namespace efield {


EODDetector::EODDetector( const string &ident, int mode )
  : Filter( ident, mode, SingleAnalogDetector, 1, 
	    "EODDetector", "EOD Detector", "EField",
	    "Jan Benda", "1.3", "June 16, 2009" ),
    EDW( (QWidget *)this )
{
  // parameter:
  Threshold = 0.0001;
  MinThresh = 0.0;
  MaxThresh = 1000.0;
  MaxEODPeriod = 0.01;  // 100 Hz
  ThreshRatio = 0.5;
  AdaptThresh = false;

  // options:
  addNumber( "threshold", "Threshold", Threshold, MinThresh, MaxThresh, 0.01*MaxThresh, "", "", "%g", 2+8+32 );
  addBoolean( "adapt", "Adapt threshold", AdaptThresh, 2+8 );
  addNumber( "ratio", "Ratio", ThreshRatio, 0.05, 1.0, 0.05, "", "%", "%g", 2+8 ).setActivation( "adapt", "true" );
  addNumber( "maxperiod", "Maximum EOD period", MaxEODPeriod, 0.0, 1.0, 0.0001, "s", "ms", "%g", 8 );
  addNumber( "rate", "Rate", 0.0, 0.0, 100000.0, 0.1, "Hz", "Hz", "%.1f", 2+4 );
  addNumber( "size", "Size", 0.0, 0.0, 100000.0, 0.1, "", "", "%.3f", 2+4 );
  addStyle( OptWidget::ValueLarge + OptWidget::ValueBold + OptWidget::ValueGreen + OptWidget::ValueBackBlack, 4 );

  EDW.assign( ((Options*)this), 2, 4, true, 0, mutex() ),
  EDW.setSpacing( 4 );
  EDW.setMargin( 4 );

  setDialogSelectMask( 8 );
  setDialogReadOnlyMask( 16 );
  setConfigSelectMask( -32 );
}


EODDetector::~EODDetector( void )
{
}


int EODDetector::init( const InData &data, EventData &outevents,
		       const EventList &other, const EventData &stimuli )
{
  outevents.setSizeScale( 1.0 );
  outevents.setSizeUnit( data.unit() );
  outevents.setSizeFormat( "%6.2f" );
  adjust( data );
  Threshold = MinThresh;
  D.init( data.begin(), data.end(), data.timeBegin() );
  return 0;
}


void EODDetector::notify( void )
{
  // no lock is needed, since the Options functions should already be locked!
  Threshold = number( "threshold" );
  AdaptThresh = boolean( "adapt" );
  ThreshRatio = number( "ratio" );
  MaxEODPeriod = number( "maxperiod" );
  EDW.updateValues( OptWidget::changedFlag() );
}


int EODDetector::adjust( const InData &data )
{
  unsetNotify();
  MaxThresh = ceil10( data.maxValue(), 0.1 );
  MinThresh = floor10( 0.01 * MaxThresh );
  setUnit( "threshold", data.unit() );
  setMinMax( "threshold", MinThresh, MaxThresh, MinThresh );
  setNumber( "threshold", Threshold );
  setUnit( "size", data.unit() );
  setNotify();
  EDW.updateSettings();
  return 0;
}


int EODDetector::detect( const InData &data, EventData &outevents,
			 const EventList &other, const EventData &stimuli )
{
  D.peak( data.minBegin(), data.end(), outevents,
	  Threshold, MinThresh, MaxThresh, *this );

  if ( outevents.count( data.currentTime() - 0.1 ) <= 0 )
    outevents.updateMean( 1 );

  unsetNotify();
  if ( AdaptThresh )
    setNumber( "threshold", Threshold );
  setNumber( "rate", outevents.meanRate() );
  setNumber( "size", outevents.meanSize() );
  setNotify();
  EDW.updateValues( OptWidget::changedFlag() );
  return 0;
}


int EODDetector::checkEvent( const InData::const_iterator &first, 
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

  // threshold for EOD time:
  double maxsize = 0.0;
  if ( prevevent > first && *event <= 0.0 )
    maxsize = *event - 0.25 * ( *event - *prevevent );
  else
    maxsize = 0.5 * *event;

  double sampleinterval = *eventtime - *(eventtime - 1);

  // previous maxsize crossing time:
  InData::const_iterator id = event;
  InDataTimeIterator it = eventtime;
  double ival = *id;
  double pval = ival;
  for ( --id, --it; id != first; --id, --it ) {
    ival = *id;
    if ( ival < maxsize ) {
      double m = sampleinterval / ( pval - ival );
      time = *it + m * ( maxsize - ival );
      if ( outevents.size() > 0 && time <= outevents.back() ) {
	cerr << "! warning in " << ident() << " time " << time << " <= back " << outevents.back() << endl;
	// discard:
	return 0;
      }
      break;
    }
    if ( *eventtime - *it > MaxEODPeriod ) {
      // discard:
      return 0;
    }
    pval = ival;
  }

  // peak:
  double y2 = *event;
  double y3 = *(event+1);
  double y1 = *(event-1);
  double a = y3 - 4.0*y2 + 3.0*y1;
  double b = 2.0*y3 - 4.0*y2 + 2.0*y1;
  /*
  // peak time:
  --event;
  time = event.time() + event.sampleInterval()*a/b;  // very noisy!
  */
  // peak size:
  size = y1 - 0.25*a*a/b;

  // width:
  width = 0.0;

  // threshold:
  if ( AdaptThresh )
    threshold = ThreshRatio * 2.0 * size;

  // accept:
  return 1; 
}


addDetector( EODDetector );

}; /* namespace efield */

#include "moc_eoddetector.cc"
