/*
  efish/sam.h
  Measures responses to sinusoidal amplitude modulations.

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
\bug Using EOD waveforms is broken (sampling rate!)
\version 2.2 (Nov 11, 2014)

\par Options
- \b duration (\c number, \e ms): Duration of stimulus.
- \b pause (\c number, \e ms): %Pause between successive stimuli.
- \b deltaf (\c number, \e Hz): Beat frequency (frequency of the SAM).
- \b contrast (\c number, \e %): Contrast (AM amplitude / EOD amplitude)
- \b repeats (\c integer): Number of stimulus repetitions (0: infinite).
- \b am (\c boolean): AM stimulus or direct stimulus.
- \b sinewave (\c boolean): If direct stimulus: use sine wave or the fishes EOD.
- \b skip (\c number ): Number of initial and trailing beat cycles that are skipped for the analysis.
- \b ratebins (\c integer ): Number of bins for the firing rate per beat cycle.
- \b before (\c number, \e ms): Spikes recorded before stimulus.
- \b after (\c number, \e ms): Spikes recorded after stimulus.

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
  double Ampl1;
  double Phase1;
  double Ampl2;
  double Phase2;
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
