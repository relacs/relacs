/*
  efish/filestimulus.h
  Load a stimulus from a text file.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_EFISH_FILESTIMULUS_H_
#define _RELACS_EFISH_FILESTIMULUS_H_ 1

#include <vector>
#include <relacs/str.h>
#include <relacs/options.h>
#include <relacs/tablekey.h>
#include <relacs/map.h>
#include <relacs/sampledata.h>
#include <relacs/multiplot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
#include <relacs/efield/traces.h>
#include <relacs/efield/eodtools.h>
using namespace relacs;

namespace efish {


/*!
\class FileStimulus
\brief [RePro] Load a stimulus from a text file.
\author Jan Benda
\version 1.3 (Mar 25, 2010)

\par Options
- \b filepath (\c string): The directory of the stimulus file.
- \b filename (\c string): The stimulus file.
- \b sigstdev (\c number): The standard deviation of the stimulus.
- \b pause (\c number, \e ms): %Pause between successive stimuli.
- \b contrast (\c number, \e %): Contrast for one standard deviation of the stimulus.
- \b repeats (\c integer): Number of stimulus repetitions (0: infinite).
- \b binwidth (\c number, \e ms): Width of the bins used for estimating the firing rate.
- \b before (\c number, \e ms): Spikes recorded before stimulus.
- \b after (\c number, \e ms): Spikes recorded after stimulus.
- \b splitfiles (\c boolean ): Each stimulus in extra file.
- \b joinspikes (\c boolean ): Add spike trains to stimulus amplitude file.

\par Files
- \b stimspikes#.dat : the spikes elicited by each stimulus of trace #.
- \b stimnerveampl.dat : the nerve potential elicited b each stimulus.
- \b stimampl.dat : each stimulus trial.
- \b stimrate.dat : the firing rate for each stimulus.

\par Plots
- \b Firing \b Rate: Spikes (red) and firing rate (yellow).
- \b Stimulus: The stimuli (darkgreen, the most recent one green).

\par Requirements
- Transdermal EOD recording (\c EODTrace2) and events (\c EODEvents2).
- One or more spike events (\c SpikeEvents[*]) or nerve recordings (\c NerveTrace1).
*/


class FileStimulus
  : public RePro,
    public ephys::Traces,
    public efield::Traces,
    public efield::EODTools
{
  Q_OBJECT

public:

  FileStimulus( void );
  virtual int main( void );

  void stop( void );
  void saveRate( int trace );
  void saveAmpl( void );
  void saveNerve( void );
  void saveSpikes( int trace );
    /*! Save results data. */
  void save( void );

    /*! Plot data. */
  void plot( void );
    /*! Analyze spikes. */
  void analyzeSpikes( const EventData &se, int k );
    /*! Analyze data. */
  void analyze( void );


private:

  // parameter:
  double SigStdev;
  double Pause;
  bool UseContrast;
  double Contrast;
  double Amplitude;
  bool AM;
  int Repeats;
  double RateDeltaT;
  double Before;
  double After;

  // variables:
  double Duration;
  double FishAmplitude;
  double FishRate;
  double TrueContrast;
  double Intensity;
  int Count;

  Str File;
  string LocalEODUnit;
  vector< MapD > EODTransAmpl;
  vector< SampleDataF > EFieldAmpl;
  EventList Spikes[MaxSpikeTraces];
  SampleDataD SpikeRate[MaxSpikeTraces];
  int Trials[MaxSpikeTraces];
  double MaxRate[MaxSpikeTraces];
  vector< MapD > NerveAmplP;
  vector< MapD > NerveAmplT;
  vector< MapD > NerveAmplM;
  SampleDataD NerveMeanAmplP;
  SampleDataD NerveMeanAmplT;
  SampleDataD NerveMeanAmplM;
  AcceptEOD< InDataIterator, InDataTimeIterator > NerveAcceptEOD;

  Options Header;
  TableKey SpikesKey;
  TableKey NerveKey;
  TableKey AmplKey;
  TableKey EFieldKey;
  
  MultiPlot P;

};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_FILESTIMULUS_H_ */
