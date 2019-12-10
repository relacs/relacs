/*
  base/setattenuatorgain.h
  Set the gain factor of an Attenuate interface

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

#ifndef _RELACS_BASE_SETATTENUATORGAIN_H_
#define _RELACS_BASE_SETATTENUATORGAIN_H_ 1

#include <relacs/repro.h>
using namespace relacs;

namespace base {


/*!
\class SetAttenuatorGain
\brief [RePro] Set the gain factor of an Attenuate interface
\author Jan Benda
\version 1.0 (Nov 26, 2014)
\par Options
- \c outrace=V-1: Input trace (\c string)
- \c gain=1: attenuator gain to be set (\c number)
- \c interactive=true: Set values interactively (\c boolean)
*/


class SetAttenuatorGain : public RePro
{
  Q_OBJECT

public:

  SetAttenuatorGain( void );
  virtual void preConfig( void );
  virtual void config( void );
  virtual int main( void );


public slots:

  void acceptGains( void );
  void setGains( void );
  void keepGains( void );


protected:

  virtual void keyPressEvent( QKeyEvent *e );
  virtual void customEvent( QEvent *qce );

  OptWidget SGW;
  Options AttOpts;
  bool Interactive;
  bool Change;
  bool Quit;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_SETATTENUATORGAIN_H_ */
