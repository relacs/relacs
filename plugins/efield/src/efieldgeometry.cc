/*
  efield/efieldgeometry.cc
  Measure electric field amplitude on a grid.

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

#include <relacs/efield/efieldgeometry.h>
#include <string>
using namespace relacs;

namespace efield {


EFieldGeometry::EFieldGeometry( void )
  : RePro( "EFieldGeometry", "base", "Alexander Ott", "1.0", "Jun 19, 2017" )
{
  // add some options:
  newSection("Enviroment data");\

  addNumber("temperature", "Temperature", 26, 0, 100, 0.1, "°C", "°C");
  addNumber("conductivity", "Conductivity", 330, 0, 10000, 1, "µS", "µS" );
  addNumber("water_level", "Water level", 20 , 0, 50, 0.1, "cm", "cm");


  newSection("Measurement data");
  addText( "robot", "Robot", "robot-1" );
  addText( "type", "Type of measurement", "");
  addNumber( "distance", "Distance of the object", 0, 0, 100, 1, "mm", "mm");

  addNumber( "head_length", "Overhang by the head", 20, -1000, 1000, 1, "mm", "mm" );
  addNumber( "tail_length", "Overhang by the tail", 20, -1000, 1000, 1, "mm", "mm" );

  addNumber( "width_posY", "Width in pos-Y direction of the fish", 50, -1000, 1000, 1, "mm", "mm" );
  addNumber( "width_negY", "Width in neg-Y direction of the fish", 50, -1000, 1000, 1, "mm", "mm" );

  addNumber( "height", "Height over the fish", 50, -1000, 1000, 1, "mm", "mm" );
  addNumber( "depth", "Depth under the fish", 50, -1000, 1000, 1, "mm", "mm" );

  addNumber( "step_length_x", "Step length on the x-axis", 5, 1, 200, 0.1, "mm", "mm");
  addNumber( "step_length_y", "Step length on the y-axis", 5, 1, 200, 0.1, "mm", "mm");
  addNumber( "step_length_z", "Step length on the z-axis", 5, 1, 200, 0.1, "mm", "mm");


   // TODO and yet another gui

     // layout:
  QVBoxLayout *vb = new QVBoxLayout;
  QHBoxLayout *hb;

  setLayout( vb );
  // base layout

  //QLabel *label;
  QColor fg( Qt::green );
  QColor bg( Qt::black );
  QPalette qp( fg, fg, fg.lighter( 140 ), fg.darker( 170 ), fg.darker( 130 ), fg, fg, fg, bg );

  vb->addWidget( new QLabel("E-Field Geometry:"));
  QFrame *line;

  //Divider line:
  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  vb->addWidget(line);


  vb->addWidget( new QLabel("Current Status:"));

  hb = new QHBoxLayout;
  hb->addWidget(new QLabel("Current Point:"));
  currentPoint = new QLCDNumber();
  currentPoint->setSegmentStyle(QLCDNumber::Flat);
  hb->addWidget(currentPoint);

  hb->addWidget(new QLabel("Total Points"));
  totalPoints = new QLCDNumber();
  totalPoints->setSegmentStyle(QLCDNumber::Flat);
  hb->addWidget(totalPoints);

  vb->addLayout(hb);


  //Divider line:
  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  vb->addWidget(line);


  hb = new QHBoxLayout;

  continueButton = new QPushButton( "Continue" );
  hb->addWidget( continueButton );
  continueButton->setFixedHeight( continueButton->sizeHint().height()*2 );
  connect( continueButton, SIGNAL( clicked() ),
	   this, SLOT( slot_continue() ) );

  pauseButton = new QPushButton( "Pause" );
  hb->addWidget( pauseButton );
  pauseButton->setFixedHeight( pauseButton->sizeHint().height()*2 );
  connect( pauseButton, SIGNAL( clicked() ),
	   this, SLOT( slot_pause() ) );

  stopButton = new QPushButton( "Stop completely!" );
  hb->addWidget( stopButton );
  stopButton->setFixedHeight( stopButton->sizeHint().height()*2 );
  connect( stopButton, SIGNAL( clicked() ),
	   this, SLOT( slot_stop() ) );

  stopButton->setEnabled(false);
  continueButton->setEnabled(false);

  vb->addLayout(hb);


  //Divider line:
  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  vb->addWidget(line);

  //Text box:
  hb = new QHBoxLayout;
  hb->setSpacing( 4 );
  vb->addLayout( hb );

  textBox = new QTextEdit();
  textBox->setFontPointSize(8);
  hb->addWidget(textBox);


  /*
  QPushButton *posButton = new QPushButton( "Go to Position!");
  bb->addWidget(posButton);
  connect( posButton, SIGNAL( clicked() ),
	   this, SLOT( pos_apply() ) );

  */

}


