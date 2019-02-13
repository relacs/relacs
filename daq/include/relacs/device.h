/*
  device.h
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

#ifndef _RELACS_DEVICE_H_
#define _RELACS_DEVICE_H_ 1


#include <iostream>
#include <string>
#include <QMutex>
#include <relacs/options.h>
using namespace std;

namespace relacs {


/*!
\class Device
\brief Basic interface for accessing a device.
\author Jan Benda

The Device class defines the interface for the basic operations open(),
close(), and reset() of a device.
You have to implement at least the isOpen(), close() and one of the
two open() functions.

In case you want to use a device within RELACS, your Device
implementation needs to provide a void default constructor (i.e. with
no parameters) that does not open the device. Make sure your constructor
calls \c initOptions() once which registers configuation options, do not
forget to also call the base class implementation. Also, include the
header file \c <relacs/relacsplugin.h> and make the Device known to
RELACS with the \c addDevice( ClassNameOfYourDeviceImplementation,
PluginSetName ) macro or one of the derived macros for any specialized
DeviceType.

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

The info() function returns Options that contain some usefull information
about the capabilities of the specific device.
The settings() functions returns Options containing
information about the current
settings of the specific device.
You should fill in both info() and settings() in an implementation
of the Device class.

There are four flags for indicating errors with handling the device:
NotOpen, InvalidDevice, ReadError, and WriteError.
In addition an arbitrary error string errorStr() can be set with setErrorStr() 
and addErrorStr() to indicate a failure in detail.

Each device has its own mutex() for locking critical sections. Use
lock() and unlock() for locking the device.
*/


class Device : public virtual Options
{

public:

    /*! Predefined device types. \sa deviceType(), setDeviceType() */
  enum  DeviceTypes {
    MiscellaneousType = 0,
    AnalogInputType = 1,
    AnalogOutputType = 2,
    DigitalIOType = 3,
    TriggerType = 4,
    AttenuatorType = 5,
    AttenuateType = 6,
    ManipulatorType = 7,
    TemperatureType = 8,
    CameraType = 9
  };

    /*! Return code indicating that the device driver is not opened. */
  static const int NotOpen = -1;
    /*! Return code indicating an invalid device or subdevice. */
  static const int InvalidDevice = -2;
    /*! Return code indicating a failure in reading from the device. */
  static const int ReadError = -3;
    /*! Return code indicating a failure in writing to the device. */
  static const int WriteError = -4;
    /*! Return code indicating an invalid parameter. */
  static const int InvalidParam = -5;
    /*! Return code indicating that a requested feature is not supported by the device. */
  static const int NotSupported = -6;

    /*! Construct a Device of type \a type. This constructor is used by
        the more specific but still abstract device classes like AnalogInput,
	AnalogOutput, etc.
        \sa setDeviceType() */
  Device( int type=MiscellaneousType );
    /*! Construct a Device with class-name \a deviceclass and type \a type.
        Use this constructor if your device does not fit in the predefined
	DeviceTypes.
        \sa setDeviceClass(), setDeviceType() */
  Device( const string &deviceclass, int type=MiscellaneousType );
    /*! Destroy a Device. In case the associated device is open, close it. */
  virtual ~Device( void );

    /*! Open the device specified by \a device with options \a opts.
	On success, in your implementation
	\a device should be passed to setDeviceFile()
	and the name and the vendor of the device should be set
	with setDeviceName() and setDeviceVendor().
        The info() should be set in the implementation of open()
	by the info().addNumber(), info().addText(), etc. functions.
        For adding some default information to info() you may use addInfo().
	Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close(), reset() */
  virtual int open( const string &device);
    /*! Open the device specified by \a device with options \a opts.
	On success, in your implementation
	\a device should be passed to setDeviceFile()
	and the name and the vendor of the device should be set
	with setDeviceName() and setDeviceVendor().
        The info() should be set in the implementation of open()
	by the info().addNumber(), info().addText(), etc. functions.
        For adding some default information to info() you may use addInfo().
	Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close(), reset() */
  virtual int open( Device &device);
    /*! Returns true if the device is open.
        \sa open(), close(), reset() */
  virtual bool isOpen( void ) const = 0;
    /*! Close the device.
        info() should be cleared in an implementation of close()
	by caling Info.clear().
        \sa open(), isOpen(), reset() */
  virtual void close( void ) = 0;
    /*! Reset the device.
        Returns zero on success, otherwise one of the flags
        NotOpen, InvalidDevice, WriteError.
	An implementation should not clear the error string
	and should onlyadd error strings using addErrorStr().
        \sa close(), open(), isOpen() */
  virtual int reset( void );

