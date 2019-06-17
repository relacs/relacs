/*
  analogoutput.h
  Interface for accessing analog output of a data-aquisition board.

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

#ifndef _RELACS_ANALOGOUTPUT_H_
#define _RELACS_ANALOGOUTPUT_H_ 1

#include <vector>
#include <QThread>
#include <QSemaphore>
#include <QWaitCondition>
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
\brief Interface for accessing analog output of a data-aquisition board.
\todo add probe function that returns a string of possible supported devices.
\todo add a flag for indicating whether device is capable of streaming output

In case you want to use a analog output device within RELACS, your
%AnalogOutput implementation needs to provide a void default constructor
(i.e. with no parameters) that does not open the device.  Also,
include the header file \c \<relacs/relacsplugin.h\> and make the
AnalogOutput device known to RELACS with the \c addAnalogOutput(
ClassNameOfYourAnalogInputImplementation, PluginSetName ) macro.
*/

class AnalogOutput : public Device, protected QThread
{

public:

    /*! Return values for analog output status(). */
  enum Status { 
      /*! Not running and no error. */
    Idle=0,
      /*! Anaolog output is still in progress. */
    Running=1,
      /*! Buffer underrun error. */
    Underrun=2,
      /*! Any other error. */
    UnknownError=3
  };
  
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
	A reimplementation of this function should extract the
	following options from opts if provided:
	- \c extref: the external reference voltage to be passed to setExternalReference().
        \sa isOpen(), close(), reset() */
  virtual int open( const string &device )  override;
    /*! Open the analog input device specified by \a device.
	Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close(), reset() */
  virtual int open( Device &device )  override;
    /*! Returns true if the device is open.
        \sa open(), close(), reset() */
  virtual bool isOpen( void ) const = 0;
    /*! Stop all activity and close the device.
        \sa open(), isOpen(), reset() */
  virtual void close( void ) = 0;

    /*! Number of analog output channels. */
  virtual int channels( void ) const = 0;
    /*! Resolution in bits of analog output. */
  virtual int bits( void ) const = 0;
    /*! Maximum sampling rate in Hz of analog output. */
  virtual double maxRate( void ) const = 0;

    /*! Delay in seconds from calling startWrite() 
        to the actual signal start of channel \a channel. */
  double delay( int channel ) const;
    /*! Set for each channel the delay in seconds it takes from
        calling startWrite() to the actual signal start.
        If there are such delays, this function should be called in
	the implementation of the open() function to set them.
	\sa setDelay() */
  void setDelays( const vector<double> &delays );
    /*! Set the delay in seconds it takes from calling startWrite() 
        to the actual signal start for all channels to \a delay.
        If there are such delays, this function should be called in
	the implementation of the open() function to set them.
	\sa setDelays() */
  void setDelay( double delay );

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

    /*! Directly writes from each signal in \a sigs the first data value
        to the data acquisition board. */
  virtual int directWrite( OutList &sigs ) = 0;

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
    /*! Prepare analog output of the output signals \a sigs on the device.
	If an error ocurred in any signal, the corresponding errorflags in
	OutData are set and a negative value is returned.
	In addition, according to OutData::requestedMin() and 
	OutData::requestedMax(), the gain, the polarity, the reference,
	and the minimum and maximum possible output voltages for each
	of the traces in \a sigs should be set (see OutData::setGain(),
	OutData::setGainIndex(), OutData::setMaxData(), OutData::setMinData() ).
	This function assumes that \a sigs successfully passed testWrite().
        The channels in \a sigs are not sorted. */
  virtual int prepareWrite( OutList &sigs ) = 0;
    /*! Start non-blocking analog output of the output signals
        that were passed to the previous call of prepareWrite().
	If an error ocurred in any signal, the corresponding errorflags in
	OutData are set and a negative value is returned.
	If no further calls of writeData() are required, 0 is returned,
	otherwise 1 is returned.
	Also start possible pending acquisition on other devices
	that are known from take().
        This function is always called after a successfull prepareRead().
	An implementation of startWrite() should call startThread() to
	start the thread.
	\param[in] sp if not null, a thread is started feeding the
        running analog output.  When the thread and analog output is
        finished, releases the semaphore by one.  On error, the
        semaphore is released by 1000 so that the process waiting on
        the semaphore is waking up immediately. */
  virtual int startWrite( QSemaphore *sp=0 ) = 0;
    /*! Write data of the output signals that were passed to the previous call
        of prepareWrite() to the analog output device.
        Returns the time in milliseconds corresponding to the transferred
	data elements (can be zero).
	Returns -1 if all data are transferred and no more calls to
	writeData() are necessary.
	If an error ocurred in any channel, the corresponding
	errorflags in the OutData structure are filled, the error
	string is set, and a -2 is returned.
        This function is called periodically after writing has been successfully
        started by startWrite() and the device mutex is already locked.
        \sa setErrorStr() */
  virtual int writeData( void ) = 0;

