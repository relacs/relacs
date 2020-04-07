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
  addNumber( "duration", "trial duration", 10.0, 0.1, 1000., 0.1, "s" );
  addNumber( "rewardfreq", "rewarded frequency", 500.0, 1.0, 2500.0, 1.0, "Hz" );
  addNumber( "rangemin", "Frequency range minimum", 1.0, 1.0, 2500.0, 1.0, "Hz" );
  addNumber( "rangemax", "Frequency range maximum", 1000.0, 1.0, 2500.0, 1.0, "Hz" );
  addNumber( "deltaf", "Stepsize of unrewarded stimulus frequency discretization",
	     10.0, 1.0, 500.0, 1.0, "Hz" );
  addNumber( "minfreqdiff", "Minimum frequency difference between rewarded and unrewarded stimulus",
	     10.0, 1.0, 500.0, 1.0, "Hz" );
  
  // Ui Layout
  QGridLayout *grid = new QGridLayout();
  sketch = new YMazeSketch();
  grid->addWidget( sketch, 0, 0, 4, 4 );
  setupTable(grid);
  QPushButton *startBtn = new QPushButton("Start");
  connect( startBtn, SIGNAL( clicked() ), this, SLOT( startNextTrial() ) );
  grid->addWidget( startBtn, 4, 5, 1, 1 );

  QPushButton *stopBtn = new QPushButton("Stop");
  connect( stopBtn, SIGNAL( clicked() ), this, SLOT( stopTrial() ) );
  grid->addWidget( stopBtn, 5, 5, 1, 1 );

  this->setLayout( grid );  
}

void YMaze::setupTable(QGridLayout *grid) {
  QString activeStyle = "QLabel{color: grey}";
  QString passiveStyle = "QLabel{color: black}";
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


MazeCondition YMaze::nextMazeCondition() {
  MazeCondition mazeCondition;
  
  int nextPosition = rand() %3;
  while ( nextPosition == lastRewardPosition ) {
    nextPosition = rand() %3;
  }
  std::vector<MazeArm> arms = {MazeArm::A, MazeArm::B, MazeArm::C};
  if (lastRewardPosition == static_cast<int> (MazeArm::NONE ) ) {
    mazeCondition.rewarded = arms[nextPosition];
    arms.erase(arms.begin() + nextPosition);
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

StimulusCondition YMaze::nextStimulusConditions() {
  StimulusCondition sc;
  sc.rewardedFreq = rewardedFreq;

  return sc;
}
  
TrialCondition YMaze::nextTrialCondition() {
  TrialCondition tc;
  tc.mazeCondition = nextMazeCondition();
  tc.stimCondition = nextStimulusConditions();
  return tc;
}
  
void createStimuli() {};
  
void YMaze::startNextTrial() {
  postCustomEvent( START_TRIAL );
}

void YMaze::stopTrial() {
  postCustomEvent ( STOP_TRIAL );
}

void YMaze::updateUI(const TrialCondition &tc) {
  sketch->setCondition( tc.mazeCondition );
  // TODO update table with stimulus conditions
}

void YMaze::customEvent( QEvent *qce ) {
  TrialCondition tc;
  switch ( qce->type() - QEvent::User ) {
  case START_TRIAL:
    std::cerr << "Trial start!" << std::endl;
    tc = nextTrialCondition();
    updateUI(tc);
    break;
  case STOP_TRIAL:
    std::cerr << "Trial stop!" << std::endl;
    break;
  default:
    break;
  } 
}



//************************************************************************
//************************************************************************

int YMaze::main( void ) {
  // get options:
  duration = number( "duration" );
  
  bool start = false;
  while ( softStop() == 0 ) {
      if ( interrupt() || softStop() > 0 )
        break;
      if ( !start ){
        sleep(0.2);
        continue;
      } else {
	std::cerr << "start stimulus" << std::endl;
      }
  }
  return Completed;
}


addRePro( YMaze, efield );

}; /* namespace efield */

#include "moc_ymaze.cc"
