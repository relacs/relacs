/*
  tracespec.h
  Specification of an output signal.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_TRACESPEC_H_
#define _RELACS_TRACESPEC_H_ 1
 
#include <string>

using namespace std;

namespace relacs {


class OutData;


/*! 
\class TraceSpec
\author Jan Benda
\version 1.0
\brief Specification of an output signal.
*/

class TraceSpec
{
  
 public:

  TraceSpec( void );
  TraceSpec( int index, const string &name,
	     int device, int channel,
	     double scale=1.0, const string &unit="",
	     double maxrate=-1.0,
	     const string &modality="" );
    /*! Copy constructor. */
  TraceSpec( const TraceSpec &trace );
  

    /*! The index of the output device. 
        \sa setDevice(), channel(), trace(), traceName() */
  int device( void ) const;
    /*! Set the device index to \a device.
        \sa device(), setChannel(), setTrace(), setTraceName() */
  void setDevice( int device );
    /*! The number of the channel on the specified device
        that is used for output. 
        \sa setChannel(), device(), trace(), traceName() */
  int channel( void ) const;
    /*! Set the number of the channel on the specified device
        that should be used for output to \a channel. 
        \sa channel(), setDevice(), setTrace(), setTraceName() */
  void setChannel( int channel );
    /*! Set the number of the channel to \a channel
        and the device to \a device. 
        \sa channel(), device(), setDevice(), setTrace(), setTraceName() */
  void setChannel( int channel, int device );
    /*! The index of the output trace.
        \sa setTrace(), channel(), device(), traceName() */
  int trace( void ) const;
    /*! Set the index of the output trace to \a index.
        \sa trace(), setChannel(), setDevice(), setTraceName() */
  void setTrace( int index );
    /*! The name of the output trace.
        \sa setTraceName(), channel(), device(), trace() */
  string traceName( void ) const;
    /*! Set the name of the output trace to \a name.
        \sa traceName(), setChannel(), setDevice(), setTrace() */
  void setTraceName( const string &name );

    /*! The scale factor used for scaling the output signal to the voltage
        that is put out by the analog output device.
        \sa setScale(), unit(), setUnit() */
  double scale( void ) const;
    /*! Set the scale factor to \a scale.
	The scale factor \a scale is used to scale the output signal
	to the voltage that is put out by the analog output device.
        \sa scale(), unit(), setUnit() */
  void setScale( double scale );
    /*! The unit of the signal.
        \sa setUnit(), scale(), setScale() */
  string unit( void ) const;
    /*! Set the unit of the signal to \a unit.
        \sa unit(), scale(), setScale() */
  void setUnit( const string &unit );
    /*! Set the specifications for the output signal.
	The signal with unit \a unit is scaled by \a scale  to the voltage 
	that is put out by the analog output device.
        \sa unit(), scale(), setScale() */
  void setUnit( double scale, const string &unit );

    /*! The maximum or fixed sampling rate to be used in Hertz.
        \sa setMaxSampleRate(), setFixedSampleRate(), fixedSampleRate() */
  double maxSampleRate( void ) const;
    /*! Set the maximum sampling rate to \a maxrate Hertz.
        \sa maxSampleRate(), setFixedSampleRate() */
  void setMaxSampleRate( double maxrate );
    /*! \c True if the sampling rate is fixed. */
  bool fixedSampleRate( void ) const;
  /*! Set the fixed sampling rate to \a rate Hertz.
      \sa maxSampleRate(), setMaxSampleRate(), fixedSampleRate() */
  void setFixedSampleRate( double rate );

    /*! The modality of the signal, i.e. electric, visual, acoustic, etc.
        \sa setModality() */
  string modality( void ) const;
    /*! Set the modality of the signal to \a modality.
        \sa modality() */
  void setModality( const string &modality );

    /*! If \a signal has an output trace specified (via
        OutData::setTrace() or OutData::setTraceName()) then the
        corresponding settings (channel, device, ...)  are applied to
        \a signal.
	If either OutData::traceName() or OutData::trace() do not
	match this TraceSpec, the DaqError::InvalidTrace error flag of
	\a signal is set.
	\return 
	-  0: success
	- -1: invalid match
    */
  int apply( OutData &signal ) const;

    /*! Returns true if \a trace and \a signal use
        the same device() and channel(). */
  friend bool operator==( const TraceSpec &trace, const OutData &signal );
    /*! Returns true if \a trace and \a signal use
        the same device() and channel(). */
  friend bool operator==( const OutData &signal, const TraceSpec &trace );
  
  
 private:
  
  int Trace;
  string TraceName;
  int Device;
  int Channel;
  double Scale;
  string Unit;
  double MaxRate;
  bool FixedRate;
  string Modality;
  
};


}; /* namespace relacs */

#endif /* ! _RELACS_TRACESPEC_H_ */
