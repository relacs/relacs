/*
  acquire.h
  Read and write data from/to the data aqcuisition board.

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

#ifndef _RELACS_ACQUIRE_H_
#define _RELACS_ACQUIRE_H_ 1

#include <string>
#include <ctime>
#include <vector>
#include <deque>
#include <QReadWriteLock>
#include <QSemaphore>
#include <QWaitCondition>
#include <relacs/tracespec.h>
#include <relacs/inlist.h>
#include <relacs/outlist.h>
#include <relacs/eventlist.h>
#include <relacs/analoginput.h>
#include <relacs/analogoutput.h>
#include <relacs/attenuate.h>

using namespace std;

namespace relacs {


/*! 
\class Acquire
\author Jan Benda
\brief Read and write data streams from/to data aqcuisition boards.
\todo Overflow in InData::Index (after 7h!) -> reset InData!
\todo internal list of Attenuate that corresponds to outTraces for faster access.
\todo write(), testWrite(): check carrier frequency?

Acquire is a general interface to data acquisition boards
that integrates multiple analog input, analog output,
and attenuator device interfaces.
Input and output data together with the specifications for the
data acquisition boards are communicated via the high level
classes InData and OutData.

For communication with the data acquisition boards the 
AnalogInput and AnalogOutput interfaces are used.
Acquisition devices must be added to Acquire with the functions addInput()
and addOutput() for analog input and output, respectively.
Attenuators are operated via the Attenuate interface.
They must be added with the addAttLine() function to Acquire.

The number of known (added) data acquisition devices 
can be retrieved by inputsSize(), outputsSize(), and attLinesSize().
The list of devices can be cleared by clearInputs(), clearOutputs(),
and clearAttLines().
Devices can be closed with closeInputs(), closeOutputs(), and closeAttLines().

Analog input is initiated by read(). 
It takes an InList as argument. The settings of the individual
InData specify the parameter (sampling rate, channels, ...)
to be used for the analog input operation.
Before calling read(), testRead() can be used to check if all parameter are valid.
stopRead() stops any activity related to analog input.
inTraces() returns a list of all available analog input traces.

The gain settings for the ananlog input lines can be modified by
the setGain() and adjustGain() functions.
The new settings are automatically activated by the next call of write()
or by explicitly calling activateGains().

Analog output is performed by write().
Its OutData or OutList argument specifies the parameter (sampling rate, channels, delay, ... )
for the analog output operation and must not be modified or deleted
while output is active.
Before calling write(), testWrite() can be used to check if all parameter are valid.
With writeZero() the output of a specified channel is set to zero.
stopWrite() stops any activity related to analog output.

A list of possible analog output traces and their properties
can be created with addOutTrace().
By using this list via applyOutTrace() and outTrace(),
analog output traces can be accessed by a trace name or trace index
(OutData::setTraceName() and OutData::setTrace(), respectively)
rather then specifying the output device and channel directly
(OutData::setDevice() and OutData::setChannel()).

Depending on the hardware drivers, different strategies for
synchronizing the analog output with the analog input are used
as reported by syncMode().

Any analog input and analog output activity is stopped altogether by stop().
*/


class Acquire
{

public:

    /*! Methods for synchronizing analog output with analog input. */
  enum SyncModes {
      /*! No synchronization method provided. Simple start AI and AO together. */
    NoSync,
      /*! AI and AO are started simultaneously by the driver. */
    StartSync,
      /*! Continuous AI. AO reads out AI counter. */
    AISync
  };

    /*! Construct an empty Acquire.
        \sa addInput(), addOutput(), addAttLine(), addOutTrace() */
  Acquire( void );
    /*! Deconstruct an Acquire.
        Does NOT close the hardware drivers.
        \sa close(), clear() */
  virtual ~Acquire( void );

