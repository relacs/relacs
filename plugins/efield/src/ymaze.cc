/*
  efield/ymaze.cc
  Repro fro controlling discrimination experiments in a y-maze

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

#include <relacs/efield/ymaze.h>
#include <relacs/base/linearattenuate.h>
#include <relacs/digitalio.h>

#include <QPainter>
#include <QPicture>
#include <QPushButton>
#include <iostream>
#include <cstdint>

using namespace relacs;

namespace efield {

YMazeSketch::YMazeSketch(MazeOrientation orientation, QWidget *parent) :
  QLabel(parent), orientation(orientation) {
    setupUI();
}

void YMazeSketch::setupUI() {
  QGridLayout *grid = new QGridLayout();
  l = new QLabel();

  lbl1 = new QLabel( "A" );
  lbl1->setFixedSize( 24, 24);
  lbl1->setAlignment( Qt::AlignLeft );

  lbl2 = new QLabel( "B" );
  lbl2->setFixedSize( 24, 24 );
  lbl2->setAlignment( Qt::AlignLeft );

  lbl3 = new QLabel( "C" );
  lbl3->setFixedSize( 24, 24 );
  lbl3->setAlignment( Qt::AlignLeft );
  drawSketch();

  labels[0] = lbl1;
  labels[1] = lbl2;
  labels[2] = lbl3;

  grid->addWidget( lbl1, 0, 0, 1, 1, Qt::AlignCenter );
  grid->addWidget( lbl2, 0, 4, 1, 1, Qt::AlignCenter );
  grid->addWidget( lbl3, 4, 2, 1, 1, Qt::AlignCenter );
  grid->addWidget( l, 1, 0, 3, 5, Qt::AlignCenter );

  this->setLayout(grid);
}

void YMazeSketch::drawSketch() {
  int width = this->width();
  int height = l->height();
  int centerx = width / 2;
  int centery = height / 2;
  int xstart = width / 10;
  int xend = width - xstart;

  l->clear();
  QPicture pi;
  QPainter p(&pi);
  p.setRenderHint(QPainter::Antialiasing);
  p.setPen(QPen(Qt::black, 12, Qt::SolidLine, Qt::RoundCap));
  p.drawLine(xstart, 0, centerx, centery);
  p.drawLine(xend, 0, centerx, centery);
  p.drawLine(centerx, height, centerx, centery);
  p.end();
  l->setPicture(pi);
}

void YMazeSketch::resizeEvent( QResizeEvent *event ) {
  drawSketch();
  QLabel::resizeEvent(event);
}

void YMazeSketch::setCondition( const MazeCondition &mc ) {
  QString styleStub = "QLabel{font-size: 20; font-weight: bold; color : ";
  QString styleSuffix = "}";
  labels[static_cast<int>(mc.rewarded)]->setStyleSheet(styleStub +
						       colors[static_cast<int>(ArmCondition::REWARDED)] +
						       styleSuffix);
  labels[static_cast<int>(mc.unrewarded)]->setStyleSheet(styleStub +
							 colors[static_cast<int>(ArmCondition::UNREWARDED)] +
							 styleSuffix);
  labels[static_cast<int>(mc.neutral)]->setStyleSheet(styleStub +
						      colors[static_cast<int>(ArmCondition::NEUTRAL)] +
						      styleSuffix);
}

void YMazeSketch::reset() {
  lastRewarded = MazeArm::NONE; 
  QString style = "QLabel{font-size: 20; font-weight: bold; color : " + colors[static_cast<int>(ArmCondition::NEUTRAL)] + "}";
  
  for ( auto l : labels ) {
    l->setStyleSheet( style );
  }
}

//***********************************************************************
//***********************************************************************  
YMaze::YMaze( void )
  : RePro( "YMaze", "efield", "Jan Grewe", "1.0", "Mar 18, 2020" )
{
  populateOptions();

  // Ui Layout
  QGridLayout *grid = new QGridLayout();
  sketch = new YMazeSketch();
  grid->addWidget( sketch, 0, 0, 4, 4 );
  setupTable(grid);

  startBtn = new QPushButton("&Start");
  startBtn->setEnabled(false);
  startBtn->setToolTip("Start stimulus output...");
  connect( startBtn, SIGNAL( clicked() ), this, SLOT( startTrial() ) );
  grid->addWidget( startBtn, 5, 5, 1, 1 );

  stopBtn = new QPushButton("&Stop");
  stopBtn->setEnabled(false);
  stopBtn->setToolTip("Stop stimulus output immediately...");
  connect( stopBtn, SIGNAL( clicked() ), this, SLOT( stopTrial() ) );
  grid->addWidget( stopBtn, 5, 6, 1, 1 );

  nextBtn = new QPushButton("&Next");
  nextBtn->setEnabled(true);
  nextBtn->setToolTip("Prepare next trial, randomize stimulus condition...");
  connect( nextBtn, SIGNAL( clicked() ), this, SLOT( prepareNextTrial() ) );
  grid->addWidget( nextBtn, 5, 4, 1, 1 );

  resetBtn = new QPushButton("&Reset");
  resetBtn->setEnabled( true );
  resetBtn->setToolTip( "Reset the experiment back to defaults" );
  connect( resetBtn, SIGNAL( clicked() ), this, SLOT( reset() ) );
  grid->addWidget( resetBtn, 5, 7, 1, 1 );

  this->setLayout( grid );  
}

void YMaze::populateOptions() {
  newSection( "Experiment" );
  addText( "name" , "Prefix used to identify the repro run, auto-generated if empty", "" );
  addBoolean( "dumbasses", "Simpler task in which the unrewarded stim is switched off", false );
  addNumber( "duration", "Trial duration", 10.0, 0.1, 1000., 0.1, "s" );
  addNumber( "samplerate", "stimulus sampling rate", 20000, 1000., 100000., 100., "Hz" );
  addNumber( "rewardfreq", "Rewarded frequency", 500.0, 1.0, 2500.0, 1.0, "Hz" );
  addNumber( "rangemin", "Frequency range minimum", 100.0, 1.0, 2500.0, 1.0, "Hz" );
  addNumber( "rangemax", "Frequency range maximum", 1000.0, 1.0, 2500.0, 1.0, "Hz" );
  addNumber( "deltaf", "Stepsize of unrewarded stimulus frequency discretization",
	     10.0, 1.0, 500.0, 1.0, "Hz" );
  addNumber( "minfreqdiff", "Minimum frequency difference between rewarded and unrewarded stimulus",
	     10.0, 1.0, 500.0, 1.0, "Hz" );
  addNumber( "rewardsignalampl", "Amplitude of the rewarded signal.", 1.0, 0.1, 10.0, 0.1, "mV" );
  addNumber( "nonrewardsignalampl", "Amplitude of the non-rewarded signal.", 1.0, 0.1, 10.0, 0.1, "mV" );
  addBoolean( "nofish", "Test mode without fish", false );
  addNumber( "fakefisheodf", "EOD frequency of fake fish", 500.0, 1.0, 10000.0, 1.0, "Hz").setActivation( "noFish", "true" );
  addBoolean( "fixedstart", "Sets the selected arm as fixed start position for all trials.", false);
  addSelection( "startbox", "The starting tube in which the fish is upon start",  "Arm-A|Arm-B|Arm-C" );
  
  newSection( "EOD estimation" );
  addSelection( "intrace", "inputTrace" );
  addBoolean( "usepsd", "Use the power spectrum", true );
  addNumber( "mineodfreq", "Minimum expected EOD frequency", 100.0, 0.0, 10000.0, 10.0, "Hz" ).setActivation( "usepsd", "true" );
  addNumber( "maxeodfreq", "Maximum expected EOD frequency", 2000.0, 0.0, 10000.0, 10.0, "Hz" ).setActivation( "usepsd", "true" );
  addNumber( "eodfreqprec", "Precision of EOD frequency measurement", 1.0, 0.0, 100.0, 0.1, "Hz" ).setActivation( "usepsd", "true" );
  addNumber( "averagetime", "Time for computing EOD frequency", 2.0, 0.0, 100000.0, 1.0, "s" );  

  newSection( "LED indicators" );
  addBoolean( "ledindicators", "Use LEDs attached to DIO lines to indicate rewarded stimulus.", true );
  addText( "diodevice", "Name of the digital I/O device", "dio-1" ).setActivation( "ledindicators", "true" );
  addInteger( "ledapin", "DIO line for LED on arm A.", 0 ).setActivation( "ledindicators", "true" );
  addInteger( "ledbpin", "DIO line for LED on arm B.", 1 ).setActivation( "ledindicators", "true" );
  addInteger( "ledcpin", "DIO line for LED on arm C.", 2 ).setActivation( "ledindicators", "true" );
}
  
void YMaze::setupTable(QGridLayout *grid) {
  QString activeStyle = "QLabel{color: red; font-size: 8pt}";
  QString passiveStyle = "QLabel{color: black; font-size: 8pt}";
  conditionA = new QLabel( "" );
  conditionB = new QLabel( "" );
  conditionC = new QLabel( "" );
  conditionA->setStyleSheet( activeStyle );
  conditionB->setStyleSheet( activeStyle );
  conditionC->setStyleSheet( activeStyle );
  
  conditionApast = new QLabel( "" );
  conditionBpast = new QLabel( "" );
  conditionCpast = new QLabel( "" );
  conditionApast->setStyleSheet( passiveStyle );
  conditionBpast->setStyleSheet( passiveStyle );
  conditionCpast->setStyleSheet( passiveStyle );

  grid->addWidget( new QLabel("current:"), 0, 6, 1, 1 );
  grid->addWidget( new QLabel("past:"), 0, 7, 1, 1 );
  
  grid->addWidget( new QLabel("A:"), 1, 5, 1, 1 );
  grid->addWidget( conditionA, 1, 6, 1, 1 );
  grid->addWidget( conditionApast, 1, 7, 1, 1);
  
  grid->addWidget( new QLabel("B:"), 2, 5, 1, 1 );
  grid->addWidget( conditionB, 2, 6, 1, 1 );
  grid->addWidget( conditionBpast, 2, 7, 1, 1);

  grid->addWidget( new QLabel("C:"), 3, 5, 1, 1 );
  grid->addWidget( conditionC, 3, 6, 1, 1 );
  grid->addWidget( conditionCpast, 3, 7, 1, 1);
}

bool YMaze::estimateEodFrequency( double &fisheodf ) {
  double averagetime = number( "averagetime" );
  fisheodf = number( "fakefisheodf" );
  if ( !boolean( "nofish" ) ) {
    if ( !boolean( "usepsd" ) ) {
      fisheodf = events( EODEvents ).frequency( currentTime() - averagetime, currentTime() );

      if ( EODEvents < 0 ) {
	      warning( "need EOD events of the EOD Trace." );
	      fisheodf = number( "fakefisheodf" );
	      return false;
      }
      return true;
    } else {

      double bigeod = 0.0;
      double bigeodf = 0.0; 
      double min_eodf = number( "mineodfreq" );
      double max_eodf = number( "maxeodfreq" );
      double eodf_prec = number( "eodfreqprec" );
      int intrace = index( "inputTrace" );
      if ( intrace == -1 )
	      return false;
      int nfft = 1;

      nfft = nextPowerOfTwo( (int)::ceil( 1.0/trace( intrace ).stepsize()/eodf_prec ) );
      eodf_prec = 1.0/trace( intrace ).stepsize()/nfft;
      if ( averagetime < 2.0/trace( intrace ).stepsize()/nfft ) {
	      averagetime = 2.0/trace( intrace ).stepsize()/nfft;
	      warning( "averagetime is too small for requested frequency resolution. I set it to " +
		             Str( averagetime ) + "s for now." );
      }
	
      SampleDataF data( 0.0, averagetime, trace( intrace ).stepsize() );
      trace( intrace ).copy( currentTime() - averagetime, data );
      SampleDataF power( nfft );
      rPSD( data, power );
      double threshold = power.max( min_eodf, max_eodf );
      EventData powerpeaks( 1000, true );
      peaks( power, powerpeaks, 0.2*threshold );
      double maxpower = 0.0;
      double maxfreq = 0.0;
      for ( int i=0; i<powerpeaks.size(); i++ ) {
	      if ( powerpeaks[i] >= min_eodf && powerpeaks[i] <= max_eodf ) {
	        if ( powerpeaks.eventSize( i ) > maxpower ) {
	          maxpower = powerpeaks.eventSize( i );
	          maxfreq = powerpeaks[i];
	        }
	      }
      }
      if ( bigeod < maxpower ) {
	      bigeod = maxpower;
	      bigeodf = maxfreq;
      }
      fisheodf = bigeodf;
      return true;
    }
  }
  return true;
}
   
MazeCondition YMaze::nextMazeCondition() {
  MazeCondition mazeCondition;
  std::vector<MazeArm> arms = {MazeArm::A, MazeArm::B, MazeArm::C};
  if ( fixedStart ) {
      mazeCondition.neutral = arms[ lastRewardPosition ];
      arms.erase( std::find( arms.begin(), arms.end(), static_cast<MazeArm>( lastRewardPosition ) ) );

      int nextPosition = rand() %2;
      mazeCondition.rewarded = arms[ nextPosition ];
      mazeCondition.unrewarded = arms[ 1 - nextPosition ];
  } else {
    int nextPosition = rand() %3;
    while ( nextPosition == lastRewardPosition ) {
      nextPosition = rand() %3;
    }
    if ( lastRewardPosition == static_cast<int> ( MazeArm::NONE ) ) {
      mazeCondition.rewarded = arms[nextPosition];
      arms.erase( arms.begin() + nextPosition );
      mazeCondition.unrewarded = arms[0];
      mazeCondition.neutral = arms[1];
      lastRewardPosition = nextPosition;
    } else {
      mazeCondition.rewarded = arms[nextPosition];
      mazeCondition.neutral = arms[lastRewardPosition];
      arms.erase( arms.begin() + nextPosition );
      arms.erase( std::find( arms.begin(), arms.end(), static_cast<MazeArm>( lastRewardPosition ) ) );
      mazeCondition.unrewarded = arms[0];
      lastRewardPosition = nextPosition;
    }
  }
  return mazeCondition;
}

bool YMaze::drawNonRewardedFrequency( double &freq ) {
  std::vector<double> freqs;
  double range = freqRangeMax - freqRangeMin;
  int steps = floor(range/deltaf);
  if ( steps == 0 )
    error( "Invalid frequency range, check options!" );
  int step = rand() % steps;
  freq = freqRangeMin + step * deltaf;
  int count = 0;
  while (fabs(rewardedFreq - freq) < minFreqDiff) {
    step = rand() % steps;
    freq = freqRangeMin + step * deltaf;
    count++;
    if (count > 1000) {
      error("YMaze: Could not shuffle a valid Non-rewarded stimulus frequency! Please check RePro settings!");
      return false;
    }
  }
  return true;
}

StimulusCondition YMaze::nextStimulusConditions() {
  bool success = estimateEodFrequency( eodf );
  if ( !success ) {
    error("YMaze: Could not get a valid fish frequency!");
  }
  double noRewardFreq;
  bool valid = drawNonRewardedFrequency( noRewardFreq );
  StimulusCondition sc;
  sc.rewardedFreq = rewardedFreq;
  sc.unrewardedFreq = noRewardFreq;
  sc.rewardedAmplitude = number("rewardsignalampl");
  sc.unrewardedAmplitude = number("nonrewardsignalampl");
  sc.valid = valid;
  return sc;
}
  
TrialCondition YMaze::nextTrialCondition() {
  TrialCondition tc;
  tc.mazeCondition = nextMazeCondition();
  tc.stimCondition = nextStimulusConditions();
  currentCondition = tc;
  return tc;
}
  
void YMaze::createStimuli( const TrialCondition &tc ) {
  string ident = name.size() == 0 ? "Ymaze" : name;
  /*for ( int i = 0; i < outList.size(); ++i ) {
     outList[i].clear();
     }*/
  outList.clear();
  double sampleInterval = 1./samplerate;

  OutData zero( 1, sampleInterval );
  zero[0] = 0.0;
  // rewarded stimulus
  OutData rwStim;
  rwStim.setTrace( armTraceMap[tc.mazeCondition.rewarded] );
  rwStim.setSampleInterval( sampleInterval );
  rwStim.sineWave( duration, rwStim.sampleInterval(), tc.stimCondition.rewardedFreq,
		   0.0, 1.0, 0.0 );
  rwStim.append( zero );
  rwStim.setIntensity( tc.stimCondition.rewardedAmplitude/2. );
  rwStim.description().addText( "RewardedType", "rewarded" ).addFlags( OutData::Mutable );
  rwStim.description().addText( "Arm", toString(tc.mazeCondition.rewarded) ).addFlags( OutData::Mutable );
  rwStim.description()["Frequency"].addFlags( OutData::Mutable );
  rwStim.setIdent( ident + "_rewarded" );
  outList.push( rwStim );
  
  // unrewarded stimulus
  OutData nrwStim;
  if ( dumbasses ) {
    nrwStim.setTrace( armTraceMap[tc.mazeCondition.unrewarded] );
    nrwStim.setSampleInterval( sampleInterval );
    nrwStim.constWave( duration, nrwStim.sampleInterval(), 0.0 );
    nrwStim.append( zero ); 
    nrwStim.setIntensity( 0.000001 );
    nrwStim.description().addText( "RewardedType", "unrewarded" ).addFlags( OutData::Mutable );
    nrwStim.description().addText( "Arm", toString(tc.mazeCondition.unrewarded) ).addFlags( OutData::Mutable );
    nrwStim.description().addNumber( "Frequency", 0.0, "Hz" ).addFlags( OutData::Mutable );
  } else {
    nrwStim.setTrace( armTraceMap[tc.mazeCondition.unrewarded] );
    nrwStim.setSampleInterval( sampleInterval );
    nrwStim.sineWave( duration, nrwStim.sampleInterval(), tc.stimCondition.unrewardedFreq,
		      0.0, 1.0 , 0.0 );
    nrwStim.append( zero );
    nrwStim.setIntensity( tc.stimCondition.unrewardedAmplitude/2. );
    nrwStim.description().addText( "RewardedType", "unrewarded" ).addFlags( OutData::Mutable );
    nrwStim.description().addText( "Arm", toString(tc.mazeCondition.unrewarded) ).addFlags( OutData::Mutable );
    nrwStim.description()["Frequency"].addFlags( OutData::Mutable );
  }
  nrwStim.setIdent( ident + "_unrewarded" );
  outList.push( nrwStim );

  // neutral stimulus
  OutData ntrlStim;
  ntrlStim.setTrace( armTraceMap[tc.mazeCondition.neutral] );
  ntrlStim.setSampleInterval( sampleInterval );
  ntrlStim.constWave( duration, ntrlStim.sampleInterval(), 0.0 );
  ntrlStim.append( zero ); 
  ntrlStim.setIntensity( 0.000001 );
  ntrlStim.description().addText( "RewardedType", "neutral" ).addFlags( OutData::Mutable );
  ntrlStim.description().addText( "Arm", toString(tc.mazeCondition.neutral) ).addFlags( OutData::Mutable );
  ntrlStim.description().addNumber( "Frequency", 0.0, "Hz" ).addFlags( OutData::Mutable );
  ntrlStim.setIdent( ident + "_neutral" );
  outList.push( ntrlStim );
										     
  postCustomEvent( static_cast<int>(YMazeEvents::STIM_READY) );
}
  
