/*
  isoresponse.h
  Measures f-I-curves for a superposition of two sine waves.

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

#ifndef _ISORESPONSE_H_
#define _ISORESPONSE_H_

#include <relacs/repro.h>
#include <relacs/rangeloop.h>
#include <relacs/multiplot.h>
#include <relacs/ephystraces.h>
#include "auditorytraces.h"


/*!
\class IsoResponse 
\brief Measures f-I-curves for a superposition of two sine waves.
\author Alexander Wolf 
\version 1.1 (Jan 10, 2008)
-# removed stop() function
\version 1.0 (Oct 1, 2005)

IsoResponse extrapolates Iso-Response-Sets (IRSs) for two sine
amplitudes A1 and A2, i.e.  for several combinations of A1 and A2
f-I-curves are recorded and the intensities determined at which the
firing rate of the cell reaches predefined values.  The stimulus
looks like this: s(t)=A1*sin(2pi*f1*t)+A2*sin(2pi*f2*t).  

The frequencies can either be set manually (use_best_freq=0), or
set at a fixed distance (dist_best_freq) from either side the best
frequency (use_best_freq=1), or set at a fixed distance from each
(dist_freq) other so that the pure tone stimuli have the same
sensitivity according to the tuning curve (use_best_freq=2). !The
last option is to be prefered; however, it does not work in all
cases!

The stimulus length is defined by 'duration', the time in between
stimuli by 'pause'. For each stimuli presentation we record the
onset response in the time intervall 'onwidth', and the steady
state response after time 'sswidth' has passed, except when
'short_stim' is true - then only the onset is recorded.

The intensity of the signal ranges from 'intmin' to 'intmax'. To
find the relevant range in which the f-I-curve rises, the interval
is tested with a resolution of 'estres' and each data point is
measure 'estrepeats' times. When the relevant range is determined,
the resolution is decreased to 'intres' and each point is repeated
'repeats' times.  The order in which the intensities is measured is
determined with the switch 'inshuffle' (see RangeLoop). 

'setFRbounds' determines the limits of the firing rate in between
which the f-I-curve is to be measured. If 'switch_high'
('switch_low') is true, the upper (lower) bound is set to
'fix_high' ('fix_low'). Otherwise, the bounds are set to the
experimentally determined maximum firing rate fmax minus 'per_high'
('per_low') percent. 'setIBounds' determines then the intensity
range by finding the intensities at which the upper and lower bound
for the firing rate are reached.

The fraction of the amplitudes ('AmplFraction') determines the
shape of the signal. In order to scan the amplitude space
equidistantly, we use the tangent of 'AmplFraction' and a scaling
factor 'Scaling'. 'Scaling' is estimated from the f-I-curves of the
pure tones ('findScaling') to equalize the difference in
sensitivity of the two stimulus frequencies. The relative
amplitudes of the mixed stimuli are determined by 'AmplRatio'.

The IRSs are determined in 'plotIsoSets' for 'isonumber' firing
rates 'FIso'. The extrapolation is only linear using two points.
It is therefore strongly recommended to analyze the data offline.

The following options are supported (brief description of each
option with default values and data type): 
- \c use_best_freq=0: Select method of determining stimulus frequencies (\c integer)
- \c dist_best_freq=1kHz: Distance of f1 and f2  from best frequency (if use_best_freq=1)  (\c number)
- \c dist_freq=2kHz: Distance between f1 and f2 (if use_best_freq=2)  (\c number)
- \c f1= 4kHz: First stimulus frequeny (\c number)
- \c f2=10kHz: Second stimulus frequeny (\c number)
- \c intmin= 30dB: Minimum stimulus intensity (\c number) 
- \c intmax=100dB: Maximum stimulus intensity (\c number) 
- \c intres=1dB: Final intensity resolution (\c number)
- \c estres=5dB: Intensity resolution for estimation of f-I-curve (\c number)
- \c intshuffle: Order in which intensities are measured (\c RangeLoop)
- \c short_stim=true: Measure onset-response only (\c boolean)
- \c duration=0.2ms: Stimulus duration (\c number)
- \c pause=0.3ms: %Pause length in between stimuli (\c number)
- \c onwidth=0.01ms: Time from start of stimulus to estimate onset response (\c number)
- \c sswidth=0.10ms: Time after which steady state response determined (\c number)
- \c repeats=12: Number of repetitions of each stimulus (\c integer)
- \c estrepeats=3: Number of repetitions for estimation of f-I-curve (\c integer)
- \c intup=5dB: Increases determined intensity where max firing rate is reached (\c number)
- \c switch_high=false: Higher bound firing rate has fixed value (\c boolean) 
- \c per_high=5%: Decreases max firing rate by ... percent (\c number) 
- \c fix_high=500Hz: Fixed max firing rate (\c number)
- \c switch_low=false: Lower bound firing rate has fixed value (\c boolean) 
- \c per_low=50%: Decreases max firing rate by ... percent (\c number) 
- \c fix_low=150Hz: Fixed lower firing rate (\c number)
- \c minfr = 30Hz: Skip all points with firing rates below minfr (\c number)
- \c isonumber=5: Number of IRSs evaluated (for graph only) (\c integer)
- \c isopoints=9: Number of f-I-curves measured (resolution in amplitude space) (\c  integer)

Keyboard shortcuts:
- \c Space: Pressing space twice aborts IsoResponse
after finishing up measuring the curent f-I curve.

Output files:
- \c isoresponsespikes.dat: The spike trains...

- \c isoresponserates.dat: Continuous recording of measured
f-I-curves. Before each f-I-curve the header data is recorded. The
'run' number marks the experiment. In the table you find the
intensity, the true intensity, first and second stimulus amplitude,
onset firing rate and standard deviation, the steady state firing
rate and standard deviation, the mean firing rate and standard
deviation, and the number of repetitions:

I(dB SPL) I_t(dB SPL) A1(mPa) A2(mPa) f_on(Hz) sd(Hz) f_s(Hz) sd(Hz) f_m(Hz) sd(Hz) trials

*/


