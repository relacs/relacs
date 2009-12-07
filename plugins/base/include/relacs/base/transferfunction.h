/*
  base/transferfunction.h
  Measures the transfer function with white-noise stimuli.

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

#ifndef _RELACS_BASE_TRANSFERFUNCTION_H_
#define _RELACS_BASE_TRANSFERFUNCTION_H_ 1

#include <relacs/multiplot.h>
#include <relacs/repro.h>
using namespace relacs;

namespace base {


/*!
\class TransferFunction
\brief [RePro] Measures the transfer function with white-noise stimuli.
\author Jan Benda
\version 1.0 (Dec 07, 2009)


\par Options
- \c outtrace=0: Output trace number (\c number)
- \c duration=1000ms: Stimulus duration (\c number)
- \c pause=1000ms: Length of pause inbetween successive stimuli (\c number)
- \c fmax=1000Hz: Maximum frequency of stimulus (\c number)
- \c repeats=10: Repetitions of stimulus (\c integer)
- \c intrace=0: Input trace number (\c number)
- \c size=1024: Number of data points for FFT (\c string)
- \c overlap=true: Overlap FFT windows (\c boolean)
- \c window=Hanning: FFT window function (\c string)
*/


class TransferFunction : public RePro
{
  Q_OBJECT

public:

  TransferFunction( void );
  virtual void config( void );
  virtual void notify( void );
  virtual int main( void );


protected:

  void analyze( const OutData &signal, const InData &data,
		double duration, int count );

  int SpecSize;
  bool Overlap;
  double (*Window)( int j, int n );

  string InUnit;
  string OutUnit;

  SampleDataD MeanGain;
  SampleDataD SquareGain;
  SampleDataD StdevGain;

  SampleDataD MeanPhase;
  SampleDataD SquarePhase;
  SampleDataD StdevPhase;

  MultiPlot P;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_TRANSFERFUNCTION_H_ */
