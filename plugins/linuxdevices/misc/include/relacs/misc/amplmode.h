/*
  misc/amplmode.h
  Control the mode of an amplifier via DigitalIO

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

#ifndef _RELACS_MISC_AMPLMODE_H_
#define _RELACS_MISC_AMPLMODE_H_ 1

#include <relacs/digitalio.h>
#include <relacs/device.h>
using namespace relacs;

namespace misc {


/*!
\class AmplMode
\author Jan Benda
\version 4.0 (Oct 5, 2015)
\brief [Device] Control the mode of an amplifier via DigitalIO

\par Options
- \c bridgepin: the dio line that switches the amplifier into bridge mode.
- \c cclamppin: the dio line that switches the amplifier into current clamp mode.
- \c vclamppin: the dio line that switches the amplifier into voltage clamp mode.
- \c dclamppin: the dio line that switches the amplifier into synchronized dynamic clamp mode.
- \c syncpin: the dio line on which synchronizing pulses for the amplifier are generated.
- \c resistancepin: the dio line that activates resistance measurement of the amplifier.
- \c buzzerpin: the dio line that activates the buzzer.
 */


class AmplMode : public Device
{

public:

  AmplMode( DigitalIO &dio, const Options &opts );
  AmplMode( void );
  virtual ~AmplMode( void );

  virtual int open( DigitalIO &dio );
  virtual int open( Device &device ) override;
  virtual bool isOpen( void ) const;
  virtual void close( void );

    /*! In case of errors returns error string of last operation. */
  string errorStr( void ) const;

    /*! \return \c true if the amplifier supports a bridge mode. 
        \sa setBridgeMode() */
  bool supportsBridgeMode( void ) const;
    /*! \return \c true if the amplifier supports a current-clamp mode.
        \sa setCurrentClampMode() */
  bool supportsCurrentClampMode( void ) const;
    /*! \return \c true if the amplifier supports a voltage-clamp mode.
        \sa setVoltageClampMode() */
  bool supportsVoltageClampMode( void ) const;
    /*! \return \c true if the amplifier supports a synchronized dynamic-clamp mode.
        \sa setDynamicClampMode() */
  bool supportsDynamicClampMode( void ) const;

    /*! Activate the bridge mode of the amplifier.
        \return the return value of DigitalIO::writeLines()
	\sa supportsBridgeMode(), setCurrentClampMode(), setVoltageClampMode(),
	setCurrentClampSyncMode(), setManualSelection(), startResistance(), startBuzz() */
  int setBridgeMode( void );
    /*! Activate the current-clamp mode of the amplifier.
        \return the return value of DigitalIO::writeLines()
	\sa supportsCurrentClampMode(), setBridgeMode(), setVoltageClampMode(),
	setCurrentClampSyncMode(), setManualSelection(), startResistance(), startBuzz() */
  int setCurrentClampMode( void );
    /*! Activate the current-clamp mode and external synchronization of the amplifier.
        \return the return value of DigitalIO::writeLines() or -1000
	if synchronizing mode is not supported by the dynamic clamp kernel module.
	\sa supportsDynamicClampMode(), setBridgeMode(), setCurrentClampMode(),
	setVoltageClampMode(), setManualSelection(), startResistance(), startBuzz() */
  int setDynamicClampMode( double duration, double mode );
    /*! Activate the voltage-clamp mode of the amplifier.
        \return the return value of DigitalIO::writeLines()
	\sa supportsVoltageClampMode(), setBridgeMode(), setCurrentClampMode(),
	setCurrentClampSyncMode(), setManualSelection(), startResistance(), startBuzz() */
  int setVoltageClampMode( void );
    /*! Activate the manual mode of the amplifier.
        \return the return value of DigitalIO::writeLines()
	\sa setBridgeMode(), setCurrentClampMode(), setVoltageClampMode(),
	setCurrentClampSyncMode(), startResistance(), startBuzz() */
  int setManualSelection( void );

    /*! Switch the amplifier into resitance measurement mode.
        \return the return value of DigitalIO::writeLines()
        \sa stopResistance() */
  int startResistance( void );
    /*! Switch the amplifier back into its previous mode.
        \return the return value of DigitalIO::writeLines()
        \sa startResistance() */
  int stopResistance( void );

    /*! Initiate buzzing by setting the pin for the buzzer high.
        \return the return value of DigitalIO::write()
	\sa stopBuzz(), startResistance(), setBridgeMode() */
  int startBuzz( void );
    /*! Stop buzzing by setting the pin for the buzzer low.
        \return the return value of DigitalIO::write()
	\sa startBuzz() */
  int stopBuzz( void );


protected:

  void initOptions() override;


private:

    /*! Initialize the amplifier. */
  void open();

    /*! The DigitalIO device for controlling the amplifier. */
  DigitalIO *DIO;
  int DIOId;

  /* The DIO lines for controlling the amplifier mode: */
  int BridgePin;
  int CurrentClampPin;
  int VoltageClampPin;
  int DynamicClampPin;
  int SyncPin;
  int ResistancePin;
  int BuzzerPin;

  /* The corresponding bit masks. */
  int BridgeMask;
  int CurrentClampMask;
  int VoltageClampMask;
  int DynamicClampMask;
  int SyncMask;
  int ResistanceMask;
  int BuzzerMask;

  int ModeMask;
  int Mask;

  int CurrentMode;

};


}; /* namespace misc */

#endif /* ! _RELACS_MISC_AMPLMODE_H_ */
