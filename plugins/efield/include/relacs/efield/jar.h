/*
  efield/jar.h
  Measure the fishes JAR and chirp characteristics at different delta f's and beat contrasts.

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

#ifndef _RELACS_EFIELD_JAR_H_
#define _RELACS_EFIELD_JAR_H_ 1

#include <relacs/multiplot.h>
#include <relacs/rangeloop.h>
#include <relacs/repro.h>
#include <relacs/efield/traces.h>
#include <relacs/efield/eodtools.h>
using namespace relacs;

namespace efield {


/*!
\class JAR
\brief [RePro] Measure the fishes JAR and chirp characteristics at different delta f's and beat contrasts.
\author Jan Benda
\version 2.0 (Mar 09, 2010)
\bug EOD waveform stimulus broken (samplerate!).


\par Options
- \b duration (\c number, \e ms): Duration of stimulus.
- \b pause (\c number, \e ms): %Pause between successive stimuli.
- \b deltafstep (\c number, \e Hz): Increment of delta f.
- \b deltafmax (\c number, \e Hz): Maximum delta f.
- \b deltafmin (\c number, \e Hz): Minimum delta f.
- \b contraststep (\c number, \e %): Increment of stimulus contrast.
- \b contrastmax (\c number, \e %): Maximum stimulus contrast.
- \b repeats (\c integer): Number of repetitions of the whole stimulus set.
- \b jaraverage (\c number, \e ms): Time for measuring EOD rate.
- \b chirpaverage (\c number, \e ms): Time for measuring EOD rate and amplitude right before a chirp.
- \b sinewave (\c boolean): Use sinewave stimulus instead of the fishes EOD waveform.

\par Files
- \b jar.dat : summary for each jamming stimulus.
- \b jarmeans.dat : summary averaged over stimuli with same delta f and contrast.
- \b jareod.dat : the fishes EOD (actually the stimulus!)
- \b jareodtraces.dat : frequency, amplitude, and phase of the transdermal EOD during each stimulus.
- \b jarchirps.dat : data about each chirp.
- \b jarchirptraces.dat : frequency, amplitude, phase, beat, and beatphase for each chirp.
- \b jarchirpeod.dat : EOD for each chirp.

\par Plots
- \b EOD \b frequency (upper plot): time course of the fishes EOD frequency (green).
  %Chirps are marked by yellow dots.
- \b JAR (lower left plot): Frequency change (yellow, last stimulus red) versus stimulus delta f.
- \b %Chirps (lower middle plot): Number of chirps (yellow) and chirp phase shift (times ten, blue) versus stimulus delta f.
- \b %Chirps (lower right plot): EOD frequency (yellow) and chirp phase shift (time 40, red)
   for each chirp evoked by the last stimulus.

\par Requirements
- EOD recording (\c EODTrace1) and events (\c EODEvents1).
- Transdermal EOD recording (\c EODTrace2) and events (\c EODEvents2).
- Recording of the stimulus events (\c SignalEvents1).
*/


class JAR : public RePro, public Traces, public EODTools
{
  Q_OBJECT

public:

  JAR( void );
  virtual int main( void );
  virtual void sessionStarted( void );
  virtual void sessionStopped( bool saved );

    /*! Save results data. */
  void save( void );
    /*! Save JAR for each stimulus. */
  void saveJAR( const Options &header );
    /*! Save mean JAR for each delta f. */
  void saveMeanJAR( const Options &header );
    /*! Save EOD stimulus. */
  void saveEOD( const Options &header );
    /*! Save trace data. */
  void saveTrace( void );
    /*! Save EOD frequency and amplitude. */
  void saveEODFreq( const Options &header );
    /*! Save chirp parameter. */
  void saveChirps( const Options &header );
    /*! Save chirp frequency, amplitude and phase traces. */
  void saveChirpTraces( const Options &header );
    /*! Save the EOD around each chirp. */
  void saveChirpEOD( const Options &header );
  void reset( bool saved );

    /*! Plot data. */
  void plot( void );
    /*! Analyze data. */
  void analyze( void );


private:

  // plot:  
  MultiPlot P;

  // parameter:
  int ReadCycles;
  double Duration;
  double Pause;
  double DeltaFStep;
  double DeltaFMin;
  double DeltaFMax;
  bool UseContrast;
  double ContrastStep;
  double ContrastMax;
  double AmplStep;
  double AmplMax;
  int Repeats;
  double After;
  double JARAverageTime;
  double ChirpAverageTime;
  double EODSaveTime;
  bool SineWave;

  // variables:
  double FishRate;
  double FishAmplitude1;
  double FishAmplitude2;
  double IntensityGain;
  double StimulusRate;
  double FirstRate;
  double LastRate;
  double Contrast;
  int ContrastCount;
  vector<double> Contrasts;
  double Intensity;
  RangeLoop DeltaFRange;
  double DeltaF;
  double TrueDeltaF;
  double TrueContrast;
  int Count;
  int FileIndex;

  struct ResponseData
  {
    ResponseData( int i, double f, double l, double j, double c ); 
    void addChirp( double size, double width, double ampl, double phase );
    int Index;
    double FirstRate;
    double LastRate;
    double Jar;
    double Contrast;
    int NChirps;
    double Size;
    double SizeSq;
    double Width;
    double WidthSq;
    double Amplitude;
    double AmplitudeSq;
    double Phase;
    double PhaseSq;
  };
  vector < vector < vector < ResponseData > > > Response;

  struct MeanResponseData
  {
    MeanResponseData( void );
    void addJAR( double contrast, double f, double l, double jar, int chirps );
    void addChirp( double size, double width, double ampl, double phase );
    int NJar;
    double Contrast;
    double ContrastSq;
    double First;
    double FirstSq;
    double Last;
    double LastSq;
    double Jar;
    double JarSq;
    double Chirps;
    double ChirpsSq;
    int NChirps;
    double Size;
    double SizeSq;
    double Width;
    double WidthSq;
    double Amplitude;
    double AmplitudeSq;
    double Phase;
    double PhaseSq;
  };
  vector < vector < MeanResponseData > > MeanResponse;

  string GlobalEODUnit;
  string LocalEODUnit;
  MapD EODFrequency;
  MapD EODAmplitude;
  MapD EODTransAmpl;
  MapD EODBeatPhase;
  MapD EODPhases;
  int PlotLabelIndex1;
  int PlotLabelIndex2;
  EventData JARChirpEvents;

  struct ChirpData 
  {
    ChirpData( double t, double s, double w, double er, double ea, double a,
	       double p, double bp, double bl, double bf, double bb, double ba );
    double Time;
    double Size;
    double Width;
    double EODRate;
    double EODAmpl;
    double Amplitude;
    double Phase;
    double BeatPhase;
    double BeatLoc;
    double Deltaf;
    double BeatBefore;
    double BeatAfter;
  };
  vector< ChirpData > Chirps;

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_JAR_H_ */
