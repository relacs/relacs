/*
  analogoutput.h
  Interface for accessing analog output of a data-aquisition board.

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

#ifndef _RELACS_ANALOGOUTPUT_H_
#define _RELACS_ANALOGOUTPUT_H_ 1

#include <vector>
#include <relacs/device.h>
#include <relacs/outlist.h>
#include <relacs/tracespec.h>

using namespace std;

namespace relacs {


class AnalogInput;

/*! 
\class AnalogOutput
\author Marco Hackenberg
\author Jan Benda
\version 0.5
\brief Interface for accessing analog output of a data-aquisition board.
\todo add wait function for blocked writes. Returns an error code in the signals.
\todo add probe function that returns a string of possible supported devices.
\todo add directwrite function for single point output
\todo add a flag for indicating whether device is capable of streaming output
*/

class AnalogOutput : public Device
{

public:
  
    /*! Create a new AnalogOutput without opening a device.
        Reimplement this constructor. */
  AnalogOutput( void );
    /*! Constructs an AnalogOutput with type id \a aotype.
        \sa setAnalogOutputType() */
  AnalogOutput( int aotype );
    /*! Constructs an AnalogOutput with device class \a deviceclass
        and type id \a aotype.  
        \sa setDeviceClass(), setAnalogOutputType() */
  AnalogOutput( const string &deviceclass, int aotype=0 );
    /*! Stop analog output and close the daq driver. */
  virtual ~AnalogOutput( void );

    /*! Open the analog output device specified by \a device.
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
        analog output device.
        This should include the names of possible writeable parameter
        (see addTraces()). */
  virtual string info( void ) const;

    /*! Number of analog output channels. */
  virtual int channels( void ) const = 0;
    /*! Resolution in bits of analog output. */
  virtual int bits( void ) const = 0;
    /*! Maximum sampling rate in Hz of analog output. */
  virtual double maxRate( void ) const = 0;

    /*! The voltage of an external reference for scaling the analog output.
        A negative number indicates that an external reference is not
        connected to the daq board. 
        Zero is returned, if the external reference is a non-constant voltage. */
  double externalReference( void ) const;
    /*! Set the voltage of an external reference for scaling the analog output
        to \a extr. 
        A negative \a extr indicates that an external reference is not
        connected to the daq board.
        Set \a extr to zero if the external reference 
	is a non-constant voltage. */
  void setExternalReference( double extr );

    /*! Test settings for analog output on the device
        for each output signal in \a sigs.
	First, all error flags in \a sigs are cleared (DaqError::clearError()).
	If an error ocurred in any signal, the corresponding errorflags in
	OutData are set and a negative value is returned.
        The channels in \a sigs are not sorted. 
        Simply calls testWriteData() and testWriteDevice().
        This function can be called any time
        independently of prepareWrite() and startWrite() with
        different \a sigs. */
  virtual int testWrite( OutList &sigs );
    /*! Convert data of the output signals \a sigs.
	If an error ocurred in any channel, the corresponding errorflags in the
	OutData structure are filled and a negative value is returned.
        The default implementation calls convert<signed short>( sigs ),
	i.e. the output signals are sorted by channel number first
        and are then multiplexed into a buffer of signed short's (2 byte).
        This should be good for most 12 or 16 bit daq boards.
        The buffer is attached to the first signal in \a sigs.
        This function is called before prepareWrite(). */
  virtual int convertData( OutList &sigs ) = 0;
    /*! Prepare analog output of the output signals \a sigs on the device.
	If an error ocurred in any signal, the corresponding errorflags in
	OutData are set and a negative value is returned.
	In addition, according to OutData::requestedMin() and 
	OutData::requestedMax(), the gain, the polarity, the reference,
	and the minimum and maximum possible output voltages for each
	of the traces in \a sigs should be set (see OutData::setGain(),
	OutData::setGainIndex(), OutData::setMaxData(), OutData::setMinData() ).
	This function assumes that \a sigs successfully passed testWrite()
	and that convertData() was already called.
        The channels in \a sigs are not sorted. */
  virtual int prepareWrite( OutList &sigs ) = 0;
    /*! Start analog output of the output signals that were passed to the previous call
        of prepareWrite().
	If an error ocurred in any signal, the corresponding errorflags in
	OutData are set and a negative value is returned.
	Also start possible pending acquisition on other devices
	that are known from take().
        This function is always called after a successfull prepareRead(). */
  virtual int startWrite( void ) = 0;
    /*! Write data of the output signals that were passed to the previous call
        of prepareWrite() to the analog output device.
        Returns the number of transferred data elements.
	Returns zero if all data are transferred.
	If an error ocurred in any channel, the corresponding errorflags in the
	InData structure are filled and a negative value is returned.
        This function is called periodically after writing has been successfully
        started by startWrite(). */
  virtual int writeData( void ) = 0;

