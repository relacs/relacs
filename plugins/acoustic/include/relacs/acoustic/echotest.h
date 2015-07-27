/*
  acoustic/echotest.h
  Check for echoes.

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

#ifndef _RELACS_ACOUSTIC_ECHOTEST_H_
#define _RELACS_ACOUSTIC_ECHOTEST_H_ 1

#include <relacs/sampledata.h>
#include <relacs/plot.h>
#include <relacs/acoustic/traces.h>
#include <relacs/repro.h>
using namespace relacs;

namespace acoustic {


/*!
\class EchoTest
\brief [RePro] Check for echoes.
\author Karin Fisch, Jan Benda
\version 1.0 (Jul 11, 2015)
\par Options
- \c duration=10ms: Stimulus duration (\c number)
- \c frequency=0kHz: Carrier frequency (\c number)
- \c intensity=80dB: Stimulus intensity (\c number)
- \c repeats=0: Repetitions of stimulus (\c integer)
- \c pause=100ms: Duration of pause between stimuli (\c number)
- \c outtrace=Left-Speaker: Output trace (\c string)
- \c intrace=Sound-1: Input trace (\c string)
- \c average=10: Number of trials to be averages (\c integer)
- \c maxecho=10m: Maximum echo distance (\c number)
- \c soundspeed=343m/s: Speed of sound (\c number)
*/


class EchoTest : public RePro, public Traces
{
  Q_OBJECT

public:

  EchoTest( void );

  virtual void preConfig( void );
  virtual int main( void );

  void plot( const SampleDataF &meanvoltage, double soundspeed );
  void save( const SampleDataF &meanvoltage, double soundspeed );
  

protected:
  
  Plot P;

};


}; /* namespace acoustic */

#endif /* ! _RELACS_ACOUSTIC_ECHOTEST_H_ */
