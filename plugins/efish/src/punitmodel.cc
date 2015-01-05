/*
  efish/punitmodel.cc
  A model for P-units of weakly-electric fish.

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
#include <relacs/attenuator.h>
#include <relacs/optwidget.h>
#include <relacs/random.h>
#include <relacs/odealgorithm.h>
#include <relacs/efish/punitmodel.h>
using namespace relacs;

namespace efish {


PUnitModel::PUnitModel( void )
  : NeuronModels( "PUnitModel", "efish", "Jan Benda", "2.0", "Oct 30, 2013" )
{
  // EOD:
  EODFreq = 800.0;
  EODFreqSD = 10.0;
  EODLocalAmplitude = 1.0;
  EODGlobalAmplitude = 1.0;
  EODFreqTau = 1000.0;
  LocalStimulusGain = 1.0;
  GlobalStimulusGain = 0.0;
  StimulusGain = 1.0;
  // Spikes:
  VoltageScale = 1.0;

  // options:
  newSection( "General" );
  newSubSection( "EOD" );
  addSelection( "eodtype", "EOD type", "Sine|None|Sine|Apteronotus|Eigenmannia" );
  addNumber( "eodfreq", "Frequency", EODFreq, 0.0, 2000.0, 10.0, "Hz" );
  addNumber( "eodfreqsd", "SD of frequency", EODFreqSD, 0.0, 1000.0, 2.0, "Hz" );
  addNumber( "eodfreqtau", "Timescale of frequency", EODFreqTau, 0.5, 100000.0, 0.5, "s" );
  addNumber( "eodlocalamplitude", "Amplitude for local electrode", EODLocalAmplitude, 0.0, 100.0, 0.1, "mV/cm" );
  addNumber( "eodglobalamplitude", "Amplitude for global electrode", EODGlobalAmplitude, 0.0, 100.0, 0.1, "mV/cm" );
  addNumber( "localstimulusgain", "Gain for additive stimulus component to local electrode", LocalStimulusGain, 1.0, 100000.0, 1.0, "", "", "%.2f" );
  addNumber( "globalstimulusgain", "Gain for additive stimulus component to global electrode", GlobalStimulusGain, 0.0, 100000.0, 1.0, "", "", "%.2f" );
  addNumber( "stimulusgain", "Gain for stimulus recording channel", StimulusGain, 0.0, 100000.0, 1.0, "", "", "%.2f" );
  newSubSection( "Spikes" );
  addNumber( "voltagescale", "Scale factor for membrane potential", VoltageScale, 0.0, 100.0, 0.1 );

  addOptions();

  addModels();
}


PUnitModel::~PUnitModel( void )
{
}


void PUnitModel::main( void )
{
  // eod:
  EODType = index( "eodtype" );
  EODFreq = 0.001*2.0*M_PI*number( "eodfreq" );
  EODFreqSD = 0.001*2.0*M_PI*number( "eodfreqsd" );
  EODFreqTau = 1000.0*number( "eodfreqtau" );
  EODLocalAmplitude = number( "eodlocalamplitude" );
  EODGlobalAmplitude = number( "eodglobalamplitude" );
  LocalStimulusGain = number( "localstimulusgain" );
  GlobalStimulusGain = number( "globalstimulusgain" );
  StimulusGain = number( "stimulusgain" );
  VoltageScale = number( "voltagescale" );

  int sigdimension = 2;

  // init traces:
  double null = 0.0;
  double voltage = 0.0;
  double *val[traces()];
  for ( int k=0; k<traces(); k++ )
    val[k] = &null;
  for ( int k=0; k<SpikeTraces; k++ )
    val[SpikeTrace[k]] = &voltage;
  if ( EODTrace >= 0 )
    val[EODTrace] = &EODGlobal;
  for ( int k=0; k<LocalEODTraces; k++ )
    val[LocalEODTrace[k]] = &EODLocal;
  for ( int j=0; j<FishEODTanks; j++ ) {
    for ( int k=0; k<FishEODTraces[j]; k++ )
      val[FishEODTrace[j][k]] = &EODGlobal;
  }
  if ( GlobalEFieldTrace >= 0 )
    val[GlobalEFieldTrace] = &Signal;
  for ( int k=0; k<LocalEFieldTraces; k++ )
    val[LocalEFieldTrace[k]] = &Signal;
  for ( int k=0; k<FishEFieldTraces; k++ )
    val[FishEFieldTrace[k]] = &Signal;

  // spiking neuron and general integration options:
  readOptions();

  // deltat( 0 ) must be integer multiple of delta t for integration:
  int maxs = int( ::floor( 1000.0*deltat( 0 )/timeStep() ) );
  if ( maxs <= 0 )
    maxs = 1;
  setTimeStep( 1000.0 * deltat( 0 ) / maxs );
  int cs = 0;
  setNoiseFac();

  // OU normalisation factor for noise term:
  EODFreqFac = ::sqrt( 2.0*EODFreqTau/timeStep());

  // state variables:
  int simn = sigdimension + neuron()->dimension();
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
  double dxdt[simn];
  for ( int k=0; k<simn; k++ )
    simx[k] = dxdt[k] = 0.0;
  neuron()->init( simx+sigdimension );

  // equilibrium:
  for ( int c=0; c<100; c++ ) {
    double t = c * timeStep();
    (*neuron())( t, 0.0, simx+sigdimension, dxdt+sigdimension, neuron()->dimension() );
    for ( int k=sigdimension; k<simn; k++ )
      simx[k] += timeStep()*dxdt[k];
  }

  // integrate:
  double t = 1000.0*time( 0 );  // time must be syncrhonous to recorded trace!
  while ( ! interrupt() ) {

    Integrate( t, simx, dxdt, simn, timeStep(), *this );

    cs++;
    if ( cs == maxs ) {
      voltage = VoltageScale*simx[sigdimension];
      for ( int k=0; k<traces(); k++ ) {
	if ( trace( k ).source() == 0 )
	  push( k, *val[k] );
      }
      cs = 0;
    }

    t += timeStep();
  }
}


void PUnitModel::process( const OutData &source, OutData &dest )
{
  dest = source;
  double intensfac = 0.0;
  if ( source.level() != OutData::NoLevel ) {
    intensfac = ( ::pow( 10.0, -source.level()/20.0 ) );
    bool scaled = false;
    if ( source.trace() == GlobalAMEField ) {
      intensfac /= 0.3;
      scaled = true;
    }
    for ( int k=0; ! scaled && k<LocalEFields; k++ ) {
      if ( source.trace() == LocalEField[k] ) {
	intensfac /= ( 0.4 + k*0.1 );
	break;
      }
    }
    for ( int k=0; ! scaled && k<LocalAMEFields; k++ ) {
      if ( source.trace() == LocalAMEField[k] ) {
	intensfac /= ( 0.2 + k*0.15 );
	break;
      }
    }
    for ( int k=0; ! scaled && k<FishEFields; k++ ) {
      if ( source.trace() == FishEField[k] ) {
	intensfac /= ( 0.5 + k*0.05 );
	break;
      }
    }
    if ( ! scaled )
      intensfac /= 0.4;
  }
  dest *= intensfac;
}


void PUnitModel::operator()( double t, double *x, double *dxdt, int n )
{
  static Random rand;

  // O-U noise for EOD frequency:
  dxdt[0] = ( -x[0] + EODFreqFac*rand.gaussian() ) / EODFreqTau;
  // phase of EOD frequency:
  dxdt[1] = EODFreq + EODFreqSD * x[0];
  double v = 0.0;
  if ( EODType == 1 )
    v = ::sin( x[1] );
  else if ( EODType == 2 )
    v = ( ::sin( x[1] ) - 0.5*::sin( 2.0*x[1] ) ) / 1.3;
  else if ( EODType == 3 )
    v = ( ::sin( x[1] ) + 0.25*::sin( 2.0*x[1] + 0.5*M_PI ) ) + 0.25;
  EODLocal = EODLocalAmplitude * v;
  EODGlobal = EODGlobalAmplitude * v;
  double sglobal = signal( 0.001 * t, GlobalEField );
  double sglobalam = signal( 0.001 * t, GlobalAMEField ) * EODGlobal;
  Signal = sglobal + sglobalam;
  EODLocal += LocalStimulusGain*Signal;
  EODGlobal += GlobalStimulusGain*Signal;
  Signal *= StimulusGain;
  double s = EODLocal * neuron()->gain() + neuron()->offset();
  s += noiseFac() * rand.gaussian();
  if ( MMCInx >= 0 )
    s -= GMC*x[MMCInx]*(x[2]-EMC);
  if ( MMHCInx >= 0 )
    s -= GMHC * ::pow( x[MMHCInx], PMMHC ) * ::pow( x[HMHCInx], PHMHC ) * (x[2]-EMHC);
  (*neuron())( t, s, x+2, dxdt+2, n-2 );
  if ( MMCInx >= 0 ) {
    double m0mc = 1.0/(exp(-(x[2]-MVMC)/MWMC)+1.0);
    dxdt[MMCInx] = ( m0mc - x[MMCInx] )/TAUMC;
  }
  if ( MMHCInx >= 0 ) {
    double m0mhc = 1.0/(exp(-(x[2]-MVMHC)/MWMHC)+1.0);
    dxdt[MMHCInx] = ( m0mhc - x[MMHCInx] )/TAUMMHC;
    double h0mhc = 1.0/(exp(-(x[2]-HVMHC)/HWMHC)+1.0);
    dxdt[HMHCInx] = ( h0mhc - x[HMHCInx] )/TAUHMHC;
  }
}


addModel( PUnitModel, efish );

}; /* namespace efish */
