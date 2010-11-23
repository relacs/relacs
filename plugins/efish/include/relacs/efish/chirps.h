/*
  efish/chirps.h
  Measures responses to chirps.

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

#ifndef _RELACS_EFISH_CHIRPS_H_
#define _RELACS_EFISH_CHIRPS_H_ 1

#include <vector>
#include <relacs/array.h>
#include <relacs/map.h>
#include <relacs/sampledata.h>
#include <relacs/options.h>
#include <relacs/tablekey.h>
#include <relacs/multiplot.h>
#include <relacs/ephys/traces.h>
#include <relacs/efield/traces.h>
#include <relacs/efield/eodtools.h>
#include <relacs/repro.h>
using namespace relacs;

namespace efish {


/*!
\class Chirps
\brief [RePro] Measures responses to chirps.
\author Jan Benda
\version 1.8 (Apr 23, 2010)
\todo Make independent of beat detector

\par Options
- \b nchirps (\c integer): Number of chirps per stimulus.
- \b minspace (\c number, \e ms): Minimum time between chirps (AM only).
- \b firstspace (\c number, \e ms): Time preceeding first chirp (AM only).
- \b pause (\c number, \e ms): %Pause between successive stimuli.
- \b deltaf (\c number, \e Hz): Beat frequency.
- \b contrast (\c number, \e %): Contrast (AM amplitude / EOD amplitude)
- \b chirpsize (\c number, \e Hz): Size of the chirps.
- \b chirpwidth (\c number, \e ms): Width of the chirps.
- \b chirpampl (\c number, \e %): Reduction of EOD amplitude during a chirp.
- \b repeats (\c integer): Number of stimulus repetitions (0: infinite).
- \b beatpos (\c integer): Number of beat positions used for analysis.
- \b ratedt (\c number, \e ms): Resolution of firing rate.
- \b am (\c boolean): AM stimulus or direct stimulus.
- \b sinewave (\c boolean): If direct stimulus: use sine wave or the fishes EOD.
- \b playback (\c boolean): Record transdermal amplitude evoked by direct stimulus
and replay it as an AM stimulus.

\par Files
- \b chirps.dat : General information for each chirp in each stimulus.
- \b chirptraces.dat : Frequency and amplitude of the transdermal EOD for each chirp.
- \b chirpspikes#.dat : The spikes elicited by each chirp of trace #.
- \b chirpallspikes#.dat : The spikes elicited by each stimulus of trace #.
- \b chirpnerveampl.dat : The nerve potential elicited by each chirp.
- \b chirpallnerveampl.dat : The nerve potential elicited by each stimulus.
- \b chirpeodampl.dat : Amplitude of the transdermal EOD for each stimulus.
- \b chirprate.dat : The firing rate for each chirp position.

\par Plots
- \b Rate: Spikes (direct stimulus: red, AM: magenta) and 
  firing rate (direct stimulus: yellow, AM: orange) for each chirp position.
- \b Beat: Amplitude of transdermal EOD for each chirp position (darkgreen). Last stimulus green.

\par Requirements
- Optional: EOD events (\c EODEvents1). Recommended if direct stimulation is used.
- Transdermal EOD recording (\c EODTrace2) and events (\c EODEvents2).
- Recording of the stimulus events (\c SignalEvents1) if direct stimuli are used.
- One or more spike events (\c SpikeEvents[*]) or nerve recordings (\c NerveTrace1).
*/


