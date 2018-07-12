/*
  base/setdigitaloutput.h
  Set the level of digital output lines.

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

#ifndef _RELACS_BASE_SETDIGITALOUTPUT_H_
#define _RELACS_BASE_SETDIGITALOUTPUT_H_ 1

#include <relacs/optwidget.h>
#include <relacs/repro.h>
using namespace relacs;

namespace base {


/*!
\class SetDigitalOutput
\brief [RePro] Set the level of digital output lines.
\author Jan Benda
\version 1.0 (Jul 12, 2018)
\par Options
- \c device=dio-1: Name of the digital I/O device (\c string)
- \c line=0: Digital output line (\c integer)
- \c value=0: Level to be writen to output line (\c integer)
- \c interactive=false: Set values interactively (\c boolean)
*/


class SetDigitalOutput : public RePro
{
  Q_OBJECT

public:

  SetDigitalOutput( void );
  virtual int main( void );


public slots:

  void acceptValues( void );
  void setValues( void );
  void keepValues( void );


protected:

  virtual void keyPressEvent( QKeyEvent *e );
  virtual void customEvent( QEvent *qce );

  OptWidget STW;
  bool Interactive;
  bool Change;
  bool Quit;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_SETDIGITALOUTPUT_H_ */
