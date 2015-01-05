/*
  plugintabs.h
  Container organizing relacs plugins.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

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

#ifndef _RELACS_PLUGINTABS_H_
#define _RELACS_PLUGINTABS_H_ 1

#include <QTabWidget>

namespace relacs {


class RELACSWidget;


/*! 
\class PluginTabs
\author Jan Benda
\brief Container organizing relacs plugins.
*/


class PluginTabs : public QTabWidget
{
  Q_OBJECT

public:

    /*! The keyboard focus is activated by pressing \a key. */
  PluginTabs( int key, RELACSWidget *rw, QWidget *parent=0 );
  ~PluginTabs( void );

    /*! Grabs ALT-KeyLeft/Right for paging through the tabs. */
  bool eventFilter( QObject *obj, QEvent *event );


protected:

  void keyPressEvent( QKeyEvent *event );
  static QWidget *firstEnabledChildWidget( QLayout *l );

  RELACSWidget *RW;


private:

  int HotKey;

};


}; /* namespace relacs */

#endif /* ! _RELACS_PLUGINTABS_H_ */

