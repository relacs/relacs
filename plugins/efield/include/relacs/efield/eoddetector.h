/*
  efield/eoddetector.h
  A detector for EOD cycles of weakly electric fish

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

#ifndef _RELACS_EFIELD_EODDETECTOR_H_
#define _RELACS_EFIELD_EODDETECTOR_H_ 1

#include <relacs/optwidget.h>
#include <relacs/detector.h>
#include <relacs/filter.h>
#include <relacs/efield/eodtools.h>
using namespace relacs;

namespace efield {


/*!
\class EODDetector
\brief [Detector] A detector for EOD cycles of weakly electric fish
\author Jan Benda
\version 1.7 (Feb 3, 2013)


Detects each cycle of a periodic input waveform, like the EOD of a wave-type
weakly electric fish.

The threshold defines the minimum distance between peaks and troughs
that are detected.  If \a adapt is set to \c true, then the threshold
is set automatically to \a ratio times the two-fold size of the EOD
amplitude. Once a peak is detected the time of the EOD cycles is
computed as the threshold crossing time of a voltage level that is at
\a zeroratio times the peak-to-peak amplitude below the peak. You can
select from four methods to determine the exact threshold crossing
time (\a interpolation):
- \c closest \c datapoint takes the time of the
  the data point right before the threshold crossing.
- \c linear \c interpolation takes the time of a linear interpolation
  between the two data points directly below and above the threshold crossing.
- \c linear \c fit and \c quadratic \c fit fit a line or a parabola to the
  region determined by \a fitwin and compute from the fit
  the threshold crossing time.
.
For computing the size (amplitude, half of the peak-to-peak amplitude)
of the EOD a parabola is fitted to the three data points around the peak
and the peak of this parabula is used as the peak size. If \a filtertau is
greater than zero the the EOD waveform is low-pass filtered with this
time constant and this mean value is subtracted from the peak size to result
in the desired amplitude.

\par Input
A single voltage trace of the periodic input waveform

\par Output
The times of the EOD peaks and their amplitude.

\par Options
- \c threshold=1mV: Threshold (\c number)
- \c adapt=false: Adapt threshold (\c boolean)
- \c ratio=50%: Ratio (\c number)
- \c autoratio=50%: Auto sets threshold relative to EOD peak-to-peak amplitude (\c number)
- \c maxperiod=100ms: Maximum EOD period for analyzing (\c number)
- \c filtertau=100ms: Filter time constant (\c number)
- \c zeroratio=25%: Time is computed from threshold crossing by ratio of peak-to-peak amplitude below peak (\c number)
- \c interpolation=linear interpolation: Method for threshold-crossing time (\c string)
- \c fitwin=40%: Fraction between threshold crossing and peak used for fit (\c number)

\par Auto Button
Clicking on the "Auto" button sets the threshold to \c autoratio of the
peak-to-peak amplitude.
*/


class EODDetector : public Filter, public EODTools
{
  Q_OBJECT

public:

  EODDetector( const string &ident="", int mode=0 );
  ~EODDetector( void );

  virtual int init( const InData &data, EventData &outevents,
		     const EventList &other, const EventData &stimuli );
  virtual void notify( void );
  virtual int adjust( const InData &data );
  virtual int autoConfigure( const InData &data, double tbegin, double tend );
    /*! Detect EODs in a single trace of the analog data \a data. */
  virtual int detect( const InData &data, EventData &outevents,
		      const EventList &other, const EventData &stimuli );

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

  void autoConfigure( void );


protected:

  Detector< InData::const_iterator, InData::const_range_iterator > D;

  double Threshold;
  double MinThresh;
  double MaxThresh;
  double ThreshRatio;
  double AutoRatio;
  double ZeroRatio;
  double FilterTau;
  int Interpolation;
  double FitWin;
  double SampleInterval;
  bool AdaptThresh;
  double MaxEODPeriod;

  OptWidget EDW;
  const InData *Data;
  InData::const_iterator FilterIterator;
  double MeanEOD;

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_EODDETECTOR_H_ */
