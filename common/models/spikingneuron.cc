/*
  spikingneuron.cc
  Base class for a spiking (point-) neuron.

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
#include <relacs/common/spikingneuron.h>


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


void SpikingNeuron::currents( vector< string > &currentnames ) const
{
  currentnames.clear();
}


void SpikingNeuron::currents( double *c ) const
{
}


void SpikingNeuron::add( void )
{
  addLabel( "Input", ScalingFlag );
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
  varnames.reserve( 1 );
  varnames.push_back( "Stimulus" );
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
  varnames.reserve( 2 );
  varnames.push_back( "V" );
  varnames.push_back( "W" );
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
  addLabel( "Parameter", ModelFlag );
  addNumber( "phi", "Phi", Phi, 0.0, 100.0, 0.1 ).setFlags( ModelFlag );
  addNumber( "a", "a", A, -100.0, 100.0, 0.1 ).setFlags( ModelFlag );
  addNumber( "b", "b", B, -100.0, 100.0, 0.1 ).setFlags( ModelFlag );

  SpikingNeuron::add();
  insertNumber( "timescale", "gain", "Timescale", TimeScale, 0.0, 1000.0, 0.001, "ms" ).setFlags( ScalingFlag );
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
  // source??
  GCa = 4.0;
  GK = 8.0;
  GL = 2.0;

  ECa = +120.0;
  EK = -80.0;
  EL = -60.0;

  MVCa = -1.2;
  MKCa = 18.0;
  MVK = 12.0;
  MKK = 17.4;
  MPhiK = 0.067;

  C = 20.0;
  TimeScale = 10.0;
  Gain = 1.0;
  Offset = 40.0;

  GCaM = GCa;
  GKW = GK;

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
  varnames.reserve( 2 );
  varnames.push_back( "V" );
  varnames.push_back( "w" );
}


void MorrisLecar::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double m = 1.0/(1.0+exp(-2.0*(x[0]-MVCa)/MKCa));
  double w = 1.0/(1.0+exp(-2.0*(x[0]-MVK)/MKK));
  double tau = 1.0/(MPhiK*cosh((x[0]-MVK)/MKK));

  GCaM = GCa*m;
  GKW = GK*x[1];

  ICa = GCaM*(x[0]-ECa);
  IK = GKW*(x[0]-EK);
  IL = GL*(x[0]-EL);

  /* V */ dxdt[0] = TimeScale*(-ICa-IK-IL+s)/C;
  /* w */ dxdt[1] = TimeScale*(w-x[1])/tau;
}


