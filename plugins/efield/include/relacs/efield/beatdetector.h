/*
  efield/beatdetector.cc
  Detects beats in EODs of wave-type weakly electric fish

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

#ifndef _RELACS_EFIELD_BEATDETECTOR_H_
#define _RELACS_EFIELD_BEATDETECTOR_H_ 1

#include <relacs/optwidget.h>
#include <relacs/detector.h>
#include <relacs/filter.h>
using namespace relacs;

namespace efield {


/*!
\class BeatDetector
\brief [Detector] Detects beats in EODs of wave-type weakly electric fish
\author Jan Benda

Detects the peaks and troughs of EOD beats.
The dynamic threshold is set to \a ratio times the amplitude of the beat
and decays after a \a delay down to \a minthresh with a time constant \a decay.

\par Input
The times and amplitudes of EOD cycles.

\par Output
The times and amplitudes of beat peaks and troughs.

\par Options
- \c threshold=6: Threshold (\c number)
- \c minthresh=6: Minimum threshold (\c number)
- \c delay=0sec: Delay time (\c number)
- \c decay=10sec: Decay time constant (\c number)
- \c ratio=50%: Ratio threshold / size (\c number)

\version 1.2 (Jun 17, 2009)
*/


class BeatDetector : public Filter
{
  Q_OBJECT

public:

    /*! The constructor. */
  BeatDetector( const string &ident="", int mode=0 );
    /*! The destructor. */
  ~BeatDetector( void );

  virtual int init( const EventList &inevents, EventList &outevents, 
		    const EventList &other, const EventData &stimuli );
  virtual void notify( void );
  virtual int adjust( const EventList &events );
    /*! Detect the beat maxima and minima in EOD events. */
  virtual int detect( const EventList &inevents, EventList &outevents, 
		      const EventList &other, const EventData &stimuli );
  int checkPeak( const EventSizeIterator &first, 
		 const EventSizeIterator &last,
		 EventSizeIterator &event, 
		 EventIterator &eventtime, 
		 EventSizeIterator &index,
		 EventIterator &indextime,
		 EventSizeIterator &prevevent, 
		 EventIterator &prevtime, 
		 EventList &outevents, 
		 double &threshold,
		 double &minthresh, double &maxthresh,
		 double &time, double &size, double &width );
  int checkTrough( const EventSizeIterator &first, 
		   const EventSizeIterator &last,
		   EventSizeIterator &event, 
		   EventIterator &eventtime, 
		   EventSizeIterator &index,
		   EventIterator &indextime,
		   EventSizeIterator &prevevent, 
		   EventIterator &prevtime, 
		   EventList &outevents, 
		   double &threshold,
		   double &minthresh, double &maxthresh,
		   double &time, double &size, double &width );


protected:

  Detector< EventSizeIterator, EventIterator > D;

  double Threshold;
  double MinThresh;
  double MaxThresh;
  double Delay;
  double Decay;
  double Ratio;

  double ChirpDistance;

  const EventData *ChirpEvents;

  OptWidget BDW;

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_BEATDETECTOR_H_ */
