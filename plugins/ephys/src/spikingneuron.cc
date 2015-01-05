/*
  ephys/spikingneuron.cc
  Base class for a spiking (point-) neuron.

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
#include <relacs/spikingneuron.h>

namespace relacs {


SpikingNeuron::SpikingNeuron( void )
  : ConfigClass( "" ),
    Gain( 1.0 ),
    Offset( 0.0 )
{
  setConfigIdent( name() );
}


SpikingNeuron::~SpikingNeuron( void )
{
}


string SpikingNeuron::name( void ) const
{
  return "";
}


void SpikingNeuron::conductances( vector< string > &conductancenames ) const
{
  conductancenames.clear();
}


void SpikingNeuron::conductances( double *g ) const
{
}


string SpikingNeuron::conductanceUnit( void ) const
{
  return "mS/cm^2";
}


void SpikingNeuron::currents( vector< string > &currentnames ) const
{
  currentnames.clear();
}


void SpikingNeuron::currents( double *c ) const
{
}


string SpikingNeuron::currentUnit( void ) const
{
  return "uA/cm^2";
}


string SpikingNeuron::inputUnit( void ) const
{
  return "uA/cm^2";
}


void SpikingNeuron::add( void )
{
  newSection( "Input", ScalingFlag );
  addNumber( "gain", "Gain", Gain, 0.0, 10000.0, 0.1 ).setFlags( ScalingFlag );
  addNumber( "offset", "Offset", Offset, -100000.0, 100000.0, 1.0, "muA/cm^2" ).setFlags( ScalingFlag );
}


void SpikingNeuron::notify( void )
{
  Gain = number( "gain" );
  Offset = number( "offset" );
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
  : SpikingNeuron()
{
}


string Stimulus::name( void ) const
{
  return "Stimulus";
}


int Stimulus::dimension( void ) const
{
  return 1;
}


void Stimulus::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( dimension() );
  varnames.push_back( "Stimulus" );
}


void Stimulus::units( vector< string > &u ) const
{
  u.clear();
  u.reserve( dimension() );
  u.push_back( "" );
}


void Stimulus::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  x[0] = s;
  dxdt[0] = 0.0;
}


void Stimulus::init( double *x ) const
{
  x[0] = 0.0;
}



FitzhughNagumo::FitzhughNagumo( void )
  : SpikingNeuron()
{
  // from Koch, Biophysics of Computation, Chap.7.1
  Phi = 0.08; 
  A = 0.7;
  B = 0.8;
  TimeScale = 1.0;

  /*
  TimeScale = 0.2;
  Gain = 0.02;
  Offset = -5.0;
  Scale = 10.0;
  */

}


string FitzhughNagumo::name( void ) const
{
  return "Fitzhugh-Nagumo";
}


int FitzhughNagumo::dimension( void ) const
{
  return 2;
}


void FitzhughNagumo::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( dimension() );
  varnames.push_back( "V" );
  varnames.push_back( "W" );
}


void FitzhughNagumo::units( vector< string > &u ) const
{
  u.clear();
  u.reserve( dimension() );
  u.push_back( "1" );
  u.push_back( "1" );
}


void FitzhughNagumo::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  /* V */ dxdt[0] = (x[0]-x[0]*x[0]*x[0]/3.0-x[1]+s)/TimeScale;
  /* W */ dxdt[1] = Phi*(x[0]+A-B*x[1])/TimeScale;
}


void FitzhughNagumo::init( double *x ) const
{
  x[0] = -1.2;
  x[1] = -0.62;
}


void FitzhughNagumo::add( void )
{
  newSection( "Parameter", ModelFlag );
  addNumber( "phi", "Phi", Phi, 0.0, 100.0, 0.1 ).setFlags( ModelFlag );
  addNumber( "a", "a", A, -100.0, 100.0, 0.1 ).setFlags( ModelFlag );
  addNumber( "b", "b", B, -100.0, 100.0, 0.1 ).setFlags( ModelFlag );

  SpikingNeuron::add();
  insertNumber( "timescale", "Input>gain", "Timescale", TimeScale, 0.0, 1000.0, 0.001, "ms" ).setFlags( ScalingFlag );
}


void FitzhughNagumo::notify( void )
{
  SpikingNeuron::notify();
  Phi = number( "phi" );
  A = number( "a" );
  B = number( "b" );
  TimeScale = number( "timescale" );
}



MorrisLecar::MorrisLecar( void )
  : SpikingNeuron()
{
  // Parameter values are from Rinzel & Ermentrout (1998), type-1.
  // As a comment, the values for type-2 are given.
  GCa = 4.0;  // 4.4
  GK = 8.0;
  GL = 2.0;

  ECa = +120.0;
  EK = -84.0;
  EL = -60.0;

  MVCa = -1.2;
  MKCa = 18.0;
  MVK = 12.0;  // 2.0
  MKK = 17.4;  // 30.0

  MPhiK = 0.067;  // 0.04

  C = 20.0;
  TimeScale = 10.0;
  Gain = 1.0;
  Offset = 40.0;

  GCaGates = GCa;
  GKGates = GK;

  ICa = 0.0;
  IK = 0.0;
  IL = 0.0;
}


string MorrisLecar::name( void ) const
{
  return "Morris-Lecar";
}


int MorrisLecar::dimension( void ) const
{
  return 2;
}


void MorrisLecar::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( dimension() );
  varnames.push_back( "V" );
  varnames.push_back( "w" );
}


void MorrisLecar::units( vector< string > &u ) const
{
  u.clear();
  u.reserve( dimension() );
  u.push_back( "mV" );
  u.push_back( "1" );
}


void MorrisLecar::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];

  double ms = 1.0/(1.0+exp(-2.0*(V-MVCa)/MKCa)); // same as 0.5*(1.0+tanh((V-MVCa)/MKCa))
  double ws = 1.0/(1.0+exp(-2.0*(V-MVK)/MKK));   // same as 0.5*(1.0+tanh((V-MVK)/MKK))
  double tauw = 1.0/(MPhiK*cosh(0.5*(V-MVK)/MKK));

  GCaGates = GCa*ms;
  GKGates = GK*x[1];

  ICa = GCaGates*(V-ECa);
  IK = GKGates*(V-EK);
  IL = GL*(V-EL);

  /* V */ dxdt[0] = TimeScale*(-ICa-IK-IL+s)/C;
  /* w */ dxdt[1] = TimeScale*(ws-x[1])/tauw;
}


void MorrisLecar::init( double *x ) const
{
  x[0] = -59.474;
  x[1] = 0.00027;
}


void MorrisLecar::conductances( vector< string > &conductancenames ) const
{
  conductancenames.clear();
  conductancenames.reserve( 3 );
  conductancenames.push_back( "g_Ca" );
  conductancenames.push_back( "g_K" );
  conductancenames.push_back( "g_l" );
}


void MorrisLecar::conductances( double *g ) const
{
  g[0] = GCaGates;
  g[1] = GKGates;
  g[2] = GL;
}


string MorrisLecar::conductanceUnit( void ) const
{
  return "nS";
}


void MorrisLecar::currents( vector< string > &currentnames ) const
{
  currentnames.clear();
  currentnames.reserve( 3 );
  currentnames.push_back( "I_Ca" );
  currentnames.push_back( "I_K" );
  currentnames.push_back( "I_l" );
}


void MorrisLecar::currents( double *c ) const
{
  c[0] = ICa;
  c[1] = IK;
  c[2] = IL;
}


string MorrisLecar::currentUnit( void ) const
{
  return "pA";
}


string MorrisLecar::inputUnit( void ) const
{
  return "pA";
}


void MorrisLecar::add( void )
{
  newSection( "General", ModelFlag );
  addSelection( "params", "Parameter set", "Custom|Type I|Type II" ).setFlags( ModelFlag );

  newSection( "Calcium current", ModelFlag );
  addNumber( "gca", "Ca conductivity", GCa, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "eca", "Ca reversal potential", ECa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "mvca", "Midpoint potential of Ca activation", MVCa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "mkca", "Width of Ca activation", MKCa, 0.0, 1000.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );

  newSection( "Potassium current", ModelFlag );
  addNumber( "gk", "K conductivity", GK, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "ek", "K reversal potential", EK, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "mvk", "Midpoint potential of K activation", MVK, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "mkk", "Width of K activation", MKK, 0.0, 1000.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "mphik", "Rate of K activation", MPhiK, 0.0, 10.0, 0.001, "kHz" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );

  newSection( "Leak current", ModelFlag );
  addNumber( "gl", "Leak conductivity", GL, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "el", "Leak reversal potential", EL, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "c", "Capacitance", C, 0.0, 100.0, 0.1, "pF" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );

  SpikingNeuron::add();
  insertNumber( "timescale", "Input>gain", "Timescale", TimeScale, 0.0, 1000.0, 0.001 ).setFlags( ScalingFlag );
}


void MorrisLecar::notify( void )
{
  SpikingNeuron::notify();

  int params = index( "params" );

  if ( params == 1 ) {
    // Rinzel & Ermentrout, 1999 in Methods of Neural Modeling by Koch & Segev
    ECa = +120.0;
    GCa = 4.4;
    MVCa = -1.2;
    MKCa = 18.0;
    EK = -84.0;
    GK = 8.0;
    MVK = 12.0;
    MKK = 17.4;
    MPhiK = 0.0667;
    EL = -60.0;
    GL = 2.0;
    C = 20.0;
  }
  else if ( params == 2 ) {
    // Rinzel & Ermentrout, 1999 in Methods of Neural Modeling by Koch & Segev
    ECa = +120.0;
    GCa = 4.0;
    MVCa = -1.2;
    MKCa = 18.0;
    EK = -84.0;
    GK = 8.0;
    MVK = 2.0;
    MKK = 30.0;
    MPhiK = 0.04;
    EL = -60.0;
    GL = 2.0;
    C = 20.0;
  }
  else {
    ECa = number( "eca" );
    GCa = number( "gca" );
    MVCa = number( "mvca" );
    MKCa = number( "mkca" );
    EK = number( "ek" );
    GK = number( "gk" );
    MVK = number( "mvk" );
    MKK = number( "mkk" );
    MPhiK = number( "mphik" );
    EL = number( "el" );
    GL = number( "gl" );
    C = number( "c" );
  }
  TimeScale = number( "timescale" );
}


MorrisLecarPrescott::MorrisLecarPrescott( void )
  : MorrisLecar()
{
  // I_M (I_AHP as comments):

  GCa = 20.0;
  GK = 20.0;
  GL = 2.0;
  GA = 0.5;   // 5.0

  ECa = +50.0;
  EK = -100.0;
  EL = -70.0;
  EA = -100.0;

  MVCa = -1.2;
  MKCa = 18.0;

  MVK = 0.0;
  MKK = 10.0;
  MPhiK = 0.15;

  MVA = -35.0; // 0.0
  MKA = 4.0;
  TauA = 100.0;

  C = 2.0;
  TimeScale = 1.0;
  Gain = 1.0;
  Offset = 0.0;

  GCaGates = GCa;
  GKGates = GK;
  GAGates = GA;

  ICa = 0.0;
  IK = 0.0;
  IL = 0.0;
  IA = 0.0;
}


string MorrisLecarPrescott::name( void ) const
{
  return "Morris-Lecar-Prescott";
}


int MorrisLecarPrescott::dimension( void ) const
{
  return 3;
}


void MorrisLecarPrescott::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( dimension() );
  varnames.push_back( "V" );
  varnames.push_back( "w" );
  varnames.push_back( "z" );
}


void MorrisLecarPrescott::units( vector< string > &u ) const
{
  u.clear();
  u.reserve( dimension() );
  u.push_back( "mV" );
  u.push_back( "1" );
  u.push_back( "1" );
}


void MorrisLecarPrescott::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];

  double ms = 1.0/(1.0+exp(-2.0*(V-MVCa)/MKCa)); // same as 0.5*(1.0+tanh((V-MVCa)/MKCa))
  double ws = 1.0/(1.0+exp(-2.0*(V-MVK)/MKK));   // same as 0.5*(1.0+tanh((V-MVK)/MKK))
  double tauw = 1.0/(MPhiK*cosh(0.5*(V-MVK)/MKK));
  double zs = 1.0/(1.0+exp(-(V-MVA)/MKA));

  GCaGates = GCa*ms;
  GKGates = GK*x[1];
  GAGates = GA*x[2];

  ICa = GCaGates*(V-ECa);
  IK = GKGates*(V-EK);
  IL = GL*(V-EL);
  IA = GAGates*(V-EA);

  /* V */ dxdt[0] = TimeScale*(-ICa-IK-IL-IA+s)/C;
  /* w */ dxdt[1] = TimeScale*(ws-x[1])/tauw;
  /* z */ dxdt[2] = TimeScale*(zs-x[2])/TauA;
}


void MorrisLecarPrescott::init( double *x ) const
{
  x[0] = -69.39045;
  x[1] = 0.0;
  x[2] = 0.00018;
}


void MorrisLecarPrescott::conductances( vector< string > &conductancenames ) const
{
  conductancenames.clear();
  conductancenames.reserve( 4 );
  conductancenames.push_back( "g_Ca" );
  conductancenames.push_back( "g_K" );
  conductancenames.push_back( "g_l" );
  conductancenames.push_back( "g_A" );
}


void MorrisLecarPrescott::conductances( double *g ) const
{
  g[0] = GCaGates;
  g[1] = GKGates;
  g[2] = GL;
  g[3] = GAGates;
}


void MorrisLecarPrescott::currents( vector< string > &currentnames ) const
{
  currentnames.clear();
  currentnames.reserve( 4 );
  currentnames.push_back( "I_Ca" );
  currentnames.push_back( "I_K" );
  currentnames.push_back( "I_l" );
  currentnames.push_back( "I_A" );
}


void MorrisLecarPrescott::currents( double *c ) const
{
  c[0] = ICa;
  c[1] = IK;
  c[2] = IL;
  c[3] = IA;
}


void MorrisLecarPrescott::add( void )
{
  unsetNotify();

  MorrisLecar::add();

  setText( "params", "Custom|I_M|I_AHP" );

  insertSection( "Adaptation current", "Input", ModelFlag );
  addNumber( "ga", "Adaptation conductivity", GA, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "ea", "Adaptation reversal potential", EA, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "mva", "Midpoint potential of adaptation activation", MVA, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "mka", "Width of adaptation activation", MKA, 0.0, 1000.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "taua", "Adaptation time-constant", TauA, 0.0, 1000.0, 1.0, "ms" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );

  setNotify();
}


