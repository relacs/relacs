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

  startBtn = new QPushButton("Start");
  startBtn->setEnabled(false);
  startBtn->setToolTip("Start stimulus output...");
  connect( startBtn, SIGNAL( clicked() ), this, SLOT( startTrial() ) );
  grid->addWidget( startBtn, 5, 6, 1, 1 );

  stopBtn = new QPushButton("Stop");
  stopBtn->setEnabled(false);
  stopBtn->setToolTip("Stop stimulus output immediately...");
  connect( stopBtn, SIGNAL( clicked() ), this, SLOT( stopTrial() ) );
  grid->addWidget( stopBtn, 5, 7, 1, 1 );

  nextBtn = new QPushButton("Next");
  nextBtn->setEnabled(true);
  nextBtn->setToolTip("Prepare next trial, randomize stimulus condition...");
  connect( nextBtn, SIGNAL( clicked() ), this, SLOT( prepareNextTrial() ) );
  grid->addWidget( nextBtn, 5, 5, 1, 1 );

  this->setLayout( grid );  
}

void YMaze::populateOptions() {
  newSection( "Experiment" );
  addNumber( "duration", "Trial duration", 10.0, 0.1, 1000., 0.1, "s" );
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
  
  newSection( "EOD estimation" );
  addSelection( "intrace", "inputTrace" );
  addBoolean( "usepsd", "Use the power spectrum", true );
  addNumber( "mineodfreq", "Minimum expected EOD frequency", 100.0, 0.0, 10000.0, 10.0, "Hz" ).setActivation( "usepsd", "true" );
  addNumber( "maxeodfreq", "Maximum expected EOD frequency", 2000.0, 0.0, 10000.0, 10.0, "Hz" ).setActivation( "usepsd", "true" );
  addNumber( "eodfreqprec", "Precision of EOD frequency measurement", 1.0, 0.0, 100.0, 0.1, "Hz" ).setActivation( "usepsd", "true" );
  addNumber( "averagetime", "Time for computing EOD frequency", 2.0, 0.0, 100000.0, 1.0, "s" );  
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
      fisheodf = events( EODEvents ).frequency( currentTime()-averagetime, currentTime() );
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
      int intrace = index( "intrace" );
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
  
  int nextPosition = rand() %3;
  while ( nextPosition == lastRewardPosition ) {
    nextPosition = rand() %3;
  }
  std::vector<MazeArm> arms = {MazeArm::A, MazeArm::B, MazeArm::C};
  if (lastRewardPosition == static_cast<int> ( MazeArm::NONE ) ) {
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
  return mazeCondition;
}

bool YMaze::drawNonRewardedFrequency( double &freq ) {
  std::vector<double> freqs;
  double range = freqRangeMax - freqRangeMin;
  int steps = floor(range/deltaf);
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
  return tc;
}
  
void YMaze::createStimuli( const TrialCondition &tc ) {
  std::cerr << "createStimuli" << std::endl;
  // rewarded stimulus
  OutData od = armSignalMap[tc.mazeCondition.rewarded];
  od.sineWave( duration, od.sampleInterval(), tc.stimCondition.rewardedFreq,
	       0.0, tc.stimCondition.rewardedAmplitude/2., 0.0, "rewarded signal" );

  // unrewarded stimulus
  od = armSignalMap[tc.mazeCondition.unrewarded];
  od.sineWave( duration, od.sampleInterval(), tc.stimCondition.unrewardedFreq,
	       0.0, tc.stimCondition.unrewardedAmplitude/2., 0.0, "unrewarded signal" );

  // neutral stimulus
  od = armSignalMap[tc.mazeCondition.rewarded];
  od.constWave( duration, od.sampleInterval(), 0.0, "neutral signal" );

  postCustomEvent( static_cast<int>(BtnActions::STIM_READY) );
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
    conditionA->setText( uf );
    conditionA->setStyleSheet( nrwrdStyle );
  } else if ( tc.mazeCondition.unrewarded == MazeArm::B ) {
    conditionB->setText( uf );
    conditionB->setStyleSheet( nrwrdStyle );
  } else {
    conditionC->setText( uf );
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

void YMaze::customEvent( QEvent *qce ) {
  TrialCondition tc;
  switch ( qce->type() - QEvent::User ) {
  case static_cast<int>(BtnActions::NEXT_TRIAL):
    tc = nextTrialCondition();
    createStimuli( tc ); 
    updateUI( tc );
    nextBtn->setEnabled(false);
    break;
  case static_cast<int>(BtnActions::STIM_READY):
    startBtn->setEnabled(true);
    break;
  case static_cast<int>(BtnActions::START_TRIAL):
    start = true;
    startBtn->setEnabled(false);
    stopBtn->setEnabled(true);
    break;
  case static_cast<int>(BtnActions::STOP_TRIAL):
    start = false;
    stopBtn->setEnabled(false);
    nextBtn->setEnabled(true);
    break;
  default:
    break;
  } 
}


bool YMaze::configureOutputTraces() {
  bool success = true;
  for (int i = 0; i < outTracesSize(); ++i) {
    std::string name = outTraceName( i );
    std::map<std::string, MazeArm>::iterator it = channelArmMap.find( name );
    if (it != channelArmMap.end()) {
      armSignalMap[it->second].setTrace( i );
      outList.push( armSignalMap[it->second] );
    } else {
      success = false;
      break;
    }
  }
  return success;
}

  
//************************************************************************
//************************************************************************
int YMaze::main( void ) {
  // get options:
  duration = number( "duration" );
  rewardedFreq = number( "rewardfreq" );
  freqRangeMin = number( "rangemin" );
  freqRangeMax = number( "rangemax" );
  minFreqDiff = number( "minfreqdiff" );
  deltaf = number( "deltaf" );
  start = false;
  bool success = configureOutputTraces();
  if ( !success ) {
    warning( "configuration of output channels failed!" );
    warning( "expecting 3 output traces names Arm-A, Arm-B, and Arm-C" );
    return Failed;
  }

  while ( softStop() == 0 ) {
      if ( interrupt() || softStop() > 0 )
        break;
      if ( !start ){
        sleep(0.2);
        continue;
      } else {
	//	std::cerr << "start stimulus" << std::endl;
      }
  }
  return Completed;
}


addRePro( YMaze, efield );

}; /* namespace efield */

#include "moc_ymaze.cc"
