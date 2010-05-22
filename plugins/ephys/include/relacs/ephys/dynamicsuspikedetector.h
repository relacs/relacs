/*
  ephys/dynamicsuspikedetector.h
  A detector for spikes in single unit recordings.

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

#ifndef _RELACS_EPHYS_DYNAMICSUSPIKEDETECTOR_H_
#define _RELACS_EPHYS_DYNAMICSUSPIKEDETECTOR_H_ 1

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
\class DynamicSUSpikeDetector
\brief [Detector] A detector for spikes in single unit recordings.
\author Jan Benda
\version 1.8 (Mar 16, 2010)

\par Options
- Detector
- \c minthresh=10mV: Minimum threshold (\c number)
- \c threshold=10mV: Threshold (\c number)
- \c delay=1sec: Delay time (\c number)
- \c decay=10sec: Decay time constant (\c number)
- \c searchdelay=1sec: Delay time for inbetween the recording sessions (\c number)
- \c searchdecay=10sec: Decay time constant for inbetween the recording sessions (\c number)
- \c ratio=50%: Ratio threshold / size (\c number)
- \c testwidth=true: Test spike width (\c boolean)
- \c maxwidth=1.5ms: Maximum spike width (\c number)
- \c testisi=true: Test interspike interval (\c boolean)
- \c minisi=1ms: Minimum interspike interval (\c number)
- \c fitpeak=false: Fit parabula to peak of spike (\c boolean)
- \c fitwidth=0.5ms: Width of parabula fit (\c number)
- Running average
- \c nospike=100ms: Interval for no spike (\c number)
- \c considerstimulus=false: Expect spikes during stimuli only (\c boolean)
- Indicators
- \c resolution=0.5mV: Resolution of spike size (\c number)
- \c log=false: Logarithmic histograms (\c boolean)
- \c update=1sec: Update time interval (\c number)
- \c history=10sec: Maximum history time (\c number)
- \c qualitythresh=5%: Quality threshold (\c number)
- \c trendthresh=1%: Trend threshold (\c number)
- \c trendtime=1sec: Trend timescale (\c number)
*/


class DynamicSUSpikeDetector : public Filter
{
  Q_OBJECT

public:

    /*! The constructor. */
  DynamicSUSpikeDetector( const string &ident="", int mode=0 );
    /*! The destructor. */
  ~DynamicSUSpikeDetector( void );

  virtual int init( const InData &data, EventData &outevents,
		    const EventList &other, const EventData &stimuli );
  virtual void notify( void );
  virtual int adjust( const InData &data );
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


public slots:

  void customEvent( QCustomEvent *qce );


protected:

  Detector< InData::const_iterator, InDataTimeIterator > D;

    /*! The threshold for detecting spikes. */
  double Threshold;
    /*! Minimum value for the threshold detecting spikes. */
  double MinThresh;
    /*! Maximum value for the threshold detecting spikes. */
  double MaxThresh;
    /*! Maximum value for the threshold detecting spikes according to the input range. */
  double MaxRangeThresh;
    /*! Delay of the threshold dynamics in seconds. */
  double RecordingDelay;
    /*! Decay time constant of the threshold dynamics in seconds. */
  double RecordingDecay;
    /*! Delay of the threshold dynamics in seconds. */
  double SearchDelay;
    /*! Decay time constant of the threshold dynamics in seconds. */
  double SearchDecay;

    /*! Test spike width? */
  bool TestWidth;
    /*! Maximum width of a spike in seconds. */
  double MaxWidth;
    /*! Test interspike interval? */
  bool TestInterval;
    /*! Minimum interspike interval. */
  double MinInterval;
    /*! Fit a parabula to the spike peak? */
  bool FitPeak;
    /*! Width of the parabula fit in seconds. */
  double FitWidth;
    /*! Width of the parabula fit in indices of the input trace. */
  int FitIndices;
    /*! Ratio of the spike size to which the new value of the threshold is set. */
  double Ratio;
    /*! If no spikes are detected, update statistic assuming that
        a single spike did not occur within \a NoSpikeInterval. */
  double NoSpikeInterval;
    /*! True if spikes are expected during stimuli only. */
  bool StimulusRequired;

    /*! Plot histogram logarithmically. */
  bool LogHistogram;
    /*! Update time for histograms and indicators. */
  double UpdateTime;
    /*! Maximum time for history spike events. */
  double HistoryTime;
    /*! Threshold for quality indicator (fraction of overlap). */
  double QualityThresh;
    /*! Threshold for trend indicator relative to spike size. */
  double TrendThresh;
    /*! Time scale for determining the trend. */
  double TrendTime;

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
  Plot *P;
  SampleDataD GoodSpikesHist;
  SampleDataD BadSpikesHist;
  SampleDataD AllSpikesHist;

  QPixmap GoodQuality;
  QPixmap OkQuality;
  QPixmap PotentialQuality;
  QPixmap BadQuality;
  QPixmap *QualityPixs[4];
  int Quality;
  QLabel *QualityIndicator;
  QPixmap BadTrend;
  QPixmap OkTrend;
  QPixmap GoodTrend;
  QPixmap BadArrow;
  QPixmap GoodArrow;
  QPixmap *TrendPixs[5];
  int Trend;
  QLabel *TrendIndicator;
  double LastSpikeSize;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_DYNAMICSUSPIKEDETECTOR_H_ */
