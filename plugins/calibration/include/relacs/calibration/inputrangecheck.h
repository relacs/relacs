/*
  calibration/inputrangecheck.h
  Checks calibration of each range (gain) of an analag input channel.

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

#ifndef _RELACS_CALIBRATION_INPUTRANGECHECK_H_
#define _RELACS_CALIBRATION_INPUTRANGECHECK_H_ 1

#include <relacs/plot.h>
#include <relacs/repro.h>
using namespace relacs;

namespace calibration {


/*!
\class InputRangeCheck
\brief [RePro] Checks calibration of each range (gain) of an analag input channel.
\author Jan Benda
\version 1.0 (Feb 18, 2014)
*/


class InputRangeCheck : public RePro
{
  Q_OBJECT

public:

  InputRangeCheck( void );
  virtual void preConfig( void );
  virtual void notify( void );
  virtual int main( void );


protected:
 
  Plot P;

  string InName;
  string InUnit;
  string OutName;
  string OutUnit;

};


}; /* namespace calibration */

#endif /* ! _RELACS_CALIBRATION_INPUTRANGECHECK_H_ */
