/*
  ephys/thresholdsuspikedetector.h
  

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

#ifndef _RELACS_EPHYS_THRESHOLDSUSPIKEDETECTOR_H_
#define _RELACS_EPHYS_THRESHOLDSUSPIKEDETECTOR_H_ 1

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
\class ThresholdSUSpikeDetector
\brief [Detector] Spike detection based on an absolute voltage threshold.
\author Jan Benda
\version 1.2 (Jul 20, 2014)

\par Options
- \c Detector
    - \c threshold=1mV: Detection threshold (\c number)
    - \c peaks=true: Detect peaks (\c boolean)
    - \c testmaxsize=false: Use maximum size (\c boolean)
    - \c maxsize=1mV: Maximum size (\c number)
    - \c testmaxsymmetry=false: Use maximum symmetry (\c boolean)
    - \c maxsymmetry=1: Maximum symmetry (\c number)
    - \c testisi=false: Test interspike interval (\c boolean)
    - \c minisi=1ms: Minimum interspike interval (\c number)
- \c Indicators
    - \c nospike=100ms: Interval for no spike (\c number)
    - \c considerstimulus=false: Expect spikes during stimuli only (\c boolean)
    - \c resolution=0.5mV: Resolution of spike size (\c number)
    - \c log=false: Logarithmic histograms (\c boolean)
    - \c update=1sec: Update time interval (\c number)
    - \c snippetstime=1sec: Spike snippets shown from the last (\c number)
    - \c snippetswidth=1ms: Width of spike snippet (\c number)
    - \c snippetssymmetry=0.1: Symmetry threshold for spike snippets (\c number)
    - \c history=10sec: Maximum history time (\c number)
    - \c rate=100Hz: Rate (\c number)
    - \c size=100mV: Spike size (\c number)
*/


class ThresholdSUSpikeDetector : public Filter
{
  Q_OBJECT

public:

  ThresholdSUSpikeDetector( const string &ident="", int mode=0 );
  virtual int init( const InData &data, EventData &outevents,
		    const EventList &other, const EventData &stimuli );
  virtual void readConfig( StrQueue &sq );
  virtual void notify( void );
  virtual int autoConfigure( const InData &data, double tbegin, double tend );
  virtual void save( const string &param );
  void save( void );
    /*! Detect spikes in a single trace of the analog data \a data. */
  virtual int detect( const InData &data, EventData &outevents,
		      const EventList &other, const EventData &stimuli );

    /*! Returns 1: this is an event, 0: this is not an event, -1: resume next time at lastindex. 
        Update the threshold \a threshold.
        After each call of checkEvent() the threshold is bounded
        to \a minthresh and \a maxthresh. */
  int checkEvent( InData::const_iterator first, 
		  InData::const_iterator last,
		  InData::const_iterator event, 
		  InData::const_range_iterator eventtime, 
		  InData::const_iterator index,
		  InData::const_range_iterator indextime, 
		  InData::const_iterator prevevent, 
		  InData::const_range_iterator prevtime, 
		  EventData &outevents,
		  double &threshold,
		  double &minthresh, double &maxthresh,
		  double &time, double &size, double &width );


public slots:

  void customEvent( QEvent *qce );
  void autoConfigure( void );


 protected:

  Detector< InData::const_iterator, InDataTimeIterator > D;

    /*! The threshold for detecting peaks. */
  double Threshold;
    /*! Detect peaks (true) or troughs (false ). */
  bool Peaks;
    /*! True if MaxSize is used. */
  bool TestMaxSize;
    /*! The maximum size of a spike. */
  double MaxSize;
    /*! True if MaxSymmetry and MinSymmetry are used. */
  bool TestSymmetry;
    /*! The maximum symmetry. */
  double MaxSymmetry;
    /*! The minimum symmetry. */
  double MinSymmetry;
    /*! Test interspike interval? */
  bool TestInterval;
    /*! Minimum interspike interval. */
  double MinInterval;
    /*! If no spikes are detected, update statistic assuming that
        a single spike did not occur within \a NoSpikeInterval. */
  double NoSpikeInterval;
    /*! True if spikes are expected during stimuli only. */
  bool StimulusRequired;

    /*! Time from which spike snippets are plotted. */
  double SnippetsTime;
    /*! Width of spike snippets. */
  double SnippetsWidth;
    /*! Threshold for symmetry of spike snippets. */
  double SnippetsSymmetry;
    /*! Plot histogram logarithmically. */
  bool LogHistogram;
    /*! Update time for histograms and indicators. */
  double UpdateTime;
    /*! Maximum time for history spike events. */
  double HistoryTime;

    /*! Resolution of spike sizes and thresholds. */
  double SizeResolution;

  OptWidget SDW;
  static const int UpdateFlag = 8192;

  long LastSize;
  double LastTime;
  double StimulusEnd;
  double IntervalStart;
  double IntervalEnd;
  double IntervalWidth;
  QTime Update;
  Plot *SP;
  Plot *HP;
  SampleDataD GoodSpikesHist;
  SampleDataD BadSpikesHist;
  SampleDataD AllSpikesHist;
  string Unit;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_THRESHOLDSUSPIKEDETECTOR_H_ */
