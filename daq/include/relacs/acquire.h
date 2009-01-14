/*
  acquire.h
  Read and write data from/to the data aqcuisition board.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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
\version 1.9
\brief Read and write data streams from/to data aqcuisition boards.
\todo Overflow in InData::Index (after 7h!) -> reset InData!
  Implement the setReset() function and the HardReset variable.
\todo internal list of Attenuate that corresponds to outTraces for faster access.
\todo write(), testWrite(): check carrier frequency?

Acquire is a general interface to data acquisition boards
that integrates multiple analog input, analog output,
and attenuator device interfaces.
Input and output data together with the specifications for the
data acquisition boards are communicated via the high level
classes InData and OutData.

For communication with the data acquisition boards the 
AnanlogInput and AnalogOutput interfaces are used.
Acquisition devices must be added to Acquire with the functions addInput()
and addOutput() for analog input and output, respectively.
Attenuators are operated via the Attenuate interface.
They must be added with the addAttLine() function to Acquire.

The hardware drivers need to know for how long they need to buffer data
between successive calls to readData() and/or writeData().
Specify this time by setBufferTime().
In addition, the data from an analog input need to be converted and
pushed into an InData once they have been obtained by readData().
This is done by periodically by convertData(). 
The time interval between successive calls to convertData() is specified by
setUpdateTime() and is used by the AnalogInput implementations for
providing an appropriately sized internal data buffer.

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
With readData() data from the data acquisition board are transfered to the InList.
stopRead() stops any activity related to analog input.
inTraces() returns a list of all available analog input traces.

The gain settings for the ananlog input lines can be modified by
the setGain() and adjustGain() functions.
The new settings are automatically activated by the next call of write()
or by explicitly calling activateGains().

Analog output is performed by write().
Its OutData argument specifies the parameter (sampling rate, channels, delay, ... )
for the analog output operation.
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
      /*! Starting AI and AO is synchronized via trigger signals. */
    TriggerSync,
      /*! Continuous AI. Counter is used for synchronization. */
    CounterSync,
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
	The default type of the channels of this device is 
	set to \a defaulttype.
        \return
	- 0 on success.
	- -1: \a ai == 0
	- -2: \a ai not opened
	- -3: \a defaulttype not valid
	\sa inputsSize(), inputDevice(), inputTraces(), clearInputs(),
	closeInputs(), addOutput(), addAttenute(), addOutTrace() */
  int addInput( AnalogInput *ai, int defaulttype=0 );
    /*! The number of device drivers for analog input
        stored in this Acquire. 
	\sa addInput(), inputDevice(), inputTraces(), clearInputs(),
	closeInputs(), outputsSize(), attLinesSize(),
	outTracesSize() */
  int inputsSize( void ) const;
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
        \sa addOutput(), ooutTraceIndex() */
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
	The trace has the reglitch circuit enabled (\a reglitch),
	allows a maximum sampling rate of \a maxrate
	and has a delay of signals of \a signaldelay.
        The signal is given in unit \a unit and multiplied by \a scale
        to convert it to the voltage of the ananlog output.
        \sa addOutTraces(), outTracesSize(), outTraceIndex(),
	outTraceName(), outTrace(), applyOutTrace(), clearOutTraces(),
        addInput(), addOutput(), addAttLine() */
  void addOutTrace( const string &name, int device, int channel,
		    double scale=1.0, const string &unit="",
		    bool reglitch=false, double maxrate=-1.0,
		    double signaldelay=0.0 );
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
  const Attenuate *outTraceAttenuate( int index ) const;
    /*! Return the output trace with index \a index.
        \sa addOutTrace(), outTracesSize(), outTraceIndex(),
	outTraceName(), applyOutTrace(), clearOutTraces() */
  const TraceSpec &outTrace( int index ) const;
    /*! Return the output trace with name \a name.
        \sa addOutTrace(), outTracesSize(), outTraceIndex(),
	outTraceName(), applyOutTrace(), clearOutTraces() */
  const TraceSpec &outTrace( const string &name ) const;
    /*! If \a signal has an output trace specified (via OutData::setTrace())
        then the corresponding settings (channel, device, ...)
        are applied to \a signal.
	If the trace is not known, the DaqError::InvalidTrace
	error flag of \a signal is set.
	\return 
	-  0: success (or empty trace name)
	- -1: invalid trace name
	- -2: invalid match
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
  void init( void );
    /*! The method that is used for synchronizing analog output with analog input. */
  SyncModes syncMode( void ) const;
    /*! A string describing the method that is used
        for synchronizing analog output with analog input. */
  string syncModeStr( void ) const;

    /*! The maximum time in seconds the hardware driver should buffer data. 
        \sa setBufferTime(), updateTime() */
  double bufferTime( void ) const;
    /*! Set the maximum time the hardware driver should be able to
	buffer the data to \a time seconds. The actually set maximum
	possible time can be retrieved from InData::readTime() or
	OutData::writeTime() after calling read() and write(),
	respectively. The default buffer time is 0.01 seconds.
	\sa bufferTime(), setUpdateTime() */
  void setBufferTime( double time );
    /*! The maximum time in seconds the AnalogInput implementation
        should buffer data between calls to readData() and convertData(). 
        \sa setUpdateTime(), bufferTime() */
  double updateTime( void ) const;
    /*! Set the maximum time the AnalogInput implementation should be able to
	buffer the data to \a time seconds. The actually set maximum
	possible time can be retrieved from InData::updateTime() or
	OutData::updateTime() after calling read() and write(),
	respectively. The default update time is 0.1 seconds.
	\sa updateTime(), setBufferTime() */
  void setUpdateTime( double time );

    /*! Inform the analog input and output devices
        about the requested input and output traces. */
  //  void initTraces( InList &data, OutList &signals );

    /*! Test of analog input settings given by \a data for validity.
	Returns 0 on success, negative numbers otherwise.
        Possible errors are indicated by the error state of \a data.
        If some parameters in \a data are invalid there are set to valid values
	so that a following call of read( InList &data ) will succeed. 
        \sa read(), readData(), stopRead() */
  virtual int testRead( InList &data );
    /*! Start analog input with the settings given by \a data. 
	Returns 0 on success, negative numbers otherwise.
        Possible errors are indicated by the error state of \a data. 
        \sa testRead(), readData(), stopRead() */
  virtual int read( InList &data );
    /*! Read data from data aquisition board and transfer them to the data
        from the last call of read() that initiated the acquisition.
	In case of an error, acquisition is restarted.
	Input gains are not updated.
	Returns 0 on success, negative numbers otherwise.
        Possible errors are indicated by the error state of the traces.
        See InList for details about handling the data. 
        \sa testRead(), read(), stopRead() */
  virtual int readData( void );
    /*! Stop analog input of all analog input devices.
        Remaining data can be still obtained with readData().
	Returns 0 on success, negative numbers otherwise. 
        \sa testRead(), read(), readData() */
  virtual int stopRead( void );

    /*! The flag that is used to mark traces whose gain was changed. 
        \sa setAdjustFlag(), setGain(), adjustGain(),
	gainChanged(), activateGains() */
  int adjustFlag( void ) const;
    /*! Returns the flag which is used to mark traces
        whose gain was changed to \a flag. 
        \sa adjustFlag(), setGain(), adjustGain(),
	gainChanged(), activateGains() */
  void setAdjustFlag( int flag );

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
        \sa convert(), write(), writeData(), writeZero(), stopWrite() */
  virtual int testWrite( OutData &signal );
    /*! Test of a multiple output signals \a signal for validity.
        If the signals \a signal are the same as from the last call of write()
	(on same devices with the same addresses),
	the devices are also checked for acquisition errors.
        Returns 0 on success or a negative number if \a signal
        is not valid.
        The error state of \a signal is set appropriately.
        \sa convert(), write(), writeData(), writeZero(), stopWrite() */
  virtual int testWrite( OutList &signal );

    /*! Convert the data of \a signal into a device dependent format.
        Subsequent calls of write( signal ) will not
	convert the data again.
        Once convert() is called, you have to maintain the internal
        data buffer of \a signal, i.e. if you change the data
	you have to call convert() again before you are able to write out 
        the changed signal with write().
	You may also clear the internal buffer by calling signal.clearBuffer().
        \sa testWrite(), write(), writeData(), writeZero(), stopWrite() */
  virtual int convert( OutData &signal );
    /*! Convert the data of \a signal into a device dependent format.
        Subsequent calls of write( signal ) will not
	convert the data again.
        Once convert() is called, you have to maintain the internal
        data buffers of \a signal, i.e. if you change the data
	you have to call convert() again before you are able to write out 
        the changed signals with write().
	You may also clear the internal buffers by calling signal.clearBuffer().
        \sa testWrite(), write(), writeData(), writeZero(), stopWrite() */
  virtual int convert( OutList &signal );

    /*! Output of a signal \a signal.
        See OutData about how to specify output channel, sampling rate, 
	intensity, delay, etc. 
	If input gains are requested to be changed,
	analog input is restarted with the new gain settings
	right before the signal is put out.
	Returns 0 on success.
        If the output of the signal failed, a negative number is returned and
        the reason is specified in the error state of \a signal.
        \sa testWrite(), convert(), writeData(), writeZero(), stopWrite() */
  virtual int write( OutData &signal );
    /*! Output of multiple signals \a signal.
        See OutData about how to specify output channel, sampling rate, 
	intensity, delay, etc. 
	If input gains are requested to be changed,
	analog input is restarted with the new gain settings
	right before the signal is put out.
	Returns 0 on success.
        If the output of the signals failed, a negative number is returned and
        the reason is specified in the error state of \a signal.
        \sa testWrite(), convert(), writeData(), writeZero(), stopWrite() */
  virtual int write( OutList &signal );

    /*! After having started an analog output with write()
        repeatedly call this function to fill up the buffer of
	the hardware driver with data.
	Returns a negative number on error, zero if no more data need to be
	transferred to the hardware driver, and one if some data were
	successfully transfered to the hardware driver.
        \sa testWrite(), convert(), write(), writeZero(), stopWrite() */
  virtual int writeData( void );

    /*! Set the output of channel \a channel on device \a device to zero.
        Returns 0 on success or a negative number on error. 
        \sa testWrite(), convert(), write(), writeData(), stopWrite() */
  virtual int writeZero( int channel, int device );
    /*! Set the output of the trace with index \a index to zero.
        Returns 0 on success or a negative number on error.
        \sa testWrite(), convert(), write(), writeData(), stopWrite() */
  virtual int writeZero( int index );
    /*! Set the output of the trace with name \a trace to zero.
        Returns 0 on success or a negative number on error.
        \sa testWrite(), convert(), write(), writeData(), stopWrite() */
  virtual int writeZero( const string &trace );

    /*! Stop analog output of all analog output devices.
        \sa testWrite(), convert(), write(), writeData(), writeZero() */
  virtual int stopWrite( void );

    /*! Add signal events to \a events. */
  virtual void readSignal( InList &data, EventList &events );

    /*! Force time zero reference to be reset. */
  void setReset( void );

    /*! Stop any activity related to
        analog output and analog input immediately. */
  virtual void stop( void );
  

