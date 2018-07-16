/*
  auditory/search.cc
  Periodically emits a search stimulus.

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

#include <QWidget>
#include <QWidget>
#include <QLabel>
#include <QGroupBox>
#include <QButtonGroup>
#include <relacs/tablekey.h>
#include <relacs/auditory/search.h>
using namespace relacs;

namespace auditory {


const double Search::ShortIntensityStep = 1.0;
const double Search::LongIntensityStep = 5.0;

const double Search::ShortDurationStep = 0.005;
const double Search::LongDurationStep = 0.05;
const double Search::MaxDuration = 10.0;
const double Search::MinDuration = 0.005;

const double Search::ShortPauseStep = 0.005;
const double Search::LongPauseStep = 0.05;
const double Search::MaxPause = 10.0;
const double Search::MinPause = 0.005;

const double Search::ShortFrequencyStep = 500.0;
const double Search::LongFrequencyStep = 5000.0;


Search::Search( void )
  : RePro( "Search", "auditory", "Jan Benda and Christian Machens", "2.8", "July 16, 2018" )
{
  // parameter:
  Intensity = 80.0;
  PrevIntensity = Intensity;
  MaxIntensity = 120.0;
  MinIntensity = 0.0;  
  Duration = 0.05;
  Pause = 0.5;
  PrePause = 0.05;
  Frequency = 5000.0;
  MinFrequency = 0.0;
  MaxFrequency = 1000000.0;
  Waveform = 0;
  SearchLeft = false;
  SetBestSide = 1;
  Ramp = 0.002;

  // options:
  newSection( "Stimulus" );
  addNumber( "intensity", "Sound intensity",  Intensity, 0.0, 200.0, ShortIntensityStep, "dB SPL", "dB SPL", "%.1f" ).setActivation( "mute", "false" );
  addNumber( "minintensity", "Minimum sound intensity",  MinIntensity, 0.0, 200.0, ShortIntensityStep, "dB SPL", "dB SPL", "%.1f" ).setActivation( "mute", "false" );
  addNumber( "maxintensity", "Maximum sound intensity",  MaxIntensity, 0.0, 200.0, ShortIntensityStep, "dB SPL", "dB SPL", "%.1f" ).setActivation( "mute", "false" );
  addBoolean( "mute", "No stimulus", false );

  addNumber( "frequency", "Frequency of stimulus", Frequency, 0.0, 1000000.0, ShortFrequencyStep, "Hz", "kHz" );
  addNumber( "minfreq", "Minimum allowed frequency", MinFrequency, 0.0, 1000000.0, ShortFrequencyStep, "Hz", "kHz" );
  addNumber( "maxfreq", "Maximum allowed frequency", MaxFrequency, 0.0, 1000000.0, ShortFrequencyStep, "Hz", "kHz" );

  addSelection( "waveform", "Waveform of stimulus", "sine|noise" );
  addNumber( "ramp", "Ramp", Ramp, 0.0, 10.0, 0.001, "sec", "ms" );
  
  newSection( "Settings" );
  addSelection( "side", "Speaker", "left|right|best" );
  addInteger( "repeats", "Number of repetitions", 0, 0, 10000, 2 ).setStyle( OptWidget::SpecialInfinite );

  addNumber( "duration", "Duration of stimulus", Duration, MinDuration, MaxDuration, ShortDurationStep, "sec", "ms" );
  addNumber( "pause", "Duration of pause", Pause, MinPause, MaxPause, ShortPauseStep, "sec", "ms" );
  addNumber( "prepause", "Part of pause before stimulus", PrePause, 0.0, MaxPause, ShortPauseStep, "sec", "ms" );

  addBoolean( "adjust", "Adjust input gains", true );
  addBoolean( "saving", "Save raw data", true );
  addSelection( "setbestside", "Set the sessions's best side", "never|no session|always" );
  addBoolean( "keep", "Keep changes", true );

  // variables:
  NewSignal = true;
  Mute = false;

  // layout:
  QGridLayout *grid = new QGridLayout;
  setLayout( grid );

  // Intensity Settings:
  ILCD = new LCDRange( "Intensity [dB SPL]", 3 );
  ILCD->setRange( MinIntensity, MaxIntensity );
  ILCD->setSteps( ShortIntensityStep, LongIntensityStep );
  ILCD->setValue( Intensity );
  grid->addWidget( ILCD, 0, 0 );
  connect( ILCD, SIGNAL( valueChanged( double ) ), 
	   this, SLOT( setIntensity( double ) ) );

  QGridLayout *sgrid = new QGridLayout;
  grid->addLayout( sgrid, 0, 1 );

  // Duration Settings:
  DLCD = new LCDRange( "Stimulus [msec]", 4 );
  DLCD->setRange( 1000.0*MinDuration, 1000.0*MaxDuration );
  DLCD->setSteps( 1000.0*ShortDurationStep, 1000.0*LongDurationStep );
  DLCD->setValue( 1000.0*Duration );
  sgrid->addWidget( DLCD, 0, 0 );
  connect( DLCD, SIGNAL( valueChanged( double ) ), 
	   this, SLOT( setDuration( double ) ) );

  // Pause Settings:
  PLCD = new LCDRange( "Pause [msec]", 4 );
  PLCD->setRange( 1000.0*MinPause, 1000.0*MaxPause );
  PLCD->setSteps( 1000.0*ShortPauseStep, 1000.0*LongPauseStep );
  PLCD->setValue( 1000.0*Pause );
  sgrid->addWidget( PLCD, 0, 1 );
  connect( PLCD, SIGNAL( valueChanged( double ) ), 
	   this, SLOT( setPause( double ) ) );

  // Waveform:
  QGroupBox *gb = new QGroupBox( "Waveform" );
  sgrid->addWidget( gb, 1, 0 );
  SineButton = new QRadioButton( "&Sine" );
  NoiseButton = new QRadioButton( "&Noise" );
  SineButton->setChecked( true );
  QVBoxLayout *gbl = new QVBoxLayout;
  gbl->addWidget( SineButton );
  gbl->addWidget( NoiseButton );
  gb->setLayout( gbl );
  QButtonGroup *WaveformButtons = new QButtonGroup;
  WaveformButtons->addButton( SineButton, 0 );
  WaveformButtons->addButton( NoiseButton, 1 );
  connect( WaveformButtons, SIGNAL( buttonClicked( int ) ), 
	   this, SLOT( setWaveform( int ) ) );

  // Frequency Settings:
  FLCD = new LCDRange( "Frequency [Hz]", 5 );
  FLCD->setRange( MinFrequency, MaxFrequency );
  FLCD->setSteps( ShortFrequencyStep, LongFrequencyStep );
  FLCD->setValue( Frequency );
  sgrid->addWidget( FLCD, 1, 1 );
  connect( FLCD, SIGNAL( valueChanged( double ) ), 
	   this, SLOT( setFrequency( double ) ) );

  // mute button:
  MuteButton = new QPushButton;
  MuteButton->setCheckable( true );
  MuteButton->setText( "Mute" );
  grid->addWidget( MuteButton, 1, 0 );
  connect( MuteButton, SIGNAL( clicked() ), 
	   this, SLOT( toggleMute() ) ); 

  // SearchSide Settings:
  QHBoxLayout *hbox = new QHBoxLayout;
  grid->addLayout( hbox, 1, 1 );
  QLabel *label = new QLabel( "Speaker:" );
  hbox->addWidget( label );
  LeftButton = new QRadioButton( "left" );
  hbox->addWidget( LeftButton );
  RightButton = new QRadioButton( "right" );
  hbox->addWidget( RightButton );
  connect( LeftButton, SIGNAL( clicked() ), 
	   this, SLOT( setSpeakerLeft() ) ); 
  connect( RightButton, SIGNAL( clicked() ), 
	   this, SLOT( setSpeakerRight() ) ); 
}


Search::~Search( void )
{
}


int Search::main( void )
{
  // get options:
  if ( ! boolean( "saving" ) )
    noSaving();
  Intensity = number( "intensity" );
  MinIntensity = number( "minintensity" );
  MaxIntensity = number( "maxintensity" );
  PrevIntensity = Intensity;
  Mute = boolean( "mute" );
  Duration = number( "duration" );
  Pause = number( "pause" );
  PrePause = number( "prepause" );
  Frequency = number( "frequency" );
  MinFrequency = number( "minfreq" );
  MaxFrequency = number( "maxfreq" );
  Waveform = index( "waveform" );
  Ramp = number( "ramp" );
  int side = index( "side" );
  int repeats = integer( "repeats" );
  bool adjustgain = boolean( "adjust" );
  SetBestSide = index( "setbestside" );
  KeepChanges = boolean( "keep" );

  if ( Intensity < MinIntensity )
    Intensity = MinIntensity;
  if ( Intensity > MaxIntensity )
    Intensity = MaxIntensity;
  if ( Frequency < MinFrequency )
    Frequency = MinFrequency;
  if ( Frequency > MaxFrequency )
    Frequency = MaxFrequency;

  if ( side > 1 ) {
    lockMetaData();
    side = metaData().index( "Cell>best side" );
    unlockMetaData();
  }
  SearchLeft = ( side == 0 );

  // update widgets:
  postCustomEvent( 11 );

  // don't print repro message:
  if ( repeats <= 0 )
    noMessage();

  if ( SetBestSide + ( sessionRunning() ? 0 : 1 ) > 1 ) {
    lockMetaData();
    metaData().selectText( "Cell>best side", SearchLeft ? "left" : "right" );
    unlockMetaData();
  }

  // Header:
  Options header;
  header.addInteger( "index", totalRuns() );
  header.addText( "session time", sessionTimeStr() );
  lockStimulusData();
  header.newSection( stimulusData() );
  unlockStimulusData();
  header.newSection( settings() );

  // stimulus:
  OutData signal;
  signal.setDelay( PrePause );
  double meanintensity = 0.0;
  NewSignal = true;
  int status = Completed;

  // plot trace:
  tracePlotSignal( 1.25*Duration, 0.125*Duration );

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
      signal.clear();
      signal.setTrace( SearchLeft ? LeftSpeaker[0] : RightSpeaker[0]  );
      if ( Mute )
	signal.constWave( 0.0 );
      else {
	if ( Waveform == 1 ) {
	  signal.bandNoiseWave( Duration, -1.0, MinFrequency, Frequency, 0.3, 0, Ramp );
	  ::relacs::clip( -1.0, 1.0, signal );
	  meanintensity = 10.458 - 3.0103;
	}
	else {
	  signal.sineWave( Duration, -1.0, Frequency, 0.0, 1.0, Ramp );
	  meanintensity = 0.0;
	}
      }
      signal.setIntensity( Intensity > 0 && ! Mute ? Intensity + meanintensity : OutData::MuteIntensity );
      NewSignal = false;
    }
    else {
      signal.setIntensity( Intensity > 0 ? Intensity + meanintensity : OutData::MuteIntensity );
      signal.setTrace( SearchLeft ? LeftSpeaker[0] : RightSpeaker[0]  );
    }

    // output stimulus:
    write( signal );
    // Attenuator overflow or underflow.
    // Set intensity appropriately and write stimulus again.
    if ( signal.underflow() || signal.overflow() ) {
      if ( signal.underflow() ) {
	Intensity = ceil( signal.intensity() - meanintensity );
	if ( PrevIntensity > Intensity )
	  Intensity = PrevIntensity;
      }
      else {
	Intensity = floor( signal.intensity() - meanintensity );
	if ( PrevIntensity < Intensity )
	  Intensity = PrevIntensity;
      }
      PrevIntensity = Intensity;
      unsetNotify();
      setNumber( "intensity", Intensity );
      if ( KeepChanges )
	setToDefault( "intensity" );
      setNotify();
      signal.setIntensity( Intensity > 0 ? Intensity + meanintensity : OutData::MuteIntensity );
      postCustomEvent( 12 );
      write( signal );
      if ( signal.error() ) {
	status = Failed;
	break;
      }
    }
    else if ( signal.error() ) {
      status = Failed;
      break;
    }

    sleepOn( Duration + Pause );
    timeStamp();
    if ( interrupt() ) {
      status = Aborted;
      break;
    }

    // adjust gain of daq board:
    if ( adjustgain ) {
      for ( int k=0; k<traces().size(); k++ )
	adjust( trace( k ), signalTime(), signalTime() + Duration, 0.8 );
    }

    // save:
    if ( repeats > 0 ) {
      for ( int trace=1; trace < events().size(); trace++ ) {
	saveEvents( events( trace ), count, header );
      }
    }

  }

  setMessage();
  writeZero( SearchLeft ? LeftSpeaker[0] : RightSpeaker[0] );
  return status;
}


void Search::saveEvents( const EventData &events, int count, Options &header )
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
    header.save( df, "# ", 0, Options::FirstOnly );
    df << '\n';
    spikeskey.saveKey( df, true, false );
  }

  // write data:
  df << '\n';
  df << "# trial: " << count << '\n';
  if ( events.count( signalTime()-PrePause, signalTime()-PrePause+Duration+Pause ) <= 0 ) {
    df << "  -0" << '\n';
  }
  else {
    long jmax = events.previous( signalTime() + Duration + Pause - PrePause );
    for ( long j=events.next( signalTime()-PrePause ); j<=jmax; j++ ) {
      spikeskey.save( df, 1000.0 * ( events[j] - signalTime() ), 0 );
      df << '\n';
    }
  }
  
}


void Search::keyPressEvent( QKeyEvent *qke )
{
  qke->ignore();
  switch ( qke->key()) {
  case Qt::Key_Up:
    if ( qke->modifiers() & Qt::ControlModifier ) {
      if ( qke->modifiers() & Qt::ShiftModifier )
	setFrequency( Frequency + LongFrequencyStep );
      else
	setFrequency( Frequency + ShortFrequencyStep );
      FLCD->setValue( Frequency );
    }
    else {
      if ( qke->modifiers() & Qt::ShiftModifier )
	setIntensity( Intensity + LongIntensityStep );
      else
	setIntensity( Intensity + ShortIntensityStep );
      ILCD->setValue( Intensity );
    }
    qke->accept();
    break;

  case Qt::Key_Down:                // arrow down
    if ( qke->modifiers() & Qt::ControlModifier ) {
      if ( qke->modifiers() & Qt::ShiftModifier )
	setFrequency( Frequency - LongFrequencyStep );
      else
	setFrequency( Frequency - ShortFrequencyStep );
      FLCD->setValue( Frequency );
    }
    else {
      if ( qke->modifiers() & Qt::ShiftModifier )
	setIntensity( Intensity - LongIntensityStep );
      else
	setIntensity( Intensity - ShortIntensityStep );
      ILCD->setValue( Intensity );
    }
    qke->accept();
    break;

  case Qt::Key_Left:
    if ( qke->modifiers() & Qt::ControlModifier ) {
      if ( qke->modifiers() & Qt::ShiftModifier )
	setPause( 1000.0*(Pause - LongPauseStep) );
      else
	setPause( 1000.0*(Pause - ShortPauseStep) );
      PLCD->setValue( 1000.0*Pause );
    }
    else
      setSpeakerLeft();
    qke->accept();
    break;

  case Qt::Key_Right:
    if ( qke->modifiers() & Qt::ControlModifier ) {
      if ( qke->modifiers() & Qt::ShiftModifier )
	setPause( 1000.0*(Pause + LongPauseStep) );
      else
	setPause( 1000.0*(Pause + ShortPauseStep) );
      PLCD->setValue( 1000.0*Pause );
    }
    else
      setSpeakerRight();
    qke->accept();
    break;

  case Qt::Key_Pause:
  case Qt::Key_X:
    toggleMute();
    qke->accept();
    break;

  default:
    RePro::keyPressEvent( qke );

  }
}


void Search::setIntensity( double intensity )
{
  if ( fabs(Intensity - intensity) < 1e-8 ) 
    return;

  lock();
  PrevIntensity = Intensity;
  Intensity = intensity;
  if ( Intensity < MinIntensity ) 
    Intensity = MinIntensity;
  if ( Intensity > MaxIntensity ) 
    Intensity = MaxIntensity;
  unlock();
  unsetNotify();
  setNumber( "intensity", Intensity );
  if ( KeepChanges )
    setToDefault( "intensity" );
  setNotify();
}


void Search::setDuration( double duration )
{
  double dur = 0.001*duration; // change to sec
 
  if ( fabs(Duration - dur) < 1e-8 ) 
    return;

  lock();
  Duration = dur;
  if ( Duration < MinDuration ) 
    Duration = MinDuration;
  if ( Duration > MaxDuration ) 
    Duration = MaxDuration;
  NewSignal = true;
  unlock();
  unsetNotify();
  setNumber( "duration", Duration );
  if ( KeepChanges )
    setToDefault( "duration" );
  setNotify();

  // plot trace:
  tracePlotSignal( 1.25*Duration, 0.125*Duration );
}


void Search::setPause( double pause )
{
  double pdur = 0.001*pause; // change to sec

  if ( fabs(Pause - pdur) < 1e-8 ) 
    return;

  lock();
  Pause = pdur;
  if ( Pause < MinPause ) 
    Pause = MinPause;
  if ( Pause > MaxPause ) 
    Pause = MaxPause;
  unlock();
  unsetNotify();
  setNumber( "pause", Pause );
  if ( KeepChanges )
    setToDefault( "pause" );
  setNotify();
}


void Search::setFrequency( double freq )
{
  if ( fabs(Frequency - freq) < 1e-8 ) 
    return;

  lock();
  Frequency = freq;
  if ( Frequency < MinFrequency ) 
    Frequency = MinFrequency;
  if ( Frequency > MaxFrequency ) 
    Frequency = MaxFrequency;
  NewSignal = true;
  unlock();
  unsetNotify();
  setNumber( "frequency", Frequency );
  if ( KeepChanges )
    setToDefault( "frequency" );
  setNotify();
}


void Search::setWaveform( int wave )
{
  if ( Waveform == wave ) 
    return;

  lock();
  Waveform = wave;
  NewSignal = true;
  unlock();
  unsetNotify();
  if ( Waveform == 1 )
    selectText( "waveform", "noise" );
  else
    selectText( "waveform", "sine" );
  if ( KeepChanges )
    setToDefault( "waveform" );
  setNotify();
  // remove focus from waveform selection widget:
  widget()->window()->setFocus();
}


void Search::setWaveformButton( int wave )
{
  if ( wave == 1 )
    NoiseButton->setChecked( true );
  else
    SineButton->setChecked( true );
}


void Search::setSpeaker( bool left )
{
  if ( ! left ) {
    setSpeakerRight();
  }
  else {
    setSpeakerLeft();
  }
}


void Search::setSpeakerLeft( void )
{
  lock();
  SearchLeft = true;
  unlock();
  if ( SetBestSide + ( sessionRunning() ? 0 : 1 ) > 1 ) {
    lockMetaData();
    metaData().selectText( "Cell>best side", "left" );
    unlockMetaData();
  }
  LeftButton->setChecked( true );
  RightButton->setChecked( false );
  unsetNotify();
  selectText( "side", "left" );
  if ( KeepChanges )
    setToDefault( "side" );
  setNotify();
}


void Search::setSpeakerRight( void )
{
  lock();
  SearchLeft = false;
  unlock();
  if ( SetBestSide + ( sessionRunning() ? 0 : 1 ) > 1 ) {
    lockMetaData();
    metaData().selectText( "Cell>best side", "right" );
    unlockMetaData();
  }
  LeftButton->setChecked( false );
  RightButton->setChecked( true );
  unsetNotify();
  selectText( "side", "right" );
  if ( KeepChanges )
    setToDefault( "side" );
  setNotify();
}


void Search::toggleMute( void )
{
  setMute( ! Mute );
}


void Search::setMute( bool mute )
{
  if ( mute != Mute ) {
    if ( ! mute ) {
      lock();
      Mute = false;
      NewSignal = true;
      unlock();
      MuteButton->setDown( false );
    }
    else {
      lock();
      Mute = true;
      NewSignal = true;
      unlock();
      MuteButton->setDown( true );
    }
  }
}


void Search::notify( void )
{
  Intensity = number( "intensity" );
  MinIntensity = number( "minintensity" );
  MaxIntensity = number( "maxintensity" );
  unsetNotify();
  if ( Intensity < MinIntensity )
    Intensity = MinIntensity;
  if ( Intensity > MaxIntensity )
    Intensity = MaxIntensity;
  setNumber( "intensity", Intensity );
  setNotify();
  PrevIntensity = Intensity;
  Mute = boolean( "mute" );
  Duration = number( "duration" );
  Pause = number( "pause" );
  PrePause = number( "prepause" );
  Frequency = number( "frequency" );
  MinFrequency = number( "minfreq" );
  MaxFrequency = number( "maxfreq" );
  unsetNotify();
  if ( Frequency < MinFrequency )
    Frequency = MinFrequency;
  if ( Frequency > MaxFrequency )
    Frequency = MaxFrequency;
  setNumber( "frequency", Frequency );
  setNotify();
  Waveform = index( "waveform" );
  Ramp = number( "ramp" );
  int side = index( "side" );
  SetBestSide = index( "setbestside" );
  if ( side > 1 ) {
    lockMetaData();
    side = metaData().index( "Cell>best side" );
    unlockMetaData();
  }
  SearchLeft = ( side == 0 );

  postCustomEvent( 11 );
}


void Search::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 11: {
    double intensity = Intensity;
    ILCD->setRange( MinIntensity, MaxIntensity );
    ILCD->setValue( intensity );
    DLCD->setValue( 1000.0*Duration );
    PLCD->setValue( 1000.0*Pause );
    double frequency = Frequency;
    FLCD->setRange( MinFrequency, MaxFrequency );
    FLCD->setValue( frequency );
    setWaveformButton( Waveform );
    setSpeaker( SearchLeft );
  }
  case 12: {
    ILCD->setValue( Intensity );
  }
  default:
    RePro::customEvent( qce );
  }
}


addRePro( Search, auditory );

}; /* namespace auditory */

#include "moc_search.cc"
