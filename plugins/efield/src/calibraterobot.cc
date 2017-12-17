/*
  efield/calibraterobot.cc
  Calibrate forbitten zones of an xyz robot.

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


#include <QString>
#include <relacs/efield/calibraterobot.h>

using namespace relacs;

namespace efield {


CalibrateRobot::CalibrateRobot( void )
  : RePro( "CalibrateRobot", "base", "Alexander Ott", "1.0", "May 19, 2017" )
{
  // add some options:
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );

  // layout:
  QVBoxLayout *vb = new QVBoxLayout;
  QHBoxLayout *bb;

  setLayout( vb );
  // base layout

  //QLabel *label;
  QColor fg( Qt::green );
  QColor bg( Qt::black );
  QPalette qp( fg, fg, fg.lighter( 140 ), fg.darker( 170 ), fg.darker( 130 ), fg, fg, fg, bg );



  //Buttons:
  //"GO Home" Button area: 

  vb->addWidget( new QLabel("Go back home area:"));

  bb = new QHBoxLayout;
  bb->setSpacing(2);
  vb->addLayout(bb);

  homeBox = new QComboBox;
  bb->addWidget(homeBox);
  homeBox->addItem("Go to -lim: z-y-x");
  homeBox->addItem("Go to -lim: x-y-z");
  homeBox->addItem("Go to -lim: y-z-x");
  homeBox->addItem("Go home");

  QPushButton *homeApplyButton = new QPushButton( "Apply" );
  bb->addWidget( homeApplyButton );
  homeApplyButton->setFixedHeight( homeApplyButton->sizeHint().height() );
  connect( homeApplyButton, SIGNAL( clicked() ),
	   this, SLOT( home_apply() ) );

  //Divider line:
  QFrame* line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  vb->addWidget(line);

  // Go to specific Point Button: 

  vb->addWidget( new QLabel("Movement diagonaly to a point:"));

  QGridLayout *setPosArea = new QGridLayout;
  setPosArea->setVerticalSpacing( 2 );
  setPosArea->setHorizontalSpacing( 2 );

  vb->addLayout(setPosArea);


  posXBox = new QSpinBox;
  posXBox->setRange(0,600);
  posXBox->setValue(0);
  QLabel *posXLabel = new QLabel("X-Position (0-600) :");

  setPosArea->addWidget(posXLabel,0,0);
  setPosArea->addWidget(posXBox,0,1);

  posYBox = new QSpinBox;
  posYBox->setRange(0,450);
  posYBox->setValue(0);
  QLabel *posYLabel = new QLabel("Y-Position (0-450) :");

  setPosArea->addWidget(posYLabel,1,0);
  setPosArea->addWidget(posYBox,1,1);

  posZBox = new QSpinBox;
  posZBox->setRange(0,250);
  posZBox->setValue(0);
  QLabel *posZLabel = new QLabel("Z-Position (0-250) :");

  setPosArea->addWidget(posZLabel,2,0);
  setPosArea->addWidget(posZBox,2,1);

  QPushButton *posButton = new QPushButton( "Go to Position!");
  setPosArea->addWidget(posButton,0,2,0,3);
  connect( posButton, SIGNAL( clicked() ),
	   this, SLOT( pos_apply() ) );

  //Divider line:
  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  vb->addWidget(line);

  //Calibrate Button: 

  vb->addWidget( new QLabel("Calibration start and stop:"));

  bb = new QHBoxLayout;
  bb->setSpacing(2);
  vb->addLayout(bb);

  calibrateBox = new QComboBox;
  bb->addWidget(calibrateBox);
  calibrateBox->addItem("calibrate area to cover.");
  calibrateBox->addItem("stop for area to cover.");

  calibrateBox->addItem("calibrate forbidden area.");
  calibrateBox->addItem("stop for forbidden area.");

  calibrateBox->addItem("calibrate fishhead and tail");
  calibrateBox->addItem("stop fish calibration");

  QPushButton *calibrateButton = new QPushButton( "Go!" );
  bb->addWidget( calibrateButton );
  calibrateButton->setFixedHeight( calibrateButton->sizeHint().height() );
  connect( calibrateButton, SIGNAL( clicked() ),
	   this, SLOT( calibrate() ) );

  //Divider line:
  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  vb->addWidget(line);

  //Modifying calibrated areas
  vb->addWidget( new QLabel("Modify calibrated areas:"));

  bb = new QHBoxLayout;
  bb->setSpacing(2);
  vb->addLayout(bb);

  modifyAreasBox = new QComboBox;
  bb->addWidget(modifyAreasBox,2);
  modifyAreasBox->setEnabled(false);

  modifyChoicesBox = new QComboBox;
  bb->addWidget(modifyChoicesBox);
  modifyChoicesBox->setEnabled(false);
  modifyChoicesBox->addItem("pos X border (right)");
  modifyChoicesBox->addItem("neg X border (left)");

  modifyChoicesBox->addItem("pos Y border (far)");
  modifyChoicesBox->addItem("neg Y border (close)");

  modifyChoicesBox->addItem("pos Z border (down)");
  modifyChoicesBox->addItem("neg Z border (up)");

  modifyChoicesBox->addItem("delete area!");

  modifyLengthBox = new QSpinBox;
  modifyLengthBox->setRange(-100,100);
  modifyLengthBox->setValue(0);
  bb->addWidget(modifyLengthBox);


  QPushButton *ModifyButton = new QPushButton( "Modify!!" );
  bb->addWidget( ModifyButton, 1 );
  connect( ModifyButton, SIGNAL( clicked() ),
	   this, SLOT( slot_modify() ) );


  //Divider line:
  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  vb->addWidget(line);

  //STOP Button: 

  vb->addWidget( new QLabel("Quick Stop!"));

  QPushButton *StopButton = new QPushButton( "Stop!" );
  vb->addWidget( StopButton );
  StopButton->setFixedHeight( StopButton->sizeHint().height()*2 );
  connect( StopButton, SIGNAL( clicked() ),
	   this, SLOT( slot_stop() ) );

  //Divider line:
  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  vb->addWidget(line);


  //Text box:
  bb = new QHBoxLayout;
  bb->setSpacing( 4 );
  vb->addLayout( bb );

  errorBox = new QTextEdit();
  errorBox->setFontPointSize(8);
  bb->addWidget(errorBox);


}


CalibrateRobot::~CalibrateRobot( void )
{
  robot_control = dynamic_cast<misc::XYZRobot*>( device("robot-2") );
  if(robot_control != 0) {
    robot_control->close_mirob();
  }
}


  void CalibrateRobot::home_apply() {
    if(false) {
      //re Init robot from this thread.TODO
    }
    switch(homeBox->currentIndex() ) {

    case 0:
      robot_control->search_reference(3,2,1);
      break;
    case 1:
      robot_control->search_reference(1,2,3);
      break;
    case 2:
      robot_control->search_reference(2,3,1);
      break;
    case 3:
      robot_control->go_home();
      break;
    default:
      break;

    }

  }


  void CalibrateRobot::pos_apply() {
    if(false) {
      //re Init robot from this thread.TODO
    }
    double posX = posXBox->value();
    double posY = posYBox->value();
    double posZ = posZBox->value();

    robot_control->PF_up_and_over(Point(posX, posY, posZ));

  }


  void CalibrateRobot::calibrate() {
    if(false) {
      //re Init robot from this thread.TODO
    }

    switch(calibrateBox->currentIndex() ) {

    case 0: // calibrate area to cover.
      cali_area = true;
      break;
    case 1: // stop calibrating area to cover
      stop_cali = true;
      break;
    case 2: // calibrate forbidden area
      cali_forb = true;
      break;
    case 3: // stop calibrating forbidden area
      stop_cali = true;
      break;
    case 4: // calibrate where the fish head and tail are
      cali_fish = true;
      break;
    case 5: // stop fish calibration
      stop_cali = true;
      break;

    default:
      break;

    }
  }


  void CalibrateRobot::slot_modify() {
    if(modifyAreasBox->count() <= 0) {
      return;
    }

    int change = modifyLengthBox->value();
    int areaIndex = modifyAreasBox->currentIndex();

    // Iformation to send to controller.
    bool allowedAreaFlag = false;
    int shapeIndex = -1;

    // With an ASSUMPTION about the ORDER 
    // allowed area is always first if it exists:
    if(areaIndex == 0 and robot_control->has_area()) {
      allowedAreaFlag = true;
    } else if(robot_control->has_area()) {
      shapeIndex = areaIndex-1;
    } else {
      shapeIndex = areaIndex;
    }

    int job = modifyChoicesBox->currentIndex();

    robot_control->modify_shape(allowedAreaFlag, shapeIndex, job, change);
    postCustomEvent(21);
  }


  void CalibrateRobot::slot_stop() {
    if(false) {
      //re Init robot from this thread.TODO
    }
    robot_control->stop_all();
  }


  void CalibrateRobot::keyReleaseEvent(QKeyEvent *e) {

    if(! keyboard_active) {
       RePro::keyReleaseEvent( e );
       return;
     }

    if(! gui_thread_init) {
      robot_control->start_mirob();
      gui_thread_init = true;
    }

  /* only accept the event if it is not from a autorepeat key */
    if(e->isAutoRepeat() ) {
      e->ignore();
    } else {
      e->accept();
      switch ( e->key() ) {

      case Qt::Key_U:
      case Qt::Key_O:
	robot_control->stop_Z();
	break;

      case Qt::Key_K:
      case Qt::Key_I:
	robot_control->stop_Y();
	break;

      case Qt::Key_J:
      case Qt::Key_L:
	robot_control->stop_X();
	break;

      default:
	RePro::keyReleaseEvent( e );
	break;
      }
    }
  }


  void CalibrateRobot::keyPressEvent(QKeyEvent *e) {

     if(! keyboard_active) {
       RePro::keyPressEvent( e );
       return;
     }

     if(! gui_thread_init) {
       robot_control->start_mirob();
       gui_thread_init = true;
     }

    switch ( e->key() ) {

    case Qt::Key_Y:
      cont = true;
      break;
    case Qt::Key_N:
      back = true;
      break;

    case Qt::Key_O:
      robot_control->move_negZ();
      break;

    case Qt::Key_U:
      robot_control->move_posZ();
      break;

    case Qt::Key_I:
      robot_control->move_posY();
      break;

    case Qt::Key_K:
      robot_control->move_negY();
      break;

    case Qt::Key_J:
      robot_control->move_posX();
      break;

    case Qt::Key_L:
      robot_control->move_negX();
      break;


    default:
      RePro::keyPressEvent( e );
      break;
    }
  }


  Shape* CalibrateRobot::calibrate_area() {
    errorBox->append("Calibration entered.");
    errorBox->append("To calibrate please move mirob to the start point. (closest point to the neg limits)");
    errorBox->append("X-Axis: 'J','L' Y-Axis: 'I','K' Z-Axis: 'U','O'");
    errorBox->append("Press 'Y' to accept the point and 'B' to go back one point.");

    Point area_start;
    Point area_depth;
    Point area_length;
    Point area_width;

    int point_num = 0;
    while(point_num <4) {
      while(! cont) {

	if(stop_cali) {
	  errorBox->append("Calibration aborted.");
	  stop_cali = false;
	  return NULL;
	}

	sleepWait( 1.0 );
	if(back) {
	  if(point_num > 0) {
	    point_num--;
	  }
	  back = false;
	}
      }
      cont = false;

      switch(point_num) {
      case 0:
	area_start = robot_control->get_position();
	errorBox->append("start point accepted.");
	errorBox->append("Please use 'U' and 'O' to calibrate the depth.");

	cerr<< "start point accepted." << endl;
	break;
      case 1:
	area_depth = robot_control->get_position();
	cerr<< "depth point accepted." << endl;
	errorBox->append("Please use 'J' and 'L' to calibrate the length.");
	break;
      case 2:
	area_length = robot_control->get_position();
	errorBox->append("Please use 'I' and 'K' to calibrate the width.");
	cerr<< "length point accepted." << endl;
	break;
      case 3:;
	area_width = robot_control->get_position();
	cerr<< "width point accepted." << endl;
	break;
      }
      point_num++;
    }
    
    Shape *shape = new Cuboid(area_start, area_depth, area_length, area_width);
    cerr<<"Build Shape."<< endl;
    return shape;
  }


  Point CalibrateRobot::calibrate_point() {
    errorBox->append("Calibration entered.");
    errorBox->append("To calibrate please move mirob to the point.");
    errorBox->append("X-Axis: 'J','L' Y-Axis: 'I','K' Z-Axis: 'U','O'");
    errorBox->append("Press 'Y' to accept the point.");


    while(! cont) {

      if(stop_cali) {
	errorBox->append("Calibration aborted.");
	stop_cali = false;
	return Point(-1,-1,-1);
      }
      sleepWait( 1.0 );
    }
    cont = false;

    return robot_control->get_position();
  }


