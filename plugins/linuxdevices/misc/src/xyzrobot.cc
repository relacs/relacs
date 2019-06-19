/*
  misc/xyzrobot.cc
  High level interface with object avoidance for the mirob robot.

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



#include <iostream>
#include <relacs/misc/xyzrobot.h>
using namespace relacs;

namespace misc {

  //Constructors: 

XYZRobot::XYZRobot( Mirob *robot )
  : Device( "XYZRobot" )
{
  open( *robot );
}


XYZRobot::XYZRobot( void )
  : Device( "XYZRobot" ),
    Robot( 0 )
{
}


XYZRobot::~XYZRobot( void )
{
  if ( this->Area != NULL)
    delete this->Area;
  clear_forbidden();
}


int XYZRobot::open( Mirob &robot )
{
  clearError();
  Info.clear();

  Robot = &robot;
  if ( ! isOpen() )
    return NotOpen;

  setDeviceVendor( "Alexander Ott" );
  setDeviceName( "XYZ Robot" );
  return 0;
}


int XYZRobot::open( Device &device )
{
  return open( dynamic_cast<Mirob&>( device ) );
}


bool XYZRobot::isOpen( void ) const
{
  return ( Robot != 0 && Robot->isOpen() );
}


void XYZRobot::close( void )
{
  /*
  if ( isOpen() ) {
  }
  */

  Info.clear();
  Robot = 0;
}


bool XYZRobot::test_point(const Point &p)
{
  for( Shape* fa : ForbiddenAreas ) {
    if ( fa->inside(p) || fa->below(p) )
      continue;
    else
      return false;
  }
  return true;
}


bool XYZRobot::test_way(const Point &pos, const Point &newP)
{
  //test if both end points are safe (to test pos is prob not needed)
  if(! test_point(newP) || ! test_point(pos)) {
    return false;
  }

  //Rekursive test the way:
  double dist = pos.distance(newP);
  //if both points are safe look if the distance is too big.
  if(dist < maxSafeDist) {
    // if the dist is small enough the way is (prob.) safe.
    return true;

  } else {
    //if it is too big get the midpoint
    //and test the way from pos to mid and from mid to newP.
    Point mid = pos.center(newP);

    if(test_way(pos,mid) && test_way(mid,newP) ) {
      return true;
    } else {
      return false;
    }
  }
}


bool XYZRobot::PF_up_and_over( const Point &p )
{
  if ( Stopped ) {
    std::cerr << "Robot was stopped movement forbidden! " << p << std::endl;
    return false;
  }
  // if there are no forbidden areas just move to the point.
  if ( ForbiddenAreas.empty() ) {
    go_to_point( p );
    return true;
  }
  if( ! test_point(p)) {
    std::cerr << "target point forbidden: " << p << std::endl;
    return false;
  }

  Point position = Robot->pos();
  //std::cerr << "Internal position:" << position << "\n";

  if(! test_point(position)) {
    std::cerr << "inside a forbidden area moving up. " << std::endl;
    position.z() -= 5;
    go_to_point(position);
    PF_up_and_over( p );
    return false;
  }


  if(test_way(position,p)) {
    // move mirob to p
    std::cerr <<"Way ok, moving to: " << p
	      << std::endl;
    go_to_point( p );
    return true;

    // if the way is blocked try to go over it.
  } else {
    int up = 2;
    // Go higher and higher until you can get over the forbidden area.
    Point pos_up = position;
    Point p_up = p;

    //first test which z is higher in space (lower value)
    //and set the other points z to it.
    if (pos_up.z() < p_up.z()) {
      p_up.z() = pos_up.z();
    } else {
      pos_up.z() = p_up.z();
    }

    while(! test_way(pos_up,p_up)) {
      //std::cerr <<"in while to move up? " << pos_up
      //		<< std::endl;
      pos_up.z() -= up;
      p_up.z() -= up;
    }


    // new path found:
    if(! test_way(position,pos_up)) {
      std::cerr << "Error: The way up is blocked. This PF cannot handle that."
		<< std::endl;
      return false;
    } else {
      // move mirob to pos_up
      std::cerr <<"Moving over obstacle pos_up is:" << pos_up
		<< std::endl;
      go_to_point(pos_up);
      wait();

    }

    // move mirob to p_up
    std::cerr <<"Moving over obstacle p_up is:" << p_up
	      << std::endl;
    go_to_point(p_up);
    wait();


    if(! test_way(p_up,p)) {
      std::cerr <<"Error: The way down is blocked. This PF cannot handle that."
		<< std::endl;
      return false;
    } else {
      // move mirob to p
      std::cerr <<"Moved over obstacle new pos is:" << p
		<< std::endl;
      go_to_point(p);
      wait();
      return true;
    }

  }
}


  //Init Robot:

