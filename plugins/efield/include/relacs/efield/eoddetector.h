/*
  efield/eoddetector.h
  A detector for EOD cycles of weakly electric fish

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

#ifndef _RELACS_EFIELD_EODDETECTOR_H_
#define _RELACS_EFIELD_EODDETECTOR_H_ 1

#include <relacs/optwidget.h>
#include <relacs/detector.h>
#include <relacs/filter.h>
using namespace relacs;

namespace efield {


/*!
\class EODDetector
\brief [Detector] A detector for EOD cycles of weakly electric fish
\author Jan Benda

Detects each cycle of a periodic input waveform, like the EOD of a wave-type
weakly electric fish.

The threshold defines the minimum distance between peaks and troughs that are detected.
If \a adapt is set to \c true, then the threshold is set automatically to
\a ratio times the two-fold size of the EOD amplitude.
If the a previous peak of similar size is more than \a maxperiod away,
then this peak is not regarded as an EOD cycle.

\par Input
A single voltage trace of the periodic input waveform

\par Output
The times of the EOD peaks and their amplitude.

\par Options
- \c threshold=0.1mV: Threshold (\c number)
- \c adapt=false: Adapt threshold (\c boolean)
- \c ratio=50%: Ratio (\c number)
- \c maxperiod=10ms: Maximum EOD period (\c number)
- \c averagecycles=100: Number of EOD cycles to be averaged over for computing mean voltage (\c integer)

\version 1.6 (Nov 23, 2010)
*/


class EODDetector : public Filter
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
  double FilterTau;
  bool AdaptThresh;
  double MaxEODPeriod;

  OptWidget EDW;
  const InData *Data;
  InData::const_iterator FilterIterator;
  double MeanEOD;

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_EODDETECTOR_H_ */
