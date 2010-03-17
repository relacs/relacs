/*
  patchclamp/ficurve.h
  f-I curve measured in current-clamp

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

#ifndef _RELACS_PATCHCLAMP_FICURVE_H_
#define _RELACS_PATCHCLAMP_FICURVE_H_ 1

#include <deque>
#include <relacs/sampledata.h>
#include <relacs/eventlist.h>
#include <relacs/multiplot.h>
#include <relacs/rangeloop.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace patchclamp {


/*!
\class FICurve
\brief [RePro] F-I curve measured in current-clamp
\author Jan Benda
\version 1.0 (Feb 17, 2010)
*/


class FICurve : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  FICurve( void );
  virtual void config( void );
  virtual void notify( void );
  virtual int main( void );
  void plot( double duration, int inx );
  void save( void );
  void saveData( void );
  void saveRate( void );
  void saveSpikes( void );
  void saveTraces( void );


protected:

  MultiPlot P;
  string VUnit;
  string IUnit;
  double VFac;
  double IFac;
  double IInFac;

  struct Data
  {
    Data( void );
    void analyze( int count, const InData &intrace,
		  const EventData &spikes, const InData *incurrent,
		  double iinfac, double delay,
		  double duration, double sswidth );
    double DC;
    double I;
    double VRest;
    double VRestSQ;
    double VRestSD;
    double VSS;
    double VSSSQ;
    double VSSSD;
    double PreRate;
    double PreRateSD;
    double SSRate;
    double SSRateSD;
    double MeanRate;
    double MeanRateSD;
    double OnRate;
    double OnRateSD;
    double OnTime;
    double Latency;
    double LatencySD;
    double SpikeCount;
    double SpikeCountSD;
    SampleDataD MeanCurrent;
    SampleDataD Rate;
    SampleDataD RateSD;
    EventList Spikes;
    deque< SampleDataF > Voltage;
    deque< SampleDataF > Current;
  };
  deque< Data > Results;
  RangeLoop Range;
  Options Header;

};


}; /* namespace patchclamp */

#endif /* ! _RELACS_PATCHCLAMP_FICURVE_H_ */