    /*! Add the analog input device \a ai to
	the list of analog input devices.
	\param[in] ai the analog input device to be added
        \return
	- 0 on success.
	- -1: \a ai == 0
	- -2: \a ai not opened
	\sa inputsSize(), inputDevice(), inputTraces(), clearInputs(),
	closeInputs(), addOutput(), addAttenuate(), addOutTrace() */
  int addInput( AnalogInput *ai );
    /*! The number of device drivers for analog input
        stored in this Acquire. 
	\sa addInput(), inputDevice(), inputTraces(), clearInputs(),
	closeInputs(), outputsSize(), attLinesSize(),
	outTracesSize() */
  int inputsSize( void ) const;
    /*! The index of the analog input device with identifier \a ident. 
        \sa addInput() */
  int inputIndex( const string &ident ) const;
    /*! The ananlog input device \a deviceindex.
	\sa addInput(), inputsSize(), inputTraces(), clearInputs(), closeInputs() */
  const AnalogInput *inputDevice( int deviceindex ) const;
    /*! The input traces acquired from ananlog input device \a deviceindex.
	\sa addInput(), inputsSize(), inputDevice(), clearInputs(),
	closeInputs() */
  const InList &inputTraces( int deviceindex ) const;
    /*! Stop analog input, clear the list of analog input devices
        without closing the devices. 
	\sa addInput(), inputsSize(), inputDevice(), inputTraces(),
	closeInputs(), clearOutputs(), clearAttLines(),
	clearOutTraces() */
  void clearInputs( void );
    /*! Stop analog input, close all devices from the list of analog
	input devices and clear the list.
	\sa addInput(), inputsSize(), inputDevice(), inputTraces(),
	clearInputs(), closeOutputs(), closeAttLines() */
  void closeInputs( void );

    /*! Add the analog output device \a ao to
	the list of analog output devices.
        \return
	- 0 on success.
	- -1: \a ao == 0
	- -2: \a ao not opened
	\sa outputsSize(), clearOutputs(), closeOutputs(),
	addInput(), addAttLine(), addOutTrace() */
  int addOutput( AnalogOutput *ao );
    /*! The number of device drivers for analog output
        stored in this Acquire.
	\sa addOutput(), clearOutputs(), closeOutputs(),
	inputsSize(), attLinesize(), outTracesSize() */
  int outputsSize( void ) const;
    /*! The index of the analog output device with identifier \a ident. 
        \sa addOutput(), outTraceIndex() */
  int outputIndex( const string &ident ) const;
    /*! Stop analog output, clear the list of analog output devices
        without closing the devices.
	\sa addOutput(), outputsSize(), closeOutputs(),
	clearInputs(), clearAttLines(), clearOutTraces() */
  void clearOutputs( void );
    /*! Stop analog output, close all devices from the list
        of analog output devices and clear the list.
	\sa addOutput(), oututsSize(), clearOutputs(),
	closeInuts(), closeAttLines(), closeOutTraces() */
  void closeOutputs( void );
    /*! Set the delay in seconds it takes from starting analog output 
        to the actual signal start for all channels of the
	analog output device \a device to \a delay. */
  void setSignalDelay( int device, double delay );

    /*! Add the attenuator \a att to the list of attenuators.
        The attenuator is connected to the output channel
	\a channel of the device with id \a device.
        \return
	- 0 on success.
	- -1: \a att == 0
	- -2: \a att not opened
	- -3: \a channel < 0
	\sa attLinesSize(), clearAttLines(), closeAttLines(),
	addInput(), addOutput(), addOutTrace() */
  int addAttLine( Attenuate *att, const string &device="", int channel=-1 );
    /*! The number of attenuators stored in this Acquire.
	\sa addAttunate(), clearAttLines(), closeAttLines(),
	inputsSize(), oututsSize(), outTracesSize() */
  int attLinesSize( void ) const;
    /*! Clear the list of attenuators without closing the devices.
	\sa addAttunate(), attLinesSize(), closeAttLines(),
	clearInputs(), clearOutputs(), clearOutTraces() */
  void clearAttLines( void );
    /*! Close all devices from the list of attenuators
        and clear the list.
	\sa addAttunates(), attLinesSize(), clearAttunates(),
	closeInputs(), closeOutputs() */
  void closeAttLines( void );

