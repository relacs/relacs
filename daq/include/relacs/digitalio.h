/*
  daq/digitalio.h
  Interface for accessing digital input and output lines of a data-aquisition board.

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

#ifndef _RELACS_DIGITALIO_H_
#define _RELACS_DIGITALIO_H_ 1

#include <string>
#include <relacs/device.h>

using namespace std;

namespace relacs {


/*!
\class DigitalIO
\author Jan Benda
\version 1.0
\brief Interface for accessing digital input and output lines of a data-aquisition board.
 */

class DigitalIO : public Device
{

public:

    /*! Constructs an DigitalIO with device class \a deviceclass. 
        \sa setDeviceClass() */
  DigitalIO( const string &deviceclass );
    /*! Close the daq driver.
        Reimplement this destructor. */
  virtual ~DigitalIO( void );

    /*! Open the digital I/O device specified by \a device.
 	Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close() */
  virtual int open( const string &device, long mode=0 );
    /*! Open the digital I/O device specified by \a device.
	Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close() */
  virtual int open( Device &device, long mode=0 );
    /*! Returns true if the device is open.
        \sa open(), close() */
  virtual bool isOpen( void ) const = 0;
    /*! Close the device.
        \sa open(), isOpen() */
  virtual void close( void ) = 0;

    /*! Returns a string with some information about the 
        digital I/O device. */
  virtual string info( void ) const;

    /*! \return the number of digital I/O lines the device supports */
  virtual int lines( void ) const;

    /*! Allocate the lines \a lines of the digital I/O device.
        \return 0 if all lines have been allocated
        \return WriteError if some of the lines have been already allocated */
  int allocateLines( unsigned long lines );
    /*! Free the previously allocated digital I/O lines \a lines. */
  void freeLines( unsigned long lines );

    /*! Configure digital I/O lines specified by \a mask for input (0) or output (1).
        \return 0 on success, otherwise a negative number indicating the error */
  virtual int configure( unsigned long dios, unsigned long mask ) const = 0;

    /*! Write \a val to the digital I/O line \a line.
        \return 0 on success, otherwise a negative number indicating the error
        \sa read() */
  virtual int write( unsigned long line, bool val ) = 0;
    /*! Read from digital I/O line \a line and return value in \a val.
        \return 0 on success, otherwise a negative number indicating the error
        \sa write() */
  virtual int read( unsigned long line, bool &val ) const = 0;

    /*! Write \a dios to the digital I/O lines defined in \a mask.
        \return 0 on success, otherwise a negative number indicating the error
        \sa read() */
  virtual int write( unsigned long dios, unsigned long mask ) = 0;
    /*! Read digital I/O lines and return them in \a dios.
        \return 0 on success, otherwise a negative number indicating the error
        \sa write() */
  virtual int read( unsigned long &dios ) const = 0;


protected:

    /*! The allocated digitial I/O lines. */
  unsigned long Lines;


private:

   /*! Device type id for all digital I/O devices. */
  static const int Type = 3;


};


}; /* namespace relacs */

#endif /* ! _RELACS_DIGITALIO_H_ */
