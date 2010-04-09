/*
  attcs3310/cs3310dio.h
  Control the cs3310 attenuator via a DigitalIO device

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

#ifndef _RELACS_ATTCS3310_CS3310DIO_H_
#define _RELACS_ATTCS3310_CS3310DIO_H_ 1

#include <relacs/digitalio.h>
#include <relacs/attenuator.h>
using namespace relacs;

namespace attcs3310 {


/*!
\class CS3310DIO
\author Jan Benda
\version 1.1
\brief [Attenuator] Control the cs3310 attenuator via a DigitalIO device

\par Technicals by Hartmut Schuetze

First, the CS\-signal has to be set low to adress the chip.
Then, the data are presented at the DATAOUT-pin, 
and are strobed via the STROBE-Signal into the 
chip. Repeat this for all the bits we  want to transfer. After this, we wait a 
little before we reset the CS\-signal back to high.
The MUTE\-signal mutes the chip (.. who would have guessed...?), 
but without changing the set attenuation.
Additionally, if you leave MUTE\ low for more than 2 ms, the 
CS 3310 recalibrates itself.
The ZCEN-signal means zero crossing detection enable, and does exactly this. 
If it is set, the chip waits for the next zero crossing of the signal 
before changing to the new attenuation. This reduces clicking (for 
you HiFi-proffesionals out there...), and is a really elegant feature. 
If there are no zero crossing, the setting changes are forced after 18 ms. 
*/


class CS3310DIO : public Attenuator
{

public:

  CS3310DIO( DigitalIO *dio );
  CS3310DIO( void );
  virtual ~CS3310DIO( void );

  virtual int open( DigitalIO &dio, const Options &opts );
  virtual int open( Device &device, const Options &opts );
  virtual bool isOpen( void ) const;
  virtual void close( void );

    /*! Returns the current settings of the attenuator. */
  virtual const Options &settings( void ) const;

    /*! Returns the number of output lines the attenuator device supports. */
  virtual int lines( void ) const;
    /*! Returns the minimum possible attenuation level in decibel.
        This number can be negative, indicating amplification. */
  virtual double minLevel( void ) const;
    /*! Returns the maximum possible attenuation level in decibel. */
  virtual double maxLevel( void ) const;
    /*! Returns in \a l all possible attenuation levels
        sorted by increasing attenuation levels (highest last). */
  virtual void levels( vector<double> &l ) const;

  virtual int attenuate( int di, double &decibel );
  virtual int testAttenuate( int di, double &decibel );

  int setZeroCrossing( bool enable=true );
  int setMute( bool mute=true );
  int calibrate( void );


private:

    /*! Initialize the attenuator. */
  int open( void );
    /*! Write the current levels to the attenuator. */
  int write( void );

    /*! The digital I/O device. */
  DigitalIO *DIO;

    /*! The id under which the digital I/O lines were allocated. */
  int DIOId;

    /*! The attenuation levels for each of the two channels. */
  unsigned char Level[2];

  static const int Tries = 5;
  static const int Delay = 10;

  static const int ZeroGain = 192;
  static const int MaxGain = 255;
  static const int MinGain = 1;
  static const int MuteGain = 0;

  static const int CS = 1;      // DIO1
  static const int MUTE = 2;    // DIO2
  static const int ZCEN = 3;    // DIO3
  static const int DATAOUT = 0; // DIO0
  static const int DATAIN = 4;  // DIO4
  static const int STROBE = 5;  // DIO5

};


}; /* namespace attcs3310 */

#endif /* ! _RELACS_ATTCS3310_CS3310DIO_H_ */
