/*
  ephys/iontophoresis.h
  Current injection for iontophoresis

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

#ifndef _RELACS_EPHYS_IONTOPHORESIS_H_
#define _RELACS_EPHYS_IONTOPHORESIS_H_ 1

#include <QLabel>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace relacs;

namespace ephys {


/*!
\class Iontophoresis
\brief [RePro] Current injection for iontophoresis
\author Jan Benda
\version 1.2 (Oct 9, 2014)

\par Screenshot
\image html iontophoresis.png

\par Options
- \c durationpos=1s: Duration of positive current (\c number)
- \c amplitudepos=1nA: Amplitude of positive current (\c number)
- \c pausepos=1s: Pause after positive current (\c number)
- \c durationneg=1s: Duration of negative current (\c number)
- \c amplitudeneg=1nA: Amplitude of negative current (\c number)
- \c pauseneg=1s: Pause after negative current (\c number)
- \c fortunes=true: Display fortunes (\c boolean)
- \c fortuneperiod=10s: Period for displaying fortunes (\c number)
*/


class Iontophoresis : public RePro, public Traces
{
  Q_OBJECT

public:

  Iontophoresis( void );
  void preConfig( void );
  virtual int main( void );


protected:

  virtual void customEvent( QEvent *qce );

  string IUnit;
  QLabel *PosTimeLabel;
  QLabel *PosTime;
  QLabel *NegTimeLabel;
  QLabel *NegTime;
  QLabel *FortuneText;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_IONTOPHORESIS_H_ */
