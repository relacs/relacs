/*
  ephys/neuronmodels.h
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

#ifndef _RELACS_EPHYS_NEURONMODELS_H_
#define _RELACS_EPHYS_NEURONMODELS_H_ 1

#include <relacs/model.h>
#include <relacs/ephys/traces.h>
#include <relacs/spikingneuron.h>
using namespace relacs;

namespace ephys {


/*! 
\class NeuronModels
\brief [Model] Various models of conductance-based spiking (point-) neurons.
\author Jan Benda
\author Alexander Wolf
\version 1.2 (Jul 11, 2012)

The Gaussian white noise and the optional additional voltage gated
currents are added to the input after the offset and gain for the
input current has been applied.

\par Options
- Spike generator
- \c spikemodel=Stimulus: Spike model (\c string)
- \c noised=0: Intensity of current noise (\c number)
- \c deltat=0.005ms: Delta t (\c number)
- \c integrator=Euler: Method of integration (\c string)
- Voltage-gated current 1 (activation only)
- \c gmc=0: Conductivity (\c number)
- \c emc=-90mV: Reversal potential (\c number)
- \c mvmc=-40mV: Midpoint potential of activation (\c number)
- \c mwmc=10mV: Width of activation (\c number)
- \c taumc=10ms: Time constant (\c number)
- Voltage-gated current 2  (activation and inactivation)
- \c gmhc=0: Conductivity (\c number)
- \c emhc=-90mV: Reversal potential (\c number)
- \c mvmhc=-40mV: Midpoint potential of activation (\c number)
- \c mwmhc=10mV: Width of activation (\c number)
- \c taummhc=10ms: Time constant of activation (\c number)
- \c pmmhc=1: Power of activation gate (\c number)
- \c hvmhc=-40mV: Midpoint potential of inactivation (\c number)
- \c hwmhc=10mV: Width of inactivation (\c number)
- \c tauhmhc=10ms: Time constant of inactivation (\c number)
- \c pmhhc=1: Power of inactivation gate (\c number)
*/


class NeuronModels : public Model, public ephys::Traces
{

public:

  NeuronModels( void );
  NeuronModels( const string &name,
		const string &pluginset="", 
		const string &author="unknown", 
		const string &version="unknown",
		const string &date=__DATE__ );
  virtual ~NeuronModels( void );

  virtual void main( void );
  virtual void process( const OutData &source, OutData &dest );

  virtual void operator()( double t, double *x, double *dxdt, int n );

  virtual void notifyStimulusData( void );

  virtual Options metaData( void );

    /*! The integration time step. */
  double timeStep( void ) const { return SimDT; };
    /*! Set the integration time step to \a deltat. */
  void setTimeStep( double deltat ) { SimDT = deltat; };
    /*! The intensity of the noise. */
  double noiseD( void ) const { return NoiseD; };
    /*! Set the intensity of the noise to \a noised. */
  void setNoiseD( double noised ) { NoiseD = noised; };
    /*! The prefactor for the Gaussian noise to get the noise intensity noiseD(). */
  double noiseFac( void ) const { return NoiseFac; };
    /* Uses noiseD() and timeStep() for setting noiseFac() correctly to \f$ \sqrt{ 2 D/ \Delta t} \f$. */
  void setNoiseFac( void ) { NoiseFac = ::sqrt( 2.0*NoiseD/SimDT ); };
    /*! The active Spiking Neuron model. */
  SpikingNeuron *neuron( void ) const { return NM; };

    /*! Add a SpikingNeuron \a model */
  void add( SpikingNeuron *model, const string &title="" );
    /*! Add all SpikingNeuron models. */
  void addModels( void );
    /*! Add some general options for integrating SpikingNeuron models
        such as time step, integration method, and the neuron model. */
  void addOptions( void );
    /*! Read the general options for integrating SpikingNeuron models
        such as time step, integration method, neuron model and its options. */
  void readOptions( void );


 protected:
  
  void (*Integrate)( double, double*, double*, int, double, NeuronModels& );

    /*! Add the options of the models as tabs to the dialog \a od.
        To be used in dialogOptions(). */
  void dialogModelOptions( OptDialog *od, string *tabhotkeys );
  virtual OptWidget *dialogOptions( OptDialog *od, string *tabhotkeys=0 );

  double CurrentInput;

  int VCInx;
  double VCGain;
  double VCTau;
  bool VCMode;

  double GMC;
  double EMC;
  double MVMC;
  double MWMC;
  double TAUMC;
  int MMCInx;

  double GMHC;
  double EMHC;
  double MVMHC;
  double MWMHC;
  double TAUMMHC;
  double PMMHC;
  int MMHCInx;
  double HVMHC;
  double HWMHC;
  double TAUHMHC;
  double PHMHC;
  int HMHCInx;

 private:
  
  vector< SpikingNeuron* > Models;
  vector< string > Titles;
  SpikingNeuron *NM;
  double NoiseD;
  double NoiseFac;
  double SimDT;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_NEURONMODELS_H_ */
