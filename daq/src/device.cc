/*
  device.cc
  Basic interface for accessing a device.

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

#include <cstring>
#include <sstream>
#include <relacs/device.h>
using namespace std;

namespace relacs {


const string Device::DeviceTypeStrs[Device::MaxDeviceTypeStrs] =
  { "Miscellaneous", "Analog Input", "Analog Output", "Digital I/O", "Attenuator",
    "Attenuate Interface", "Manipulator", "Temperature","Camera" };


Device::Device( int type )
  : Device("", type)
{
}


Device::Device( const string &deviceclass, int type )
  : DeviceType( type ),
    DeviceClass ( deviceclass ),
    DeviceIdent( "" ),
    DeviceFile( "" ),
    DeviceName( "" ),
    DeviceVendor( "" ),
    ErrorString( "" )
{
}


Device::~Device( void )
{
}


void Device::initOptions()
{
  addText( "plugin", "Plugin", "" );
  addText( "ident", "Ident", "" );
  addText( "device", "Device", "" );
}


int Device::open( const string &device )
{
  setErrorStr( "open(string) with string='" + device + "' not supported" );
  setDeviceFile( device );
  return InvalidDevice;
}


int Device::open( Device &device )
{
  setErrorStr( "open(device) with device='" + device.deviceClass() + "' not supported" );
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


void Device::clearError( void ) const
{
  ErrorString = "";
}


string Device::errorStr( void ) const
{
  return ErrorString;
}


void Device::setErrorStr( const string &strg ) const
{
  ErrorString = strg;
}


void Device::addErrorStr( const string &strg ) const
{
  if ( strg.empty() )
    return;
  if ( !ErrorString.empty() ) {
    if ( ErrorString[ErrorString.size()-1] != '.' )
      ErrorString += ".";
    ErrorString += "<br> ";
  }
  ErrorString += strg;
}


void Device::setErrorStr( int errnum ) const
{
  char buf[1000];
  char *ep = strerror_r( errnum, buf, 1000 );
  if ( strlen( ep ) > 0 )
    ErrorString = ep;
  else
    ErrorString = "unknown error";
}


void Device::addErrorStr( int errnum ) const
{
  if ( !ErrorString.empty() )
    ErrorString += ", ";

  char buf[1000];
  char *ep = strerror_r( errnum, buf, 1000 );
  if ( strlen( ep ) > 0 )
    ErrorString += ep;
  else
    ErrorString += "unknown error";
}


string Device::getErrorStr( int ern )
{
  string es = "";
  switch ( ern ) {
  case NotOpen :
    es = "device not open";
    break;
  case InvalidDevice :
    es = "invalid device";
    break;
  case ReadError :
    es = "read error";
    break;
  case WriteError :
    es = "write error";
    break;
  case InvalidParam :
    es = "invalid parameter";
    break;
  case NotSupported :
    es = "requested feature is not supported";
    break;
  }
  return es;
}


bool Device::success( void ) const
{
  return ( ErrorString.empty() );
}


bool Device::failed( void ) const
{
  return ( ! ErrorString.empty() );
}


void Device::lock( void ) const
{
  Mutex.lock();
}


void Device::unlock( void ) const
{
  Mutex.unlock();
}


QMutex *Device::mutex( void ) const
{
  return &Mutex;
}


ostream &operator<<( ostream &str, const Device &d )
{
  str << d.info();
  return str;
}


}; /* namespace relacs */

