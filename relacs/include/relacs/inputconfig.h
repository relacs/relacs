/*
  inputconfig.h
  Configures analog input traces.

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

#ifndef INPUTCONFIG_H
#define INPUTCONFIG_H
 
#include <QTableWidget>
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


public slots:

    /*! Insert a number of rows. */
  void insertRows( void );
    /*! Erase selected rows. */
  void eraseRows( void );
    /*! Fill in selected cells. */
  void fillCells( void );
    /*! Writes new settings to the input trace options and emit newInputSettings(). */
  void dialogClosed( int r );
 

signals:

    /*! This signal is emited whenever new settings are written to the input trace options. */
  void newInputSettings( void );

private:

    /*! Read in table from current input trace options. */
  void fillTable( void );
    /*! Fill in a row of the table. */
  void fillRow( int row, const string &name, const string &device, int channel,
		const string &reference, double samplerate, int gainindex,
		double scale, const string &unit, bool center );
    /*! Get content of a row of the table. */
  void getRow( int row, string &basename, int &nameinx, string &device, int &channel,
	       string &reference, double &samplerate, int &gainindex,
	       double &scale, string &unit, bool &center );

  QTableWidget *Table;

  Options &Opts;

  bool Processing;

};


}; /* namespace relacs */
 
#endif // INPUTCONFIG_H
