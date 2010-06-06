/*
  ephys/intraspikedetector.cc
  A detector for spikes in intracellular recordings.

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
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPainter>
#include <QPushButton>
#include <relacs/map.h>
#include <relacs/basisfunction.h>
#include <relacs/fitalgorithm.h>
#include <relacs/str.h>
#include <relacs/tablekey.h>
#include <relacs/ephys/intraspikedetector.h>
using namespace relacs;

namespace ephys {


IntraSpikeDetector::IntraSpikeDetector( const string &ident, int mode )
  : Filter( ident, mode, SingleAnalogDetector, 1,
	    "IntraSpikeDetector", "EPhys", "Jan Benda", "1.0", "Mar 16, 2010" ),
    GoodSpikesHist( 0.0, 200.0, 0.5 ),
    BadSpikesHist( 0.0, 200.0, 0.5 ),
    AllSpikesHist( 0.0, 200.0, 0.5 )
{
  // parameter:
  Threshold = 10.0;
  AbsPeak = 0.0;
  TestWidth = true;
  MaxWidth = 0.0015;
  FitPeak = false;
  FitWidth = 0.0005;
  FitIndices = 0;
  UpdateTime = 1.0;
  LogHistogram = false;
  HistoryTime = 10.0;
  SizeResolution = 1.0;

  // options:
  int strongstyle = OptWidget::ValueLarge + OptWidget::ValueBold + OptWidget::ValueGreen + OptWidget::ValueBackBlack;
  addLabel( "Detector", 8 );
  addNumber( "threshold", "Detection threshold", Threshold, 0.0, 200.0, 1.0, "mV", "mV", "%.1f", 2+8+32 );
  addNumber( "abspeak", "Absolute threshold", AbsPeak, -200.0, 200.0, 1.0, "mV", "mV", "%.1f", 2+8+32 ).setActivation( "testpeak", "true" );
  addBoolean( "testwidth", "Test spike width", TestWidth ).setFlags( 0+8+32 );
  addNumber( "maxwidth", "Maximum spike width", MaxWidth, 0.0001, 1.000, 0.0001, "sec", "ms", "%.1f", 0+8+32 ).setActivation( "testwidth", "true" );
  addBoolean( "fitpeak", "Fit parabula to peak of spike", FitPeak ).setFlags( 0+8+32 );
  addNumber( "fitwidth", "Width of parabula fit", FitWidth, 0.0, 0.1, 0.00001, "sec", "ms", "%.2f", 0+8+32 );
  addLabel( "Indicators", 8 );
  addNumber( "resolution", "Resolution of spike size", SizeResolution, 0.0, 1000.0, 0.1, "mV", "mV", "%.2f", 0+8+32 );
  addBoolean( "log", "Logarithmic histograms", LogHistogram, 0+8+32 );
  addNumber( "update", "Update time interval", UpdateTime, 0.2, 1000.0, 0.2, "sec", "sec", "%.1f", 0+8+32 );
  addNumber( "history", "Maximum history time", HistoryTime, 0.2, 1000.0, 0.2, "sec", "sec", "%.1f", 0+8+32 );
  addNumber( "rate", "Rate", 0.0, 0.0, 100000.0, 0.1, "Hz", "Hz", "%.0f", 0+4 );
  addNumber( "size", "Spike size", 0.0, 0.0, 10000.0, 0.1, "mV", "mV", "%.1f", 2+4, strongstyle );
  addTypeStyle( OptWidget::Bold, Parameter::Label );

  // main layout:
  QVBoxLayout *vb = new QVBoxLayout;
  vb->setContentsMargins( 0, 0, 0, 0 );
  vb->setSpacing( 0 );
  setLayout( vb );

  // parameter widgets:
  SDW.assign( ((Options*)this), 2, 4, true, 0, mutex() );
  SDW.setMargins( 4, 2, 4, 0 );
  SDW.setVerticalSpacing( 1 );
  vb->addWidget( &SDW, 0, Qt::AlignHCenter );

  setDialogSelectMask( 8 );
  setDialogReadOnlyMask( 16 );
  setConfigSelectMask( -32 );

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
  P->setXLabel( "mV" );
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

  pm.fill( Qt::yellow );
  key = new QLabel;
  key->setPixmap( pm );
  gl->addWidget( key, 3, 0, Qt::AlignRight | Qt::AlignVCenter );
  key = new QLabel( "min thresh" );
  key->setFixedHeight( is );
  gl->addWidget( key, 3, 1, Qt::AlignLeft );

  // dialog:
  QPushButton *pb = new QPushButton( "Dialog" );
  gl->addWidget( pb, 4, 1, Qt::AlignRight );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( dialog( void ) ) );

  // help:
  pb = new QPushButton( "Help" );
  gl->addWidget( pb, 5, 1, Qt::AlignRight );
  connect( pb, SIGNAL( clicked( void ) ), this, SLOT( help( void ) ) );
}


IntraSpikeDetector::~IntraSpikeDetector( void )
{
}


int IntraSpikeDetector::init( const InData &data, EventData &outevents,
			      const EventList &other, const EventData &stimuli )
{
  outevents.setSizeScale( 1.0 );
  outevents.setSizeUnit( data.unit() );
  outevents.setSizeFormat( "%5.1f" );
  outevents.setWidthScale( 1000.0 );
  outevents.setWidthUnit( "ms" );
  outevents.setWidthFormat( "%4.2f" );
  D.setHistorySize( int( HistoryTime*1000.0 ) );
  D.init( data.begin(), data.end(), data.timeBegin() );
  return 0;
}


void IntraSpikeDetector::notify( void )
{
  Threshold = number( "threshold" );
  AbsPeak = number( "abspeak" );
  TestWidth = boolean( "testwidth" );
  MaxWidth = number( "maxwidth" );
  FitPeak = boolean( "fitpeak" );
  FitWidth = number( "fitwidth" );
  UpdateTime = number( "update" );
  LogHistogram = boolean( "log" );
  Parameter &ht = Options::operator[]( "history" );
  if ( ht.flags( OptWidget::changedFlag() ) ) {
    HistoryTime = ht.number();
    D.setHistorySize( int( HistoryTime*1000.0 ) );
  }
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
    setFormat( "threshold", 4+pre, pre, 'f' );
    setFormat( "size", 4+pre, pre, 'f' );
    SDW.updateSettings( "minthresh" );
    GoodSpikesHist = SampleDataD( 0.0, 200.0, SizeResolution );
    BadSpikesHist = SampleDataD( 0.0, 200.0, SizeResolution );
    AllSpikesHist = SampleDataD( 0.0, 200.0, SizeResolution );
  }
  SDW.updateValues( OptWidget::changedFlag() );
}


void IntraSpikeDetector::save( const string &param )
{
  save();
}


void IntraSpikeDetector::save( void )
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


int IntraSpikeDetector::detect( const InData &data, EventData &outevents,
				const EventList &other, const EventData &stimuli )
{
  FitIndices = data.indices( FitWidth );

  D.peakHist( data.minBegin(), data.end(), outevents,
	      Threshold, Threshold, Threshold, *this );

  unsetNotify();
  setNumber( "threshold", Threshold ).addFlags( OptWidget::changedFlag() );
  setNumber( "rate", outevents.meanRate() ).addFlags( OptWidget::changedFlag() );
  setNumber( "size", outevents.meanSize() ).addFlags( OptWidget::changedFlag() );
  setNotify();
  notify();

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
  P->plotVLine( Threshold, Plot::White, 2 );
  P->unlock();
  P->draw();

  return 0;
}


int IntraSpikeDetector::checkEvent( const InData::const_iterator &first, 
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

  // absolute height:
  if ( *event < AbsPeak )
    return 0;

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
      time += -0.5*param[1]/param[2];
      size += param[0] + 0.25*(param[1]*param[1]-2.0)/param[2] - *event;
    }
    else
      printlog( "Parabula fit failed and returned " + Str( r ) );
  }

  return 1; 
}


addDetector( IntraSpikeDetector );

}; /* namespace ephys */

#include "moc_intraspikedetector.cc"
