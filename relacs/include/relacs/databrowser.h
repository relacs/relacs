/*
  databrowser.h
  Interface for browsing recorded data.

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

#ifndef _RELACS_DATABROWSER_H_
#define _RELACS_DATABROWSER_H_ 1

#include <QWidget>
#include <QTreeView>
using namespace std;

namespace relacs {


class DataIndex;
class DataTreeModel;


/*! 
\class DataBrowser
\brief Interface for browsing recorded data.
\author Karin Fisch, Jan Benda
*/


class DataBrowser : public QWidget
{
  Q_OBJECT

public:

  DataBrowser( DataIndex *data, QWidget *parent=0 );
  ~DataBrowser( void );


private:

  QTreeView *TreeWidget;

};


}; /* namespace relacs */

#endif /* ! _RELACS_DATABROWSER_H_ */