class Chirps
  : public RePro,
    public ephys::Traces,
    public efield::Traces,
    public efield::EODTools
{
  Q_OBJECT

public:

  Chirps( void );
  virtual int main( void );
  virtual void sessionStarted( void );
  virtual void sessionStopped( bool saved );

  void stop( void );
  void saveChirps( void );
  void saveChirpTraces( void );
  void saveChirpSpikes( int trace );
  void saveChirpNerve( void );
  void saveAmplitude( void );
  void saveSpikes( int trace );
  void saveChirpRate( int trace );
  void saveNerve( void );
  void save( void );

  void plot( void );
  void analyze( void );


private:

  void initMultiPlot( double maxampl );
  void createEOD( OutData &signal );
  void createPlayback( OutData &signal );
  int createAM( OutData &signal );

  // parameter:
  int ReadCycles;
  int NChirps;
  double MinSpace;
  double FirstSpace;
  double Pause;
  double ChirpSize;
  double ChirpWidth;
  double ChirpDip;
  int BeatPos;
  double BeatStart;
  double Sigma;
  double DeltaF;
  double Contrast;
  int Repeats;
  double SaveWindow;
  bool AM;
  bool SineWave;
  bool Playback;

  // variables:
  int Mode;
  double TrueDeltaF;
  double TrueContrast;
  double Duration;
  double StimulusRate;
  double IntensityGain;
  double FishRate;
  double FishAmplitude;
  double ChirpPhase;
  ArrayD ChirpTimes;
  ArrayD BeatPhases;
  double Intensity;
  int Count;
  int StimulusIndex;
  bool OutWarning;
  string EOD2Unit;

  struct ChirpData
  {
    ChirpData( int i, int m, int tr, double t, 
	       double s, double w, double a, double p,
	       double er,
	       double bf, double bph, double bl, int bbin,
	       double bb, double ba, 
	       double bp, double bt ) 
      : Index( i ), Mode( m ), Trace( tr ), Time( t ), Size( s ), Width( w ),
	 Amplitude( a ), Phase( p ), EODRate( er ),
	 BeatFreq( bf ), BeatPhase( bph ), BeatLoc( bl ), BeatBin( bbin ), 
	 BeatBefore( bb ), BeatAfter( ba ), BeatPeak( bp ), BeatTrough( bt ),
	 EODTime(), EODFreq(), EODAmpl(), Spikes(),
	NerveAmplP(), NerveAmplT(), NerveAmplM() {};
    int Index;
    int Mode;
    int Trace;
    double Time;
    double Size;
    double Width;
    double Amplitude;
    double Phase;
    double EODRate;
    double BeatFreq;
    double BeatPhase;
    double BeatLoc;
    int BeatBin;
    double BeatBefore;
    double BeatAfter;
    double BeatPeak;
    double BeatTrough;
    ArrayD EODTime;
    ArrayD EODFreq;
    ArrayD EODAmpl;
    EventData Spikes[MaxSpikeTraces];
    MapD NerveAmplP;
    MapD NerveAmplT;
    MapD NerveAmplM;
  };
  vector < ChirpData > Response;
  long FirstResponse;

  struct RateData
  {
    RateData( void ) : Trials( 0 ), Rate( 0 ) {};
    RateData( double width, double dt ) : Trials( 0 ), Rate( -width, width, dt ) {};
    int Trials;
    SampleDataD Rate;
  };

  EventData Spikes[MaxSpikeTraces];
  vector < vector < RateData > > SpikeRate[MaxSpikeTraces];
  double MaxRate[MaxSpikeTraces];

  vector< vector < SampleDataD > > NerveMeanAmplP;
  vector< vector < SampleDataD > > NerveMeanAmplT;
  vector< vector < SampleDataD > > NerveMeanAmplM;
  MapD NerveAmplP;
  MapD NerveAmplT;
  MapD NerveAmplM;
  AcceptEOD<InData::const_iterator,InDataTimeIterator> NerveAcceptEOD;

  MapD EODAmplitude;

  Options Header;
  TableKey ChirpKey;
  TableKey ChirpTraceKey;
  TableKey SpikesKey;
  TableKey NerveKey;
  TableKey AmplKey;

  MultiPlot P;
  int Rows;
  int Cols;

};


}; /* namespace efish */

#endif /* ! _RELACS_EFISH_CHIRPS_H_ */