void CalibrateRobot::customEvent( QEvent *qce ) {

    switch (qce->type() - QEvent::User) {

    case 21: { // update modify list.
      modifyAreasBox->clear();
      if(robot_control->has_area()) {
	Cuboid *cuboid = dynamic_cast<Cuboid*>(robot_control->get_area());
	Point start = cuboid->startPoint();
	QString part1 = QString("Allowed Area ");
	QString part2 = QString("start: (") += QString::number(int(start.x())) += QString(", ");
	QString part3 = QString::number(int(start.y())) += QString(", ");
	QString part4 = QString::number(int(start.z())) += QString(")");
	QString complete = part1 += part2 += part3 += part4;
	modifyAreasBox->addItem(complete);
      }
      for ( Shape* shape: robot_control->forbiddenAreas() ) {
	Cuboid *cuboid = dynamic_cast<Cuboid*>(shape);
	Point start = cuboid->startPoint();

	QString part1 = QString("Forbidden Area ");
	QString part2 = QString("start: (") += QString::number(int(start.x())) += QString(", ");
	QString part3 = QString::number(int(start.y())) += QString(", ");
	QString part4 = QString::number(int(start.z())) += QString(")");
	QString complete = part1 += part2 += part3 += part4;
	modifyAreasBox->addItem(complete);
      }

      if(modifyAreasBox->count() > 0) {
	modifyAreasBox->setEnabled(true);
	modifyChoicesBox->setEnabled(true);
	modifyLengthBox->setEnabled(true);
      } else {
	modifyAreasBox->setEnabled(false);
	modifyChoicesBox->setEnabled(false);
	modifyLengthBox->setEnabled(false);
      }
      break;
    }

    default: {
      RePro::customEvent(qce);
    }
    }
  }

  //forward declaration it is below main.
  void test_go_to_point();

