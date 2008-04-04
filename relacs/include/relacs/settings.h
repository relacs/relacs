/*
  ../include/relacs/settings.h
  Includes some general Settings into the configure mechanism.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _SETTINGS_H_
#define _SETTINGS_H_


#include <relacs/configdialog.h>


/*! 
\class Settings
\brief Includes some general Settings into the configure mechanism.
\author Jan Benda
\version 1.0

Settings sets the following environment variable:
- \c RELACSDEFAULTPATH : The default path where RELACS stores data (inbetween sessions).
- \c RELACSREPROPATH : Base directory for RePros to store some general stuff.
*/


class Settings : public ConfigDialog
{
  Q_OBJECT

public:
  
  Settings( class RELACSWidget *RW );
  ~Settings( void );

  virtual void configure( void );
  virtual void notify( void );


private:

  class RELACSWidget *RW;

};


#endif
