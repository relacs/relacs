/*
  auditory/ficurve.h
  Optimized measuring of f-I curves.

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

#ifndef _RELACS_AUDITORY_FICURVE_H_
#define _RELACS_AUDITORY_FICURVE_H_ 1

#include <relacs/sampledata.h>
#include <relacs/rangeloop.h>
#include <relacs/multiplot.h>
#include <relacs/ephys/traces.h>
#include <relacs/acoustic/traces.h>
#include <relacs/repro.h>
using namespace relacs;

namespace auditory {


/*!
\class FICurve
\brief [RePro] Optimized measuring of f-I curves.
\author Jan Benda
\bug Need to sleep 10ms longer to prevent busy error!

Features:
- Shorter pauses when there is no response.

\par Options
- Intensities
- \c intmin=30dB SPL: Minimum stimulus intensity (\c number)
- \c intmax=100dB SPL: Maximum stimulus intensity (\c number)
- \c intstep=1dB SPL: Sound intensity step (\c number)
- \c usebestthresh=false: Relative to the cell's best threshold (\c boolean)
- \c usebestsat=false: Maximum intensity relative to the cell's best saturation (\c boolean)
- \c intshuffle=Up: Order of intensities (\c string)
- \c intincrement=0: Initial increment for intensities (\c integer)
- \c singlerepeat=6: Number of immediate repetitions of a single stimulus (\c integer)
- \c blockrepeat=1: Number of repetitions of a fixed intensity increment (\c integer)
- \c repeat=1: Number of repetitions of the whole f-I curve measurement (\c integer)
- \c manualskip=false: Show buttons for manual selection of intensities (\c boolean)
- Waveform
- \c waveform=sine: Waveform of stimulus (\c string)
- \c carrierfreq=5kHz: Frequency of carrier (\c number)
- \c usebestfreq=false: Use the cell's best frequency (\c boolean)
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
- \c setbest=true: Set results to the session variables (\c boolean)
- \c setcurves=none: F-I curves to be passed to session (\c string)

\par Plots 
If you have selected \c manualskip then move the mouse into the top
part of the f-I curve plot. This brings up a line of red and green
buttons, each one corresponding to an intensity value.
Green buttons indicated intensities that are measured,
red buttons indicate skipped (not measured) intensitties.
Left click on a button to toggle measurement of the corresponding intensity value.
Simultaneously holding down the \c shift key toggels all intensities below the 
selected one.
Holding down the \c ctrl key toggels all intensities above the selected one.

\version 1.4 (Oct 1, 2008)
*/


class FICurve : public RePro, public ephys::Traces, public acoustic::Traces
{
  Q_OBJECT

public:

    /*! Constructor. */
  FICurve( void );
    /*! Destructor. */
  ~FICurve( void );

  virtual int main( void );
    /*! Initialize on start session. */
  virtual void init( void );


protected slots:

  void plotMouseEvent( Plot::MouseEvent &me );


protected:

  struct FIData {
    FIData( double p, double d, double dt ) : Rate( p, d, dt ),
	 RateSD( p, d, dt ), MaxPlotRate( 50.0 ) {};
    SampleDataD Rate;
    SampleDataD RateSD;
    double Intensity;
    double TrueIntensity;
    double MaxPlotRate;
    double PreRate;
    double PreRateSD;
    double OnRate;
    double OnRateSD;
    double OnTime;
    double SSRate;
    double SSRateSD;
    double MeanRate;
    double MeanRateSD;
    double Count;
    double CountSD;
    double Latency;
    double LatencySD;
    EventList Spikes;
  };

  void saveSpikes( const string &file, const vector< FIData > &results );
  void saveRates( const string &file, const vector< FIData > &results );
  void saveFICurve( const string &file, const vector< FIData > &results );
  void analyzeFICurve( const vector< FIData > &results, double minrate=0.0 );
  void setHeader( void );
  void updateSession( const vector< FIData > &results );
  void save( const vector< FIData > &results );

  void plotIntensitySelection( void );
    /*! Plot data. */
  virtual void plot( const vector< FIData > &results );
    /*! Analyze data. */
  virtual void analyze( vector< FIData > &results );
    /*! Set next stimulus. */
  virtual DoneState next( vector< FIData > &results, bool msg=true );

    /*! The main stimulus loop. */
  int loop( vector< FIData > &results );

  void silentActivity( void );
  void resetSilentActivity( void );

  double MinIntensity;
  double MaxIntensity;
  double IntensityStep;
  bool UseBestThresh;
  bool UseBestSaturation;
  RangeLoop::Sequence IntShuffle;
  int IntIncrement;
  int SingleRepeat;
  int IntBlockRepeat;
  int IntRepeat;
  double CarrierFrequency;
  bool UseBestFreq;
  int Waveform;
  double Ramp;
  int Side;
  double Duration;
  double Pause;
  bool SkipPause;
  int SkipSilentIncrement;
  int SlopeIntIncrement;
  double MinRateSlope;
  double MaxRateFrac;
  double IntensityExtension;
  int MaxSilent;
  double SilentFactor;
  int ResetSilent;
  double RateDt;
  double PreWidth;
  double PeakWidth;
  double SSWidth;
  bool SetBest;
  int SetCurves;

  RangeLoop IntensityRange;
  double Intensity;
  OutData Signal;
  double MeanIntensity;
  double MinCarrierFrequency;
  int FICurveStops;

  int NSilent;
  double SilentRate;
  double LastSilentRate;
  double SilentRateSq;
  double SilentRateSD;
  double MaxSilentRate;
  double MaxPlotRate;
  MultiPlot P;
  bool PlotIntensitySelection;
  Options Settings;
  Options Header;

  struct ThreshData {
    ThreshData( void ) { reset(); };
    void reset( void ) 
    {
      Frequency = 0.0;
      Threshold = -1.0;
      ThresholdSD = -1.0;
      Slope = -1.0;
      SlopeSD = -1.0;
      RateIntensity = -1.0;
      RateIntensitySD = -1.0;
      Saturation = -1.0;
      SaturationSD = -1.0;
      MaxRate = -1.0;
      MaxRateSD = -1.0;
      N = 0;
      Measured = false;
    }
    double Frequency;
    double Threshold;
    double ThresholdSD;
    double Slope;
    double SlopeSD;
    double RateIntensity;
    double RateIntensitySD;
    double Saturation;
    double SaturationSD;
    double MaxRate;
    double MaxRateSD;
    int N;
    bool Measured;
  };
  ThreshData Threshold;

};


}; /* namespace auditory */

#endif /* ! _RELACS_AUDITORY_FICURVE_H_ */
