/*
  comedianaloginput.h
  Interface for accessing analog input of a daq-board via comedi.

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

#ifndef _COMEDIANALOGINPUT_H_
#define _COMEDIANALOGINPUT_H_

#include <comedilib.h>
#include <vector>
#include <relacs/analoginput.h>
using namespace std;

class ComediAnalogOutput;

/*! 
\class AnalogInput
\author Marco Hackenberg
\version 0.1
\brief Interface for accessing analog input of a daq-board via comedi.


insmod /usr/src/kernels/rtai/modules/rtai_hal.ko
insmod /usr/src/kernels/rtai/modules/rtai_ksched.ko
modprobe kcomedilib
modprobe ni_pcimio
sleep 1
/usr/src/comedilib/comedi_config/comedi_config /dev/comedi0 ni_pcimio
sleep 1
/usr/src/comedilib/comedi_calibrate/comedi_calibrate /dev/comedi0

*/


class ComediAnalogInput : public AnalogInput
{

  friend class DynClampAnalogInput;

public:

   /*! Device type id for comedi DAQ input devices. */
  static const int ComediAnalogInputType = 3;


    /*! Create a new ComediAnalogInput without opening a device. */
  ComediAnalogInput( void );
    /*! Constructs an ComediAnalogInput with device name \a device
        and type id \a aitype.  
        \sa setDeviceName() */
  ComediAnalogInput( const string &devicename );
    /*! Stop analog input and close the daq driver. */
  ~ComediAnalogInput( void );

    /*! Open the analog input device specified by \a device.
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

    /*! Comedi internal index of analog input subdevice. */
  int subdevice( void ) const;

  int maxBufSize ( void ) const;

    /*! Number of analog input channels. */
  int channels( void ) const;

    /*! Resolution in bits of analog input. */
  int bits( void ) const;

    /*! Maximum sampling rate in Hz of analog input. */
  double maxRate( void ) const;

    /*! returns buffer-size of device in samples. */
  int bufferSize( void ) const;

    /*! Maximum number of analog input ranges. */
  int maxRanges( void ) const;
    /*! Voltage range \a index in Volt for unipolar mode.
        If -1 is returned this range is not supported. */
  double unipolarRange( int index ) const;
    /*! Voltage range \a index in Volt for bipolar mode.
        If -1 is returned this range is not supported. */
  double bipolarRange( int index ) const;

    /*! Device driver specific tests on the settings in \a sigs
        for each input channel.
	Before this function is called, the validity of the settings in 
	\a sigs was already tested by testReadData().
	This function should test whether the settings are really supported
	by the hardware.
	If an error ocurred in any trace, the corresponding errorflags in the
	InData are set and a negative value is returned.
        The channels in \a sigs are not sorted.
        This function is called by testRead(). */
  int testReadDevice( InList &traces );

    /*! Prepare analog input of the input traces \a traces on the device.
	If an error ocurred in any trace, the corresponding errorflags in
	InData are set and a negative value is returned.
	This function assumes that \a traces successfully passed testRead().
        The channels in \a traces are not sorted. */
  int prepareRead( InList &traces );

    /*! Start analog input of the input traces \a traces on the device
        after they were prepared by prepareRead().
	If an error ocurred in any channel, the corresponding errorflags in the
	InData structure are filled and a negative value is returned.
        The channels in \a traces are not sorted.
	Also start possible pending acquisition on other devices
	that are known from take(). */
  int startRead( InList &sigs );

    /*! Read data from a running data acquisition.
        Returns the number of new data values that were added to the \a traces
	(sum over all \a traces).
	If an error ocurred in any channel, the corresponding errorflags in the
	InList structure are filled and a negative value is returned.  */
  int readData( InList &sigs );

    /*! Stop any running ananlog input activity on the device.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, ReadError.
        \sa close(), open(), isOpen() */
  int stop ( void );

    /*! Stop any running ananlog input activity and reset the device.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, ReadError.
        \sa close(), open(), isOpen() */
  int reset( void );
  
    /* Reloads the prepared configuration commands of the following acquisition 
       into the registers of the hardware after stop() was performed.
       Deletes all data values that a still available via readData().
       For internal usage.
       \sa stop(), readData(), prepareRead() */
  int reload( void );

    /* True, if configuration command for acquisition is successfully loaded
       into the registers of the hardware.
       For internal usage.
       \sa running(), reload() */
  bool loaded( void ) const;

    /*! True if analog input was prepared using testReadDevice() and prepareRead() */
  bool prepared( void ) const;
  
    /*! True if analog input is running. */
  bool running( void ) const;

    /* Sets the running status and unsets the prepared status. For internal 
       usage. */
  void setRunning( void );

    /*! Get error status of the device. 
        0: no error
	-1: input buffer overflow
        other: unknown */
  int error( void ) const;

    /*! Check for every analog input and input device in \a ais and \a aos
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

  string Devicename;
  comedi_t *DeviceP;
  unsigned int Subdevice;
  double MaxRate;
  bool LongSampleType;
  unsigned int BufferElemSize;  
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

  vector< ComediAnalogInput* > ComediAIs;
  vector< ComediAnalogOutput* > ComediAOs;
  vector< int > ComediAIsLink;
  vector< int > ComediAOsLink;

};


#endif
