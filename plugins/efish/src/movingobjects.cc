/*
  efish/movingobjects.cc
  Use the robot to present moving objects as stimuli.

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

#include <relacs/efish/movingobjects.h>
using namespace relacs;

namespace efish {


MovingObjects::MovingObjects( void )
  : RePro( "MovingObjects", "efish", "Jan Grewe", "1.0", "Sep 10, 2018" )
{
  // add some options:
  newSection( "Movement" );
  addPoint( "startpos", "Starting point of object motion (x,y,z  rel. to fish head)", Point::Origin);
  addNumber( "traveldist", "Travel distance of the object", 0.0, -1000., 1000., 0.1, "mm" );
  addNumber( "travelspeed", "Default travel speed", 40.0, 0.0, 100.0, 0.1, "mm/s");
  addSelection( "movementaxis", "Movement along which dimension", "x|y|z" );

  newSection( "Parameter space");
  addNumber( "distmin", "Minimum lateral distance from fish (z).", 0.0, -200.00, 200.0, 1., "mm" );
  addNumber( "distmax", "Maximum lateral distance", 0.0, -1000., 1000., 0.1, "mm" );
  addNumber( "diststep", "Desired step size of the distance-range", 1.0, 0.1, 1000., 0.1, "mm" );

  addNumber( "speedmin", "Minimum travel speed of the object", 0, 0, 450, 1, "mm/s" );
  addNumber( "speedmax", "Maximum travel speed of the object", 0, 0, 450, 1, "mm/s" );
  addNumber( "speedstep", "Desired step size of the speed range", 0, 0, 400, 1., "mm/s" );

  newSection( "Robot setup" );
  addSelection( "xmapping", "Mapping of x-axis to robot axis", "y|z|x" );
  addBoolean( "xinvert", "Select to map 0 position in relacs to max position of the robot.", true);
  addSelection( "ymapping", "Mapping of y-axis to robot axis", "z|x|y");
  addBoolean( "yinvert", "Select to map 0 position in relacs to max position of the robot.", false);
  addSelection( "zmapping", "Mapping of z-axis to robot axis", "x|y|z");
  addBoolean( "zinvert", "Select to map 0 position in relacs to max position of the robot.", false);

  addPoint( "safepos", "Safe position to which the robot returns", Point::Origin);
}


Point MovingObjects::convertAxes( const Point &p ) {
  Point destination( fish_head );
  for (size_t i = 0; i < axis_map.size(); i++ ) {
    destination[axis_map[i]] +=  axis_invert[i] * p[i];
  }
  return destination;
}


bool MovingObjects::moveToPosition( const Point &p ) {
  Point destination( fish_head );
  Point temp_dest( p );
  for (size_t i = 0; i < axis_map.size(); i++ ) {
    destination[axis_map[i]] +=  axis_invert[i] * temp_dest[i];
  }
  if ( robot->stopped() )
    return false;
  robot->PF_up_and_over( destination );
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


int MovingObjects::main( void )
{
  Point start = point( "startpos" );
  Point safe_pos = point( "safepos" );

  double travel_dist = number( "traveldist" );
  std::string axis = text( "movementaxis" );

  int speed_min = number( "speedmin" );
  int speed_max = number( "speedmax" );
  int speed_step = number( "speedstep" );

  double z_min = number( "distmin" );
  double z_max = number( "distmax" );
  double z_step = number( "diststep" );

  LinearRange speedrange( (double)speed_min, (double)speed_max, (double)speed_step );
  LinearRange distrange( z_min, z_max, z_step );

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
  if ( !robot->PF_up_and_over( safe_pos ) ) {
    warning( "Robot can not move or desired point is forbidden!" );
    return Failed;
  }
  robot->wait();
  if ( interrupt() ) {
    robot->PF_up_and_over( safe_pos );
    robot->wait();
    return Aborted;
  }

  fish_head = robot->get_fish_head();
  fish_tail = robot->get_fish_tail();

  axis_map = {0, 1, 2};
  axis_invert = {1, 1, 1};
  axis_map[0] = map_axis( text("xmapping") );
  axis_map[1] = map_axis( text("ymapping") );
  axis_map[2] = map_axis( text("zmapping") );
  axis_invert[0] = boolean("xinvert") ? -1 : 1;
  axis_invert[1] = boolean("yinvert") ? -1 : 1;
  axis_invert[2] = boolean("zinvert") ? -1 : 1;


  Point dest( start );
  if ( axis == "x" )
    dest[ 0 ] += travel_dist;
  else if ( axis == "y" )
    dest[ 1 ] += travel_dist;
  else
    dest[ 2 ] += travel_dist;

  Point rdest = convertAxes( dest ); //convert fish to robot coordinates
  Point rstart = convertAxes( start );

  robot->PF_up_and_over( safe_pos );

  OutData sig;
  sig.setTrace( 0 );
  sig.constWave( 0.01, -1, 0.0 );
  sig.setIdent( "moving object" );
  sig.mute();
  sig.description().newSection( "Robot" );

  Options opts;
  Parameter &p1 = opts.addNumber( "speed", 0.0, "mm/s" );
  Parameter &p2 = opts.addNumber( "lateral position", 0., "mm" );
  Parameter &p3 = opts.addNumber( "direction", 1 );
  sig.setMutable( p1 );
  sig.setMutable( p2 );
  sig.setMutable( p3 );
  sig.setDescription( opts );

  for(int i = 0; i < distrange.size(); i ++) {
    double z_pos = distrange[i];
    rdest[axis_map[2]] += (z_pos * axis_invert[2]);
    rstart[axis_map[2]] += (z_pos * axis_invert[2]);
    robot->go_to_point( rstart );
    robot->wait();
    for (int j = 0; j < speedrange.size(); j++) {
      int speed = (int)speedrange[j];
      sleep(1.5);
      if ( !interrupt() ) {
	sig.description().setNumber("speed", speed);
	sig.description().setNumber("direction", 1);
	sig.description().setNumber("lateral position", z_pos);
	write(sig);
	robot->go_to_point( rdest, speed );
	robot->wait();
      }

      if ( !interrupt() ) {
	sleep(1.5);
	sig.description().setNumber("speed", speed);
	sig.description().setNumber("direction", -1);
	sig.description().setNumber("lateral position", z_pos);
	write( sig );
	robot->go_to_point( rstart, speed );
	robot->wait();
      }

      if ( interrupt() ) {
	robot->PF_up_and_over( safe_pos );
	robot->wait();
	return Failed;
      }
    }
  }
  robot->PF_up_and_over( safe_pos );
  robot->wait();
  robot->powerAxes( false );
  return Completed;
}


addRePro( MovingObjects, efish );

}; /* namespace efish */

#include "moc_movingobjects.cc"
