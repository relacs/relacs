/*
  patchclamp/session.h
  Session for current-clamp recordings

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

#ifndef _RELACS_PATCHCLAMP_SESSION_H_
#define _RELACS_PATCHCLAMP_SESSION_H_ 1

#include <relacs/ephys/traces.h>
#include <relacs/control.h>
using namespace relacs;

namespace patchclamp {


/*! 
\class Session
\brief [Control] %Session for current-clamp recordings
\author Jan Benda
\version 1.0 (Oct 13, 2008)
*/


class Session : public Control, public ephys::Traces
{
  Q_OBJECT

public:

  Session( void );
  ~Session( void );

  virtual void initialize( void );

};


}; /* namespace patchclamp */

#endif /* ! _RELACS_PATCHCLAMP_SESSION_H_ */
