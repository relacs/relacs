/*
  amplmode.h
  Control the mode of an amplifier via NIDIO

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

#ifndef _RELACS_HARDWARE_AMPLMODE_H_
#define _RELACS_HARDWARE_AMPLMODE_H_ 1

#include "nidio.h"
#include <relacs/device.h>


/*!
\class AmplMode
\author Jan Benda
\version 1.0
\brief Control the mode of an amplifier via NIDIO
 */


class AmplMode : public Device
{

public:

  AmplMode( const string &device, long mode );
  AmplMode( NIDIO *nidio, long mode );
  AmplMode( void );
  virtual ~AmplMode( void );

  virtual int open( const string &device, long mode );
  virtual int open( NIDIO &nidio, long mode );
  virtual int open( Device &device, long mode );
  virtual bool isOpen( void ) const;
  virtual void close( void );

  int bridge( void );
  int resistance( void );
  int voltageClamp( void );
  int currentClamp( void );
  int manual( void );

  int buzzer( void );


private:

    /*! Initialize the amplifier. */
  void open( long mode );

    /*! The NI DIO. */
  NIDIO *DIO;
  bool Own;

  int FirstPin;

  int Buzzer;
  int Resistance;
  int Bridge;
  int CurrentClamp;
  int VoltageClamp;

  int ModeMask;
  int Mask;

  static const int BuzzPulse = 10;  // milliseconds

  int MixerHandle;
  int MixerChannel;
  int Volume;

};

#endif /* ! _RELACS_HARDWARE_AMPLMODE_H_ */
