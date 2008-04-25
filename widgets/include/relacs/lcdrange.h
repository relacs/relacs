/*
  lcdrange.h
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

#ifndef LCDRANGE_H
#define LCDRANGE_H

#include <qwidget.h>

class QScrollBar;
class QLCDNumber;
class QLabel;

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
  LCDRange( QWidget *parent=0, const char *name=0,
	    int nodigits=2, int minval=0, int maxval=99,
	    int linestep=5, int pagestep=10, int initval=0);
  LCDRange( const char *s, QWidget *parent=0, const char *name=0,
	    int nodigits=2, int minval=0, int maxval=99,
	    int linestep=5, int pagestep=10, int initval=0);
  
  int         value() const;
  const char *text()  const;
  QSize sizeHint ( void ) const;
  QSize minimumSizeHint ( void ) const;
  QSizePolicy sizePolicy( void ) const;
  
public slots:
  void setValue( int );
  void setRange( int minVal, int maxVal );
  void setText( const char * );
  void setSteps( int, int );

signals:
  void valueChanged( int );

protected:
  void resizeEvent( QResizeEvent * );

private:
  void init(int nodigits, int minval, int maxval,
	    int linestep, int pagestep, int initval);
  
  QScrollBar  *sBar;
  QLCDNumber  *lcd;
  QLabel      *label;
};

#endif // LCDRANGE_H
