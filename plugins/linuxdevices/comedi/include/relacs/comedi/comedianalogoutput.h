/*
  comedi/comedianalogoutput.h
  Interface for accessing analog output of a daq-board via comedi.

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
\brief [AnalogOutput] Interface for accessing analog output of a daq-board via comedi.
\bug NI DAQCard cmd.stop_arg += 2048 needs to tested
\bug unipolar support is not really working

\par Options
usenipfistart: Use as start source NI PFI channel

\par Trigger to analog input
You need to route the analog input start signal to pfi channel 0:
\code
*Devices
  Device1:
      plugin : ComediNIPFI
      device : /dev/comedi0
      ident  : pfi-1
      channel: 0
      routing: AI_START1
\endcode
and tell the ComediAnalogOutput that it will be triggered by this signal:
\code
*Analog Output Devices
  Device1:
      plugin       : ComediAnalogOutput
      device       : /dev/comedi0
      ident        : ao-1
      usenipfistart: 0
      delays       : 0ms
\endcode


\par Calibration:
For hardware calibrated boards (like NI E-Series boards) do
\code
$ comedi_calibrate --reset --calibrate -f /dev/comedi0
\endcode

For software calibrated boards (like NI M-Series boards) do
\code
$ comedi_soft_calibrate -f /dev/comedi0
\endcode

You may want to read the man page of \c comedi_calibrate.
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
  virtual int open( const string &device ) override;
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
        This function is always called after a successfull prepareRead().
	\param[in] sp if not null, a thread is started feeding the running analog output.
        When the thread and analog output is finished, releases the semaphore by one.
        On error, the semaphore is released by 1000 so that the process waiting
        on the semaphore is waking up immediately. */
  virtual int startWrite( QSemaphore *sp = 0 );
    /*! Write data to a running data acquisition.
        Returns the number of data values that were popped from the signal 
	device-buffers (sum over all signals).
	If an error ocurred in any channel, the corresponding errorflags in the
	OutList structure are filled and a negative value is returned.  */
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


protected:

    /*! Initialize all channels with zero. */
  void writeZeros( void );
    /*! Initializes the \a chanlist from \a sigs. */
  void setupChanList( OutList &sigs, unsigned int *chanlist, int maxchanlist, bool setscale );
    /*! Setup and test \a cmd according to \a sigs. */
  int setupCommand( OutList &sigs, comedi_cmd &cmd, bool setscale );

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

    /*! \return \c true if writeData() does not need to be called any more. */
  bool noMoreData( void ) const;

    /*! Comedi internal index of analog output subdevice. */
  int comediSubdevice( void ) const;

    /*! Return \c true if start trigger from analog input is used
        for starting analog output. */
  bool useAIStart( void ) const;

    /*! returns buffer-size of device in samples. */
  int bufferSize( void ) const;

    /*! True if analog output was prepared using testWriteDevice() and prepareWrite() */
  bool prepared( void ) const;

  virtual void initOptions() override;

private:
  
    /*! Converts the signal traces to raw integer values for the DAQ board. */
  template < typename T >
    int convert( char *cbuffer, int nbuffer );

    /*! Clear the buffers and reset status variables. */
  void clearBuffers( void );

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
    /*! The maximum valid data value. */
  vector<lsampl_t> MaxData;
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

    /*! Use as start trigger for analog input this PFI channel: */
  int UseNIPFIStart;

    /*! Comedi command for asynchronous acquisition. */
  comedi_cmd Cmd;
    /*! True if data are transferred to comedi but output ha not been started yet. */
  bool FillData;
    /*! True if the command is prepared and can be started. */
  bool IsPrepared;
    /*! True if no more data need to be written to the board. */
  bool NoMoreData;

    /*! Calibration info. */
  comedi_calibration_t *Calibration;

    /*! The sorted output signals that were prepared by prepareWrite(). */
  mutable OutList Sigs;
    /*! Size of the buffer for transfering data to the driver. */
  int BufferSize;
    /*! Buffer used for transfering data to the driver. */
  char *Buffer;
    /*! Current number of elements in the buffer. */
  int NBuffer;
    /*! Stores the last output value for each channel. */
  float *ChannelValues;
    /*! Number of data elements added to the signals. */
  int ExtendedData;

};


}; /* namespace comedi */

#endif /* ! _COMEDI_COMEDIANALOGOUTPUT_H_ */
