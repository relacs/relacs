/*
  auditory/fifield.h
  Optimized measuring of f-I curves for a range of carrier frequencies.

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

#ifndef _RELACS_AUDITORY_FIFIELD_H_
#define _RELACS_AUDITORY_FIFIELD_H_ 1

#include <vector>
#include <relacs/map.h>
#include <relacs/rangeloop.h>
#include <relacs/multiplot.h>
#include <relacs/auditory/ficurve.h>
using namespace std;
using namespace relacs;

namespace auditory {


/*!
\class FIField
\brief [RePro] Optimized measuring of f-I curves for a range of carrier frequencies.
\version 1.7 (Jul 23, 2017)
\author Jan Benda

\par Options
- \c Intensities
    - \c intmin=30dB SPL: Minimum stimulus intensity (\c number)
    - \c intmax=100dB SPL: Maximum stimulus intensity (\c number)
    - \c intstep=1dB SPL: Sound intensity step (\c number)
    - \c usethresh=false: Relative to the cell's threshold (\c boolean)
    - \c usesat=false: Maximum intensity relative to the cell's best saturation (\c boolean)
    - \c useprevints=no: Re-use intensities from the previous run (\c string)
    - \c intshuffle=Up: Order of intensities (\c string)
    - \c intincrement=0: Initial increment for intensities (\c integer)
    - \c singlerepeat=6: Number of immediate repetitions of a single stimulus (\c integer)
    - \c blockrepeat=1: Number of repetitions of a fixed intensity increment (\c integer)
    - \c repeats=1: Number of repetitions of the whole f-I curve measurement (\c integer)
    - \c manualskip=false: Show buttons for manual selection of intensities (\c boolean)
- \c Waveform
    - \c waveform=sine: Waveform of stimulus (\c string)
    - \c carrierfreq=5kHz: Frequency of carrier (\c number)
    - \c usebestfreq=false: Relative to the cell's best frequency (\c boolean)
    - \c ramp=2ms: Ramp of stimulus (\c number)
    - \c duration=400ms: Duration of stimulus (\c number)
    - \c pause=400ms: Pause (\c number)
    - \c side=left: Speaker (\c string)
- \c Optimization
    - \c Baseline activity
        - \c usesilent=false: Use measured baseline activity (\c boolean)
        - \c maxbaserate=10Hz: Maximum baseline activity (\c number)
        - \c maxsilent=100: Maximum trials used for baseline activity (\c integer)
        - \c resetsilent=Never: Reset estimation of baseline activity at (\c string)
        - \c silentfactor=3: Weight for standard deviation of baseline activity (\c number)
    - \c No response
        - \c skippause=true: Skip pause if there is no response (\c boolean)
        - \c silentintincrement=1: Skip all stimuli below not responding ones<br> at intensity increments below (\c integer)
    - \c Slope
        - \c slopeintincrement=2: Optimize slopes at intensity increments below (\c integer)
        - \c minrateslope=0Hz/dB: Minimum slope of firing rate (\c number)
        - \c maxratefrac=100%: Fraction of maximum firing rate above which slopes are optimized (\c number)
        - \c extint=0dB SPL: Extend intensity range by (\c number)
- \c Analysis
    - \c ratedt=1ms: Bin width for firing rate (\c number)
    - \c prewidth=50ms: Window length for baseline firing rate (\c number)
    - \c peakwidth=100ms: Window length for peak firing rate (\c number)
    - \c sswidth=50ms: Window length for steady-state firing rate (\c number)
    - \c plotonssrate=true: Plot onset- and steady-state rate (\c boolean)
    - \c adjust=true: Adjust input gain (\c boolean)
    - \c setbest=true: Set results to the session variables (\c boolean)
    - \c setcurves=none: F-I curves to be passed to session (\c string)

\par Plots 
The left plot shows the sensitivity of the cell as a function of
carrier frequency. At each carrier frequency a straight line is fitted
into the dynamic range of the f-I curve. The intersection of this line
with zero is the threshold of the cell (yellow line). The intersection
with the maximum firing rate of the f-I curve is the red line, and the
intersection with the "best rate" from the session dialog is the
orange line.

The right plot shows the f-I curve of the currently measured carrier
frequency.  Cyan is the firing rate before the stimulus, orange is the
mean firing rate (spike count divided by stimulus duration), red is
the steady-state firing rate, and green is the onset spike frequency
(mean max spike frequency).
*/


class FIField : public FICurve
{
  Q_OBJECT

public:

    /*! Constructor. */
  FIField( void );
    /*! Destructor. */
  ~FIField( void );

    /*! Read options, create stimulus and start output of stimuli. */
  virtual int main( void );

  void saveHeader( const string &file );
  void saveThreshold( const string &file );
  void save( void );

    /*! Plot data. */
  virtual void plot( const vector< FIData > &results );
    /*! Set next stimulus. */
  virtual DoneState next( vector< FIData > &results, bool msg=true );


protected:

  double MinFreq;
  double MaxFreq;
  double FreqStep;
  double FreqFac;
  string FreqStr;
  RangeLoop::Sequence FreqShuffle;
  int FreqIncrement;
  RangeLoop FrequencyRange;
  int ReducedFreqIncrement;
  int ReducedFreqNumber;

  vector < ThreshData > FieldData;
  int BestIndex;
  MapD BestFICurve;
  MapD BestOnFICurve;
  MapD BestSSFICurve;

  double MinRateFrac;
  double MaxRate;

  Options FIFieldHeader;

  MapD Field;

};


}; /* namespace auditory */

#endif /* ! _RELACS_AUDITORY_FIFIELD_H_ */
