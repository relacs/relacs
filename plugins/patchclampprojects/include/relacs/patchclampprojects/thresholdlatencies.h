/*
  patchclampprojects/thresholdlatencies.h
  Measures spike latencies in response to pulses close to the firing threshold.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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
\version 1.1 (Oct 15, 2010)
\par Options
- Test-Pulse
- \c durationsel=as multiples of membrane time constant: Set duration of stimulus (\c string)
- \c duration=15ms: Duration of stimulus (\c number)
- \c durationfac=1tau_m: Duration of stimulus (\c number)
- \c startamplitudesrc=custom: Set initial amplitude to (\c string)
- \c startamplitude=0.1nA: Initial amplitude of current stimulus (\c number)
- \c startamplitudestep=0.5nA: Initial size of amplitude steps used for searching threshold (\c number)
- \c amplitudestep=0.003nA: Final size of amplitude steps used for oscillating around threshold (\c number)
- \c adjust=DC: Adjust (\c string)
- Pre- and Post-Pulse
- \c preduration=50ms: Duration of pre-pulse stimulus (\c number)
- \c preamplitudesrc=custom: Set pre-pulse amplitude to (\c string)
- \c preamplitude=0.2: Amplitude of pre-pulse stimulus (\c number)
- \c prepulseramp=cosine: Start the pre-pulse with a ramp (\c string)
- \c prepulserampwidth=10ms: Width of the ramp (\c number)
- \c postduration=100ms: Duration of post-pulse stimulus (\c number)
- \c postamplitudesrc=custom: Set post-pulse amplitude to (\c string)
- \c postamplitude=0.1: Amplitude of post-pulse stimulus (\c number)
- Control
- \c searchpause=3000ms: Duration of pause between outputs during search (\c number)
- \c pause=5000ms: Duration of pause between outputs (\c number)
- \c delay=50ms: Time before stimullus onset (\c number)
- \c savetracetime=1050ms: Length of trace to be saved and analyzed (\c number)
- \c repeats=300: Repetitions of stimulus (\c integer)
- Used amplitudes
- \c stimulusamplitude=0: stimulusamplitude (\c number)
- \c dcstimulusamplitude=0: dcstimulusamplitude (\c number)
- \c dcamplitude=0: dcamplitude (\c number)

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
  void analyze( double dcamplitude, double preamplitude, double amplitude, double postamplitude,
		double delay, double preduration, double duration, double postduration,
		double savetime, double pause );
  void plot( bool record, double preduration, double duration, double postduration );
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
    Data( double delay, double preduration, double savetime,
	  const InData &voltage, const InData &current );
    Data( double delay, double preduration, double savetime,
	  const InData &voltage );
    double DCAmplitude;
    double Amplitude;
    double PreAmplitude;
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
  ArrayD PostAmplitudes;
  ArrayD Latencies;
  ArrayI SpikeCounts;
  EventList Spikes;

  Options Header;

};


}; /* namespace patchclampprojects */

#endif /* ! _RELACS_PATCHCLAMPPROJECTS_THRESHOLDLATENCIES_H_ */
