/*
  base/setinputgain.h
  Set the gain of analog input traces.

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

#ifndef _RELACS_BASE_SETINPUTGAIN_H_
#define _RELACS_BASE_SETINPUTGAIN_H_ 1

#include <relacs/repro.h>
using namespace relacs;

namespace base {


/*!
\class SetInputGain
\brief [RePro] Set the gain of analog input traces.
\author Jan Benda
\version 1.0 (Feb 18, 2014)

\par Options
- \c intrace=V-1: Input trace (\c string)
- \c gainindex=0: Index of the gain to be set (\c integer)
- \c interactive=true: Set values interactively (\c boolean)
*/


class SetInputGain : public RePro
{
  Q_OBJECT

public:

  SetInputGain( void );
  virtual void preConfig( void );
  virtual void notify( void );
  virtual int main( void );


public slots:

  void acceptGains( void );
  void setGains( void );
  void setMaxRanges( void );
  void keepGains( void );


protected:

  virtual void keyPressEvent( QKeyEvent *e );
  virtual void customEvent( QEvent *qce );

  OptWidget SGW;
  Options InOpts;
  bool Interactive;
  bool Change;
  bool Quit;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_SETINPUTGAIN_H_ */