class IsoResponse : public RePro, public EPhysTraces, public AuditoryTraces
{
  Q_OBJECT

public:

    /*! Constructor. */
  IsoResponse( void );
    /*! Destructor. */
  ~IsoResponse( void );

    /*! Read options, create stimulus and start output of stimuli. */
  virtual int main( void );

  void saveSpikes( void );
  void saveRates( void );

    /*! Plot data. */
  void plot( void );
    /*! Analyze data. */
  void analyze( const OutData &signal );


protected:

  void setFRBounds( double max ); //defines range of firing rate from where the IRS come
  double Flow, Fup;
  vector< double > FIso; //contains firing rates of IRS

  void setIBounds( void ); //defines range of Intensity from where the IRS come

  void createSignal( OutData &signal, double a1, double a2 );
  double IntCorrection;

  int findIsoFreq( void );

  void findScaling( void );
  double Scaling;

  void plotIsoSets( void );

  double pi;
  double A0; //air-pressure for 0dB

  /**************************************************************
  control parameters*********************************************
  **************************************************************/
  int UseBestFrequency;
  double BestFreq;
  double DistBestFrequency;
  double DistFrequencies;
  double Frequency1;
  double Frequency2;
  double MinIntensity;
  double MaxIntensity;
  double FinalResolution;
  double EstimResolution;
  RangeLoop::Sequence IntShuffle;

  bool ShortStim;
  double   Duration;
  double   Pause;
  double   OnWidth; //
  double   SSWidth; //steady state
  bool FITest;

  int Repeats;
  int EstRepeats;
  int Side;

  double IntensityUp; //increases intensity of max firing rate by a fixed value
  bool Switch_high;
  double   FRhigh_per;
  double   FRhigh_fix;
  bool Switch_low;
  double   FRlow_per;
  double   FRlow_fix;
  double   MinFR;

  int IrsNumber; //number of iso-response-sets displayed and evaluated
  int IrsPoints; //number of points on each irs
  


  int State;
  MultiPlot P;                  // Plots!
  RangeLoop IntensityRange;    
  double Intensity;
  RangeLoop AmplitudeRelation;  // fraction of sine amplitudes
  double AmplFraction;
  OutData Signal;                // Stimulus!
  double AmplRatio;

  Options Header; 
  Options Settings;

  int MaxRatePos;
  double MaxRate;


  struct FIData {              //stores data for one Intensity
    FIData( void ) : OnRate( -1.0 ), SSRate( -1.0 ) {};
    double Intensity;
    double TrueIntensity;
    double OnRate;
    double CurOnRate;
    double OnRateSD;
    double SSRate;
    double CurSSRate;
    double SSRateSD;
    double MeanRate;
    double CurMeanRate;
    double MeanRateSD;
    double A1;
    double A2;
    EventList Spikes;
  };
  vector< FIData > Results;    //stores data for one f-I-curve

  struct IsoData {             //stores data for one f-I-curve
    double IntCor;             //intensity transform between maximum and rms
    double Alpha;
    MapD IsoSets;
    MapD OnIsoSets;
    vector< FIData > FinalResults;
  };
  vector< IsoData > IsoResults; //stores data for several f-I-curves
  
};

#endif 
