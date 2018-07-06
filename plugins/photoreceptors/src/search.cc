/*
  photoreceptors/search.cc
  Search repro giving current and light pulses on two analog outs.

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

#include <relacs/photoreceptors/search.h>
using namespace relacs;

namespace photoreceptors {


  Search::Search( void )
    : RePro( "Search", "photoreceptors", "Jan Grewe", "1.0", "May 06, 2015" )
  {
    this->populateOptions();
    this->createLayout();
    this->Start = false;
  }


  void Search::populateOptions() {
    addNumber( "duration", "Duration", 1.0, 0.0, 10.0, 0.1, "s", "s", "%.2f" ).setFlags( 1 );
    addBoolean( "endless", "Endless repetition", true).setFlags( 1 );
    addNumber( "repeats", "Number of repeats", 10, 1, 100).setFlags( 1 );
    addBoolean( "savetrace", "Save the recorded data", false).setFlags( 1 );
    newSection( "LED" );
    addNumber( "led_intensity", "Light intensity", 1.0, 0.0, 10.0, 0.05, "V" ).setFlags( 1 );
    addNumber( "led_duration", "Light pulse duration", 0.25, 0, 1.0, 0.01, "s" ).setFlags( 1 );
    addNumber( "led_onset", "Start time of light pulse", 0.5, 0.0, 1.0, 0.01, "s" ).setFlags( 1 );
    newSection( "Current" );
    addNumber( "current_intensity", "Current", -0.25, -10.0, 0.0, 0.01, "nA" ).setFlags( 1 );
    addNumber( "current_duration", "Current pulse duration", 0.25, 0.0, 1.0, 0.01, "s" ).setFlags( 1 );
    addNumber( "current_onset", "Start time of current pulse", 0.0, 0.0, 1.0, 0.01, "s" ).setFlags( 1 );
    
    setConfigSelectMask( 1+8 );
    setDialogSelectMask( 1 ); 
  }


  void Search::createLayout() {
    QVBoxLayout *vb = new QVBoxLayout;
    setLayout( vb );

    QHBoxLayout *bb = new QHBoxLayout;
    bb->setSpacing( 4 );
    vb->addLayout( bb );

    // display values:
    jw.assign( (Options*)this, 2, 4, true, 0, mutex() );
    jw.setVerticalSpacing( 2 );
    jw.setMargins( 4 );
    bb->addWidget( &jw );
  }


  SampleDataF Search::createPulse( double total_duration, double pulse_duration, double offset,
				   double sample_rate, double intensity ) {
    SampleDataF pulse(static_cast<int>(total_duration * sample_rate), 0.0, 1./sample_rate, 0.0);
    if ( offset < 0.0 )
      offset = 0.0;
    if ( pulse_duration > total_duration )
      pulse_duration = total_duration;
    if ( (pulse_duration + offset)  > total_duration )
      offset = total_duration - pulse_duration;

    int start_index = static_cast<int>(offset * sample_rate);
    for (int i = start_index; i < ((offset + pulse_duration) * sample_rate); ++i ) {
      pulse[i] = intensity;
    } 
    return pulse;
  }


  void Search::openTraceFile( ofstream &tf, TableKey &tracekey, const Options &header ) {
    tracekey.addNumber( "t", "ms", "%7.2f" );
    tracekey.addNumber( "V", Vunit, "%6.1f" );
    tf.open( addPath( "search-traces.dat" ).c_str(), ofstream::out | ofstream::app );
    header.save( tf, "# " );
    tf << '\n';
    tracekey.saveKey( tf, true, false );
    tf << '\n';
  }


  void Search::saveTrace( ofstream &tf, TableKey &tracekey, int index, const SampleDataF &voltage )  {
    tf << "# index: " << index << '\n';
    for ( int k=0; k<voltage.size(); k++ ) {
      tracekey.save( tf, 1000.0*voltage.pos( k ), 0 );
      tracekey.save( tf, voltage[k] );
      tf << '\n';
    }
    tf << '\n';
  }


  int Search::main( void )
  {
    double duration = number( "duration" );;  
    double led_intensity = number( "led_intensity" );
    double led_duration = number( "led_duration" );
    double led_onset = number( "led_onset" );
    double current_intensity = number( "current_intensity" );
    double current_duration = number( "current_duration" );
    double current_onset = number( "current_onset" );
    double samplerate = trace( 0 ).sampleRate();
    bool endless = boolean("endless");
    int repeats = number("repeats");
    bool savetrace = boolean("savetrace");
    int count = 0;
    Vunit = trace( 0 ).unit();
    OutList out_list;
    OutData signal_1;
    OutData signal_2;  
   
    ofstream tf;
    TableKey tracekey;
    Options header;
    header.addInteger( "index", completeRuns() );
    header.addInteger( "ReProIndex", reproCount() );
    header.addNumber( "ReProTime", reproStartTime(), "s", "%0.3f" );
    header.addNumber( "duration", 1000.0*duration, "ms", "%.1f" );
    lockStimulusData();
    header.newSection( stimulusData() );
    unlockStimulusData();
    header.newSection( settings() );
    
    SampleDataF voltage(0., duration, trace( 0 ).stepsize(), 0.0 );
   
    while ( ((softStop() == 0) && endless) || ((!endless && (softStop() == 0) && (count < repeats))) ) {
      if ( interrupt() || softStop() > 0){
	break;
      }
    
      signal_1.clear();
      signal_1.setTraceName( "LED-1" );
      SampleDataF pulse_1 = createPulse( duration, led_duration, led_onset, samplerate, led_intensity );
      signal_1.resize( pulse_1.size() );
      signal_1.assign( pulse_1 );
      // signal_1.setIntensity( 1. );
      signal_1.setIdent( "pulse_1" );

      signal_2.clear();
      signal_2.setTraceName( "Current-1" );
      SampleDataF pulse_2 = createPulse( duration, current_duration, current_onset, samplerate, current_intensity );
      signal_2.resize( pulse_2.size() );
      signal_2.assign( pulse_2 );
      // signal_2.setIntensity( 1. );
      signal_2.setIdent( "pulse_2" );
    
      out_list.clear();
      out_list.push( signal_1 );
      out_list.push( signal_2 );
      write( out_list );
      if ( signal_1.failed() || signal_2.failed() ) {
	std::string msg = "Output of stimulus failed!<br>Error code is <b>";
	msg += signal_1.errorText() + "</b>";
	warning( msg, 2.0 );
	writeZero( 0 );
	writeZero( 0 );
	return Failed;
      } 
      if ( savetrace ) {
	if ( count == 0 )
	  openTraceFile( tf, tracekey, header );
	trace( 0 ).copy( signalTime(), voltage );
	saveTrace( tf, tracekey, count, voltage );
      }
      writeZero( 0 );
      writeZero( 1 );
      count++;
      sleep(0.25);
    }
    writeZero( 0 );
    writeZero( 1 );
    return Completed;
  }

  addRePro( Search, photoreceptors );

}; /* namespace photoreceptors */

#include "moc_search.cc"
