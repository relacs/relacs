/*
  photoreceptors/twochannelstimulus.cc
  Puts out two stimuli on two output channels

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

#include <relacs/photoreceptors/twochannelstimulus.h>
#include <relacs/translate.h>

using namespace relacs;

namespace photoreceptors {


  TwoChannelStimulus::TwoChannelStimulus( void )
    : RePro( "TwoChannelStimulus", "photoreceptors", "Jan Grewe", "1.0", "Jun 02, 2015" )
  {
    // add some options:
    i_unit = "nA";
    double amplitude = 1.0;
    // options:
    newSection( "Channel 1" );
    addSelection( "ch1_waveform", "Stimulus waveform", "Const|Sine|Rectangular|Whitenoise" );
    addNumber( "ch1_amplitude", "Amplitude of stimulus", amplitude, 0.0, 10000.0, 1.0, i_unit ).setActivation( "ch1_waveform", "Const", false );;
    addSelection( "ch1_freqsel", "Specify", "frequency|period|number of periods" ).setActivation( "ch1_waveform", "Const", false );
    addNumber( "ch1_freq", "Frequency of waveform", 10.0, 0.0, 1000000.0, 1.0, "Hz" ).setActivation( "ch1_freqsel", "frequency" ).addActivation( "ch1_waveform", "Const", false );
    addNumber( "ch1_period", "Period of waveform", 0.1, 0.0, 1000000.0, 0.001, "s", "ms" ).setActivation( "ch1_freqsel", "period" ).addActivation( "ch1_waveform", "Const", false );
    addNumber( "ch1_numperiods", "Number of periods", 1.0, 0.0, 1000000.0, 1.0 ).setActivation( "ch1_freqsel", "number of periods" ).addActivation( "ch1_waveform", "Const", false );
    addSelection( "ch1_pulsesel", "Specify", "pulse duration|duty-cycle" ).setActivation( "ch1_waveform", "Rectangular" );
    addNumber( "ch1_pulseduration", "Pulse duration", 0.01, 0.0, 10000.0, 0.001, "s", "ms" ).setActivation( "ch1_pulsesel", "pulse duration" ).addActivation( "ch1_waveform", "Rectangular" );
    addNumber( "ch1_dutycycle", "Duty-cycle", 0.5, 0.0, 1.0, 0.05, "1", "%" ).setActivation( "ch1_pulsesel", "duty-cycle" ).addActivation( "ch1_waveform", "Rectangular" );
    addInteger( "ch1_seed", "Seed for random number generation", 0 ).setActivation( "ch1_waveform", "Whitenoise" );;
    addNumber( "ch1_cycleramp", "Ramp for each cycle", 0.0, 0.0, 10.0, 0.001, "seconds", "ms" ).setActivation( "ch1_waveform", "Rectangular" );
    addNumber( "ch1_ramp", "Ramp for stimulus onset and offset", 0.0, 0.0, 10.0, 0.001, "seconds", "ms" );
    addSelection( "ch1_outtrace", "Output trace", "V-1" );
    addNumber( "ch1_offset", "Stimulus mean", 0.0, -2000.0, 2000.0, 5.0, i_unit );
    addSelection( "ch1_offsetbase", "Stimulus mean relative to", "absolute|amplitude|current" );
    addNumber( "ch1_intertrial_intensity", "Intensity between trials", 0.0, -2000.0, 2000.0, 5.0, i_unit );

    newSection( "Channel 2" );
    addSelection( "ch2_waveform", "Stimulus waveform", "Const|Sine|Rectangular|Whitenoise" );
    addNumber( "ch2_amplitude", "Amplitude of stimulus", amplitude, 0.0, 10000.0, 1.0, i_unit ).setActivation( "ch2_waveform", "Const", false );
    addSelection( "ch2_freqsel", "Specify", "frequency|period|number of periods" ).setActivation( "ch2_waveform", "Const", false );
    addNumber( "ch2_freq", "Frequency of waveform", 10.0, 0.0, 1000000.0, 1.0, "Hz" ).setActivation( "ch2_freqsel", "frequency" ).addActivation( "ch2_waveform", "Const", false );
    addNumber( "ch2_period", "Period of waveform", 0.1, 0.0, 1000000.0, 0.001, "s", "ms" ).setActivation( "ch2_freqsel", "period" ).addActivation( "ch2_waveform", "Const", false );
    addNumber( "ch2_numperiods", "Number of periods", 1.0, 0.0, 1000000.0, 1.0 ).setActivation( "ch2_freqsel", "number of periods" ).addActivation( "ch2_waveform", "Const", false );
    addSelection( "ch2_pulsesel", "Specify", "pulse duration|duty-cycle" ).setActivation( "ch2_waveform", "Rectangular" );
    addNumber( "ch2_pulseduration", "Pulse duration", 0.01, 0.0, 10000.0, 0.001, "s", "ms" ).setActivation( "ch2_pulsesel", "pulse duration" ).addActivation( "ch2_waveform", "Rectangular" );
    addNumber( "ch2_dutycycle", "Duty-cycle", 0.5, 0.0, 1.0, 0.05, "1", "%" ).setActivation( "ch2_pulsesel", "duty-cycle" ).addActivation( "ch2_waveform", "Rectangular" );
    addInteger( "ch2_seed", "Seed for random number generation", 0 ).setActivation( "ch2_waveform", "Whitenoise" );;
    addNumber( "ch2_cycleramp", "Ramp for each cycle", 0.0, 0.0, 10.0, 0.001, "seconds", "ms" ).setActivation( "ch2_waveform", "Rectangular" );
    addNumber( "ch2_ramp", "Ramp for stimulus onset and offset", 0.0, 0.0, 10.0, 0.001, "seconds", "ms" );
    addSelection( "ch2_outtrace", "Output trace", "V-1" );
    addNumber( "ch2_offset", "Stimulus mean", 0.0, -2000.0, 2000.0, 5.0, i_unit );
    addSelection( "ch2_offsetbase", "Stimulus mean relative to", "absolute|amplitude|current" );
    addNumber( "ch2_intertrial_intensity", "Intensity between trials", 0.0, -2000.0, 2000.0, 5.0, i_unit );

    newSection( "Stimulus" );
    addNumber( "duration", "Maximum duration of stimulus", 1.0, 0.0, 1000.0, 0.01, "seconds", "ms" );
    addBoolean( "samerate", "Use sampling rate of input", true ).setActivation( "waveform", "From file", false );
    addNumber( "samplerate", "Sampling rate of output", 1000.0, 0.0, 10000000.0, 1000.0, "Hz", "kHz" ).setActivation( "samerate", "true", false );
    addNumber( "repeats", "Number of stimulus presentations", 10, 0, 10000, 1, "times" ).setStyle( OptWidget::SpecialInfinite );
    addNumber( "pause", "Duration of pause between stimuli", 1.0, 0.0, 1000.0, 0.01, "seconds", "ms" );
    addNumber( "before", "Time before stimulus to be analyzed", 0.1, 0.0, 100.0, 0.01, "seconds", "ms" );
    addNumber( "after", "Time after stimulus to be analyzed", 0.1, 0.0, 100.0, 0.01, "seconds", "ms" );
    addText( "comment", "A comment", "" );

    newSection( "Analysis" );
    addNumber( "skipwin", "Initial portion of stimulus not used for analysis", 0.1, 0.0, 100.0, 0.01, "seconds", "ms" );
    addBoolean( "storevoltage", "Save voltage trace", true );
    addBoolean( "storestimuli", "Save stimulus traces", true );
  }


  void TwoChannelStimulus::preConfig( void )
  {
    if ( SpikeTrace[0] >= 0 )
      v_unit = trace( SpikeTrace[0] ).unit();
    setText( "ch1_outtrace", outTraceNames() );
    setText( "ch2_outtrace", outTraceNames() );
    setToDefault( "outtrace" );
    if ( CurrentTrace[0] >= 0 ) {
      string iinunit = trace( CurrentTrace[0] ).unit();
      i_in_factor = Parameter::changeUnit( 1.0, iinunit, i_unit );
    }
    if (outTracesSize() < 2) {
      error("Not enought output traces!");
    }
  }

  
  double TwoChannelStimulus::checkChannelOffset(double offset, int offsetbase, double amplitude, double current_dc) {
    double Offset = 0.0;
    if ( offsetbase == 1 ) // amplitude
      Offset = offset + amplitude;
    else if ( offsetbase == 2 ) // current
      Offset = offset + current_dc;
    else   // absolute
      Offset = offset;
    return Offset;
  }
  

  bool TwoChannelStimulus::checkStimulusDuration(double duration, double skipwin) {
    bool pass = true;
    pass = pass & (duration > 1.0e-8);
    pass = pass & (duration >= skipwin );
    return pass;
  }


  Options TwoChannelStimulus::createStimulus( WaveForms waveform, OutData &signal, double &duration, double deltat,
					      double ramp, double cycleramp, double pulse_duration, 
					      double duty_cycle, double frequency, int seed, double amplitude ) {
    string wavename;
    bool store = false;
    Options header;
    double peak_amplitude_factor = 0.0;
    signal.clear();
    if ( waveform >= Sine && waveform <= Whitenoise ) {
      string waveforms[3] = { "Sine", "Rectangular", "Whitenoise" };
      header.addText( "waveform", waveforms[waveform - 1] );
      header.addText( "frequency", Str( frequency ) + "Hz" );
      if ( waveform == Sine || waveform == Whitenoise ) {
	if ( waveform == Sine ) {
	  peak_amplitude_factor = 1.0;
	  signal.sineWave( duration, deltat, frequency );
	}
	else {
	  unsigned long rnd_seed = seed;
	  signal.noiseWave( duration, deltat, frequency, 1.0, &rnd_seed );
	  peak_amplitude_factor = 0.3;
	  header.addInteger( "random seed", int( rnd_seed ) );
	  signal.resize(static_cast<int>(duration/deltat));
	}
      }
      else {
	peak_amplitude_factor  = 1.0;
	signal.rectangleWave( duration, deltat, 1.0/frequency, pulse_duration, cycleramp );
	if ( duty_cycle >= 0.0 )
	  header.addText( "dutycycle", Str( 100.0*duty_cycle ) + "%" );
	else
	  header.addText( "pulseduration", Str( 1000.0*pulse_duration ) + "ms" );
	signal = 2.0*signal - 1.0;
      }
    }
    else {
      // constant:
      signal.pulseWave( duration, deltat, 0.0, 0.0 );
      peak_amplitude_factor = 1.0;
      header.addText( "waveform", "const" );
    }

    string labelpattern = "$(waveform)$(filename)$(frequency)$(random seed) $(pulseduration)$(dutycycle)$(tau)";
    string StimulusLabel = translate( labelpattern, header );
    if ( wavename.empty() )
      wavename = StimulusLabel;

    signal *= amplitude;
    signal.ramp( ramp );
    //signal.setIntensity( 1.0 );
    header.addText( "ramp", Str( 1000.0*ramp ) + "ms" );
    double peak_amplitude = amplitude / peak_amplitude_factor;
    signal.setIdent( "signal=" + wavename + ", amplitude=" + Str( amplitude ) + i_unit );
    header.addText( "amplitude", Str( amplitude ) + i_unit );
    header.addText( "amplitudefactor", Str( peak_amplitude, 0, 3, 'g' ) );
    return header;
  }


  double TwoChannelStimulus::checkFrequency(int freq_selection, double frequency, double period, int num_periods, double duration) {
    if ( freq_selection == 1 ) { // period
      if ( period < 1.0e-8 ) {
	warning( "The period must be greater than zero!" );
	return Aborted;
      }
      frequency = 1.0/period;
    }
    else if ( freq_selection == 2 ) // number of periods
      frequency = num_periods/duration;
    return frequency;
  }


  void TwoChannelStimulus::checkPulseDuration(int pulse_selection, double &duty_cycle, double &pulse_duration, double frequency) {
    if ( pulse_selection == 1 )
      pulse_duration = duty_cycle/frequency;
    else
      duty_cycle = -1.0;
  }


  void TwoChannelStimulus::openTraceFile(ofstream &tf, TableKey &key, const Options &header) {
    key.addNumber( "t", "ms", "%7.2f" );
    key.addNumber( "V", v_unit, "%6.1f" );
    tf.open( addPath( "twoChannelStimulus-traces.dat" ).c_str(), ofstream::out | ofstream::app );
    header.save( tf, "# " );
    tf << '\n';
    key.saveKey( tf, true, false );
    tf << '\n';
  }


  string TwoChannelStimulus::writeStimulusFile(const OutData &signal, Options &header, const string &path, const string &channel_prefix) {
    Str filepattern = "$(waveform)_$(filename)f:$(frequency)$(random seed)$(pulseduration)$(dutycycle)$(tau)r:$(ramp)d:$(duration)_a:$(amplitude)_$(amplitudefactor).dat";
    string filename = path + channel_prefix + "_" + translate( filepattern, header );
    ifstream cf( filename.c_str() );
    if ( ! cf ) {
      ofstream of( filename.c_str() );
      header.erase( "filename" );
      header.save( of, "# " );
      of << '\n';
      of << "#Key\n";
      of << "# t   x\n";
      of << "# s   1\n";
      signal.save( of, 7, 5 );
      printlog( "wrote " + filename );
    }
    return filename;
  }


  void TwoChannelStimulus::saveTrace( ofstream &tf, TableKey &tracekey, int index, const SampleDataF &voltage )  {
    tf << "# index: " << index << '\n';
    for ( int k=0; k<voltage.size(); k++ ) {
      tracekey.save( tf, 1000.0*voltage.pos( k ), 0 );
      tracekey.save( tf, voltage[k] );
      tf << '\n';
    }
    tf << '\n';
  }


  int TwoChannelStimulus::main( void )
  {
    OutList out_list;
    int count = 0;
    // channel 1 options
    WaveForms ch1_waveform = (WaveForms)index( "ch1_waveform" );
    int ch1_freqsel = index( "ch1_freqsel" );
    double ch1_frequency = number( "ch1_freq" );
    double ch1_period = number( "ch1_period" );
    double ch1_numperiods = number( "ch1_numperiods" );
    int ch1_pulsesel = index( "ch1_pulsesel" );
    double ch1_pulse_duration = number( "ch1_pulseduration" );
    double ch1_dutycycle = number( "ch1_dutycycle" );
    int ch1_seed = integer( "ch1_seed" );
    double ch1_amplitude = number( "ch1_amplitude" );
    double ch1_ramp = number( "ch1_ramp" );
    int ch1_outtrace = index( "ch1_outtrace" );
    double ch1_cycleramp = number( "ch1_cycleramp" );
    double ch1_offset = number( "ch1_offset" );
    int ch1_offsetbase = index( "ch1_offsetbase" );
    double ch1_intertrial_int = number( "ch1_intertrial_intensity" );

    //channel 2 options
    WaveForms ch2_waveform = (WaveForms)index( "ch2_waveform" );
    int ch2_freqsel = index( "ch2_freqsel" );
    double ch2_frequency = number( "ch2_freq" );
    double ch2_period = number( "ch2_period" );
    double ch2_numperiods = number( "ch2_numperiods" );
    int ch2_pulsesel = index( "ch2_pulsesel" );
    double ch2_pulse_duration = number( "ch2_pulseduration" );
    double ch2_dutycycle = number( "ch2_dutycycle" );
    int ch2_seed = integer( "ch2_seed" );
    double ch2_amplitude = number( "ch2_amplitude" );
    double ch2_ramp = number( "ch2_ramp" );
    int ch2_outtrace = index( "ch2_outtrace" );
    double ch2_cycleramp = number( "ch2_cycleramp" );
    double ch2_offset = number( "ch2_offset" );
    int ch2_offsetbase = index( "ch2_offsetbase" );
    double ch2_intertrial_int = number( "ch2_intertrial_intensity" );

    if ( ch1_outtrace == ch2_outtrace ) {
      warning( "Channel 1 and two are setup to use the same outtrace!" );
      return Failed;
    }
    // Stimulus options
    double duration = number( "duration" );
    bool samerate = boolean( "samerate" );
    double samplerate = number( "samplerate" );
    int repeats = integer( "repeats" );
    double pause = number( "pause" );
    double before = number( "before" );
    double after = number( "after" );
    if ( before > pause )
      before = pause;
    if ( after > pause )
      after = pause;
    // Analysis options
    double skipwin = number( "skipwin" );
    bool storevoltage = boolean( "storevoltage" );
    bool storestimuli = boolean( "storestimuli" );
    string storepath = addPath( "" );
  
    lockStimulusData();
    double ch1_current_dc = stimulusData().number( outTraceName( ch1_outtrace ) );
    double ch2_current_dc = stimulusData().number( outTraceName( ch2_outtrace ) );
    unlockStimulusData();
  
    ch1_offset = checkChannelOffset(ch1_offset, ch1_offsetbase, ch1_amplitude, ch1_current_dc);
    ch2_offset = checkChannelOffset(ch2_offset, ch2_offsetbase, ch2_amplitude, ch2_current_dc);
   
    if (!checkStimulusDuration(duration, skipwin)) {
      warning( "The stimulus duration must be greater than zero and stimulus duration must be larger than analysis window!" );
      return Aborted;
    }
   
    // stimulus frequency:
    ch1_frequency = checkFrequency(ch1_freqsel, ch1_frequency, ch1_period, ch1_numperiods, duration);
    ch2_frequency = checkFrequency(ch2_freqsel, ch2_frequency, ch2_period, ch2_numperiods, duration);
    
    // pulse duration:
    checkPulseDuration(ch1_pulsesel, ch1_dutycycle, ch1_pulse_duration, ch1_frequency);
    checkPulseDuration(ch2_pulsesel, ch2_dutycycle, ch2_pulse_duration, ch2_frequency);

    // signal:
    OutData signal_1, signal_2;
    signal_1.setTrace( ch1_outtrace );
    signal_2.setTrace( ch2_outtrace );

    if ( samerate )
      samplerate = trace( 0 ).sampleRate();
    else if ( samplerate <= 0.0 )
      samplerate = signal_1.maxSampleRate();
    
    // create stimuli
    Options header_1, header_2;
    header_1 = createStimulus( ch1_waveform, signal_1, duration, 1./samplerate, ch1_ramp, ch1_cycleramp, 
			       ch1_pulse_duration, ch1_dutycycle, ch1_frequency, ch1_seed, ch1_amplitude );
    header_2 = createStimulus( ch2_waveform, signal_2, duration, 1./samplerate, ch2_ramp, ch2_cycleramp, 
			       ch2_pulse_duration, ch2_dutycycle, ch2_frequency, ch2_seed, ch2_amplitude );
    signal_1 += ch1_offset;
    signal_2 += ch2_offset;

    OutData ch1_intertrial(pause, 1./samplerate);
    OutData ch2_intertrial(pause, 1./samplerate);
    ch1_intertrial.setTrace( ch1_outtrace );
    ch2_intertrial.setTrace( ch2_outtrace );
    ch1_intertrial += ch1_intertrial_int;
    ch2_intertrial += ch2_intertrial_int;

    signal_1.append( ch1_intertrial );
    signal_2.append( ch2_intertrial );

    out_list.clear();
    out_list.push( signal_1 );
    out_list.push( signal_2 );
      
    string stim1_file, stim2_file;
    if ( storestimuli ) {
      string prefix = "ch1_";
      stim1_file = writeStimulusFile(signal_1, header_1, storepath, prefix);
      prefix = "ch2_";
      stim2_file = writeStimulusFile(signal_2, header_2, storepath, prefix);
    }
    ofstream tf;
    TableKey tracekey;
    Options header;
    header.addInteger( "index", completeRuns() );
    header.addInteger( "ReProIndex", reproCount() );
    header.addNumber( "ReProTime", reproStartTime(), "s", "%0.3f" );
    header.addNumber( "ch1_offset", ch1_offset, i_unit, "%g" );
    header.addNumber( "ch2_offset", ch2_offset, i_unit, "%g" );
    header.addNumber( "ch1_amplitude", ch1_amplitude, i_unit, "%g" );
    header.addNumber( "ch2_amplitude", ch2_amplitude, i_unit, "%g" );
    header.addNumber( "duration", 1000.0*duration, "ms", "%.1f" );
    header.addText( "ch1_stimfile", stim1_file );
    header.addText( "ch2_stimfile", stim2_file );
    lockStimulusData();
    header.newSection( stimulusData() );
    unlockStimulusData();
    header.newSection( settings() );

    SampleDataF voltage( -before, duration+after, trace( 0 ).stepsize(), 0.0 );
   
    timeStamp();
    while ( (softStop() == 0) && (count < repeats) ) {
      if ( interrupt() || softStop() > 0){
	break;
      }
      string s =  "<b>TwoChannelStim</b>";
      s += ",  Loop <b>" + Str( count+1 ) + "</b>";
      if ( repeats > 0 )
	s += " of <b>" + Str( repeats ) + "</b>";
      message( s );
    
      write( out_list );
      if ( signal_1.failed() || signal_2.failed() ) {
	std::string msg = "Output of stimulus failed!<br>Error code is <b>";
	msg += signal_1.errorText() + "</b>";
	warning( msg, 2.0 );
	writeZero( 0 );
	writeZero( 1 );
	return Failed;
      }
      
      if ( storevoltage ) {
	if ( count == 0 )
	  openTraceFile( tf, tracekey, header );
	if (signalTime() < duration) {
	  cerr << "NoData!!!" << endl;
	  std::string msg = "Output of stimulus failed!<br>Error code is <b>";
	  msg += signal_1.errorText() + "</b>";
	  warning( msg, 2.0 );
	  writeZero( 0 );
	  writeZero( 1 );
	  return Failed;
	}
	trace( 0 ).copy( signalTime(), voltage );
	cerr << voltage.size() << endl;
	saveTrace( tf, tracekey, count, voltage );
      }
      count++;
    }
    writeZero( 0 );
    writeZero( 1 );

    return Completed;
  }


  addRePro( TwoChannelStimulus, photoreceptors );

}; /* namespace photoreceptors */

#include "moc_twochannelstimulus.cc"