    /*! Add an output trace with name \a name on channel \a channel of
        device \a device to the list of traces.
	The trace allows a maximum sampling rate of \a maxrate.
        The signal is given in unit \a unit and multiplied by \a scale
        to convert it to the voltage of the ananlog output.
	The initial value of the trace is \a value.
	The \a modality describes whether the output trace
	drivesacoustic, visual, electric, etc. stimuli.
        \sa addOutTraces(), outTracesSize(), outTraceIndex(),
	outTraceName(), outTrace(), applyOutTrace(), clearOutTraces(),
        addInput(), addOutput(), addAttLine() */
  void addOutTrace( const string &name, int device, int channel,
		    double scale=1.0, const string &unit="", double value=0.0,
		    double maxrate=-1.0, const string &modality="" );
    /*! Check every analog output device for additionaly provided 
        output channels and add them to the list of output traces.
	These output traces are usually no physical outputs but
	rather writeable parameter of the output device,
	like model parameter of a dynamic clamp modul.
        \sa addOutTrace(), outTracesSize(), outTraceIndex(),
	outTraceName(), outTrace(), applyOutTrace(), clearOutTraces(),
        inTraces() */
  void addOutTraces( void );
    /*! Return the number of output traces.
        \sa addOutTrace(), outTraceIndex(),
	outTraceName(), outTrace(), applyOutTrace(), clearOutTraces(),
        inputsSize(), outputsSize(), attLinesSize() */
  int outTracesSize( void ) const;
    /*! Return the index of the output trace with name \a name.
        If there isn't any trace with name \a name, -1 is returned.
        \sa addOutTrace(), outTracesSize(),
	outTraceName(), outTrace(), applyOutTrace(), clearOutTraces(),
        outputIndex() */
  int outTraceIndex( const string &name ) const;
    /*! Return the name of the output trace with index \a index.
        If \a index is invalid, an empty string is returned.
        \sa addOutTrace(), outTracesSize(), outTraceIndex(),
	outTrace(), applyOutTrace(), clearOutTraces() */
  string outTraceName( int index ) const;
    /*! Return the Attenuate class that is connected to
        the output trace with index \a index.
        If \a index is invalid or no Atenuate class is connected to the 
	output trace, then 0 is returned.
        \sa outTraceName() */
  Attenuate *outTraceAttenuate( int index );
  const Attenuate *outTraceAttenuate( int index ) const;
    /*! Return the output trace with index \a index.
        \sa addOutTrace(), outTracesSize(), outTraceIndex(),
	outTraceName(), applyOutTrace(), clearOutTraces() */
  const TraceSpec &outTrace( int index ) const;
    /*! Return the output trace with name \a name.
        \sa addOutTrace(), outTracesSize(), outTraceIndex(),
	outTraceName(), applyOutTrace(), clearOutTraces() */
  const TraceSpec &outTrace( const string &name ) const;
    /*! If \a signal has an output trace specified (via
        OutData::setTrace() or OutData::setTraceName()) then the
        corresponding settings of this trace (channel, device, ...)
        are applied to \a signal.
	\return 
	-  0: success
	- -1: traceName() is empty or trace() < 0
	- -2: trace does not exist
	- -3: invalid match
        \sa addOutTrace(), outTracesSize(), outTraceIndex(),
	outTraceName(), outTrace(), clearOutTraces() */
  int applyOutTrace( OutData &signal ) const;
    /*! Applies the trace settings to each of the OutData in \a signals.
	If the trace is not known, the DaqError::InvalidTrace
	error flag of the corresponding \a signal is set.
	\return
        -  0: success
	- -1: invalid trace name in at least one of the signals
	- -2: invalid match in at least one of the signals
        \sa addOutTrace(), outTracesSize(), outTraceIndex(),
	outTraceName(), outTrace(), clearOutTraces() */
  int applyOutTrace( OutList &signal ) const;
    /*! Clear the list of output traces.
        \sa addOutTrace(), outTracesSize(), outTraceIndex(),
	outTraceName(), outTrace(), applyOutTrace(),
        clearInputs(), clearOutputs(), clearAttLines() */
  void clearOutTraces( void );

