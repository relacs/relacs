/*
  misc/mirob.cc
  The Mirob module linear robot from MPH

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <relacs/misc/mirob.h>
using namespace std;
using namespace relacs;

namespace misc {

const string Mirob::SetupFile = "mirob.t.zip";


Mirob::Mirob( const string &device )
  : Manipulator( "Mirob" )
{
  Opened = false;
  for ( int k=0; k<3; k++ ) {
    Speed[k] = 100.0;
    Acceleration[k] = 0.6;
  }
  Options opts;
  open( device, opts );
}


Mirob::Mirob( void )
  : Manipulator( "Mirob" )
{
  for ( int k=0; k<3; k++ ) {
    Speed[k] = 100.0;
    Acceleration[k] = 0.6;
  }
  Opened = false;
}


Mirob::~Mirob( void )
{
  close();
}


int Mirob::open( const string &device, const Options &opts )
{
  cerr << "MIROB open " << device << '\n';
  if ( Opened )
    return 0;

  Info.clear();
  Settings.clear();

  // open device:
  if ( TS_OpenChannel( device.c_str(), ChannelType, HostID, Baudrate ) < 0 ) {
    cerr << "Communication error! " << TS_GetLastErrorText() << '\n';
    return InvalidDevice;
  }

  // load setup file:
  int setupindex = TS_LoadSetup( SetupFile.c_str() );
  if ( setupindex < 0 ) {
    cerr << "Failed to load setup file! " << TS_GetLastErrorText() << '\n';
    return 1;
  }

  // setup axis:
  for ( int k=1; k<=3; k++ ) {
    if ( ! TS_SetupAxis( k, setupindex ) )  {
      cerr << "Failed to setup axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    if ( ! TS_SelectAxis( k ) ) {
      cerr << "Failed to select axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    if ( ! TS_DriveInitialisation() ) {
      cerr << "Failed to initialize drive for axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    if ( ! TS_Power( POWER_ON ) ) {
      cerr << "Failed to power on drive for axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    WORD axison = 0;
    while ( axison == 0 ) {
      // Check the status of the power stage:
      if ( ! TS_ReadStatus( REG_SRL, axison ) ) {
	cerr << "Failed toread status for axis " << k << "! " << TS_GetLastErrorText() << '\n';
	return 1;
      }
      axison = ((axison & 1<<15) != 0 ? 1 : 0);
    }
  }

  setDeviceName( "Mirob" );
  setDeviceVendor( "MPH" );
  setDeviceFile( device );
  Device::addInfo();

  Opened = true;

  return 0;
}


void Mirob::close( void )
{
  if ( Opened ) {
    for ( int k=1; k<=3; k++ ) {
      if ( ! TS_SelectAxis( k ) )
	cerr << "Failed to select axis " << k << "! " << TS_GetLastErrorText() << '\n';
      if ( ! TS_Power( POWER_OFF ) )
	cerr << "Failed to power off drive for axis " << k << "! " << TS_GetLastErrorText() << '\n';
    }
    TS_CloseChannel( -1 );
  }
  Opened = false;
  Info.clear();
  Settings.clear();
}


int Mirob::reset( void )
{
  for ( int k=1; k<=3; k++ ) {
    if ( ! TS_SelectAxis( k ) ) {
      cerr << "Failed to select axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    if ( ! TS_ResetFault() ) {
      cerr << "Failed to reset fault axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    if ( ! TS_Power( POWER_ON ) ) {
      cerr << "Failed to power on drive for axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
    WORD axison = 0;
    while ( axison == 0 ) {
      // Check the status of the power stage:
      if ( ! TS_ReadStatus( REG_SRL, axison ) ) {
	cerr << "Failed toread status for axis " << k << "! " << TS_GetLastErrorText() << '\n';
	return 1;
      }
      axison = ((axison & 1<<15) != 0 ? 1 : 0);
    }
    /*
    if ( ! TS_Reset() ) {
      cerr << "Failed to reset axis " << k << "! " << TS_GetLastErrorText() << '\n';
      return 1;
    }
   // Requires much more setup afterwards!
    */
  }

  return 0;
}

                             
int Mirob::step( double x, int axis )
{
  cerr << "AXIS " << axis << " step by " << x << '\n';
  // select axis:
  if ( ! TS_SelectAxis( axis ) ) {
    cerr << "Failed to select axis " << axis << "! " << TS_GetLastErrorText() << '\n';
    return 1;
  }

  /*
  // set the actual motor position to 0 [position internal units]:
  if ( ! TS_SetPosition( 0 ) ) {
    cerr << "Failed to set position on axis " << axis << "! " << TS_GetLastErrorText() << '\n';
    return 1;
  }
  */

  // move:
  long step = long( rint( x ) );
  if ( ! TS_MoveRelative( step, Speed[axis], Acceleration[axis],
			  false, UPDATE_IMMEDIATE, FROM_REFERENCE ) ) {
    cerr << "Failed to move on axis " << axis << "! " << TS_GetLastErrorText() << '\n';
    return 1;
  }

  // wait:
  if( ! TS_SetEventOnMotionComplete( true, false ) ) { 
    cerr << "Failed to wait on axis " << axis << "! " << TS_GetLastErrorText() << '\n';
    return 1;
  }

  return 0;
}


