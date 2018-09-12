 /*
  base/robot.cc
  Shows the state of a robot.

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


#include <QLabel>
#include <QColor>
#include <QString>
// for RenderArea:
#include <QPainter>
#include <QBrush>
#include <QPen>
#include <relacs/shape.h>
#include <relacs/base/robot.h>
#include <relacs/str.h>

using namespace relacs;

namespace base {


Robot::Robot( void )
  : Control( "Robot", "base", "Alexander Ott", "1.0", "Jun 02, 2017" )
{
  newSection("General");
  addText( "robot", "Robot", "robot-1" );

  newSection("Stored positions");
  addPoint( "FishHeadPosition", "Position of fish head (x,y,z)mm", Point(0.,0.,0.));
  addPoint( "FishTailPosition", "Position of fish tail (x,y,z)mm", Point(0.,0.,0.));
  addPoint( "MovementAreaStart", "Start position of the movement area (x,y,z)mm", Point(0.,0.,0.));
  addPoint( "MovementAreaEnd", "End position of movement area (x,y,z)mm", Point(0.,0.,0.));
  addPoint( "ForbiddenAreaStart", "Start position of the forbidden area (x,y,z)mm", Point(0.,0.,0.));
  addPoint( "ForbiddenAreaEnd", "End position of the forbidden area (x,y,z)mm", Point(0.,0.,0.));
  addPoint( "CustomPosition", "Stored custom position (x,y,z)mm", Point(0.,0.,0.));

  newSection("Tool clamp");
  addNumber( "toolreleasedelay", "Delay before opening the tool clamp", 1.0, 0.0, 10.0, 0.25, "s");
  addNumber( "toolfixdelay", "Delay before closing the tool clamp", 1.0, 0.0, 10.0, 0.25, "s");
  addBoolean( "autoclamp", "Automatically close clamp after opening", false );

  // layout:
  QVBoxLayout *vb = new QVBoxLayout;
  setLayout( vb );

  QColor fg( Qt::green );
  QColor bg( Qt::black );
  QPalette qp( fg, fg, fg.lighter( 140 ), fg.darker( 170 ), fg.darker( 130 ), fg, fg, fg, bg );

  vb->addWidget(new QLabel("I am the small Watchdog of mirob:"));
  QFrame *line;
  //Divider line:
  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  vb->addWidget(line);

  QHBoxLayout *hb = new QHBoxLayout;

  plot = new RenderArea();
  hb->addWidget(plot,Qt::AlignCenter);
  vb->addLayout(hb);

  hb = new QHBoxLayout;
  vb->addLayout(hb);

  hb->addWidget(new QLabel("Height of graphic:"));

  heightBox = new QComboBox();
  heightBox->addItem("All objects");
  heightBox->addItem("Height of robot");
  hb->addWidget(heightBox);

  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  vb->addWidget(line);

  hb = new QHBoxLayout;
  xPos = new QLCDNumber();
  xPos->setSegmentStyle(QLCDNumber::Flat);
  hb->addWidget(new QLabel("x:"));
  hb->addWidget(xPos);

  yPos = new QLCDNumber();
  yPos->setSegmentStyle(QLCDNumber::Flat);
  hb->addWidget(new QLabel("y:"));
  hb->addWidget(yPos);

  zPos = new QLCDNumber();
  zPos->setSegmentStyle(QLCDNumber::Flat);
  hb->addWidget(new QLabel("z:"));
  hb->addWidget(zPos);

  vb->addLayout(hb);

  //Divider line:
  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  vb->addWidget(line);

  errorBox = new QTextEdit();
  errorBox->setFontPointSize( 8 );
  errorBox->setMaximumHeight( 80 );
  errorBox->setReadOnly( true );
  vb->addWidget( errorBox );

  //Divider line:
  line = new QFrame();
  line->setFrameShape(QFrame::HLine);
  line->setFrameShadow(QFrame::Sunken);
  vb->addWidget(line);

  QHBoxLayout *bb1 = new QHBoxLayout;

  StopButton = new QPushButton( "Stop!" );
  bb1->addWidget( StopButton );
  connect( StopButton, SIGNAL( clicked() ),
	   this, SLOT( stop() ) );
  ResetButton = new QPushButton( "Reset" );
  bb1->addWidget( ResetButton );
  connect( ResetButton, SIGNAL( clicked() ),
	   this, SLOT( resetRobot() ) );
  vb->addLayout( bb1 );

  QHBoxLayout *bb2 = new QHBoxLayout;

  StorePositionButton = new QPushButton( "Store" );
  bb2->addWidget( StorePositionButton );
  connect( StorePositionButton, SIGNAL( clicked() ),
	   this, SLOT( storePos() ) );

  ReturnToPositionButton = new QPushButton( "Return" );
  ReturnToPositionButton->setEnabled( false );
  bb2->addWidget( ReturnToPositionButton );
  connect( ReturnToPositionButton, SIGNAL( clicked() ),
	   this, SLOT( returnToPos() ) );

  HomeButton = new QPushButton( "Home" );
  bb2->addWidget( HomeButton );
  connect( HomeButton, SIGNAL( clicked() ),
	   this, SLOT( goHome() ) );
  vb->addLayout( bb2 );

  QHBoxLayout *bb3 = new QHBoxLayout;
  ToolReleaseButton = new QPushButton( "Release" );
  ToolReleaseButton->setToolTip("Release Tool");
  bb3->addWidget( ToolReleaseButton );
  connect( ToolReleaseButton, SIGNAL( clicked() ),
	   this, SLOT( toolRelease() ) );

  ToolFixButton = new QPushButton( "Clamp" );
  ToolFixButton->setToolTip( "Close the tool clamp" );
  bb3->addWidget( ToolFixButton );
  connect( ToolFixButton, SIGNAL( clicked() ),
	   this, SLOT( toolFix() ) );
  vb->addLayout( bb3 );
  //grabKey( Qt::ALT+Qt::Key_S );
  connect( this, SIGNAL( dialogAccepted() ), this, SLOT( updateConfig() ) );
}


Robot::~Robot( void ) {
  plot->~RenderArea();
}


void Robot::stop( void ) {
  robot->stop();
  robot->setStopped();
}


void Robot::goHome( void ) {
  postCustomEvent( 24 );
  /*
  robot->go_home();
  robot->wait();
  robot->powerAxes( false );
  */
}