    /*! Stop any running ananlog output activity and reset the device,
        i.e. clear all buffers.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, WriteError.
        \sa close(), open(), isOpen() */
  virtual int reset( void );
  
    /*! True if analog output is running. */
  virtual bool running( void ) const  = 0;

    /*! Get error status of the device. 
        0: no error
        1: output underrun
        other: unknown */
  virtual int error( void ) const = 0;

    /*! Index of signal start.
        The default implemetation returns -1, indicating that
        no index is available.
        If the analog output driver can return
        an index into the data stream of a running analog input
        where the last analog output started,
        then this function should return the this index.
        You also need to reimplement getAISyncDevice()
        to let the user know about this property. */
  virtual long index( void ) const;
    /*! In case the analog output driver can return
        an index (via the index() function)
        into the data stream of a running analog input
        where the last analog output started,
        then this function should return the index
        of the corresponding analog input device in \a ais.
        This function is called once after opening the device
        and before any IO operation.
        The default implementation simply returns -1. */
  virtual int getAISyncDevice( const vector< AnalogInput* > &ais ) const;

    /*! Check for every analog output device in \a aos
        whether it can be simultaneously started by startWrite()
	from this device.
	Add the indices of those devices to \a aoinx. */
  virtual void take( const vector< AnalogOutput* > &aos,
		     vector< int > &aoinx );

    /*! If the analog output device supports outputs that
        are not physical output lines but rather writeable parameter,
        like model parameter for a dynamic clamp modul,
        then reimplement this function. 
        Add for each such parameter a TraceSpec to \a traces.
        \a deviceid is the id of the analog output device
        that you should use for initializing the TraceSpec. */
  virtual void addTraces( vector< TraceSpec > &traces, int deviceid ) const;

    /*! May match trace names with driver internal trace names,
        for example trace names of a dynamic clamp model.
        \return 0: no traces needed to be matched.
        \return n: \a n traces have been successfully matched.
	\return -1: Failed to match a trace. */
  virtual int matchTraces( vector< TraceSpec > &traces ) const;

    /*! The id of the analog output implementation.
        \sa setAnalogOutputType(), deviceType(), deviceName(), ident() */
  int analogOutputType( void ) const;


protected:

    /*! Test each output signal in \a sigs for valid settings.
	If an error ocurred in any signal, the corresponding errorflags in the
	InData are set and a negative value is returned.
	The following error conditions are checked:
        DeviceNotOpen, NoData (OutData::size() <= 0), 
	MultipleDevices, MultipleStartSources,
	MultipleDelays, MultipleSampleRates, MultipleContinuous, MultipleRestart,
	MultipleBuffersizes (OutData::size()), MultipleBufferTimes, 
	InvalidStartSource (OutData::startSource() < 0),
	InvalidDelay (OutData::delay() < 0),
	InvalidSampleRate (OutData::sampleRate() > maxRate() or < 1.0), 
	InvalidChannel (OutData::channel() >= channels() or < 0), 
	MultipleChannels.
        This function is called by testWrite(). */
  virtual int testWriteData( OutList &sigs );
    /*! Device driver specific tests on the settings in \a sigs
        for each output signal.
	Before this function is called, the validity of the settings in 
	\a sigs was already tested by testReadData().
	testWriteDevice() is called even if an error was detected by
	testWriteData().
	This function should test whether the settings are really supported
	by the hardware.
	If an error ocurred in any trace, the corresponding errorflags in the
	OutData are set and a negative value is returned.
	In addition, according to OutData::requestedMin() and 
	OutData::requestedMax(), the gain, the polarity, the reference,
	and the minimum and maximum possible output voltages for each
	of the traces in \a sigs should be set (see OutData::setGain(),
	OutData::setGainIndex(), OutData::setMaxData(), OutData::setMinData() ).
        The channels in \a sigs are not sorted.
        This function is called by testWrite(). */
  virtual int testWriteDevice( OutList &sigs ) = 0;

    /*! Set id of the analog output implementation to \a aotype.
	This function should be called in the constructor
	of an implementation of AnalogOutput.
        \sa analogOutputType(), setDeviceType(), setDeviceName(), setIdent() */
  void setAnalogOutputType( int aotype );

    /*! Set the settings() string for \a sigs.
        Call this function from within a successful prepareWrite().
	\param[in] writebuffer is the size of the driver's buffer in bytes.
        \sa settings() */
  void setSettings( const OutList &sigs, int writebuffer=0 );


private:

   /*! Device type id for all analog output devices. */
  static const int Type = 2;

    /*! The type of the implementation of AnalogOutput. */
  int AnalogOutputType;
  double ExternalReference;

};


}; /* namespace relacs */

#endif /* ! _RELACS_ANALOGOUTPUT_H_ */