void YMaze::startTrial() {
  postCustomEvent( static_cast<int>(BtnActions::START_TRIAL) );
}

void YMaze::prepareNextTrial() {
  postCustomEvent( static_cast<int>(BtnActions::NEXT_TRIAL) );
}

void YMaze::stopTrial() {
  postCustomEvent ( static_cast<int>(BtnActions::STOP_TRIAL) );
}

void YMaze::reset() {
  postCustomEvent ( static_cast<int>(BtnActions::STOP_TRIAL) );
  postCustomEvent ( static_cast<int>(BtnActions::RESET) );
}

void YMaze::updateUI( const TrialCondition &tc ) {
  sketch->setCondition( tc.mazeCondition );
  updateTable( tc );
}

void YMaze::updateTable( const TrialCondition &tc ) {
  QString rwrdStyle = "QLabel{color: green; font-size: 8pt; font-weight: bold}";
  QString ntrlStyle = "QLabel{color: black; font-size: 8pt; font-weight: bold}";
  QString nrwrdStyle = "QLabel{color: red; font-size: 8pt; font-weight: bold}";
  
  conditionApast->setText(conditionA->text());
  conditionBpast->setText(conditionB->text());
  conditionCpast->setText(conditionC->text());
  QString rf = QString::number(tc.stimCondition.rewardedFreq) + "Hz;\n " +
    QString::number(tc.stimCondition.rewardedAmplitude) + "mV";
  QString uf = QString::number(tc.stimCondition.unrewardedFreq) + "Hz;\n " +
    QString::number(tc.stimCondition.unrewardedAmplitude) + "mV";
  QString duf = "no stim\n blocked tube";
  QString neutral = "---";
  
  if ( tc.mazeCondition.rewarded == MazeArm::A ) {
    conditionA->setText( rf );
    conditionA->setStyleSheet( rwrdStyle );
  } else if ( tc.mazeCondition.rewarded == MazeArm::B ) {
    conditionB->setText( rf );
    conditionB->setStyleSheet( rwrdStyle );
  } else {
    conditionC->setText( rf );
    conditionC->setStyleSheet( rwrdStyle );
  }

  if ( tc.mazeCondition.unrewarded == MazeArm::A ) {
    conditionA->setText( dumbasses ? duf : uf  );
    conditionA->setStyleSheet( nrwrdStyle );
  } else if ( tc.mazeCondition.unrewarded == MazeArm::B ) {
    conditionB->setText( dumbasses ? duf : uf );
    conditionB->setStyleSheet( nrwrdStyle );
  } else {
    conditionC->setText( dumbasses ? duf : uf );
    conditionC->setStyleSheet( nrwrdStyle );
  }
  
  if ( tc.mazeCondition.neutral == MazeArm::A ) {
    conditionA->setText( neutral );
    conditionA->setStyleSheet( ntrlStyle );
  } else if ( tc.mazeCondition.neutral == MazeArm::B ) {
    conditionB->setText( neutral );
    conditionB->setStyleSheet( ntrlStyle );
  } else {
    conditionC->setText( neutral );
    conditionC->setStyleSheet( ntrlStyle );
  }
}