    /*! Stop any running ananlog output activity.
        Assumes that analog output is open.
	Should call stopWrite() to stop the thread.
        Returns zero on success.
        \sa reset(), startWrite() */
  virtual int stop( void ) = 0;
    /*! Clear any internal data buffers and reset the device.
        Assumes that analog output is open and already stopped.
        Returns zero on success.
	An implementation should NOT clear the error string!
        \sa stop(), close(), open(), isOpen() */
  virtual int reset( void ) = 0;
  
    /*! \return the status of the analog output.
        If an error is detected, this function could also set the appropriate error code
	in the signals.
        The device is already locked. */
  virtual Status statusUnlocked( void ) const = 0;
    /*! \return the status of the analog output.
        The device is locked and statusUnlocked() is called. */
  Status status( void ) const;
  
    /*! \return true if the analog input thread is running. */
  bool running( void ) const;

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
    /*! Return \c true if the analog output is required to have the
        same sampling rate as the analog input returned by getAISyncDevice().
        The default implementation returns \c false. */
  virtual bool useAIRate( void ) const;

    /*! Check for every analog output device in \a aos
        whether it can be simultaneously started by startWrite()
	from this device.
	Add the indices of those devices to \a aoinx.
        The corresponding entries in \a aorate should
        be set to \c true if these devices should
        use the same sampling rate as this. */
  virtual void take( const vector< AnalogOutput* > &aos,
		     vector< int > &aoinx, vector< bool > &aorate );

    /*! If the analog output device supports outputs that
        are not physical output lines but rather writeable parameter,
        like model parameter for a dynamic clamp modul,
        then reimplement this function. 
        Add for each such parameter a TraceSpec to \a traces.
        \a deviceid is the id of the analog output device
        that you should pass as the \a device to TraceSpec. */
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
	OutData are set and a negative value is returned.
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
    /*! Set the maximum time for sleeping between calls to writeData() 
        to \a ms milliseconds. */
  void setWriteSleep( int ms );
    /*! The maximum time in milliseconds used for sleeping between calls to writeData(). */ 
  int writeSleep( void ) const;

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
    /*! If sigs.failed(), set error string to the error set in \a sigs
        otherwise clear the error string. \sa addErrorStr() */
  void setErrorStr( const OutList &sigs ) const;
    /*! If sigs.failed(), add error string to the error set in \a sigs.
        \sa setErrorStr() */
  void addErrorStr( const OutList &sigs ) const;

    /*! Set the device info().
        Call this function from open().
        \sa info() */
  void setInfo( void );
    /*! Set the settings() for \a sigs.
        Call this function from within a successful prepareWrite().
	\param[in] sigs the output data fro which the settings string should be constructed.
	\param[in] writebuffer is the size of the driver's buffer in bytes.
        \sa settings() */
  void setSettings( const OutList &sigs, int writebuffer=0 );

    /*! Start the thread if \a sp is not null.
        If \a error do not start the thread and release the semaphore \a sp. */
  virtual void startThread( QSemaphore *sp = 0, bool error=false );
    /*! The thread feeding data to a running analog output. */
  virtual void run( void );
    /*! Stop the running thread. */
  virtual void stopWrite( void );


private:

    /*! The type of the implementation of AnalogOutput. */
  int AnalogOutputSubType;
    /*! Delay in seconds from calling startWrite() 
        to the actual signal start for each analog output channel. */
  vector< double > Delays;
    /*! Value of the external reference in Volt. */
  double ExternalReference;
    /*! True while the thread is running. */
  bool Run;
    /*! Sleeping between writing data. */
  QWaitCondition SleepWait;
    /*! A semaphore guarding analog output. */
  QSemaphore *Semaphore;
    /*! Milliseconds to sleep between calls of writeData(). Defaults to 0. */
  int WriteSleepMS;

};


}; /* namespace relacs */

#endif /* ! _RELACS_ANALOGOUTPUT_H_ */
