/*
  misc/amplmode.h
  Control the mode of an amplifier via NIDIO

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

#ifndef _RELACS_MISC_AMPLMODE_H_
#define _RELACS_MISC_AMPLMODE_H_ 1

#include <relacs/nieseries/nidio.h>
#include <relacs/device.h>
using namespace relacs;
using namespace nieseries;

namespace misc {


/*!
\class AmplMode
\author Jan Benda
\version 1.0
\brief [Device] Control the mode of an amplifier via NIDIO

\par Options
- \c firstpin: the first pin of the dio lines used for controlling the amplifier.
 */


class AmplMode : public Device
{

public:

  AmplMode( const string &device, const Options &opts );
  AmplMode( NIDIO *nidio, const Options &opts );
  AmplMode( void );
  virtual ~AmplMode( void );

  virtual int open( const string &device, const Options &opts );
  virtual int open( NIDIO &nidio, const Options &opts );
  virtual int open( Device &device, const Options &opts );
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
  void open( const Options &opts );

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


}; /* namespace misc */

#endif /* ! _RELACS_MISC_AMPLMODE_H_ */