void YMaze::resetTable( void ) {
  vector<QLabel*> labels = {conditionA, conditionApast, conditionB, conditionBpast, conditionC, conditionCpast};
  QString ntrlStyle = "QLabel{color: black; font-size: 8pt; font-weight: bold}";
  
  for ( auto l : labels ) {
    l->setText( "" );
    l->setStyleSheet( ntrlStyle );
  }
}

void YMaze::customEvent( QEvent *qce ) {
  TrialCondition tc;
  switch ( qce->type() - QEvent::User ) {
  case static_cast<int>(BtnActions::NEXT_TRIAL):
    tc = nextTrialCondition();
    currentCondition = tc;
    createStimuli( currentCondition ); 
    updateUI( currentCondition );
    nextBtn->setEnabled( false );
    resetBtn->setEnabled( true );
    break;
  case static_cast<int>(BtnActions::START_TRIAL):
    startBtn->setEnabled( false );
    stopBtn->setEnabled( true );
    if ( useLEDs && dio != 0 && dio->isOpen() ) {
      int rewarded_line = armLEDMap[currentCondition.mazeCondition.rewarded];
      int value, r;
      for ( auto l : ledLines ) {
        value = l == rewarded_line ? 1 : 0;
        r = dio->configureLine( l, true );
	r = dio->write( l, value );
        if ( r != 0 ) {
          warning( "Failed to set level on DIO line <b>" + Str( l ) + "</b>!" );
        }
      } 
    }
    start = true;
    break;
  case static_cast<int>(BtnActions::STOP_TRIAL):
    start = false;
    stopOutputs();
    stopBtn->setEnabled( false );
    nextBtn->setEnabled( true );
    break;
  case static_cast<int>(YMazeEvents::STIM_READY):
    startBtn->setEnabled( true );
    break;
  case static_cast<int>(YMazeEvents::IDLE):
    stopBtn->setEnabled( false );
    nextBtn->setEnabled( true );
    break;
  case static_cast<int>(BtnActions::RESET):
    nextBtn->setEnabled( true );
    stopBtn->setEnabled( false );
    startBtn->setEnabled( false );
    resetBtn->setEnabled( false );
    sketch->reset();
    resetTable();
    TrialCondition tc;
    currentCondition = tc;
    stopOutputs();
    start = false;
    break;
  default:
    break;
  } 
}


