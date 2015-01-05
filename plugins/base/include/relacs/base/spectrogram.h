/*
  base/spectrogram.h
  Displays a spectrogram of an input trace.

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

#ifndef _RELACS_BASE_SPECTROGRAM_H_
#define _RELACS_BASE_SPECTROGRAM_H_ 1

#include <relacs/plot.h>
#include <relacs/repro.h>
using namespace relacs;

namespace base {


/*!
\class Spectrogram
\brief [RePro] Displays a spectrogram of an input trace.
\author Jan Benda
\version 1.0 (Feb 10, 2013)

The Spectrogram-%RePro records data without writing out any stimulus
and displays a spectrogram of an input trace.
The %RePro terminates after \c duration seconds. If \c duration is
set to zero, the %RePro keeps running infinitely. Recording can be
interrupted any time by pressing the space key or starting a different
%RePro.

A powerspectrum is computed every \c step second on a \c width seconds
long chunk of data. If \c step is set to zero, it is set to \c width.
The powerspectrum is averaged over ffts that use \c specsize
datapoints, overlap by half if \c overlap is \c true, and are windowed
using the window function \a window.

The resulting spectrogram is displayed for the last \a tmax
seconds. The default maximum frequency shown is \c fmax. The color
code displays all values between \c pmin and \c pmax decibel of the
maximum possible power in the spectrum. The maximum power is either
set to the maximum power in each powerspectrum (\c powermax is set to
\c true) or to the maximum possible power of the input trace range.

\par Options
- \c intrace=V-1: Input trace (\c string)
- \c width=100ms: Width of powerspectrum  window (\c number)
- \c step=0ms: Temporal increment for powerspectrum window (\c number)
- \c tmax=10s: Maximum time to plot in spectrogram (\c number)
- \c duration=0s: Total duration of spectrogram (\c number)
- \c size=1024: Number of data points for FFT (\c string)
- \c overlap=true: Overlap FFT windows within single powerspectrum (\c boolean)
- \c window=Hanning: FFT window function (\c string)
- \c powermax=true: Power relative to maximum (\c boolean)
- \c fmax=2000Hz: Maximum frequency (\c number)
- \c pmax=0dB: Maximum power (\c number)
- \c pmin=-50dB: Minimum power (\c number)
*/


class Spectrogram : public RePro
{
  Q_OBJECT

public:

  Spectrogram( void );
  virtual void preConfig( void );
  virtual int main( void );


private:

  Plot P;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_SPECTROGRAM_H_ */
