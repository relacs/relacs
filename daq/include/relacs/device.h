/*
  device.h
  Virtual class for accessing a device. 

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _DEVICE_H_
#define _DEVICE_H_


#include <iostream>
#include <string>
using namespace std;


/*!
\class Device
\brief Basic interface for accessing a device. 
\author Jan Benda
\version 1.2
\todo Error codes and strings for errors in the open function

The Device class defines the interface for the basic operations open(),
close(), and reset() of a device. 
You have to implement at least the isOpen(), close() and one of the
two open() functions.

Reimplement the info() function to return some usefull information
about the capabilities of the specific device.
The settings() functions is ment to return information about the current
settings of the specific device.

A subclass of Device can be identified by its \a deviceType().
For example, the Temperature class provides a uniqe interface for measuring
temperatures. All classes derived from the Temperature class then
share the same deviceType().
The type of a Device class should be set in the constructor of
the interface class either directly using setDeviceType()
or by passing the type to the constructor of the Device class.

A name for class of devices a particular implementation of the Device class 
is handling is returned by deviceClass().
Usually deviceClass() is the name of the class derived from Device.
Set the class-name for the devices in the constructor
with setDeviceClass() or by passing the name
to the constructor of the Device class.

Each individual physical device may have an unique identifier string 
deviceIdent() that can be set by setDeviceIdent().
The identifier string should be unique for each instance of a device class.
The name and the vendor of the physical device is returned by
deviceName() and deviceVendor().
Both should be set from within the open() function using 
setDeviceName() and setDeviceVendor().
The device file on which the device was opened is returned by deviceFile().

There are four flags for indicating errors with handling the device:
NotOpen, InvalidDevice, ReadError, and WriteError.
*/
  

class Device
{

public:

    /*! Return code indicating that the device driver is not opened. */
  static const int NotOpen = -1;
    /*! Return code indicating an invalid device or subdevice. */
  static const int InvalidDevice = -2;
    /*! Return code indicating a failure in reading from the device. */
  static const int ReadError = -3;
    /*! Return code indicating a failure in writing to the device. */
  static const int WriteError = -4;

    /*! Construct a Device of type \a type.
        \sa setDeviceType() */
  Device( int type=0 );
    /*! Construct a Device with class-name \a deviceclass and type \a type.
        \sa setDeviceClass(), setDeviceType() */
  Device( const string &deviceclass, int type=0 );
    /*! Destroy a Device. In case the associated device is open, close it. */
  virtual ~Device( void );

    /*! Open the device specified by \a device with mode \a mode.
        You don't have to use \a mode in your implementation.
	On success, in your implementation
	\a device should be passed to setDeviceFile()
	and the name and the vendor of the device should be set
	with setDeviceName() and setDeviceVendor().
	Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close(), reset() */
  virtual int open( const string &device, long mode );
    /*! Open the device specified by \a device with mode \a mode.
        You don't have to use \a mode in your implementation.
	On success, in your implementation
	\a device should be passed to setDeviceFile()
	and the name and the vendor of the device should be set
	with setDeviceName() and setDeviceVendor().
	Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close(), reset() */
  virtual int open( Device &device, long mode );
    /*! Returns true if the device is open.
        \sa open(), close(), reset() */
  virtual bool isOpen( void ) const = 0;
    /*! Close the device.
        \sa open(), isOpen(), reset() */
  virtual void close( void ) = 0;
    /*! Reset the device. 
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, WriteError.
        \sa close(), open(), isOpen() */
  virtual int reset( void );

    /*! Returns a string with some information about the capabilities of 
        the device (for example, maximum possible sampling rate).
        The string is a series of 'name: value' pairs separated by semicolons ';'.
	This implementation returns the device's type (deviceType()),
	class (deviceClass()), identifier (deviceIdent()), name (deviceName()),
	vencdor (deviceVendor()), file (deviceFile()), and status (isOpen()).
        \sa settings() */
  virtual string info( void ) const;
    /*! Returns a string with some information about the current settings 
        of the device (for example, the currently used sampling rate).
        The string is a series of 'name: value' pairs separated by semicolons ';'.
	This implementation returns an empty string.
        \sa info() */
  virtual string settings( void ) const;

    /*! The id of the device class.
        \sa setDeviceType(), deviceClass(), deviceFile(), deviceIdent(),
	deviceName(), deviceVendor() */
  int deviceType( void ) const;
    /*! The name for the device(s) a particular implementation
        of the Device class is handling.
        \sa setDeviceClass(), deviceIdent(), deviceType(), deviceFile(),
        deviceName(), deviceVendor() */
  string deviceClass( void ) const;
    /*! The name of the device file on which the device was opened.
        \sa setDeviceFile(), deviceIdent(), deviceClass(), deviceType(),
        deviceName(), deviceVendor() */
  string deviceFile( void ) const;
    /*! The name of the particular physical device that is handled by 
        this instance of the Device class.
        \sa setDeviceName(), deviceIdent(), deviceClass(), deviceType(),
        deviceFile(), deviceVendor() */
  string deviceName( void ) const;
    /*! The vendor of the particular physical device that is handled by 
        this instance of the Device class.
        \sa setDeviceVendor(), deviceIdent(), deviceClass(), deviceType(),
        deviceFile(), deviceName() */
  string deviceVendor( void ) const;

    /*! The uniqe identifier string of an individual physical device.
        \sa setDeviceIdent(), deviceType(), deviceClass(), deviceFile(),
	deviceName(), deviceVendor() */
  string deviceIdent( void ) const;
    /*! Set the identifier string of an individual physical device to \a ident.
        \sa deviceIdent(), setDeviceClass(), setDeviceType(), setDeviceFile(),
	setDeviceName(), setDeviceVendor() */
  virtual void setDeviceIdent( const string &ident );

  friend ostream &operator<<( ostream &str, const Device &d );


protected:

    /*! Set id of the device class to \a type.
	This function should be called in the constructor
	of an device interface class.
        \sa deviceType(), setDeviceClass(), setDeviceFile(), setDeviceIdent(),
	setDeviceName(), setDeviceVendor() */
  void setDeviceType( int type );
    /*! Set the name for the class of devices a particular implementation
        of the Devive class is handling to \a deviceclass.
	This function should be called in the constructor and/or
	the open function of the Device class implementation.
        \sa deviceClass(), setDeviceFile(), setDeviceIdent(), setDeviceType(),
	setDeviceName(), setDeviceVendor() */
  void setDeviceClass( const string &deviceclass );
    /*! Set the device file for which the device was opened.
        \sa deviceFile(), setDeviceIdent(), setDeviceClass(), setDeviceType(),
	setDeviceName(), setDeviceVendor() */
  void setDeviceFile( const string &devicefile );
    /*! Set the name of the particular physical device that is handled
        by this instance of the Device class to \a devicename.
        \sa deviceName(), setDeviceIdent(), setDeviceClass(), setDeviceType(),
	setDeviceFile(), setDeviceVendor() */
  void setDeviceName( const string &devicename );
    /*! Set the vendor of the particular physical device that is handled
        by this instance of the Device class to \a devicevendor.
        \sa deviceName(), setDeviceIdent(), setDeviceClass(), setDeviceType(),
	setDeviceFile(), setDeviceName() */
  void setDeviceVendor( const string &devicevendor );


private:
  
  int DeviceType;
  string DeviceClass;
  string DeviceIdent;
  string DeviceFile;
  string DeviceName;
  string DeviceVendor;

};

#endif