int Mirob::stepX( double x )
{
  return step( x, 1 );
}


int Mirob::stepY( double y )
{
  return step( y, 2 );
}


int Mirob::stepZ( double z )
{
  return step( z, 3 );
}


double Mirob::pos( int axis ) const
{
  // select axis:
  if ( ! TS_SelectAxis( axis ) ) {
    cerr << "Failed to select axis " << axis << "! " << TS_GetLastErrorText() << '\n';
    return 1;
  }

  // read position:
  long apos = -1;
  if ( ! TS_GetLongVariable( "APOS", apos) ) {
    cerr << "Failed to read position of axis " << axis << "! " << TS_GetLastErrorText() << '\n';
    return 0.0;
  }

  return (double)(apos);
}


double Mirob::posX( void ) const
{
  return pos( 1 );
}


double Mirob::posY( void ) const
{
  return pos( 2 );
}


double Mirob::posZ( void ) const
{
  return pos( 3 );
}


int Mirob::clearX( void )
{
  return 0;
}


int Mirob::clearY( void )
{
  return 0;
}


int Mirob::clearZ( void )
{
  return 0;
}


int Mirob::clear( void )
{
  clearX();
  clearY();
  clearZ();
  return 0;
}


int Mirob::homeX( void )
{
  /*
  double dist = -Pos[0];
  double steps = 0.0;
  if ( dist > 0.0 )
    steps = dist / PosAmplitude[0] / PosGain[0];
  else
    steps = dist / NegAmplitude[0] / NegGain[0];
  return stepX( steps );
  */
  return 0;
}


int Mirob::homeY( void )
{
  /*
  double dist = -Pos[1];
  double steps = 0.0;
  if ( dist > 0.0 )
    steps = dist / PosAmplitude[1] / PosGain[1];
  else
    steps = dist / NegAmplitude[1] / NegGain[1];
  return stepY( steps );
  */
  return 0;
}


int Mirob::homeZ( void )
{
  /*
  double dist = -Pos[2];
  double steps = 0.0;
  if ( dist > 0.0 )
    steps = dist / PosAmplitude[2] / PosGain[2];
  else
    steps = dist / NegAmplitude[2] / NegGain[2];
  return stepZ( steps );
  */
  return 0;
}


int Mirob::home( void )
{
  return homeX() + ( homeY() << 1 ) + ( homeZ() << 2 );
}


int Mirob::setAmplX( double posampl, double negampl )
{
  /*
  int pa = int( rint( posampl ) );
  int na = negampl < 0.0 ? pa : int( rint( negampl ) );

  if ( pa >= 1 && pa <= 80 && na >= 1 && na <= 80 ) {
    amplitudepos( 0, pa );
    amplitudeneg( 0, na );
    return 0;
  }
  else
    return 1;
  */
  return 0;
}


int Mirob::setAmplY( double posampl, double negampl )
{
  /*
  int pa = int( rint( posampl ) );
  int na = negampl < 0.0 ? pa : int( rint( negampl ) );

  if ( pa >= 1 && pa <= 80 && na >= 1 && na <= 80 ) {
    amplitudepos( 1, pa );
    amplitudeneg( 1, na );
    return 0;
  }
  else
    return 2;
  */
  return 0;
}


int Mirob::setAmplZ( double posampl, double negampl )
{
  /*
  int pa = int( rint( posampl ) );
  int na = negampl < 0.0 ? pa : int( rint( negampl ) );

  if ( pa >= 1 && pa <= 80 && na >= 1 && na <= 80 ) {
    amplitudepos( 2, pa );
    amplitudeneg( 2, na );
    return 0;
  }
  else
    return 4;
  */
  return 0;
}


double Mirob::minAmplX( void ) const
{
  return 1.0;
}


double Mirob::maxAmplX( void ) const
{
  return 80.0;
}


}; /* namespace misc */
