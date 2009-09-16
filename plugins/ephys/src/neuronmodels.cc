/*
  ephys/neuronmodels.cc
  Various models of spiking (point-) neurons.

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

#include <cmath>
#include <relacs/optwidget.h>
#include <relacs/random.h>
#include <relacs/odealgorithm.h>
#include <relacs/ephys/neuronmodels.h>
using namespace relacs;

namespace ephys {


NeuronModels::NeuronModels( void )
  : Model( "NeuronModels", "Neuron-Models", "EPhys",
	   "Jan Benda", "1.0", "Jan 10, 2006" )
{
  addOptions();
  addTypeStyle( OptWidget::Bold, Parameter::Label );
  addModels();
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
  Titles.clear();
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
  double s = signal( 0.001 * t ) + NoiseSD * rnd.gaussian();;
  (*NM)( t, s, x, dxdt, n );
}


void NeuronModels::process( const OutData &source, OutData &dest )
{
  dest = source;
  dest += NM->offset();
  dest *= NM->gain();
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
  model->addTypeStyle( OptWidget::Bold, Parameter::Label );
  model->unsetNotify();
  Models.push_back( model );
  Titles.push_back( ts );
}


void NeuronModels::addModels( void )
{
  add( new Stimulus() );
  add( new MorrisLecar() );
  add( new HodgkinHuxley() );
  add( new WangBuzsakiAdapt(), "Wang-Buzsaki" );
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
  NM->notify();
  int integrator = index( "integrator" );
  if ( integrator == 1 )
    Integrate = midpointStep;
  else if ( integrator == 2 )
    Integrate = rk4Step;
  else
    Integrate = eulerStep;
}


void NeuronModels::dialogModelOptions( OptDialog *od )
{
  for ( unsigned int k=0; k<Models.size(); k++ ) {
    od->addTabOptions( Titles[k], *Models[k], dialogSelectMask(),
		       dialogReadOnlyMask(), dialogStyle(), mutex() );
  }
}


void NeuronModels::dialogOptions( OptDialog *od )
{
  od->addTabOptions( "General", *this, dialogSelectMask(),
		     dialogReadOnlyMask(), dialogStyle(), mutex() );
  dialogModelOptions( od );
  od->setVerticalSpacing( 1 );
  od->setMargin( 10 );
}


addModel( NeuronModels );

}; /* namespace ephys */
