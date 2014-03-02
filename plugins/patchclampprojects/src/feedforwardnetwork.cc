/*
  patchclampprojects/feedforwardnetwork.cc
  Generates a FFN in an iterative way with one neuron, like in Reyes 2003 Nat Neurosci 6(6):593-599
  Feedforward inhibition in the FFN is included like in Kremkow et al. 2010 J Comp Neurosci 28(3):579:594

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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <relacs/patchclampprojects/feedforwardnetwork.h>
#include <relacs/kernel.h>
#include <relacs/random.h>
#include <relacs/eventlist.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <map>
#include <string>
// #include <cstring>
// include <boost/algorithm/string/regex.hpp>
//#include <boost/regex.hpp>
#include <relacs/tablekey.h>

using namespace std;
using namespace relacs;

namespace patchclampprojects {


FeedForwardNetwork::FeedForwardNetwork( void )
  : RePro( "FeedForwardNetwork", "patchclampprojects", "Jens Kremkow", "1.0", "Oct 29, 2010" )
{
  // add some options:
  newSection( "Network" );
  addInteger( "nGroups", "Number of groups", 4, 1, 100, 1 );
  addInteger( "nExc", "Number of Exc neurons in each group", 100, 1, 1000, 1 );
  //addInteger( "nInh", "Number of Inh neurons in each group", 25, 1, 1000, 1 );
  addNumber( "cp", "Connection probability", 0.5, 0.0, 1.0, 0.001 );
  addNumber( "JeFFN", "Exc. Synaptic strength", 0.1, 0.0, 1000.0, 0.1 );
  addNumber( "scaleJeFFN", "Exc. Synaptic scaling", 1., 0.0, 100.0, 0.01 );
  addNumber( "gFFN1", "Inhibitory gain S1", 0.1, 0.0, 100.0, 0.1 );
  addNumber( "gFFN2", "Inhibitory gain S2", 0.1, 0.0, 100.0, 0.1 );
  
  addNumber( "synapse_jitter", "Synapse Jitter", 0.001, 0.0, 10.0, 0.1 ,"sec","ms");
  addNumber("intergroupdelay","Delay between groups",0.0,0.,1.,0.001,"sec","ms");
  
  addNumber( "delay", "Delay between Exc & Inh", 0.003, -10.0, 10.0, 0.1 ,"sec","ms");
  addNumber( "gateDelay1", "Delay between Exc & Inh in gate S1", 0., -10.0, 10.0, 0.1 ,"sec","ms");
  addNumber( "gateDelay2", "Delay between Exc & Inh in gate S2", 0., -10.0, 10.0, 0.1 ,"sec","ms");
  addInteger("gateGroup","Gate Group",0,0,100);
  addInteger("seedNetwork","RNG seed of the network",2,1000,1);
  //addSelection("calibrateBKG","Background calibration","yes|no");
  addSelection("calibrateFFN","FFN calibration","yes|no");
  addSelection("calibrateNoise","Noise calibration","yes|no");
  addSelection("stimulate","Stimulate","yes|no");
  
  
  newSection("Synapses");
  addNumber("E_ex","Excitatory reversal potential",0.0,-100.,100.,0.1,"mV");
  addNumber("E_in","Inhibitory reversal potential",-85.0,-100.,100.,0.1,"mV");
  addNumber("tau_syn_exc","Excitatory synaptic time-constant",0.001,0.0,1.,0.00001,"sec","ms");
  addNumber("tau_syn_inh","Inhibitory synaptic time-constant",0.01,0.0,1.,0.00001,"sec","ms");
  
  newSection("Stimuli");
  addSelection("stimulus","Stimulus type","pulse packet|MIP|poisson|file");
  addNumber("onset1","Onset S1",0.05,0.,10000.,1,"sec","ms");
  addNumber("onset2","Onset S2",0.05,0.,10000.,1,"sec","ms");
  addNumber( "duration", "Duration", 0.1, 0.01, 1000.0, 0.01,"sec","ms");
  addInteger("seedStimulus","RNG seed of the stimulus",1,1000,1);
  addNumber("pause","Pause",0.,0.,10.,0.1,"sec","ms");
  addNumber("pre_pause","PreStimPause",0.,0.,10.,0.1,"sec","ms");
  addNumber("prestim_pause","Prestim-Group-Pause",0.,0.,10.,0.1,"sec","ms");
  addSelection("plot_trace","Plot Trace","yes|no");
  //addSelection("skip","Skip when no input spikes","yes|no");
  
  newSection("Pulse Packet");
  addNumber("alpha1","Number of spikes S1",1,0,1000.,1);
  addNumber("sigma1","Temporal spread S1",0.002,0.00001,1000.,0.1,"sec","ms");	
  addNumber("alpha2","Number of spikes S2",1,0,1000.,1);
  addNumber("sigma2","Temporal spread S2",0.002,0.00001,1000.,0.1,"sec","ms");
  
  newSection("Poisson");
  addNumber("poissonstimulusrate1","Rate S1",0.,0.,1000.,0.1,"Hz");
  addNumber("poissonstimulusrate2","Rate S2",0.,0.,1000.,0.1,"Hz");
  addNumber( "duration1", "Duration S1", 0.1, 0.01, 1000.0, 0.01,"sec","ms");
  addNumber( "duration2", "Duration S2", 0.1, 0.01, 1000.0, 0.01,"sec","ms");
  
  newSection("Spikes from File");
  addNumber( "durationSfF", "Duration SfF", 0.1, 0.01, 1000.0, 0.01,"sec","ms");
  addNumber( "startSfF", "Start SfF", 0., 0., 1000.0, 0.0,"sec","ms");

  newSection("Background Network");
  addNumber("poissonrate","Rate of background input",5000.,0.,50000.,1.,"Hz");
  addNumber( "JeBKG", "Exc. Synaptic strength", 0.3, 0.0, 1000.0, 0.1 );
  addNumber( "gBKG", "Inhibitory gain", 1., 0.0, 100.0, 0.1 );
  addInteger("seedBKG","RNG seed of the BKG",1,1000,1);
  addNumber("noise_std","noise std",0.,0.0,100.,0.01);
  addNumber("noise_mean","noise mean",0.,-100,100.,0.01);
  
  newSection("Calibration");
  addInteger("calibrationtrials","Trials",30,1,100,1);
  addNumber("calibrationalpha","Number of spikes",60,1,1000.,1);
  addNumber("calibrationsigma","Temporal spread",0.002,0.00001,1000.,0.1,"sec","ms");
  addNumber("JStep","Synaptic Weight Change Step",0.01,0.,100.,0.001);
  addNumber("JeBKGStep","Synaptic Weight Change Step Bkg",0.01,0.,100.,0.001);
  addNumber("targetMean","Target Mean vm",-70.,-100.,0.,0.01);
  addNumber("targetStd","Target Std vm",3.,0.,100,0.001);
  addNumber( "calibrationduration", "Duration", 1., 0.01, 1000.0, 0.01,"sec","ms");
  addNumber("noise_std_step","noise std step",0.,100.,0.,0.001);
  addNumber("noise_mean_step","noise mean step",0.,0.,100,0.001);
  addNumber("noise_mean_step_FFN","noise mean step FFN",0.,0.,100,0.001); 

  newSection("MIP");
  addNumber("MIPstimulusrate1","Rate S1",10.,0.00001,1000.,0.1,"Hz" );
  addNumber("MIPcorr1","Correlation S1",0.0,0.0,1.,0.01);
  addNumber("MIPstimulusrate2","Rate S2",10.,0.00001,1000.,0.1,"Hz" );
  addNumber("MIPcorr2","Correlation S2",0.0,0.0,1.,0.01);
  
  newSection( "Setup" );
  addSelection("rec_curr","record currents","yes|no");
  
  P.clear();
  P.lock();
  P.resize( 2, 1, true );
  P.unlock();
  setWidget( &P ); 
}
  

int FeedForwardNetwork::main( void )
{
  /*
    This RePro scans a feedforward network. It can run a calibration method before starting the FFN.
  */
  noMessage();
  P.lock();
  P.clear();
  P.unlock();
  // init:
  DoneState state = Completed;
  double samplerate = trace( SpikeTrace[0] ).sampleRate();
  message("Samplerate: "+Str(samplerate));
  double JeFFN = number("JeFFN");
  double scaleJeFFN = number("scaleJeFFN");
  double JeBKG = number("JeBKG");
  
  double gFFN1 = number("gFFN1");
  double gFFN2 = number("gFFN2");
  double gBKG = number("gBKG");
  int nGroups = integer("nGroups");
  int nExc = integer("nExc");
  //int nInh = integer("nInh");
  double noise_std = number("noise_std");
  double noise_mean = number("noise_mean");
  double intergroupdelay = number("intergroupdelay");
  // Stimulus
  //  double onset = number("onset");
  double onset1 = number("onset1");
  double onset2 = number("onset2");
  double duration = number("duration");
  double duration1 = number("duration1");
  double duration2 = number("duration2");
  // stimulus from file
  double durationSfF = number("durationSfF");
  double startSfF = number("startSfF");
  message("Duration: "+Str(duration));
  
  // RNG
  rngNetwork.setSeed(integer("seedNetwork"));
  rngStimulus.setSeed(integer("seedStimulus"));
  rngBKG.setSeed(integer("seedBKG"));
  
  // Data
  // SpikeTimes of the stimuli
  vector<vector<EventData> > SpikeTimesStimulusVector(2,vector<EventData> (nExc));
  for (int group=0;group<2;group++)
    {
      for (int neuron=0;neuron<nExc;neuron++){
	SpikeTimesStimulusVector[group][neuron].reserve(10000);
      }
    }


  // SpikeTimes as a vector of vector of EventData
  vector<vector<EventData> > SpikeTimesVector(nGroups,vector<EventData> (nExc));
  for (int group=0;group<nGroups;group++)
    {
      for (int neuron=0;neuron<nExc;neuron++){
	SpikeTimesVector[group][neuron].reserve(10000);
      }
    }
  
  // vm as a vector
  vector<vector<SampleDataD> > VmVector(nGroups,vector<SampleDataD> (nExc));
  
  // curr as a vector
  vector<vector<SampleDataD> > CurrVector(nGroups,vector<SampleDataD> (nExc));
  
  // Conductances as a vector of vector of EventData
  vector<vector<SampleDataD> > GeVector(nGroups,vector<SampleDataD> (nExc));
  vector<vector<SampleDataD> > GiVector(nGroups,vector<SampleDataD> (nExc));
  
  // Signaltimes
  vector<vector<double> > SignalTimesVector(nGroups,vector<double> (nExc));
  vector<vector<int> > nospikeVector(nGroups,vector<int> (nExc));
  
  // calibration
  if (index("calibrateBKG")==0 || index("calibrateFFN")==0 || index("calibrateNoise")==0){
    message("We calibrate the FFN");
    message("JeFFN before calibration: "+Str(JeFFN));
    message("JeBKG before calibration: "+Str(JeBKG));
    message("gBKG before calibration: "+Str(gBKG));
    message("noise_std before calibration: "+Str(noise_std));
    message("noise_mean before calibration: "+Str(noise_mean));
    if (! calibrateFFN(JeFFN,JeBKG,gBKG,noise_std,noise_mean)){
      return Completed;
    }
    setNumber("JeFFN",JeFFN);
    setNumber("JeBKG",JeBKG);
    setNumber("gBKG",gBKG);
    setNumber("noise_std",noise_std);
    setNumber("noise_mean",noise_mean);
    setToDefaults();
    message("JeFFN after calibration: "+Str(JeFFN));
    message("JeBKG after calibration: "+Str(JeBKG));
    message("gBKG after calibration: "+Str(gBKG));
    message("noise_std after calibration: "+Str(noise_std));
    message("noise_mean after calibration: "+Str(noise_mean));
  }
  else {
    P.lock();
    P.resize( 2, 1, true );
    P.unlock();
    message("JeFFN: "+Str(JeFFN));
    message("JeBKG: "+Str(JeBKG));
    message("gBKG: "+Str(gBKG));
  }
  if (index("stimulate")==1){ 
    // we dont want to stimulate
    return state;
  }

  // Stimulus generation
  if (index("stimulus")==0) {
    message("Generate Pulse Packet stimulus with sigma: "+Str(number("sigma"))+" (ms) and alpha: "+Str(number("alpha"))+" (spikes)");
    //PulsePacket(SpikeTimesVector[0],integer("alpha1"),number("sigma1"),nExc,onset);
    PulsePacket(SpikeTimesStimulusVector[0],number("alpha1"),number("sigma1"),nExc,onset1); // S1
    PulsePacket(SpikeTimesStimulusVector[1],number("alpha2"),number("sigma2"),nExc,onset2); // S2
  }
  else if (index("stimulus")==1){
    message("Loading MIP");
    MIP(SpikeTimesStimulusVector[0],number("MIPstimulusrate1"),number("MIPcorr1"),duration1,onset1);
    MIP(SpikeTimesStimulusVector[1],number("MIPstimulusrate2"),number("MIPcorr2"),duration2,onset2);
    //return 0;
  }
  else if (index("stimulus")==2){
    message("Loading Poisson");
    //message("Generate poisson stimulus with rate: "+Str(number("rate")));
    Poisson(SpikeTimesStimulusVector[0], number("poissonstimulusrate1"),duration1, onset1); // S1
    Poisson(SpikeTimesStimulusVector[1], number("poissonstimulusrate2"),duration2, onset2); // S2
  }
  else if (index("stimulus")==3){
    message("Loading spikes from file");
    // we load spikes from the text file: ffn_input_spikes.txt
    StimulusFromFile(SpikeTimesStimulusVector[0],durationSfF, onset1,startSfF);
  }
  
  // Plot stimulus
  P[0].clear();
  for (int neuron=0;neuron<integer("nExc");neuron++){
    rasterplot(SpikeTimesStimulusVector,0,neuron,-2*integer("nExc"));
    rasterplot(SpikeTimesStimulusVector,1,neuron,-2*integer("nExc"));
  }
  
  // Generate FeedForwardNetwork
  // The FFN is generate by stimuluating each neuron a group with its convergent inputs from the preceeding group.
  GammaKernel gkexc(number("tau_syn_exc"),1);
  GammaKernel gkinh(number("tau_syn_inh"),1);

  // open outputfiles
  ofstream dfvm(addPath( "ffn_vm_trace_stimulation_"+Str(completeRuns())+".dat" ).c_str());
  ofstream dfge(addPath( "ffn_ge_trace_stimulation_"+Str(completeRuns())+".dat" ).c_str());
  ofstream dfgi(addPath( "ffn_gi_trace_stimulation_"+Str(completeRuns())+".dat" ).c_str());
  ofstream dfcurr(addPath( "ffn_curr_trace_stimulation_"+Str(completeRuns())+".dat" ).c_str());
  ofstream dfsp(addPath( "ffn_spike_times_stimulation_"+Str(completeRuns())+".dat" ).c_str());
  ofstream dfsi(addPath( "ffn_signal_times_stimulation_"+Str(completeRuns())+".dat" ).c_str());
  // Groups
  for (int group=0;group<nGroups && softStop() == 0;group++){
    // Neuron in groups
    message("Calculating inputs in group: "+Str(group));
    for (int neuron=0;neuron<nExc;neuron++){
      unlockAll();
      // Collect the convergent input spikes
      // Excitatory spikes
      double delay1 = 0.;
      double delay2 = 0.;
      EventData ExcInputSpikes1;
      EventData ExcInputSpikes2;
      EventData InhInputSpikes1;
      EventData InhInputSpikes2;
      int nospike = 0;
      if (group==0){
	// we have to collect the data from the stimulus, here we can have two stimuli
	ExcInputSpikes1 = convergentInput(SpikeTimesStimulusVector[0],intergroupdelay); // S1
	ExcInputSpikes2 = convergentInput(SpikeTimesStimulusVector[1],intergroupdelay); // S2
	if (group == integer("gateGroup")){
	  delay1 = number("gateDelay1");
	  delay2 = number("gateDelay1");
	}
	else{
	  delay1 = number("delay");
	  delay2 = number("delay");
	}
	InhInputSpikes1 = convergentInput(SpikeTimesStimulusVector[0],intergroupdelay+delay1);
	InhInputSpikes2 = convergentInput(SpikeTimesStimulusVector[1],intergroupdelay+delay2);
      }
      else {
	// we are in the FFN, here we can not have two stimuli as we only have one pre-group
	ExcInputSpikes1 = convergentInput(SpikeTimesVector[group-1],intergroupdelay);
	//message("Rate: "+Str(ExcInputSpikes1.size()));
	
	// Inhibitory spikes
	// If we are in the gate group we have to change the delay
	if (group == integer("gateGroup")){
	  delay1 = number("gateDelay1");
	}
	else{
	  delay1 = number("delay");
	}
	InhInputSpikes1 = convergentInput(SpikeTimesVector[group-1],intergroupdelay+delay1);
	if (ExcInputSpikes1.size()==0 && InhInputSpikes1.size()==0){
	  nospike = 1;
	  nospikeVector[group][neuron] = 1;
	}
	else{
	  nospike = 0;
	  nospikeVector[group][neuron] = 0;
	}
      }
            
      // Convert input spikes into conductance traces
      // Excitatory cond input
      SampleDataD ExcStimCond1( 0.0, duration, 1.0/samplerate );
      SampleDataD ExcStimCond2( 0.0, duration, 1.0/samplerate );
      ExcInputSpikes1.rate(ExcStimCond1,gkexc);
      ExcInputSpikes2.rate(ExcStimCond2,gkexc);
      ExcStimCond1 /= gkexc.max(); // 1 spike would have ampltiude 1
      ExcStimCond2 /= gkexc.max(); // 1 spike would have ampltiude 1
      // we add
      ExcStimCond1 += ExcStimCond2;
      // we scale after we add, because exc is not used in the amplitude gating
      ExcStimCond1 *= JeFFN; // scale by JeFFN
      ExcStimCond1 *= scaleJeFFN; // scale by scaleJeFFN
      
      // Inhibitory cond input
      SampleDataD InhStimCond1( 0.0, duration, 1.0/samplerate );
      SampleDataD InhStimCond2( 0.0, duration, 1.0/samplerate );
      InhInputSpikes1.rate(InhStimCond1,gkinh);
      InhInputSpikes2.rate(InhStimCond2,gkinh);
      InhStimCond1 /= gkinh.max();
      InhStimCond2 /= gkinh.max();
      //message("max inhcond1: "+Str(max(InhStimCond1)));
      //message("max inhcond2: "+Str(max(InhStimCond2)));
      // we scale first, because of the amplitude gating
      //message("cp : "+Str(number("cp")));
      //message("scaleJeFFN : "+Str(scaleJeFFN));
      //message("gFFN1 : "+Str(gFFN1));
      //message("gFFN2 : "+Str(gFFN2));
      //message("gatedelay1 : "+Str(number("gateDelay1")));
      //message("gatedelay2 : "+Str(number("gateDelay2")));
      
      InhStimCond1 *= gFFN1; // scale by gFFN
      InhStimCond2 *= gFFN2; // scale by gFFN
      // we add
      InhStimCond1 += InhStimCond2;
      // we scale overall
      InhStimCond1 *= JeFFN; // scale by JeFFN
      InhStimCond1 *= scaleJeFFN; // scale by scaleJeFFN
      
      // Background input
      //EventData ExcBkg;
      //ExcBkg.poisson(number("poissonrate"),0.,duration,rngBKG); // Exc Bkg
      //EventData InhBkg;
      //InhBkg.poisson(number("poissonrate"),0.,duration,rngBKG); // Inh Bkg
      
      
      //SampleDataD ExcBkgCond( 0.0, duration, 1.0/samplerate );
      //ExcBkg.rate(ExcBkgCond,gkexc);
      //ExcBkgCond /= gkexc.max();
      //ExcBkgCond *= JeBKG;
      //SampleDataD InhBkgCond( 0.0, duration, 1.0/samplerate );
      //InhBkg.rate(InhBkgCond,gkinh);
      //InhBkgCond /= gkinh.max();
      //InhBkgCond *= JeBKG;
      //InhBkgCond *= gBKG;
      
      // combined data
      //SampleDataD ge(ExcBkgCond+ExcStimCond1);
      //SampleDataD gi(InhBkgCond+InhStimCond1);
      SampleDataD ge(ExcStimCond1);
      SampleDataD gi(InhStimCond1);
      
      // Save input
      GeVector[group][neuron] = ge;
      GiVector[group][neuron] = gi;
      
      SampleDataD vm( 0.0, duration, 1.0/samplerate );
      VmVector[group][neuron] = vm;
      SampleDataD curr( 0.0, duration, 1.0/samplerate );
      CurrVector[group][neuron] = curr;
      lockAll();
      //}
      //message("Start stimulations");
      //sleep( number("prestim_pause")); 
      //for (int neuron=0;neuron<nExc;neuron++){
      message("Group: "+Str(group+1)+"/"+Str(nGroups)+" neuron: "+Str(neuron)+"/"+Str(nExc));
      //int nospike = 0;
      // Stimulation
      if (nospikeVector[group][neuron] == 0 || neuron==0) {
	stimulate(GeVector[group][neuron],GiVector[group][neuron],VmVector[group][neuron],CurrVector[group][neuron],SpikeTimesVector[group][neuron],SignalTimesVector[group][neuron],duration,noise_std,noise_mean);
	if ( interrupt() ) {
	  break;
	}
	if (index("plot_trace")==0) {
	  // plot trace:
	  tracePlotSignal(duration);
	  // plotToggle( true, true, duration, 0.0);// has been removed, changed to ??
	  //tracePlotSignal( duration );
	  
	  // plot rasters and vm and conductance
	  rasterplot(SpikeTimesVector,group,neuron,0);
	  traceplot(GeVector[group][neuron],GiVector[group][neuron],1,duration);
	  //}
	}
	//else {
	//tracePlotSignal(duration);
	//  rasterplot(SpikeTimesVector,group,neuron,0);
	//}
      }
      else{
	sleep(number("pause")); 
      }
      // save online      
      saveOnline(dfvm,dfge,dfgi,dfcurr,dfsp,dfsi,VmVector[group][neuron],GeVector[group][neuron],GiVector[group][neuron],CurrVector[group][neuron],SpikeTimesVector[group][neuron],SignalTimesVector[group][neuron],group,neuron);
    }
    if ( interrupt() ) {
      break;
    }
  } 
  
  
  // save the data
  if ( ! interrupt()) {
    //saveEvents(SpikeTimesVector,"spike_times_stimulation");
    //saveTraces(GeVector,GiVector,VmVector,CurrVector,duration,"stimulation");
    saveSettings();
    saveEvents(SpikeTimesStimulusVector,"stimulus_times_stimulation");
  }
  return state;
  
}
  
