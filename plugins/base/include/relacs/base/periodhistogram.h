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
\brief [RePro] Displays logarithmic histogram of dynamic clamp sampling intervals.
\author Jan Benda
\version 1.0 (Jun 12, 2018)
\par Options
- \c duration=1000ms: Width of analysis window (\c number)
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

  MultiPlot P;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_PERIODHISTOGRAM_H_ */
