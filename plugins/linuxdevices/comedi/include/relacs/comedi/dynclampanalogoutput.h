/*
  coemdi/dynclampanalogoutput.h
  Interface for accessing analog output of a daq-board via the dynamic clamp 
  kernel module.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#ifndef _COMEDI_DYNCLAMPANALOGOUTPUT_H_
#define _COMEDI_DYNCLAMPANALOGOUTPUT_H_

#include <vector>
#include <comedilib.h>
#include <relacs/daqerror.h>
#include <relacs/analogoutput.h>
#include <relacs/comedi/moduledef.h>
using namespace std;
using namespace relacs;

namespace comedi {


class ComediAnalogOutput;

/*! 
\class DynClampAnalogOutput
\author Marco Hackenberg
\author Jan Benda
\brief [AnalogOutput] Interface for accessing analog output of a daq-board via a dynamic clamp kernel module.
\bug fix errno usage
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
  virtual int open( const string &device, const Options &opts );
    /*! Returns true if dynamic clamp module was succesfully opened. */
  virtual bool isOpen( void ) const;
    /*! Stop all activity and close the device. */
  virtual void close( void );

    /*! Set the name of the dynamic clamp module file. This has to be done 
        before performing prepareWrite() or  startWrite().
        Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).        
	\sa moduleName() \sa setDeviceName() \sa deviceName()  */
  int setModuleName( string modulename );
    /*! Return the name of the dynamic clamp module file.
      \sa setModuleName() \sa setDeviceName() \sa deviceName()  */
  string moduleName( void ) const;

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
	OutData are set and a negative value is returned. */
  virtual int startWrite( void );
    /*! Write data of the output signals that were passed to the previous call
        of prepareWrite() to the analog output device.
        Returns the number of transferred data elements.
	Returns zero if all data are transferred.
	If an error ocurred in any channel, the corresponding errorflags in the
	InData structure are filled and a negative value is returned.
        This function is called periodically after writing has been successfully
        started by startWrite(). */
  virtual int writeData( void );

    /*! Stop any running ananlog output activity and reset the device.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, WriteError.
        \sa close(), open(), isOpen() */
  virtual int reset( void );
  
    /*! True if analog output is running. */
  virtual bool running( void ) const;

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

    /*! Initializes the \a chanlist from \a sigs. */
  void setupChanList( OutList &sigs, unsigned int *chanlist, int maxchanlist, bool setscale );

    /*! Write data to a running data acquisition.
        Returns the number of data values that were popped from the \a trace- 
	device-buffer (sum over all \a traces).
	If an error ocurred in any channel, the corresponding errorflags in the
	OutList structure are filled and a negative value is returned.  
	For internal usage! */
  int fillWriteBuffer( void );

    /*! True if analog output was prepared using testWriteDevice() and prepareWrite() */
  bool prepared( void ) const;


private:

    /*! Unique analog I/O device type id for all 
        DynClamp DAQ devices. */
  static const int DynClampAnalogIOType = 2;

    /*! Pointer to the user space comedi interface. */
  ComediAnalogOutput *CAO;
    /*! Subdevice flags of the comedi analog output subdevice. */
  unsigned int CAOSubDevFlags;

    /*! needed for assigning TraceInfo strings to channels. */
  int SubdeviceID;

    /*! Name of the kernel module device file. */
  string ModuleDevice;
    /*! File descriptor for the kernel module. */
  int ModuleFd;
    /*! FIFO file descriptor for data exchange with kernel module. */
  int FifoFd;

    /*! The comedi subdevice number. */
  unsigned int SubDevice;
    /*! The size of a single sample in bytes. */
  unsigned int BufferElemSize;  
    /*! Number of channels available on the device. */
  int Channels;
    /*! Resolution in bits of each channel. */
  int Bits;
    /*! Maximum sampling rate. */
  double MaxRate;
    /*! Conversion polynomials for all channels and unipolar gains. */
  comedi_polynomial_t **UnipConverter;
    /*! Conversion polynomials for all channels and bipolar gains. */
  comedi_polynomial_t **BipConverter;

  bool IsPrepared;
  mutable bool IsRunning;

    /*! The output signals that were prepared by prepareWrite(). */
  OutList Sigs;
    /*! Size of the FIFO for transfering data to the driver. */
  int FIFOSize;
    /*! Size of the buffer for transfering data to the driver. */
  int BufferSize;
    /*! Buffer used for transfering data to the driver. */
  char *Buffer;
    /*! Current number of elements in the buffer. */
  int NBuffer;

};


}; /* namespace comedi */

#endif /* ! _COMEDI_DYNCLAMPANALOGOUTPUT_H_ */
