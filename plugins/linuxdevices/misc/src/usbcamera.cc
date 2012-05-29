/*
  misc/usbcamera.cc
  The Usbcamera module linear robot from MPH

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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
#include <relacs/misc/usbcamera.h>
using namespace std;
using namespace relacs;

namespace misc {


USBCamera::USBCamera( const string &device )
  : Camera( "USBCamera" )
{
  Opened = false;

  Options opts;
  open( device, opts );
}


USBCamera::USBCamera( void )
  : Camera( "USBCamera" )
{
  Opened = true;
  fprintf(stderr,"USB CAMERA CONTRUCTOR!\n");
}


USBCamera::~USBCamera( void )
{

}

int USBCamera::open( const string &device, const Options &opts )
{
  Opened = true;

  return 0;
}

void USBCamera::close( void )
{
  Opened = false;
  // Info.clear();
  // Settings.clear();

  cerr << "CAMERA closed " << endl;
}


int USBCamera::reset( void )
{
  cerr << "CAMERA resetted " << endl;
  return 0;
}


}; /* namespace misc */
