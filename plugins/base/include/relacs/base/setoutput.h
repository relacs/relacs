/*
  base/setoutput.h
  Set an output to a specified value

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

#ifndef _RELACS_BASE_SETOUTPUT_H_
#define _RELACS_BASE_SETOUTPUT_H_ 1

#include <relacs/optwidget.h>
#include <relacs/repro.h>
using namespace relacs;

namespace base {


/*!
\class SetOutput
\brief [RePro] Set an output to a specified value
\author Jan Benda

Either sets the value of a single output trace or
shows a dialog, where the values of all output parameter
can be specified and set.

\par Options
- \c outtrace=V-1: Output trace (\c string)
- \c value=0: Value to be writen to output trace (\c number)
- \c intensity=0: Intensity for an attenuator (\c number)
- \c interactive=false: Set values interactively (\c boolean)

\version 1.4 (Dec 19, 2014)
*/


class SetOutput : public RePro
{
  Q_OBJECT

public:

  SetOutput( void );
  virtual void preConfig( void );
  virtual void notify( void );
  virtual int main( void );

  const Options &outTraces( void ) const;


public slots:

  void acceptValues( void );
  void setValues( void );
  void setZeros( void );
  void keepValues( void );


protected:

  virtual void keyPressEvent( QKeyEvent *e );
  virtual void customEvent( QEvent *qce );

  OptWidget STW;
  Options OutOpts;
  bool Interactive;
  bool Change;
  bool Quit;
  static const int ChannelFlag;
  static const int ParameterFlag;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_SETOUTPUT_H_ */

