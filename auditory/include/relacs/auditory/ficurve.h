/*
  ficurve.h
  Optimized measuring of f-I curves.

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

#ifndef _RELACS_AUDITORY_FICURVE_H_
#define _RELACS_AUDITORY_FICURVE_H_ 1

#include <relacs/sampledata.h>
#include <relacs/rangeloop.h>
#include <relacs/multiplot.h>
#include <relacs/common/ephystraces.h>
#include <relacs/auditory/auditorytraces.h>
#include <relacs/repro.h>


/*!
\class FICurve
\brief Optimized measuring of f-I curves.
\author Jan Benda
\version 1.3 (Jan 10, 2008)
-# removed stop() function
\version 1.2 (Jan 10, 2006)
\bug Need to sleep 10ms longer to prevent busy error!

Features:
- Shorter pauses when there is no response.

\par Options
\arg \b duration (\c number, \e ms): Duration of calibration stimulus.
\arg \b repeats (\c integer): Maximum repetitions of the calibration protocol.

\par Files
\arg \b calibrate.dat : the calibration data (measured versus requested stimulus intensity).

\par Plots
The plot shows the measured versus the requested stimulus intensity (red circles).
The yellow line is a fit of a straight line to the data.
This line should for a successful calibration coincide with the blue 1:1 line.

\par Requirements
\arg Transdermal EOD recording (\c EODTrace2) and events (\c EODEvents2).
*/


class FICurve : public RePro, public EPhysTraces, public AuditoryTraces
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

#endif /* ! _RELACS_AUDITORY_FICURVE_H_ */
