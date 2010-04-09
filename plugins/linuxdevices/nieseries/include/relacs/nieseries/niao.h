/*
  nieseries/niao.h
  Implementation of AnalogOutput for National Instruments E-Series

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

#ifndef _RELACS_NIESERIES_NIAO_H_
#define _RELACS_NIESERIES_NIAO_H_ 1

#include <relacs/nieseries/nidaq.h>
#include <relacs/analogoutput.h>
using namespace relacs;

namespace nieseries {


/*! 
\class NIAO
\author Jan Benda
\version 1.0
\brief [AnalogOutput] Implementation of AnalogOutput for National Instruments E-Series
 */

class NIAO: public AnalogOutput
{

public:

    /*! Create a new NIAO without opening a device. */
  NIAO( void );
    /*! Open the analog output driver specified by its device file \a device. */
  NIAO( const string &device, const Options &opts ); 
    /*! Stop analog output and close the daq driver. */
  ~NIAO( void );           

    /*! Open the analog output on device file \a device */
  virtual int open( const string &device, const Options &opts );
    /*! Returns true if the device is open. */
  virtual bool isOpen( void ) const;
    /*! Close the device. */
  virtual void close( void );

    /*! Number of analog output channels. */
  virtual int channels( void ) const;
    /*! Resolution in bits of analog output. */
  virtual int bits( void ) const;
    /*! Maximum sampling rate in Hz of analog output. */
  virtual double maxRate( void ) const;

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
    /*! Prepare analog output of the output signals \a sigs on the device. */
  virtual int prepareWrite( OutList &sigs );
    /*! Start analog output. */
  virtual int startWrite( void );
    /*! Write data of the output signals to the analog output device. */
  virtual int writeData( void );

    /*! Reset the analog output device simulation. */
  virtual int reset( void );

    /*! True if analog output is running. */
  virtual bool running( void ) const;

    /*! get error status of the AI-device. */
  virtual int error( void ) const;

    /*! Index of signal start. */
  virtual long index( void ) const;
    /*! Return the index of the analog input device \a ais
        from which the index() obtained. */
  virtual int getAISyncDevice( const vector< AnalogInput* > &ais ) const;


 protected:

    /*! Test settings for analog output on the device
        for each output signal in \a sigs. */
  virtual int testWriteDevice( OutList &sigs );

  
 private:

    /*! Unique analog I/O device type id for all 
        National Instruments E-Series DAQ devices. */
  static const int NIAnalogIOType = 100;

    /*! handle for driver. */
  int Handle;

    /*! Properties of the daq board. */
  nidaq_info Board;

    /*! The output signals that were prepared by prepareWrite(). */
  OutList *Sigs;
};


}; /* namespace nieseries */

#endif /* ! _RELACS_NIESERIES_NIAO_H_ */
