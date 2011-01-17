/*
  device.cc
  Virtual class for accessing a device. 

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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

#include <sstream>
#include <relacs/device.h>
using namespace std;

namespace relacs {


const string Device::DeviceTypeStrs[Device::MaxDeviceTypeStrs] =
  { "Miscellaneous", "Analog Input", "Analog Output", "Digital I/O", "Attenuator",
    "Attenuate Interface", "Manipulator", "Temperature" };


Device::Device( int type )
  : DeviceType( type ),
    DeviceClass ( "" ),
    DeviceIdent( "" ),
    DeviceFile( "" ),
    DeviceName( "" ),
    DeviceVendor( "" )
{
}


Device::Device( const string &deviceclass, int type )
  : DeviceType( type ),
    DeviceClass ( deviceclass ),
    DeviceIdent( "" ),
    DeviceFile( "" ),
    DeviceName( "" ),
    DeviceVendor( "" )
{
}


Device::~Device( void )
{
}


int Device::open( const string &device, const Options &opts )
{
  setDeviceFile( device );
  return InvalidDevice;
}


int Device::open( Device &device, const Options &opts )
{
  setDeviceFile( device.deviceIdent() );
  return InvalidDevice;
}


int Device::reset( void )
{
  return 0;
}


const Options &Device::info( void ) const
{
  return Info;
}


void Device::addInfo( void )
{
  Info.addInteger( "type", deviceType() );
  Info.addText( "class", deviceClass() );
  Info.addText( "ident", deviceIdent() );
  if ( ! deviceName().empty() )
    Info.addText( "name", deviceName() );
  if ( ! deviceVendor().empty() )
  Info.addText( "vendor", deviceVendor() );
  Info.addText( "file", deviceFile() );
  Info.addText( "status", isOpen() ? "open" : "not open" );
}


const Options &Device::settings( void ) const
{
  return Settings;
}


int Device::deviceType( void ) const
{
  return DeviceType;
}


string Device::deviceTypeStr( void ) const
{
  return deviceTypeStr( DeviceType );
}


void Device::setDeviceType( int type )
{
  DeviceType = type;
}


int Device::deviceTypes( void )
{
  return MaxDeviceTypeStrs;
}


string Device::deviceTypeStr( int type )
{
  if ( type >=0 && type < MaxDeviceTypeStrs )
    return DeviceTypeStrs[ type ];
  else
    return "Unknown";
}


string Device::deviceClass( void ) const
{
  return DeviceClass;
}


void Device::setDeviceClass( const string &deviceclass )
{
  DeviceClass = deviceclass;
}


string Device::deviceFile( void ) const
{
  return DeviceFile;
}


void Device::setDeviceFile( const string &devicefile )
{
  DeviceFile = devicefile;
}


string Device::deviceIdent( void ) const
{
  return DeviceIdent;
}


void Device::setDeviceIdent( const string &ident )
{
  DeviceIdent = ident;
  Info.setText( "ident", DeviceIdent );
}


string Device::deviceName( void ) const
{
  return DeviceName;
}


void Device::setDeviceName( const string &devicename )
{
  DeviceName = devicename;
}


string Device::deviceVendor( void ) const
{
  return DeviceVendor;
}


void Device::setDeviceVendor( const string &devicevendor )
{
  DeviceVendor = devicevendor;
}


ostream &operator<<( ostream &str, const Device &d )
{
  str << d.info();
  return str;
}


}; /* namespace relacs */

