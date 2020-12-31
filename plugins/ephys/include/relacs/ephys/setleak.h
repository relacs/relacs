/*
  ephys/setleak.cc
  Set leak resistance and reversal potential of a dynamic clamp model.

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

#ifndef _RELACS_EPHYS_SETLEAK_H_
#define _RELACS_EPHYS_SETLEAK_H_ 1

#include <QPushButton>
#include <relacs/optwidget.h>
#include <relacs/repro.h>
using namespace relacs;

namespace ephys {


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
*/


class SetLeak : public RePro
{
  Q_OBJECT

public:

  SetLeak( void );
  virtual int main( void );
  virtual void preConfig( void );
  virtual void notify( void );


public slots:

  void setValues( void );
  void keepValues( void );
  void zeroValues( void );
  void measureVRest( void );


protected:

  virtual void keyPressEvent( QKeyEvent *e );
  virtual void customEvent( QEvent *qce );

  OptWidget STW;
  bool Change;
  bool Zero;

  QPushButton *OKButton;
  QPushButton *CancelButton;
  QPushButton *ZeroButton;
  QPushButton *VRestButton;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_SETLEAK_H_ */

