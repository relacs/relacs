/*
  neuronmodels.cc
  Various models of spiking (point-) neurons.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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
#include <relacs/common/neuronmodels.h>


NeuronModels::NeuronModels( void )
  : Model( "NeuronModels", "Neuron-Models", "Common",
	   "Jan Benda", "1.0", "Jan 10, 2006" )
{
  addLabel( "General" ).setStyle( OptWidget::TabLabel );
  addOptions();
  addModels();
  addTypeStyle( OptWidget::Bold, Parameter::Label );
}


NeuronModels::NeuronModels( const string &name,
			    const string &title, 
			    const string &pluginset,
			    const string &author, 
			    const string &version,
			    const string &date )
  : Model( name, title, pluginset, author, version, date )
{
}


NeuronModels::~NeuronModels( void )
{
  for ( unsigned int k=0; k<Models.size(); k++ )
    delete Models[k];
  Models.clear();
}


void NeuronModels::main( void )
{
  readOptions();

  // deltat( 0 ) must be integer multiple of delta t for integration:
  int maxs = int( ::floor( 1000.0*deltat( 0 )/timeStep() ) );
  if ( maxs <= 0 )
    maxs = 1;
  setTimeStep( 1000.0 * deltat( 0 ) / maxs );
  int cs = 0;

  // state variables:
  int simn = neuron()->dimension();
  double simx[simn];
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
  double t = 0.0;
  while ( ! interrupt() ) {

    Integrate( t, simx, dxdt, simn, timeStep(), *this );

    cs++;
    if ( cs == maxs ) {
      push( 0, simx[0] );
      cs = 0;
    }

    t += timeStep();
  }

}


void NeuronModels::operator()( double t, double *x, double *dxdt, int n )
{
  double s = signal( 0.001 * t ) + NoiseSD * numerics::rnd.gaussian();;
  (*NM)( t, s, x, dxdt, n );
}


void NeuronModels::process( const OutData &source, OutData &dest )
{
  dest = source;
  dest += NM->offset();
  dest *= NM->gain();
}


void NeuronModels::add( SpikingNeuron *model )
{
  addLabel( model->name() ).setStyle( OptWidget::TabLabel | OptWidget::ReadPatternLabel );
  int opts = Options::size();
  model->add( *this );
  if ( opts == Options::size() )
    Options::pop();
  if ( text( "spikemodel", 0 ).empty() )
    setText( "spikemodel", model->name() );
  else
    pushText( "spikemodel", model->name() );
  Models.push_back( model );
}


void NeuronModels::addModels( void )
{
  add( new Stimulus() );
  add( new FitzhughNagumo() );
  add( new MorrisLecar() );
  add( new HodgkinHuxley() );
  add( new WangAdapt() );
}


void NeuronModels::addOptions( void )
{
  addLabel( "Spike generator" );
  addSelection( "spikemodel", "Spike model", "" );
  addNumber( "noise", "Standard deviation of current noise", 0.0, 0.0, 100.0, 1.0 );
  addNumber( "deltat", "Delta t", 0.005, 0.0, 1.0, 0.001, "ms" );
  addSelection( "integrator", "Method of integration", "Euler|Midpoint|Runge-Kutta 4" );
}


void NeuronModels::readOptions( void )
{
  // read out options:
  NoiseSD = number( "noise" );
  SimDT = number( "deltat" );
  NM = Models[ index( "spikemodel" ) ];
  NM->read( *this, NM->name() + ">" );
  int integrator = index( "integrator" );
  if ( integrator == 1 )
    Integrate = midpointStep;
  else if ( integrator == 2 )
    Integrate = rk4Step;
  else
    Integrate = eulerStep;
}


SpikingNeuron::SpikingNeuron( void )
  : Gain( 1.0 ),
    Offset( 0.0 )
{
}


SpikingNeuron::~SpikingNeuron( void )
{
}


double SpikingNeuron::gain( void ) const
{
  return Gain;
}


double SpikingNeuron::offset( void ) const
{
  return Offset;
}


Stimulus::Stimulus( void )
{
  Gain=1.0;
  Offset = 0.0;
}


string Stimulus::name( void )
{
  return "Stimulus";
}


int Stimulus::dimension( void )
{
  return 1;
}


void Stimulus::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  x[0] = s;
  dxdt[0] = 0.0;
}


void Stimulus::init( double *x )
{
  x[0] = 0.0;
}


void Stimulus::add( Options &o )
{
}


void Stimulus::read( const Options &o, const string &label )
{
}



FitzhughNagumo::FitzhughNagumo( void )
{
  // from Koch, Biophysics of Computation, Chap.7.1
  Phi = 0.08; 
  A = 0.7;
  B = 0.8;
  TimeScale = 5.0;
  Gain=0.02;
  Offset = -5.0;
  Scale=10.0;
}


string FitzhughNagumo::name( void )
{
  return "Fitzhugh-Nagumo";
}


int FitzhughNagumo::dimension( void )
{
  return 3;
}


void FitzhughNagumo::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  x[0] = Scale*x[1];
  /* out */ dxdt[0] = 0.0;
  /* V */ dxdt[1] = TimeScale*(x[1]-x[1]*x[1]*x[1]/3.0-x[2]+s);
  /* W */ dxdt[2] = TimeScale*Phi*(x[1]+A-B*x[2]);
}


