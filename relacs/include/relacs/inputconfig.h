/*
  inputconfig.h
  Configures analog input traces.

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

#ifndef INPUTCONFIG_H
#define INPUTCONFIG_H
 
#include <QStringList>
#include <QTableWidget>
#include <QPushButton>
#include <relacs/options.h>
using namespace std;

namespace relacs {


/*!
\class InputConfig
\brief Configures analog input traces.
\author Jan Benda
*/

class InputConfig : public QWidget
{
  Q_OBJECT
 
public:

    /*! Construct widget for setting input traces based on the input trace options \a opts. */
  InputConfig( Options &opts, QWidget *parent=0 );

  void fillCells( int, bool );
  

public slots:

  void addRow( void );
  void deleteRow( void );
    /*! Writes new settings to the input trace options and emit newInputSettings(). */
  void dialogClosed( int r );
 

signals:

    /*! This signal is emited whenever new settings are written to the input trace options. */
  void newInputSettings( void );

private:

    /*! Read in table from current input trace options. */
  void fillTable( void );

  QTableWidget *Table;
  QPushButton *AddButton;
  QPushButton *RemoveButton;

  Options &Opts;

  QItemSelectionModel *Selection;
  QModelIndexList Indexes;

};


}; /* namespace relacs */
 
#endif // INPUTCONFIG_H