bool XYZRobot::start_mirob( void )
{
  wasStarted = true;
  return Robot->start();
}


bool XYZRobot::init_mirob( void )
{
  return ( Robot->init_mirob() != -1 );
}


void XYZRobot::close_mirob( void )
{
  if ( !wasStarted ) {
    std::cerr << "Mirob cannot be closed, is not started." << std::endl;
  }
  else {
    Robot->close();
    wasStarted = false;
  }
}


bool XYZRobot::has_area( void ) const
{
  return ( Area != NULL );
}


// General movement:

void XYZRobot::go_home( void )
{
  PF_up_and_over( Home );
  wait();
  powerAxes( false );
}


void XYZRobot::search_reference( int firstmirobaxis, int secondmirobaxis, int thirdmirobaxis )
{
  bool ref = false;
  Robot->search_home( firstmirobaxis, 40, ref );
  Robot->search_home( secondmirobaxis, 40, ref );
  Robot->search_home( thirdmirobaxis, 40, ref );
}


void XYZRobot::go_to_point( double posX, double posY, double posZ )
{
  Point point = Point( posX, posY, posZ );
  go_to_point( point );
}


  // speed defaults to 40 if speed given is 0.
void XYZRobot::go_to_point( const Point &coords, int speed )
{
  if ( speed == 0 )
    speed = Robot->speed();

  Point position = Robot->pos();

  int to_move = how_many_move( position, coords );
  if ( to_move == 0 )
    return;
  //  Point dists = position.abs_diff(coords);
  Point dists = abs(position - coords);
  Point speeds = Point( speed*Robot->get_axis_factor( 0 ),
			speed*Robot->get_axis_factor( 1 ),
			speed*Robot->get_axis_factor( 2 ) );

  if ( to_move == 1 ) {
    for ( int axis=0; axis<3; axis++ ) {
      if ( dists[axis] > Robot->get_step_length( axis )/2 )
	Robot->move( axis, coords[axis], speeds[axis] );
    }
  }

  Point times = calculate_times( speeds, dists );
  double maxTime = get_max( times[0], times[1], times[2] );
  double precision = 0.005;

  if ( to_move == 2 || to_move == 3 ) {
    for ( int axis=0; axis<3; axis++ ) {
      if( times[axis] < maxTime ) {
	speeds[axis] = calc_speed( axis, speeds[axis],
				   dists[axis], maxTime, precision );
      }
    }
    if ( !Robot->checkPowerState() )
      powerAxes( true );
    if ( speeds[2] > 450 )
      speeds[2] = 450;
    Robot->move( 0, coords.x(), speeds[0] );
    Robot->move( 1, coords.y(), speeds[1] );
    Robot->move( 2, coords.z(), speeds[2] );
  }
  /*
  std::cerr << "how many axis need to move :" << to_move << std::endl;

  std::cerr << "distanceX is: " << dists[0] << std::endl;
  std::cerr << "distanceY is: " << dists[1] << std::endl;
  std::cerr << "distanceZ is: " << dists[2]<< std::endl;

  std::cerr << "maxTime is: " << maxTime << std::endl;
  std::cerr << "timeX is: " << times[0] << std::endl;
  std::cerr << "timeY is: " << times[1] << std::endl;
  std::cerr << "timeZ is: " << times[2] << std::endl;

  std::cerr << "speedX is: " << speeds[0] << std::endl;
  std::cerr << "speedY is: " << speeds[1] << std::endl;
  std::cerr << "speedZ is: " << speeds[2] << std::endl;
  */

  return;
}


void XYZRobot::move_posX()
{
  Robot->step( 0, 2, 40 );
}


void XYZRobot::move_negX()
{
  Robot->step( 0, -2, 40 );
}


