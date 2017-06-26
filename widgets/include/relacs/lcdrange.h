/*
  lcdrange.h
  An LCD number with range.

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
\brief An LCD number with range.
*/

class LCDRange : public QWidget
{
  Q_OBJECT

public:

  LCDRange( QWidget *parent=0,
	    int nodigits=2, double minval=0.0, double maxval=100.0,
	    double linestep=5.0, double pagestep=10.0, double value=0 );
  LCDRange( const char *s, QWidget *parent,
	    int nodigits=2, double minval=0.0, double maxval=100.0,
	    double linestep=5.0, double pagestep=10.0, double value=0 );
  LCDRange( const char *s,
	    int nodigits=2, double minval=0.0, double maxval=100.0,
	    double linestep=5.0, double pagestep=10.0, double value=0,
	    QWidget *parent=0 );
  
    /*! The text that is displayed below the LCD display. */
  string text( void ) const;
    /*! The value that is currently displayed. */
  double value( void ) const;

  
public slots:

    /*! Set the text that is displayed below the LCD display to \a text . */
  void setText( const string &text );
    /*! Set the value that is displayed to \a value . */
  void setValue( double value );
    /*! Set the maximum range of allowed values to \a minvalue to \a maxvalue . */
  void setRange( double minvalue, double maxvalue );
    /*! Set the step increments to \a linestep and \a pagestep. */
  void setSteps( double linestep, double pagestep );


signals:

    /*! This signal is emmited whenever a new value is displayed.
        \a value is the new value. */
  void valueChanged( double value );


private:

  void init( int nodigits, double minvalue, double maxvalue,
	     double linestep, double pagestep, double value );
  
  QScrollBar *SBar;
  QLCDNumber *LCD;
  QLabel *Label;
  double MinValue;
  double MaxValue;
  double PageStep;
  double LineStep;
  double Factor;


private slots:

    /*! Transform the integer value from the scrollbar to the displayed value. */
  void transformValue( int value );

};


}; /* namespace relacs */

#endif /* ! _RELACS_LCDRANGE_H_ */
