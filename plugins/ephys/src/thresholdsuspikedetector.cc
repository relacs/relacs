/*
  ephys/thresholdsuspikedetector.cc
  Spike detection based on an absolute voltage threshold.

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
	    "Jan Benda", "1.2", "Jul 20, 2014" ),
    GoodSpikesHist( -2000.0, 2000.0, 0.5 ),
    BadSpikesHist( -2000.0, 2000.0, 0.5 ),
    AllSpikesHist( -2000.0, 2000.0, 0.5 )
{
  // parameter:
  Threshold = 1.0;
  Peaks = true;
  TestMaxSize = false;
  MaxSize = 1.0;
  TestSymmetry = false;
  MaxSymmetry = 1.0;
  MinSymmetry = -1.0;
  TestInterval = false;
  MinInterval = 0.001;
  NoSpikeInterval = 0.1;
  StimulusRequired = false;
  SnippetsTime = 1.0;
  SnippetsWidth = 0.001;
  SnippetsSymmetry = 0.1;
  LogHistogram = false;
  UpdateTime = 1.0;
  HistoryTime = 10.0;
  SizeResolution = 0.5;
  Unit = "mV";

  // options:
  int strongstyle = OptWidget::ValueLarge + OptWidget::ValueBold + OptWidget::ValueGreen + OptWidget::ValueBackBlack;
  newSection( "Detector", 8 );
  addNumber( "threshold", "Detection threshold", Threshold, -2000.0, 2000.0, SizeResolution, Unit, Unit, "%.1f", 2+8 );
  addBoolean( "peaks", "Detect peaks", Peaks, 8 );
  addBoolean( "testmaxsize", "Use maximum size", TestMaxSize, 0+8 );
  addNumber( "maxsize", "Maximum size", MaxSize, 0.0, 2000.0, SizeResolution, Unit, Unit, "%.1f", (TestMaxSize ? 2 : 0)+8 ).setActivation( "testmaxsize", "true" );
  addBoolean( "testsymmetry", "Use symmetry thresholds", TestSymmetry, 0+8 );
  addNumber( "maxsymmetry", "Maximum symmetry", MaxSymmetry, -1.0, 1.0, 0.05 ).setFlags( (TestSymmetry ? 2 : 0)+8 ).setActivation( "testsymmetry", "true" );
  addNumber( "minsymmetry", "Minimum symmetry", MinSymmetry, -1.0, 1.0, 0.05 ).setFlags( (TestSymmetry ? 2 : 0)+8 ).setActivation( "testsymmetry", "true" );
  addBoolean( "testisi", "Test interspike interval", TestInterval ).setFlags( 0+8 );
  addNumber( "minisi", "Minimum interspike interval", MinInterval, 0.0, 0.1, 0.0002, "sec", "ms", "%.1f", 0+8 ).setActivation( "testisi", "true" );
  newSection( "Indicators", 8 );
  addNumber( "nospike", "Interval for no spike", NoSpikeInterval, 0.0, 1000.0, 0.01, "sec", "ms", "%.0f", 0+8 );
  addBoolean( "considerstimulus", "Expect spikes during stimuli only", StimulusRequired, 0+8 );
  addNumber( "resolution", "Resolution of spike size", SizeResolution, 0.0, 1000.0, 0.01, Unit, Unit, "%.3f", 0+8 );
  addBoolean( "log", "Logarithmic histograms", LogHistogram, 0+8 );
  addNumber( "update", "Update time interval", UpdateTime, 0.2, 1000.0, 0.2, "sec", "sec", "%.1f", 0+8 );
  addNumber( "snippetstime", "Spike snippets shown from the last", SnippetsTime, 0, 10000.0, 0.2, "sec", "sec", "%.1f", 0+8 );
  addNumber( "snippetswidth", "Width of spike snippet", SnippetsWidth, 0.0, 1.0, 0.0005, "sec", "ms", "%.1f", 0+8 );
  addNumber( "snippetssymmetry", "Symmetry threshold for spike snippets", SnippetsSymmetry, 0.0, 1.0, 0.05 ).setFlags( 0+8 );
  addNumber( "history", "Maximum history time", HistoryTime, 0.2, 1000.0, 0.2, "sec", "sec", "%.1f", 0+8 );
  addNumber( "rate", "Rate", 0.0, 0.0, 100000.0, 0.1, "Hz", "Hz", "%.0f", 0+4 );
  addNumber( "size", "Spike size", 0.0, 0.0, 10000.0, 0.1, Unit, Unit, "%.1f", 2+4, strongstyle );

  setDialogSelectMask( 8 );
  setConfigSelectMask( -8 );

  // main layout:
  QGridLayout *gb = new QGridLayout;
  gb->setContentsMargins( 0, 0, 0, 0 );
  gb->setSpacing( 0 );
  setLayout( gb );

  // parameter widgets:
  SDW.assign( ((Options*)this), 2, 4, true, 0, mutex() );
  SDW.setMargins( 4, 2, 4, 0 );
  SDW.setVerticalSpacing( 1 );
  gb->addWidget( &SDW, 0, 1, Qt::AlignHCenter );

  LastSize = 0;
  LastTime = 0.0;
  Update.start();

  SP = new Plot( Plot::Copy );
  SP->lock();
  SP->noGrid();
  SP->setTMarg( 1 );
  SP->setBMarg( 2.5 );
  SP->setLMarg( 5 );
  SP->setRMarg( 1 );
  SP->setXLabel( "ms" );
  SP->setXLabelPos( 1.0, Plot::FirstMargin, 0.0, Plot::FirstAxis, Plot::Left, 0.0 );
  SP->setXTics();
  SP->setXRange( -1000.0*SnippetsWidth, 1000.0*SnippetsWidth );
  SP->setYRange( Plot::AutoScale, Plot::AutoScale );
  SP->setYLabel( Unit );
  SP->unlock();
  gb->addWidget( SP, 0, 0 );

  HP = new Plot( Plot::Copy );
  HP->lock();
  HP->noGrid();
  HP->setTMarg( 1 );
  HP->setBMarg( 2.5 );
  HP->setLMarg( 5 );
  HP->setRMarg( 1 );
  /*
  HP->setXLabel( Unit );
  HP->setXLabelPos( 1.0, Plot::FirstMargin, 0.0, Plot::FirstAxis, Plot::Left, 0.0 );
  HP->setXTics();
  HP->setYRange( 0.0, Plot::AutoScale );
  HP->setYLabel( "" );
  */
  HP->setXLabel( "L [" + Unit + "]" );
  HP->setXLabelPos( 1.0, Plot::FirstMargin, 0.0, Plot::FirstAxis, Plot::Left, 0.0 );
  HP->setXTics();
  //  HP->setYLabel( "R [" + Unit + "]" );
  HP->setYLabel( "Symmetry" );
  HP->unlock();
  gb->addWidget( HP, 1, 0 );

  // key to histogram plot:  XXX provide a function in Plot!
  QGridLayout *gl = new QGridLayout;
  gl->setContentsMargins( 0, 0, 0, 0 );
  gl->setVerticalSpacing( 0 );
  gl->setHorizontalSpacing( 4 );
  gb->addLayout( gl, 1, 1 );

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

  gl->setColumnStretch( 0, 1 );
  gl->setColumnStretch( 1, 4 );

}


