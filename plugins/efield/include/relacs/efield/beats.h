/*
  efield/beats.h
  Play EOD mimicks with optional chirps from a range of automatically set difference frequencies and amplitudes.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_EFIELD_BEATS_H_
#define _RELACS_EFIELD_BEATS_H_ 1

#include <QLabel>
#include <QPushButton>
#include <relacs/plot.h>
#include <relacs/optwidget.h>
#include <relacs/repro.h>
#include <relacs/efield/traces.h>
#include <relacs/efield/eodtools.h>
using namespace relacs;

namespace efield {


/*!
\class Beats
\brief [RePro]   Play EOD mimicks with optional chirps from a range of automatically set difference frequencies and amplitudes.
\author Jan Benda
\version 2.0 (Sep 19, 2013)
\par Options
- \c Stimulation
    - \c duration=10seconds: Signal duration (\c number)
    - \c pause=20seconds: Pause between signals (\c number)
    - \c ramp=0.5seconds: Duration of linear ramp (\c number)
    - \c deltafrange=10: Range of delta f's (\c string)
    - \c deltafshuffle=Up: Order of delta f's (\c string)
    - \c fixeddf=false: Keep delta f fixed (\c boolean)
    - \c amplitude=1mV/cm: Amplitude (\c number)
    - \c repeats=10: Repeats (\c integer)
    - \c fakefish=0Hz: Assume a fish with frequency (\c number)
- \c Chirps
    - \c chirpsize=100Hz: Size of chirp (\c number)
    - \c chirpwidth=100ms: Width of chirp (\c number)
    - \c chirpampl=0%: Amplitude reduction during chirp (\c number)
    - \c chirpkurtosis=1: Kurtosis of Gaussian chirp (\c number)
    - \c chirpfrequencies=: Chirp frequencies for each delta f (\c string)
    - \c chirptimesfile=: File with chirp times (\c string)
- \c Analysis
    - \c before=1seconds: Time before stimulation to be analyzed (\c number)
    - \c after=1seconds: Time after stimulation to be analyzed (\c number)
    - \c averagetime=1seconds: Time for computing EOD frequency (\c number)
    - \c showstimulus=false: Plot frequency of stimulus (\c boolean)
    - \c split=false: Save each run into a separate file (\c boolean)
    - \c savetraces=false: Save traces during pause (\c boolean)

\par Chirps 

Chirps are generated whenever at least one frequency of chirps
(chirpfrequency) or a file containing the timepoints of chirps is
specified. If only a chirp frequency is specified, then series of
evenly spaced chirps with that frequency are produced. If only a file
containing times is specified (no chirpfrequency) then the chirps are
positioned as specified in the file. If in addition a chirp frequency
is specified, the timepoints in the file are scaled with the inverse
frequency. By specifying as many chirp frequency as there are delta f's
the chirp frequency can be made depenendent on delta f.  */


class Beats : public RePro, public Traces, public EODTools
{
  Q_OBJECT

public:

  Beats( void );
  virtual int main( void );
  virtual void sessionStarted( void );

  void initPlot( double deltaf, double amplitude, double duration,
		 const MapD &eodfrequency, const EventData &fishchirps,
		 bool showstimulus, const MapD &stimfrequency, const EventData &chirptimes );
  void save( double deltaf, double amplitude, double duration, double pause,
	     double fishrate, double stimulusrate,
	     const MapD &eodfrequency, const MapD &eodamplitude,
	     const EventData &fishchirps, const EventData &playedchirpevents,
	     const MapD &stimfrequency, const Options &chirpheader, bool split, int count );
  void saveEODFreq( const Options &header, const MapD &eodfrequency, const MapD &eodamplitude,
		    bool split, int count );
  void saveChirps( const Options &header, const EventData &chirps,
		   bool split, int count );
  void savePlayedChirps( const Options &header, const EventData &chirps,
			 bool split, int count );


protected:

  Plot P;

  bool Start;
  int FileCount;

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_BEATS_H_ */
