/*
  rtaicomedi/dynclampdigitalio.h
  Interface for accessing digital I/O lines of a daq-board via comedi and the dynclamp kernel module.

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

#ifndef _RTAICOMEDI_DYNCLAMPDIGITALIO_H_
#define _RTAICOMEDI_DYNCLAMPDIGITALIO_H_

#include <comedilib.h>
#include <relacs/digitalio.h>
#include <relacs/comedi/comedidigitalio.h>
#include <relacs/rtaicomedi/moduledef.h>
using namespace std;
using namespace relacs;

namespace rtaicomedi {


/*! 
\class DynClampDigitalIO
\author Jan Benda
\brief [DigitalIO] Interface for accessing digital I/O lines of a daq-board via comedi and the dynclamp kernel module.

\par Options
- \c subdevice: the subdevice id of the digital I/O (check with comedi_test -t info).
- \c startsubdevice: start searching for a digital I/O subdevice at the subdevice with the specified id.
- \c ttlpulseNline: digital I/O line (channel) on which ttl pulse \a N should be generated.
- \c ttlpulseNhigh: condition on which ttl pulse \a N is set high (startwrite, endwrite, startread, endread, startao, or endao).
- \c ttlpulseNlow: condition on which ttl pulse \a N is set low (startwrite, endwrite, startread, endread, startao, or endao).
*/


class DynClampDigitalIO : public DigitalIO
{

public:

    /*! Create a new %DynClampDigitalIO without opening a device. */
  DynClampDigitalIO( void );
    /*! Open the digital I/O driver specified by its device file \a device. */
  DynClampDigitalIO( const string &device, const Options &opts );
    /*! Close the daq driver. */
  virtual ~DynClampDigitalIO( void );

    /*! Open a digital I/O device on the device \a device.
        The digital I/O subdevice can be specified by the "subdevice" option in \a opts.
        Alternatively, the first digital I/O subdevice is taken.
	The search for the digital I/O subdevice starts at the subdevice specified
	by the "startsubdevice" option in \a opts (defaults to "0").
 	Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close() */
  virtual int open( const string &device ) override;
    /*! Returns true if the device is open.
        \sa open(), close() */
  virtual bool isOpen( void ) const;
    /*! Close the device.
        \sa open(), isOpen() */
  virtual void close( void );

    /*! \return the number of digital I/O lines the device supports */
  virtual int lines( void ) const;

    /*! Returns the ids and further info for each digital I/O line. */
  virtual const Options &settings( void ) const;

    /*! Configure digital I/O line \a line for input (\a output = \c false) or output 
        (\a output = \c true).
        \return 0 on success, otherwise a negative number indicating the error */
  virtual int configureLineUnlocked( unsigned int line, bool output );
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
  virtual int writeUnlocked( unsigned int line, bool val );
    /*! Write \a val to the digital I/O lines defined in \a lines.
        \param[in] lines a bit mask selecting the digital lines to be written.
        \param[in] val a bit mask indicating what should be written to the digital output lines
	(\c 1: high, \c 0: low).
        \return 0 on success, otherwise a negative number indicating the error
        \sa read() */
  virtual int writeLines( unsigned int lines, unsigned int val );

    /*! Read from digital I/O line \a line and return value in \a val.
        \param[in] line the digital line (not its bitmask!)
        \param[out] val the value that was read from to the digital input line
	(\c true: high, \c false: low).
        \return 0 on success, otherwise a negative number indicating the error
        \sa write() */
  virtual int readUnlocked( unsigned int line, bool &val );
    /*! Read digital I/O lines and return them in \a val.
        \param[in] lines a bit mask selecting the digital lines from which to read.
        \param[out] val a bit field returning the values read from to the digital input lines
	(\c 1: high, \c 0: low).
        \return 0 on success, otherwise a negative number indicating the error
        \sa write() */
  virtual int readLines( unsigned int lines, unsigned int &val );

