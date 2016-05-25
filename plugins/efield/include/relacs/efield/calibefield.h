/*
  efield/calibefield.h
  Calibrates an attenuator for electric field stimuli.

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
\version 2.2 (Nov 30, 2013)
\note Intensity for attenuation is interpreted as 0.5 * p-p amplitude
\note Calibration non-sine EODs has an offset!
\par Options
- \c reset=false: Reset calibration? (\c boolean)
- \c resetval=0.1: Reset gain factor to (\c number)
- \c am=false: Calibrate amplitude modulation? (\c boolean)
- \c beatfreq=20Hz: Beat frequency to be used when fish EOD present (\c number)
- \c frequency=600Hz: Stimulus frequency to be used when no fish EOD is present (\c number)
- \c duration=400ms: Duration of stimulus (\c number)
- \c pause=0ms: Pause (\c number)
- \c amplsel=contrast: Calibrate for (\c string)
- \c targetcontrast=20%: Target contrast to be tested first (\c number)
- \c mincontrast=10%: Minimum contrast (\c number)
- \c maxcontrast=25%: Maximum contrast (\c number)
- \c targetamplitude=1mV/cm: Target contrast to be tested first (\c number)
- \c minamplitude=0.5mV/cm: Minimum amplitude (\c number)
- \c maxamplitude=2mV/cm: Maximum amplitude (\c number)
- \c numintensities=10: Number of intensities (amplitudes) to be measured (\c integer)

\par Files
- \b calibrate.dat : the calibration data (measured versus requested stimulus intensity).

\par Plots
The plot shows the measured versus the requested stimulus intensity (red circles).
The yellow line is a fit of a straight line to the data.
In case of a successful calibration this line should coincide
with the blue line.
The horizontal white line is the target intensity.

\par Requirements
- An EOD recording (\c GlobalEOD, LocalEOD, FishEOD), that is to be calibrated, 
  and corresponding events.
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
  void plotIntensities( const MapD &intensities, double targetintensity, double maxx );
    /*! Analyze data. */
  int analyze( const InData &eodtrace, double duration,
	       double beatfrequency, bool usecontrast,
	       double mincontrast, double maxcontrast, double minamplitude, double maxamplitude,
	       double intensity, bool fish, double &amplitude );


private:

  double FitGain;
  double FitOffset;
  string EODUnit;
  Plot P;

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_CALIBEFIELD_H_ */
