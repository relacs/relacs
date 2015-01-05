/*
  efish/sam.h
  Measures responses to sinusoidal amplitude modulations.

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

#ifndef _RELACS_EFISH_SAM_H_
#define _RELACS_EFISH_SAM_H_ 1

#include <vector>
#include <relacs/map.h>
#include <relacs/sampledata.h>
#include <relacs/options.h>
#include <relacs/tablekey.h>
#include <relacs/multiplot.h>
#include <relacs/ephys/traces.h>
#include <relacs/efield/traces.h>
#include <relacs/efield/eodtools.h>
#include <relacs/repro.h>
using namespace relacs;

namespace efish {


/*!
\class SAM
\brief [RePro] Measures responses to sinusoidal amplitude modulations.
\author Jan Benda
\version 2.4 (Dec 10, 2014)

\par Options
\par Options
- \c Stimulus
    - \c duration=1000ms: Duration of signal (\c number)
    - \c pause=1000ms: Pause between signals (\c number)
    - \c freqsel=relative to EOD: Stimulus frequency is (\c string)
    - \c deltaf=5Hz: Delta f (beat frequency) (\c number)
    - \c contrast=20%: Contrast (\c number)
    - \c repeats=6: Repeats (\c integer)
    - \c am=true: Amplitude modulation (\c boolean)
    - \c sinewave=true: Use sine wave (\c boolean)
    - \c ampl=0.0: Relative amplitude of harmonics (\c string)
    - \c phase=0.0: Phase of harmonics (\c string)
    - \c contrastsel=fundamental: Contrast is (\c string)
- \c Analysis
    - \c skip=0.5Periods: Skip (\c number)
    - \c ratebins=10: Number of bins for firing rate (\c integer)
    - \c before=0ms: Spikes recorded before stimulus (\c number)
    - \c after=0ms: Spikes recorded after stimulus (\c number)
    - \c adjust=true: Adjust input gain? (\c boolean)

Harmonics can be defined by \c ampl and \c phase.
Enter the relative amplitude and the corresponding phase of each harmonic
as comma separated values.

\par Files
- \b samspikes#.dat : the spikes elicited by each SAM period of trace #.
- \b samallspikes#.dat : the spikes elicited by each SAM stimulus of trace #.
- \b samnerveampl.dat : the nerve potential elicited by each SAM period.
- \b samnervesmoothampl.dat : the smoothed nerve potential elicited by each SAM period.
- \b samallnerveampl.dat : the nerve potential elicited by each SAM stimulus.
- \b samallnervesmoothampl.dat : the smoothed nerve potential elicited by each SAM stimulus.
- \b samampl.dat : the SAM amplitudes for each SAM period.
- \b samallampl.dat : the SAM amplitude for each stimulus.
- \b samrate#.dat : the cyclic firing rate for a single SAM period of trace #.

\par Plots
- \b Firing \b Rate: Spikes (red) and cyclic firing rate (yellow) 
  for each SAM cycle.
- \b SAM: Each period of the SAM stimulus (darkgreen). Last period green.

\par Requirements
- Transdermal EOD recording (\c EODTrace2) and events (\c EODEvents2).
- Recording of the stimulus events (\c SignalEvents1) if non AM stimuli are used.
- One or more spike events (\c SpikeEvents[*]) or nerve recordings (\c NerveTrace1).
*/


class SAM
  : public RePro,
    public ephys::Traces,
    public efield::Traces,
    public efield::EODTools
{
  Q_OBJECT

public:

  SAM( void );
  virtual int main( void );
  void stop( void );

  void saveRate( int trace );
  void saveAmpl( void );
  void saveAllAmpl( void );
  void saveSpikes( int trace );
  void saveAllSpikes( int trace );
  void saveNerve( void );
  void saveAllNerve( void );
    /*! Save results data. */
  virtual void save( void );

    /*! Plot data. */
  void plot( void );
    /*! Analyze spikes. */
  void analyzeSpikes( const EventData &se, int k, const EventData &beattimes );
    /*! Analyze data. */
  void analyze( void );


private:

  int createSignal( const InData &data, const EventData &events );

  // parameter:
  int ReadCycles;
  double Duration;
  double Pause;
  bool FreqAbs;
  double DeltaF;
  double Contrast;
  vector< double > HarmonicAmpls;
  vector< double > HarmonicPhases;
  bool ContrastFundamental;
  int Repeats;
  bool AM;
  bool SineWave;
  double Skip;
  int RateN;
  double Before;
  double After;

  // variables:
  OutData* Signal;
  double IntensityGain;
  double FishAmplitude;
  double FishRate;
  double TrueDeltaF;
  double TrueContrast;
  double Intensity;
  int Count;

  string EOD2Unit;
  double Period;
  vector< MapD > EODTransAmpl;
  MapD AllEODTransAmpl;
  vector < vector < double > > Spikes[MaxTraces];
  vector < double > AllSpikes[MaxTraces];
  double RateDeltaT;
  SampleDataD *SpikeRate[MaxTraces];
  SampleDataD *SpikeFrequency[MaxTraces];
  int Trials[MaxTraces];
  double MaxRate[MaxTraces];

  int Offset;  
  vector< MapD > NerveAmplP;
  vector< MapD > NerveAmplT;
  vector< MapD > NerveAmplM;
  vector< SampleDataD > NerveAmplS;
  SampleDataD NerveMeanAmplP;
  SampleDataD NerveMeanAmplT;
  SampleDataD NerveMeanAmplM;
  SampleDataD NerveMeanAmplS;
  MapD AllNerveAmplP;
  MapD AllNerveAmplT;
  MapD AllNerveAmplM;
  SampleDataD AllNerveAmplS;
  AcceptEOD<InData::const_iterator,InDataTimeIterator> NerveAcceptEOD;

  Options Header;
  TableKey SpikesKey;
  TableKey NerveKey;
  TableKey SmoothKey;
  TableKey AmplKey;
  
  MultiPlot P;

};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_SAM_H_ */
