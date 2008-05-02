/*
  syslatency.h
  Measures latency of the whole system, i.e. signal delay, transduction, 

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <relacs/common/ephystraces.h>
#include <relacs/auditory/auditorytraces.h>


/*! 
\class SysLatency
\brief Measures latency of the whole system, i.e. signal delay, transduction, 
synapses, axonal delay, ...
\author Jan Benda
\version 1.5 (Jan 10, 2008)
-# removed stop() function
-# moved class variables to the main() functions
-# changed offset in coincidentSpikes() from spikewidth to 0.5*(spikewidth+pduration)
\version 1.4 (Mar 9, 2006)
-# Fixed algorithm for finding the right intensity in f-I curve (maxIndex).
-# Added mean firing rate to plot and files.
\version 1.3 (Feb 27, 2006)
-# Fixed some bugs regarding the calculation of coincident spikes (especially spike width).
-# Added plot showing number of coincident spikes.
*/


class SysLatency : public RePro, public EPhysTraces, public AuditoryTraces
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

  Options Settings;

};


#endif /* ! _RELACS_AUDITORY_SYSLATENCY_H_ */
