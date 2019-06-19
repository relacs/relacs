/*
  analoginput.h
  Interface for accessing analog input of a data-aquisition board.

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

#ifndef _RELACS_ANALOGINPUT_H_
#define _RELACS_ANALOGINPUT_H_ 1

#include <vector>
#include <QThread>
#include <QReadWriteLock>
#include <QSemaphore>
#include <QWaitCondition>
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
\todo add probe function that returns a string of possible supported devices.
\todo add directread function for single point acquisition
\todo add a flag for indicating whether device is capable of streaming input

In case you want to use a analog input device within RELACS, your
%AnalogInput implementation needs to provide a void default constructor
(i.e. with no parameters) that does not open the device.  Also,
include the header file \c \<relacs/relacsplugin.h\> and make the
AnalogInput device known to RELACS with the \c addAnalogInput(
ClassNameOfYourAnalogInputImplementation, PluginSetName ) macro.
*/

class AnalogInput : public Device, protected QThread
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
  virtual int open( const string &device ) override;
    /*! Open the analog input device specified by \a device.
	Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close(), reset() */
  virtual int open( Device &device) override;
    /*! Returns true if the device is open.
        \sa open(), close(), reset() */
  virtual bool isOpen( void ) const = 0;
    /*! Stop all activity and close the device.
        \sa open(), isOpen(), reset() */
  virtual void close( void ) = 0;

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

    /*! \return the gain index for the minimum gain (largest range)
        available for inputs of polarity \a unipolar. \sa maxGainIndex(), gainIndex() */
  int minGainIndex( bool unipolar ) const;
    /*! \return the gain index for the maximum gain (smallest range)
        available for inputs of polarity \a unipolar. \sa minGainIndex(), gainIndex() */
  int maxGainIndex( bool unipolar ) const;
    /*! \return the gain index for the largest gain (smallest range)
        available for inputs of polarity \a unipolar that just fits \a maxvoltage.
        \sa minGainIndex(), maxGainIndex() */
  int gainIndex( bool unipolar, double maxvoltage ) const;

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
	\param[in] sp if not null, a thread is started reading out the running analog input.
        When the thread and analog input is finished, releases the semaphore by one.
        On error, the semaphore is released by 1000 so that the process waiting
        on the semaphore is waking up immediately.
	\param[in] datamutex the mutex for locking the input traces that were passed 
        to the previous call of prepareRead().
	\param[in] datawait a waitcondition that is waken up whnever
	the thread put new data into the input traces.
	\param[in] aosp is the semaphore that guards analog outputs.
	if \c null, then do not start analog outputs.
	\return -1 on failure, 0 on success and 1 on succes and if
	for an analog output further calls to writeData() are necessary.
        This function is called after a successfull prepareRead() or after stop().
        This function should be as quick as possible. */
  virtual int startRead( QSemaphore *sp=0, QReadWriteLock *datamutex=0,
			 QWaitCondition *datawait=0, QSemaphore *aosp=0 ) = 0;
    /*! Read data from a running data acquisition
        and store them in an internal buffer.
        Returns the total number of read data values.
	If an error ocurred in any channel, the corresponding errorflags in the
	InData structure are filled and a -2 is returned.
	If no acquisition is running and therefore no more data are to be expected,
	-1 is returned.
        This function is called periodically after reading has been successfully
        started by startRead() and the device mutex is already locked.
        This function does not modify the traces passed to prepareRead()! */
  virtual int readData( void ) = 0;
    /*! Convert the acquired data from the internal buffer
        to numbers in the secondary unit and store the data
	in the traces that were passed to the previous call of prepareRead().
        Returns the number of data values that were added to the traces
	(sum over all traces).
	If an error ocurred in any channel, the corresponding errorflags in the
	InData structure are filled and a negative value is returned.
        This function is called periodically after one or several calls to readData()
	and the device mutex is already locked. */
  virtual int convertData( void ) = 0;

    /*! Compute a dynamic clamp model.
        An implementation is only needed for an analog input simulation. */
  virtual void model( InList &data,
		      const vector< int > &aochannels, vector< float > &aovalues );

    /*! Stop any running ananlog input activity,
        but preserve all so far read in data.
	The next call to readData() will return the data.
	Should call stopRead() to stop the thread.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, ReadError.
        \sa reset(), startRead() */
  virtual int stop( void ) = 0;
    /*! Clear any internal data buffers and reset the device.
        Assumes that analog input is already stopped.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, ReadError.
        \sa stop(), close(), open(), isOpen() */
  virtual int reset( void ) = 0;

    /*! \return true if analog input is running.
        The default implementation checks whether the thread is still running.
        In your implementation you should use this information as well. */
  virtual bool running( void ) const;

    /*! Check for every analog input and analog output device in \a ais
        and \a aos, respectively,
        whether it can be simultaneously started by startRead()
	from this device.
	Add the indices of those devices to \a aiinx and \a aoinx,
        respectively.
        The corresponding entries in \a airate and \a aorate should
        be set to \c true if these devices should
        use the same sampling rate as this. */
  virtual void take( const vector< AnalogInput* > &ais,
		     const vector< AnalogOutput* > &aos,
		     vector< int > &aiinx, vector< int > &aoinx,
		     vector< bool > &airate, vector< bool > &aorate );

    /*! If the analog input device supports inputs that
        are not physical input lines but rather readable internal variables,
        like model outputs of a dynamic clamp modul,
        then reimplement this function. 
        Add for each such variable a TraceSpec to \a traces.
        \a deviceid is the id of this analog input device
        that you should pass as the \a device to TraceSpec. */
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
    /*! Set the time for sleeping between calls of readData() to \a ms milliseconds. */
  void setReadSleep( unsigned long ms );

    /*! Set the device info().
        Call this function from open().
        \sa info() */
  void setInfo( void );
    /*! Set the settings() for \a traces.
        Call this function from within a successful prepareRead().
	\param[in] traces the input traces for which the settings string should be constructed.
	\param[in] fifobuffer is the size of the hardware's fifo buffer in bytes.
	\param[in] pluginbuffer is the size of the buffer used by the plugin in bytes.
        \sa settings() */
  void setSettings( const InList &traces, int fifobuffer=0,
		    int pluginbuffer=0 );

    /*! Start the thread if \a sp is not null.
        If \a error do not start the thread and release the semaphore \a sp. */
  virtual void startThread( QSemaphore *sp = 0, QReadWriteLock *datamutex=0,
			    QWaitCondition *datawait=0, bool error=false );
    /*! The thread reading data from a running analog input. */
  virtual void run( void );
    /*! Stop the running thread. */
  virtual void stopRead( void );


private:

    /*! The type of the implementation of AnalogInput. */
  int AnalogInputSubType;
    /*! True while the thread is running. */
  bool Run;
    /*! Sleeping between reading data. */
  QWaitCondition SleepWait;
    /*! A semaphore guarding analog input. */
  QSemaphore *Semaphore;
    /*! A mutex locking the data buffer where the acquired data is stored to. */
  QReadWriteLock *DataMutex;
    /*! A waitcondition that is woken up whenever new data are written to the buffer. */
  QWaitCondition *DataWait;
    /*! Milliseconds to sleep between calls of readData(). Defaults to 0. */
  unsigned long ReadSleepMS;

};


}; /* namespace relacs */

#endif /* ! _RELACS_ANALOGINPUT_H_ */
