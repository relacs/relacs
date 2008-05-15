/*
  comedi/comedianalogoutput.h
  Interface for accessing analog output of a daq-board via comedi.

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

#ifndef _RELACS_COMEDI_COMEDIANALOGOUTPUT_H_
#define _RELACS_COMEDI_COMEDIANALOGOUTPUT_H_ 1

#include <vector>
#include <comedilib.h>
#include <relacs/analogoutput.h>
using namespace std;
using namespace relacs;

namespace comedi {


class ComediAnalogInput;

/*! 
\class AnalogOutput
\author Marco Hackenberg
\version 0.1
\brief Interface for accessing analog output of a daq-board via comedi.
*/


class ComediAnalogOutput : public AnalogOutput
{

  friend class DynClampAnalogOutput;

public:

   /*! Device type id for comedi DAQ output devices. */
  static const int ComediAnalogOutputType = 4;


    /*! Create a new ComediAnalogOutput without opening a device. */
  ComediAnalogOutput( void );
    /*! Constructs an ComediAnalogOutput with device name \a device
        and type id \a aotype.  
        \sa setDeviceName() */
  ComediAnalogOutput( const string &devicename );
    /*! Stop analog output and close the daq driver. */
  ~ComediAnalogOutput( void );

    /*! Open the analog output device specified by \a device.
	Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close(), reset() */
  int open( const string &devicename, long mode=0 );
    /*! Returns true if driver was succesfully opened.
        \sa open(), close(), reset() */
  bool isOpen( void ) const;
    /*! Stop all activity and close the device.
        \sa open(), isOpen(), reset() */
  void close( void );

  long index( void ) const { return -1; };

    /*! Returns the mode for which the driver is opened. */
  int mode( void ) const;
    /*! Set the mode for which the driver is opened to \a mode. */
  void setMode( int mode );

    /*! Returns the name of the device file.
      \sa setDeviceName() \sa open() \sa subdevice() */
  string deviceName( void ) const;

    /*! Returns the pointer to the device file.
      \sa setDeviceName() \sa open() \sa subdevice() */
  comedi_t* device( void ) const;

    /*! Comedi internal index of analog output subdevice. */
  int subdevice( void ) const;

  int maxBufSize ( void ) const;

    /*! Number of analog output channels. */
  int channels( void ) const;

    /*! Resolution in bits of analog output. */
  int bits( void ) const;

    /*! Maximum sampling rate in Hz of analog output. */
  double maxRate( void ) const;

    /*! returns buffer-size of device in samples. */
  int bufferSize( void ) const;

    /*! Maximum number of analog output ranges. */
  int maxRanges( void ) const;
    /*! Voltage range \a index in Volt for unipolar mode.
        If -1 is returned this range is not supported. */
  double unipolarRange( int index ) const;
    /*! Voltage range \a index in Volt for bipolar mode.
        If -1 is returned this range is not supported. */
  double bipolarRange( int index ) const;

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
  int testWriteDevice( OutList &sigs );

    /*! Prepare analog output of the output signals \a sigs on the device.
	If an error ocurred in any signal, the corresponding errorflags in
	OutData are set and a negative value is returned.
	This function assumes that \a sigs successfully passed testWrite().
        The channels in \a sigs are not sorted. */
  int prepareWrite( OutList &sigs );

    /*! Convert data of the output signals \a sigs.
	If an error ocurred in any channel, the corresponding errorflags in the
	OutData structure are filled and a negative value is returned.
	The output signals are sorted by channel number first
        and are then multiplexed into a buffer of signed short's (2 byte).
        The buffer is attached to the first signal in \a sigs. */
  int convertData( OutList &sigs );

    /*! Start analog output of the output signals \a sigs
        after they were prepared by prepareWrite().
	If an error ocurred in any signal, the corresponding errorflags in
	OutData are set and a negative value is returned.
        The channels in \a sigs are not sorted.
	Also start possible pending acquisition on other devices
	that are known from take(). */
  int startWrite( OutList &sigs );