void FeedForwardNetwork::rasterplot(const vector<vector<EventData> > &SpikeTimes,int group, int neuron, int groupoffset){
  
  P.lock();
  // we do not clear the plot because we only add the latest spike train
  P[0].setYLabel("ID");
  
  // Raster Plot
  double x = 0.;
  double y = 0.;
  Plot::Coordinates xcoor=Plot::First;
  Plot::Coordinates ycoor=Plot::First;
  int lwidth = 1.;
  Plot::Points ptype=Plot::StrokeVertical;
  double size = 2.0;
  Plot::Coordinates sizecoor=Plot::First;
  int pcolor=Plot::White;
  
  double ymax = (number("nGroups")*number("nExc"));
  double ymin = -2*number("nExc");
  //ymin = -1.*ymax*0.1;
  ymax = ymax+0.1*ymax;
  
    
  P[0].plotHLine((group*number("nExc"))-0.5+groupoffset, Plot::Green, 1 );
  
  EventData ed = SpikeTimes[group][neuron];
  for (int sp=0;sp<ed.size();sp++){
    x = ed[sp];
    y = neuron+(group*number("nExc"))+groupoffset;
    P[0].plotPoint(x,xcoor,y,ycoor,lwidth,ptype,size,sizecoor,pcolor);
  }
  
  if ( ! P[0].zoomedXRange() )
    P[0].setXRange( 0.0,number("duration"));
  if ( ! P[0].zoomedYRange() )
    P[0].setYRange( ymin, ymax);
  
  P.draw();
  P.unlock();
}

  void FeedForwardNetwork::traceplot(SampleDataD &ge,SampleDataD &gi, int pindex, double duration){
  
    P.lock();
    P[pindex].setXLabel("Time (sec)");
    P[pindex].setYLabel("G (nS)");
    P[pindex].clear();
    // cond
    P[pindex].plot(ge,1.,Plot::Red);
    P[pindex].plot(gi,1.,Plot::Blue);
    if ( ! P[pindex].zoomedXRange() )
      P[pindex].setXRange( 0.0, duration);
    P.draw();
    P.unlock();
  }


  void FeedForwardNetwork::PulsePacket(vector<EventData> &SpikeTimes, double alpha, double sigma, int groupsize, double onset)
{
  message("alpha "+Str(alpha));
  double time;
  
  int alpha_int = floor(alpha);
  double p = alpha-alpha_int;
  double pd;
  for (int neuron=0;neuron<groupsize;neuron++){
    for (int i=0;i<alpha_int;i++){
      time = rngStimulus.gaussian();
      SpikeTimes[neuron].insert((sigma*time)+onset);
      //message("neuron "+Str(neuron)+" time "+Str(time)+" +onset "+Str((sigma*time)+onset));
      //message("PP: neuron: "+Str(neuron)+" spikes "+Str(SpikeTimes[neuron].size()));
    }
    if (p > 0){
      pd = rngStimulus.uniform();
      if (pd <= p){
	time = rngStimulus.gaussian();
	SpikeTimes[neuron].insert((sigma*time)+onset);
      }
    }
    
  }
}