void MorrisLecarPrescott::notify( void )
{
  SpikingNeuron::notify();

  int params = index( "params" );

  if ( params == 1 ) {
    // I_M:
    ECa = +50.0;
    GCa = 20.0;
    MVCa = -1.2;
    MKCa = 18.0;
    EK = -100.0;
    GK = 20.0;
    MVK = 0.0;
    MKK = 10.0;
    MPhiK = 0.15;
    EL = -70.0;
    GL = 2.0;
    EA = -100.0;
    GA = 0.5;
    MVA = -35.0;
    MKA = 4.0;
    TauA = 100.0;
    C = 2.0;
  }
  else if ( params == 2 ) {
    // I_AHP:
    ECa = +50.0;
    GCa = 20.0;
    MVCa = -1.2;
    MKCa = 18.0;
    EK = -100.0;
    GK = 20.0;
    MVK = 0.0;
    MKK = 10.0;
    MPhiK = 0.15;
    EL = -70.0;
    GL = 2.0;
    EA = -100.0;
    GA = 5.0;
    MVA = 0.0;
    MKA = 4.0;
    TauA = 100.0;
    C = 2.0;
  }
  else {
    ECa = number( "eca" );
    GCa = number( "gca" );
    MVCa = number( "mvca" );
    MKCa = number( "mkca" );
    EK = number( "ek" );
    GK = number( "gk" );
    MVK = number( "mvk" );
    MKK = number( "mkk" );
    MPhiK = number( "mphik" );
    EL = number( "el" );
    GL = number( "gl" );
    EA = number( "ea" );
    GA = number( "ga" );
    MVA = number( "mva" );
    MKA = number( "mka" );
    TauA = number( "taua" );
    C = number( "c" );
  }
  TimeScale = number( "timescale" );
}


HodgkinHuxley::HodgkinHuxley( void )
  : SpikingNeuron()
{
  GNa = 120.0;
  GK = 36.0;
  GL = 0.3;

  ENa = +50.0;
  EK = -77.0;
  EL = -54.384;

  C = 1.0;
  PT = 1.0;

  GNaGates = GNa;
  GKGates = GK;

  INa= 0.0;
  IK= 0.0;
  IL= 0.0;
}


string HodgkinHuxley::name( void ) const
{
  return "Hodgkin-Huxley";
}


int HodgkinHuxley::dimension( void ) const
{
  return 4;
}


void HodgkinHuxley::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( dimension() );
  varnames.push_back( "V" );
  varnames.push_back( "m" );
  varnames.push_back( "h" );
  varnames.push_back( "n" );
}


void HodgkinHuxley::units( vector< string > &u ) const
{
  u.clear();
  u.reserve( dimension() );
  u.push_back( "mV" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
}


void HodgkinHuxley::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];

  double z = 0.1*(V+40.0);
  double am = fabs( z ) < 1e-4 ? 1.0 : z/(1.0-exp(-z));
  double bm = 4.0*exp(-(V+65.0)/18.0);

  double ah = 0.07*exp(-(V+65)/20.0);
  double bh = 1.0/(1.0+exp(-(V+35.0)/10.0));

  z = 0.1*(V+55.0);
  double an = fabs( z ) < 1e-4 ? 0.1 : 0.1*z/(1.0-exp(-z));
  double bn = 0.125*exp(-(V+65.0)/80.0);

  GNaGates = GNa*x[1]*x[1]*x[1]*x[2];
  GKGates = GK*x[3]*x[3]*x[3]*x[3];

  INa = GNaGates*(V-ENa);
  IK = GKGates*(V-EK);
  IL = GL*(V-EL);

  /* V */ dxdt[0] = (-INa-IK-IL+s)/C;
  /* m */ dxdt[1] = PT*( am*(1.0-x[1]) - x[1]*bm );
  /* h */ dxdt[2] = PT*( ah*(1.0-x[2]) - x[2]*bh );
  /* n */ dxdt[3] = PT*( an*(1.0-x[3]) - x[3]*bn );
}


void HodgkinHuxley::init( double *x ) const
{
  x[0] = -65.0;
  x[1] = 0.053;
  x[2] = 0.596;
  x[3] = 0.318;
}


void HodgkinHuxley::conductances( vector< string > &conductancenames ) const
{
  conductancenames.clear();
  conductancenames.reserve( 3 );
  conductancenames.push_back( "g_Na" );
  conductancenames.push_back( "g_K" );
  conductancenames.push_back( "g_l" );
}


void HodgkinHuxley::conductances( double *g ) const
{
  g[0] = GNaGates;
  g[1] = GKGates;
  g[2] = GL;
}


void HodgkinHuxley::currents( vector< string > &currentnames ) const
{
  currentnames.clear();
  currentnames.reserve( 3 );
  currentnames.push_back( "I_Na" );
  currentnames.push_back( "I_K" );
  currentnames.push_back( "I_l" );
}


void HodgkinHuxley::currents( double *c ) const
{
  c[0] = INa;
  c[1] = IK;
  c[2] = IL;
}


void HodgkinHuxley::add( void )
{
  newSection( "Sodium current", ModelFlag );
  addNumber( "gna", "Na conductivity", GNa, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "ena", "Na reversal potential", ENa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );

  newSection( "Potassium current", ModelFlag );
  addNumber( "gk", "K conductivity", GK, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "ek", "K reversal potential", EK, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );

  newSection( "Leak current", ModelFlag );
  addNumber( "gl", "Leak conductivity", GL, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "el", "Leak reversal potential", EL, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  addNumber( "c", "Capacitance", C, 0.0, 100.0, 0.1, "muF/cm^2" ).setFlags( ModelFlag );
  addNumber( "phi", "Phi", PT, 0.0, 100.0, 1.0 ).setFlags( ModelFlag );

  SpikingNeuron::add();
}


void HodgkinHuxley::notify( void )
{
  SpikingNeuron::notify();
  ENa = number( "ena" );
  GNa = number( "gna" );
  EK = number( "ek" );
  GK = number( "gk" );
  EL = number( "el" );
  GL = number( "gl" );
  C = number( "c" );
  PT = number( "phi" );
}


Abbott::Abbott( void )
  : HodgkinHuxley()
{
}


string Abbott::name( void ) const
{
  return "Abbott";
}


int Abbott::dimension( void ) const
{
  return 2;
}


void Abbott::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( dimension() );
  varnames.push_back( "V" );
  varnames.push_back( "U" );
}


void Abbott::units( vector< string > &u ) const
{
  u.clear();
  u.reserve( dimension() );
  u.push_back( "mV" );
  u.push_back( "mV" );
}


void Abbott::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];
  double U = x[1];

  double z = (U+55.0)/10.0;
  double ns = 1.0/(1.0+12.5*exp(-(U+65.0)/80.0)*
		   0.1*(fabs( z ) < 1e-4 ? 1.0 : (1.0-exp(-z))/z) );
  //    ns = 1.0/(1.0+12.5*exp(-(U+65.0)/80.0)*(1.0-exp(-(U+55.0)/10.0))/(U+55.0));

  double dU = 0.001;
  z = (U+dU+55.0)/10.0;
  double nsU = 1.0/(1.0+12.5*exp(-(U+dU+65.0)/80.0)*
		    0.1*(fabs( z ) < 1e-4 ? 1.0 : (1.0-exp(-z))/z) );
  //    nsU = 1.0/(1.0+12.5*exp(-(U+dU+65.0)/80.0)*(1.0-exp(-(U+dU+55.0)/10.0))/(U+dU+55.0));

  z = (V+55.0)/10.0;
  double nsV = 1.0/(1.0+12.5*exp(-(V+65.0)/80.0)*
		    0.1*(fabs( z ) < 1e-4 ? 1.0 : (1.0-exp(-z))/z) );
  //    nsV = 1.0/(1.0+12.5*exp(-(V+65.0)/80.0)*(1.0-exp(-(V+55.0)/10.0))/(V+55.0));

  z = (V+40.0)/10.0;
  double ms = 1.0/(1.0+40.0*exp(-(V+65)/18.0)*
		   0.1*(fabs( z ) < 1e-4 ? 1.0 : (1.0-exp(-z))/z) );
  //    ms = 1.0/(1.0+40.0*exp(-(V+65)/18.0)*(1.0-exp(-(V+40.0)/10.0))/(V+40.0));

  double hs = 1.0/(1.0+1.0/(0.07*exp(-(U+65)/20.0)*(exp(-(U+35.0)/10.0)+1.0)));
  double hsU = 1.0/(1.0+1.0/(0.07*exp(-(U+dU+65)/20.0)*(exp(-(U+dU+35.0)/10.0)+1.0)));
  double hsV = 1.0/(1.0+1.0/(0.07*exp(-(V+65)/20.0)*(exp(-(V+35.0)/10.0)+1.0)));

  double dgNa = GNa*ms*ms*ms*(V-ENa);
  double dgK = GK*4.0*ns*ns*ns*(V-EK);
  double th = 1.0/(0.07*exp(-(V+65.0)/20.0)+1.0/(exp(-(V+35.0)/10.0)+1.0));

  z = (V+55.0)/10.0;
  double tn = 1.0/(0.125*exp(-(V+65)/80.0)+
		   0.1*(fabs( z ) < 1e-4 ? 1.0 : z/(1.0-exp(-z))) );
  //    tn = 1.0/(0.125*exp(-(V+65)/80.0)+0.01*(V+55.0)/(1.0-exp(-(V+55.0)/10.0)));

  double a = dgNa*(hsV-hs)/th+dgK*(nsV-ns)/tn;
  double b = dgNa*(hsU-hs)/dU+dgK*(nsU-ns)/dU;

  GNaGates = GNa*ms*ms*ms*hs;
  GKGates = GK*ns*ns*ns*ns;

  INa = GNaGates*(V-ENa);
  IK = GKGates*(V-EK);
  IL = GL*(V-EL);

  /* V */ dxdt[0] = (-INa-IK-IL+s)/C;
  /* U */ dxdt[1] = a/b;
  /* U */// dxdt[1] = fabs(b)>1.0e-8 ? a/b : 1.0e20;
}


void Abbott::init( double *x ) const
{
  x[0] = -64.99561;
  x[1] = -64.99561;
}


Kepler::Kepler( void )
  : Abbott()
{
}


string Kepler::name( void ) const
{
  return "Kepler";
}


void Kepler::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];
  double U = x[1];

  double z = (U+55.0)/10.0;
  double ns = 1.0/(1.0+12.5*exp(-(U+65.0)/80.0)*
		   0.1*(fabs( z ) < 1e-4 ? 1.0 : (1.0-exp(-z))/z) );
  //    ns = 1.0/(1.0+12.5*exp(-(U+65.0)/80.0)*(1.0-exp(-(U+55.0)/10.0))/(U+55.0));

  double dU = 0.001;
  z = (U+dU+55.0)/10.0;
  double nsU = 1.0/(1.0+12.5*exp(-(U+dU+65.0)/80.0)*
		    0.1*(fabs( z ) < 1e-4 ? 1.0 : (1.0-exp(-z))/z) );
  //    nsU = 1.0/(1.0+12.5*exp(-(U+dU+65.0)/80.0)*(1.0-exp(-(U+dU+55.0)/10.0))/(U+dU+55.0));

  z = (V+55.0)/10.0;
  double nsV = 1.0/(1.0+12.5*exp(-(V+65.0)/80.0)*
		    0.1*(fabs( z ) < 1e-4 ? 1.0 : (1.0-exp(-z))/z) );
  //    nsV = 1.0/(1.0+12.5*exp(-(V+65.0)/80.0)*(1.0-exp(-(V+55.0)/10.0))/(V+55.0));

  z = (V+40.0)/10.0;
  double ms = 1.0/(1.0+40.0*exp(-(V+65)/18.0)*
		   0.1*(fabs( z ) < 1e-4 ? 1.0 : (1.0-exp(-z))/z) );
  //    ms = 1.0/(1.0+40.0*exp(-(V+65)/18.0)*(1.0-exp(-(V+40.0)/10.0))/(V+40.0));

  double dV = 0.001;
  z = (V+dV+40.0)/10.0;
  double msV = 1.0/(1.0+40.0*exp(-(V+dV+65)/18.0)*
		    0.1*(fabs( z ) < 1e-4 ? 1.0 : (1.0-exp(-z))/z) );
  //    msV = 1.0/(1.0+40.0*exp(-(V+dV+65)/18.0)*(1.0-exp(-(V+dV+40.0)/10.0))/(V+dV+40.0));

  double hs = 1.0/(1.0+1.0/(0.07*exp(-(U+65)/20.0)*(exp(-(U+35.0)/10.0)+1.0)));
  double hsU = 1.0/(1.0+1.0/(0.07*exp(-(U+dU+65)/20.0)*(exp(-(U+dU+35.0)/10.0)+1.0)));
  double hsV = 1.0/(1.0+1.0/(0.07*exp(-(V+65)/20.0)*(exp(-(V+35.0)/10.0)+1.0)));

  double dgNa = GNa*ms*ms*ms*(V-ENa);
  double dgK = GK*4.0*ns*ns*ns*(V-EK);
  double th = 1.0/(0.07*exp(-(V+65.0)/20.0)+1.0/(exp(-(V+35.0)/10.0)+1.0));

  z = (V+55.0)/10.0;
  double tn = 1.0/(0.125*exp(-(V+65)/80.0)+
		   0.1*(fabs( z ) < 1e-4 ? 1.0 : z/(1.0-exp(-z))) );
  //    tn = 1.0/(0.125*exp(-(V+65)/80.0)+0.01*(V+55.0)/(1.0-exp(-(V+55.0)/10.0)));

  z = (V+40.0)/10.0;
  //    tm = 1.0/(4.0*exp(-(V+65.0)/18.0)+0.1*(V+40.0)/(1.0-exp(-(V+40.0)/10.0)));
  double tm = 1.0/(4.0*exp(-(V+65.0)/18.0)+(fabs( z ) < 1e-4 ? 1.0 : z/(1.0-exp(-z))));

  double a = dgNa*(hsV-hs)/th+dgK*(nsV-ns)/tn;
  double b = dgNa*(hsU-hs)/dU+dgK*(nsU-ns)/dU;

  double g = GL + GK*ns*ns*ns*ns + GNa*ms*ms*ms*hs;
  double dFdVm = GNa*3.0*ms*ms*hs*(V-ENa)*(msV-ms)/dV;
  double alpha = 0.5*(C/tm+g - ::sqrt((C/tm+g)*(C/tm+g)-4.0*(g+dFdVm)*C/tm))/(g+dFdVm);

  GNaGates = GNa*ms*ms*ms*hs;
  GKGates = GK*ns*ns*ns*ns;

  INa = GNaGates*(V-ENa);
  IK = GKGates*(V-EK);
  IL = GL*(V-EL);

  /* V */ dxdt[0] = alpha * (-INa-IK-IL+s)/C;
  /* U */ dxdt[1] = a/b;
  /* U */// dxdt[1] = fabs(b)>1.0e-8 ? a/b : 1.0e20;
}


Connor::Connor( void )
  : HodgkinHuxley()
{
  GNa = 120.0;
  GK = 20.0;
  GKA = 47.0;
  GL = 0.3;

  ENa = +50.0;
  EK = -77.0;
  EKA = -80.0;
  EL = -22.0;

  C = 1.0;
  PT = 1.0;

  GNaGates = GNa;
  GKGates = GK;
  GKAGates = GKA;

  INa= 0.0;
  IK= 0.0;
  IKA = 0.0;
  IL= 0.0;
}


string Connor::name( void ) const
{
  return "Connor";
}


int Connor::dimension( void ) const
{
  return 6;
}


void Connor::variables( vector< string > &varnames ) const
{
  HodgkinHuxley::variables( varnames );
  varnames.push_back( "a" );
  varnames.push_back( "b" );
}


void Connor::units( vector< string > &u ) const
{
  HodgkinHuxley::units( u );
  u.push_back( "1" );
  u.push_back( "1" );
}


