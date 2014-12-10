/*
  doublespinbox.cc
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


#include <cmath>
#include <relacs/str.h>
#include <relacs/doublespinbox.h>

namespace relacs {


DoubleSpinBox::DoubleSpinBox( QWidget *parent )
  : QDoubleSpinBox( parent ),
    FormatType( 'g' )
{
}


char DoubleSpinBox::formatType( void ) const
{
  return FormatType;
}


void DoubleSpinBox::setFormatType( char f )
{
  if ( f == 'f' || f == 'F' ||
       f == 'e' || f == 'E' ||
       f == 'g' || f == 'G' )
    FormatType = f;
}


void DoubleSpinBox::setFormat( const string &format )
{
  Str f( format );
  int width = 0;
  int precision = 6;
  char type = 'g';
  char pad = ' ';
  f.readFormat( 0, width, precision, type, pad );
  setDecimals( precision >= 0 ? precision : 6 );
  FormatType = type != ' ' ? type : 'g';
}


int DoubleSpinBox::precision( void ) const
{
  return decimals();
}


void DoubleSpinBox::setPrecision( int p )
{
  setDecimals( p );
}


QString DoubleSpinBox::textFromValue( double value ) const
{
  if ( ::fabs( value ) < 0.001*::pow( 10.0, -decimals() ) )
    value = 0.0;
  QLocale ql;
  ql.setNumberOptions( QLocale::OmitGroupSeparator );
  return ql.toString( value, FormatType, decimals() );
}


double DoubleSpinBox::valueFromText( const QString &text ) const
{
  double value = QDoubleSpinBox::valueFromText( text );
  if ( ::fabs( value ) < 0.001*::pow( 10.0, -decimals() ) )
    value = 0.0;
  return value;
}


}; /* namespace relacs */

#include "moc_doublespinbox.cc"

