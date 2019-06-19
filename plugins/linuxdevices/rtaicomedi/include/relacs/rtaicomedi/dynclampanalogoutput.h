/*
  rtaicoemdi/dynclampanalogoutput.h
  Interface for accessing analog output of a daq-board via the dynamic clamp 
  kernel module.

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

#ifndef _RTAICOMEDI_DYNCLAMPANALOGOUTPUT_H_
#define _RTAICOMEDI_DYNCLAMPANALOGOUTPUT_H_

#include <vector>
#include <comedilib.h>
#include <relacs/daqerror.h>
#include <relacs/analogoutput.h>
#include <relacs/rtaicomedi/moduledef.h>
using namespace std;
using namespace relacs;

namespace rtaicomedi {


/*! 
\class DynClampAnalogOutput
\author Marco Hackenberg
\author Jan Benda
\brief [AnalogOutput] Interface for accessing analog output of a daq-board via a dynamic clamp kernel module.
\bug fix errno usage

\par Output traces 

As usual, output traces can be assigned to channels
of the analog output device. All the output traces the dynamic clamp
model requires (its \c outputNames) must be available.

Additional output traces as defined by the dynamic clamp model via its
\c paramOutputNames can be accessed like normal analog output traces
by their name.
*/


class DynClampAnalogOutput : public AnalogOutput
{

public:

    /*! Create a new DynClampAnalogOutput without opening a device. */
  DynClampAnalogOutput( void );
    /*! Open the analog output driver specified by its device file \a device. */
  DynClampAnalogOutput( const string &device, const Options &opts );
    /*! Stop analog output and close the daq driver. */
  virtual ~DynClampAnalogOutput( void );

    /*! Open the analog output device on device file \a device. */
  virtual int open( const string &device ) override;
    /*! Returns true if dynamic clamp module was succesfully opened. */
  virtual bool isOpen( void ) const;
    /*! Stop all activity and close the device. */
  virtual void close( void );

    /*! Number of analog output channels. */
  virtual int channels( void ) const;
    /*! Resolution in bits of analog output. */
  virtual int bits( void ) const;
    /*! Maximum sampling rate in Hz of analog output. */
  virtual double maxRate( void ) const;

    /*! Maximum number of analog output ranges. */
  virtual int maxRanges( void ) const;
    /*! Voltage range \a index in Volt for unipolar mode.
        If -1 is returned this range is not supported. */
  virtual double unipolarRange( int index ) const;
    /*! Voltage range \a index in Volt for bipolar mode.
        If -1 is returned this range is not supported. */
  virtual double bipolarRange( int index ) const;

    /*! Directly writes from each signal in \a sigs the first data value
        to the data acquisition board. */
  virtual int directWrite( OutList &sigs );

    /*! Prepare analog output of the output signals \a sigs on the device.
	If an error ocurred in any signal, the corresponding errorflags in
	OutData are set and a negative value is returned.
	This function assumes that \a sigs successfully passed testWrite().
        The channels in \a sigs are not sorted. */
  virtual int prepareWrite( OutList &sigs );
    /*! Start analog output of the output signals that were passed to the previous call
        of prepareWrite().
	If an error ocurred in any signal, the corresponding errorflags in
	OutData are set and a negative value is returned.
	If no further calls of writeData() are required, 0 is returned,
	otherwise 1 is returned.
	Also start possible pending acquisition on other devices
	that are known from take().
        This function is always called after a successfull prepareRead().
	\param[in] sp if not null, a thread is started feeding the running analog output.
        When the thread and analog output is finished, releases the semaphore by one.
        On error, the semaphore is released by 1000 so that the process waiting
        on the semaphore is waking up immediately. */
  virtual int startWrite( QSemaphore *sp = 0 );
    /*! Write data of the output signals that were passed to the previous call
        of prepareWrite() to the analog output device.
        Returns the number of transferred data elements.
	Returns zero if all data are transferred.
	If an error ocurred in any channel, the corresponding errorflags in the
	InData structure are filled and a negative value is returned.
        This function is called periodically after writing has been successfully
        started by startWrite(). */
  virtual int writeData( void );

    /*! Stop any running ananlog output activity.
        Returns zero on success, otherwise NotOpen.
        \sa reset(), startWrite() */
  virtual int stop( void );
    /*! Clear any internal data buffers and reset the device.
        Assumes that analog output is already stopped.
        Returns zero on success, otherwise NotOpen.
        \sa stop(), close(), open(), isOpen() */
  virtual int reset( void );
  
