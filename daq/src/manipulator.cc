/*
  manipulator.cc
  Virtual class for controlling a Manipulator. 

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

#include <cmath>
#include <relacs/manipulator.h>

namespace relacs {


Manipulator::Manipulator( void ) 
  : Device( ManipulatorType )
{
  for ( int k=0; k<3; k++ ) {
    PosAmpl[k] = 1.0;
    NegAmpl[k] = 1.0;
    SpeedFac[k] = 1.0;
    AccFac[k] = 1.0;
    DefaultSpeed[k] = 1;
    DefaultAcc[k] = 1;
  }
}


Manipulator::Manipulator( const string &deviceclass )
  : Device( deviceclass, ManipulatorType )
{
  for ( int k=0; k<3; k++ ) {
    PosAmpl[k] = 1.0;
    NegAmpl[k] = 1.0;
    SpeedFac[k] = 1.0;
    AccFac[k] = 1.0;
    DefaultSpeed[k] = 1;
    DefaultAcc[k] = 1;
  }
}


Manipulator::~Manipulator( void )
{
}


int Manipulator::stepTo( int axis, int pos, int speed, int acc )
{
  return 0;
}


int Manipulator::stepTo( const Point &pos, const Point &speed, const Point &acc )
{
  int r = 0;
  for ( int k=0; k<3; k++ )
    r |= stepTo( k, pos[k], speed[k], acc[k] );
  return r;
}


int Manipulator::stepBy( int axis, int steps, int speed, int acc )
{
  return 0;
}


int Manipulator::stepBy( const Point &dist, const Point &speed, const Point &acc )
{
  int r = 0;
  for ( int k=0; k<3; k++ )
    r |= stepBy( k, dist[k], speed[k], acc[k] );
  return r;
}


int Manipulator::moveBy( int axis, double dist, double speed, double acc )
{
  int d = (int) ::round( dist/(dist>=0.0?PosAmpl[axis]:NegAmpl[axis]));
  int s = (int) ::round( speed/SpeedFac[axis]);
  int a = (int) ::round( acc/AccFac[axis]);
  return stepBy( axis, d, s, a );
}


int Manipulator::moveBy( const Point &dist, const Point &speed, const Point &acc )
{
  int r = 0;
  for ( int k=0; k<3; k++ )
    r |= moveBy( k, dist[k], speed[k], acc[k] );
  return r;
}


int Manipulator::moveTo( int axis, double pos, double speed, double acc )
{
  // XXX positive or negative depends on current position of axis!
  int p = (int) ::round( pos/(pos>=0.0?PosAmpl[axis]:NegAmpl[axis]));
  int s = (int) ::round( speed/SpeedFac[axis]);
  int a = (int) ::round( acc/AccFac[axis]);
  return stepTo( axis, p, s, a );
}


int Manipulator::moveTo( const Point &pos, const Point &speed, const Point &acc )
{
  int r = 0;
  for ( int k=0; k<3; k++ )
    r |= moveTo( k, pos[k], speed[k], acc[k] );
  return r;
}


int Manipulator::stop( int axis )
{
  return 0;
}


int Manipulator::stop( void )
{
  int r = 0;
  for ( int k=0; k<3; k++ )
    r |= stop( k );
  return r;
}


double Manipulator::posX( void ) const
{
  return pos( 0 );
}


double Manipulator::posY( void ) const
{
  return pos( 1 );
}


double Manipulator::posZ( void ) const
{
  return pos( 2 );
}


double Manipulator::pos( int axis ) const
{
  return 0.0;
}


Point Manipulator::pos( void ) const
{
  return Point( 0.0, 0.0, 0.0 );
}


int Manipulator::wait( void ) const
{
  return 0;
}


int Manipulator::clearX( void )
{
  return 0;
}


int Manipulator::clearY( void )
{
  return 0;
}


int Manipulator::clearZ( void )
{
  return 0;
}


int Manipulator::clear( void )
{
  return clearX() + ( clearY() << 1 ) + ( clearZ() << 2 );
}


int Manipulator::homeX( void )
{
  return 0;
}


int Manipulator::homeY( void )
{
  return 0;
}


int Manipulator::homeZ( void )
{
  return 0;
}


int Manipulator::home( void )
{
  return homeX() + ( homeY() << 1 ) + ( homeZ() << 2 );
}


int Manipulator::setAmplX( double posampl, double negampl )
{
  return 0;
}


int Manipulator::setAmplY( double posampl, double negampl )
{
  return 0;
}


int Manipulator::setAmplZ( double posampl, double negampl )
{
  return 0;
}


double Manipulator::minAmplX( void ) const
{
  return 0.0;
}


double Manipulator::maxAmplX( void ) const
{
  return 0.0;
}


double Manipulator::minAmplY( void ) const
{
  return minAmplX();
}


double Manipulator::maxAmplY( void ) const
{
  return maxAmplX();
}


double Manipulator::minAmplZ( void ) const
{
  return minAmplX();
}


double Manipulator::maxAmplZ( void ) const
{
  return maxAmplX();
}


}; /* namespace relacs */