int ThresholdSUSpikeDetector::init( const InData &data, EventData &outevents,
				    const EventList &other, const EventData &stimuli )
{
  Unit = data.unit();
  setOutUnit( "threshold", Unit );
  setOutUnit( "maxsize", Unit );
  setOutUnit( "size", Unit );
  setOutUnit( "resolution", Unit );
  setMinMax( "resolution", 0.0, 1000.0, 0.01, Unit );
  setNotify();
  notify();
  HP->lock();
  //  HP->setXLabel( Unit );
  HP->setXLabel( "L [" + Unit + "]" );
  //  HP->setYLabel( "R [" + Unit + "]" );
  HP->setYLabel( "Symmetry" );
  HP->setYRange( -1.0, 1.0 );
  HP->unlock();
  outevents.setSizeScale( 1.0 );
  outevents.setSizeUnit( Unit );
  outevents.setSizeFormat( "%5.1f" );
  outevents.setWidthScale( 1000.0 );
  outevents.setWidthUnit( "ms" );
  outevents.setWidthFormat( "%4.2f" );
  D.setHistorySize( int( HistoryTime*1000.0 ) );
  LastTime = 0.0;
  StimulusEnd = 0.0;
  IntervalStart = 0.0;
  IntervalEnd = 0.0;
  IntervalWidth = 0.0;
  D.init( data.begin(), data.end(), data.timeBegin() );
  SDW.updateSettings();
  SDW.updateValues();
  return 0;
}