    /*! Add and initialize stimulus events to \a events. */
  void addStimulusEvents( InList &data, EventList &events );
    /*! Add and initialize restart events to \a events. */
  void addRestartEvents( InList &data, EventList &events );

    /*! Add all available traces of each analog input device
        to \a traces.
        \sa addOutTraces() */
  void inTraces( vector< TraceSpec > &traces );

    /*! Stop all input and output activity.
        Clear all device lists without closing the devices
	and clear the analog output traces.
        \sa close(), clearIn(), clearOut(), clearAtt(), clearOutTraces() */
  void clear( void );
    /*! Stop all input and output activity.
        Close all hardware drivers and clear the lists
	and the analog output traces.
        \sa clear(), closeIn(), closeOut(), closeAtt(), clearOutTraces() */
  void close( void );

    /*! Check and initialize the analog input and output devices
        for supported mechanisms of synchronisation. */
  void initSync( void );
    /*! The method that is used for synchronizing analog output with analog input. */
  SyncModes syncMode( void ) const;
    /*! A string describing the method that is used
        for synchronizing analog output with analog input. */
  string syncModeStr( void ) const;

    /*! Inform the analog input and output devices
        about the requested input and output traces. */
  //  void initTraces( InList &data, OutList &signals );

    /*! Test of analog input settings given by \a data for validity.
	Returns 0 on success, negative numbers otherwise.
        Possible errors are indicated by the error state of \a data.
        If some parameters in \a data are invalid there are set to valid values
	so that a following call of read( InList &data ) will succeed. 
        \sa read(), stopRead() */
  virtual int testRead( InList &data );
    /*! Start analog input with the settings given by \a data. 
	Returns 0 on success, negative numbers otherwise.
        Possible errors are indicated by the error state of \a data. 
        \sa testRead(), stopRead() */
  virtual int read( InList &data );
    /*! \return an error string describing problems that occured during analog input. */
  string readError( void ) const;
    /*! Stop analog input of all analog input devices.
	Returns 0 on success, negative numbers otherwise. 
        \sa testRead(), read(), readData(), convertData() */
  virtual int stopRead( void );
    /*! Restart data aquisition in case of an error. */
  virtual int restartRead( void );
    /*! Wait for the analog input threads to finish.
        \return 0 on success, i.e. all analog inputs finished successfully
	or -1 if some input failed. */
  virtual int waitForRead( void );
    /*! \return \c true if all the threads acquiering data are still running. */
  bool isReadRunning( void ) const;
    /*! Updates the raw traces of the lists \a datalist and \a eventslist
        and of \a data and \a events
        to the current state of the data buffers.
        Also set \a signaltime to the time of the most recent output signal.
	If \a mintracetime is greater than zero getRawData() waits until the input
        traces of the currently running acquisition contain a minimum
        number of data elements.  Returns immediately in case of
        errors or the acquisition was stopped.
	\param[in] mintracetime If \a mintracetime is greater than zero,
	blocks until data upto \a mintracetime seconds are available.
	\param[in] prevsignal If in addition \a prevsignal is greater than zero,
	first block until the time of the last signal is greater than \a prevsignal
	and afterwards until data until the signal time plus \a mintracetime are available.
        \return \c 1 if the input traces contain the required data,
	\c 0 if interrupted, or \c -1 on error. */
  int getRawData( InList &data, EventList &events, double &signaltime,
		  double mintracetime, double prevsignal );
    /*! Wait for new data and set \a signaltime to the time of the most recent output signal
	or to -1, if there wasn't any new output signal.
        \return \c 1 if the input traces got new data,
	\c 0 if no more data are available, or \c -1 on error. */
  int waitForData( double &signaltime );
    /*! Lock the input data for reading. \sa unlockRead(), lockWrite() */
  void lockRead( void );
    /*! Unlock the input data. \sa lockRead(), unlockWrite() */
  void unlockRead( void );

