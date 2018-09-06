/*
  efish/robottofishposition.cc
  move the robot in fish coordinates

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

#include <relacs/efish/robottofishposition.h>

using namespace relacs;

namespace efish {


RobotToFishPosition::RobotToFishPosition( void )
  : RePro( "RobotToFishPosition", "efish", "Jan Grewe", "1.0", "Aug 20, 2018" )
{
  // add some options:
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  newSection("Position");
  addPoint("destination", "Position of fish head (x,y,z)mm", Point(0.,0.,0.), -1000.0, 1000.0, 1.0, "mm");
  addBoolean( "followmidline", "Auto-adjust y to follow the fish midline.", true);

  newSection( "Robot setup" );
  addSelection( "xmapping", "Mapping of x-axis to robot axis", "y|z|x" );
  addBoolean( "xinvert", "Select to map 0 position in relacs to max position of the robot.", true);
  addSelection( "ymapping", "Mapping of y-axis to robot axis", "z|x|y");
  addBoolean( "yinvert", "Select to map 0 position in relacs to max position of the robot.", false);
  addSelection( "zmapping", "Mapping of z-axis to robot axis", "x|y|z");
  addBoolean( "zinvert", "Select to map 0 position in relacs to max position of the robot.", false);

  done = false;
  start = false;

  QVBoxLayout *vb = new QVBoxLayout;
  goBtn = new QPushButton("go");
  cancelBtn = new QPushButton("done");
  vb->addWidget( goBtn );
  vb->addWidget( cancelBtn );
  connect( goBtn, SIGNAL( clicked() ), this, SLOT( go() ));
  connect( cancelBtn, SIGNAL( clicked() ), this, SLOT( cancel() ));
  setLayout( vb );
}

void RobotToFishPosition::go( ) {
  start = true;
  wake();
}

void RobotToFishPosition::cancel( ) {
  done = true;
  wake();
}


double RobotToFishPosition::getYSlope( ) {
  Point fish_head = robot->get_fish_head( );
  Point fish_tail = robot->get_fish_tail( );

  double slope = (fish_tail[axis_map[1]] - fish_head[axis_map[1]]) /
    (fish_tail[axis_map[0]] - fish_head[axis_map[0]]) * axis_invert[axis_map[1]];
   std::cerr << "TSlope: "  << slope << std::endl;
  return slope;
}


bool RobotToFishPosition::moveToPosition( ) {
  bool adjust_y = boolean( "followmidline" );
  Point destination( point( "destination" ) );
  Point fish_head = robot->get_fish_head( );

  for (size_t i = 0; i < axis_map.size(); i++ ) {
    fish_head[axis_map[i]] +=  axis_invert[i] * destination[i];
  }

  double y_slope = 0.0;
  double y_corrector = 0.0;
  if ( adjust_y ) {
    y_slope = getYSlope();
    y_corrector = y_slope * destination[0];
    fish_head[axis_map[1]] +=  axis_invert[1] * y_corrector;
  }

  if ( robot->stopped() )
    return false;
  robot->powerAxes( true );
  sleep( 0.75 );
  robot->PF_up_and_over( fish_head );
  robot->wait();
  sleep( 0.1 );
  robot->powerAxes( false );
  return true;
}


int map_axis( const string &s ){
  vector<string> axes = {"x", "y", "z"};
  for ( int i = 0; i < (int)axes.size(); i++ ) {
    if ( axes[i].compare(s) == 0 )
      return i;
  }
  return -1;
}


int RobotToFishPosition::main( void )
{
  done = false;
  // get options:
  axis_map = {0, 1, 2};
  axis_invert = {1, 1, 1};
  axis_map[0] = map_axis( text("xmapping") );
  axis_map[1] = map_axis( text("ymapping") );
  axis_map[2] = map_axis( text("zmapping") );
  axis_invert[0] = boolean("xinvert") ? -1 : 1;
  axis_invert[1] = boolean("yinvert") ? -1 : 1;
  axis_invert[2] = boolean("zinvert") ? -1 : 1;

  robot = dynamic_cast<misc::XYZRobot*>( device( "robot-2" ) );
  if ( robot == 0 ) {
    warning( "No Robot! please add 'XYZRobot' to the controlplugins int he config file." );
    return Failed;
  }
  if ( !robot->isOpen() )
    robot->start_mirob();
  if ( robot->stopped() ) {
    warning( "Robot can not move or desired point is forbidden!" );
    return Failed;
  }
  std::cerr << "running!" << std::endl;
  while( softStop() == 0 && ! interrupt() && ! done) { 
    postCustomEvent( 11 ); // setFocus();
    if ( done )
      break;
    if ( start ) {
      moveToPosition( );
      start = false;
      break;
    }

    sleep(0.2);
  }
  return Completed;
}


addRePro( RobotToFishPosition, efish );

}; /* namespace efish */

#include "moc_robottofishposition.cc"
