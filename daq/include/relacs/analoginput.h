/*
  analoginput.h
  Interface for accessing analog input of a data-aquisition board.

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

#ifndef _RELACS_ANALOGINPUT_H_
#define _RELACS_ANALOGINPUT_H_ 1

#include <vector>
#include <relacs/device.h>
#include <relacs/inlist.h>
#include <relacs/tracespec.h>

using namespace std;

namespace relacs {


class AnalogOutput;

/*! 
\class AnalogInput
\brief Interface for accessing analog input of a data-aquisition board.
\author Marco Hackenberg
\author Jan Benda
\version 0.2
\todo add probe function that returns a string of possible supported devices.
\todo add wait function
\todo add directread function for single point acquisition
\todo add a flag for indicating whether device is capable of streaming input
*/

class AnalogInput : public Device
{

public:

    /*! Create a new AnalogInput without opening a device.
        Reimplement this constructor. */
  AnalogInput( void );
    /*! Constructs an AnalogInput with type id \a aitype.
        \sa setAnalogInputType() */
  AnalogInput( int aitype );
    /*! Constructs an AnalogInput with device class \a deviceclass
        and type id \a aitype. 
        \sa setDeviceClass(), setAnalogInputType() */
  AnalogInput( const string &deviceclass, int aitype=0 );
    /*! Stop analog input and close the daq driver. */
  virtual ~AnalogInput( void );

    /*! Open the analog input device specified by \a device.
 	Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close(), reset() */
  virtual int open( const string &device, long mode=0 );
    /*! Open the analog input device specified by \a device.
	Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close(), reset() */
  virtual int open( Device &device, long mode=0 );
    /*! Returns true if the device is open.
        \sa open(), close(), reset() */
  virtual bool isOpen( void ) const = 0;
    /*! Stop all activity and close the device.
        \sa open(), isOpen(), reset() */
  virtual void close( void ) = 0;

    /*! Returns a string with some information about the 
        analog input device.
        This should include the names of possible readable internal variables
        (see addTraces()). */
  virtual string info( void ) const;

    /*! Number of analog input channels. */
  virtual int channels( void ) const = 0;
    /*! Resolution in bits of analog input. */
  virtual int bits( void ) const = 0;
    /*! Maximum sampling rate in Hz of analog input. */
  virtual double maxRate( void ) const = 0;

    /*! Maximum number of analog input ranges. */
  virtual int maxRanges( void ) const = 0;
    /*! Voltage range \a index in Volt for unipolar mode.
        If -1 is returned this range is not supported. */
  virtual double unipolarRange( int index ) const = 0;
    /*! Voltage range \a index in Volt for bipolar mode.
        If -1 is returned this range is not supported. */
  virtual double bipolarRange( int index ) const = 0;

    /*! Test settings for analog input on the device
        for each input channel in \a traces.
	First, all error flags in \a traces are cleared (DaqError::clearError()).
	If an error ocurred in any trace, the corresponding errorflags in the
	InData are set and a negative value is returned.
        The channels in \a traces are not sorted. 
        Simply calls testReadData() and testReadDevice().
        This function can be called any time
        independently of prepareRead() and startRead() with
        different \a traces. */
  virtual int testRead( InList &traces );
    /*! Prepare analog input of the input traces \a traces on the device
        as much as possible.
	If an error ocurred in any channel, the corresponding errorflags in the
	InData structure are filled and a negative value is returned.
	In addition, according to the InData::gainIndex() of each trace
	the gain and the minimum and maximum possible output voltages
	for each of the \a traces should be set (see InData::setGain(),
	InData::setMaxVoltage(), InData::setMinVoltage() ).
	This function assumes that \a traces successfully passed testRead().
        The channels in \a traces are not sorted. */
  virtual int prepareRead( InList &traces ) = 0;
    /*! Start non-blocking analog input for the input traces that were passed 
        to the previous call of prepareRead().
	If an error ocurred in any channel, the corresponding errorflags in the
	InData structure are filled and a negative value is returned.
	Also start possible pending acquisition on other devices
	that are known from take().
        This function is called after a successfull prepareRead() or after stop().
        This function should be as quick as possible. */
  virtual int startRead( void ) = 0;
    /*! Read data from a running data acquisition
        and store them in an internal buffer.
        Returns the total number of read data values.
	If an error ocurred in any channel, the corresponding errorflags in the
	InData structure are filled and a negative value is returned.
	If no acquisition is running and therefore no more data are to be expected,
	a negative number is returned without setting an errorflag of the InData.
        This function is called periodically after reading has been successfully
        started by startRead().
        This function does not modify the traces passed to prepareRead()! */
  virtual int readData( void ) = 0;
    /*! Convert the acquired data from the internal buffer
        to numbers in the secondary unit and store the data
	in the traces that were passed to the previous call of prepareRead().
        Returns the number of data values that were added to the traces
	(sum over all traces).
	If an error ocurred in any channel, the corresponding errorflags in the
	InData structure are filled and a negative value is returned.
        This function is called periodically after one or several calls to readData(). */
  virtual int convertData( void ) = 0;

