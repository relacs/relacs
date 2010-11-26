/*
  base/transferfunction.h
  Measures the transfer function with white-noise stimuli.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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
\version 1.1 (Nov 25, 2010)
\par Screenshot
\image html transferfunction.png

\par Options
- Stimulus
- \c outtrace=Current-1: Output trace (\c string)
- \c amplitude=1nA: Amplitude (\c number)
- \c fmax=1000Hz: Maximum frequency (\c number)
- \c duration=1000ms: Width of analysis window (\c number)
- \c pause=1000ms: Length of pause inbetween successive stimuli (\c number)
- \c repeats=100: Repeats (\c integer)
- Analysis
- \c intrace=V-1: Input trace (\c string)
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
		double duration, int count,
		SampleDataF &input, SampleDataF &output );
  void openTraceFile( ofstream &tf, TableKey &tracekey, const Options &header );
  void saveTrace( ofstream &tf, TableKey &tracekey, int index,
		  const SampleDataF &input, const SampleDataF &output );
  void saveData( const Options &header );

  int SpecSize;
  bool Overlap;
  double (*Window)( int j, int n );

  string InName;
  string InUnit;
  string OutName;
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