void MorrisLecar::init( double *x ) const
{
  x[0] = -59.469;
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
  g[0] = GCaM;
  g[1] = GKW;
  g[2] = GL;
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


void MorrisLecar::add( void )
{
  addLabel( "General", ModelFlag );
  addSelection( "params", "Parameter set", "Custom|Type I|Type II" ).setFlags( ModelFlag );

  addLabel( "Calcium current", ModelFlag );
  addNumber( "gca", "Ca conductivity", GCa, 0.0, 10000.0, 0.1, "nS" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "eca", "Ca reversal potential", ECa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "mvca", "Midpoint potential of Ca activation", MVCa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "mkca", "Width of Ca activation", MKCa, 0.0, 1000.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );

  addLabel( "Potassium current", ModelFlag );
  addNumber( "gk", "K conductivity", GK, 0.0, 10000.0, 0.1, "nS" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "ek", "K reversal potential", EK, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "mvk", "Midpoint potential of K activation", MVK, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "mkk", "Width of K activation", MKK, 0.0, 1000.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "mphik", "Rate of K activation", MPhiK, 0.0, 10.0, 0.001, "kHz" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );

  addLabel( "Leak current", ModelFlag );
  addNumber( "gl", "Leak conductivity", GL, 0.0, 10000.0, 0.1, "nS" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "el", "Leak reversal potential", EL, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );
  addNumber( "c", "Capacitance", C, 0.0, 100.0, 0.1, "pF" ).setFlags( ModelFlag ).setActivation( "params", "Custom" );

  SpikingNeuron::add();
  insertNumber( "timescale", "gain", "Timescale", TimeScale, 0.0, 1000.0, 0.001 ).setFlags( ScalingFlag );
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

  GNaM3H = GNa;
  GKN4 = GK;

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
  varnames.reserve( 4 );
  varnames.push_back( "V" );
  varnames.push_back( "m" );
  varnames.push_back( "h" );
  varnames.push_back( "n" );
}


void HodgkinHuxley::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  GNaM3H = GNa*x[1]*x[1]*x[1]*x[2];
  GKN4 = GK*x[3]*x[3]*x[3]*x[3];

  INa = GNaM3H*(x[0]-ENa);
  IK = GKN4*(x[0]-EK);
  IL = GL*(x[0]-EL);

  /* V */ dxdt[0] = (-INa-IK-IL+s)/C;
  /* m */ dxdt[1] = PT*( 0.1*(x[0]+40.0)/(1.0-exp(-(x[0]+40.0)/10.0))*(1.0-x[1]) - x[1]*4.0*exp(-(x[0]+65.0)/18.0) );
  /* h */ dxdt[2] = PT*( 0.07*exp(-(x[0]+65)/20.0)*(1.0-x[2]) - x[2]*1.0/(1.0+exp(-(x[0]+35.0)/10.0)) );
  /* n */ dxdt[3] = PT*( 0.01*(x[0]+55.0)/(1.0-exp(-(x[0]+55.0)/10.0))*(1.0-x[3]) - x[3]*0.125*exp(-(x[0]+65.0)/80.0) );
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
  g[0] = GNaM3H;
  g[1] = GKN4;
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
  addLabel( "Sodium current", ModelFlag );
  addNumber( "gna", "Na conductivity", GNa, 0.0, 10000.0, 0.1, "mS/cm^2" ).setFlags( ModelFlag );
  addNumber( "ena", "Na reversal potential", ENa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );

  addLabel( "Potassium current", ModelFlag );
  addNumber( "gk", "K conductivity", GK, 0.0, 10000.0, 0.1, "mS/cm^2" ).setFlags( ModelFlag );
  addNumber( "ek", "K reversal potential", EK, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );

  addLabel( "Leak current", ModelFlag );
  addNumber( "gl", "Leak conductivity", GL, 0.0, 10000.0, 0.1, "mS/cm^2" ).setFlags( ModelFlag );
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

  GNaM3H = GNa;
  GKN4 = GK;

  INa = 0.0;
  IK = 0.0;
  IL = 0.0;
}


string TraubHH::name( void ) const
{
  return "Traub & Miles (1991) HH currents only";
}


void TraubHH::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];

  double am = 0.32*(V+54.0)/(1.0-exp(-(V+54.0)/4));
  double bm = 0.28*(V+27.0)/(exp((V+27.0)/5.0)-1.0);

  double ah = 0.128*exp(-(V+50.0)/18.0);
  double bh = 4.0/(1.0+exp(-(V+27.0)/5.0));
  
  double an = 0.032*(V+52.0)/(1-exp(-(V+52.0)/5.0));
  double bn = 0.5*exp(-(V+57.0)/40.0);

  GNaM3H = GNa*x[1]*x[1]*x[1]*x[2];
  GKN4 = GK*x[3]*x[3]*x[3]*x[3];

  INa = GNa*(V-ENa);
  IK = GK*(V-EK);
  IL = GL*(V-EL);

  /* V */ dxdt[0] = ( - INa - IK - IL + s )/C;
  /* m */ dxdt[1] = PT*( am*(1.0-x[1]) - x[1]*bm );
  /* h */ dxdt[2] = PT*( ah*(1.0-x[2]) - x[2]*bh );
  /* n */ dxdt[3] = PT*( an*(1.0-x[3]) - x[3]*bn );
}


void TraubHH::init( double *x ) const
{
  x[0] = -38.6761;
  x[1] = 0.580671;
  x[2] = 0.161987;
  x[3] = 0.591686;
}


Traub::Traub( void )
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

  GNaM3H = GNa;
  GKN4 = GK;
  GCaS5R = GCa;
  GAHPQ = GAHP;

  INa = 0.0;
  IK = 0.0;
  IL = 0.0;
  ICa = 0.0;
  IAHP = 0.0;
}


string Traub::name( void ) const
{
  return "Traub-Miles (1991)";
}


int Traub::dimension( void ) const
{
  return 9;
}


void Traub::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( 9 );
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