void Connor::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];

  double z = 0.1*(V+40.0);
  double am = fabs( z ) < 1e-4 ? 1.0 : z/(1.0-exp(-z));
  double bm = 4.0*exp(-(V+65.0)/18.0);

  double ah = 0.07*exp(-(V+65)/20.0);
  double bh = 1.0/(1.0+exp(-(V+35.0)/10.0));

  z = 0.1*(V+55.0);
  double an = fabs( z ) < 1e-4 ? 0.1 : 0.1*z/(1.0-exp(-z));
  double bn = 0.125*exp(-(V+65.0)/80.0);

  double as = pow(0.0761*exp((V+99.22)/31.84)/(1.0+exp((V+6.17)/28.93)),1.0/3.0);
  double at = (0.3632+1.158/(1.0+exp((V+60.96)/20.12)));

  double bs = 1.0/(pow(1.0+exp((V+58.3)/14.54),4.0));
  double bt = (1.24+2.678/(1.0+exp((V+55.0)/16.072)));

  GNaGates = GNa*x[1]*x[1]*x[1]*x[2];
  GKGates = GK*x[3]*x[3]*x[3]*x[3];
  GKAGates = GKA*x[4]*x[4]*x[4]*x[5];

  INa = GNaGates*(V-ENa);
  IK = GKGates*(V-EK);
  IKA = GKAGates*(V-EKA);
  IL = GL*(V-EL);

  /* V */ dxdt[0] = (-INa-IK-IKA-IL+s)/C;
  /* m */ dxdt[1] = PT*( am*(1.0-x[1]) - x[1]*bm );
  /* h */ dxdt[2] = PT*( ah*(1.0-x[2]) - x[2]*bh );
  /* n */ dxdt[3] = PT*( an*(1.0-x[3]) - x[3]*bn );
  /* a */ dxdt[4] = (as-x[4])/at;
  /* b */ dxdt[5] = (bs-x[5])/bt;
}


void Connor::init( double *x ) const
{
  x[0] = -72.975;
  x[1] = 0.01008;
  x[2] = 0.9659;
  x[3] = 0.15589;
  x[4] = 0.54044;
  x[5] = 0.28848;
}


void Connor::conductances( vector< string > &conductancenames ) const
{
  conductancenames.clear();
  conductancenames.reserve( 4 );
  conductancenames.push_back( "g_Na" );
  conductancenames.push_back( "g_K" );
  conductancenames.push_back( "g_KA" );
  conductancenames.push_back( "g_l" );
}


void Connor::conductances( double *g ) const
{
  g[0] = GNaGates;
  g[1] = GKGates;
  g[2] = GKAGates;
  g[3] = GL;
}


void Connor::currents( vector< string > &currentnames ) const
{
  currentnames.clear();
  currentnames.reserve( 4 );
  currentnames.push_back( "I_Na" );
  currentnames.push_back( "I_K" );
  currentnames.push_back( "I_KA" );
  currentnames.push_back( "I_l" );
}


void Connor::currents( double *c ) const
{
  c[0] = INa;
  c[1] = IK;
  c[2] = IKA;
  c[3] = IL;
}


void Connor::add( void )
{
  HodgkinHuxley::add();

  insertSection( "A current", "Leak current", ModelFlag );
  addNumber( "gka", "A conductivity", GKA, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "eka", "A reversal potential", EKA, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
}


void Connor::notify( void )
{
  SpikingNeuron::notify();
  HodgkinHuxley::notify();
  EKA = number( "eka" );
  GKA = number( "gka" );
}


RushRinzel::RushRinzel( void )
  : Connor()
{
  GNa = 120.0;
  GK = 20.0;
  GKA = 60.0;
  GL = 0.315; // ????

  ENa = +50.0;
  EK = -72.0;
  EKA = -72.0;
  EL = -17.0;

  AV0 = -75.0;
  ADV = -50.0;
  BV0 = -70.0;
  BDV = 6.0;
  BTau = 1.0;

  C = 1.0;
  PT = 3.82;

  GNaGates = GNa;
  GKGates = GK;
  GKAGates = GKA;

  INa= 0.0;
  IK= 0.0;
  IKA = 0.0;
  IL= 0.0;
}


string RushRinzel::name( void ) const
{
  return "Rush-Rinzel";
}


int RushRinzel::dimension( void ) const
{
  return 3;
}


void RushRinzel::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( dimension() );
  varnames.push_back( "V" );
  varnames.push_back( "n" );
  varnames.push_back( "b" );
}


void RushRinzel::units( vector< string > &u ) const
{
  u.clear();
  u.reserve( dimension() );
  u.push_back( "mV" );
  u.push_back( "1" );
  u.push_back( "1" );
}


void RushRinzel::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];

  double z = 0.1*(V+35);
  double am = fabs( z ) < 1e-4 ? 1.0 : z/(1.0-exp(-z));
  double bm = 4.0*exp(-0.05*(V+60.0));
  double m0 = am/(am+bm);

  z = 0.1*(V+50.01);
  double an = fabs( z ) < 1e-4 ? 0.1 : 0.1*z/(1.0-exp(-z));
  double bn = 0.125*exp( -0.0125*(V+60));
  double n0 = an/(an+bn);
  double tn = 1.0/(an+bn);

  double a0 = 1.0/(1.0+exp((V-AV0)/ADV));
  double b0 = 1.0/(1.0+exp((V-BV0)/BDV));
  double tb = BTau;

  GNaGates = GNa*m0*m0*m0*(0.9-1.2*x[1]);
  GKGates = GK*x[1]*x[1]*x[1]*x[1];
  GKAGates = GKA*a0*a0*a0*x[2];

  INa = GNaGates*(V-ENa);
  IK = GKGates*(V-EK);
  IKA = GKAGates*(V-EKA);
  IL = GL*(V-EL);

  /* V */ dxdt[0] = (-INa-IK-IKA-IL+s)/C;
  /* n */ dxdt[1] = PT*( n0 - x[1] )/tn;
  /* b */ dxdt[2] = (b0 - x[2])/tb;
}


void RushRinzel::init( double *x ) const
{
  x[0] = -67.78;
  x[1] = 0.207863;
  x[2] = 0.408565;
}


void RushRinzel::add( void )
{
  Connor::add();

  insertNumber( "av0", "Leak current", "Midpoint potential of a-gate", AV0, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  insertNumber( "adv", "Leak current", "Dynamic range of a-gate", ADV, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  insertNumber( "bv0", "Leak current", "Midpoint potential of b-gate", BV0, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  insertNumber( "bdv", "Leak current", "Dynamic range of b-gate", BDV, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  insertNumber( "btau", "Leak current", "Time constant of b-gate", BTau, 0.0, 1000.0, 0.2, "ms" ).setFlags( ModelFlag );
}


void RushRinzel::notify( void )
{
  SpikingNeuron::notify();
  Connor::notify();
  AV0 = number( "av0" );
  ADV = number( "adv" );
  BV0 = number( "bv0" );
  BDV = number( "bdv" );
  BTau = number( "btau" );
}


Awiszus::Awiszus( void )
  : Connor()
{
  GNa = 240.0;
  GK = 36.0;
  GKA = 61.0;
  GL = 0.068;

  ENa = +64.7;
  EK = -95.2;
  EKA = -95.2;
  EL = -51.3;

  C = 1.0;

  GNaGates = GNa;
  GKGates = GK;
  GKAGates = GKA;

  INa= 0.0;
  IK= 0.0;
  IKA = 0.0;
  IL= 0.0;
}


string Awiszus::name( void ) const
{
  return "Awiszus";
}


void Awiszus::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];
  
  double z = -(53.0+V)/6.0;
  double am = fabs( z ) < 1e-4 ? 11.3 : 11.3*z/(exp(z)-1.0);
  z = (57.0+V)/9.0;
  double bm = fabs( z ) < 1e-4 ? 37.4 : 37.4*z/(exp(z)-1.0);

  z = (V+106.0)/9.0;
  double ah = fabs( z ) < 1e-4 ? 5.0 : 5.0*z/(exp(z)-1.0);
  double bh = 22.6/(exp(-(V+22.0)/12.5)+1.0);

  double ns = 1.0/(1.0+exp((1.7-V)/11.4));
  double nt = (0.24+0.7/(1.0+exp((V+12.0)/16.4)));

  double as = 1.0/(1.0+exp(-(55+V)/13.8));
  double at = (0.12+0.6/(1.0+exp((V+24)/16.5)));

  double bs = 1.0/(1.0+exp((77.0+V)/7.8));
  double bt = (2.1+1.8/(1.0+exp((V-18.0)/5.7)));

  GNaGates = GNa*x[1]*x[1]*x[1]*x[2];
  GKGates = GK*x[3]*x[3]*x[3];
  GKAGates = GKA*x[4]*x[4]*x[4]*x[4]*x[5];

  INa = GNaGates*(V-ENa);
  IK = GKGates*(V-EK);
  IKA = GKAGates*(V-EKA);
  IL = GL*(V-EL);

  /* V */ dxdt[0] = (-INa-IK-IKA-IL+s)/C;
  /* m */ dxdt[1] = (1-x[1])*am-x[1]*bm;
  /* h */ dxdt[2] = (1-x[2])*ah-x[2]*bh;
  /* n */ dxdt[3] = (ns-x[3])/nt;
  /* a */ dxdt[4] = (as-x[4])/at;
  /* b */ dxdt[5] = (bs-x[5])/bt;
}


void Awiszus::init( double *x ) const
{
  x[0] = -67.78;
  x[1] = 0.207863;
  x[2] = 0.408565;
}


FleidervishSI::FleidervishSI( void )
  : HodgkinHuxley()
{
  GNa = 10.0;
  GK = 1.5;
  GL = 0.008;

  ENa = +50.0;
  EK = -90.0;
  EL = -70.0;

  GNa = 120.0;
  GK = 36.0;
  GL = 0.3;

  ENa = +50.0;
  EK = -77.0;
  EL = -54.384;

  C = 1.0;
  PT = 1.0;

  GNaGates = GNa;
  GKGates = GK;

  INa = 0.0;
  IK = 0.0;
  IL = 0.0;
}


string FleidervishSI::name( void ) const
{
  return "Fleidervish";
}


int FleidervishSI::dimension( void ) const
{
  return 5;
}


void FleidervishSI::variables( vector< string > &varnames ) const
{
  HodgkinHuxley::variables( varnames );
  varnames.push_back( "s" );
}


void FleidervishSI::units( vector< string > &u ) const
{
  HodgkinHuxley::units( u );
  u.push_back( "1" );
}


void FleidervishSI::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];

  double z = (V+40.0)/5.0;
  double am = fabs( z ) < 1e-4 ? 0.091*5.0 : 0.091*5.0*z/(1.0-exp(-z));
  double bm = fabs( z ) < 1e-4 ? 0.062*5.0 : -0.062*5.0*z/(1.0-exp(z));

  double ah = 0.06*exp(-(V+55.0)/15.0);
  double bh = 6.01/(1.0+exp(-(V-17.0)/21.0));

  z = (V+45.0)/5.0;
  double an = fabs( z ) < 1e-4 ? 0.034*5.0 : 0.034*5.0*z/(1.0-exp(-z));
  double bn = 0.54*exp(-(V+75.0)/40);

  double as = 0.001*exp(-(V+85.0)/30.0);
  double bs = 0.0034/(1.0+exp(-(V+17.0)/10.0));

  GNaGates = GNa*x[1]*x[1]*x[1]*x[2]*x[4];
  GKGates = GK*x[3]*x[3]*x[3]*x[3];

  INa = GNaGates*(V-ENa);
  IK = GKGates*(V-EK);
  IL = GL*(V-EL);

  /* V */ dxdt[0] = ( - INa - IK - IL + s )/C;
  /* m */ dxdt[1] = (1-x[1])*am-x[1]*bm;
  /* h */ dxdt[2] = (1-x[2])*ah-x[2]*bh;
  /* n */ dxdt[3] = (1-x[3])*an-x[3]*bn;
  /* s */ dxdt[4] = (1-x[4])*as-x[4]*bs;
}


void FleidervishSI::init( double *x ) const
{
  x[0] = -72.975;
  x[1] = 0.01008;
  x[2] = 0.9659;
  x[3] = 0.15589;
  x[4] = 0.54044;
}


TraubHH::TraubHH( void )
  : HodgkinHuxley()
{
  GNa = 100.0;
  GK = 200.0;
  GL = 0.1;

  ENa = +48.0;
  EK = -82.0;
  EL = -67.0;

  C = 1.0;
  PT = 1.0;

  GNaGates = GNa;
  GKGates = GK;

  INa = 0.0;
  IK = 0.0;
  IL = 0.0;
}


string TraubHH::name( void ) const
{
  return "Traub-Miles, HH currents only";
}


void TraubHH::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];

  double z = (V+54.0)/4.0;
  double am = fabs( z ) < 1e-4 ? 0.32*4.0 : 0.32*4.0*z/(1.0-exp(-z));
  z = (V+27.0)/5.0;
  double bm = fabs( z ) < 1e-4 ? 0.28*5.0 : 0.28*5.0*z/(exp(z)-1.0);

  double ah = 0.128*exp(-(V+50.0)/18.0);
  double bh = 4.0/(1.0+exp(-(V+27.0)/5.0));
  
  z = (V+52.0)/5.0;
  double an = fabs( z ) < 1e-4 ? 0.032*5.0 : 0.032*5.0*z/(1.0-exp(-z));
  double bn = 0.5*exp(-(V+57.0)/40.0);

  GNaGates = GNa*x[1]*x[1]*x[1]*x[2];
  GKGates = GK*x[3]*x[3]*x[3]*x[3];

  INa = GNaGates*(V-ENa);
  IK = GKGates*(V-EK);
  IL = GL*(V-EL);

  /* V */ dxdt[0] = ( - INa - IK - IL + s )/C;
  /* m */ dxdt[1] = PT*( am*(1.0-x[1]) - x[1]*bm );
  /* h */ dxdt[2] = PT*( ah*(1.0-x[2]) - x[2]*bh );
  /* n */ dxdt[3] = PT*( an*(1.0-x[3]) - x[3]*bn );
}


void TraubHH::init( double *x ) const
{
  x[0] = -66.61556;
  x[1] = 0.01596;
  x[2] = 0.99552;
  x[3] = 0.04012;
}


TraubMiles::TraubMiles( void )
  : HodgkinHuxley()
{
  GNa = 100.0;
  GK = 200.0;
  GL = 0.1;
  GCa = 119.9;
  GAHP = 3.01;

  ENa = +48.0;
  EK = -82.0;
  EL = -67.0;
  ECa = +73.0;
  EAHP = -82.0;

  C = 1.0;
  PT = 1.0;

  GNaGates = GNa;
  GKGates = GK;
  GCaGates = GCa;
  GAHPGates = GAHP;

  INa = 0.0;
  IK = 0.0;
  IL = 0.0;
  ICa = 0.0;
  IAHP = 0.0;
}


string TraubMiles::name( void ) const
{
  return "Traub-Miles";
}


int TraubMiles::dimension( void ) const
{
  return 9;
}


