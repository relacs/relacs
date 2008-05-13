/*
  acoustic/calibspeakers.h
  Calibrating attenuation for loudspeakers.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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
\brief Calibrating attenuation for loudspeakers.
\author Jan Benda
\version 1.1 (Jan 10, 2008)
-# removed stop() function
-# moved class variables to the main() functions
\version 1.0 (Sep 1, 2004)
\bug too many values pushed into Gains and Offsets?

Calibrates attenuators such that a full amplitude sine wave
is put out at an sound pressure level as requested by
setIntensity().
The sound pressure level is base on 
the root mean square of the sound waveform!

\par Options
\arg \b reset (\c boolean): Reset calibration, i.e. do not use old calibration results.
\arg \b am (\c boolean): Use amplitude modulation signal instead of sine wave.
\arg \b frequency (\c number, \e Hz): If there is no fish EOD, then use sine wave with this frequency as a calibration stimulus.
\arg \b beatfreq (\c number, \e Hz): If there is a fish EOD, then use calibration stimulus that results in this beat frequency.
\arg \b duration (\c number, \e ms): Duration of calibration stimulus.
\arg \b pause (\c number, \e ms): %Pause between successive stimuli.
\arg \b maxcontrast (\c number, \e %): Maximum contrast (beat amplitude / EOD amplitude) to be used.
\arg \b maxint (\c integer): Maximum number of test intensities.
\arg \b minintensity (\c number, \e %): Minimum stimulus intensity relative to EOD amplitude.
\arg \b repeats (\c integer): Maximum repetitions of the calibration protocol.

\par Files
\arg \b calibrate.dat : the calibration data (measured versus requested stimulus intensity).

\par Plots
The plot shows the measured versus the requested stimulus intensity (red circles).
The yellow line is a fit of a straight line to the data.
This line should for a successful calibration coincide with the blue 1:1 line.

\par Requirements
\arg Transdermal EOD recording (\c EODTrace2) and events (\c EODEvents2).
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

  Options Settings;

};


}; /* namespace acoustic */

#endif /* ! _RELACS_ACOUSTIC_CALIBSPEAKERS_H_ */
