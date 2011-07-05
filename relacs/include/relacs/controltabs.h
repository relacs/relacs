/*
  controltabs.h
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

#ifndef _RELACS_CONTROLTABS_H_
#define _RELACS_CONTROLTABS_H_ 1

#include <relacs/plugintabs.h>

namespace relacs {


/*! 
\class ControlTabs
\author Jan Benda
\brief Container organizing Control plugins.
*/


class ControlTabs : public PluginTabs
{
  Q_OBJECT

public:

  ControlTabs( QWidget *parent=0 );
  ~ControlTabs( void );

};


}; /* namespace relacs */

#endif /* ! _RELACS_CONTROLTABS_H_ */