void Traub::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double V = x[0];
  double Ca = x[8];

  double am = 0.32*(V+54.0)/(1.0-exp(-(V+54.0)/4.0));
  double bm = 0.28*(V+27.0)/(exp((V+27.0)/5.0)-1.0);

  double ah = 0.128*exp(-(V+50.0)/18.0);
  double bh = 4.0/(1.0+exp(-(V+27.0)/5.0));
  
  double an = 0.032*(V+52.0)/(1-exp(-(V+52.0)/5.0));
  double bn = 0.5*exp(-(V+57.0)/40.0);

  double ay = 0.028*exp(-(V+52.0)/15.0)+2.0/(1.0+exp(-0.1*(V-18.0)));
  double by = 0.4/(1.0+exp(-0.1*(V+27.0)));

  double as = 0.04*(V+7.0)/(1.0-exp(-0.1*(V+7.0)));
  double bs = 0.005*(V+22.0)/(exp(0.1*(V+22.0))-1.0);

  double ar = 0.005;
  double br = 0.025*(200.0-Ca)/(exp((200.0-Ca)/20.0)-1.0);

  double aq = exp((V+67.0)/27.0)*0.005*(200.0-Ca)/(exp((200.0-Ca)/20.0)-1.0);
  double bq = 0.002;

  GNaM3H = GNa*x[1]*x[1]*x[1]*x[2];
  GKN4 = GK*x[3]*x[3]*x[3]*x[3]*x[4];
  GCaS5R = GCa*x[5]*x[5]*x[5]*x[5]*x[5]*x[6];
  GAHPQ = GAHP*x[7];

  INa = GNa*(V-ENa);
  IK = GK*(V-EK);
  IL = GL*(V-EL);
  ICa = GCa*(V-ECa);
  IAHP = GAHP*(V-EAHP);

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


void Traub::init( double *x ) const
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


void Traub::conductances( vector< string > &conductancenames ) const
{
  conductancenames.clear();
  conductancenames.reserve( 5 );
  conductancenames.push_back( "g_Na" );
  conductancenames.push_back( "g_K" );
  conductancenames.push_back( "g_l" );
  conductancenames.push_back( "g_Ca" );
  conductancenames.push_back( "g_AHP" );
}


void Traub::conductances( double *g ) const
{
  g[0] = GNaM3H;
  g[1] = GKN4;
  g[2] = GL;
  g[3] = GCaS5R;
  g[4] = GAHPQ;
}


void Traub::currents( vector< string > &currentnames ) const
{
  currentnames.clear();
  currentnames.reserve( 5 );
  currentnames.push_back( "I_Na" );
  currentnames.push_back( "I_K" );
  currentnames.push_back( "I_l" );
  currentnames.push_back( "I_Ca" );
  currentnames.push_back( "I_AHP" );
}


void Traub::currents( double *c ) const
{
  c[0] = INa;
  c[1] = IK;
  c[2] = IL;
  c[3] = ICa;
  c[4] = IAHP;
}


void Traub::add( void )
{
  HodgkinHuxley::add();

  insertLabel( "Calcium current", "Input", ModelFlag );
  insertNumber( "gca", "Input", "Ca conductivity", GCa, 0.0, 10000.0, 0.1, "mS/cm^2" ).setFlags( ModelFlag );
  insertNumber( "eca", "Input", "Ca reversal potential", ECa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );

  insertLabel( "AHP-type current", "Input", ModelFlag );
  insertNumber( "gahp", "Input", "AHP conductivity", GAHP, 0.0, 10000.0, 0.1, "mS/cm^2" ).setFlags( ModelFlag );
  insertNumber( "eahp", "Input", "AHP reversal potential", EAHP, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
}


void Traub::notify( void )
{
  HodgkinHuxley::notify();

  ECa = number( "eca" );
  GCa = number( "gca" );
  EAHP = number( "eahp" );
  GAHP = number( "gahp" );
}


TraubErmentrout::TraubErmentrout( void )
  : HodgkinHuxley()
{
  GNa = 100.0;
  GK = 80.0;
  GL = 0.1;
  GCa = 5.0;
  GM = 8.0;
  GAHP = 4.0;

  ENa = +50.0;
  EK = -100.0;
  EL = -67.0;
  ECa = +120.0;
  EM = -100.0;
  EAHP = -100.0;

  TauW = 100.0;
  C = 1.0;
  PT = 1.0;

  GNaM3H = GNa;
  GKN4 = GK;
  GCaS = GCa;
  GMW = GM;
  GAHPQ = GAHP;

  INa = 0.0;
  IK = 0.0;
  IL = 0.0;
  ICa = 0.0;
  IM = 0.0;
  IAHP = 0.0;
}


string TraubErmentrout::name( void ) const
{
  return "Traub-Miles (1991) / Ermentrout";
}


int TraubErmentrout::dimension( void ) const
{
  return 8;
}


void TraubErmentrout::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( 8 );
  varnames.push_back( "V" );
  varnames.push_back( "m" );
  varnames.push_back( "h" );
  varnames.push_back( "n" );
  varnames.push_back( "s" );
  varnames.push_back( "w" );
  varnames.push_back( "q" );
  varnames.push_back( "[Ca]" );
}