void Robot::resetRobot( void ) {
  robot->setStopped( false );
  robot->go_home();
  robot->wait();
  robot->powerAxes( false );
}


void Robot::storePos( void ) {
  this->customPosition = robot->pos();
  ReturnToPositionButton->setEnabled( true );
  setPoint("CustomPosition", this->customPosition );
}


void Robot::returnToPos( void ) {
  postCustomEvent( 25 );
}


void Robot::toolRelease( void ) {
  robot->powerAxes( true );
  sleep( toolReleaseDelay );
  robot->releaseTool();
  if (this->autoclamp)
    toolFix();
  robot->powerAxes( false );
}


void Robot::toolFix( void ) {
  robot->powerAxes( true );
  sleep( toolFixDelay );
  robot->fixTool();
  robot->powerAxes( false );
}


void Robot::storePosition( const string &name, const Point &p ) {
  if ( !metaData().exist( name ))
    metaData().addPoint( name, "", p );
  else
    metaData().setPoint( name, p );
  if ( !exist( name ) )
    addPoint( name, "", p );
  else
    setPoint( name, p );
}


void Robot::config( void ) {
  robot = dynamic_cast<misc::XYZRobot*>( device( text( "robot" ) ) );
  if ( exist( "FishHeadPosition" ) ) {
    Point fish_start( point("FishHeadPosition") );
    Point fish_end( point("FishTailPosition") );
    if ( fish_start.distance( fish_end ) > 0.01 ) {
      robot->set_fish_head(fish_start);
      robot->set_fish_tail(fish_end);
      storePosition( "FishHeadPosition", fish_start );
      storePosition( "FishTailPosition", fish_end );
    }
  }
  if ( exist( "MovementAreaStart" ) ) {
    Point area_start( point( "MovementAreaStart" ) );
    Point area_end( point( "MovementAreaEnd" ) );
    if ( area_start.distance( area_end ) > 0.01 ) {
      robot->set_Area( new Cuboid( area_start, area_end ) );
      storePosition( "MovementAreaStart", area_start );
      storePosition( "MovementAreaEnd", area_end );
    }
  }
  if ( exist( "ForbiddenAreaStart" ) ) {
    Point forbidden_start( point( "ForbiddenAreaStart" ) );
    Point forbidden_end( point( "ForbiddenAreaEnd" ) );
    if ( forbidden_start.distance( forbidden_end ) > 0.01 ) {
      robot->add_forbidden( new Cuboid( forbidden_start, forbidden_end ) );
      storePosition( "ForbiddenAreaStart", forbidden_start );
      storePosition( "ForbiddenAreaEnd", forbidden_end );
    }
  }
  if ( exist( "CustomPosition" ) ) {
    Point p( point("CustomPosition") );
    if ( (p[0] + p[1] + p[2]) > 0.01 ) {
      this->customPosition = p;
      this->ReturnToPositionButton->setEnabled( true );
    }
  }
}


