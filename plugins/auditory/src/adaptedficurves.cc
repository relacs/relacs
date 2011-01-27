/*
  auditory/adaptedficurves.cc
  First adapts the neuron to a background sound intensity and then measures f-I curves.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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

#include <relacs/rangeloop.h>
#include <relacs/tablekey.h>
#include <relacs/auditory/adaptedficurves.h>
using namespace relacs;

namespace auditory {


AdaptedFICurves::AdaptedFICurves( void )
  : RePro( "AdaptedFICurves", "auditory", "Jan Benda", "1.0", "Jan 27, 2011" )
{
  // add some options:
  addLabel( "Stimulus" ).setStyle( OptWidget::TabLabel );
  addLabel ( "Adaptation stimulus" );
  addSelection( "adaptbase", "Intensity of adapting stimulus relative to", "SPL|Threshold" );
  addNumber( "adaptint", "Sound intensity of adapting stimulus", 50.0, 0.0, 200.0, 0.5, "dB SPL" );
  addNumber( "adaptinit", "Duration of initial adaptation stimulus", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "adaptduration", "Duration of subsequent adaptation stimuli", 0.1, 0.001, 100000.0, 0.001, "s", "ms" );
  addLabel ( "Test stimuli" );
  addSelection( "intbase", "Intensities of test stimulus relative to", "SPL|Threshold|Adaptation stimulus" );
  addNumber( "intmin", "Minimum sound intensity of test stimulus", 50.0, 0.0, 200.0, 0.5, "dB" );
  addNumber( "intmax", "Maximum sound intensity of test stimulus", 100.0, 0.0, 200.0, 0.5, "dB" );
  addNumber( "intstep", "Sound-intensity steps of test stimulus", 10.0, 0.0, 200.0, 0.5, "dB SPL" );
  addNumber( "duration", "Duration of test stimuli", 0.1, 0.001, 100000.0, 0.001, "s", "ms" );
  addLabel( "General" ).setStyle( OptWidget::TabLabel );
  addSelection( "side", "Speaker", "left|right|best" );
  addNumber( "carrierfreq", "Frequency of carrier", 0.0, -40000.0, 40000.0, 500.0, "Hz", "kHz" );
  addBoolean( "usebestfreq", "Relative to the cell's best frequency", true );
  addNumber( "ramp", "Duration of ramps for all intenisty transitions", 0.001, 0.001, 1000.0, 0.001, "s", "ms" );
  addNumber( "pause", "Pause between stimuli", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "delay", "Part of pause before stimulus", 0.1, 0.001, 100000.0, 0.001, "s", "ms" );
  addInteger( "repetitions", "Number of repetitions of the stimulus", 10, 0, 10000, 1 );
  addLabel ( "Analysis" );
  addNumber( "onsettime", "Onset rate occurs within", 0.1, 0.0, 1000.0, 0.002, "seconds", "ms" );
  addNumber( "sstime", "Width for measuring steady-states", 0.1, 0.0, 1000.0, 0.002, "seconds", "ms" );

  addTypeStyle( OptWidget::Bold, Parameter::Label );

  // plot:
  setWidget( &P );
}


int AdaptedFICurves::main( void )
{
  // get options:
  int adaptbase = index( "adaptbase" );
  double adaptint = number( "adaptint" );
  double adaptinit = number( "adaptinit" );
  double adaptduration = number( "adaptduration" );
  int intbase = index( "intbase" );
  double intmin = number( "intmin" );
  double intmax = number( "intmax" );
  double intstep = number( "intstep" );
  double duration = number( "duration" );
  int side = index( "side" );
  double carrierfrequency = number( "carrierfreq" );
  bool usebestfreq = boolean( "usebestfreq" );
  double pause = number( "pause" );
  double delay = number( "delay" );
  double ramp = number( "ramp" );
  double repetitions = integer( "repetitions" );
  double onsettime = number( "onsettime" );
  double sstime = number( "sstime" );

  if ( side > 1 )
    side = metaData( "Cell" ).index( "best side" );
  string sidestr = side > 0 ? "right" :  "left";
  if ( usebestfreq ) {
    double cf = metaData( "Cell" ).number( sidestr + " frequency" );
    if ( cf > 0.0 )
      carrierfrequency += cf;
  }

  // intensities:
  double bt = metaData( "Cell" ).number( sidestr + " threshold" );
  if ( adaptbase == 1 )
    adaptint += bt;
  if ( intbase == 1 ) {
    intmin += bt;
    intmax += bt;
  }
  else if ( intbase == 2 ) {
    intmin += adaptint;
    intmax += adaptint;
  }

  // test intensities:
  RangeLoop intrange( intmin, intmax, intstep );
  intrange.alternateInDown();

  // amplitude modulation:
  MapD times;
  SampleDataD am( 0.0, 10.0, 0.0005 );
  am.clear();
  int rn = am.indices( ramp );
  double xb = ::pow( 10.0, 0.05*(adaptint - intmax) );
  double x = 0.0;
  double x0 = 0.0;
  if ( rn > 0 )
    am.push( x );
  for ( intrange.reset(); ! intrange; ++intrange ) {
    // adapt:
    x0 = x;
    x = xb;
    for ( int j=1; j<=rn; j++ )
      am.push( (x-x0)*j/rn+x0 );
    for ( int j=0; j<am.indices( intrange.loop()==0 ? adaptinit : adaptduration ) - rn; j++ )
      am.push( x );
    // test:
    times.push( *intrange, am.length() );
    x0 = x;
    x = ::pow( 10.0, 0.05*(*intrange - intmax) );
    for ( int j=1; j<=rn; j++ )
      am.push( (x-x0)*j/rn+x0 );
    for ( int j=0; j<am.indices( duration ) - rn; j++ )
      am.push( x );
  }
  // final ramp:
  x0 = x;
  x = 0.0;
  for ( int j=1; j<=rn; j++ )
    am.push( (x-x0)*j/rn+x0 );

  // stimulus:
  OutData signal;
  signal.setTrace( Speaker[ side ] );
  signal.fill( am, carrierfrequency, "pulses" );
  signal.setDelay( delay );
  signal.setIntensity( intmax );

  // amplitude modulation:
  am.decibel( 1.0 );
  am *= 2.0;
  am += intmax;
  am.front() = 0.0;
  am.back() = 0.0;

  // plot:
  P.lock();
  P.resize( 3, Plot::Copy );

  P[0].clear();
  P[0].setSize( 0.6, 0.55 );
  P[0].setOrigin( 0.0, 0.45 );
  P[0].setLMarg( 7 );
  P[0].setRMarg( 2 );
  P[0].setBMarg( 0.5 );
  P[0].setTMarg( 1 );
  P[0].noXTics();
  P[0].setXRange( 0.0, 1000.0*signal.length() );
  P[0].setYLabel( "Frequency [Hz]" );
  P[0].setYLabelPos( 2.0, Plot::FirstMargin, 0.5, Plot::Graph, Plot::Center, -90.0 );
  P[0].setYRange( 0.0, Plot::AutoScale );
  P[0].setYTics( );

  P[1].clear();
  P[1].setSize( 0.6, 0.45 );
  P[1].setOrigin( 0.0, 0.0 );
  P[1].setLMarg( 7 );
  P[1].setRMarg( 2 );
  P[1].setBMarg( 5 );
  P[1].setTMarg( 0 );
  P[1].setXLabel( "Time [ms]" );
  P[1].setXRange( 0.0, 1000.0*signal.length() );
  P[1].setYLabel( "Intensity [dB SPL]" );
  P[1].setYLabelPos( 2.0, Plot::FirstMargin, 0.5, Plot::Graph, Plot::Center, -90.0 );
  P[1].setYRange( intmin, intmax );
  P[1].setYTics( );
  P[1].plot( am, 1000.0, Plot::Green, 2, Plot::Solid );

  P[2].clear();
  P[2].setSize( 0.4, 1.0 );
  P[2].setOrigin( 0.6, 0.0 );
  P[2].setLMarg( 7 );
  P[2].setRMarg( 2 );
  P[2].setBMarg( 5 );
  P[2].setTMarg( 1 );
  P[2].setXLabel( "Intensity [dB SPL]" );
  P[2].setXRange( intmin, intmax );
  P[2].setXTics();
  P[2].setYLabel( "Frequency [Hz]" );
  P[2].setYLabelPos( 2.0, Plot::FirstMargin, 0.5, Plot::Graph, Plot::Center, -90.0 );
  P[2].setYRange( 0.0, Plot::AutoScale );
  P[2].setYTics( );

  P.setCommonXRange( 0, 1 );

  P.draw();
  P.unlock();

  // plot trace:
  tracePlotSignal( signal.length()+delay, delay );

  // header:
  Options header;
  header.addInteger( "index1", totalRuns()-1 );
  header.addNumber( "carrier frequency", 0.001*carrierfrequency, "kHz", "%.3f" );
  header.addInteger( "side", side );
  header.addNumber( "minimum intensity", intmin, "dB SPL", "%.1f" );
  header.addNumber( "maximum intensity", intmax, "dB SPL", "%.1f" );
  header.addNumber( "adapting intensity", adaptint, "dB SPL", "%.1f" );
  header.addText( "session time", sessionTimeStr() ); 
  header.addLabel( "status:" );

  // variables:
  EventList spikes;
  SampleDataD rate( 0.0, signal.length(), 0.001 );
  MapD onsetrates;
  MapD onsetratessd;
  MapD ssrates;
  MapD ssratessd;
  int state = Completed;

  timeStamp();

  for ( int count=0;
        ( repetitions <= 0 || count < repetitions ) && softStop() == 0;
        count++ ) {

    // message:
    Str s = "Background intensity: <b>" + Str( adaptint, 0, 1, 'f' ) + " dB SPL</b>";
    s += ",  Loop <b>" + Str( count+1 ) + "</b>";
    if ( repetitions > 0 )
      s += " of <b>" + Str( repetitions ) + "</b>";
    message( s );

    // output:
    write( signal );
    if ( signal.error() ) {
      warning( "Output of stimulus failed!<br>Signal error: <b>" +
               signal.errorText() + "</b>," +
               "<br> Loop: <b>" + Str( count+1 ) + "</b>" +
               "<br>Exit now!" );
      writeZero( Speaker[ side ] );
      return Failed;
    }

    sleep( signal.length() + delay + ( pause > 0.01 ? 0.01 : pause ) );

    if ( interrupt() ) {
      if ( count == 0 )
        state = Aborted;
      break;
    }

    analyze( spikes, rate, delay, signal.length(), pause, count,
	     sstime, onsettime, times, onsetrates, onsetratessd, ssrates, ssratessd );
    plot( spikes, rate, am, onsetrates, ssrates, adaptint );
 
    sleepOn( signal.length() + pause - delay );
    if ( interrupt() ) {
      if ( count == 0 )
        state = Aborted;
      break;
    }
    timeStamp();
   
  }

  if ( state == Completed ) {
    unlockAll();
    saveSpikes( header, spikes );
    saveRate( header, rate );
    saveData( header, times, onsetrates, onsetratessd, ssrates, ssratessd );
    lockAll();
  }

  writeZero( Speaker[ side ] );
  return state;
}


void AdaptedFICurves::saveSpikes( const Options &header, const EventList &spikes )
{
  // create file:
  ofstream df( addPath( "adaptedficurves-spikes.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# " );
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   ", -1, 0, false, true );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.saveKey( df, true, false );

  // write data:
  spikes.saveText( df, 1000.0, 7, 1, 'f', 1, "-0" );
  df << '\n';
}


void AdaptedFICurves::saveRate( const Options &header, const SampleDataD &rate )
{
  // create file:
  ofstream df( addPath( "adaptedficurves-rate.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# " );
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   ", -1, 0, false, true );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.addNumber( "f", "Hz", "%5.1f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<rate.size(); k++ ) {
    key.save( df, rate.pos( k ) * 1000.0, 0 );
    key.save( df, rate[k] );
    df << '\n';
  }
  df << "\n\n";
}


void AdaptedFICurves::saveData( const Options &header, const MapD &times,
				const MapD &onsetrates, const MapD &onsetratessd,
				const MapD &ssrates, const MapD &ssratessd )
{
  // create file:
  ofstream df( addPath( "adaptedficurves-data.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# " );
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   ", -1, 0, false, true );
  df << '\n';
  TableKey key;
  key.addNumber( "I", "dB SPL", "%5.1f" );
  key.addNumber( "t", "ms", "%7.1f" );
  key.addNumber( "f_on", "Hz", "%5.1f" );
  key.addNumber( "s.d.", "Hz", "%5.1f" );
  key.addNumber( "f_ss", "Hz", "%5.1f" );
  key.addNumber( "s.d.", "Hz", "%5.1f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<times.size(); k++ ) {
    key.save( df, times.x(k), 0 );
    key.save( df, times[k] * 1000.0 );
    key.save( df, onsetrates[k] );
    key.save( df, onsetratessd[k] );
    key.save( df, ssrates[k] );
    key.save( df, ssratessd[k] );
    df << '\n';
  }
  df << "\n\n";
}


void AdaptedFICurves::analyze( EventList &spikes, SampleDataD &rate,
			       double delay, double duration, double pause,
			       int count, double sstime, double onsettime,
			       const MapD &times,
			       MapD &onsetrates, MapD &onsetratessd,
			       MapD &ssrates, MapD &ssratessd )
{
  // spikes:
  const EventData &se = events( SpikeEvents[0] );
  spikes.push( se, signalTime()-delay,
	       signalTime()+duration+pause,
	       signalTime() );

  // firing frequency:
  SampleDataD ratesd( rate );
  spikes.frequency( rate, ratesd );

  /*
  // rest rate:
  double rr = 0.0;
  for ( int j=0; j < rate.index( 0.0 ); j++ )
    rr += ( rate[ j ] - rr ) / ( j+1 );
  //  response.RestRate = rr;

  // steady-state rate:
  double ssr = 0.0; 
  for ( int j=rate.index( times[0] - sstime ); j<rate.index( times[0] ); j++ )
    ssr += ( rate[j] - ssr ) / ( j+1 );
  //  response.PreRate = ssr;
  */

  // peak and steady-state firing rates:
  onsetrates.clear();
  onsetratessd.clear();
  ssrates.clear();
  ssratessd.clear();
  for ( int k=0; k<times.size(); k++ ) {
    // steady-state:
    double ssfsd = 0.0;
    double ssf = spikes.frequency( times[k] - sstime, times[k], ssfsd );
    ssrates.push( times.x(k), ssf );
    ssratessd.push( times.x(k), ssfsd );
    // onset:
    double maxr = ssf;
    double maxrsd = 0.0;
    for ( int j=rate.index( times[k] );
	  j<rate.index( times[k] + onsettime );
	  j++ ) {
      if ( ::fabs( rate[j] - ssf ) > ::fabs( maxr - ssf ) ) {
	maxr = rate[j];
	maxrsd = ratesd[j];
      }
    }
    onsetrates.push( times.x(k), maxr );
    onsetratessd.push( times.x(k), maxrsd );
  }
  onsetrates.sortByX();
  onsetratessd.sortByX();
  ssrates.sortByX();
  ssratessd.sortByX();
}


void AdaptedFICurves::plot( const EventList &spikes, const SampleDataD &rate,
			    const SampleDataD &am,
			    const MapD &onsetrates, const MapD &ssrates,
			    double adaptint )
{
  P.lock();

  // rate and spikes:
  P[0].clear();
  for ( int i=0; i<spikes.size() && i<20; i++ )
    P[0].plot( spikes[i], 1000.0,
	       1.0 - (i+1)*0.05, Plot::Graph, 2, Plot::StrokeUp,
	       0.045, Plot::Graph, Plot::Red, Plot::Red );
  P[0].plot( rate, 1000.0, Plot::Yellow, 2, Plot::Solid );


  // f-I-curves:
  P[2].clear();
  P[2].plotVLine( adaptint, Plot::White, 2 );
  P[2].plot( ssrates, 1.0, Plot::Red, 4, Plot::Solid,
	     Plot::Circle, 10, Plot::Red, Plot::Red );
  P[2].plot( onsetrates, 1.0, Plot::Blue, 4, Plot::Solid,
	     Plot::Circle, 10, Plot::Blue, Plot::Blue );
      
  P.draw();

  P.unlock();  
}


addRePro( AdaptedFICurves );

}; /* namespace auditory */

#include "moc_adaptedficurves.cc"
