/*
  patchclamp/session.cc
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

#include <QWidget>
#include <QLabel>
#include <relacs/patchclamp/session.h>
using namespace relacs;

namespace patchclamp {


Session::Session( void )
  : Control( "Session", "Patch-clamp", "Jan Benda", "1.0", "Oct 13, 2008" )

{
}


Session::~Session( void )
{
}


void Session::initialize( void )
{
  // get trace indices:
  ephys::Traces::initialize( traces(), events() );
}


addControl( Session );

}; /* namespace patchclamp */

#include "moc_session.cc"
