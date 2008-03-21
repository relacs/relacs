/*
  oneclick.cc
  

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <relacs/map.h>
#include <relacs/str.h>
#include <relacs/tablekey.h>
#include <relacs/auditorysession.h>
#include <relacs/oneclick.h>


OneClick::OneClick( void )
  : RePro( "OneClick", "OneClick", "Alexander Wolf", "0.2", "Jan 10, 2008" ),
    EPhysTraces(),
    AuditoryTraces(),
    P( 1, 1, true, this)    
{
  // parameter:
  MinIntensity = 30.0;
  MaxIntensity = 100.0;
  IntensityStep = 5.0;
  IntRepeat = 15;

  Duration = 0.002;
  Latency = 0.002;
  Pause = 0.4;
  Side = 0;


  // add some parameter as options:
  addLabel( "Intensities" ).setStyle( OptWidget::Bold );
  addNumber( "intmin", "Minimum Click intensity", MinIntensity, 0.0, 200.0, 5.0, "dB SPL" );
  addNumber( "intmax", "Maximum Click intensity", MaxIntensity, 0.0, 200.0, 5.0, "dB SPL" );
  addNumber( "intstep", "Click intensity step", IntensityStep, 0.0, 200.0, 1.0, "dB SPL" );
  addInteger( "repeat", "Number of repetitions of the whole f-I curve measurement", IntRepeat, 1, 1000, 1 );

  addLabel( "Waveform" ).setStyle( OptWidget::Bold );
  addNumber( "duration", "Duration of stimulus", Duration, 0.0, 0.1, 0.000001, "seconds", "microsec" );
  addNumber( "latency", "Latency after stimulus", Latency, 0.0, 10.0, 0.05, "seconds", "ms" );
  addNumber( "pause", "Pause", Pause, 0.0, 10.0, 0.05, "seconds", "ms" );
  addText( "side", "Speaker", "left|right|best" ).setStyle( Parameter::SelectText );

  // variables:
  Intensity = 0.0;

  // plot:
  P[0].setXLabel( "Amplitude [dB SPL]" );
  P[0].setYLabel( "Spike Probability [%]" );
}


OneClick::~OneClick( void )
{
}


int OneClick::main( void )
{
  if ( SpikeEvents[0] < 0 || SpikeTrace[0] < 0 ) {
    warning( "No spike trace!" );
    return Failed;
  }

  // get options:

  MinIntensity = number( "intmin" );
  MaxIntensity = number( "intmax" );
  IntensityStep = number( "intstep" );
  IntRepeat = integer( "repeat" );

  Duration = number( "duration" );
  Latency = number( "latency" );
  Pause = number( "pause" );
  Side = index( "side" );

  // plot trace:
  plotToggle( true, true, Duration+Pause, 0 );

  // plot:
  P[0].clear();
  P[0].setXRange( MinIntensity-IntensityStep, MaxIntensity+IntensityStep);
  P[0].setYRange( 0.0, 1.1 );

  // stimulus:
  OutData signal( Duration, 10000.0 );
  //		  OutData::traceRate( Speaker[ Side ] ) );
  signal.setTrace( Speaker[ Side ] );
  int hw = signal.size()/2;
  int j=0;
  for ( j=0; j<hw; j++ )
    signal[j] = double(j) / hw;
  for ( ; j<signal.size(); j++ )
    signal[j] = 1.0 - double( j - hw  ) / hw;
  signal.back() = 0;
  signal.setDelay( 0 );
  //  signal.saveText( "signal.dat" );

  /*
    if ( signal.underflow() ) {
      cerr << "OneClick::start() -> attenuator underflow: " << signal.intensity( 0 ) << endl;
      IntensityRange.setSkipBelow( k );
      ++IntensityRange;
      Intensity = *IntensityRange;
    }
    else if ( signal.overflow() ) {
      cerr << "OneClick::start() -> attenuator overflow: " << signal.intensity( 0 ) << endl;
      IntensityRange.setSkipAbove( k );
      ++IntensityRange;
      Intensity = *IntensityRange;
    }

  }
  */

  for ( Intensity = MinIntensity; Intensity <= MaxIntensity; Intensity += IntensityStep ) {

    signal.setIntensity( Intensity );

    for ( int count = 0; count < IntRepeat; count++ ) {

      Str s = "Intensity <b>" + Str( Intensity ) + " dB SPL</b>";
      s += ",  Loop <b>" + Str( count+1 ) + "</b>";
      message( s );

      write( signal );
      sleep( Pause );
      if ( interrupt() ) {
	writeZero( Speaker[ Side ] );
	return Aborted;
      }

      analyze();
      plot();
    }
  }

  writeZero( Speaker[ Side ] );
  return Completed;
}


