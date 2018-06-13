/*
  base/periodhistogram.h
  Displays logarithmic histogram of dynamic clamp sampling intervals.

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

#ifndef _RELACS_BASE_PERIODHISTOGRAM_H_
#define _RELACS_BASE_PERIODHISTOGRAM_H_ 1

#include <relacs/control.h>
#include <relacs/multiplot.h>
using namespace relacs;

namespace base {


/*!
\class PeriodHistogram
\brief [Control] Displays logarithmic histogram of dynamic clamp sampling intervals.
\author Jan Benda
\version 1.0 (Jun 12, 2018)
\par Screenshot
\image html periodhistogram.png

\par Plots

The top plot shows the histogram of the sampling intervals measured
over the last `duration` seconds. Via the `maxjitter` option you can
set the maximum width of the histogram. Note the logarithmic y-axis.
For every unit the number of events increases ten-fold. 0 are single
events, 1 are 10, 2 are 100, etc.

The bottom plot shows the cumulative jitter, i.e. the devition of the
interval from the the actual sampling interval. The negative jitters,
i.e. the intervals shorter than the sampling interval, are shown in
orange. The intervals longer than sampling interval are shown in
yellow.  Note the logarithmic axes! On the x-axis -3 is 1 nanosecond,
0 is one microsecond. On the y-axis 0 is 100%, -1 is 10%, -2 is 1% and
so on.

\par Options
- \c duration=1000ms: Width of analysis window (\c number)
- \c maxjitter=10us: Maximum jitter shown in histogram (\c number)
*/


class PeriodHistogram : public Control
{
  Q_OBJECT

public:

  PeriodHistogram( void );

  virtual void notify( void );
  virtual void main( void );


private:

  double Duration;
  double MaxJitter;

  MultiPlot P;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_PERIODHISTOGRAM_H_ */
