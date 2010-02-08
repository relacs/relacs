/*
  patchclampprojects/findthreshold.h
  Finds the current threshold.

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
*/


  class FindThreshold : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  FindThreshold( void );
  virtual int main( void );
  virtual void config( void );
  virtual void notify( void );
  void analyze( int involtage, int incurrent,
		double amplitude, double duration, double savetime );
  void plot( double duration );
  void openFiles( ofstream &tf, TableKey &tracekey,
		  ofstream &sf, TableKey &spikekey, int incurrent );
  void saveTrace( ofstream &tf, TableKey &tracekey, int index );
  void saveSpikes( ofstream &sf, TableKey &spikekey, int index );
  void saveData( bool dc );


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

};


}; /* namespace patchclampprojects */

#endif /* ! _RELACS_PATCHCLAMPPROJECTS_FINDTHRESHOLD_H_ */
