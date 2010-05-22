/*
  auditory/fifield.h
  Optimized measuring of f-I curves for a range of carrier frequencies.

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
\author Jan Benda

\par Options
- Frequencies
- Range
- \c freqmin=2.5kHz: Minimum stimulus frequency (\c number)
- \c freqmax=40kHz: Maximum stimulus frequency (\c number)
- \c freqstep=1kHz: Frequency step (\c number)
- \c freqfac=1.1892: Frequency factor (\c number)
- \c freqrange=: Frequency range (\c string)
- \c freqshuffle=Up: Order of frequencies (\c string)
- \c freqincrement=0: Initial increment for frequencies (\c integer)
- Optimization
- \c reducedfreqincrement=0: Reduce frequency range at frequency increments below (\c integer)
- \c reducedfreqnumber=10: Number of frequencies used for the reduced frequency range (\c integer)
- Intensities
- \c intmin=30dB SPL: Minimum stimulus intensity (\c number)
- \c intmax=100dB SPL: Maximum stimulus intensity (\c number)
- \c intstep=1dB SPL: Sound intensity step (\c number)
- \c intshuffle=Up: Order of intensities (\c string)
- \c intincrement=0: Initial increment for intensities (\c integer)
- \c singlerepeat=6: Number of immediate repetitions of a single stimulus (\c integer)
- \c blockrepeat=1: Number of repetitions of a fixed intensity increment (\c integer)
- \c repeat=1: Number of repetitions of the whole f-I curve measurement (\c integer)
- Waveform
- \c ramp=2ms: Ramp of stimulus (\c number)
- \c duration=400ms: Duration of stimulus (\c number)
- \c pause=400ms: Pause (\c number)
- \c side=left: Speaker (\c string)
- Optimization
- Baseline activity
- \c maxsilent=100: Maximum trials used for baseline activity (\c integer)
- \c silentfactor=3: Weight for standard deviation of baseline activity (\c number)
- \c resetsilent=Never: Reset estimation of baseline activity at (\c string)
- No response
- \c skippause=true: Skip pause if there is no response (\c boolean)
- \c silentintincrement=1: Skip all stimuli below not responding ones<br> at intensity increments below (\c integer)
- Slope
- \c slopeintincrement=2: Optimize slopes at intensity increments below (\c integer)
- \c minrateslope=0Hz/dB: Minimum slope of firing rate (\c number)
- \c maxratefrac=100%: Fraction of maximum firing rate above which slopes are optimized (\c number)
- \c extint=0dB SPL: Extend intensity range by (\c number)
- Analysis
- \c ratedt=1ms: Bin width for firing rate (\c number)
- \c prewidth=50ms: Window length for baseline firing rate (\c number)
- \c peakwidth=100ms: Window length for peak firing rate (\c number)
- \c sswidth=50ms: Window length for steady-state firing rate (\c number)
- \c minratefrac=30%: Fraction of maximum rate required for f-I curve analysis (\c number)
- \c setbest=true: Set results to the session variables (\c boolean)
- \c setcurves=none: F-I curves to be passed to session (\c string)

\version 1.4 (Oct 1, 2008)
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

};


}; /* namespace auditory */

#endif /* ! _RELACS_AUDITORY_FIFIELD_H_ */
