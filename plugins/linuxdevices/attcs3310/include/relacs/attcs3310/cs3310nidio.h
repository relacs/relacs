/*
  attcs3310/cs3310nidio.h
  Control the cs3310 attenuator via the nieseries/nidio software timed digital IO  

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

#ifndef _RELACS_ATTCS3310_CS3310NIDIO_H_
#define _RELACS_ATTCS3310_CS3310NIDIO_H_ 1

#include <relacs/nieseries/nidio.h>
#include <relacs/attenuator.h>
using namespace relacs;
using namespace nieseries;

namespace attcs3310 {


/*!
\class CS3310NIDIO
\author Jan Benda
\version 1.1
\brief [Attenuator] Control the cs3310 attenuator via the nieseries/nidio software timed digital IO
\note This plugin is as obsolete as the plugins from the nieseries plugin set.

\par Technicals by Hartmut Schuetze

Module for the programming of the CS 3310 attenuator chip, 
connected to the parallel port. this connection has the great 
advantage that one can use the status bit and several data bits 
to supply the necessary signals to the chip. 
First, the CS\-signal (Pin 14 SPP, Pin 2 chip) has to be set low to adress the chip.
Then, the data are presented at the D0 Pin (dataport 0 / pin 2 of the SPP, 
Pin 3 on the chip), and are strobed via the SCLK-Signal (Pin 1 SPP, Pin 6 chip) into the 
chip. repeat this for all the bits we  want to transfer. After this, we wait a 
little before we reset the CS\-signal back to high. Attention: 
the strobe bit and the status bit 1 are inverted at the SPP!. 
The timing we use here could perhaps be faster, but the 2 meters of 
parallel cable make the signal flanks look really sloppy, then.. 
nevertheless, these signals are recognised by the chip from their 
rising slopes (rising  flank, not level triggering), so this should be no 
problem... anyway, we have time enough.
The MUTE\-signal mutes the chip (.. who would have guessed...?), 
but without changing the set attenuation. it is active when Pin 8 is LOW. 
Additionally, if you leave MUTE\ low for more than 2 ms, the 
CS 3310 recalibrates itself. :-)
The ZCEN-signal means zero crossing detection enable, and does exactly this. 
If it is set, the chip waits for the next zero crossing of the signal 
before changing to the new attenuation. This reduces clicking (for 
you HiFi-proffesionals out there...), and is a really elegant feature. 
If there are no zero crossing, the setting changes are forced after 18 ms. 
*/


class CS3310NIDIO : public Attenuator
{

public:

  CS3310NIDIO( const string &device );
  CS3310NIDIO( NIDIO *nidio );
  CS3310NIDIO( void );
  virtual ~CS3310NIDIO( void );

  virtual int open( const string &device, const Options &opts );
  virtual int open( NIDIO &nidio, const Options &opts );
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

    /*! The NI DIO. */
  NIDIO *DIO;
  bool Own;

    /*! The attenuation levels for each of the two channels. */
  unsigned char Level[2];

  static const int Tries = 5;
  static const int Delay = 10;

  static const int ZeroGain = 192;
  //  static const int MaxGain = 255; amplification saturates the output!
  static const int MaxGain = ZeroGain;
  static const int MinGain = 1;
  static const int MuteGain = 0;

  static const int CS = 0x02;      // DIO1
  static const int MUTE = 0x04;    // DIO2
  static const int ZCEN = 0x08;    // DIO3
  static const int DATAOUT = 0x01; // DIO0
  static const int DATAIN = 0x10;  // DIO4
  static const int DioPins = CS | MUTE | ZCEN | DATAOUT | DATAIN;

};


}; /* namespace attcs3310 */

#endif /* ! _RELACS_ATTCS3310_CS3310NIDIO_H_ */