    /*! \return the flag that is used to mark traces whose gain was changed. 
        \sa setAdjustFlag(), setGain(), adjustGain(), gainChanged(), activateGains() */
  int adjustFlag( void ) const;
    /*! Set the flag which is used to mark traces whose gain was changed to \a flag. 
        \sa adjustFlag(), setGain(), adjustGain(), gainChanged(), activateGains() */
  void setAdjustFlag( int flag );

    /*! The list of input ranges the DAQ board supports for the input trace \a data. 
        \param[in] data: the input trace that specifies the input device, channel,
	and unipolar/bipolar mode.
	\param[out] ranges: the list of available ranges specified as the maximum
	voltage that can beacquired. The corresponding index can be passed directly to setGain().
	On failure an empty list is returned.
	\return 0 in succes, otherwise an DAQError code. */
  int maxVoltages( const InData &data, vector<double> &ranges ) const;
    /*! The list of input ranges the DAQ board supports for the input trace \a data. 
        \param[in] data: the input trace that specifies the input device, channel,
	and unipolar/bipolar mode.
	\param[out] ranges: the list of available ranges specified as the maximum
	values measured in data.unit(). The corresponding index can be passed directly to setGain().
	On failure an empty list is returned.
	\return 0 in succes, otherwise an DAQError code. */
  int maxValues( const InData &data, vector<double> &ranges ) const;

    /*! Set the gain for input trace \a data to \a gainindex.
        It depends on your hardware what gain is corresponding to \a gainindex.
        In order to activate the new gain you have to call activateGains().
	Returns 0 on success, negative numbers otherwise.
        \sa adjustGain( InData, double, double ), 
	adjustGain( InData, double ), activateGains() */
  int setGain( const InData &data, int gainindex );
    /*! Adjust the gain for input trace \a data 
        such that the maximum value of \a maxvalue is within the
        input range.
        In order to activate the new gain you have to call activateGains().
	Returns 0 on success, negative numbers otherwise.
        \sa setGain(), adjustGain( InData, double, double ), activateGains() */
  int adjustGain( const InData &data, double maxvalue );
    /*! Increases or decreases the gain for input trace \a data
        by one step.
        If \a maxvalue is larger than the maximum possible value
        of the current input range times, then the gain is decreased.
        If \a minvalue is smaller than the maximum possible value
        of the following input range, then the gain is increased.
        In order to activate the new gain you have to call activateGains().
	Returns 0 on success, negative numbers otherwise.
        \sa setGain(), adjustGain( InData, double ), activateGains() */
  int adjustGain( const InData &data, double minvalue, double maxvalue );
    /*! Returns \c true if the gain of at least one input trace
        was requested to be changed.
	\sa adjustFlag(), setGain(), adjustGain(), activateGains() */
  bool gainChanged( void ) const;
    /*! Activates the new gain settings for analog input traces
        as set by the setGain() and adjustGain() functions
        immediately by restarting the analog input.
	\note If you requested to change the gain via setGain() or adjustGain()
	then a subsequent call of write() will activate the new gain settings
	automatically.
        \sa setGain(), adjustGain( InData, double ),
	adjustGain( InData, double, double ), gainChanged() */
  virtual int activateGains( void );

    /*! Test of a single output signal \a signal for validity.
        If \a signal is the same as from the last call of write()
	(on same device with the same address),
	the device is also checked for acquisition errors.
        Returns 0 on success or a negative number if \a signal
        is not valid.
	The error state of \a signal is set appropriately.
        \sa write(), writeData(), writeZero(), stopWrite() */
  virtual int testWrite( OutData &signal );
    /*! Test of a multiple output signals \a signal for validity.
        If the signals \a signal are the same as from the last call of write()
	(on same devices with the same addresses),
	the devices are also checked for acquisition errors.
        Returns 0 on success or a negative number if \a signal
        is not valid.
        The error state of \a signal is set appropriately.
        \sa write(), writeData(), writeZero(), stopWrite() */
  virtual int testWrite( OutList &signal );

