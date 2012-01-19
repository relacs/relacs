/*
  lcdrange.cc
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


#include <iostream>
#include <QScrollBar>
#include <QLCDNumber>
#include <QLabel>
#include <QVBoxLayout>
#include <relacs/lcdrange.h>

namespace relacs {


LCDRange::LCDRange( QWidget *parent,
		    int nodigits, int minval, int maxval,
		    int linestep, int pagestep, int initval )
  : QWidget( parent )
{
  init( nodigits, minval, maxval, linestep, pagestep, initval );
}


LCDRange::LCDRange( const char *s, QWidget *parent,
		    int nodigits, int minval, int maxval,
		    int linestep, int pagestep, int initval )
  : QWidget( parent )
{
  init( nodigits, minval, maxval, linestep, pagestep, initval );
  setText( s );
}


LCDRange::LCDRange( const char *s, int nodigits, int minval, int maxval,
		    int linestep, int pagestep, int initval, QWidget *parent )
  : QWidget( parent )
{
  init( nodigits, minval, maxval, linestep, pagestep, initval );
  setText( s );
}


void LCDRange::init( int nodigits, int minval, int maxval,
		     int linestep, int pagestep, int initval )
{
  QVBoxLayout *bl = new QVBoxLayout;
  bl->setMargin( 0 );
  setLayout( bl );
  LCD = new QLCDNumber( nodigits );
  LCD->setSegmentStyle( QLCDNumber::Filled );
  bl->addWidget( LCD );
  SBar = new QScrollBar( Qt::Horizontal );
  SBar->setRange( minval, maxval );
  SBar->setSingleStep( linestep );
  SBar->setPageStep( pagestep );
  SBar->setValue( initval );
  SBar->setFixedHeight( SBar->sizeHint().height() );
  bl->addWidget( SBar );
  Label  = new QLabel;
  Label->setAlignment( Qt::AlignCenter );
  Label->setFixedHeight( Label->sizeHint().height() );
  bl->addWidget( Label );
  connect( SBar, SIGNAL(valueChanged(int)), LCD, SLOT(display(int)) );
  connect( SBar, SIGNAL(valueChanged(int)), SIGNAL(valueChanged(int)) );
}


int LCDRange::value( void ) const
{
  return SBar->value();
}


string LCDRange::text( void ) const
{
  return Label->text().toStdString();
}


void LCDRange::setValue( int value )
{
  SBar->setValue( value );
}


void LCDRange::setRange( int minval, int maxval )
{
  if ( minval < 0 || minval > maxval ) {
    cerr << "LCDRange::setRange( " << minval << ", " << maxval << ")\n"
	 << "\tRange must start with zero\n"
	 << "\tand minVal must not be greater than maxVal\n";
    return;
  }
  SBar->setRange( minval, maxval );
}


void LCDRange::setText( const string &s )
{
  Label->setText( s.c_str() );
}


void LCDRange::setSteps( int lstep, int pstep )
{
  SBar->setSingleStep( lstep );
  SBar->setPageStep( pstep );
}


}; /* namespace relacs */

#include "moc_lcdrange.cc"

