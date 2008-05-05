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
using namespace relacs;


SpikingNeuron::SpikingNeuron( void )
  : Gain( 1.0 ),
    Offset( 0.0 )
{
}


SpikingNeuron::~SpikingNeuron( void )
{
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
  Gain = 1.0;
  Offset = 0.0;
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


void Stimulus::add( Options &o ) const
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
  Gain = 0.02;
  Offset = -5.0;
  Scale = 10.0;
}


string FitzhughNagumo::name( void ) const
{
  return "Fitzhugh-Nagumo";
}


int FitzhughNagumo::dimension( void ) const
{
  return 3;
}


void FitzhughNagumo::variables( vector< string > &varnames ) const
{
  varnames.clear();
  varnames.reserve( 3 );
  varnames.push_back( "Out" );
  varnames.push_back( "V" );
  varnames.push_back( "W" );
}


void FitzhughNagumo::operator()(  double t, double s, double *x, double *dxdt, int n )
{
  x[0] = Scale*x[1];
  /* out */ dxdt[0] = 0.0;
  /* V */ dxdt[1] = TimeScale*(x[1]-x[1]*x[1]*x[1]/3.0-x[2]+s);
  /* W */ dxdt[2] = TimeScale*Phi*(x[1]+A-B*x[2]);
}


void FitzhughNagumo::init( double *x ) const
{
  x[1] = -1.2;
  x[2] = -0.62;
  x[0] = Scale*x[1];
}


void FitzhughNagumo::add( Options &o ) const
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
  ECa = +120.0;
  GCa = 4.0;
  GCaM = GCa;
  ICa = 0.0;
  MVCa = -1.2;
  MKCa = 18.0;
  EK = -80.0;
  GK = 8.0;
  GKW = GK;
  IK = 0.0;
  MVK = 12.0;
  MKK = 17.4;
  MPhiK = 0.067;
  EL = -60.0;
  GL = 2.0;
  IL = 0.0;
  C = 20.0;
  TimeScale = 10.0;
  Gain = 1.0;
  Offset = 40.0;
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


void MorrisLecar::add( Options &o ) const
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
  ENa = +50.0;
  GNa = 120.0;
  GNaM3H = GNa;
  INa= 0.0;
  EK = -77.0;
  GK = 36.0;
  GKN4 = GK;
  IK= 0.0;
  EL = -54.384;
  GL = 0.3;
  IL= 0.0;
  C = 1.0;
  PT = 1.0;
  Gain = 1.0;
  Offset = 0.0;
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


void HodgkinHuxley::add( Options &o ) const
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


TraubHH::TraubHH( void )
  : HodgkinHuxley()
{
  ENa = +48.0;
  GNa = 100.0;
  EK = -82.0;
  GK = 200.0;
  EL = -67.0;
  GL = 0.1;
  C = 1.0;
  PT = 1.0;
  Gain = 1.0;
  Offset = 0.0;

  GNaM3H = GNa;
  INa = 0.0;
  GKN4 = GK;
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
  x[0] = -66.61;
  x[1] = 0.015995;
  x[2] = 0.995513;
  x[3] = 0.040180;
}


