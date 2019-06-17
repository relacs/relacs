/*
  calibration/stimulusdelay.h
  Measures delays between actual and reported onset of a stimulus

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

#ifndef _RELACS_CALIBRATION_STIMULUSDELAY_H_
#define _RELACS_CALIBRATION_STIMULUSDELAY_H_ 1

#include <relacs/plot.h>
#include <relacs/repro.h>
using namespace relacs;

namespace calibration {


/*!
\class StimulusDelay
\brief [RePro] Measures delays between actual and reported onset of a stimulus
\author Jan Benda
\version 2.2 (Dec 13, 2018)

\par Options
- \c intrace=V-1: Input trace (\c string)
- \c outtrace=V-1: Output trace (\c string)
- \c intensity=1: Intensity for an attenuator (\c number)
- \c samplerate=10kHz: Sampling rate of output (\c number)
- \c duration=10ms: Duration of output (\c number)
- \c pause=50ms: Pause between outputs (\c number)
- \c repeats=100: Repeats (\c integer)
- \c setdelay=none: Set signal delay of analog output device (\c string)

\par Files
\arg No output files.

\par Plots
\arg The read in stimulus aligned to the reported stimulus onset.

\par Requirements
\arg The output must be connected to the input.
*/


class StimulusDelay : public RePro
{
  Q_OBJECT

public:

  StimulusDelay( void );
  virtual void preConfig( void );
  virtual int main( void );
  int analyze( const InData &data, double duration, double pause, int count,
	       double &meandeltat, double &meansquaredeltat,
	       double &mindeltat, double &maxdeltat );

protected:

  Plot P;

};


}; /* namespace calibration */

#endif /* ! _RELACS_CALIBRATION_STIMULUSDELAY_H_ */
