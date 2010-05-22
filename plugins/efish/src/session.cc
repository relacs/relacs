/*
  efish/session.cc
  Session for in vivo recordings in weakly electric fish.

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
#include <qapplication.h>
#include <qlayout.h>
#include <qlabel.h>
#include <relacs/tablekey.h>
#include <relacs/efish/session.h>
using namespace relacs;

namespace efish {


Session::Session( void )
  : Control( "Session", "Session", "efish",
	     "Jan Benda", "1.0", "Nov 27, 2009" ),
    EODPlot( 2, 1, true, Plot::Copy, this )
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

  // this options is needed to update LCDs via notify!
  // set by Search RePro
  addNumber( "firingrate1", "Baseline Firing Rate 1", 0.0, 0.0, 10000.0, 10.0, "Hz", "", "%.1f", 4 );
  addNumber( "firingrate2", "Baseline Firing Rate 2", 0.0, 0.0, 10000.0, 10.0, "Hz", "", "%.1f", 4 );
  addNumber( "pvalue1", "P-Value 1", 0.0, 0.0, 10.0, 0.01, "", "", "%.2f", 4 );
  addNumber( "pvalue2", "P-Value 2", 0.0, 0.0, 10.0, 0.01, "", "", "%.2f", 4 );
  setConfigSelectMask( -32 );

  // plots:
  EODPlot.lock();
  EODPlot.setCommonXRange( 0, 1 );

  EODPlot[0].setXLabel( "sec" );
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

  Numbers = new QGrid( 6, Qt::Horizontal, this );
  Numbers->setSpacing( 2 );
  QLabel *label;
  label = new QLabel( "EOD", Numbers );
  label->setAlignment( Qt::AlignCenter );
  EODRateLCD = new QLCDNumber( 4, Numbers );
  EODRateLCD->display( EODRate );
  EODRateLCD->setPalette( QPalette( green, black ) );

  SessionButton = new QPushButton( "Cell Found (Enter)", this );
  SessionButton->setMinimumSize( SessionButton->sizeHint() );
  connect( SessionButton, SIGNAL( clicked() ),
	   this, SLOT( toggleSession() ) );
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
  hide();
  int n=0;
  for ( int k=0; k<SpikeTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      QLabel *label;
      if ( n > 0 ) {
	label = new QLabel( "", Numbers );
	label = new QLabel( "", Numbers );
      }
      label = new QLabel( "Rate", Numbers );
      label->setAlignment( Qt::AlignCenter );
      FiringRateLCD[k] = new QLCDNumber( 3, Numbers );
      FiringRateLCD[k]->display( 0.0 );
      FiringRateLCD[k]->setPalette( QPalette( green, black ) );
      label = new QLabel( "P-Val", Numbers );
      label->setAlignment( Qt::AlignCenter );
      PValueLCD[k] = new QLCDNumber( 4, Numbers );
      PValueLCD[k]->display( 0.0 );
      PValueLCD[k]->setPalette( QPalette( green, black ) );
      n++;
    }
  }
  show();

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


void Session::sessionStarted( void )
{
  SessionButton->setText( "Cell Lost (Enter)" );

  EODOffset = EODRates.size() - 1;
  if ( EODOffset < 0 )
    EODOffset = 0;
}


void Session::sessionStopped( bool saved )
{
  SessionButton->setText( "Cell Found (Enter)" );
  if ( saved )
    saveEOD();
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
  double offs = EODRates.x(EODOffset);
  for ( long k=EODOffset; k < EODRates.size() && k < EODAmplitudes.size(); k++ ) {
    key.save( df, EODRates.x(k) - offs, 0 );
    key.save( df, EODRates.y(k) );
    key.save( df, EODAmplitudes.y(k) );
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
    xunit = "sec";
    xfac = 1.0;
  }
  else if ( xmax < 60.0*60.0 ) {
    xmax /= 60.0;
    xmin /=  60.0;
    xunit = "min";
    xfac = 1.0/60.0;
  }
  else {
    xmax /= 3600.0;
    xmin /= 3600.0;
    xunit = "hours";
    xfac = 1.0/3600.0;
  }

  EODPlot.lock();
  // EOD rate:
  EODPlot[0].clear();
  if ( ! EODPlot[0].zoomedXRange() && ! EODPlot[1].zoomedXRange() )
    EODPlot[0].setXRange( xmin, xmax );
  EODPlot[0].setXLabel( xunit );
  EODPlot[0].plot( EODRates, xfac, Plot::Green, 2 );

  // EOD amplitude:
  EODPlot[1].clear();
  EODPlot[1].setYLabel( "Ampl. [" + EODUnit + "]" );
  if ( ! EODPlot[0].zoomedXRange() && ! EODPlot[1].zoomedXRange() )
    EODPlot[1].setXRange( xmin, xmax );
  EODPlot[1].plot( EODAmplitudes, xfac, Plot::Red, 2 );

  EODPlot.unlock();

  EODPlot.draw();
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

      // time:
      double ct = trace( 0 ).currentTime();

      // EOD Rate:
      EODRate = eode.frequency( ct - 0.5, ct );
      EODRates.push( ct, EODRate );
      stimulusData().setNumber( "EOD Rate", EODRate );
      postCustomEvent( 11 );

      // EOD Amplitude:
      EODAmplitude = eode.meanSize( ct - 0.5, ct );
      EODAmplitudes.push( ct, EODAmplitude );
      stimulusData().setNumber( "EOD Amplitude", EODAmplitude );
      stimulusData().setUnit( "EOD Amplitude", trace( EODTrace ).unit() );

      plot();
    }
  }
}


void Session::notify( void )
{
  postCustomEvent( 12 );
}


void Session::customEvent( QCustomEvent *qce )
{
  if ( qce->type() == QEvent::User+11 ) {
    lock();
    EODRateLCD->display( rint( stimulusData().number( "EOD Rate" ) ) );
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
    Control::customEvent( qce );
}


addControl( Session );

}; /* namespace efish */

#include "moc_session.cc"