Traub::Traub( void )
  : HodgkinHuxley()
{
  ENa = +48.0;
  GNa = 100.0;
  EK = -82.0;
  GK = 200.0;
  EL = -67.0;
  GL = 0.1;
  ECa = +73.0;
  GCa = 119.9;
  EAHP = -82.0;
  GAHP = 3.01;
  C = 1.0;
  PT = 1.0;
  Gain = 1.0;
  Offset = 0.0;

  GNaM3H = GNa;
  INa = 0.0;
  GKN4 = GK;
  IK = 0.0;
  IL = 0.0;
  GCaS5R = GCa;
  ICa = 0.0;
  GAHPQ = GAHP;
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


void Traub::add( Options &o ) const
{
  HodgkinHuxley::add( o );

  o.insertLabel( "Calcium current", "Input" );
  o.insertNumber( "gca", "Input", "Ca conductivity", GCa, 0.0, 10000.0, 0.1, "mS/cm^2" );
  o.insertNumber( "eca", "Input", "Ca reversal potential", ECa, -200.0, 200.0, 1.0, "mV" );

  o.insertLabel( "AHP-type current", "Input" );
  o.insertNumber( "gahp", "Input", "AHP conductivity", GAHP, 0.0, 10000.0, 0.1, "mS/cm^2" );
  o.insertNumber( "eahp", "Input", "AHP reversal potential", EAHP, -200.0, 200.0, 1.0, "mV" );
}


void Traub::read( const Options &o, const string &label )
{
  HodgkinHuxley::read( o, label );

  ECa = o.number( label + "eca" );
  GCa = o.number( label + "gca" );
  EAHP = o.number( label + "eahp" );
  GAHP = o.number( label + "gahp" );
}


TraubErmentrout::TraubErmentrout( void )
  : HodgkinHuxley()
{
  ENa = +50.0;
  GNa = 100.0;
  EK = -100.0;
  GK = 80.0;
  EL = -67.0;
  GL = 0.1;
  ECa = +120.0;
  GCa = 5.0;
  EM = -100.0;
  GM = 8.0;
  TauW = 100.0;
  EAHP = -100.0;
  GAHP = 4.0;
  C = 1.0;
  PT = 1.0;
  Gain = 1.0;
  Offset = 0.0;

  GNaM3H = GNa;
  INa = 0.0;
  GKN4 = GK;
  IK = 0.0;
  IL = 0.0;
  GCaS = GCa;
  ICa = 0.0;
  GMW = GM;
  IM = 0.0;
  GAHPQ = GAHP;
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


void TraubErmentrout::add( Options &o ) const
{
  HodgkinHuxley::add( o );

  o.insertLabel( "Calcium current", "Input" );
  o.insertNumber( "gca", "Input", "Ca conductivity", GCa, 0.0, 10000.0, 0.1, "mS/cm^2" );
  o.insertNumber( "eca", "Input", "Ca reversal potential", ECa, -200.0, 200.0, 1.0, "mV" );

  o.insertLabel( "M-type current", "Input" );
  o.insertNumber( "gm", "Input", "M conductivity", GM, 0.0, 10000.0, 0.1, "mS/cm^2" );
  o.insertNumber( "em", "Input", "M reversal potential", EM, -200.0, 200.0, 1.0, "mV" );
  o.insertNumber( "tauw", "Input", "W time constant", TauW, 0.0, 1000.0, 1.0, "ms" );

  o.insertLabel( "AHP-type current", "Input" );
  o.insertNumber( "gahp", "Input", "AHP conductivity", GAHP, 0.0, 10000.0, 0.1, "mS/cm^2" );
  o.insertNumber( "eahp", "Input", "AHP reversal potential", EAHP, -200.0, 200.0, 1.0, "mV" );
}


void TraubErmentrout::read( const Options &o, const string &label )
{
  HodgkinHuxley::read( o, label );

  ECa = o.number( label + "eca" );
  GCa = o.number( label + "gca" );
  EM = o.number( label + "em" );
  GM = o.number( label + "gm" );
  TauW = o.number( label + "tauw" );
  EAHP = o.number( label + "eahp" );
  GAHP = o.number( label + "gahp" );
}


Wang::Wang( void )
  : HodgkinHuxley()
{
  ENa = +55.0;
  GNa = 35.0;
  GNaM3H = GNa;
  INa = 0.0;
  EK = -90.0;
  GK = 9.0;
  GKN4 = GK;
  IK = 0.0;
  EL = -65.0;
  GL = 0.1;
  IL = 0.0;
  C = 1.0;
  PT = 5.0;
  Gain = 0.3;
  Offset = 0.0;
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
  GAA = GA;
  IA = 0.0;
  Atau = 100.0;
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


void WangAdapt::add( Options &o ) const
{
  Wang::add( o );
  o.insertLabel( "Adaptation current", "Input" );
  o.insertNumber( "ga", "Input", "A conductivity", GA, 0.0, 10000.0, 0.1, "mS/cm^2" );
  o.insertNumber( "ea", "Input", "A reversal potential", EA, -200.0, 200.0, 1.0, "mV" );
  o.insertNumber( "atau", "Input", "A time constant", Atau, 0.0, 1000.0, 1.0, "ms" );
}


void WangAdapt::read( const Options &o, const string &label )
{
  Wang::read( o, label );
  EA = o.number( label + "ea" );
  GA = o.number( label + "ga" );
  Atau = o.number( label + "atau" );
}
