/*
  ephys/thresholdsuspikedetector.cc
  Spike detection based on an absolute voltage threshold.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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

#include <cmath>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPolygon>
#include <QPushButton>
#include <relacs/map.h>
#include <relacs/tablekey.h>
#include <relacs/ephys/thresholdsuspikedetector.h>
using namespace relacs;

namespace ephys {


ThresholdSUSpikeDetector::ThresholdSUSpikeDetector( const string &ident, int mode )
  : Filter( ident, mode, SingleAnalogDetector, 1,
	    "ThresholdSUSpikeDetector", "ephys",
	    "Jan Benda", "1.0", "Jan 17, 2011" ),
    GoodSpikesHist( -2000.0, 2000.0, 0.5 ),
    BadSpikesHist( -2000.0, 2000.0, 0.5 ),
    AllSpikesHist( -2000.0, 2000.0, 0.5 )
{
  // parameter:
  Threshold = 1.0;
  NoSpikeInterval = 0.1;
  StimulusRequired = false;
  LogHistogram = false;
  UpdateTime = 1.0;
  HistoryTime = 10.0;
  QualityThresh = 0.05;
  TrendThresh = 0.01;
  TrendTime = 1.0;
  SizeResolution = 0.5;
  Unit = "mV";

  // options:
  int strongstyle = OptWidget::ValueLarge + OptWidget::ValueBold + OptWidget::ValueGreen + OptWidget::ValueBackBlack;
  addLabel( "Detector", 8 );
  addNumber( "threshold", "Threshold", Threshold, -2000.0, 2000.0, SizeResolution, Unit, Unit, "%.1f", 2+8+32 );
  addLabel( "Indicators", 8 );
  addNumber( "nospike", "Interval for no spike", NoSpikeInterval, 0.0, 1000.0, 0.01, "sec", "ms", "%.0f", 0+8+32 );
  addBoolean( "considerstimulus", "Expect spikes during stimuli only", StimulusRequired, 0+8+32 );
  addNumber( "resolution", "Resolution of spike size", SizeResolution, 0.0, 1000.0, 0.01, Unit, Unit, "%.3f", 0+8+32 );
  addBoolean( "log", "Logarithmic histograms", LogHistogram, 0+8+32 );
  addNumber( "update", "Update time interval", UpdateTime, 0.2, 1000.0, 0.2, "sec", "sec", "%.1f", 0+8+32 );
  addNumber( "history", "Maximum history time", HistoryTime, 0.2, 1000.0, 0.2, "sec", "sec", "%.1f", 0+8+32 );
  addNumber( "qualitythresh", "Quality threshold", QualityThresh, 0.0, 1.0, 0.01, "1", "%", "%.0f", 0+8+32 );
  addNumber( "trendthresh", "Trend threshold", TrendThresh, 0.0, 1.0, 0.01, "1", "%", "%.0f", 0+8+32 );
  addNumber( "trendtime", "Trend timescale", TrendTime, 0.2, 1000.0, 0.2, "sec", "sec", "%.1f", 0+8+32 );
  addNumber( "rate", "Rate", 0.0, 0.0, 100000.0, 0.1, "Hz", "Hz", "%.0f", 0+4 );
  addNumber( "size", "Spike size", 0.0, 0.0, 10000.0, 0.1, Unit, Unit, "%.1f", 2+4, strongstyle );
  addInteger( "trend", "Trend", 0, 0, 4 );
  addInteger( "quality", "Quality", 0, 0, 3 );
  addTypeStyle( OptWidget::Bold, Parameter::Label );

  // main layout:
  QVBoxLayout *vb = new QVBoxLayout;
  vb->setContentsMargins( 0, 0, 0, 0 );
  vb->setSpacing( 0 );
  setLayout( vb );

  // parameter widgets:
  TDW.assign( ((Options*)this), 2, 4, true, 0, mutex() );
  TDW.setMargins( 4, 2, 4, 0 );
  TDW.setVerticalSpacing( 1 );
  vb->addWidget( &TDW, 0, Qt::AlignHCenter );

  setDialogSelectMask( 8 );
  setDialogReadOnlyMask( 16 );
  setConfigSelectMask( -32 );

  LastSize = 0;
  LastTime = 0.0;
  Update.start();

  QHBoxLayout *hb = new QHBoxLayout;
  vb->addLayout( hb );
  hb->setContentsMargins( 4, 4, 4, 4 );
  hb->setSpacing( 4 );

  P = new Plot( Plot::Copy );
  P->lock();
  P->noGrid();
  P->setTMarg( 1 );
  P->setRMarg( 1 );
  P->setXLabel( Unit );
  P->setXLabelPos( 1.0, Plot::FirstMargin, 0.0, Plot::FirstAxis, Plot::Left, 0.0 );
  P->setXTics();
  P->setYRange( 0.0, Plot::AutoScale );
  P->setYLabel( "" );
  P->setLMarg( 5 );
  P->unlock();
  hb->addWidget( P );

  // key to histogram plot:  XXX provide a function in Plot!
  QGridLayout *gl = new QGridLayout;
  gl->setContentsMargins( 0, 0, 0, 0 );
  gl->setVerticalSpacing( 0 );
  gl->setHorizontalSpacing( 4 );
  hb->addLayout( gl );

  int is = widget()->fontInfo().pixelSize();
  QPixmap pm( is*2, is/3 );
  pm.fill( Qt::green );
  QLabel *key = new QLabel;
  key->setPixmap( pm );
  gl->addWidget( key, 0, 0, Qt::AlignRight | Qt::AlignVCenter );
  key = new QLabel( "detected" );
  key->setFixedHeight( is );
  gl->addWidget( key, 0, 1, Qt::AlignLeft );

  pm.fill( Qt::red );
  key = new QLabel;
  key->setPixmap( pm );
  gl->addWidget( key, 1, 0, Qt::AlignRight | Qt::AlignVCenter );
  key = new QLabel( "not detected" );
  key->setFixedHeight( is );
  gl->addWidget( key, 1, 1, Qt::AlignLeft );

  pm.fill( Qt::white );
  key = new QLabel;
  key->setPixmap( pm );
  gl->addWidget( key, 2, 0, Qt::AlignRight | Qt::AlignVCenter );
  key = new QLabel( "threshold" );
  key->setFixedHeight( is );
  gl->addWidget( key, 2, 1, Qt::AlignLeft );

  // indicators:
  QColor orange( 255, 165, 0 );
  is *= 2;
  GoodQuality = QPixmap( is, is );
  QPainter p;
  p.begin( &GoodQuality );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( GoodQuality.rect(), Qt::black );
  p.setPen( Qt::NoPen );
  p.setBrush( Qt::green );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( QColor( Qt::green ).lighter( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawEllipse( is/5, is/5, 3*is/5+1, 3*is/5+1 );
  p.drawLine( 6*is/10, 6*is/10, 4*is/5, 4*is/5 );
  p.end();

  OkQuality = QPixmap( is, is );
  p.begin( &OkQuality );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( OkQuality.rect(), Qt::black );
  p.setPen( Qt::NoPen );
  p.setBrush( Qt::yellow );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( QColor( Qt::yellow ).lighter( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawEllipse( is/5, is/5, 3*is/5, 3*is/5 );
  p.drawLine( is/2, is/2, 4*is/5, 4*is/5 );
  p.end();

  PotentialQuality = QPixmap( is, is );
  p.begin( &PotentialQuality );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( PotentialQuality.rect(), Qt::black );
  p.setPen( Qt::NoPen );
  p.setBrush( orange );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( QColor( orange ).lighter( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawEllipse( is/5, is/5, 3*is/5, 3*is/5 );
  p.drawLine( is/2, is/2, 4*is/5, 4*is/5 );
  p.end();

  BadQuality = QPixmap( is, is );
  p.begin( &BadQuality );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( BadQuality.rect(), Qt::black );
  p.setPen( Qt::NoPen );
  p.setBrush( Qt::red );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( QColor( Qt::red ).lighter( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawEllipse( is/5, is/5, 3*is/5, 3*is/5 );
  p.drawLine( is/2, is/2, 4*is/5, 4*is/5 );
  p.end();

  QPolygon pa( 7 );
  BadArrow = QPixmap( is, is );
  p.begin( &BadArrow );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( BadArrow.rect(), Qt::black );
  p.setPen( QPen( Qt::black, 1 ) );
  p.setBrush( Qt::red );
  pa.setPoint( 0, is/4, 0 );
  pa.setPoint( 1, 3*is/4, 0 );
  pa.setPoint( 2, 3*is/4, 2*is/3 );
  pa.setPoint( 3, is, 2*is/3 );
  pa.setPoint( 4, is/2, is );
  pa.setPoint( 5, 0, 2*is/3 );
  pa.setPoint( 6, is/4, 2*is/3 );
  p.drawPolygon( pa );
  p.end();

  BadTrend = QPixmap( is, is );
  p.begin( &BadTrend );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( BadTrend.rect(), Qt::black );
  p.setPen( Qt::NoPen );
  p.setBrush( Qt::red );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( QColor( Qt::red ).lighter( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawLine( is/2, is/4, is/2, 3*is/4 );
  p.drawLine( is/4, is/4, 3*is/4, is/4 );
  p.end();

  OkTrend = QPixmap( is, is );
  p.begin( &OkTrend );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( OkTrend.rect(), Qt::black );
  p.setPen( Qt::NoPen );
  p.setBrush( Qt::yellow );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( QColor( Qt::yellow ).lighter( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawLine( is/2, is/4, is/2, 3*is/4 );
  p.drawLine( is/4, is/4, 3*is/4, is/4 );
  p.end();

  GoodTrend = QPixmap( is, is );
  p.begin( &GoodTrend );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( GoodTrend.rect(), Qt::black );
  p.setPen( Qt::NoPen );
  p.setBrush( Qt::green );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( QColor( Qt::green ).lighter( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawLine( is/2, is/4, is/2, 3*is/4 );
  p.drawLine( is/4, is/4, 3*is/4, is/4 );
  p.end();

  GoodArrow = QPixmap( is, is );
  p.begin( &GoodArrow );
  p.setBackgroundMode( Qt::OpaqueMode );
  p.fillRect( GoodArrow.rect(), Qt::black );
  p.setPen( QPen( Qt::black, 1 ) );
  p.setBrush( Qt::green );
  pa.setPoint( 0, is/4, is-1 );
  pa.setPoint( 1, 3*is/4, is-1 );
  pa.setPoint( 2, 3*is/4, is/3 );
  pa.setPoint( 3, is, is/3 );
  pa.setPoint( 4, is/2, 0 );
  pa.setPoint( 5, 0, is/3 );
  pa.setPoint( 6, is/4, is/3 );
  p.drawPolygon( pa );
  p.end();

  // quality indicator:
  Quality = 0;
  QualityPixs[0] = &BadQuality;
  QualityPixs[1] = &PotentialQuality;
  QualityPixs[2] = &OkQuality;
  QualityPixs[3] = &GoodQuality;
  QualityIndicator = new QLabel;
  QualityIndicator->setPixmap( *QualityPixs[Quality] );
  gl->addWidget( QualityIndicator, 4, 0, Qt::AlignLeft );

  // trend indicator:
  Trend = 2;
  TrendPixs[0] = &BadArrow;
  TrendPixs[1] = &BadTrend;
  TrendPixs[2] = &OkTrend;
  TrendPixs[3] = &GoodTrend;
  TrendPixs[4] = &GoodArrow;
  TrendIndicator = new QLabel;
  TrendIndicator->setPixmap( *TrendPixs[Trend] );
  gl->addWidget( TrendIndicator, 5, 0, Qt::AlignLeft );

  // dialog:
  QPushButton *pb = new QPushButton( "Dialog" );
  gl->addWidget( pb, 4, 1, Qt::AlignRight );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( dialog( void ) ) );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( removeFocus( void ) ) );

  // auto configure:
  pb = new QPushButton( "Auto" );
  gl->addWidget( pb, 5, 1, Qt::AlignRight );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( autoConfigure( void ) ) );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( removeFocus( void ) ) );

}


int ThresholdSUSpikeDetector::init( const InData &data, EventData &outevents,
				    const EventList &other, const EventData &stimuli )
{
  Unit = data.unit();
  setOutUnit( "threshold", Unit );
  setOutUnit( "size", Unit );
  setOutUnit( "resolution", Unit );
  setMinMax( "resolution", 0.0, 1000.0, 0.01, Unit );
  setNotify();
  notify();
  postCustomEvent( 12 );
  P->lock();
  P->setXLabel( Unit );
  P->unlock();
  outevents.setSizeScale( 1.0 );
  outevents.setSizeUnit( Unit );
  outevents.setSizeFormat( "%5.1f" );
  outevents.setWidthScale( 1000.0 );
  outevents.setWidthUnit( "ms" );
  outevents.setWidthFormat( "%4.2f" );
  D.setHistorySize( int( HistoryTime*1000.0 ) );
  LastSpikeSize = 0.0;
  LastTime = 0.0;
  StimulusEnd = 0.0;
  IntervalStart = 0.0;
  IntervalEnd = 0.0;
  IntervalWidth = 0.0;
  D.init( data.begin(), data.end(), data.timeBegin() );
  return 0;
}


void ThresholdSUSpikeDetector::readConfig( StrQueue &sq )
{
  unsetNotify(); // we have no unit of the input trace yet!
  Options::read( sq, 0, ":" );
}


void ThresholdSUSpikeDetector::notify( void )
{
  Threshold = number( "threshold", Unit );
  NoSpikeInterval = number( "nospike" );
  StimulusRequired = boolean( "considerstimulus" );
  LogHistogram = boolean( "log" );
  Parameter &ht = Options::operator[]( "history" );
  if ( ht.flags( OptWidget::changedFlag() ) ) {
    HistoryTime = ht.number();
    D.setHistorySize( int( HistoryTime*1000.0 ) );
  }
  UpdateTime = number( "update" );
  QualityThresh = number( "qualitythresh" );
  TrendThresh = number( "trendthresh" );
  TrendTime = number( "trendtime" );
  double resolution = number( "resolution", Unit );

  if ( changed( "resolution" ) && resolution > 0.0 ) {
    if ( resolution < 0.001 ) {
      resolution = 0.001;
      setNumber( "resolution", resolution, Unit );
    }
    SizeResolution = resolution;
    // necessary precision:
    int pre = -1;
    do {
      pre++;
      double f = pow( 10.0, -pre );
      resolution -= floor( 1.001*resolution/f ) * f;
    } while ( pre < 3 && fabs( resolution ) > 1.0e-3 );
    setFormat( "threshold", 4+pre, pre, 'f' );
    setStep( "threshold", SizeResolution, Unit );
    setFormat( "size", 4+pre, pre, 'f' );
    double max = 1000.0*SizeResolution;
    GoodSpikesHist = SampleDataD( -max, max, SizeResolution );
    BadSpikesHist = SampleDataD( -max, max, SizeResolution );
    AllSpikesHist = SampleDataD( -max, max, SizeResolution );
    TDW.updateSettings();
  }
  TDW.updateValues( OptWidget::changedFlag() );
  delFlags( OptWidget::changedFlag() );
}


void ThresholdSUSpikeDetector::autoConfigure( void )
{
  // skip initial zeros:
  int lp = 0;
  for ( lp=0; lp<AllSpikesHist.size() && AllSpikesHist[lp] <= 0.0; lp++ );
  // skip final zeros:
  int rp = 0;
  for ( rp=AllSpikesHist.size()-1; rp>=0 && AllSpikesHist[rp] <= 0.0; rp-- );
  // no data in histogram:
  if ( lp >= rp )
    return;
  // find longest stretch of zeros:
  int zp = -1;
  int zn = 0;
  for ( int k=lp; k<=rp; k++ ) {
    if ( AllSpikesHist[k] <= 0.0 ) {
      int p = k;
      int n = 0;
      for ( n++, k++; k<=rp && AllSpikesHist[k] <= 0.0; n++, k++ );
      if ( n >= zn ) {
	zp = p;
	zn = n;
      }
    }
  }
  // no zero stretch:
  if ( zp < 0 )
    Threshold = AllSpikesHist.pos( rp+2 );
  else
    Threshold = AllSpikesHist.pos( zp+1 );
  TDW.updateValues( OptWidget::changedFlag() );
  delFlags( OptWidget::changedFlag() );
}


int ThresholdSUSpikeDetector::autoConfigure( const InData &data,
					   double tbegin, double tend )
{
  autoConfigure();
  return 0;
}


void ThresholdSUSpikeDetector::save( const string &param )
{
  save();
}


void ThresholdSUSpikeDetector::save( void )
{
  // create file:
  ofstream df( addPath( Str( ident() ).lower() + "-distr.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  lock();

  // write header and key:
  Options header;
  header.addText( "ident", ident() );
  header.addText( "detector", name() );
  header.addText( "session time", sessionTimeStr() );
  header.addLabel( "settings:" );
  header.save( df, "# " );
  Options::save( df, "#   " );
  df << '\n';
  TableKey key;
  key.addNumber( "ampl", Unit, "%5.1f" );
  key.addNumber( "bad", "1", "%5.0f" );
  key.addNumber( "good", "1", "%5.0f" );
  key.saveKey( df, true, false );

  // range:
  int max = AllSpikesHist.size()-1;
  for ( ; max >= 0; max-- )
    if ( AllSpikesHist[max] > 0.0 )
      break;

  // write data:
  if ( max > 0 ) {
    for ( int n=0; n<max; n++ ) {
      key.save( df, AllSpikesHist.pos( n ), 0 );
      key.save( df, BadSpikesHist[n] );
      key.save( df, GoodSpikesHist[n] );
      df << '\n';
    }
  }
  else {
    key.save( df, 0.0, 0 );
    key.save( df, 0.0 );
    key.save( df, 0.0 );
    df << '\n';
  }
  df << '\n' << '\n';
  
  unlock();

}


int ThresholdSUSpikeDetector::detect( const InData &data, EventData &outevents,
				      const EventList &other, const EventData &stimuli )
{
  D.thresholdPeakHist( data.minBegin(), data.end(), outevents,
		       Threshold, Threshold, Threshold, *this );

  // update mean spike size in case of no spikes:
  if ( StimulusRequired && stimuli.size() > 0 ) {
    double stimulusstart = stimuli.back();
    if ( stimulusstart >= LastTime && stimulusstart < currentTime() ) {
      IntervalWidth = stimuli.backWidth();
      StimulusEnd = stimulusstart + IntervalWidth;
      if ( IntervalWidth > NoSpikeInterval )
	IntervalWidth = NoSpikeInterval;
      IntervalStart = stimulusstart;
      IntervalEnd = IntervalStart + IntervalWidth;
    }
    LastTime = currentTime();
    while ( IntervalWidth > 0.0 &&
	    IntervalEnd <= StimulusEnd &&
	    IntervalEnd <= currentTime() ) {
      if ( outevents.count( IntervalStart, IntervalEnd ) == 0 )
	outevents.updateMean( (int)::rint(IntervalWidth/NoSpikeInterval + 0.5) );
      IntervalStart = IntervalEnd;
      IntervalEnd += IntervalWidth;
    }
  }
  else {
    if ( currentTime() > LastTime + NoSpikeInterval ) {
      if ( outevents.size() - LastSize <= 0 )
	outevents.updateMean( (int)::rint((currentTime() - LastTime)/NoSpikeInterval) );
      LastTime = currentTime();
      LastSize = outevents.size();
    }
  }

  unsetNotify();
  setNumber( "rate", outevents.meanRate() );
  setNumber( "size", outevents.meanSize(), Unit );
  setNotify();

  // update indicator widgets:
  if ( Update.elapsed()*0.001 < UpdateTime )
    return 0;
  Update.start();

  // histogramms:
  D.goodEvents().sizeHist( currentTime() - HistoryTime, currentTime(), GoodSpikesHist );
  D.badEvents().sizeHist( currentTime() - HistoryTime, currentTime(), BadSpikesHist );
  AllSpikesHist = GoodSpikesHist + BadSpikesHist;

  // plot:
  P->lock();
  P->clear();
  double xmin = -10.0;
  for ( int k=0; k<AllSpikesHist.size(); k++ )
    if ( AllSpikesHist[k] > 0.0 ) {
      xmin = AllSpikesHist.pos( k );
      break;
    }
  double xmax = 10.0;
  for ( int k=AllSpikesHist.size()-1; k >= 0; k-- )
    if ( AllSpikesHist[k] > 0.0 ) {
      xmax = AllSpikesHist.pos( k+1 );
      break;
    }
  if ( ! P->zoomedXRange() )
    P->setXRange( xmin, xmax );
  if ( LogHistogram ) {
    SampleDataD bh( BadSpikesHist );
    for ( int k=0; k<bh.size(); k++ )
      bh[k] = bh[k] > 1.0 ? log( bh[k] ) : 0.0;
    SampleDataD gh( GoodSpikesHist );
    for ( int k=0; k<gh.size(); k++ )
      gh[k] = gh[k] > 1.0 ? log( gh[k] ) : 0.0;
    P->plot( bh, 1.0, Plot::Red, 2, Plot::Solid );
    P->plot( gh, 1.0, Plot::Green, 2, Plot::Solid );
    P->noYTics();
  }
  else {
    P->plot( BadSpikesHist, 1.0, Plot::Red, 2, Plot::Solid );
    P->plot( GoodSpikesHist, 1.0, Plot::Green, 2, Plot::Solid );
    P->setYTics();
  }
  P->plotVLine( Threshold, Plot::White, 2 );
  P->draw();
  P->unlock();

  // indicators:

  // spikes detected:
  bool spikes = ( outevents.count( currentTime() - TrendTime ) > 1 );

  // set update speed for spike size:
  double nratio = outevents.meanRate();
  if ( nratio < 1.0 )
    nratio = 1.0;
  nratio *= TrendTime;
  outevents.setMeanRatio( 1.0/nratio );
  // trend indicator:
  Trend = 2;  // ok trend
  if ( spikes ) {
    if ( outevents.meanSize() > LastSpikeSize * (1.0 + 0.2*TrendThresh) ) {
      if ( outevents.meanSize() > LastSpikeSize * (1.0 + TrendThresh) )
	Trend = 4;  // good arrow
      else
	Trend = 3;  // good trend
    }
    else if ( outevents.meanSize() < LastSpikeSize * (1.0 - 0.2*TrendThresh) ) {
      if ( outevents.meanSize() < LastSpikeSize * (1.0 - TrendThresh) )
	Trend = 0;  // bad arrow
      else
	Trend = 1;  // bad trend
    }
  }
  unsetNotify();
  setInteger( "trend", Trend );
  setNotify();
  LastSpikeSize = outevents.meanSize();

  // gap in histogram (two peaks)?
  int lp=0; // end of first peak in histogram
  for ( ; lp<AllSpikesHist.size() && AllSpikesHist[lp] <= 0.0; lp++ );
  for ( ; lp<AllSpikesHist.size() && AllSpikesHist[lp] > 0.0; lp++ );
  int hp=lp; // start of second peak in histogram
  for ( ; hp<AllSpikesHist.size() && AllSpikesHist[hp] <= 0.0; hp++ );
  bool gap = ( lp < AllSpikesHist.size() &&
	       hp < AllSpikesHist.size() &&
	       hp > lp );

  Quality = 0;
  if ( ! gap || ! spikes ) {
    Quality = gap ? 1 : 0;
    unsetNotify();
    setInteger( "quality", Quality );
    setNotify();
    TDW.updateValues( 2+4 );
    postCustomEvent( 11 );
    return 0;
  }

  // overlap:
  double sum = 0.0;
  double overlap = 0.0;
  for ( int k=0; k<AllSpikesHist.size(); k++ ) {
    sum += AllSpikesHist[k];
    overlap += GoodSpikesHist[k] > BadSpikesHist[k] ? BadSpikesHist[k] : GoodSpikesHist[k];
  }
  double qualityval = overlap / sum;

  // quality indicator:
  if ( qualityval <= 0.0 ) {
    Quality = 3;
  }
  else if ( qualityval < QualityThresh ) {
    Quality = 2;
  }
  else {
    Quality = 1;
  }
  unsetNotify();
  setInteger( "quality", Quality );
  setNotify();
  TDW.updateValues( 2+4 );
  postCustomEvent( 11 );

  return 0;
}


int ThresholdSUSpikeDetector::checkEvent( InData::const_iterator first, 
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

  time = *eventtime;
  size = *event;

  // accept:
  return 1; 
}


void ThresholdSUSpikeDetector::customEvent( QEvent *qce )
{
  if ( qce->type() == QEvent::User+11 ) {
    lock();
    QualityIndicator->setPixmap( *QualityPixs[Quality] );
    TrendIndicator->setPixmap( *TrendPixs[Trend] );
    unlock();
  }
  else if ( qce->type() == QEvent::User+12 ) {
    TDW.updateSettings();
    TDW.updateValues();
  }
  else
    Filter::customEvent( qce );
}


addDetector( ThresholdSUSpikeDetector );

}; /* namespace ephys */

#include "moc_thresholdsuspikedetector.cc"