void TraubErmentrout::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  GNaM3H = GNa*x[1]*x[1]*x[1]*x[2];
  GKN4 = GK*x[3]*x[3]*x[3]*x[3];

  INa = GNaM3H*(x[0]-ENa);
  IK = GKN4*(x[0]-EK);
  IL = GL*(x[0]-EL);

  /* V */ dxdt[0] = (-INa-IK-IL+s)/C;
  /* m */ dxdt[1] = PT*( 0.1*(x[0]+40.0)/(1.0-exp(-(x[0]+40.0)/10.0))*(1.0-x[1]) - x[1]*4.0*exp(-(x[0]+65.0)/18.0) );
  /* h */ dxdt[2] = PT*( 0.07*exp(-(x[0]+65)/20.0)*(1.0-x[2]) - x[2]*1.0/(1.0+exp(-(x[0]+35.0)/10.0)) );
  /* n */ dxdt[3] = PT*( 0.01*(x[0]+55.0)/(1.0-exp(-(x[0]+55.0)/10.0))*(1.0-x[3]) - x[3]*0.125*exp(-(x[0]+65.0)/80.0) );


  double V = x[0];
  double Ca = x[7];

  double am = 0.32*(V+54.0)/(1.0-exp(-(V+54.0)/4));
  double bm = 0.28*(V+27.0)/(exp((V+27.0)/5.0)-1.0);

  double ah = 0.128*exp(-(V+50.0)/18.0);
  double bh = 4.0/(1.0+exp(-(V+27.0)/5.0));
  
  double an = 0.032*(V+52.0)/(1-exp(-(V+52.0)/5.0));
  double bn = 0.5*exp(-(V+57.0)/40.0);

  x[4] = 1.0/(1.0+exp(-(V+25.0)/5.0));
  x[6] = Ca/(30.0+Ca);

  GNaM3H = GNa*x[1]*x[1]*x[1]*x[2];
  GKN4 = GK*x[3]*x[3]*x[3]*x[3];
  GCaS = GCa*x[4];
  GMW = GM*x[5];
  GAHPQ = GAHP*x[6];

  INa = GNaM3H*(V-ENa);
  IK = GKN4*(V-EK);
  IL = GL*(V-EL);
  ICa = GCaS*(V-ECa);
  IM = GMW*(V-EM);
  IAHP = GAHPQ*(V-EAHP);

  /* V */ dxdt[0] = ( - INa - IK - IL - ICa - IM - IAHP + s )/C;
  /* m */ dxdt[1] = am*(1.0-x[1]) - x[1]*bm;
  /* h */ dxdt[2] = ah*(1.0-x[2]) - x[2]*bh;
  /* n */ dxdt[3] = an*(1.0-x[3]) - x[3]*bn;
  /* s */ dxdt[4] = 0.0;
  /* w */ dxdt[5] = (1.0/(1.0+exp(-(V+20.0)/5.0)) - x[5])/TauW;
  /* q */ dxdt[6] = 0.0;
  /* Ca */ dxdt[7] = -0.002*ICa - 0.0125*Ca;
}


