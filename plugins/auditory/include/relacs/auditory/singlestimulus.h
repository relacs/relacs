/*
  auditory/singlestimulus.h
  Output of a single stimulus stored in a file.

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

#ifndef _RELACS_AUDITORY_SINGLESTIMULUS_H_
#define _RELACS_AUDITORY_SINGLESTIMULUS_H_ 1

#include <QStackedLayout>
#include <relacs/eventlist.h>
#include <relacs/sampledata.h>
#include <relacs/multiplot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
#include <relacs/acoustic/traces.h>
using namespace relacs;

namespace auditory {


/*! 
\class SingleStimulus
\brief [RePro] Output of a single stimulus stored in a file.
\version 1.6 (Jul 11, 2015)
\author Jan Benda
\par Options
- \c Waveform
    - \c type=Wave: Type of stimulus (\c string)
    - \c waveform=From file: Stimulus waveform (\c string)
    - \c stimfile=: Stimulus file (\c string)
    - \c stimscale=false: Scale stimulus to maximum amplitudes (\c boolean)
    - \c stimhighcut=0Hz: Cutoff frequency of high-pass filter applied to stimulus (\c number)
    - \c stimampl=0: Amplitude factor (standard deviation) of stimulus file (\c number)
    - \c amplitude=1dB: Amplitude of stimulus (\c number)
    - \c freqsel=frequency: Specify (\c string)
    - \c freq=10Hz: Frequency of waveform (\c number)
    - \c period=100ms: Period of waveform (\c number)
    - \c numperiods=1: Number of periods (\c number)
    - \c pulsesel=pulse duration: Specify (\c string)
    - \c pulseduration=10ms: Pulse duration (\c number)
    - \c dutycycle=50%: Duty-cycle (\c number)
    - \c seed=0: Seed for random number generation (\c integer)
    - \c duration=0ms: Maximum duration of stimulus (\c number)
    - \c ramp=2ms: Ramp of stimulus (\c number)
- \c Stimulus
    - \c intensity=50dB: Stimulus intensity (\c number)
    - \c intensitybase=SPL: Stimulus intensity relative to (\c string)
    - \c repeats=10times: Number of stimulus presentations (\c number)
    - \c pause=1000ms: Duration of pause between stimuli (\c number)
    - \c before=100ms: Time before stimulus to be analyzed (\c number)
    - \c after=100ms: Time after stimulus to be analyzed (\c number)
    - \c side=left: Speaker (\c string)
    - \c Carrier
        - \c carrierfreq=5kHz: Carrier frequency (\c number)
        - \c usebestfreq=true: Relative to the cell's best frequency (\c boolean)
- \c Intensity - search
    - \c userate=true: Search intensity for target firing rate (\c boolean)
    - \c rate=100Hz: Target firing rate (\c number)
    - \c ratetol=5Hz: Tolerance for target firing rate (\c number)
    - \c intensitystep=8dB: Initial intensity step (\c number)
    - \c searchrepeats=2times: Number of search stimulus presentations (\c number)
    - \c silentrate=0Hz: Ignore response below (\c number)
    - \c maxsilent=1: Number of stimulus presentations if response is below silentrate (\c integer)
    - \c skippause=true: Skip pause if response is below silentrate (\c boolean)
    - \c maxsearch=1intensities: Stop search if response does not change for more than (\c integer)
    - \c method=Bisect: Method (\c string)
    - \c minslope=4Hz/dB: Minimum slope required for interpolation (\c number)
    - \c searchduration=0ms: Maximum duration of stimulus (\c number)
    - \c searchpause=0ms: Duration of pause between stimuli (\c number)
- \c Analysis
    - \c skipwin=100ms: Initial portion of stimulus not used for analysis (\c number)
    - \c sigma1=2ms: Standard deviation of rate smoothing kernel 1 (\c number)
    - \c sigma2=20ms: Standard deviation of rate smoothing kernel 2 (\c number)
    - \c storevoltage=false: Save voltage trace (\c boolean)
    - \c plot=Current voltage trace: Plot shows (\c string)
    - \c adjust=true: Adjust input gain (\c boolean)
    - \c Save stimuli
        - \c storemode=session: Save stimuli in (\c string)
        - \c storepath=: Save stimuli in custom directory (\c string)
        - \c storelevel=all: Save (\c string)
*/


class SingleStimulus : public RePro, public ephys::Traces, public acoustic::Traces
{
  Q_OBJECT

public:

    /*! Constructor. */
  SingleStimulus( void );
    /*! Destructor. */
  ~SingleStimulus( void );

  virtual void preConfig( void );
  virtual int main( void );

  void openTraceFile( ofstream &tf, TableKey &tracekey, const Options &header );
  void saveTrace( ofstream &tf, TableKey &tracekey, int index,
		  const SampleDataF &voltage );
  void saveMeanTrace( Options &header, const SampleDataF &meanvoltage );
  void saveSpikes( Options &header, const EventList &spikes );
  void saveRate( Options &header, const SampleDataD &rate1,
		 const SampleDataD &rate2 );

    /*! Plot data. */
  void plot( const EventList &spikes, const SampleDataD &rate1,
	     const SampleDataD &rate2, const SampleDataF &voltage,
	     const SampleDataF &meanvoltage );
    /*! Analyze data. */
  void analyze( EventList &spikes, SampleDataD &rate1, SampleDataD &rate2,
		double before, double after );


public slots:

    /*! This slot is called whenever a value in the dialog is changed.
        \a p contains the name of the corresponding Parameter and the
	new value from the widget. */
  virtual void notifyDialog( const Parameter &p );


protected:

  int createStimulus( OutData &signal, const Str &file,
		      double &duration, bool stoream );

  void customEvent( QEvent *qce );

  double CarrierFreq;
  double Amplitude;
  double PeakAmplitude;
  double PeakAmplitudeFac;
  bool StimScale;
  double StimHighCut;
  enum WaveTypes { Wave=0, Envelope, AM };
  WaveTypes WaveType;
  enum WaveForms { File=0, Const, Sine, Rectangular, Triangular,
		   Sawup, Sawdown, Whitenoise, OUnoise };
  WaveForms WaveForm;
  double Frequency;
  double PulseDuration;
  double DutyCycle;
  int Seed;
  double Ramp;
  double Intensity;
  double Duration;
  int Side;
  int Repeats;
  double SkipWin;
  double Sigma1;
  double Sigma2;

  OutData Signal;
  string StimulusLabel;
  enum StoreModes { SessionPath, ReProPath, CustomPath };
  enum StoreLevels { All, AMGenerated, Generated, Noise, None };
  StoreLevels StoreLevel;
  Str StorePath;
  Str StoreFile;
  OutData AMDB;
  double MeanRate;
  SampleDataD Rate1;
  SampleDataD Rate2;

  string VUnit;

  MultiPlot SP;
  MultiPlot P;
  QStackedLayout *Stack;

};


}; /* namespace auditory */

#endif /* ! _RELACS_AUDITORY_SINGLESTIMULUS_H_ */