void FeedForwardNetwork::Poisson(vector<EventData> &SpikeTimes, double rate, double duration, double onset)
  {
    //message("pg: "+Str(number("duration"))+"  rate "+Str(number("poissonstimulusrate")));
    for (int neuron=0;neuron<number("nExc");neuron++){
      EventData pgtmp;
      pgtmp.poisson(rate,0.0,duration,rngStimulus);//-number("onset"));
      //message("pg "+Str(neuron)+" size "+Str(pgtmp.size()));
      for (int i=0;i<pgtmp.size();i++){
	pgtmp[i] += onset;
      }
      SpikeTimes[neuron] = pgtmp;
            
    }
  }

  void FeedForwardNetwork::MIP(vector<EventData> &SpikeTimes,double rate, double corr, double duration, double onset)
  {
    // MIP process like in Kuhn et al. Neural Comput 2003
    double MIPrate = rate*(1./corr);//number("MIPstimulusrate")*(1./number("MIPcorr"));
    EventData pgMother;
    // pgMother.poisson(MIPrate,0.,number("duration")-number("onset"),rngStimulus);
    pgMother.poisson(MIPrate,0.,duration,rngStimulus);
    
    message("Generate MIP stimulus with rate: "+Str(MIPrate)+" and correlation: "+Str(corr));
    double cp = 1.-corr;//number("MIPcorr");
    double p = 0.;
    for (int neuron=0;neuron<integer("nExc");neuron++){
      EventData pgtmp;
      pgtmp.reserve(10000);
      for (int i=0;i<pgMother.size();i++){
	p = rngStimulus.uniform();
	if (p>=cp){
	  pgtmp.insert(pgMother[i]+onset);
	}
      }
      SpikeTimes[neuron] = pgtmp;
    }
  }

