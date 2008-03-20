/*
  cs3310pp.h
  Control the cs3310 attenuator via the Linux ppdev parallel-port device driver.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _CS3310PP_H_
#define _CS3310PP_H_

#include <relacs/attenuator.h>

/*!
\class CS3310PP
\author Jan Benda
\version 1.1
\brief Control the cs3310 attenuator via the Linux ppdev parallel-port device driver.


\par Installation

Make sure that you have read and write permissions on /dev/parport0!
Either do a "chmod a+rw /dev/parport0" manually, place it in rc.local (bad)
or make yourself member of the group /dev/parport0 belongs to using vigr 
(better) or check /etc/udev/rules.d and /etc/security/console.perms (best).

/etc/security/console.perms transfers the ownership of devices to 
the one that is logged in. You need the following lines in 
/etc/security/console.perms or a file in /etc/security/console.perms.d/ :
\code
<paralellport>=/dev/parport[0-7]
<console>  0600 <paralellport>    0660 root.lp
\endcode

For manipulating udev, 
create a file /etc/udev/rules.d/20-local.rules with the following
content:
\code
KERNEL=="parport*",             GROUP="lp", MODE="0666"
\endcode
However, this is overwritten by console.perms.


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

class CS3310PP : public Attenuator
{

public:

  CS3310PP( const string &device );
  CS3310PP( void );
  virtual ~CS3310PP( void );

  virtual int open( const string &device, long mode=0 );
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

    /*! Write the current levels to the attenuator. */
  int write( void );

    /*! Handle for the device. */
  int Handle;

    /*! The attenuation levels for each of the two channels. */
  unsigned char Level[2];

  const static int ZeroGain = 192;
  static const int MaxGain = 255;
  static const int MinGain = 1;
  static const int MuteGain = 0;
  const static int Tries = 5;

};

#endif
