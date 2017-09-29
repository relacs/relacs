/*
  auditoryprojects/mothasongs.cc
  Artificial songs of Arctiid moths

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

#include <relacs/stats.h>
#include <relacs/kernel.h>
#include <relacs/sampledata.h>
#include <relacs/eventlist.h>
#include <relacs/tablekey.h>
#include <relacs/auditoryprojects/mothasongs.h>
#include <relacs/rangeloop.h>
using namespace relacs;

namespace auditoryprojects {


MothASongs::MothASongs( void )
  : RePro( "MothASongs", "auditoryprojects", "Nils Brehm", "1.0", "Sep 29, 2017" )
{
  // add some options:
  newSection( "Stimulus" );
  addNumber( "duration", "Stimulus duration", 0.11, 0.001, 10000.0, 0.001, "s", "ms" );
  addNumber( "intensity", "Intensity",80,10,100,1, "dB SPL" );
  addInteger( "repeats", "Repeats", 10, 0, 100,  1);
  addNumber( "pause", "Pause", 1, 0.1, 100, 0.1, "s", "ms");
  addSelection( "side", "Speaker", "left|right|best" );
  newSection("Pulse settings");
  addNumber( "tau", "Damping time-scale", 0.001, 0.00001, 10, 0.0001, "s", "ms" );
  addText( "apulserange", "Active pulse times", "0..40..10" ).setUnit( "ms" );
  addText( "ppulserange", "Passive pulse times", "60..100..10" ).setUnit( "ms" );
  addText( "afreq", "Active pulse frequencies", "10" ).setUnit( "kHz" );
  addText( "pfreq", "Passive pulse Frequencies", "10" ).setUnit( "kHz" );
  addNumber("samplingrate", "Sampling rate", 200000, 10000, 1000000, 1000, "Hz", "kHz");
  newSection( "Analysis" );
  addNumber( "before", "Time before stimulus to be analyzed", 0.1, 0.0, 100.0, 0.01, "s", "ms" );
  addNumber( "after", "Time after stimulus to be analyzed", 0.1, 0.0, 100.0, 0.01, "s", "ms" );
  addNumber( "sigma", "Standard deviation of rate smoothing kernel", 0.001, 0.0, 1.0, 0.0001, "seconds", "ms" );

  // plots:
  P.lock();
  P.resize( 2 );
  P.setCommonXRange( 0, 1 );
  P[0].setOrigin( 0.0, 0.5 );
  P[0].setSize( 1.0, 0.5 );
  P[0].setLMarg( 7.0 );
  P[0].setRMarg( 1.5 );
  P[0].setTMarg( 3.5 );
  P[0].setBMarg( 1.0 );
  P[0].noXTics();
  P[0].setYRange( 0.0, Plot::AutoScale );
  P[0].setYLabel( "Firing rate [Hz]" );
  P[0].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		     Plot::Center, -90.0 );
  P[1].setOrigin( 0.0, 0.0 );
  P[1].setSize( 1.0, 0.5 );
  P[1].setLMarg( 7.0 );
  P[1].setRMarg( 1.5 );
  P[1].setTMarg( 0.5 );
  P[1].setBMarg( 5.0 );
  P[1].setXLabel( "Time [ms]" );
  P[1].setYLabel( "Sound pressure" );
  P[1].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		     Plot::Center, -90.0 );
  P.unlock();

  setWidget( &P );
}


int MothASongs::main( void )
{
  // get options:
  double duration = number( "duration" );
  int side = index( "side" );
  double intensity = number("intensity");

  double tau = number("tau");
  double pause = number("pause");
  int repeats = integer("repeats");
  double samplingrate = number("samplingrate");
  
  string afreq = allText( "afreq" );
  RangeLoop apulsefreq( afreq, 1000.0 );

  string pfreq = allText( "pfreq" );
  RangeLoop ppulsefreq( pfreq, 1000.0 );

  string apulserange = allText( "apulserange" );
  RangeLoop atimeofpulse( apulserange, 0.001 ); 
 
  string ppulserange = allText( "ppulserange" );
  RangeLoop ptimeofpulse( ppulserange, 0.001 ); 

  double before = number( "before" );
  if ( before > pause )
    before = pause;
  double after = number( "after" );
  if ( after > pause )
    after = pause;
  double sigma = number( "sigma" );

  // check parameter:
  if ( atimeofpulse.maxValue() >= duration ) {
    warning( "Active pulse position exceeds stimulus duration!" );
    return Failed;
  }
  if ( ptimeofpulse.maxValue() >= duration ) {
    warning( "Passive pulse position exceeds stimulus duration!" );
    return Failed;
  }
  if ( atimeofpulse.size() != apulsefreq.size() && apulsefreq.size() != 1 ) {
    warning( "Need as many active pulse frequencies as times!" );
    return Failed;
  }
  if ( ptimeofpulse.size() != ppulsefreq.size() && ppulsefreq.size() != 1 ) {
    warning( "Need as many passive pulse frequencies as times!" );
    return Failed;
  }
  
  // get best side:
  lockMetaData();
  if ( side > 1 )
    side = metaData().index( "Cell>best side" );
  unlockMetaData();

  // plot trace:
  tracePlotSignal( duration, 0.0 );

  // make stimulus:
  OutData wave;
  wave.setTrace( Speaker[ side ] );
  wave.setSampleRate( samplingrate );
  wave.setIntensity( intensity );
  wave.setCarrierFreq( apulsefreq.front() );  // XXXX this needs to be improved!!!!
  int n = duration * wave.sampleRate();
  wave.resize( n );
  wave.description().setType( "stimulus/moth_song" );
  // add pulses:
  OutData pulse;
  pulse.setTrace( Speaker[ side ] );
  for ( int j=0; j<atimeofpulse.size(); j++ ) {
    double freq = apulsefreq[0];
    if ( j < apulsefreq.size() )
      freq = apulsefreq[j];
    pulse.dampedOscillationWave( 5.0*tau, wave.stepsize(), tau, freq );
    wave += pulse.shift( atimeofpulse[j] ); 
  }
  for ( int j=0; j<ptimeofpulse.size(); j++ ) {
    double freq = ppulsefreq[0];
    if ( j < ppulsefreq.size() )
      freq = ppulsefreq[j];
    pulse.dampedOscillationWave( 5.0*tau, wave.stepsize(), tau, freq, M_PI );
    wave += pulse.shift( ptimeofpulse[j] );
  }

  // results:
  EventList spikes;
  SampleDataD rate( -before, duration+after, 0.0001, 0.0 );

  // plot stimulus:
  P.lock();
  P.clearPlots();
  P[0].setXRange( 0.0, 1000.0*duration );
  P[0].setYRange( 0.0, Plot::AutoScale );
  P[1].setXRange( 0.0, 1000.0*duration );
  P[1].setXLabel( "Time [ms]" );
  P[1].plot( wave, 1000.0, Plot::Green, 2 );
  P.draw();
  P.unlock();

  int state = Completed;

  // output stimulus:  
  for ( int count=0;
	( repeats <= 0 || count<repeats ) && softStop() == 0;
	count++ ) {

    write( wave );
    if ( wave.error() ) {
      warning( "Output of stimulus failed!<br>Signal error: <b>" +
	       wave.errorText() + "</b>," +
	       "<br> Loop: <b>" + Str( count+1 ) + "</b>" +
	       "<br>Exit now!" );
      writeZero( Speaker[ side ] );
      return Failed;
    }
    
    sleep( pause );
    if ( interrupt() ) {
      if ( count == 0 )
	state = Aborted;
      break;
    }

    // ananlyze:
    if ( SpikeEvents[0] >= 0 ) {
      // spikes:
      spikes.push( events( SpikeEvents[0] ), signalTime()-before,
		   signalTime() + duration + after, signalTime() );
      int trial = spikes.size()-1;
      spikes.back().addRate( rate, trial, GaussKernel( sigma ) );
    }

    // plot spikes and firing rate:
    P.lock();
    P[0].clear();
    int maxspikes = (int)rint( 20.0 / SpikeTraces );
    if ( maxspikes < 4 )
      maxspikes = 4;
    int offs = (int)spikes.size() > maxspikes ? spikes.size() - maxspikes : 0;
    double delta = repeats > 0 && repeats < maxspikes ? 1.0/repeats : 1.0/maxspikes;
    for ( int i=offs, j=0; i<spikes.size(); i++ ) {
      j++;
      P[0].plot( spikes[i], 0, 0.0, 1000.0, 1.0 - delta*(j-0.1), Plot::Graph, 2, Plot::StrokeUp, delta*0.8, Plot::Graph, Plot::Red, Plot::Red );
    }
    P[0].plot( rate, 1000.0, Plot::Yellow, 2, Plot::Solid );
    P.draw();
    P.unlock();

  }

  // file header:
  Options header;
  header.addInteger( "index", completeRuns() );
  header.addInteger( "repro index", reproCount() );
  header.addNumber( "repro time", reproStartTime(), "s", "%0.3f" );
  header.addNumber( "carrier frequency", 0.001*wave.carrierFreq(), "kHz", "%.3f" );
  header.addInteger( "side", side );
  header.addText( "session time", sessionTimeStr() ); 
  lockStimulusData();
  header.newSection( stimulusData() );
  unlockStimulusData();
  header.newSection( settings() );
  saveSpikes( header, spikes );
  saveRate( header, rate );
  saveStimulus( header, wave );
  
  writeZero( Speaker[ side ] );
  return state;
}


void MothASongs::saveSpikes( const Options &header, const EventList &spikes )
{
  // create file:
  ofstream df( addPath( "mothasong-spikes.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.saveKey( df, true, false );

  // write data:
  spikes.saveText( df, 1000.0, 7, 1, 'f', 1, "-0" );
  df << '\n';
}


void MothASongs::saveRate( const Options &header, const SampleDataD &rate )
{
  // create file:
  ofstream df( addPath( "mothasong-rate.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "time", "ms", "%7.1f" );
  key.addNumber( "rate", "Hz", "%5.1f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<rate.size(); k++ ) {
    double t = rate.pos( k );
    key.save( df, t * 1000.0, 0 );
    key.save( df, rate[k] );
    df << '\n';
  }
  df << "\n\n";
}


void MothASongs::saveStimulus( const Options &header, const OutData &wave )
{
  // create file:
  ofstream df( addPath( "mothasong-stimulus.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "time", "ms", "%8.3f" );
  key.addNumber( "sound", "pressure", "%7.4f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<wave.size(); k++ ) {
    double t = wave.pos( k );
    key.save( df, t * 1000.0, 0 );
    key.save( df, wave[k] );
    df << '\n';
  }
  df << "\n\n";
}


addRePro( MothASongs, auditoryprojects );

}; /* namespace auditoryprojects */

#include "moc_mothasongs.cc"
