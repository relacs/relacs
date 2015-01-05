/*
  base/transferfunction.h
  Measures the transfer function with white-noise stimuli.

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
\version 1.6 (Dec 1, 2014)
\par Screenshot
\image html transferfunction.png

\par Options
- \c Stimulus
    - \c outtrace=V-1: Output trace (\c string)
    - \c offsetbase=custom: Set offset relative to (\c string)
    - \c offset=0V: Offset (\c number)
    - \c amplitude=1V: Amplitude (\c number)
    - \c clip=4: Multiple of amplitude where to clip signal (\c number)
    - \c intensity=1: Intensity for an attenuator (\c number)
    - \c fmax=1000Hz: Maximum frequency (\c number)
    - \c duration=1000ms: Duration of noise stimulus (\c number)
    - \c pause=1000ms: Length of pause inbetween successive stimuli (\c number)
    - \c repeats=100: Repeats (\c integer)
- \c Analysis
    - \c intrace=V-1: Input trace (\c string)
    - \c size=1024: Number of data points for FFT (\c string)
    - \c overlap=true: Overlap FFT windows (\c boolean)
    - \c window=Hanning: FFT window function (\c string)
    - \c plotstdevs=true: Plot standard deviations (\c boolean)
    - \c plotcoherence=true: Plot coherence (\c boolean)
*/


class TransferFunction : public RePro
{
  Q_OBJECT

public:

  TransferFunction( void );
  virtual void preConfig( void );
  virtual void notify( void );
  virtual int main( void );


protected:

  void analyze( const SampleDataF &input, const SampleDataF &output,
		double duration, int count );
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

  SampleDataD MeanCoherence;
  SampleDataD SquareCoherence;
  SampleDataD StdevCoherence;

  MultiPlot P;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_TRANSFERFUNCTION_H_ */
