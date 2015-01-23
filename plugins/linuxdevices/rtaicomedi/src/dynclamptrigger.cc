/*
  rtaicomedi/dynclamptrigger.cc
  Interface for setting up an analog trigger in the dynamic clamp kernel module.

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
#include <sstream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <ctime>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <comedilib.h>
#include <relacs/rtaicomedi/moduledef.h>
#include <relacs/rtaicomedi/dynclamptrigger.h>
using namespace std;
using namespace relacs;

namespace rtaicomedi {


DynClampTrigger::DynClampTrigger( void ) 
  : Trigger( "DynClampTrigger" ),
    AIDevice( "" ),
    AIChannel( -1 )
{
}


DynClampTrigger::DynClampTrigger( const string &device, const Options &opts ) 
  : Trigger( "DynClampTrigger" ),
    AIDevice( "" ),
    AIChannel( -1 )
{
  open( device, opts );
}


DynClampTrigger::~DynClampTrigger( void ) 
{
  close();
}


int DynClampTrigger::open( const string &device, const Options &opts )
{ 
  clearError();
  Info.clear();
  Settings.clear();

  if ( device.empty() )
    return InvalidDevice;
  setDeviceFile( device );

  // set basic device infos:
  setDeviceName( "RT Analog Trigger" );
  setDeviceVendor( "Jan Benda" );
  setDeviceFile( device );

  // open kernel module:
  ModuleDevice = device;
  ModuleFd = ::open( ModuleDevice.c_str(), O_RDONLY );
  if ( ModuleFd == -1 ) {
    setErrorStr( "opening dynclamp-module " + ModuleDevice + " failed" );
    return -1;
  }

  AIDevice = opts.text( "aidevice" );
  AIChannel = opts.integer( "aichannel" );

  if ( set( opts ) > 0 )
    activate();

  return 0;
}


bool DynClampTrigger::isOpen( void ) const 
{ 
  return ( ModuleFd >= 0 );
}


void DynClampTrigger::close( void )
{ 
  clearError();
  if ( ! isOpen() )
    return;

  reset();

  if ( ::close( ModuleFd ) < 0 )
    setErrorStr( "closing of module file failed" );

  ModuleFd = -1;

  Info.clear();
  Settings.clear();
}


int DynClampTrigger::activate( void )
{ 
  if ( ! isOpen() )
    return -1;

  struct triggerIOCT triggerIOC;
  strcpy( triggerIOC.devname, AIDevice.c_str() );
  triggerIOC.subdev = -1;
  triggerIOC.channel = AIChannel;
  triggerIOC.alevel = Hoop[0].ALevel;

  int retval = ::ioctl( ModuleFd, IOC_SET_TRIGGER, &triggerIOC );
  if ( retval < 0 ) {
    cerr << " DynClampTrigger::activate -> ioctl command IOC_SET_TRIGGER on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }

  setSettings();
  return 0;
}


int DynClampTrigger::disable( void )
{ 
  if ( ! isOpen() )
    return -1;

  struct triggerIOCT triggerIOC;
  strcpy( triggerIOC.devname, AIDevice.c_str() );
  triggerIOC.subdev = -1;
  triggerIOC.channel = AIChannel;
  triggerIOC.alevel = 0.0;

  int retval = ::ioctl( ModuleFd, IOC_UNSET_TRIGGER, &triggerIOC );
  if ( retval < 0 ) {
    cerr << " DynClampTrigger::disable -> ioctl command IOC_UNSET_TRIGGER on device "
	 << ModuleDevice << " failed!\n";
    return -1;
  }

  Settings.clear();
  return 0;
}


}; /* namespace rtaicomedi */
