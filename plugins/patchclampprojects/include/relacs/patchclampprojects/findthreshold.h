/*
  patchclampprojects/findthreshold.h
  Finds the current threshold.

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

#ifndef _RELACS_PATCHCLAMPPROJECTS_FINDTHRESHOLD_H_
#define _RELACS_PATCHCLAMPPROJECTS_FINDTHRESHOLD_H_ 1

#include <deque>
#include <relacs/array.h>
#include <relacs/sampledata.h>
#include <relacs/tablekey.h>
#include <relacs/plot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace patchclampprojects {


/*!
\class FindThreshold
\brief [RePro] Finds the current threshold.
\author Jan Benda
\version 1.0 (Feb 08, 2010)
\par Options
- Stimuli
- \c amplitudesrc=custom: Use initial amplitude from (\c string)
- \c startamplitude=0nA: Initial amplitude of current stimulus (\c number)
- \c startamplitudestep=0.5nA: Initial size of amplitude steps used for searching threshold (\c number)
- \c amplitudestep=0.01nA: Final size of amplitude steps used for oscillating around threshold (\c number)
- \c minspikecount=1: Minimum required spike count for each trial (\c number)
- \c resetcurrent=false: Reset current to zero after each stimulus (\c boolean)
- Timing
- \c durationsel=in milliseconds: Set duration of stimulus (\c string)
- \c duration=1000ms: Duration of stimulus (\c number)
- \c durationfac=1tau_m: Duration of stimulus (\c number)
- \c searchpause=10ms: Duration of pause between outputs during search (\c number)
- \c pause=10ms: Duration of pause between outputs (\c number)
- \c savetime=1000ms: Length of trace to be saved and analyzed (\c number)
- \c skiptime=100ms: Initial time skipped from spike-count analysis (\c number)
- \c repeats=100: Repetitions of stimulus (\c integer)
*/


  class FindThreshold : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  FindThreshold( void );
  virtual int main( void );
  virtual void preConfig( void );
  void analyze( double amplitude, double duration,
		double savetime, double skiptime );
  void plot( bool record, double duration );
  void openFiles( ofstream &tf, TableKey &tracekey );
  void saveTrace( ofstream &tf, TableKey &tracekey, int index );
  void save( void );
  void saveSpikes( void );
  void saveData( void );


protected:

  Plot P;
  string VUnit;
  string IUnit;
  double IInFac;

  struct Data {
    Data( double savetime,
	  const InData &voltage, const InData &current );
    Data( double savetime,
	  const InData &voltage );
    double Amplitude;
    SampleDataF Voltage;
    SampleDataF Current;
    EventData Spikes;
    int SpikeCount;
  };
  deque< Data > Results;

  int SpikeCount;
  int TrialCount;
  ArrayD Amplitudes;
  ArrayD Latencies;
  EventList Spikes;

  Options Header;

};


}; /* namespace patchclampprojects */

#endif /* ! _RELACS_PATCHCLAMPPROJECTS_FINDTHRESHOLD_H_ */
