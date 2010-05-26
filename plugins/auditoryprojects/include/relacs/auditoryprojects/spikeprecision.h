/*
  auditoryprojects/spikeprecision.h
  Assess spike precision in locust auditory receptors

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

#ifndef _RELACS_AUDITORYPROJECTS_SPIKEPRECISION_H_
#define _RELACS_AUDITORYPROJECTS_SPIKEPRECISION_H_ 1

#include <QStackedLayout>
#include <relacs/sampledata.h>
#include <relacs/multiplot.h>
#include <relacs/rangeloop.h>
#include <relacs/ephys/traces.h>
#include <relacs/acoustic/traces.h>
#include <relacs/repro.h>
using namespace relacs;

namespace auditoryprojects {


/*! 
\class SpikePrecision
\brief [RePro] Assess spike precision in locust auditory receptors
\author Samuel Glauser
\author Jan Benda

\par Options
- Stimulus
- \c freqrange=: Frequency range (\c string)
- \c amplitude=4dB: Amplitude of envelope (\c number)
- \c intensity=80dB SPL: Stimulus intensity (\c number)
- \c intensitybase=SPL: Stimulus intensity relative to (\c string)
- \c repeats=10times: Number of stimulus presentations (\c number)
- \c duration=500ms: Duration of stimulus (\c number)
- \c pause=1000ms: Duration of pause between stimuli (\c number)
- \c carrierfreq=5kHz: Carrier frequency (\c number)
- \c usebestfreq=true: Use the cell's best frequency (\c boolean)
- \c side=left: Speaker (\c string)
- Waveform
- \c waveform=sine: Type of amplitude modulation (\c string)
- \c dutycycle=50%: Duty-cycle of rectangular waveform (\c number)
- \c stimfile=: Stimulus file (\c string)
- \c stimampl=0: Amplitude factor (standard deviation) of stimulus file (\c number)
- \c relfreqgap=20%: Relative width of frequency gap (\c number)
- \c absfreqgap=10Hz: Absolute width of frequency gap (\c number)
- \c rescale=true: Rescale filtered noise-gap stimuli (\c boolean)
- \c ramp2=0ms: Ramp for rectangles and saw tooths (\c number)
- \c ramp=2ms: Ramp at beginning and end of stimulus (\c number)
- Intensity - search
- \c userate=true: Search intensity for target firing rate (\c boolean)
- \c rate=100Hz: Target firing rate (\c number)
- \c ratetol=5Hz: Tolerance for target firing rate (\c number)
- \c intensitystep=8dB: Initial intensity step (\c number)
- \c searchrepeats=2times: Number of search stimulus presentations (\c number)
- Analysis
- \c skipwin=200ms: Initial portion of stimulus not used for analysis (\c number)
- \c sigma1=1ms: Standard deviation of rate smoothing kernel 1 (\c number)
- \c sigma2=20ms: Standard deviation of rate smoothing kernel 2 (\c number)
- Save stimuli
- \c storemode=session: Save stimuli in (\c string)
- \c storepath=: Save stimuli in custom directory (\c string)

\version 1.5 (Jan 10, 2008)
*/


class SpikePrecision : public RePro, public ephys::Traces, public acoustic::Traces
{
  Q_OBJECT

public:

    /*! Constructs a SpikePrecision-RePro: intialize widgets and create options. */
  SpikePrecision( void );
    /*! Destructs a SpikePrecision-RePro. */
  ~SpikePrecision( void );

  virtual int main( void );


protected:

  struct EnvelopeFrequencyData
  {
    EnvelopeFrequencyData( double duration, double deltat )
      : Rate1( 0.0, duration, deltat, 0.0 ), Rate2( 0.0, duration, deltat, 0.0 ) {};
      EventList Spikes;
      double Frequency;
      double Intensity;
      double SSRate;
      double Correlation;
      string Envelope;
      SampleDataD Rate1;
      SampleDataD Rate2;
  };

  void analyze( vector < EnvelopeFrequencyData > &results );
  void plot( const SampleDataD &amdb,
	     const vector < EnvelopeFrequencyData > &results );
  void saveSpikes( const vector < EnvelopeFrequencyData > &results );
  void saveRates( const vector < EnvelopeFrequencyData > &results );
  void save( const vector < EnvelopeFrequencyData > &results );

  int createStimulus( OutData &signal, SampleDataD &amdb,
		      double frequency, const Str &file, bool store=true );

  void customEvent( QEvent *qce );

  double CarrierFrequency;
  double Amplitude;
  double PeakAmplitude;
  double PeakAmplitudeFac;
  RangeLoop FreqRange;
  double Frequency;
  enum WaveForms { Sine, Rectangular, Triangular,
		   Sawup, Sawdown, Noisegap, Noisecutoff };
  WaveForms WaveForm;
  double DutyCycle;
  double RelFreqGap;
  double AbsFreqGap;
  bool Rescale;
  double Ramp;
  double Ramp2;
  double Intensity;
  double Duration;
  int StimRepetition;
  int Side;
  double SkipWindow;
  double Sigma1;
  double Sigma2;
  enum StoreModes { SessionPath, ReProPath, CustomPath };
  Str StorePath;
  Str StoreFile;

  MultiPlot SP;
  MultiPlot P;
  QStackedLayout *Stack;

  string StimulusLabel;

};


}; /* namespace auditoryprojects */

#endif /* ! _RELACS_AUDITORYPROJECTS_SPIKEPRECISION_H_ */
