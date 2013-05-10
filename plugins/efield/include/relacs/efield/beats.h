/*
  efield/beats.h
  Apply sinewaves with automatically set difference frequencies and amplitudes.

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
\brief [RePro] Apply sinewaves with automatically set difference frequencies and amplitudes.
\author Jan Benda
\version 1.0 (May 10, 2013)
\par Options
- \c duration=10seconds: Signal duration (\c number)
- \c pause=20seconds: Pause between signals (\c number)
- \c ramp=0.5seconds: Duration of linear ramp (\c number)
- \c deltafrange=10: Range of delta f's (\c string)
- \c amplitude=1mV/cm: Amplitude (\c number)
- \c repeats=10: Repeats (\c integer)
- \c fakefish=0Hz: Assume a fish with frequency (\c number)
- \c before=1seconds: Time before stimulation to be analyzed (\c number)
- \c after=1seconds: Time after stimulation to be analyzed (\c number)
- \c averagetime=1seconds: Time for computing EOD frequency (\c number)
- \c split=false: Save each run into a separate file (\c boolean)
- \c savetraces=false: Save traces during pause (\c boolean)
*/


class Beats : public RePro, public Traces, public EODTools
{
  Q_OBJECT

public:

  Beats( void );
  virtual int main( void );
  virtual void sessionStarted( void );

  void plot( double deltaf, double amplitude, double duration,
	     const MapD &eodfrequency, const EventData &jarchirpevents );
  void save( double deltaf, double amplitude, double duration, double pause,
	     double fishrate, double stimulusrate,
	     const MapD &eodfrequency, const MapD &eodamplitude, const EventData &jarchirpevents,
	     bool split, int count );
  void saveEODFreq( const Options &header, const MapD &eodfrequency, const MapD &eodamplitude,
		    bool split, int count );
  void saveChirps( const Options &header, const EventData &jarchirpevents,
		   bool split, int count );


protected:

  Plot P;

  bool Start;
  int FileCount;

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_BEATS_H_ */
