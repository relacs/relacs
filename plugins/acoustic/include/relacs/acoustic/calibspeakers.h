/*
  acoustic/calibspeakers.h
  Calibrating attenuation for loudspeakers.

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

#ifndef _RELACS_ACOUSTIC_CALIBSPEAKERS_H_
#define _RELACS_ACOUSTIC_CALIBSPEAKERS_H_ 1

#include <relacs/repro.h>
#include <relacs/acoustic/traces.h>
#include <relacs/rangeloop.h>
#include <relacs/multiplot.h>
#include <relacs/acoustic/loudspeaker.h>
using namespace relacs;

namespace acoustic {


/*!
\class CalibSpeakers
\brief [RePro] Calibrating attenuation for loudspeakers.
\author Jan Benda
\version 1.2 (Jul 5, 2017)

Calibrates attenuators such that a full amplitude sine wave
is put out at an sound pressure level as requested by
setIntensity().
The sound pressure level is based on 
the root mean square of the sound waveform!

\par Options
- \c frequencyrange=2000..20000..500: Frequency range (Hz) (\c string)
- \c intmin=45dB SPL: Minimum sound intensity (\c number)
- \c intrange=35dB SPL: Sound intensity range (\c number)
- \c intstep=5dB SPL: Sound intensity step (\c number)
- \c outtrace=Left-Speaker: Output trace (\c string)
- \c intrace=Sound-1: Input trace (\c string)
- \c reset=false: Reset calibration for each frequency? (\c boolean)
- \c clear=false: Clear calibration table? (\c boolean)
- \c duration=80ms: Duration of stimulus (\c number)
- \c skip=20ms: Initial time to skip for analysis (\c number)
- \c win=1ms: Window for computing rms response (\c number)
- \c ramp=1ms: Ramp time of stimulus (\c number)
- \c pause=0ms: Pause between stimuli (\c number)
- \c scale=3.16: Scale for V/Pa (\c number)

\par Files
\arg \b calibspeakers-intensities.dat : the calibration data (measured versus requested stimulus intensity).
\arg \b calibspeakers-offsets.dat : the calibration data (measured versus requested stimulus intensity).

\par Plots
The plot shows the measured versus the requested stimulus intensity (red circles).
The yellow line is a fit of a straight line to the data.
This line should for a successful calibration coincide with the blue 1:1 line.

\par Requirements
\arg The LoadSpeaker Attenuate class associated with output trace Speaker-Left or Speaker-Right
\arg Microphone recording (e.g. a Sound-1 trace)
*/


class CalibSpeakers : public RePro, public Traces
{
  Q_OBJECT

public:

    /*! Constructor. */
  CalibSpeakers( void );
    /*! Destructor. */
  virtual ~CalibSpeakers( void );

  virtual void preConfig( void );
  virtual int main( void );

    /*! Save data. */
  void saveIntensities( double frequency, const MapD &intensities,
			double fitgain, double fitoffset );
  void saveOffsets( const MapD &offsets, const ArrayD &gains );
    /*! Plot data. */
  void plot( double minintensity, double intensityrange,
	     const MapD &intensities, double fitgain, double fitoffset,
	     const MapD &oldoffsets, const MapD &offsets );
    /*! Analyze data. */
  void analyze( int intrace, double duration, double skip, double win,
		double ramp, double frequency, double soundpressurescale,
		double intensity, MapD &intensities,
		double &fitgain, double &fitoffset );

protected:

  static const int MaxGainTries = 6;
  static const int MaxNoSignalTries = 6;
  MultiPlot P;

};


}; /* namespace acoustic */

#endif /* ! _RELACS_ACOUSTIC_CALIBSPEAKERS_H_ */