void TraubMiles::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( dimension() );
  varnames.push_back( "V" );
  varnames.push_back( "m" );
  varnames.push_back( "h" );
  varnames.push_back( "n" );
  varnames.push_back( "y" );
  varnames.push_back( "s" );
  varnames.push_back( "r" );
  varnames.push_back( "q" );
  varnames.push_back( "[Ca]" );
}


void TraubMiles::units( vector< string > &u ) const
{
  u.clear();
  u.reserve( dimension() );
  u.push_back( "mV" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "mM" );
}


void TraubMiles::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];
  double Ca = x[8];

  double z = (V+54.0)/4.0;
  double am = fabs( z ) < 1e-4 ? 0.32*4.0 : 0.32*4.0*z/(1.0-exp(-z));
  z = (V+27.0)/5.0;
  double bm = fabs( z ) < 1e-4 ? 0.28*5.0 : 0.28*5.0*z/(exp(z)-1.0);

  double ah = 0.128*exp(-(V+50.0)/18.0);
  double bh = 4.0/(1.0+exp(-(V+27.0)/5.0));
  
  z = (V+52.0)/5.0;
  double an = fabs( z ) < 1e-4 ? 0.032*5.0 : 0.032*5.0*z/(1.0-exp(-z));
  double bn = 0.5*exp(-(V+57.0)/40.0);

  double ay = 0.028*exp(-(V+52.0)/15.0)+2.0/(1.0+exp(-0.1*(V-18.0)));
  double by = 0.4/(1.0+exp(-0.1*(V+27.0)));

  z = 0.1*(V+7.0);
  double as = fabs( z ) < 1e-4 ? 0.4 : 0.4*z/(1.0-exp(-z));
  z = 0.1*(V+22.0);
  double bs = fabs( z ) < 1e-4 ? 0.05 : 0.05*z/(exp(z)-1.0);

  double ar = 0.005;
  z = (200.0-Ca)/20.0;
  double br = fabs( z ) < 1e-4 ? 0.025*20.0 : 0.025*20.0*z/(exp(z)-1.0);

  double aq = exp((V+67.0)/27.0)*0.005*20.0*(fabs( z ) < 1e-4 ? 1.0 : z/(exp(z)-1.0) );
  double bq = 0.002;

  GNaGates = GNa*x[1]*x[1]*x[1]*x[2];
  GKGates = GK*x[3]*x[3]*x[3]*x[3]*x[4];
  GCaGates = GCa*x[5]*x[5]*x[5]*x[5]*x[5]*x[6];
  GAHPGates = GAHP*x[7];

  INa = GNaGates*(V-ENa);
  IK = GKGates*(V-EK);
  IL = GL*(V-EL);
  ICa = GCaGates*(V-ECa);
  IAHP = GAHPGates*(V-EAHP);

  /* V */ dxdt[0] = ( - INa - IK - IL - ICa - IAHP + s )/C;
  /* m */ dxdt[1] = am*(1.0-x[1]) - x[1]*bm;
  /* h */ dxdt[2] = ah*(1.0-x[2]) - x[2]*bh;
  /* n */ dxdt[3] = an*(1.0-x[3]) - x[3]*bn;
  /* y */ dxdt[4] = ay*(1.0-x[4]) - x[4]*by;
  /* s */ dxdt[5] = as*(1.0-x[5]) - x[5]*bs;
  /* r */ dxdt[6] = ar*(1.0-x[6]) - x[6]*br;
  /* q */ dxdt[7] = aq*(1.0-x[7]) - x[7]*bq;
  /* Ca */dxdt[8] = -0.002*ICa - 0.0125*x[6];
}


void TraubMiles::init( double *x ) const
{
  x[0] = -66.61;
  x[1] = 0.015995;
  x[2] = 0.995513;
  x[3] = 0.040180;
  x[4] = 0.908844;
  x[5] = 0.026259;
  x[6] = 0.138319;
  x[7] = 0.760006;
  x[8] = 115.0;
}


void TraubMiles::conductances( vector< string > &conductancenames ) const
{
  conductancenames.clear();
  conductancenames.reserve( 5 );
  conductancenames.push_back( "g_Na" );
  conductancenames.push_back( "g_K" );
  conductancenames.push_back( "g_l" );
  conductancenames.push_back( "g_Ca" );
  conductancenames.push_back( "g_AHP" );
}


void TraubMiles::conductances( double *g ) const
{
  g[0] = GNaGates;
  g[1] = GKGates;
  g[2] = GL;
  g[3] = GCaGates;
  g[4] = GAHPGates;
}


void TraubMiles::currents( vector< string > &currentnames ) const
{
  currentnames.clear();
  currentnames.reserve( 5 );
  currentnames.push_back( "I_Na" );
  currentnames.push_back( "I_K" );
  currentnames.push_back( "I_l" );
  currentnames.push_back( "I_Ca" );
  currentnames.push_back( "I_AHP" );
}


void TraubMiles::currents( double *c ) const
{
  c[0] = INa;
  c[1] = IK;
  c[2] = IL;
  c[3] = ICa;
  c[4] = IAHP;
}


void TraubMiles::add( void )
{
  HodgkinHuxley::add();

  insertSection( "Calcium current", "Input", ModelFlag );
  addNumber( "gca", "Ca conductivity", GCa, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "eca", "Ca reversal potential", ECa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );

  insertSection( "AHP-type current", "Input", ModelFlag );
  addNumber( "gahp", "AHP conductivity", GAHP, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "eahp", "AHP reversal potential", EAHP, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
}


void TraubMiles::notify( void )
{
  HodgkinHuxley::notify();

  ECa = number( "eca" );
  GCa = number( "gca" );
  EAHP = number( "eahp" );
  GAHP = number( "gahp" );
}


TraubKepler::TraubKepler( void )
  : Abbott()
{
  // conductances are from Traub, scaled to Ermentrouts Na:
  GNa = 100.0;
  GK = 200.0;
  GL = 0.1;

  // potentials are from Traub & Miles (1991):
  ENa = +48.0;
  EK = -82.0;
  EL = -67.0;

  C = 1.0;
  PT = 1.0;

  GNaGates = GNa;
  GKGates = GK;
}


string TraubKepler::name( void ) const
{
  return "Traub-Kepler";
}


void TraubKepler::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];
  double U = x[1];

  double z1 = (V+54.0)/4.0;
  double ez1 = fabs(z1) < 1.0e-4 ? 1.0 : z1/(1.0-exp(-z1));
  double z2 = (V+27.0)/5.0;
  double ez2 = fabs(z2) < 1.0e-4 ? 1.0 : z2/(exp(z2)-1.0);
  double ms = 1.0/(1.0+0.28*5.0*ez2/ez1/0.32/4.0);
  //    ms = 1.0/(1.0+0.28*(V+27)*(1.0-exp(-(V+54.0)/4.0))/0.32/(V+54.0)/(exp((V+27)/5.0)-1.0));

  double dV = 0.001;
  double dU = 0.001;
  double dz1 = (V+dV+54.0)/4.0;
  double edz1 = fabs(dz1) < 1.0e-4 ? 1.0 : (1.0-exp(-dz1))/dz1;
  double dz2 = (V+dV+27)/5.0;
  double edz2 = fabs(dz2) < 1.0e-4 ? 1.0 : (exp(dz2)-1.0)/dz2;
  double msV = 1.0/(1.0+0.28*5.0*edz1/edz2/0.32/4.0);
//    msV = 1.0/(1.0+0.28*(V+dV+27)*(1.0-exp(-(V+dV+54.0)/4.0))/0.32/(V+dV+54.0)/(exp((V+dV+27)/5.0)-1.0));

  double hs = 1.0/(1.0+4.0/(0.128*exp(-(U+50)/18.0)*(exp(-(U+27.0)/5.0)+1.0)));
  double hsU = 1.0/(1.0+4.0/(0.128*exp(-(U+dU+50)/18.0)*(exp(-(U+dU+27.0)/5.0)+1.0)));
  double hsV = 1.0/(1.0+4.0/(0.128*exp(-(V+50)/18.0)*(exp(-(V+27.0)/5.0)+1.0)));

  double zu = (U+52.0)/5.0;
  double ezu = fabs(zu) < 1.0e-4 ? 1.0 : (1.0-exp(-zu))/zu;
  double ns = 1.0/(1.0+0.5*exp(-(U+57.0)/40.0)*ezu/0.032/5.0);
  //    ns = 1.0/(1.0+0.5*exp(-(U+57.0)/40.0)*(1.0-exp(-(U+52.0)/5.0))/0.032/(U+52.0));

  double dzu = (U+dU+52.0)/5.0;
  double edzu = fabs(dzu) < 1.0e-4 ? 1.0 : (1.0-exp(-dzu))/dzu;
  double nsU = 1.0/(1.0+0.5*exp(-(U+dU+57.0)/40.0)*edzu/0.032/5.0);
  //    nsU = 1.0/(1.0+0.5*exp(-(U+dU+57.0)/40.0)*(1.0-exp(-(U+dU+52.0)/5.0))/0.032/(U+dU+52.0));

  double z3 = (V+52.0)/5.0;
  double ez3 = fabs(z3) < 1.0e-4 ? 1.0 : (1.0-exp(-z3))/z3;
  double nsV = 1.0/(1.0+0.5*exp(-(V+57.0)/40.0)*ez3/0.032/5.0);
  //    nsV = 1.0/(1.0+0.5*exp(-(V+57.0)/40.0)*(1.0-exp(-(V+52.0)/5.0))/0.032/(V+52.0));

  double dgNa = GNa*ms*ms*ms*(V-ENa);
  double dgK = GK*4.0*ns*ns*ns*(V-EK);

  double tm = 1.0/(0.32*4.0*ez1+0.28*5.0*ez2);
  //    tm = 1.0/(0.32*(V+54)/(1.0-exp(-(V+54.0)/4.0))+0.28*(V+27.0)/(exp((V+27.0)/5.0)-1.0));
  
  double th = 1.0/(0.128*exp(-(V+50.0)/18.0)+4.0/(exp(-(V+27.0)/5.0)+1.0));

  double tn = 1.0/(0.5*exp(-(V+57)/40.0)+0.032*5.0/ez3);
  //    tn = 1.0/(0.5*exp(-(V+57)/40.0)+0.032*(V+52.0)/(1.0-exp(-(V+52.0)/5.0)));

  double a = dgNa*(hsV-hs)/th+dgK*(nsV-ns)/tn;
  double b = dgNa*(hsU-hs)/dU+dgK*(nsU-ns)/dU;

  double g = GL + GK*ns*ns*ns*ns + GNa*ms*ms*ms*hs;
  double dFdVm = GNa*3.0*ms*ms*hs*(V-ENa)*(msV-ms)/dV;
  double alpha = 0.5*(C/tm+g - ::sqrt((C/tm+g)*(C/tm+g)-4.0*(g+dFdVm)*C/tm))/(g+dFdVm);

  GNaGates = GNa*ms*ms*ms*hs;
  GKGates = GK*ns*ns*ns*ns;

  INa = GNaGates*(V-ENa);
  IK = GKGates*(V-EK);
  IL = GL*(V-EL);

  /* V */ dxdt[0] = alpha*( - INa - IK - IL + s )/C;
  /* U */ dxdt[1] = a/b;
  /* U */ // dxdt[1] = fabs(b)>ZERO ? a/b : HUGE_VAL;
}


void TraubKepler::init( double *x ) const
{
  x[0] = -66.61556;
  x[1] = -66.61556;
}


TraubErmentrout::TraubErmentrout( void )
  : HodgkinHuxley()
{
  GNa = 100.0;
  GK = 80.0;
  GL = 0.1;
  GCa = 1.0;
  GM = 5.0;
  GAHP = 5.0;

  ENa = +50.0;
  EK = -100.0;
  EL = -67.0;
  ECa = +120.0;
  EM = -100.0;
  EAHP = -100.0;

  C = 1.0;
  PT = 1.0;

  GNaGates = GNa;
  GKGates = GK;
  GCaGates = GCa;
  GMGates = GM;
  GAHPGates = GAHP;

  INa = 0.0;
  IK = 0.0;
  IL = 0.0;
  ICa = 0.0;
  IM = 0.0;
  IAHP = 0.0;
}


int TraubErmentrout::dimension( void ) const
{
  return 8;
}


void TraubErmentrout::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( dimension() );
  varnames.push_back( "V" );
  varnames.push_back( "m" );
  varnames.push_back( "h" );
  varnames.push_back( "n" );
  varnames.push_back( "s" );
  varnames.push_back( "w" );
  varnames.push_back( "q" );
  varnames.push_back( "[Ca]" );
}


void TraubErmentrout::units( vector< string > &u ) const
{
  u.clear();
  u.reserve( dimension() );
  u.push_back( "mV" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "mM" );
}


void TraubErmentrout::conductances( vector< string > &conductancenames ) const
{
  conductancenames.clear();
  conductancenames.reserve( 6 );
  conductancenames.push_back( "g_Na" );
  conductancenames.push_back( "g_K" );
  conductancenames.push_back( "g_l" );
  conductancenames.push_back( "g_Ca" );
  conductancenames.push_back( "g_M" );
  conductancenames.push_back( "g_AHP" );
}


void TraubErmentrout::conductances( double *g ) const
{
  g[0] = GNaGates;
  g[1] = GKGates;
  g[2] = GL;
  g[3] = GCaGates;
  g[4] = GMGates;
  g[5] = GAHPGates;
}


void TraubErmentrout::currents( vector< string > &currentnames ) const
{
  currentnames.clear();
  currentnames.reserve( 6 );
  currentnames.push_back( "I_Na" );
  currentnames.push_back( "I_K" );
  currentnames.push_back( "I_l" );
  currentnames.push_back( "I_Ca" );
  currentnames.push_back( "I_M" );
  currentnames.push_back( "I_AHP" );
}


void TraubErmentrout::currents( double *c ) const
{
  c[0] = INa;
  c[1] = IK;
  c[2] = IL;
  c[3] = ICa;
  c[4] = IM;
  c[5] = IAHP;
}


