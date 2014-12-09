/*
  base/record.h
  Simply records data

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

#ifndef _RELACS_BASE_RECORD_H_
#define _RELACS_BASE_RECORD_H_ 1

#include <relacs/repro.h>
using namespace relacs;

namespace base {


/*!
\class Record
\brief [RePro] Simply records data
\author Jan Benda
\version 1.2 (Dec 9, 2014)

The Record-%RePro simply records data without writing out any stimulus
and terminates after \c duration seconds. If \c duration is
set to zero, the %RePro keeps recording infinitely. Recording can be
interrupted any time by pressing the space key or starting a different
%RePro.

Data are recorded into the raw-traces files of RELACS (trace-?.raw). If
you want to have the data saved into ascii files use the SaveTraces %RePro.

\par Options
- \c duration=0s: Duration (\c number)

\par Files
No output files.

\par Plots
No plot.

\par Requirements
No requirements.
*/


class Record : public RePro
{
  Q_OBJECT

public:

  Record( void );
  virtual int main( void );

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_RECORD_H_ */