void XYZRobot::move_posY()
{
  Robot->step( 1, 2, 40 );
}


void XYZRobot::move_negY()
{
  Robot->step( 1, -2, 40 );
}


void XYZRobot::move_posZ()
{
  Robot->step( 2, 4, 40 );
}


void XYZRobot::move_negZ()
{
  Robot->step( 2, -4, 40 );
}


int XYZRobot::stop( int axis )
{
  return Robot->stop( axis );
}


int XYZRobot::stop( void )
{
  return Robot->stop();
}


void XYZRobot::setStopped( void )
{
  this->setStopped( true );
}

void XYZRobot::setStopped( bool stopped )
{
  this->Stopped = stopped;
}


bool XYZRobot::stopped( void )
{
  return this->Stopped;
}


void XYZRobot::wait( void )
{
  Robot->wait();
}


void XYZRobot::releaseTool( void )
{
  Robot->toolRelease();
}


void XYZRobot::fixTool( void )
{
  Robot->toolFix();
}


bool XYZRobot::modify_shape( bool area, int forb_index, int job, int change )
{
  /*Jobs: 0 and 1 modify x, 2 and 3 modify y, 4 and 5 modify z, 6 delete shape */

  if(job > 6 or job < 0) {
    cerr << "Unknown job in modify_shape:XYZRobot." << endl;
    return false;
  }

  if ( !area and ( forb_index < 0 or (unsigned)forb_index > ForbiddenAreas.size() ) ) {
    cerr << "Wrong forb_index in modify_shape:XYZRobot." << endl;
    return false;
  }
  // JOB 6: deleting a shape.
  if(job == 6) {
    if(area) {
      this->Area = NULL;
      return true;
    } else {
      return del_forbidden_at_index(forb_index);
    }
  }

  // JOB 0-5 Modifying the shapes ONLY FOR CUBOIDS
  if(area) {
    modify_cuboid(dynamic_cast<Cuboid*>(this->Area), job, change);
  } else {
    modify_cuboid(dynamic_cast<Cuboid*>(ForbiddenAreas[forb_index]), job, change);
  }
  return true;
}

void XYZRobot::modify_cuboid( Cuboid* cuboid, int job, int change )
{
  /*Jobs: 0 and 1 modify x, 2 and 3 modify y, 4 and 5 modify z*/
  switch(job) {
  case 0:
    {
      int length = cuboid->length();
      cuboid->setLength(length+change);
      break;
    }
  case 1:
    {
      change = change * -1; // to make positive values increase the length and negative reduce it.
      int length = cuboid->length();
      Point start = cuboid->corner();
      start.x() = start.x()+change;

      cuboid->setCorner(start);
      cuboid->setLength(length-change);
      break;
    }
  case 2:
    {
      int width = cuboid->width();
      cuboid->setWidth(width+change);
      break;
    }
  case 3:
    {
      change = change * -1; // to make positive values increase the length and negative reduce it.
      int width = cuboid->width();
      Point start = cuboid->corner();
      start.y() = start.y()+change;

      cuboid->setCorner(start);
      cuboid->setWidth(width-change);
      break;
    }
  case 4:
    {
      int height = cuboid->height();
      cuboid->setHeight(height+change);
      break;
    }
  case 5:
    {
      change = change * -1; // to make positive values increase the length and negative reduce it.
      int height = cuboid->height();
      Point start = cuboid->corner();
      start.z() = start.z()+change;

      cuboid->setCorner(start);
      cuboid->setHeight(height-change);
      break;
    }
  }
}


// Setters:
void XYZRobot::set_Area( Shape *newArea )
{
  this->Area = newArea;
}


void XYZRobot::add_forbidden( Shape *forbidden )
{
  std::cerr << "XYZRobot::addforbidden\n";
  ForbiddenAreas.push_back(forbidden);
}


bool XYZRobot::del_forbidden_at_index( int i )
{
  cerr << "del forbidden at index\n";
  if( i < 0 || i >= (int)ForbiddenAreas.size() )
    return false;
  delete ForbiddenAreas[i];
  ForbiddenAreas.erase( ForbiddenAreas.begin() + i );
  return true;
}


void XYZRobot::clear_forbidden()
{
  for ( Shape *s : ForbiddenAreas ) {
    if (s != NULL)
      delete s;
  }
  ForbiddenAreas.clear();
}