void YMaze::stopOutputs( void ) {
  stopWrite();
  stop = true;
  /*
  OutList zero;
  for ( int i = 0; i < outList.size(); ++i ) {
     OutData z;
     z.setTrace( outList[i].trace() );
     z.setIntensity( 1.0 );
     //z.setSampleInterval( .01 ); 
     z.pulseWave( 10, -1, 0.0, 0.0 );
     zero.push( z );
  }
  write( zero );
  */
  /* 
  for ( int i = 0; i < outList.size(); ++i ) {
    writeZero( outList[i].trace() );
  }
  
  if ( useLEDs && dio != 0 && dio->isOpen() ) {
    for ( auto l : ledLines ) {
      int r = dio->write( l, 0 );
      if ( r != 0 ) {
	warning( "Failed to set level on DIO line <b>" + Str( l ) + "</b>!" );
      }
    }
  }
  */
}


bool YMaze::configureOutputTraces() {
  bool success = true;
  for (int i = 0; i < outTracesSize(); ++i) {
    std::string name = outTraceName( i );
    std::map<std::string, MazeArm>::iterator it = channelArmMap.find( name );
    if (it != channelArmMap.end()) {
      armTraceMap[it->second] = i;
    } else {
      success = false;
      break;
    }
  }
  if ( useLEDs ) {
    ledLines = {led_a_pin, led_b_pin, led_c_pin};
    dio = digitalIO( device );
    if ( dio == 0 && dio->isOpen() ) {
      cerr << ( "Digital I/O device <b>" + device + "</b> not found!" ) << endl;
      success = false;
    } else { 
      armLEDMap[MazeArm::A] = led_a_pin;
      armLEDMap[MazeArm::B] = led_b_pin;
      armLEDMap[MazeArm::C] = led_c_pin;
      int r = 0;
      for ( int l : ledLines ) {
        r = dio->configureLine( l, true );
        if (r != 0 ) {
          cerr << ( "Failed to configure on DIO line <b>" + Str( l ) + "</b>!" ) << endl;
          success = false;
        }
      }
    }
  }
  base::LinearAttenuate *latt_a = dynamic_cast<base::LinearAttenuate*>( attenuator( "Arm-A" ) );
  base::LinearAttenuate *latt_b = dynamic_cast<base::LinearAttenuate*>( attenuator( "Arm-B" ) );
  base::LinearAttenuate *latt_c = dynamic_cast<base::LinearAttenuate*>( attenuator( "Arm-C" ) );
  if ( latt_a == 0 || latt_b == 0 || latt_c == 0 ) { 
     success = false;
     warning( "Attenuators are not open!!" );
  }
   
  if ( latt_a != 0 && fabs( latt_a->gain() - 1.0 ) < 1.0e-8 )
     warning( "Attenuator gain is probably not set!<br>(it is set to 1.0)", 2.0 );

  return success;
}

