/*
  ephys/neuronmodels.cc
  Various models of spiking (point-) neurons.

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

#include <cmath>
#include <relacs/optwidget.h>
#include <relacs/random.h>
#include <relacs/odealgorithm.h>
#include <relacs/ephys/neuronmodels.h>
using namespace relacs;

namespace ephys {


NeuronModels::NeuronModels( void )
  : Model( "NeuronModels", "ephys", "Jan Benda", "1.0", "Jan 10, 2006" )
{
  addOptions();
  addModels();
  MMCInx = -1;
  MMHCInx = -1;
  HMHCInx = -1;
  VCMode = false;
}


NeuronModels::NeuronModels( const string &name,
			    const string &pluginset,
			    const string &author, 
			    const string &version,
			    const string &date )
  : Model( name, pluginset, author, version, date )
{
}


NeuronModels::~NeuronModels( void )
{
  for ( unsigned int k=0; k<Models.size(); k++ )
    delete Models[k];
  Models.clear();
  Titles.clear();
}


void NeuronModels::main( void )
{
  readOptions();

  // traces:
  int traceinx[2] = { -1, -1 };
  for ( int k=0; k<traces(); k++ ) {
    if ( traceName( k ) == "V-1" )
      traceinx[0] = k;
    else if ( traceName( k ) == "Current-1" )
      traceinx[1] = k;
    else if ( trace( k ).channel() < InData::ParamChannel )
      warning( "Input trace <b>" + traceName( k ) + "</b> not known to NeuronModels!" );
  }

  // deltat( 0 ) must be integer multiple of delta t for integration:
  int maxs = int( ::floor( 1000.0*deltat( 0 )/timeStep() ) );
  if ( maxs <= 0 )
    maxs = 1;
  setTimeStep( 1000.0 * deltat( 0 ) / maxs );
  int cs = 0;
  setNoiseFac();

  // state variables:
  int simn = neuron()->dimension();
  if ( GMC > 1e-8  ) {
    MMCInx = simn;
    simn++;
  }
  else
    MMCInx = -1;
  if ( GMHC > 1e-8  ) {
    MMHCInx = simn;
    simn++;
    HMHCInx = simn;
    simn++;
  }
  else {
    MMHCInx = -1;
    HMHCInx = -1;
  }
  double simx[simn];
  for ( int k=0; k<simn; k++ )
    simx[k] = 0.0;
  double dxdt[simn];
  neuron()->init( simx );

  // equilibrium:
  for ( int c=0; c<100; c++ ) {
    double t = c * timeStep();
    (*neuron())( t, 0.0, simx, dxdt, simn );
    for ( int k=0; k<simn; k++ )
      simx[k] += timeStep()*dxdt[k];
  }

  // integrate:
  double t = 1000.0*time( 0 );  // time must be syncrhonous to recorded trace!
  while ( ! interrupt() ) {

    Integrate( t, simx, dxdt, simn, timeStep(), *this );

    cs++;
    if ( cs == maxs ) {
      push( traceinx[0], simx[0] );
      if ( traceinx[1] >= 0 )
	push( traceinx[1], CurrentInput );
      next();
      cs = 0;
    }

    t += timeStep();
  }

}


void NeuronModels::process( const OutData &source, OutData &dest )
{
  dest = source;
}


void NeuronModels::operator()( double t, double *x, double *dxdt, int n )
{
  // current noise:
  double s = noiseFac() * rnd.gaussian();
  CurrentInput = 0.0;
  if ( VCMode ) {
    // voltage-clamp current:
    double vccurrent = VCGain*(x[0] - signal( 0.001 * t, PotentialOutput[0] ));
    CurrentInput -= vccurrent;
    s -= vccurrent;
  }
  else {
    // current-clamp current:
    double cccurrent = signal( 0.001 * t, CurrentOutput[0] );
    CurrentInput += cccurrent;
    s += ( cccurrent + NM->offset() ) * NM->gain();
  }

  if ( MMCInx >= 0 )
    s -= GMC*x[MMCInx]*(x[0]-EMC);
  if ( MMHCInx >= 0 )
    s -= GMHC * ::pow( x[MMHCInx], PMMHC ) * ::pow( x[HMHCInx], PHMHC ) * (x[0]-EMHC);

  (*NM)( t, s, x, dxdt, n );

  if ( MMCInx >= 0 ) {
    double m0mc = 1.0/(exp(-(x[0]-MVMC)/MWMC)+1.0);
    dxdt[MMCInx] = ( m0mc - x[MMCInx] )/TAUMC;
  }
  if ( MMHCInx >= 0 ) {
    double m0mhc = 1.0/(exp(-(x[0]-MVMHC)/MWMHC)+1.0);
    dxdt[MMHCInx] = ( m0mhc - x[MMHCInx] )/TAUMMHC;
    double h0mhc = 1.0/(exp(-(x[0]-HVMHC)/HWMHC)+1.0);
    dxdt[HMHCInx] = ( h0mhc - x[HMHCInx] )/TAUHMHC;
  }
}


void NeuronModels::notifyStimulusData( void )
{
  VCMode = ( stimulusData().text( "AmplifierMode" ) == "VC" );
}


Options NeuronModels::metaData( void )
{
  Options opts( *this );
  opts.setName( configIdent() );
  if ( NM != 0 ) {
    opts.newSection( "Model" );
    opts.addText( "Name", NM->name() );
    opts.add( *NM );
  }
  return opts;
}


void NeuronModels::add( SpikingNeuron *model, const string &title )
{
  string ts = title;
  if ( ts.empty() )
    ts = model->name();
  if ( text( "spikemodel", 0 ).empty() )
    setText( "spikemodel", ts );
  else
    pushText( "spikemodel", ts );
  model->setConfigIdent( "Model: " + model->name() );
  model->setConfigGroup( RELACSPlugin::Plugins );
  model->add();
  model->addValueTypeStyles( OptWidget::Bold, Parameter::Section );
  model->unsetNotify();
  Models.push_back( model );
  Titles.push_back( ts );
}


void NeuronModels::addModels( void )
{
  add( new Stimulus() );
  add( new PassiveMembrane() );
  add( new MorrisLecar() );
  add( new HodgkinHuxley() );
  add( new Connor() );
  //  add( new RushRinzel() );
  //  add( new Awiszus() );
  //  add( new TraubMiles() );
  //  add( new TraubErmentrout2001() );
  add( new WangBuzsaki() );
}


void NeuronModels::addOptions( void )
{
  newSection( "Spike generator" );
  addSelection( "spikemodel", "Spike model", "" );
  addNumber( "noised", "Intensity of current noise", 0.0, 0.0, 100.0, 1.0 );
  addNumber( "deltat", "Delta t", 0.005, 0.0, 1.0, 0.001, "ms" );
  addSelection( "integrator", "Method of integration", "Euler|Midpoint|Runge-Kutta 4" );
  addNumber( "vcgain", "Voltage-clamp gain", 100.0, 0.0, 100000.0, 10.0 );
  newSection( "Voltage-gated current 1 - activation only" );
  addNumber( "gmc", "Conductivity", 0.0, 0.0, 10000.0, 0.1 );
  addNumber( "emc", "Reversal potential", -90.0, -200.0, 200.0, 1.0, "mV" ).setActivation( "gmc", ">0" );
  addNumber( "mvmc", "Midpoint potential of activation", -40, -200.0, 200.0, 1.0, "mV" ).setActivation( "gmc", ">0" );
  addNumber( "mwmc", "Width of activation", 10.0, -1000.0, 1000.0, 1.0, "mV" ).setActivation( "gmc", ">0" );
  addNumber( "taumc", "Time constant", 10.0, 0.0, 1000.0, 1.0, "ms" ).setActivation( "gmc", ">0" );
  newSection( "Voltage-gated current 2 - activation and inactivation" );
  addNumber( "gmhc", "Conductivity", 0.0, 0.0, 10000.0, 0.1 );
  addNumber( "emhc", "Reversal potential", -90.0, -200.0, 200.0, 1.0, "mV" ).setActivation( "gmhc", ">0" );
  addNumber( "mvmhc", "Midpoint potential of activation", -40, -200.0, 200.0, 1.0, "mV" ).setActivation( "gmhc", ">0" );
  addNumber( "mwmhc", "Width of activation", 10.0, -1000.0, 1000.0, 1.0, "mV" ).setActivation( "gmhc", ">0" );
  addNumber( "taummhc", "Time constant of activation", 10.0, 0.0, 1000.0, 1.0, "ms" ).setActivation( "gmhc", ">0" );
  addNumber( "pmmhc", "Power of activation gate", 1.0, 0.0, 100.0, 1.0 ).setActivation( "gmhc", ">0" );
  addNumber( "hvmhc", "Midpoint potential of inactivation", -40, -200.0, 200.0, 1.0, "mV" ).setActivation( "gmhc", ">0" );
  addNumber( "hwmhc", "Width of inactivation", 10.0, -1000.0, 1000.0, 1.0, "mV" ).setActivation( "gmhc", ">0" );
  addNumber( "tauhmhc", "Time constant of inactivation", 10.0, 0.0, 1000.0, 1.0, "ms" ).setActivation( "gmhc", ">0" );
  addNumber( "pmhhc", "Power of inactivation gate", 1.0, 0.0, 100.0, 1.0 ).setActivation( "gmhc", ">0" );
}


void NeuronModels::readOptions( void )
{
  // read out options:
  setNoiseD( number( "noised" ) );
  setTimeStep( number( "deltat", "ms" ) );
  NM = Models[ index( "spikemodel" ) ];
  NM->notify();
  int integrator = index( "integrator" );
  if ( integrator == 1 )
    Integrate = midpointStep;
  else if ( integrator == 2 )
    Integrate = rk4Step;
  else
    Integrate = eulerStep;

  VCGain = number( "vcgain" );
  GMC = number( "gmc" );
  EMC = number( "emc" );
  MVMC = number( "mvmc" );
  MWMC = number( "mwmc" );
  TAUMC = number( "taumc" );
  GMHC = number( "gmhc" );
  EMHC = number( "emhc" );
  MVMHC = number( "mvmhc" );
  MWMHC = number( "mwmhc" );
  TAUMMHC = number( "taummhc" );
  PMMHC = number( "pmmhc" );
  HVMHC = number( "hvmhc" );
  HWMHC = number( "hwmhc" );
  TAUHMHC = number( "tauhmhc" );
  PHMHC = number( "phmhc" );
}


void NeuronModels::dialogModelOptions( OptDialog *od, string *tabhotkeys )
{
  for ( unsigned int k=0; k<Models.size(); k++ ) {
    od->addTabOptions( Titles[k], *Models[k], dialogSelectMask(),
		       dialogReadOnlyMask(), dialogStyle(), mutex(), tabhotkeys );
  }
}


OptWidget *NeuronModels::dialogOptions( OptDialog *od, string *tabhotkeys )
{
  OptWidget *ow = od->addTabOptions( "General", *this, dialogSelectMask(),
				     dialogReadOnlyMask(), dialogStyle(), 
				     mutex(), tabhotkeys );
  dialogModelOptions( od, tabhotkeys );
  od->setVerticalSpacing( 1 );
  return ow;
}


addModel( NeuronModels, ephys );

}; /* namespace ephys */
