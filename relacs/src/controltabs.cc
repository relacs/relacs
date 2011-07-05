/*
  controltabs.cc
  Container organizing Control plugins.

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

#include <relacs/controltabs.h>

namespace relacs {


ControlTabs::ControlTabs( QWidget *parent )
  : PluginTabs( Qt::Key_C, parent )
{
}


ControlTabs::~ControlTabs( void )
{
}


}; /* namespace relacs */

#include "moc_controltabs.cc"