std::string YMaze::toString(const MazeArm &arm) const {
  if (arm == MazeArm::A) {
    return "Arm A";
  } else if (arm == MazeArm::B) {
    return "Arm B";
  } else if (arm == MazeArm::C) {
    return "Arm C";
  }
  return "";
}

void YMaze::keyPressEvent( QKeyEvent *e ) {
  e->ignore();
  if ( e->key() == Qt::Key_S && ( e->modifiers() & Qt::AltModifier ) ) {
    if ( start )
      stopBtn->animateClick();
    else if ( startBtn->isEnabled() )
      startBtn->animateClick();
    e->accept();
  } else if ( e->key() == Qt::Key_N && ( e->modifiers() & Qt::AltModifier ) ) {
    if ( nextBtn->isEnabled() )
      nextBtn->animateClick();
    e->accept();
  } else {
    RePro::keyPressEvent( e );
  }
}

//************************************************************************
//************************************************************************
int YMaze::main( void ) {
  string msg;
  duration = number( "duration" );
  samplerate = number( "samplerate" );
  rewardedFreq = number( "rewardfreq" );
  freqRangeMin = number( "rangemin" );
  freqRangeMax = number( "rangemax" );
  minFreqDiff = number( "minfreqdiff" );
  deltaf = number( "deltaf" );
  name = text( "name" );
  useLEDs = boolean( "ledindicators" );
  led_a_pin = integer( "ledapin" );
  led_b_pin = integer( "ledbpin" );
  led_c_pin = integer( "ledcpin" );
  device = text( "diodevice" );
  start_arm = text( "startbox" );
  lastRewardPosition = static_cast<int>( channelArmMap[start_arm] );
  dumbasses = boolean( "dumbasses" );
  fixedStart = boolean( "fixedstart" );
  
  start = false;
  reset();
  bool success = configureOutputTraces();
  if ( !success ) {
    warning( "configuration of output channels failed!" );
    warning( "expecting 3 output traces names Arm-A, Arm-B, and Arm-C" );
    return Failed;
  }
  stopOutputs();
  keepFocus();

  while ( softStop() == 0 ) {
      if ( interrupt() || softStop() > 0 )
        break;
      if ( !start ){
        sleep(0.2);
        continue;
      }

      msg = "Stimulation is running.<br>Rewarded arm is " + toString(currentCondition.mazeCondition.rewarded) ;
      message( msg );
      stop = false;
      write( outList );
      if (stop) {
	outList.clearError();
	for ( int i = 0; i < outList.size(); ++i ) {
	  writeZero( outList[i].trace() );
	}
	
	if ( useLEDs && dio != 0 && dio->isOpen() ) {
	  for ( auto l : ledLines ) {
	    int r = dio->write( l, 0 );
	    if ( r != 0 ) {
	      warning( "Failed to set level on DIO line <b>" + Str( l ) + "</b>!" );
	    }
	  }
	}
	stop = false;
      }
      message( "" );
      if ( outList.failed() ) {
	msg = "Output of stimulus failed!<br>Error code is <b>";
	msg += outList.errorText() + "</b>";
	message( msg );
	for ( int i = 0; i < outList.size(); ++i )
	  writeZero( outList[i].trace() );
        return Failed;
      }
      if ( useLEDs && dio != 0 && dio->isOpen() ) {
        for (int l : ledLines)
          dio->write( l, 0 );
      }
      start = false;
      postCustomEvent( static_cast<int>(YMazeEvents::IDLE) );
  }
  stopOutputs();
  return Completed;
}

addRePro( YMaze, efield );

}; /* namespace efield */

#include "moc_ymaze.cc"