void ThresholdSUSpikeDetector::readConfig( StrQueue &sq )
{
  unsetNotify(); // we have no unit of the input trace yet!
  Options::read( sq, 0, ":" );
}


void ThresholdSUSpikeDetector::notify( void )
{
  bool tms = boolean( "testmaxsize" );
  if ( tms != TestMaxSize ) {
    TestMaxSize = tms;
    if ( TestMaxSize )
      addFlags( "maxsize", 2 );
    else
      delFlags( "maxsize", 2 );
    postCustomEvent( 12 );
  }
  tms = boolean( "testsymmetry" );
  if ( tms != TestSymmetry ) {
    TestSymmetry = tms;
    if ( TestSymmetry ) {
      addFlags( "maxsymmetry", 2 );
      addFlags( "minsymmetry", 2 );
    }
    else {
      delFlags( "maxsymmetry", 2 );
      delFlags( "minsymmetry", 2 );
    }
    postCustomEvent( 12 );
  }
  Threshold = number( "threshold", Unit );
  MaxSize = number( "maxsize", Unit );
  MaxSymmetry = number( "maxsymmetry" );
  MinSymmetry = number( "minsymmetry" );
  Peaks = boolean( "peaks" );
  TestInterval = boolean( "testinterval" );
  MinInterval = number( "minisi" );
  NoSpikeInterval = number( "nospike" );
  StimulusRequired = boolean( "considerstimulus" );
  SnippetsTime = number( "snippetstime" );
  SnippetsWidth = number( "snippetswidth" );
  SnippetsSymmetry = number( "snippetssymmetry" );
  SP->lock();
  SP->setXRange( -1000.0*SnippetsWidth, 1000.0*SnippetsWidth );
  SP->unlock();
  LogHistogram = boolean( "log" );
  Parameter &ht = Options::operator[]( "history" );
  if ( ht.flags( OptWidget::changedFlag() ) ) {
    HistoryTime = ht.number();
    D.setHistorySize( int( HistoryTime*1000.0 ) );
  }
  UpdateTime = number( "update" );
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
    setFormat( "maxsize", 4+pre, pre, 'f' );
    setStep( "maxsize", SizeResolution, Unit );
    setFormat( "size", 4+pre, pre, 'f' );
    double max = 1000.0*SizeResolution;
    GoodSpikesHist = SampleDataD( -max, max, SizeResolution );
    BadSpikesHist = SampleDataD( -max, max, SizeResolution );
    AllSpikesHist = SampleDataD( -max, max, SizeResolution );
    SDW.updateSettings();
  }
  SDW.updateValues( OptWidget::changedFlag() );
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
  // set threshold:
  if ( zp < 0 ) {
    // no zero stretch:
    Threshold = Peaks ? AllSpikesHist.pos( rp+2 ) : AllSpikesHist.pos( lp-2 );
  }
  else {
    Threshold = AllSpikesHist.pos( zp+zn/2 );
  }
  unsetNotify();
  setNumber( "threshold", Threshold, Unit );
  setNotify();
  SDW.updateValues( OptWidget::changedFlag() );
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
  header.newSection( *this, "Settings" );
  header.save( df, "# " );
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
  if ( Peaks )
    D.peakHist( data.minBegin(), data.end(), outevents,
		Threshold, Threshold, Threshold, *this );
  else
    D.troughHist( data.minBegin(), data.end(), outevents,
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

  SDW.updateValues( OptWidget::changedFlag() );

  // snippets:
  int nsnippets = (int)( ::ceil( outevents.meanRate() * SnippetsTime ) );
  if ( nsnippets < 100 )
    nsnippets = 100;
  ArrayD snippetleftheight;
  snippetleftheight.reserve( nsnippets );
  ArrayD snippetrightheight;
  snippetrightheight.reserve( nsnippets );
  ArrayF snippetsymmetry;
  snippetsymmetry.reserve( nsnippets );
  ArrayD snippetwidth;
  snippetwidth.reserve( nsnippets );
  SP->lock();
  SP->clear();
  SP->plotVLine( 0.0, Plot::White, 2 );
  for ( int k=0; ; k++ ) {
    if ( k >= outevents.size() )
      break;
    double st = outevents[outevents.size()-1-k];
    if ( currentTime() - st > SnippetsTime )
      break;
    if ( st - SnippetsWidth <= data.minTime() )
      break;
    SampleDataF snippet( -SnippetsWidth, SnippetsWidth, data.stepsize(), 0.0f );
    data.copy( st, snippet );
    int k0 = snippet.index( 0.0 );
    snippet -= snippet[k0];

    k0 = data.index( st );
    float peakheight = data[ k0 ];
    // find first right minimum:
    int kr = -1;
    double rightheight = 0.0;
    if ( Peaks ) {
      for ( int k=k0+1; k<data.size()-2; k++ ) {
	if ( data[k-2] > data[k] && data[k] < data[k+2] ) {
	  rightheight = peakheight - data[k];
	  kr = k;
	  break;
	}
      }
    }
    else {
      for ( int k=k0+1; k<data.size()-2; k++ ) {
	if ( data[k-2] < data[k] && data[k] > data[k+2] ) {
	  rightheight = data[k] - peakheight;
	  kr = k;
	  break;
	}
      }
    }
    // find first left minimum:
    int kl = -1;
    double leftheight = 0.0;
    if ( Peaks ) {
      for ( int k=k0-1; k>data.minIndex()+1; k-- ) {
	if ( data[k-2] > data[k] && data[k] < data[k+2] ) {
	  leftheight = peakheight - data[k];
	  kl = k;
	  break;
	}
      }
    }
    else {
      for ( int k=k0-1; k>data.minIndex()+1; k-- ) {
	if ( data[k-2] < data[k] && data[k] > data[k+2] ) {
	  leftheight = data[k] - peakheight;
	  kl = k;
	  break;
	}
      }
    }

    snippetrightheight.push( leftheight );
    snippetleftheight.push( rightheight );

    // width:
    double minheight = snippetrightheight.back();
    if ( minheight > snippetleftheight.back() )
      minheight = snippetleftheight.back();
    double rt = 0.0;
    if ( Peaks ) {
      for ( int k=k0; k<kr; k++ ) {
	if ( data[k] < peakheight - minheight/2 ) {
	  rt = data.pos( k );
	  break;
	}
      }
    }
    else {
      for ( int k=k0; k<kr; k++ ) {
	if ( data[k] > peakheight + minheight/2 ) {
	  rt = data.pos( k );
	  break;
	}
      }
    }
    double lt = 0.0;
    if ( Peaks ) {
      for ( int k=k0; k>kl; k-- ) {
	if ( data[k] < peakheight - minheight/2 ) {
	  lt = data.pos( k );
	  break;
	}
      }
    }
    else {
      for ( int k=k0; k>kl; k-- ) {
	if ( data[k] > peakheight + minheight/2 ) {
	  lt = data.pos( k );
	  break;
	}
      }
    }
    snippetwidth.push( rt - lt );
    // plot snippet:
    float symmetry = (snippetleftheight.back() - snippetrightheight.back())/(snippetleftheight.back() + snippetrightheight.back());
    snippetsymmetry.push( symmetry );
    if ( symmetry < -SnippetsSymmetry )
      SP->plot( snippet, 1000.0, Plot::Red, 1, Plot::Solid );
    else if ( symmetry > SnippetsSymmetry )
      SP->plot( snippet, 1000.0, Plot::Yellow, 1, Plot::Solid );
    else
      SP->plot( snippet, 1000.0, Plot::Orange, 1, Plot::Solid );
  }
  SP->draw();
  SP->unlock();

  // plot snippet properties:
  HP->lock();
  HP->clear();
  HP->plot( snippetleftheight, snippetsymmetry, Plot::Transparent, 0, Plot::Solid, 
	    Plot::Circle, 3, Plot::Yellow, Plot::Yellow );
  HP->draw();
  HP->unlock();

  // histogramms:
  D.goodEvents().sizeHist( currentTime() - HistoryTime, currentTime(), GoodSpikesHist );
  D.badEvents().sizeHist( currentTime() - HistoryTime, currentTime(), BadSpikesHist );
  AllSpikesHist = GoodSpikesHist + BadSpikesHist;

  /*
  // plot:
  HP->lock();
  HP->clear();
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
  if ( xmin > Threshold )
    xmin = Threshold;
  if ( xmax < MaxSize )
    xmax = MaxSize;
  if ( ! HP->zoomedXRange() )
    HP->setXRange( xmin, xmax );
  if ( LogHistogram ) {
    SampleDataD bh( BadSpikesHist );
    for ( int k=0; k<bh.size(); k++ )
      bh[k] = bh[k] > 1.0 ? log( bh[k] ) : 0.0;
    SampleDataD gh( GoodSpikesHist );
    for ( int k=0; k<gh.size(); k++ )
      gh[k] = gh[k] > 1.0 ? log( gh[k] ) : 0.0;
    HP->plot( bh, 1.0, Plot::Red, 2, Plot::Solid );
    HP->plot( gh, 1.0, Plot::Green, 2, Plot::Solid );
    HP->noYTics();
  }
  else {
    HP->plot( BadSpikesHist, 1.0, Plot::Red, 2, Plot::Solid );
    HP->plot( GoodSpikesHist, 1.0, Plot::Green, 2, Plot::Solid );
    HP->setYTics();
  }
  HP->plotVLine( Threshold, Plot::White, 2 );
  if ( TestMaxSize )
    HP->plotVLine( MaxSize, Plot::White, 2 );
  HP->draw();
  HP->unlock();
  */

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
  time = *eventtime;
  size = fabs( *event );

  // right size:
  double rightsize = 0.0;
  InData::const_iterator right = event;
  for ( ++right; ; ++right ) {
    if ( right+2 >= last )
      return -1;
    if ( Peaks ) {
      if ( *(right+2) > *right && *(right-2) > *right ) {
	rightsize = *event - *right;
	break;
      }
    }
    else {
      if ( *(right+2) < *right && *(right-2) < *right ) {
	rightsize = *right - *event;
	break;
      }
    }
  }
  // left size:
  double leftsize = 0.0;
  InData::const_iterator left = event;
  for ( --left; ; --left ) {
    if ( left <= first+2 )
      return 0;
    if ( Peaks ) {
      if ( *(left+2) > *left && *(left-2) > *left ) {
	leftsize = *event - *left;
	break;
      }
    }
    else {
      if ( *(left+2) < *left && *(left-2) < *left ) {
	leftsize = *left - *event;
	break;
      }
    }
  }

  // size:
  size = leftsize;
  if ( size > rightsize )
    size = rightsize;

  // symmetry:
  double symmetry = ( leftsize - rightsize )/( leftsize + rightsize );

  // check:
  if ( rightsize < threshold || leftsize < threshold )
    return 0;
  if ( TestMaxSize && ( rightsize > MaxSize || leftsize > MaxSize ) )
    return 0;
  if ( TestSymmetry && ( symmetry > MaxSymmetry || symmetry < MinSymmetry ) )
    return 0;
  if ( TestInterval && 
       outevents.size() > 0 && time - outevents.back() < MinInterval )
    return 0;

  // accept:
  return 1; 
}


void ThresholdSUSpikeDetector::customEvent( QEvent *qce )
{
  if ( qce->type() == QEvent::User+12 ) {
    SDW.assign( ((Options*)this), 2, 4, true, 0, mutex() );
  }
  else
    Filter::customEvent( qce );
}


addDetector( ThresholdSUSpikeDetector, ephys );

}; /* namespace ephys */

#include "moc_thresholdsuspikedetector.cc"
