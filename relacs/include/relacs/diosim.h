/*
  relacs/diosim.h
  Implementation of DigitialIO simulating a digital I/O device.

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

#ifndef _RELACS_DIOSIM_H_
#define _RELACS_DIOSIM_H_ 1

#include <relacs/digitalio.h>

using namespace std;

namespace relacs {


/*!
\class DIOSim
\author Jan Benda
\version 1.0
\brief Implementation of DigitialIO simulating a digital I/O device.
*/

class DIOSim : public DigitalIO
{

public:

    /*! Create a new %DynClampDigitalIO without opening a device. */
  DynClampDigitalIO( void );
    /*! Open the digital I/O driver specified by its device file \a device. */
  DynClampDigitalIO( const string &device, const Options &opts );
    /*! Close the daq driver. */
  virtual ~DynClampDigitalIO( void );

    /*! Open the digital I/O device specified by \a device.
 	\return zero on success, or InvalidDevice (or any other negative number
	indicating the error).
	You should call freeLines() somewhere at the beginning 
	and setInfo() somewhere at the end of 
	the reimplmentation of this function.
        \sa isOpen(), close() */
  virtual int open( const string &device ) override;
    /*! Open the digital I/O device specified by \a device.
	\return zero on success, or InvalidDevice (or any other negative number
	indicating the error).
	You should call freeLines() somewhere at the beginning 
	and setInfo() somewhere at the end of 
	the reimplementation of this function.
        \sa isOpen(), close() */
  virtual int open( Device &device) override;
    /*! \return \c true if the device is open.
        \sa open(), close() */
  virtual bool isOpen( void ) const = 0;
    /*! Close the device.
        \sa open(), isOpen() */
  virtual void close( void ) = 0;

    /*! \return the number of digital I/O lines the device supports */
  virtual int lines( void ) const = 0;

    /*! Returns the ids for each digital I/O line. */
  virtual const Options &settings( void ) const;

    /*! Allocate the lines \a lines of the digital I/O device.
        \param[in] lines a bit mask of the digital lines to be allocated.
        \return the id, a positive number, of the allocated lines
        \return WriteError if some of the lines have been already allocated
        \sa freeLines(), allocatedLines() */
  int allocateLines( int lines );
    /*! Allocate the single digital I/O line \a line of the digital I/O device.
        Further lines can be allocated by calling allocateLine( int, int ).
        \param[in] line the digital line (not its bitmask!) to be allocated.
        \return the id, a positive number, of the allocated line
        \return WriteError if the line has been already allocated
        \sa freeLines(), allocatedLine() */
  int allocateLine( int line );
    /*! Allocate one more digital I/O line \a line of the digital I/O device for id \a id.
        The \a id should be the returned value of a previous call to
	allocateLine( int ).
        \param[in] line the digital line (not its bitmask!) to be allocated.
	\param[in] id the id under which this line should be allocated.
        \return the id, a positive number, of the allocated line (same as \a id)
        \return WriteError if the line has been already allocated
        \sa freeLines(), allocatedLine() */
  int allocateLine( int line, int id );
    /*! Free the previously allocated digital I/O lines with id \a id.
        \sa allocateLines() */
  void freeLines( int id );
    /*! \return \c true if all the digitial I/O lines \a lines have been allocated
        under id \a id.
        \param[in] lines the bit mask of the digital lines.
	\param[in] id the id under which this line was previously allocated.
        \sa allocateLines() */
  bool allocatedLines( int lines, int id );
    /*! \return \c true if all the digitial I/O lines \a lines have been allocated,
        independent of the ids.
        \param[in] lines the bit mask of the digital lines.
        \sa allocateLines() */
  bool allocatedLines( int lines );
    /*! \return \c true if digitial I/O line \a line was allocated under id \a id.
        \param[in] line the digital line (not its bitmask!)
	\param[in] id the id under which this line was previously allocated.
        \sa allocateLine() */
  bool allocatedLine( int line, int id );
    /*! \return \c true if digitial I/O line \a line is allocated,
        independent of the id.
	\param[in] line the digital line (not its bitmask!)
	\sa allocateLine() */
  bool allocatedLine( int line );

