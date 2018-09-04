/*
  patchclamp/membraneresistance.h
  Measures membrane resistance, capacitance, and time constant with current pulses

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

#ifndef _RELACS_PATCHCLAMP_MEMBRANERESISTANCE_H_
#define _RELACS_PATCHCLAMP_MEMBRANERESISTANCE_H_ 1

#include <deque>
#include <vector>
#include <relacs/sampledata.h>
#include <relacs/plot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace std;
using namespace relacs;

namespace patchclamp {


/*!
\class MembraneResistance
\brief [RePro] Measures membrane resistance, capacitance, and time constant with current pulses
\author Jan Benda
\version 1.6 (Oct 9, 2015)
\par Screenshot
\image html membraneresistance.png

\par Options
- Stimulus
    - \c amplitude=-0.1nA: Amplitude of output signal (\c number)
    - \c duration=500ms: Duration of output (\c number)
    - \c pause=1000ms: Duration of pause bewteen outputs (\c number)
    - \c repeats=100: Repetitions of stimulus (\c integer)
- Analysis
    - \c skipspikes=true: Skip trials with detected spikes (\c boolean)
    - \c sswidth=100ms: Window length for steady-state analysis (\c number)
    - \c nossfit=false: Fix steady-state potential for fit (\c boolean)
    - \c plotstdev=true: Plot standard deviation of membrane potential (\c boolean)
    - \c setdata=rest only: Set results to the session variables (\c string)
    - \c checkoutput=Current-1: Outputs that need to be at their default value (\c string)
*/


class MembraneResistance : public RePro, public ephys::Traces
{
  Q_OBJECT

  friend Ramp;

public:

  MembraneResistance( void );
  virtual void preConfig( void );
  virtual int main( void );
  void analyzeOn( double duration, double sswidth, bool nossfit );
  void analyzeOff( double duration, double sswidth, bool nossfit );
  void plot( void );
  void save( void );
  void saveData( void );
  void saveTrace( const Options &header );
  void saveExpFit( const Options &header );


protected:

  Plot P;
  string VUnit;
  string IUnit;
  double VFac;
  double IFac;
  double IInFac;
  SampleDataD MeanVoltage;
  SampleDataD SquareVoltage;
  SampleDataD StdevVoltage;
  deque< int > TraceIndices;
  deque< SampleDataD > MeanTraces;
  deque< SampleDataD > SquareTraces;
  double DCCurrent;
  double Amplitude;
  double Duration;
  double VRest;
  double VRestsd;
  double VSS;
  double VSSsd;
  double VPeak;
  double VPeaksd;
  int VPeakInx;
  double VPeakTime;
  double RMss;
  double RMOn;
  double CMOn;
  double TauMOn;
  double RMOff;
  double CMOff;
  double TauMOff;
  SampleDataF ExpOn;
  SampleDataF ExpOff;
  int Count;
  vector< string > OutParams;

};


}; /* namespace patchclamp */

#endif /* ! _RELACS_PATCHCLAMP_MEMBRANERESISTANCE_H_ */
