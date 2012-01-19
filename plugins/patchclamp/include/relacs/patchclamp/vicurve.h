/*
  patchclamp/vicurve.h
  V-I curve measured in current-clamp

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

#ifndef _RELACS_PATCHCLAMP_VICURVE_H_
#define _RELACS_PATCHCLAMP_VICURVE_H_ 1

#include <vector>
#include <relacs/sampledata.h>
#include <relacs/multiplot.h>
#include <relacs/rangeloop.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace patchclamp {


/*!
\class VICurve
\brief [RePro] V-I curve measured in current-clamp
\author Jan Benda
\version 1.1 (Nov 03, 2010)
\par Screenshot
\image html vicurve.png

\par Options
- Stimuli
- \c ibase=zero: Currents are relative to (\c string)
- \c imin=-2nA: Minimum injected current (\c number)
- \c imax=1.5nA: Maximum injected current (\c number)
- \c istep=0.1nA: Minimum step-size of current (\c number)
- \c userm=true: Use membrane resistance for estimating istep from vstep (\c boolean)
- \c vstep=2mV: Minimum step-size of voltage (\c number)
- Timing
- \c duration=500ms: Duration of current output (\c number)
- \c delay=100ms: Delay before current pulses (\c number)
- \c pause=1000ms: Duration of pause between current pulses (\c number)
- \c ishuffle=AlternateOutUp: Initial sequence of currents for first repetition (\c string)
- \c shuffle=Random: Sequence of currents (\c string)
- \c iincrement=-3: Initial increment for currents (\c integer)
- \c singlerepeat=1: Number of immediate repetitions of a single stimulus (\c integer)
- \c blockrepeat=5: Number of repetitions of a fixed intensity increment (\c integer)
- \c repeat=100: Number of repetitions of the whole V-I curve measurement (\c integer)
- Analysis
- \c vmin=-120mV: Minimum value for membrane voltage (\c number)
- \c sswidth=100ms: Window length for steady-state analysis (\c number)
- \c ton=20ms: Timepoint of onset-voltage measurement (\c number)
- \c plotstdev=true: Plot standard deviation of membrane potential (\c boolean)
*/


class VICurve : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  VICurve( void );
  virtual void config( void );
  virtual int main( void );
  void plot( double duration, int inx );
  void save( void );
  void saveData( void );
  void saveTrace( void );


protected:

  MultiPlot P;
  string VUnit;
  string IUnit;
  double VFac;
  double IFac;
  double IInFac;

  struct Data
  {
    Data( void );
    void analyze( int count, const InData &intrace,
		  const EventData &spikes, const InData *incurrent,
		  double iinfac, double delay,
		  double duration, double ton, double sswidth );
    double DC;
    double I;
    double VRest;
    double VRestsd;
    double VOn;
    double VOnsd;
    double VPeak;
    double VPeaksd;
    double VPeakTime;
    double VSS;
    double VSSsd;
    ArrayD SpikeCount;
    SampleDataD MeanTrace;
    SampleDataD SquareTrace;
    SampleDataD StdevTrace;
    SampleDataD MeanCurrent;
  };
  vector< Data > Results;
  RangeLoop Range;
  Options Header;

};


}; /* namespace patchclamp */

#endif /* ! _RELACS_PATCHCLAMP_VICURVE_H_ */
