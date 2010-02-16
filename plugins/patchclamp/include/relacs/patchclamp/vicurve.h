/*
  patchclamp/vicurve.h
  V-I curve measured in current-clamp

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

#ifndef _RELACS_PATCHCLAMP_VICURVE_H_
#define _RELACS_PATCHCLAMP_VICURVE_H_ 1

#include <vector>
#include <relacs/sampledata.h>
#include <relacs/multiplot.h>
#include <relacs/rangeloop.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace patchclamp {


/*!
\class VICurve
\brief [RePro] V-I curve measured in current-clamp
\author Jan Benda
\version 1.0 (Feb 12, 2010)
*/


class VICurve : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

  VICurve( void );
  virtual void config( void );
  virtual void notify( void );
  virtual int main( void );
  void plot( double duration );
  void save( void );
  void saveData( void );
  void saveTrace( void );


protected:

  MultiPlot P;
  string VUnit;
  string IUnit;
  double VFac;
  double IFac;
  double IInFac;

  struct Data
  {
    Data( double delay, double duration, double stepsize, bool current );
    void analyze( int count, const InData &intrace,
		  const EventData &spikes, const InData *incurrent,
		  double iinfac, double delay,
		  double duration, double ton, double sswidth );
    double DC;
    double I;
    double VRest;
    double VRestsd;
    double VOn;
    double VOnsd;
    double VPeak;
    double VPeaksd;
    double VPeakTime;
    double VSS;
    double VSSsd;
    double SpikeCount;
    SampleDataD MeanTrace;
    SampleDataD SquareTrace;
    SampleDataD StdevTrace;
    SampleDataD MeanCurrent;
  };
  vector< Data > Results;
  RangeLoop Range;
  Options Header;

};


}; /* namespace patchclamp */

#endif /* ! _RELACS_PATCHCLAMP_VICURVE_H_ */