void TraubErmentrout::add( void )
{
  HodgkinHuxley::add();

  insertSection( "Calcium current", "Input", ModelFlag );
  addNumber( "gca", "Ca conductivity", GCa, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "eca", "Ca reversal potential", ECa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );

  insertSection( "M-type current", "Input", ModelFlag );
  addNumber( "gm", "M conductivity", GM, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "em", "M reversal potential", EM, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );

  insertSection( "AHP-type current", "Input", ModelFlag );
  addNumber( "gahp", "AHP conductivity", GAHP, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "eahp", "AHP reversal potential", EAHP, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
}


void TraubErmentrout::notify( void )
{
  HodgkinHuxley::notify();

  ECa = number( "eca" );
  GCa = number( "gca" );
  EM = number( "em" );
  GM = number( "gm" );
  EAHP = number( "eahp" );
  GAHP = number( "gahp" );
}


TraubErmentrout1998::TraubErmentrout1998( void )
  : TraubErmentrout()
{
  GNa = 100.0;
  GK = 80.0;
  GL = 0.1;
  GCa = 1.0;
  GM = 5.0;
  GAHP = 5.0;

  ENa = +50.0;
  EK = -100.0;
  EL = -67.0;
  ECa = +120.0;
  EM = -100.0;
  EAHP = -100.0;

  TauW = 100.0;
  C = 1.0;
  PT = 1.0;

  GNaGates = GNa;
  GKGates = GK;
  GCaGates = GCa;
  GMGates = GM;
  GAHPGates = GAHP;

  INa = 0.0;
  IK = 0.0;
  IL = 0.0;
  ICa = 0.0;
  IM = 0.0;
  IAHP = 0.0;
}


string TraubErmentrout1998::name( void ) const
{
  return "Traub-Miles-Ermentrout 1998";
}


void TraubErmentrout1998::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];
  double Ca = x[7];

  double z = (V+54.0)/4.0;
  double am = fabs( z ) < 1e-4 ? 0.32*4.0 : 0.32*4.0*z/(1.0-exp(-z));
  z = (V+27.0)/5.0;
  double bm = fabs( z ) < 1e-4 ? 0.28*5.0 : 0.28*5.0*z/(exp(z)-1.0);

  double ah = 0.128*exp(-(V+50.0)/18.0);
  double bh = 4.0/(1.0+exp(-(V+27.0)/5.0));
  
  z = (V+52.0)/5.0;
  double an = fabs( z ) < 1e-4 ? 0.032*5.0 : 0.032*5.0*z/(1.0-exp(-z));
  double bn = 0.5*exp(-(V+57.0)/40.0);

  x[4] = 1.0/(1.0+exp(-(V+25.0)/5.0));

  double ws = 1.0/(1.0+exp(-(V+20.0)/5.0));

  x[6] = Ca/(30.0+Ca);

  GNaGates = GNa*x[1]*x[1]*x[1]*x[2];
  GKGates = GK*x[3]*x[3]*x[3]*x[3];
  GCaGates = GCa*x[4];
  GMGates = GM*x[5];
  GAHPGates = GAHP*x[6];

  INa = GNaGates*(V-ENa);
  IK = GKGates*(V-EK);
  IL = GL*(V-EL);
  ICa = GCaGates*(V-ECa);
  IM = GMGates*(V-EM);
  IAHP = GAHPGates*(V-EAHP);

  /* V */ dxdt[0] = ( - INa - IK - IL - ICa - IM - IAHP + s )/C;
  /* m */ dxdt[1] = am*(1.0-x[1]) - x[1]*bm;
  /* h */ dxdt[2] = ah*(1.0-x[2]) - x[2]*bh;
  /* n */ dxdt[3] = an*(1.0-x[3]) - x[3]*bn;
  /* s */ dxdt[4] = 0.0;
  /* w */ dxdt[5] = (ws - x[5])/TauW;
  /* q */ dxdt[6] = 0.0;
  /* Ca */ dxdt[7] = -0.002*ICa - 0.0125*Ca;
}


void TraubErmentrout1998::init( double *x ) const
{
  x[0] = -66.71572;
  x[1] = 0.01564;
  x[2] = 0.99564;
  x[3] = 0.03947;
  x[4] = 0.00024;
  x[5] = 0.00009;
  x[6] = 0.00024;
  x[7] = 0.00711;
}


void TraubErmentrout1998::add( void )
{
  TraubErmentrout::add();
  insertNumber( "tauw", "AHP-type current", "W time constant", TauW, 0.0, 1000.0, 1.0, "ms" ).setFlags( ModelFlag );
}


void TraubErmentrout1998::notify( void )
{
  TraubErmentrout::notify();
  TauW = number( "tauw" );
}


TraubErmentrout2001::TraubErmentrout2001( void )
  : TraubErmentrout()
{
  GNa = 100.0;
  GK = 80.0;
  GL = 0.2;
  GCa = 1.0;
  GM = 1.5;
  GAHP = 1.5;

  ENa = +50.0;
  EK = -100.0;
  EL = -67.0;
  ECa = +120.0;
  EM = -100.0;
  EAHP = -100.0;

  C = 1.0;
  PT = 1.0;

  GNaGates = GNa;
  GKGates = GK;
  GCaGates = GCa;
  GMGates = GM;
  GAHPGates = GAHP;

  INa = 0.0;
  IK = 0.0;
  IL = 0.0;
  ICa = 0.0;
  IM = 0.0;
  IAHP = 0.0;
}


string TraubErmentrout2001::name( void ) const
{
  return "Traub-Miles-Ermentrout 2001";
}


void TraubErmentrout2001::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];
  double Ca = x[7];

  double z = (V+54.0)/4.0;
  double am = fabs( z ) < 1e-4 ? 0.32*4.0 : 0.32*4.0*z/(1.0-exp(-z));
  z = (V+27.0)/5.0;
  double bm = fabs( z ) < 1e-4 ? 0.28*5.0 : 0.28*5.0*z/(exp(z)-1.0);

  double ah = 0.128*exp(-(V+50.0)/18.0);
  double bh = 4.0/(1.0+exp(-(V+27.0)/5.0));
  
  z = (V+52.0)/5.0;
  double an = fabs( z ) < 1e-4 ? 0.032*5.0 : 0.032*5.0*z/(1.0-exp(-z));
  double bn = 0.5*exp(-(V+57.0)/40.0);

  x[4] = 1.0/(1.0+exp(-(V+25.0)/2.5));

  double ws = 1.0/(1.0+exp(-(V+35.0)/10.0));
  z = (V+35.0)/20.0;
  double tauw = 100.0/(3.3*exp(z) + exp(-z));

  x[6] = Ca/(Ca+1.0);

  GNaGates = GNa*x[1]*x[1]*x[1]*x[2];
  GKGates = GK*x[3]*x[3]*x[3]*x[3];
  GCaGates = GCa*x[4];
  GMGates = GM*x[5];
  GAHPGates = GAHP*x[6];

  INa = GNaGates*(V-ENa);
  IK = GKGates*(V-EK);
  IL = GL*(V-EL);
  ICa = GCaGates*(V-ECa);
  IM = GMGates*(V-EM);
  IAHP = GAHPGates*(V-EAHP);

  /* V */ dxdt[0] = ( - INa - IK - IL - ICa - IM - IAHP + s )/C;
  /* m */ dxdt[1] = am*(1.0-x[1]) - x[1]*bm;
  /* h */ dxdt[2] = ah*(1.0-x[2]) - x[2]*bh;
  /* n */ dxdt[3] = an*(1.0-x[3]) - x[3]*bn;
  /* s */ dxdt[4] = 0.0;
  /* w */ dxdt[5] = (ws - x[5])/tauw;
  /* q */ dxdt[6] = 0.0;
  /* Ca */ dxdt[7] = -0.002*ICa - 0.0125*Ca;
}


void TraubErmentrout2001::init( double *x ) const
{
  x[0] = -72.94663;
  x[1] = 0.00415;
  x[2] = 0.99911;
  x[3] = 0.01366;
  x[4] = 0.00000;
  x[5] = 0.02200;
  x[6] = 0.00000;
  x[7] = 0.00000;
}


SimplifiedTraub::SimplifiedTraub( void )
  : HodgkinHuxley()
{
  GNa = 200.0;
  GK = 100.0;
  GL = 0.2;

  ENa = +48.0;
  EK = -82.0;
  EL = -82.49;

  C = 1.0;
  PT = 1.0;

  // type I:
  MV0 = -43.0;
  MDV = 9.0;
  HV0 = -43.0;
  HDV = 9.0;
  HTDV = 14.0;
  HTOffs = 0.4;

  /*
  // type II:
  MV0 = -43.0;
  MDV = 9.0;
  HV0 = -59.5;
  HDV = 9.0;
  HTDV = 14.0;
  HTOffs = 0.4;
  */

  GNaGates = GNa;
  GKGates = GK;

  INa = 0.0;
  IK = 0.0;
  IL = 0.0;
}


string SimplifiedTraub::name( void ) const
{
  return "Simplified Traub-Miles";
}


int SimplifiedTraub::dimension( void ) const
{
  return 3;
}


void SimplifiedTraub::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( dimension() );
  varnames.push_back( "V" );
  varnames.push_back( "m" );
  varnames.push_back( "h" );
}


void SimplifiedTraub::units( vector< string > &u ) const
{
  u.clear();
  u.reserve( dimension() );
  u.push_back( "mV" );
  u.push_back( "1" );
  u.push_back( "1" );
}


void SimplifiedTraub::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];

  double m0 = 1.0/(1.0+exp(-(V-MV0)/MDV));

  double h0 = 1.0/(1.0+exp((V-HV0)/HDV));
  double ht = 10.0*exp(-((V-HV0)/HTDV)*((V-HV0)/HTDV))+HTOffs;

  GNaGates = GNa*x[1]*x[1]*x[1]*x[2];
  double ng = 1.0 - x[2];
  GKGates = GK*ng*ng*ng*ng;

  INa = GNaGates*(V-ENa);
  IK = GKGates*(V-EK);
  IL = GL*(V-EL);

  /* V */ dxdt[0] = ( - INa - IK - IL + s )/C;
  /* m */ dxdt[1] = ( m0 - x[1] )/0.1;
  /* h */ dxdt[2] = ( h0 - x[2] )/ht;
}


void SimplifiedTraub::init( double *x ) const
{
  x[0] = -82.231;
  x[1] = 0.0126297;
  x[2] = 0.98737;
}


void SimplifiedTraub::add( void )
{
  HodgkinHuxley::add();

  insertNumber( "mv0", "Potassium current", "Midpoint potential m-gate", MV0, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  insertNumber( "mdv", "Potassium current", "Width m-gate", MDV, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  insertNumber( "hv0", "Potassium current", "Midpoint potential h-gate", HV0, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  insertNumber( "hdv", "Potassium current", "Width h-gate", HDV, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  insertNumber( "htdv", "Potassium current", "Width h-gate time constant", HTDV, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  insertNumber( "htoffs", "Potassium current", "Minimum time constant of h-gate", HTOffs, 0.0, 100.0, 0.1, "ms" ).setFlags( ModelFlag );
}


void SimplifiedTraub::notify( void )
{
  HodgkinHuxley::notify();

  MV0 = number( "mv0" );
  MDV = number( "mdv" );
  HV0 = number( "hv0" );
  HDV = number( "hdv" );
  HTDV = number( "htdv" );
  HTOffs = number( "htoffs" );
}


WangBuzsaki::WangBuzsaki( void )
  : HodgkinHuxley()
{
  GNa = 35.0;
  GK = 9.0;
  GL = 0.1;

  ENa = +55.0;
  EK = -90.0;
  EL = -65.0;

  C = 1.0;
  PT = 5.0;

  GNaGates = GNa;
  GKGates = GK;

  INa = 0.0;
  IK = 0.0;
  IL = 0.0;
}


string WangBuzsaki::name( void ) const
{
  return "Wang-Buzsaki";
}


int WangBuzsaki::dimension( void ) const
{
  return 3;
}


void WangBuzsaki::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( dimension() );
  varnames.push_back( "V" );
  varnames.push_back( "h" );
  varnames.push_back( "n" );
}


void WangBuzsaki::units( vector< string > &u ) const
{
  u.clear();
  u.reserve( dimension() );
  u.push_back( "mV" );
  u.push_back( "1" );
  u.push_back( "1" );
}


void WangBuzsaki::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];

  double z = 0.1*(V+35.0);
  double ms = 1.0/(1.0+4.0*exp(-(V+60.0)/18.0)*(fabs( z ) < 1e-4 ? 1.0 : (exp(-z)-1.0)/(-z) ) );

  double ah = 0.07*exp(-(V+58.0)/20.0);
  double bh = 1.0/(exp(-0.1*(V+28.0))+1.0);

  z = 0.1*(V+34.0);
  double an = fabs( z ) < 1e-4 ? 0.1 : -0.1*z/(exp(-z)-1.0);
  double bn = 0.125*exp(-(V+44.0)/80.0);

  GNaGates = GNa*ms*ms*ms*x[1];
  GKGates = GK*x[2]*x[2]*x[2]*x[2];

  INa = GNaGates*(V-ENa);
  IK = GKGates*(V-EK);
  IL = GL*(V-EL);

  /* V */ dxdt[0] = (-INa-IK-IL+s)/C;
  /* h */ dxdt[1] = PT*(ah*(1.0-x[1])-bh*x[1]);
  /* n */ dxdt[2] = PT*(an*(1.0-x[2])-bn*x[2]);
}


void WangBuzsaki::init( double *x ) const
{
  x[0] = -64.018;
  x[1] = 0.7808;
  x[2] = 0.0891;
}


WangBuzsakiAdapt::WangBuzsakiAdapt( void )
  : WangBuzsaki()
{
  EA = -90.0;

  GA = 0.8;

  Atau = 100.0;

  GAGates = GA;
  IA = 0.0;
}


string WangBuzsakiAdapt::name( void ) const
{
  return "Wang-Buzsaki Adapt";
}


int WangBuzsakiAdapt::dimension( void ) const
{
  return 4;
}


void WangBuzsakiAdapt::variables( vector< string > &varnames ) const
{
  WangBuzsaki::variables( varnames );
  varnames.push_back( "a" );
}


void WangBuzsakiAdapt::units( vector< string > &u ) const
{
  WangBuzsaki::units( u );
  u.push_back( "1" );
}


void WangBuzsakiAdapt::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];

  double z = 0.1*(V+35.0);
  double ms = 1.0/(1.0+4.0*exp(-(V+60.0)/18.0)*(fabs( z ) < 1e-4 ? 1.0 : (exp(-z)-1.0)/(-z) ) );

  double ah = 0.07*exp(-(V+58.0)/20.0);
  double bh = 1.0/(exp(-0.1*(V+28.0))+1.0);

  z = 0.1*(V+34.0);
  double an = fabs( z ) < 1e-4 ? 0.1 : -0.1*z/(exp(-z)-1.0);
  double bn = 0.125*exp(-(V+44.0)/80.0);

  double w0 = 1.0/(exp(-(x[0]+35.0)/10.0)+1.0);

  GNaGates = GNa*ms*ms*ms*x[1];
  GKGates = GK*x[2]*x[2]*x[2]*x[2];
  GAGates = GA*x[3];

  INa = GNaGates*(x[0]-ENa);
  IK = GKGates*(x[0]-EK);
  IA = GAGates*(x[0]-EA);
  IL = GL*(x[0]-EL);

  /* V */ dxdt[0] = (-INa-IK-IL-IA+s)/C;
  /* h */ dxdt[1] = PT*(ah*(1.0-x[1])-bh*x[1]);
  /* n */ dxdt[2] = PT*(an*(1.0-x[2])-bn*x[2]);
  /* a */ dxdt[3] = ( w0 - x[3] )/Atau;
}


void WangBuzsakiAdapt::init( double *x ) const
{
  WangBuzsaki::init( x );
  x[3] = 0.0;
}


void WangBuzsakiAdapt::conductances( vector< string > &conductancenames ) const
{
  WangBuzsaki::conductances( conductancenames );
  conductancenames.push_back( "g_A" );
}


void WangBuzsakiAdapt::conductances( double *g ) const
{
  g[0] = GNaGates;
  g[1] = GKGates;
  g[2] = GL;
  g[3] = GAGates;
}


void WangBuzsakiAdapt::currents( vector< string > &currentnames ) const
{
  WangBuzsaki::currents( currentnames );
  currentnames.push_back( "I_A" );
}


void WangBuzsakiAdapt::currents( double *c ) const
{
  c[0] = INa;
  c[1] = IK;
  c[2] = IL;
  c[3] = IA;
}


