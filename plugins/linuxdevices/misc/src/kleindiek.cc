/*
  misc/kleindiek.cc
  The Kleindiek nanotechnik MM3A micromanipulator  

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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cmath>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <relacs/misc/kleindiek.h>
using namespace std;
using namespace relacs;

namespace misc {


Kleindiek::Kleindiek( const string &device )
  : Manipulator( "Kleindiek" )
{
  construct();
  open( device );
}


Kleindiek::Kleindiek( void )
  : Manipulator( "Kleindiek" )
{
  construct();
}


Kleindiek::~Kleindiek( void )
{
  close();
}


void Kleindiek::construct( void )
{
  Handle = -1;
  clear();
  for ( int k=0; k<3; k++ ) {
    PosAmplitude[k] = 80;
    NegAmplitude[k] = 80;
    PosGain[k] = 1.0/80.0;
    NegGain[k] = 1.0/80.0;
  }
}


int Kleindiek::open( const string &device )
{
  clearError();
  if ( Handle >= 0 )
    return 0;

  Info.clear();
  Settings.clear();

  // open device:
  Handle = ::open( device.c_str(), O_RDWR | O_NOCTTY );
  if ( Handle < 0 ) {
    setErrorStr( errno );
    return InvalidDevice;
  }

  // save current Kleindiek port settings:
  tcgetattr( Handle, &OldTIO );

  // clear struct for new port settings:
  memset( &NewTIO, 0, sizeof( NewTIO ) );

  /* 
     BAUDRATE: Set bps rate. You could also use cfsetispeed and cfsetospeed.
     CS8     : 8n1 (8bit,no parity,1 stopbit)
     CLOCAL  : local connection, no modem contol
     CREAD   : enable receiving characters
  */
  NewTIO.c_cflag = B19200 | CS8 | CLOCAL | CREAD;
  NewTIO.c_iflag = IGNPAR | IGNBRK;
  NewTIO.c_oflag = 0;
  NewTIO.c_lflag = 0;
  NewTIO.c_cc[VTIME] = 2;  // inter-character timer unused (decisecond)
  NewTIO.c_cc[VMIN] = 0;   // blocking read until n character arrives
  
  // clean the modem line and activate the settings for the port:
  tcflush( Handle, TCIFLUSH );
  tcsetattr( Handle, TCSANOW, &NewTIO );

  setDeviceName( "MM3A" );
  setDeviceVendor( "Kleindiek Nanotechnik GmbH" );
  setDeviceFile( device );
  Device::addInfo();

  return 0;
}


void Kleindiek::close( void )
{
  if ( Handle >= 0 )
    tcsetattr( Handle, TCSANOW, &OldTIO );
  Handle = -1;
  clear();
  Info.clear();
  Settings.clear();
}


int Kleindiek::reset( void )
{
  tcflush( Handle, TCIFLUSH );
  return 0;
}


int Kleindiek::doStepBy( int axis, int steps, double speed, double acc )
{
  tcflush( Handle, TCIFLUSH );

  char com[200];
  sprintf( com, "coarse %c %+d;", 'A' + axis, steps );
  write( Handle, com, strlen( com ) );

  /*
  usleep( 100000 );
  char buf[100];
  int n = read( Handle, buf, 100 );
  buf[n] = '\0';
  cout << buf << endl;
  */

  return 0;
}


int Kleindiek::clearX( void )
{
  CurrentPos[0] = 0.0;
  CurrentSteps[0] = 0;
  return 0;
}


int Kleindiek::clearY( void )
{
  CurrentPos[1] = 0.0;
  CurrentSteps[1] = 0;
  return 0;
}


int Kleindiek::clearZ( void )
{
  CurrentPos[2] = 0.0;
  CurrentSteps[2] = 0;
  return 0;
}


int Kleindiek::clear( void )
{
  clearX();
  clearY();
  clearZ();
  return 0;
}


int Kleindiek::homeX( void )
{
  double dist = -CurrentPos[0];
  int steps = 0;
  if ( dist > 0.0 )
    steps = (int)::round( dist / PosAmplitude[0] / PosGain[0] );
  else
    steps = (int)::round( dist / NegAmplitude[0] / NegGain[0] );
  return stepByX( steps );
}


int Kleindiek::homeY( void )
{
  double dist = -CurrentPos[1];
  int steps = 0;
  if ( dist > 0.0 )
    steps = (int)::round( dist / PosAmplitude[1] / PosGain[1] );
  else
    steps = (int)::round( dist / NegAmplitude[1] / NegGain[1] );
  return stepByY( steps );
}


int Kleindiek::homeZ( void )
{
  double dist = -CurrentPos[2];
  int steps = 0;
  if ( dist > 0.0 )
    steps = (int)::round( dist / PosAmplitude[2] / PosGain[2] );
  else
    steps = (int)::round( dist / NegAmplitude[2] / NegGain[2] );
  return stepByZ( steps );
}


int Kleindiek::home( void )
{
  return homeX() + ( homeY() << 1 ) + ( homeZ() << 2 );
}


