/*
  auditory/receptormodel.h
  Spiking neuron model stimulated through an auditory transduction chain.

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

#ifndef _RELACS_AUDITORY_RECEPTORMODEL_H_
#define _RELACS_AUDITORY_RECEPTORMODEL_H_ 1

#include <relacs/ephys/neuronmodels.h>
#include <relacs/acoustic/traces.h>
using namespace relacs;

namespace auditory {


/*! 
\class ReceptorModel
\brief [Model] Spiking neuron model stimulated through an %auditory transduction chain.
\author Alexander Wolf
\author Jan Benda

%ReceptorModel simulates the auditory transduction chain of Locusta
migratoria. It is designed to work with RELACS in the dry mode (relacs
-3) so new repros can be tested for different settings. The plugin
is based on a biophysical model that describes the transformation
of the sound stimulus in a chain of three distinct steps: 1. the
tympanum; 2.  mechano-electrical transduction; 3. spike generator.

\image html transductionchain.jpg

For each of the three steps, the plugin provides several options
for the user to adapt to his/her needs and the hardware
restrictions. Additionally, the user can chose between between
several integration algorithms. The following options can be
selected (in order of increasing complexity):


1. Tympanum  
- \c None: No modification of the stimulus
- \c Scaling: Adjusts the stimulus strength according to tuning curve
- \c Oscillator: Model as damped harmonic oscillator
.
2. Mechano-Electrical Transduction:  
- \c None: No modification of the tympanum output
- \c Linear: Absolut value of the tympanum output
- \c Linear \c saturated: Absolut value up to saturation point
- \c Box: Two Heavyside functions
- \c Square: Square of the tympanum output
- \c Square \c saturated: Square value up to saturation point
- \c Linear \c Boltzman: Superposition of two Boltzman functions with linear dependency in exponent
- \c Square \c Boltzman: One Boltzman function with quadratic dependency in exponent
.
3. Spike Generators: 
- \c Stimulus: No modification of previous function
- \c FitzhughNagumo: Simplified abstract neuron model with 2 differential equations.
- \c MorrisLecar: Simplified neuron model based on ion channels with 2 differential equations.
- \c WangAdapt: Neuron Model with adaptation currents with 5 differential equations.
.
4. Integrators: 
- \c Euler
- \c Midpoint
- \c Runge-Kutta \c 4th \c order
.

\par Options
The following options are supported (brief description of each
option with default values and data type): 
- Transduction chain
- \c tymp=Scaling: Tympanum model (\c string)
- \c freq=5kHz: Eigenfrequency (\c number)
- \c tdec=0.154ms: Decay constant (\c number)
- \c nl=Square saturated: Static nonlinearity (\c string)
- Spike generator
- \c spikemodel=Stimulus: Spike model (\c string)
- \c noise=0: Standard deviation of current noise (\c number)
- \c deltat=0.005ms: Delta t (\c number)
- \c integrator=Euler: Method of integration (\c string)
- Square = ax^2+imin, a=(imax-imin)/cut^2
- Square saturated = imax, for |x|>=cut
- Linear = b|x|+imin, b=(imax-imin)/cut
- Linear saturated = imax, for |x|>=cut
- Box = imin, for |x|<cut, = imax else
- None = ax, a=(imax-imin)/cut
- \c imax=60muA/cm^2: Maximum current (\c number)
- \c imin=0muA/cm^2: Minimum current (zero point current) (\c number)
- \c cut=0.02mPa: Amplitude of tympanum where imax is reached (\c number)
- Boltzmann, (imax/(1-f_0))*(1/(1+exp[-slope*(x-x0)])+1/(1+exp[slope*(x+x0)])-f_0)+imin
- \c slope=3000mPa^-1: Slope of Boltzmann (\c number)
- \c matchslope=true: Set slope of Boltzmann to match square (\c boolean)
- \c x0=0.01mPa: 1/2 of Imax-Imin is reached (\c number)
- Boltzmann, 2(imax-imin)(1/(1+exp[-slope2*x^2])-1/2)+imin
- \c slope2=6e+05mPa^-2: Slope of square Boltzmann (\c number)
- \c matchslope2=true: Set slope of square Boltzmann to match square (\c boolean)
-  Many more options for spike generators.
.

\version 1.1 (Jan 15, 2006)
*/


class ReceptorModel : public ephys::NeuronModels, public acoustic::Traces
{

public:

  ReceptorModel( void );
  ~ReceptorModel( void );

  virtual void main( void );
  virtual void process( const OutData &source, OutData &dest );

  virtual void operator()( double t, double *x, double *dxdt, int n );


 protected:

  double identity( double x ) const;
  double box( double x ) const;
  double linear( double x ) const;
  double linearSaturated( double x ) const;
  double square( double x ) const;
  double squareSaturated( double x ) const;
  double linearBoltzman( double x ) const;
  double squareBoltzman( double x ) const;
  double (ReceptorModel::*Nonlinearity)( double ) const;

  virtual OptWidget *dialogOptions( OptDialog *od, string *tabhotkeys=0 );

  // tympanum:
  int TympanumModel;
  double Omega;
  double TDec;
  double Alpha;
  double Beta;
  // nonlinearities:
  double Imax;
  double Imin;
  double CutPoint;
  double DI; // Imax-Imin
  double DIC; // (Imax-Imin)/CutPoint
  double DICC; // (Imax-Imin)/(CutPoint*CutPoint)
  double Slope;
  double X0;
  double F0;
  double Slope2;

};


}; /* namespace auditory */

#endif /* ! _RELACS_AUDITORY_RECEPTORMODEL_H_ */
