/*
  cs3310serialnidio.h
  

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

#ifndef _RELACS_HARDWARE_CS3310SERIALNIDIO_H_
#define _RELACS_HARDWARE_CS3310SERIALNIDIO_H_ 1

#include <relacs/hardware/nidio.h>
#include <relacs/attenuator.h>

/* 
Module for the programming of the CS 3310 attenuator chip, 
connected to DIO lines of an NI E-Series board
using hardware controlled serial output.
First, the CS\-signal (DIO1, Pin 2 chip) has to be set low to adress the chip.
Then, the data are presented at the D0 Pin (dataport 0 / DIO0, 
Pin 3 on the chip), and are strobed via the SCLK-Signal (EXTSTRB, Pin 6 chip) into the 
chip. After this, reset the CS\-signal back to high to activate the new settings.

input     input  gain/attenuation
11111111  255    +31.5
11111110  254    +31.0
...
11000000  192    0
...
00000010    2    -95.0
00000001    1    -95.5
00000000    0    software mute

The MUTE\-signal mutes the chip (.. who would have guessed...?), 
but without changing the set attenuation. It is active when Pin 8 (DIO2) is LOW. 
Additionally, if you leave MUTE\ low for more than 2 ms, the 
CS 3310 recalibrates itself. :-)

The ZCEN-signal (DIO3) means zero crossing detection enable, and does exactly this. 
If it is set, the chip waits for the next zero crossing of the signal 
before changing to the new attenuation. This reduces clicking (for 
you HiFi-proffesionals out there...), and is a really elegant feature. 
If there is no zero crossing, the setting changes are forced after 18 ms. 
*/


class CS3310SerialNIDIO : public Attenuator
{

public:

  CS3310SerialNIDIO( const string &device );
  CS3310SerialNIDIO( NIDIO *nidio );
  CS3310SerialNIDIO( void );
  virtual ~CS3310SerialNIDIO( void );

  virtual int open( const string &device, long mode=0 );
  virtual int open( NIDIO &nidio, long mode=0 );
  virtual int open( Device &device, long mode=0 );
  virtual bool isOpen( void ) const;
  virtual void close( void );

    /*! Returns a string with the current settings of the attenuator. */
  virtual string settings( void ) const;

    /*! Returns the number of output lines the attenuator device supports. */
  virtual int lines( void ) const;

  virtual int attenuate( int di, double &decibel );
  virtual int testAttenuate( int di, double &decibel );

  int setZeroCrossing( bool enable=true );
  int setMute( bool mute=true );
  int calibrate( void );


private:

    /*! Initialize the attenuator. */
  int open( void );

    /*! The NI DIO. */
  NIDIO *DIO;
  bool Own;

    /*! The attenuation levels for each of the two channels. */
  unsigned char Level[2];

  static const int Tries = 5;
  static const int Delay = 10;

  static const int ZeroGain = 192;
  static const int MaxGain = 255;
  static const int MinGain = 1;
  static const int MuteGain = 0;

  static const int CS = 0x02;     // DIO1
  static const int MUTE = 0x04;   // DIO2
  static const int ZCEN = 0x08;   // DIO3
  static const int Mask = CS | MUTE | ZCEN; 
  static const int DATAOUT = 0x01;    // DIO0
  static const int DATAIN = 0x10;     // DIO4
  static const int DioPins = Mask | DATAOUT | DATAIN;

};

#endif /* ! _RELACS_HARDWARE_CS3310SERIALNIDIO_H_ */