void FitzhughNagumo::init( double *x )
{
  x[1] = -1.2;
  x[2] = -0.62;
  x[0] = Scale*x[1];
}


void FitzhughNagumo::add( Options &o )
{
  o.addLabel( "Parameter" );
  o.addNumber( "phi", "Phi", Phi, 0.0, 100.0, 0.1 );
  o.addNumber( "a", "a", A, -100.0, 100.0, 0.1 );
  o.addNumber( "b", "b", B, -100.0, 100.0, 0.1 );

  o.addLabel( "Input" );
  o.addNumber( "timescale", "Timescale", TimeScale, 0.0, 1000.0, 0.001 );
  o.addNumber( "gain", "Gain", Gain, 0.0, 10000.0, 0.1 );
  o.addNumber( "offset", "Offset", Offset, -100000.0, 100000.0, 1.0 );

  o.addLabel( "Output" );
  o.addNumber( "scale", "Scale factor for output voltage", Scale, 0.0, 10000.0, 0.1 );
}


void FitzhughNagumo::read( const Options &o, const string &label )
{
  Phi = o.number( label + "phi" );
  A = o.number( label + "a" );
  B = o.number( label + "b" );
  TimeScale = o.number( label + "timescale" );
  Gain = o.number( label + "gain" );
  Offset = o.number( label + "offset" );
  Scale = o.number( label + "scale" );
}



MorrisLecar::MorrisLecar( void )
{
  // source??
  ECa=+120.0;
  GCa=4.0;
  MVCa = -1.2;
  MKCa = 18.0;
  EK=-80.0;
  GK=8.0;
  MVK = 12.0;
  MKK = 17.4;
  MPhiK = 0.067;
  EL=-60.0;
  GL=2.0;
  C=20.0;
  TimeScale = 10.0;
  Gain=1.0;
  Offset = 40.0;
}


string MorrisLecar::name( void )
{
  return "Morris-Lecar";
}


int MorrisLecar::dimension( void )
{
  return 2;
}


void MorrisLecar::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double m = 1.0/(1.0+exp(-2.0*(x[0]-MVCa)/MKCa));
  double w = 1.0/(1.0+exp(-2.0*(x[0]-MVK)/MKK));
  double tau = 1.0/(MPhiK*cosh((x[0]-MVK)/MKK));

  double ica = GCa*m*(x[0]-ECa);
  double ik = GK*x[1]*(x[0]-EK);
  double il = GL*(x[0]-EL);

  /* V */ dxdt[0] = TimeScale*(-ica-ik-il+s)/C;
  /* w */ dxdt[1] = TimeScale*(w-x[1])/tau;
}


