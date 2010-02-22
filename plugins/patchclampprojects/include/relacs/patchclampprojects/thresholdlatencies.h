/*
  patchclampprojects/thresholdlatencies.h
  Measures spike latencies in response to pulses close to the firing threshold.

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

#ifndef _RELACS_PATCHCLAMPPROJECTS_THRESHOLDLATENCIES_H_
#define _RELACS_PATCHCLAMPPROJECTS_THRESHOLDLATENCIES_H_ 1

#include <deque>
#include <relacs/array.h>
#include <relacs/sampledata.h>
#include <relacs/eventlist.h>
#include <relacs/tablekey.h>
#include <relacs/plot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace patchclampprojects {


/*!
\class ThresholdLatencies
\brief [RePro] Measures spike latencies in response to pulses close to the firing threshold.
\author Jan Benda
\version 1.0 (Feb 04, 2010)
*/


class ThresholdLatencies : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  ThresholdLatencies( void );
  virtual int main( void );
  virtual void config( void );
  virtual void notify( void );
  void analyze( int involtage, int incurrent,
		double amplitude, double dcamplitude,
		double delay, double duration, double savetime, double pause );
  void plot( bool record, double duration );
  void openTraceFile( ofstream &tf, TableKey &tracekey, int incurrent );
  void saveTrace( ofstream &tf, TableKey &tracekey, int index );
  void save( bool dc );
  void saveSpikes( void );
  void saveData( bool dc );


protected:

  Plot P;
  string VUnit;
  string IUnit;
  double IInFac;

  struct Data {
    Data( double delay, double savetime,
	  const InData &voltage, const InData &current );
    Data( double delay, double savetime,
	  const InData &voltage );
    double DCAmplitude;
    double Amplitude;
    SampleDataF Voltage;
    SampleDataF Current;
    EventData Spikes;
    int SpikeCount;
    int BaseSpikeCount;
  };
  deque< Data > Results;

  int SpikeCount;
  int TrialCount;
  ArrayD Amplitudes;
  ArrayD DCAmplitudes;
  ArrayD Latencies;
  ArrayI SpikeCounts;
  EventList Spikes;

  Options Header;

};


}; /* namespace patchclampprojects */

#endif /* ! _RELACS_PATCHCLAMPPROJECTS_THRESHOLDLATENCIES_H_ */
