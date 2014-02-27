/*
  comedi/comedianaloginput.h
  Interface for accessing analog input of a daq-board via comedi.

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

#ifndef _COMEDI_COMEDIANALOGINPUT_H_
#define _COMEDI_COMEDIANALOGINPUT_H_

#include <comedilib.h>
#include <vector>
#include <relacs/analoginput.h>
using namespace std;
using namespace relacs;

namespace comedi {


class ComediAnalogOutput;

/*! 
\class ComediAnalogInput
\author Marco Hackenberg
\author Jan Benda
\brief [AnalogInput] Interface for accessing analog input of a daq-board via comedi.
\todo Support delays in testReadDevice() and convertData()!
\todo Error handling in readData()

\par Options:
- \c gainblacklist: List of daq board gains that should not be used. Each gain is identified by its
  maximal range value in volts.

\code
# for NI E-Series PCI daq boards
modprobe ni_pcimio
comedi_config /dev/comedi0 ni_pcimio
# for NI E-Series DaqCard
modprobe ni_mio_cs
comedi_config /dev/comedi0 ni_mio_cs

# calibrate all ranges, references and channels:
for C in 0 $(seq 16); do for A in 0 1 2; do for R in 0 $(seq 20); do comedi_calibrate -reset -calibrate -f /dev/comedi0 -s 0 -r $R -a $A -c $C; done; done; done
\endcode
*/


class ComediAnalogInput : public AnalogInput
{

  friend class ComediAnalogOutput;
  friend class DynClampAnalogInput;

public:

    /*! Create a new ComediAnalogInput without opening a device. */
  ComediAnalogInput( void );
    /*! Open the analog input driver specified by its device file \a device. */
  ComediAnalogInput( const string &device, const Options &opts );
    /*! Stop analog input and close the daq driver. */
  virtual ~ComediAnalogInput( void );

    /*! Open the analog input device on device file \a device.
        \todo  if a ranges is not supported but comedi thinks so: set max = -1.0
        i.e. NI 6070E PCI: range #8 (0..20V) not supported
        \todo maybe use an internal maximum buffer size (in case comedi max is way too much)? */
  virtual int open( const string &device, const Options &opts );
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

    /*! Prepare analog input of the input traces \a traces on the device.
	If an error ocurred in any trace, the corresponding errorflags in
	InData are set and a negative value is returned.
	This function assumes that \a traces successfully passed testRead().
        The channels in \a traces are not sorted. */
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
  
    /*! True if the analog input driver is running. */
  virtual bool running( void ) const;

    /*! Check for every analog input and analog output device in \a ais
        and \a aos, respectively,
        whether it can be simultaneously started by startRead()
        from this device. */
  virtual void take( const vector< AnalogInput* > &ais,
		     const vector< AnalogOutput* > &aos,
		     vector< int > &aiinx, vector< int > &aoinx,
		     vector< bool > &airate, vector< bool > &aorate );


protected:

    /*! Setup and test \a cmd according to \a traces. */
  int setupCommand( InList &traces, comedi_cmd &cmd );

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

    /*! Execute the command that was prepared by prepareRead(). */
  int executeCommand( void );

    /*! Convert \a n data values from the daq board \a buffer and push them into \a traces. */
  template< typename T >
    void convert( InList &traces, char *buffer, int n );

    /*! Comedi internal index of analog input subdevice. */
  int comediSubdevice( void ) const;

    /*! returns buffer-size of device in samples. */
  int bufferSize( void ) const;
  
    /*! True if analog input was prepared using testReadDevice() and prepareRead() */
  bool prepared( void ) const;


private:

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
    /*! Maps unipolar range indices to comei range indices. */
  vector< unsigned int > UnipolarRangeIndex;
    /*! Maps bipolar range indices to comei range indices. */
  vector< unsigned int > BipolarRangeIndex;

    /*! List of analog input subdevices that can be 
        started via an instruction list together with this subdevice. */
  vector< ComediAnalogInput* > ComediAIs;
    /*! List of analog output subdevices that can be 
        started via an instruction list together with this subdevice. */
  vector< ComediAnalogOutput* > ComediAOs;

    /*! Comedi command for asynchronous acquisition. */
  comedi_cmd Cmd;
    /*! True if the command is prepared. */
  bool IsPrepared;

    /*! True if a command is supposed to be running.
        \note this differs from running(), which indicated that the driver is still running. */
  bool IsRunning;

    /*! Calibration info. */
  comedi_calibration_t *Calibration;

    /*! The input traces that were prepared by prepareRead(). */
  InList *Traces;
    /*! Size of the driver buffer used for getting the data from the daq board. */
  int ReadBufferSize;
    /*! Size of the internal buffer used for getting the data from the driver. */
  int BufferSize;
    /*! The number of samples written so far to the internal buffer. */
  int BufferN;
    /*! The internal buffer used for getting the data from the driver. */
  char *Buffer;
    /*! Index to the trace in the internal buffer. */
  int TraceIndex;

    /*! The total number of samples to be acquired, 0 for continuous acquisition. */
  int TotalSamples;
    /*! The number of samples so far read in by readData(). */
  int CurrentSamples;

};


}; /* namespace comedi */

#endif /* ! _COMEDI_COMEDIANALOGINPUT_H_ */
