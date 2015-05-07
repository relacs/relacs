/*
  base/instantaneousrate.h
  Computes the instantaneous rate between succeeding events.

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

#ifndef _RELACS_BASE_INSTANTANEOUSRATE_H_
#define _RELACS_BASE_INSTANTANEOUSRATE_H_ 1

#include <relacs/filter.h>
using namespace relacs;

namespace base {


/*!
\class InstantaneousRate
\brief [Filter] Computes the instantaneous rate between succeeding events.
\author Jan Benda
\version 1.2 (May 07, 2015)

\par Options
- \c tau=0ms: Filter time constant (\c number)
- \c minrate=0Hz: Minimum initial rate (\c number)
- \c maxrate=1000Hz: Maximum initial rate (\c number)
*/


class InstantaneousRate : public Filter
{
  Q_OBJECT

public:

  InstantaneousRate( const string &ident="", int mode=0 );
  virtual void notify( void );
  virtual int init( const EventData &inevents, InData &outdata );
  virtual int filter( const EventData &inevents, InData &outdata );


private:

  OptWidget IRW;

  double Tau;

  double DeltaT;
  double TFac;
  float X;
  int Index;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_INSTANTANEOUSRATE_H_ */
