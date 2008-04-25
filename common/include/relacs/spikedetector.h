/*
  spikedetector.h
  A detector for spikes in single unit recordings.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _SPIKEDETECTOR_H_
#define _SPIKEDETECTOR_H_

#include <qpixmap.h>
#include <qlabel.h>
#include <qdatetime.h>
#include <relacs/optwidget.h>
#include <relacs/sampledata.h>
#include <relacs/plot.h>
#include <relacs/detector.h>
#include <relacs/filter.h>


/*! 
\class SpikeDetector
\brief A detector for spikes in single unit recordings.
\author Jan Benda
\version 1.6 (Jan 24, 2008)
-# fixed bugs in trend indication
-# added options testwidth and testinterval
\version 1.5 (Jul 1, 2005)
\bug dynamicPeakTrough should get minIndex and not begin() as first!

Sorry! No documentation.
*/


class SpikeDetector : public Filter
{
  Q_OBJECT

public:

    /*! The constructor. */
  SpikeDetector( const string &ident="", int mode=0 );
    /*! The destructor. */
  ~SpikeDetector( void );

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
    /*! Delay of the threshold dynamics in seconds. */
  double Delay;
    /*! Decay time constant of the threshold dynamics in seconds. */
  double Decay;

    /*! Test spike width? */
  bool TestWidth;
    /*! Maximum width of a spike in seconds. */
  double MaxWidth;
    /*! Test interspike interval? */
  bool TestInterval;
    /*! Minimum interspike interval. */
  double MinInterval;
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


#endif
