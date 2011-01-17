/*
  ephys/thresholdsuspikedetector.h
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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
\version 1.0 (Jan 17, 2011)
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

    /*! The threshold for detecting spikes. */
  double Threshold;
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

  OptWidget TDW;
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
  string Unit;

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

#endif /* ! _RELACS_EPHYS_THRESHOLDSUSPIKEDETECTOR_H_ */
