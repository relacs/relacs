/*
  efish/ficurve.h
  Measures f-I curves of electrosensory neurons.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_EFISH_FICURVE_H_
#define _RELACS_EFISH_FICURVE_H_ 1

#include <vector>
#include <relacs/sampledata.h>
#include <relacs/options.h>
#include <relacs/tablekey.h>
#include <relacs/multiplot.h>
#include <relacs/rangeloop.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
#include <relacs/efield/traces.h>
#include <relacs/efield/eodtools.h>
using namespace relacs;

namespace efish {


/*!
\class FICurve
\brief [RePro] Measures f-I curves of electrosensory neurons.
\author Jan Benda
\version 1.6 (Nov 19, 2010)

\par Options
- \b duration (\c number, \e ms): Duration of stimulus.
- \b preduration (\c number, \e ms): Duration of preadapting stimulus.
- \b pause (\c number, \e ms): %Pause between successive stimuli.
- \b delay (\c number, \e ms): Part of pause before stimulus.
- \b maxintfac (\c number): Maximum intensity factor. 
- \b minintfac (\c number): Minimum intensity factor. 
- \b nints (\c integer): Number of stimulus intensities. 
- \b maxpreintfac (\c number): Maximum intensity of preadapting stimulus. 
- \b minpreintfac (\c number): Minimum intensity of preadapting stimulus. 
- \b npreints (\c integer): Number of preadapting intensities. 
- \b repeats (\c integer): Number of repetitions.
- \b blockrepeats (\c integer): Number of repetitions of an intensity sequence.
- \b singlerepeats (\c integer): Number of immediate repetitions of each intensity.
- \b medres (\c integer): Medium resolution.
- \b nskip (\c integer): NSkip.
- \b minrateslope (\c number, \e Hz/mV/cm): Minimum slope of f-I curve. 

\par Files
- \b fispikes#.dat : the spikes of trace #.
- \b firate.dat : the firing rate.
- \b ficurve#.dat : Summary of f-I curve data of trace #.

\par Plots
- \b f-I curve : baseline activity (blue), prestimulus rate (orange),
     onset rate (green), steady state (red)
- \b firing rate : spikes (red), firing frequency (yellow), 
     stimuli onsets (white)

\par Requirements
- Transdermal EOD recording (\c EODTrace2) and events (\c EODEvents2).
- One or more spike events (\c SpikeEvents[*]).
*/


class FICurve
  : public RePro,
    public ephys::Traces,
    public efield::Traces,
    public efield::EODTools
{
  Q_OBJECT

public:

  FICurve( void );
  virtual int main( void );
  virtual void sessionStarted( void );

  void stop( void );
  void saveRate( int trace );
  void saveSpikes( int trace );
  void saveFICurves( int trace );
  void saveData( void );

  void selectSlopes( void );
  void selectRange( void );

  void plot( void );
  void analyzeSpikes( const EventData &se, int k,
		      double truepreintensity, double trueintensity );
  void analyze( void );


private:

  // parameter:
  double Duration;
  double PreDuration;
  double Pause;
  double Delay;
  double RateDeltaT;
  double SSTime;
  double OnsetTime;
  double MinRate;
  int IntIncrement;
  int RangeIntIncrement;
  double MinRateFrac;
  double MinRateSlope;

  // variables:
  RangeLoop IntensityRange;
  double Intensity;
  double Contrast;
  RangeLoop PreIntensityRange;
  double PreIntensity;
  double PreContrast;
  double FishRate;
  double FishAmplitude;
  string EOD2Unit;
  int Index;

  struct ResponseData
  {
    ResponseData( void ) : Trial( 0 ), Intensity( 0.0 ), PreIntensity( 0.0 ),
      RestRate( 0.0 ), PreRate( 0.0 ), OnsetRate( 0.0 ), SSRate( 0.0 )
      { Rate.clear(), Spikes.clear(); };
    ~ResponseData( void ) { Rate.clear(); Spikes.clear(); };
    EventList Spikes;
    SampleDataD Rate;
    int Trial;
    double Intensity;
    double PreIntensity;
    double RestRate;
    double PreRate;
    double OnsetRate;
    double SSRate;
  };
  vector < vector < ResponseData > > Response[MaxSpikeTraces]; 
  double MaxRate[MaxSpikeTraces];
    
  Options Header;
 
  MultiPlot P;

};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_FICURVE_H_ */