    /*! Returns some information about the capabilities of
        the device (for example, maximum possible sampling rate).
	This function can be reimplemented to set the
	the infos right before returning them.
        \sa settings() */
  virtual const Options &info( void ) const;
    /*! Returns some information about the current settings
        of the device (for example, the currently used sampling rate).
	This function can be reimplemented to set the
	the settings right before returning them.
        \sa info() */
  virtual const Options &settings( void ) const;

    /*! The id of the device type.
        \sa deviceTypeStr() setDeviceType(), deviceClass(),
	deviceFile(), deviceIdent(), deviceName(), deviceVendor() */
  int deviceType( void ) const;
    /*! \return a string naming the device type.
        \sa deviceType() setDeviceType(), deviceClass(), deviceFile(),
	deviceIdent(), deviceName(), deviceVendor() */
  string deviceTypeStr( void ) const;
    /*! \return the number of known device types.
        \sa deviceTypeStr(), deviceType() */
  static int deviceTypes( void );
    /*! \return a string naming the device type \a type.
        \sa deviceType(), \a deviecTypes() */
  static string deviceTypeStr( int type );
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

    /*! Clear the error string.
        \sa setErrorStr(), addErrorStr() */
  virtual void clearError( void ) const;
    /*! Return error string of the last operation.
        \sa setErrorStr(), addErrorStr() */
  virtual string errorStr( void ) const;
    /*! Set error string to \a strg. 
        \sa addErrorStr(), errorStr() */
  void setErrorStr( const string &strg ) const;
    /*! Add \a strg to the error string. 
        \sa setErrorStr(), errorStr() */
  void addErrorStr( const string &strg ) const;
    /*! Set error string to the string describing the 
        standard C error code \a errnum (from \c errno). 
        \sa addErrorStr(), errorStr() */
  void setErrorStr( int errnum ) const;
    /*! Add the string describing the standard C error code \a errnum 
        (from \c errno) to the error string. 
        \sa setErrorStr(), errorStr() */
  void addErrorStr( int errnum ) const;

    /*! Translates the error code \a ern returned by open(), reset(), etc. into a string. */
  static string getErrorStr( int ern );

    /*! Return \c true if the last operation was successfull and did not set an error string,
        i.e. errorStr().empty(). */
  virtual bool success( void ) const;
    /*! Return \c true if the last operation failed and did set an error string,
        i.e. not errorStr().empty(). */
  virtual bool failed( void ) const;

    /*! Write info() to \a str. */
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

    /*! Adds the device's type (deviceType()),
	class (deviceClass()), identifier (deviceIdent()), name (deviceName()),
	vencdor (deviceVendor()), file (deviceFile()), and status (isOpen())
	to info().
        \sa info(), settings() */
  void addInfo( void );

    /*! Lock the mutex of this device. \sa unlock(), mutex() */
  void lock( void ) const;
    /*! Unlock the mutex of this device. \sa lock(), mutex() */
  void unlock( void ) const;
    /*! \return A pointer to the mutex of this device. \sa lock(), unlock() */
  QMutex *mutex( void ) const;

    /*! Intializes available options */
  virtual void initOptions( void );

  mutable Options Info;
  mutable Options Settings;


private:

  static const int MaxDeviceTypeStrs = 9;
  static const string DeviceTypeStrs[MaxDeviceTypeStrs];

  int DeviceType;
  string DeviceClass;
  string DeviceIdent;
  string DeviceFile;
  string DeviceName;
  string DeviceVendor;

  mutable string ErrorString;

  mutable QMutex Mutex;

};


}; /* namespace relacs */

#endif /* ! _RELACS_DEVICE_H_ */
