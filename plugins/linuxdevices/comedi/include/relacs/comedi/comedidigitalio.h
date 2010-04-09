/*
  comedi/comedidigitalio.h
  Interface for accessing digital I/O lines of a daq-board via comedi.

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

#ifndef _COMEDI_COMEDIDIGITALIO_H_
#define _COMEDI_COMEDIDIGITALIO_H_

#include <comedilib.h>
#include <vector>
#include <relacs/digitalio.h>
using namespace std;
using namespace relacs;

namespace comedi {


/*! 
\class ComediDigitalIO
\author Jan Benda
\brief [DigitalIO] Interface for accessing digital I/O lines of a daq-board via comedi.

\par Options
- \c subdevice: the subdevice id of the digital I/O (check with comedi_test -t info).
- \c startsubdevice: start searching for a digital I/O subdevice at the subdevice with the specified id.
*/


class ComediDigitalIO : public DigitalIO
{

public:

    /*! Create a new ComediDigitalIO without opening a device. */
  ComediDigitalIO( void );
    /*! Open the digital I/O driver specified by its device file \a device. */
  ComediDigitalIO( const string &device, const Options &opts );
    /*! Close the daq driver. */
  virtual ~ComediDigitalIO( void );

    /*! Open a digital I/O device on the device \a device.
        The digital I/O subdevice can be specified by the "subdevice" option in \a opts.
        Alternatively, the first digital I/O subdevice is taken.
	The search for the digital I/O subdevice starts at the subdevice specified
	by the "startsubdevice" option in \a opts (defaults to "0").
 	Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close() */
  virtual int open( const string &device, const Options &opts );
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
  virtual int configureLine( int line, bool output ) const;
    /*! Configure digital I/O lines specified by \a lines for input (0) or output (1)
        according to \a output.
        \return 0 on success, otherwise a negative number indicating the error */
  virtual int configureLines( unsigned long lines, unsigned long output ) const;

    /*! Write \a val to the digital I/O line \a line.
        \return 0 on success, otherwise a negative number indicating the error
        \sa read() */
  virtual int write( int line, bool val );
    /*! Read from digital I/O line \a line and return value in \a val.
        \return 0 on success, otherwise a negative number indicating the error
        \sa write() */
  virtual int read( int line, bool &val ) const;

    /*! Write \a val to the digital I/O lines defined in \a lines.
        \return 0 on success, otherwise a negative number indicating the error
        \sa read() */
  virtual int write( unsigned long lines, unsigned long val );
    /*! Read digital I/O lines and return them in \a val.
        \return 0 on success, otherwise a negative number indicating the error
        \sa write() */
  virtual int read( unsigned long &val ) const;


private:

    /*! Pointer to the comedi device. */
  comedi_t *DeviceP;
    /*! The comedi subdevice number. */
  unsigned int SubDevice;
    /*! The number of supported digital I/O lines */
  int MaxLines;

};


}; /* namespace comedi */

#endif /* ! _COMEDI_COMEDIDIGITALIO_H_ */
