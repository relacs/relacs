/*
  doublespinbox.h
  A spin-box for editing floating point numbers.

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

#ifndef _RELACS_DOUBLESPINBOX_H_
#define _RELACS_DOUBLESPINBOX_H_ 1

#include <QDoubleSpinBox>

using namespace std;


namespace relacs {


/*! 
\class DoubleSpinBox
\author Jan Benda
\brief A spin-box for editing floating point numbers.
*/

class DoubleSpinBox : public QDoubleSpinBox
{
  Q_OBJECT

public:

    /*! Constructs an empty DoubleSpinBox. */
  DoubleSpinBox( QWidget *parent=0 );

    /*! Returns the format type (g, f, or e).
        \sa setFormatType(), setFormat(), precision()  */
  char formatType( void ) const;
    /*! Set the format type to \a f. Can be f, g, or e.
        \sa formatType(), setFormat(), setPrecision() */
  void setFormatType( char f );
    /*! Set the formatType() and the precision() from \a format.
        The width of the \a format is ignored.
        \sa formatType(), setFormatType(), precision(), setPrecision() */
  void setFormat( const string &format );
    /*! The precision used for displaying the floating-point number. */
  int precision( void ) const;
    /*! Set the precision used for displaying the floating-point number
        to \a p. */
  void setPrecision( int p );

    /*! Converts \a value to the text displayed in the DoubleSpinBox. */
  virtual QString textFromValue( double value ) const;
    /*! Converts \a text to a value. */
  virtual double valueFromText( const QString &text ) const;


 private:

  char FormatType;

};


}; /* namespace relacs */

#endif /* ! _RELACS_DOUBLESPINBOX_H_ */