int CalibrateRobot::main( void )
{
  // get options:
  // double duration = number( "duration" );

  //Testing of the components of Controller::go_to_point().
  //test_go_to_point();


  robot_control = dynamic_cast<misc::XYZRobot*>( device( "robot-2" ) );
  if ( robot_control == 0 ) {
    warning( "No Robot! please add 'RobotController' to the controlplugins int he config file." );
    return Failed;
  }

  if( robot_control->start_mirob()) {
      robot_control->go_to_reference(false,30);
  }

  
  // PRECALIBRATION:

  robot_control->clear_forbidden();

  Shape* area = new Cuboid(Point(5,5,55),Point(615,340,230));
  robot_control->set_Area(area);
  
  Shape* forb1 = new Cuboid(Point(220,111,120),Point(281,199,195));
  robot_control->add_forbidden(forb1);

  Shape* forb2 = new Cuboid(Point(280,136,120),Point(391,173,195));
  robot_control->add_forbidden(forb2);
  
  Shape* forb3 = new Cuboid(Point(390,111,120),Point(443,199,195));
  robot_control->add_forbidden(forb3);
  /*
  Shape* forb4 = new Cuboid(Point(275,120,195),Point(395,190,240));
  robot_control->add_forbidden(forb4);
  */
  robot_control->set_fish_head(Point(215,155,155));
  robot_control->set_fish_tail(Point(445,155,155));
  
  postCustomEvent(21);
  errorBox->append("Mirob calibrated from hard coded.");


  keyboard_active = true;

  while(true) {
    sleep(0.1);
    //cerr << "Position in while main: " << robot_control->get_position() << endl;

    if(cali_area) {
      cali_area = false;
      Shape* n = calibrate_area();
      if (n != NULL) {
	robot_control->set_Area(n);
	errorBox->append("Area calibrated.");
	postCustomEvent(21);
      }
    }

    if ( cali_forb ) {
      cali_forb = false;
      Shape* n = calibrate_area();
      if (n != NULL) {
	robot_control->add_forbidden(n);
	errorBox->append("Forbidden zone added.");
	postCustomEvent(21);
      }
    }

    if(cali_fish) {
      cali_fish = false;

      errorBox->append("Fish head:");
      Point p = calibrate_point();
      if(p.x() != -1) {
	robot_control->set_fish_head(p);
      }

      errorBox->append("Fish tail:");
      p = calibrate_point();
      if(p.x() != -1)
	robot_control->set_fish_tail(p);

      errorBox->append("Fish calibrated.");
    }

    if ( interrupt() ) {
      robot_control->close_mirob();
      return Aborted;
    }
  }


  keyboard_active = false;

  robot_control->go_home();

  robot_control->wait();

  if ( interrupt() ) {
    robot_control->close_mirob();
    return Aborted;
  }
  robot_control->close_mirob();
  return Completed;
}