void WangBuzsakiAdapt::add( void )
{
  WangBuzsaki::add();

  insertSection( "Adaptation current", "Input", ModelFlag );
  addNumber( "ga", "A conductivity", GA, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "ea", "A reversal potential", EA, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  addNumber( "atau", "A time constant", Atau, 0.0, 1000.0, 1.0, "ms" ).setFlags( ModelFlag );
}


void WangBuzsakiAdapt::notify( void )
{
  WangBuzsaki::notify();
  EA = number( "ea" );
  GA = number( "ga" );
  Atau = number( "atau" );
}


Crook::Crook( void )
  : HodgkinHuxley()
{
  GNa = 221.0;
  GK = 47.0;
  GL = 2.0;
  GCa = 8.5;
  GKAHP = 7.0;
  GKM = 6.5;
  GLD = 0.05;
  GDS = 1.1;

  // potentials are shifted by -70mV:
  ENa = +62.0;
  EK = -83.0;
  EL = -70.0;
  ECa = +127.0;

  PT = 1.0;
  C = 0.8;
  SFrac = 0.05;
  CaA = 3.0;
  CaTau = 60.0;

  GNaGates = GNa;
  GKGates = GK;
  GCaGates = GCa;
  GKAHPGates = GKAHP;
  GKMGates = GKM;
  GDSGates = GDS;
  GSDGates = GDS;

  INa = 0.0;
  IK = 0.0;
  IL = 0.0;
  ICa = 0.0;
  IKAHP = 0.0;
  IKM = 0.0;
  IDS = 0.0;
  ILD = 0.0;
  ISD = 0.0;
}


string Crook::name( void ) const
{
  return "Crook";
}


int Crook::dimension( void ) const
{
  return 10;
}


void Crook::variables( vector< string > &varnames ) const
{
  HodgkinHuxley::variables( varnames );
  varnames[0] = "VS";
  varnames.push_back( "s" );
  varnames.push_back( "r" );
  varnames.push_back( "q" );
  varnames.push_back( "w" );
  varnames.push_back( "[Ca]" );
  varnames.push_back( "VD" );
}


void Crook::units( vector< string > &u ) const
{
  HodgkinHuxley::units( u );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "mM" );
  u.push_back( "mV" );
}


void Crook::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double VS = x[0];
  double VD = x[9];
  double Ca = x[8];

  double z = 0.25*(-47.1-VS);
  double am = fabs( z ) < 1e-4 ? 0.32*4.0 : 0.32*4.0*z/(exp(z)-1.0);
  z = (VS+20.1)/5.0;
  double bm = fabs( z ) < 1e-4 ? 0.28*5.0 : 0.28*5.0*z/(exp(z)-1.0);

  double ah = 0.128*exp((-43.0-VS)/18.0);
  double bh = 4.0/(exp((-20.0-VS)/5.0)+1.0);

  z = (-25.1-VS)/5.0;
  double an = fabs( z ) < 1e-4 ? 0.59*5.0 : 0.59*5.0*z/(exp(z)-1.0);
  double bn = 0.925*exp(0.925-0.025*(VS+77));

  double as = 0.912/(exp(-0.072*(VS-5.0))+1.0);
  z = (VS+8.9)/5.0;
  double bs = fabs( z ) < 1e-4 ? 0.0114*5.0 : 0.0114*5.0*z/(exp(z)-1.0);
  
  z = exp(-(VS+60.0)/20.0);
  double r0 = z<1.0 ? z : 1.0;
  double tr = 1.0/0.005;

  double q0 = (0.0005*Ca)*(0.0005*Ca);
  double tq = 0.0338/((0.00001*Ca < 0.01 ? 0.00001*Ca : 0.01)+0.001);

  double w0 = 1.0/(exp(-(VS+35.0)/10.0)+1.0);
  double tw = 92.0*exp(-(VS+35.0)/20.0)/(1.0+0.3*exp(-(VS+35.0)/10.0));

  GNaGates = GNa*x[1]*x[1]*x[2];
  GKGates = GK*x[3];
  GCaGates = GCa*x[4]*x[4]*x[5];
  GKAHPGates = GKAHP*x[6];
  GKMGates = GKM*x[7];
  GDSGates = GDS/SFrac;
  GSDGates = GDS/SFrac;

  INa = GNaGates*(VS-ENa);
  IK = GKGates*(VS-EK);
  ICa = GCaGates*(VS-ECa);
  IKAHP = GKAHPGates*(VS-EK);
  IKM = GKMGates*(VS-EK);
  IL = GL*(VS-EL);
  IDS = GDSGates*(VS-VD);
  ILD = GLD*(VD-EL);
  ISD = -GSDGates*(VS-VD);

  /* VS */ dxdt[0] = ( - INa - IK - ICa - IKAHP - IKM - IL - IDS + s/SFrac )/C;
  /* m */  dxdt[1] = am*(1.0-x[1]) - x[1]*bm;
  /* h */  dxdt[2] = ah*(1.0-x[2]) - x[2]*bh;
  /* n */  dxdt[3] = an*(1.0-x[3]) - x[3]*bn;
  /* s */  dxdt[4] = as*(1.0-x[4]) - x[4]*bs;
  /* r */  dxdt[5] = (r0-x[5])/tr;
  /* q */  dxdt[6] = (q0-x[6])/tq;
  /* w */  dxdt[7] = (w0-x[7])/tw;
  /* Ca */ dxdt[8] = -CaA*ICa - x[8]/CaTau;
  /* VD */ dxdt[9] = ( - ILD - ISD )/C;
}


void Crook::init( double *x ) const
{
  x[0] = -71.27126;
  x[1] = 0.00128;
  x[2] = 0.99977;
  x[3] = 0.00131;
  x[4] = 0.00524;
  x[5] = 1.0;
  x[6] = 0.00002;
  x[7] = 0.02590;
  x[8] = 8.32021;
  x[9] = -71.26;
}


void Crook::conductances( vector< string > &conductancenames ) const
{
  conductancenames.clear();
  conductancenames.reserve( 9 );
  conductancenames.push_back( "g_Na" );
  conductancenames.push_back( "g_K" );
  conductancenames.push_back( "g_lS" );
  conductancenames.push_back( "g_Ca" );
  conductancenames.push_back( "g_M" );
  conductancenames.push_back( "g_AHP" );
  conductancenames.push_back( "g_DS" );
  conductancenames.push_back( "g_lD" );
  conductancenames.push_back( "g_SD" );
}


void Crook::conductances( double *g ) const
{
  g[0] = GNaGates;
  g[1] = GKGates;
  g[2] = GL;
  g[3] = GCaGates;
  g[4] = GKMGates;
  g[5] = GKAHPGates;
  g[6] = GDSGates;
  g[7] = GLD;
  g[8] = GSDGates;
}


void Crook::currents( vector< string > &currentnames ) const
{
  currentnames.clear();
  currentnames.reserve( 9 );
  currentnames.push_back( "I_Na" );
  currentnames.push_back( "I_K" );
  currentnames.push_back( "I_lS" );
  currentnames.push_back( "I_Ca" );
  currentnames.push_back( "I_M" );
  currentnames.push_back( "I_AHP" );
  currentnames.push_back( "I_DS" );
  currentnames.push_back( "I_lD" );
  currentnames.push_back( "I_SD" );
}


void Crook::currents( double *c ) const
{
  c[0] = INa;
  c[1] = IK;
  c[2] = IL;
  c[3] = ICa;
  c[4] = IKM;
  c[5] = IKAHP;
  c[6] = IDS;
  c[7] = ILD;
  c[8] = ISD;
}


void Crook::add( void )
{
  newSection( "Soma Sodium current", ModelFlag );
  addNumber( "gna", "Na conductivity", GNa, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "ena", "Na reversal potential", ENa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );

  newSection( "Soma Potassium current", ModelFlag );
  addNumber( "gk", "K conductivity", GK, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "ek", "K reversal potential", EK, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );

  newSection( "Soma Calcium current", ModelFlag );
  addNumber( "gca", "Ca conductivity", GCa, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "eca", "Ca reversal potential", ECa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  addNumber( "caa", "Ca activation", CaA, 0.0, 200.0, 1.0, "1" ).setFlags( ModelFlag );
  addNumber( "catau", "Ca removal time constant", CaTau, 0.0, 10000.0, 1.0, "ms" ).setFlags( ModelFlag );

  newSection( "Soma Leak current", ModelFlag );
  addNumber( "gl", "Leak conductivity", GL, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "el", "Leak reversal potential", EL, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  addNumber( "c", "Capacitance", C, 0.0, 100.0, 0.1, "muF/cm^2" ).setFlags( ModelFlag );

  newSection( "Other currents", ModelFlag );
  addNumber( "gahp", "Soma AHP-type K conductivity", GKAHP, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "gm", "Soma M-type K conductivity", GKM, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "gld", "Dendrite leak conductivity", GLD, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "gds", "Soma-dendrite conductivity", GDS, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "sfrac", "Soma coupling fraction", SFrac, 0.0, 1.0, 0.01 ).setFlags( ModelFlag );

  SpikingNeuron::add();
}


void Crook::notify( void )
{
  SpikingNeuron::notify();
  HodgkinHuxley::notify();
  ECa = number( "eca" );
  GCa = number( "gca" );
  CaA = number( "caa" );
  CaTau = number( "catau" );
  GKAHP = number( "gahp" );
  GKM = number( "gm" );
  GLD = number( "gld" );
  SFrac = number( "sfrac" );
}


MilesDai::MilesDai( void )
  : HodgkinHuxley()
{
  // surface areas of soma and dendrite with an additional scaling factor
  // for converting mS to muS and muF to nF.
  double AS = 3.76991e-5*1.0e3;  // surface area soma in cm^2 (d=12um, l=100um) times 1000 muS/mS
  double AD = 5.02655e-5*1.0e3;  // surface area dendrite in cm^2 (d=8um, l=200um) times 1000 muS/mS

  GNa = 120.0*AS;
  GK = 100.0*AS;
  GL = 0.142857*AS;         // 1/(7kOhm*cm^2) =  0.142857 mS/cm^2
  GCa = 4.0*AS;
  GKAHP = 1.0*AS;
  GLD = 0.142857*AD;        // 1/(7kOhm*cm^2) =  0.142857 mS/cm^2
  GDS = 1.5e-3*1.0e3;

  // all potentials have -60mV added:
  ENa = +55.0;
  EK = -70.0;
  EL = -60.0;
  ECa = +80.0;

  PT = 1.0;
  C = 1.0*AS;
  CaA = 50.0*1.0e-3;
  CaTau = 20.0;
  CD = 1.0*AD;

  GNaGates = GNa;
  GKGates = GK;
  GCaGates = GCa;
  GKAHPGates = GKAHP;

  INa = 0.0;
  IK = 0.0;
  IL = 0.0;
  ICa = 0.0;
  IKAHP = 0.0;
  IDS = 0.0;
  ILD = 0.0;
  ISD = 0.0;
}


string MilesDai::name( void ) const
{
  return "Miles-Dai";
}


int MilesDai::dimension( void ) const
{
  return 10;
}


void MilesDai::variables( vector< string > &varnames ) const
{
  HodgkinHuxley::variables( varnames );
  varnames.push_back( "s" );
  varnames.push_back( "mn" );
  varnames.push_back( "hn" );
  varnames.push_back( "q" );
  varnames.push_back( "[Ca]" );
  varnames.push_back( "VD" );
}


void MilesDai::units( vector< string > &u ) const
{
  HodgkinHuxley::units( u );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "mM" );
  u.push_back( "mV" );
}


void MilesDai::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double VS = x[0];
  double Ca = x[8];
  double VD = x[9];

  // all potentials have -60mV added:
  double alpham = 10.0/(1.0+exp(-(VS+39.0)/5.3));
  double betam = 10.0/(1.0+exp((VS+39.0)/5.3));
  // tau_m = 0.1

  double alphah = 0.83/(1.0+exp((VS+41.0)/7.0));
  double betah = 0.83/(1.0+exp(-(VS+41.0)/7.0));
  // tau_h = 1.205

  double alphas = 0.0077/(1.0+exp((VS+42.0)/9.0));
  double betas = 0.0077/(1.0+exp(-(VS+42.0)/9.0));
  // tau_s = 129.9

  double z = (VS+38.0)/10.0;
  double alphan = fabs( z ) < 1e-4 ? 0.2 : 0.2*z/(1.0-exp(-z));
  double betan = 0.25*exp(-(VS+55.0)/80.0);

  double alphamn = 0.2*exp((VS+20.0)/6.13);
  double betamn = 0.2*exp(-(VS+20.0)/55.2);
  // The additional 1 ms for the mn time constant is not in the manuscript 
  // but was in the original code from Yue Dai:
  double taumn = 1.0+1.0/(alphamn+betamn);
  double ssmn = alphamn/(alphamn+betamn);

  double alphahn = 0.05*exp(-(VS+35.0)/55.2);
  double betahn = 0.05*exp((VS+35.0)/6.13);
  // The additional 5 ms for the hn time constant is not in the manuscript 
  // but was in the original code from Yue Dai:
  double tauhn = 5.0+1.0/(alphahn+betahn);
  double sshn = alphahn/(alphahn+betahn);

  double alphaq = 4.0*Ca*Ca;
  double betaq = 0.3;

  //  GNaGates = GNa*x[1]*x[1]*x[1]*x[2]*x[4];
  GNaGates = GNa*x[1]*x[1]*x[1]*x[2];
  GKGates = GK*x[3]*x[3]*x[3]*x[3];
  GCaGates = GCa*x[5]*x[5]*x[6];
  GKAHPGates = GKAHP*x[7];

  INa = GNaGates*(VS-ENa);
  IK = GKGates*(VS-EK);
  ICa = GCaGates*(VS-ECa);
  IKAHP= GKAHPGates*(VS-EK);
  IL = GL*(VS-EL);
  IDS = GDS*(VS-VD);
  ILD = GLD*(VD-EL);
  ISD = GDS*(VD-VS);

  /* VS */ dxdt[0] = ( - INa - IK -ICa - IKAHP - IL - IDS + s )/C;
  /* m  */ dxdt[1] = (1.0-x[1])*alpham-x[1]*betam;
  /* h  */ dxdt[2] = (1.0-x[2])*alphah-x[2]*betah;
  /* n  */ dxdt[3] = (1.0-x[3])*alphan-x[3]*betan;
  /* s  */ dxdt[4] = (1.0-x[4])*alphas-x[4]*betas;
  //  /* mn */ dxdt[5] = (1.0-x[5])*alphamn-x[5]*betamn;
  //  /* hn */ dxdt[6] = (1.0-x[6])*alphahn-x[6]*betahn;
  /* mn */ dxdt[5] = (ssmn-x[5])/taumn;
  /* hn */ dxdt[6] = (sshn-x[6])/tauhn;
  /* q  */ dxdt[7] = (1.0-x[7])*alphaq-x[7]*betaq;
  /* Ca */ dxdt[8] = -CaA*ICa - x[8]/CaTau;
  /* VD */ dxdt[9] = ( - ILD - ISD )/CD;
}


