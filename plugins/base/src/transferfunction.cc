/*
  base/transferfunction.cc
  Measures the transfer function with white-noise stimuli.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <relacs/base/transferfunction.h>
using namespace relacs;

namespace base {


TransferFunction::TransferFunction( void )
  : RePro( "TransferFunction", "TransferFunction", "base",
	   "Jan Benda", "1.0", "Dec 07, 2009" ),
    P( 2, this )
{
  // options:
  addLabel( "Stimulus" ).setStyle( OptWidget::Bold );
  addSelection( "outtrace", "Output trace", "V-1" );
  addNumber( "amplitude", "Amplitude", 1.0, 0.0, 100000.0, 1.0, "", "" );
  addNumber( "fmax", "Maximum frequency", 1000.0, 0.0, 100000.0, 100.0, "Hz", "Hz" );
  addNumber( "duration", "Width of analysis window", 1.0, 0.0, 100.0, 0.1, "s", "ms" );
  addNumber( "pause", "Length of pause inbetween successive stimuli", 1.0, 0.0, 100.0, 0.1, "s", "ms" );
  addInteger( "repeats", "Repeats", 100, 0, 10000, 1 );
  addLabel( "Analysis" ).setStyle( OptWidget::Bold );
  addSelection( "intrace", "Input trace", "V-1" );
  addSelection( "size", "Number of data points for FFT", "1024|64|128|256|512|1024|2048|4096|8192|16384|32768|65536|131072|262144|524288|1048576" );
  addBoolean( "overlap", "Overlap FFT windows", true );
  addSelection( "window", "FFT window function", "Hanning|Bartlett|Blackman|Blackman-Harris|Hamming|Hanning|Parzen|Square|Welch" );

  // plot:
  P.lock();
  P.setCommonXRange( 0, 1 );
  P[0].setLMarg( 8 );
  P[0].noXTics();
  P[0].setXRange( 0.0, 1000.0 );
  P[0].setYLabel( "Gain" );
  P[0].setYLabelPos( 2.0, Plot::FirstMargin,
		     0.5, Plot::Graph, Plot::Center, -90.0 );
  P[0].setYRange( 0.0, Plot::AutoScale );
  P[1].setLMarg( 8 );
  P[1].setXLabel( "Frequency [Hz]" );
  P[1].setXRange( 0.0, 1000.0 );
  P[1].setYLabel( "Phase" );
  P[1].setYLabelPos( 2.0, Plot::FirstMargin,
		     0.5, Plot::Graph, Plot::Center, -90.0 );
  P[1].setYRange( Plot::AutoScale, Plot::AutoScale );
  P.unlock();
}


void TransferFunction::config( void )
{
  setText( "intrace", traceNames() );
  setToDefault( "intrace" );
  setText( "outtrace", outTraceNames() );
  setToDefault( "outtrace" );
}


void TransferFunction::notify( void )
{
  int outtrace = index( "outtrace" );
  OutUnit = outTrace( outtrace ).unit();
  setUnit( "amplitude", OutUnit );

  int intrace = index( "intrace" );
  InUnit = trace( intrace ).unit();
}


int TransferFunction::main( void )
{
  int outtrace = index( "outtrace" );
  double amplitude = number( "amplitude" );
  double fmax = number( "fmax" );
  double duration = number( "duration" );
  double pause = number( "pause" );
  int repeats = integer( "repeats" );
  int intrace = traceIndex( text( "intrace", 0 ) );
  SpecSize = integer( "size" );
  Overlap = boolean( "overlap" );
  int win = index( "window" );
  switch ( win ) {
  case 0: Window = bartlett; break;
  case 1: Window = blackman; break;
  case 2: Window = blackmanHarris; break;
  case 3: Window = hamming; break;
  case 4: Window = hanning; break;
  case 5: Window = parzen; break;
  case 6: Window = square; break;
  case 7: Window = welch; break;
  default: Window = hanning;
  }

  double samplerate = trace( intrace ).sampleRate();

  MeanGain.clear();
  SquareGain.clear();
  StdevGain.clear();
  MeanPhase.clear();
  SquarePhase.clear();
  StdevPhase.clear();

  // don't print repro message:
  noMessage();

  // plot trace:
  plotToggle( true, true, duration, 0.0 );

  // plot:
  P.lock();
  P[0].setYLabel( "Gain [" + OutUnit + "/" + InUnit + "]" );
  P.unlock();

  // signal:
  OutData signal( duration, 1.0/samplerate );
  signal.noiseWave( fmax, duration, amplitude );
  signal.setIdent( "WhiteNoise, fmax=" + Str( fmax ) + "Hz" );
  signal.back() = 0.0;
  signal.setTrace( outtrace );

  // write stimulus:
  sleep( pause );
  timeStamp();
  for ( int count=0;
	( repeats <= 0 || count < repeats ) && softStop() == 0;
	count++ ) {

    Str s = "Amplitude <b>" + Str( amplitude ) + " " + outTrace( outtrace ).unit() +"</b>";
    s += ",  Frequency <b>" + Str( fmax, "%.0f" ) + " Hz</b>";
    s += ",  Loop <b>" + Str( count+1 ) + "</b>";
    message( s );

    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      return Failed;
    }

    sleep( duration + 0.1*pause );
    if ( interrupt() ) {
      if ( count > 0 )
	break;
      else
	return Aborted;
    }

    analyze( signal, trace( intrace ), duration, count );

    // plot gain:
    P.lock();
    P[0].clear();
    if ( ! P[0].zoomedXRange() && ! P[1].zoomedXRange() )
      P[0].setXRange( 0.0, fmax < MeanGain.rangeBack() ? fmax : MeanGain.rangeBack() );
    P[0].plotVLine( fmax, Plot::White, 2 );
    P[0].plot( MeanGain, 1.0, Plot::Red, 3, Plot::Solid );
    P[1].clear();
    if ( ! P[0].zoomedXRange() && ! P[1].zoomedXRange() )
      P[1].setXRange( 0.0, fmax < MeanGain.rangeBack() ? fmax : MeanGain.rangeBack() );
    P[1].plotVLine( fmax, Plot::White, 2 );
    P[1].plot( MeanPhase, 1.0, Plot::Blue, 3, Plot::Solid );
    P.unlock();
    P.draw();

    sleepOn( duration+pause );
    if ( interrupt() ) {
      if ( count > 0 )
	break;
      else
	return Aborted;
    }

  }

  return Completed;
}



void TransferFunction::analyze( const OutData &signal, const InData &data,
				double duration, int count )
{
  SampleDataD x( signal );
  
  // de-mean:
  SampleDataD y( 0.0, duration, data.stepsize() );
  data.copy( data.signalTime(), y );
  y -= mean( y );

  // transfer fucntion:
  SampleDataD trans( SpecSize );
  transfer( x, y, trans, Overlap, Window );

  // gain and phase:
  SampleDataD gain( trans.size()/2 );
  SampleDataD phase( trans.size()/2 );
  hcMagnitude( trans, gain );
  hcPhase( trans, phase );

  // average:
  if ( count <= 0 ) {
    MeanGain = gain;
    SquareGain = gain*gain;
    StdevGain = gain;
    StdevGain = 0.0;
    MeanPhase = phase;
    SquarePhase = phase*phase;
    StdevPhase = phase;
    StdevPhase = 0.0;
  }
  else {
    for ( int k=0; k<gain.size(); k++ ) {
      double g = gain[k];
      MeanGain[k] += (g - MeanGain[k])/(count+1);
      SquareGain[k] += (g*g - SquareGain[k])/(count+1);
      StdevGain[k] = sqrt( SquareGain[k] - MeanGain[k]*MeanGain[k] );
      double p = phase[k];
      MeanPhase[k] += (p - MeanPhase[k])/(count+1);
      SquarePhase[k] += (p*p - SquarePhase[k])/(count+1);
      StdevPhase[k] = sqrt( SquarePhase[k] - MeanPhase[k]*MeanPhase[k] );
    }
  }
}


addRePro( TransferFunction );

}; /* namespace base */

#include "moc_transferfunction.cc"
