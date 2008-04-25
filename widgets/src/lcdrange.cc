/*
  lcdrange.cc
  An LCD number with range.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <relacs/lcdrange.h>

#include <qscrollbar.h>
#include <qlcdnumber.h>
#include <qlabel.h>

LCDRange::LCDRange( QWidget *parent, const char *name ,
		    int nodigits, int minval, int maxval,
		    int linestep, int pagestep, int initval )
  : QWidget( parent, name )
{
  init(nodigits, minval, maxval, linestep, pagestep, initval);
}

LCDRange::LCDRange( const char *s, QWidget *parent, const char *name,
		    int nodigits, int minval, int maxval,
		    int linestep, int pagestep, int initval )
  : QWidget( parent, name )
{
  init(nodigits, minval, maxval, linestep, pagestep, initval);
  setText( s );
}

void LCDRange::init(int nodigits, int minval, int maxval,
		    int linestep, int pagestep, int initval)
{
  lcd  = new QLCDNumber( nodigits, this, "lcd"  );
  lcd->move( 0, 0 );
  lcd->setSegmentStyle( QLCDNumber::Filled );
  sBar = new QScrollBar( minval, maxval, linestep, pagestep, initval,
			 QScrollBar::Horizontal, this, "scrollbar" );
  label  = new QLabel( this, "label"  );
  label->setAlignment( AlignCenter );
  connect( sBar, SIGNAL(valueChanged(int)), lcd, SLOT(display(int)) );
  connect( sBar, SIGNAL(valueChanged(int)), SIGNAL(valueChanged(int)) );
  
}

int LCDRange::value() const
{
  return sBar->value();
}

const char *LCDRange::text() const
{
  return label->text();
}

QSize LCDRange::sizeHint( void ) const
{
  QSize s = label->sizeHint();
  s += QSize( 0, 100 );
  return s;
}


QSize LCDRange::minimumSizeHint( void ) const 
{ 
  QSize QS( 50, 50 ); 
  return QS; 
}


QSizePolicy LCDRange::sizePolicy( void ) const 
{ 
  QSizePolicy QSP( QSizePolicy::Expanding, QSizePolicy::Expanding ); 
  return QSP; 
}


void LCDRange::setValue( int value )
{
  sBar->setValue( value );
}

void LCDRange::setRange( int minVal, int maxVal )
{
  if ( minVal < 0 || minVal > maxVal ) {
    warning( "LCDRange::setRange(%d,%d)\n"
	     "\tRange must start with zero\n"
	     "\tand minVal must not be greater than maxVal",
	     minVal, maxVal );
    return;
  }
  sBar->setRange( minVal, maxVal );
}

void LCDRange::setText( const char *s )
{
  label->setText( s );
}

void LCDRange::setSteps( int lstep, int pstep )
{
  sBar->setSteps(lstep,pstep);
}

void LCDRange::resizeEvent( QResizeEvent * )
{
  lcd->resize( width(), height() - 41 - 5 );
  sBar->setGeometry( 0, lcd->height() + 5, width(), 16 );
  label->setGeometry( 0, lcd->height() + 21, width(), 20 );
}

#include "moc_lcdrange.cc"
