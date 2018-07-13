/*
  base/spectrumanalyzer.h
  Displays the spectrum of the voltage traces.

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

#ifndef _RELACS_BASE_SPECTRUMANALYZER_H_
#define _RELACS_BASE_SPECTRUMANALYZER_H_ 1

#include <relacs/control.h>
#include <relacs/optwidget.h>
#include <relacs/plot.h>
using namespace relacs;

namespace base {

/*!
\class SpectrumAnalyzer
\brief [Control] Displays the spectrum of a voltage trace.
\author Jan Benda
\version 1.4 (Jul 13, 2018)
\par Options
- \c intrace=V-1: Input trace (\c string)
- \c origin=before end of data: Analysis window (\c string)
- \c offset=0ms: Offset of analysis window (\c number)
- \c duration=1000ms: Width of analysis window (\c number)
- \c resolution=10Hz: Frequency resolution of power spectrum (\c number)
- \c overlap=true: Overlap FFT windows (\c boolean)
- \c window=Hanning: FFT window function (\c string)
- \c fmax=500Hz: Maximum frequency (\c number)
- \c decibel=true: Plot decibel relative to maximum (\c boolean)
- \c peak=true: Decibel relative to maximum peak (\c boolean)
- \c pmin=-50dB: Minimum power (\c number)
*/


class SpectrumAnalyzer : public Control
{
  Q_OBJECT

public:

  SpectrumAnalyzer( void );
  virtual ~SpectrumAnalyzer( void );

  virtual void preConfig( void );
  virtual void notify( void );
  virtual void main( void );


private:

  int InTrace;
  int Origin;
  double Offset;
  double Duration;
  double Resolution;
  int SpecSize;
  bool Overlap;
  double (*Window)( int j, int n );
  bool Decibel;
  bool Peak;
  double FMax;
  double PMin;

  OptWidget SW;
  Plot P;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_SPECTRUMANALYZER_H_ */
