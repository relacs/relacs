/*
  base/pause.h
  Does nothing

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_BASE_PAUSE_H_
#define _RELACS_BASE_PAUSE_H_ 1

#include <relacs/repro.h>
using namespace relacs;

namespace base {


/*!
\class Pause
\brief [RePro] Does nothing
\author Jan Benda
\version 1.4 (Dec 9, 2014)

The Pause-RePro does not write any stimulus and terminates after 
\c duration seconds.  If \c duration is set to zero, the
%RePro keeps running infinitely. The %RePro can be interrupted any
time by pressing the space key or starting a different %RePro. With
the \c savedata option you can switch off saving of the raw data
recorded during Pause.

\par Options
- \c duration=0s: Duration (\c number)
- \c savedata=false: Save Data (\c boolean)

\par Files
No output files.

\par Plots
No plot.

\par Requirements
No requirements.
*/


class Pause : public RePro
{
  Q_OBJECT

public:

  Pause( void );
  virtual int main( void );

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_PAUSE_H_ */
