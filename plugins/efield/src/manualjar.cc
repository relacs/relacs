/*
  efield/manualjar.cc
  Apply sinewaves with manually set difference frequencies and amplitudes.

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
#include <relacs/map.h>
#include <relacs/eventdata.h>
#include <relacs/tablekey.h>
#include <relacs/base/linearattenuate.h>
#include <relacs/efield/manualjar.h>
using namespace relacs;

namespace efield {


ManualJAR::ManualJAR( void )
  : RePro( "ManualJAR", "efield", "Jan Benda", "1.0", "Jan 29, 2013" )
{
  // dialog:
  addNumber( "eodf", "Current EOD frequency", 0.0, 0.0, 10000.0, 1.0, "Hz", "Hz", "%.1f" ).setFlags( 2+4 );
  addNumber( "deltaf", "Difference frequency", 0.0, -1000.0, 1000.0, 1.0, "Hz", "Hz", "%.1f" ).setFlags( 2 );
  addNumber( "amplitude", "Amplitude", 1.0, 0.0, 1000.0, 0.1, "mV", "mV", "%.1f" ).setFlags( 2 );

  // add some parameter as options:
  //  newSection( "Stimulation" );
  addNumber( "duration", "Signal duration", 10.0, 0.0, 1000000.0, 1.0, "seconds" ).setFlags( 1+2 );
  addNumber( "ramp", "Duration of linear ramp", 0.5, 0, 10000.0, 0.1, "seconds" ).setFlags( 1 );
  addNumber( "fakefish", "Assume a fish with frequency", 0.0, 0.0, 2000.0, 10.0, "Hz" ).setFlags( 1 );
  //  newSection( "Analysis" );
  addNumber( "before", "Time before stimulation to be analyzed", 1.0, 0.0, 100000.0, 1.0, "seconds" ).setFlags( 1 );
  addNumber( "after", "Time after stimulation to be analyzed", 1.0, 0.0, 100000.0, 1.0, "seconds" ).setFlags( 1 );
  addNumber( "averagetime", "Time for computing EOD frequency", 1.0, 0.0, 100000.0, 1.0, "seconds" ).setFlags( 1 );
  addBoolean( "split", "Save each run into a separate file", false ).setFlags( 1 );
  addBoolean( "savetraces", "Save traces during pause", false ).setFlags( 1 );
  setConfigSelectMask( 1 );
  setDialogSelectMask( 1 );

  // layout:
  QVBoxLayout *vb = new QVBoxLayout;
  setLayout( vb );

  // plot:
  P.lock();
  P.setXLabel( "[sec]" );
  P.setYRange( Plot::AutoScale, Plot::AutoScale );
  P.setYLabel( "EOD [Hz]" );
  P.setLMarg( 6 );
  P.setRMarg( 1 );
  P.setTMarg( 3 );
  P.setBMarg( 4 );
  P.unlock();
  vb->addWidget( &P );

  QHBoxLayout *bb = new QHBoxLayout;
  bb->setSpacing( 4 );
  vb->addLayout( bb );

  // display values:
  JW.assign( (Options*)this, 2, 4, true, 0, mutex() );
  JW.setVerticalSpacing( 2 );
  JW.setMargins( 4 );
  bb->addWidget( &JW );

  bb->addWidget( new QLabel() );

  QVBoxLayout *tl = new QVBoxLayout;
  bb->addLayout( tl );

  // elapsed time:
  tl->addWidget( new QLabel() );
  ModeLabel = new QLabel( "Pause" );
  ModeLabel->setAlignment( Qt::AlignHCenter );
  tl->addWidget( ModeLabel );
  ElapsedTimeLabel = new QLabel;
  ElapsedTimeLabel->setAlignment( Qt::AlignHCenter );
  tl->addWidget( ElapsedTimeLabel );
  tl->addWidget( new QLabel() );

  bb->addWidget( new QLabel() );

  // start button:
  StartButton = new QPushButton( "&Start" );
  bb->addWidget( StartButton );
  connect( StartButton, SIGNAL( clicked() ),
	   this, SLOT( setValues() ) );
  grabKey( Qt::ALT+Qt::Key_S );

  Count = 0;
}


void ManualJAR::setValues( void )
{
  Start = true;
  JW.accept();
  wake();
}


class ManualJAREvent : public QEvent
{

public:

  ManualJAREvent( double val )
    : QEvent( Type( User+14 ) ),
      Value( val )
  {
  }

  double Value;
};


int ManualJAR::main( void )
{
  // get options:
  double duration = number( "duration" );
  double ramp = number( "ramp" );
  double before = number( "before" );
  double after = number( "after" );
  double averagetime = number( "averagetime" );
  bool split = boolean( "split" );
  bool savetraces = boolean( "savetraces" );
  double fakefish = number( "fakefish" );
  if ( fakefish > 0.0 ) {
    warning( "Do you really want a fish with frequency " + Str( fakefish )
	     + " Hz to be simulated? Switch this off by setting the fakefish option to zero." );
  }
  if ( EODTrace < 0 ) {
    warning( "need a recording of the EOD Trace." );
    return Failed;
  }
  if ( EODEvents < 0 ) {
    warning( "need EOD events of the EOD Trace." );
    return Failed;
  }

  // check gain of attenuator:
  base::LinearAttenuate *latt =
    dynamic_cast<base::LinearAttenuate*>( attenuator( outTraceName( GlobalEField ) ) );
  if ( fakefish == 0.0 && latt != 0 && fabs( latt->gain() - 1.0 ) < 1.0e-8 )
    warning( "Attenuator gain is probably not set!" );

  // plot trace:
  tracePlotContinuous();

  // plot:
  P.lock();
  P.clear();
  P.setXRange( -before, duration+after );
  P.plotVLine( 0.0 );
  P.plotVLine( duration );
  P.draw();
  P.unlock();

  Start = false;
  double starttime = currentTime();
  keepFocus();

  while ( softStop() == 0 ) {

    postCustomEvent( 11 ); // setFocus();

    do {
      // EOD rate:
      setNumber( "eodf", events( EODEvents ).frequency( currentTime()-averagetime, currentTime() ) );
      postCustomEvent( 13 );
      QCoreApplication::postEvent( this, new ManualJAREvent( currentTime() - starttime ) );
      sleepWait( 0.2 );
    } while ( ( ! Start && ! interrupt() && softStop() == 0 ) ||
	      currentTime() - starttime < before );

    postCustomEvent( 12 ); // removeFocus();

    if ( interrupt() || softStop() > 0 )
      break;

    double deltaf = number( "deltaf" );
    double amplitude = number( "amplitude" );
    duration = number( "duration" );
    double pause = currentTime() - starttime;
    double fishrate = events( EODEvents ).frequency( currentTime()-averagetime, currentTime() );
    if ( fakefish > 0.0 )
      fishrate = fakefish;

    Start = false;
    setSaving( true );

    // create signal:
    OutData signal;
    signal.setTrace( GlobalEField );
    unlockAll();
    double stimulusrate = fishrate + deltaf;
    double p = 1.0;
    if ( fabs( deltaf ) > 0.01 )
      p = rint( stimulusrate / fabs( deltaf ) ) / stimulusrate;
    else
      p = 1.0/stimulusrate;
    int n = (int)::rint( duration / p );
    if ( n < 1 )
      n = 1;
    signal.sineWave( n*p, -1.0, stimulusrate, 1.0, ramp );
    signal.setIdent( "sinewave" );
    lockAll();
    duration = signal.length();
    signal.setDelay( before );
    signal.setIntensity( amplitude );

    // output signal:
    starttime = currentTime();
    write( signal );

    // signal failed?
    if ( signal.failed() ) {
      string s = "Output of stimulus failed!<br>Error code is <b>";
      s += signal.errorText() + "</b>";
      warning( s, 2.0 );
      writeZero( GlobalEField );
      return Failed;
    }

    // meassage:
    Str s = "Delta F:  <b>" + Str( deltaf, 0, 1, 'f' ) + "Hz</b>";
    s += "  Amplitude: <b>" + Str( amplitude, "%g" ) + "mV/cm</b>";
    message( s );

    // results:
    MapD eodfrequency;
    eodfrequency.reserve( (int)::ceil( 1000.0*(before+duration+after) ) );
    MapD eodamplitude;
    eodamplitude.reserve( (int)::ceil( 1000.0*(before+duration+after) ) );
    EventData jarchirpevents;
    jarchirpevents.reserve( 1000 );
    const EventData &eodglobal = events( EODEvents );
    EventIterator eoditer = eodglobal.begin( signalTime() - before );
    for ( int k=0; k<10; k++ )
      ++eoditer; // XXX

    P.lock();
    P.setXRange( -before, duration+after );
    P.unlock();

    // stimulation loop:
    do {
      setNumber( "eodf", events( EODEvents ).frequency( currentTime()-averagetime, currentTime() ) );
      postCustomEvent( 13 );
      QCoreApplication::postEvent( this, new ManualJAREvent( currentTime() - starttime ) );
      sleepWait( 0.2 );
      if ( interrupt() ) {
	writeZero( GlobalEField );
	return Aborted;
      }
      // get data:
      for ( ; eoditer < eodglobal.end(); ++eoditer ) {
	EventFrequencyIterator fiter = eoditer;
	eodfrequency.push( fiter.time() - signalTime(), *fiter );
	EventSizeIterator siter = eoditer;
	eodamplitude.push( siter.time() - signalTime(), *siter );
      }
      plot( deltaf, amplitude, duration, eodfrequency, jarchirpevents );

    } while ( currentTime() - starttime < before + duration );

    starttime = currentTime();
    postCustomEvent( 15 );

    // after stimulus recording loop:
    do {
      setNumber( "eodf", events( EODEvents ).frequency( currentTime()-averagetime, currentTime() ) );
      postCustomEvent( 13 );
      QCoreApplication::postEvent( this, new ManualJAREvent( currentTime() - starttime ) );
      sleepWait( 0.2 );
      if ( interrupt() ) {
	writeZero( GlobalEField );
	return Aborted;
      }
      // get data:
      for ( ; eoditer < eodglobal.end(); ++eoditer ) {
	EventFrequencyIterator fiter = eoditer;
	eodfrequency.push( fiter.time() - signalTime(), *fiter );
	EventSizeIterator siter = eoditer;
	eodamplitude.push( siter.time() - signalTime(), *siter );
      }
      plot( deltaf, amplitude, duration, eodfrequency, jarchirpevents );

    } while ( currentTime() - starttime < after );

    // analyze:
    // chirps:
    if ( ChirpEvents >= 0 )
      jarchirpevents.assign( events( ChirpEvents ),
			     signalTime() - before,
			     signalTime() + duration + after, signalTime() );
    plot( deltaf, amplitude, duration, eodfrequency, jarchirpevents );
    save( deltaf, amplitude, duration, pause, fishrate, stimulusrate,
	  eodfrequency, eodamplitude, jarchirpevents, split, Count );
    Count++;

    setSaving( savetraces );
  }

  writeZero( GlobalEField );
  return Completed;
}


void ManualJAR::sessionStarted( void )
{
  Count = 0;
  RePro::sessionStarted();
}


void ManualJAR::plot( double deltaf, double amplitude, double duration,
		      const MapD &eodfrequency, const EventData &jarchirpevents )
{
  P.lock();
  // eod frequency with chirp events:
  P.clear();
  Str s;
  s = "Delta f = " + Str( deltaf, 0, 0, 'f' ) + "Hz";
  s += ", Amplitude = " + Str( amplitude ) + "mV/cm";
  P.setTitle( s );
  P.plotVLine( 0.0 );
  P.plotVLine( duration );
  P.plot( eodfrequency, 1.0, Plot::Green, 2, Plot::Solid );
  P.plot( jarchirpevents, 2, 0.0, 1.0, 0.9, Plot::Graph,
	  1, Plot::Circle, 5, Plot::Pixel, Plot::Yellow, Plot::Yellow );
  P.draw();
  P.unlock();
}


void ManualJAR::save( double deltaf, double amplitude, double duration, double pause,
		      double fishrate, double stimulusrate,
		      const MapD &eodfrequency, const MapD &eodamplitude, const EventData &jarchirpevents,
		      bool split, int count )
{
  Options header;
  header.addNumber( "Delta f", deltaf, "Hz", "%.1f" );
  header.addNumber( "EODf", fishrate, "Hz", "%.1f" );
  header.addNumber( "StimulusFrequency", stimulusrate, "Hz", "%.1f" );
  header.addNumber( "Amplitude", amplitude, "mV/cm", "%.3f" );
  header.addNumber( "Duration", duration, "sec", "%.3f" );
  header.addNumber( "Pause", pause, "sec", "%.3f" );
  header.addText( "RePro Time", reproTimeStr() );
  header.addText( "Session Time", sessionTimeStr() );
  header.newSection( settings(), 1 );

  unlockAll();
  setWaitMouseCursor();
  saveEODFreq( header, eodfrequency, eodamplitude, split, count );
  saveChirps( header, jarchirpevents, split, count );
  restoreMouseCursor();
  lockAll();
}


void ManualJAR::saveEODFreq( const Options &header,
			     const MapD &eodfrequency, const MapD &eodamplitude, bool split, int count )
{
  ofstream df( addPath( "manualjar-eod" + ( split ? "-"+Str( count+1, 2, '0' ) : "" ) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  header.save( df, "# " );
  df << '\n';

  // write key:
  const EventData &eodglobal = events( EODEvents );
  TableKey key;
  key.addNumber( "time", "s", "%11.7f" );
  key.addNumber( "freq", "Hz", "%6.2f" );
  key.addNumber( "ampl", eodglobal.sizeUnit(), eodglobal.sizeFormat() );
  key.saveKey( df );

  // write data into file:
  for ( int k=0; k<eodfrequency.size(); k++ ) {
    key.save( df, eodfrequency.x(k), 0 );
    key.save( df, eodfrequency.y(k) );
    key.save( df, eodglobal.sizeScale() * eodamplitude.y(k) );
    df << '\n';
  }
  df << "\n\n";
}


void ManualJAR::saveChirps( const Options &header, const EventData &jarchirpevents, bool split, int count )
{
  if ( ChirpEvents < 0 )
    return;

  ofstream df( addPath( "manualjar-chirps" + ( split ? "-"+Str( count+1, 2, '0' ) : "" ) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  header.save( df, "# " );
  df << '\n';

  // write key:
  const EventData &chirps = events( ChirpEvents );
  TableKey key;
  key.addNumber( "time", "s", "%9.5f" );
  key.addNumber( "freq", chirps.sizeUnit(), chirps.sizeFormat() );
  key.addNumber( "width", chirps.widthUnit(), chirps.widthFormat() );
  key.saveKey( df );

  // write data into file:
  for ( int k=0; k<jarchirpevents.size(); k++ ) {
    key.save( df, jarchirpevents[k], 0 );
    key.save( df, chirps.sizeScale() * jarchirpevents.eventSize( k ) );
    key.save( df, chirps.widthScale() * jarchirpevents.eventWidth( k ) );
    df << '\n';
  }
  if ( jarchirpevents.size() <= 0 ) {
    key.save( df, "-0", 0 );
    key.save( df, "-0" );
    key.save( df, "-0" );
    df << '\n';
  }
  df << "\n\n";
}



void ManualJAR::keyPressEvent( QKeyEvent *e )
{
  if ( e->key() == Qt::Key_S && ( e->modifiers() & Qt::AltModifier ) ) {
    StartButton->animateClick();
    e->accept();
  }
  else
    RePro::keyPressEvent( e );
}


void ManualJAR::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 11: {
    StartButton->setEnabled( true );
    if ( JW.firstWidget() != 0 )
      JW.firstWidget()->setFocus( Qt::TabFocusReason );
    QFont nf( widget()->font() );
    nf.setPointSize( 3 * widget()->fontInfo().pointSize() / 2 );
    ModeLabel->setFont( nf );
    ModeLabel->setText( "Pause" );
    nf.setPointSize( 2 * widget()->fontInfo().pointSize() );
    nf.setBold( true );
    ElapsedTimeLabel->setFont( nf );
    break;
  }
  case 12: {
    removeFocus();
    ModeLabel->setText( "Stimulation" );
    StartButton->setEnabled( false );
    break;
  }
  case 13: {
    JW.updateValue( "eodf" );
    break;
  }
  case 14: {
    ManualJAREvent *me = dynamic_cast<ManualJAREvent*>( qce );
    int secs = (int)::round( me->Value );
    int mins = secs / 60;
    secs -= mins * 60;
    int hours = mins / 60;
    mins -= hours * 60;
    string ts = "";
    if ( hours > 0 )
      ts += Str( hours ) + ":";
    ts += Str( mins, 2, '0' ) + ":";
    ts += Str( secs, 2, '0' );
    ElapsedTimeLabel->setText( ts.c_str() );
    break;
  }
  case 15: {
    ModeLabel->setText( "Pause" );
    break;
  }
  default:
    RePro::customEvent( qce );
  }
}


addRePro( ManualJAR, efield );

}; /* namespace efield */

#include "moc_manualjar.cc"