/*
  efield/calibefield.h
  Calibrates an attenuator for electric field stimuli.

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

#ifndef _RELACS_EFIELD_CALIBEFIELD_H_
#define _RELACS_EFIELD_CALIBEFIELD_H_ 1

#include <relacs/multiplot.h>
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
\version 1.3 (Dec 03, 2009)
\bug if slope == 0 increase Gain by factor 10.
\bug if fish, then set Duration and BeatFrequency so that we get at least 6 beats!

\par Options
- \b reset (\c boolean): Reset calibration, i.e. do not use old calibration results.
- \b am (\c boolean): Use amplitude modulation signal instead of sine wave.
- \b frequency (\c number, \e Hz): If there is no fish EOD, then use sine wave with this frequency as a calibration stimulus.
- \b beatfreq (\c number, \e Hz): If there is a fish EOD, then use calibration stimulus that results in this beat frequency.
- \b duration (\c number, \e ms): Duration of calibration stimulus.
- \b pause (\c number, \e ms): %Pause between successive stimuli.
- \b maxcontrast (\c number, \e %): Maximum contrast (beat amplitude / EOD amplitude) to be used.
- \b maxint (\c integer): Maximum number of test intensities.
- \b minintensity (\c number, \e %): Minimum stimulus intensity relative to EOD amplitude.
- \b repeats (\c integer): Maximum repetitions of the calibration protocol.

\par Files
- \b calibrate.dat : the calibration data (measured versus requested stimulus intensity).

\par Plots
The plot shows the measured versus the requested stimulus intensity (red circles).
The yellow line is a fit of a straight line to the data.
This line should for a successful calibration coincide with the blue 1:1 line.

\par Requirements
- Transdermal EOD recording (\c EODTrace2) and events (\c EODEvents2).
*/


class CalibEField : public RePro, public Traces, public EODTools
{
  Q_OBJECT

public:

  CalibEField( void );
  ~CalibEField( void );

  virtual int main( void );
  void stop( void );
  void saveData( void );
  void save( void );

    /*! Plot data. */
  void plot( void );
    /*! Analyze data. */
  void analyze( void );


private:

  base::LinearAttenuate *LAtt;
  bool Reset;
  double OrigGain;
  double OrigOffset;

  bool AM;
  double Frequency;
  double BeatFrequency;
  double Duration;
  double Pause;
  int Repeats;
  double MaxContrast;
  int MaxIntensities;
  double MinIntensityFrac;

  bool Fish;
  double FishRate;
  double FishAmplitude;

  double FitGain;
  double FitOffset;
  int FitFlag;
  double Amplitude;
  int RepeatCount;
  int IntensityCount;
  int IntensitiesOffs;
  string EOD2Unit;
  double MaxSignal;
  double IntensityStep;
  double Intensity;
  MapD Intensities;
  MultiPlot P;

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_CALIBEFIELD_H_ */