void MilesDai::init( double *x ) const
{
  x[0] = -61.39687;
  x[1] = 0.01440;
  x[2] = 0.94853;
  x[3] = 0.15558;
  x[4] = 0.89834;
  x[5] = 0.00055;
  x[6] = 0.99171;
  x[7] = 0.0;
  x[8] = 0.00001;
  x[9] = -61.39022;
}


void MilesDai::conductances( vector< string > &conductancenames ) const
{
  conductancenames.clear();
  conductancenames.reserve( 8 );
  conductancenames.push_back( "g_Na" );
  conductancenames.push_back( "g_K" );
  conductancenames.push_back( "g_lS" );
  conductancenames.push_back( "g_Ca" );
  conductancenames.push_back( "g_AHP" );
  conductancenames.push_back( "g_DS" );
  conductancenames.push_back( "g_lD" );
  conductancenames.push_back( "g_SD" );
}


void MilesDai::conductances( double *g ) const
{
  g[0] = GNaGates;
  g[1] = GKGates;
  g[2] = GL;
  g[3] = GCaGates;
  g[4] = GKAHPGates;
  g[5] = GDS;
  g[6] = GLD;
  g[7] = GDS;
}


string MilesDai::conductanceUnit( void ) const
{
  return "muS";
}


void MilesDai::currents( vector< string > &currentnames ) const
{
  currentnames.clear();
  currentnames.reserve( 9 );
  currentnames.push_back( "I_Na" );
  currentnames.push_back( "I_K" );
  currentnames.push_back( "I_lS" );
  currentnames.push_back( "I_Ca" );
  currentnames.push_back( "I_AHP" );
  currentnames.push_back( "I_DS" );
  currentnames.push_back( "I_lD" );
  currentnames.push_back( "I_SD" );
}


void MilesDai::currents( double *c ) const
{
  c[0] = INa;
  c[1] = IK;
  c[2] = IL;
  c[3] = ICa;
  c[4] = IKAHP;
  c[5] = IDS;
  c[6] = ILD;
  c[7] = ISD;
}


string MilesDai::currentUnit( void ) const
{
  return "nA";
}


string MilesDai::inputUnit( void ) const
{
  return "nA";
}