void  CalibrateRobot::test_how_many_move() {
  Point a = Point(0,0,0);
  Point b = Point(0,0,0);
  Point c = Point(10,0,0);
  Point d = Point(10,20,0);
  Point e = Point(10,20,30);
  Point f = Point(50,50,50);

  cerr << "Expected: 0 actual:" << robot_control->how_many_move(a,b) << endl;
  cerr << "Expected: 1 actual:" << robot_control->how_many_move(a,c) << endl;
  cerr << "Expected: 1 actual:" << robot_control->how_many_move(d,e) << endl;
  cerr << "Expected: 1 actual:" << robot_control->how_many_move(c,d) << endl;
  cerr << "Expected: 2 actual:" << robot_control->how_many_move(a,d) << endl;
  cerr << "Expected: 2 actual:" << robot_control->how_many_move(c,e) << endl;
  cerr << "Expected: 3 actual:" << robot_control->how_many_move(a,e) << endl;
  cerr << "Expected: 3 actual:" << robot_control->how_many_move(e,f) << endl;
}


void  CalibrateRobot::test_point_distances() {

  Point a = Point(0,0,0);
  Point b = Point(0,0,0);
  Point c = Point(10,0,0);
  Point d = Point(10,20,0);
  Point e = Point(10,20,30);
  Point f = Point(50,50,50);

  cerr << "Expected: 0, 0 , 0 actual:" << a.abs_diff(b) << endl;
  cerr << "Expected: 10,0 , 0 actual:" << a.abs_diff(c) << endl;
  cerr << "Expected: 10,20, 0 actual:" << a.abs_diff(d) << endl;
  cerr << "Expected: 10,20,30 actual:" << a.abs_diff(e) << endl;
  cerr << "Expected: 50,50,50 actual:" << a.abs_diff(f) << endl;
  cerr << "Expected: 40,50,50 actual:" << c.abs_diff(f) << endl;
  cerr << "Expected: 40,30,50 actual:" << d.abs_diff(f) << endl;
  cerr << "Expected: 40,30,20 actual:" << e.abs_diff(f) << endl;
}

void  CalibrateRobot::test_go_to_point() {

  test_how_many_move();
  test_point_distances();
}


addRePro( CalibrateRobot, efield );

}; /* namespace efield */

#include "moc_calibraterobot.cc"
