/*
  patchclamp/membraneresistance.h
  Measures membrane resistance, capacitance, and time constant with current pulses

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

#ifndef _RELACS_PATCHCLAMP_MEMBRANERESISTANCE_H_
#define _RELACS_PATCHCLAMP_MEMBRANERESISTANCE_H_ 1

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
\version 1.0 (Nov 12, 2009)
*/


class MembraneResistance : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  MembraneResistance( void );
  virtual void config( void );
  virtual void notify( void );
  virtual int main( void );
  void analyzeOn( int involtage, int incurrent, double duration, int count,
		  double sswidth, bool nossfit );
  void analyzeOff( int involtage, int incurrent, double duration, int count,
		   double sswidth, bool nossfit );
  void plot( void );
  void save( void );
  void saveData( void );
  void saveTrace( void );
  void saveExpFit( void );


protected:

  Plot P;
  string VUnit;
  string IUnit;
  double VFac;
  double IFac;
  double IInFac;
  SampleDataD MeanTrace;
  SampleDataD SquareTrace;
  SampleDataD StdevTrace;
  SampleDataD MeanCurrent;
  double DCCurrent;
  double Amplitude;
  double TrueAmplitude;
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
  SampleDataD ExpOn;
  SampleDataD ExpOff;

};


}; /* namespace patchclamp */

#endif /* ! _RELACS_PATCHCLAMP_MEMBRANERESISTANCE_H_ */