    /*! Stop any running ananlog input activity,
        but preserve all so far read in data.
	The next call to readData() will return the data.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, ReadError.
        \sa reset() */
  virtual int stop( void ) = 0;
    /*! Stop any running ananlog input activity,
        clear any internal data buffers and reset the device.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, ReadError.
        \sa stop(), close(), open(), isOpen() */
  virtual int reset( void ) = 0;

    /*! True if analog input is running. */
  virtual bool running( void ) const = 0;

    /*! Get error status of the device. 
        0: no error
	1: input buffer overflow
        2: overrun
        other: unknown */
  virtual int error( void ) const = 0;

    /*! Check for every analog input and analog output device in \a ais
        and \a aos, respectively,
        whether it can be simultaneously started by startRead()
	from this device.
	Add the indices of those devices to \a aiinx and \a aoinx,
        respectively. */
  virtual void take( const vector< AnalogInput* > &ais,
		     const vector< AnalogOutput* > &aos,
		     vector< int > &aiinx, vector< int > &aoinx );

    /*! If the analog input device supports inputs that
        are not physical input lines but rather readable internal variables,
        like model outputs of a dynamic clamp modul,
        then reimplement this function. 
        Add for each such variable a TraceSpec to \a traces.
        \a deviceid is the id of the analog output device
        that you should use for initializing the TraceSpec. */
  virtual void addTraces( vector< TraceSpec > &traces, int deviceid ) const;

    /*! May match trace names with driver internal trace names,
        for example trace names of a dynamic clamp model.
        \return 0: no traces needed to be matched.
        \return n: \a n traces have been successfully matched.
	\return -1: Failed to match a trace. */
  virtual int matchTraces( InList &traces ) const;

    /*! The id of the analog input implementation.
        \sa setAnalogInputType(), deviceType(), deviceName(), ident() */
  int analogInputType( void ) const;


protected:

    /*! Test each input channel in \a traces for valid settings.
	If an error ocurred in any trace, the corresponding errorflags in the
	InData are set and a negative value is returned.
	The following error conditions are checked:
        DeviceNotOpen, NoData (InData::capacity() <= 0), 
	MultipleDevices, MultipleStartSources,
	MultipleDelays, MultipleSampleRates, MultipleContinuous, 
	MultipleBuffersizes (InData::capacity()),
	MultipleBufferTimes, MultipleUpdateTimes, 
	InvalidStartSource (InData::startSource() < 0),
	InvalidDelay (InData::delay() < 0),
	InvalidSampleRate (InData::sampleRate() > maxRate() or < 1.0), 
	InvalidChannel (InData::channel() >= channels() or < 0), 
	MultipleChannels,
	InvalidReference (InData::reference() != InData::Differential,
	InData::Referenced, InData::NonReferenced),
	InvalidGain (InData::gainIndex() not valid, i.e. < 0 or >= maxRanges()
	or the corresponding range in unipolarRange() or bipolarRange does not exist).
        This function is called by testRead(). */
  virtual int testReadData( InList &traces );
    /*! Device driver specific tests on the settings in \a traces
        for each input channel.
	Before this function is called, the validity of the settings in 
	\a traces was already tested by testReadData().
	testWriteDevice() is called even if an error was detected by
	testWriteData().
	This function should test whether the settings are really supported
	by the hardware.
	If an error ocurred in any trace, the corresponding errorflags in the
	InData are set and a negative value is returned.
	In addition, according to the gainIndex() of each trace
	the gain and the minimum and maximum possible output voltages
	for each of the \a traces should be set (see InData::setGain(),
	InData::setMaxVoltage(), InData::setMinVoltage() ).
        The channels in \a traces are not sorted.
        This function is called by testRead(). */
  virtual int testReadDevice( InList &traces ) = 0;

    /*! Set id of the analog input implementation to \a aitype.
	This function should be called in the constructor
	of an implementation of AnalogInput.
        \sa analogInputType(), setDeviceType(), setDeviceName(),
	setDeviceFile(), setIdent() */
  void setAnalogInputType( int aitype );

    /*! Set the settings() string for \a traces.
        Call this function from within a successful prepareRead().
	\param[in] readbuffer is the size of the driver's buffer in bytes.
	\param[in] updatebuffer is the size of the internal buffer in bytes.
        \sa settings() */
  void setSettings( const InList &traces, int readbuffer=0,
		    int updatebuffer=0 );


private:

    /*! The type of the implementation of AnalogInput. */
  int AnalogInputSubType;

};


}; /* namespace relacs */

#endif /* ! _RELACS_ANALOGINPUT_H_ */
