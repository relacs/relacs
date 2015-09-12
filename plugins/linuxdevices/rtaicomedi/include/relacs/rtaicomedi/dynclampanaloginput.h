/*
  rtaicomedi/dynclampanaloginput.h
  Interface for accessing analog input of a daq-board via a dynamic clamp kernel module.

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

#ifndef _RTAICOMEDI_DYNCLAMPANALOGINPUT_H_
#define _RTAICOMEDI_DYNCLAMPANALOGINPUT_H_

#include <vector>
#include <comedilib.h>
#include <relacs/daqerror.h>
#include <relacs/analoginput.h>
#include <relacs/rtaicomedi/moduledef.h>
using namespace std;
using namespace relacs;

namespace rtaicomedi {


/*! 
\class DynClampAnalogInput
\author Marco Hackenberg
\author Jan Benda
\brief [AnalogInput] Interface for accessing analog input of a daq-board via a dynamic clamp kernel module.
\todo testReadDevice(): we don't get integer data!!! sigs[k].setGain( 1.0 );
\todo Implement take()
\todo Error handling in readData()
\bug fix errno usage

\par Input traces 

As usual, input traces can be assigned to channels
of the analog input device. All the input traces the dynamic clamp
model requires (its \c inputNames) must be available.

Some additional input traces might be available. They can be accessed like
normal analog input traces, but the channel nummber needs to be set to 1000 or larger.
The following three types of additional inputs are supported:
- additional inputs as defined by the dynamic clamp model via its \c paramInputNames.
- timing measurements, if they are enabled in moduledef.h:
  - \c Interval : the full period of the dynamic clamp loop
  - \c AI-time : the time required by the analog input part
  - \c AI-acquisition-time : the time required by reading in a single analog input channel
  - \c AI-conversion-time : the time required to convert the read in value
  - \c AO-time : the time required by the analog output part
  - \c Model-time : the time required for calling computModel()
  - \c Wait-time : the time rt_task_wait_period() waits for the net period
  .
  all times are measured in seconds.
- for each analog output trace the dynamic clamp model is using (its \c outputNames), 
  - \c Stimulus-xxx contains the stimulus a RePro wrote to this channel,
  - \c Model-xxx contains the value the model computed, and
  - \c Total-xxx contains the total requested output for this channel.
  - \c Injected-xxx contains the total output for this channel as it is written to the analog output.
    This equals \c Total-xxx multiplied by a possible correction factor and is only available with ENABLE_SYNCSEC
  .
  Stimulus-xxx plus Model-xxx equals Total-xxx.
  \c xxx is the name of the analog output trace.

\par Options:
- \c gainblacklist: List of daq board gains that should not be used. Each gain is identified by its
  maximal range value in volts.
*/


class DynClampAnalogInput : public AnalogInput
{

public:

    /*! Create a new DynClampAnalogInput without opening a device. */
  DynClampAnalogInput( void );
    /*! Open the analog input driver specified by its device file \a device. */
  DynClampAnalogInput( const string &device, const Options &opts );
    /*! Stop analog input and close the daq driver. */
  virtual ~DynClampAnalogInput( void );

    /*! Open the analog input device on device file \a device. */
  virtual int open( const string &device ) override;
    /*! Returns true if dynamic clamp module was succesfully opened. */
  virtual bool isOpen( void ) const;
    /*! Stop all activity and close the device. */
  virtual void close( void );

    /*! Number of analog input channels. */
  virtual int channels( void ) const;
    /*! Resolution in bits of analog input. */
  virtual int bits( void ) const;
    /*! Maximum sampling rate in Hz of analog input. */
  virtual double maxRate( void ) const;

    /*! Maximum number of analog input ranges. */
  virtual int maxRanges( void ) const;
    /*! Voltage range \a index in Volt for unipolar mode.
        If -1 is returned this range is not supported. */
  virtual double unipolarRange( int index ) const;
    /*! Voltage range \a index in Volt for bipolar mode.
        If -1 is returned this range is not supported. */
  virtual double bipolarRange( int index ) const;

    /*! Prepare analog input of the input signals \a traces on the device.
	If an error ocurred in any signal, the corresponding errorflags in
	InData are set and a negative value is returned.
	This function assumes that \a traces successfully passed testRead().
        The channels in \a traces are not sorted.
        \todo check whether channel >=1000 is valid! */
  virtual int prepareRead( InList &traces );
    /*! Start analog input of the input traces on the device
        after they were prepared by prepareRead().
	If an error ocurred in any channel, the corresponding errorflags in the
	InData structure are filled and a negative value is returned.
	Also start possible pending acquisition on other devices
	that are known from take(). */
  virtual int startRead( QSemaphore *sp=0, QReadWriteLock *datamutex=0,
			 QWaitCondition *datawait=0, QSemaphore *aosp=0 );
    /*! Read data from a running data acquisition.
        Returns the total number of read data values.
	If an error ocurred in any channel, the corresponding errorflags in the
	InList structure are filled and -2 is returned. */
  virtual int readData( void );
    /*! Convert data from and push them to the traces.
        Returns the number of new data values that were added to the traces
	(sum over all traces).
	If an error ocurred in any channel, the corresponding errorflags in the
	InList structure are filled and a negative value is returned. */
  virtual int convertData( void );

