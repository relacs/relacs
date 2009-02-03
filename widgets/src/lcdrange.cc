/*
  lcdrange.cc
  An LCD number with range.

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


#include <qscrollbar.h>
#include <qlcdnumber.h>
#include <qlabel.h>
#include <qlayout.h>
#include <relacs/lcdrange.h>

namespace relacs {


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
  QBoxLayout *bl = new QBoxLayout( this, QBoxLayout::TopToBottom );
  bl->setAutoAdd( true );
  LCD  = new QLCDNumber( nodigits, this, "LCD"  );
  LCD->setSegmentStyle( QLCDNumber::Filled );
  SBar = new QScrollBar( minval, maxval, linestep, pagestep, initval,
			 QScrollBar::Horizontal, this, "scrollbar" );
  SBar->setFixedHeight( SBar->sizeHint().height() );
  Label  = new QLabel( this, "Label"  );
  Label->setAlignment( AlignCenter );
  Label->setFixedHeight( Label->sizeHint().height() + 8 );
  connect( SBar, SIGNAL(valueChanged(int)), LCD, SLOT(display(int)) );
  connect( SBar, SIGNAL(valueChanged(int)), SIGNAL(valueChanged(int)) );
}


int LCDRange::value( void ) const
{
  return SBar->value();
}


string LCDRange::text( void ) const
{
  return Label->text();
}


void LCDRange::setValue( int value )
{
  SBar->setValue( value );
}


void LCDRange::setRange( int minval, int maxval )
{
  if ( minval < 0 || minval > maxval ) {
    warning( "LCDRange::setRange(%d,%d)\n"
	     "\tRange must start with zero\n"
	     "\tand minVal must not be greater than maxVal",
	     minval, maxval );
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
  SBar->setSteps( lstep, pstep );
}


}; /* namespace relacs */

#include "moc_lcdrange.cc"

