/*
  aisim.h
  Implementation of AnalogInput simulating an analog input device

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

#ifndef _RELACS_AISIM_H_
#define _RELACS_AISIM_H_ 1

#include <relacs/analoginput.h>

namespace relacs {


/*! 
\class AISim
\author Jan Benda
\version 1.0
\brief Implementation of AnalogInput simulating an analog input device
 */

class AISim: public AnalogInput
{

public:

    /*! Device type id for simulated DAQ input devices. */
  static const int SimAnalogInputType = 1;

    /*! Create a new AISim without opening a device. */
  AISim( void );
    /*! stop analog input and close the driver. */
  ~AISim( void );           

    /*! Open the analog input device simulation */
  virtual int open( const string &device, const Options &opts );
    /*! Open the analog input device simulation. */
  virtual int open( Device &device, const Options &opts );
    /*! Returns true. */
  virtual bool isOpen( void ) const;
    /*! Close the device simulation. */
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

    /*! Prepare analog input of the input traces \a traces on the device. */
  virtual int prepareRead( InList &traces );
    /*! Start analog input. */
  virtual int startRead( void );
    /*! Read data from a running data acquisition. */
  virtual int readData( void );
    /*! Convert data. */
  virtual int convertData( void );

    /*! Reset the analog input device simulation. */
  virtual int stop( void );
    /*! Reset the analog input device simulation. */
  virtual int reset( void );

    /*! True if analog input is running. */
  virtual bool running( void ) const;

    /*! get error status of the AI-device. */
  virtual int error( void ) const;


private:

    /*! Device driver specific tests on the settings in \a traces
        for each input channel. */
  virtual int testReadDevice( InList &traces );

  int MaxRanges;
  double AIuniRanges[30];
  double AIbiRanges[30];
};


}; /* namespace relacs */

#endif /* ! _RELACS_AISIM_H_ */
