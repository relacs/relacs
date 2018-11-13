/*
  efish/chirps.cc
  Measures responses to chirps.

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

#include <cmath>
#include <fstream>
#include <iomanip>
#include <relacs/str.h>
#include <relacs/datafile.h>
#include <relacs/detector.h>
#include <relacs/kernel.h>
#include <relacs/efish/chirps.h>
using namespace relacs;

namespace efish {


Chirps::Chirps( void )
  : RePro( "Chirps", "efish", "Jan Benda", "2.0", "Nov 11, 2014" )
{
  // parameter:
  ReadCycles = 100;
  NChirps = 10;
  FirstSpace = 0.2;
  MinSpace = 0.2;
  Pause = 1.0;
  ChirpSize = 100.0;  // Hertz
  ChirpWidth = 0.014;  // seconds
  ChirpKurtosis = 1.0;
  ChirpDip = 0.02;
  DeltaF = 10.0;
  Contrast = 0.2;
  Repeats = 12;
  BeatPos = 10;
  BeatStart = 0.25;
  Sigma = 0.003;
  SaveWindow = 0.4;
  AM = false;
  SineWave = true;
  Playback = false;

  // add some parameter as options:
  newSection( "Chirp parameter" );
  addInteger( "nchirps", "Number of chirps", NChirps, 1, 10000, 2 );
  addInteger( "beatpos", "Number of beat positions", BeatPos, 1, 100, 1 );
  addNumber( "beatstart", "Beat position of first chirp", BeatStart, 0.0, 1.0, 0.01, "1" );
  addNumber( "minspace", "Minimum time between chirps", MinSpace, 0.01, 1000.0, 0.05, "sec", "ms" );
  addNumber( "firstspace", "Time preceeding first chirp", FirstSpace, 0.01, 1000.0, 0.05, "sec", "ms" );
  addNumber( "chirpsize", "Size of chirp", ChirpSize, 0.0, 1000.0, 10.0, "Hz" );
  addNumber( "chirpwidth", "Width of chirp", ChirpWidth, 0.002, 1.0, 0.001, "sec", "ms" );
  addNumber( "chirpampl", "Amplitude of chirp", ChirpDip, 0.0, 1.0, 0.01, "1", "%", "%.0f" );
  addSelection( "chirpsel", "Chirp waveform", "generated|from file" );
  addNumber( "chirpkurtosis", "Kurtosis of Gaussian chirp", ChirpKurtosis, 0.01, 100.0, 0.01, "", "" ).setActivation( "chirpsel", "generated" );
  addText( "file", "Chirp-waveform file", "" ).setStyle( OptWidget::BrowseExisting ).setActivation( "chirpsel", "from file" );
  newSection( "Beat parameter" );
  addNumber( "deltaf", "Delta f", DeltaF, -10000.0, 10000.0, 5, "Hz" );
  addNumber( "contrast", "Contrast", Contrast, 0.0, 1.0, 0.01, "", "%" );
  addBoolean( "am", "Amplitude modulation", AM );
  addBoolean( "sinewave", "Use sine wave", SineWave );
  addBoolean( "playback", "Playback AM", Playback );
  addNumber( "pause", "Pause between signals", Pause, 0.01, 1000.0, 0.05, "sec", "ms" );
  addInteger( "repeats", "Repeats", Repeats, 0, 100000, 2 ).setStyle( OptWidget::SpecialInfinite );
  newSection( "Analysis" );
  addNumber( "sigma", "Standard deviation of rate smoothing kernel", Sigma, 0.0, 1.0, 0.0001, "seconds", "ms" );
  addBoolean( "adjust", "Adjust input gain?", true );

  // variables:
  OutWarning = true;
  ChirpTimes.clear();
  BeatPhases.clear();
  FishAmplitude = 0.0;
  Intensity = 0.0;
  StimulusIndex = 0;
  Count = 0; 
  Response.clear();
  for ( int k=0; k<MaxTraces; k++ ) {
    Spikes[k].clear();
    SpikeRate[k].clear();
  }
  NerveMeanAmplP.clear();
  NerveMeanAmplT.clear();
  NerveMeanAmplM.clear();
  NerveMeanAmplS.clear();
  NerveAmplP.clear();
  NerveAmplT.clear();
  NerveAmplM.clear();
  NerveAmplS.clear();
  EODAmplitude.clear();

  // header for files:
  Header.addInteger( "index" );
  Header.addInteger( "trace" );
  Header.addNumber( "chirpphase", "1", "%.3f" );
  Header.addNumber( "duration", "sec", "%.3f" );
  Header.addText( "waveform" );
  Header.addText( "stimulus type" );
  Header.addNumber( "true deltaf", "Hz", "%.1f" );
  Header.addNumber( "true contrast", "%", "%.1f" );
  Header.addNumber( "EOD rate", "Hz", "%.1f" );
  Header.addNumber( "trans. amplitude", "", "%.2f" );
  Header.addNumber( "upper trans. amplitude", "", "%.2f" );
  Header.addNumber( "lower trans. amplitude", "", "%.2f" );
  Header.addText( "repro time" );
  Header.addText( "session time" );
  Header.newSection( "Settings" );

  // tablekeys:
  ChirpKey.newSection( "chirp" );
  ChirpKey.addNumber( "inx", "1", "%3.0f" );
  ChirpKey.addNumber( "time", "sec", "%9.5f" );
  ChirpKey.addNumber( "src", "-", "%3.0f" );
  ChirpKey.addNumber( "size", "Hz", "%5.1f" );
  ChirpKey.addNumber( "width", "ms", "%5.1f" );
  ChirpKey.addNumber( "kurtosis", "1", "%5.2f" );
  ChirpKey.addNumber( "ampl", "%", "%5.1f" );
  ChirpKey.addNumber( "phase", "1", "%5.3f" );
  ChirpKey.newSection( "eod" );
  ChirpKey.addNumber( "rate", "Hz", "%5.1f" );
  ChirpKey.newSection( "beat" );
  ChirpKey.addNumber( "freq", "Hz", "%5.1f" );
  ChirpKey.addNumber( "phase", "1", "%5.3f" );
  ChirpKey.addNumber( "loc", "1", "%5.3f" );
  ChirpKey.addNumber( "bin", "-", "%3.0f" );
  ChirpKey.addNumber( "ampl-", "", "%5.3f" );
  ChirpKey.addNumber( "ampl+", "", "%5.3f" );
  ChirpKey.addNumber( "jump", "", "%5.3f" );
  ChirpKey.addNumber( "max", "", "%5.3f" );
  ChirpKey.addNumber( "min", "", "%5.3f" );
  ChirpKey.addNumber( "diff", "", "%5.3f" );
  ChirpKey.addNumber( "contr", "%", "%5.1f" );

  ChirpTraceKey.addNumber( "time", "ms", "%7.2f" );
  ChirpTraceKey.addNumber( "freq", "Hz", "%5.1f" );
  ChirpTraceKey.addNumber( "ampl", "", "%5.3f" );

  SpikesKey.addNumber( "time", "ms", "%8.2f" );

  NerveKey.newSection( "peak" );
  NerveKey.addNumber( "time", "ms", "%7.2f" );
  NerveKey.addNumber( "ampl", "uV", "%6.1f" );
  NerveKey.newSection( "trough" );
  NerveKey.addNumber( "time", "ms", "%7.2f" );
  NerveKey.addNumber( "ampl", "uV", "%6.1f" );
  NerveKey.newSection( "average" );
  NerveKey.addNumber( "time", "ms", "%7.2f" );
  NerveKey.addNumber( "ampl", "uV", "%7.2f" );

  SmoothKey.newSection( "peak" );
  SmoothKey.addNumber( "time", "ms", "%7.2f" );
  SmoothKey.addNumber( "ampl", "uV", "%7.3f" );

  AmplKey.addNumber( "time", "ms", "%8.2f" );
  AmplKey.addNumber( "ampl", "", "%5.3f" );

  // plot:
  setWidget( &P );
}


void Chirps::createEOD( OutData &signal )
{
  double pointspercycle;

  // create base waveform:
  OutData waveform;
  waveform.setTrace( GlobalEField );
  if ( SineWave ) {
    // create sine wave:
    StimulusRate = FishRate + DeltaF;
    waveform.sineWave( ReadCycles / StimulusRate, -1.0, StimulusRate );
    pointspercycle = waveform.size() / ReadCycles;
    IntensityGain = 1.0;
  }
  else {
    // extract an EOD waveform:
    double t1 = events(LocalEODEvents[0]).back( ReadCycles );
    double t2 = events(LocalEODEvents[0]).back();
    trace(LocalEODTrace[0]).copy( t1, t2, waveform );
    double g = waveform.maximize();
    signal.setSampleRate( trace(LocalEODTrace[0]).sampleRate() * ( FishRate + DeltaF ) / FishRate );
    signal.setCarrierFreq( FishRate + DeltaF );
    StimulusRate = ReadCycles/signal.duration();
    pointspercycle = waveform.size() / ReadCycles;
    double maxamplitude = trace(LocalEODTrace[0]).maxValue() - trace(LocalEODTrace[0]).minValue();
    IntensityGain = maxamplitude / FishAmplitude / g;
  }

  // create chirp:
  double dt = waveform.sampleInterval();
  SampleDataF chirp( 0.0, 4.0*ChirpWidth, dt, 0.0 );
  chirp.clear();
  double sig = 0.5*ChirpWidth / ::pow( 2.0*log(10.0), 0.5/ChirpKurtosis );
  double T = waveform.length()/ReadCycles;
  double chirptime = 2.0*ChirpWidth;
  int j = 0;
  double p = 0.0;
  double dp = 0.0;
  if ( ChirpSel == 0 ) {
    // generate Gaussian chirp:
    for ( double t=0.0; t<4.0*ChirpWidth; ) {
      t = chirp.length();
      double x = t - chirptime;
      double g = exp( -0.5 * ::pow( (x/sig)*(x/sig), ChirpKurtosis ) );
      double f = ChirpSize*g + StimulusRate;
      double a = 1.0 - ChirpDip*g;
      p += f * dt;
      dp += (f-StimulusRate) * dt;
      for ( ; ; j++ ) {
	if ( dt*(j+1) > p*T )
	  break;
      }
      // interpolate:
      double m = (waveform[(j+1) % waveform.size()] - waveform[j % waveform.size()])/dt;
      double v = m*(p*T - j*dt) + waveform[j % waveform.size()];
      chirp.push( a*v );
    }
  }
  else {
    // chirp from file:
    for ( double t=0.0; t<4.0*ChirpWidth; ) {
      t = chirp.length();
      double x = t - chirptime;
      double f = ChirpSize*ChirpFreqs.interpolate( x ) + StimulusRate;
      double a = 1.0 - ChirpDip*ChirpAmpls.interpolate( x );
      p += f * dt;
      dp += (f-StimulusRate) * dt;
      for ( ; ; j++ ) {
	if ( dt*(j+1) > p*T )
	  break;
      }
      // interpolate:
      double m = (waveform[(j+1) % waveform.size()] - waveform[j % waveform.size()])/dt;
      double v = m*(p*T - j*dt) + waveform[j % waveform.size()];
      chirp.push( a*v );
    }
  }
  for ( int k = chirp.size()-1; k > 0; k-- ) {
    if ( chirp[k-1] < 0.0 && chirp[k] >= 0.0 ) {
      chirp.resize( k );
      break;
    }
    p -= StimulusRate * dt;
  }
  ChirpPhase = dp;

  // setup signal:
  int pointsperbeat = waveform.indices( 1.0 / fabs( DeltaF ) );
  signal = OutData( NChirps*(2*pointsperbeat + chirp.size()) + pointsperbeat,
		    waveform.sampleInterval() );
  signal.clear();
  signal.setCarrierFreq( StimulusRate );
  signal.setStartSource( 1 );
  signal.setTrace( GlobalEField );
  Str s = SineWave ? "sinewave+chirps_" : "EOD+chirps_";
  s += Str( StimulusIndex );
  signal.setIdent( s );
  s = "C=" + Str( 100.0 * Contrast, 0, 5, 'g' ) + "%";
  s += ", Df=" + Str( DeltaF, 0, 1, 'f' ) + "Hz";
  s += ", chirps=" + Str( NChirps );
  s += ", size=" + Str( ChirpSize, 0, 0, 'f' ) + "Hz";
  s += ", width=" + Str( 1000.0*ChirpWidth, 0, 0, 'f' ) + "ms";
  s += ", kurtosis=" + Str( ChirpKurtosis, 0, 2, 'f' );
  s += ", ampl=" + Str( 100.0*ChirpDip, 0, 0, 'f' ) + "%";
  s += ", phase: " + Str( ChirpPhase, 0, 2, 'f' );
  //  signal.setComment( s );

  // single cycle phase shift:
  double cyclephase = fabs( DeltaF ) * pointspercycle * signal.sampleInterval();

  // create signal:
  ChirpTimes.resize( NChirps );
  int ip;
  p = 0.0;
  // start with at least one beat cycle:
  for ( int i=0; ; ) {
    ip = i;
    signal.push( waveform[i++] );
    p += fabs( DeltaF ) * signal.sampleInterval();
    // only copy complete EOD cycles:
    for( ; j<signal.size(); i++, j++ ) {
      if ( i >= waveform.size() )
	i = 0;
      if ( waveform[i] >= 0.0 && waveform[ip] < 0.0 )
	break;
      signal.push( waveform[i] );
      p += fabs( DeltaF ) * signal.sampleInterval();
      ip = i;
    }
    if ( p > 1.0 )
      break;
  }
  int cyclesperbeat = (int)rint( signal.sampleRate() / fabs( DeltaF ) / pointspercycle );

  // insert chirps:
  for ( int k=0; k<NChirps; k++ ) {
    // go to the required beat position:
    for ( int i=0; ; ) {
      ip = i;
      signal.push( waveform[i++] );
      // only copy complete EOD cycles:
      for( ; ; i++, j++ ) {
	if ( i >= waveform.size() )
	  i = 0;
	if ( waveform[i] >= 0.0 && waveform[ip] < 0.0 )
	  break;
	signal.push( waveform[i] );
	ip = i;
      }
      double tpp = signal.interval( j );
      p = 1.0 - ( tpp - floor( tpp * FishRate ) / FishRate ) * FishRate;
      // beat phase at a possible chirp mod 1:
      double cp = p + fabs( DeltaF ) * chirptime;
      cp -=  floor( cp );
      // the beat phase mod 1 where we want the chirp to be:
      double beatphase = double(k) / double(BeatPos);
      beatphase -= floor( beatphase );
      if ( fabs( cp - beatphase ) < cyclephase )
	break;
    }
    // chirp:
    ChirpTimes[k] = signal.length() + chirptime;
    for ( int i=0; i<chirp.size(); i++ )
      signal.push( chirp[i] );

    // one beat cycle:
    int n = 0;
    for ( int i=0; n < cyclesperbeat; n++ ) {
      ip = i;
      signal.push( waveform[i++] );
      // only copy complete EOD cycles:
      for( ; ; i++, j++ ) {
	if ( i >= waveform.size() )
	  i = 0;
	if ( waveform[i] >= 0.0 && waveform[ip] < 0.0 )
	  break;
	signal.push( waveform[i] );
	ip = i;
      }
    }
  }
  signal.push( 0.0 );
  Duration = signal.duration();
}


void Chirps::createPlayback( OutData &signal )
{
  // setup signal:
  signal = OutData( Duration, signal.minSampleInterval() );
  signal.setCarrierFreq( StimulusRate + DeltaF );
  signal.setStartSource( 1 );
  signal.setTrace( GlobalAMEField );
  Str s = "am_";
  s += Str( StimulusIndex );
  signal.setIdent( s );
  s = "C=" + Str( 100.0 * Contrast, 0, 5, 'g' ) + "%";
  s += ", Df=" + Str( DeltaF, 0, 1, 'f' ) + "Hz";
  s += ", amplitude modulation";
  //  signal.setComment( s );

  double first = events(LocalEODEvents[0]).meanSize( signalTime() - 0.5, signalTime() );
  double max = EODAmplitude.max() - first;
  double min = EODAmplitude.min() - first;
  double norm = max > -min ? 1.0/max : -1.0/min; 
  int k = 1;
  for ( int j=0; j<signal.size(); j++ ) {
    double t = signal.interval( j );
    for ( ;
	  k<EODAmplitude.size() && EODAmplitude.x(k) < t;
	  k++ );
    double m = ( EODAmplitude.y(k) - EODAmplitude.y(k-1) ) / ( EODAmplitude.x(k) - EODAmplitude.x(k-1) );
    double y = m * ( t - EODAmplitude.x(k) ) + EODAmplitude.y(k);
    double s = ( y - first ) * norm;
    signal[j] = s;
  }
  signal.back() = 0.0;
  // XXX Scaling????
  Duration = signal.duration();
}


int Chirps::createAM( OutData &signal )
{
  if ( ! SineWave ) {
    signal.clear();
    warning( "Non-Sinewave as AM not supported!" );
    return 1;
  }
  signal.clear(); 
  signal.setTrace( GlobalAMEField );
  double sr = fabs( DeltaF ) + ChirpSize;
  signal.setSampleRate( floor( signal.maxSampleRate()/sr ) * sr );
  signal.setIntensity( 0.2 * FishAmplitude );
  // get the actual set sampling rate.
  // no signal is put out, because there isn't any.
  write( signal );

  // get size of signal:
  double period = 1.0 / fabs( DeltaF );
  int firstcycle = int( ceil( FirstSpace / period ) );
  int mincycle = int( ceil( MinSpace / period ) );
  if ( mincycle < 1 )
    mincycle = 1;
  double chirpslot = 4.0*ChirpWidth + period;
  double deltat = signal.sampleInterval();
  int pointsperbeat = int( rint( period / deltat ) );
  int pointsperchirp = int( rint( chirpslot / deltat ) );
  signal.reserve( firstcycle*pointsperbeat + NChirps*(mincycle*pointsperbeat+pointsperchirp) );
  double phasestep = 1.0/BeatPos;
  double sig = 0.5*ChirpWidth / ::pow( 2.0*log(10.0), 0.5/ChirpKurtosis );

  // first beat:
  double p = -0.25;   // current beat phase
  for ( int j=0; j<firstcycle; j++ ) {
    for ( int i=0; i<pointsperbeat; i++ ) {
      p += DeltaF*deltat;
      signal.push( cos( 6.28318 * p ) );
    }
  }

  // chirps and beats:
  ChirpTimes.resize( NChirps );
  BeatPhases.resize( NChirps );
  double chirpphase = BeatStart;
  for ( int n=0; n<NChirps; n++ ) {
    // chirp:
    double p0 = fabs( p + DeltaF*2.0*ChirpWidth );
    double cp = floor( p0 );
    cp += chirpphase;
    if ( cp < p0 )
      cp += 1.0;
    double t0 = ( cp - fabs( p ) )/fabs( DeltaF );
    ChirpTimes[n] = signal.length() + t0;
    BeatPhases[n] = chirpphase;   // XXX wrong for negative DeltaF!!
    double dp = 0.0;
    if ( ChirpSel == 0 ) {
      // generate Gaussian chirp:
      for ( int i=0; i<pointsperchirp; i++ ) {
	double tt = i*deltat - t0;
	double gg = exp( -0.5 * ::pow( (tt/sig)*(tt/sig), ChirpKurtosis ) );
	double cc = ChirpSize*gg;
	p += (DeltaF + cc)*deltat;
	dp += cc * deltat;
	signal.push( (1.0 - ChirpDip*gg) * cos( 6.28318 * p ) );
      }
    }
    else {
      // chirp from file:
      for ( int i=0; i<pointsperchirp; i++ ) {
	double tt = i*deltat - t0;
	double cc = ChirpSize*ChirpFreqs.interpolate( tt );
	p += (DeltaF + cc)*deltat;
	dp += cc * deltat;
	signal.push( (1.0 - ChirpDip*ChirpAmpls.interpolate( tt )) * cos( 6.28318 * p ) );
      }
    }
    ChirpPhase = dp;

    // beat:
    for ( int j=0; j<mincycle; j++ ) {
      for ( int i=0; i<pointsperbeat; i++ ) {
	p += DeltaF*deltat;
	signal.push( cos( 6.28318 * p ) );
      }
    }
    chirpphase += phasestep;
    if ( chirpphase > 1.0 )
      chirpphase -= 1.0;
  }
  signal.push( 0.0 );

  IntensityGain = 1.0;
  Duration = signal.duration();
  signal.setStartSource( 1 );
  signal.setTrace( GlobalAMEField );
  signal.setIdent( "chirpam" );
  Str s = "C=" + Str( 100.0 * Contrast, 0, 5, 'g' ) + "%";
  s += ", Df=" + Str( DeltaF, 0, 1, 'f' ) + "Hz";
  s += ", AM";
  //  signal.setComment( s );
  return 0;
}


  void Chirps::initMultiPlot( double ampl, double contrast )
{
  int nsub = 1 + SpikeTraces + NerveTraces;

  P.lock();
  P.resize( nsub*BeatPos );

  const double xlabelmarg = 3.0;
  const double xmarg = 0.3;
  const double ylabelmarg = 7.0;
  const double ymarg = 1.0;
  const double ratemax = 100.0;
  if ( contrast < 0.05 )
    contrast = 0.05;
  double minampl = (1.0-1.5*contrast)*ampl;
  if ( minampl < 0.0 )
    minampl = 0.0;
  double maxampl = (1.0+1.5*contrast)*ampl;
  if ( maxampl > 2.0*ampl )
    maxampl = 2.0*ampl;
  
  double xr = 100.0;
  if ( fabs( DeltaF ) > 50.0 )
    xr = 50.0;
  double xrange = xr;
  while ( xrange <= 2.0*ChirpWidth )
    xrange += xr;

  Rows = BeatPos < 4 ? 1 : 2;
  Cols = BeatPos / Rows;
  double dx = double( widget()->width() - P[0].fontPixel( ylabelmarg ) ) / double( widget()->width() ) / double(Cols);
  double xo = 1.0 - Cols * dx;
  double dy = double( widget()->height() - P[0].fontPixel( xlabelmarg ) ) / double( widget()->height() ) / Rows;
  double yo = 1.0 - Rows * dy;
  double dys = 1.0/nsub;

  int n = 0;
  for ( int row=Rows-1; row >= 0; row-- ) {
    for ( int col=0; col < Cols; col++ ) {
      for ( int sub=0; sub < nsub; sub++ ) {
	P[n].clear();
	P[n].setLMarg( 0.0 );
	P[n].setRMarg( ymarg );
	P[n].setBMarg( xmarg );
	P[n].setTMarg( xmarg );
	P[n].setXRange( -xrange, xrange );
	P[n].setXLabel( "" );
	P[n].noXTics();
	P[n].setYLabel( "" );
	P[n].noYTics();
	if ( row == 0 && col == 0 && sub == 0 ) {
	  P[n].setOrigin( 0.0, 0.0 );
	  P[n].setSize( xo+dx, yo+dys*dy );
	  P[n].setLMarg( ylabelmarg );
	  P[n].setBMarg( xlabelmarg );
	  P[n].setXLabel( "[ms]" );
	  P[n].setXLabelPos( 0.0, Plot::Screen, 0.0, Plot::FirstAxis, 
			     Plot::Left, 0.0 );
	  P[n].setXTics();
	  P[n].setYLabel( "Beat" );
	  P[n].setYLabelPos( -0.55, Plot::FirstAxis, 0.5, Plot::Graph, 
			     Plot::Center, -90.0 );
	  P[n].setYRange( minampl, maxampl );
	  P[n].setYTics( );
	}
	else if ( row == 0 && sub == 0 ) {
	  P[n].setOrigin( xo+col*dx, 0.0 );
	  P[n].setSize( dx, yo+dys*dy );
	  P[n].setBMarg( xlabelmarg );
	  P[n].setXTics();
	  P[n].setYRange( minampl, maxampl );
	}
	else if ( col == 0 && sub == 0 ) {
	  P[n].setOrigin( 0.0, yo+row*dy );
	  P[n].setSize( xo+dx, dys*dy );
	  P[n].setLMarg( ylabelmarg );
	  P[n].setBMarg( 3.0*xmarg );
	  P[n].setYLabel( "Beat" );
	  P[n].setYLabelPos( -0.55, Plot::FirstAxis, 0.5, Plot::Graph, 
			     Plot::Center, -90.0 );
	  P[n].setYRange( minampl, maxampl );
	  P[n].setYTics();
	}
	else if ( col == 0 && sub > 0 ) {
	  P[n].setOrigin( 0.0, yo+row*dy+dys*(nsub-sub)*dy );
	  P[n].setSize( xo+dx, dys*dy );
	  P[n].setLMarg( ylabelmarg );
	  if ( sub == nsub-1 )
	    P[n].setTMarg( 3.0*xmarg );
	  if ( NerveTraces > 0 && sub > SpikeTraces )
	    P[n].setYLabel( "Potential [" + trace( NerveTrace[sub-SpikeTraces-1] ).unit() + "]" );
	  else
	    P[n].setYLabel( "Rate " + Str( sub ) + " [Hz]" );
	  P[n].setYLabelPos( -0.55, Plot::FirstAxis, 0.5, Plot::Graph, 
			     Plot::Center, -90.0 );
	  P[n].setYRange( 0.0, ratemax );
	  P[n].setYTics();
	}
	else if ( sub == 0 ) {
	  P[n].setOrigin( xo+col*dx, yo+row*dy );
	  P[n].setSize( dx, dys*dy );
	  P[n].setBMarg( 3.0*xmarg );
	  P[n].setYRange( minampl, maxampl );
	}
	else {
	  P[n].setOrigin( xo+col*dx, yo+row*dy+dys*(nsub-sub)*dy );
	  P[n].setSize( dx, dys*dy );
	  if ( sub == nsub-1 )
	    P[n].setTMarg( 3.0*xmarg );
	  P[n].setYRange( 0.0, ratemax );
	}
	for ( int kr=0; kr<Rows; kr++ )
	  for ( int kc=0; kc<Cols; kc++ )
	    P.setCommonYRange( kr*Cols*nsub+kc*nsub+sub, n );
	n++;
      }
    }
  }
  P.setCommonXRange();
  P.draw();
  P.unlock();
}


int Chirps::main( void )
{
  // get options:
  NChirps = integer( "nchirps" );
  MinSpace = number( "minspace" );
  FirstSpace = number( "firstspace" );
  Pause = number( "pause" );
  Repeats = integer( "repeats" );
  DeltaF = number( "deltaf" );
  Contrast = number( "contrast" );
  ChirpSize = number( "chirpsize" );
  ChirpWidth = number( "chirpwidth" );
  ChirpSel = index( "chirpsel" );
  ChirpKurtosis = number( "chirpkurtosis" );
  ChirpDip = number( "chirpampl" );
  ChirpFile = text( "file" );
  BeatPos = integer( "beatpos" );
  BeatStart = number( "beatstart" );
  Sigma = number( "sigma" );
  AM = boolean( "am" );
  SineWave = boolean( "sinewave" );
  Playback = boolean( "playback" );
  bool adjustg = boolean( "adjust" );

  // checks:
  if ( ::fabs(DeltaF) < 1e-3 ) {
    warning( "deltaf must not be zero." );
    return Failed;
  }
  if ( LocalEODTrace[0] < 0 ) {
    warning( "need local recording of the EOD Trace." );
    return Failed;
  }
  if ( LocalEODEvents[0] < 0 ) {
    warning( "need EOD events of local recording of the EOD Trace." );
    return Failed;
  }

  // data:
  OutWarning = true;
  Mode = 0;
  Intensity = 0.0;
  Count = 0;

  // check for GlobalEFieldEvents:
  if ( !AM && GlobalEFieldEvents < 0 ) {
    warning( "Need stimulus recording for non AM-stimuli!" );
    stop();
    return Failed;
  }

  // EOD rate:
  if ( EODEvents >= 0 )
    FishRate = events(EODEvents).frequency( ReadCycles );
  else
    FishRate = events(LocalEODEvents[0]).frequency( ReadCycles );
  if ( FishRate <= 0.0 ) {
    warning( "Not enough EOD cycles recorded!" );
    stop();
    return Failed;
  }

  Header.erase( "Settings" );
  Header.newSection( settings() );
  Header.erase( "FileChirp" );

  // trigger:
  //  setupTrigger( data, events );

  // EOD amplitude:
  FishAmplitude = eodAmplitude( trace(LocalEODTrace[0]),
				currentTime() - 0.5, currentTime() );

  // plot:
  double ampl = meanPeaks( trace(LocalEODTrace[0]), currentTime() - 0.5, currentTime(),
			   eodThreshold( trace(LocalEODTrace[0]), currentTime() - 0.5, currentTime(), 0.0 ) );
  initMultiPlot( ampl, Contrast );

  // first stimulus:
  if ( ChirpSel == 1 ) {
    // from file:
    DataFile sf( ChirpFile );
    sf.read();
    if ( sf.dataLines() == 0 ) {
      warning( "Chirp-waveform file " + ChirpFile + " does not exist or does not contain any data." );
      return 1;
    }
    if(sf.data().columns() < 2){
      warning( "Chirp-waveform file " + ChirpFile + " needs at least two columns." );
      return 1;
    }

    ArrayD times = sf.col( 0 );
    times *= Parameter::changeUnit( 1.0, sf.key().unit( 0 ), "s" );
    ArrayD freqs = sf.col( 1 );
    freqs /= maxAbs( freqs );
    ArrayD ampls;
    if(sf.data().columns() == 2){
      ampls = freqs;
    }
    else{
      ampls = sf.col( 2 );
      if ( ampls.empty() )
	ampls = freqs;
    }

    // chirp width:
    int maxfreqinx = maxIndex( freqs );
    int l = 0;
    int r = freqs.size()-1;
    for ( l=maxfreqinx-1; l>=0; l-- )
      if ( freqs[l] < 0.1 )
	break;
    for ( r=maxfreqinx+1; r< freqs.size(); r++ )
      if ( freqs[r] < 0.1 )
	break;
    double filechirpwidth = times[r] - times[l];
    times -= times[maxfreqinx];
    times *= ChirpWidth/filechirpwidth;
    ChirpFreqs.assign( times, freqs );
    ChirpAmpls.assign( times, ampls );
    lockMetaData();
    Header.newSection( sf.metaDataOptions( 0 ), "FileChirp" );
    unlockMetaData();
  }

  OutData signal;
  if ( AM ) {
    if ( createAM( signal ) ) {
      stop();
      return Failed;
    }
  }
  else
    createEOD( signal );

  // more data:
  Response.clear();
  Response.reserve( 2 * (Repeats>0?Repeats:100) * 2 * NChirps );
  for ( int k=0; k<MaxTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      Spikes[k].clear();
      SpikeRate[k].resize( BeatPos );
      for ( unsigned int i=0; i<SpikeRate[k].size(); i++ )
	SpikeRate[k][i].resize( 2, RateData( SaveWindow, 0.25*Sigma ) );
      MaxRate[k] = 20.0;
    }
  }
  if ( NerveTrace[0] >= 0 ) {
    NerveMeanAmplP.resize( BeatPos );
    NerveMeanAmplT.resize( BeatPos );
    NerveMeanAmplM.resize( BeatPos );
    NerveMeanAmplS.resize( BeatPos );
    for ( unsigned int i=0; i<NerveMeanAmplP.size(); i++ ) {
      NerveMeanAmplP[i].resize( 2, SampleDataD( -SaveWindow, SaveWindow, 0.001 ) );
      NerveMeanAmplT[i].resize( 2, SampleDataD( -SaveWindow, SaveWindow, 0.001 ) );
      NerveMeanAmplM[i].resize( 2, SampleDataD( -SaveWindow, SaveWindow, 0.001 ) );
      NerveMeanAmplS[i].resize( 2, SampleDataD( -SaveWindow, SaveWindow, 0.0001 ) );
    }
    NerveAmplP.clear();
    NerveAmplT.clear();
    NerveAmplM.clear();
    NerveAmplS.clear();
  }
  EODAmplitude.clear();

  // plot trace:
  if ( Duration <= 0.5 )
    tracePlotSignal( Duration );
  else
    tracePlotContinuous( 1.0 );

  // adjust transdermal EOD:
  double val2 = trace(LocalEODTrace[0]).maxAbs( currentTime()-0.1,
						currentTime() );
  if ( val2 > 0.0 )
    adjustGain( trace(LocalEODTrace[0]), ( 1.0 + Contrast ) * val2 );

  for ( Count=0;
	( Repeats <= 0 || Count < Repeats ) && softStop() == 0; 
	Count++ ) {

    // stimulus intensity:
    Intensity = Contrast * FishAmplitude * IntensityGain;
    signal.setIntensity( Intensity );
    //  detectorOpts( BeatPeakEvents2 ).setNumber( "threshold", 0.5*signal.intensity() );

    // meassage: 
    Str s = "<b>" + Str( Mode == 1 || AM ? "AM " : "EOD" ) + "</b>"; 
    s += "  Contrast: <b>" + Str( 100.0 * Contrast, 0, 5, 'g' ) + "%</b>";
    s += "  Delta F: <b>" + Str( DeltaF, 0, 1, 'f' ) + "Hz</b>";
    s += "  Size: <b>" + Str( ChirpSize, 0, 0, 'f' ) + "Hz</b>";
    s += "  Phase: <b>" + Str( ChirpPhase, 0, 2, 'f' ) + "</b>";
    s += "  Loop: <b>" + Str( Count+1 ) + "</b>";
    message( s );

    // output signal:
    write( signal );
    if ( !signal.success() ) {
      string s = "Output of stimulus failed!<br>Error code is <b>" + signal.errorText() + "</b>";
      warning( s, 4.0 );
      stop();
      return Failed;
    }

    sleep( Pause );
    if ( interrupt() ) {
      if ( Count > 1 ) {
	save();
	stop();
	return Completed;
      }
      else {
	stop();
	return Aborted;
      }
    }

    // signal failed?
    if ( !signal.success() ) {
      if ( signal.busy() ) {
	if ( OutWarning ) {
	  warning( "Output still busy!<br> Probably missing trigger.<br> Output of this signal software-triggered.", 4.0 );
	  OutWarning = false;
	}
	signal.setStartSource( 0 );
	signal.setPriority();
	write( signal );
	sleep( Pause );
	if ( interrupt() ) {
	  if ( Count > 1 ) {
	    save();
	    stop();
	    return Completed;
	  }
	  else {
	    stop();
	    return Aborted;
	  }
	}
	// trigger:
	//	setupTrigger( data, events );
      }
      else if ( signal.error() == signal.OverflowUnderrun ) {
	if ( OutWarning ) {
	  warning( "Analog Output Overrun Error!<br> Try again.", 2.0 );
	  OutWarning = false;
	}
	write( signal );
	sleep( Pause );
	if ( interrupt() ) {
	  if ( Count > 1 ) {
	    save();
	    stop();
	    return Completed;
	  }
	  else {
	    stop();
	    return Aborted;
	  }
	}
      }
      else {
	if ( OutWarning ) {
	  string s = "Output of stimulus failed!<br>Error code is <b>";
	  s += Str( signal.error() ) + ": " + signal.errorStr() + "</b>";
	  warning( s, 4.0 );
	  OutWarning = false;
	}
	stop();
	return Failed;
      }
      continue;
    }

    // adjust input gains:
    if ( adjustg ) {
      for ( int k=0; k<MaxTraces; k++ )
	if ( SpikeTrace[k] >= 0 )
	  adjust( trace(SpikeTrace[k]), Duration, 0.8 );
      if ( NerveTrace[0] >= 0 )
	adjust( trace(NerveTrace[0]), Duration, 0.8 );
    }
    if ( GlobalEFieldTrace >= 0 )
      adjustGain( trace(GlobalEFieldTrace), 1.05 * trace(GlobalEFieldTrace).maxAbs( signalTime(), Duration ) );

    // analyze:
    analyze();
    plot();

    // save:
    if ( Repeats > 0 ) {
      Header.setInteger( "index", StimulusIndex );
      if ( Count == 0 ) {
	Header.setNumber( "chirpphase", ChirpPhase );
	Header.setNumber( "duration", Duration );
	Header.setText( "waveform", SineWave ? "Sine-Wave" : "Fish-EOD" );
	Header.setNumber( "true deltaf", TrueDeltaF );
	Header.setNumber( "true contrast", 100.0 * TrueContrast );
	Header.setNumber( "EOD rate", FishRate );
	Header.setUnit( "trans. amplitude", EOD2Unit );
	Header.setNumber( "trans. amplitude", FishAmplitude );
	Header.setText( "repro time", reproTimeStr() );
	Header.setText( "session time", sessionTimeStr() );
	ChirpKey.setUnit( "ampl-", EOD2Unit );
	ChirpKey.setUnit( "ampl+", EOD2Unit );
	ChirpKey.setUnit( "jump", EOD2Unit );
	ChirpKey.setUnit( "max", EOD2Unit );
	ChirpKey.setUnit( "min", EOD2Unit );
	ChirpKey.setUnit( "diff", EOD2Unit );
	ChirpTraceKey.setUnit( "ampl", EOD2Unit );
	AmplKey.setUnit( "ampl", EOD2Unit );
      }
      Header.setText( "stimulus type", Mode>0 || AM ? "AM" : "EOD" );
      Header.setInteger( "trace", -1 );
      saveChirps();
      saveChirpTraces();
      saveAmplitude();
      for ( int trace=0; trace<MaxTraces; trace++ ) {
	if ( SpikeEvents[trace] >= 0 ) {
	  Header.setInteger( "trace", trace );
	  saveChirpSpikes( trace );
	  saveSpikes( trace );
	}
      }
      if ( NerveTrace[0] >= 0 ) {
	Header.setInteger( "trace", 0 );
	saveChirpNerve();
	saveNerve();
      }
    }
    StimulusIndex++;

    // next stimulus:
    if ( Playback ) {
      Mode++;
      if ( Mode == 1 )
	createPlayback( signal );
      else {
	Mode = 0;
	createEOD( signal );
      }
    }

  }

  save();
  stop();
  return Completed;
}


void Chirps::sessionStarted( void )
{
  StimulusIndex = 0;
  RePro::sessionStarted();
}


void Chirps::sessionStopped( bool saved )
{
  StimulusIndex = 0;
  RePro::sessionStopped( saved );
}


void Chirps::stop( void )
{
  ChirpTimes.clear();
  BeatPhases.clear();
  Response.clear();
  for ( int k=0; k<MaxTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      Spikes[k].clear();
      SpikeRate[k].clear();
    }
  }
  NerveMeanAmplP.clear();
  NerveMeanAmplT.clear();
  NerveMeanAmplM.clear();
  NerveMeanAmplS.clear();
  NerveAmplP.free();
  NerveAmplT.free();
  NerveAmplM.free();
  NerveAmplS.free();
  EODAmplitude.free();
  writeZero( Mode == 1 || AM ? GlobalAMEField : GlobalEField );
}


void Chirps::saveChirps( void )
{
  // create file:
  ofstream df( addPath( SineWave ? "chirpss.dat" : "chirps.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  ChirpKey.saveKey( df, true, false );

  // write data into file:
  for ( unsigned int k=FirstResponse; k<Response.size(); k++ ) {
    ChirpKey.save( df, k-FirstResponse, 0 );
    ChirpKey.save( df, Response[k].Time );
    ChirpKey.save( df, Response[k].Trace );
    ChirpKey.save( df, Response[k].Size );
    ChirpKey.save( df, Response[k].Width );
    ChirpKey.save( df, Response[k].Kurtosis );
    ChirpKey.save( df, Response[k].Amplitude );
    ChirpKey.save( df, Response[k].Phase );
    ChirpKey.save( df, Response[k].EODRate );
    ChirpKey.save( df, Response[k].BeatFreq );
    ChirpKey.save( df, Response[k].BeatPhase );
    ChirpKey.save( df, Response[k].BeatLoc );
    ChirpKey.save( df, Response[k].BeatBin );
    ChirpKey.save( df, Response[k].BeatBefore );
    ChirpKey.save( df, Response[k].BeatAfter );
    ChirpKey.save( df, Response[k].BeatAfter - Response[k].BeatBefore );
    ChirpKey.save( df, Response[k].BeatPeak );
    ChirpKey.save( df, Response[k].BeatTrough );
    ChirpKey.save( df, Response[k].BeatPeak - Response[k].BeatTrough );
    ChirpKey.save( df, 100.0*( Response[k].BeatPeak - Response[k].BeatTrough ) /
		   ( Response[k].BeatPeak + Response[k].BeatTrough ) );
    df << '\n';
  }
  df << "\n\n";
}


void Chirps::saveChirpTraces( void )
{
  // create file:
  ofstream df( addPath( SineWave ? "chirptracess.dat" : "chirptraces.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  ChirpTraceKey.saveKey( df, true, false );

  // write data into file:
  for ( unsigned int k=FirstResponse; k<Response.size(); k++ ) {
    df << '\n';
    df << "# chirp index: " << k-FirstResponse << '\n';
    df << "#  beat phase: " << Str( Response[k].BeatPhase, "%.3f" ) << '\n';
    df << "#    beat pos: " << Str( Response[k].BeatLoc, "%.3f" ) << '\n';
    df << "#    beat bin: " << Response[k].BeatBin << '\n';
    for ( int j=0; j<Response[k].EODTime.size(); j++ ) {
      ChirpTraceKey.save( df, Response[k].EODTime[j], 0 );
      ChirpTraceKey.save( df, Response[k].EODFreq[j], 1 );
      ChirpTraceKey.save( df, Response[k].EODAmpl[j], 2 );
      df << '\n';
    }
  }
  df << "\n\n";
}


void Chirps::saveChirpSpikes( int trace )
{
  // create file:
  ofstream df( addPath( SineWave ? "chirpspikess" + Str(trace+1) + ".dat" : 
			"chirpspikes" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  SpikesKey.saveKey( df, true, false );

  // write data into file:
  for ( unsigned int j=FirstResponse; j<Response.size(); j++ ) {
    df << '\n';
    df << "# chirp index: " << j-FirstResponse << '\n';
    df << "#  beat phase: " << Str( Response[j].BeatPhase, "%.3f" ) << '\n';
    df << "#    beat pos: " << Str( Response[j].BeatLoc, "%.3f" ) << '\n';
    df << "#    beat bin: " << Response[j].BeatBin << '\n';
    if ( Response[j].Spikes[trace].size() > 0 ) {
      for ( int i=0; i<Response[j].Spikes[trace].size(); i++ ) {
	SpikesKey.save( df, 1000.0*Response[j].Spikes[trace][i], 0 );
	df << '\n';
      }
    }
    else 
      df << "  -0\n";
  }
  df << "\n\n";
}


void Chirps::saveChirpNerve( void )
{
  {
    // create file:
    ofstream df( addPath( SineWave ? "chirpnerveampls.dat" : 
			  "chirpnerveampl.dat" ).c_str(),
		 ofstream::out | ofstream::app );
    if ( ! df.good() )
      return;

    // write header and key:
    Header.save( df, "# ", 0, Options::FirstOnly );
    df << '\n';
    NerveKey.saveKey( df, true, true );

    // write data into file:
    for ( unsigned int j=FirstResponse; j<Response.size(); j++ ) {
      df << '\n';
      df << "# chirp index: " << j-FirstResponse << '\n';
      df << "#  beat phase: " << Str( Response[j].BeatPhase, "%.3f" ) << '\n';
      df << "#    beat pos: " << Str( Response[j].BeatLoc, "%.3f" ) << '\n';
      df << "#    beat bin: " << Response[j].BeatBin << '\n';
      if ( Response[j].NerveAmplP.empty() ) {
	NerveKey.save( df, 0.0, 0 );
	NerveKey.save( df, 0.0 );
	NerveKey.save( df, 0.0 );
	NerveKey.save( df, 0.0 );
	NerveKey.save( df, 0.0 );
	NerveKey.save( df, 0.0 );
	df << '\n';
      }
      else {
	for ( int i=0; i<Response[j].NerveAmplP.size(); i++ ) {
	  NerveKey.save( df, 1000.0 * Response[j].NerveAmplP.x( i ), 0 );
	  NerveKey.save( df, Response[j].NerveAmplP.y(i) );
	  NerveKey.save( df, 1000.0 * Response[j].NerveAmplT.x( i ) );
	  NerveKey.save( df, Response[j].NerveAmplT.y(i) );
	  NerveKey.save( df, 1000.0 * Response[j].NerveAmplM.x( i ) );
	  NerveKey.save( df, Response[j].NerveAmplM.y(i) );
	  df << '\n';
	}
      }
    }
    df << "\n\n";
  }

  {
    // create file:
    ofstream df( addPath( SineWave ? "chirpnervesmoothampls.dat" : 
			  "chirpnervesmoothampl.dat" ).c_str(),
		 ofstream::out | ofstream::app );
    if ( ! df.good() )
      return;

    // write header and key:
    Header.save( df, "# ", 0, Options::FirstOnly );
    df << '\n';
    SmoothKey.saveKey( df, true, true );

    // write data into file:
    for ( unsigned int j=FirstResponse; j<Response.size(); j++ ) {
      df << '\n';
      df << "# chirp index: " << j-FirstResponse << '\n';
      df << "#  beat phase: " << Str( Response[j].BeatPhase, "%.3f" ) << '\n';
      df << "#    beat pos: " << Str( Response[j].BeatLoc, "%.3f" ) << '\n';
      df << "#    beat bin: " << Response[j].BeatBin << '\n';
      if ( Response[j].NerveAmplS.empty() ) {
	SmoothKey.save( df, 0.0, 0 );
	SmoothKey.save( df, 0.0 );
	df << '\n';
      }
      else {
	for ( int i=0; i<Response[j].NerveAmplS.size(); i++ ) {
	  SmoothKey.save( df, 1000.0 * Response[j].NerveAmplS.pos( i ), 0 );
	  SmoothKey.save( df, Response[j].NerveAmplS[i] );
	  df << '\n';
	}
      }
    }
    df << "\n\n";
  }
}


void Chirps::saveAmplitude( void )
{
  // create file:
  ofstream df( addPath( SineWave ? "chirpeodampls.dat" : "chirpeodampl.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  AmplKey.saveKey( df, true, false );

  // write data into file:
  for ( int k=0; k<EODAmplitude.size(); k++ ) {
    AmplKey.save( df, 1000.0 * EODAmplitude.x(k), 0 );
    AmplKey.save( df, EODAmplitude.y(k), 1 );
    df << '\n';
  }
  df << "\n\n";
}


void Chirps::saveSpikes( int trace )
{
  // create file:
  ofstream df( addPath( SineWave ? "chirpallspikess" + Str(trace+1) + ".dat" :
			"chirpallspikes" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  SpikesKey.saveKey( df, true, false );

  // write data into file:
  if ( Spikes[trace].size() > 0 ) {
    for ( int j=0; j<Spikes[trace].size(); j++ ) {
      SpikesKey.save( df, 1000.0 * Spikes[trace][j], 0 );
      df << '\n';
    }
  }
  else 
    df << "  -0\n";
  df << "\n\n";
}


void Chirps::saveChirpRate( int trace )
{
  // create file:
  ofstream df( addPath( SineWave ? "chirprates" + Str(trace+1) + ".dat" :
			"chirprate" + Str(trace+1) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Header.setInteger( "index", totalRuns() );
  Header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "time", "ms", "%7.2f" );
  key.addNumber( "rate", "Hz", "%5.1f" );
  key.addNumber( "frequency", "Hz", "%5.1f" );

  // write data into file:
  for ( unsigned int n=0; n<SpikeRate[trace].size(); n++ ) {
    for ( int m=0; m<2; m++ ) {
      df << "#      beat bin: " << n << '\n';
      df << "#      beat pos: " << Str( n*1.0/BeatPos, "%.3f" ) << '\n';
      df << "# stimulus type: " << ( m > 0 ? "AM" : "EOD" ) << '\n';
      df << "# stimulus mode: " << m << '\n';
      // write key:
      key.saveKey( df, true, false );
      // write data:
      for ( int j=0; j<SpikeRate[trace][n][m].Rate.size(); j++ ) {
	key.save( df, 1000.0*SpikeRate[trace][n][m].Rate.pos( j ), 0 );
	key.save( df, SpikeRate[trace][n][m].Rate[j] );
	key.save( df, SpikeRate[trace][n][m].Frequency[j] );
	df << '\n';
      }
      df << '\n';
    }
  }
  df << '\n';
}


void Chirps::saveNerve( void )
{
  {
    // create file:
    ofstream df( addPath( SineWave ? "chirpallnerveampls.dat" :
			  "chirpallnerveampl.dat" ).c_str(),
		 ofstream::out | ofstream::app );
    if ( ! df.good() )
      return;

    // write header and key:
    Header.save( df, "# ", 0, Options::FirstOnly );
    df << '\n';
    NerveKey.saveKey( df, true, true );

    // write data into file:
    if ( NerveAmplP.empty() ) {
      NerveKey.save( df, 0.0, 0 );
      NerveKey.save( df, 0.0 );
      NerveKey.save( df, 0.0 );
      NerveKey.save( df, 0.0 );
      NerveKey.save( df, 0.0 );
      NerveKey.save( df, 0.0 );
      df << '\n';
    }
    else {
      for ( int j=0; j<NerveAmplP.size(); j++ ) {
	NerveKey.save( df, 1000.0 * NerveAmplP.x( j ), 0 );
	NerveKey.save( df, NerveAmplP.y( j ) );
	NerveKey.save( df, 1000.0 * NerveAmplT.x( j ) );
	NerveKey.save( df, NerveAmplT.y( j ) );
	NerveKey.save( df, 1000.0 * NerveAmplM.x( j ) );
	NerveKey.save( df, NerveAmplM.y( j ) );
	df << '\n';
      }
    }
    df << "\n\n";
  }

  {
    // create file:
    ofstream df( addPath( SineWave ? "chirpallnervesmoothampls.dat" :
			  "chirpallnervesmoothampl.dat" ).c_str(),
		 ofstream::out | ofstream::app );
    if ( ! df.good() )
      return;

    // write header and key:
    Header.save( df, "# ", 0, Options::FirstOnly );
    df << '\n';
    SmoothKey.saveKey( df, true, true );

    // write data into file:
    if ( NerveAmplS.empty() ) {
      SmoothKey.save( df, 0.0, 0 );
      SmoothKey.save( df, 0.0 );
      df << '\n';
    }
    else {
      for ( int j=0; j<NerveAmplS.size(); j++ ) {
	SmoothKey.save( df, 1000.0 * NerveAmplS.pos( j ), 0 );
	SmoothKey.save( df, NerveAmplS[j] );
	df << '\n';
      }
    }
    df << "\n\n";
  }
}


void Chirps::save( void )
{
  if ( Repeats <= 0 )
    return;

  for ( int trace=0; trace<MaxTraces; trace++ ) {
    if ( SpikeEvents[trace] >= 0 ) {
      Header.setInteger( "trace", trace );
      saveChirpRate( trace );
    }
  }
}


void Chirps::plot( void )
{
  P.lock();
  int n = 0;
  int beatbin = 0;
  for ( int row=Rows-1; row >= 0; row-- ) {
    for ( int col=0; col < Cols; col++ ) {

      // amplitude:
      P[n].clear();
      P[n].plotVLine( 0.0, Plot::White, 1 );
      for ( unsigned int k = 0; k < Response.size(); k++ )
	if ( Response[k].BeatBin == beatbin ) {
	  int color = Plot::DarkGreen;
	  if ( Response[k].Index == StimulusIndex )
	    color = Plot::Green;
	  P[n].plot( Response[k].EODTime,
		     Response[k].EODAmpl,
		     color, 2, Plot::Solid );
	}
      n++;

      // rate and spikes:
      int maxspikes = (int)rint( 14.0 / SpikeTraces );
      if ( maxspikes < 4 )
	maxspikes = 4;
      for ( int k=0; k<MaxTraces; k++ ) {
	if ( SpikeEvents[k] >= 0 ) {
	  P[n].clear();
	  if ( ! P[n].zoomedYRange() )
	    P[n].setYRange( 0.0, MaxRate[k] );
	  P[n].plotVLine( 0.0, Plot::White, 1 );
	  double delta = Repeats > 0 && Repeats < maxspikes ? 1.0/Repeats : 1.0/maxspikes;
	  int nsp=0;
	  for ( unsigned int j = 0; j < Response.size(); j++ ) {
	    if ( Response[j].BeatBin == beatbin )
	      nsp++;
	  }
	  int offs = nsp > maxspikes ? nsp - maxspikes : 0;
	  int ns = 0;
	  for ( unsigned int j = 0; j < Response.size(); j++ ) {
	    if ( Response[j].BeatBin == beatbin ) {
	      ns++;
	      if ( ns > offs ) {
		int color = Plot::Red;
		if ( Response[j].Mode == 1 )
		  color = Plot::Magenta;
		P[n].plot( Response[j].Spikes[k], 1000.0,
			   1.0 - delta*(ns-offs-0.1), Plot::Graph, 2, Plot::StrokeUp,
			   delta*0.8, Plot::Graph, color, color );
	      }
	    }
	  }
	  if ( Playback )
	    P[n].plot( SpikeRate[k][beatbin][1].Rate,
		       1000.0, Plot::Orange, 2, Plot::Solid );
	  else
	    P[n].plot( SpikeRate[k][beatbin][0].Frequency,
		       1000.0, Plot::Orange, 2, Plot::Solid );
	  P[n].plot( SpikeRate[k][beatbin][0].Rate,
		     1000.0, Plot::Yellow, 2, Plot::Solid );
	  n++;
	}
      }

      if ( NerveTraces > 0 ) {
	P[n].clear();
	P[n].setAutoScaleY();
	P[n].plotVLine( 0.0, Plot::White, 1 );
	for ( unsigned int j = 0; j < Response.size(); j++ )
	  if ( Response[j].BeatBin == beatbin ) {
	    P[n].plot( Response[j].NerveAmplM, 1000.0, Plot::Cyan, 1, Plot::Solid );
	  }
	P[n].plot( NerveMeanAmplM[beatbin][0], 1000.0, Plot::Magenta, 2, Plot::Solid );
	n++;
      }

      beatbin++;
    }
  }

  P.draw();

  P.unlock();
}


void Chirps::analyze( void )
{
  const EventData &eod1 = EODEvents >= 0 ? events(EODEvents) : events(LocalEODEvents[0]);
  const EventData &eod2 = events(LocalEODEvents[0]);

  GaussKernel kernel;
  kernel.setStdev( Sigma );

  // EOD trace:
  EOD2Unit = trace(LocalEODTrace[0]).unit();

  // EOD rate:
  FishRate = eod1.frequency( ReadCycles );

  // signal frequency:
  double sigfreq = AM ? 0.0 : events(GlobalEFieldEvents).frequency( signalTime(),
								    signalTime()+Duration );

  // Delta F:
  bool noglobaleod = false;
  if ( AM )
    TrueDeltaF = DeltaF;
  else {
    TrueDeltaF = sigfreq - FishRate;
    if ( fabs( (TrueDeltaF - DeltaF)/DeltaF ) > 0.1 ) {
      TrueDeltaF = DeltaF;
      noglobaleod = true;
    }
  }

  // EOD amplitude:
  FishAmplitude = eodAmplitude( trace(LocalEODTrace[0]),
				currentTime() - 0.5, currentTime() );

  // contrast:
  TrueContrast = beatContrast( trace(LocalEODTrace[0]),
			       signalTime()+0.1*Duration,
			       signalTime()+0.9*Duration,
			       fabs( DeltaF ) );

  // EOD transdermal amplitude:
  EventIterator first2 = eod2.begin( signalTime() );
  EventIterator last2 = eod2.begin( signalTime() + Duration ); 
  EODAmplitude.clear();
  EODAmplitude.reserve( last2 - first2 + 2 );
  for ( EventSizeIterator sindex = first2; sindex < last2; ++sindex ) {
    EODAmplitude.push( sindex.time() - signalTime(), *sindex );
  }

  // all spikes:
  for ( int k=0; k<MaxTraces; k++ )
    if ( SpikeEvents[k] >= 0 )
      events(SpikeEvents[k]).copy( signalTime(), signalTime()+Duration,
				   Spikes[k] );

  // Nerve potential:
  if ( NerveTrace[0] >= 0 ) {
    const InData &nd = trace( NerveTrace[0] );
    // nerve amplitudes:
    // peak and trough amplitudes:
    double threshold = nd.stdev( signalTime(), signalTime()+4.0/FishRate );
    if ( threshold < 1.0e-8 )
      threshold = 0.001;
    EventList peaktroughs( 2, (int)rint(1500.0*Duration), true );
    InData::const_iterator firstn = nd.begin( signalTime() );
    InData::const_iterator lastn = nd.begin( signalTime()+Duration );
    if ( lastn > nd.end() )
      lastn = nd.end();
    Detector< InData::const_iterator, InDataTimeIterator > D;
    D.init( firstn, lastn, nd.timeBegin( signalTime() ) );
    D.peakTrough( firstn, lastn, peaktroughs, threshold,
		  threshold, threshold, NerveAcceptEOD );
    // store amplitudes:
    NerveAmplP.clear();
    NerveAmplT.clear();
    NerveAmplM.clear();
    NerveAmplS.clear();
    NerveAmplP.reserve( peaktroughs[0].size() + 100 );
    NerveAmplT.reserve( peaktroughs[0].size() + 100 );
    NerveAmplM.reserve( peaktroughs[0].size() + 100 );
    for ( int k=0; k<peaktroughs[0].size() && k<peaktroughs[1].size(); k++ ) {
      NerveAmplP.push( peaktroughs[0][k] - signalTime(), 
		       peaktroughs[0].eventSize( k ) );
      NerveAmplT.push( peaktroughs[1][k] - signalTime(), 
		       peaktroughs[1].eventSize( k ) );
    }
    // averaged amplitude:
    if ( peaktroughs[0].size() > 1 ) {
      double st = (peaktroughs[0].back() - peaktroughs[0].front())/double(peaktroughs[0].size()-1);
      for ( int k=0; k<NerveAmplP.size(); k++ ) {
	double t = NerveAmplP.x( k ) + signalTime();
	NerveAmplM.push( t - signalTime(), nd.mean( t-0.5*st, t+0.5*st ) );
      }
      // smoothed averaged amplitude:
      NerveAmplS = SampleDataD( 0.0, Duration, 0.0001 );
      for ( int k=0; k<NerveAmplS.size(); k++ ) {
	double t = signalTime() + NerveAmplS.pos(k);
	NerveAmplS[k] = nd.mean( t-0.5*st, t+0.5*st );
      }
    }
  }

  FirstResponse = Response.size();
  int lastr = Response.size() - ChirpTimes.size();

  // chirps:
  for ( int k=0; k<ChirpTimes.size(); k++ ) {

    // mean rate at chirp:
    double eodrate = eod1.frequency( signalTime() + ChirpTimes[k] - 3.0*ChirpWidth,
				     signalTime() + ChirpTimes[k] + 3.0*ChirpWidth );

    double beatpeak;
    double beattrough;
    double beatfreq;
    double beatphase;
    double beatloc;
    int beatbin;
    double beatbefore;
    double beatafter;

    if ( Mode == 0 ) {

      // mean EOD interval at the chirp:
      double meaninterv = 1.0 / eodrate;

      // current deltaf:
      double cdeltaf = AM || noglobaleod ? DeltaF : sigfreq - eodrate;

      // beat amplitudes before chirp:
      double t = signalTime() + ChirpTimes[k] - 1.0 * ChirpWidth;
      int bpe = events(LocalBeatPeakEvents[0]).previous( t );
      int bpee = eod2.next( events(LocalBeatPeakEvents[0])[bpe] );
      beatpeak = eod2.eventSize( bpee );
      int bte = events(LocalBeatTroughEvents[0]).previous( t );
      int btee = eod2.next( events(LocalBeatTroughEvents[0])[bte] );
      beattrough = eod2.eventSize( btee );

      // beat frequency before chirp:
      beatfreq = 1.0/(events(LocalBeatPeakEvents[0])[bpe] - events(LocalBeatPeakEvents[0])[bpe-1]);

      // location of chirp relative to beat:
      if ( AM ) {
	beatphase = BeatPhases[k];
      }
      else {
	const EventData &sige = events(GlobalEFieldEvents);
	double siginterv = 1.0 / sigfreq;
	int sigi = sige.next( signalTime() + ChirpTimes[k] - 2.0*ChirpWidth );
	double sigtime = 0.0;
	int maxn = 8;
	for ( int n=0; n < maxn && sigi-n < sige.size(); n++ )
	  sigtime += ( sige[ sigi - n ] + siginterv * double(n) ) / double( maxn );
	sigi = (int)floor( ( signalTime() + ChirpTimes[k] - sigtime ) / siginterv );
	maxn = 1;
	beatphase = 0.0;
	for ( int n=0; n < maxn; n++ ) {
	  double t1 = sigtime + ( sigi - maxn/2 + n ) * siginterv;
	  int pi = eod2.previous( t1 );
	  double t0 = eod2[ pi ];
	  double phase = ( t1 - t0 ) / meaninterv;
	  beatphase += ( phase - beatphase ) / (n+1);
	}
      }

      // beat location:
      beatloc = cdeltaf <= 0.0 ? beatphase : 1.0 - beatphase;

      // beat bin:
      double db = 1.0/BeatPos;
      beatbin = int( floor( (beatloc + 0.5*db)/db ) );
      if ( beatbin < 0 )
	beatbin = 0;
      if ( beatbin >= BeatPos )
	beatbin = 0;
      if ( beatbin >= BeatPos )
	beatbin = BeatPos-1;

      // beat amplitude right before chirp:
      beatbefore = eodAmplitude( trace(LocalEODTrace[0]),
				 signalTime() + ChirpTimes[k] - ChirpWidth,
				 signalTime() + ChirpTimes[k] - ChirpWidth + 4.0 * meaninterv );
      // beat amplitude right after chirp:
      beatafter = eodAmplitude( trace(LocalEODTrace[0]),
				signalTime() + ChirpTimes[k] + ChirpWidth - 4.0 * meaninterv,
				signalTime() + ChirpTimes[k] + ChirpWidth );

    }
    else {
      beatfreq = Response[lastr+k].BeatFreq;
      beatphase = Response[lastr+k].BeatPhase;
      beatloc = Response[lastr+k].BeatLoc;
      beatbin = Response[lastr+k].BeatBin;
      beatbefore = Response[lastr+k].BeatBefore;
      beatafter = Response[lastr+k].BeatAfter;
      beatpeak = Response[lastr+k].BeatPeak;
      beattrough = Response[lastr+k].BeatTrough;
    }

    // store results:
    ChirpData d( StimulusIndex, Mode, 2, ChirpTimes[k], ChirpSize,
		 1000.0*ChirpWidth, ChirpKurtosis, 100.0*ChirpDip, ChirpPhase,
		 eodrate, beatfreq, beatphase, beatloc, beatbin,
		 beatbefore, beatafter, beatpeak, beattrough );
    Response.push_back( d );
    ChirpData &rd = Response.back();

    // frequency and amplitude:
    double chirptime =  signalTime() + ChirpTimes[k];
    EventFrequencyIterator findex = eod2.begin() + eod2.next( chirptime - SaveWindow );
    EventFrequencyIterator lindex = eod2.begin() + eod2.previous( chirptime + SaveWindow );
    EventSizeIterator sindex = findex;
    int ns = lindex - findex + 1;
    rd.EODTime.reserve( ns );
    rd.EODFreq.reserve( ns );
    rd.EODAmpl.reserve( ns );
    while ( findex <= lindex && ! findex ) {
      double t = findex.time() - signalTime() - ChirpTimes[k];
      rd.EODTime.push( 1000.0*t );
      rd.EODFreq.push( *findex );
      rd.EODAmpl.push( *sindex );
      ++findex;
      ++sindex;
    }

    // spikes:
    for ( int j=0; j<MaxTraces; j++ ) {
      if ( SpikeEvents[j] >= 0 ) {
	EventData spikes;
	events(SpikeEvents[j]).copy( chirptime-SaveWindow,
				     chirptime+2.0*SaveWindow,
				     chirptime, spikes );
	rd.Spikes[j].reserve( spikes.size() );
	for ( int i=0; i<spikes.size(); i++ )
	  rd.Spikes[j].push( spikes[i] );
	
	// spike rate:
	events(SpikeEvents[j]).addRate( SpikeRate[j][beatbin][Mode].Rate,
					SpikeRate[j][beatbin][Mode].Trials,
					kernel, chirptime );
	SpikeRate[j][beatbin][Mode].Trials--;
	events(SpikeEvents[j]).addFrequency( SpikeRate[j][beatbin][Mode].Frequency,
					     SpikeRate[j][beatbin][Mode].Trials,
					     chirptime );
	
	double maxr = max( SpikeRate[j][beatbin][Mode].Rate );
	double maxf = max( SpikeRate[j][beatbin][Mode].Frequency );
	if ( maxf > maxr )
	  maxr = maxf;
	if ( maxr+100.0 > MaxRate[j] ) {
	  MaxRate[j] = ::ceil((maxr+100.0)/20.0)*20.0;
	}
      }
    }

    // nerve:
    if ( NerveTrace[0] >= 0 ) {
      const InData &nd = trace( NerveTrace[0] );
      // nerve amplitudes:
      // peak and trough amplitudes:
      double left = chirptime - SaveWindow;
      double threshold = nd.stdev( left, left+4.0/FishRate );
      if ( threshold < 1.0e-8 )
	threshold = 0.001;
      EventList peaktroughs( 2, (int)rint(1500.0*(Duration>2.0*SaveWindow?Duration:2.0*SaveWindow)), true );
      InData::const_iterator firstn = nd.begin( left );
      InData::const_iterator lastn = nd.begin( left+2.0*SaveWindow );
      if ( lastn > nd.end() )
	lastn = nd.end();
      Detector< InData::const_iterator, InDataTimeIterator > D;
      D.init( firstn, lastn, nd.timeBegin( left ) );
      D.peakTrough( firstn, lastn, peaktroughs, threshold,
		    threshold, threshold, NerveAcceptEOD );
      // store amplitudes:
      rd.NerveAmplP.clear();
      rd.NerveAmplT.clear();
      rd.NerveAmplM.clear();
      rd.NerveAmplS.clear();
      rd.NerveAmplP.reserve( peaktroughs[0].size() + 100 );
      rd.NerveAmplT.reserve( peaktroughs[0].size() + 100 );
      rd.NerveAmplM.reserve( peaktroughs[0].size() + 100 );
      for ( int k=0; k<peaktroughs[0].size() && k<peaktroughs[1].size(); k++ ) {
	rd.NerveAmplP.push( peaktroughs[0][k] - chirptime, 
			    peaktroughs[0].eventSize( k ) );
	rd.NerveAmplT.push( peaktroughs[1][k] - chirptime, 
			    peaktroughs[1].eventSize( k ) );
      }
      // averaged amplitude:
      if ( peaktroughs[0].size() > 1 ) {
	double st = (peaktroughs[0].back() - peaktroughs[0].front())/double(peaktroughs[0].size()-1);
	for ( int k=0; k<rd.NerveAmplP.size(); k++ ) {
	  double t = rd.NerveAmplP.x( k ) + chirptime;
	  rd.NerveAmplM.push( t - chirptime, nd.mean( t-0.5*st, t+0.5*st ) );
	}
	// smoothed averaged amplitude:
	rd.NerveAmplS = SampleDataD( -SaveWindow, SaveWindow, 0.0001 );
	for ( int k=0; k<rd.NerveAmplS.size(); k++ ) {
	  double t = chirptime + rd.NerveAmplS.pos(k);
	  rd.NerveAmplS[k] = nd.mean( t-0.5*st, t+0.5*st );
	}
      }
    }
  }

  // mean nerve:
  if ( NerveTrace[0] >= 0 ) {
    for ( int b=0; b<BeatPos; b++ ) {
      // get nerve amplitudes:
      vector< MapD > nerveamplp;
      vector< MapD > nerveamplt;
      vector< MapD > nerveamplm;
      vector< SampleDataD > nerveampls;
      nerveamplp.reserve( (Repeats>0?Repeats:100)*2*NChirps/BeatPos );
      nerveamplt.reserve( (Repeats>0?Repeats:100)*2*NChirps/BeatPos );
      nerveamplm.reserve( (Repeats>0?Repeats:100)*2*NChirps/BeatPos );
      nerveampls.reserve( (Repeats>0?Repeats:100)*2*NChirps/BeatPos );
      for ( unsigned int j = 0; j < Response.size(); j++ ) {
	if ( Response[j].BeatBin == b ) {
	  nerveamplp.push_back( Response[j].NerveAmplP );
	  nerveamplt.push_back( Response[j].NerveAmplT );
	  nerveamplm.push_back( Response[j].NerveAmplM );
	  nerveampls.push_back( Response[j].NerveAmplS );
	}
      }

      // nerve mean amplitudes:
      average( NerveMeanAmplP[b][Mode], nerveamplp );
      average( NerveMeanAmplT[b][Mode], nerveamplt );
      average( NerveMeanAmplM[b][Mode], nerveamplm );
      average( NerveMeanAmplS[b][Mode], nerveampls );
    }
  }

}


addRePro( Chirps, efish );

}; /* namespace efish */

#include "moc_chirps.cc"
