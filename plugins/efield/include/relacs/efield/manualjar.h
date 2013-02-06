/*
  efield/manualjar.h
  Apply sinewaves with manually set difference frequencies and amplitudes.

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

#ifndef _RELACS_EFIELD_MANUALJAR_H_
#define _RELACS_EFIELD_MANUALJAR_H_ 1

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
\class ManualJAR
\brief [RePro] Apply sinewaves with manually set difference frequencies and amplitudes.
\author Jan Benda
\version 1.0 (Jan 29, 2013)
\par Options
- \c Stimulation
    - \c duration=10seconds: Signal duration (\c number)
    - \c ramp=0.5seconds: Duration of linear ramp (\c number)
    - \c fakefish=0Hz: Assume a fish with frequency (\c number)
- \c Analysis
    - \c before=1seconds: Time before stimulation to be analyzed (\c number)
    - \c after=1seconds: Time after stimulation to be analyzed (\c number)
    - \c averagetime=1seconds: Time for computing EOD frequency (\c number)
    - \c split=false: Save each run into a separate file (\c boolean)
    - \c savetraces=false: Save traces during pause (\c boolean)
*/


class ManualJAR : public RePro, public Traces, public EODTools
{
  Q_OBJECT

public:

  ManualJAR( void );
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


public slots:

  void setValues( void );


protected:

  virtual void keyPressEvent( QKeyEvent *e );
  virtual void customEvent( QEvent *qce );

  Plot P;
  OptWidget JW;
  QLabel *ModeLabel;
  QLabel *ElapsedTimeLabel;
  QPushButton *StartButton;

  bool Start;
  int Count;

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_MANUALJAR_H_ */
