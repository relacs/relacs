/*
  efield/beats.cc
  Play EOD mimicks with optional chirps from a range of automatically set difference frequencies and amplitudes.

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

#include <relacs/map.h>
#include <relacs/eventdata.h>
#include <relacs/tablekey.h>
#include <relacs/datafile.h>
#include <relacs/rangeloop.h>
#include <relacs/base/linearattenuate.h>
#include <relacs/efield/beats.h>
using namespace relacs;

namespace efield {


Beats::Beats( void )
  : RePro( "Beats", "efield", "Jan Benda", "2.0", "Sep 19, 2013" )
{
  // add some parameter as options:
  newSection( "Stimulation" );
  addNumber( "duration", "Signal duration", 10.0, 0.0, 1000000.0, 1.0, "seconds" );
  addNumber( "pause", "Pause between signals", 20.0, 1.0, 1000000.0, 1.0, "seconds" );
  addNumber( "ramp", "Duration of linear ramp", 0.5, 0, 10000.0, 0.1, "seconds" );
  addText( "deltafrange", "Range of delta f's", "10" ).setUnit( "Hz" );
  addSelection( "deltafshuffle", "Order of delta f's", RangeLoop::sequenceStrings() );
  addBoolean( "fixeddf", "Keep delta f fixed", false );
  addNumber( "amplitude", "Amplitude", 1.0, 0.1, 1000.0, 0.1, "mV/cm" );
  addInteger( "repeats", "Repeats", 10, 0, 1000, 2 );
  addNumber( "fakefish", "Assume a fish with frequency", 0.0, 0.0, 2000.0, 10.0, "Hz" );
  newSection( "Chirps" );
  addNumber( "chirpsize", "Size of chirp", 100.0, 0.0, 1000.0, 10.0, "Hz" );
  addNumber( "chirpwidth", "Width of chirp", 0.1, 0.002, 100.0, 0.001, "sec", "ms" );
  addNumber( "chirpampl", "Amplitude reduction during chirp", 0.0, 0.0, 1.0, 0.01, "1", "%", "%.0f" );
  addNumber( "chirpkurtosis", "Kurtosis of Gaussian chirp", 1.0, 0.01, 100.0, 0.01, "", "" );
  addText( "chirpfrequencies", "Chirp frequencies for each delta f", "" ).setUnit( "Hz" );
  addText( "chirptimesfile", "File with chirp times", "" ).setStyle( OptWidget::BrowseExisting );
  newSection( "Analysis" );
  addNumber( "before", "Time before stimulation to be analyzed", 1.0, 0.0, 100000.0, 1.0, "seconds" );
  addNumber( "after", "Time after stimulation to be analyzed", 1.0, 0.0, 100000.0, 1.0, "seconds" );
  addNumber( "averagetime", "Time for computing EOD frequency", 1.0, 0.0, 100000.0, 1.0, "seconds" );
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
  double duration = number( "duration" );
  double pause = number( "pause" );
  double ramp = number( "ramp" );
  double amplitude = number( "amplitude" );
  string deltafrange = text( "deltafrange" );
  RangeLoop::Sequence deltafshuffle = RangeLoop::Sequence( index( "deltafshuffle" ) );
  bool fixeddf = boolean( "fixeddf" );
  int repeats = integer( "repeats" );
  double chirpsize = number( "chirpsize" );
  double chirpwidth = number( "chirpwidth" );
  double chirpampl = number( "chirpampl" );
  double chirpkurtosis = number( "chirpkurtosis" );
  Str cfs = text( "chirpfrequencies" );
  vector< double > chirpfrequencies;
  cfs.range( chirpfrequencies );
  string chirptimesfile = text( "chirptimesfile" );
  double before = number( "before" );
  double after = number( "after" );
  double averagetime = number( "averagetime" );
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
  if ( EODTrace < 0 ) {
    warning( "need a recording of the EOD Trace." );
    return Failed;
  }
  if ( EODEvents < 0 ) {
    warning( "need EOD events of the EOD Trace." );
    return Failed;
  }
  EventData chirptimes;
  if ( ! chirptimesfile.empty() ) {
    DataFile cf( chirptimesfile );
    cf.read( 2 );
    if ( cf.data().rows() <= 0 ) {
      warning( "File " + chirptimesfile + " does not exist or doesnot contain data.\n" );
      return Failed;
    }
    chirptimes = cf.col( 0 );
  }
  bool generatechirps = ( ! chirptimes.empty() || ! chirpfrequencies.empty() );

  // check gain of attenuator:
  base::LinearAttenuate *latt =
    dynamic_cast<base::LinearAttenuate*>( attenuator( outTraceName( GlobalEField ) ) );
  if ( fakefish == 0.0 && latt != 0 && fabs( latt->gain() - 1.0 ) < 1.0e-8 )
    warning( "Attenuator gain is probably not set!", 10.0 );

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

  RangeLoop dfrange( deltafrange );
  if ( chirpfrequencies.size() > 1 && (int)chirpfrequencies.size() != dfrange.size() ) {
    warning( "The number of chirp frequencies must match the number of delta f's!" );
    return Failed;
  }
  dfrange.setSequence( deltafshuffle );
  for ( int count = 0;
	(repeats <= 0 || count < repeats ) && softStop() == 0;
	count++ ) {
    for ( dfrange.reset(); !dfrange && softStop() <= 2; ++dfrange ) {

      // results:
      MapD eodfrequency;
      eodfrequency.reserve( (int)::ceil( 1000.0*(before+duration+after) ) );
      MapD stimfrequency;
      stimfrequency.reserve( (int)::ceil( 1000.0*(before+duration+after) ) );
      MapD eodamplitude;
      eodamplitude.reserve( (int)::ceil( 1000.0*(before+duration+after) ) );
      EventData jarchirpevents;
      jarchirpevents.reserve( 1000 );
      EventIterator eoditer;
      bool initeoditer = true;
      EventFrequencyIterator stimiter;
      bool initstimiter = true;

      // eodf:
      double deltaf = *dfrange;
      double fishrate = events( EODEvents ).frequency( currentTime()-averagetime, currentTime() );
      if ( fakefish > 0.0 )
	fishrate = fakefish;

      setSaving( true );

      // meassage:
      Str s = "Delta F:  <b>" + Str( deltaf, 0, 1, 'f' ) + "Hz</b>";
      s += "  Amplitude: <b>" + Str( amplitude, "%g" ) + "mV/cm</b>";
      message( s );

      // create signal:
      double starttime = currentTime();
      double stimulusrate = fishrate + deltaf;
      double ramptime = 0.0;
      if ( fixeddf ) {
	OutList signal;
	OutData sig;
	sig.setTraceName( "Frequency" );
	sig.constWave( ramp, -1.0, stimulusrate );
	signal.push( sig );

	sig.setTraceName( "Amplitude" );
	sig.rampWave( ramp, -1.0, 0.0, 1.0 );
	signal.push( sig );

	sig.setTrace( GlobalEField );
	sig.constWave( ramp, -1.0, 0.0 );
	sig.setIntensity( amplitude );
	signal.push( sig );

	signal.setDelay( before );

	// output signal:
	starttime = currentTime();
	write( signal );
	
	// signal failed?
	if ( signal.failed() ) {
	  string s = "Output of stimulus failed!<br>Error code is <b>";
	  s += signal.errorText() + "</b>";
	  warning( s, 2.0 );
	  writeZero( "Amplitude" );
	  return Failed;
	}
	ramptime = ramp;
	sleep( before + ramptime );
      }
      else {
	OutData signal;
	signal.setTrace( GlobalEField );
	unlockAll();
	if ( generatechirps ) {
	  // EOD with chirps:
	  double cf = 1.0;
	  if ( chirpfrequencies.size() == 1 )
	    cf = chirpfrequencies[0];
	  else if ( chirpfrequencies.size() > 1 )
	    cf = chirpfrequencies[dfrange.pos()];
	  if ( cf < 1e-8 ) {
	    warning( "Chirp frequency too small or negative!" );
	    return Failed;
	  }
	  EventData ct( chirptimes );
	  ct *= 1.0/cf;
	  if ( ct.empty() ) {
	    double p = 1.0/cf;
	    double t = p;
	    do {
	      ct.push( t );
	      t += p;
	    } while ( t < duration );
	  }
	  signal.clear();
	  signal.reserve( signal.indices( duration ) );
	  if ( signal.fixedSampleRate() )
	    signal.setSampleInterval( signal.minSampleInterval() );
	  else
	    signal.setSampleInterval( signal.bestSampleInterval( 2.0*stimulusrate ) );
	  double sig = 0.5*chirpwidth / ::pow( 2.0*log(10.0), 0.5/chirpkurtosis );
	  double p = 0.0;
	  int k = 0;
	  for ( double t=0.0; t<duration; ) {
	    t = signal.length();
	    double f = stimulusrate;
	    double a = 1.0;
	    if ( t < ramp )
	      a = t/ramp;
	    else if ( t > duration - ramp )
	      a = (duration - t)/ramp;
	    if ( k<ct.size() && fabs( t - ct[k] ) < 2.0*chirpwidth ) {
	      double x = t - ct[k];
	      double g = exp( -0.5 * ::pow( (x/sig)*(x/sig), chirpkurtosis ) );
	      f = chirpsize*g + stimulusrate;
	      a *= 1.0 - chirpampl*g;
	    }
	    else if ( k<ct.size() && t > ct[k] + 2.0*chirpwidth )
	      k++;
	    p += f * signal.stepsize();
	    signal.push( a * ::sin( 6.28318530717959 * p ) );
	  }
	  signal.description().setType( "stimulus/eod_chirps" );
	  signal.description().addNumber( "Frequency", stimulusrate, "Hz" );
	  signal.description().addNumber( "Amplitude", amplitude, "mV" );
	  signal.description().addNumber( "TemporalOffset", 0.0, "s" );
	  signal.description().addNumber( "Duration", duration, "s" );
	  signal.description().addNumber( "ChirpSize", chirpsize, "Hz" );
	  signal.description().addNumber( "ChirpWidth", 1000.0*chirpwidth, "ms" );
	  signal.description().addNumber( "ChirpAmplitude", 100.0*(1.0-chirpampl), "%" );
	  signal.description().addNumber( "ChirpKurtosis", chirpkurtosis );
	  signal.description().addNumber( "ChirpFrequency", cf, "Hz" );
	  if ( ! chirptimesfile.empty() && ! chirptimes.empty() )
	    signal.description().addText( "ChirpTimesFile", chirptimesfile );
	  signal.description().addInteger( "ChirpNumber", k );
	  signal.description().addNumber( "ChirpTimes", ct[0] );
	  for ( int j=1; j<k; j++ )
	    signal.description().pushNumber( "ChirpTimes", ct[j] );
	}
	else {
	  double p = 1.0;
	  if ( fabs( deltaf ) > 0.01 )
	    p = rint( stimulusrate / fabs( deltaf ) ) / stimulusrate;
	  else
	    p = 1.0/stimulusrate;
	  int n = (int)::rint( duration / p );
	  if ( n < 1 )
	    n = 1;
	  signal.sineWave( n*p, -1.0, stimulusrate, 1.0, ramp );
	  signal.setIdent( "sinewave" );
	}
	lockAll();
	duration = signal.length();
	signal.setDelay( before );
	signal.setIntensity( amplitude );

	signal.save( "chirpeod.dat" );

	// output signal:
	starttime = currentTime();
	write( signal );

	// signal failed?
	if ( signal.failed() ) {
	  string s = "Output of stimulus failed!<br>Error code is <b>";
	  s += signal.errorText() + "</b>";
	  warning( s, 2.0 );
	  writeZero( GlobalEField );
	  return Failed;
	}
	sleep( 0.2 );
      }
      if ( interrupt() ) {
	if ( fixeddf )
	  writeZero( "Amplitude" );
	else
	  writeZero( GlobalEField );
	return Aborted;
      }
      double signaltime = signalTime();

      // stimulation loop:
      do {
	// get data:
	const EventData &eodglobal = events( EODEvents );
	if ( initeoditer ) {
	  eoditer = eodglobal.begin( signaltime - before );
	  int k = 0;
	  for ( ; eoditer < eodglobal.end() && k<10; ++eoditer, ++k );
	  if ( eoditer != eodglobal.end() )
	    initeoditer =  false;
	}
	for ( ; eoditer < eodglobal.end(); ++eoditer ) {
	  EventFrequencyIterator fiter = eoditer;
	  eodfrequency.push( fiter.time() - signaltime, *fiter );
	  EventSizeIterator siter = eoditer;
	  eodamplitude.push( siter.time() - signaltime, *siter );
	}
	if ( GlobalEFieldEvents >= 0 ) {
	  const EventData &stimglobal = events( GlobalEFieldEvents );
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
	plot( deltaf, amplitude, duration, eodfrequency, jarchirpevents,
	      showstimulus, stimfrequency, chirptimes );

	if ( fixeddf ) {
	  double fishrate = events( EODEvents ).frequency( currentTime()-averagetime, currentTime() );
	  if ( fakefish > 0.0 )
	    fishrate = fakefish;
	  OutData signal;
	  signal.setTraceName( "Frequency" );
	  signal.constWave( fishrate + deltaf );
	  directWrite( signal, false );
	  // signal failed?
	  if ( signal.failed() ) {
	    string s = "Output of frequency stimulus failed!<br>Error code is <b>";
	    s += signal.errorText() + "</b>";
	    warning( s, 2.0 );
	    writeZero( "Amplitude" );
	    return Failed;
	  }
	}

	sleepWait( 0.2 );
	if ( interrupt() ) {
	  if ( fixeddf )
	    writeZero( "Amplitude" );
	  else
	    writeZero( GlobalEField );
	  return Aborted;
	}

      } while ( currentTime() - starttime < before + duration -  ramptime );

      // ending stimulus:
      if ( fixeddf && ramptime > 0.0 ) {
	OutData signal;
	signal.setTraceName( "Amplitude" );
	signal.rampWave( ramp, -1.0, 1.0, 0.0 );
	write( signal );
	// signal failed?
	if ( signal.failed() ) {
	  string s = "Output of final ramp stimulus failed!<br>Error code is <b>";
	  s += signal.errorText() + "</b>";
	  warning( s, 2.0 );
	  writeZero( "Amplitude" );
	  return Failed;
	}
      }

      // after stimulus recording loop:
      do {
	// get data:
	const EventData &eodglobal = events( EODEvents );
	for ( ; eoditer < eodglobal.end(); ++eoditer ) {
	  EventFrequencyIterator fiter = eoditer;
	  eodfrequency.push( fiter.time() - signaltime, *fiter );
	  EventSizeIterator siter = eoditer;
	  eodamplitude.push( siter.time() - signaltime, *siter );
	}
	if ( GlobalEFieldEvents >= 0 ) {
	  const EventData &stimglobal = events( GlobalEFieldEvents );
	  for ( ; stimiter < stimglobal.end(); ++stimiter )
	    stimfrequency.push( stimiter.time() - signaltime, *stimiter );
	}
	plot( deltaf, amplitude, duration, eodfrequency, jarchirpevents,
	      showstimulus, stimfrequency, chirptimes );

	sleepWait( 0.2 );
	if ( interrupt() ) {
	  if ( fixeddf )
	    writeZero( "Amplitude" );
	  else
	    writeZero( GlobalEField );
	  return Aborted;
	}

      } while ( currentTime() - starttime < before + duration + after + 0.01 );

      setSaving( savetraces );

      // analyze:
      // chirps:
      if ( ChirpEvents >= 0 )
	jarchirpevents.assign( events( ChirpEvents ),
			       signaltime - before,
			       signaltime + duration + after, signaltime );
      plot( deltaf, amplitude, duration, eodfrequency, jarchirpevents,
	    showstimulus, stimfrequency, chirptimes );
      save( deltaf, amplitude, duration, pause, fishrate, stimulusrate,
	    eodfrequency, eodamplitude, jarchirpevents, stimfrequency, split, FileCount );
      FileCount++;

      // pause:
      sleepWait( pause - after - before );
      if ( interrupt() ) {
	if ( fixeddf )
	  writeZero( "Amplitude" );
	else
	  writeZero( GlobalEField );
      }

    }
  }
  
  if ( fixeddf )
    writeZero( "Amplitude" );
  else
    writeZero( GlobalEField );
  return Completed;
}


void Beats::sessionStarted( void )
{
  FileCount = 0;
  RePro::sessionStarted();
}


void Beats::plot( double deltaf, double amplitude, double duration,
		  const MapD &eodfrequency, const EventData &jarchirpevents, 
		  bool showstimulus, const MapD &stimfrequency, const EventData &chirptimes )
{
  P.lock();
  // eod frequency with chirp events:
  P.clear();
  Str s;
  s = "Delta f = " + Str( deltaf, 0, 0, 'f' ) + "Hz";
  s += ", Amplitude = " + Str( amplitude ) + "mV/cm";
  P.setTitle( s );
  P.plotVLine( 0.0 );
  P.plotVLine( duration );
  if ( showstimulus )
    P.plot( stimfrequency, 1.0, Plot::Cyan, 2, Plot::Solid );
  P.plot( eodfrequency, 1.0, Plot::Green, 2, Plot::Solid );
  if ( ! eodfrequency.empty() ) {
    EventData ct( chirptimes );
    ct.resize( ct.next( eodfrequency.x().back() ) );
    P.plot( ct, 2, 0.0, 1.0, 0.9, Plot::Graph,
	    1, Plot::Circle, 5, Plot::Pixel, Plot::Blue, Plot::Blue );
  }
  P.plot( jarchirpevents, 2, 0.0, 1.0, 0.9, Plot::Graph,
	  1, Plot::Circle, 5, Plot::Pixel, Plot::Yellow, Plot::Yellow );
  P.draw();
  P.unlock();
}


void Beats::save( double deltaf, double amplitude, double duration, double pause,
		  double fishrate, double stimulusrate,
		  const MapD &eodfrequency, const MapD &eodamplitude, const EventData &jarchirpevents,
		  const MapD &stimfrequency, bool split, int count )
{
  Options header;
  header.addNumber( "Delta f", deltaf, "Hz", "%.1f" );
  header.addNumber( "EODf", fishrate, "Hz", "%.1f" );
  header.addNumber( "StimulusFrequency", stimulusrate, "Hz", "%.1f" );
  header.addNumber( "Amplitude", amplitude, "mV/cm", "%.3f" );
  header.addNumber( "Duration", duration, "sec", "%.3f" );
  header.addNumber( "Pause", pause, "sec", "%.3f" );
  header.addText( "RePro Time", reproTimeStr() );
  header.addText( "Session Time", sessionTimeStr() );
  header.newSection( settings(), 1 );

  unlockAll();
  setWaitMouseCursor();
  saveEODFreq( header, eodfrequency, eodamplitude, split, count );
  saveChirps( header, jarchirpevents, split, count );
  restoreMouseCursor();
  lockAll();
}


void Beats::saveEODFreq( const Options &header, const MapD &eodfrequency,
			 const MapD &eodamplitude, bool split, int count )
{
  ofstream df( addPath( "beats-eod" + ( split ? "-"+Str( count+1, 2, '0' ) : "" ) + ".dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header:
  header.save( df, "# " );
  df << '\n';

  // write key:
  const EventData &eodglobal = events( EODEvents );
  TableKey key;
  key.addNumber( "time", "s", "%11.7f" );
  key.addNumber( "freq", "Hz", "%6.2f" );
  key.addNumber( "ampl", eodglobal.sizeUnit(), eodglobal.sizeFormat() );
  key.saveKey( df );

  // write data into file:
  for ( int k=0; k<eodfrequency.size(); k++ ) {
    key.save( df, eodfrequency.x(k), 0 );
    key.save( df, eodfrequency.y(k) );
    key.save( df, eodglobal.sizeScale() * eodamplitude.y(k) );
    df << '\n';
  }
  df << "\n\n";
}


void Beats::saveChirps( const Options &header, const EventData &jarchirpevents,
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
  const EventData &chirps = events( ChirpEvents );
  TableKey key;
  key.addNumber( "time", "s", "%9.5f" );
  key.addNumber( "freq", chirps.sizeUnit(), chirps.sizeFormat() );
  key.addNumber( "width", chirps.widthUnit(), chirps.widthFormat() );
  key.saveKey( df );

  // write data into file:
  for ( int k=0; k<jarchirpevents.size(); k++ ) {
    key.save( df, jarchirpevents[k], 0 );
    key.save( df, chirps.sizeScale() * jarchirpevents.eventSize( k ) );
    key.save( df, chirps.widthScale() * jarchirpevents.eventWidth( k ) );
    df << '\n';
  }
  if ( jarchirpevents.size() <= 0 ) {
    key.save( df, "-0", 0 );
    key.save( df, "-0" );
    key.save( df, "-0" );
    df << '\n';
  }
  df << "\n\n";
}


addRePro( Beats, efield );

}; /* namespace efield */

#include "moc_beats.cc"
