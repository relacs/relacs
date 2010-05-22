/*
  comedi/dynclampanaloginput.h
  Interface for accessing analog input of a daq-board via a dynamic clamp kernel module.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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

#ifndef _COMEDI_DYNCLAMPANALOGINPUT_H_
#define _COMEDI_DYNCLAMPANALOGINPUT_H_

#include <vector>
#include <comedilib.h>
#include <relacs/daqerror.h>
#include <relacs/analoginput.h>
#include <relacs/comedi/moduledef.h>
//#include <relacs/comedi/dynclampanalogoutput.h>
using namespace std;
using namespace relacs;

namespace comedi {


class ComediAnalogInput;

/*! 
\class DynClampAnalogInput
\author Marco Hackenberg, Jan Benda
\brief [AnalogInput] Interface for accessing analog input of a daq-board via a dynamic clamp kernel module.
\todo: testReadDevice(): we don't get integer data!!! sigs[k].setGain( 1.0 );
\todo: Implement take()
\bug fix errno usage
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
  virtual int open( const string &device, const Options &opts );
    /*! Returns true if dynamic clamp module was succesfully opened. */
  virtual bool isOpen( void ) const;
    /*! Stop all activity and close the device. */
  virtual void close( void );

    /*! Set the name of the dynamic clamp module file. This has to be done 
        before performing prepareRead() or  startRead().
        Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).        
	\sa moduleName() \sa setDeviceName() \sa deviceName()  */
  int setModuleName( string modulename );
    /*! Return the name of the dynamic clamp module file.
      \sa setModuleName() \sa setDeviceName() \sa deviceName()  */
  string moduleName( void ) const;

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
  virtual int startRead( void );
    /*! Read data from a running data acquisition.
        Returns the total number of read data values.
	If an error ocurred in any channel, the corresponding errorflags in the
	InList structure are filled and a negative value is returned. */
  virtual int readData( void );
    /*! Convert data from and push them to the traces.
        Returns the number of new data values that were added to the traces
	(sum over all traces).
	If an error ocurred in any channel, the corresponding errorflags in the
	InList structure are filled and a negative value is returned. */
  virtual int convertData( void );

    /*! Stop any running ananlog input activity on the device.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, ReadError.
        \sa close(), open(), isOpen() */
  virtual int stop ( void );
    /*! Stop any running ananlog input activity and reset the device.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, ReadError.
        \sa close(), open(), isOpen() */
  virtual int reset( void );
  
    /*! True if analog input is running. */
  virtual bool running( void ) const;

    /*! Get error status of the device. 
        0: no error
	-1: underrun
        other: unknown */
  virtual int error( void ) const;

    /*! Check for every analog input and input device in \a ais and \a aos
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


private:

    /*! Unique analog I/O device type id for all 
        DynClamp DAQ devices. */
  static const int DynClampAnalogIOType = 2;

    /*! Pointer to the user space comedi interface. */
  ComediAnalogInput *CAI;
    /*! Subdevice flags of the comedi analog input subdevice. */
  unsigned int CAISubDevFlags;

    /*! needed by for assigning TraceInfo strings to channels. */
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
  
  unsigned int ChanList[MAXCHANLIST];
  bool IsPrepared;
  mutable bool IsRunning;
  int ErrorState;

    /*! The input traces that were prepared by prepareRead(). */
  InList *Traces;
    /*! Size of the driver buffer used for getting the data from the daq board. */
  int ReadBufferSize;
    /*! Size of the internal buffer used for getting the data from the driver. */
  int BufferSize;
    /*! The number of bytes written so far to the internal buffer. */
  int BufferN;
    /*! The internal buffer used for getting the data from the driver. */
  char *Buffer;
    /*! Index to the trace in the internal buffer. */
  int TraceIndex;

  /*
  vector< DynClampAnalogInput* > DynClampAIs;
  vector< DynClampAnalogOutput* > DynClampAOs;
  vector< int > ComediAIsLink;
  vector< int > ComediAOsLink;
  */

};


}; /* namespace comedi */

#endif /* ! _COMEDI_DYNCLAMPANALOGINPUT_H_ */
