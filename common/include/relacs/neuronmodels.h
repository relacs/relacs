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

#ifndef _NEURONMODELS_H_
#define _NEURONMODELS_H_

#include <relacs/model.h>

/*! 
\class NeuronModels
\brief Various models of spiking (point-) neurons.
\author Jan Benda
\author Alexander Wolf
\version 1.1 (Jan 10, 2006)
*/


class SpikingNeuron;

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

 private:
  
  vector< SpikingNeuron* > Models;
  SpikingNeuron *NM;
  double NoiseSD;
  double SimDT;

};


/*! 
\class SpikingNeuron
\brief Base class for a spiking (point-) neuron.
\author Jan Benda
\author Alexander Wolf
\version 1.1 (Jan 10, 2006)
*/

class SpikingNeuron
{

 public:

  SpikingNeuron( void );
  virtual ~SpikingNeuron( void );

    /*! The name of the model. */
  virtual string name( void ) = 0;
    /*! The dimension of the system. */
  virtual int dimension( void ) = 0;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n ) = 0;
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) = 0;
    /*! Add some options to \a o. */
  virtual void add( Options &o ) {};
    /*! Read values of options \a o.
        Prepend \a label to the search strings. */
  virtual void read( const Options &o, const string &label=0 ) {};

    /*! Returns a gain that should be applied to the input. */
  double gain( void ) const;
    /*! Returns an offset that should be applied to the input. */
  double offset( void ) const;
  
 protected:

    /*! The gain that should be applied to the input. */
  double Gain;
    /*! The offset that should be applied to the input. */
  double Offset;

};


class Stimulus : public SpikingNeuron
{
 public:
  Stimulus( void );

    /*! The name of the model. */
  virtual string name( void );
    /*! The dimension of the system. */
  virtual int dimension( void );
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x );
    /*! Add some options to \a o. */
  virtual void add( Options &o );
    /*! Read values op options \a o.
        Prepend \a label to the search strings. */
  virtual void read( const Options &o, const string &label=0 );

};


class FitzhughNagumo : public SpikingNeuron
{
 public:
  FitzhughNagumo( void );

    /*! The name of the model. */
  virtual string name( void );
    /*! The dimension of the system. */
  virtual int dimension( void );
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x );
    /*! Add some options to \a o. */
  virtual void add( Options &o );
    /*! Read values op options \a o.
        Prepend \a label to the search strings. */
  virtual void read( const Options &o, const string &label=0 );

 protected:

  double Phi, A, B, TimeScale, Scale;

};


class MorrisLecar : public SpikingNeuron
{
 public:
  MorrisLecar( void );

    /*! The name of the model. */
  virtual string name( void );
    /*! The dimension of the system. */
  virtual int dimension( void );
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x );
    /*! Add some options to \a o. */
  virtual void add( Options &o );
    /*! Read values op options \a o.
        Prepend \a label to the search strings. */
  virtual void read( const Options &o, const string &label=0 );

 protected:

  double ECa, EK, EL;
  double GCa, GK, GL;
  double MVCa, MKCa, MVK, MKK, MPhiK;
  double C, TimeScale;

};


class HodgkinHuxley : public SpikingNeuron
{
 public:
  HodgkinHuxley( void );

    /*! The name of the model. */
  virtual string name( void );
    /*! The dimension of the system. */
  virtual int dimension( void );
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x );
    /*! Add some options to \a o. */
  virtual void add( Options &o );
    /*! Read values op options \a o.
        Prepend \a label to the search strings. */
  virtual void read( const Options &o, const string &label=0 );

 protected:

  double C, PT, ENa, EK, EL;
  double GNa, GK, GL;

};


class WangAdapt : public SpikingNeuron
{
 public:
  WangAdapt( void );

    /*! The name of the model. */
  virtual string name( void );
    /*! The dimension of the system. */
  virtual int dimension( void );
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x );
    /*! Add some options to \a o. */
  virtual void add( Options &o );
    /*! Read values op options \a o.
        Prepend \a label to the search strings. */
  virtual void read( const Options &o, const string &label=0 );

 protected:

  double C, PT, ENa, EK, EL, EA;
  double GNa, GK, GL, GA;
  double Atau;

};


#endif
