/*
  daqflex/daqflexanalogoutput.h
  Interface for accessing analog output of a DAQFlex board from Measurement Computing.

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

#ifndef _DAQFLEX_DAQFLEXANALOGOUTPUT_H_
#define _DAQFLEX_DAQFLEXANALOGOUTPUT_H_

#include <pthread.h>
#include <relacs/daqflex/daqflexcore.h>
#include <relacs/analogoutput.h>
using namespace std;
using namespace relacs;

namespace daqflex {


/*! 
\class DAQFlexAnalogOutput
\author Jan Benda
\brief [AnalogOutput] Interface for accessing analog output of a DAQFlex board from Measurement Computing.
\bug Calibration is not yet implemented.
*/


class DAQFlexAnalogOutput : public AnalogOutput
{

public:

    /*! Create a new DAQFlexAnalogOutput without opening a device. */
  DAQFlexAnalogOutput( void );
    /*! Open the analog input driver specified by its device file \a device. */
  DAQFlexAnalogOutput( DAQFlexCore &device, const Options &opts );
    /*! Stop analog input and close the daq driver. */
  virtual ~DAQFlexAnalogOutput( void );

    /*! Open analog input on DAQFlexCore device \a device. */
  virtual int open( DAQFlexCore &daqflexdevice, const Options &opts );
    /*! Open analog input on DAQFlexCore device \a device. */
  virtual int open( Device &device, const Options &opts );
    /*! Returns true if driver was succesfully opened. */
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

    /*! Directly writes from each signal in \a sigs the first data value
        to the data acquisition board. */
  virtual int directWrite( OutList &sigs );

    /*! Prepare analog output of the output signals \a sigs on the device.
	If an error ocurred in any signal, the corresponding errorflags in
	OutData are set and a negative value is returned.
	This function assumes that \a sigs successfully passed testWrite().
        The channels in \a sigs are not sorted. */
  virtual int prepareWrite( OutList &sigs );
    /*! Start analog output of the output signals
        after they were prepared by prepareWrite().
	If an error ocurred in any signal, the corresponding errorflags in
	OutData are set and a negative value is returned.
	Also start possible pending acquisition on other devices
	that are known from take(). */
  virtual int startWrite( void );
    /*! Write data to a running data acquisition.
        Returns the number of data values that were popped from the signal 
	device-buffers (sum over all signals).
	If an error ocurred in any channel, the corresponding errorflags in the
	OutList structure are filled and a negative value is returned.  */
  virtual int writeData( void );

    /*! Stop any running ananlog input activity and reset the device.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, ReadError.
        \sa close(), open(), isOpen() */
  virtual int reset( void );
  
    /*! True if the analog input driver is running. */
  virtual bool running( void ) const;


protected:

    /*! Lock the mutex. */
  void lock( void ) const;
    /*! Unlock the mutex. */
  void unlock( void ) const;

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

    /*! Write data to a running data acquisition.
        Returns the number of data values that were popped from the \a trace- 
	device-buffer (sum over all \a traces).
	If an error ocurred in any channel, the corresponding errorflags in the
	OutList structure are filled and a negative value is returned.  
	For internal usage! */
  int fillWriteBuffer( void );


private:
  
    /*! Converts the signal traces to raw integer values for the DAQ board. */
  template < typename T >
    int convert( char *cbuffer, int nbuffer );

    /*! Unique analog I/O device type id for all 
        DAQFlex devices. */
  static const int DAQFlexAnalogIOType = 2;

    /*! The DAQFlex device. */
  DAQFlexCore *DAQFlexDevice;

    /*! Holds the list of supported bipolar ranges. */
  vector< double > BipolarRange;

  struct Calibration {
    double Offset;
    double Slope;
  };

    /*! True if the command is prepared. */
  bool IsPrepared;
    /*! True if no more data need to be written to the board. */
  bool NoMoreData;

    /*! The sorted output signals that were prepared by prepareWrite(). */
  OutList Sigs;
    /*! Size of the buffer for transfering data to the driver. */
  int BufferSize;
    /*! Buffer used for transfering data to the driver. */
  char *Buffer;
    /*! Current number of elements in the buffer. */
  int NBuffer;
    /*! Overall number of samples to be transmmitted. */
  int Samples;

    /*! A lock guarding all operations on the analog output. */
  mutable pthread_mutex_t Mutex;

};


}; /* namespace comedi */

#endif /* ! _DAQFLEX_DAQFLEXANALOGOUTPUT_H_ */
