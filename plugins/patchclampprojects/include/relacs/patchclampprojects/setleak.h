/*
  patchclampprojects/setleak.cc
  Set leak resistance and reversal potential of a dynamic clamp model.

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

#ifndef _RELACS_BASE_SETLEAK_H_
#define _RELACS_BASE_SETLEAK_H_ 1

#include <qpushbutton.h>
#include <relacs/optwidget.h>
#include <relacs/repro.h>
using namespace relacs;

namespace base {


/*!
\class SetLeak
\brief [RePro] Set leak resistance and reversal potential of a dynamic clamp model.
\author Jan Benda
\version 1.0 (Feb 20, 2010)
*/


class SetLeak : public RePro
{
  Q_OBJECT

public:

  SetLeak( void );
  virtual int main( void );
  virtual void notify( void );


public slots:

  void setValues( void );
  void keepValues( void );
  void resetValues( void );


protected:

  virtual void keyPressEvent( QKeyEvent *e );
  virtual void customEvent( QCustomEvent *qce );

  OptWidget STW;
  bool Change;
  bool Reset;

  QPushButton *OKButton;
  QPushButton *CancelButton;
  QPushButton *ResetButton;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_SETLEAK_H_ */

