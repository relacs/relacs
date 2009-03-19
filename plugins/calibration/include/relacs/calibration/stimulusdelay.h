/*
  calibration/stimulusdelay.h
  Measures delays between actual and reported onset of a stimulus

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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
\version 1.2 (Feb 8, 2008)
-# check interrupt() after each sleep
\version 1.1 (Jan 10, 2008)
-# removed stop() function
-# moved class variables to the main() functions
\version 1.0 (Dec 20, 2007)

\par Options
\arg \b intrace (\c text): Name of the input trace. 
\arg \b outtrace (\c text): Name of the output trace. 
\arg \b samplerate (\c number, \e kHz): Sampling rate used for output of stimulus.
\arg \b duration (\c number, \e ms): Duration of stimulus.
\arg \b repeats (\c integer): Number of repetitions. 

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
  virtual void config( void );
  virtual int main( void );
  int analyze( const InData &data, double duration, double pause, int count,
	       double &deltat );

protected:

  Plot P;

};


}; /* namespace calibration */

#endif /* ! _RELACS_CALIBRATION_STIMULUSDELAY_H_ */
