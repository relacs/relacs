/*
  patchclamp/simple.h
  Simple RePro for testing dynamic clamp

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

#ifndef _RELACS_PATCHCLAMP_SIMPLE_H_
#define _RELACS_PATCHCLAMP_SIMPLE_H_ 1

#include <relacs/plot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace patchclamp {


/*!
\class Simple
\brief [RePro] Test dynamic clamp
\author Jan Benda
\version 0.0 (Feb 14, 2008)
\par Options
- \c intrace=V-1: Input trace (\c string)
- \c outtrace=Current-1: Output trace (\c string)
- \c amplitude=1nA: Amplitude of output signal (\c number)
- \c duration=100ms: Duration of output (\c number)
- \c stimulus=constant: Stimulus type (\c string)
- \c samerate=true: Use sampling rate of input (\c boolean)
- \c samplerate=1kHz: Sampling rate of output (\c number)
- \c pause=400ms: Duration of pause bewteen outputs (\c number)
- \c repeats=0: Repeats (\c integer)
- \c save=false: Save recorded traces (\c boolean)
*/


class Simple : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  Simple( void );
  virtual void config( void );
  virtual void notify( void );
  virtual int main( void );

protected:

  Plot P;

};


}; /* namespace patchclamp */

#endif /* ! _RELACS_PATCHCLAMP_SIMPLE_H_ */