void TraubErmentrout::init( double *x ) const
{
  x[0] = -66.01;
  x[1] = 0.018030;
  x[2] = 0.994788;
  x[3] = 0.044163;
  x[4] = 0.000274;
  x[5] = 0.000137;
  x[6] = 0.001291;
  x[7] = 0.038781;
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
  g[0] = GNaM3H;
  g[1] = GKN4;
  g[2] = GL;
  g[3] = GCaS;
  g[4] = GMW;
  g[5] = GAHPQ;
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

  insertLabel( "Calcium current", "Input", ModelFlag );
  insertNumber( "gca", "Input", "Ca conductivity", GCa, 0.0, 10000.0, 0.1, "mS/cm^2" ).setFlags( ModelFlag );
  insertNumber( "eca", "Input", "Ca reversal potential", ECa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );

  insertLabel( "M-type current", "Input", ModelFlag );
  insertNumber( "gm", "Input", "M conductivity", GM, 0.0, 10000.0, 0.1, "mS/cm^2" ).setFlags( ModelFlag );
  insertNumber( "em", "Input", "M reversal potential", EM, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  insertNumber( "tauw", "Input", "W time constant", TauW, 0.0, 1000.0, 1.0, "ms" ).setFlags( ModelFlag );

  insertLabel( "AHP-type current", "Input", ModelFlag );
  insertNumber( "gahp", "Input", "AHP conductivity", GAHP, 0.0, 10000.0, 0.1, "mS/cm^2" ).setFlags( ModelFlag );
  insertNumber( "eahp", "Input", "AHP reversal potential", EAHP, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
}


void TraubErmentrout::notify( void )
{
  HodgkinHuxley::notify();

  ECa = number( "eca" );
  GCa = number( "gca" );
  EM = number( "em" );
  GM = number( "gm" );
  TauW = number( "tauw" );
  EAHP = number( "eahp" );
  GAHP = number( "gahp" );
}


TraubErmentroutNaSI::TraubErmentroutNaSI( void )
  : TraubErmentrout()
{
}


string TraubErmentroutNaSI::name( void ) const
{
  return "Traub-Miles (1991) / Ermentrout with slow inactivating sodium current";
}


int TraubErmentroutNaSI::dimension( void ) const
{
  return 9;
}


void TraubErmentroutNaSI::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( 9 );
  varnames.push_back( "V" );
  varnames.push_back( "m" );
  varnames.push_back( "h" );
  varnames.push_back( "l" );
  varnames.push_back( "n" );
  varnames.push_back( "s" );
  varnames.push_back( "w" );
  varnames.push_back( "q" );
  varnames.push_back( "[Ca]" );
}


void TraubErmentroutNaSI::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double dl=0.3;
  double zl=-3.5;
  double vl=0.0;
  double tlmax=1700.0;
  double Vl=-53.0;
  double T=291.0;  /* K, 18GradCelsius */
  double e=1.60217653e-19; /* C   */
  double k=1.3806505e-23; /*  J/K */
  double eKT=0.001*e/k/T; /*  */

  double V = x[0];
  double Ca = x[8];

  double am = 0.32*(V+54.0)/(1.0-exp(-(V+54.0)/4));
  double bm = 0.28*(V+27.0)/(exp((V+27.0)/5.0)-1.0);

  double ah = 0.128*exp(-(V+50.0)/18.0);
  double bh = 4.0/(1.0+exp(-(V+27.0)/5.0));
  
  double an = 0.032*(V+52.0)/(1-exp(-(V+52.0)/5.0));
  double bn = 0.5*exp(-(V+57.0)/40.0);

  double tl = tlmax*(pow( (1.0-dl)/dl, dl ) + pow( (1.0-dl)/dl, dl-1.0 ))/(exp(dl*zl*eKT*(V-Vl))+exp((dl-1.0)*zl*eKT*(V-Vl)));
  double ls = vl+(1.0-vl)/(1.0+exp(-zl*eKT*(V-Vl)));

  x[5] = 1.0/(1.0+exp(-(V+25.0)/5.0));
  x[7] = Ca/(30.0+Ca);

  GNaM3H = GNa*x[1]*x[1]*x[1]*x[2]*x[3];
  GKN4 = GK*x[4]*x[4]*x[4]*x[4];
  GCaS = GCa*x[5];
  GMW = GM*x[6];
  GAHPQ = GAHP*x[7];

  INa = GNaM3H*(V-ENa);
  IK = GKN4*(V-EK);
  IL = GL*(V-EL);
  ICa = GCaS*(V-ECa);
  IM = GMW*(V-EM);
  IAHP = GAHPQ*(V-EAHP);

  /* V */ dxdt[0] = ( - INa - IK - IL - ICa - IM - IAHP + s )/C;
  /* m */ dxdt[1] = am*(1.0-x[1]) - x[1]*bm;
  /* h */ dxdt[2] = ah*(1.0-x[2]) - x[2]*bh;
  /* l */ dxdt[3] = ( ls - x[3] ) / tl;
  /* n */ dxdt[4] = an*(1.0-x[4]) - x[4]*bn;
  /* s */ dxdt[5] = 0.0;
  /* w */ dxdt[6] = (1.0/(1.0+exp(-(V+20.0)/5.0)) - x[6])/TauW;
  /* q */ dxdt[7] = 0.0;
  /* Ca */ dxdt[8] = -0.002*ICa - 0.0125*Ca;
}


