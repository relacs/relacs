/*
  neuronmodels.h
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

#ifndef _RELACS_COMMON_NEURONMODELS_H_
#define _RELACS_COMMON_NEURONMODELS_H_ 1

#include <relacs/common/spikingneuron.h>
#include <relacs/model.h>
using namespace relacs;


/*! 
\class NeuronModels
\brief Various models of spiking (point-) neurons.
\author Jan Benda
\author Alexander Wolf
\version 1.1 (Jan 10, 2006)
*/


class NeuronModels : public Model
{
  Q_OBJECT

public:

  NeuronModels( void );
  NeuronModels( const string &name,
		const string &title="", 
		const string &pluginset="", 
		const string &author="unknown", 
		const string &version="unknown",
		const string &date=__DATE__ );
  virtual ~NeuronModels( void );

  virtual void main( void );
  virtual void process( const OutData &source, OutData &dest );

  virtual void operator()( double t, double *x, double *dxdt, int n );

    /*! The integration time step. */
  double timeStep( void ) const { return SimDT; };
    /*! Set the integration time step to \a deltat. */
  void setTimeStep( double deltat ) { SimDT = deltat; };
    /*! The standard deviation of the noise. */
  double noiseSD( void ) const { return NoiseSD; };
    /*! The active Spiking Neuron model. */
  SpikingNeuron *neuron( void ) const { return NM; };

    /*! Add a SpikingNeuron \a model */
  void add( SpikingNeuron *model );
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
  void dialogModelOptions( OptDialog *od );
  virtual void dialogOptions( OptDialog *od );

 private:
  
  vector< SpikingNeuron* > Models;
  SpikingNeuron *NM;
  double NoiseSD;
  double SimDT;

};


#endif /* ! _RELACS_COMMON_NEURONMODELS_H_ */
