/*
  efield/eoddetector.cc
  A detector for EOD cycles of weakly electric fish

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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <relacs/efield/eoddetector.h>
using namespace relacs;

namespace efield {


EODDetector::EODDetector( const string &ident, int mode )
  : Filter( ident, mode, SingleAnalogDetector, 1, 
	    "EODDetector", "efield",
	    "Jan Benda", "1.6", "Dec 07, 2010" )
{
  // parameter:
  Threshold = 0.0001;
  MinThresh = 0.0;
  MaxThresh = 1000.0;
  MaxEODPeriod = 0.01;  // 100 Hz
  AdaptThresh = false;
  ThreshRatio = 0.5;
  AutoRatio = 0.5;
  FilterTau = 0.1;

  // options:
  addNumber( "threshold", "Threshold", Threshold, MinThresh, MaxThresh, 0.01*MaxThresh, "", "", "%g", 2+8+32 );
  addBoolean( "adapt", "Adapt threshold", AdaptThresh, 2+8 );
  addNumber( "ratio", "Ratio", ThreshRatio, 0.05, 1.0, 0.05, "", "%", "%g", 2+8 ).setActivation( "adapt", "true" );
  addNumber( "autoratio", "Auto sets threshold relative to EOD peak-to-peak amplitude", AutoRatio, 0.05, 1.0, 0.05, "", "%", "%g", 8 );
  addNumber( "maxperiod", "Maximum EOD period", MaxEODPeriod, 0.0, 1.0, 0.0001, "s", "ms", "%g", 8 );
  addNumber( "filtertau", "Filter time constant", FilterTau, 0.0, 10000.0, 0.001, "s", "ms", "%g", 8 );
  addNumber( "rate", "Rate", 0.0, 0.0, 100000.0, 0.1, "Hz", "Hz", "%.1f", 2+4 );
  addNumber( "size", "Size", 0.0, 0.0, 100000.0, 0.1, "", "", "%.3f", 2+4 );
  addNumber( "meanvolts", "Average", 0.0, -10000.0, 10000.0, 0.1, "", "", "%.1f", 2+4 );
  addStyles( OptWidget::ValueLarge + OptWidget::ValueBold + OptWidget::ValueGreen + OptWidget::ValueBackBlack, 4 );

  // main layout:
  QVBoxLayout *vb = new QVBoxLayout;
  setLayout( vb );

  // parameter:
  EDW.assign( ((Options*)this), 2, 4, true, 0, mutex() ),
  EDW.setVerticalSpacing( 4 );
  EDW.setMargins( 4 );
  vb->addWidget( &EDW, 0, Qt::AlignHCenter );

  // buttons:
  QHBoxLayout *hb = new QHBoxLayout;
  vb->addLayout( hb );

  // dialog:
  QPushButton *pb = new QPushButton( "Dialog" );
  hb->addWidget( pb );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( dialog( void ) ) );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( removeFocus( void ) ) );

  // auto configure:
  pb = new QPushButton( "Auto" );
  hb->addWidget( pb );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( autoConfigure( void ) ) );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( removeFocus( void ) ) );

  setDialogSelectMask( 8 );
  setDialogReadOnlyMask( 16 );
  setConfigSelectMask( -32 );

  Data = 0;
  MeanEOD = 0.0;
}


EODDetector::~EODDetector( void )
{
}


int EODDetector::init( const InData &data, EventData &outevents,
		       const EventList &other, const EventData &stimuli )
{
  Data = &data;
  outevents.setSizeScale( 1.0 );
  outevents.setSizeUnit( data.unit() );
  outevents.setSizeFormat( "%6.2f" );
  adjust( data );
  D.init( data.begin(), data.end(), data.timeBegin() );
  FilterIterator = data.begin();
  if ( FilterIterator < data.end() )
    MeanEOD = *FilterIterator;
  SampleInterval = data.stepsize();
  return 0;
}


void EODDetector::notify( void )
{
  // no lock is needed, since the Options functions should already be locked!
  Threshold = number( "threshold" );
  AdaptThresh = boolean( "adapt" );
  ThreshRatio = number( "ratio" );
  AutoRatio = number( "autoratio" );
  MaxEODPeriod = number( "maxperiod" );
  FilterTau = number( "filtertau" );
  EDW.updateValues( OptWidget::changedFlag() );
  delFlags( OptWidget::changedFlag() );
}


int EODDetector::adjust( const InData &data )
{
  unsetNotify();
  setUnit( "threshold", data.unit() );
  setUnit( "size", data.unit() );
  setUnit( "meanvolts", data.unit() );
  setNotify();
  EDW.updateSettings();
  return 0;
}


void EODDetector::autoConfigure( void )
{
  autoConfigure( *Data, currentTime() - 0.2, currentTime() );
}


int EODDetector::autoConfigure( const InData &data,
				double tbegin, double tend )
{
  // get rough estimate for a threshold:
  double ampl = eodAmplitude( data, tbegin, tend );
  // set range:
  double min = ceil10( 0.1*ampl );
  double max = ceil10( ::floor( 10.0*ampl/min )*min );
  // refine threshold:
  Threshold = floor10( 2.0*AutoRatio*ampl, 0.1 );
  if ( Threshold < MinThresh )
    Threshold = MinThresh;
  // update values:
  unsetNotify();
  setMinMax( "threshold", min, max, min );
  setNumber( "threshold", Threshold );
  setNotify();
  EDW.updateSettings( "threshold" );
  EDW.updateValue( "threshold" );
  return 0;
}


int EODDetector::detect( const InData &data, EventData &outevents,
			 const EventList &other, const EventData &stimuli )
{
  D.peak( data.minBegin(), data.end(), outevents,
	  Threshold, MinThresh, MaxThresh, *this );

  if ( outevents.count( currentTime() - 0.1 ) <= 0 )
    outevents.updateMean( 1 );
  unsetNotify();
  if ( AdaptThresh )
    setNumber( "threshold", Threshold );
  setNumber( "rate", outevents.meanRate() );
  setNumber( "size", outevents.meanSize() );
  setNumber( "meanvolts", MeanEOD );
  setNotify();
  EDW.updateValues( OptWidget::changedFlag() );
  delFlags( OptWidget::changedFlag() );
  return 0;
}


int EODDetector::checkEvent( InData::const_iterator first, 
			     InData::const_iterator last,
			     InData::const_iterator event,
			     InData::const_range_iterator eventtime,
			     InData::const_iterator index,
			     InData::const_range_iterator indextime,
			     InData::const_iterator prevevent,
			     InData::const_range_iterator prevtime,
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

  // update mean:
  if ( FilterTau > 0.0 ) {
    if ( FilterIterator < first ) {
      FilterIterator = last - 1 - (int)::ceil(10.0*FilterTau/SampleInterval);
      if ( FilterIterator < first )
	FilterIterator = first;
    }
    while ( FilterIterator < last ) {
      MeanEOD += ( *FilterIterator - MeanEOD )*SampleInterval/FilterTau;
      ++FilterIterator;
    }
  }
  else
    MeanEOD = 0.0;

  // threshold for EOD time:
  double maxsize = 0.0;
  if ( prevevent > first )
    maxsize = *event - 0.25 * ( *event - *prevevent );
  else
    maxsize = 0.5 * *event;

  // previous maxsize crossing time:
  InData::const_iterator id = event;
  InData::const_range_iterator it = eventtime;
  double ival = *id;
  double pval = ival;
  for ( --id, --it; id != first; --id, --it ) {
    ival = *id;
    if ( ival < maxsize ) {
      double m = SampleInterval / ( pval - ival );
      time = *it + m * ( maxsize - ival );
      if ( outevents.size() > 0 && time <= outevents.back() ) {
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
  if ( fabs( b ) < 1.0e-5 )
    return 0;
  double peakampl = y1 - 0.25*a*a/b;
  /*
  // peak time:
  --event;
  time = event.time() + event.sampleInterval()*a/b;  // very noisy!
  */

  /*
  // previous trough:
  double troughampl = 0.0;
  if ( prevevent-1 > first ) {
    y2 = *prevevent;
    y3 = *(prevevent+1);
    y1 = *(prevevent-1);
    a = y3 - 4.0*y2 + 3.0*y1;
    b = 2.0*y3 - 4.0*y2 + 2.0*y1;
    if ( fabs( b ) >= 1.0e-5 )
      troughampl = y1 - 0.25*a*a/b;
  }
  */

  // amplitude:
  //  size = 0.5*(peakampl - troughampl);
  size = peakampl - MeanEOD;
  if ( size <= 0.0 )
    return 0;

  // width:
  width = 0.0;

  // threshold:
  if ( AdaptThresh )
    threshold = ThreshRatio * 2.0 * size;

  // accept:
  return 1; 
}


addDetector( EODDetector, efield );

}; /* namespace efield */

#include "moc_eoddetector.cc"
