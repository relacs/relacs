/*
  patchclampprojects/voltagereconstruction.h
  Reconstructs the membrane voltage inbetween the spikes from latency measurements.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_PATCHCLAMPPROJECTS_VOLTAGERECONSTRUCTION_H_
#define _RELACS_PATCHCLAMPPROJECTS_VOLTAGERECONSTRUCTION_H_ 1

#include <QStackedLayout>
#include <relacs/plot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace patchclampprojects {


/*!
\class VoltageReconstruction
\brief [RePro] Reconstructs the membrane voltage inbetween the spikes from latency measurements.
\author JanBenda, Ales Skorjanc
\version 1.0 (Oct 18, 2010)
\par Options
- \c startamplitudesrc=custom: Set initial dc-current to (\c string)
- \c startamplitude=0.1nA: Initial amplitude of dc-current (\c number)
- \c amplitude=12nA: Test-pulse amplitude (\c number)
- \c duration=10ms: Duration of test-pulse (\c number)
- \c repeats=100: Number of test-pulses (\c integer)
- Rate - search
- \c userate=true: Search dc-current for target firing rate (\c boolean)
- \c rate=20Hz: Target firing rate (\c number)
- \c ratetol=1Hz: Tolerance for target firing rate (\c number)
- \c amplitudestep=4nA: Initial size of dc-current steps used for searching target rate (\c number)
- \c searchduration=500ms: Duration of dc-current stimulus (\c number)
- \c skipwin=100ms: Initial portion of stimulus not used for analysis (\c number)
*/


class VoltageReconstruction : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  VoltageReconstruction( void );
  virtual void config( void );
  virtual int main( void );


protected:

  void customEvent( QEvent *qce );

  void openTraceFile( ofstream &tf, TableKey &tracekey, const Options &header );
  void saveTrace( ofstream &tf, TableKey &tracekey, int index,
		  const SampleDataF &voltage, const SampleDataF &current,
		  double x, double y );
  void saveMeanTrace( const Options &header, const SampleDataF &voltage,
		      const SampleDataF &voltagesd );
  void saveData( const Options &header, const MapD &latencies );

  Plot SP;
  Plot P;
  QStackedLayout *Stack;

  string VUnit;
  string IUnit;
  double IInFac;

  double PrevDCAmplitude;

};


}; /* namespace patchclampprojects */

#endif /* ! _RELACS_PATCHCLAMPPROJECTS_VOLTAGERECONSTRUCTION_H_ */
