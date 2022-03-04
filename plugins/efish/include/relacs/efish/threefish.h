/*
  efish/threefish.h
  Beats of three fish.

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

#ifndef _RELACS_EFISH_THREEFISH_H_
#define _RELACS_EFISH_THREEFISH_H_ 1

#include <relacs/options.h>
#include <relacs/multiplot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
#include <relacs/efield/traces.h>
#include <relacs/efield/eodtools.h>
using namespace relacs;

namespace efish {


/*!
\class ThreeFish
\brief [RePro] Beats of three fish
\author Jan Benda, Jan Grewe
\version 1.4 (Jul 05, 2021)
\par Options
- \c Stimulus
    - \c eodmultiples=true: Stimulus frequencies in multiples of EODf (\c boolean)
    - \c relfreqs=true: Stimulus frequencies relative to EODf (\c boolean)
    - \c skipeodf=true: Skip stimulus frequencies at EODf multiples (\c boolean)
    - \c fakefish=0Hz: Assume a fish with frequency (\c number)
    - \c amplsel=contrast: Stimulus amplitudes are relative to the fish amplitude or absolute (\c string)
    - \c shuffle=Up: Order of delta f's (\c string)
    - \c increment=-1: Initial increment for delta f's (\c integer)
    - \c repeats=10: Repeats (\c integer)
    - \c duration12=1000ms: Duration of stimulus segment with both fish (\c number)
    - \c pause=500ms: Pause between signals (\c number)
- \c Fish 1
    - \c duration1=0ms: Duration of first fish alone (\c number)
    - \c deltaf1min=0EODf: Minimum delta f (beat frequency) of first fish (\c number)
    - \c deltaf1max=0.5EODf: Maximum delta f (beat frequency) of first fish (\c number)
    - \c deltaf1step=0.1EODf: Increment delta f (beat frequency) of first fish (\c number)
    - \c deltaf1minhz=10Hz: Minimum delta f (beat frequency) of first fish (\c number)
    - \c deltaf1maxhz=100Hz: Maximum delta f (beat frequency) of first fish (\c number)
    - \c deltaf1stephz=10Hz: Increment delta f (beat frequency) of first fish (\c number)
    - \c contrast1=10%: Contrast of first fish (\c number)
    - \c amplitude1=1mV: Amplitude of first fish (\c number)
- \c Fish 2
    - \c duration2=0ms: Duration of second fish alone (\c number)
    - \c delay2=0ms: Delay of second fish (\c number)
    - \c deltaf2min=0EODf: Minimum delta f (beat frequency) of second fish (\c number)
    - \c deltaf2max=0.5EODf: Maximum delta f (beat frequency) of second fish (\c number)
    - \c deltaf2step=0.1EODf: Increment delta f (beat frequency) of second fish (\c number)
    - \c deltaf2minhz=10Hz: Minimum delta f (beat frequency) of second fish (\c number)
    - \c deltaf2maxhz=100Hz: Maximum delta f (beat frequency) of second fish (\c number)
    - \c deltaf2stephz=10Hz: Increment delta f (beat frequency) of second fish (\c number)
    - \c contrast2=10%: Contrast of second fish (\c number)
    - \c amplitude2=1mV: Amplitude of second fish (\c number)
- \c Analysis
    - \c before=100ms: Spikes recorded before stimulus (\c number)
    - \c after=100ms: Spikes recorded after stimulus (\c number)
    - \c sigma=10ms: Standard deviation of rate smoothing kernel (\c number)
*/


class ThreeFish
  : public RePro,
    public ephys::Traces,
    public efield::Traces,
    public efield::EODTools
{
  Q_OBJECT

public:

  ThreeFish( void );
  virtual int main( void );
  

protected:

  int fishEOD(double pause, double &rate, double &amplitude);
  int makeEOD(OutData &eod, double fishrate, double deltaf, double duration,
	      double phase, bool fullperiods);
  void plot( const vector< MapD > &amtraces, const EventList &spikes,
	     const SampleDataD &spikerate, double maxrate, int repeats );
  void analyze( vector< MapD > &amtraces, EventList &spikes,
		SampleDataD &spikerate, double &maxrate,
		double duration, double before, double after, double sigma );
  void saveRate( const Options &header, const SampleDataD &spikerate );
  void saveSpikes( const Options &header, const EventList &spikes );
  void saveAmpl( const Options &header, const vector< MapD > &amtracesa );
  void save( double fishrate, double fishamplitude, double deltaf1, double deltaf2,
	     const EventList &spikes, const SampleDataD &spikerate, const vector< MapD > &amtraces );
  void stop( void );

  MultiPlot P;

};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_THREEFISH_H_ */