void FeedForwardNetwork::StimulusFromFile(vector<EventData> &SpikeTimes,double duration, double onset, double startSfF)
  {  
    int cap;
    cap = SpikeTimes[0].capacity();
    message("Capacity of the event data: "+Str(cap));
    //    int group;
    int neuron;
    double spiketime;
    string line;
    message("Loading spikes from file: duration: "+Str(duration)+" onset: "+Str(onset)+" startSfF: "+Str(startSfF));
    std::ifstream inputfile("/home/experiment/data/ffn/input_spikes/ffn_stimulus_spikes.datpy");
    if (!inputfile.is_open())
      {
	message(" Failed to open");
      }
    else
      {
	message("Open ok, reading data now");
	/*while(inputfile)
	  {
	    inputfile >> group >> neuron >> time;
	    cout << "group " << group << " neuron "<< neuron << " time "<< time << endl;
	  }
	*/
	while(!inputfile.eof())
	  {
	    //vector< string > Results;
	    getline(inputfile,line);
	    
	    int firstToken = line.find_first_of(' ');
	    int secondToken = line.find_last_of(' ');
	    //cout << firstToken << secondToken << endl;
	    if (firstToken == -1)
	      {
		continue;
	      }
	    neuron = atoi(line.substr(0,firstToken).c_str());
	    //neuron = atoi(line.substr(firstToken+1,secondToken-firstToken).c_str());
	    spiketime = atof(line.substr(secondToken+1).c_str());
	    //cout << "[ " << line << " ]" << group << neuron << spiketime  << endl;
	    
	    //boost::algorithm::split_regex(Results,line,boost::regex(" "));
	    if (spiketime >= startSfF)
	      {
		spiketime -= startSfF;
		if (spiketime <= duration)
		  {
		    SpikeTimes[neuron].insert((spiketime)+onset);
		    //message("fT: "+Str(firstToken)+" sT: "+Str(secondToken)+"spiketime Used: "+Str(spiketime)+" neuron "+Str(neuron));
		  }
	      }
	  }
	
	
      }
    
    //message("a"+Str((inputfile.fail()==1 ? "true" : "false")));
    

    
    /*while (file)
      {
	file >> group >> neuron >> time;
	//cout << time << endl;
	message("spiketime: "+Str(time));
	if (time >= startSfF)
	  {
	    time -= startSfF;
	    if (time <= duration)
	      {
		SpikeTimes[neuron].insert((time)+onset);
		//cout << time << endl;
		message("spiketime Used: "+Str(time));
	      }
	  }
      }
    */
    inputfile.close();
    message("Loading spikes done");
   
  }
  

    
   
  

