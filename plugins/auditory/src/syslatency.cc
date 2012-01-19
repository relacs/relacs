/*
  auditory/syslatency.cc
  Measures latency of the whole system, i.e. signal delay, transduction, 

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

#include <fstream>
#include <iomanip>
#include <relacs/tablekey.h>
#include <relacs/stats.h>
#include <relacs/sampledata.h>
#include <relacs/random.h>
#include <relacs/auditory/session.h>
#include <relacs/auditory/syslatency.h>
using namespace relacs;

namespace auditory {


SysLatency::SysLatency( void )
  : RePro( "SysLatency", "auditory", "Jan Benda", "1.5", "Jan 10, 2008" )
{
  // options:
  addLabel( "Stimulus" );
  addNumber( "rate", "Target firing rate", 100.0, 0.0, 1000.0, 20.0, "Hz" );
  addInteger( "pwaves", "Number of cycles of pertubation", 10, 0, 1000, 1 );
  addNumber( "pintensity", "Intensity of pertubations", 10.0, 0.0, 100.0, 1.0, "dB" );
  addNumber( "minpintensity", "Minimum intensity of pertubations", 4.0, 0.0, 100.0, 0.5, "dB" );
  addNumber( "carrierfreq", "Frequency of carrier", 0.0, -40000.0, 40000.0, 2000.0, "Hz", "kHz" );
  addBoolean( "usebestfreq", "Relative to the cell's best frequency", true );
  addNumber( "ramp", "Ramp of stimulus", 0.002, 0.0, 10.0, 0.001, "seconds", "ms" );
  addNumber( "duration", "Duration of stimulus", 0.6, 0.0, 10.0, 0.05, "seconds", "ms" );
  addNumber( "pause", "Pause", 0.6, 0.0, 10.0, 0.05, "seconds", "ms" );
  addInteger( "repeats", "Number of stimulus repetitions", 10, 0, 10000, 2 );
  addSelection( "side", "Speaker", "left|right|best" );
  addLabel( "Analysis" );
  addNumber( "skipwin", "Initial portion of stimulus not used for analysis", 0.1, 0.0, 100.0, 0.01, "seconds", "ms" );
  addNumber( "analysewin", "Window used for ISI analysis", 0.01, 0.0, 100.0, 0.01, "seconds", "ms" );
  addNumber( "maxlat", "Maximum latency", 0.01, 0.0, 1.0, 0.001, "seconds", "ms" );
  addNumber( "latstep", "Resolution of latency", 0.0001, 0.0, 1.0, 0.0001, "seconds", "ms" );
  addNumber( "coincwin", "Window width for coincident spikes", 0.0005, 0.0, 1.0, 0.0005, "seconds", "ms" );
  addBoolean( "adjust", "Adjust input gain", true );

  addTypeStyle( OptWidget::Bold, Parameter::Label );

  // plot:
  P.lock();
  P.resize( 2, 2, true );
  P[0].setLMarg( 7.0 );
  P[0].setTMarg( 3.0 );
  P[0].setXLabel( "Latency" );
  P[0].setYLabel( "Coincident spikes" );
  P[0].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		     Plot::Center, -90.0 );
  P[1].setLMarg( 7.0 );
  P[1].setTMarg( 3.0 );
  P[1].setXLabel( "Trigger phase" );
  P[1].setXRange( 0.0, 1.0 );
  P[1].setYLabel( "Phase response" );
  P[1].setYLabelPos( 2.3, Plot::FirstMargin, 0.5, Plot::Graph,
		     Plot::Center, -90.0 );
  P[1].setYRange( -0.5, 1.0 );
  P.unlock();
  setWidget( &P );
}


SysLatency::~SysLatency( void )
{
}


int SysLatency::main( void )
{
  if ( SpikeEvents[0] < 0 || SpikeTrace[0] < 0 ) {
    warning( "No spike trace!" );
    return Failed;
  }

  // get options:
  settings().setTypeFlags( 16, -Parameter::Blank );
  double targetrate = number( "rate" );
  int pwaves = integer( "pwaves" );
  double pintensity = number( "pintensity" );
  double minpintensity = number( "minpintensity" );
  double carrierfrequency = number( "carrierfreq" );
  bool usebestfreq = boolean( "usebestfreq" );
  double ramp = number( "ramp" );
  double duration = number( "duration" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  int side = index( "side" );
  double skipwin = number( "skipwin" );
  double analysewin = number( "analysewin" );
  double maxlatency = number( "maxlat" );
  double latencystep = number( "latstep" );
  double coincwin = number( "coincwin" );
  bool adjustgain = boolean( "adjust" );

  if ( side > 1 )
    side = metaData( "Cell" ).index( "best side" );
  if ( usebestfreq ) {
    double cf = metaData( "Cell" ).number( side > 0 ? "right frequency" :  "left frequency" );
    if ( cf > 0.0 )
      carrierfrequency += cf;
  }
  double intensity = 0.0;

  // plot trace:
  tracePlotSignal( duration );

  // get intensity:
  auditory::Session *as = dynamic_cast<auditory::Session*>( control( "Session" ) );
  if ( as == 0 ) {
    warning( "No auditory session!", 4.0 );
    return Failed;
  }
  else {
    MapD fic = as->ssFICurve( side, carrierfrequency );
    if ( fic.empty() )
      fic = as->fICurve( side, carrierfrequency );
    // find appropriate intensity:
    for ( int k = ::relacs::maxIndex( fic.y() ); k >= 0; k-- ) {
      if ( fic[k] <= targetrate ) {
	if ( k+1 < fic.size() &&
	     ::fabs( fic[k+1] - targetrate ) < ::fabs( fic[k] - targetrate ) )
	  intensity = fic.x(k+1);
	else
	  intensity = fic.x(k);
	break;
      }
    }
  }
  if ( intensity == 0.0 ) {
    warning( "Could not get appropriate intensity for target firing rate!" );
    return Failed;
  }

  // create random perturbation times:
  ArrayD trigger;
  trigger.reserve( 5 + int( (duration - skipwin)*targetrate ) );
  double t = skipwin;
  do {
    trigger.push( t );
    t += 3.0 * ( 1.0 + ::relacs::rnd() ) / targetrate;
  } while ( t < duration - analysewin - 3.0 / targetrate );

  // create stimulus and adjust trigger:
  OutData signal;
  signal.setTrace( Speaker[ side ] );
  applyOutTrace( signal );
  signal.setBestSample( carrierfrequency );
  signal.reserve( signal.indices( duration ) );
  signal.clearError();
  int tries = 0;
  do {
    // generate sine waves:
    SampleDataD sin2;
    sin2.sin( LinearRange( 0.0, 1.0/carrierfrequency, signal.stepsize() ), carrierfrequency );
    SampleDataD sin1 = ::pow( 10.0, -pintensity/20.0 ) * sin2;
    // create stimulus:
    for ( int i=0; i<trigger.size(); i++ ) {
      // background:
      do {
	signal.append( sin1 );
      } while ( signal.duration() < trigger[i] );
      // trigger:
      trigger[i] = signal.duration();
      // perturbation:
      for ( int n=0; n<pwaves; n++ )
	signal.append( sin2 );
    }
    // complete stimulus:
    do {
      signal.append( sin1 );
    } while ( signal.size() < signal.capacity() );
    // add ramps:
    signal.ramp( ramp );
    // stimulus parameter:
    signal.setIdent( "random pertubations " + Str( pwaves ) + "cycles, " + Str( pintensity ) + "dB" );
    double peakintensity = intensity + pintensity;
    signal.setIntensity( peakintensity );
    testWrite( signal );
    if ( signal.overflow() ) {
      pintensity = signal.intensity() - intensity;
      if ( pintensity < minpintensity ) {
	warning( "Maximum possible perturbation stimulus too small!<br> Maximum itensity: " 
		 + Str( signal.intensity(), 0, 1, 'f' ) + " dB SPL<br> Background intensity: "
		 + Str( intensity, 0, 1, 'f' ) + " dB SPL" );
	return Failed;
      }
    }
    tries++;
    if ( tries > 10 ) {
	warning( "Failed to output stimulus!<br> Maximum itensity: " 
		 + Str( signal.intensity(), 0, 1, 'f' ) + " dB SPL<br> Background intensity: "
		 + Str( intensity, 0, 1, 'f' ) + " dB SPL" );
	return Failed;
    }
  } while ( signal.overflow() );

  if ( signal.error() ) {
    warning( "Output of perturbation stimulus not possible!<br>Signal error: <b>" +
	     signal.errorText() + "</b>.<br>Exit now!" );
    return Failed;
  }

  // variables:
  EventList spikes;
  double spikewidth = 0.0;
  double latency = 0.0;
  double latencysd = 0.0;
  MapD coincidentspikes;
  int maxcoincidence;
  double coinclatency = 0.0;
  MapD prc;
  prc.reserve( repeats * trigger.size() );
  double offset = 0.0;
  double slope = 1.0;
  double meanrate = 0.0;

  // plots:
  P.lock();
  P[0].setXRange( 0.0, 1000.0*maxlatency );
  P[0].setYRange( 0.0, repeats * trigger.size() );
  P.unlock();
    
  for ( int counter=0; counter<repeats; counter++ ) {
    
    // message:
    Str s = "Pertubation Intensity: <b>" + Str( pintensity ) + " dB</b>";
    s += ",  Cycles: <b>" + Str( pwaves );
    s += ",  Loop <b>" + Str( counter+1 ) + "</b> of <b>" + Str( repeats ) + "</b>";
    message( s );
    
    // output:
    for ( int k=0; k<10; k++ ) {
      write( signal );
      if ( signal.success() )
	break;
    }
    if ( signal.error() ) {
      warning( "Output of perturbation stimulus failed!<br>Signal error: <b>" +
	       signal.errorText() + "</b>," + 
               "<br>Loop: <b>" + Str( counter ) + "<\b>." +
	       "<br>Exit now!" );
      writeZero( Speaker[ side ] );
      return Failed;
    }
    sleep( duration + pause );
    if ( interrupt() ) {
      save( carrierfrequency, side, pwaves/carrierfrequency, intensity,
	    spikes, trigger, coincidentspikes, prc,
	    spikewidth, latency, latencysd, maxcoincidence, coinclatency,
	    offset, slope, meanrate );
      writeZero( Speaker[ side ] );
      return Aborted;
    }
    
    // adjust gain of daq board:
    if ( adjustgain )
      adjust( trace( SpikeTrace[0] ), signalTime(), 
	      signalTime() + duration, 0.8 );
    
    analyze( duration, skipwin, analysewin, pwaves/carrierfrequency,
	     coincwin, maxlatency, latencystep,
	     spikes, trigger, coincidentspikes, prc,
	     spikewidth, latency, latencysd, maxcoincidence, coinclatency,
	     offset, slope, meanrate );
    plot( coincidentspikes, prc, coinclatency, offset, slope, meanrate );
    
    if ( softStop() > 0 ) {
      save( carrierfrequency, side, pwaves/carrierfrequency, intensity,
	    spikes, trigger, coincidentspikes, prc,
	    spikewidth, latency, latencysd, maxcoincidence, coinclatency,
	    offset, slope, meanrate );
      writeZero( Speaker[ side ] );
      return Completed;
    }
    
  }

  save( carrierfrequency, side, pwaves/carrierfrequency, intensity,
	spikes, trigger, coincidentspikes, prc,
	spikewidth, latency, latencysd, maxcoincidence, coinclatency,
	offset, slope, meanrate );
  writeZero( Speaker[ side ] );
  return Completed;
}


void SysLatency::saveSpikes( Options &header, const EventList &spikes )
{
  // create file:
  ofstream df( addPath( "syslatencyspikes.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# " );
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   ", -1, 16, false, true );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.saveKey( df, true, false );

  // write data:
  spikes.saveText( df, 1000.0, 7, 1, 'f', 1, "-0" );
  df << '\n';
}


void SysLatency::saveTrigger( Options &header, const ArrayD &trigger )
{
  // create file:
  ofstream df( addPath( "syslatencytrigger.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# " );
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   ", -1, 16, false, true );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<trigger.size(); k++ ) {
    key.save( df, 1000.0*trigger[k], 0 );
    df << '\n';
  }
  df << "\n\n";
}


void SysLatency::saveCoincidentSpikes( Options &header,
				       const MapD &coincidentspikes )
{
  // create file:
  ofstream df( addPath( "syslatencycoincidentspikes.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# " );
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   ", -1, 16, false, true );
  df << '\n';
  TableKey key;
  key.addNumber( "lat", "ms", "%5.1f" );
  key.addNumber( "coinc", "1", "%5.0f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<coincidentspikes.size(); k++ ) {
    key.save( df, 1000.0*coincidentspikes.x( k ), 0 );
    key.save( df, coincidentspikes.y( k ) );
    df << '\n';
  }
  df << "\n\n";
}


void SysLatency::savePRC( Options &header, const MapD &prc )
{
  // create file:
  ofstream df( addPath( "syslatencyprc.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  header.save( df, "# " );
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   ", -1, 16, false, true );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "1", "%5.3f" );
  key.addNumber( "p", "1", "%6.3f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<prc.size(); k++ ) {
    key.save( df, prc.x( k ), 0 );
    key.save( df, prc.y( k ) );
    df << '\n';
  }
  df << "\n\n";
}


void SysLatency::save( double carrierfrequency, int side, double pduration,
		       double intensity, const EventList &spikes,
		       const ArrayD &trigger,
		       const MapD &coincidentspikes, const MapD &prc,
		       double spikewidth, double latency, double latencysd,
		       int maxcoincidence, double coinclatency,
		       double offset, double slope, double meanrate )
{
  if ( ! metaData( "Cell" ).exist( "system latency" ) )
    metaData( "Cell" ).insertNumber( "system latency", "metadata", "System latency",
				-1.0, "s", "%.1f", 1+4 ).setUnit( "s", "ms" );
  metaData( "Cell" ).setNumber( "system latency", coinclatency );

  Options header;
  header.addInteger( "index1", totalRuns() );
  header.addNumber( "carrier frequency", 0.001*carrierfrequency, "kHz", "%.3f" );
  header.addInteger( "side", side );
  header.addNumber( "base intensity", intensity, "dB SPL", "%.1f" );
  header.addNumber( "pertubation duration", 1000.0*pduration, "ms", "%.1f" );
  header.addNumber( "spike width", 1000.0*spikewidth, "ms", "%.1f" );
  header.addNumber( "onset latency", 1000.0*latency, 1000.0*latencysd, "ms", "%.1f" );
  header.addNumber( "coincidence latency", 1000.0*coinclatency, "ms", "%.1f" );
  header.addInteger( "coincident spikes", maxcoincidence );
  header.addInteger( "maximum possible coincident spikes", trigger.size() * spikes.size() );
  header.addNumber( "mean rate", meanrate, "Hz", "%.1f" );
  header.addNumber( "prc slope", slope, "", "%.3f" );
  header.addNumber( "prc offset", offset, "", "%.3f" );
  header.addText( "session time", sessionTimeStr() );
  header.addLabel( "status:" );

  saveSpikes( header, spikes );
  saveTrigger( header, trigger );
  saveCoincidentSpikes( header, coincidentspikes );
  savePRC( header, prc );
}


void SysLatency::plot( const MapD &coincidentspikes, const MapD &prc,
		       double coinclatency, double offset, double slope,
		       double meanrate )
{
  P.lock();
  P[0].clear();
  P[0].setTitle( "Latency=" + Str( 1000.0*coinclatency, 0, 1, 'f' ) + "ms" );
  P[0].plotVLine( 1000.0*coinclatency, Plot::White, 2 );
  P[0].plot( coincidentspikes, 1000.0, Plot::Orange, 2, Plot::Solid, Plot::Circle, 5, Plot::Orange, Plot::Orange );

  P[1].clear();
  P[1].setTitle( "Firing rate=" + Str( meanrate, 0, 0, 'f' ) + "Hz" );
  P[1].plotHLine( 0.0, Plot::White, 2 );
  P[1].plotLine( 0.0, 1.0, 1.0, 0.0, Plot::Blue, 2 );
  P[1].plotLine( 0.0, offset, 1.0, offset+slope, Plot::Yellow, 2 );
  P[1].plot( prc, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 5, Plot::Red, Plot::Red );

  P.draw();

  P.unlock();
}


int SysLatency::coincidentSpikes( double coincwin, double latency,
				  double offset,
				  const EventList &spikes,
				  const ArrayD &trigger )
{
  int coincidence = 0;
  for ( int j=0; j<spikes.size(); j++ ) {
    if ( spikes[j].size() <= 0 )
      continue;
    
    int k = 0;
    for ( int t=0; t<trigger.size(); t++ ) {
      // search for spike at trigger:
      for ( ; k<spikes[j].size() && spikes[j][k]<trigger[t] + latency + offset; k++ );
      if ( k >= spikes[j].size() )
	break;
      if ( spikes[j][k] < trigger[t] + latency + offset + coincwin )
	coincidence++;
    }
  }
  return coincidence;
}


void SysLatency::phaseResponse( double duration, double skipwin,
				double analysewin, double latency,
				const EventList &spikes,
				const ArrayD &trigger, MapD &prc )
{
  prc.clear();
  for ( int j=0; j<spikes.size(); j++ ) {
    if ( spikes[j].size() <= 0 )
      continue;

    double ISI1 = 0.0;
    double ISI2 = 0.0;

    int k;
    // steady-state inter-spike intervals
    double t1 = skipwin;
    for ( k=1; k<spikes[j].size() && spikes[j][k]<t1; k++ );
    int i = 0;
    for ( ISI1=0.0; 
	  k<spikes[j].size() && spikes[j][k]<t1 + analysewin; 
	  k++, i++ )
      ISI1 += (spikes[j][k]-spikes[j][k-1] - ISI1)/(i+1);
    double t2 = duration-analysewin;
    if ( trigger.back() + 2.0*ISI1 > t2 )
      t2 = trigger.back() + 2.0*ISI1;
    for ( ; k<spikes[j].size() && spikes[j][k]<t2; k++ );
    for ( i=0, ISI2=0.0; 
	  k<spikes[j].size() && spikes[j][k]<duration; 
	  k++, i++ )
      ISI2 += (spikes[j][k]-spikes[j][k-1] - ISI2)/(i+1);
    if ( ISI1 <= 0.0 )
      continue;
    if ( ISI2 <= 0.0 || ISI2 > duration )
      ISI2 = ISI1;
    
    // phase response
    k = 0;
    for ( int t=0; t<trigger.size(); t++ ) {
      // estimate the isi at trigger time:
      double isi = (trigger[t]-t1) * (ISI2-ISI1)/(t2-t1) + ISI1;
      // search for preceeding spike:
      for ( ; k+1<spikes[j].size() && spikes[j][k+1]<trigger[t]+latency; k++ );
      if ( k+1>=spikes[j].size() )
	break;
      // calculate phases:
      prc.push( ( trigger[t] + latency - spikes[j][k] ) / isi, 
		( spikes[j][k] + isi - spikes[j][k+1] ) / isi );
    }
  }
}


void SysLatency::analyze( double duration, double skipwin, double analysewin,
			  double pduration, double coincwin,
			  double maxlatency, double latencystep,
			  EventList &spikes, const ArrayD &trigger,
			  MapD &coincidentspikes, MapD &prc,
			  double &spikewidth,
			  double &latency, double &latencysd,
			  int &maxcoincidence, double &coinclatency,
			  double &offset, double &slope, double &meanrate )
{
  if ( SpikeEvents[0] < 0 )
    return;

  // spikes:
  spikes.push( events( SpikeEvents[0] ), signalTime(), signalTime() + duration );

  // spike width:
  spikewidth += ( events( SpikeEvents[0] ).meanWidth() - spikewidth ) / spikes.size();

  // latency to first spike:
  latency = spikes.latency( 0.0, &latencysd );

  // search for best system latency:
  coincidentspikes.clear();
  coincidentspikes.reserve( (int)::rint( maxlatency / latencystep ) + 10 );
  coinclatency = 0.0;
  maxcoincidence = 0;
  for ( double latency=0.0; latency <= maxlatency; latency += latencystep ) {
    int coincidence = coincidentSpikes( coincwin, latency,
					0.5*(spikewidth+pduration),
					spikes, trigger );
    coincidentspikes.push( latency, double( coincidence ) );
    if ( coincidence > maxcoincidence ) {
      maxcoincidence = coincidence;
      coinclatency = latency;
    }
  }

  // PRC of best latency:
  phaseResponse( duration, skipwin, analysewin, coinclatency,
		 spikes, trigger, prc );

  // fit line:
  MapD prcdata;
  prcdata.reserve( prc.size() );
  for ( int k=0; k<prc.size(); k++ )
    if ( prc.x( k ) > 0.6 && prc.x( k ) < 0.9 )
      prcdata.push( prc.x( k ), prc.y( k ) );
  prcdata.lineFit( offset, slope );

  // mean firing rate:
  meanrate = spikes.rate( skipwin, duration );

}


addRePro( SysLatency, auditory );

}; /* namespace auditory */

#include "moc_syslatency.cc"
