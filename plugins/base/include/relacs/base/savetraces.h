/*
  base/savetraces.h
  Saves selected input traces or events for each run into files

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

#ifndef _RELACS_BASE_SAVETRACES_H_
#define _RELACS_BASE_SAVETRACES_H_ 1

#include <relacs/repro.h>
using namespace relacs;

namespace base {


/*!
\class SaveTraces
\brief [RePro] Saves selected input traces or events for each run into files
\author Jan Benda
\version 1.0 (Jan 17, 2013)
*/


class SaveTraces : public RePro
{
  Q_OBJECT

public:

  SaveTraces( void );
  virtual int main( void );

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_SAVETRACES_H_ */
