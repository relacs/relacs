/*
  auditory/syslatency.h
  Measures latency of the whole system, i.e. signal delay, transduction, 

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

#ifndef _RELACS_AUDITORY_SYSLATENCY_H_
#define _RELACS_AUDITORY_SYSLATENCY_H_ 1

#include <relacs/repro.h>
#include <relacs/array.h>
#include <relacs/map.h>
#include <relacs/eventlist.h>
#include <relacs/multiplot.h>
#include <relacs/ephys/traces.h>
#include <relacs/acoustic/traces.h>
using namespace relacs;

namespace auditory {


/*! 
\class SysLatency
\brief [RePro] Measures latency of the whole system, i.e. signal delay, transduction, 
synapses, axonal delay, ...
\author Jan Benda

\par Options
- Stimulus
- \c rate=100Hz: Target firing rate (\c number)
- \c pwaves=10: Number of cycles of pertubation (\c integer)
- \c pintensity=10dB: Intensity of pertubations (\c number)
- \c minpintensity=4dB: Minimum intensity of pertubations (\c number)
- \c carrierfreq=5kHz: Frequency of carrier (\c number)
- \c usebestfreq=true: Use the cell's best frequency (\c boolean)
- \c ramp=2ms: Ramp of stimulus (\c number)
- \c duration=600ms: Duration of stimulus (\c number)
- \c pause=600ms: Pause (\c number)
- \c repeats=10: Number of stimulus repetitions (\c integer)
- \c side=left: Speaker (\c string)
- Analysis
- \c skipwin=100ms: Initial portion of stimulus not used for analysis (\c number)
- \c analysewin=10ms: Window used for ISI analysis (\c number)
- \c maxlat=10ms: Maximum latency (\c number)
- \c latstep=0.1ms: Resolution of latency (\c number)
- \c coincwin=0.5ms: Window width for coincident spikes (\c number)

\version 1.5 (Jan 10, 2008)
*/


  class SysLatency : public RePro, public ephys::Traces, public acoustic::Traces
{
  Q_OBJECT

public:

    /*! Constructor. */
  SysLatency( void );
    /*! Destructor. */
  ~SysLatency( void );

  virtual int main( void );

  void saveSpikes( Options &header, const EventList &spikes );
  void saveTrigger( Options &header, const ArrayD &trigger );
  void saveCoincidentSpikes( Options &header, const MapD &coincidentspikes );
  void savePRC( Options &header, const MapD &prc );
  void save( double carrierfrequency, int side, double pduration,
	     double intensity,
	     const EventList &spikes, const ArrayD &trigger,
	     const MapD &coincidentspikes, const MapD &prc,
	     double spikewidth, double latency, double latencysd,
	     int maxcoincidence, double coinclatency,
	     double offset, double slope, double meanrate );

    /*! Plot data. */
  void plot( const MapD &coincidentspikes, const MapD &prc,
	     double coinclatency, double offset, double slope,
	     double meanrate );
    /*! Analyze data. */
  void analyze( double duration, double skipwin, double analysewin,
		double pduration, double coincwin,
		double maxlatency, double latencystep,
		EventList &spikes, const ArrayD &trigger,
		MapD &coincidentspikes, MapD &prc,
		double &spikewidth, double &latency, double &latencysd,
		int &maxcoincidence, double &coinclatency,
		double &offset, double &slope, double &meanrate );


protected:

  int coincidentSpikes( double coincwin, double latency, double offset,
			const EventList &spikes, const ArrayD &trigger );
  void phaseResponse( double duration, double skipwin, double analysewin,
		      double latency, const EventList &spikes,
		      const ArrayD &trigger, MapD &prc );

  MultiPlot P;

};


}; /* namespace auditory */

#endif /* ! _RELACS_AUDITORY_SYSLATENCY_H_ */