void TraubErmentroutNaSI::init( double *x ) const
{
  x[0] = -66.01;
  x[1] = 0.018030;
  x[2] = 0.994788;
  x[3] = 1.0;
  x[4] = 0.044163;
  x[5] = 0.000274;
  x[6] = 0.000137;
  x[7] = 0.001291;
  x[8] = 0.038781;
}


Wang::Wang( void )
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

  GNaM3H = GNa;
  GKN4 = GK;

  INa = 0.0;
  IK = 0.0;
  IL = 0.0;
}


string Wang::name( void ) const
{
  return "Wang";
}


int Wang::dimension( void ) const
{
  return 3;
}


void Wang::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( 3 );
  varnames.push_back( "V" );
  varnames.push_back( "h" );
  varnames.push_back( "n" );
}


void Wang::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double ms = 1.0/(1.0+4.0*exp(-(x[0]+60.0)/18.0)*(exp(-0.1*(x[0]+35.0))-1.0)/(-0.1*(x[0]+35.0)));

  GNaM3H = GNa*ms*ms*ms*x[1];
  GKN4 = GK*x[2]*x[2]*x[2]*x[2];

  INa = GNaM3H*(x[0]-ENa);
  IK = GKN4*(x[0]-EK);
  IL = GL*(x[0]-EL);

  /* V */ dxdt[0] = (-INa-IK-IL+s)/C;
  /* h */ dxdt[1] = PT*(0.07*exp(-(x[0]+58)/20)*(1.0-x[1])-x[1]/(exp(-0.1*(x[0]+28))+1));
  /* n */ dxdt[2] = PT*(-0.01*(x[0]+34.0)*(1.0-x[2])/(exp(-0.1*(x[0]+34.0))-1)-0.125*exp(-(x[0]+44.0)/80.0)*x[2]);
}


void Wang::init( double *x ) const
{
  x[0] = -64.018;
  x[1] = 0.7808;
  x[2] = 0.0891;
}


WangAdapt::WangAdapt( void )
  : Wang()
{
  EA = -90.0;

  GA = 0.8;

  Atau = 100.0;

  GAA = GA;
  IA = 0.0;
}


string WangAdapt::name( void ) const
{
  return "WangAdapt";
}


int WangAdapt::dimension( void ) const
{
  return 4;
}


void WangAdapt::variables( vector< string > &varnames ) const
{
  Wang::variables( varnames );
  varnames.push_back( "a" );
}


void WangAdapt::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double ms = 1.0/(1.0+4.0*exp(-(x[0]+60.0)/18.0)*(exp(-0.1*(x[0]+35.0))-1.0)/(-0.1*(x[0]+35.0)));
  double w0 = 1.0/(exp(-(x[0]+35.0)/10.0)+1.0);

  GNaM3H = GNa*ms*ms*ms*x[1];
  GKN4 = GK*x[2]*x[2]*x[2]*x[2];
  GAA = GA*x[3];

  INa = GNaM3H*(x[0]-ENa);
  IK = GKN4*(x[0]-EK);
  IA = GAA*(x[0]-EA);
  IL = GL*(x[0]-EL);

  /* V */ dxdt[0] = (-INa-IK-IL-IA+s)/C;
  /* h */ dxdt[1] = PT*(0.07*exp(-(x[0]+58)/20)*(1.0-x[1])-x[1]/(exp(-0.1*(x[0]+28))+1));
  /* n */ dxdt[2] = PT*(-0.01*(x[0]+34.0)*(1.0-x[2])/(exp(-0.1*(x[0]+34.0))-1)-0.125*exp(-(x[0]+44.0)/80.0)*x[2]);
  /* a */ dxdt[3] = ( w0 - x[3] )/Atau;
}


void WangAdapt::init( double *x ) const
{
  Wang::init( x );
  x[3] = 0.0;
}


void WangAdapt::conductances( vector< string > &conductancenames ) const
{
  Wang::conductances( conductancenames );
  conductancenames.push_back( "g_A" );
}


void WangAdapt::conductances( double *g ) const
{
  g[0] = GNaM3H;
  g[1] = GKN4;
  g[2] = GL;
  g[3] = GAA;
}