int Kleindiek::setStepAmplX( double posampl, double negampl )
{
  int pa = int( rint( posampl ) );
  int na = negampl < 0.0 ? pa : int( rint( negampl ) );

  if ( pa >= 1 && pa <= 80 && na >= 1 && na <= 80 ) {
    amplitudepos( 0, pa );
    amplitudeneg( 0, na );
    return 0;
  }
  else
    return 1;
}


int Kleindiek::setStepAmplY( double posampl, double negampl )
{
  int pa = int( rint( posampl ) );
  int na = negampl < 0.0 ? pa : int( rint( negampl ) );

  if ( pa >= 1 && pa <= 80 && na >= 1 && na <= 80 ) {
    amplitudepos( 1, pa );
    amplitudeneg( 1, na );
    return 0;
  }
  else
    return 2;
}


int Kleindiek::setStepAmplZ( double posampl, double negampl )
{
  int pa = int( rint( posampl ) );
  int na = negampl < 0.0 ? pa : int( rint( negampl ) );

  if ( pa >= 1 && pa <= 80 && na >= 1 && na <= 80 ) {
    amplitudepos( 2, pa );
    amplitudeneg( 2, na );
    return 0;
  }
  else
    return 4;
}


double Kleindiek::minAmplX( void ) const
{
  return 1.0;
}


double Kleindiek::maxAmplX( void ) const
{
  return 80.0;
}


int Kleindiek::pause( int ms )
{
  tcflush( Handle, TCIFLUSH );

  char com[200];
  sprintf( com, "pause %d;", ms );
  write( Handle, com, strlen( com ) );

  usleep( 100000 );
  char buf[100];
  int n = ::read( Handle, buf, 100 );
  buf[n] = '\0';
  cout << buf << endl;

  return 0;
}


int Kleindiek::speed( int channel, int speed )
{
  tcflush( Handle, TCIFLUSH );

  char com[200];
  sprintf( com, "speed %c %d;", 'A' + channel, speed );
  write( Handle, com, strlen( com ) );

  usleep( 100000 );
  char buf[100];
  int n = ::read( Handle, buf, 100 );
  buf[n] = '\0';
  cout << buf << endl;

  return 0;
}


int Kleindiek::amplitudepos( int channel, int ampl )
{
  tcflush( Handle, TCIFLUSH );

  char com[200];
  sprintf( com, "amplitudepos %c %d;", 'A' + channel, ampl );
  write( Handle, com, strlen( com ) );

  char buf[100];
  int n = ::read( Handle, buf, 100 );
  buf[n] = '\0';

  PosAmplitude[channel] = ampl;

  PosAmpl[channel] = PosAmplitude[channel]*PosGain[channel];

  return 0;
}


int Kleindiek::amplitudeneg( int channel, int ampl )
{
  tcflush( Handle, TCIFLUSH );

  char com[200];
  sprintf( com, "amplitudeneg %c %d;", 'A' + channel, ampl );
  write( Handle, com, strlen( com ) );

  char buf[100];
  int n = ::read( Handle, buf, 100 );
  buf[n] = '\0';

  NegAmplitude[channel] = ampl;

  PosAmpl[channel] = NegAmplitude[channel]*NegGain[channel];

  return 0;
}


int Kleindiek::countermode( int channel, long mode )
{
  tcflush( Handle, TCIFLUSH );

  char com[50];
  sprintf( com, "countermode %c %ld;", 'A' + channel, mode );
  write( Handle, com, strlen( com ) );

  usleep( 100000 );
  char buf[100];
  int n = ::read( Handle, buf, 100 );
  buf[n] = '\0';
  cout << buf << endl;

  return 0;
}


int Kleindiek::counterread( void )
{
  tcflush( Handle, TCIFLUSH );

  char com[30];
  strcpy( com, "counterread;" );
  write( Handle, com, strlen( com ) );

  usleep( 100000 );
  char buf[100];
  int n = ::read( Handle, buf, 100 );
  buf[n] = '\0';
  cout << buf << endl;

  return 0;
}


int Kleindiek::counterreset( void )
{
  tcflush( Handle, TCIFLUSH );

  char com[30];
  strcpy( com, "counterreset;" );
  write( Handle, com, strlen( com ) );

  usleep( 100000 );
  char buf[100];
  int n = ::read( Handle, buf, 100 );
  buf[n] = '\0';
  cout << buf << endl;

  return 0;
}


ostream &operator<<( ostream &str, const Kleindiek &k )
{
  if ( k.Handle < 0 ) {
    str << "Kleindiek not opened!\n";
  }
  else {

    char com1[9] = "version;";
    write( k.Handle, com1, 8 );
    usleep( 100000 );
    char buf1[100];
    read( k.Handle, buf1, 100 );
    str << buf1 << endl;

    char com2[13] = "configprint;";
    write( k.Handle, com2, 12 );
    usleep( 3000000 );
    char buf2[10000];
    read( k.Handle, buf2, 10000 );
    str << buf2 << endl;
  }

  return str;
}


}; /* namespace misc */