void MilesDai::add( void )
{
  newSection( "Soma Sodium current", ModelFlag );
  addNumber( "gna", "Na conductivity", GNa, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "ena", "Na reversal potential", ENa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );

  newSection( "Soma Delayed Rectifier Potassium current", ModelFlag );
  addNumber( "gk", "K conductivity", GK, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "ek", "K reversal potential", EK, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );

  newSection( "Soma N-Type Calcium current", ModelFlag );
  addNumber( "gca", "Ca conductivity", GCa, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "eca", "Ca reversal potential", ECa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  addNumber( "caa", "Ca activation", CaA, 0.0, 200.0, 1.0, "1" ).setFlags( ModelFlag );
  addNumber( "catau", "Ca removal time constant", CaTau, 0.0, 10000.0, 1.0, "ms" ).setFlags( ModelFlag );

  newSection( "Soma Leak current", ModelFlag );
  addNumber( "gl", "Leak conductivity", GL, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "el", "Leak reversal potential", EL, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  addNumber( "c", "Capacitance", C, 0.0, 100.0, 0.1, "nF" ).setFlags( ModelFlag );

  newSection( "Other currents", ModelFlag );
  addNumber( "gahp", "Soma AHP-type potassium conductivity", GKAHP, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "gld", "Dendrite leak conductivity", GLD, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "gds", "Soma-dendrite coupling conductivity", GDS, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "cd", "Capacitance of dendrite", CD, 0.0, 100.0, 0.1, "nF" ).setFlags( ModelFlag );

  SpikingNeuron::add();
}


void MilesDai::notify( void )
{
  SpikingNeuron::notify();
  HodgkinHuxley::notify();
  ECa = number( "eca" );
  GCa = number( "gca" );
  CaA = number( "caa" );
  CaTau = number( "catau" );
  GKAHP = number( "gahp" );
  GLD = number( "gld" );
  GDS = number( "gds" );
}


WangIKNa::WangIKNa( void )
  : HodgkinHuxley()
{
  GNa = 45.0;
  GK = 18.0;
  GL = 0.1;
  GCaS = 1.0;
  GKCaS = 5.0;
  GKNa = 5.0;
  GDS = 2.0;
  GLD = 0.1;
  GCaD = 1.0;
  GKCaD = 5.0;

  ENa = +55.0;
  EK = -80.0;
  EL = -65.0;
  ECa = +120.0;

  C = 1.0;
  PT = 4.0;

  CaSA = 0.002;
  CaSTau = 240.0;
  CaDA = 0.00067;
  CaDTau = 80.0;

  GNaGates = GNa;
  GKGates = GK;
  GCaSGates = GCaS;
  GKCaSGates = GKCaS;
  GKNaGates = GKNa;
  GDSGates = GDS;
  GCaDGates = GCaD;
  GKCaDGates = GKCaD;
  GSDGates = GDS;

  INa = 0.0;
  IK = 0.0;
  IL = 0.0;
  ICaS = 0.0;
  IKCaS = 0.0;
  IKNa = 0.0;
  IDS = 0.0;
  ILD = 0.0;
  ICaD = 0.0;
  IKCaD = 0.0;
  ISD = 0.0;
}


string WangIKNa::name( void ) const
{
  return "Wang I_KNa";
}


int WangIKNa::dimension( void ) const
{
  return 11;
}


void WangIKNa::variables( vector< string > &varnames ) const
{
  HodgkinHuxley::variables( varnames );
  varnames[0] = "VS";
  varnames.push_back( "vs" );
  varnames.push_back( "[CaS]" );
  varnames.push_back( "ws" );
  varnames.push_back( "[Na]" );
  varnames.push_back( "VD" );
  varnames.push_back( "vd" );
  varnames.push_back( "[CaD]" );
}


void WangIKNa::units( vector< string > &u ) const
{
  HodgkinHuxley::units( u );
  u.push_back( "1" );
  u.push_back( "uM" );
  u.push_back( "1" );
  u.push_back( "mM" );
  u.push_back( "mV" );
  u.push_back( "1" );
  u.push_back( "uM" );
}


void WangIKNa::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double VS = x[0];
  double CaS = x[5];
  double Na = x[7];
  double Na3 = Na*Na*Na;
  double Nae3 = 8.0*8.0*8.0;
  double CaD = x[10];
  double VD = x[8];

  double z = -0.1*(VS+33.0);
  double ms = 1.0/(1.0+4.0*exp(-(VS+58.0)/12.0)*( fabs( z ) < 1e-4 ? 1.0 : (exp(z)-1.0)/z ) );
  x[1] = ms;

  double ah = 0.07*exp(-(VS+50.0)/10.0);
  double bh = 1.0/(exp(-0.1*(VS+20.0))+1.0);

  z = -0.1*(VS+34.0);
  double an = fabs( z ) < 1e-4 ? 0.1 : 0.1*z/(exp(z)-1.0);
  double bn = 0.125*exp(-(VS+44.0)/25.0);

  double vs = 1.0/(1.0+exp(-(VS+20.0)/9.0));
  x[4] = vs;
  double ws = 0.37/(1.0+pow(38.7/Na,3.5));
  x[6] = ws;
  double vd = 1.0/(1.0+exp(-(VD+20.0)/9.0));
  x[9] = vd;

  GNaGates = GNa*ms*ms*ms*x[2];
  GKGates = GK*x[3]*x[3]*x[3]*x[3];
  GCaSGates = GCaS*vs*vs;
  GKCaSGates = GKCaS*CaS/(CaS+30.0);
  GKNaGates = GKNa*ws;
  GDSGates = GDS/0.5;
  GCaDGates = GCaD*vd*vd;
  GKCaDGates = GKCaD*CaD/(CaD+30.0);
  GSDGates = GDS/(1.0-0.5);

  INa = GNaGates*(VS-ENa);
  IK = GKGates*(VS-EK);
  IL = GL*(VS-EL);
  ICaS = GCaSGates*(VS-ECa);
  IKCaS = GKCaSGates*(VS-EK);
  IKNa = GKNaGates*(VS-EK);
  IDS = GDSGates*(VS-VD);
  ILD = GLD*(VD-EL);
  ICaD = GCaDGates*(VD-ECa);
  IKCaD = GKCaDGates*(VD-EK);
  ISD = GSDGates*(VD-VS);

  /* VS */  dxdt[0] = ( - INa - IK - IL - ICaS - IKCaS - IKNa - IDS + s )/C;
  /* m */   dxdt[1] = 0.0;
  /* h */   dxdt[2] = PT*(ah*(1.0-x[2])-x[2]*bh);
  /* n */   dxdt[3] = PT*(an*(1.0-x[3])-x[3]*bn);
  /* vs */  dxdt[4] = 0.0;
  /* CaS */ dxdt[5] = -CaSA*ICaS - x[5]/CaSTau;
  /* ws */  dxdt[6] = 0.0;
  /* Na */  dxdt[7] = -0.0003*INa - 3.0*0.0006*( Na3/(Na3+15.0*15.0*15.0) - Nae3/(Nae3+15.0*15.0*15.0));
  /* VD */  dxdt[8] = ( - ILD - ICaD - IKCaD - ISD )/C;
  /* vd */  dxdt[9] = 0.0;
  /* CaD */ dxdt[10] = -CaDA*ICaD - x[10]/CaDTau;
}


void WangIKNa::init( double *x ) const
{
  x[0] = -64.86;
  x[1] = 0.019024;
  x[2] = 0.965235;
  x[3] = 0.048809;
  x[4] = 0.006798;
  x[5] = 0.004101;
  x[6] = 0.0;
  x[7] = 2.5494;
  x[8] = -64.86;
  x[9] = 0.0068;
  x[10] = 0.00046;
}


void WangIKNa::conductances( vector< string > &conductancenames ) const
{
  conductancenames.clear();
  conductancenames.reserve( 11 );
  conductancenames.push_back( "g_Na" );
  conductancenames.push_back( "g_K" );
  conductancenames.push_back( "g_lS" );
  conductancenames.push_back( "g_CaS" );
  conductancenames.push_back( "g_KCaS" );
  conductancenames.push_back( "g_KNa" );
  conductancenames.push_back( "g_DS" );
  conductancenames.push_back( "g_lD" );
  conductancenames.push_back( "g_CaD" );
  conductancenames.push_back( "g_KCaD" );
  conductancenames.push_back( "g_SD" );
}


void WangIKNa::conductances( double *g ) const
{
  g[0] = GNaGates;
  g[1] = GKGates;
  g[2] = GL;
  g[3] = GCaSGates;
  g[4] = GKCaSGates;
  g[5] = GKNaGates;
  g[6] = GDSGates;
  g[7] = GLD;
  g[8] = GCaDGates;
  g[9] = GKCaDGates;
  g[10] = GSDGates;
}


void WangIKNa::currents( vector< string > &currentnames ) const
{
  currentnames.clear();
  currentnames.reserve( 11 );
  currentnames.push_back( "I_Na" );
  currentnames.push_back( "I_K" );
  currentnames.push_back( "I_lS" );
  currentnames.push_back( "I_CaS" );
  currentnames.push_back( "I_KCaS" );
  currentnames.push_back( "I_KNa" );
  currentnames.push_back( "I_DS" );
  currentnames.push_back( "I_lD" );
  currentnames.push_back( "I_CaD" );
  currentnames.push_back( "I_KCaD" );
  currentnames.push_back( "I_SD" );
}


void WangIKNa::currents( double *c ) const
{
  c[0] = INa;
  c[1] = IK;
  c[2] = IL;
  c[3] = ICaS;
  c[4] = IKCaS;
  c[5] = IKNa;
  c[6] = IDS;
  c[7] = ILD;
  c[8] = ICaD;
  c[9] = IKCaD;
  c[10] = ISD;
}


void WangIKNa::add( void )
{
  newSection( "Soma Sodium current", ModelFlag );
  addNumber( "gna", "Na conductivity", GNa, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "ena", "Na reversal potential", ENa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );

  newSection( "Soma Potassium current", ModelFlag );
  addNumber( "gk", "K conductivity", GK, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "ek", "K reversal potential", EK, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );

  newSection( "Soma Calcium current", ModelFlag );
  addNumber( "gcas", "Ca conductivity", GCaS, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "eca", "Ca reversal potential", ECa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  addNumber( "casa", "Ca activation", CaSA, 0.0, 200.0, 1.0, "1" ).setFlags( ModelFlag );
  addNumber( "castau", "Ca removal time constant", CaSTau, 0.0, 10000.0, 1.0, "ms" ).setFlags( ModelFlag );

  newSection( "Soma Leak current", ModelFlag );
  addNumber( "gl", "Leak conductivity", GL, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "el", "Leak reversal potential", EL, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  addNumber( "c", "Capacitance", C, 0.0, 100.0, 0.1, "muF/cm^2" ).setFlags( ModelFlag );
  addNumber( "phi", "Phi", PT, 0.0, 100.0, 1.0 ).setFlags( ModelFlag );

  newSection( "Dendrite Calcium current", ModelFlag );
  addNumber( "gcad", "Ca conductivity", GCaD, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "cada", "Ca activation", CaDA, 0.0, 200.0, 1.0, "1" ).setFlags( ModelFlag );
  addNumber( "cadtau", "Ca removal time constant", CaDTau, 0.0, 10000.0, 1.0, "ms" ).setFlags( ModelFlag );

  newSection( "Other currents", ModelFlag );
  addNumber( "gkcas", "Soma Ca dependent K conductivity", GKCaS, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "gkna", "Soma Na dependent K conductivity", GKNa, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "gds", "Soma-dendrite conductivity", GDS, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "gld", "Dendrite leak conductivity", GLD, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );
  addNumber( "gkcad", "Dendrite Ca dependent K conductivity", GKCaD, 0.0, 10000.0, 0.1, conductanceUnit() ).setFlags( ModelFlag );

  SpikingNeuron::add();
}


void WangIKNa::notify( void )
{
  SpikingNeuron::notify();
  HodgkinHuxley::notify();
  ECa = number( "eca" );
  GCaS = number( "gcas" );
  CaSA = number( "casa" );
  CaSTau = number( "castau" );
  GCaD = number( "gcad" );
  CaDA = number( "cada" );
  CaDTau = number( "cadtau" );
  GKCaS = number( "gkcas" );
  GKNa = number( "gkna" );
  GDS = number( "gds" );
  GLD = number( "gld" );
  GKCaD = number( "gkcad" );
}


Edman::Edman( void )
  : SpikingNeuron()
{
  A = 1.0e-3;     // cm^2
  Vol = 1.25e-6;  // cm^3 

  GNa = 5.6e-4;   // cm/s
  GK = 2.4e-4;    // cm/s
  GLNa = 5.8e-8;  // cm/s
  GLK = 1.8e-6;   // cm/s
  GLCl = 1.1e-7;  // cm/s
  GP = 3.0e-10;   // mol/cm^2/s

  Narest = 10.0;  // mM
  Krest = 160.0;  // mM
  ClI = 46.0;     // mM

  NaO = 325.0;    // mM, from methods: solutions
  KO = 5.0;       // mM, from methods: solutions
  ClO = 325.0+5.0+2.0*25.0+2.0*4.0+26.0; // mM, from methods: solutions

  Vm = -13.0; // mV
  Vh = -35.0; // mV
  Vl = -53.0; // mV
  Vn = -18.0; // mV
  Vr = -61.0; // mV

  Tmmax = 0.3;    // ms
  Thmax = 5.0;    // ms
  Tlmax = 1700.0; // ms
  Tnmax = 6.0;    // ms
  Trmax = 1200.0; // ms

  Km = 7.7;       // mM
  C = 7.8;        // muF/cm^2
  T = 291.0;      // K, 18GradCelsius

  GNaGates = GNa;
  GKGates = GK;
  GLNaA = GLNa;
  GLKA = GLK;
  GLClA = GLCl;
  GPA = GP;

  INa= 0.0;
  IK= 0.0;
  ILNa = 0.0;
  ILK = 0.0;
  ILCl = 0.0;
  IP = 0.0;

  FRT=0.001*Faraday/GasConst/T; // 1/mV
  F2RT=0.001*Faraday*Faraday/GasConst/T;  // makes all currents measured in muA
  ekT=-0.001*eCharge/kBoltz/T;  // -1/mV, without the negative sign all the activation functions and time constants are wrong
}


string Edman::name( void ) const
{
  return "Edman";
}


int Edman::dimension( void ) const
{
  return 7;
}


void Edman::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( dimension() );
  varnames.push_back( "V" );
  varnames.push_back( "m" );
  varnames.push_back( "h" );
  varnames.push_back( "l" );
  varnames.push_back( "n" );
  varnames.push_back( "r" );
  varnames.push_back( "[Na]" );
}


void Edman::units( vector< string > &u ) const
{
  u.clear();
  u.reserve( dimension() );
  u.push_back( "mV" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "1" );
  u.push_back( "mM" );
}


void Edman::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  const double dm=0.3, dh=0.5, dl=0.3, dn=0.3, dr=0.5;
  const double zm=3.1, zh=-4.0, zl=-3.5, zn=2.6, zr=-4.0;
  const double vm=0.0, vh=0.0, vl=0.0, vn=0.03, vr=0.3;

  double V = x[0];
  double Na = x[6];
  double K = Krest - ( Na - Narest );

  double ms = vm+(1.0-vm)/(1.0+exp(zm*ekT*(V-Vm)));
  double hs = vh+(1.0-vh)/(1.0+exp(zh*ekT*(V-Vh)));
  double ls = vl+(1.0-vl)/(1.0+exp(zl*ekT*(V-Vl)));
  double ns = vn+(1.0-vn)/(1.0+exp(zn*ekT*(V-Vn)));
  double rs = vr+(1.0-vr)/(1.0+exp(zr*ekT*(V-Vr)));

  double tm = Tmmax*(pow( (1.0-dm)/dm, dm ) + pow( (1.0-dm)/dm, dm-1.0 ))/(exp(dm*zm*ekT*(V-Vm))+exp((dm-1.0)*zm*ekT*(V-Vm)));
  double th = Thmax*(pow( (1.0-dh)/dh, dh ) + pow( (1.0-dh)/dh, dh-1.0 ))/(exp(dh*zh*ekT*(V-Vh))+exp((dh-1.0)*zh*ekT*(V-Vh)));
  double tl = Tlmax*(pow( (1.0-dl)/dl, dl ) + pow( (1.0-dl)/dl, dl-1.0 ))/(exp(dl*zl*ekT*(V-Vl))+exp((dl-1.0)*zl*ekT*(V-Vl)));
  double tn = Tnmax*(pow( (1.0-dn)/dn, dn ) + pow( (1.0-dn)/dn, dn-1.0 ))/(exp(dn*zn*ekT*(V-Vn))+exp((dn-1.0)*zn*ekT*(V-Vn)));
  double tr = Trmax*(pow( (1.0-dr)/dr, dr ) + pow( (1.0-dr)/dr, dr-1.0 ))/(exp(dr*zr*ekT*(V-Vr))+exp((dr-1.0)*zr*ekT*(V-Vr)));

  GNaGates = A*GNa*x[1]*x[1]*x[2]*x[3];
  GKGates = A*GK*x[4]*x[4]*x[5];
  GLNaA = A*GLNa;
  GLKA = A*GLK;
  GLClA = A*GLCl;
  GPA = A*GP;

  INa = GNaGates*V*F2RT*(NaO-Na*exp(V*FRT))/(1.0-exp(V*FRT));
  IK = GKGates*V*F2RT*(KO-K*exp(V*FRT))/(1.0-exp(V*FRT));
  ILNa = GLNaA*V*F2RT*(NaO-Na*exp(V*FRT))/(1.0-exp(V*FRT));
  ILK = GLKA*V*F2RT*(KO-K*exp(V*FRT))/(1.0-exp(V*FRT));
  ILCl = GLClA*V*F2RT*(ClO-ClI*exp(-V*FRT))/(1.0-exp(-V*FRT));
  IP = 1.0e6*GPA*Faraday/3.0/pow( 1.0+Km/Na, 3.0 );

  /* V */ dxdt[0] = ( - INa - IK - ILNa - ILK - ILCl - IP + 0.001*s )/C/A;
  /* m */ dxdt[1] = ( ms - x[1] ) / tm;
  /* h */ dxdt[2] = ( hs - x[2] ) / th;
  /* l */ dxdt[3] = ( ls - x[3] ) / tl;
  /* n */ dxdt[4] = ( ns - x[4] ) / tn;
  /* r */ dxdt[5] = ( rs - x[5] ) / tr;
  /* Na */ dxdt[6] = - 1.0e-3 * ( INa + ILNa + 3.0*IP ) / Faraday / Vol;
}


void Edman::init( double *x ) const
{
  x[0] = -64.89677;
  x[1] = 0.00163;
  x[2] = 0.99158;
  x[3] = 0.84030;
  x[4] = 0.03744;
  x[5] = 0.75540;
  x[6] = 9.95128;
}


void Edman::conductances( vector< string > &conductancenames ) const
{
  conductancenames.clear();
  conductancenames.reserve( 6 );
  conductancenames.push_back( "g_Na" );
  conductancenames.push_back( "g_K" );
  conductancenames.push_back( "g_lNa" );
  conductancenames.push_back( "g_lK" );
  conductancenames.push_back( "g_lCl" );
  conductancenames.push_back( "g_P" );
}


void Edman::conductances( double *g ) const
{
  g[0] = GNaGates;
  g[1] = GKGates;
  g[2] = GLNaA;
  g[3] = GLKA;
  g[4] = GLClA;
  g[5] = GPA;
}


string Edman::conductanceUnit( void ) const
{
  return "cm^3/s";
}


void Edman::currents( vector< string > &currentnames ) const
{
  currentnames.clear();
  currentnames.reserve( 6 );
  currentnames.push_back( "I_Na" );
  currentnames.push_back( "I_K" );
  currentnames.push_back( "I_lNa" );
  currentnames.push_back( "I_lK" );
  currentnames.push_back( "I_lCl" );
  currentnames.push_back( "I_P" );
}


void Edman::currents( double *c ) const
{
  c[0] = INa;
  c[1] = IK;
  c[2] = ILNa;
  c[3] = ILK;
  c[4] = ILCl;
  c[5] = IP;
}


string Edman::currentUnit( void ) const
{
  return "muA";
}


string Edman::inputUnit( void ) const
{
  return "nA";
}


void Edman::add( void )
{
  newSection( "Sodium current", ModelFlag );
  addNumber( "gna", "Na conductivity", GNa, 0.0, 10000.0, 0.1, "cm/s" ).setFlags( ModelFlag );
  addNumber( "glna", "Na leak conductivity", GLNa, 0.0, 10000.0, 0.1, "cm/s" ).setFlags( ModelFlag );
  addNumber( "naex", "Extracellular concentration", NaO, 0.0, 1000.0, 1.0, "mM" ).setFlags( ModelFlag );
  addNumber( "narest", "Intracellular concentration at rest", Narest, 0.0, 1000.0, 1.0, "mM" ).setFlags( ModelFlag );
  addNumber( "vm", "Midpoint potential of m gate", Vm, -1000.0, 1000.0, 0.1, "mV" ).setFlags( ModelFlag );
  addNumber( "tmmax", "Maximum time constant of m gate", Tmmax, 0.0, 1000.0, 0.1, "ms" ).setFlags( ModelFlag );
  addNumber( "vh", "Midpoint potential of h gate", Vh, -1000.0, 1000.0, 0.1, "mV" ).setFlags( ModelFlag );
  addNumber( "thmax", "Maximum time constant of h gate", Thmax, 0.0, 1000.0, 0.1, "ms" ).setFlags( ModelFlag );
  addNumber( "vl", "Midpoint potential of l gate", Vl, -1000.0, 1000.0, 0.1, "mV" ).setFlags( ModelFlag );
  addNumber( "tlmax", "Maximum time constant of l gate", Tlmax, 0.0, 1000.0, 01., "ms" ).setFlags( ModelFlag );
  newSection( "Potassium current", ModelFlag );
  addNumber( "gk", "K conductivity", GK, 0.0, 10000.0, 0.1, "cm/s" ).setFlags( ModelFlag );
  addNumber( "glk", "K leak conductivity", GLK, 0.0, 10000.0, 0.1, "cm/s" ).setFlags( ModelFlag );
  addNumber( "kex", "Extracellular concentration", KO, 0.0, 1000.0, 1.0, "mM" ).setFlags( ModelFlag );
  addNumber( "krest", "Intracellular concentration at rest", Krest, 0.0, 1000.0, 1.0, "mM" ).setFlags( ModelFlag );
  addNumber( "vn", "Midpoint potential of n gate", Vn, -1000.0, 1000.0, 0.1, "mV" ).setFlags( ModelFlag );
  addNumber( "tnmax", "Maximum time constant of n gate", Tnmax, 0.0, 1000.0, 0.1, "ms" ).setFlags( ModelFlag );
  addNumber( "vr", "Midpoint potential of r gate", Vr, -1000.0, 1000.0, 0.1, "mV" ).setFlags( ModelFlag );
  addNumber( "trmax", "Maximum time constant of r gate", Trmax, 0.0, 1000.0, 01., "ms" ).setFlags( ModelFlag );
  newSection( "Chloride current", ModelFlag );
  addNumber( "glcl", "Cl leak conductivity", GLCl, 0.0, 10000.0, 0.1, "cm/s" ).setFlags( ModelFlag );
  addNumber( "clex", "Extracellular concentration", ClO, 0.0, 1000.0, 1.0, "mM" ).setFlags( ModelFlag );
  addNumber( "clin", "Intracellular concentration", ClI, 0.0, 1000.0, 1.0, "mM" ).setFlags( ModelFlag );
  newSection( "Na-K Pump", ModelFlag );
  addNumber( "gp", "Maximum extrusion capacity", GP, 0.0, 1.0, 1.0e-11, "mol/cm^2/s" ).setFlags( ModelFlag );
  addNumber( "km", "Dissociation constant", Km, 0.0, 100.0, 0.1, "mM" ).setFlags( ModelFlag );
  newSection( "Other", ModelFlag );
  addNumber( "c", "Capacitance", C, 0.0, 100.0, 0.1, "muF/cm^2" ).setFlags( ModelFlag );
  addNumber( "area", "Membrane area", A, 0.0, 1.0, 0.0001, "cm^2" ).setFlags( ModelFlag );
  addNumber( "volume", "Cell volume", Vol, 0.0, 1.0, 1e-7, "cm^3" ).setFlags( ModelFlag );
  addNumber( "temp", "Temperature", T, 0.0, 1000.0, 1.0, "K" ).setFlags( ModelFlag );

  SpikingNeuron::add();
}


void Edman::notify( void )
{
  SpikingNeuron::notify();

  GNa = number( "gna" );
  GLNa = number( "glna" );
  NaO = number( "naex" );
  Narest = number( "narest" );
  Vm = number( "vm" );
  Tmmax = number( "tmmax" );
  Vh = number( "vh" );
  Thmax = number( "thmax" );
  Vl = number( "vl" );
  Tlmax = number( "tlmax" );

  GK = number( "gk" );
  GLK = number( "glk" );
  KO = number( "kex" );
  Krest = number( "krest" );
  Vn = number( "vn" );
  Tnmax = number( "tnmax" );
  Vr = number( "vr" );
  Trmax = number( "trmax" );

  GLCl = number( "glcl" );
  ClO = number( "clex" );
  ClI = number( "clin" );

  GP = number( "gp" );
  Km = number( "km" );

  C = number( "c" );
  A = number( "area" );
  Vol = number( "volume" );
  T = number( "temp" );

  FRT=0.001*Faraday/GasConst/T; // 1/mV
  F2RT=0.001*Faraday*Faraday/GasConst/T;  // makes all currents measured in muA
  ekT=-0.001*eCharge/kBoltz/T;  // -1/mV, without the negative sign all the activation functions and time constants are wrong
}


Chacron2007::Chacron2007( void )
  : HodgkinHuxley()
{
  GNa = 55.0;
  GK = 40.0;
  GL = 0.18;

  ENa = +40.0;
  EK = -88.5;
  EL = -70.0;

  C = 1.0;
  PT = 1.0;

  GNaGates = GNa;
  GKGates = GK;

  INa = 0.0;
  IK = 0.0;
  IL = 0.0;
}


string Chacron2007::name( void ) const
{
  return "Chacron2007";
}


int Chacron2007::dimension( void ) const
{
  return 5;
}


void Chacron2007::variables( vector< string > &varnames ) const
{
  HodgkinHuxley::variables( varnames );
  varnames.push_back( "Vth" );
}


void Chacron2007::units( vector< string > &u ) const
{
  HodgkinHuxley::units( u );
  u.push_back( "mV" );
}


string Chacron2007::conductanceUnit( void ) const
{
  return "uS";
}


string Chacron2007::currentUnit( void ) const
{
  return "pA";
}


string Chacron2007::inputUnit( void ) const
{
  return "pA";
}


void Chacron2007::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];
  double Vth = x[4];

  double m0 = 1.0/(1.0+exp(-(V-Vth)/3.0));
  double h0 = 1.0/(1.0+exp((V-Vth)/3.0));
  double n0 = 1.0/(1.0+exp(-(V+40.0)/3.0));

  double taum = 0.02;
  double tauh = 0.39;
  double taun = 0.39;

  GNaGates = GNa*x[1]*x[1]*x[1]*x[2];
  GKGates = GK*x[3]*x[3];

  INa = GNaGates*(V-ENa);
  IK = GKGates*(V-EK);
  IL = GL*(V-EL);

  /* V */ dxdt[0] = ( - INa - IK - IL + s )/C;
  /* m */ dxdt[1] = (m0-x[1])/taum;
  /* h */ dxdt[2] = (h0-x[2])/tauh;
  /* n */ dxdt[3] = (n0-x[3])/taun;
  /* Vth */ dxdt[4] = (-55.0-x[4])/40.0;
}


void Chacron2007::init( double *x ) const
{
  x[0] = -69.98990;
  x[1] = 0.00672;
  x[2] = 0.99328;
  x[3] = 0.0000;
  x[4] = -55.0;
}


void Chacron2007::add( void )
{
  HodgkinHuxley::add();
  setUnit( "c", "pF" );
  erase( "phi" );
}


}; /* namespace relacs */
