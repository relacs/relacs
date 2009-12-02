/*
  efish/baselineactivity.h
  ISI statistics and EOD locking of baseline activity.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _RELACS_EFISH_BASELINEACTIVITY_H_
#define _RELACS_EFISH_BASELINEACTIVITY_H_ 1

#include <qmutex.h>
#include <relacs/options.h>
#include <relacs/map.h>
#include <relacs/multiplot.h>
#include <relacs/detector.h>
#include <relacs/ephys/traces.h>
#include <relacs/efield/traces.h>
#include <relacs/efield/eodtools.h>
#include <relacs/repro.h>
using namespace relacs;

namespace efish {


/*!
\class BaselineActivity
\brief [RePro] ISI statistics and EOD locking of baseline activity.
\author Jan Benda
\version 1.0 (Nov 26, 2009)

\par Options
- \b duration (\c number, \e ms): Duration of single sweep.
- \b repeats (\c integer): Maximum number of sweeps (\c repeats=0: infinite).
- \b isimax (\c number, \e ms): Maximum ISI length displayed in the ISIH plot.
- \b isistep (\c number, \e ms): Resolution of the ISI histogram.
- \b ratedt (\c number, \e ms): Resolution of the firing rate.
- \b ratemax (\c number, \e ms): Maximum time for measuring firing frequency for each EOD cycle.
- \b eodduration (\c number, \e ms): Duration of EOD stored into file.
- \b saveeodtimes (\c boolean): Save EOD times in a file.
- \b auto (\c integer): 1) Adjust detector parameter automatically, 2) + reset beat and chirp detector.
- \b adjust (\c boolean): Adjust input gains.

\par Files
PUnitSearch writes some information about baseline activity if \c repeats>0.
- \b basespikes#.dat : the spiketrain of trace #.
- \b baseisih#.dat : the interspike-interval histogram of trace #.
- \b baserate#.dat : the cyclic firing rate for a single EOD period of trace #.
- \b basenerveampl.dat : the nerve potential.
- \b baseeodtrace.dat : \c eodduration ms of the EOD.
- \b baseeodtimes.dat : times of the eod peaks, saved only if \c saveeodtimes is true.

\par Plots
- \b ISI \b Histogram: The interspike-interval histogram. 
      The vertical lines are multiple of the EOD period.
- \b EOD \b Firing \b Rate: Spikes (red) and cyclic firing rate (yellow)
      in comparision with one EOD period (green).

\par Requirements
- Transdermal EOD recording (\c EODTrace2) and events (\c EODEvents2).
- One or more spike events (\c SpikeEvents[*]) or nerve recordings (\c NerveTrace1).
*/


class BaselineActivity
  : public RePro,
    public ephys::Traces,
    public efield::Traces,
    public efield::EODTools
{
  Q_OBJECT

public:

  BaselineActivity( void );
  ~BaselineActivity( void );

  virtual int main( void );
  void stop( void );

    /*! Save all spikes of the whole run of PUnitSearch. */
  void saveSpikes( int trace );
    /*! Save the interspike interval histogram. */
  void saveISIH( int trace );
    /*! Save the cyclic spike rate. */
  void saveRate( int trace );
    /*! Save potential of the nerve recording for the whole run of PUnitSearch. */
  void saveNerve( void );
    /*! Save the fishes EOD waveform. */
  void saveEODTrace( void );
    /*! Save the fishes EOD times. */
  void saveEODTimes( void );
    /*! Save results data. */
  virtual void save( void );

  void plot( void );
  void analyzeSpikes( const EventData &se, int k );
  void analyze( void );


private:

  double Duration;
  int Repeats;

  double FirstSignal;
  double SearchDuration;

  SampleDataD *ISIH[MaxSpikeTraces];
  double ISIMax;
  double ISIStep;
  double FRate[MaxSpikeTraces];
  double CV[MaxSpikeTraces];
  double PValue[MaxSpikeTraces];

  double RateDeltaT;
  double RateTMax;
  SampleDataD *SpikeRate[MaxSpikeTraces];
  int Trials[MaxSpikeTraces];

  EventList Spikes;
  vector< ArrayD > EODSpikes[MaxSpikeTraces];

  MapD NerveAmplP;
  MapD NerveAmplT;
  MapD NerveAmplM;
  Detector< InData::const_iterator, InDataTimeIterator > D;
  AcceptEOD< InData::const_iterator, InDataTimeIterator > NerveAcceptEOD;

  SampleDataD *EODCycle;
  double EODPeriod;
  double EODRate;
  string EOD2Unit;

  EventData EODTimes;
  double EODDuration;
  bool SaveEODTrace;
  bool SaveEODTimes;

  bool Adjust;

  Options Header;

  MultiPlot P;

  int AutoDetect;
  double SpikesFastDelay;
  double SpikesFastDecay;
  double SpikesSlowDelay;
  double SpikesSlowDecay;
  double BeatStep;
  double ChirpMin;
  double ChirpStep;

  int Count;

};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_BASELINEACTIVITY_H_ */
