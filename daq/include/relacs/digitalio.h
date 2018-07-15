/*
  daq/digitalio.h
  Interface for accessing digital input and output lines of a data-aquisition board.

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

#ifndef _RELACS_DIGITALIO_H_
#define _RELACS_DIGITALIO_H_ 1

#include <relacs/device.h>

using namespace std;

namespace relacs {


/*!
\class DigitalIO
\author Jan Benda
\version 1.0
\brief Interface for accessing digital input and output lines of a data-aquisition board.

A digital I/O device has lines() of input or output lines that can be
high or low.
There a two ways to access the digital I/O lines.
Either individually by their line (channel) number, or in groups by a
bit pattern, where ech bit represents a single line (channel).

After opening the device by open(), the digitial I/O lines that you
are going to use should be allocated by allocateLines() or
allocateLine().

Then the digital I/O lines should be configured for input or output
using configureLines() or configureLine().

Bits can be read from or written to the digitial I/O lines by the
read() and write() functions.

In case you want to use a digitial I/O device within RELACS, your
DigitalIO implementation needs to provide a void default constructor
(i.e. with no parameters) that does not open the device.  Also,
include the header file \c <relacs/relacsplugin.h> and make the
AnalogInput device known to RELACS with the \c addDigitalIO(
ClassNameOfYourDigitalIOImplementation, PluginSetName ) macro.
*/

class DigitalIO : public Device
{

public:

    /*! Constructs an %DigitalIO with device class \a deviceclass. 
        \sa setDeviceClass() */
  DigitalIO( const string &deviceclass );
    /*! Close the daq driver.
        Reimplement this destructor. */
  virtual ~DigitalIO( void );

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
  int allocateLines( unsigned int lines );
    /*! Allocate the single digital I/O line \a line of the digital I/O device.
        Further lines can be allocated by calling allocateLine( int, int ).
        \param[in] line the digital line (not its bitmask!) to be allocated.
        \return the id, a positive number, of the allocated line
        \return WriteError if the line has been already allocated
        \sa freeLines(), allocatedLine() */
  int allocateLine( unsigned int line );
    /*! Allocate one more digital I/O line \a line of the digital I/O device for id \a id.
        The \a id should be the returned value of a previous call to
	allocateLine( int ).
        \param[in] line the digital line (not its bitmask!) to be allocated.
	\param[in] id the id under which this line should be allocated.
        \return the id, a positive number, of the allocated line (same as \a id)
        \return WriteError if the line has been already allocated
        \sa freeLines(), allocatedLine() */
  int allocateLine( unsigned int line, int id );
    /*! Free the previously allocated digital I/O lines with id \a id.
        \sa allocateLines() */
  void freeLines( int id );
    /*! \return \c true if all the digitial I/O lines \a lines have been allocated
        under id \a id.
        \param[in] lines the bit mask of the digital lines.
	\param[in] id the id under which this line was previously allocated.
        \sa allocateLines() */
  bool allocatedLines( unsigned int lines, int id );
    /*! \return \c true if all the digitial I/O lines \a lines have been allocated,
        independent of the ids.
        \param[in] lines the bit mask of the digital lines.
        \sa allocateLines() */
  bool allocatedLines( unsigned int lines );
    /*! \return \c true if digitial I/O line \a line was allocated under id \a id.
        \param[in] line the digital line (not its bitmask!)
	\param[in] id the id under which this line was previously allocated.
        \sa allocateLine() */
  bool allocatedLine( unsigned int line, int id );
    /*! \return \c true if digitial I/O line \a line is allocated,
        independent of the id.
	\param[in] line the digital line (not its bitmask!)
	\sa allocateLine() */
  bool allocatedLine( unsigned int line );

    /*! Configure digital I/O line \a line for input (\a output = \c
        false) or output (\a output = \c true) while locking the device.
	When reimplementing this function, call this function on success.
        \param[in] line the digital line (not its bitmask!)
        \param[in] output \c true if this line should be configured for output.
        \return 0 on success, otherwise a negative number indicating
        the error.
	\sa configureLineUnlocked(), configureLines(),
	lineConfiguration(), lock() */
  int configureLine( unsigned int line, bool output );
    /*! Configure digital I/O line \a line for input (\a output = \c
        false) or output (\a output = \c true) without locking the device.
	When reimplementing this function, call this function on success.
        \param[in] line the digital line (not its bitmask!)
        \param[in] output \c true if this line should be configured for output.
        \return 0 on success, otherwise a negative number indicating
        the error.
	\sa configureLines(), lineConfiguration() */
  virtual int configureLineUnlocked( unsigned int line, bool output );
    /*! Configure digital I/O lines specified by \a lines for input
        (0) or output (1) according to \a output.
	When reimplementing this function, call this function on success.
        \param[in] lines a bit mask of the digital lines to be configured.
        \param[in] output a bitmask for tha digital I/O lines that should 
	be configured for output.
        \return 0 on success, otherwise a negative number indicating
        the error.
	\sa configureLine(), lineConfiguration() */
  virtual int configureLines( unsigned int lines, unsigned int output );
    /*! Returns the configuation of an digital I/O line.
        \param[in] line the digital line (channel)
	\return \c true, if the line has been configured for output.
	\sa configureLine(), configureLines() */
  bool lineConfiguration( unsigned int line ) const;