void XYZRobot::set_safe_distance(int dist)
{
  this->maxSafeDist = dist;
}


Point XYZRobot::home( void ) const
{
  return Home;
}

void XYZRobot::setHome( const Point &newhome )
{
  Home = newhome;
}


void XYZRobot::set_fish_head(const Point &head)
{
  this->fish_head = head;
}


void XYZRobot::set_fish_tail(const Point &tail)
{
  this->fish_tail = tail;
  /*
  Cuboid *cb = new Cuboid(this->fish_head, this->fish_tail);
  if (cb != NULL) {
    cb->setName( "Fish" );
    this->add_forbidden( cb );
  }
  */
}

// Getter:
Point XYZRobot::get_fish_head()
{
  return fish_head;
}

Point XYZRobot::get_fish_tail()
{
  return fish_tail;
}

Point XYZRobot::pos( void ) const
{
  return Robot->pos();
}

bool XYZRobot::axis_in_pos_limit( int mirobaxis )
{
  return Robot->check_pos_limit( mirobaxis );
}


bool XYZRobot::axis_in_neg_limit( int mirobaxis )
{
  return Robot->check_neg_limit( mirobaxis );
}


Shape* XYZRobot::area()
{
  return Area;
}


int XYZRobot::how_many_move( const Point &position, const Point &coords )
{
  int count = 0;
  for ( int i=0; i<3; i++ ) {
    if ( abs(position[i] - coords[i]) > 0.5 * Robot->get_step_length(i) )
      count++;
  }
  return count;
}


void  XYZRobot::test_how_many_move()
{
  Point a = Point(0,0,0);
  Point b = Point(0,0,0);
  Point c = Point(10,0,0);
  Point d = Point(10,20,0);
  Point e = Point(10,20,30);
  Point f = Point(50,50,50);

  cerr << "Expected: 0 actual:" << how_many_move(a,b) << endl;
  cerr << "Expected: 1 actual:" << how_many_move(a,c) << endl;
  cerr << "Expected: 1 actual:" << how_many_move(d,e) << endl;
  cerr << "Expected: 1 actual:" << how_many_move(c,d) << endl;
  cerr << "Expected: 2 actual:" << how_many_move(a,d) << endl;
  cerr << "Expected: 2 actual:" << how_many_move(c,e) << endl;
  cerr << "Expected: 3 actual:" << how_many_move(a,e) << endl;
  cerr << "Expected: 3 actual:" << how_many_move(e,f) << endl;
}


double XYZRobot::calc_speed( int axis, double speed, double dist,
			     double maxTime, double precision )
{
  if (dist <= 1)
    return 1;
  
  double time = calculate_intern_time( axis, speed, dist );
  
  int Safetycount = 0;
  int maxCount = speed/precision;

  while( time < maxTime ) {
      speed -= precision;
      time = calculate_intern_time( axis, speed, dist );

      Safetycount++;
      if ( Safetycount > maxCount ) {
	std::cerr << "calc_speed broke for axis:" << axis  << " with maxCount:"
		  << maxCount << std::endl;
	return 1;
      }

    }
  return speed;
}


double XYZRobot::calculate_intern_time( int axis, double axisSpeed, double distance )
{
  double axisAcc = Robot->acceleration() * Robot->get_axis_factor( axis );
  double axisSteps = distance / Robot->get_step_length( axis );
  
  double time = ( (2*axisSpeed/axisAcc +
		   (axisSteps - (axisSpeed*axisSpeed/axisAcc))/axisSpeed) );
  return time;
}


Point XYZRobot::calculate_times( const Point &speeds, const Point &dists )
{
  Point times = Point();
  for ( int k=0; k<3; k++ )
    times[k] = calculate_intern_time( k, speeds[k], dists[k] );
  return times;
}


double XYZRobot::get_max(double a, double b, double c) {
  double max;
  max = (a > b)   ? a : b;
  max = (c > max) ? c : max;
  return max;
}


void XYZRobot::powerAxes( bool on ) {
  if ( on && !Robot->checkPowerState() ) {
    Robot->power_on();
    return;
  }
  if ( !on && Robot->checkPowerState() ) {
    Robot->power_off();
  }
}

}; /* namespace misc */

