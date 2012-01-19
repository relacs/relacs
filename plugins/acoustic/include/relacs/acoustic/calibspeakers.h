/*
  acoustic/calibspeakers.h
  Calibrating attenuation for loudspeakers.

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
\bug too many values pushed into Gains and Offsets?

Calibrates attenuators such that a full amplitude sine wave
is put out at an sound pressure level as requested by
setIntensity().
The sound pressure level is base on 
the root mean square of the sound waveform!

\par Options
- \c frequencyrange=2000..40000..1000: Frequency range (Hz) (\c string)
- \c intmin=60dB SPL: Minimum sound intensity (\c number)
- \c intrange=40dB SPL: Sound intensity range (\c number)
- \c intstep=5dB SPL: Sound intensity step (\c number)
- \c outtrace=Speaker-1: Output trace (\c string)
- \c intrace=Sound-1: Input trace (\c string)
- \c reset=false: Reset calibration for each frequency? (\c boolean)
- \c clear=true: Clear calibration table? (\c boolean)
- \c duration=400ms: Duration of stimulus (\c number)
- \c skip=10ms: Skip initial stimulus (\c number)
- \c pause=0ms: Pause (\c number)
- \c scale=1: Scale for V/Pa (\c number)

\par Files
\arg \b calibrate.dat : the calibration data (measured versus requested stimulus intensity).

\par Plots
The plot shows the measured versus the requested stimulus intensity (red circles).
The yellow line is a fit of a straight line to the data.
This line should for a successful calibration coincide with the blue 1:1 line.

\par Requirements
\arg Transdermal EOD recording (\c EODTrace2) and events (\c EODEvents2).

\version 1.1 (Aug 12, 2008)
*/


class CalibSpeakers : public RePro, public Traces
{
  Q_OBJECT

public:

    /*! Constructor. */
  CalibSpeakers( void );
    /*! Destructor. */
  virtual ~CalibSpeakers( void );

  virtual void config( void );

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
  void analyze( int intrace, double duration, double skip, double frequency,
		double soundpressurescale,
		double intensity, MapD &intensities,
		double &fitgain, double &fitoffset );

protected:

  static const int MaxGainTries = 6;
  static const int MaxNoSignalTries = 6;
  MultiPlot P;

};


}; /* namespace acoustic */

#endif /* ! _RELACS_ACOUSTIC_CALIBSPEAKERS_H_ */