    /*! Output of a signal \a signal.
        See OutData about how to specify output channel, sampling rate, 
	intensity, delay, etc. 
	If input gains are requested to be changed,
	analog input is restarted with the new gain settings
	right before the signal is put out.
	If \a setsignal is set \c true (default) then the start time of this
	signal can be retrieved by readSignal().
	Returns 0 or 1 on success.
	If 1 is  returned writeData() needs to be called to further fill up the write buffer.
        If the output of the signal failed, a negative number is returned and
        the reason is specified in the error state of \a signal.
	\note During the output of the stimulus, \a signal must exist and must not be modified!
        \sa testWrite(), writeData(), writeZero(), stopWrite() */
  virtual int write( OutData &signal, bool setsignaltime=true );
    /*! Output of multiple signals \a signal.
        See OutData about how to specify output channel, sampling rate, 
	intensity, delay, etc. 
	If input gains are requested to be changed,
	analog input is restarted with the new gain settings
	right before the signal is put out.
	If \a setsignal is set \c true (default) then the start time of this
	signal can be retrieved by readSignal().
	Returns 0 or 1 on success.
	If 1 is  returned writeData() needs to be called to further fill up the write buffer.
        If the output of the signals failed, a negative number is returned and
        the reason is specified in the error state of \a signal.
	\note During the output of the stimulus, \a signal must exist and must not be modified!
        \sa testWrite(), writeData(), writeZero(), stopWrite() */
  virtual int write( OutList &signal, bool setsignaltime=true );

    /*! Wait for the analog output threads to finish.
        \return 0 on success, i.e. all analog outputs finished successfully
	or -1 if some output failed. */
  virtual int waitForWrite( void );
    /*! \return \c true if all the threads writing data are still running. */
  bool isWriteRunning( void ) const;
    /*! Lock the output signals for reading. \sa unlockWrite(), lockRead() */
  void lockWrite( void );
    /*! Unlock the output signals. \sa lockWrite(), unlockRead() */
  void unlockWrite( void );

    /*! Direct output of a single data value as specified by \a signal
        to the DAQ boards.
	Only the output trace ( OutData::setTrace() ) or the the name of the
	output trace ( OutData::setTraceName() ), as well as the
	single data value need to be specified.
	If \a setsignal is set \c true (default) then the start time of this
	signal can be retrieved by readSignal().
	\return 0 on success, a negative number if the output of the signal
	failed. The reason for the failure is specified in the error state
	of \a signal. */
  virtual int directWrite( OutData &signal, bool setsignaltime=true );
    /*! Direct output of single data values as specified by \a signal
        to different channels of the DAQ boards.
	Only the output traces ( OutData::setTrace() ) or the the name of the
	output traces ( OutData::setTraceName() ), as well as the
	single data values need to be specified.
	If \a setsignal is set \c true (default) then the start time of this
	signal can be retrieved by readSignal().
	\return 0 on success, a negative number if the output of the signals
	failed. The reason for the failure is specified in the error state
	of \a signal. */
  virtual int directWrite( OutList &signal, bool setsignaltime=true );

    /*! Set the output of channel \a channel on device \a device to zero.
        Returns 0 on success or a negative number on error. 
        \sa testWrite(), write(), writeData(), stopWrite() */
  virtual int writeZero( int channel, int device );
    /*! Set the output of the trace with index \a index to zero.
        Returns 0 on success or a negative number on error.
        \sa testWrite(), write(), writeData(), stopWrite() */
  virtual int writeZero( int index );
    /*! Set the output of the trace with name \a trace to zero.
        Returns 0 on success or a negative number on error.
        \sa testWrite(), write(), writeData(), stopWrite() */
  virtual int writeZero( const string &trace );