/*
void OneClick::saveSpikes( const string &file )
{
  // create file:
  ofstream df( addPath( file ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  Header.save( df, "# " );
  Options::save( df, "#   ", -1, 1 );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%7.1f" );
  key.saveKey( df, true, false );
  df << '\n';

  // write data:
  int n = 0;
  for ( unsigned int k=IntensityRange.next( 0 ); 
	k<Results.size(); 
	k=IntensityRange.next( ++k ) ) {
    df << "#         index0: " << n++ << '\n';
    df << "#      intensity: " << Results[k].Intensity << "dB SPL\n";
    df << "# true intensity: " << Results[k].TrueIntensity << "dB SPL\n";
    Results[k].Spikes.saveText( df, 1000.0, 7, 1, 'f', "-0", 1 );
    df << '\n';
  }
  df << '\n';
}
*/


void OneClick::plot( void )
{
  /*
  P.lock();

  FIData &fid = Results[IntensityRange.pos()];

  // rate and spikes:
  P[0].clear();
  P[0].setYRange( 0.0, fid.MaxPlotRate );
  P[0].plotVLine( 0.0, Plot::White, 2 );
  P[0].plotVLine( 1000.0*Duration, Plot::White, 2 );
  int maxspikes = (int)rint( 20.0 / SpikeTraces );
  if ( maxspikes < 4 )
    maxspikes = 4;
  int j = 0;
  int r = IntensityRange.maxCount();
  double delta = r > 0 && r < maxspikes ? 1.0/r : 1.0/maxspikes;
  int offs = (int)fid.Spikes.size() > maxspikes ? fid.Spikes.size() - maxspikes : 0;
  for ( int i=offs; i<fid.Spikes.size(); i++ ) {
    j++;
    P[0].plot( fid.Spikes[i], 0, 0.0, 1000.0,
	       1.0 - delta*(j-0.1), Plot::Graph, 2, Plot::StrokeUp,
	       delta*0.8, Plot::Graph, Plot::Red, Plot::Red );
  }
  P[0].plot( fid.Rate, 1000.0, Plot::Yellow, 2, Plot::Solid );

  // f-I curve:
  P[1].clear();
  P[1].setYRange( 0.0, MaxPlotRate );
  DMap om, sm, cm, bm;
  for ( unsigned int k=0; k<Results.size(); k++ ) {
    if ( Results[k].Spikes.size() > 0 ) {
      bm.push( IntensityRange.value( k ), Results[k].PreRate );
      om.push( IntensityRange.value( k ), Results[k].OnRate );
      sm.push( IntensityRange.value( k ), Results[k].SSRate );
      cm.push( IntensityRange.value( k ), Results[k].MeanRate );
    }
  }
  P[1].plot( bm, 1.0, Plot::Cyan, 3, Plot::Solid, Plot::Circle, 6, Plot::Cyan, Plot::Cyan );
  P[1].plot( om, 1.0, Plot::Green, 3, Plot::Solid, Plot::Circle, 6, Plot::Green, Plot::Green );
  P[1].plot( sm, 1.0, Plot::Red, 3, Plot::Solid, Plot::Circle, 6, Plot::Red, Plot::Red );
  P[1].plot( cm, 1.0, Plot::Orange, 3, Plot::Solid, Plot::Circle, 6, Plot::Orange, Plot::Orange );
  int c = IntensityRange.pos();
  DMap am;
  am.push( Results[c].Intensity, Results[c].PreRate );
  am.push( Results[c].Intensity, Results[c].OnRate );
  am.push( Results[c].Intensity, Results[c].SSRate );
  am.push( Results[c].Intensity, Results[c].MeanRate );
  P[1].plot( am, 1.0, Plot::Transparent, 3, Plot::Solid, Plot::Circle, 8, Plot::Yellow, Plot::Transparent );

  P.unlock();

  P.draw();
  */
}


void OneClick::analyze( void )
{
}


addRePro( OneClick );
