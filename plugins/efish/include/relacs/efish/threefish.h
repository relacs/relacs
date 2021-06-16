/*
  efish/threefish.h
  Beats of three fish.

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

#ifndef _RELACS_EFISH_THREEFISH_H_
#define _RELACS_EFISH_THREEFISH_H_ 1

#include <relacs/multiplot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
#include <relacs/efield/traces.h>
#include <relacs/efield/eodtools.h>
using namespace relacs;

namespace efish {


/*!
\class ThreeFish
\brief [RePro] Beats of three fish
\author Jan Benda
\version 1.0 (Jun 15, 2021)
\par Options
- \c Stimulus
    - \c duration1=1000ms: Duration of signal (\c number)
    - \c deltaf1min=0Hz: Minimum delta f (beat frequency) of first fish (\c number)
    - \c deltaf1max=100Hz: Maximum delta f (beat frequency) of first fish (\c number)
    - \c deltaf1step=10Hz: Increment delta f (beat frequency) of first fish (\c number)
    - \c contrast1=10%: Contrast of first fish (\c number)
    - \c duration2=1000ms: Duration of second fish (\c number)
    - \c offset2=0ms: Offset of second fish (\c number)
    - \c deltaf2min=0Hz: Minimum delta f (beat frequency) of second fish (\c number)
    - \c deltaf2max=100Hz: Maximum delta f (beat frequency) of second fish (\c number)
    - \c deltaf2step=10Hz: Increment delta f (beat frequency) of second fish (\c number)
    - \c contrast2=10%: Contrast of second fish (\c number)
    - \c shuffle=Up: Order of delta f's (\c string)
    - \c increment=-1: Initial increment for delta f's (\c integer)
    - \c repeats=10: Repeats (\c integer)
    - \c pause=100ms: Pause between signals (\c number)
- \c Analysis
    - \c before=100ms: Spikes recorded before stimulus (\c number)
    - \c after=100ms: Spikes recorded after stimulus (\c number)
    - \c sigma=10ms: Standard deviation of rate smoothing kernel (\c number)
*/


class ThreeFish
  : public RePro,
    public ephys::Traces,
    public efield::Traces,
    public efield::EODTools
{
  Q_OBJECT

public:

  ThreeFish( void );
  virtual int main( void );


protected:

  int fishEOD(double &rate, double &amplitude);
  int makeEOD(double fishrate, double deltaf, double duration,
	      double phase, OutData &eod);
  void stop( void );

  MultiPlot P;

};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_THREEFISH_H_ */
