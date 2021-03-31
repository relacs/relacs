/*
  ephys/updatedcparameters.h
  short pulse to estimate membrane parameters, updates dynclamp parameters accordingly

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

#ifndef _RELACS_EPHYS_UPDATEDCPARAMETERS_H_
#define _RELACS_EPHYS_UPDATEDCPARAMETERS_H_ 1

#include <deque>
#include <vector>
#include <relacs/sampledata.h>
#include <relacs/plot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace std;
using namespace relacs;
using namespace relacs;

namespace ephys {

class UpdateDCParameters : public RePro, public ephys::Traces
{

  Q_OBJECT

//  friend Ramp;

public:

  UpdateDCParameters( void );
  virtual void preConfig( void );
  virtual int main( void );
  void analyzeOn( double duration, double sswidth, bool nossfit );
  void analyzeOff( double duration, double sswidth, bool nossfit );
//    void plot( void );
  void save( void );
  void saveData( void );
  void saveTrace( const Options &header );
  void saveExpFit( const Options &header );


protected:

//    Plot P;
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
  double EM;
  SampleDataF ExpOn;
  SampleDataF ExpOff;
  int Count;
  vector< string > CheckOutParams;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_UPDATEDCPARAMETERS_H_ */
