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

void YMazeSketch::resizeEvent(QResizeEvent *event) {
  drawSketch();
  QLabel::resizeEvent(event);
}


void YMazeSketch::setCondition(MazeArm rewarded, MazeArm unrewarded, MazeArm neutral) {
  QString styleStub = "QLabel{font-size: 20; font-weight: bold; color : ";
  QString styleSuffix = "}";
  labels[static_cast<int>(rewarded)]->setStyleSheet(styleStub +
						    colors[static_cast<int>(ArmCondition::REWARDED)] +
						    styleSuffix);
 labels[static_cast<int>(unrewarded)]->setStyleSheet(styleStub +
						     colors[static_cast<int>(ArmCondition::UNREWARDED)] +
						     styleSuffix);
  labels[static_cast<int>(neutral)]->setStyleSheet(styleStub +
						  colors[static_cast<int>(ArmCondition::NEUTRAL)] +
						  styleSuffix);

}

//***********************************************************************
//***********************************************************************  
YMaze::YMaze( void )
  : RePro( "YMaze", "efield", "Jan Grewe", "1.0", "Mar 18, 2020" )
{
  addNumber( "duration", "trial duration", 1.0, 0.1, 1000., 10.0, "s" );
  //  add
 // add some options:
  addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );

  // UI Layout
  QGridLayout *grid = new QGridLayout();
  sketch = new YMazeSketch();
  grid->addWidget( sketch, 0, 0, 4, 4 );
  setupTable(grid);
  QPushButton *startBtn = new QPushButton("Start");
  connect( startBtn, SIGNAL( clicked() ), this, SLOT( startNextTrial() ) );
  grid->addWidget( startBtn, 4, 5, 1, 1 );
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


void YMaze::startNextTrial() {
  int nextPosition = rand() %3;
  while ( nextPosition == lastRewardPosition ) {
    nextPosition = rand() %3;
  }
  std::vector<MazeArm> arms = {MazeArm::A, MazeArm::B, MazeArm::C};
  MazeArm rewarded, unrewarded, neutral;
  if (lastRewardPosition == static_cast<int> (MazeArm::NONE ) ) {
    rewarded = arms[nextPosition];
    arms.erase(arms.begin() + nextPosition);
    unrewarded = arms[0];
    neutral = arms[1];
    lastRewardPosition = nextPosition;
  } else {
    rewarded = arms[nextPosition];
    neutral = arms[lastRewardPosition];
    arms.erase( arms.begin() + nextPosition );
    arms.erase( std::find( arms.begin(), arms.end(), static_cast<MazeArm>( lastRewardPosition ) ) );
    unrewarded = arms[0];
    lastRewardPosition = nextPosition;
  }
  sketch->setCondition( rewarded, unrewarded, neutral );
}


int YMaze::main( void )
{
  // get options:
  duration = number( "duration" );
  bool start = false;
  while ( softStop() == 0 ) {
      if ( interrupt() || softStop() > 0 )
        break;
      if ( !start ){
        sleep(0.2);
        continue;
      }
  }
  return Completed;
}


addRePro( YMaze, efield );

}; /* namespace efield */

#include "moc_ymaze.cc"