EventData FeedForwardNetwork::convergentInput(const vector<EventData> &SpikeTimes,double delay){
  // 
  double cp = 1-number("cp");
  double p = 0.;
  double jitter=0.;
  // 
  EventData InputSpikeTimes;
  int numberOfRepeats = 1;
  for (int h=0;h<numberOfRepeats;h++){
		for (int preneuron = 0;preneuron<number("nExc");preneuron++){
			p = rngNetwork.uniform();
			if (p>=cp){
			  //message("preneuron "+Str(preneuron)+" spikes "+Str(SpikeTimes[preneuron].size()));
			  EventData ed;
			  ed.reserve(10000);
			  jitter = rngNetwork.gaussian()*number("synapse_jitter");
			  for(int h=0;h<SpikeTimes[preneuron].size();h++){
			    ed.insert(SpikeTimes[preneuron][h]+delay+jitter);
			  }
			  InputSpikeTimes.insert(ed);
			}
		}
	}
		
	return InputSpikeTimes;
}

void FeedForwardNetwork::saveOnline(ofstream &dfvm,ofstream &dfge,ofstream &dfgi,ofstream &dfcurr,ofstream &dfsp,ofstream &dfsi,const SampleDataD &vm,const SampleDataD &ge,const SampleDataD &gi,const SampleDataD &curr,EventData &sp,double &si,int group, int neuron){
  unlockAll();  
  // traces
  for (int i=0;i<vm.size();i++){
    // vm
    dfvm << vm[i];
    dfvm << " ";
    // ge
    dfge << ge[i];
    dfge << " ";
    // gi
    dfgi << gi[i];
    dfgi << " ";
  }
  dfvm << "\n";
  dfge << "\n";
  dfgi << "\n";
  // if we record currents
  if (index("rec_curr")==0){
    for (int i=0;i<curr.size();i++){
      // curr
      dfcurr << curr[i];
      dfcurr << " ";
    }
    dfcurr << "\n";
  }
  // spikes
  for (int spike=0;spike<sp.size();spike++){
    Str line = Str(group)+" "+Str(neuron)+" "+Str(sp[spike])+"\n";
    dfsp << line;
  }

  // signal times
  Str line = Str(group)+" "+Str(neuron)+" "+Str(si)+"\n";
  dfsi << line;
  dfsi << "\n";
  lockAll();
}

void FeedForwardNetwork::saveSettings(){
  // settings at start
  ofstream dfs(addPath( "ffn_settings_default_"+Str(completeRuns())+".dat" ).c_str());
  Header.save( dfs, "# " );
  stimulusData().save( dfs, "# ", 0, Options::FirstOnly );
  settings().save( dfs, "# ", 0, Options::FirstOnly );
  dfs << '\n';
  

  // settings during repro, might be changed during run
  ofstream df(addPath( "ffn_settings_"+Str(completeRuns())+".dat" ).c_str());
  Str parameters[] = {"nGroups","nExc","cp","JeFFN","scaleJeFFN","gFFN1","gFFN2","delay","gateDelay1","gateDelay2","gateGroup","noise_std","noise_mean","seedNetwork","E_ex","E_in","tau_syn_exc","tau_syn_inh","onset1","onset2","duration","seedStimulus","pause","poissonrate","JeBKG","gBKG","seedBKG","alpha1","sigma1","alpha2","sigma2","poissonstimulusrate1","poissonstimulusrate2","duration1","duration2","calibrationalpha","calibrationsigma","calibrationtrials","JStep","calibrationduration"};
  
  df << "# ";
  for (int p=0;p<40;p++){
    df << parameters[p] + " ";
}
  df << "\n";
  for (int p=0;p<40;p++){
    df << Str(number(parameters[p])) + " ";
  }
  df << "\n";

  message("Saving Settings done");

}

  void FeedForwardNetwork::saveEvents(const vector<vector<EventData> > &SpikeTimes,Str name){
  
  ofstream df(addPath( "ffn_"+name+'_'+Str(completeRuns())+".dat" ).c_str());
  
  df << "# group neuron spiketime\n";
  // SpikeTimes we save even in group 0, because this is the stimulus
  for (unsigned int group=0;group<SpikeTimes.size();group++){
    //message("group "+Str(group));
    for (unsigned int neuron=0;neuron<SpikeTimes[0].size();neuron++){
      //message("neuron "+Str(neuron));
      for (int spike=0;spike<SpikeTimes[group][neuron].size();spike++){
	//message('spike '+Str(SpikeTimes[group][neuron][spike]));
	Str line = Str(group)+" "+Str(neuron)+" "+Str(SpikeTimes[group][neuron][spike])+"\n";
	df << line;
      }
      
    }
  }
  message("Saving "+name+"done");
  }  
/*
  ofstream dfs(addPath( "ffn_signal_times_"+name+'_'+Str(completeRuns())+".dat" ).c_str());
  // SignalTimes we dont save from group 0, since all are 0, because group 0 is stimulus
  dfs << "# group neuron signaltimes\n";
  for (int group=1;group<SpikeTimes.size();group++){
    for (int neuron=0;neuron<SpikeTimes[0].size();neuron++){
      Str line = Str(group)+" "+Str(neuron)+" "+Str(SignalTimes[group][neuron])+"\n";
      dfs << line;
    }
  }
  message("Saving Signal Times done");
	
}
  */

