/*
  auditoryprojects/fiphaselocking.cc
  Phase locking at different carrier frequencies and firing rates.

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

#include <relacs/rangeloop.h>
#include <relacs/options.h>
#include <relacs/auditory/session.h>
#include <relacs/outdata.h>
#include <relacs/tablekey.h>
#include <relacs/auditoryprojects/fiphaselocking.h>
using namespace relacs;

namespace auditoryprojects {



FIPhaseLocking::FIPhaseLocking( void )
  : RePro( "FIPhaseLocking", "FIPhaseLocking", "auditoryprojects",
	   "Karin Fisch", "1.0", "Feb 01, 2010" ),
    P( 2, 2, true, this )
{
  //parameters
  // double  CarrierFrequency = 5000.0;
  // bool UseBestFreq = false;
  double Duration = 0.5;
  double Pause = 1.0;


  // add some options:
  //addNumber( "carrierfreq", "Frequency of carrier", CarrierFrequency, 0.0, 40000.0, 2000.0, "Hz", "kHz" ).setActivation( "usebestfreq", "false" );
  // addBoolean( "usebestfreq", "Use the cell's best frequency", UseBestFreq );
  addNumber( "ramp", "Ramp of stimulus", 0.002, 0.0, 10.0, 0.001, "seconds", "ms" );
  addNumber( "duration", "Duration of stimulus", Duration, 0.0, 10.0, 0.05, "seconds", "ms" );
  addNumber( "pause", "Pause", Pause, 0.0, 10.0, 0.05, "seconds", "ms" );
  // addNumber( "skipwin", "Offset for measuring mean firing rate", 0.5, 0.0, 1000.0, 10.0, "seconds", "ms" );
  // addInteger( "addcf", "Number of carrier frequencies above and below of the best frequency", 1 );
  //addNumber( "cfstep", "Stepsize of carrier frequencies", 1000.0, 0.0, 10000000.0, 100.0, "Hz", "kHz" );
  addInteger( "addintensities", "Number of sound intensities above and below of the intensity resulting in 100Hz", 2 );
  addNumber( "intensitystep", "Stepsize of sound intensities", 2.0, 0.0, 50.0, 1.0, "dB SPL", "dB SPL" );
  addNumber( "firingrate", "Firing rate", 100.0, 10.0, 500.0, 10.0, "Hz", "Hz" );
  addInteger( "repeat", "repeat", 100 );
  addInteger( "cfs", "Number of carrier frequencies", 3 );
  addSelection( "side", "Speaker", "left|right|best" );
  addSelection( "intshuffle", "Order of intensities", "Random|" + RangeLoop::sequenceStrings() );



  //plot
  P.setDataMutex( mutex() );
  P.lock();
  P[0].setLMarg( 5.0 );
  P[0].setRMarg( 1.0 );
  P[0].setXLabel( "Time [ms]" );
  P[0].setYLabel( "Trial" );
  P[1].setLMarg( 4.0 );
  P[1].setRMarg( 2.0 );
  P[1].setXLabel( "Interspike interval [ms]" );
  P[1].setYLabel( "Count" );
  P.unlock();
}


void FIPhaseLocking::saveSpikes( void )
{
  // create file:
  ofstream df( addPath( "fiphaselocking-spikes.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  settings().save( df, "# " );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.saveKey( df, true, false );
  df << '\n';

  double cf = -1.0;

  // write data:
  int n = 0;
  for ( unsigned int k=0; k<Results.size(); k++ ) {

    if( Results[k].CarrierFrequency != cf ){
      cf = Results[k].CarrierFrequency;
      df << "\n# carrier frequency: " << Results[k].CarrierFrequency << "Hz\n\n";
    }

    df << "#         index0: " << n++ << '\n';
    df << "#      intensity: " << Results[k].Intensity << "dB SPL\n";
    // df << "# carrier frequency: " << Results[k].CarrierFrequency << "Hz\n";
    Results[k].Spikes.saveText( df, 1000.0, 8, 2, 'f', 1, "-0" );
    if( Results[k].Spikes.empty() ) {
      df << "-0\n";
    }
    df << '\n';
  }
  df << '\n';
}


void FIPhaseLocking::plot( int index )
{

  //const Data &data = Results[index];

  P.lock();

  // spikes
  P[0].clear();
  P[0].plotVLine( 0.0, Plot::White, 2 );
  P[0].plotVLine( 1000.0*duration, Plot::White, 2 );
  int maxspikes = 20;
  int j = 0;
  double delta = 1.0/maxspikes;
  int offs = (int)Results[index].Spikes.size() > maxspikes ? Results[index].Spikes.size() - maxspikes : 0;
  for ( int i=offs; i<Results[index].Spikes.size(); i++ ) {
    j++;
    P[0].plot( Results[index].Spikes[i], 0, 0.0, 1000.0,
	       0.0 + delta*(j-0.9), Plot::Graph, 2, Plot::StrokeUp,
	       delta*0.8, Plot::Graph, Plot::Red, Plot::Red );
  }

  //isih
  double skiponset = 0.4 * duration;
  double ivsd = 0.0;
  // Mean of the ISIH
  double iv = Results[index].Spikes.interval( skiponset, duration, ivsd );
  // double stepsize = 0.001;
  int isihn = 125;
  if(1.0/iv>160.0)
	isihn = 175;
  else if(1.0/iv>120.0)
	isihn = 150;
  else if(1.0/iv>80.0)
	isihn = 125;
  else if(1.0/iv>40.0)
	isihn = 100;
  else
	isihn = 75;

  double stepsize = 3.0*iv/double( isihn );
  SampleDataD hist(isihn, 0.0, stepsize);
  Results[index].Spikes.intervalHistogram( skiponset, duration, hist );
  
  double x1 = ((iv - 4.5*ivsd) < 0) ? 0 : (iv - 3.5*ivsd);
  double x2 = iv + 5.0*ivsd;
  P[1].setXRange(1000.0*x1,1000.0*x2);
  P[1].clear();
  P[1].plot(hist, 1000.0, Plot::Transparent, 0, Plot::Solid, Plot::Box, 0, Plot::DarkOrange, Plot::Orange);
  P.unlock();
  P.draw();
}


int FIPhaseLocking::main( void )
{
  // get options:
  double ramp = number( "ramp" );
  //double CarrierFrequency = number( "carrierfreq" );
  //bool UseBestFreq = boolean( "usebestfreq" );
  int side = index( "side" );
  duration = number( "duration" );
  double pause = number( "pause" );
  //int addcf = integer( "addcf" );
  //double cfstep = number( "cfstep" );
  int addintensities = integer( "addintensities" );
  double intensitystep = number( "intensitystep" );
  //double skipwin =  number( "skipwin" ); // ...
  int repeat = integer( "repeat" );
  int cfs = integer( "cfs" );
  double firingrate = number( "firingrate" );
  RangeLoop::Sequence intshuffle = RangeLoop::Sequence( index( "intshuffle" ) );
  

  if ( side > 1 )
    side = metaData( "Cell" ).index( "best side" );

  // plot trace:
  plotToggle( true, true, duration, 0.0 );
  

  //int arraylength = (2*(int)(addcf/(cfstep/1000.0))+1)*(2*(int)(addintensities/intensitystep)+1);
  int arraylength = 0;

  // cout << arraylength << endl;

  Results.clear();
  // Results.resize( arraylength+1 );

  // double cfs[arraylength];
  // double ints[arraylength];

  double Carrierfrequency = 5000.0;

  // int index = 0;
  for ( int i=0; i<cfs; i++ ) {

     auditory::Session *as = dynamic_cast<auditory::Session*>( control( "Session" ) );
     MapD ssficurve = as->ssFICurve( i, side, Carrierfrequency );

     if ( ssficurve.empty() )
       break;

     //MapD ssficurve = as->ssFICurve( side, CarrierFrequency+i*cfstep );
     double intensity = 0.0;
     double intensity2 = 0.0;
     double rate = 0.0;
     double rate2 = 0.0;
     for ( int k=ssficurve.size()-1; k>=0; k-- ) {
    
       if(ssficurve.y( k )<firingrate) {
	 rate2 = ssficurve.y( k );
	 intensity2 = ssficurve.x( k );
	 break;
       }

       rate = ssficurve.y( k );
       intensity = ssficurve.x( k );
    
     }

     intensity = intensity2 + ((intensity-intensity2)/(rate-rate2)) * (firingrate-rate2);

     for(int j=-addintensities; j<=addintensities; j++ ) {
       // cfs[index] = CarrierFrequency + i*cfstep;
       //ints[index] = intensity + j*intensitystep;
       Results.resize( arraylength+1 );
       Results[arraylength].Intensity =  intensity + j*intensitystep;
       Results[arraylength].CarrierFrequency = Carrierfrequency;
       // cout << Results[arraylength].Intensity << "\t" <<  Results[arraylength].CarrierFrequency << endl;
       arraylength++;
     }
  }

  if ( arraylength <= 0 ) {
    warning( "No f-I curves found!" );
    double intensity = 50.0;
    Carrierfrequency = 5000.0;
    for ( int i=0; i<cfs; i++ ) {

      for(int j=-addintensities; j<=addintensities; j++ ) {
	// cfs[index] = CarrierFrequency + i*cfstep;
	//ints[index] = intensity + j*intensitystep;
	Results.resize( arraylength+1 );
	Results[arraylength].Intensity =  intensity + j*intensitystep;
	Results[arraylength].CarrierFrequency = Carrierfrequency+i*1000.0;
	//cout << Results[arraylength].Intensity << "\t" <<  Results[arraylength].CarrierFrequency << endl;
	arraylength++;
      }
    }
    //return Failed;
  }


  // plot:
  P.lock();
  P[0].clear();
  P[0].setXRange( -20.0, 1000.0*duration+20.0 );
  //P[0].setYFallBackRange( 0.0, 100.0 );
  P[0].setYRange( 0.0, 20.0 );
  P[1].clear();
  //P[1].setXFallBackRange( Results[0].Intensity, Results[arraylength-1].Intensity );
  P[1].setXRange( 0.0, Plot::AutoScale );
  //P[1].setYFallBackRange( 0.0, 100.0 );
  P[1].setYRange( 0.0, Plot::AutoScale );
  P.unlock();
  


  int Intval;


  //  EventList spikes;

  RangeLoop range;
  range.set( 0.0, double(arraylength-1), 1.0, repeat );
  range.setSequence( intshuffle );
  for ( range.reset(); !range; ++range ) {
    Intval = (int) *range;
    // double decibel = ints[Intval];
    // double frequency = cfs[Intval];
    double decibel = Results[Intval].Intensity;
    double frequency = Results[Intval].CarrierFrequency;
    message( "Carrier frequency = " + Str( 0.001*frequency ) + " kHz, intensity = " + Str( decibel ) + "dB SPL" );

    OutData signal;
    signal.setTrace( Speaker[ side ] );
    applyOutTrace( signal );  // to get maximum sampling rate!
    signal.sineWave( frequency, duration, 1.0, ramp );
    signal.back() = 0;
    signal.setIntensity( decibel + 3.0103 );

    write( signal );
    if ( ! signal.success() ) {
      warning( "Output of signal failed!<br>Signal error <b>" +
	    signal.errorText() + "</b>.<br>Exit now!" );
      return Failed;
    }
    sleep( duration + pause );
    if ( interrupt() ) {
      saveSpikes();
      return Completed;
    }

    if ( SpikeEvents[0] < 0 )
      return Failed;

    const EventData &spikes = events( SpikeEvents[0] );

    // spikes:
    //cerr << spikes.size() << " " << spikes.signalTime() << " " << duration << " " << trace( 0 ).currentTime() << '\n';
    Results[Intval].Spikes.push( spikes, spikes.signalTime(),
				 spikes.signalTime() + duration );


    plot(Intval);

    if ( softStop() > 1 ) {
      saveSpikes();
      return Completed;
    }

  }


  saveSpikes();
  

  return Completed;
}


addRePro( FIPhaseLocking );

}; /* namespace auditoryprojects */

#include "moc_fiphaselocking.cc"
