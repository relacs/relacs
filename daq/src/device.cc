/*
  device.cc
  Virtual class for accessing a device. 

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

#include <sstream>
#include <relacs/device.h>
using namespace std;

namespace relacs {


Device::Device( int type )
  : DeviceType( type ),
    DeviceClass ( "" ),
    DeviceIdent( "" ),
    DeviceFile( "" ),
    DeviceName( "" ),
    DeviceVendor( "" ),
    Settings( "" )
{
}


Device::Device( const string &deviceclass, int type )
  : DeviceType( type ),
    DeviceClass ( deviceclass ),
    DeviceIdent( "" ),
    DeviceFile( "" ),
    DeviceName( "" ),
    DeviceVendor( "" ),
    Settings( "" )
{
}


Device::~Device( void )
{
}


int Device::open( const string &device, long mode )
{
  setDeviceFile( device );
  return InvalidDevice;
}


int Device::open( Device &device, long mode )
{
  setDeviceFile( device.deviceIdent() );
  return InvalidDevice;
}


int Device::reset( void )
{
  return 0;
}


string Device::info( void ) const
{
  string s = "";
  ostringstream ss;
  ss << "type: " << deviceType() << ends;
  s += ss.str();
  s += ";class: " + deviceClass();
  s += ";ident: " + deviceIdent();
  if ( ! deviceName().empty() )
    s += ";name: " + deviceName();
  if ( ! deviceVendor().empty() )
    s += ";vendor: " + deviceVendor();
  s += ";file: " + deviceFile();
  if ( isOpen() )
    s += ";status: open";
  else
    s += ";status: not open";
  return s;
}


string Device::settings( void ) const
{
  return Settings;
}


void Device::setSettings( const string &settings )
{
  Settings = settings;
}


void Device::clearSettings( void )
{
  Settings = "";
}


int Device::deviceType( void ) const
{
  return DeviceType;
}


void Device::setDeviceType( int type )
{
  DeviceType = type;
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

