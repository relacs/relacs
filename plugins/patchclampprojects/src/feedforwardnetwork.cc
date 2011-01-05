/*
  patchclampprojects/feedforwardnetwork.cc
  Generates a FFN in an iterative way with one neuron, like in Reyes 2003 Nat Neurosci 6(6):593-599
  Feedforward inhibition in the FFN is included like in Kremkow et al. 2010 J Comp Neurosci 28(3):579:594

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

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <relacs/patchclampprojects/feedforwardnetwork.h>
#include <relacs/kernel.h>
#include <relacs/random.h>
#include <relacs/eventlist.h>
#include <vector>
#include <fstream>
#include <relacs/tablekey.h>



using namespace std;
using namespace relacs;

namespace patchclampprojects {


FeedForwardNetwork::FeedForwardNetwork( void )
  : RePro( "FeedForwardNetwork", "patchclampprojects", "Jens Kremkow", "1.0", "Oct 29, 2010" )
{
  
  // add some options:
  addLabel( "Network" );
  addInteger( "nGroups", "Number of groups", 4, 1, 100, 1 );
  addInteger( "nExc", "Number of Exc neurons in each group", 100, 1, 1000, 1 );
  addInteger( "nInh", "Number of Inh neurons in each group", 25, 1, 1000, 1 );
  addNumber( "cp", "Connection probability", 0.5, 0.0, 1.0, 0.001 );
  addNumber( "JeFFN", "Exc. Synaptic strength", 0.1, 0.0, 1000.0, 0.1 );
  addNumber( "scaleJeFFN", "Exc. Synaptic scaling", 1., 0.0, 100.0, 0.01 );
  addNumber( "gFFN", "Inhibitory gain", 0.1, 0.0, 100.0, 0.1 );
  addNumber( "delay", "Delay between Exc & Inh", 0.003, -10.0, 10.0, 0.1 ,"sec","ms");
  addNumber("intergroupdelay","Delay between groups",0.0,0.,1.,0.001,"sec","ms");
  addNumber( "gateDelay", "Delay between Exc & Inh in gate", 0., -10.0, 10.0, 0.1 ,"sec","ms");
  addInteger("gateGroup","Gate Group",2,0,100);
  addInteger("seedNetwork","RNG seed of the network",2,1000,1);
  addSelection("calibrateBKG","Background calibration","yes|no");
  addSelection("calibrateFFN","FFN calibration","yes|no");
  addSelection("stimulate","Stimulate","yes|no");
  
  
  addLabel("Synapses");
  addNumber("E_ex","Excitatory reversal potential",0.0,-100.,100.,0.1,"mV");
  addNumber("E_in","Inhibitory reversal potential",-85.0,-100.,100.,0.1,"mV");
  addNumber("tau_syn_exc","Excitatory synaptic time-constant",0.001,0.0,1.,0.00001,"sec","ms");
  addNumber("tau_syn_inh","Inhibitory synaptic time-constant",0.01,0.0,1.,0.00001,"sec","ms");
  
  addLabel("Stimuli");
  addSelection("stimulus","Stimulus type","pulse packet|MIP|poisson");
  addNumber("onset","Onset",0.05,0.,10000.,1,"sec","ms");
  addNumber( "duration", "Duration", 0.1, 0.01, 1000.0, 0.01,"sec","ms");
  addInteger("seedStimulus","RNG seed of the stimulus",1,1000,1);
  addNumber("pause","Pause",0.,0.,10.,0.1,"sec","ms");
    
  addLabel("Background Network");
  addNumber("poissonrate","Rate of background input",5000.,0.,50000.,1.,"Hz");
  addNumber( "JeBKG", "Exc. Synaptic strength", 0.3, 0.0, 1000.0, 0.1 );
  addNumber( "gBKG", "Inhibitory gain", 1., 0.0, 100.0, 0.1 );
  addInteger("seedBKG","RNG seed of the BKG",1,1000,1);
  
  addLabel("Pulse Packet");
  addNumber("alpha","Number of spikes",1,1,1000.,1);
  addNumber("sigma","Temporal spread",0.002,0.00001,1000.,0.1,"sec","ms");	
  
  addLabel("Poisson");
  addNumber("poissonstimulusrate","Rate",0.,0.,1000.,0.1,"Hz");
  
  addLabel("MIP");
  addNumber("MIPstimulusrate","Rate",10.,0.00001,1000.,0.1,"Hz" );
  addNumber("MIPcorr","Correlation",0.0,0.0,1.,0.01);
    
  addLabel("Calibration");
  addInteger("calibrationtrials","Trials",30,1,100,1);
  addNumber("calibrationalpha","Number of spikes",60,1,1000.,1);
  addNumber("calibrationsigma","Temporal spread",0.002,0.00001,1000.,0.1,"sec","ms");
  addNumber("JStep","Synaptic Weight Change Step",0.01,0.,100.,0.001);
  addNumber("JeBKGStep","Synaptic Weight Change Step Bkg",0.01,0.,100.,0.001);
  addNumber("targetMean","Target Mean vm",-70.,-100.,0.,0.01);
  addNumber("targetStd","Target Std vm",3.,0.,100,0.001);
  addNumber( "calibrationduration", "Duration", 1., 0.01, 1000.0, 0.01,"sec","ms");
  
    
  addTypeStyle( OptWidget::TabLabel, Parameter::Label );
  
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
  
  double gFFN = number("gFFN");
  double gBKG = number("gBKG");
  int nGroups = integer("nGroups");
  int nExc = integer("nExc");
  //int nInh = integer("nInh");
  
  // Stimulus
  double onset = number("onset");
  double duration = number("duration");
  message("Duration: "+Str(duration));
  
  // RNG
  rngNetwork.setSeed(integer("seedNetwork"));
  rngStimulus.setSeed(integer("seedStimulus"));
  rngBKG.setSeed(integer("seedBKG"));
  
  // Data 
  // SpikeTimes as a vector of vector of EventData
  vector<vector<EventData> > SpikeTimesVector(nGroups,vector<EventData> (nExc));
  
  // Conductances as a vector of vector of EventData
  vector<vector<SampleDataD> > GeVector(nGroups,vector<SampleDataD> (nExc));
  vector<vector<SampleDataD> > GiVector(nGroups,vector<SampleDataD> (nExc));
  
  // Signaltimes
  vector<vector<double> > SignalTimesVector(nGroups,vector<double> (nExc));
  
  
  // calibration
  if (index("calibrateBKG")==0 || index("calibrateFFN")==0){
    message("We calibrate the FFN");
    message("JeFFN before calibration: "+Str(JeFFN));
    message("JeBKG before calibration: "+Str(JeBKG));
    message("gBKG before calibration: "+Str(gBKG));
    if (! calibrateFFN(JeFFN,JeBKG,gBKG)){
	return Completed;
    }
    setNumber("JeFFN",JeFFN);
    setNumber("JeBKG",JeBKG);
    setNumber("gBKG",gBKG);
    setToDefaults();
    message("JeFFN after calibration: "+Str(JeFFN));
    message("JeBKG after calibration: "+Str(JeBKG));
    message("gBKG after calibration: "+Str(gBKG));
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
    PulsePacket(SpikeTimesVector[0],integer("alpha"),number("sigma"),nExc,onset);
  }
  else if (index("stimulus")==1){
    MIP(SpikeTimesVector[0]);
    //return 0;
  }
  else if (index("stimulus")==2){
    message("Generate poisson stimulus with rate: "+Str(number("rate")));
    Poisson(SpikeTimesVector[0]);
  }
  
  // Plot stimulus
  P[0].clear();
  for (int neuron=0;neuron<integer("nExc");neuron++){
    rasterplot(SpikeTimesVector,0,neuron); 
  }
    
  // Generate FeedForwardNetwork
  // The FFN is generate by stimuluating each neuron a group with its convergent inputs from the preceeding group.
  GammaKernel gkexc(number("tau_syn_exc"),1);
  GammaKernel gkinh(number("tau_syn_inh"),1);
  // Groups
  for (int group=1;group<nGroups && softStop() == 0;group++){
    // Neuron in groups
    for (int neuron=0;neuron<nExc;neuron++){
      // Collect the convergent input spikes
      // Excitatory spikes
      EventData ExcInputSpikes = convergentInput(SpikeTimesVector[group-1],1,number("intergroupdelay"));
      // Inhibitory spikes
      // If we are in the gate group we have to change the delay
      double delay = 0.;
      if (group == integer("gateGroup")){
	delay = number("gateDelay");
      }
      else{
	delay = number("delay");
      }
      EventData InhInputSpikes = convergentInput(SpikeTimesVector[group-1],integer("nInh"),number("intergroupdelay")+delay);
            
      // Convert input spikes into conductance traces
      // Excitatory cond input
      SampleDataD ExcStimCond( 0.0, duration, 1.0/samplerate );
      ExcInputSpikes.rate(ExcStimCond,gkexc);
      ExcStimCond /= gkexc.max(); // 1 spike would have ampltiude 1
      ExcStimCond *= JeFFN; // scale by JeFFN
      ExcStimCond *= scaleJeFFN; // scale by scaleJeFFN
      // Inhibitory cond input
      SampleDataD InhStimCond( 0.0, duration, 1.0/samplerate );
      InhInputSpikes.rate(InhStimCond,gkinh);
      InhStimCond /= gkinh.max();
      InhStimCond *= JeFFN; // scale by JeFFN
      InhStimCond *= gFFN; // scale by gFFN
      
      // Background input
      EventData ExcBkg;
      ExcBkg.poisson(number("poissonrate"),0.,duration,rngBKG); // Exc Bkg
      EventData InhBkg;
      InhBkg.poisson(number("poissonrate"),0.,duration,rngBKG); // Inh Bkg
      
      
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
      SampleDataD gi(InhBkgCond+InhStimCond);
      
      
      // Save input
      GeVector[group][neuron] = ge;
      GiVector[group][neuron] = gi;
      
      // Stimulation
      stimulate(ge,gi,SpikeTimesVector[group][neuron],SignalTimesVector[group][neuron],duration);
      if ( interrupt() ) {
	break;
      }
      
      // plot trace:
      tracePlotSignal(duration);
      // plotToggle( true, true, duration, 0.0);// has been removed, changed to ??
      tracePlotSignal( duration );
      
      // plot rasters and vm and conductance
      rasterplot(SpikeTimesVector,group,neuron);
      traceplot(ge,gi,1,duration);
        
    }
    if ( interrupt() ) {
	break;
      }
  } 
  
  
  // save the data
  saveEvents(SpikeTimesVector,SignalTimesVector,"stimulation");
  saveTraces(GeVector,GiVector,SignalTimesVector,duration,"stimulation");
  saveSettings();
  return state;
  
}
  
void FeedForwardNetwork::rasterplot(const vector<vector<EventData> > &SpikeTimes,int group, int neuron){
  
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
  double ymin = 0;
  ymin = -1.*ymax*0.1;
  ymax = ymax+0.1*ymax;
  
    
  P[0].plotHLine((group*number("nExc"))-0.5, Plot::Green, 1 );
  
  EventData ed = SpikeTimes[group][neuron];
  for (int sp=0;sp<ed.size();sp++){
    x = ed[sp];
    y = neuron+(group*number("nExc"));
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


  void FeedForwardNetwork::PulsePacket(vector<EventData> &SpikeTimes, int alpha, double sigma, int groupsize, double onset)
{
	double time;
	for (int neuron=0;neuron<groupsize;neuron++){
	  for (int i=0;i<alpha;i++){
	    time = rngStimulus.gaussian();
	    SpikeTimes[neuron].insert((sigma*time)+onset);
	    //message("neuron "+Str(neuron)+" time "+Str(time)+" +onset "+Str((sigma*time)+onset));
	    //message("PP: neuron: "+Str(neuron)+" spikes "+Str(SpikeTimes[neuron].size()));
	  }
	}
}

void FeedForwardNetwork::Poisson(vector<EventData> &SpikeTimes)
  {
    //message("pg: "+Str(number("duration"))+"  rate "+Str(number("poissonstimulusrate")));
    for (int neuron=0;neuron<number("nExc");neuron++){
      EventData pgtmp;
      pgtmp.poisson(number("poissonstimulusrate"),0.0,number("duration")-number("onset"),rngStimulus);//-number("onset"));
      //message("pg "+Str(neuron)+" size "+Str(pgtmp.size()));
      for (int i=0;i<pgtmp.size();i++){
	pgtmp[i] += number("onset");
      }
      SpikeTimes[neuron] = pgtmp;
            
    }
  }

void FeedForwardNetwork::MIP(vector<EventData> &SpikeTimes)
  {
    // MIP process like in Kuhn et al. Neural Comput 2003
    double MIPrate = number("MIPstimulusrate")*(1./number("MIPcorr"));
    EventData pgMother;
    pgMother.poisson(MIPrate,0.,number("duration")-number("onset"),rngStimulus);
    
    message("Generate MIP stimulus with rate: "+Str(MIPrate)+" and correlation: "+Str(number("MIPcorr")));
    double cp = 1.-number("MIPcorr");
    double p = 0.;
    for (int neuron=0;neuron<integer("nExc");neuron++){
      EventData pgtmp;
      pgtmp.reserve(10000);
      for (int i=0;i<pgMother.size();i++){
	p = rngStimulus.uniform();
	if (p>=cp){
	  pgtmp.insert(pgMother[i]+number("onset"));
	}
      }
      SpikeTimes[neuron] = pgtmp;
      
    }
  }

EventData FeedForwardNetwork::convergentInput(const vector<EventData> &SpikeTimes,int numberOfRepeats,double delay){
  // 
  double cp = 1-number("cp");
  double p = 0.;
  // 
  EventData InputSpikeTimes;
  
  for (int h=0;h<numberOfRepeats;h++){
		for (int preneuron = 0;preneuron<number("nExc");preneuron++){
			p = rngNetwork.uniform();
			if (p>=cp){
			  //message("preneuron "+Str(preneuron)+" spikes "+Str(SpikeTimes[preneuron].size()));
			  EventData ed;
			  ed.reserve(10000);
			  for(int h=0;h<SpikeTimes[preneuron].size();h++){
			    ed.insert(SpikeTimes[preneuron][h]+delay);
			  }
			  InputSpikeTimes.insert(ed);
			}
		}
	}
		
	return InputSpikeTimes;
}


void FeedForwardNetwork::saveSettings(){
  // settings at start
  ofstream dfs(addPath( "ffn_settings_default_"+Str(completeRuns())+".dat" ).c_str());
  Header.save( dfs, "# " );
  dfs << "# settings:\n";
  settings().save( dfs, "#   " );
  dfs << '\n';
  

  // settings during repro, might be changed during run
  ofstream df(addPath( "ffn_settings_"+Str(completeRuns())+".dat" ).c_str());
  Str parameters[] = {"nGroups","nExc","nInh","cp","JeFFN","scaleJeFFN","gFFN","delay","intergroupdelay","gateDelay","gateGroup","seedNetwork","E_ex","E_in","tau_syn_exc","tau_syn_inh","onset","duration","seedStimulus","pause","poissonrate","JeBKG","gBKG","seedBKG","alpha","sigma","poissonstimulusrate"};
  df << "# ";
  for (int p=0;p<26;p++){
    df << parameters[p] + " ";
}
  df << "\n";
  for (int p=0;p<26;p++){
    df << Str(number(parameters[p])) + " ";
  }
  df << "\n";
  message("Saving Settings done");

}

  void FeedForwardNetwork::saveEvents(const vector<vector<EventData> > &SpikeTimes,const vector<vector<double> > &SignalTimes, Str name){
  
  ofstream df(addPath( "ffn_spike_times_"+name+'_'+Str(completeRuns())+".dat" ).c_str());
  
  df << "# group neuron spiketime\n";
  // SpikeTimes we save even in group 0, because this is the stimulus
  for (int group=0;group<SpikeTimes.size();group++){
    message("group "+Str(group));
    for (int neuron=0;neuron<SpikeTimes[0].size();neuron++){
      message("neuron "+Str(neuron));
      for (int spike=0;spike<SpikeTimes[group][neuron].size();spike++){
	message('spike '+Str(SpikeTimes[group][neuron][spike]));
	Str line = Str(group)+" "+Str(neuron)+" "+Str(SpikeTimes[group][neuron][spike])+"\n";
	df << line;
      }
      
    }
  }
  message("Saving Spikes done");
  
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

  void FeedForwardNetwork::saveTraces(const vector<vector<SampleDataD> > &ge,const vector<vector<SampleDataD> > &gi,const vector<vector<double> > &SignalTimes, double duration,Str name){
    ofstream dfge(addPath( "ffn_ge_trace_"+name+'_'+Str(completeRuns())+".dat" ).c_str());
    ofstream dfgi(addPath( "ffn_gi_trace_"+name+'_'+Str(completeRuns())+".dat" ).c_str());
    ofstream dfvm(addPath( "ffn_vm_trace_"+name+'_'+Str(completeRuns())+".dat" ).c_str());
    ofstream dftime(addPath( "ffn_time_trace_"+name+'_'+Str(completeRuns())+".dat" ).c_str());
    
    message("ge shape: "+Str(ge.size()));
    message("ge shape: "+Str(ge[0].size()));
    
    // vm trace
    const InData &intrace = trace( SpikeTrace[0] );
    double samplerate = trace( SpikeTrace[0] ).sampleRate();
    
    // time trace
    double time = 0.;
    for (int i=0;i<ge[1][0].size();i++){
      dftime << time;
      dftime << " ";
      time += (1./samplerate);
    }
    dftime << "\n";
    
    for (int group=1;group<ge.size();group++){
      for (int neuron=0;neuron<ge[0].size();neuron++){
	
	SampleDataD vm( 0.0,duration, 1.0/samplerate );
	double signaltime = SignalTimes[group][neuron];
	intrace.copy(signaltime,vm);
	
	for (int i=0;i<ge[group][neuron].size();i++){
	  // ge
	  dfge << ge[group][neuron][i];
	  dfge << " ";
	  // gi
	  dfgi << gi[group][neuron][i];
	  dfgi << " ";
	  // vm
	  dfvm << vm[i];
	  dfvm << " ";
	}
	dfge << "\n";
	dfgi << "\n";
	dfvm << "\n";
	
      }
    }
    message("Saving Traces done");
    
    
  }
  
void FeedForwardNetwork::stimulate(SampleDataD &ge, SampleDataD &gi, EventData &SpikeTimes, double &signaltime, double duration){
   
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
    


    //double samplerate = trace( SpikeTrace[0] ).sampleRate();
    //SampleDataD gg( 0.0, number("duration"), 1.0/samplerate );
    //SampleDataD EE( 0.0, number("duration"), 1.0/samplerate );
    //gg *= 0.;
    //EE *= 0.;
    
    OutList signalL; 
       
    signalL.resize( 2 );
    signalL[0] = OutData(g);
    signalL[0].setTraceName( "g" );
    //signalL[0].addDescription( "stimulus/value" );
    signalL[0].back() = 0;
    signalL[1] = OutData(E);
    signalL[1].setTraceName( "E" );
    //signalL[1].addDescription( "stimulus/value" );
    signalL[1].back() = 0;

    write( signalL );
    if ( signalL.failed() ) {
      warning( signalL.errorText() );
      //directWrite( dcsignal );
      //return Failed;
    }

    sleep( duration + number("pause")); 

  }
  else {
    // Let's simply put out some stimulus as a current:
    // input as currents
    message("We put out the stimulus as a current -- buh");
    SampleDataD istim(ge-gi);
    //istim -= gi;
    
    // OutData
    OutData signal(istim);
    signal.setTraceName("Current-1");//setTrace(CurrentOutput[0]); //setTraceName("Current-1");
    signal.back() = 0;
    
    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      //directWrite( dcsignal );
      //return Failed;
    }
  
    sleep( duration + number("pause")); 

  }
  
  signaltime = signalTime();
  if ( ! interrupt() ) {
    // Calculate spiking response
    EventData spikes = events(SpikeEvents[0]);
    // This should copy the spikes between signaltime and signaltime+duration, with a reference of signaltime 
    spikes.copy(signaltime,signaltime+duration,signaltime,SpikeTimes);
  }
  
}
  
  int FeedForwardNetwork::calibrateFFN(double &JeFFN, double &JeBKG, double &gBKG){
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
    
    if (index("calibrateBKG")==0){
      // Data 
      // SpikeTimes as a vector of vector of EventData
      vector<vector<EventData> > SpikeTimesVector(2,vector<EventData> (integer("calibrationtrials")));
      // Conductances as a vector of vector of EventData
      vector<vector<SampleDataD> > GeVector(2,vector<SampleDataD> (integer("calibrationtrials")));
      vector<vector<SampleDataD> > GiVector(2,vector<SampleDataD> (integer("calibrationtrials")));
      // Signaltimes
      vector<vector<double> > SignalTimesVector(2,vector<double> (integer("calibrationtrials")));
  
    // First we tune the background input
    // so far we use a fixed number of trials to find the good JeBKG and gBKG
    // it would be better to do that dynamically
    for (int trial=0;trial<integer("calibrationtrials");trial++){
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
      SampleDataD ge(ExcBkgCond);
      SampleDataD gi(InhBkgCond);
      GeVector[1][trial] = ge;
      GiVector[1][trial] = gi;
      
      // stimulation
      //EventData SpikeTimesResponse;
      //double sigt;
      stimulate(ge,gi,SpikeTimesVector[1][trial],SignalTimesVector[1][trial],duration);
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
      const InData &intrace = trace( SpikeTrace[0] );
      double samplerate = trace( SpikeTrace[0] ).sampleRate();
      SampleDataD vm( 0.0, duration, 1.0/samplerate );
      intrace.copy(signalTime(),vm);
      
      double meanvm = vm.mean(0.,duration);
      double stdvm = vm.stdev(0.,duration);
      
      if (meanvm<number("targetMean")){
	gBKG -= 0.05;
      }
      else if (meanvm >number("targetMean")){
	gBKG += 0.05;
      }
      
      if (stdvm < number("targetStd")){
	JeBKG += 0.01;
      }
      else if (stdvm > number("targetStd")){
	JeBKG -= 0.01;
      }
      
      if (SpikeTimesVector[1][trial].size()>1){
	JeBKG -= JebkgStep;
      }
    }
    saveEvents(SpikeTimesVector,SignalTimesVector,"bkg_calibration");
    saveTraces(GeVector,GiVector,SignalTimesVector,duration,"bkg_calibration");
      
    
    }
    
    if (index("calibrateFFN")==0){
      // Data 
      // SpikeTimes as a vector of vector of EventData
      vector<vector<EventData> > SpikeTimesVector(2,vector<EventData> (integer("calibrationtrials")));
      // Conductances as a vector of vector of EventData
      vector<vector<SampleDataD> > GeVector(2,vector<SampleDataD> (integer("calibrationtrials")));
      vector<vector<SampleDataD> > GiVector(2,vector<SampleDataD> (integer("calibrationtrials")));
      // Signaltimes
      vector<vector<double> > SignalTimesVector(2,vector<double> (integer("calibrationtrials")));
      
    // Now we tune the stimulus
    // we change duration to the regular duration because we are only interested in the spiking response
    duration = number("duration");
    message("Start with stimulus");
    for (int trial=0;trial<integer("calibrationtrials");trial++){
      EventData SpikeTimes;
      SpikeTimes.reserve(1000);
      double time=0.;
      for (int i=0;i<integer("calibrationalpha");i++){
	time = rngStimulus.gaussian();
	SpikeTimes.insert((number("calibrationsigma")*time)+number("onset"));
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
      
      //
      stimulate(ge,gi,SpikeTimesVector[1][trial],SignalTimesVector[1][trial],duration);
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
      
      // update Je
      if (SpikeTimesVector[1][trial].size()<1){
	JeFFN += JStep;
      }
      else if (SpikeTimesVector[1][trial].size()>1){
	JeFFN -= JStep/2.;
      }
      else if (SpikeTimesVector[1][trial].size()==1){
	
      }
      
      
    }
    saveEvents(SpikeTimesVector,SignalTimesVector,"ffn_calibration");
    saveTraces(GeVector,GiVector,SignalTimesVector,duration,"ffn_calibration");
    }
      
    P.lock();
    P.resize( 2, 1, true );
    P.unlock();
    return 1;
  }
  
  
 addRePro( FeedForwardNetwork );
 
}; /* namespace patchclampprojects */

#include "moc_feedforwardnetwork.cc"