void WangAdapt::currents( vector< string > &currentnames ) const
{
  Wang::currents( currentnames );
  currentnames.push_back( "I_A" );
}


void WangAdapt::currents( double *c ) const
{
  c[0] = INa;
  c[1] = IK;
  c[2] = IL;
  c[3] = IA;
}


void WangAdapt::add( void )
{
  Wang::add();

  insertLabel( "Adaptation current", "Input", ModelFlag );
  insertNumber( "ga", "Input", "A conductivity", GA, 0.0, 10000.0, 0.1, "mS/cm^2" ).setFlags( ModelFlag );
  insertNumber( "ea", "Input", "A reversal potential", EA, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
  insertNumber( "atau", "Input", "A time constant", Atau, 0.0, 1000.0, 1.0, "ms" ).setFlags( ModelFlag );
}


void WangAdapt::notify( void )
{
  Wang::notify();
  EA = number( "ea" );
  GA = number( "ga" );
  Atau = number( "atau" );
}


Edman::Edman( void )
  : SpikingNeuron()
{
  GNa = 5.6e-4;
  GNa = 0.004;
  GNa = 0.015;
  GNa = 0.02;
  GK = 2.4e-4;
  GLNa = 5.8e-8;
  GLK = 1.8e-6;
  GLK = 1.0e-5;
  GLCl = 1.1e-7;
  GP = 3.0e-10;

  C = 7.8;

  GNaM2HL = GNa;
  GKN2R = GK;
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
  varnames.reserve( 7 );
  varnames.push_back( "V" );
  varnames.push_back( "m" );
  varnames.push_back( "h" );
  varnames.push_back( "l" );
  varnames.push_back( "n" );
  varnames.push_back( "r" );
  varnames.push_back( "[Na]" );
}


void Edman::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  double A=1.0e-3;
  double vol=1.25e-6; /* cm^3 */
  double Narest=0.001*10.0, Krest=0.001*160.0, Clrest=0.001*46.0;
  double NaO=0.001*325.0, KO=0.001*5.0, ClO=0.001*(325.0+5.0+2.0*25.0+2.0*4.0+26.0);
  double dm=0.3, dh=0.5, dl=0.3, dn=0.3, dr=0.5;
  double zm=3.1, zh=-4.0, zl=-3.5, zn=2.6, zr=-4.0;
  double vm=0.0, vh=0.0, vl=0.0, vn=0.03, vr=0.3;
  double tmmax=0.3, thmax=5.0, tlmax=1700.0, tnmax=6.0, trmax=1200.0;
  double Vm=-13.0, Vh=-35.0, Vl=-53.0, Vn=-18.0, Vr=-61.0;
  double Km=7.7;
  double F=96485.0; /* C/mol */
  double R=8.3144; /* J/K/mol */
  double T=291.0;  /* K, 18GradCelsius */
  double FRT=0.001*F/R/T;
  double F2RT=F*F/R/T;
  double e=1.60217653e-19; /* C   */
  double k=1.3806505e-23; /*  J/K */
  double eKT=0.001*e/k/T; /*  */

  double V = x[0];
  double Na = x[6];
  double K = Krest - ( Na - Narest );
  double Cl = Clrest;

  double ms = vm+(1.0-vm)/(1.0+exp(-zm*eKT*(V-Vm)));
  double hs = vh+(1.0-vh)/(1.0+exp(-zh*eKT*(V-Vh)));
  double ls = vl+(1.0-vl)/(1.0+exp(-zl*eKT*(V-Vl)));
  double ns = vn+(1.0-vn)/(1.0+exp(-zn*eKT*(V-Vn)));
  double rs = vr+(1.0-vr)/(1.0+exp(-zr*eKT*(V-Vr)));

  double tm = tmmax*(pow( (1.0-dm)/dm, dm ) + pow( (1.0-dm)/dm, dm-1.0 ))/(exp(dm*zm*eKT*(V-Vm))+exp((dm-1.0)*zm*eKT*(V-Vm)));
  double th = thmax*(pow( (1.0-dh)/dh, dh ) + pow( (1.0-dh)/dh, dh-1.0 ))/(exp(dh*zh*eKT*(V-Vh))+exp((dh-1.0)*zh*eKT*(V-Vh)));
  double tl = tlmax*(pow( (1.0-dl)/dl, dl ) + pow( (1.0-dl)/dl, dl-1.0 ))/(exp(dl*zl*eKT*(V-Vl))+exp((dl-1.0)*zl*eKT*(V-Vl)));
  double tn = tnmax*(pow( (1.0-dn)/dn, dn ) + pow( (1.0-dn)/dn, dn-1.0 ))/(exp(dn*zn*eKT*(V-Vn))+exp((dn-1.0)*zn*eKT*(V-Vn)));
  double tr = trmax*(pow( (1.0-dr)/dr, dr ) + pow( (1.0-dr)/dr, dr-1.0 ))/(exp(dr*zr*eKT*(V-Vr))+exp((dr-1.0)*zr*eKT*(V-Vr)));

  GNaM2HL = A*GNa*x[1]*x[1]*x[2]*x[3];
  GKN2R = A*GK*x[4]*x[4]*x[5];
  GLNaA = A*GLNa;
  GLKA = A*GLK;
  GLClA = A*GLCl;
  GPA = A*GP;

  INa = GNaM2HL*V*F2RT*(NaO-Na*exp(V*FRT))/(1.0-exp(V*FRT));
  IK = GKN2R*V*F2RT*(KO-K*exp(V*FRT))/(1.0-exp(V*FRT));
  ILNa = GLNaA*V*F2RT*(NaO-Na*exp(V*FRT))/(1.0-exp(V*FRT));
  ILK = GLKA*V*F2RT*(KO-K*exp(V*FRT))/(1.0-exp(V*FRT));
  ILCl = GLClA*V*F2RT*(ClO-Cl*exp(V*FRT))/(1.0-exp(V*FRT));
  IP = 1.0e6*GPA*F/3.0/pow( 1.0+Km/Na, 3.0 );

  /* V */ dxdt[0] = ( - INa - IK - ILNa - ILK - ILCl - IP + 0.001*s )/C/A;
  /* m */ dxdt[1] = ( ms - x[1] ) / tm;
  /* h */ dxdt[2] = ( hs - x[2] ) / th;
  /* l */ dxdt[3] = ( ls - x[3] ) / tl;
  /* n */ dxdt[4] = ( ns - x[4] ) / tn;
  /* r */ dxdt[5] = ( rs - x[5] ) / tr;
  /* Na */ dxdt[6] = - 1.0e-6 * ( INa + ILNa + 3.0*IP ) / F / vol;
}


