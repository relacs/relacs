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

A digital I/O device has lines() of input or output lines that can be high or low.
There a two ways to access the digital I/O lines.
Either individually by their line (channel) number,
or in groups by a bit pattern, where ech bit represents a single line (channel).

After opening the device by open(), the digitial I/) lines that you are going to use
should be allocated by allocateLines() or allocateLine().
Then the digital I/O lines should be configured for input or output using 
configureLines() or configureLine().

Bits can be read from or written to the digitial I/O lines by the read() and write() functions.
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
 	\return zero on success, or InvalidDevice (or any other negative number
	indicating the error).
	You should call freeLines() somewhere in the reimplmentation of this function.
        \sa isOpen(), close() */
  virtual int open( const string &device, long mode=0 );
    /*! Open the digital I/O device specified by \a device.
	You should call freeLines() somewhere in the reimplmentation of this function.
	\return zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close() */
  virtual int open( Device &device, long mode=0 );
    /*! \return \c true if the device is open.
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
        \return the id, a positive number, of the allocated lines
        \return WriteError if some of the lines have been already allocated
        \sa freeLines(), allocated() */
  int allocateLines( unsigned long lines );
    /*! Allocate the single digital I/O line \a line of the digital I/O device.
        Further lines can be allocated by calling allocateLine( int, int ).
        \return the id, a positive number, of the allocated line
        \return WriteError if the line has been already allocated
        \sa freeLines(), allocated() */
  int allocateLine( int line );
    /*! Allocate one more digital I/O line \a line of the digital I/O device for id \a id.
        The \a id should be the returned value of a previous call to
	allocateLine( int ).
        \return the id, a positive number, of the allocated line (same as \a id)
        \return WriteError if the line has been already allocated
        \sa freeLines(), allocated() */
  int allocateLine( int line, int id );
    /*! Free the previously allocated digital I/O lines with id \a id.
        \sa allocatedLines() */
  void freeLines( int id );
    /*! \return \c true if digitial I/O line \a line was allocated under id \a id.
        \sa allocateLines() */
  bool allocated( int line, int id );
    /*! \return \c true if digitial I/O line \a line is allocated, independent of the id.
        \sa allocateLines() */
  bool allocated( int line );

    /*! Configure digital I/O line \a line for input (\a output = \c false) or output 
        (\a output = \c true).
        \return 0 on success, otherwise a negative number indicating the error */
  virtual int configureLine( int line, bool output ) const = 0;
    /*! Configure digital I/O lines specified by \a lines for input (0) or output (1)
        according to \a output.
        \return 0 on success, otherwise a negative number indicating the error */
  virtual int configureLines( unsigned long lines, unsigned long output ) const = 0;

    /*! Write \a val to the digital I/O line \a line.
        \return 0 on success, otherwise a negative number indicating the error
        \sa read() */
  virtual int write( int line, bool val ) = 0;
    /*! Read from digital I/O line \a line and return value in \a val.
        \return 0 on success, otherwise a negative number indicating the error
        \sa write() */
  virtual int read( int line, bool &val ) const = 0;

    /*! Write \a val to the digital I/O lines defined in \a lines.
        \return 0 on success, otherwise a negative number indicating the error
        \sa read() */
  virtual int write( unsigned long lines, unsigned long val ) = 0;
    /*! Read digital I/O lines and return them in \a val.
        \return 0 on success, otherwise a negative number indicating the error
        \sa write() */
  virtual int read( unsigned long &val ) const = 0;


protected:

    /*! Free all allocated digital I/O lines.
        \sa freeLines() */
  void freeLines( void );


private:

   /*! Device type id for all digital I/O devices. */
  static const int Type = 3;

    /*! The maximum number of digital I/O lines. */
  static const int MaxDIOLines = 32;
    /*! The ids of allocated digitial I/O lines. */
  int DIOLines[32];


};


}; /* namespace relacs */

#endif /* ! _RELACS_DIGITALIO_H_ */
