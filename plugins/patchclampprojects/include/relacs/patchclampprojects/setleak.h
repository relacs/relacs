/*
  patchclampprojects/setleak.cc
  Set leak resistance and reversal potential of a dynamic clamp model.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_PATCHCLAMPPROJECTS_SETLEAK_H_
#define _RELACS_PATCHCLAMPPROJECTS_SETLEAK_H_ 1

#include <QPushButton>
#include <relacs/optwidget.h>
#include <relacs/repro.h>
using namespace relacs;

namespace patchclampprojects {


/*!
\class SetLeak
\brief [RePro] Set leak resistance and reversal potential of a dynamic clamp model.
\author Jan Benda
\version 1.0 (Mar 21, 2009)
\par Options
- \c interactive=false: Set values interactively (\c boolean)
- \c preset=zero: Set g and E to (\c string)
- \c g=0nS: New value of leak conductance (\c number)
- \c E=0mV: New value of leak reversal-potential (\c number)
- \c reversaltorest=false: Set leak reversal-potential to resting potential (\c boolean)
- \c involtage=V-1: Input voltage trace for measuring resting potential (\c string)
- \c duration=100ms: Duration of resting potential measurement (\c number)
- Passive membrane properties of the cell:
- \c Rm=0MOhm: Resistance R_m (\c number)
- \c Taum=0ms: Time constant tau_m (\c number)
- Injected current I=g(E-V):
- \c gdc=0nS: Additional leak conductance g (\c number)
- \c Edc=0mV: Reversal potential E (\c number)
- Resulting membrane properties:
- \c Rdc=0MOhm: New membrane resistance 1/R=1/R_m+g (\c number)
- \c taudc=0ms: New membrane time constant (\c number)
*/


class SetLeak : public RePro
{
  Q_OBJECT

public:

  SetLeak( void );
  virtual int main( void );
  virtual void config( void );
  virtual void notify( void );


public slots:

  void setValues( void );
  void keepValues( void );
  void resetValues( void );
  void measureVRest( void );


protected:

  virtual void keyPressEvent( QKeyEvent *e );
  virtual void customEvent( QEvent *qce );

  OptWidget STW;
  bool Change;
  bool Reset;

  QPushButton *OKButton;
  QPushButton *CancelButton;
  QPushButton *ResetButton;
  QPushButton *VRestButton;

};


}; /* namespace patchclampprojects */

#endif /* ! _RELACS_PATCHCLAMPPROJECTS_SETLEAK_H_ */