protected:

    /*! \return a string with the current time. */
  string currentTime( void );

  struct AIData {
      /*! Construct an AIData. */
    AIData( AnalogInput *ai, int type=0 )
      : AI( ai ), DefaultType( type ),
	AIDevice( -1 ) {};
      /*! Pointer to the interface to the hardware driver. */
    AnalogInput *AI;
      /*! Default analog input type. */
    int DefaultType;
      /*! The traces acquired from this device. */
    InList Traces;
      /*! New gain indices corresponding to \a Traces. */
    vector< int > Gains;
      /*! This analog input device will always be started 
	  by analog input device \a AIDevice (index to \a AI). */
    int AIDevice;
  };
    /*! All devices for analog input. */
  vector < AIData > AI;

    /*! The flag that is used to mark adjusted traces in InData. */
  int AdjustFlag;

  struct AOData {
    AOData( AnalogOutput *ao )
      : AO( ao ), AISyncDevice( -1 ), AIDevice( -1 ), AODevice( -1 ) {};
      /*! Pointer to the interface to the hardware driver. */
    AnalogOutput *AO;
      /*! The signals to be emitted from this device. */
    OutList Signals;
      /*! Output can be synced with running analog input device (index). */
    int AISyncDevice;
      /*! This analog output device will always be started 
	  by analog input device \a AIDevice (index to \a AI). */
    int AIDevice;
      /*! This analog output device will always be started 
	  by analog output device \a AODevice (index to \a AO). */
    int AODevice;
  };
    /*! All devices for analog output. */
  vector < AOData > AO;
    /*! OutList Dummy for single channel signals. */
  OutList Signal;
    /*! Index of last output device. */
  int LastDevice;
    /*! Time of last signal output. */
  double LastWrite;
    /*! Duration of last signal output. */
  double LastDuration;
    /*! Delay of last signal output. */
  double LastDelay;

    /*! Restart data aquisition and write signals 
        pending on devices in \a aos.
        If still running, stop analog input first.
        If \a updategains, the input gains are updated as well. */
  virtual int restartRead( vector< AOData* > &aod, bool updategains );

    /*! The currently used synchronization method. */
  SyncModes SyncMode;
    /*! Human readable strings describing the synchronization methods. */
  static const string SyncModeStrs[5];

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

    /*! The time, the buffers of analog input and output drives should
        be able to buffer data. */
  double BufferTime;
    /*! The time, the buffers of AnalogInput implementations should
        be able to buffer data. */
  double UpdateTime;

    /*! True if at the next best opportunity the input buffer Trace 
        should be reset to avoid an index overflow. */
  bool SoftReset;
    /*! True if the input buffer Trace should immediately 
        be reset to avoid an index overflow. */
  bool HardReset;

    /*! Set new time zero reference point. */
  void reset( void );

};


}; /* namespace relacs */

#endif /* ! _RELACS_ACQUIRE_H_ */
