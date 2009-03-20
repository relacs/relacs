/*
  aosim.h
  Implementation of AnalogOutput simulating an analog output device

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

#ifndef _RELACS_AOSIM_H_
#define _RELACS_AOSIM_H_ 1

#include <relacs/analogoutput.h>

namespace relacs {


/*! 
\class AOSim
\author Jan Benda
\version 1.0
\brief Implementation of AnalogOutput simulating an analog output device
 */

class AOSim: public AnalogOutput
{

public:

    /*! Device type id for simulated DAQ output devices. */
  static const int SimAnalogOutputType = 1;

    /*! Create a new AOSim without opening a device. */
  AOSim( void );
    /*! Stop analog output and close the daq driver. */
  ~AOSim( void );           

    /*! Open the analog output device simulation */
  virtual int open( const string &device, long mode );
    /*! Open the analog output device simulation. */
  virtual int open( Device &device, long mode );
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

    /*! Directly writes from each signal in \a sigs the first data value
        to the data acquisition board. */
  virtual int directWrite( OutList &sigs );

    /*! Prepare analog output of the output signals \a sigs on the device. */
  virtual int prepareWrite( OutList &sigs );
    /*! Convert data of the output signals \a sigs. */
  virtual int convertData( OutList &sigs );
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

    /*! Return the index of the analog input device \a ais
        from which the index() obtained. */
  virtual int getAISyncDevice( const vector< AnalogInput* > &ais ) const;


 protected:

    /*! Test settings for analog output on the device
        for each output signal in \a sigs. */
  virtual int testWriteDevice( OutList &sigs );

};


}; /* namespace relacs */

#endif /* ! _RELACS_AOSIM_H_ */

