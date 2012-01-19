/*
  lcdrange.h
  An LCD number with range.

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

#ifndef _RELACS_LCDRANGE_H_
#define _RELACS_LCDRANGE_H_ 1

#include <string>
#include <QWidget>

using namespace std;


class QScrollBar;
class QLCDNumber;
class QLabel;

namespace relacs {


/*! 
\class LCDRange
\author Jan Benda
\version 1.0
\brief An LCD number with range.
*/

class LCDRange : public QWidget
{
  Q_OBJECT

public:

  LCDRange( QWidget *parent=0,
	    int nodigits=2, int minval=0, int maxval=99,
	    int linestep=5, int pagestep=10, int initval=0 );
  LCDRange( const char *s, QWidget *parent,
	    int nodigits=2, int minval=0, int maxval=99,
	    int linestep=5, int pagestep=10, int initval=0 );
  LCDRange( const char *s,
	    int nodigits=2, int minval=0, int maxval=99,
	    int linestep=5, int pagestep=10, int initval=0,
	    QWidget *parent=0 );
  
    /*! The (integer) value that is currently displayed. */
  int value( void ) const;
    /*! The text that is displayed below the LCD display. */
  string text( void ) const;

  
public slots:

    /*! Set the (integer) value that is displayed to \a val . */
  void setValue( int val );
    /*! Set the maximum range of allowed values to \a minval to \a maxval . */
  void setRange( int minval, int maxval );
    /*! Set the text that is displayed below the LCD display to \a text . */
  void setText( const string &text );
    /*! Set the step increments to \a linestep and \a pagestep. */
  void setSteps( int linestep, int pagestep );


signals:

    /*! This signal is emmited whenever a new value is displayed.
        \a val is the new value. */
  void valueChanged( int val );


private:

  void init( int nodigits, int minval, int maxval,
	     int linestep, int pagestep, int initval );
  
  QScrollBar *SBar;
  QLCDNumber *LCD;
  QLabel *Label;

};


}; /* namespace relacs */

#endif /* ! _RELACS_LCDRANGE_H_ */
