/*
  ephys/setvgate.h
  Set parameter of voltage gated channel for dynamic clamp.

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

#ifndef _RELACS_EPHYS_SETVGATE_H_
#define _RELACS_EPHYS_SETVGATE_H_ 1

#include <QPushButton>
#include <relacs/multiplot.h>
#include <relacs/optwidget.h>
#include <relacs/repro.h>
using namespace relacs;

namespace ephys {


/*!
\class SetVGate
\brief [RePro] Set parameter of voltage gated channel for dynamic clamp.
\author Jan Benda
\version 1.2 (Jan 4, 2021)
\par Options
- \c interactive=true: Set values interactively (\c boolean)
- \c preset=previous: Set g and E to (\c string)
- \c g=0nS: Maximum conductance (\c number)
- \c E=0mV: Reversal-potential (\c number)
- \c vmid=0mV: Position of activation curve (\c number)
- \c width=0mV: Width of activation curve (\c number)
- \c tau=10ms: Activation time constant (\c number)
- \c reversaltorest=false: Set reversal-potential to resting potential (\c boolean)
- \c involtage=V-1: Input voltage trace for measuring resting potential (\c string)
- \c duration=100ms: Duration of resting potential measurement (\c number)
*/


class SetVGate : public RePro
{
  Q_OBJECT

public:

  SetVGate( void );
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

  MultiPlot P;
  OptWidget STW;
  bool Change;
  bool Zero;
  bool HaveMembrane;
  bool HaveVGate;
  bool HaveTauV;

  QPushButton *OKButton;
  QPushButton *CancelButton;
  QPushButton *ZeroButton;
  QPushButton *VRestButton;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_SETVGATE_H_ */

