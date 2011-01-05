/*
  patchclampprojects/setleak.cc
  Set leak resistance and reversal potential of a dynamic clamp model.

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

#ifndef _RELACS_PATCHCLAMPPROJECTS_FEEDFORWARDNETWORK_H_
#define _RELACS_PATCHCLAMPPROJECTS_FEEDFORWARDNETWORK_H_ 1

#include <deque>
#include <QPushButton>
#include <relacs/optwidget.h>
#include <relacs/repro.h>
#include <relacs/array.h>
#include <relacs/random.h>
#include <relacs/multiplot.h>
#include <relacs/sampledata.h>
#include <relacs/ephys/traces.h>
using namespace std;
using namespace relacs;

namespace patchclampprojects {


/*!
\class FeedForwardNetwork
\brief [RePro] FFN
\author Jan Benda
\version 1.0 (Feb 20, 2010)
*/


class FeedForwardNetwork : public RePro, public ephys::Traces
{
  Q_OBJECT




public:
  FeedForwardNetwork( void );
  virtual int main( void );
  void saveSettings();
  void saveEvents(const vector<vector<EventData> > &SpikeTimes,const vector<vector<double> > &SignalTimes, Str name);
  void saveTraces(const vector<vector<SampleDataD> > &ge,const vector<vector<SampleDataD> > &gi,const vector<vector<double> > &SignalTimes, double duration, Str name);
  
  int calibrateFFN(double &JeFFN, double &JeBKG, double &gBKG);
  void stimulate(
SampleDataD &ge, SampleDataD &gi, EventData &SpikeTimes, double &signaltime, double duration);
  // actual version
  void PulsePacket(vector<EventData> &SpikeTimes, int alpha, double sigma, int groupsize, double onset);		
  void Poisson(vector<EventData> &SpikeTimes);		
  void MIP(vector<EventData> &SpikeTimes);		
  EventData convergentInput(const vector<EventData> &SpikeTimes,int numberOfRepeats, double delay);
  // plotting
  void rasterplot(const vector<vector<EventData> > &SpikeTimes, int group, int neuron);
  void traceplot(SampleDataD &ge,SampleDataD &gi, int pindex, double duration);
  
  
protected:
	RandomGSL rngNetwork;	
	RandomGSL rngStimulus;
	RandomGSL rngBKG;
	MultiPlot P;
	Options Header;
	
 };
  
  
}; /* namespace patchclampprojects */

#endif /* ! _RELACS_PATCHCLAMPPROJECTS_FEEDFORWARDNETWORK_H_ */

