/*
  databrowser.h
  Interface for browsing previously recorded data.

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

#ifndef _RELACS_DATABROWSER_H_
#define _RELACS_DATABROWSER_H_ 1

#include <QWidget>
#include <QTreeWidget>
#include <relacs/str.h>
using namespace std;

namespace relacs {


class RePro;


/*! 
\class DataBrowser
\brief Interface for browsing previously recorded data.
\author Jan Benda
*/


class DataBrowser : public QWidget
{
  Q_OBJECT


public:

  DataBrowser( QWidget *parent=0 );
  ~DataBrowser( void );

  string folder;
  bool session;

  void addStimulus(const OutData &Signal);
  void addStimulus(const OutList &Signal);
  void addRepro( const RePro *Repro );
  void addSession( const string &path );
  void endSession( void );
  void load( const string &dir );
  void read(string file, QTreeWidgetItem *parent);

public slots:

  void list(QTreeWidgetItem * item, int col);


private:

  QTreeWidget *TreeWidget;
  struct Stimulus;
  struct Rep;
  struct Cell;
  map<string, map<long,DataBrowser::Stimulus>* > * Cells; 
  map<long, DataBrowser::Stimulus> * NStimuli;
  map< string, map<int,DataBrowser::Rep>* > * Protocol;
  map< string, DataBrowser::Cell* > * Header;

};


}; /* namespace relacs */

#endif /* ! _RELACS_DATABROWSER_H_ */