void MorrisLecar::init( double *x )
{
  x[0] = -59.469;
  x[1] = 0.00027;
}


void MorrisLecar::add( Options &o )
{
  o.addLabel( "General" );
  o.addSelection( "params", "Parameter set", "Custom|Type I|Type II" );

  o.addLabel( "Calcium current" );
  o.addNumber( "gca", "Ca conductivity", GCa, 0.0, 10000.0, 0.1, "nS" ).setActivation( "params", "Custom" );
  o.addNumber( "eca", "Ca reversal potential", ECa, -200.0, 200.0, 1.0, "mV" ).setActivation( "params", "Custom" );
  o.addNumber( "mvca", "Midpoint potential of Ca activation", MVCa, -200.0, 200.0, 1.0, "mV" ).setActivation( "params", "Custom" );
  o.addNumber( "mkca", "Width of Ca activation", MKCa, 0.0, 1000.0, 1.0, "mV" ).setActivation( "params", "Custom" );

  o.addLabel( "Potassium current" );
  o.addNumber( "gk", "K conductivity", GK, 0.0, 10000.0, 0.1, "nS" ).setActivation( "params", "Custom" );
  o.addNumber( "ek", "K reversal potential", EK, -200.0, 200.0, 1.0, "mV" ).setActivation( "params", "Custom" );
  o.addNumber( "mvk", "Midpoint potential of K activation", MVK, -200.0, 200.0, 1.0, "mV" ).setActivation( "params", "Custom" );
  o.addNumber( "mkk", "Width of K activation", MKK, 0.0, 1000.0, 1.0, "mV" ).setActivation( "params", "Custom" );
  o.addNumber( "mphik", "Rate of K activation", MPhiK, 0.0, 10.0, 0.001, "kHz" ).setActivation( "params", "Custom" );

  o.addLabel( "Leak current" );
  o.addNumber( "gl", "Leak conductivity", GL, 0.0, 10000.0, 0.1, "nS" ).setActivation( "params", "Custom" );
  o.addNumber( "el", "Leak reversal potential", EL, -200.0, 200.0, 1.0, "mV" ).setActivation( "params", "Custom" );
  o.addNumber( "c", "Capacitance", C, 0.0, 100.0, 0.1, "pF" ).setActivation( "params", "Custom" );

  o.addLabel( "Input" );
  o.addNumber( "timescale", "Timescale", TimeScale, 0.0, 1000.0, 0.001 );
  o.addNumber( "gain", "Gain", Gain, 0.0, 10000.0, 0.1 );
  o.addNumber( "offset", "Offset", Offset, -100000.0, 100000.0, 1.0, "pA" );
}


void MorrisLecar::read( const Options &o, const string &label )
{
  int params = o.index( label + "params" );

  if ( params == 1 ) {
    // Rinzel & Ermentrout, 1999 in Methods of Neural Modeling by Koch & Segev
    ECa=+120.0;
    GCa=4.4;
    MVCa = -1.2;
    MKCa = 18.0;
    EK=-84.0;
    GK=8.0;
    MVK = 12.0;
    MKK = 17.4;
    MPhiK = 0.0667;
    EL=-60.0;
    GL=2.0;
    C=20.0;
  }
  else if ( params == 2 ) {
    // Rinzel & Ermentrout, 1999 in Methods of Neural Modeling by Koch & Segev
    ECa=+120.0;
    GCa=4.0;
    MVCa = -1.2;
    MKCa = 18.0;
    EK=-84.0;
    GK=8.0;
    MVK = 2.0;
    MKK = 30.0;
    MPhiK = 0.04;
    EL=-60.0;
    GL=2.0;
    C=20.0;
  }
  else {
    ECa = o.number( label + "eca" );
    GCa = o.number( label + "gca" );
    MVCa = o.number( label + "mvca" );
    MKCa = o.number( label + "mkca" );
    EK = o.number( label + "ek" );
    GK = o.number( label + "gk" );
    MVK = o.number( label + "mvk" );
    MKK = o.number( label + "mkk" );
    MPhiK = o.number( label + "mphik" );
    EL = o.number( label + "el" );
    GL = o.number( label + "gl" );
    C = o.number( label + "c" );
  }
  TimeScale = o.number( label + "timescale" );
  Gain = o.number( label + "gain" );
  Offset = o.number( label + "offset" );
}


