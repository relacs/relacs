/*
  efield/beats.cc
  Play EOD mimicks with optional chirps from a range of automatically set difference frequencies and amplitudes.

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

#include <relacs/map.h>
#include <relacs/spectrum.h>
#include <relacs/eventdata.h>
#include <relacs/tablekey.h>
#include <relacs/datafile.h>
#include <relacs/rangeloop.h>
#include <relacs/base/linearattenuate.h>
#include <relacs/efield/beats.h>
using namespace relacs;

namespace efield {


Beats::Beats( void )
  : RePro( "Beats", "efield", "Jan Benda", "2.7", "May 5, 2021" )
{
  // add some parameter as options:
  newSection( "Stimulation" );
  addText( "name", "Name of stimuli", "");
  addNumber( "duration", "Signal duration", 10.0, 0.0, 1000000.0, 1.0, "seconds" );
  addNumber( "pause", "Pause between signals", 20.0, 1.0, 1000000.0, 1.0, "seconds" );
  addNumber( "ramp", "Duration of linear ramp", 0.5, 0, 10000.0, 0.1, "seconds" );
  addText( "deltafrange", "Range of delta f's", "10" ).setUnit( "Hz" );
  addSelection( "deltafshuffle", "Order of delta f's", RangeLoop::sequenceStrings() );
  addInteger( "eodmult", "EOD multiples", 1, 0, 100, 1 );
  addBoolean( "fixeddf", "Keep delta f fixed", false );
  addNumber( "amplitude", "Amplitude", 1.0, 0.0, 1000.0, 0.1, "mV/cm" );
  addSelection( "amtype", "Amplitude modulation of signal", "none|sine|rectangular" );
  addText( "amfreq", "Frequencies of amplitude modulation", "1" ).setUnit( "Hz" ).setActivation( "amtype", "none", false );
  addText( "amamplitude", "Corresponding amplitudes", "100" ).setUnit( "%" ).setActivation( "amtype", "none", false );
  addInteger( "repeats", "Repeats", 10, 0, 1000, 2 ).setStyle( OptWidget::SpecialInfinite );
  addNumber( "fakefish", "Assume a fish with frequency", 0.0, 0.0, 2000.0, 10.0, "Hz" );
  newSection( "Chirps" );
  addBoolean( "generatechirps", "Generate chirps", false ).setActivation( "amtype", "none" );
  addNumber( "chirpsize", "Size of chirp", 100.0, 0.0, 1000.0, 10.0, "Hz" ).setActivation( "amtype", "none" ).addActivation( "generatechirps", "true" );
  addNumber( "chirpwidth", "Width of chirp", 0.1, 0.002, 100.0, 0.001, "sec", "ms" ).setActivation( "amtype", "none" ).addActivation( "generatechirps", "true" );
  addNumber( "chirpampl", "Amplitude reduction during chirp", 0.0, 0.0, 1.0, 0.01, "1", "%", "%.0f" ).setActivation( "amtype", "none" ).addActivation( "generatechirps", "true" );
  addNumber( "chirpkurtosis", "Kurtosis of Gaussian chirp", 1.0, 0.01, 100.0, 0.01, "", "" ).setActivation( "amtype", "none" ).addActivation( "generatechirps", "true" );
  addText( "chirpfrequencies", "Chirp frequencies for each delta f", "" ).setUnit( "Hz" ).setActivation( "amtype", "none" ).addActivation( "generatechirps", "true" );
  addText( "chirptimesfile", "File with chirp times", "" ).setStyle( OptWidget::BrowseExisting ).setActivation( "amtype", "none" ).addActivation( "generatechirps", "true" );
  addSelection( "chirptimeshuffle", "Order of chirp-time sequences from file", RangeLoop::sequenceStrings() ).setActivation( "amtype", "none" ).addActivation( "generatechirps", "true" ).addActivation( "chirptimesfile", "", false );
  newSection( "Analysis" );
  addNumber( "before", "Time before stimulation to be analyzed", 1.0, 0.0, 100000.0, 1.0, "seconds" );
  addNumber( "after", "Time after stimulation to be analyzed", 1.0, 0.0, 100000.0, 1.0, "seconds" );
  addNumber( "averagetime", "Time for computing EOD frequency", 1.0, 0.0, 100000.0, 1.0, "seconds" );
  addBoolean( "usepsd", "Use power spectrum to measure EOD frequency", true );
  addNumber( "mineodfreq", "Minimum expected EOD frequency", 100.0, 0.0, 10000.0, 10.0, "Hz" ).setActivation( "usepsd", "true" );
  addNumber( "maxeodfreq", "Maximum expected EOD frequency", 2000.0, 0.0, 10000.0, 10.0, "Hz" ).setActivation( "usepsd", "true" );
  addNumber( "eodfreqprec", "Precision of EOD frequency measurement", 1.0, 0.0, 100.0, 0.1, "Hz" ).setActivation( "usepsd", "true" );
  addInteger( "neod", "Number of attempts for measuring EOD frequency", 2, 1, 100000 );
  addBoolean( "showstimulus", "Plot frequency of stimulus", false );
  addBoolean( "split", "Save each run into a separate file", false );
  addBoolean( "savetraces", "Save traces during pause", false );

  // plot:
  P.lock();
  P.setXLabel( "[sec]" );
  P.setYRange( Plot::AutoScale, Plot::AutoScale );
  P.setYLabel( "EOD [Hz]" );
  P.setLMarg( 6 );
  P.setRMarg( 1 );
  P.setTMarg( 3 );
  P.setBMarg( 4 );
  P.unlock();
  setWidget( &P );

  FileCount = 0;
}


int Beats::main( void )
{
  // get options:
  string name = text( "name" );
  double duration = number( "duration" );
  double pause = number( "pause" );
  double ramp = number( "ramp" );
  double amplitude = number( "amplitude" );
  int amtype = index( "amtype" );
  vector< double > amfreqs;
  vector< double > amampls;
  Str amfs = text( "amfreq" );
  Str amas = text( "amamplitude" );
  amfs.range( amfreqs );
  amas.range( amampls );
  string deltafrange = text( "deltafrange" );
  RangeLoop::Sequence deltafshuffle = RangeLoop::Sequence( index( "deltafshuffle" ) );
  int eodmult = integer( "eodmult" );
  bool fixeddf = boolean( "fixeddf" );
  int repeats = integer( "repeats" );
  bool generatechirps = boolean( "generatechirps" );
  double chirpsize = number( "chirpsize" );
  double chirpwidth = number( "chirpwidth" );
  double chirpampl = number( "chirpampl" );
  double chirpkurtosis = number( "chirpkurtosis" );
  Str cfs = text( "chirpfrequencies" );
  vector< double > chirpfrequencies;
  cfs.range( chirpfrequencies );
  string chirptimesfile = text( "chirptimesfile" );
  RangeLoop::Sequence chirptimeshuffle = RangeLoop::Sequence( index( "chirptimeshuffle" ) );
  double before = number( "before" );
  double after = number( "after" );
  double averagetime = number( "averagetime" );
  bool usepsd = boolean( "usepsd" );
  double mineodfreq = number( "mineodfreq" );
  double maxeodfreq = number( "maxeodfreq" );
  double eodfreqprec = number( "eodfreqprec" );
  int neod = integer( "neod" );
  bool showstimulus = boolean( "showstimulus" );
  bool split = boolean( "split" );
  bool savetraces = boolean( "savetraces" );
  double fakefish = number( "fakefish" );
  if ( before + after > pause ) {
    pause = before + after;
    warning( "Pause is too small. Set it to before + after for now." );
  }
  if ( fakefish > 0.0 ) {
    warning( "Do you really want a fish with frequency " + Str( fakefish )
	     + " Hz to be simulated? Switch this off by setting the fakefish option to zero." );
  }
  if ( FishEODTanks <= 0 ) {
    warning( "need recordings of EOD Traces." );
    return Failed;
  }
  for ( int k=0; k<FishEODTanks; k++ ) {
    for ( int j=0; j<FishEODTraces[k]; j++ ) {
      if ( FishEODEvents[k][j] < 0 ) {
	warning( "need EOD events of each EOD Trace." );
	return Failed;
      }
    }
  }
  if ( ::fabs(amplitude) < 1e-8 ) {
    warning( "amplitude needs to be larger than zero." );
    return Failed;
  }
  int nfft = 0;
  if ( usepsd ) {
    nfft = nextPowerOfTwo( (int)::ceil( 1.0/trace( FishEODTrace[0][0] ).stepsize()/eodfreqprec ) );
    eodfreqprec = 1.0/trace( FishEODTrace[0][0] ).interval(nfft);
    if ( averagetime < 1.0/eodfreqprec ) {
      double newaveragetime = 1.0/eodfreqprec;
      warning( "averagetime of " + Str( averagetime ) + 
	       "s is too small for requested frequency resolution. Set it to " +
	       Str( newaveragetime ) + "s for now." );
      averagetime = newaveragetime;
    }
  }
  if ( averagetime > pause ) {
    warning( "Pause of " + Str( pause ) + "s is smaller than averagetime of " + Str(averagetime ) + "s. Set it to averagetime for now." );
    pause = averagetime;
  }

  double amamplsum = 0.0;
  if ( amtype > 0 ) {
    if ( amfreqs.size() == 0 ) {
      warning( "no AM frequency specified." );
      return Failed;
    }
    if ( amfreqs.size() != amampls.size() ) {
      warning( "You need to specify for each AM frequency a corresponding amplitude." );
      return Failed;
    }
    for ( unsigned int k=0; k<amampls.size(); k++ ) {
      amampls[k] *= 0.01;
      amamplsum += amampls[k];
    }
    if ( amamplsum > 1.0 ) {
      warning( "Sum of all AM amplitudes must be smaller than 100%." );
      return Failed;
    }
    generatechirps = false;
  }

  EventList chirptimes;
  int maxchirptimes = 0;
  if ( generatechirps ) {
    if ( ! chirptimesfile.empty() ) {
      DataFile cf( chirptimesfile );
      int allchirptimes = 0;
      int chirptimesblocks = 0;
      while ( cf.read( 2 ) ) {
	if ( cf.data().rows() > 0 ) {
	  chirptimes.push( cf.col( 0 ) );
	  allchirptimes += chirptimes.back().size();
	  chirptimesblocks++;
	  if ( chirptimes.back().size() > maxchirptimes )
	    maxchirptimes = chirptimes.back().size();
	}
      }
      if ( maxchirptimes <= 0 ) {
	warning( "File " + chirptimesfile + " does not exist or does not contain data.\n" );
	return Failed;
      }
      else
	printlog( "Read in " + Str( allchirptimes ) + " chirp times from " +
		  Str( chirptimesblocks ) + " blocks of data from file " +
		  chirptimesfile + "." );
    }
    else if ( ! chirpfrequencies.empty() ) {
      chirptimes.push( EventData() );
      double maxt = duration * max( chirpfrequencies );
      double t = 1.0;
      do {
	chirptimes.back().push( t );
	t += 1.0;
      } while ( t < maxt );
      if ( chirptimes.back().size() > maxchirptimes )
	maxchirptimes = chirptimes.back().size();
      printlog( "Generated " + Str( chirptimes.back().size() ) + " evenly spaced chirp times." );
    }
    generatechirps = ( ( !chirptimes.empty() && ! chirptimes[0].empty() ) ||
		       ! chirpfrequencies.empty() );
  }

  // check gain of attenuator:
  base::LinearAttenuate *latt =
    dynamic_cast<base::LinearAttenuate*>( attenuator( outTraceName( GlobalEField ) ) );
  if ( fakefish == 0.0 && latt != 0 && fabs( latt->gain() - 1.0 ) < 1.0e-8 )
    warning( "Attenuator gain is probably not set!<br>(it is set to 1.0)", 2.0 );

  // reset outputs:
  if ( fixeddf )
    writeZero( GlobalEField );
  else
    writeZero( "Amplitude" );

  // plot trace:
  tracePlotContinuous();

  // plot:
  P.lock();
  P.clear();
  P.setXRange( -before, duration+after );
  P.plotVLine( 0.0 );
  P.plotVLine( duration );
  P.draw();
  P.unlock();

  // results:
  MapD eodfrequency;
  eodfrequency.reserve( (int)::ceil( 1000.0*(before+duration+after) ) );
  MapD eodfrequencies[ FishEODTraces[0] ];
  MapD eodamplitudes[ FishEODTraces[0] ];
  EventData fishchirps;
  fishchirps.reserve( (int)::rint( 100*duration ) );
  EventData currentchirptimes;
  currentchirptimes.reserve( maxchirptimes );
  EventData playedchirptimes;
  playedchirptimes.reserve( maxchirptimes );
  for ( int k=0; k<FishEODTraces[0]; k++ ) {
    eodfrequencies[k].reserve( (int)::ceil( 1000.0*(before+duration+after) ) );
    eodamplitudes[k].reserve( (int)::ceil( 1000.0*(before+duration+after) ) );
  }
  MapD stimfrequency;
  stimfrequency.reserve( (int)::ceil( 1000.0*(before+duration+after) ) );

  RangeLoop chirptimesrange( 0.0, 1.0, chirptimes.size() );
  chirptimesrange.setSequence( chirptimeshuffle );
  chirptimesrange.reset();
  RangeLoop dfrange( deltafrange );
  if ( chirpfrequencies.size() > 1 && (int)chirpfrequencies.size() != dfrange.size() ) {
    warning( "The number of chirp frequencies must match the number of delta f's!" );
    return Failed;
  }
  dfrange.setSequence( deltafshuffle );
  
  for ( int count = 0;
	(repeats <= 0 || count < repeats ) && softStop() == 0;
	count++ ) {
    for ( dfrange.reset(); !dfrange && softStop() < 2; ++dfrange ) {

      // results:
      eodfrequency.clear();
      fishchirps.clear();
      currentchirptimes.clear();
      playedchirptimes.clear();
      stimfrequency.clear();
      int eodinx[ FishEODTraces[0] ];
      EventIterator eoditer[FishEODTraces[0]];
      bool initeoditer[FishEODTraces[0]];
      for ( int k=0; k<FishEODTraces[0]; k++ ) {
	eodfrequencies[k].clear();
	eodamplitudes[k].clear();
	eodinx[k] = 0;
	initeoditer[k] = true;
      }
      EventFrequencyIterator stimiter;
      bool initstimiter = true;

      // eod frequency:
      double fishrate = 0.0;
      if ( fakefish > 0.0 )
	fishrate = fakefish;
      for ( int j=0; j<neod && fishrate < 0.1; j++ ) {
	if ( usepsd ) {
	  // take EOD from largest peak:
	  double bigeod = 0.0;
	  double bigeodf = 0.0;
	  if ( currentTime() < averagetime )
	    sleep( averagetime );
	  for ( int k=0; k<FishEODTraces[0]; k++ ) {
	    SampleDataF data( 0.0, averagetime, trace( FishEODTrace[0][k] ).stepsize() );
	    trace( FishEODTrace[0][k] ).copy( currentTime()-averagetime, data );
	    SampleDataF power( nfft );
	    rPSD( data, power );
	    double threshold = power.max( mineodfreq, maxeodfreq );
	    EventData powerpeaks( 1000, true );
	    peaks( power, powerpeaks, 0.1*threshold );
	    double maxpower = 0.0;
	    double maxfreq = 0.0;
	    for ( int i=0; i<powerpeaks.size(); i++ ) {
	      if ( powerpeaks[i] >= mineodfreq && powerpeaks[i] <= maxeodfreq ) {
		if ( powerpeaks.eventSize( i ) > maxpower ) {
		  maxpower = powerpeaks.eventSize( i );
		  maxfreq = powerpeaks[i];
		}
	      }
	    }
	    if ( bigeod < maxpower ) {
	      bigeod = maxpower;
	      bigeodf = maxfreq;
	    }
	  }
	  fishrate = bigeodf;
	}
	else {
	  // find EOD with largest amplitude:
	  int bigeodinx = 0;
	  double bigeod = 0.0;
	  for ( int k=0; k<FishEODTraces[0]; k++ ) {
	    double a = events( FishEODEvents[0][k] ).meanSize( currentTime()-averagetime, currentTime() );
	    if ( bigeod < a ) {
	      bigeod = a;
	      bigeodinx = k;
	    }
	  }
	  fishrate = events( FishEODEvents[0][bigeodinx] ).frequency( currentTime()-averagetime, currentTime() );
	}
	if ( fishrate < 0.1 )
	  sleep( averagetime );
      }
      printlog( "EOD Frequency of fish is " + Str( fishrate, 0, 1, 'f' ) + "Hz" );
      if ( fishrate < 0.1 ) {
	warning( "No fish EOD detected!" );
	return Failed;
      }

      double deltaf = *dfrange;

      setSaving( true );

      // plot:
      initPlot( deltaf, amplitude, duration, fishrate, eodfrequency, fishchirps,
		showstimulus, stimfrequency, playedchirptimes );

      // create signal:
      double starttime = currentTime();
      double stimulusrate = eodmult*fishrate + deltaf;
      double ramptime = 0.0;
      Options chirpheader;
      double chirpfrequency = 0.0;
      int chirpsequence = -1;
      OutList signal;

      if ( fixeddf ) {
	chirpheader.clear();
	OutData sig;
        sig.setTraceName( "Frequency" );
	sig.constWave( ramp, -1.0, stimulusrate );
	signal.push( sig );

	sig.setTraceName( "Amplitude" );
	sig.rampWave( ramp, -1.0, 0.0, 1.0 );
	signal.push( sig );

	sig.setTrace( FishEField[0] );
	sig.constWave( ramp, -1.0, 0.0 );
	sig.setIntensity( amplitude );

        sig.description().addNumber( "DeltaF", deltaf, "Hz" );
        sig.description().addNumber( "Amplitude", amplitude, "mV" );
        sig.description().addNumber( "Duration", duration, "s" );
        if ( settings().flags( "deltafrange", OutData::Mutable ) )
          sig.description()["DeltaF"].addFlags( OutData::Mutable );
        if ( settings().flags( "Amplitude", OutData::Mutable ) )
          sig.description()["Amplitude"].addFlags( OutData::Mutable );
        sig.setIdent( name );

        signal.push( sig );
        signal.setDelay( before );

	// output signal:
	starttime = currentTime();
	startWrite( signal );

	// signal failed?
	if ( signal.failed() ) {
	  string s = "Output of stimulus failed!<br>Error code is <b>";
	  s += signal.errorText() + "</b>";
	  warning( s, 2.0 );
	  writeZero( "Amplitude" );
	  P.lock();
	  P.clear();
	  P.unlock();
	  return Failed;
	}
	ramptime = ramp;
	sleep( before + ramptime );
      }
      else { // no fixeddf
	OutData sig;
        sig.setTrace( FishEField[0] );
	OutData led;
	if ( LEDOutput[0] >= 0 )
	  led.setTrace( LEDOutput[0] );
	if ( generatechirps ) {
	  // EOD with chirps:
	  chirpfrequency = 1.0;
	  if ( chirpfrequencies.size() == 1 )
	    chirpfrequency = chirpfrequencies[0];
	  else if ( chirpfrequencies.size() > 1 )
	    chirpfrequency = chirpfrequencies[dfrange.pos()];
	  if ( chirpfrequency < -1e-8 ) {
	    warning( "Negative chirp frequency!" );
	    P.lock();
	    P.clear();
	    P.unlock();
	    return Failed;
	  }
	  chirpsequence = chirptimesrange.pos();
	  if ( chirpsequence < 0 ) {
	    chirpsequence = 0;
	    printlog( "! ERROR: chirpsequence < 0" );
	  }
	  if ( chirpfrequency > 1e-8 ) {
	    currentchirptimes = chirptimes[chirpsequence];
	    currentchirptimes *= 1.0/chirpfrequency;
	  }
	  else
	    currentchirptimes.clear();
	  if ( ! chirptimesrange )
	    ++chirptimesrange;
	  else
	    chirptimesrange.reset();
	  sig.clear();
	  if ( sig.fixedSampleRate() )
	    sig.setSampleInterval( sig.minSampleInterval() );
	  else
	    sig.setBestSample( 2.0*stimulusrate );
	  sig.resize( sig.indices( duration ) );
	  double csig = 0.5*chirpwidth / ::pow( 2.0*log(10.0), 0.5/chirpkurtosis );
	  double p = 0.0;
	  int ck = 0;
	  for ( int k=0; k<sig.size(); k++ ) {
	    double t = sig.pos( k );
	    double f = stimulusrate;
	    double a = 1.0;
	    if ( t < ramp )
	      a = t/ramp;
	    else if ( t > duration - ramp )
	      a = (duration - t)/ramp;
	    if ( ck<currentchirptimes.size() ) {
	      if ( fabs( t - currentchirptimes[ck] ) < 2.0*chirpwidth ) {
		double x = t - currentchirptimes[ck];
		double g = exp( -0.5 * ::pow( (x/csig)*(x/csig), chirpkurtosis ) );
		f = chirpsize*g + stimulusrate;
		a *= 1.0 - chirpampl*g;
	      }
	      else if ( t > currentchirptimes[ck] + 2.0*chirpwidth )
		ck++;
	    }
	    p += f * sig.stepsize();
	    sig[k] = a * ::sin( 6.28318530717959 * p );
	  }
	  sig.back() = 0.0;
	  sig.setIntensity( amplitude );
	  currentchirptimes.resize( ck );
	  chirpheader.clear();
	  chirpheader.addNumber( "ChirpSize", chirpsize, "Hz" );
	  chirpheader.addNumber( "ChirpWidth", 1000.0*chirpwidth, "ms" );
	  chirpheader.addNumber( "ChirpAmplitude", 100.0*(1.0-chirpampl), "%" );
	  chirpheader.addNumber( "ChirpKurtosis", chirpkurtosis );
	  Parameter & cfp = chirpheader.addNumber( "ChirpFrequency", chirpfrequency, "Hz" );
          if ( chirpfrequencies.size() > 1 )
            cfp.addFlags( OutData::Mutable );
	  if ( ! chirptimesfile.empty() && ! chirptimes.empty() ) {
	    chirpheader.addText( "ChirpTimesFile", chirptimesfile );
	    chirpheader.addInteger( "ChirpSequence", chirpsequence );
	  }
	  chirpheader.addInteger( "ChirpNumber", ck );
	  if ( ck > 0 ) {
	    chirpheader.addNumber( "ChirpTimes", currentchirptimes[0], "s" );
	    for ( int j=1; j<ck; j++ )
	      chirpheader.pushNumber( "ChirpTimes", currentchirptimes[j] );
	  }
	  sig.description().setType( "stimulus/eod_chirps" );
	  Parameter &p1 = sig.description().addNumber( "Frequency", stimulusrate, "Hz" );
	  Parameter &p2 = sig.description().addNumber( "DeltaF", deltaf, "Hz" );
	  sig.description().addNumber( "Amplitude", amplitude, "mV" );
	  sig.description().addNumber( "TemporalOffset", 0.0, "s" );
	  sig.description().addNumber( "Duration", duration, "s" );
	  sig.description().append( chirpheader );
	  if ( dfrange.size() > 1 ) {
	    sig.setMutable( p1 );
	    sig.setMutable( p2 );
	  }
	  if ( LEDOutput[0] >= 0 ) {
	    double von = 5.0;
	    double vchirp = -5.0;
	    double minledduration = 0.001;
	    led.pulseWave( sig.length(), sig.stepsize(), von, 0.0 );
	    led.resize( sig.size() );
	    int w = led.indices( chirpwidth>minledduration ? chirpwidth : minledduration );
	    for ( int k=0; k<currentchirptimes.size() && currentchirptimes[k] < sig.length(); k++ ) {
	      int s = led.index( currentchirptimes[k] - 0.5*chirpwidth );
	      for ( int j=s; j<s+w && j < led.size(); j++ )
		led[j] = vchirp;
	    }
	  }
	}
	else { // not fixed, no chirps
	  chirpheader.clear();
	  double p = 1.0;
	  if ( fabs( deltaf ) > 0.01 )
	    p = 1.0 / fabs( deltaf );
	  else
	    p = 1.0/fishrate;
	  int n = (int)::rint( duration / p );
	  if ( n < 1 )
	    n = 1;
	  if ( amtype == 0 ) {
	    sig.sineWave( n*p, -1.0, stimulusrate, 0.0, 1.0, ramp );
	    sig.setIdent( "sinewave" );
	    sig.setIntensity( amplitude );
	    sig.description().setNumber( "Amplitude", amplitude );
	    sig.description().setUnit( "Amplitude", "mV" );
	    if ( dfrange.size() > 1 )
	      sig.setMutable( "Frequency" );
	  }
	  else {
	    OutData am;
	    am.setTrace( FishEField[0] );
	    am.setUnit( "" );
	    if ( amtype == 2 ) {
	      am.rectangleWave( n*p, -1.0, 1.0/amfreqs[0], 0.5/amfreqs[0], 0.0, 2.0*amampls[0] );
	      am -= amampls[0];
	      for ( unsigned int k=1; k<amfreqs.size(); k++ ) {
		OutData amk;
		amk.setTrace( FishEField[0] );
		amk.setUnit( "" );
		amk.rectangleWave( n*p, -1.0, 1.0/amfreqs[k], 0.5/amfreqs[k], 0.0, 2.0*amampls[k] );
		amk -= amampls[k];
		am += amk;
	      }
	    }
	    else {
	      am.sineWave( n*p, -1.0, amfreqs[0], 0.75*2.0*M_PI, amampls[0], 0.0 );
	      for ( unsigned int k=1; k<amfreqs.size(); k++ ) {
		OutData amk;
		amk.setTrace( FishEField[0] );
		amk.setUnit( "" );
		amk.sineWave( n*p, -1.0, amfreqs[k], 0.75*2.0*M_PI, amampls[k], 0.0 );
		am += amk;
	      }
	    }
	    am += 1.0;
	    am.description().clearSections();
	    am.description().addNumber( "Intensity", 1.0, "" );
	    am /= 1.0+amamplsum;
	    sig.fill( am, stimulusrate );
	    if ( dfrange.size() > 1 ) {
	      Options &opt = sig.description().lastSection();
	      sig.setMutable( "Frequency", opt );
	    }
	    sig.description().clearSections();
	    sig.ramp( ramp );
	    if ( amtype == 2 )
	      sig.setIdent( "am-rectangularwave" );
	    else
	      sig.setIdent( "am-sinewave" );
	    sig.setIntensity( amplitude*(1.0+amamplsum) );
	  }
	  sig.description().clearSections();
          if ( !sig.description().exist( "DeltaF" ) )
            sig.description().addNumber( "DeltaF", deltaf, "Hz" );
          else
            sig.description().setNumber( "DeltaF", deltaf );
          if ( settings().flags( "deltafrange", OutData::Mutable ) )
            sig.description()["DeltaF"].addFlags( OutData::Mutable );

          if ( !sig.description().exist( "Amplitude" ) )
            sig.description().addNumber( "Amplitude", amplitude, "mV" );
          else
            sig.description().setNumber( "Amplitude", amplitude );
          if ( settings().flags( "Amplitude", OutData::Mutable ) )
            sig.description()["Amplitude"].addFlags( OutData::Mutable );

          if ( !sig.description().exist( "Duration" ) )
            sig.description().addNumber( "Duration", duration, "s" );
          else
            sig.description().setNumber( "Duration", duration );

          if ( sig.description().exist( "Frequency" ) )
            sig.description()["Frequency"].addFlags( OutData::Mutable );

	  if ( LEDOutput[0] >= 0 )
	    led.pulseWave( sig.length(), sig.stepsize(), 5.0, 0.0 );
	}
	duration = sig.length();

        sig.setDelay( before );
        sig.setIdent( name );
        signal.push( sig );
	if ( LEDOutput[0] >= 0 ) {
	  led.setDelay( before );
	  signal.push( led );
	}

	// output signal:
	starttime = currentTime();
	startWrite( signal );

	// signal failed?
	if ( signal.failed() ) {
	  string s = "Output of stimulus failed!<br>Error code is <b>";
	  s += signal.errorText() + "</b>";
	  warning( s, 2.0 );
	  writeZero( FishEField[0] );
	  if ( LEDOutput[0] >= 0 )
	    writeZero( LEDOutput[0] );
	  P.lock();
	  P.clear();
	  P.unlock();
	  return Failed;
	}
	sleep( 0.2 );
      }
      if ( interrupt() ) {
	if ( fixeddf )
	  writeZero( "Amplitude" );
	else
	  writeZero( FishEField[0] );
	if ( LEDOutput[0] >= 0 )
	  writeZero( LEDOutput[0] );
	P.lock();
	P.clear();
	P.unlock();
	return Aborted;
      }
      double signaltime = signalTime();

      // meassage:
      Str s = "Delta f: <b>" + Str( deltaf, "%g" ) + "Hz</b>";
      if ( eodmult != 1 ) {
	s += "  @ EOD multiple: <b>" + Str( eodmult, "%d" ) + "</b>";
	s += "  (=<b>" + Str( stimulusrate, "%.1f" ) + "Hz)</b>";
      }
      s += "  Amplitude: <b>" + Str( amplitude, "%g" ) + "mV/cm</b>";
      if ( amtype > 0 ) {
	s += "  <b>";
	if ( amtype == 2 )
	  s += "Rectangular";
	else
	  s += "Sine";
	s += "</b> AM";
	s += "  f:  <b>" + Str( amfreqs[0], "%g" );
	for ( unsigned int k=1; k<amfreqs.size(); k++ )
	  s += ", " + Str( amfreqs[k], "%g" );
	s += "Hz</b>";
	s += "  A: <b>" + Str( 100.0*amamplsum, "%g" ) + "%</b>";
      }
      if ( generatechirps ) {
	s += "  Chirps: <b>" + Str( chirpsize, "%g" ) + "Hz @ " + Str( chirpfrequency, "%.2f" ) + "Hz</b>";
	if ( chirptimes.size() > 1 && chirpsequence >= 0 )
	  s += " from <b>sequence " + Str( chirpsequence ) + "</b>";
      }
      if ( repeats != 1 ) {
	s += "  Loop:  <b>" + Str( count+1 ) + "</b>";
	if ( repeats > 0 )
	  s += " from  <b>" + Str( repeats ) + "</b>";
      }
      if ( repeats > 0 ) {
	int rc = dfrange.remainingCount();
	rc += dfrange.maxCount()*(repeats-count-1);
	int rs = ::round((duration + pause)*rc);
	double rm = floor( rs/60.0 );
	rs -= rm*60.0;
	double rh = floor( rm/60.0 );
	rm -= rh*60.0;
	string rt = "";
	if ( rh > 0.0 )
	  rt += Str( rh, 0, 0, 'f' ) + "h";
	rt += Str( rm, 2, 0, 'f', '0' ) + "min";
	rt += Str( rs, 2, 0, 'f', '0' ) + "sec";
	s += "  Remaining time:  <b>" + rt + "</b>";
      }
      message( s );

      // stimulation loop:
      do {
	analyze( signaltime, before, fishrate, initeoditer, eoditer, eodfrequencies, eodamplitudes,
		 eodinx, eodfrequency, initstimiter, stimiter, stimfrequency );
	if ( generatechirps )
	  playedchirptimes.assign( currentchirptimes, 0.0, currentTime()-signalTime() );
	else
	  playedchirptimes.clear();
	P.draw();

	if ( fixeddf ) {
	  double fishrate = events( EODEvents ).frequency( currentTime()-averagetime, currentTime() );
	  if ( fakefish > 0.0 )
	    fishrate = fakefish;
	  OutData sig;
	  sig.setTraceName( "Frequency" );
	  sig.constWave( fishrate + deltaf );
	  directWrite( sig, false );
	  // signal failed?
	  if ( sig.failed() ) {
	    string s = "Output of frequency stimulus failed!<br>Error code is <b>";
	    s += sig.errorText() + "</b>";
	    warning( s, 2.0 );
	    writeZero( "Amplitude" );
	    if ( LEDOutput[0] >= 0 )
	      writeZero( LEDOutput[0] );
	    P.lock();
	    P.clear();
	    P.unlock();
	    return Failed;
	  }
	}

	sleep( 0.2 );
	if ( interrupt() ) {
	  if ( fixeddf )
	    writeZero( "Amplitude" );
	  else
	    writeZero( FishEField[0] );
	  if ( LEDOutput[0] >= 0 )
	    writeZero( LEDOutput[0] );
	  P.lock();
	  P.clear();
	  P.unlock();
	  return Aborted;
	}

      } while ( currentTime() - starttime < before + duration -  ramptime );

      // ending stimulus:
      if ( generatechirps )
	playedchirptimes.assign( currentchirptimes );
      else
	playedchirptimes.clear();
      if ( fixeddf && ramptime > 0.0 ) {
	OutData sig;
	sig.setTraceName( "Amplitude" );
	sig.rampWave( ramp, -1.0, 1.0, 0.0 );
	startWrite( sig );
	// signal failed?
	if ( sig.failed() ) {
	  string s = "Output of final ramp stimulus failed!<br>Error code is <b>";
	  s += sig.errorText() + "</b>";
	  warning( s, 2.0 );
	  writeZero( "Amplitude" );
	  if ( LEDOutput[0] >= 0 )
	    writeZero( LEDOutput[0] );
	  P.lock();
	  P.clear();
	  P.unlock();
	  return Failed;
	}
      }

      // after stimulus recording loop:
      do {
	analyze( signaltime, before, fishrate, initeoditer, eoditer, eodfrequencies, eodamplitudes,
		 eodinx, eodfrequency, initstimiter, stimiter, stimfrequency );
	P.draw();

	sleep( 0.2 );
	if ( interrupt() ) {
	  if ( fixeddf )
	    writeZero( "Amplitude" );
	  else
	    writeZero( FishEField[0] );
	  if ( LEDOutput[0] >= 0 )
	    writeZero( LEDOutput[0] );
	  P.lock();
	  P.clear();
	  P.unlock();
	  return Aborted;
	}

      } while ( currentTime() - starttime < before + duration + after + 0.2 );

      setSaving( savetraces );

      // analyze:
      // chirps:
      if ( FishChirpEvents[0][0] >= 0 )
	fishchirps.assign( events( FishChirpEvents[0][0] ),
			   signaltime - before,
			   signaltime + duration + after, signaltime );
      else
	fishchirps.clear();
      P.draw();
      save( deltaf, eodmult, amplitude, duration, pause, amtype, amampls, amfreqs,
	    fishrate, stimulusrate, nfft, eodfreqprec,
	    eodfrequencies, eodamplitudes, eodfrequency, fishchirps, playedchirptimes,
	    stimfrequency, chirpheader, split, FileCount );
      FileCount++;

      if ( softStop() > 1 )
	break;

      // pause:
      sleep( pause - after - before );
      if ( interrupt() )
	break;

    }
  }
  
  if ( fixeddf )
    writeZero( "Amplitude" );
  else
    writeZero( FishEField[0] );
  if ( LEDOutput[0] >= 0 )
    writeZero( LEDOutput[0] );
  P.lock();
  P.clear();
  P.unlock();
  return Completed;
}


void Beats::sessionStarted( void )
{
  FileCount = 0;
  RePro::sessionStarted();
}


void Beats::analyze( double signaltime, double before, double fishrate,
		     bool initeoditer[], EventIterator eoditer[], 
		     MapD eodfrequencies[], MapD eodamplitudes[],
		     int eodinx[], MapD &eodfrequency,
		     bool &initstimiter, EventFrequencyIterator &stimiter, MapD &stimfrequency )
{
  // get data:
  for ( int k=0; k<FishEODTraces[0]; k++ ) {
    const EventData &eodglobal = events( FishEODEvents[0][k] );
    if ( initeoditer[k] ) {
      eoditer[k] = eodglobal.begin( signaltime - before );
      for ( int j=0; eoditer[k] < eodglobal.end() && j<10; ++eoditer[k], ++j );
      if ( eoditer[k] != eodglobal.end() )
	initeoditer[k] =  false;
    }
    for ( ; eoditer[k] < eodglobal.end(); ++eoditer[k] ) {
      EventFrequencyIterator fiter = eoditer[k];
      eodfrequencies[k].push( fiter.time() - signaltime, *fiter );
      EventSizeIterator siter = eoditer[k];
      eodamplitudes[k].push( siter.time() - signaltime, *siter );
    }
  }
  if ( FishEODTraces[0] == 1 ) {
    while ( eodinx[0] < eodamplitudes[0].size() ) {
      eodfrequency.push( eodfrequencies[0].x( eodinx[0] ),
			 eodfrequencies[0].y( eodinx[0] ) );
      eodinx[0]++;
    }
  }
  else {
    // merge EOD frequencies:
    for ( ; ; ) {
      // find trace with minimum EOD time:
      double mint = currentTime();
      int n = 0;
      for ( int k=0; k<FishEODTraces[0]; k++ ) {
	if ( eodinx[k] < eodamplitudes[k].size() ) {
	  if ( mint > eodamplitudes[k].x( eodinx[k] ) )
	    mint = eodamplitudes[k].x( eodinx[k] );
	  n++;
	}
      }
      if ( n==0 )
	break;
      // find trace with largest EOD amplitude:
      double maxa = 0.0;
      int maxk = 0;
      int maxi = 0;
      for ( int k=0; k<FishEODTraces[0]; k++ ) {
	double t = eodamplitudes[k].x( eodinx[k] );
	if ( fabs( t - mint ) < 0.5/fishrate ) {
	  if ( maxa < eodamplitudes[k].y( eodinx[k] ) ) {
	    maxa = eodamplitudes[k].y( eodinx[k] );
	    maxk = k;
	    maxi = eodinx[k];
	  }
	  eodinx[k]++;
	}
      }
      eodfrequency.push( eodfrequencies[maxk].x( maxi ),
			 eodfrequencies[maxk].y( maxi ) );
    }
  }
  if ( FishEFieldEvents[0] >= 0 ) {
    const EventData &stimglobal = events( FishEFieldEvents[0] );
    if ( initstimiter ) {
      stimiter = stimglobal.begin( signaltime - before );
      int k = 0;
      for ( ; stimiter < stimglobal.end() && k<10; ++stimiter, ++k );
      if ( stimiter != stimglobal.end() )
	initstimiter =  false;
    }
    for ( ; stimiter < stimglobal.end(); ++stimiter )
      stimfrequency.push( stimiter.time() - signaltime, *stimiter );
  }
}


void Beats::initPlot( double deltaf, double amplitude, double duration, double eodf,
		      const MapD &eodfrequency, const EventData &fishchirps, 
		      bool showstimulus, const MapD &stimfrequency, const EventData &chirptimes )
{
  P.lock();
  P.keepPointer();
  P.setDataMutex( mutex() );
  // eod frequency with chirp events:
  P.clear();
  Str s;
  s = "Delta f = " + Str( deltaf, 0, 0, 'f' ) + "Hz";
  s += ", Amplitude = " + Str( amplitude ) + "mV/cm";
  P.setTitle( s );
  P.plotVLine( 0.0 );
  P.plotVLine( duration );
  P.setYRange( Plot::AutoMinScale, Plot::AutoMinScale );
  P.setYFallBackRange( eodf-10.0, eodf+10.0  );
  if ( showstimulus )
    P.plot( stimfrequency, 1.0, Plot::Cyan, 2, Plot::Solid );
  P.plot( eodfrequency, 1.0, Plot::Green, 2, Plot::Solid );
  P.plot( chirptimes, 2, 0.0, 1.0, 0.9, Plot::Graph,
	  1, Plot::Circle, 5, Plot::Pixel, Plot::Blue, Plot::Blue );
  P.plot( fishchirps, 2, 0.0, 1.0, 0.9, Plot::Graph,
	  1, Plot::Circle, 5, Plot::Pixel, Plot::Yellow, Plot::Yellow );
  P.draw();
  P.unlock();
}


void Beats::save( double deltaf, int eodmult, double amplitude, double duration, double pause,
		  int amtype, const vector<double> &amampls, const vector<double> &amfreqs,
		  double fishrate, double stimulusrate, int nfft, double eodfreqprec,
		  const MapD eodfrequencies[], const MapD eodamplitudes[], const MapD &eodfrequency, 
		  const EventData &fishchirps, const EventData &playedchirpevents,
		  const MapD &stimfrequency, const Options &chirpheader,
		  bool split, int count )
{
  Options header;
  header.addNumber( "EODf", fishrate, "Hz", "%.1f" );
  header.addNumber( "Delta f", deltaf, "Hz", "%.1f" );
  header.addNumber( "StimulusFrequency", stimulusrate, "Hz", "%.1f" );
  header.addInteger( "EODMultiple", eodmult );
  header.addNumber( "Amplitude", amplitude, "mV/cm", "%.3f" );
  if ( amtype > 0 ) {
    header.addText( "AMType", amtype == 2? "Rectangular" : "Sine" );
    Str fs = Str( amfreqs[0], "%g" );
    Str as = Str( 100.0*amampls[0], "%.0f" );
    for ( unsigned int k=1; k<amfreqs.size(); k++ ) {
      fs += ", " + Str( amfreqs[k], "%g" );
      as += ", " + Str( 100.0*amampls[k], "%0.1f" );
    }
    header.addText( "AMFrequency", fs + "Hz" );
    header.addText( "AMAmplitude", as + "%" );
  }
  header.addNumber( "Duration", duration, "sec", "%.3f" );
  header.addNumber( "Pause", pause, "sec", "%.3f" );
  if ( nfft > 0 ) {
    header.addInteger( "NFFT", nfft );
    header.addNumber( "FrequencyResolution", eodfreqprec, "Hz", "%.2f" );
  }
  header.addInteger( "Electrode", 0 );
  header.append( chirpheader );
  header.addText( "RePro Time", reproTimeStr() );
  header.addText( "Session Time", sessionTimeStr() );
  header.newSection( settings(), 1 );

  setWaitMouseCursor();
  for ( int k=0; k<FishEODTraces[0]; k++ ) {
    header.setInteger( "Electrode", k+1 );
    string es = "";
    if ( FishEODTraces[0] > 1 )
      es = Str( k+1 );
    saveEODFreq( k, es, header, eodfrequencies[k], eodamplitudes[k], split, count );
  }
  if ( FishEODTraces[0] > 1 ) {
    header.setInteger( "Electrode", 0 );
    saveMergedEODFreq( header, eodfrequency, split, count );
  }
  saveChirps( header, fishchirps, split, count );
  if ( ! chirpheader.empty() )
    savePlayedChirps( header, playedchirpevents, split, count );
  restoreMouseCursor();
}


void Beats::saveEODFreq( int trace, const string &es, const Options &header, const MapD &eodfrequency,
			 const MapD &eodamplitude, bool split, int count )
{
  ofstream df( addPath( "beats-eod" + es + ( split ? "-"+Str( count+1, 2, '0' ) : "" ) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  header.save( df, "# " );
  df << '\n';

  // write key:
  const EventData &eodglobal = events( FishEODEvents[0][trace] );
  TableKey key;
  key.addNumber( "time", "s", "%11.7f" );
  key.addNumber( "freq", "Hz", "%6.2f" );
  key.addNumber( "ampl", eodglobal.sizeUnit(), eodglobal.sizeFormat() );
  key.saveKey( df );

  // write data into file:
  if ( eodfrequency.size() <= 0 ) {
    key.save( df, "-0", 0 );
    key.save( df, "-0" );
    key.save( df, "-0" );
    df << '\n';
  }
  else {
    for ( int k=0; k<eodfrequency.size(); k++ ) {
      key.save( df, eodfrequency.x(k), 0 );
      key.save( df, eodfrequency.y(k) );
      key.save( df, eodglobal.sizeScale() * eodamplitude.y(k) );
      df << '\n';
    }
  }
  df << "\n\n";
}


void Beats::saveMergedEODFreq( const Options &header, const MapD &eodfrequency, bool split, int count )
{
  ofstream df( addPath( "beats-mergedeod" + ( split ? "-"+Str( count+1, 2, '0' ) : "" ) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  header.save( df, "# " );
  df << '\n';

  // write key:
  TableKey key;
  key.addNumber( "time", "s", "%11.7f" );
  key.addNumber( "freq", "Hz", "%6.2f" );
  key.saveKey( df );

  // write data into file:
  if ( eodfrequency.size() <= 0 )
    df << "  -0\n";
  else {
    for ( int k=0; k<eodfrequency.size(); k++ ) {
      key.save( df, eodfrequency.x(k), 0 );
      key.save( df, eodfrequency.y(k) );
      df << '\n';
    }
  }
  df << "\n\n";
}


void Beats::saveChirps( const Options &header, const EventData &chirps,
			bool split, int count )
{
  if ( ChirpEvents < 0 )
    return;

  ofstream df( addPath( "beats-chirps" + ( split ? "-"+Str( count+1, 2, '0' ) : "" ) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  header.save( df, "# " );
  df << '\n';

  // write key:
  TableKey key;
  key.addNumber( "time", "s", "%9.5f" );
  key.addNumber( "freq", chirps.sizeUnit(), chirps.sizeFormat() );
  key.addNumber( "width", chirps.widthUnit(), chirps.widthFormat() );
  key.saveKey( df );

  // write data into file:
  for ( int k=0; k<chirps.size(); k++ ) {
    key.save( df, chirps[k], 0 );
    key.save( df, chirps.sizeScale() * chirps.eventSize( k ) );
    key.save( df, chirps.widthScale() * chirps.eventWidth( k ) );
    df << '\n';
  }
  if ( chirps.size() <= 0 ) {
    key.save( df, "-0", 0 );
    key.save( df, "-0" );
    key.save( df, "-0" );
    df << '\n';
  }
  df << "\n\n";
}


void Beats::savePlayedChirps( const Options &header, const EventData &chirps,
			      bool split, int count )
{
  ofstream df( addPath( "beats-playedchirps" + ( split ? "-"+Str( count+1, 2, '0' ) : "" ) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  header.save( df, "# " );
  df << '\n';

  // write key:
  TableKey key;
  key.addNumber( "time", "s", "%8.3f" );
  key.saveKey( df );

  // write data into file:
  for ( int k=0; k<chirps.size(); k++ ) {
    key.save( df, chirps[k], 0 );
    df << '\n';
  }
  if ( chirps.size() <= 0 ) {
    key.save( df, "-0", 0 );
    df << '\n';
  }
  df << "\n\n";
}


addRePro( Beats, efield );

}; /* namespace efield */

#include "moc_beats.cc"
