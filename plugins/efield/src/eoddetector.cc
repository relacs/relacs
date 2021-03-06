/*
  efield/eoddetector.cc
  A detector for EOD cycles of weakly electric fish

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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <relacs/stats.h>
#include <relacs/basisfunction.h>
#include <relacs/fitalgorithm.h>
#include <relacs/efield/eoddetector.h>
using namespace relacs;

namespace efield {


EODDetector::EODDetector( const string &ident, int mode )
  : Filter( ident, mode, SingleAnalogDetector, 1,
	    "EODDetector", "efield",
	    "Jan Benda", "1.7", "Feb 3, 2013" )
{
  // parameter:
  Threshold = 0.0001;
  MinThresh = 0.0;
  MaxThresh = 1000.0;
  MaxEODPeriod = 0.1;  // 10 Hz
  AdaptThresh = false;
  ThreshRatio = 0.5;
  ZeroRatio = 0.25;
  AutoRatio = 0.5;
  FilterTau = 0.1;
  Interpolation = 1;
  FitWin = 0.4;

  // options:
  addNumber( "threshold", "Threshold", Threshold, MinThresh, MaxThresh, 0.01*MaxThresh, "", "", "%g", 2+8+32 );
  addBoolean( "adapt", "Adapt threshold", AdaptThresh, 2+8 );
  addNumber( "ratio", "Ratio", ThreshRatio, 0.05, 1.0, 0.05, "", "%", "%g", 2+8 ).setActivation( "adapt", "true" );
  addNumber( "autoratio", "Auto sets threshold relative to EOD peak-to-peak amplitude", AutoRatio, 0.05, 1.0, 0.05, "", "%", "%g", 8 );
  addNumber( "maxperiod", "Maximum EOD period for analyzing", MaxEODPeriod, 0.0, 1.0, 0.0001, "s", "ms", "%g", 8 );
  addNumber( "filtertau", "Filter time constant", FilterTau, 0.0, 10000.0, 0.001, "s", "ms", "%g", 8 );
  addNumber( "zeroratio", "Time is computed from threshold crossing by ratio of peak-to-peak amplitude below peak", ZeroRatio, 0.05, 1.0, 0.05, "", "%", "%g", 8 );
  addSelection( "interpolation", "Method for threshold-crossing time", "closest datapoint|linear interpolation|linear fit|quadratic fit", 8 ).selectText( Interpolation );
  addNumber( "fitwin", "Fraction between threshold crossing and peak used for fit", FitWin, 0.05, 1.0, 0.05, "", "%", "%g", 8 ).setActivation( "interpolation", "linear fit|quadratic fit" );
  addNumber( "rate", "Rate", 0.0, 0.0, 100000.0, 0.1, "Hz", "Hz", "%.1f", 2+4 );
  addNumber( "size", "Size", 0.0, 0.0, 100000.0, 0.1, "", "", "%.3f", 2+4 );
  addNumber( "meanvolts", "Average", 0.0, -10000.0, 10000.0, 0.1, "", "", "%.1f", 2+4 );
  addStyles( OptWidget::ValueLarge + OptWidget::ValueBold + OptWidget::ValueGreen + OptWidget::ValueBackBlack, 4 );

  setDialogSelectMask( 8 );
  setConfigSelectMask( -8 );

  // main layout:
  QVBoxLayout *vb = new QVBoxLayout;
  setLayout( vb );

  // parameter:
  if ( desktopHeight() < 700 ) {
    delFlags( "adapt", 2 );
    delFlags( "ratio", 2 );
  }
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

  // help:
  pb = new QPushButton( "Help" );
  hb->addWidget( pb );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( help( void ) ) );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( removeFocus( void ) ) );

  // auto configure:
  pb = new QPushButton( "Auto" );
  hb->addWidget( pb );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( autoConfigure( void ) ) );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( removeFocus( void ) ) );

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
  ZeroRatio = number( "zeroratio" );
  FilterTau = number( "filtertau" );
  Interpolation = index( "interpolation" );
  FitWin = number( "fitwin" );
  EDW.updateValues( OptWidget::changedFlag() );
}


int EODDetector::adjust( const InData &data )
{
  MaxThresh = data.maxValue();
  MinThresh = MaxThresh*0.0001;
  unsetNotify();
  setMinMax( "threshold", 0.0, MaxThresh, MaxThresh*0.01 );
  setUnit( "threshold", data.unit() );
  setUnit( "size", data.unit() );
  setUnit( "meanvolts", data.unit() );
  setNotify();
  EDW.updateSettings();
  return 0;
}


void EODDetector::autoConfigure( void )
{
  autoConfigure( *Data, currentTime() - 0.3, currentTime() - 0.1 );
}


int EODDetector::autoConfigure( const InData &data,
				double tbegin, double tend )
{
  // get rough estimate for a threshold:
  double ampl = eodAmplitude( data, tbegin, tend );
  // refine threshold:
  Threshold = floor10( 2.0*AutoRatio*ampl, 0.1 );
  if ( Threshold < MinThresh )
    Threshold = MinThresh;
  // update values:
  unsetNotify();
  setMinMax( "threshold", 0.0, data.maxValue(), ceil10( 0.1*Threshold ) );
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
    maxsize = *event - ZeroRatio * ( *event - *prevevent );
  else
    maxsize = *event - ZeroRatio * 2.0 * (*event);

  // previous maxsize crossing time:
  InData::const_iterator id = event;
  InData::const_range_iterator it = eventtime;
  double ival = *id;
  double pval = ival;
  int w = 1;
  for ( --id, --it; id != first; --id, --it, ++w ) {
    ival = *id;
    if ( ival < maxsize ) {
      if ( Interpolation == 1 ) {
	// linear interpolation:
	double m = SampleInterval / ( pval - ival );
	time = *it + m * ( maxsize - ival );
      }
      else if ( Interpolation >= 2 ) {
	// fit:
	w = (int)::round( FitWin*w );
	if ( w < 1 )
	  w = 1;
	double t0 = *it;
	for ( int k=1; k<w && id != first; ++k, --id, --it );
	int nw = 2*w;
	if ( Interpolation == 3 && nw < 3 )
	  nw = 3;
	ArrayD times;
	ArrayD volts;
	for ( int k=0; k<nw && id != event; ++k, ++id, ++it ) {
	  times.push( *it - t0 );
	  volts.push( *id );
	}
	if ( times.size() < nw ) {
	  times.clear();
	  volts.clear();
	  for ( int k=0; k<nw && id != first; ++k, --id, --it ) {
	    times.push( *it - t0 );
	    volts.push( *id );
	  }
	}
	if ( Interpolation == 3 ) {
	  // quadratic fit:
	  Polynom qp;
	  ArrayD p( 3 );
	  ArrayI pi( 3, 1 );
	  ArrayD u( 3 );
	  double chisq = 0.0;
	  linearFit( times, volts, qp, p, pi, u, chisq );
	  double sq = sqrt( p[1]*p[1] - 4.0*p[2]*(p[0]-maxsize) );
	  double t1 = (-p[1] + sq)/2.0/p[2];
	  double t2 = (-p[1] - sq)/2.0/p[2];
	  if ( t1 > -0.5*SampleInterval && t1 < 1.5*SampleInterval )
	    time = t0 + t1;
	  else if ( t2 > -0.5*SampleInterval && t2 < 1.5*SampleInterval )
	    time = t0 + t2;
	  else {
#ifndef NDEBUG
	    printlog( "\"" + ident() + "\" wrong quadratic fit: t1=" + Str( t1 ) + " t2=" + Str( t2 ) );
#endif
	    return 0;
	  }
	}
	else {
	  // linear fit:
	  double b=0.0;
	  double bu=0.0;
	  double m=0.0;
	  double mu=0.0;
	  double chisq=0.0;
	  lineFit( times, volts, b, bu, m, mu, chisq );
	  if ( m > 0.0 )
	    time = (maxsize-b)/m;
	  if ( m <= 0.0 || time < -0.5*SampleInterval || time > 1.5*SampleInterval ) {
#ifndef NDEBUG
	    printlog( "\"" + ident() + "\" wrong linear fit: time=" + Str( time ) + " m=" + Str( m ) );
#endif
	    return 0;
	  }
	  time += t0;
	}
      }
      else {
	// closest data point:
	time = *it;
      }

      if ( outevents.size() > 0 && time <= outevents.back() ) {
	// discard:
#ifndef NDEBUG
	printlog( "\"" + ident() + "\" peak discarded because of doublet time=" + Str( time ) + " back=" + Str( outevents.back() ) );
#endif
	return 0;
      }
      break;
    }
    if ( *eventtime - *it > MaxEODPeriod ) {
      // discard:
#ifndef NDEBUG
      printlog( "\"" + ident() + "\" peak discarded because of MaxEODPeriod=" + Str( MaxEODPeriod ) + " eventtime=" + Str( *eventtime ) + " it=" + Str( *it ) );
#endif
      return 0;
    }
    pval = ival;
  }

  // peak:
  double y1 = *(event-1);
  double y2 = *event;
  double y3 = *(event+1);
  double a = y3 - 4.0*y2 + 3.0*y1;
  double b = 2.0*y3 - 4.0*y2 + 2.0*y1;
  if ( fabs( b ) < 1e-8 ) {
#ifndef NDEBUG
    printlog( "\"" + ident() + "\" peak discarded because of vanishing b=" + Str( b ) );
#endif
    return 0;
  }
  double peakampl = fabs( b ) > 1e-8 ? y1 - 0.25*a*a/b : y2;
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
  if ( size <= 0.0 ) {
#ifndef NDEBUG
    printlog( "\"" + ident() + "\" peak discarded because of size=" + Str( size ) );
#endif
    if ( AdaptThresh )
      threshold *= 1.1;
    return 0;
  }

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
