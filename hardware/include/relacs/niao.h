/*
  niao.h
  Implementation of AnalogOutput for National Instruments E-Series

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

#ifndef _NIAO_H_
#define _NIAO_H_

#include "nidaq.h"
#include <relacs/analogoutput.h>


/*! 
\class NIAO
\author Jan Benda
\version 1.0
\brief Implementation of AnalogOutput for National Instruments E-Series
 */

class NIAO: public AnalogOutput
{

public:

    /*! Device type id for National Instruments E-Series DAQ output devices. */
  static const int NIAnalogOutputType = 3;

    /*! Create a new NIAO without opening a device. */
  NIAO( void );
    /*! Open the analog output driver specified by its device file \a device. */
  NIAO( const string &device, long mode=0 ); 
    /*! Stop analog output and close the daq driver. */
  ~NIAO( void );           

    /*! Open the analog output device simulation */
  virtual int open( const string &device, long mode );
    /*! Returns true. */
  virtual bool isOpen( void ) const;
    /*! Close the device simulation. */
  virtual void close( void );

    /*! Number of analog output channels. */
  virtual int channels( void ) const;
    /*! Resolution in bits of analog output. */
  virtual int bits( void ) const;
    /*! Maximum sampling rate in Hz of analog output. */
  virtual double maxRate( void ) const;

    /*! Prepare analog output of the output signals \a sigs on the device. */
  virtual int prepareWrite( OutList &sigs );
    /*! Start analog output of the output signals \a sigs
        after they were prepared by prepareWrite(). */
  virtual int startWrite( OutList &sigs );
    /*! Write data of the output signals \a sigs to the analog output device. */
  virtual int writeData( OutList &sigs );

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

    /*! handle for driver. */
  int Handle;

    /*! Properties of the daq board. */
  nidaq_info Board;

};


#endif
