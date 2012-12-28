/*
  patchclamp/findrate.cc
  Finds the DC current need to evoke a given target firing rate.

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

#include <relacs/tablekey.h>
#include <relacs/map.h>
#include <relacs/patchclamp/findrate.h>
using namespace relacs;

namespace patchclamp {


FindRate::FindRate( void )
  : RePro( "FindRate", "patchclamp", "Jan Benda", "1.0", "Nov 25, 2010" ),
    IUnit( "nA" )
{
  // add some options:
  addNumber( "rate", "Target firing rate", 100.0, 0.0, 1000.0, 10.0, "Hz" );
  addNumber( "ratetol", "Tolerance for target firing rate", 5.0, 0.0, 1000.0, 1.0, "Hz" );
  addSelection( "startamplitudesrc", "Set initial dc-current to", "custom|DC|threshold|previous" );
  addNumber( "startamplitude", "Initial amplitude of dc-current", 0.0, -1000.0, 1000.0, 0.01 ).setActivation( "startamplitudesrc", "custom" );
  addNumber( "amplitudestep", "Initial size of dc-current steps used for searching target rate", 8.0, 0.0, 100.0, 1.0, IUnit );
  addNumber( "duration", "Duration of dc-current stimulus", 0.5, 0.0, 1000.0, 0.01, "seconds", "ms" );
  addNumber( "skipwin", "Initial portion of stimulus not used for analysis", 0.1, 0.0, 100.0, 0.01, "seconds", "ms" );

  PrevDCAmplitude = 0.0;
    
  P.lock();
  P.setLMarg( 7.0 );
  P.setRMarg( 1.5 );
  P.setTMarg( 3.0 );
  P.setBMarg( 5.0 );
  P.setYLabel( "Firing rate [Hz]" );
  P.unlock();
  setWidget( &P );
}


void FindRate::config( void )
{
  if ( CurrentOutput[0] >= 0 ) {
    IUnit = outTrace( CurrentOutput[0] ).unit();
    setUnit( "startamplitude", IUnit );
    setUnit( "amplitudestep", IUnit );
  }
}


int FindRate::main( void )
{
  // get options:
  int startamplitudesrc = index( "startamplitudesrc" );
  double startamplitude = number( "startamplitude" );
  double targetrate = number( "rate" );
  double ratetolerance = number( "ratetol" );
  double amplitudestep = number( "amplitudestep" );
  double duration = number( "duration" );
  double skipwin = number( "skipwin" );

  double orgdcamplitude = stimulusData().number( outTraceName( 0 ) );
  double dcamplitude = startamplitude;
  if ( startamplitudesrc == 1 ) // dc
    dcamplitude = orgdcamplitude;
  else if ( startamplitudesrc == 2 ) {  // thresh
    dcamplitude = metaData( "Cell" ).number( "ithreshss" );
    if ( dcamplitude == 0.0 )
      dcamplitude = metaData( "Cell" ).number( "ithreshon" );
  }
  else if ( startamplitudesrc == 3 )  // prev
    dcamplitude = PrevDCAmplitude;

  if ( SpikeTrace[ 0 ] < 0 || SpikeEvents[ 0 ] < 0 ) {
    warning( "Invalid input voltage trace or missing input spikes!" );
    return Failed;
  }
  if ( duration < 5.0/targetrate ) {
    warning( "Duration too small for requested targetrate!" );
    return Failed;
  }
  if ( skipwin > 0.5*duration ) {
    warning( "Skipwin too large compared to duration!" );
    return Failed;
  }
  if ( ratetolerance > 0.3*targetrate ) {
    warning( "Ratetolerance certainly too high!" );
    return Failed;
  }
  double silentrate = 1.0/(duration-skipwin);
  double minamplitudestep = 0.001;

  OutData orgdcsignal( orgdcamplitude, IUnit );
  orgdcsignal.setTrace( CurrentOutput[0] );
  orgdcsignal.setIdent( "DC=" + Str( orgdcamplitude ) + IUnit );
    
  // plot trace:
  tracePlotSignal( duration );
    
  P.lock();
  P.clear();
  P.setTitle( "Search target firing rate " + Str( targetrate ) + " Hz" );
  P.setXLabel( "DC current [" + IUnit + "]" );
  P.draw();
  P.unlock();

  // data:
  double meanrate = 0.0;
  MapD rates;
  rates.reserve( 20 );
    
  double minampl = dcamplitude;
  double maxampl = dcamplitude;
  double maxf = ::ceil((targetrate+5.0)/10.0)*10.0;

  // search dc-curent amplitude:
  while ( true ) {

    // stimulus:
    OutData signal( dcamplitude, IUnit );
    signal.setTrace( CurrentOutput[0] );
    signal.setIdent( "DC=" + Str( dcamplitude ) + IUnit );
	
    // message:
    Str s = "<b>Search</b> rate <b>" + Str( targetrate ) + " Hz</b>";
    s += ":  DC = <b>" + Str( dcamplitude, 0, 3, 'f' ) + " " + IUnit + "</b>";
    message( s );

    // output:
    directWrite( signal );
    if ( ! signal.success() )
      break;
    sleep( duration );
    if ( interrupt() ) {
      directWrite( orgdcsignal );
      return Aborted;
    }

    // analyze:
    meanrate = events( SpikeEvents[0] ).rate( signalTime() + skipwin,
					      signalTime() + duration );

    // message:
    s = "<b>Measured</b> rate <b>" + Str( meanrate ) + " Hz</b>";
    s += ":  DC = <b>" + Str( dcamplitude, 0, 3, 'f' ) + " " + IUnit + "</b>";
    message( s );
	
    // plot:
    {
      P.lock();
      // firing rate versus stimulus offset:
      P.clear();
      double mina = minampl;
      double maxa = maxampl;
      if ( maxa - mina < amplitudestep ) {
	mina -= 0.5*amplitudestep;
	maxa += 0.5*amplitudestep;
      }
      if ( ! P.zoomedXRange() )
	P.setXRange( mina, maxa );
      if ( meanrate+10.0 > maxf )
	maxf = ::ceil((meanrate+10.0)/10.0)*10.0;
      if ( ! P.zoomedYRange() )
	P.setYRange( 0.0, maxf );
      P.plotHLine( targetrate, Plot::White, 2 );
      P.plot( rates, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 10, Plot::Red, Plot::Red );
      MapD cr;
      cr.push( dcamplitude, meanrate );
      P.plot( cr, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 10, Plot::Yellow, Plot::Yellow );
      P.draw();
      P.unlock();
    }

    int rinx = 0;
    if ( signal.success() )
      rinx = rates.insert( dcamplitude, meanrate );

    if ( softStop() > 0 )
      return Aborted;

    // new offset:
    if ( signal.success() && 
	 fabs( meanrate - targetrate ) < ratetolerance ) {
      // ready:
      break;
    }
    else {
      // cell lost?
      if ( signal.success() && meanrate < silentrate &&
	   rinx > 0 && rates.y( rinx-1 ) > 2.0*silentrate ) {
	if ( ( rinx < rates.size()-1 && 
	       rates.y( rinx+1 ) > 2.0*silentrate ) ||
	     rates.y( rinx-1 ) > silentrate + 0.3*(targetrate - silentrate ) ) {
	  info( "Cell probably lost!" );
	  directWrite( orgdcsignal );
	  return Failed;
	}
      }
      if ( ( signal.success() && meanrate < targetrate ) || signal.underflow() ) {
	double maxr = 0.0;
	if ( rates.y().maxIndex( maxr ) < rinx && maxr > 2.0*silentrate ) {
	  // depolarization block reached:
	  dcamplitude -= amplitudestep;
	  if ( dcamplitude < minampl )
	    minampl = dcamplitude;
	}
	else {
	  // rate below target rate:
	  if ( dcamplitude < maxampl )
	    amplitudestep *= 0.5;
	  dcamplitude += amplitudestep;
	  if ( dcamplitude > maxampl )
	    maxampl = dcamplitude;
	}
      }
      else if ( ( signal.success() && meanrate > targetrate ) || signal.overflow() ) {
	// overflow:
	if ( rates.size() > 0 && dcamplitude > rates.x().back() &&
	     amplitudestep < minamplitudestep ) {
	  info( "dcamplitude needed for targetrate is too high!" );
	  directWrite( orgdcsignal );
	  return Failed;
	}
	// rate above target rate:
	if ( dcamplitude > minampl )
	  amplitudestep *= 0.5;
	dcamplitude -= amplitudestep;
	if ( dcamplitude < minampl )
	  minampl = dcamplitude;
      }
      else if ( signal.failed() ) {
	warning( "Output of stimulus failed!<br>Signal error: <b>" +
		 signal.errorText() + "</b><br>Exit now!" );
	directWrite( orgdcsignal );
	return Failed;
      }
      else {
	warning( "Could not establish firing rate!" );
	directWrite( orgdcsignal );
	return Failed;
      }
    }

  }

  // remember baseline current:
  PrevDCAmplitude = dcamplitude;

  // save data:
  saveData( rates );

  return Completed;
}


void FindRate::saveData( const MapD &rates )
{
  ofstream df( addPath( "findrate-data.dat" ).c_str(),
	       ofstream::out | ofstream::app );

  Options header;
  header.addInteger( "index", completeRuns() );
  header.addInteger( "ReProIndex", reproCount() );
  header.addNumber( "ReProTime", reproStartTime(), "s", "%0.3f" );

  header.save( df, "# " );
  df << "# status:\n";
  stimulusData().save( df, "#   " );
  df << "# settings:\n";
  settings().save( df, "#   " );
  df << '\n';

  TableKey datakey;
  datakey.addNumber( "I", IUnit, "%8.3f" );
  datakey.addNumber( "r", "Hz", "%6.1f" );
  datakey.saveKey( df );

  for ( int k=0; k<rates.size(); k++ ) {
    datakey.save( df, rates.x( k ), 0 );
    datakey.save( df, rates.y( k ) );
    df << '\n';
  }
  
  df << "\n\n";
}


addRePro( FindRate, patchclamp );

}; /* namespace patchclamp */

#include "moc_findrate.cc"
