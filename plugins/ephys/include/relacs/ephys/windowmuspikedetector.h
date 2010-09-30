/*
  ephys/windowmuspikedetector.h
  bla bnla

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

#ifndef _RELACS_EPHYS_WINDOWMUSPIKEDETECTOR_H_
#define _RELACS_EPHYS_WINDOWMUSPIKEDETECTOR_H_ 1

#include <relacs/filter.h>
#include <relacs/eventdata.h>
#include <relacs/eventlist.h>
#include <relacs/indata.h>
#include <relacs/optwidget.h>
using namespace relacs;

namespace ephys {


/*!
\class WindowMUSpikeDetector
\brief [Filter] bla bnla
\author Karin Fisch
\version 1.0 (Mai 26, 2010)
*/


class WindowMUSpikeDetector : public Filter
{
  Q_OBJECT

public:

  WindowMUSpikeDetector( void );
  virtual int main( void );
  virtual int init( const InData &data, EventData &outevents,
		     const EventList &other, const EventData &stimuli );
  virtual int detect( const InData &data, EventData &outevents,
		      const EventList &other, const EventData &stimuli );

  int checkEvent( const InData::const_iterator &first, 
		  const InData::const_iterator &last,
		  InData::const_iterator &event, 
		  InDataTimeIterator &eventtime, 
		  InData::const_iterator &index,
		  InDataTimeIterator &indextime, 
		  InData::const_iterator &prevevent, 
		  InDataTimeIterator &prevtime, 
		  EventData &outevents,
		  double &threshold,
		  double &minthresh, double &maxthresh,
		  double &time, double &size, double &width );


 protected:

  Detector< InData::const_iterator, InDataTimeIterator > D;

  double Threshold;
  double MinThresh;
  double MaxThresh;
  double ThreshRatio;
  bool AdaptThresh;

    /*! Maximum period of the EOD to detect in seconds. */
  double MaxEODPeriod;

  OptWidget EDW;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_WINDOWMUSPIKEDETECTOR_H_ */