    /*! Make an digital I/O line an TTL pulse that is automatically generated
        by the dynamic clamp real time loop.
	The digital I/O line has to be allocated and configured for output
	before by the allocateLine() and configureLine() functions.
	\param[in] line the digital I/O line (channel) on which the pulse is generated.
	\param[in] high event on which the digital I/O line is set high.
	Can be set to TTL_START_WRITE, TTL_END_WRITE, TTL_START_READ,
	TTL_END_READ, TTL_START_AO, or TTL_END_AO for setting the line
	high right before analog data is written, right after analog
	data is written, righ before analog data is read, right
	after analog data is read, when an analog output signal is started,
	or when an analog output signal is stopped, respectively.
	\param[in] low event on which the digital I/O line is set high.
	Can be set to TTL_START_WRITE, TTL_END_WRITE, TTL_START_READ,
	TTL_END_READ, TTL_START_AO, or TTL_END_AO for setting the line
	high right before analog data is written, right after analog
	data is written, righ before analog data is read, right
	after analog data is read, when an analog output signal is started,
	or when an analog output signal is stopped, respectively.
	\param[in] inithigh set the digital I/O line initially high if \a high is \c true
        \return 0 on success, otherwise a negative number indicating the error. */
  int addTTLPulse( unsigned int line, enum ttlPulses high, enum ttlPulses low, bool inithigh=false );
    /*! Clear any automatically generated pulses that are generated for an
        digital I/O line.
	The digital I/O line has to be allocated and configured for output
	before by the allocateLine() and configureLine() functions.
	\param[in] line the digital I/O line (channel) for which no more TTL pulses
	should be generated.
	\param[in] high set the digital I/O line high if \a high is \c true
	\return 0 on success, otherwise a negative number indicating the error. */
  int clearTTLPulse( unsigned int line, bool high=false );

    /*! Enables generation of TTL Pulses on DIO line \a line that is
        high during reading analog input.  In addition the current for
        analog output is rescaled according to the measured period
        divided by the curent injection time of \a duration
        microseconds. The measured period is eather the 
	requested sampling interval (\a mode=0), the duration of the 
	previous loop (\a mode=1), or averaged over the last \a mode periods.
	\param[in] modemask the dio lines that switch the mode of the amplifier
	\param[in] modebits the dio lines to be set high to switch the mode of the amplifier
	into synchronized current clamp mode
	\param[in] line the dio line where to put out the synchronizing pulses
	\param[in] duration the duration of the current injection 
	of the amplifier in seconds.
	\param[in] mode determines how the injected current is scaled:
	  -1: no scaling,
	  0: use fixed period of dynamic clamp loop (the requested sampling rate),
          1: use the actual duration of the previous cycle,
          n: use the period averaged over the last `n` cycles.
	\return 0 on success, a Device error code on failure.
	\sa clearSyncPulse() */
  virtual int setSyncPulse( int modemask, int modebits, unsigned int line,
			    double duration, int mode=0 );
    /*! Disable TTL Pulse generation and current scaling. \sa setSyncPulse() */
  virtual int clearSyncPulse( int modemask, int modebits );


protected:

  void initOptions() override;

  static const string TTLCommands[7];


private:

    /*! Pointer to the user space comedi interface. */
  comedi::ComediDigitalIO *CDIO;

    /*! The comedi subdevice number. */
  unsigned int SubDevice;
    /*! The number of supported digital I/O lines */
  unsigned int MaxLines;

    /*! Name of the kernel module device file. */
  string ModuleDevice;
    /*! File descriptor for the kernel module. */
  int ModuleFd;

    /*! Memorizes activated TTL pulses. */
  enum ttlPulses TTLPulseHigh[MaxDIOLines];
    /*! Memorizes activated TTL pulses. */
  enum ttlPulses TTLPulseLow[MaxDIOLines];

};


}; /* namespace rtaicomedi */

#endif /* ! _RTAICOMEDI_DYNCLAMPDIGITALIO_H_ */
