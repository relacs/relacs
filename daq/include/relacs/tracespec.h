/*
  tracespec.h
  Specification of an output signal.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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
	     double scale=1.0, double offset=0.0, const string &unit="",
	     bool reglitch=false, double maxrate=-1.0,
	     double signaldelay=0.0 );
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
        \sa setScale(), offset(), setOffset(), unit(), setUnit() */
  double scale( void ) const;
    /*! Set the scale factor to \a scale.
	The scale factor \a scale is used to scale the output signal
	to the voltage that is put out by the analog output device.
        \sa scale(), offset(), setOffset(), unit(), setUnit() */
  void setScale( double scale );
    /*! The offset that is added to the output signal before it is
        scaled by scale() to the voltage that is put out 
	by the analog output device.
        \sa setOffset(), scale(), setScale(), unit(), setUnit() */
  double offset( void ) const;
    /*! Set the offset that is added to the output signal to \a offset.
        \sa offset(), scale(), setScale(), unit(), setUnit() */
  void setOffset( double offset );
    /*! The unit of the signal.
        \sa setUnit(), scale(), setScale(), offset(), setOffset() */
  string unit( void ) const;
    /*! Set the unit of the signal to \a unit.
        \sa unit(), scale(), setScale(), offset(), setOffset() */
  void setUnit( const string &unit );
    /*! Set the specifications for the output signal.
	First, \a offset() is added to the output signal
	that is given in units \a unit.
	Then the signal is scaled by \a scale  to the voltage 
	that is put out by the analog output device.
        \sa unit(), scale(), setScale(), offset(), setOffset() */
  void setUnit( double scale, double offset, const string &unit );

    /*! Returns \c true if reglitch circuit to make glitches more uniform
        is enabled. 
        \sa setReglitch() */
  bool reglitch( void ) const;
    /*! Enable reglitch circuit to make glitches more uniform.
        By default reglitch is disabled. 
        \sa reglitch() */
  void setReglitch( bool reglitch );

    /*! The maximum sampling rate to be used in Hertz.
        \sa setMaxSampleRate() */
  double maxSampleRate( void );
    /*! Set the maximum sampling rate to \a maxrate Hertz.
        \sa maxSampleRate() */
  void setMaxSampleRate( double maxrate );

    /*! The signal delay in seconds,
	i.e. the time the signal needs from its emission from the daq board to
	its destination. 
        \sa setSignalDelay() */
  double signalDelay( void ) const;
    /*! Set the signal delay, i.e. the time the signal needs from
        its emission from the daq board to its destination,
	to \a sigdelay seconds.
        \sa signalDelay() */
  void setSignalDelay( double sigdelay );

    /*! Apply the values of TraceSpec to \a signal
        if signal.traceName() matches traceName()
	or signal.trace() matches trace().
	Otherwise the DaqError::InvalidTrace error flag is set.
        \return
        -  0: success
        - -1: signal.traceName() or signal.trace() do not match the TraceSpec. */
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
  double Offset;
  string Unit;
  bool Reglitch;
  double MaxRate;
  double SignalDelay;
  
};


}; /* namespace relacs */

#endif /* ! _RELACS_TRACESPEC_H_ */
