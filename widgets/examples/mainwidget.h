/*
  mainwidget.h
  

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

#ifndef _MAINWIDGET_H_
#define _MAINWIDGET_H_

#include <QWidget>
#include <relacs/options.h>
using namespace relacs;


class MainWidget : public QWidget
{
  Q_OBJECT

public:
  MainWidget( void );
  ~MainWidget( void ) {};

private slots:
  void dialog( void );
  void done( int r );
  void action( int r );
  void accepted( void );

private:
  Options Opt1;
  Options Opt2;

};

#endif
