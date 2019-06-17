/*
  daqerror.h
  Error codes for analog input and output

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

#ifndef _RELACS_DAQERROR_H_
#define _RELACS_DAQERROR_H_ 1
 
#include <cerrno>
#include <iostream>
#include <string>
using namespace std;

namespace relacs {


/*! 
  \class DaqError
  \author Marco Hackenberg, Jan Benda
  \version 1.0
  \brief Error codes for analog input and output
*/

class DaqError
{
  
 public:
  
    /*! Constructor. Initializes the error flag with zero
	and the additional error string as an empty string. */
  DaqError( void );

    /* Error codes related to input/output operations. */
  static const long long NoDevice = 1LL<<0;
  static const long long DeviceNotOpen = 1LL<<1;
  static const long long MultipleDevices = 1LL<<2;

  static const long long InvalidStartSource = 1LL<<3;
  static const long long MultipleStartSources = 1LL<<4;
  static const long long InvalidDelay = 1LL<<5;
  static const long long MultipleDelays = 1LL<<6;
  static const long long MultiplePriorities = 1LL<<7;
  static const long long InvalidSampleRate = 1LL<<8;
  static const long long MultipleSampleRates = 1LL<<9;
  static const long long InvalidContinuous = 1LL<<10;
  static const long long MultipleContinuous = 1LL<<11;
  static const long long MultipleRestart = 1LL<<12;
  static const long long NoData = 1LL<<13;
  static const long long MultipleBuffersizes = 1LL<<14;
  // XXX the buffer and update time errors can go:
  static const long long InvalidBufferTime = 1LL<<15;
  static const long long MultipleBufferTimes = 1LL<<16;
  static const long long InvalidUpdateTime = 1LL<<17;
  static const long long MultipleUpdateTimes = 1LL<<18;

  static const long long InvalidTrace = 1LL<<19;
  static const long long InvalidChannel = 1LL<<20;
  static const long long MultipleChannels = 1LL<<21;
  static const long long InvalidReference = 1LL<<22;
  static const long long MultipleReferences = 1LL<<23;
  static const long long InvalidDither = 1LL<<24;
  static const long long MultipleDither = 1LL<<25;
  static const long long InvalidReglitch = 1LL<<26;
  static const long long MultipleReglitch = 1LL<<27;
  static const long long InvalidGain = 1LL<<28;
  static const long long MultipleGains = 1LL<<29;
  static const long long Underflow = 1LL<<30;
  static const long long Overflow = 1LL<<31;
  static const long long CalibrationFailed = 1LL<<32;
  static const long long InvalidChannelType = 1LL<<33;
  static const long long InvalidChannelSequence = 1LL<<34;

  static const long long Busy = 1LL<<35;
  static const long long DeviceError = 1LL<<36;
  static const long long OverflowUnderrun = 1LL<<37;
  static const long long Unknown = 1LL<<38;

  static const long long NoIntensity = 1LL<<39;
  static const long long AttNotOpen = 1LL<<40;
  static const long long AttInvalidDevice = 1LL<<41;
  static const long long AttFailed = 1LL<<42;
  static const long long AttUnderflow = 1LL<<43;
  static const long long AttOverflow = 1LL<<44;
  static const long long AttIntensityUnderflow = 1LL<<45;
  static const long long AttIntensityOverflow = 1LL<<46;
  static const long long AttIntensityFailed = 1LL<<47;

    /*! \var NoDevice
        %Device for requested operation does not exist. */
    /*! \var DeviceNotOpen
        %Device is not opened. */
    /*! \var MultipleDevices
        Different devices requested. */

    /*! \var InvalidStartSource
        Invalid start source. */
    /*! \var MultipleStartSources
        Multiple start sources. */
    /*! \var InvalidDelay
        Invalid delay. */
    /*! \var MultipleDelays
        Multiple delays. */
    /*! \var MultiplePriorities
        Multiple priority settings. */
    /*! \var InvalidSampleRate
        Invalid sample rate. */
    /*! \var MultipleSampleRates
        Multiple sampling rates. */
    /*! \var InvalidContinuous
        Continuous mode not supported. */
    /*! \var MultipleContinuous
        Multiple continuous modes. */
    /*! \var MultipleRestart
        Multiple restart requests. */
    /*! \var NoData
        No data for analog output. */
    /*! \var MultipleBuffersizes
        Multiple buffer sizes. */
    /*! \var InvalidBufferTime
        Invalid size for the driver's buffer requested. */
    /*! \var MultipleBufferTimes
        Multiple buffer times. */
    /*! \var InvalidUpdateTime
        Invalid size for the update buffer requested. */
    /*! \var MultipleUpdateTimes
        Multiple update times. */