void Robot::customEvent( QEvent *qce )
{
  switch (qce->type() - QEvent::User) {
  case 21: {
    Point p = robot->pos();
    xPos->display(int(p.x()));
    yPos->display(int(p.y()));
    zPos->display(int(p.z()));
    break;
  }
  case 22:
    {
      for( int i=1; i<=3; i++ ){
	if( robot->axis_in_pos_limit(i) ) {
	  QString msg = QString("Axis").append(QString::number(i)).append(QString("is in the pos Limit!"));
	  errorBox->setTextColor(Qt::darkRed);
	  errorBox->append(msg);
	} else if ( robot->axis_in_neg_limit(i) ) {
	  QString msg = QString("Axis").append(QString::number(i)).append(QString("is in the neg Limit!"));
	  errorBox->setTextColor(Qt::darkRed);
	  errorBox->append(msg);
	}
      }
      break;
    }
  case 23:
    {
      if( robot->has_area() ) {
	Cuboid* cuboid = dynamic_cast<Cuboid*>( robot->area());
	if(test_height(cuboid)) {
	  QRect rect = prepare_cuboid_plot(cuboid);
	  plot->setAllowed(rect);
	} else {
	  plot->setAllowed(QRect(0,0,0,0));
	}
      } else {
	plot->setAllowed(QRect(0,0,0,0));
      }

      plot->clearForbidden();
      for ( Shape* shape: robot->forbiddenAreas() ) {
	Cuboid* cuboid = dynamic_cast<Cuboid*>(shape);
	if( test_height(cuboid) ) {
	  QRect rect = prepare_cuboid_plot(cuboid);
	  plot->addForbidden(rect);
	}
      }

      double width_fac = double(plot->size().width())  / robot->xlength();
      double height_fac= double(plot->size().height()) / robot->ylength();

      Point p = robot->pos();
      plot->setPosition(Point(p.x()*width_fac, p.y()*height_fac, 0));

      plot->update();
    }
    break;
  case 24: //go home robi!
    {
      robot->go_home();
      break;
    }
  case 25: // go to stored position
    {
      robot->PF_up_and_over( this->customPosition );
      robot->wait();
      robot->powerAxes( false );
      break;
    }

  default:
    Control::customEvent( qce );
  }
}


bool Robot::test_height(Cuboid* cuboid)
{
  switch (heightBox->currentIndex()) {
  case 0: { // height is not important.
    return true;
    break;
  }
  case 1: { // is the cuboid on the same height as the robot.
    Point start = cuboid->corner();
    double height = cuboid->height();
    double height_robot = robot->pos().z();

    if (start.z() <= height_robot and start.z() + height >= height_robot) {
      return true;
    } else {
      return false;
    }
    break;
  }
  default: {
    return false;
  }
  } //of switch
}