    /*! \return the start time of the last output signal relative to the input data. */
  double signalTime( void ) const;

    /*! \return an error string describing problems that occured
        during analog output. The error string is assembled from the
        error strings of the analog output devices - not from the
        signals. */
  string writeError( void ) const;

    /*! Stop analog output of all analog output devices.
        \sa testWrite(), write(), writeData(), writeZero() */
  virtual int stopWrite( void );

    /*! Returns the minimum possible attenuation level for the output trace
        at index \a trace.
        If \a trace is invalid or \a trace is not connected to an attenuator
        then OutData::NoLevel is returned.
        \sa maxLevel() */
  double minLevel( int trace ) const;
    /*! Returns the minimum possible attenuation level for output trace
        with name \a trace. */
  double minLevel( const string &trace ) const;
    /*! Returns the maximum possible attenuation level for the output trace
        at index \a trace.
        If \a trace is invalid or \a trace is not connected to an attenuator
        then OutData::NoLevel is returned.
        \sa minLevel() */
  double maxLevel( int trace ) const;
    /*! Returns the maximum possible attenuation level for output trace
        with name \a trace. */
  double maxLevel( const string &trace ) const;
    /*! Returns in \a l all possible attenuation levels
        sorted by increasing attenuation levels (highest last) 
	for the output trace at index \a trace.
        If \a trace is invalid or \a trace is not connected to an attenuator
        then \a l is returned empty.
        \sa minLevel(), maxLevel() */
  void levels( int trace, vector<double> &l ) const;
    /*! Returns in \a l all possible attenuation levels
        sorted by increasing attenuation levels (highest last) 
	for the output trace with name \a trace. */
  void levels( const string &trace, vector<double> &l ) const;

    /*! Returns the minimum possible intensity at the given stimulus
        \a frequency for the output trace at index \a trace.
        If \a trace is invalid or \a trace is not connected to an attenuator
        then OutData::NoIntensity is returned.
        \sa maxIntensity(), minLevel(), maxLevel() */
  double minIntensity( int trace, double frequency=0.0 ) const;
    /*! Returns the minimum possible intensity for output trace
        with name \a trace. */
  double minIntensity( const string &trace, double frequency=0.0 ) const;
    /*! Returns the maximum possible intensity at the given stimulus
        \a frequency for the output trace at index \a trace.
        If \a trace is invalid or \a trace is not connected to an attenuator
        then OutData::NoIntensity is returned.
        \sa minIntensity(), minLevel(), maxLevel() */
  double maxIntensity( int trace, double frequency=0.0 ) const;
    /*! Returns the maximum possible intensity for output trace
        with name \a trace. */
  double maxIntensity( const string &trace, double frequency=0.0 ) const;
    /*! Returns in \a ints all possible intensities at the given stimulus
        \a frequency sorted by increasing intensities 
	for the output trace at index \a trace.
        If \a trace is invalid or \a trace is not connected to an attenuator
        then \a intens is returned empty.
        \sa minIntensity(), maxIntensity() */
  void intensities( int trace, vector<double> &ints, double frequency=0.0 ) const;
    /*! Returns in \a ints all possible intensities at the given stimulus
        \a frequency sorted by increasing intensities 
	for the output trace with name \a trace. */
  void intensities( const string &trace, vector<double> &ints, double frequency=0.0 ) const;

    /*! Stop any activity related to
        analog output and analog input immediately. */
  virtual void stop( void );
  

protected:

    /*! Check for a new signal time and return it.
        \return the new signal time, -1.0 if there is no new signal. */
  virtual double getSignal( void );

    /*! \return a string with the current time. */
  string currentTime( void );

