/*
  doublespinbox.h
  Spin box that allows to handle doubles.

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

#ifndef _DOUBLESPINBOX_H_
#define _DOUBLESPINBOX_H_

#include <string>
#include <qvalidator.h> 
#include <qspinbox.h>
#include "str.h"
using namespace std;


/*!
  \class DoubleSpinBox
  \author Jan Benda
  \author Christian Machens
  \version 1.0
  \brief Spin box that allows to handle doubles.
*/

class DoubleSpinBox : public QSpinBox
{
  Q_OBJECT

public:

    /*! Constructs a DoubleSpinBox. The minimum
        and maximum allowed value are specified by \a min and
        \a max, respectively.
        The stepsize of the spinbox is given by \a step.
        How the value is displayed is specified by \a format,
        a C-style format string for floats (e.g. %5.2f). */ 
  DoubleSpinBox( double val, double min, double max,
		 double step = 1.0,
		 const string &format = "%g",
		 QWidget *parent = 0,
		 const char *name = 0 );
    /*! Constructs a DoubleSpinBox. The minimum
        and maximum allowed value are specified by \a min and
        \a max, respectively.
        The stepsize of the spinbox is given by \a step.
	The maximum precision is set to \a precision.
        How the value is displayed is specified by \a format,
        a C-style format string for floats (e.g. %5.2f). */ 
  DoubleSpinBox( double val, double min, double max,
		 double step, double precision,
		 const string &format = "%g",
		 QWidget *parent = 0,
		 const char *name = 0 );

    /*! Returns the value currently displayed by the spinbox.
        \sa setValue(). */
  double value( void ) const;

    /*! Returns the current minimum value. \sa setMinValue(). */
  double minValue( void ) const;
    /*! Set the minimum possible value to \a min. \sa minValue(). */
  void setMinValue( double min );

    /*! Returns the current maximum value. \sa setMaxValue(). */
  double maxValue( void ) const;
    /*! Set the maximum possible value to \a max. */
  void setMaxValue( double max );

    /*! Returns the current step size.
	\sa setStep(). */
  double step( void ) const;
    /*! Set the step size to \a step.
        If \a prec is greater than zero the precision is set to \a prec.
        \sa step(). */
  void setStep( double step, double prec=0.0 );

    /*! Set the maximum precision to \a prec. */
  void setPrecision( double prec );
    /*! Set the possible range of values to [ \a min, \a max ].
        If \a step is greater than zero the step size is set to \a step. */
  void setRange ( double min, double max, double step=-1, double prec=0.0 );

    /*! The format string. */
  string format( void ) const;
    /*! Set the format string to \a format. */
  void setFormat( const string &format );

    /*! Returns the largest power of ten smaller or equal to \a prec. */
  static double floorLog10( double prec );


public slots:

    /*! Set value of spin box to \a number. \sa value(). */
  void setValue( double number );

signals:

    /*! Send signal that the value of the spin box changed to \a number.
        \sa value(), setValue(). */
  void valueChanged( double number );


protected:

  QString mapValueToText ( int value );
  int mapTextToValue ( bool *ok );

  double IntToValue( int v ) const;
  int ValueToInt( double v ) const;

  double Origin;
  double Precision;
  Str Format;


private:
  
  void construct( double min, double max, double step,
		  double origin, double prec );

private slots:

  void passValue( int );

private:

  QDoubleValidator *DValid;

};


#endif