QRect Robot::prepare_cuboid_plot(Cuboid* cuboid) {
  double width_fac = double(plot->size().width())  / robot->xlength();
  double height_fac= double(plot->size().height()) / robot->ylength();

  Point start = cuboid->corner();
  int ploted_start_x = start.x()*width_fac;
  int ploted_start_y = start.y()*height_fac;
  int ploted_width = cuboid->length()*width_fac;
  int ploted_height= cuboid->width()*height_fac;

  return QRect(ploted_start_x, ploted_start_y, ploted_width, ploted_height);
}


void Robot::main( void )
{
  if( robot == 0 ) {
    errorBox->append( "Couldn't find the RobotController. Closing." );
    return;
  }
  this->customPosition = Point(point("CustomPosition"));
  this->toolReleaseDelay = number("toolreleasedelay");
  this->toolFixDelay = number("toolfixdelay");
  this->autoclamp = boolean( "autoclamp" );

  robot->init_mirob();

  while( ! interrupt() ) {
    sleep( 0.2 );
    postCustomEvent( 23 ); // draw update
    postCustomEvent( 22 ); // Limit switch control
    postCustomEvent( 21 ); // position LCDNumbers
  }
}
   
   
void Robot::updateConfig( void ) {
   if ( exist( "FishHeadPosition" ) ) {
      Point fish_start( point("FishHeadPosition") );
      Point fish_end( point("FishTailPosition") );
      if ( fish_start.distance( fish_end ) > 0.01 ) {
	 robot->set_fish_head(fish_start);
	 robot->set_fish_tail(fish_end);
	 storePosition( "FishHeadPosition", fish_start );
	 storePosition( "FishTailPosition", fish_end );
      }
   }
   if ( exist( "MovementAreaStart" ) ) {
      Point area_start( point( "MovementAreaStart" ) );
      Point area_end( point( "MovementAreaEnd" ) );
      if ( area_start.distance( area_end ) > 0.01 ) {
	 robot->set_Area( new Cuboid( area_start, area_end ) );
	 storePosition( "MovementAreaStart", area_start );
	 storePosition( "MovementAreaEnd", area_end );    
      }
   }

   if ( exist( "ForbiddenAreaStart" ) ) {
      Point forbidden_start( point( "ForbiddenAreaStart" ) );
      Point forbidden_end( point( "ForbiddenAreaEnd" ) );
      if ( forbidden_start.distance( forbidden_end ) > 0.01 ) {
	 robot->add_forbidden( new Cuboid( forbidden_start, forbidden_end ) );
	 storePosition( "ForbiddenAreaStart", forbidden_start );
	 storePosition( "ForbiddenAreaEnd", forbidden_end );
      }
   }

   if ( exist( "CustomPosition" ) ) {
      Point p( point("CustomPosition") );
      if ( (p[0] + p[1] + p[2]) > 0.01 ) {
	 this->customPosition = p;
	 this->ReturnToPositionButton->setEnabled( true );
      }
   }
}


void Robot::updateConfig( void )
{
  if ( exist( "FishHeadPosition" ) ) {
    Point fish_start( point("FishHeadPosition") );
    Point fish_end( point("FishTailPosition") );
    if ( fish_start.distance( fish_end ) > 0.01 ) {
      robot->set_fish_head(fish_start);
      robot->set_fish_tail(fish_end);
      storePosition( "FishHeadPosition", fish_start );
      storePosition( "FishTailPosition", fish_end );
    }
  }
  if ( exist( "MovementAreaStart" ) ) {
    Point area_start( point( "MovementAreaStart" ) );
    Point area_end( point( "MovementAreaEnd" ) );
    if ( area_start.distance( area_end ) > 0.01 ) {
      robot->set_Area( new Cuboid( area_start, area_end ) );
      storePosition( "MovementAreaStart", area_start );
      storePosition( "MovementAreaEnd", area_end );
    }
  }
  if ( exist( "ForbiddenAreaStart" ) ) {
    Point forbidden_start( point( "ForbiddenAreaStart" ) );
    Point forbidden_end( point( "ForbiddenAreaEnd" ) );
    if ( forbidden_start.distance( forbidden_end ) > 0.01 ) {
      robot->add_forbidden( new Cuboid( forbidden_start, forbidden_end ) );
      storePosition( "ForbiddenAreaStart", forbidden_start );
      storePosition( "ForbiddenAreaEnd", forbidden_end );
    }
  }
  if ( exist( "CustomPosition" ) ) {
    Point p( point("CustomPosition") );
    if ( (p[0] + p[1] + p[2]) > 0.01 ) {
      this->customPosition = p;
      this->ReturnToPositionButton->setEnabled( true );
    }
  }
}


