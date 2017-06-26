/*
  lcdrange.cc
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


#include <iostream>
#include <QScrollBar>
#include <QLCDNumber>
#include <QLabel>
#include <QVBoxLayout>
#include <relacs/lcdrange.h>

namespace relacs {


LCDRange::LCDRange( QWidget *parent,
		    int nodigits, double minvalue, double maxvalue,
		    double linestep, double pagestep, double value )
  : QWidget( parent )
{
  init( nodigits, minvalue, maxvalue, linestep, pagestep, value );
}


LCDRange::LCDRange( const char *s, QWidget *parent,
		    int nodigits, double minvalue, double maxvalue,
		    double linestep, double pagestep, double value )
  : QWidget( parent )
{
  init( nodigits, minvalue, maxvalue, linestep, pagestep, value );
  setText( s );
}


LCDRange::LCDRange( const char *s, int nodigits, double minvalue, double maxvalue,
		    double linestep, double pagestep, double value, QWidget *parent )
  : QWidget( parent )
{
  init( nodigits, minvalue, maxvalue, linestep, pagestep, value );
  setText( s );
}


void LCDRange::init( int nodigits, double minvalue, double maxvalue,
		     double linestep, double pagestep, double value )
{
  MinValue = minvalue;
  MaxValue = maxvalue;
  LineStep = linestep;
  PageStep = pagestep;
  QVBoxLayout *bl = new QVBoxLayout;
  bl->setMargin( 0 );
  setLayout( bl );
  LCD = new QLCDNumber( nodigits );
  LCD->setSegmentStyle( QLCDNumber::Filled );
  bl->addWidget( LCD );
  SBar = new QScrollBar( Qt::Horizontal );
  // scrollbarindex = ( value - minvalue )/factor
  // value = scrollbarindex*factor + minvalue
  Factor = 0.1*LineStep;
  SBar->setRange( 0, (int)::round(( MaxValue - MinValue )/Factor) );
  SBar->setSingleStep( (int)::round(LineStep/Factor) );
  SBar->setPageStep( (int)::round(PageStep/Factor) );
  SBar->setValue( (int)::round((value - MinValue)/Factor) );
  SBar->setFixedHeight( SBar->sizeHint().height() );
  bl->addWidget( SBar );
  Label = new QLabel;
  Label->setAlignment( Qt::AlignCenter );
  Label->setFixedHeight( Label->sizeHint().height() );
  bl->addWidget( Label );
  connect( SBar, SIGNAL(valueChanged(int)), this, SLOT(transformValue(int)) );
  connect( this, SIGNAL(valueChanged(double)), LCD, SLOT(display(double)) );
}


string LCDRange::text( void ) const
{
  return Label->text().toStdString();
}


void LCDRange::setText( const string &s )
{
  Label->setText( s.c_str() );
}


double LCDRange::value( void ) const
{
  return SBar->value() * Factor + MinValue;
}


void LCDRange::setValue( double value )
{
  SBar->setValue( (int)::round(( value - MinValue )/Factor) );
}


void LCDRange::setRange( double minvalue, double maxvalue )
{
  if ( minvalue < 0 || minvalue > maxvalue ) {
    cerr << "LCDRange::setRange( " << minvalue << ", " << maxvalue << ")\n"
	 << "\tand minValue must not be greater than maxValue\n";
    return;
  }
  double val = value();
  MinValue = minvalue;
  MaxValue = maxvalue;
  SBar->setRange( 0, (int)::round(( MaxValue - MinValue )/Factor) );
  SBar->setValue( (int)::round((val - MinValue)/Factor) );
}


void LCDRange::setSteps( double linestep, double pagestep )
{
  double val = value();
  LineStep = linestep;
  PageStep = pagestep;
  Factor = 0.1*LineStep;
  SBar->setRange( 0, (int)::round(( MaxValue - MinValue )/Factor) );
  SBar->setSingleStep( (int)::round(LineStep/Factor) );
  SBar->setPageStep( (int)::round(PageStep/Factor) );
  SBar->setValue( (int)::round((val - MinValue)/Factor) );
}


void LCDRange::transformValue( int value )
{
  double dval = value*Factor + MinValue;
  emit valueChanged( dval );
}


}; /* namespace relacs */

#include "moc_lcdrange.cc"

