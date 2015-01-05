/*
  calibration/attenuatorcheck.h
  Checks performance of attenuator device

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

#ifndef _RELACS_CALIBRATION_ATTENUATORCHECK_H_
#define _RELACS_CALIBRATION_ATTENUATORCHECK_H_ 1

#include <relacs/repro.h>
#include <relacs/plot.h>
using namespace relacs;

namespace calibration {


/*!
\author Jan Benda
\version 1.2 (Feb 15, 2014)
\par Options
- \c outtrace=V-1: Output trace (\c string)
- \c intrace=V-1: Input trace (\c string)
- \c duration=1s: Stimulus duration (\c number)
- \c type=attenuation: Measurement type (\c string)
- \c frequency=50Hz: Frequency of stimulus (\c number)
- \c amplitude=1V: Amplitude of stimulus (\c number)
- \c minlevel=0dB: Minimum attenuation level (\c number)
- \c maxlevel=100dB: Maximum attenuation level (\c number)
- \c dlevel=1dB: Increment of attenuation level (\c number)
*/


class AttenuatorCheck : public RePro
{
  Q_OBJECT

public:

  AttenuatorCheck( void );
  virtual void preConfig( void );
  virtual void notify( void );
  virtual int main( void );


protected:
 
  Plot P;

  string InName;
  string InUnit;
  double InFac;
  string OutName;

};


}; /* namespace calibration */

#endif /* ! _RELACS_CALIBRATION_ATTENUATORCHECK_H_ */