    /*! Write \a val to the digital I/O line \a line while locking the device.
        \param[in] line the digital line (not its bitmask!)
        \param[in] val the value that should be written to the digital
        output line (\c true: high, \c false: low).
        \return 0 on success, otherwise a negative number indicating the error
        \sa writeUnlocked(), writeLines(), read(), lock() */
  int write( unsigned int line, bool val );
    /*! Write \a val to the digital I/O line \a line without locking the device.
        \param[in] line the digital line (not its bitmask!)
        \param[in] val the value that should be written to the digital
        output line (\c true: high, \c false: low).
        \return 0 on success, otherwise a negative number indicating the error
        \sa read() */
  virtual int writeUnlocked( unsigned int line, bool val ) = 0;
    /*! Read from digital I/O line \a line and return value in \a val
        while locking the device.
        \param[in] line the digital line (not its bitmask!)
        \param[out] val the value that was read from to the digital
        input line (\c true: high, \c false: low).
        \return 0 on success, otherwise a negative number indicating
	the error
        \sa readUnlocked(), readLines(), write(), lock() */
  int read( unsigned int line, bool &val );
    /*! Read from digital I/O line \a line and return value in \a val
        without locking the device.
        \param[in] line the digital line (not its bitmask!)
        \param[out] val the value that was read from to the digital
        input line (\c true: high, \c false: low).
        \return 0 on success, otherwise a negative number indicating
	the error
        \sa write() */
  virtual int readUnlocked( unsigned int line, bool &val ) = 0;

    /*! Write \a val to the digital I/O lines defined in \a lines.
        \param[in] lines a bit mask selecting the digital lines to be
        written.
        \param[in] val a bit mask indicating what should be written to
        the digital output lines (\c 1: high, \c 0: low).
        \return 0 on success, otherwise a negative number indicating
        the error
        \sa read() */
  virtual int writeLines( unsigned int lines, unsigned int val ) = 0;
    /*! Read digital I/O lines and return them in \a val.
        \param[in] lines a bit mask selecting the digital lines from
        which to read.
        \param[out] val a bit field returning the values read from to
        the digital input lines (\c 1: high, \c 0: low).
        \return 0 on success, otherwise a negative number indicating
        the error
        \sa write() */
  virtual int readLines( unsigned int lines, unsigned int &val ) = 0;

    /*! Enables generation of TTL Pulses on DIO line \a line
        and the scaling of the current for analog output
        according to the measured period divided by the curent
        injection time of \a duration microseconds. This is used for
        synchronizing a discontinous current-clamp amplifier with an
        dynamic-clamp loop. 
	\param[in] modemask the dio lines that switch the mode of the amplifier
	\param[in] modebits the dio lines to be set high to switch the mode of the amplifier
	into synchronized current clamp mode
	\param[in] line the dio line where to put out the synchronizing pulses
	\param[in] duration the duration of the current injection of 
	the amplifier in seconds.
	\param[in] mode how the measured period is determined:
          0: the fixed requested sampling interval,
	  1: the current period,
          > 1: the average over the \a mode last periods.
	\return 0 on success, a Device error code on failure.
	\sa clearSyncPulse() */
  virtual int setSyncPulse( int modemask, int modebits, unsigned int line, 
			    double duration, int mode=0 );
    /*! Disable TTL Pulse generation and current scaling.
        Switch amplifier into a different mode.
	\param[in] modemask the dio lines that switch the mode of the amplifier
	\param[in] modebits the dio lines to be set high to switch into the desired non syncrhonizing mode
	into synchronized current clamp mode
	\return 0 on success, a Device error code on failure.
	\sa setSyncPulse() */
  virtual int clearSyncPulse( int modemask, int modebits );

  using Device::lock;
  using Device::unlock;


protected:

    /*! Free all allocated digital I/O lines.
        \sa freeLines() */
  void freeLines( void );

    /*! Set the device info().
        Call this function from open().
        \sa info() */
  void setInfo( void );

    /*! The maximum number of digital I/O lines. */
  static const unsigned int MaxDIOLines = 32;


private:

    /*! The ids of allocated digitial I/O lines. */
  int DIOLineIDs[MaxDIOLines];
    /*! The configuration of all digitial I/O lines. */
  bool DIOLineWriteable[MaxDIOLines];


};


}; /* namespace relacs */

#endif /* ! _RELACS_DIGITALIO_H_ */
