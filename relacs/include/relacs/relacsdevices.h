/*
  relacsdevices.h
  Implementations of the DeviceLists

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

#ifndef _RELACS_RELACSDEVICES_H_
#define _RELACS_RELACSDEVICES_H_ 1

#include <relacs/devicelist.h>
#include <relacs/device.h>
#include <relacs/analoginput.h>
#include <relacs/analogoutput.h>
#include <relacs/digitalio.h>
#include <relacs/attenuator.h>
#include <relacs/attenuate.h>

namespace relacs {


/*!
\class Devices
\brief A container for all devices
*/

class AllDevices : public DeviceList< Device, 0 >
{
public:
  AllDevices( void )
    : DeviceList< Device, 0 >( "Device", "All Devices" ) {};
  void clear( void )
    {
      for ( int k=DVs.size()-1; k>=0; k-- )
	delete DVs[k];
      DVs.clear();
      for ( unsigned int k=0; k<Menus.size(); k++ ) {
	if ( Menus[k] != 0 )
	  delete Menus[k];
      }
      Menus.clear();
    }
  int create( Plugins &plugins, int n, const string &dflt="" )
    { return 0; };
};


/*!
\class Devices
\brief A container for miscellaneous devices
*/

class Devices : public DeviceList< Device, RELACSPlugin::DeviceId >
{
public:
  Devices( void )
    : DeviceList< Device, RELACSPlugin::DeviceId >( "Device", "Devices" ) {};
};


/*!
\class AIDevices
\brief A container for analog input devices
*/

class AIDevices : public DeviceList< AnalogInput, RELACSPlugin::AnalogInputId >
{
public:
  AIDevices( void )
    : DeviceList< AnalogInput, RELACSPlugin::AnalogInputId >( "Analog Input Device", "Analog Input Devices" ) {};
};


/*!
\class AODevices
\brief A container for analog output devices
*/

class AODevices : public DeviceList< AnalogOutput, RELACSPlugin::AnalogOutputId >
{
public:
  AODevices( void )
    : DeviceList< AnalogOutput, RELACSPlugin::AnalogOutputId >( "Analog Output Device", "Analog Output Devices" ) {};
protected:
  virtual void init( AnalogOutput *dv, int n, const Str &ns, AllDevices &devices )
    {
      double extr = number( "extref" + ns, -1.0, "V" );
      dv->setExternalReference( extr );
    };
};


/*!
\class DIODevices
\brief A container for digital I/O devices
*/

class DIODevices : public DeviceList< DigitalIO, RELACSPlugin::DigitalIOId >
{
public:
  DIODevices( void )
    : DeviceList< DigitalIO, RELACSPlugin::DigitalIOId >( "Digital I/O Device", "Digital I/O Devices" ) {};
};


/*!
\class AttDevices
\brief A container for attenuator devices
*/

class AttDevices : public DeviceList< Attenuator, RELACSPlugin::AttenuatorId >
{
public:
  AttDevices( void )
    : DeviceList< Attenuator, RELACSPlugin::AttenuatorId >( "Attenuator", "Attenuator Devices" ) {};

protected:
  virtual int mode( int n, const Str &ns )
    { return 0; };
};


/*!
\class AttInterfaces
\brief A container for attenuator interfaces
*/

class AttInterfaces : public DeviceList< Attenuate, RELACSPlugin::AttenuateId >
{
public:
  AttInterfaces( AODevices *aod )
    : DeviceList< Attenuate, RELACSPlugin::AttenuateId >( "Attenuate", "Attenuator Interfaces" ),
    AOD( aod ) {};
  Attenuate *attenuate( const string &device, int channel )
    {
      for ( unsigned int k=0; k < DVs.size(); k++ )
	if ( DVs[k]->aoDevice() == device &&
	     DVs[k]->aoChannel() == channel )
	  return DVs[k];
      return 0;
    }
  Attenuate *attenuate( int index, int channel )
    {
      if ( index < 0 || index >= AOD->size() )
	return 0;
      string device = (*AOD)[index].deviceIdent();
      return attenuate( device, channel );
    }
  void save( const string &path )
    {
      for ( unsigned int k=0; k < DVs.size(); k++ ) {
	DVs[k]->save( path );
      }
    }

protected:
  virtual int mode( int n, const Str &ns )
    { return integer( "line" + ns, 0, 0 ); };
  virtual void init( Attenuate *dv, int n, const Str &ns, AllDevices &devices )
    {
      string aos = text( "ao" + ns, 0, "" );
      Device *d = AOD->device( aos );
      if ( d == 0 && ! AOD->empty() )
	d = &(*AOD)[0];
      if ( d != 0 )
	dv->setAODevice( d->deviceIdent() );
      else
	dv->setAODevice( "" );
      dv->setAOChannel( integer( "channel" + ns, 0, 0 ) );
    };

  AODevices *AOD;
    
};


}; /* namespace relacs */

#endif /* ! _RELACS_RELACSDEVICES_H_ */