    /*! \var InvalidTrace
        Invalid trace specification. */
    /*! \var InvalidChannel
        Invalid channel number. */
    /*! \var MultipleChannels
        More than one trace for the same channel. */
    /*! \var InvalidReference
        Invalid reference type for channel. */
    /*! \var MultipleReferences
        Multiple reference types for the channels. */
    /*! \var InvalidDither
        Dither not supported. */
    /*! \var MultipleDither
        Multiple dither settings for the channels. */
    /*! \var InvalidReglitch
        Reglitch not supported. */
    /*! \var MultipleReglitch
        Multiple reglitch settings for the channels. */
    /*! \var InvalidGain
        Invalid gain index. */
    /*! \var MultipleGains
        Multiple gain indices for the channels. */
    /*! \var Underflow
        The minimum value of the output signal is larger than the minimum possible output value. */
    /*! \var Overflow
        The maximum value of the output signal is larger than the maximum possible output value. */
    /*! \var InvalidChannelType
        Invalid channel type. */
    /*! \var InvalidChannelSequence
        Invalid sequence of channel numbers. */

    /*! \var Busy
        Device is still busy. */
    /*! \var BufferOverflow
        Overflow of the buffer of the AnalogInput class. */
    /*! \var OverflowUnderrun
        Overflow of FIFO buffer (input) or underrun (output). */
    /*! \var Unknown
        Unknown error of the daq board. */

    /*! \var NoIntensity
        Intensity for output signal on attenuator not set. */
    /*! \var AttNotOpen
        %Attenuator device is not open. */
    /*! \var AttInvalidDevice
        %Attenuator device does not exist. */
    /*! \var AttFailed
        Failure in setting attenuator. */
    /*! \var AttUnderflow
        Requested signal amplitude to small for attenuator. */
    /*! \var AttOverflow
        Requested signal amplitude too large for attenuator. */
    /*! \var AttIntensityUnderflow
        Requested signal amplitude to small for calculating the attenuation level. */
    /*! \var AttIntensityOverflow
        Requested signal amplitude too large for calculating the attenuation level. */
    /*! \var AttIntensityFailed
        Error in calculating the attenuation level. */

    /*! Clear all error flags and the error string. */
  void clearError( void );

    /*! Return error flags of last in/output operation. */
  long long error( void ) const;
    /*! Set error flags to \a flags. */
  void setError( long long flags );
    /*! Add the bits specified by \a flags to the error flags. */
  void addError( long long flags );
    /*! Clear the bits specified by \a flags of the error flags. */
  void delError( long long flags );

    /*! Add error code \a de originating from daq board to the error flags. */
  void addDaqError( int de );
    /*! Add error code \a ae originating from Attenuate to the error flags. */
  void addAttError( int ae );

    /*! Return the additional error string of the last in/output operation.
        \sa setErrorStr(), addErrorStr() */
  string errorStr( void ) const;
    /*! Set additional error string to \a strg. 
        \sa addErrorStr(), errorStr() */
  void setErrorStr( const string &strg );
    /*! Add \a strg to the additional error string. 
        \sa setErrorStr(), errorStr() */
  void addErrorStr( const string &strg );
    /*! Set additional error string to the string describing the 
        standard C error code \a errnum (from \c errno). 
        \sa addErrorStr(), errorStr() */
  void setErrorStr( int errnum );
    /*! Add the string describing the standard C error code \a errnum 
        (from \c errno) to the additional error string. 
        \sa setErrorStr(), errorStr() */
  void addErrorStr( int errnum );

    /*! Return string describing errors indicated by \a flags. */
  static string errorText( long long flags );
    /*! Return string describing errors from error() and errorStr()
        of last in/output operation. */
  string errorText( void ) const;

    /*! Return true if last input/output operation was successfull,
        i.e. error() == 0 and errorStr().empty(). */
  bool success( void ) const;
    /*! Input/output operation was not possible,
        i.e. error() != 0 or  not errorStr().empty(). */
  bool failed( void ) const;
    /*! Another signal is still running. \sa Busy */
  bool busy( void ) const;
    /*! Requested signal amplitude or intensity was too high. 
        \sa Overflow, AttOverflow, AttIntensityOverflow */
  bool overflow( void ) const;
    /*! Requested signal amplitude or intensity was too low. 
        \sa Underflow, AttUnderflow, AttIntensityUnderflow */
  bool underflow( void ) const;
    /*! Failure in attenuator. \sa AttFailed, AttIntensityFailed */
  bool attenuatorFailed( void ) const;

    /*! Copy the error flag and string from \a de to this. */
  void update( const DaqError *de );

    /*! Write the internal variables to \a str. */
  friend ostream &operator<<( ostream &str, const DaqError &de );


private:

    /*! State of last output operation. */
  long long State;
    /*! Number of last implemented error-code. */
  static const int LastState = 48;
    /*! Plain text error codes. */
  static string ErrorText[LastState];
    /*! Additional error string for messages 
        that are not handled by the error flags. */
  string ErrorString;

};


}; /* namespace relacs */

#endif /* ! _RELACS_DAQERROR_H_ */