  struct AIData {
      /*! Construct an AIData. */
    AIData( AnalogInput *ai )
      : AI( ai ), AIDevice( -1 ), AIRate( false ) {};
      /*! Pointer to the interface to the hardware driver. */
    AnalogInput *AI;
      /*! The traces acquired from this device. */
    InList Traces;
      /*! New gain indices corresponding to \a Traces. */
    vector< int > Gains;
      /*! This analog input device will always be started 
	  by analog input device \a AIDevice (index to \a AI). */
    int AIDevice;
      /*! ... and has to use the same sampling rate. */
    bool AIRate;
  };
    /*! All devices for analog input. */
  vector < AIData > AI;
    /*! Semaphore guarding analog inputs. */
  QSemaphore AISemaphore;
    /*! Locks analog input data traces. */
  mutable QReadWriteLock ReadMutex;
    /*! Waits on new data in input traces. */
  QWaitCondition ReadWait;
    /*! The input data from the last read(). */
  InList InTraces;
    /*! The size of InTraces at the last updateRawData(). */
  double PreviousTime;
    /*! Count the successive calls of updateRawData() that did not get more data. */
  int NumEmptyData;

    /*! The flag that is used to mark adjusted traces in InData. */
  int AdjustFlag;

  struct AOData {
    AOData( AnalogOutput *ao )
      : AO( ao ), AISyncDevice( -1 ), AISyncRate( false ),
	AIDevice( -1 ), AIRate( false ), AODevice( -1 ), AORate( false ) {};
      /*! Pointer to the interface to the hardware driver. */
    AnalogOutput *AO;
      /*! The signals to be emitted from this device. */
    OutList Signals;
      /*! Output can be synced with running analog input device (index). */
    int AISyncDevice;
      /*! ... and has to use the same sampling rate. */
    bool AISyncRate;
      /*! This analog output device will always be started 
	  by analog input device \a AIDevice (index to \a AI). */
    int AIDevice;
      /*! ... and has to use the same sampling rate. */
    bool AIRate;
      /*! This analog output device will always be started 
	  by analog output device \a AODevice (index to \a AO). */
    int AODevice;
      /*! ... and has to use the same sampling rate. */
    bool AORate;
  };
    /*! All devices for analog output. */
  vector < AOData > AO;
    /*! Semaphore guarding analog outputs. */
  QSemaphore AOSemaphore;
    /*! Locks analog output signals. */
  mutable QReadWriteLock WriteMutex;
    /*! Index of last output device. */
  int LastDevice;
    /*! Time of last signal output. */
  double LastWrite;
    /*! Duration of last signal output. */
  double LastDuration;
    /*! Delay of last signal output. */
  double LastDelay;
    /*! The start time of the last signal that was put out. */
  double SignalTime;
    /*! The events recording the times in the input traces 
        where signals where put out. */
  EventData *SignalEvents;
    /*! The events recording the times in the input traces 
        where data acquisition was restarted. */
  EventData *RestartEvents;

    /*! Restart data aquisition and write signals 
        pending on devices in \a aos.
        If still running, stop analog input first.
	If \a directao, then the analog output signals are scheduled for direct outut.
        If \a updategains, the input gains are updated as well. 
	\return 0 on success and no further calls to writeData() are needed.
	\return 1 on success and further calls to writeData() are needed. 
	\return -1 on failure. */
  virtual int restartRead( vector< AOData* > &aod, bool directao,
			   bool updategains );

    /*! The currently used synchronization method. */
  SyncModes SyncMode;
    /*! Human readable strings describing the synchronization methods. */
  static const string SyncModeStrs[3];

  struct AttData {
    AttData( Attenuate *att )
      : Att( att ), Id( 0 ) {};
    AttData( Attenuate *att, int id )
      : Att( att ), Id( id ) {};
      /*! Pointer to the interface to the hardware driver. */
    Attenuate *Att;
      /*! Numerical Id of the corresponding AO device. */
    int Id;
  };
    /*! All attenuators for setting the intensity of analog output. */
  vector < AttData > Att;

    /*! A list of available output traces. */
  vector< TraceSpec > OutTraces;
    /*! Dummy trace. */
  static const TraceSpec DummyTrace;

};


}; /* namespace relacs */

#endif /* ! _RELACS_ACQUIRE_H_ */