HodgkinHuxley::HodgkinHuxley( void )
{
  ENa=+50.0;
  GNa=120.0;
  EK=-77.0;
  GK=36.0;
  EL=-54.384;
  GL=0.3;
  C=1.0;
  PT=1.0;
  Gain=1.0;
  Offset = 0.0;
}


string HodgkinHuxley::name( void )
{
  return "Hodgkin-Huxley";
}


int HodgkinHuxley::dimension( void )
{
  return 4;
}


void HodgkinHuxley::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  /* V */ dxdt[0] = (GNa*x[1]*x[1]*x[1]*x[2]*(ENa-x[0])+GK*x[3]*x[3]*x[3]*x[3]*(EK-x[0])+GL*(EL-x[0])+s)/C;
  /* m */ dxdt[1] = PT*( 0.1*(x[0]+40.0)/(1.0-exp(-(x[0]+40.0)/10.0))*(1.0-x[1]) - x[1]*4.0*exp(-(x[0]+65.0)/18.0) );
  /* h */ dxdt[2] = PT*( 0.07*exp(-(x[0]+65)/20.0)*(1.0-x[2]) - x[2]*1.0/(1.0+exp(-(x[0]+35.0)/10.0)) );
  /* n */ dxdt[3] = PT*( 0.01*(x[0]+55.0)/(1.0-exp(-(x[0]+55.0)/10.0))*(1.0-x[3]) - x[3]*0.125*exp(-(x[0]+65.0)/80.0) );
}


void HodgkinHuxley::init( double *x )
{
  x[0] = -65.0;
  x[1] = 0.053;
  x[2] = 0.596;
  x[3] = 0.318;
}


void HodgkinHuxley::add( Options &o )
{
  o.addLabel( "Sodium current" );
  o.addNumber( "gna", "Na conductivity", GNa, 0.0, 10000.0, 0.1, "mS/cm^2" );
  o.addNumber( "ena", "Na reversal potential", ENa, -200.0, 200.0, 1.0, "mV" );

  o.addLabel( "Potassium current" );
  o.addNumber( "gk", "K conductivity", GK, 0.0, 10000.0, 0.1, "mS/cm^2" );
  o.addNumber( "ek", "K reversal potential", EK, -200.0, 200.0, 1.0, "mV" );

  o.addLabel( "Leak current" );
  o.addNumber( "gl", "Leak conductivity", GL, 0.0, 10000.0, 0.1, "mS/cm^2" );
  o.addNumber( "el", "Leak reversal potential", EL, -200.0, 200.0, 1.0, "mV" );
  o.addNumber( "c", "Capacitance", C, 0.0, 100.0, 0.1, "muF/cm^2" );
  o.addNumber( "phi", "Phi", PT, 0.0, 100.0, 1.0 );

  o.addLabel( "Input" );
  o.addNumber( "gain", "Gain", Gain, 0.0, 10000.0, 0.1 );
  o.addNumber( "offset", "Offset", Offset, -100000.0, 100000.0, 1.0, "muA/cm^2" );
}


void HodgkinHuxley::read( const Options &o, const string &label )
{
  ENa = o.number( label + "ena" );
  GNa = o.number( label + "gna" );
  EK = o.number( label + "ek" );
  GK = o.number( label + "gk" );
  EL = o.number( label + "el" );
  GL = o.number( label + "gl" );
  C = o.number( label + "c" );
  PT = o.number( label + "phi" );
  Gain = o.number( label + "gain" );
  Offset = o.number( label + "offset" );
}


WangAdapt::WangAdapt( void )
{
  ENa=+55.0;
  GNa=35.0;
  EK=-90.0;
  GK=9.0;
  EA=-90.0;
  GA=0.8;
  Atau = 100.0;
  EL=-65.0;
  GL=0.1;
  C=1.0;
  PT=5.0;
  Gain=0.3;
  Offset = 0.0;
}


