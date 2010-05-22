/*
  ephys/intraspikedetector.h
  A detector for spikes in intracellular recordings.

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

#ifndef _RELACS_EPHYS_INTRASPIKEDETECTOR_H_
#define _RELACS_EPHYS_INTRASPIKEDETECTOR_H_ 1

#include <qpixmap.h>
#include <qlabel.h>
#include <qdatetime.h>
#include <relacs/optwidget.h>
#include <relacs/sampledata.h>
#include <relacs/plot.h>
#include <relacs/detector.h>
#include <relacs/filter.h>
using namespace relacs;

namespace ephys {


/*! 
\class IntraSpikeDetector
\brief [Detector] A detector for spikes in intracellular recordings.
\author Jan Benda

\par Options
- Detector
- \c threshold=10mV: Detection threshold (\c number)
- \c abspeak=0mV: Absolute threshold (\c number)
- \c testwidth=true: Test spike width (\c boolean)
- \c maxwidth=1.5ms: Maximum spike width (\c number)
- \c fitpeak=false: Fit parabula to peak of spike (\c boolean)
- \c fitwidth=0.5ms: Width of parabula fit (\c number)
- Indicators
- \c resolution=1mV: Resolution of spike size (\c number)
- \c log=false: Logarithmic histograms (\c boolean)
- \c update=1sec: Update time interval (\c number)
- \c history=10sec: Maximum history time (\c number)
*/


class IntraSpikeDetector : public Filter
{
  Q_OBJECT

public:

    /*! The constructor. */
  IntraSpikeDetector( const string &ident="", int mode=0 );
    /*! The destructor. */
  ~IntraSpikeDetector( void );

  virtual int init( const InData &data, EventData &outevents,
		    const EventList &other, const EventData &stimuli );
  virtual void notify( void );
  virtual void save( const string &param );
  void save( void );
    /*! Detect spikes in a single trace of the analog data \a ID. */
  virtual int detect( const InData &data, EventData &outevents,
		      const EventList &other, const EventData &stimuli );


    /*! Returns 1: this is an event, 0: this is not an event, -1: resume next time at lastindex. 
        Update the threshold \a threshold.
        After each call of checkEvent() the threshold is bounded
        to \a minthresh and \a maxthresh. */
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

    /*! The threshold for detecting spikes. */
  double Threshold;
    /*! Absolute height of a spike peak. */
  double AbsPeak;
    /*! Test spike width? */
  bool TestWidth;
    /*! Maximum width of a spike in seconds. */
  double MaxWidth;
    /*! Fit a parabula to the spike peak? */
  bool FitPeak;
    /*! Width of the parabula fit in seconds. */
  double FitWidth;
    /*! Width of the parabula fit in indices of the input trace. */
  int FitIndices;

    /*! Plot histogram logarithmically. */
  bool LogHistogram;
    /*! Update time for histograms and indicators. */
  double UpdateTime;
    /*! Maximum time for history spike events. */
  double HistoryTime;

    /*! Resolution of spike sizes and thresholds. */
  double SizeResolution;

  OptWidget SDW;

  QTime Update;
  Plot *P;
  SampleDataD GoodSpikesHist;
  SampleDataD BadSpikesHist;
  SampleDataD AllSpikesHist;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_INTRASPIKEDETECTOR_H_ */