void EFieldGeometry::slot_pause()
{
  stopButton->setEnabled(true);
  continueButton->setEnabled(true);

  paused = true;
}


void EFieldGeometry::slot_continue()
{
  stopButton->setEnabled(false);
  continueButton->setEnabled(false);

  paused = false;

}


void EFieldGeometry::slot_stop()
{
  stopButton->setEnabled(false);
  continueButton->setEnabled(false);

  requestStop();

  paused = false;
}


class EFieldGeometryEvent : public QEvent
{

public:
  EFieldGeometryEvent(int point, int max_number) :
    QEvent( Type( User+21 )), Point(point), Max_Number(max_number)
  {
  }

  int Point;
  int Max_Number;
};


void EFieldGeometry::customEvent( QEvent *qce )
{
  switch (qce->type() - QEvent::User) {

  case 21: {
    EFieldGeometryEvent *efg = dynamic_cast<EFieldGeometryEvent*>(qce);
    int current = efg->Point;
    int max = efg->Max_Number;

    this->currentPoint->display(current);
    this->totalPoints->display(max);
    break;
  }
  default: {
    RePro::customEvent(qce);
  }
  }
}


void EFieldGeometry::keyPressEvent(QKeyEvent *e)
{
  /*
   if(! keyboard_active) {
     RePro::keyPressEvent( e );
     return;
   }
  */

  switch ( e->key() ) {

  default:
    RePro::keyPressEvent( e );
    break;
  }
}


void EFieldGeometry::keyReleaseEvent(QKeyEvent *e)
{
  /*
  if(! keyboard_active) {
     RePro::keyReleaseEvent( e );
     return;
   }
  */

/* only accept the event if it is not from a autorepeat key */
  if(e->isAutoRepeat() ) {
    e->ignore();
  } else {
    e->accept();
    switch ( e->key() ) {

    default:
	RePro::keyReleaseEvent( e );
	break;
    }
  }
}


bool EFieldGeometry::build_grid()
{
  double head_length = number( "head_length" );
  double tail_length = number( "tail_length" );

  double width_posY = number( "width_posY" );
  double width_negY = number( "width_negY" );

  double height = number( "height" );
  double depth = number( "depth" );

  Point head = robot_control->get_fish_head();
  Point tail = robot_control->get_fish_tail();

  /*
    cerr<< "head length" << head_length << endl;
    cerr<< "tail length" << tail_length << endl;

    cerr<< "width posY" << width_posY << endl;
    cerr<< "width negY" << width_negY << endl;

    cerr<< "heigth" << heigth << endl;
    cerr<< "depth" << depth << endl;

    cerr<< "head" << head << endl;
    cerr<< "tail" << tail << endl;
  */

  double tiefe = height + depth;
  double width = width_posY + width_negY;
  double length = abs(tail.x()-head.x()) + head_length + tail_length;

  double step_x = number("step_length_x");
  double step_y = number("step_length_y");
  double step_z = number("step_length_z");

  Point start;

  if(head.x() < tail.x()) {
    start = head;
    start.x() -= (double) ((int) (head_length / step_x)) * step_x;
    start.y() -= (double) ((int) (width_negY / step_y)) * step_y;
    start.z() -= (double) ((int) (height / step_z)) * step_z;
  } else {
    start = head;
    start.x() -= (double) ((int) ((length-head_length)) / step_x) * step_x;
    start.y() -= (double) ((int) (width_negY / step_y)) * step_y;
    start.z() -= (double) ((int) (height / step_z))  * step_z;
  }

  cerr << "start point: " << start << endl;



  grid.clear();

  double x=0;

  for(double z=0; z<=tiefe; z+=step_z) {
    for(double y=0; y<=width; y+=step_y) {
      for(x=0; x<=length; x+=step_x) {
	Point next = start;
	next.x() += x;
	next.y() += y;
	next.z() += z;
	if(point_safe(next)) {
	  grid.push_back(next);
	} else {
	  //cerr << "wf point unsafe: " << next << endl;
	}
      }

      x-=step_x; // because it becomes one step too big after the first for-loop.

      if(y+step_y <= width) {
	y+=step_y;
      } else {
	continue;
      }

      for(x+=0; x>=0; x-=step_x) {
	Point next = start;
	next.x() += x;
	next.y() += y;
	next.z() += z;
	if(point_safe(next)) {
	  grid.push_back(next);
	} else {
	  //cerr << "wb point unsafe: " << next << endl;
	}
      }

    }
  }

  grid.push_back(start);

  if(grid.size() > 0) {
    return true;
  } else {
    return false;
  }

}