void FeedForwardNetwork::saveTraces(const vector<vector<SampleDataD> > &ge,const vector<vector<SampleDataD> > &gi, const vector<vector<SampleDataD> > &vm,const vector<vector<SampleDataD> > &curr,double duration,Str name){
    ofstream dfge(addPath( "ffn_ge_trace_"+name+'_'+Str(completeRuns())+".dat" ).c_str());
    ofstream dfgi(addPath( "ffn_gi_trace_"+name+'_'+Str(completeRuns())+".dat" ).c_str());
    ofstream dfvm(addPath( "ffn_vm_trace_"+name+'_'+Str(completeRuns())+".dat" ).c_str());
    //if (index("rec_curr")==0){
    ofstream dfcurr(addPath( "ffn_curr_trace_"+name+'_'+Str(completeRuns())+".dat" ).c_str());
    
    ofstream dftime(addPath( "ffn_time_trace_"+name+'_'+Str(completeRuns())+".dat" ).c_str());
    
    message("ge shape: "+Str(ge.size()));
    message("ge shape: "+Str(ge[0].size()));
    
    // vm trace
    //const InData &intrace = trace( SpikeTrace[0] );
    double samplerate = trace( SpikeTrace[0] ).sampleRate();
    // time trace
    double time = 0.;
    for (int i=0;i<ge[0][0].size();i++){
      dftime << time;
      dftime << " ";
      time += (1./samplerate);
    }
    dftime << "\n";
    for (unsigned int group=0;group<ge.size();group++){
      for (unsigned int neuron=0;neuron<ge[0].size();neuron++){
	
	//SampleDataD vm( 0.0,duration, 1.0/samplerate );
	//double signaltime = SignalTimes[group][neuron];
	//intrace.copy(signaltime,vm);
	
	for (int i=0;i<ge[group][neuron].size();i++){
	  // ge
	  dfge << ge[group][neuron][i];
	  dfge << " ";
	  // gi
	  dfgi << gi[group][neuron][i];
	  dfgi << " ";
	  // vm
	  dfvm << vm[group][neuron][i];
	  dfvm << " ";
	  if (index("rec_curr")==0){
	    // curr
	    dfcurr << curr[group][neuron][i];
	    dfcurr << " ";
	  }
	}
	dfge << "\n";
	dfgi << "\n";
	dfvm << "\n";
	if (index("rec_curr")==0){
	  dfcurr << "\n";
	}
	
      }
    }
    message("Saving Traces done");
    
    
  }
  
void FeedForwardNetwork::stimulate(SampleDataD &ge, SampleDataD &gi, SampleDataD &vm,SampleDataD &curr,EventData &SpikeTimes, double &signaltime, double duration,double noise_std, double noise_mean){
  message("cp : "+Str(number("cp")));
  message("scaleJeFFN : "+Str(number("scaleJeFFN")));
  message("gFFN1 : "+Str(number("gFFN1")));
  message("gFFN2 : "+Str(number("gFFN2")));
  message("gatedelay1 : "+Str(number("gateDelay1")));
  message("gatedelay2 : "+Str(number("gateDelay2")));
  //ge += 0.00001;
  //gi += 0.00001;
  
  //ge.save("/data/feedforwardnetwork/data/ge.data");
  //gi.save("/data/feedforwardnetwork/data/gi.data");
  
  //SampleDataD g(ge+gi);
  //double e1 = number("E_ex");
  //double e2 = number("E_in");
  //SampleDataD E(((ge*e1)+(gi*e2))/(ge+gi));
  
  //g.save("/data/feedforwardnetwork/data/g.data");
  //E.save("/data/feedforwardnetwork/data/E.data");
  double samplerate = trace( SpikeTrace[0] ).sampleRate();
  
  if ( outTraceIndex( "g" ) >= 0 && outTraceIndex( "E" ) >= 0) {
    // yes, we have dynamic clamp with "g" and "E"!
    // calculate conductance output, will be used in the live experiment, or once the simulation mode has dynamic clamp
    // g1(V-E1)+g2(V-E2)+...
    // (g1+g2)*(V-((g1*E1+g2*E2)/(g1+g2)))
    // g = g1+g2
    // E = (g1*E1+g2*E2)/(g1+g2)
    //message("We use dynamic clamp for stimulation -- yeah ");
    
    SampleDataD g(ge+gi);
    double e1 = number("E_ex");
    double e2 = number("E_in");
    SampleDataD E(((ge*e1)+(gi*e2))/(ge+gi+0.00001));
    


    double samplerate = trace( SpikeTrace[0] ).sampleRate();
    SampleDataD bkg( 0.0, number("duration"), 1.0/samplerate );
    //bkg.size();
    
    for (int pos=0;pos<bkg.size();pos++){
      bkg[pos] = rngBKG.gaussian();
      bkg[pos] *= noise_std;
      bkg[pos] += noise_mean;
    }
    //SampleDataD gg( 0.0, number("duration"), 1.0/samplerate );
    //SampleDataD EE( 0.0, number("duration"), 1.0/samplerate );
    //gg *= 0.;
    //EE *= 0.;
    
    OutList signalL; 
       
    signalL.resize( 3 );
    signalL[0] = OutData(g);
    signalL[0].setTraceName( "g" );
    signalL[0].back() = 0;   // XXX this sets the whole stimulus to zero!!!
    signalL[1] = OutData(E);
    signalL[1].setTraceName( "E" );
    signalL[1].back() = 0;   // XXX this sets the whole stimulus to zero!!!
    
    signalL[2] = OutData(bkg);
    signalL[2].setTraceName("Current-1");
    signalL[2].back() = 0;   // XXX this sets the whole stimulus to zero!!!

    sleep( number("pre_pause")); 
    
    write( signalL );
    if ( signalL.failed() ) {
      warning( signalL.errorText() );
      //directWrite( dcsignal );
      //return Failed;
    }

    sleep( number( "pause" ) ); 

  }
  else {
    // Let's simply put out some stimulus as a current:
    // input as currents
    message("We put out the stimulus as a current -- buh");
    SampleDataD istim(ge-gi);

    for (int pos=0;pos<istim.size();pos++){
      istim[pos] = rngBKG.gaussian();
      istim[pos] *= noise_std;
      istim[pos] += noise_mean;
    }

    //istim -= gi;
    
    // OutData
    OutData signal(istim);
    signal.setTraceName("Current-1");//setTrace(CurrentOutput[0]); //setTraceName("Current-1");
    signal.back() = 0;
    
    sleep( number("pre_pause")); 
    
    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      //directWrite( dcsignal );
      //return Failed;
    }
  
    sleep( number( "pause" ) ); 

  }
  
  signaltime = signalTime();
  
  if ( ! interrupt() ) {
    // Calculate spiking response
    EventData spikes = events(SpikeEvents[0]);
    // This should copy the spikes between signaltime and signaltime+duration, with a reference of signaltime 
    spikes.copy(signaltime,signaltime+duration,signaltime,SpikeTimes);  
    // cut out vm
    SampleDataD vmtmp( 0.0,duration, 1.0/samplerate );
    SampleDataD ctmp( 0.0,duration, 1.0/samplerate );
    //double currentime = trace(SpikeTrace[0]).currentTime();
    if (trace(SpikeTrace[0]).currentTime()>signaltime+duration){
      const InData &intraceVM = trace( SpikeTrace[0] );
      intraceVM.copy(signaltime,vmtmp);
      if ( CurrentTrace[0] >= 0 && index("rec_curr")==0 ) {
	const InData &intraceC = trace( CurrentTrace[0] );
	intraceC.copy(signaltime,ctmp);
      }
    }
    vm = vmtmp;
    curr = ctmp;
  }
  /* we set the current to 0
  SampleDataD reset_curr( 0.0, 0.0001, 1./samplerate);
  OutData signal(reset_curr);
  signal.setTraceName("Current-1");//setTrace(CurrentOutput[0]); //setTraceName("Current-1");
  signal.back() = 0;
  write( signal );
  sleep(0.0001);
  */
}
  
