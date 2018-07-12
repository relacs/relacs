/*
  daqflex/daqflexdigitalio.h
  Interface for accessing digital I/O lines of a DAQFlex board from Measurement Computing.

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


#ifndef _DAQFLEX_DAQFLEXDIGITALIO_H_
#define _DAQFLEX_DAQFLEXDIGITALIO_H_

#include <relacs/daqflex/daqflexcore.h>
#include <relacs/digitalio.h>
using namespace std;
using namespace relacs;

namespace daqflex {


/*! 
\class DAQFlexDigitalIO
\author Jan Benda
\brief [DigitalIO] Interface for accessing digital I/O lines of a DAQFlex board from Measurement Computing.
*/


class DAQFlexDigitalIO : public DigitalIO
{

public:

    /*! Create a new %DAQFlexDigitalIO without opening a device. */
  DAQFlexDigitalIO( void );
    /*! Open the digital I/O driver on DAQFlexCore device \a daqflexdevice. */
  DAQFlexDigitalIO( DAQFlexCore &daqflexdevice, const Options &opts );
    /*! Close the daq driver. */
  virtual ~DAQFlexDigitalIO( void );

    /*! Open a digital I/O device on DAQFlexCore device \a device.
 	Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close() */
  virtual int open( DAQFlexCore &daqflexdevice );
    /*! Open a digital I/O device on DAQFlexCore device \a device. */
  virtual int open( Device &device ) override;
    /*! Returns true if the device is open.
        \sa open(), close() */
  virtual bool isOpen( void ) const;
    /*! Close the device.
        \sa open(), isOpen() */
  virtual void close( void );

    /*! \return the number of digital I/O lines the device supports */
  virtual int lines( void ) const;

    /*! Configure digital I/O line \a line for input (\a output = \c false) or output 
        (\a output = \c true).
        \return 0 on success, otherwise a negative number indicating the error */
  virtual int configureLine( unsigned int line, bool output );
    /*! Configure digital I/O lines specified by \a lines for input (0) or output (1)
        according to \a output.
        \param[in] lines a bit mask of the digital lines to be configured.
        \param[in] output a bitmask for tha digital I/O lines that should 
	be configured for output.
        \return 0 on success, otherwise a negative number indicating the error */
  virtual int configureLines( unsigned int lines, unsigned int output );

    /*! Write \a val to the digital I/O line \a line.
        \param[in] line the digital line (not its bitmask!)
        \param[in] val the value that should be written to the digital output line
	(\c true: high, \c false: low).
        \return 0 on success, otherwise a negative number indicating the error
        \sa read() */
  virtual int write( unsigned int line, bool val );
    /*! Read from digital I/O line \a line and return value in \a val.
        \param[in] line the digital line (not its bitmask!)
        \param[out] val the value that was read from to the digital input line
	(\c true: high, \c false: low).
        \return 0 on success, otherwise a negative number indicating the error
        \sa write() */
  virtual int read( unsigned int line, bool &val );

    /*! Write \a val to the digital I/O lines defined in \a lines.
        \param[in] lines a bit mask selecting the digital lines to be written.
        \param[in] val a bit mask indicating what should be written to the digital output lines
	(\c 1: high, \c 0: low).
        \return 0 on success, otherwise a negative number indicating the error
        \sa read() */
  virtual int writeLines( unsigned int lines, unsigned int val );
    /*! Read digital I/O lines and return them in \a val.
        \param[in] lines a bit mask selecting the digital lines from which to read.
        \param[out] val a bit field returning the values read from to the digital input lines
	(\c 1: high, \c 0: low).
        \return 0 on success, otherwise a negative number indicating the error
        \sa write() */
  virtual int readLines( unsigned int lines, unsigned int &val );


private:

    /*! The DAQFlex device. */
  DAQFlexCore *DAQFlexDevice;

  unsigned int Levels;

};


}; /* namespace daqflex */

#endif /* ! _DAQFLEX_DAQFLEXDIGITALIO_H_ */
