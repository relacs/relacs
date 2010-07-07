/*
  base/wait.h
  Wait until a specified (date and) time.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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
\brief [RePro] Wait until a specified (date and) time.
\author Jan Benda
\version 1.0 (Jul 07, 2010)

The Pause-RePro does not write any stimulus
and terminates after \c repeats times \c duration ms.
With the \c savedata option you can switch off saving of the raw data
recorded during Pause.

\par Options
- \c date
- \c time
- \c savedata=false: Save Data (\c boolean)

\par Files
No output files.

\par Plots
No plot.

\par Requirements
No requirements.
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
