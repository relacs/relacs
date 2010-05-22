/*
  ephys/dynamicsuspikedetector.cc
  A detector for spikes in single unit recordings.

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

#include <cmath>
#include <qhbox.h>
#include <qvbox.h>
#include <qgrid.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qapplication.h>
#include <relacs/map.h>
#include <relacs/basisfunction.h>
#include <relacs/fitalgorithm.h>
#include <relacs/str.h>
#include <relacs/tablekey.h>
#include <relacs/ephys/dynamicsuspikedetector.h>
using namespace relacs;

namespace ephys {


DynamicSUSpikeDetector::DynamicSUSpikeDetector( const string &ident, int mode )
  : Filter( ident, mode, SingleAnalogDetector, 1,
	    "DynamicSUSpikeDetector", "DynamicSUSpikeDetector", "EPhys",
	    "Jan Benda", "1.8", "Mar 16, 2010" ),
    SDW( (QWidget*)this ),
    GoodSpikesHist( 0.0, 200.0, 0.5 ),
    BadSpikesHist( 0.0, 200.0, 0.5 ),
    AllSpikesHist( 0.0, 200.0, 0.5 )
{
  // parameter:
  Threshold = 10.0;
  MinThresh = 10.0;
  MaxThresh = 100.0;
  RecordingDelay = 1.0;
  RecordingDecay = 10.0;
  SearchDelay = 1.0;
  SearchDecay = 10.0;
  TestWidth = true;
  MaxWidth = 0.0015;
  TestInterval = true;
  MinInterval = 0.001;
  FitPeak = false;
  FitWidth = 0.0005;
  FitIndices = 0;
  Ratio = 0.5;
  NoSpikeInterval = 0.1;
  StimulusRequired = false;
  LogHistogram = false;
  UpdateTime = 1.0;
  HistoryTime = 10.0;
  QualityThresh = 0.05;
  TrendThresh = 0.01;
  TrendTime = 1.0;
  SizeResolution = 0.5;

  // options:
  int strongstyle = OptWidget::ValueLarge + OptWidget::ValueBold + OptWidget::ValueGreen + OptWidget::ValueBackBlack;
  addLabel( "Detector", 8 );
  addNumber( "minthresh", "Minimum threshold", MinThresh, 0.0, 200.0, 0.5, "mV", "mV", "%.1f", 2+8+32 );
  addNumber( "maxthresh", "Maximum threshold", MaxThresh, 0.0, 200.0, 0.5, "mV", "mV", "%.1f", 8+32 );
  addNumber( "threshold", "Threshold", Threshold, 0.0, 200.0, 1.0, "mV", "mV", "%.1f", 2+4+32 );
  addNumber( "delay", "Delay time", RecordingDelay, 0.0, 1000.0, 1.0, "sec", "sec", "%.0f", 0+8+32 );
  addNumber( "decay", "Decay time constant", RecordingDecay, 0.0, 1000.0, 1.0,  "sec", "sec", "%.0f", 0+8+32 );
  addNumber( "searchdelay", "Delay time used inbetween the recordings", SearchDelay, 0.0, 1000.0, 1.0, "sec", "sec", "%.0f", 0+8+32 );
  addNumber( "searchdecay", "Decay time constant used inbetween the recordings", SearchDecay, 0.0, 1000.0, 1.0,  "sec", "sec", "%.0f", 0+8+32 );
  addNumber( "ratio", "Ratio threshold / size", Ratio, 0.0, 1.0, 0.05, "1", "%", "%.0f",  2+8+32 );
  addBoolean( "testwidth", "Test spike width", TestWidth ).setFlags( 0+8+32 );
  addNumber( "maxwidth", "Maximum spike width", MaxWidth, 0.0001, 0.006, 0.0001, "sec", "ms", "%.1f", 0+8+32 ).setActivation( "testwidth", "true" );
  addBoolean( "testisi", "Test interspike interval", TestInterval ).setFlags( 0+8+32 );
  addNumber( "minisi", "Minimum interspike interval", MinInterval, 0.0, 0.1, 0.0002, "sec", "ms", "%.1f", 0+8+32 ).setActivation( "testisi", "true" );
  addBoolean( "fitpeak", "Fit parabula to peak of spike", FitPeak ).setFlags( 0+8+32 );
  addNumber( "fitwidth", "Width of parabula fit", FitWidth, 0.0, 0.1, 0.00001, "sec", "ms", "%.2f", 0+8+32 );
  addLabel( "Indicators", 8 );
  addNumber( "nospike", "Interval for no spike", NoSpikeInterval, 0.0, 1000.0, 0.01, "sec", "ms", "%.0f", 0+8+32 );
  addBoolean( "considerstimulus", "Expect spikes during stimuli only", StimulusRequired, 0+8+32 );
  addNumber( "resolution", "Resolution of spike size", SizeResolution, 0.0, 1000.0, 0.1, "mV", "mV", "%.2f", 0+8+32 );
  addBoolean( "log", "Logarithmic histograms", LogHistogram, 0+8+32 );
  addNumber( "update", "Update time interval", UpdateTime, 0.2, 1000.0, 0.2, "sec", "sec", "%.1f", 0+8+32 );
  addNumber( "history", "Maximum history time", HistoryTime, 0.2, 1000.0, 0.2, "sec", "sec", "%.1f", 0+8+32 );
  addNumber( "qualitythresh", "Quality threshold", QualityThresh, 0.0, 1.0, 0.01, "1", "%", "%.0f", 0+8+32 );
  addNumber( "trendthresh", "Trend threshold", TrendThresh, 0.0, 1.0, 0.01, "1", "%", "%.0f", 0+8+32 );
  addNumber( "trendtime", "Trend timescale", TrendTime, 0.2, 1000.0, 0.2, "sec", "sec", "%.1f", 0+8+32 );
  addNumber( "rate", "Rate", 0.0, 0.0, 100000.0, 0.1, "Hz", "Hz", "%.0f", 0+4 );
  addNumber( "size", "Spike size", 0.0, 0.0, 10000.0, 0.1, "mV", "mV", "%.1f", 2+4, strongstyle );
  addInteger( "trend", "Trend", 0, 0, 4 );
  addInteger( "quality", "Quality", 0, 0, 3 );
  addTypeStyle( OptWidget::TabLabel, Parameter::Label );

  SDW.assign( ((Options*)this), 2, 4, true, 0, mutex() );
  SDW.setSpacing( 4 );
  SDW.setMargin( 4 );

  setDialogSelectMask( 8 );
  setDialogReadOnlyMask( 16 );
  setConfigSelectMask( -32 );

  MaxRangeThresh = 100.0;
  LastSize = 0;
  LastTime = 0.0;
  Update.start();

  QHBox *hb = new QHBox( this );
  hb->setSpacing( 4 );

  P = new Plot( Plot::Copy, hb );
  P->lock();
  P->noGrid();
  P->setTMarg( 1 );
  P->setRMarg( 1 );
  P->setXLabel( "mV" );
  P->setXLabelPos( 1.0, Plot::FirstMargin, 0.0, Plot::FirstAxis, Plot::Left, 0.0 );
  P->setXTics();
  P->setYRange( 0.0, Plot::AutoScale );
  P->setYLabel( "" );
  P->setLMarg( 5 );
  P->unlock();

  // key to histogram plot:  XXX provide a function in Plot!
  QVBox *vb = new QVBox( hb );

  QGrid *gl = new QGrid( 4, vb );
  gl->setSpacing( 0 );

  QPixmap pm( 20, 10 );
  QPainter p;
  p.begin( &pm );
  p.setBackgroundMode( OpaqueMode );
  p.fillRect( pm.rect(), this->backgroundColor() );
  p.setPen( QPen( green, 4 ) );
  p.drawLine( 0, 5, pm.width(), 5 );
  p.end();
  QLabel *key = new QLabel( gl );
  key = new QLabel( gl );
  key->setPixmap( pm );
  key = new QLabel( "  ", gl );
  key = new QLabel( "detected", gl );

  p.begin( &pm );
  p.setPen( QPen( red, 4 ) );
  p.drawLine( 0, 5, pm.width(), 5 );
  p.end();
  key = new QLabel( gl );
  key = new QLabel( gl );
  key->setPixmap( pm );
  key = new QLabel( "  ", gl );
  key = new QLabel( "not detected", gl );

  p.begin( &pm );
  p.setPen( QPen( white, 4 ) );
  p.drawLine( 0, 5, pm.width(), 5 );
  p.end();
  key = new QLabel( gl );
  key = new QLabel( gl );
  key->setPixmap( pm );
  key = new QLabel( "  ", gl );
  key = new QLabel( "threshold", gl );

  p.begin( &pm );
  p.setPen( QPen( yellow, 4 ) );
  p.drawLine( 0, 5, pm.width(), 5 );
  p.end();
  key = new QLabel( gl );
  key = new QLabel( gl );
  key->setPixmap( pm );
  key = new QLabel( "  ", gl );
  key = new QLabel( "min thresh", gl );

  // indicators:
  int is = fontInfo().pixelSize() * 2;
  QColor orange( 255, 165, 0 );
  GoodQuality.resize( is, is );
  p.begin( &GoodQuality );
  p.setBackgroundMode( OpaqueMode );
  p.fillRect( GoodQuality.rect(), black );
  p.setPen( Qt::NoPen );
  p.setBrush( green );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( green.light( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawEllipse( is/5, is/5, 3*is/5+1, 3*is/5+1 );
  p.drawLine( 6*is/10, 6*is/10, 4*is/5, 4*is/5 );
  p.end();

  OkQuality.resize( is, is );
  p.begin( &OkQuality );
  p.setBackgroundMode( OpaqueMode );
  p.fillRect( OkQuality.rect(), black );
  p.setPen( Qt::NoPen );
  p.setBrush( yellow );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( yellow.light( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawEllipse( is/5, is/5, 3*is/5, 3*is/5 );
  p.drawLine( is/2, is/2, 4*is/5, 4*is/5 );
  p.end();

  PotentialQuality.resize( is, is );
  p.begin( &PotentialQuality );
  p.setBackgroundMode( OpaqueMode );
  p.fillRect( PotentialQuality.rect(), black );
  p.setPen( Qt::NoPen );
  p.setBrush( orange );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( orange.light( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawEllipse( is/5, is/5, 3*is/5, 3*is/5 );
  p.drawLine( is/2, is/2, 4*is/5, 4*is/5 );
  p.end();

  BadQuality.resize( is, is );
  p.begin( &BadQuality );
  p.setBackgroundMode( OpaqueMode );
  p.fillRect( BadQuality.rect(), black );
  p.setPen( Qt::NoPen );
  p.setBrush( red );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( red.light( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawEllipse( is/5, is/5, 3*is/5, 3*is/5 );
  p.drawLine( is/2, is/2, 4*is/5, 4*is/5 );
  p.end();

  QPointArray pa( 7 );
  BadArrow.resize( is, is );
  p.begin( &BadArrow );
  p.setBackgroundMode( OpaqueMode );
  p.fillRect( BadArrow.rect(), black );
  p.setPen( QPen( black, 1 ) );
  p.setBrush( red );
  pa.setPoint( 0, is/4, 0 );
  pa.setPoint( 1, 3*is/4, 0 );
  pa.setPoint( 2, 3*is/4, 2*is/3 );
  pa.setPoint( 3, is, 2*is/3 );
  pa.setPoint( 4, is/2, is );
  pa.setPoint( 5, 0, 2*is/3 );
  pa.setPoint( 6, is/4, 2*is/3 );
  p.drawPolygon( pa );
  p.end();

  BadTrend.resize( is, is );
  p.begin( &BadTrend );
  p.setBackgroundMode( OpaqueMode );
  p.fillRect( BadTrend.rect(), black );
  p.setPen( Qt::NoPen );
  p.setBrush( red );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( red.light( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawLine( is/2, is/4, is/2, 3*is/4 );
  p.drawLine( is/4, is/4, 3*is/4, is/4 );
  p.end();

  OkTrend.resize( is, is );
  p.begin( &OkTrend );
  p.setBackgroundMode( OpaqueMode );
  p.fillRect( OkTrend.rect(), black );
  p.setPen( Qt::NoPen );
  p.setBrush( yellow );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( yellow.light( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawLine( is/2, is/4, is/2, 3*is/4 );
  p.drawLine( is/4, is/4, 3*is/4, is/4 );
  p.end();

  GoodTrend.resize( is, is );
  p.begin( &GoodTrend );
  p.setBackgroundMode( OpaqueMode );
  p.fillRect( GoodTrend.rect(), black );
  p.setPen( Qt::NoPen );
  p.setBrush( green );
  p.drawEllipse( 1, 1, is-2, is-2 );
  p.setPen( QPen( green.light( 160 ), is/8 > 0 ? is/8 : 1 ) );
  p.drawLine( is/2, is/4, is/2, 3*is/4 );
  p.drawLine( is/4, is/4, 3*is/4, is/4 );
  p.end();

  GoodArrow.resize( is, is );
  p.begin( &GoodArrow );
  p.setBackgroundMode( OpaqueMode );
  p.fillRect( GoodArrow.rect(), black );
  p.setPen( QPen( black, 1 ) );
  p.setBrush( green );
  pa.setPoint( 0, is/4, is-1 );
  pa.setPoint( 1, 3*is/4, is-1 );
  pa.setPoint( 2, 3*is/4, is/3 );
  pa.setPoint( 3, is, is/3 );
  pa.setPoint( 4, is/2, 0 );
  pa.setPoint( 5, 0, is/3 );
  pa.setPoint( 6, is/4, is/3 );
  p.drawPolygon( pa );
  p.end();

  gl = new QGrid( 2, vb );

  // quality indicator:
  Quality = 0;
  QualityPixs[0] = &BadQuality;
  QualityPixs[1] = &PotentialQuality;
  QualityPixs[2] = &OkQuality;
  QualityPixs[3] = &GoodQuality;
  QualityIndicator = new QLabel( gl );

  // the complete dialog:
  QPushButton *pb = new QPushButton( "Dialog", gl );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( dialog( void ) ) );

  // trend indicator:
  Trend = 2;
  TrendPixs[0] = &BadArrow;
  TrendPixs[1] = &BadTrend;
  TrendPixs[2] = &OkTrend;
  TrendPixs[3] = &GoodTrend;
  TrendPixs[4] = &GoodArrow;
  TrendIndicator = new QLabel( gl );

  // help:
  pb = new QPushButton( "Help", gl );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( help( void ) ) );
}


DynamicSUSpikeDetector::~DynamicSUSpikeDetector( void )
{
}


int DynamicSUSpikeDetector::init( const InData &data, EventData &outevents,
				  const EventList &other, const EventData &stimuli )
{
  outevents.setSizeScale( 1.0 );
  outevents.setSizeUnit( data.unit() );
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


void DynamicSUSpikeDetector::notify( void )
{
  Threshold = number( "threshold" );
  MinThresh = number( "minthresh" );
  MaxThresh = number( "maxthresh" );
  RecordingDelay = number( "delay" );
  RecordingDecay = number( "decay" );
  SearchDelay = number( "searchdelay" );
  SearchDecay = number( "searchdecay" );
  Ratio = number( "ratio" );
  TestWidth = boolean( "testwidth" );
  MaxWidth = number( "maxwidth" );
  TestInterval = boolean( "testinterval" );
  MinInterval = number( "minisi" );
  FitPeak = boolean( "fitpeak" );
  FitWidth = number( "fitwidth" );
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
  double resolution = number( "resolution" );

  if ( resolution != SizeResolution && resolution > 0.0 ) {
    SizeResolution = resolution;
    // necessary precision:
    int pre = -1;
    do {
      pre++;
      double f = pow( 10.0, -pre );
      resolution -= floor( 1.001*resolution/f ) * f;
    } while ( pre < 8 && fabs( resolution ) > 1.0e-8 );
    setStep( "minthresh", SizeResolution );
    setFormat( "minthresh", 4+pre, pre, 'f' );
    setStep( "maxthresh", SizeResolution );
    setFormat( "maxthresh", 4+pre, pre, 'f' );
    setFormat( "threshold", 4+pre, pre, 'f' );
    setFormat( "size", 4+pre, pre, 'f' );
    SDW.updateSettings( "minthresh" );
    GoodSpikesHist = SampleDataD( 0.0, 200.0, SizeResolution );
    BadSpikesHist = SampleDataD( 0.0, 200.0, SizeResolution );
    AllSpikesHist = SampleDataD( 0.0, 200.0, SizeResolution );
  }
  SDW.updateValues( OptWidget::changedFlag() );
  postCustomEvent( 11 );
}


int DynamicSUSpikeDetector::adjust( const InData &data )
{
  MaxRangeThresh = ceil10( 2.0*data.maxValue(), 0.1 );
  return 0;
}


void DynamicSUSpikeDetector::save( const string &param )
{
  save();
}


void DynamicSUSpikeDetector::save( void )
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
  key.addNumber( "ampl", "mV", "%5.1f" );
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


int DynamicSUSpikeDetector::detect( const InData &data, EventData &outevents,
				    const EventList &other, const EventData &stimuli )
{
  FitIndices = data.indices( FitWidth );

  double delay = sessionRunning() ? RecordingDelay : SearchDelay;
  double decay = sessionRunning() ? RecordingDecay : SearchDecay;

  D.dynamicPeakHist( data.minBegin(), data.end(), outevents,
		     Threshold, MinThresh,
		     MaxThresh < MaxRangeThresh ? MaxThresh : MaxRangeThresh,
		     delay, decay, *this );

  // update mean spike size in case of no spikes:
  if ( StimulusRequired && stimuli.size() > 0 ) {
    double ct = data.currentTime();
    double stimulusstart = stimuli.back();
    if ( stimulusstart >= LastTime && stimulusstart < ct ) {
      IntervalWidth = stimuli.backWidth();
      StimulusEnd = stimulusstart + IntervalWidth;
      if ( IntervalWidth > NoSpikeInterval )
	IntervalWidth = NoSpikeInterval;
      IntervalStart = stimulusstart;
      IntervalEnd = IntervalStart + IntervalWidth;
    }
    LastTime = ct;
    while ( IntervalWidth > 0.0 &&
	    IntervalEnd <= StimulusEnd &&
	    IntervalEnd <= ct ) {
      if ( outevents.count( IntervalStart, IntervalEnd ) == 0 )
	outevents.updateMean( (int)::rint(IntervalWidth/NoSpikeInterval + 0.5) );
      IntervalStart = IntervalEnd;
      IntervalEnd += IntervalWidth;
    }
  }
  else {
    double ct = data.currentTime();
    if ( ct > LastTime + NoSpikeInterval ) {
      if ( outevents.size() - LastSize <= 0 )
	outevents.updateMean( (int)::rint((ct - LastTime)/NoSpikeInterval) );
      LastTime = ct;
      LastSize = outevents.size();
    }
  }

  unsetNotify();
  setNumber( "threshold", Threshold ).addFlags( OptWidget::changedFlag() );
  setNumber( "rate", outevents.meanRate() ).addFlags( OptWidget::changedFlag() );
  setNumber( "size", outevents.meanSize() ).addFlags( OptWidget::changedFlag() );
  setNotify();

  // update indicator widgets:
  if ( Update.elapsed()*0.001 < UpdateTime )
    return 0;
  Update.start();

  // histogramms:
  D.goodEvents().sizeHist( data.currentTime() - HistoryTime, data.currentTime(), GoodSpikesHist );
  D.badEvents().sizeHist( data.currentTime() - HistoryTime, data.currentTime(), BadSpikesHist );
  AllSpikesHist = GoodSpikesHist + BadSpikesHist;

  // plot:
  P->lock();
  P->clear();
  double xmax = 10.0;
  for ( int k=AllSpikesHist.size()-1; k >= 0; k-- )
    if ( AllSpikesHist[k] > 0.0 ) {
      xmax = AllSpikesHist.pos( k+1 );
      break;
    }
  if ( ! P->zoomedXRange() )
    P->setXRange( 0.0, xmax );
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
  P->plotVLine( MinThresh, Plot::Yellow, 2 );
  P->plotVLine( Threshold, Plot::White, 2 );
  P->unlock();
  P->draw();

  // indicators:

  // spikes detected:
  bool spikes = ( outevents.count( data.currentTime() - TrendTime ) > 1 );

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
    SDW.updateValues( OptWidget::changedFlag() );
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
  SDW.updateValues( OptWidget::changedFlag() );
  postCustomEvent( 11 );
  return 0;
}


int DynamicSUSpikeDetector::checkEvent( const InData::const_iterator &first, 
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
  // time of spike:
  time = *eventtime;

  // go down to the left:
  InDataIterator left = event;
  InDataTimeIterator lefttime = eventtime;
  if ( left <= first )
    return 0;
  for ( --left, --lefttime; ; --left, --lefttime ) {
    if ( left <= first )
      return 0;
    if ( *(left-1) >= *(left+1) )
      break;
    // spike too broad?
    if ( time - *lefttime > 3.0 * MaxWidth )
      break;
  }
  double base1 = *left;
  // go down to the right:
  InDataIterator right = event;
  InDataTimeIterator righttime = eventtime;
  for ( ++right, ++righttime; ; ++right, ++righttime ) {
    if ( right+1 >= last )
      return -1;
    if ( *(right+1) >= *(right-1) )
      break;
    // spike too broad?
    if ( *righttime - time > 3.0 * MaxWidth )
      break;
  }
  double base2 = *right;

  // size:
  double base = base1<base2 ? base1 : base2;
  size = *event - base;

  // width of spike:
  double minval = *event - 0.5 * size;
  InDataIterator linx = event;
  InDataTimeIterator linxtime = eventtime;
  for ( --linx, --linxtime; linx >= left; --linx, --linxtime ) {
    if ( left <= first )
      return 0;
    if ( *linx <= minval )
      break;
  }
  InDataIterator rinx = event;
  InDataTimeIterator rinxtime = eventtime;
  for ( ++rinx, ++rinxtime; rinx < right; ++rinx, ++rinxtime ) {
    if ( right >= last )
      return -1;
    if ( *rinx <= minval )
      break;
  }
  width = *rinxtime - *linxtime;
  if ( TestWidth && width > MaxWidth )
    return 0;

  if ( TestInterval && 
       outevents.size() > 0 && time - outevents.back() < MinInterval )
    return 0;

  // adjust spike time by parabula fit:
  if ( FitPeak ) {
    MapD peak;
    peak.reserve( FitIndices );
    InDataIterator peakp = event - FitIndices/2;
    InDataTimeIterator peakt = eventtime - FitIndices/2;
    for ( int k=0; k<FitIndices; k++ ) {
      peak.push( *peakt - time, *peakp );
      ++peakt;
      ++peakp;
    }
    ArrayD sd( peak.size(), 1.0 );
    Polynom poly;
    ArrayD param( 3, 1.0 );
    ArrayI paramfit( 3, 1 );
    ArrayD uncert( 3, 0.0 );
    double chisq = 0.0;
    int r = linearFit( peak.x(), peak.y(), sd,
		       poly, param, paramfit, uncert, chisq );
    if ( r == 0 ) {
      double offs = -0.5*param[1]/param[2];
      if ( offs >= peak.x().front() && offs <= peak.x().back() ) {
	time += offs;
	size += param[0] + 0.25*(param[1]*param[1]-2.0)/param[2] - *event;
      }
    }
    else
      printlog( "Parabula fit failed and returned " + Str( r ) );
  }

  // adjust threshold:
  threshold = Ratio * size;

  return 1; 
}


void DynamicSUSpikeDetector::customEvent( QCustomEvent *qce )
{
  if ( qce->type() == QEvent::User+11 ) {
    lock();
    QualityIndicator->setPixmap( *QualityPixs[Quality] );
    TrendIndicator->setPixmap( *TrendPixs[Trend] );
    unlock();
  }
  else
    Filter::customEvent( qce );
}


addDetector( DynamicSUSpikeDetector );

}; /* namespace ephys */

#include "moc_dynamicsuspikedetector.cc"
