/*
  base/wait.h
  Wait until a specified (date and) time.

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

#ifndef _RELACS_BASE_WAIT_H_
#define _RELACS_BASE_WAIT_H_ 1

#include <QLabel>
#include <QProgressBar>
#include <relacs/repro.h>
using namespace relacs;

namespace base {


/*!
\class Wait
\brief [RePro] %Wait until a specified (date and) time.
\author Jan Benda
\version 1.0 (Jul 07, 2010)
\par Screenshot
\image html wait.png

\par Options
- \c absdate=false: Specify absolute date (\c boolean)
- \c date=0000-00-00: Date (\c date)
- \c days=0days: Days from start of RePro on (\c integer)
- \c time=00:00:00: Time (\c time)
*/


class Wait : public RePro
{
  Q_OBJECT

public:

  Wait( void );
  virtual int main( void );


protected:

  virtual void customEvent( QEvent *qce );

  QProgressBar QPB;
  QLabel EndTimeLabel;
  QLabel RemainingTimeLabel;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_WAIT_H_ */
