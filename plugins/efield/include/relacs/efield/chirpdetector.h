/*
  efield/chirpdetector.h
  Detects chirps of wave-type weakly electric fish

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

#ifndef _RELACS_EFIELD_CHIRPDETECTOR_H_
#define _RELACS_EFIELD_CHIRPDETECTOR_H_ 1

#include <relacs/optwidget.h>
#include <relacs/detector.h>
#include <relacs/filter.h>
using namespace relacs;

namespace efield {


/*!
\class ChirpDetector
\brief [Detector] Detects chirps of wave-type weakly electric fish
\author Jan Benda

Chirps are transient increases of the EOD frequency of wave-type weakly electric fish.
A chirp is detected if the EOD frequency increases by more than \a threshold Hz.
A chirp has to last at least \a minwidth and at most \a maxwidth
and has a minimum size of \a minthresh.

\par Input
The event times of an EOD.

\par Output
The times, sizes, and widths of chirps.

\par Options
- \c threshold=8Hz: Threshold (\c number)
- \c minthresh=0Hz: Minimum threshold (\c number)
- \c minwidth=3ms: Minimum width (\c number)
- \c maxwidth=50ms: Maximum width (\c number)

\version 1.2 (Jun 17, 2009)
*/


class ChirpDetector : public Filter
{
  Q_OBJECT

public:

  ChirpDetector( const string &ident="", int mode=0 );
  ~ChirpDetector( void );

  virtual int init( const EventData &inevents, EventData &outevents,
		    const EventList &other, const EventData &stimuli );
  virtual void notify( void );
    /*! Detect chirps in EOD events \a inevents. */
  virtual int detect( const EventData &inevents, EventData &outevents,
		      const EventList &other, const EventData &stimuli );

  int checkEvent( const EventFrequencyIterator &first, 
		  const EventFrequencyIterator &last,
		  EventFrequencyIterator &event, 
		  EventIterator &eventtime, 
		  EventFrequencyIterator &index,
		  EventIterator &indextime,
		  EventFrequencyIterator &prevevent, 
		  EventIterator &prevtime, 
		  EventData &outevents, 
		  double &threshold,
		  double &minthresh, double &maxthresh,
		  double &time, double &size, double &width );


protected:

  Detector< EventFrequencyIterator, EventIterator > D;

  double Threshold;
  double MinThresh;
  double ChirpMinWidth;
  double ChirpMaxWidth;
  int ChirpCycles;
  int AverageCycles;

  const EventList *Other;

  OptWidget CDW;

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_CHIRPDETECTOR_H_ */
