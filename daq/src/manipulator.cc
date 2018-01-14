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
  PosAmpl = Point( 1.0, 1.0, 1.0 );
  NegAmpl = Point( 1.0, 1.0, 1.0 );
  SpeedFac = Point( 1.0, 1.0, 1.0 );
  AccFac = Point( 1.0, 1.0, 1.0 );
  DefaultSpeed = Point( 1.0, 1.0, 1.0 );
  DefaultAcc = Point( 1.0, 1.0, 1.0 );
  CurrentSteps = Point( 0.0, 0.0, 0.0 );
  CurrentPos = Point( 0.0, 0.0, 0.0 );
  MoveMode = PathOutside;
  ForbiddenZones.clear();
}


Manipulator::Manipulator( const string &deviceclass )
  : Device( deviceclass, ManipulatorType )
{
  PosAmpl = Point( 1.0, 1.0, 1.0 );
  NegAmpl = Point( 1.0, 1.0, 1.0 );
  SpeedFac = Point( 1.0, 1.0, 1.0 );
  AccFac = Point( 1.0, 1.0, 1.0 );
  DefaultSpeed = Point( 1.0, 1.0, 1.0 );
  DefaultAcc = Point( 1.0, 1.0, 1.0 );
  CurrentSteps = Point( 0.0, 0.0, 0.0 );
  CurrentPos = Point( 0.0, 0.0, 0.0 );
}


Manipulator::~Manipulator( void )
{
}


int Manipulator::stepBy( int axis, int steps, double speed, double acc )
{
  if ( speed <= 0 )
    speed = DefaultSpeed[axis];
  if ( acc <= 0 )
    acc = DefaultAcc[axis];
  if ( MoveMode <= PathOutside ) {
    if ( MoveMode >= TargetOutside ) {
      Point p = CurrentPos;
      p[axis] += steps*(steps>=0.0?PosAmpl[axis]:NegAmpl[axis]);
      if ( MoveMode == TargetOutside ) {
	if ( forbidden( p ) )
	  return WriteError;
      }
      else if ( MoveMode == PathOutside ) {
	if ( intersect( p, IntersectResolution ) )
	  return WriteError;
      }
    }
    int r = doStepBy( axis, steps, speed, acc );
    if ( r == 0 ) {
      CurrentSteps[axis] += steps;
      CurrentPos[axis] += steps*(steps>=0.0?PosAmpl[axis]:NegAmpl[axis]);
    }
    return r;
  }
  else
    return NotSupported;
  // TODO: implement obstacle avoidance if absolute coordinates are supported!
}


int Manipulator::stepTo( int axis, int pos, double speed, double acc )
{
  if ( PosAmpl[axis] != NegAmpl[axis] )
    return NotSupported;
  if ( speed <= 0 )
    speed = DefaultSpeed[axis];
  if ( acc <= 0 )
    acc = DefaultAcc[axis];
  if ( MoveMode <= PathOutside ) {
    if ( MoveMode >= TargetOutside ) {
      Point p = CurrentPos;
      p[axis] = pos*PosAmpl[axis];
      if ( MoveMode == TargetOutside ) {
	if ( forbidden( p ) )
	  return WriteError;
      }
      else if ( MoveMode == PathOutside ) {
	if ( intersect( p, IntersectResolution ) )
	  return WriteError;
      }
    }
    int r = doStepTo( axis, pos, speed, acc );
    if ( r == 0 ) {
      CurrentSteps[axis] = pos;
      CurrentPos[axis] = pos*PosAmpl[axis];
    }
    return r;
  }
  else
    return NotSupported;
  // TODO: implement obstacle avoidance if absolute coordinates are supported!
}


int Manipulator::moveBy( int axis, double dist, double speed, double acc )
{
  int d = (int) ::round( dist/(dist>=0.0?PosAmpl[axis]:NegAmpl[axis]));
  double s = speed/SpeedFac[axis];
  double a = acc/AccFac[axis];
  return stepBy( axis, d, s, a );
}


int Manipulator::moveTo( int axis, double pos, double speed, double acc )
{
  if ( PosAmpl[axis] != NegAmpl[axis] )
    return NotSupported;
  int p = (int) ::round( pos/PosAmpl[axis] );
  double s = speed/SpeedFac[axis];
  double a = acc/AccFac[axis];
  return stepTo( axis, p, s, a );
}


int Manipulator::moveBy( const Point &dist, double speed, double acc )
{
  if ( MoveMode <= PathOutside ) {
    if ( MoveMode == TargetOutside ) {
      if ( forbidden( CurrentPos + dist ) )
	return WriteError;
    }
    else if ( MoveMode == PathOutside ) {
      if ( intersect( CurrentPos + dist, IntersectResolution ) )
	return WriteError;
    }
    int r = doMoveBy( dist, speed, acc );
    if ( r == 0 ) {
      for ( int k=0; k<3; k++ )
	CurrentSteps[k] += dist[k]/(dist[k]>0.0?PosAmpl[k]:NegAmpl[k]);
      CurrentPos += dist;
    }
    return r;
  }
  else
    return NotSupported;
  // TODO: implement obstacle avoidance if absolute coordinates are supported!
}


int Manipulator::moveTo( const Point &pos, double speed, double acc )
{
  if ( PosAmpl != NegAmpl )
    return NotSupported;
  if ( MoveMode <= PathOutside ) {
    if ( MoveMode == TargetOutside ) {
      if ( forbidden( pos ) )
	return WriteError;
    }
    else if ( MoveMode == PathOutside ) {
      if ( intersect( pos, IntersectResolution ) )
	return WriteError;
    }
    int r = doMoveTo( pos, speed, acc );
    if ( r == 0 ) {
      CurrentSteps = pos/PosAmpl;
      CurrentPos = pos;
    }
    return r;
  }
  else
    return NotSupported;
  // TODO: implement obstacle avoidance if absolute coordinates are supported!
}


int Manipulator::stop( void )
{
  int r = 0;
  for ( int k=0; k<3; k++ ) {
    int rr = stop( k );
    if ( r == 0 && r != 0 )
      r = rr;
  }
  return r;
}


int Manipulator::wait( void ) const
{
  return NotSupported;
}


int Manipulator::setHome( int axis )
{
  CurrentSteps[axis] = 0.0;
  CurrentPos[axis] = 0.0;
  return 0;
}


int Manipulator::setHome( void )
{
  int r = 0;
  for ( int k=0; k<3; k++ ) {
    int rr = setHome( k );
    if ( r == 0 && r != 0 )
      r = rr;
  }
  return r;
}


int Manipulator::setStepAmpl( int axis, double posampl, double negampl )
{
  PosAmpl[axis] = posampl;
  NegAmpl[axis] = negampl >= 0.0 ? negampl : posampl;
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

