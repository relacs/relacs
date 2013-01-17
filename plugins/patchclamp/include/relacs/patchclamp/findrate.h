/*
  patchclamp/findrate.h
  Finds the DC current need to evoke a given target firing rate.

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

#ifndef _RELACS_PATCHCLAMP_FINDRATE_H_
#define _RELACS_PATCHCLAMP_FINDRATE_H_ 1

#include <relacs/plot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace patchclamp {


/*!
\class FindRate
\brief [RePro] Finds the DC current need to evoke a given target firing rate.
\author Jan Benda
\version 1.0 (Nov 25, 2010)
\par Screenshot
\image html findrate.png

\par Options
- \c rate=100Hz: Target firing rate (\c number)
- \c ratetol=5Hz: Tolerance for target firing rate (\c number)
- \c startamplitudesrc=custom: Set initial dc-current to (\c string)
- \c startamplitude=0nA: Initial amplitude of dc-current (\c number)
- \c amplitudestep=8nA: Initial size of dc-current steps used for searching target rate (\c number)
- \c duration=500ms: Duration of dc-current stimulus (\c number)
- \c skipwin=100ms: Initial portion of stimulus not used for analysis (\c number)
*/


class FindRate : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  FindRate( void );
  virtual void preConfig( void );
  virtual int main( void );


protected:

  void saveData( const MapD &rates );

  Plot P;
  string IUnit;
  double PrevDCAmplitude;

};


}; /* namespace patchclamp */

#endif /* ! _RELACS_PATCHCLAMP_FINDRATE_H_ */
