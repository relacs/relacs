/*
  doublespinbox.cc
  Spin box that allows to handle doubles.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <iostream>
#include <cmath>
#include <limits.h>
#include <relacs/doublespinbox.h>

using namespace std;

namespace relacs {


DoubleSpinBox::DoubleSpinBox( double val, double min, double max,
			      double step,
			      const string &format,
			      QWidget *parent, const char *name )
  : QSpinBox ( parent, name ), 
    Format( format )
{
  construct( min, max, step, min, 0.0 );
  setValue( val );
}


DoubleSpinBox::DoubleSpinBox( double val, double min, double max,
			      double step,
			      double precision,
			      const string &format,
			      QWidget *parent, const char *name )
  : QSpinBox ( parent, name ), 
    Format( format )
{
  construct( min, max, step, 0.0, precision );
  setValue( val );
}


void DoubleSpinBox::construct( double min, double max, double step,
			       double origin, double prec )
{
  Origin = origin;

  DValid = new QDoubleValidator( this );
  Precision = 0.0;

  setRange( min, max, step, prec );

  connect( this, SIGNAL( valueChanged( int ) ),
	   this, SLOT( passValue( int ) ) );
}


double DoubleSpinBox::value( void ) const
{
  return IntToValue( QSpinBox::value() );
}


double DoubleSpinBox::minValue( void ) const
{
  return IntToValue( QSpinBox::minValue() );
}


void DoubleSpinBox::setMinValue( double min )
{
  min = IntToValue( ValueToInt( min ) );
  DValid->setBottom( min );
  setValidator( DValid );
  QSpinBox::setMinValue( ValueToInt( min ) );
}


double DoubleSpinBox::maxValue( void ) const
{
  return IntToValue( QSpinBox::maxValue() );
}


void DoubleSpinBox::setMaxValue( double max )
{
  max = IntToValue( ValueToInt( max ) );
  DValid->setTop( max );
  setValidator( DValid );
  QSpinBox::setMaxValue( ValueToInt( max ) );
}


double DoubleSpinBox::step( void ) const
{
  return QSpinBox::lineStep() * Precision;
}


void DoubleSpinBox::setStep( double step, double prec )
{
  if ( prec > 0.0 )
    setPrecision( prec );
  else if ( step > 0.0 && ( step < Precision || Precision == 0 ) )
    setPrecision( step );

  QSpinBox::setLineStep( (int)rint( step / Precision ) );
}


void DoubleSpinBox::setPrecision( double prec )
{
  bool reset = ( Precision > 0.0 );

  // retrieve current settings:
  double min = minValue();
  double max = maxValue();
  double steps = step();

  // set new precision:
  Precision = prec;

  // set maximum number of displayed decimals:
  int pdec = (int)ceil( - log10( Precision ) );
  int width, dec;
  char type, pad;
  Format.readFormat( 0, width, dec, type, pad );
  if ( type == 'g' )
    dec = pdec;
  else {
    if ( dec <= pdec )
      dec = pdec;
    else
      Precision = pow( 10.0, -dec );
  }
  DValid->setDecimals( dec );
  setValidator( DValid );

  // set settings for new precision:
  if ( reset ) {
    QSpinBox::setLineStep( (int)rint( steps / Precision ) );
    QSpinBox::setMinValue( ValueToInt( min ) );
    QSpinBox::setMaxValue( ValueToInt( max ) );
  }
}


void DoubleSpinBox::setRange ( double min, double max,
			       double step, double prec )
{
  setStep( step, prec );
  setMinValue( min );
  setMaxValue( max );
}


string DoubleSpinBox::format( void ) const
{
  return Format;
}


void DoubleSpinBox::setFormat( const string &format )
{
  Format = format;
}


void DoubleSpinBox::passValue( int val )
{
  emit valueChanged( IntToValue( val ) );
}


void DoubleSpinBox::setValue( double val )
{
  QSpinBox::setValue( ValueToInt( val ) );
}


QString DoubleSpinBox::mapValueToText( int value )
{
  Str s( IntToValue( value ), Format ); 
  //  cerr << value << " " << IntToValue( value ) << " " << s << endl;
  return QString( s.c_str() );
}


int DoubleSpinBox::mapTextToValue( bool *ok )
{
  double v = cleanText().toDouble( ok );
  return ValueToInt( v );
}


double DoubleSpinBox::IntToValue( int v ) const
{
  return Origin + Precision * v;
}


int DoubleSpinBox::ValueToInt( double v ) const
{
  double i = rint( ( v - Origin ) / Precision );
  if ( i < INT_MIN )
    return INT_MIN;
  else if ( i > INT_MAX )
    return INT_MAX;
  else
    return (int)i;
}


double DoubleSpinBox::floorLog10( double prec )
{
  return pow( 10.0, floor( log10( prec ) ) );
}


}; /* namespace relacs */

#include "moc_doublespinbox.cc"

