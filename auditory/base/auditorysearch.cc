/*
  auditorysearch.cc
  Periodically emits a search stimulus.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <qhbox.h>
#include <qvbox.h>
#include <qlabel.h>
#include <qkeycode.h>
#include <relacs/tablekey.h>
#include <relacs/lcdrange.h>
#include <relacs/auditorysearch.h>


const double AuditorySearch::ShortIntensityStep = 1.0;
const double AuditorySearch::LongIntensityStep = 5.0;
const double AuditorySearch::MaxIntensity = 100.0;
const double AuditorySearch::MinIntensity = 0.0;  

const double AuditorySearch::ShortDurationStep = 0.005;
const double AuditorySearch::LongDurationStep = 0.05;
const double AuditorySearch::MaxDuration = 10.0;
const double AuditorySearch::MinDuration = 0.005;

const double AuditorySearch::ShortPauseStep = 0.005;
const double AuditorySearch::LongPauseStep = 0.05;
const double AuditorySearch::MaxPause = 10.0;
const double AuditorySearch::MinPause = 0.005;

const double AuditorySearch::ShortFrequencyStep = 500.0;
const double AuditorySearch::LongFrequencyStep = 5000.0;
const double AuditorySearch::MaxFrequency = 40000.0;
const double AuditorySearch::MinFrequency = 2000.0;


AuditorySearch::AuditorySearch( void )
  : RePro( "AuditorySearch", "Search", "Auditory",
	   "Jan Benda and Christian Machens", "2.2", "Jan 10, 2008" ),
    AuditoryTraces()
{
  // parameter:
  Intensity = 80.0;
  Duration = 0.05;
  Pause = 0.5;
  PrePause = 0.05;
  Frequency = 5000.0;
  Waveform = 0;
  SearchLeft = false;
  SetBestSide = 1;

  // options:
  addNumber( "intensity", "Intensity",  Intensity, MinIntensity, MaxIntensity,
	     ShortIntensityStep, "dB", "dB", "%.1f" ).setActivation( "mute", "false" );
  addBoolean( "mute", "No stimulus", false );
  addNumber( "duration", "Duration of stimulus", Duration, MinDuration, 
	     MaxDuration, ShortDurationStep, "sec", "ms" );
  addNumber( "pause", "Duration of pause", Pause, MinPause, MaxPause, 
	     ShortPauseStep, "sec", "ms" );
  addNumber( "prepause", "Part of pause before stimulus", PrePause, 0.0, MaxPause, 
	     ShortPauseStep, "sec", "ms" );
  addNumber( "frequency", "Frequency of stimulus", Frequency, MinFrequency, 
	     MaxFrequency, ShortFrequencyStep, "Hz", "kHz" );
  addSelection( "waveform", "Waveform of stimulus", "sine|noise" );
  addNumber( "ramp", "Ramp", 0.002, 0.0, 10.0, 0.001, "sec", "ms" );
  addSelection( "side", "Speaker", "left|right|best" );
  addInteger( "repeats", "Number of repetitions", 0, 0, 10000, 2 );
  addBoolean( "adjust", "Adjust input gains", true );
  addSelection( "setbestside", "Set the sessions's best side", "never|no session|always" );
  addBoolean( "keep", "Keep changes", true );

  // variables:
  NewSignal = true;
  Mute = false;

  // layout:
  delete boxLayout();
  QGridLayout *grid = new QGridLayout( this, 2, 2 );
  grid->setAutoAdd( true );

  // Intensity Settings:
  LCDRange *lcd;
  lcd = new LCDRange( "Intensity (dB)", this, "LCD", 3 );
  lcd->setRange( int(MinIntensity), int(MaxIntensity) );
  lcd->setValue( int(Intensity) );
  lcd->setSteps( int(ShortIntensityStep), int(LongIntensityStep) );
  connect( lcd, SIGNAL( valueChanged( int ) ), 
	   this, SLOT( setIntensity( int ) ) );
  connect( this, SIGNAL( intensityChanged( int ) ), 
	   lcd, SLOT( setValue( int ) ) );

  QVBox *vbox = new QVBox( this );

  QHBox *hbox = new QHBox( vbox );
  // Duration Settings:
  lcd = new LCDRange( "Stimulus (msec)", hbox, "Noise", 4 );
  lcd->setRange( int(1000.0*MinDuration), int(1000.0*MaxDuration) );
  lcd->setValue( int(1000.0*Duration) );
  lcd->setSteps( int(1000.0*ShortDurationStep), int(1000.0*LongDurationStep) );
  connect( lcd, SIGNAL( valueChanged( int ) ), 
	   this, SLOT( setDuration( int ) ) );
  connect( this, SIGNAL( durationChanged( int ) ), 
	   lcd, SLOT( setValue( int ) ) );

  // Pause Settings:
  lcd = new LCDRange( "Pause (msec)", hbox, "Pause", 4 );
  lcd->setRange( int(1000.0*MinPause), int(1000.0*MaxPause) );
  lcd->setValue( int(1000.0*Pause) );
  lcd->setSteps( int(1000.0*ShortPauseStep), int(1000.0*LongPauseStep) );
  connect( lcd, SIGNAL( valueChanged( int ) ), 
	   this, SLOT( setPause( int ) ) );
  connect( this,  SIGNAL( pauseChanged( int ) ), 
	   lcd, SLOT( setValue( int ) ) );

  hbox = new QHBox( vbox );
  // Waveform:
  WaveformButtons = new QButtonGroup( 1, Qt::Horizontal, "Waveform", hbox );
  new QRadioButton( "Sine", WaveformButtons );
  new QRadioButton( "Noise", WaveformButtons );
  WaveformButtons->setButton( 0 );
  connect( WaveformButtons, SIGNAL( clicked( int ) ), 
	   this, SLOT( setWaveform( int ) ) );
  connect( this, SIGNAL( waveformChanged( int ) ), 
	   this, SLOT( setWaveformButton( int ) ) );

  // Frequency Settings:
  lcd = new LCDRange( "Frequency (Hz)", hbox, "Frequency", 5 );
  lcd->setRange( int(MinFrequency), int(MaxFrequency) );
  lcd->setValue( int(Frequency) );
  lcd->setSteps( int(ShortFrequencyStep), int(LongFrequencyStep) );
  connect( lcd, SIGNAL( valueChanged( int ) ), 
	   this, SLOT( setFrequency( int ) ) );
  connect( this,  SIGNAL( frequencyChanged( int ) ), 
	   lcd, SLOT( setValue( int ) ) );

  // mute button:
  MuteButton = new QPushButton( this );
  MuteButton->setToggleButton( true );
  MuteButton->setText( "Mute" );
  connect( MuteButton, SIGNAL( clicked() ), 
	   this, SLOT( toggleMute() ) ); 

  // SearchSide Settings:
  hbox = new QHBox( this );
  new QLabel( "Speaker:", hbox );
  LeftButton = new QRadioButton( "left", hbox );
  RightButton = new QRadioButton( "right", hbox );
  connect( LeftButton, SIGNAL( clicked() ), 
	   this, SLOT( setSpeakerLeft() ) ); 
  connect( RightButton, SIGNAL( clicked() ), 
	   this, SLOT( setSpeakerRight() ) ); 
}


AuditorySearch::~AuditorySearch( void )
{
}


int AuditorySearch::main( void )
{
  // get options:
  Intensity = int( number( "intensity" ) );
  Mute = boolean( "mute" );
  Duration = number( "duration" );
  Pause = number( "pause" );
  PrePause = number( "prepause" );
  Frequency = int( number( "frequency" ) );
  Waveform = index( "waveform" );
  double ramp = number( "ramp" );
  int side = index( "side" );
  int repeats = integer( "repeats" );
  bool adjustgain = boolean( "adjust" );
  SetBestSide = index( "setbestside" );
  bool keepchanges = boolean( "keep" );

  if ( side > 1 )
    side = metaData().index( "best side" );
  SearchLeft = ( side == 0 );

  // update widgets:
  postCustomEvent( 1 );

  // don't print repro message:
  if ( repeats <= 0 )
    noMessage();

  if ( SetBestSide + ( sessionRunning() ? 0 : 1 ) > 1 )
    metaData().selectText( "best side", SearchLeft ? "left" : "right" );

  // Header:
  Options header;
  header.addInteger( "index" );
  header.addText( "session time" );
  header.addLabel( "settings:" );

  // stimulus:
  OutData signal;
  signal.setDelay( PrePause );
  double meanintensity = 0.0;
  NewSignal = true;

  // plot trace:
  plotToggle( true, true, 1.25*Duration, 0.125*Duration );

  timeStamp();

  for ( int count=0;
	( repeats <= 0 || count < repeats ) && softStop() == 0; 
	count++ ) {

    // message:
    if ( repeats == 0 && count%60 == 0 )
      message( "Search ..." );
    else if ( repeats > 0 )
      message( "Search loop <b>" + Str( count ) + "</b> of <b>" + Str( repeats ) + "</b>" );

    // create stimulus:
    if ( NewSignal ) {
      signal.free();
      signal.setTrace( SearchLeft ? LeftSpeaker[0] : RightSpeaker[0]  );
      applyOutTrace( signal );  // needed for maximum sampling rate!
      if ( Mute ) {
	signal.setSampleInterval( 0.0001 );
	signal.resize( 10 );
	signal = 0.0;
      }
      else {
	if ( Waveform == 1 ) {
	  signal.bandNoiseWave( MinFrequency, Frequency, Duration, 0.3, ramp );
	  meanintensity = 6.0206; // stdev=0.5
	  meanintensity = 10.458; // stdev = 0.3
	}
	else {
	  signal.sineWave( Frequency, Duration, 1.0, ramp );
	  meanintensity = 3.0103;
	}
      }
      signal.setIntensity( Intensity > 0 ? Intensity + meanintensity : -100.0 );
      convert( signal );
      NewSignal = false;
    }
    else {
      signal.setIntensity( Intensity > 0 ? Intensity + meanintensity : -100.0 );
      signal.setTrace( SearchLeft ? LeftSpeaker[0] : RightSpeaker[0]  );
    }

    // output stimulus:
    write( signal );
    if ( signal.error() ) {
      // Attenuator overflow or underflow.
      // Set intensity appropriately and write stimulus again.
      if ( signal.underflow() )
	setIntensity( int( ceil( signal.intensity() - meanintensity )) );
      else
	setIntensity( int( floor( signal.intensity() - meanintensity )) );
      postCustomEvent( 2 );
      write( signal );
    }

    sleepOn( Duration + Pause );
    if ( interrupt() ) {
      writeZero( SearchLeft ? LeftSpeaker[0] : RightSpeaker[0] );
      if ( keepchanges )
	setToDefaults();
      return Aborted;
    }
    timeStamp();

    // adjust gain of daq board:
    if ( adjustgain ) {
      for ( int k=0; k<traces().size(); k++ )
	adjust( trace( k ), trace( k ).signalTime(),
		trace( k ).signalTime() + Duration, 0.8 );
      //      activateGains();
    }

    // save:
    if ( repeats > 0 ) {
      if ( count == 0 ) {
	header.setInteger( "index", totalRuns() );
	header.setText( "session time", sessionTimeStr() );
      }
      for ( int trace=1; trace < events().size(); trace++ ) {
	saveEvents( events( trace ), count, header );
      }
    }

  }

  setMessage();
  writeZero( SearchLeft ? LeftSpeaker[0] : RightSpeaker[0] );
  if ( keepchanges )
    setToDefaults();
  return Completed;
}


void AuditorySearch::saveEvents( const EventData &events, int count, const Options &header )
{
  // create file:
  ofstream df;
  df.open( addPath( "search-" + Str( events.ident() ).lower() + "-events.dat" ).c_str(),
	   ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  TableKey spikeskey;
  spikeskey.addNumber( "time", "ms", "%9.2f" );
  if ( count == 0 ) {
    df << '\n' << '\n';
    header.save( df, "# " );
    Options::save( df, "#   ", -1, 0, false, true );
    df << '\n';
    spikeskey.saveKey( df, true, false );
  }

  // write data:
  double t0 = events.signalTime();
  df << '\n';
  df << "# trial: " << count << '\n';
  if ( events.count( t0-PrePause, t0-PrePause+Duration+Pause ) <= 0 ) {
    df << "  -0" << '\n';
  }
  else {
    long jmax = events.previous( t0 + Duration + Pause - PrePause );
    for ( long j=events.next( t0-PrePause ); j<=jmax; j++ ) {
      spikeskey.save( df, 1000.0 * ( events[j] - t0 ), 0 );
      df << '\n';
    }
  }
  
}


void AuditorySearch::keyPressEvent( QKeyEvent *qke )
{
  RePro::keyPressEvent( qke );

  switch ( qke->key()) {
  case Key_Up:
    if ( qke->state() & AltButton ) {
      if ( qke->state() & ShiftButton )
	setFrequency( int(Frequency + LongFrequencyStep) );
      else
	setFrequency( int(Frequency + ShortFrequencyStep) );
      emit frequencyChanged( int(Frequency) );
    }
    else {
      if ( qke->state() & ShiftButton )
	setIntensity( int(Intensity + LongIntensityStep) );
      else
	setIntensity( int(Intensity + ShortIntensityStep) );
      emit intensityChanged( int(Intensity) );
    }
    break;
  case Key_Down:                // arrow down
    if ( qke->state() & AltButton ) {
      if ( qke->state() & ShiftButton )
	setFrequency( int(Frequency - LongFrequencyStep) );
      else
	setFrequency( int(Frequency - ShortFrequencyStep) );
      emit frequencyChanged( int(Frequency) );
    }
    else {
      if ( qke->state() & ShiftButton )
	setIntensity( int(Intensity - LongIntensityStep) );
      else
	setIntensity( int(Intensity - ShortIntensityStep) );
      emit intensityChanged( int(Intensity) );
    }
    break;

  case Key_Left:
    setSpeakerLeft();
    break;
  case Key_Right:
    setSpeakerRight();
    break;

  case Key_Pause:
  case Key_M:
    toggleMute();
    break;

  default: qke->ignore();      // Erase event from list
  }
}


void AuditorySearch::setIntensity( int intensity )
{
  if ( Intensity == intensity ) 
    return;

  Intensity = intensity;
  if ( Intensity < MinIntensity ) 
    Intensity = MinIntensity;
  if ( Intensity > MaxIntensity ) 
    Intensity = MaxIntensity;
  setNumber( "intensity", Intensity );
}


void AuditorySearch::setDuration( int duration )
{
  double dur = 0.001*duration; // change to sec
 
  if ( Duration == dur ) 
    return;

  Duration = dur;
  if ( Duration < MinDuration ) 
    Duration = MinDuration;
  if ( Duration > MaxDuration ) 
    Duration = MaxDuration;
  setNumber( "duration", Duration );

  // plot trace:
  plotToggle( true, true, 1.25*Duration, 0.125*Duration );

  // new stimulus:
  NewSignal = true;
}


void AuditorySearch::setPause( int pause )
{
  double pdur = 0.001*pause; // change to sec

  if ( Pause == pdur ) 
    return;

  Pause = pdur;
  if ( Pause < MinPause ) 
    Pause = MinPause;
  if ( Pause > MaxPause ) 
    Pause = MaxPause;
  setNumber( "pause", Pause );
}


void AuditorySearch::setFrequency( int freq )
{
  double f = freq;

  if ( Frequency == f ) 
    return;

  Frequency = f;
  if ( Frequency < MinFrequency ) 
    Frequency = MinFrequency;
  if ( Frequency > MaxFrequency ) 
    Frequency = MaxFrequency;
  setNumber( "frequency", Frequency );

  // new stimulus:
  NewSignal = true;
}


void AuditorySearch::setWaveform( int wave )
{
  if ( Waveform == wave ) 
    return;

  Waveform = wave;
  if ( Waveform == 1 )
    selectText( "waveform", "noise" );
  else
    selectText( "waveform", "sine" );

  // new stimulus:
  NewSignal = true;
}


void AuditorySearch::setWaveformButton( int wave )
{
  WaveformButtons->setButton( wave );
}


void AuditorySearch::setSpeaker( bool left )
{
  if ( ! left ) {
    setSpeakerRight();
  }
  else {
    setSpeakerLeft();
  }
}


void AuditorySearch::setSpeakerLeft( void )
{
  SearchLeft = true;
  if ( SetBestSide + ( sessionRunning() ? 0 : 1 ) > 1 )
    metaData().selectText( "best side", "left" );
  LeftButton->setChecked( true );
  RightButton->setChecked( false );
  selectText( "side", "left" );
}


void AuditorySearch::setSpeakerRight( void )
{
  SearchLeft = false;
  if ( SetBestSide + ( sessionRunning() ? 0 : 1 ) > 1 )
    metaData().selectText( "best side", "right" );
  LeftButton->setChecked( false );
  RightButton->setChecked( true );
  selectText( "side", "right" );
}


void AuditorySearch::toggleMute( void )
{
  setMute( ! Mute );
}


void AuditorySearch::setMute( bool mute )
{
  if ( mute != Mute ) {
    if ( ! mute ) {
      Mute = false;
      MuteButton->setOn( false );
      NewSignal = true;
    }
    else {
      Mute = true;
      MuteButton->setOn( true );
      NewSignal = true;
    }
  }
}


void AuditorySearch::dialogAccepted( void )
{
  emit intensityChanged( int( number( "intensity" ) ) );
  emit durationChanged( int( number( "duration", "ms" ) ) );
  emit pauseChanged( int( number( "pause", "ms" ) ) );
  emit frequencyChanged( int( number( "frequency", "Hz" ) ) );
  emit waveformChanged( index( "waveform" ) );
  int side = index( "side" );
  if ( side > 1 )
    side = metaData().index( "best side" );
  setSpeaker( ( side == 0 ) );
}


void AuditorySearch::customEvent( QCustomEvent *qce )
{
  emit intensityChanged( int(Intensity) );
  if ( qce->type() - QEvent::User == 1  ) {
    emit durationChanged( int(1000.0*Duration) );
    emit pauseChanged( int(1000.0*Pause) );
    emit frequencyChanged( int(Frequency) );
    emit waveformChanged( Waveform );
    int side = index( "side" );
    if ( side > 1 )
      side = metaData().index( "best side" );
    setSpeaker( ( side == 0 ) );
  }

}


addRePro( AuditorySearch );

#include "moc_auditorysearch.cc"