    /*! \return the status of the analog output. */
  virtual Status statusUnlocked( void ) const;

    /*! Index of signal start relative to the data stream of a running analog input. */
  virtual long index( void ) const;
  
    /*! Returns the index of the corresponding analog input device in \a ais
        from which the index of a signal start is taken. */
  virtual int getAISyncDevice( const vector< AnalogInput* > &ais ) const;
    /*! Returns \c true to indicate that the analog output is required to have the
        same sampling rate as the analog input returned by getAISyncDevice(). */
  virtual bool useAIRate( void ) const;

    /*! If the analog output device supports outputs that
        are not physical output lines but rather writeable parameter,
        like model parameter for a dynamic clamp modul,
        then reimplement this function. 
        Add for each such parameter a TraceSpec to \a traces.
        \a deviceid is the id of the analog output device
        that you should use for initializing the TraceSpec. */
  virtual void addTraces( vector< TraceSpec > &traces, int deviceid ) const;

    /*! Match trace names with model output trace names. */
  virtual int matchTraces( vector< TraceSpec > &traces ) const;


protected:

    /*! Device driver specific tests on the settings in \a sigs
        for each output signal.
	Before this function is called, the validity of the settings in 
	\a sigs was already tested by testReadData().
	This function should test whether the settings are really supported
	by the hardware.
	If an error ocurred in any trace, the corresponding errorflags in the
	OutData are set and a negative value is returned.
        The channels in \a sigs are not sorted.
        This function is called by testWrite(). */
  virtual int testWriteDevice( OutList &sigs );

    /*! Initialize all channels with zero. */
  void writeZeros( void );

    /*! Initializes the \a chanlist from \a sigs. */
  void setupChanList( OutList &sigs, unsigned int *chanlist, int maxchanlist, bool setscale ) const;
    /*! Load channels from \a sigs to kernel module. */
  int loadChanList( OutList &sigs, int isused ) const;

    /*! True if analog output was prepared using testWriteDevice() and prepareWrite() */
  bool prepared( void ) const;

  void initOptions() override;

private:

    /*! Unique analog I/O device type id for all 
        DynClamp DAQ devices. */
  static const int DynClampAnalogIOType = 2;

    /*! Name of the kernel module device file. */
  string ModuleDevice;
    /*! File descriptor for the kernel module. */
  int ModuleFd;

    /*! Pointer to the comedi device. */
  comedi_t *DeviceP;
    /*! The comedi subdevice number. */
  unsigned int SubDevice;
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
    /*! Maps descendingly sorted range indices to (unsorted) \a UnipolarRange
        indices. */
  vector< unsigned int > UnipolarRangeIndex;
    /*! Maps descendingly sorted range indices to (unsorted) \a BipolarRange
        indices. */
  vector< unsigned int > BipolarRangeIndex;
  int UnipolarExtRefRangeIndex;
  int BipolarExtRefRangeIndex;
    /*! The largest range that will be used for all analog outputs. */
  comedi_range LargestRange;
    /*! The index of the largest range that will be used for all analog outputs. */
  int LargestRangeIndex;
    /*! The polarity of the largest range that will be used for all analog outputs. */
  int LargestRangeUnipolar;

    /*! Conversion polynomials for all channels and unipolar gains. */
  comedi_polynomial_t **UnipConverter;
    /*! Conversion polynomials for all channels and bipolar gains. */
  comedi_polynomial_t **BipConverter;

    /*! Maximum valid data value for each channel. */
  vector< lsampl_t > MaxData;
    /*! Minimum possible voltage of each channel. */
  vector< float > MinVoltage;
    /*! Maximum possible voltage of each channel. */
  vector< float > MaxVoltage;

  bool IsPrepared;
    /*! True if no more data need to be written to the board. */
  bool NoMoreData;
  mutable bool IsRunning;

    /*! Calibration info. */
  comedi_calibration_t *Calibration;

    /*! The output signals that were prepared by prepareWrite(). */
  mutable OutList Sigs;
    /*! Size of the FIFO for transfering data to the driver. */
  int FIFOSize;
    /*! Size of the buffer for transfering data to the driver. */
  int BufferSize;
    /*! Buffer used for transfering data to the driver. */
  char *Buffer;
    /*! Current number of elements in the buffer. */
  int NBuffer;

};


}; /* namespace rtaicomedi */

#endif /* ! _RTAICOMEDI_DYNCLAMPANALOGOUTPUT_H_ */
