/*
  auditory/receptormodel.h
  Spiking neuron model stimulated through an auditory transduction chain.

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

#ifndef _RELACS_AUDITORY_RECEPTORMODEL_H_
#define _RELACS_AUDITORY_RECEPTORMODEL_H_ 1

#include <relacs/ephys/neuronmodels.h>
using namespace relacs;

namespace auditory {


/*! 
\class ReceptorModel
\brief [Model] Spiking neuron model stimulated through an auditory transduction chain.
\author Alexander Wolf
\author Jan Benda
\version 1.1 (Jan 15, 2006)

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

The following options are supported (brief description of each
option with default values and data type): 
- \c tymp=Scaling: Select model for tympanum (1.) (\c integer)
- \c freq=5.0kHz: Eigenfrequency of the tympanal membrane (\c number)
- \c tdec=0.154ms: Damping time constant of tympanal membrane (\c number)
- \c nl=Square saturated: Select model for mechano-electrical transduction (2.) (\c integer)
- \c imax=60.0: Saturation = maximum current (\c number)
- \c imin=0.0: Minimum = Zeron point current(\c number)
- \c cut=0.02mPa: Amplitude where \c imax is reached (\c number)
- \c slope=3000mPa^-1: Slope of linear Boltzman (\c number)
- \c matchslope=1: Match slope of Boltzman to that of Square (\c boolean)
- \c slope2=600000: Slope of square Boltzman (\c number)
- \c matchslope2=1: Match slope of square Boltzman to that of Square (\c boolean)
- \c spikemodel=: Select spikemodel (3.) (\c integer)
- \c noise=0.0: Standard deviation of current noise (\c number)
- \c deltat: Integration time constant (step size) (\c number)
- \c integrator=Euler: Method of integration (4.) (\c integer)
-  Many more options for spike generators.
.
*/


class ReceptorModel : public ephys::NeuronModels
{
  Q_OBJECT

public:

  ReceptorModel( void );
  ~ReceptorModel( void );

  virtual void main( void );
  virtual void process( const OutData &source, OutData &dest );

  virtual void operator()( double t, double *x, double *dxdt, int n ) const;


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

  virtual void dialogOptions( OptDialog *od );

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