    /*! Write data to a running data acquisition.
        Returns the number of data values that were popped from the \a trace- 
	device-buffer (sum over all \a traces).
	If an error ocurred in any channel, the corresponding errorflags in the
	OutList structure are filled and a negative value is returned.  */
  int writeData( OutList &sigs );

    /*! Write data to a running data acquisition.
        Returns the number of data values that were popped from the \a trace- 
	device-buffer (sum over all \a traces).
	If an error ocurred in any channel, the corresponding errorflags in the
	OutList structure are filled and a negative value is returned.  
	For internal usage!
    */
  int fillWriteBuffer( void );
  
    /*! A template function that is used for the implementation
        of the convertData() function.
	This function first sorts the output signals by channel number
	and then multiplexes the signals in a buffer of type \a T
	after appropriate scaling.
        Data values exceeding the range of the daq board are truncated.
        The buffer is attached to the first signal in \a sigs. */
  template < typename T >
    int convert( OutList &sigs );

    /*! Stop any running ananlog output activity on the device.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, WriteError.
        \sa close(), open(), isOpen() */
  int stop ( void );

    /*! Stop any running ananlog output activity and reset the device.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, WriteError.
        \sa close(), open(), isOpen() */
  int reset( void );

    /* Reloads the prepared configuration commands of the following acquisition 
       into the registers of the hardware after stop() was performed.
       For internal usage.
       \sa stop(), prepareRead() */
  int reload( void );

    /* True, if configuration command for acquisition is successfully loaded
       into the registers of the hardware.
       For internal usage.
       \sa running(), reload() */
  bool loaded( void ) const;

    /*! True if analog output was prepared using testWriteDevice() and prepareWrite() */
  bool prepared( void ) const;
  
    /*! True if analog output is running. */
  bool running( void ) const;

    /* Sets the running status and unsets the prepared status. */
  void setRunning( void );

    /*! Get error status of the device. 
        0: no error
	-1: underrun
        other: unknown */
  int error( void ) const;

    /*! Check for every analog input and output device in \a ais and \a aos
        whether it can be simultaneously started by startRead()
	from this device (\a syncmode = 0)
	or whether the device driver can read the index of an running
	analog input at the time of starting an analog output (\a syncmode = 1).
	Add the indices of those devices to \a aiinx and \a aoinx. */
  void take( int syncmode, 
	     vector< AnalogInput* > &ais, vector< AnalogOutput* > &aos,
		     vector< int > &aiinx, vector< int > &aoinx );


private:

  long Mode;
  bool AsyncMode;
  int ErrorState;
  mutable bool IsRunning;
  bool IsPrepared;
  
  OutList *Sigs;

  string Devicename;
  comedi_t *DeviceP;
  unsigned int Subdevice;
  double MaxRate;
  unsigned int BufferElemSize;  
  bool LongSampleType;
  comedi_cmd Cmd;
  unsigned int ChanList[512];

  /*! holds the list of supported unipolar comedi ranges. */
  vector< comedi_range > UnipolarRange;
  /*! holds the list of supported bipolar comedi ranges. */
  vector< comedi_range > BipolarRange;
  /*! maps descendingly sorted range indices to (unsorted) \a UnipolarRange
      indices. */
  vector< unsigned int > UnipolarRangeIndex;
  /*! maps descendingly sorted range indices to (unsorted) \a BipolarRange
      indices. */
  vector< unsigned int > BipolarRangeIndex;
  int UnipolarExtRefRangeIndex;
  int BipolarExtRefRangeIndex;

  vector< ComediAnalogInput* > ComediAIs;
  vector< ComediAnalogOutput* > ComediAOs;
  vector< int > ComediAIsLink;
  vector< int > ComediAOsLink;

};


}; /* namespace comedi */

#endif /* ! _RELACS_COMEDI_COMEDIANALOGOUTPUT_H_ */