bool EFieldGeometry::point_safe(const Point &p)
{
  //if the point is ouside of the area.
  if(! (robot_control->get_area()->inside(p))) {
    return false;
  }
  //if the point is inside or below a forbidden area.
  if(! (robot_control->test_point(p)) ) {
    return false;
  }

  return true;
}


int EFieldGeometry::main( void )
{
  // get options:
  string robotid = text( "robot" );

  robot_control = dynamic_cast<misc::XYZRobot*>( device( robotid ) );
  if ( robot_control == 0 ) {
    warning( "No Robot! please add 'XYZRobot' to the controlplugins int he config file." );
    return Failed;
  }
  
  if(! robot_control->has_area()) {
    warning( "Robot has no area. Please calibrate it first with the CalibrateRobot repro." );
    return Aborted;
  }
  robot_control->start_mirob();

  if ( interrupt() ) {
    robot_control->close_mirob();
    return Aborted;
  }

  // add some more options to be saved:
  Point head = robot_control->get_fish_head();
  Point tail = robot_control->get_fish_tail();

  string head_text = "head-"+ to_string(head.x()) +"-" +
    to_string(head.y()) +"-" + to_string(head.z());

  string tail_text = "tail-"+ to_string(tail.x()) +"-" +
    to_string(tail.y()) +"-" + to_string(tail.z());

  // Measurement:
  build_grid();

  int size = grid.size();
  int countTotal = 1;
  QCoreApplication::postEvent( this, new EFieldGeometryEvent(countTotal,size));

  OutData signal;
  signal.setTrace(0);
  signal.constWave( 0.5, 0.01, 0.0, "x-y-z_coordinate" );
  signal.setDelay( 0 );
  signal.description().addNumber( "x-coord", 0., "mm" ).addFlags( OutData::Mutable );
  signal.description().addNumber( "y-coord", 0., "mm" ).addFlags( OutData::Mutable );
  signal.description().addNumber( "z-coord", 0., "mm" ).addFlags( OutData::Mutable );
  signal.description().addText("head", head_text);
  signal.description().addText("tail", tail_text);

  for ( Point p : grid ) {
    robot_control->PF_up_and_over( p );
    robot_control->wait();
    std::cerr << "Time mirob stopped moving: " << currentTime() << endl;
    sleep( 0.1 );
    QCoreApplication::postEvent( this, new EFieldGeometryEvent(countTotal,size));
    cerr << "Point: " << countTotal++ << " of " << size << endl;


    // Pause
    while ( paused ) {
      sleep(0.5);
    }

    if ( interrupt() ) {
      robot_control->PF_up_and_over(Point(0,0,0));
      robot_control->wait();
      robot_control->close_mirob();
      return Aborted;
    }

    // close mirob to avoid interference ?? power off

    signal.description().setNumber( "x-coord", p.x());
    signal.description().setNumber( "y-coord", p.y());
    signal.description().setNumber( "z-coord", p.z());
 
    //std::cerr << "***** Test Write: " << testWrite(signal) << " *****" << std::endl;


    std::cerr << "Time right before write: " << currentTime() << endl;
    startWrite( signal );
    sleep(0.6);


    // analysieren:
    /*   const InData &data = trace( "V-1" );
	 SampleDataF input( 0.0, signal.length(), data.stepsize() ); // numerics/include/relacs/sampledata.h
	 data.copy( data.signalTime(), input );
    */


    if ( interrupt() ) {
      robot_control->PF_up_and_over(Point(0,0,0));
      robot_control->wait();
      robot_control->close_mirob();
      return Aborted;
    }

    // restart mirob after the avoidoidance of interference ?? power on ?

  }
 
  robot_control->PF_up_and_over(Point(0,0,0));
  robot_control->wait();

  robot_control->close_mirob();
  return Completed;
}


addRePro( EFieldGeometry, efield );

}; /* namespace efield */

#include "moc_efieldgeometry.cc"
