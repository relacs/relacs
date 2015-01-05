/*
  auditory/adaptedficurves.h
  First adapts the neuron to a background sound intensity and then measures f-I curves.

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

#ifndef _RELACS_AUDITORY_ADAPTEDFICURVES_H_
#define _RELACS_AUDITORY_ADAPTEDFICURVES_H_ 1

#include <relacs/multiplot.h>
#include <relacs/ephys/traces.h>
#include <relacs/acoustic/traces.h>
#include <relacs/repro.h>
using namespace relacs;

namespace auditory {


/*!
\class AdaptedFICurves
\brief [RePro] First adapts the neuron to a background sound intensity and then measures f-I curves.
\author Jan Benda
\version 1.0 (Jan 27, 2011)
\par Screenshot
\image html adaptedficurves.png

\par Options
- Stimulus
- Adaptation stimulus
- \c adaptbase=SPL: Intensity of adapting stimulus relative to (\c string)
- \c adaptint=50dB SPL: Sound intensity of adapting stimulus (\c number)
- \c adaptinit=1000ms: Duration of initial adaptation stimulus (\c number)
- \c adaptduration=100ms: Duration of subsequent adaptation stimuli (\c number)
- Test stimuli
- \c intbase=SPL: Intensities of test stimulus relative to (\c string)
- \c intmin=50dB: Minimum sound intensity of test stimulus (\c number)
- \c intmax=100dB: Maximum sound intensity of test stimulus (\c number)
- \c intstep=10dB SPL: Sound-intensity steps of test stimulus (\c number)
- \c duration=100ms: Duration of test stimuli (\c number)
- General
- \c side=left: Speaker (\c string)
- \c carrierfreq=0kHz: Frequency of carrier (\c number)
- \c usebestfreq=true: Relative to the cell's best frequency (\c boolean)
- \c ramp=1ms: Duration of ramps for all intenisty transitions (\c number)
- \c pause=1000ms: Pause between stimuli (\c number)
- \c delay=100ms: Part of pause before stimulus (\c number)
- \c repetitions=10: Number of repetitions of the stimulus (\c integer)
- Analysis
- \c onsettime=100ms: Onset rate occurs within (\c number)
- \c sstime=100ms: Width for measuring steady-states (\c number)
*/


class AdaptedFICurves : public RePro, public ephys::Traces, public acoustic::Traces
{
  Q_OBJECT

public:

  AdaptedFICurves( void );
  virtual int main( void );


protected:

  void analyze( EventList &spikes, SampleDataD &rate,
		double delay, double duration, double pause,
		int count, double sstime, double onsettime,
		const MapD &times, MapD &onsetrates, MapD &onsetratessd,
		MapD &ssrates, MapD &ssratessd, MapD &spikecount );
  void plot( const EventList &spikes, const SampleDataD &rate, const SampleDataD &am,
	     const MapD &onsetrates, const MapD &ssrates,
	     double adaptint, const MapD &spikecount );
  void saveSpikes( const Options &header, const EventList &spikes );
  void saveRate( const Options &header, const SampleDataD &rate );
  void saveData( const Options &header,
		 const MapD &times, const MapD &onsetrates, const MapD &onsetratessd,
		 const MapD &ssrates, const MapD &ssratessd );

  MultiPlot P;

};


}; /* namespace auditory */

#endif /* ! _RELACS_AUDITORY_ADAPTEDFICURVES_H_ */
