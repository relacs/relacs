/*
  patchclamp/ficurve.h
  f-I curve measured in current-clamp

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

#ifndef _RELACS_PATCHCLAMP_FICURVE_H_
#define _RELACS_PATCHCLAMP_FICURVE_H_ 1

#include <deque>
#include <relacs/sampledata.h>
#include <relacs/eventlist.h>
#include <relacs/multiplot.h>
#include <relacs/rangeloop.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace patchclamp {


/*!
\class FICurve
\brief [RePro] F-I curve measured in current-clamp
\author Jan Benda
\version 1.2 (Nov 25, 2010)
\par Screenshot
\image html ficurve.png

\par Options
- Stimuli
- \c ibase=zero: Currents are relative to (\c string)
- \c imin=0nA: Minimum injected current (\c number)
- \c imax=1nA: Maximum injected current (\c number)
- \c istep=0.001nA: Minimum step-size of current (\c number)
- \c userm=false: Use membrane resistance for estimating istep from vstep (\c boolean)
- \c vstep=1mV: Minimum step-size of voltage (\c number)
- \c optimizeimin=false: Skip currents that do not evoke action potentials (\c boolean)
- Timing
- \c duration=100ms: Duration of current output (\c number)
- \c delay=100ms: Delay before current pulses (\c number)
- \c pause=400ms: Duration of pause between current pulses (\c number)
- \c ishuffle=Up: Initial sequence of currents for first repetition (\c string)
- \c shuffle=Up: Sequence of currents (\c string)
- \c iincrement=-1: Initial increment for currents (\c integer)
- \c singlerepeat=1: Number of immediate repetitions of a single stimulus (\c integer)
- \c blockrepeat=10: Number of repetitions of a fixed intensity increment (\c integer)
- \c repeat=1: Number of repetitions of the whole V-I curve measurement (\c integer)
- Analysis
- \c fmax=100Hz: Maximum firing rate (\c number)
- \c vmax=-50mV: Maximum steady-state potential (\c number)
- \c sswidth=50ms: Window length for steady-state analysis (\c number)
- \c ratioincrement=0: Optimize range at current increments below (\c integer)
- \c maxratediff=10Hz: Maximum difference between onset and steady-state firing rate for optimization (\c number)
*/


class FICurve : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  FICurve( void );
  virtual void preConfig( void );
  virtual int main( void );
  void plot( double duration, int inx );
  void save( void );
  void saveData( void );
  void saveRate( void );
  void saveSpikes( void );
  void saveTraces( void );


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
		  double duration, double sswidth );
    double DC;
    double I;
    double VRest;
    double VRestSQ;
    double VRestSD;
    double VSS;
    double VSSSQ;
    double VSSSD;
    double PreRate;
    double PreRateSD;
    double SSRate;
    double SSRateSD;
    double MeanRate;
    double MeanRateSD;
    double OnRate;
    double OnRateSD;
    double OnTime;
    double Latency;
    double LatencySD;
    double SpikeCount;
    double SpikeCountSD;
    SampleDataD MeanCurrent;
    SampleDataD Rate;
    SampleDataD RateSD;
    EventList Spikes;
    deque< SampleDataF > Voltage;
    deque< SampleDataF > Current;
  };
  deque< Data > Results;
  RangeLoop Range;
  Options Header;

};


}; /* namespace patchclamp */

#endif /* ! _RELACS_PATCHCLAMP_FICURVE_H_ */
