/*
  efield/calibefield.h
  Calibrates an attenuator for electric field stimuli.

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

#ifndef _RELACS_EFIELD_CALIBEFIELD_H_
#define _RELACS_EFIELD_CALIBEFIELD_H_ 1

#include <relacs/plot.h>
#include <relacs/base/linearattenuate.h>
#include <relacs/efield/traces.h>
#include <relacs/efield/eodtools.h>
#include <relacs/repro.h>
using namespace relacs;

namespace efield {


/*!
\class CalibEField
\brief [RePro] Calibrates an attenuator for electric field stimuli.
\author Jan Benda
\version 2.2 (Oct 30, 2013)

\par Options
- \b reset (\c boolean): Reset calibration, i.e. do not use old calibration results.
- \b am (\c boolean): Use amplitude modulation signal instead of sine wave.
- \b frequency (\c number, \e Hz): If there is no fish EOD, then use sine wave with this frequency as a calibration stimulus.
- \b beatfreq (\c number, \e Hz): If there is a fish EOD, then use calibration stimulus that results in this beat frequency.
- \b duration (\c number, \e ms): Duration of calibration stimulus.
- \b pause (\c number, \e ms): %Pause between successive stimuli.
- \b targetcontrast (\c number, \e %): Target contrast to be tested first.
- \b maxcontrast (\c number, \e %): Maximum contrast (beat amplitude / EOD amplitude) to be used.
- \b maxint (\c integer): Maximum number of test intensities.
- \b minintensity (\c number, \e %): Minimum stimulus intensity relative to EOD amplitude.

\par Files
- \b calibrate.dat : the calibration data (measured versus requested stimulus intensity).

\par Plots
The plot shows the measured versus the requested stimulus intensity (red circles).
The yellow line is a fit of a straight line to the data.
In case of a successful calibration this line should coincide
with the blue line.

\par Requirements
- Local EOD recording (\c LocalEOD), that is to be calibrated, 
  and corresponding events (\c LocalEOD).
*/


class CalibEField : public RePro, public Traces, public EODTools
{
  Q_OBJECT

public:

  CalibEField( void );
  ~CalibEField( void );

  virtual int main( void );
  void saveData( const MapD &intensities, const base::LinearAttenuate *latt );

    /*! Plot data. */
  void plotGain( const MapD &gainamplitudes, double targetintensity );
  void plotIntensities( const MapD &intensities, double maxx );
    /*! Analyze data. */
  int analyze( const InData &eodtrace, double duration,
	       double beatfrequency, double mincontrast, double maxcontrast,
	       double intensity, bool fish, double &amplitude );


private:

  double FitGain;
  double FitOffset;
  string EODUnit;
  Plot P;

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_CALIBEFIELD_H_ */