void Robot::updateCalibration( void )
{
  storePosition( "FishHeadPosition", robot->get_fish_head() );
  storePosition( "FishTailPosition", robot->get_fish_tail() );

  int count = 0;
  for ( Shape* s : robot->forbiddenAreas() ) {
    Cuboid *c = dynamic_cast<Cuboid*>(s);
    if ( c == NULL )
      continue;
    Str name = "ForbiddenArea" + (count > 0 ? "_" + Str( count , 0, 1, 'i') : "" );
    storePosition( name + "Start", c->boundingBoxMin() );
    storePosition( name + "End", c->boundingBoxMax() );
    count++;
  }
  if ( robot->has_area() && robot->area() != NULL ) {
    storePosition( "MovementAreaStart", robot->area()->boundingBoxMin() );
    storePosition( "MovementAreaEnd", robot->area()->boundingBoxMax() );
  }
}


addControl( Robot, base );


/*-----Class RenderArea:-----*/

RenderArea::RenderArea(QWidget *parent)
  : QWidget(parent)
{
  setBackgroundRole(QPalette::Base);
  setAutoFillBackground(true);
  setMaximumSize(QSize(300,225));
}


QSize RenderArea::minimumSizeHint() const {
  return QSize(150,113);
}

QSize RenderArea::sizeHint() const {
  return QSize(300,225);
}

void RenderArea::setAllowed(QRect allowed) {
  this->allowed = allowed;
}

void RenderArea::addForbidden(QRect forb) {
  this->forbidden.push_back(forb);
}

void RenderArea::clearForbidden() {
  this->forbidden.clear();
}

void RenderArea::setPosition(const Point &p) {
  this->position = p;
}


void RenderArea::paintEvent(QPaintEvent *event)
{
  QPen pen = QPen(Qt::SolidLine);
  pen.setColor(Qt::white);

  QBrush brush = QBrush(Qt::Dense6Pattern);
  brush.setColor(Qt::black);

  bool antialiased = true;

  QPainter painter(this);
  painter.setPen(pen);
  painter.setBrush(brush);
  painter.setRenderHint(QPainter::Antialiasing, antialiased);

  //Draw the whole space black (the space in which the robot COULD move)
  painter.drawRect(0, 0, this->size().width(), this->size().height());

  // Drawing the allowed area:
  pen.setStyle(Qt::SolidLine);
  pen.setColor(Qt::black);

  brush.setStyle(Qt::SolidPattern);
  brush.setColor(Qt::white);

  painter.setPen(pen);
  painter.setBrush(brush);

  if(allowed.isValid()) {
    painter.drawRect(allowed);
  }

  //Drawing forbidden areas:
  pen.setStyle(Qt::SolidLine);
  pen.setColor(Qt::darkRed);

  brush.setStyle(Qt::Dense2Pattern);
  brush.setColor(Qt::darkRed);

  painter.setPen(pen);
  painter.setBrush(brush);

  for (QRect rect: forbidden) {
    painter.drawRect(rect);
  }

  // Darwing the position of the robot.
  pen.setStyle(Qt::SolidLine);
  pen.setColor(Qt::black);

  brush.setStyle(Qt::SolidPattern);
  brush.setColor(Qt::black);

  painter.setPen(pen);
  painter.setBrush(brush);

  int ellipseSize = 3;
  painter.drawEllipse(position.x(), position.y(), ellipseSize, ellipseSize);

}


}; /* namespace base */


#include "moc_robot.cc"