    /*! Stop any running ananlog input activity,
        but preserve all so far read in data.
	The next call to readData() will return the data.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, ReadError.
        \sa reset(), startRead() */
  virtual int stop( void );
    /*! Clear any internal data buffers and reset the device.
        Assumes that analog input is already stopped.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, ReadError.
        \sa stop(), close(), open(), isOpen() */
  virtual int reset( void );
  
    /*! True if analog input is running. */
  virtual bool running( void ) const;

    /*! Check for every analog input and analog output device 
        in \a ais and \a aos, respectively,
        whether it can be simultaneously started by startRead()
	from this device. */
  virtual void take( vector< AnalogInput* > &ais,
		     vector< AnalogOutput* > &aos,
		     vector< int > &aiinx, vector< int > &aoinx,
		     vector< bool > &airate, vector< bool > &aorate );

    /*! If the analog input device supports inputs that
        are not physical input lines but rather readable internal variables,
        like model outputs of a dynamic clamp modul,
        then reimplement this function. 
        Add for each such variable a TraceSpec to \a traces.
        \a deviceid is the id of the analog output device
        that you should use for initializing the TraceSpec. */
  virtual void addTraces( vector< TraceSpec > &traces, int deviceid ) const;

    /*! Match trace names with model trace names. */
  virtual int matchTraces( InList &traces ) const;


protected:

    /*! Device driver specific tests on the settings in \a traces
        for each input signal.
	Before this function is called, the validity of the settings in 
	\a traces was already tested by testReadData().
	This function should test whether the settings are really supported
	by the hardware.
	If an error ocurred in any trace, the corresponding errorflags in the
	InData are set and a negative value is returned.
        The channels in \a traces are not sorted.
        This function is called by testRead(). */
  virtual int testReadDevice( InList &traces );

    /*! Initializes the \a chanlist from \a traces. */
  int setupChanList( InList &traces, unsigned int *chanlist, int maxchanlist );


    /*! True if analog input was prepared using testReadDevice() and prepareRead() */
  bool prepared( void ) const;

  void initOptions() override;


private:

    /*! Unique analog I/O device type id for all 
        DynClamp DAQ devices. */
  static const int DynClampAnalogIOType = 2;

    /*! Pointer to the comedi device. */
  comedi_t *DeviceP;
    /*! The comedi subdevice number. */
  unsigned int SubDevice;

    /*! Name of the kernel module device file. */
  string ModuleDevice;
    /*! File descriptor for the kernel module. */
  int ModuleFd;
    /*! FIFO file descriptor for data exchange with kernel module. */
  int FifoFd;

    /*! The size of a single sample in bytes. */
  unsigned int BufferElemSize;  
    /*! Number of channels available on the device. */
  int Channels;
    /*! Maximum sampling rate. */
  double MaxRate;

    /*! Holds the list of supported unipolar comedi ranges. */
  vector< comedi_range > UnipolarRange;
    /*! Holds the list of supported bipolar comedi ranges. */
  vector< comedi_range > BipolarRange;
    /*! Maps unipolar range indices to comei range indices. */
  vector< unsigned int > UnipolarRangeIndex;
    /*! Maps bipolar range indices to comei range indices. */
  vector< unsigned int > BipolarRangeIndex;
    /*! Conversion polynomials for all channels and unipolar gains. */
  comedi_polynomial_t **UnipConverter;
    /*! Conversion polynomials for all channels and bipolar gains. */
  comedi_polynomial_t **BipConverter;
  
  unsigned int ChanList[MAXCHANLIST];
  bool IsPrepared;

    /*! Calibration info. */
  comedi_calibration_t *Calibration;

    /*! The input traces that were prepared by prepareRead(). */
  InList *Traces;
    /*! Size of the internal buffer used for getting the data from the driver. */
  int BufferSize;
    /*! The number of bytes written so far to the internal buffer. */
  int BufferN;
    /*! The internal buffer used for getting the data from the driver. */
  char *Buffer;
    /*! Index to the trace in the internal buffer. */
  int TraceIndex;

};


}; /* namespace rtaicomedi */

#endif /* ! _RTAICOMEDI_DYNCLAMPANALOGINPUT_H_ */