    /*! Configure digital I/O line \a line for input (\a output = \c
        false) or output (\a output = \c true).
	When reimplementing this function, call this function on success.
        \param[in] line the digital line (not its bitmask!)
        \param[in] output \c true if this line should be configured for output.
        \return 0 on success, otherwise a negative number indicating
        the error.
	\sa configureLines(), lineConfiguration() */
  virtual int configureLineUnlocked( int line, bool output ) = 0;
    /*! Configure digital I/O lines specified by \a lines for input
        (0) or output (1) according to \a output.
	When reimplementing this function, call this function on success.
        \param[in] lines a bit mask of the digital lines to be configured.
        \param[in] output a bitmask for tha digital I/O lines that should 
	be configured for output.
        \return 0 on success, otherwise a negative number indicating
        the error.
	\sa configureLine(), lineConfiguration() */
  virtual int configureLines( int lines, int output );
    /*! Returns the configuation of an digital I/O line.
        \param[in] line the digital line (channel)
	\return \c true, if the line has been configured for output.
	\sa configureLine(), configureLines() */
  bool lineConfiguration( int line ) const;

    /*! Write \a val to the digital I/O line \a line.
        \param[in] line the digital line (not its bitmask!)
        \param[in] val the value that should be written to the digital
        output line (\c true: high, \c false: low).
        \return 0 on success, otherwise a negative number indicating the error
        \sa read() */
  virtual int writeUnlocked( int line, bool val ) = 0;
    /*! Read from digital I/O line \a line and return value in \a val.
        \param[in] line the digital line (not its bitmask!)
        \param[out] val the value that was read from to the digital
        input line (\c true: high, \c false: low).
        \return 0 on success, otherwise a negative number indicating
	the error
        \sa write() */
  virtual int readUnlocked( int line, bool &val ) const = 0;

    /*! Write \a val to the digital I/O lines defined in \a lines.
        \param[in] lines a bit mask selecting the digital lines to be
        written.
        \param[in] val a bit mask indicating what should be written to
        the digital output lines (\c 1: high, \c 0: low).
        \return 0 on success, otherwise a negative number indicating
        the error
        \sa read() */
  virtual int writeLines( int lines, int val ) = 0;
    /*! Read digital I/O lines and return them in \a val.
        \param[in] lines a bit mask selecting the digital lines from
        which to read.
        \param[out] val a bit field returning the values read from to
        the digital input lines (\c 1: high, \c 0: low).
        \return 0 on success, otherwise a negative number indicating
        the error
        \sa write() */
  virtual int readLines( int lines, int &val ) const = 0;

    /*! Enables generation of TTL Pulses on a previously defined DIO
        line and the scaling of the current for analog output
        according to the measured period divided by the curent
        injection time of \a duration microseconds. This is used for
        synchronizing a discontinous current-clamp amplifier with an
        dynamic-clamp loop. 
	\param[in] duration the duration of the current injection of 
	the amplifier in seconds.
	\return 0 on success, a Device error code on failure.
	\sa clearSyncPulse() */
  virtual int setSyncPulse( double duration );
    /*! Disable TTL Pulse generation and current scaling.
	\return 0 on success, a Device error code on failure.
	\sa setSyncPulse() */
  virtual int clearSyncPulse( void );


protected:

    /*! Free all allocated digital I/O lines.
        \sa freeLines() */
  void freeLines( void );

    /*! Set the device info().
        Call this function from open().
        \sa info() */
  void setInfo( void );

    /*! The maximum number of digital I/O lines. */
  static const int MaxDIOLines = 32;


private:

    /*! The ids of allocated digitial I/O lines. */
  int DIOLineIDs[MaxDIOLines];
    /*! The configuration of all digitial I/O lines. */
  bool DIOLineWriteable[MaxDIOLines];


};


}; /* namespace relacs */

#endif /* ! _RELACS_DIOSIM_H_ */
