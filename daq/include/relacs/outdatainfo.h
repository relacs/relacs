/*
  outdatainfo.h
  Stores some properties of an OutData.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_OUTDATAINFO_H_
#define _RELACS_OUTDATAINFO_H_ 1
 
#include <deque>
#include <relacs/options.h>
#include <relacs/outdatainfo.h>

using namespace std;

namespace relacs {


  class OutData;


  /*! 
    \class OutDataInfo
    \brief Stores some properties of an OutData.
    \author Jan Benda
  */

  class OutDataInfo {

  public:
    
      /*! Constructor. Sets all properties to their default values. */
    OutDataInfo( void );
      /*! Copy constructor. */
    OutDataInfo( const OutDataInfo&signal );
      /*! Constructor. Sets all properties to the values from \a signal. */
    OutDataInfo( const OutData &signal );

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

      /*! Delay in seconds from start trigger to start of aquisition. */
    double delay( void ) const;
      /*! Set delay to \a delay (in seconds). */
    void setDelay( double delay );
      /*! The sampling rate of the signal in Hertz */
    double sampleRate( void ) const;
      /*! Set the sampling rate of the signal to \a rate Hertz */
    void setSampleRate( double rate );
      /*! Length of signal in seconds. */
    double length( void ) const;
      /*! Set length of signal to \a length (in seconds). */
    void setLength( double length );

      /*! Peak intensity of the signal.
          If MuteIntensity is returned, the attenuator is muted.
	  If NoIntensity is returned, no attenuator is connected
	  to the output channel.
	  \sa setIntensity(), setNoIntensity(), noIntensity(), level() */
    double intensity( void ) const;
      /*! Set output peak intensity to \a intensity.
          The intensity is used in conjunction with the carrier frequency
	  to set an attenuator or amplifier.
	  Passing MuteIntensity requests to mute the attenuator
	  connected to the output channel.
	  Setting the intensity overrides a possible set level().
	  If no such hardware is used,
	  then the intensity as well the carrier frequency settings 
	  have no effect.
	  \sa setCarrierFreq(), intensity(), setNoIntensity(), noIntensity(), setLevel() */
    void setIntensity( double intensity );
      /*! Set the intensity to its default value, indicating that
          the intensity is not used for computing the output signal.
	  This will cause an error, if the requested output channel
	  is connected to an attenuator.
	  \sa setIntensity(), setNoLevel() */
    void setNoIntensity( void );
      /*! Returns \c true if no intensity is set. \sa intensity(), noLevel() */
    bool noIntensity( void ) const;
      /*! Request to mute the attenuator connected to the output channel. 
          Same as setIntensity( MuteIntensity ). 
	  \sa intensity(), setIntensity(), setNoIntensity() */
    void mute( void );
      /*! Carrier frequency of the signal in Hertz. */
    double carrierFreq( void ) const;
      /*! Set carrier frequency of the signal to \a carrierfreq Hertz.
          The carrier frequency is used in conjunction with the intensity
	  to set an attenuator or amplifier.
	  If no such hardware is used,
	  then the intensity as well the carrier frequency settings 
	  have no effect.
	  \sa setIntensity() */
    void setCarrierFreq( double carrierfreq );

      /*! The level that was set for the attenuator.
          \sa setLevel(), intensity() */
    double level( void ) const;
      /*! Set attenuator level directly to \a level.
          The specified level is only used if no intensity()
	  was specified.
	  If no atenuator is connected,
	  then setting the level has no effect.
	  \sa level(), setIntensity() */
    void setLevel( double level );
      /*! Set the attenuation level to its default value, indicating that
          the level is not used for computing the output signal.
	  This will cause an error, if the requested output channel
	  is connected to an attenuator and also no intensity() was specified.
	  \sa setLevel(), setNoIntensity() */
    void setNoLevel( void );
      /*! Returns \c true if no level is set. \sa level(), noIntensity() */
    bool noLevel( void ) const;

      /*! The description of the signal. */
    string ident( void ) const;
      /*! Set the description of the ouput signal to \a ident. */
    void setIdent( const string &ident );

      /*! The number of descriptions.
          \sa description(), addDescription(), clearDescriptions() */
    int descriptions( void ) const;
      /*! Returns the description of the \a i-th component of the output signal.
          \sa descriptions(), addDescription(), clearDescriptions() */
    const Options &description( int i ) const;
      /*! Returns the description of the \a i-th component of the output signal.
          \sa descriptions(), addDescription(), clearDescriptions() */
    Options &description( int i );
      /*! Returns the description of the component of the output signal
          that was last added by addDescription().
	  \sa descriptions(), allDescriptions(), addDescription(), clearDescriptions() */
    const Options &description( void ) const;
      /*! Returns the description of the component of the output signal
          that was last added by addDescription().
	  \sa descriptions(), allDescriptions(), addDescription(), clearDescriptions() */
    Options &description( void );
      /*! Returns all descriptions of the output signal.
          \sa descriptions(), description(), addDescription(), clearDescriptions() */
    const deque< Options > &allDescriptions( void ) const;
      /*! Returns all descriptions of the output signal.
          \sa descriptions(), description(), addDescription(), clearDescriptions() */
    deque< Options > &allDescriptions( void );
      /*! Add a description for another component of the output signal.
          \param[in] type the type of description, e.g. "stimulus/squarewave".
	  \sa descriptions(), description(), clearDescriptions() */
    Options &addDescription( const string &type );
      /*! Erase all descriptions.
          \sa descriptions(), description(), addDescription() */
    void clearDescriptions( void );
      /*! Returns the type of the \a i-th component of the output signal.
          \sa description() */
    string type( int i ) const;
      /*! Returns the type with the leading "stimulus/" removed 
	  of the \a i-th component of the output signal.
          \sa type(), description() */
    string typeName( int i ) const;


  protected:
    
      /*! Device identifier. */
    int Device;
      /*! Channel number. */
    int Channel;
      /*! Trace index that later specifies channel and device. */
    int Trace;
      /*! Trace name that later specifies channel and device. */
    string TraceName;
      /*! Delay in seconds from start trigger to start of aquisition. */
    double Delay;
      /*! The sampling rate of the signal. */
    double SampleRate;
      /*! Length of the signal in seconds. */
    double Length;
      /*! Output intensity. */
    double Intensity;
      /*! Attenuation level. */
    double Level;
      /*! Carrier frequency of signal. */
    double CarrierFreq;
      /*! Identifier for the output signal. */
    string Ident;
      /*! Descriptions of the output signal. */
    deque< Options > Descriptions;
      /*! The stimulus types extracted from the Description. */
    deque< string > Types;
      /*! The stimulus types without the leading "stimulus/". */
    deque< string > TypeNames;

      /*! A dummy description. */
    mutable Options Dummy;
    
  };


}; /* namespace relacs */

#endif /* ! _RELACS_OUTDATAINFO_H_ */
