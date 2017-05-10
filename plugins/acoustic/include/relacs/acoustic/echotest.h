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
\version 1.2 (May 10, 2017)

Repeatedly play a signal with an triangular amplitude modulation and
average the voltage output of a microphone. The averaged sound is then
plotted as a function of distance the sound must have travelled. This
way echos can be inspected.

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

\par Plot 

The plot shows the averaged sound (orange) recorded by the microphone
as a function of distance the sound must have travelled. The x-axis is
shifted such that the peak of the triangular sound pulse indicates the
distances. Without any delays the peak amplitude of the recorded sound
will sit at distance zero (not the beginning of the sound pulse).

For orientation the original sound pulse is plotted in yellow at the
position of the largest peak.
*/


class EchoTest : public RePro, public Traces
{
  Q_OBJECT

public:

  EchoTest( void );

  virtual void preConfig( void );
  virtual int main( void );

  void plot( const SampleDataF &meanvoltage, const OutData &signal, double soundspeed );
  void save( const SampleDataF &meanvoltage, double soundspeed );
  

protected:
  
  Plot P;

};


}; /* namespace acoustic */

#endif /* ! _RELACS_ACOUSTIC_ECHOTEST_H_ */