void Edman::init( double *x ) const
{
  x[0] = -60.86;
  x[1] = 0.002688;
  x[2] = 0.984;
  x[3] = 0.8609;
  x[4] = 0.0412;
  x[5] = 0.8117;
  x[6] = 0.001*10.022;
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
  g[0] = GNaM2HL;
  g[1] = GKN2R;
  g[2] = GLNaA;
  g[3] = GLKA;
  g[4] = GLClA;
  g[5] = GPA;
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


void Edman::add( void )
{
  addLabel( "Conductivities", ModelFlag );
  addNumber( "gna", "Na conductivity", GNa, 0.0, 10000.0, 0.1, "mS/cm^2" ).setFlags( ModelFlag );
  addNumber( "gk", "K conductivity", GK, 0.0, 10000.0, 0.1, "mS/cm^2" ).setFlags( ModelFlag );
  addNumber( "glna", "Na leak conductivity", GLNa, 0.0, 10000.0, 0.1, "mS/cm^2" ).setFlags( ModelFlag );
  addNumber( "glk", "K leak conductivity", GLK, 0.0, 10000.0, 0.1, "mS/cm^2" ).setFlags( ModelFlag );
  addNumber( "glcl", "Cl leak conductivity", GLCl, 0.0, 10000.0, 0.1, "mS/cm^2" ).setFlags( ModelFlag );
  addNumber( "gp", "Pump conductivity", GP, 0.0, 10000.0, 0.1, "mS/cm^2" ).setFlags( ModelFlag );
  addNumber( "c", "Capacitance", C, 0.0, 100.0, 0.1, "muF/cm^2" ).setFlags( ModelFlag );

  SpikingNeuron::add();
}


void Edman::notify( void )
{
  SpikingNeuron::notify();
  GNa = number( "gna" );
  GK = number( "gk" );
  GLNa = number( "glna" );
  GLK = number( "glk" );
  GLCl = number( "glcl" );
  GP = number( "gp" );
  C = number( "c" );
}

