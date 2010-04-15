/*
  comedi/comedianalogoutput.h
  Interface for accessing analog output of a daq-board via comedi.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _COMEDI_COMEDIANALOGOUTPUT_H_
#define _COMEDI_COMEDIANALOGOUTPUT_H_

#include <vector>
#include <comedilib.h>
#include <relacs/analogoutput.h>
using namespace std;
using namespace relacs;

namespace comedi {


class ComediAnalogInput;

/*! 
\class ComediAnalogOutput
\author Marco Hackenberg
\author Jan Benda
\version 0.2
\brief [AnalogOutput] Interface for accessing analog output of a daq-board via comedi.
\todo manual convert of signals does not work, because it needs the comedi_polynomial

\code
# calibrate all ranges, references and channels:
for C in 0 1; do for A in 0 1 2; do for R in 0 1; do comedi_calibrate -reset -calibrate -f /dev/comedi0 -s 1 -r $R -a $A -c $C; done; done; done
\endcode
*/


class ComediAnalogOutput : public AnalogOutput
{

  friend class ComediAnalogInput;
  friend class DynClampAnalogOutput;

public:

    /*! Create a new ComediAnalogOutput without opening a device. */
  ComediAnalogOutput( void );
    /*! Open the analog output driver specified by its device file \a device. */
  ComediAnalogOutput( const string &device, const Options &opts );
    /*! Stop analog output and close the daq driver. */
  virtual ~ComediAnalogOutput( void );

    /*! Open the analog output device on device file \a device.
        \todo  if a ranges is not supported but comedi thinks so: set max = -1.0,
        i.e. NI 6070E PCI: range #3&4 (-1..1V, 0..1V) not supported
        \todo maybe use an internal maximum buffer size (in case comedi max is way too much)? */
  virtual int open( const string &device, const Options &opts );
    /*! Returns true if driver was succesfully opened. */
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

    /*! Convert data of the output signals \a sigs.
	If an error ocurred in any channel, the corresponding errorflags in the
	OutData structure are filled and a negative value is returned.
	The output signals are sorted by channel number first
        and are then multiplexed into a buffer of signed short's (2 byte).
        The buffer is attached to the first signal in \a sigs. */
  virtual int convertData( OutList &sigs );
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

    /*! Stop any running ananlog output activity and reset the device.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, WriteError.
        \sa close(), open(), isOpen() */
  virtual int reset( void );
  
    /*! True if analog output is running. */
  virtual bool running( void ) const;

    /*! Get error status of the device. 
        0: no error
	-1: underrun
        other: unknown */
  virtual int error( void ) const;

    /*! Check for every analog output device in \a aos
        whether it can be simultaneously started by startWrite()
        from this device. */
  virtual void take( const vector< AnalogOutput* > &aos,
                     vector< int > &aoinx, vector< bool > &aorate );


protected:

    /*! Initializes the \a chanlist from \a sigs. */
  void setupChanList( OutList &sigs, unsigned int *chanlist, int maxchanlist );
    /*! Setup and test \a cmd according to \a sigs. */
  int setupCommand( OutList &sigs, comedi_cmd &cmd );

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
  int fillWriteBuffer( OutList &sigs );

    /*! Execute the command that was prepared by prepareWrite(). */
  int executeCommand( void );
    /*! Clear the command that was prepared by prepareWrite()
        after successfull execution via the instruction list. */
  void clearCommand( void );

    /*! Comedi internal index of analog output subdevice. */
  int comediSubdevice( void ) const;

    /*! returns buffer-size of device in samples. */
  int bufferSize( void ) const;

    /*! True if analog output was prepared using testWriteDevice() and prepareWrite() */
  bool prepared( void ) const;


private:
  
    /*! Converts the signal traces to raw integer values for the DAQ board. */
  template < typename T >
    int convert( OutList &sigs );

    /*! Unique analog I/O device type id for all 
        Comedi DAQ devices. */
  static const int ComediAnalogIOType = 1;

    /*! Pointer to the comedi device. */
  comedi_t *DeviceP;
    /*! The comedi subdevice number. */
  unsigned int SubDevice;
    /*! True if the sample type is lsampl_t. */
  bool LongSampleType;
    /*! The size of a single sample in bytes. */
  unsigned int BufferElemSize;  
    /*! The maximum sampling rate supported by the DAQ board. */
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

    /*! List of analog output subdevices that can be 
        started via an instruction list together with this subdevice. */
  vector< ComediAnalogOutput* > ComediAOs;

    /*! Comedi command for asynchronous acquisition. */
  comedi_cmd Cmd;
    /*! True if the command is prepared. */
  bool IsPrepared;

  int ErrorState;

    /*! Calibration info. */
  comedi_calibration_t *Calibration;

    /*! The output signals that were prepared by prepareWrite(). */
  OutList *Sigs;
    /*! Size of the internal buffer used for getting the data from the driver. */
  int BufferSize;

};


}; /* namespace comedi */

#endif /* ! _COMEDI_COMEDIANALOGOUTPUT_H_ */
