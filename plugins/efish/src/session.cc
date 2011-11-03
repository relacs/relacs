/*
  efish/session.cc
  Session for in vivo recordings in weakly electric fish.

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
#include <QLabel>
#include <relacs/tablekey.h>
#include <relacs/efish/session.h>
using namespace relacs;

namespace efish {


Session::Session( void )
  : Control( "Session", "efish", "Jan Benda", "1.0", "Nov 27, 2009" )
{
  // parameter:
  PlotWindow = 3600.0;

  // other data:
  EODRate = 0.0;
  EODRates.clear();
  EODRates.reserve( 1000000 );  // should be enough for 24h!
  EODAmplitude = 0.0;
  EODAmplitudes.clear();
  EODAmplitudes.reserve( 1000000 );  // should be enough for 24h!
  EODUnit = "";
  EODUpdate = 30;
  WaterTemp = 0.0;
  WaterTemps.clear();
  WaterTemps.reserve( 1000000 );  // should be enough for 24h!

  // this options is needed to update LCDs via notify!
  // set by Search RePro
  addNumber( "firingrate1", "Baseline Firing Rate 1", 0.0, 0.0, 10000.0, 10.0, "Hz", "", "%.1f", 4 );
  addNumber( "firingrate2", "Baseline Firing Rate 2", 0.0, 0.0, 10000.0, 10.0, "Hz", "", "%.1f", 4 );
  addNumber( "pvalue1", "P-Value 1", 0.0, 0.0, 10.0, 0.01, "", "", "%.2f", 4 );
  addNumber( "pvalue2", "P-Value 2", 0.0, 0.0, 10.0, 0.01, "", "", "%.2f", 4 );
  setConfigSelectMask( -32 );

  // layout:
  QVBoxLayout *vb = new QVBoxLayout;
  setLayout( vb );

  // plots:
  EODPlot.lock();
  EODPlot.resize( 2, 1, true );
  EODPlot.setCommonXRange();

  EODPlot[0].setXLabel( "[sec]" );
  EODPlot[0].setXLabelPos( 1.0, Plot::FirstMargin, 0.0, Plot::FirstAxis, 
			   Plot::Left, 0.0 );
  EODPlot[0].setYFallBackRange( 800.0, 820.0 );
  EODPlot[0].setYRange( Plot::AutoScale, Plot::AutoScale );
  EODPlot[0].setMinYTics( 1.0 );
  EODPlot[0].setYLabel( "EOD freq. [Hz]" );
  EODPlot[0].setYLabelPos( 2.5, Plot::FirstMargin, 0.5, Plot::Graph, 
			   Plot::Center, -90.0 );
  EODPlot[0].setLMarg( 8.0 );
  EODPlot[0].setRMarg( 2.0 );
  EODPlot[0].setTMarg( 1.0 );
  EODPlot[0].setBMarg( 4.0 );
  EODPlot[0].setOrigin( 0.0, 0.0 );
  EODPlot[0].setSize( 1.0, 0.55 );

  EODPlot[1].setXLabel( "" );
  EODPlot[1].noXTics();
  EODPlot[1].setYFallBackRange( 0.0, 0.02 );
  EODPlot[1].setYRange( 0.0, Plot::AutoScale );
  EODPlot[1].setMinYTics( 0.01 );
  EODPlot[1].setYLabel( "Ampl. [mV/cm]" );
  EODPlot[1].setYLabelPos( 2.5, Plot::FirstMargin, 0.5, Plot::Graph, 
			   Plot::Center, -90.0 );
  EODPlot[1].setLMarg( 8.0 );
  EODPlot[1].setRMarg( 2.0 );
  EODPlot[1].setTMarg( 0.5 );
  EODPlot[1].setBMarg( 1.0 );
  EODPlot[1].setOrigin( 0.0, 0.55 );
  EODPlot[1].setSize( 1.0, 0.45 );

  EODPlot.unlock();
  vb->addWidget( &EODPlot );

  Numbers = new QGridLayout;
  Numbers->setHorizontalSpacing( 2 );
  Numbers->setVerticalSpacing( 2 );
  vb->addLayout( Numbers );
  QLabel *label;
  label = new QLabel( "EOD" );
  label->setAlignment( Qt::AlignCenter );
  Numbers->addWidget( label, 0, 0 );
  EODRateLCD = new QLCDNumber( 4 );
  EODRateLCD->display( EODRate );
  EODRateLCD->setSegmentStyle( QLCDNumber::Filled );
  QColor fg( Qt::green );
  QColor bg( Qt::black );
  QPalette qp( fg, fg, fg.lighter( 140 ), fg.darker( 170 ), fg.darker( 130 ), fg, fg, fg, bg );
  EODRateLCD->setPalette( qp );
  EODRateLCD->setAutoFillBackground( true );
  Numbers->addWidget( EODRateLCD, 0, 1 );

  SessionButton = new QPushButton( "Cell Found (Enter)" );
  SessionButton->setMinimumSize( SessionButton->sizeHint() );
  vb->addWidget( SessionButton );
  connect( SessionButton, SIGNAL( clicked() ),
	   this, SLOT( toggleSession() ) );

  TempDev = 0;
}


Session::~Session( void )
{
}


void Session::initialize( void )
{
  // get trace indices:
  ephys::Traces::initialize( this, traces(), events() );
  efield::Traces::initialize( this, traces(), events() );

  if ( EODEvents >= 0 )
    EODUnit = trace( EODTrace ).unit();

  // options that are written into the trigger file
  // the values are set in EFishSession::main()
  stimulusData().addNumber( "EOD Rate", "", EODRate, 0, 100000, 5.0, "Hz", "Hz", "%4.0f" );
  stimulusData().addNumber( "EOD Amplitude", "", EODAmplitude,
			    0, 100000, 0.1, "mV", "mV", "%6.3f" );

  // LCDs:
  unsetNotify();
  widget()->hide();
  int n=0;
  for ( int k=0; k<SpikeTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      QLabel *label = new QLabel( "Rate" );
      label->setAlignment( Qt::AlignCenter );
      Numbers->addWidget( label, k, 2 );
      FiringRateLCD[k] = new QLCDNumber( 3 );
      FiringRateLCD[k]->display( 0.0 );
      FiringRateLCD[k]->setSegmentStyle( QLCDNumber::Filled );
      FiringRateLCD[k]->setPalette( EODRateLCD->palette() );
      FiringRateLCD[k]->setAutoFillBackground( true );
      Numbers->addWidget( FiringRateLCD[k], k, 3 );
      label = new QLabel( "P-Val" );
      label->setAlignment( Qt::AlignCenter );
      Numbers->addWidget( label, k, 4 );
      PValueLCD[k] = new QLCDNumber( 4 );
      PValueLCD[k]->display( 0.0 );
      PValueLCD[k]->setSegmentStyle( QLCDNumber::Filled );
      PValueLCD[k]->setPalette( EODRateLCD->palette() );
      PValueLCD[k]->setAutoFillBackground( true );
      Numbers->addWidget( PValueLCD[k], k, 5 );
      n++;
    }
  }
  widget()->show();

  setNotify();
}


void Session::config( void )
{
  lockMetaData();

  // additional meta data properties:
  if ( ! metaData().exist( "Cell" ) )
    metaData().add( "Cell", "Cell" );

  Options &mo = metaData( "Cell" );
  mo.unsetNotify();
  mo.addLabel( "Cell properties" );
  // following options are set by the BaselineActivity RePro
  mo.addNumber( "EOD Frequency", "EOD Frequency", 0.0, 0.0, 2000.0, 10.0, "Hz", "Hz", "%0.0f" );
  for ( int k=0; k<SpikeTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      Str ns( k+1 );
      mo.addNumber( "Firing Rate"+ns, "Firing Rate "+ns, 0.0, 0.0, 2000.0, 1.0, "Hz", "Hz", "%0.0f" );
      mo.addNumber( "P-Value"+ns, "P-Value "+ns, 0.0, 0.0, 2.0, 0.1, "1", "1", "%0.2f" );
    }
  }
  mo.setNotify();

  unlockMetaData();
}


void Session::initDevices( void )
{
  TempDev = dynamic_cast< Temperature* >( device( "temp-1" ) );
  if ( TempDev != 0 ) {
    lockMetaData();
    Options &mo = metaData( "Recording" );
    mo.unsetNotify();
    mo.erase( "temp-1" );
    mo.addNumber( "temp-1", "Temperature", 0.0, "°C", "%.1f", 0, 
		  OptWidget::ValueBold + OptWidget::ValueRed + OptWidget::ValueBackBlack );
    mo.setNotify();
    unlockMetaData();
    lockStimulusData();
    stimulusData().erase( "temp-1" );
    stimulusData().addNumber( "temp-1", 0.0, "°C", "%.1f" );
    unlockStimulusData();
    // plots:
    EODPlot.lock();
    EODPlot.resize( 3, 1, true );
    EODPlot.setCommonXRange();
    EODPlot[0].setOrigin( 0.0, 0.0 );
    EODPlot[0].setSize( 1.0, 0.4 );
    EODPlot[1].setOrigin( 0.0, 0.4 );
    EODPlot[1].setSize( 1.0, 0.3 );
    EODPlot[2].setXLabel( "" );
    EODPlot[2].noXTics();
    EODPlot[2].setYFallBackRange( 20.0, 30.0 );
    EODPlot[2].setYRange( Plot::AutoScale, Plot::AutoScale );
    EODPlot[2].setMinYTics( 0.1 );
    EODPlot[2].setYLabel( "°C" );
    EODPlot[2].setYLabelPos( 2.5, Plot::FirstMargin, 0.5, Plot::Graph, 
			     Plot::Center, -90.0 );
    EODPlot[2].setLMarg( 8.0 );
    EODPlot[2].setRMarg( 2.0 );
    EODPlot[2].setTMarg( 0.5 );
    EODPlot[2].setBMarg( 1.0 );
    EODPlot[2].setOrigin( 0.0, 0.7 );
    EODPlot[2].setSize( 1.0, 0.3 );
    EODPlot.unlock();
  }
}


void Session::sessionStarted( void )
{
  SessionButton->setText( "Cell Lost (Enter)" );

  EODOffset = EODRates.size() - 1;
  if ( EODOffset < 0 )
    EODOffset = 0;

  TemperatureOffset = WaterTemps.size() - 1;
  if ( TemperatureOffset < 0 )
    TemperatureOffset = 0;
}


void Session::sessionStopped( bool saved )
{
  SessionButton->setText( "Cell Found (Enter)" );
  if ( saved ) {
    saveEOD();
    saveTemperature();
  }
}


void Session::saveEOD( void )
{
  // create file:
  ofstream df( addPath( "eodampl.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  time_t tp = ::time( 0 );
  struct tm *t = localtime( &tp );
  df << "#       time: " << Str( t->tm_hour, "%02d" ) << "h" << Str( t->tm_min, "%02d" ) << "min\n";
  df << '\n';
  TableKey key;
  key.addNumber( "time", "sec", "%9.2f" );
  key.addNumber( "f", "Hz", "%5.1f" );
  key.addNumber( "A", EODUnit, "%5.3f" );
  key.saveKey( df, true, false );

  // write data into file:
  double offs = EODRates.x( EODOffset );
  for ( long k=EODOffset; k < EODRates.size() && k < EODAmplitudes.size(); k++ ) {
    key.save( df, EODRates.x(k) - offs, 0 );
    key.save( df, EODRates.y(k) );
    key.save( df, EODAmplitudes.y(k) );
    df << '\n';
  }
  df << '\n' << '\n';
}


void Session::saveTemperature( void )
{
  if ( TempDev == 0 )
    return;

  // create file:
  ofstream df( addPath( "temperatures.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  time_t tp = ::time( 0 );
  struct tm *t = localtime( &tp );
  df << "#       time: " << Str( t->tm_hour, "%02d" ) << "h" << Str( t->tm_min, "%02d" ) << "min\n";
  df << '\n';
  TableKey key;
  key.addNumber( "time", "sec", "%9.2f" );
  key.addNumber( "T", "°C", "%5.1f" );
  key.saveKey( df, true, false );

  // write data into file:
  double offs = WaterTemps.x( TemperatureOffset );
  for ( long k=TemperatureOffset; k < WaterTemps.size(); k++ ) {
    key.save( df, WaterTemps.x(k) - offs, 0 );
    key.save( df, WaterTemps.y(k) );
    df << '\n';
  }
  df << '\n' << '\n';
}


void Session::plot( void )
{
  // time axis:
  double xmin = EODRates.x().back() - PlotWindow;
  double xmax = EODRates.x().back();
  if ( xmin < 0.0 )
    xmin = 0.0;
  string xunit = "";
  double xfac = 1.0;
  if ( xmax < 60.0 ) {
    xunit = "[sec]";
    xfac = 1.0;
  }
  else if ( xmax < 60.0*60.0 ) {
    xmax /= 60.0;
    xmin /=  60.0;
    xunit = "[min]";
    xfac = 1.0/60.0;
  }
  else {
    xmax /= 3600.0;
    xmin /= 3600.0;
    xunit = "[hours]";
    xfac = 1.0/3600.0;
  }

  EODPlot.lock();

  // zoom:
  bool zoomed = ( EODPlot[0].zoomedXRange() || EODPlot[1].zoomedXRange() );
  if ( EODPlot.size() > 2 )
    zoomed |= EODPlot[2].zoomedXRange();

  // EOD rate:
  EODPlot[0].clear();
  if ( ! zoomed )
    EODPlot[0].setXRange( xmin, xmax );
  EODPlot[0].setXLabel( xunit );
  EODPlot[0].plot( EODRates, xfac, Plot::Green, 2 );

  // EOD amplitude:
  EODPlot[1].clear();
  EODPlot[1].setYLabel( "Ampl. [" + EODUnit + "]" );
  if ( ! zoomed )
    EODPlot[1].setXRange( xmin, xmax );
  EODPlot[1].plot( EODAmplitudes, xfac, Plot::Red, 2 );

  // Temperature:
  if ( EODPlot.size() > 2 ) {
    EODPlot[2].clear();
    if ( ! zoomed )
      EODPlot[2].setXRange( xmin, xmax );
    EODPlot[2].setLabel( Str( WaterTemp, "%.1f" ) + "°C", 0.95, Plot::Graph,
			 0.1, Plot::Graph, Plot::Right, 0.0, Plot::Orange, 1.6 );
    EODPlot[2].plot( WaterTemps, xfac, Plot::Orange, 2 );
  }

  EODPlot.draw();

  EODPlot.unlock();
}


void Session::main( void )
{
  if ( EODEvents < 0 )
    return;
  const EventData &eode = events( EODEvents );
  int eodcount = 20;

  sleep( 0.5 );

  while ( ! waitOnData() ) {

    eodcount++;
    if ( eodcount >= EODUpdate ) {
      eodcount = 0;

      // EOD Rate:
      EODRate = eode.frequency( currentTime() - 0.5, currentTime() );
      EODRates.push( currentTime(), EODRate );
      stimulusData().setNumber( "EOD Rate", EODRate );
      postCustomEvent( 11 );

      // EOD Amplitude:
      EODAmplitude = eodAmplitude( trace( EODTrace ), currentTime() - 0.5, currentTime() );
      EODAmplitudes.push( currentTime(), EODAmplitude );
      stimulusData().setNumber( "EOD Amplitude", EODAmplitude );
      stimulusData().setUnit( "EOD Amplitude", trace( EODTrace ).unit() );

      // Temperature:
      if ( TempDev != 0 ) {
	WaterTemp = TempDev->temperature();
	WaterTemps.push( currentTime(), WaterTemp );
	metaData( "Recording" ).setNumber( "temp-1", WaterTemp );
	stimulusData().setNumber( "temp-1", WaterTemp );
      }

      plot();
    }
  }
}


void Session::notify( void )
{
  postCustomEvent( 12 );
}


void Session::customEvent( QEvent *qce )
{
  if ( qce->type() == QEvent::User+11 ) {
    lock();
    if ( ! stimulusDataMutex()->tryLock( 5 ) ) {
      // we do not get the lock for the stimulus data now,
      // so we repost the event to a later time.
      unlock();
      postCustomEvent( 11 );
      return;
    }
    EODRateLCD->display( rint( stimulusData().number( "EOD Rate" ) ) );
    unlockStimulusData();
    unlock();
  }
  else if ( qce->type() == QEvent::User+12 ) {
    lock();
    for ( int k=0; k<SpikeTraces; k++ ) {
      // XXX this option is set from a RePro without locking it!
      double frate = number( "firingrate" + Str( k+1 ) );
      FiringRateLCD[k]->display( floor( frate ) );
      double pval = number( "pvalue" + Str( k+1 ) );
      PValueLCD[k]->display( pval );
    }
    unlock();
  }
  else
    RELACSPlugin::customEvent( qce );
}


addControl( Session, efish );

}; /* namespace efish */

#include "moc_session.cc"
