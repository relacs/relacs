/*
  efield/eodmodel.h
  EOD with interruptions or chirps

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

#ifndef _RELACS_EFIELD_EODMODEL_H_
#define _RELACS_EFIELD_EODMODEL_H_ 1

#include <relacs/model.h>
using namespace relacs;

namespace efield {


/*!
\class EODModel
\brief [Model] EOD with interruptions or chirps
\author Jan Benda
\version 1.0 (Feb 03, 2013)
\par Options
- \c eodtype=Sine: EOD type (\c string)
- \c amplitude=1.0mV/cm: Amplitude (\c number)
- \c frequency=1000Hz: Frequency (\c number)
- \c freqsd=10Hz: Standard deviation of frequency modulation (\c number)
- \c freqtau=1000s: Timescale of frequency modulation (\c number)
- \c interrupt=false: Add interruptions (\c boolean)
- \c interruptduration=100ms: Duration of interruption (\c number)
- \c interruptamplitude=0%: Amplitude fraction of interruption (\c number)
*/


class EODModel : public Model
{
  Q_OBJECT

public:

  EODModel( void );
  virtual void main( void );
  virtual void process( const OutData &source, OutData &dest );

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_EODMODEL_H_ */
