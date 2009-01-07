/*
  nieseries/niai.h
  Implementation of AnalogInput for National Instruments E-Series.

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

#ifndef _RELACS_NIESERIES_NIAI_H_
#define _RELACS_NIESERIES_NIAI_H_ 1

#include <relacs/nieseries/nidaq.h>
#include <relacs/analoginput.h>
using namespace relacs;

namespace nieseries {


/*! 
\class NIAI
\author Jan Benda
\version 1.0
\brief [AnalogInput] Implementation of AnalogInput for National Instruments E-Series.
\todo error(): map error code appropriately
 */

class NIAI: public AnalogInput
{

public:

    /*! Create a new NIAI without opening a device. */
  NIAI( void );
    /*! Open the analog input driver specified by its device file \a device. */
  NIAI( const string &device, long mode=0 ); 
    /*! Stop analog input and close the driver. */
  ~NIAI( void );           

    /*! Open the analog input on device file \a device. */
  virtual int open( const string &device, long mode=0 );
    /*! Returns true if the device is open. */
  virtual bool isOpen( void ) const;
    /*! Close the device. */
  virtual void close( void );

    /*! Number of analog input channels. */
  virtual int channels( void ) const;
    /*! Resolution in bits of analog input. */
  virtual int bits( void ) const;
    /*! Maximum sampling rate in Hz of analog input. */
  virtual double maxRate( void ) const;

    /*! Maximum number of analog input ranges. */
  virtual int maxRanges( void ) const;
    /*! Voltage range \a index in millivolt for unipolar mode.
        If -1 is returned this range is not supported. */
  virtual double unipolarRange( int index ) const;
    /*! Voltage range \a index in millivolt for bipolar mode.
        If -1 is returned this range is not supported. */
  virtual double bipolarRange( int index ) const;

    /*! Prepare analog input of the input traces \a traces on the device. */
  virtual int prepareRead( InList &traces );
    /*! Start analog input of the inpput traces \a traces on the device. */
  virtual int startRead( InList &traces );
    /*! Read data from a running data acquisition. */
  virtual int readData( InList &traces );

    /*! Stop any running ananlog input activity,
        but preserve all so far read in data.
	The next call to readData() will return the data. */
  virtual int stop( void );
    /*! Reset the analog input device simulation. */
  virtual int reset( void );

    /*! True if analog input is running. */
  virtual bool running( void ) const;

    /*! get error status of the AI-device. */
  virtual int error( void ) const;


 protected:

    /*! Test settings for analog input on the device
        for each input channel in \a traces. */
  virtual int testReadDevice( InList &traces );

    /*! Convert \a n data values from the daq board \a buffer and push them into \a traces. */
  void convert( InList &traces, signed short *buffer, int n );

  
 private:

    /*! Unique analog I/O device type id for all 
        National Instruments E-Series DAQ devices. */
  static const int NIAnalogIOType = 100;

    /*! handle for driver. */
  int Handle;

    /*! Properties of the daq board. */
  nidaq_info Board;

    /*! maximum number of analog input ranges. */
  int MaxRanges;

    /*! Size of the internal buffer used for getting the data from the driver. */
  int BufferSize;
    /*! Index to the trace in the internal buffer. */
  int TraceIndex;

};


}; /* namespace nieseries */

#endif /* ! _RELACS_NIESERIES_NIAI_H_ */