int FeedForwardNetwork::calibrateFFN(double &JeFFN, double &JeBKG, double &gBKG, double &noise_std, double &noise_mean){
    /*
      We calibrate the neuron by:
      1.: Adjust the mean and std of the vm to target values. The mean vm is adjusted by increasing the inhibitory gain (gBKG), the std by increasing the synaptic weight (JeBKG)
      2. We ajdust the synaptic weight of the stimulus synapse (JeFFN). We stimulate the neuron with a defined pulse packet. This pulse packet is supposed to induce 1 spike. The neuron receives the exc&inh backgroun input that was adjusted in step 1. 
    */
    
    P.lock();
    P.resize( 1, 1, true );
    P.unlock();
    
    // init:
    // we use the calibration duration because we may need longer vm traces to calucate good mean and std
    double duration = number("calibrationduration");
    // RNG
    rngStimulus.setSeed(integer("seedStimulus"));
    double samplerate = trace( SpikeTrace[0] ).sampleRate();
    
    double JStep = number("JStep");
    GammaKernel gkexc(number("tau_syn_exc"),1);
    GammaKernel gkinh(number("tau_syn_inh"),1);
    
    double rate = number("poissonrate");
    double JebkgStep = number("JeBKGStep");
    
    double noise_std_step = number("noise_std_step");
    double noise_mean_step = number("noise_mean_step");
    double noise_mean_step_FFN = number("noise_mean_step_FFN");
    /*
    if (index("calibrateBKG")==0){
      // Data 
      // SpikeTimes as a vector of vector of EventData
      vector<vector<EventData> > SpikeTimesVector(2,vector<EventData> (integer("calibrationtrials")));
      // Conductances as a vector of vector of EventData
      vector<vector<SampleDataD> > GeVector(2,vector<SampleDataD> (integer("calibrationtrials")));
      vector<vector<SampleDataD> > GiVector(2,vector<SampleDataD> (integer("calibrationtrials")));
      // vm
      vector<vector<SampleDataD> > VmVector(2,vector<SampleDataD> (integer("calibrationtrials")));
      // Signaltimes
      vector<vector<double> > SignalTimesVector(2,vector<double> (integer("calibrationtrials")));
      
    // First we tune the background input
    // so far we use a fixed number of trials to find the good JeBKG and gBKG
    // it would be better to do that dynamically
    for (int trial=0;trial<integer("calibrationtrials");trial++){
      // Background input
      //EventData ExcBkg;
      //ExcBkg.poisson(rate,0.,duration,rngStimulus); // Exc Bkg
      //EventData InhBkg;
      //InhBkg.poisson(rate,0.,duration,rngStimulus); // Inh Bkg
      
      //SampleDataD ExcBkgCond( 0.0, duration, 1.0/samplerate );
      //ExcBkg.rate(ExcBkgCond,gkexc);
      //ExcBkgCond /= gkexc.max();
      //ExcBkgCond *= JeBKG;
      //SampleDataD InhBkgCond( 0.0, duration, 1.0/samplerate );
      //InhBkg.rate(InhBkgCond,gkinh);
      //InhBkgCond /= gkinh.max();
      //InhBkgCond *= JeBKG;
      //InhBkgCond *= gBKG;
      
      // combined data
      //SampleDataD ge(ExcBkgCond);
      //SampleDataD gi(InhBkgCond);
      SampleDataD ge(0.0, duration, 1.0/samplerate );
      SampleDataD gi(0.0, duration, 1.0/samplerate );
      GeVector[1][trial] = ge;
      GiVector[1][trial] = gi;
      

      
      // stimulation
      //EventData SpikeTimesResponse;
      //double sigt;
      //stimulate(ge,gi,VmVector[1][trial],SpikeTimesVector[1][trial],SignalTimesVector[1][trial],duration,0.1,0.);
      stimulate(ge,gi,VmVector[1][trial],SpikeTimesVector[1][trial],SignalTimesVector[1][trial],duration,noise_std,noise_mean);
      if ( interrupt() ) {
	return 0;
	// XXX Break is not enough here!
	// XXX You need to make sure to quit the RePro as quick as possible!
      }
      // plot trace:
      //plotToggle( true, true, duration, 0.0);
      tracePlotSignal( duration );
      traceplot(ge,gi,0,duration);
            
      // vm trace
      //const InData &intrace = trace( SpikeTrace[0] );
      //double samplerate = trace( SpikeTrace[0] ).sampleRate();
      //SampleDataD vm( 0.0, duration, 1.0/samplerate );
      //intrace.copy(signalTime(),vm);
      
      //double meanvm = VmVector[1][trial].mean(0.,duration);
      //double stdvm = VmVector[1][trial].stdev(0.,duration);
      
      if (meanvm<number("targetMean")){
	//gBKG -= 0.05;
	noise_mean += noise_mean_step;
      }
      else if (meanvm >number("targetMean")){
	//gBKG += 0.05;
	noise_mean -= noise_mean_step;
      }
      
      if (stdvm < number("targetStd")){
	//JeBKG += 0.01;
	noise_std += noise_std_step;
      }
      else if (stdvm > number("targetStd")){
	//JeBKG -= 0.01;
	noise_std -= noise_std_step;
      }
      
      //if (SpikeTimesVector[1][trial].size()>1){
	//JeBKG -= JebkgStep;
      //}
      
      }
      
    //saveEvents(SpikeTimesVector,"spike_times_bkg_calibration");
    //saveTraces(GeVector,GiVector,VmVector,duration,"bkg_calibration");
    
    
    }
    */

    if (index("calibrateNoise")==0){
      // Data 
      // SpikeTimes as a vector of vector of EventData
      vector<vector<EventData> > SpikeTimesVector(2,vector<EventData> (integer("calibrationtrials")));
      // Conductances as a vector of vector of EventData
      vector<vector<SampleDataD> > GeVector(2,vector<SampleDataD> (integer("calibrationtrials")));
      vector<vector<SampleDataD> > GiVector(2,vector<SampleDataD> (integer("calibrationtrials")));
      // Vm
      vector<vector<SampleDataD> > VmVector(2,vector<SampleDataD> (integer("calibrationtrials")));
      // Curr
      vector<vector<SampleDataD> > CurrVector(2,vector<SampleDataD> (integer("calibrationtrials")));
      // Signaltimes
      vector<vector<double> > SignalTimesVector(2,vector<double> (integer("calibrationtrials")));
      
      // JeFFN
      vector<double> JeFFNVector(vector<double> (integer("calibrationtrials")));
    // Now we tune the stimulus
    // we change duration to the regular duration because we are only interested in the spiking response
    duration = number("duration");
    message("Start with stimulus");
    for (int trial=0;trial<integer("calibrationtrials");trial++){
      /*
      EventData SpikeTimes;
      SpikeTimes.reserve(1000);
      double time=0.;
      for (int i=0;i<integer("calibrationalpha");i++){
	time = rngStimulus.gaussian();
	SpikeTimes.insert((number("calibrationsigma")*time)+number("onset1"));
      }
      SpikeTimesVector[0][trial] = SpikeTimes;
      */
      SampleDataD ExcStimCond( 0.0, duration, 1.0/samplerate );
      
      //SpikeTimes.rate(ExcStimCond,gkexc);
      //ExcStimCond /= gkexc.max(); 
      //ExcStimCond *= JeFFN;
      // Background input
      //EventData ExcBkg;
      //ExcBkg.poisson(rate,0.,duration,rngStimulus); // Exc Bkg
      //EventData InhBkg;
      //InhBkg.poisson(rate,0.,duration,rngStimulus); // Inh Bkg
      
      
      //SampleDataD ExcBkgCond( 0.0, duration, 1.0/samplerate );
      //ExcBkg.rate(ExcBkgCond,gkexc);
      //ExcBkgCond /= gkexc.max();
      //ExcBkgCond *= JeBKG;
      //SampleDataD InhBkgCond( 0.0, duration, 1.0/samplerate );
      //InhBkg.rate(InhBkgCond,gkinh);
      //InhBkgCond /= gkinh.max();
      //InhBkgCond *= JeBKG;
      //InhBkgCond *= gBKG;
      
      // combined data
      SampleDataD ge(ExcStimCond);
      SampleDataD gi(ExcStimCond);
      GeVector[1][trial] = ge;
      GiVector[1][trial] = gi;
      
      //
      stimulate(ge,gi,VmVector[1][trial],CurrVector[1][trial],SpikeTimesVector[1][trial],SignalTimesVector[1][trial],duration,noise_std,noise_mean);
      //EventData SpikeTimesResponse;
      //double sigt;
      //stimulate(ge,gi,SpikeTimesResponse,sigt,duration);
      if ( interrupt() ) {
	return 0;
      }
      
      // plot trace:
      //plotToggle( true, true, duration, 0.0);
      tracePlotSignal( duration );
      traceplot(ge,gi,0,duration);
      
      
      double vmstdev = VmVector[1][trial].stdev(0.,duration);
      double vmmean = VmVector[1][trial].mean(0.,duration);
      message("mean "+Str(vmmean)+" std "+Str(vmstdev));
      
      if (vmmean < number("targetMean")){
	noise_mean += noise_mean_step;
      }
      if (vmmean > number("targetMean")){
	noise_mean -= noise_mean_step;
      }
      if (vmstdev < number("targetStd")){
	noise_std += noise_std_step;
      }
      if (vmstdev > number("targetStd")){
	noise_std -= noise_std_step;
      }	      
	
      
      
      
    }
    saveEvents(SpikeTimesVector,"spike_times_noise_calibration");
    saveTraces(GeVector,GiVector,VmVector,CurrVector,duration,"noise_calibration");
    }



    if (index("calibrateFFN")==0){
      // Data 
      // SpikeTimes as a vector of vector of EventData
      vector<vector<EventData> > SpikeTimesVector(2,vector<EventData> (integer("calibrationtrials")));
      // Conductances as a vector of vector of EventData
      vector<vector<SampleDataD> > GeVector(2,vector<SampleDataD> (integer("calibrationtrials")));
      vector<vector<SampleDataD> > GiVector(2,vector<SampleDataD> (integer("calibrationtrials")));
      // Vm
      vector<vector<SampleDataD> > VmVector(2,vector<SampleDataD> (integer("calibrationtrials")));
      // Curr
      vector<vector<SampleDataD> > CurrVector(2,vector<SampleDataD> (integer("calibrationtrials")));
      // Signaltimes
      vector<vector<double> > SignalTimesVector(2,vector<double> (integer("calibrationtrials")));
      
      // JeFFN
      vector<double> JeFFNVector(vector<double> (integer("calibrationtrials")));
    // Now we tune the stimulus
    // we change duration to the regular duration because we are only interested in the spiking response
    duration = number("duration");
    message("Start with stimulus");
    int calibrationtrials = integer("calibrationtrials");
    int calibrationalpha = integer("calibrationalpha");
    double calibrationsigma = number("calibrationsigma");
    double onset1 = number("onset1");
    for (int trial=0;trial<calibrationtrials;trial++){
      unlockAll();
      EventData SpikeTimes;
      SpikeTimes.reserve(1000);
      double time=0.;
      for (int i=0;i<calibrationalpha;i++){
	time = rngStimulus.gaussian();
	SpikeTimes.insert((calibrationsigma*time)+onset1);
      }
      SpikeTimesVector[0][trial] = SpikeTimes;

      SampleDataD ExcStimCond( 0.0, duration, 1.0/samplerate );
    
      SpikeTimes.rate(ExcStimCond,gkexc);
      ExcStimCond /= gkexc.max(); 
      ExcStimCond *= JeFFN;
      // Background input
      EventData ExcBkg;
      ExcBkg.poisson(rate,0.,duration,rngStimulus); // Exc Bkg
      EventData InhBkg;
      InhBkg.poisson(rate,0.,duration,rngStimulus); // Inh Bkg
      
      
      SampleDataD ExcBkgCond( 0.0, duration, 1.0/samplerate );
      ExcBkg.rate(ExcBkgCond,gkexc);
      ExcBkgCond /= gkexc.max();
      ExcBkgCond *= JeBKG;
      SampleDataD InhBkgCond( 0.0, duration, 1.0/samplerate );
      InhBkg.rate(InhBkgCond,gkinh);
      InhBkgCond /= gkinh.max();
      InhBkgCond *= JeBKG;
      InhBkgCond *= gBKG;
      
      // combined data
      SampleDataD ge(ExcBkgCond+ExcStimCond);
      SampleDataD gi(InhBkgCond);
      GeVector[1][trial] = ge;
      GiVector[1][trial] = gi;
      
      lockAll();

      //
      stimulate(ge,gi,VmVector[1][trial],CurrVector[1][trial],SpikeTimesVector[1][trial],SignalTimesVector[1][trial],duration,noise_std,noise_mean);
      //EventData SpikeTimesResponse;
      //double sigt;
      //stimulate(ge,gi,SpikeTimesResponse,sigt,duration);
      if ( interrupt() ) {
	return 0;
      }
      
      // plot trace:
      //plotToggle( true, true, duration, 0.0);
      tracePlotSignal( duration );
      traceplot(ge,gi,0,duration);
      
      JeFFNVector[trial] = JeFFN;
      // update Je
      if (SpikeTimesVector[1][trial].size()<1){
      	JeFFN += JStep;
	//noise_mean += noise_mean_step_FFN;
      }
      else if (SpikeTimesVector[1][trial].size()>1){
	//noise_mean -= noise_mean_step_FFN;
	
	JeFFN -= JStep/2.;
      }
      else if (SpikeTimesVector[1][trial].size()==1){
	
      }
      message("Noise mean: "+Str(noise_mean));
      
    }
    saveEvents(SpikeTimesVector,"spike_times_ffn_calibration");
    saveTraces(GeVector,GiVector,VmVector,CurrVector,duration,"ffn_calibration");
    }
      
    P.lock();
    P.resize( 2, 1, true );
    P.unlock();
    return 1;
  }
  
  
  addRePro( FeedForwardNetwork, patchclampprojects );
 
}; /* namespace patchclampprojects */

#include "moc_feedforwardnetwork.cc"

