/*
  patchclamp/singlestimulus.h
  Output of a single stimulus stored in a file.

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

#ifndef _RELACS_PATCHCLAMP_SINGLESTIMULUS_H_
#define _RELACS_PATCHCLAMP_SINGLESTIMULUS_H_ 1

#include <QStackedLayout>
#include <relacs/repro.h>
#include <relacs/eventlist.h>
#include <relacs/sampledata.h>
#include <relacs/multiplot.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace patchclamp {


/*! 
\class SingleStimulus
\brief [RePro] Output of a single stimulus stored in a file.
\author Jan Benda
\version 1.6 (Oct 22, 2013)
\par Options
- \c Waveform
    - \c waveform=From file: Stimulus waveform (\c string)
    - \c stimfile=: Stimulus file (\c string)
    - \c stimampl=0: Amplitude factor (standard deviation) of stimulus file (\c number)
    - \c amplitude=1nA: Amplitude of stimulus (\c number)
    - \c freqsel=frequency: Specify (\c string)
    - \c freq=10Hz: Frequency of waveform (\c number)
    - \c periods=1: Number of periods (\c number)
    - \c dutycycle=50%: Duty-cycle of rectangular waveform (\c number)
    - \c seed=0: Seed for random number generation (\c integer)
    - \c startfreq=1Hz: Start sweep with frequency (\c number)
    - \c endfreq=100Hz: End sweep with frequency (\c number)
    - \c duration=0ms: Maximum duration of stimulus (\c number)
    - \c ramp=2ms: Ramp of stimulus (\c number)
- \c Stimulus
    - \c offset=0nA: Stimulus mean (\c number)
    - \c offsetbase=absolute: Stimulus mean relative to (\c string)
    - \c samerate=true: Use sampling rate of input (\c boolean)
    - \c samplerate=1kHz: Sampling rate of output (\c number)
    - \c repeats=10times: Number of stimulus presentations (\c number)
    - \c pause=1000ms: Duration of pause between stimuli (\c number)
    - \c outtrace=Current-1: Output trace (\c string)
- \c Offset - search
    - \c userate=false: Search offset for target firing rate (\c boolean)
    - \c rate=100Hz: Target firing rate (\c number)
    - \c ratetol=5Hz: Tolerance for target firing rate (\c number)
    - \c offsetstep=8nA: Initial offset step (\c number)
    - \c searchrepeats=2times: Number of search stimulus presentations (\c number)
    - \c silentrate=0Hz: Ignore response below (\c number)
    - \c maxsilent=1: Number of stimulus presentations if response is below silentrate (\c integer)
    - \c skippause=true: Skip pause if response is below silentrate (\c boolean)
    - \c maxsearch=1intensities: Stop search if response does not change for more than (\c integer)
    - \c method=Bisect: Method (\c string)
    - \c minslope=4Hz/nA: Minimum slope required for interpolation (\c number)
    - \c searchduration=0ms: Maximum duration of stimulus (\c number)
    - \c searchpause=0ms: Duration of pause between stimuli (\c number)
- \c Analysis
    - \c skipwin=100ms: Initial portion of stimulus not used for analysis (\c number)
    - \c sigma=10ms: Standard deviation of rate smoothing kernel (\c number)
    - \c before=100ms: Time before stimulus to be analyzed (\c number)
    - \c after=100ms: Time after stimulus to be analyzed (\c number)
    - \c storevoltage=true: Save voltage trace (\c boolean)
    - \c plot=Current voltage trace: Plot shows (\c string)
    - \c Save stimuli
        - \c storemode=session: Save stimuli in (\c string)
        - \c storepath=: Save stimuli in custom directory (\c string)
        - \c storelevel=all: Save (\c string)
*/


class SingleStimulus : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

    /*! Constructor. */
  SingleStimulus( void );
    /*! Destructor. */
  ~SingleStimulus( void );

  virtual void preConfig( void );
  virtual void notify( void );
  virtual int main( void );


protected:

  void openTraceFile( ofstream &tf, TableKey &tracekey, const Options &header );
  void saveTrace( ofstream &tf, TableKey &tracekey, int index,
		  const SampleDataF &voltage, const SampleDataF &current );
  void saveMeanTrace( Options &header, TableKey &tracekey,
		      const SampleDataF &meanvoltage, const SampleDataF &meancurrent );
  void saveSpikes( Options &header, const EventList &spikes );
  void saveRate( Options &header, const SampleDataD &rate );
  void plot( const EventList &spikes, const SampleDataD &rate, const OutData &signal,
	     const SampleDataF &voltage, const SampleDataF &meanvoltage, int plotmode );
  void analyze( EventList &spikes, SampleDataD &rate );

  int createStimulus( OutData &signal, const Str &file,
		      double &duration, double deltat, bool storesignal );

  void customEvent( QEvent *qce );

  string VUnit;
  string IUnit;
  double IInFac;
  double Amplitude;
  double PeakAmplitude;
  double PeakAmplitudeFac;
  enum WaveForms { File=0, Const, Sine, Rectangular, Triangular,
		   Sawup, Sawdown, Whitenoise, OUnoise, Sweep };
  WaveForms WaveForm;
  double Frequency;
  double DutyCycle;
  int Seed;
  double StartFreq;
  double EndFreq;
  double Ramp;
  double Offset;
  double Duration;
  int Side;
  int Repeats;
  double SkipWin;
  double Sigma;

  string StimulusLabel;
  enum StoreModes { SessionPath, ReProPath, CustomPath };
  enum StoreLevels { All, Generated, Noise, None };
  StoreLevels StoreLevel;
  Str StorePath;
  Str StoreFile;
  double MeanRate;

  MultiPlot SP;
  MultiPlot P;
  QStackedLayout *Stack;

};


}; /* namespace patchclamp */

#endif /* ! _RELACS_PATCHCLAMP_SINGLESTIMULUS_H_ */