string WangAdapt::name( void )
{
  return "Wang";
}


int WangAdapt::dimension( void )
{
  return 4;
}


void WangAdapt::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double ms = 1.0/(1.0+4.0*exp(-(x[0]+60.0)/18.0)*(exp(-0.1*(x[0]+35.0))-1.0)/(-0.1*(x[0]+35.0)));
  double w0 = 1.0/(exp(-(x[0]+35.0)/10.0)+1.0);
  /* V */ dxdt[0] = (GNa*ms*ms*ms*x[1]*(ENa-x[0])+GK*x[2]*x[2]*x[2]*x[2]*(EK-x[0])+GL*(EL-x[0])+GA*x[3]*(EA-x[0])+s)/C;
  /* h */ dxdt[1] = PT*(0.07*exp(-(x[0]+58)/20)*(1.0-x[1])-x[1]/(exp(-0.1*(x[0]+28))+1));
  /* n */ dxdt[2] = PT*(-0.01*(x[0]+34.0)*(1.0-x[2])/(exp(-0.1*(x[0]+34.0))-1)-0.125*exp(-(x[0]+44.0)/80.0)*x[2]);
  /* a */ dxdt[3] = ( w0 - x[3] )/Atau;
}


void WangAdapt::init( double *x )
{
  x[0] = -64.018;
  x[1] = 0.7808;
  x[2] = 0.0891;
  x[3] = 0.0;
}


void WangAdapt::add( Options &o )
{
  o.addLabel( "Sodium current" );
  o.addNumber( "gna", "Na conductivity", GNa, 0.0, 10000.0, 0.1, "mS/cm^2" );
  o.addNumber( "ena", "Na reversal potential", ENa, -200.0, 200.0, 1.0, "mV" );

  o.addLabel( "Potassium current" );
  o.addNumber( "gk", "K conductivity", GK, 0.0, 10000.0, 0.1, "mS/cm^2" );
  o.addNumber( "ek", "K reversal potential", EK, -200.0, 200.0, 1.0, "mV" );

  o.addLabel( "Adaptation current" );
  o.addNumber( "ga", "A conductivity", GA, 0.0, 10000.0, 0.1, "mS/cm^2" );
  o.addNumber( "ea", "A reversal potential", EA, -200.0, 200.0, 1.0, "mV" );
  o.addNumber( "atau", "A time constant", Atau, 0.0, 1000.0, 1.0, "ms" );

  o.addLabel( "Leak current" );
  o.addNumber( "gl", "Leak conductivity", GL, 0.0, 10000.0, 0.1, "mS/cm^2" );
  o.addNumber( "el", "Leak reversal potential", EL, -200.0, 200.0, 1.0, "mV" );
  o.addNumber( "c", "Capacitance", C, 0.0, 100.0, 0.1, "muF/cm^2" );
  o.addNumber( "phi", "Phi", PT, 0.0, 100.0, 1.0 );

  o.addLabel( "Input" );
  o.addNumber( "gain", "Gain", Gain, 0.0, 10000.0, 0.1 );
  o.addNumber( "offset", "Offset", Offset, -100000.0, 100000.0, 1.0, "muA/cm^2" );
}


void WangAdapt::read( const Options &o, const string &label )
{
  ENa = o.number( label + "ena" );
  GNa = o.number( label + "gna" );
  EK = o.number( label + "ek" );
  GK = o.number( label + "gk" );
  EA = o.number( label + "ea" );
  GA = o.number( label + "ga" );
  Atau = o.number( label + "atau" );
  EL = o.number( label + "el" );
  GL = o.number( label + "gl" );
  C = o.number( label + "c" );
  PT = o.number( label + "phi" );
  Gain = o.number( label + "gain" );
  Offset = o.number( label + "offset" );
}


addModel( NeuronModels );

#include "moc_neuronmodels.cc"
