/*
  misc/amplmode.h
  Control the mode of an amplifier via DigitalIO

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

#ifndef _RELACS_MISC_AMPLMODE_H_
#define _RELACS_MISC_AMPLMODE_H_ 1

#include <relacs/digitalio.h>
#include <relacs/device.h>
using namespace relacs;

namespace misc {


/*!
\class AmplMode
\author Jan Benda
\version 2.0 (Apr 3, 2014)
\brief [Device] Control the mode of an amplifier via DigitalIO

\par Options
- \c vclamppin: the dio line that switches the amplifier into voltage clamp mode.
- \c cclamppin: the dio line that switches the amplifier into current clamp mode.
- \c bridgepin: the dio line that switches the amplifier into bridge mode.
- \c resistancepin: the dio line that activates resistance measurement of the amplifier.
- \c buzzerpin: the dio line that activates the buzzer.

\par Sound
When activating the buzzer the plugin mutes the soundcard via the OSS sound interface.

We stay with the OSS interface since it is so simple to program. The ALSA mixer interface
is too complicated, but ALSA provides compatibility modules for OSS.

Check whether you have a \c /dev/mixer device file. If not you need to load the 
\c snd-mixer-oss kernel module:
\code
sudo modprobe snd-mixer-oss
\endcode
See http://alsa.opensrc.org/OSS_emulation for some information on that.
 */


class AmplMode : public Device
{

public:

  AmplMode( DigitalIO &dio, const Options &opts );
  AmplMode( void );
  virtual ~AmplMode( void );

  virtual int open( DigitalIO &dio, const Options &opts );
  virtual int open( Device &device, const Options &opts );
  virtual bool isOpen( void ) const;
  virtual void close( void );

    /*! Activate the bridge mode of the amplifier.
        \return the return value of DigitalIO::writeLines()
	\sa setCurrentClampMode(), setVoltageClampMode(), setManualSelection(),
	startResistance(), startBuzz() */
  int setBridgeMode( void );
    /*! Activate the current-clamp mode of the amplifier.
        \return the return value of DigitalIO::writeLines()
	\sa setBridgeMode(), setVoltageClampMode(), setManualSelection(),
	startResistance(), startBuzz() */
  int setCurrentClampMode( void );
    /*! Activate the voltage-clamp mode of the amplifier.
        \return the return value of DigitalIO::writeLines()
	\sa setBridgeMode(), setCurrentClampMode(), setManualSelection(),
	startResistance(), startBuzz() */
  int setVoltageClampMode( void );
    /*! Activate the manual mode of the amplifier.
        \return the return value of DigitalIO::writeLines()
	\sa setBridgeMode(), setCurrentClampMode(), setVoltageClampMode(),
	startResistance(), startBuzz() */
  int setManualSelection( void );

    /*! Mute the sound card and switch the amplifier into resitance measurement mode.
        \return the return value of DigitalIO::writeLines()
        \sa stopResistance() */
  int startResistance( void );
    /*! Switch the amplifier back into its previous mode and unmute the sound card.
        \return the return value of DigitalIO::writeLines()
        \sa startResistance() */
  int stopResistance( void );

    /*! Mute the sound card and initiate buzzing by setting the pin for the buzzer high.
        \return the return value of DigitalIO::write()
	\sa stopBuzz(), startResistance(), setBridgeMode() */
  int startBuzz( void );
    /*! Stop buzzing by setting the pin for the buzzer low and unmute the sound card.
        \return the return value of DigitalIO::write()
	\sa startBuzz() */
  int stopBuzz( void );


private:

    /*! Initialize the amplifier. */
  void open( const Options &opts );

    /*! The DigitalIO device for controlling the amplifier. */
  DigitalIO *DIO;
  int DIOId;

  /* The DIO lines for controlling the amplifier mode: */
  int BridgePin;
  int CurrentClampPin;
  int VoltageClampPin;
  int ResistancePin;
  int BuzzerPin;

  /* The corresponding bit masks. */
  int BridgeMask;
  int CurrentClampMask;
  int VoltageClampMask;
  int ResistanceMask;
  int BuzzerMask;

  int ModeMask;
  int Mask;

  int CurrentMode;

  /* The sound mixer device. */
  int MixerHandle;
  int MixerChannel;
  int Volume;

};


}; /* namespace misc */

#endif /* ! _RELACS_MISC_AMPLMODE_H_ */
