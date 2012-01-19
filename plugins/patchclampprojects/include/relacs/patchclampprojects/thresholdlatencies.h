/*
  patchclampprojects/thresholdlatencies.h
  Measures spike latencies in response to pulses close to the firing threshold.

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

#ifndef _RELACS_PATCHCLAMPPROJECTS_THRESHOLDLATENCIES_H_
#define _RELACS_PATCHCLAMPPROJECTS_THRESHOLDLATENCIES_H_ 1

#include <deque>
#include <relacs/array.h>
#include <relacs/sampledata.h>
#include <relacs/eventlist.h>
#include <relacs/tablekey.h>
#include <relacs/plot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace patchclampprojects {


/*!
\class ThresholdLatencies
\brief [RePro] Measures spike latencies in response to pulses close to the firing threshold.
\author Jan Benda
\version 1.2 (Nov 03, 2010)
\par Options
- Test-Pulse
- \c durationsel=in milliseconds: Set duration of stimulus (\c string)
- \c duration=100ms: Duration of stimulus (\c number)
- \c durationfac=1tau_m: Duration of stimulus (\c number)
- \c startamplitudesrc=custom: Set initial amplitude to (\c string)
- \c startamplitude=0.1nA: Initial amplitude of current stimulus (\c number)
- \c startamplitudestep=0.1nA: Initial size of amplitude steps used for searching threshold (\c number)
- \c amplitudestep=0.01nA: Final size of amplitude steps used for oscillating around threshold (\c number)
- \c adjust=DC: Adjust (\c string)
- First Pre-Pulse
- \c preduration=0ms: Duration of first pre-pulse stimulus (\c number)
- \c preamplitudesrc=custom: Set amplitude of first pre-pulse to (\c string)
- \c preamplitude=0.1nA: Amplitude of first pre-pulse stimulus (\c number)
- \c prevcamplitude=0mV: Voltage clamp value of first pre-pulse (\c number)
- \c prevcgain=0mS: Gain for voltage clamp of first pre-pulse (\c number)
- \c prepulseramp=none: Start first pre-pulse with a ramp (\c string)
- \c prepulserampwidth=0ms: Width of the ramp (\c number)
- Second Pre-Pulse
- \c pre2duration=0ms: Duration of second pre-pulse stimulus (\c number)
- \c pre2amplitudesrc=custom: Set amplitude of second pre-pulse to (\c string)
- \c pre2amplitude=0.1nA: Amplitude of second pre-pulse stimulus (\c number)
- Post-Pulse
- \c postduration=0ms: Duration of post-pulse stimulus (\c number)
- \c postamplitudesrc=custom: Set post-pulse amplitude to (\c string)
- \c postamplitude=0.1nA: Amplitude of post-pulse stimulus (\c number)
- Timing
- \c searchpause=500ms: Duration of pause between outputs during search (\c number)
- \c pause=1000ms: Duration of pause between outputs (\c number)
- \c delay=50ms: Time before stimullus onset (\c number)
- \c savetracetime=500ms: Length of trace to be saved and analyzed (from test-pulse on) (\c number)
- \c repeats=10: Repetitions of stimulus (\c integer)

Possible stimulus configurations are shown in the figure:
\image html thresholdlatenciesstimuli.png
The left column shows the stimuli without a DC input
whereas in the right column stimuli starting out from a preset DC input
(for example by the RePro SetDC) are shown.
Stimuli A and B are the standard test protocol, whereas for testing out the dynamics
stimuli F and G might be most usefull.
*/


class ThresholdLatencies : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  ThresholdLatencies( void );
  virtual int main( void );
  virtual void config( void );
  void analyze( double dcamplitude, double preamplitude, double prevcamplitude,
		double pre2amplitude, double amplitude,
		double postamplitude, double delay, double preduration,
		double pre2duration, double duration, double postduration,
		double savetime, double pause );
  void plot( bool record, double preduration, double pre2duration,
	     double duration, double postduration );
  void openTraceFile( ofstream &tf, TableKey &tracekey );
  void saveTrace( ofstream &tf, TableKey &tracekey, int index );
  void save( bool dc );
  void saveSpikes( void );
  void saveData( bool dc );


protected:

  Plot P;
  string VUnit;
  string IUnit;
  double IInFac;

  double PrevMeanTestAmplitude;
  double PrevMeanDCTestAmplitude;
  double PrevMeanDCAmplitude;

  struct Data {
    Data( double delay, double predurations, double savetime,
	  const InData &voltage, const InData &current );
    Data( double delay, double predurations, double savetime,
	  const InData &voltage );
    double DCAmplitude;
    double Amplitude;
    double PreAmplitude;
    double PreVCAmplitude;
    double Pre2Amplitude;
    double PostAmplitude;
    SampleDataF Voltage;
    SampleDataF Current;
    EventData Spikes;
    int SpikeCount;
    int BaseSpikeCount;
  };
  deque< Data > Results;

  int SpikeCount;
  int TrialCount;
  ArrayD Amplitudes;
  ArrayD DCAmplitudes;
  ArrayD PreAmplitudes;
  ArrayD Pre2Amplitudes;
  ArrayD PostAmplitudes;
  ArrayD Latencies;
  ArrayI SpikeCounts;
  EventList Spikes;

  Options Header;

};


}; /* namespace patchclampprojects */

#endif /* ! _RELACS_PATCHCLAMPPROJECTS_THRESHOLDLATENCIES_H_ */
