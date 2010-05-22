/*
  patchclamp/singlestimulus.h
  Output of a single stimulus stored in a file.

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

#ifndef _RELACS_PATCHCLAMP_SINGLESTIMULUS_H_
#define _RELACS_PATCHCLAMP_SINGLESTIMULUS_H_ 1

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

\par Options
- Waveform
- \c waveform=From file: Stimulus waveform (\c string)
- \c stimfile=: Stimulus file (\c string)
- \c stimampl=0: Amplitude factor (standard deviation) of stimulus file (\c number)
- \c amplitude=1nA: Amplitude of stimulus (\c number)
- \c freq=10Hz: Frequency of waveform (\c number)
- \c dutycycle=50%: Duty-cycle of rectangular waveform (\c number)
- \c seed=0: Seed for random number generation (\c integer)
- \c duration=0ms: Maximum duration of stimulus (\c number)
- \c ramp=2ms: Ramp of stimulus (\c number)
- Stimulus
- \c offset=0nA: Stimulus mean (\c number)
- \c offsetbase=absolute: Stimulus mean relative to (\c string)
- \c samerate=true: Use sampling rate of input (\c boolean)
- \c samplerate=1kHz: Sampling rate of output (\c number)
- \c repeats=10times: Number of stimulus presentations (\c number)
- \c pause=1000ms: Duration of pause between stimuli (\c number)
- \c outtrace=V-1: Output trace (\c string)
- Offset - search
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
- Analysis
- \c skipwin=100ms: Initial portion of stimulus not used for analysis (\c number)
- \c sigma1=2ms: Standard deviation of rate smoothing kernel 1 (\c number)
- \c sigma2=20ms: Standard deviation of rate smoothing kernel 2 (\c number)
- \c adjust=true: Adjust input gain (\c boolean)
- Save stimuli
- \c storemode=session: Save stimuli in (\c string)
- \c storepath=: Save stimuli in custom directory (\c string)
- \c storelevel=all: Save (\c string)

\version 1.2 (Oct 13, 2008)
*/


class SingleStimulus : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

    /*! Constructor. */
  SingleStimulus( void );
    /*! Destructor. */
  ~SingleStimulus( void );

  virtual void config( void );
  virtual void notify( void );
  void readConfig( StrQueue &sq );

  virtual int main( void );

  void saveSpikes( Options &header, const EventList &spikes );
  void saveRate( Options &header, const SampleDataD &rate1,
		 const SampleDataD &rate2 );
  void save( const EventList &spikes, const SampleDataD &rate1,
	     const SampleDataD &rate2 );

    /*! Plot data. */
  void plot( const EventList &spikes, const SampleDataD &rate1,
	     const SampleDataD &rate2, const OutData &signal );
    /*! Analyze data. */
  void analyze( EventList &spikes, SampleDataD &rate1, SampleDataD &rate2 );


protected:

  int createStimulus( OutData &signal, const Str &file,
		      double &duration, double deltat, bool storesignal );

  void customEvent( QCustomEvent *qce );

  string AmplitudeUnit;
  double Amplitude;
  double PeakAmplitude;
  double PeakAmplitudeFac;
  enum WaveForms { File=0, Const, Sine, Rectangular, Triangular,
		   Sawup, Sawdown, Whitenoise, OUnoise };
  WaveForms WaveForm;
  double Frequency;
  double DutyCycle;
  int Seed;
  double Ramp;
  double Offset;
  double Duration;
  int Side;
  int Repeats;
  double SkipWin;
  double Sigma1;
  double Sigma2;

  string StimulusLabel;
  enum StoreModes { SessionPath, ReProPath, CustomPath };
  enum StoreLevels { All, Generated, Noise, None };
  StoreLevels StoreLevel;
  Str StorePath;
  Str StoreFile;
  double MeanRate;
  SampleDataD Rate1;
  SampleDataD Rate2;

  MultiPlot SP;
  MultiPlot P;

};


}; /* namespace patchclamp */

#endif /* ! _RELACS_PATCHCLAMP_SINGLESTIMULUS_H_ */
