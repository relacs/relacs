/*
  ephys/spikingneuron.h
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

#ifndef _RELACS_EPHYS_SPIKINGNEURON_H_
#define _RELACS_EPHYS_SPIKINGNEURON_H_ 1

#include <string>
#include <vector>
#include <relacs/configclass.h>
using namespace std;
using namespace relacs;

namespace ephys {


/*! 
\class SpikingNeuron
\brief [lib] Base class for a spiking (point-) neuron
\author Jan Benda
\author Alexander Wolf
\version 1.2 (May 7, 2008)
-# SpikingNeuron inherits Options
-# Added interface for accessing the values 
   of the iionic currents and conductances
\version 1.1 (Jan 10, 2006)

Each model of a spiking neuron has a name().
The model is implemented as a set of differential equations
\f[ \left( \begin{array}{c} \frac{dx_1}{dt} \\ \frac{dx_2}{dt} \\
                            \vdots \\ \frac{dx_n}{dt} \end{array} \right) = 
    \left( \begin{array}{c} f_1(x_1, x_2, \ldots, x_n, s) \\ f_2(x_1, x_2, \ldots, x_n, s) \\
                            \vdots \\ f_n(x_1, x_2, \ldots, x_n, s) \end{array} \right) \f]
The state of the model is described by the state vector \f$ \vec x = (x_1, x_2, \ldots, x_n)\f$
of dimension \a n = dimension().
operator() computes the derivatives \f$ d\vec x/dt\f$ with respect to time \a t
for the current state \a x and the stimulus \a s.
The variables() function returns names for each of the state variables \a x,
units() returns the corrsponding units,
and init() sets the state variables \a x to useful initial conditions.
The unit of the input is given by inputUnit().

While integrating the model, the current values and the names
of the ionic currents and their corresponding conductances can be 
retrieved by the currents() and conductances() functions.
The units of the currents and conductances are returned
by conductanceUnits() and currentUnit().

Parameter values of the model can be made accessible by adding them to
the Options in add().
Changed parameter values are read out from the Options by notify().
The parameter values are classified as either scalingFlag(), modelFlag(), 
or descriptionFlag().

SpikingNeuron defines two parameters offset() and gain() with default values
0 and 1, respectively, that should be applied to whatever input before
it is passed on as the stimulus \a s for computing the derivatives
via operator().
*/

class SpikingNeuron : public ConfigClass
{

 public:

    /*! Constructs the model, i.e. initializes model parameter with
        useful default values.
        \note The constructor should not add any new options.
	This goes into the add() function. */
  SpikingNeuron( void );
  virtual ~SpikingNeuron( void );

    /*! \return the name of the model. */
  virtual string name( void ) const;
    /*! \return the dimension of the system. \sa variables, operator() */
  virtual int dimension( void ) const = 0;
    /*! \param[out] varnames the names of each of the dimension() variables.
        Gating variables (ranging between 0 and 1) should be a 
        single lower-case character, potentials and equivalent potentials
	a single upper-case character, and concentrations should be in 
	brackets (e.g. [Ca]).
        \sa dimension(), units(), operator() */
  virtual void variables( vector< string > &varnames ) const = 0;
    /*! \param[out] u the units of the variables returned by variables().
        \sa dimension(), operator() */
  virtual void units( vector< string > &u ) const = 0;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x.
	Implement this function with your model.
        \param[in] t the time.
        \param[in] s the stimulus.
        \param[in,out] x the state vector.
        \param[out] dxdt the derivative with respect to time.
        \param[in] n the number of variables, usually equalt to dimension().
        \sa dimension(), init(), variables(), inputUnit() */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n ) = 0;
    /*! Initialize the state \a x with useful inital conditions.
        \param[out] x the dimension() state variables of the model.
        \sa dimension(), operator() */
  virtual void init( double *x ) const = 0;
    /*! Implement this function to return the names of the individual
        ionic conductances that conductances(double*) would return.
        The default implementation returns an empty vector.
        \param[out] conductancenames the names of the individual 
	ionic conductances.
        \sa currents() */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Implement this function to return in \a g the values of the individual
        ionic conductances. The number of conductances is defined by the size
	of \a conductancenames the function conductances(vector<string>&)
	returns. The default implementation returns nothing.
        \param[out] g the current values of the individual ionic conductances.
        \sa currents() */
  virtual void conductances( double *g ) const;
    /*! Returns the unit of the conductances returned by conductances().
        The default implementation returns \c mS/cm^2. */
  virtual string conductanceUnit( void ) const;
    /*! Implement this function to return in \a currentnames the names of the
        individual ionic currents that currents(double*) would return.
        The default implementation returns an empty vector.
	\param[out] currentnames the names of the individual ionic currents.
        \sa conductances */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Implement this function to return in \a c the values of the individual
        ionic currents. The number of currents is defined by the size of
	\a currentnames the function currents(vector<string>&) returns.
        The default implementation returns nothing.
        \param[out] c the values of the individual ionic currents.
        \sa conductances */
  virtual void currents( double *c ) const;
    /*! Returns the unit of the currents returned by currents().
        The default implementation returns \c uA/cm^2. */
  virtual string currentUnit( void ) const;
    /*! Returns the unit of the input (the stimulus).
        The default implementation returns \c uA/cm^2.
        \sa operator() */
  virtual string inputUnit( void ) const;

    /*! Implement this function to add all necessary options.
        Use the flags of each option to either one of
	ScalingFlag, ModelFlag, or DescriptionFlag.
        For example:
        \code
	addLabel( "Sodium current", ModelFlag );
	addNumber( "gna", "Na conductivity", GNa, 0.0, 10000.0, 0.1, "mS/cm^2" ).setFlags( ModelFlag );
	addNumber( "ena", "Na reversal potential", ENa, -200.0, 200.0, 1.0, "mV" ).setFlags( ModelFlag );
        \endcode
        This implementation adds a label "Input" followed by two numbers
        "Gain" and "Offset".
        Call it in your reimplementation via
        \code
	SpikingNeuron::add();
        \endcode
        \sa notify() */
  virtual void add( void );
    /*! Implement this function to read out the current values
        from the list of Options.
        For example:
        \code
	ENa = number( "ena" );
	GNa = number( "gna" );
	\endcode
        This implementation reads out the gain() and the offset().
        Call it in your reimplementation via
        \code
	SpikingNeuron::notify();
        \endcode
        \sa add() */
  virtual void notify( void );

    /*! \return a gain that should be applied to the input.
        \sa offset() */
  double gain( void ) const;
    /*! \return an offset that should be applied to the input.
        \sa gain() */
  double offset( void ) const;

    /*! Flag for selecting input / output gain and offset options. */
  static const int ScalingFlag = 16;
    /*! Flag for selecting the model options. */
  static const int ModelFlag = 32;
    /*! Flag for selecting the model description. */
  static const int DescriptionFlag = 64;

  
 protected:

    /*! The gain that should be applied to the input. */
  double Gain;
    /*! The offset that should be applied to the input. */
  double Offset;

};


/*! 
\class Stimulus
\brief [lib] Implementation of SpikingNeuron that just returns the stimulus
\author Jan Benda
*/

class Stimulus : public SpikingNeuron
{
 public:
  Stimulus( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! \copydoc SpikingNeuron::dimension()  */
  virtual int dimension( void ) const;
    /*! \copydoc SpikingNeuron::variables() */
  virtual void variables( vector< string > &varnames ) const;
    /*! \copydoc SpikingNeuron::units() */
  virtual void units( vector< string > &u ) const;
    /*! Simply sets the first variable to the stimulus \a s
        \code
	x[0] = s;
	dxdt[0] = 0.0;
	\endcode */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the first variable with zero. */
  virtual void init( double *x ) const;

};


/*! 
\class FitzhughNagumo
\brief [lib] The Fitzhugh-Nagumo model
\author Jan Benda
\todo verify reference, parameter values, and units

From Koch, Biophysics of Computation, Chap.7.1
*/

class FitzhughNagumo : public SpikingNeuron
{
 public:
  FitzhughNagumo( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! \copydoc SpikingNeuron::dimension()  */
  virtual int dimension( void ) const;
    /*! \copydoc SpikingNeuron::variables() */
  virtual void variables( vector< string > &varnames ) const;
    /*! \copydoc SpikingNeuron::units() */
  virtual void units( vector< string > &u ) const;
    /*! Implementation of the Fitzhugh-Nagumo equations:
        \f{eqnarray*}
           dV/dt & = & (V-V^3/3-W+s)/{\rm TimeScale} \\
           dW/dt & = & {\rm Phi}(V+A-BW)/{\rm TimeScale}
	\f} 
        x is a scaled version of the \a V variable. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;

    /*! Add parameters as options. */
  virtual void add( void );
    /*! Read out the current values from the list of Options. */
  virtual void notify( void );

 protected:

  double Phi, A, B, TimeScale;

};


/*! 
\class MorrisLecar
\brief [lib] The Morris-Lecar model
\author Jan Benda
\todo verify reference, parameter values, and units
*/

class MorrisLecar : public SpikingNeuron
{
 public:
  MorrisLecar( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! \copydoc SpikingNeuron::dimension()  */
  virtual int dimension( void ) const;
    /*! \copydoc SpikingNeuron::variables() */
  virtual void variables( vector< string > &varnames ) const;
    /*! \copydoc SpikingNeuron::units() */
  virtual void units( vector< string > &u ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;

    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns. */
  virtual void conductances( double *g ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns. */
  virtual void currents( double *c ) const;

    /*! Add parameters as options. */
  virtual void add( void );
    /*! Read out the current values from the list of Options. */
  virtual void notify( void );

 protected:

  double ECa, EK, EL;
  double GCa, GK, GL;
  double GCaGates, GKGates;
  double ICa, IK, IL;
  double MVCa, MKCa, MVK, MKK, MPhiK;
  double C, TimeScale;

};


/*! 
\class HodgkinHuxley
\brief [lib] The Hodgkin-Huxley (1952) model
\author Jan Benda
\todo test it!

This is an implementation of the famous Hodgkin-Huxley model for the
membrane potential of the squid giant axon (A. L. Hodgkin and
A. F. Huxley (1952): A quantitative description of membrane current
and its application to conduction and excitation in nerve. J. Physiol. 117,
pp. 500-544).
In contrast to the original paper all potentials are shifted such that the
resting potential is at -65 mV.
*/

class HodgkinHuxley : public SpikingNeuron
{
 public:
  HodgkinHuxley( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! \copydoc SpikingNeuron::dimension()  */
  virtual int dimension( void ) const;
    /*! \copydoc SpikingNeuron::variables() */
  virtual void variables( vector< string > &varnames ) const;
    /*! \copydoc SpikingNeuron::units() */
  virtual void units( vector< string > &u ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;

    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns. */
  virtual void conductances( double *g ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns. */
  virtual void currents( double *c ) const;

    /*! Add parameters as options. */
  virtual void add( void );
    /*! Read out the current values from the list of Options. */
  virtual void notify( void );

 protected:

  double C, PT, ENa, EK, EL;
  double GNa, GK, GL;
  double GNaGates, GKGates;
  double INa, IK, IL;

};


/*! 
\class Connor
\brief [lib] The %Connor model with A current.
\author Jan Benda
\todo test it!

This is the implementation of the %Connor model (John A. %Connor and
David Walter and Russell McKown (1977): Neural Repetitive Firing.
Biophys. J. 18, pp. 81-102), which is a slightly-modified
Hodgkin-Huxley model with an additional A current.
*/

class Connor : public HodgkinHuxley
{
 public:
  Connor( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! \copydoc SpikingNeuron::dimension()  */
  virtual int dimension( void ) const;
    /*! \copydoc SpikingNeuron::variables() */
  virtual void variables( vector< string > &varnames ) const;
    /*! \copydoc SpikingNeuron::units() */
  virtual void units( vector< string > &u ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;

    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns. */
  virtual void conductances( double *g ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns. */
  virtual void currents( double *c ) const;

    /*! Add parameters as options. */
  virtual void add( void );
    /*! Read out the current values from the list of Options. */
  virtual void notify( void );

 protected:

  double EKA;
  double GKA;
  double GKAGates;
  double IKA;

};


/*! 
\class RushRinzel
\brief [lib] The Rush-Rinzel model with A current
\author Jan Benda
\todo test it!

This is the implementation of the Rush-Rinzel model (Maureen E. Rush
and John Rinzel (1995): The potassium A-current, low firing rates and
rebound excitation in Hodgkin-Huxley models. Bulletin of Mathematical
Biology 57(6), pp. 899-929), a modified Hodgkin-Huxley model with an
additional A current.
*/

class RushRinzel : public Connor
{
 public:
  RushRinzel( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! \copydoc SpikingNeuron::dimension()  */
  virtual int dimension( void ) const;
    /*! \copydoc SpikingNeuron::variables() */
  virtual void variables( vector< string > &varnames ) const;
    /*! \copydoc SpikingNeuron::units() */
  virtual void units( vector< string > &u ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;

    /*! Add parameters as options. */
  virtual void add( void );
    /*! Read out the current values from the list of Options. */
  virtual void notify( void );

 protected:

  double AV0, ADV, BV0, BDV, BTau;

};


/*! 
\class Awiszus
\brief [lib] The %Awiszus model with A current
\author Jan Benda
\todo verify reference, parameter values, and units

(Friedemann %Awiszus (1988): The adaptation ability of neuronal models
subject to a current step stimulus. Biol. Cybern. 59, pp. 295-302)
*/

class Awiszus : public Connor
{
 public:
  Awiszus( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;
};


/*! 
\class FleidervishSI
\brief [lib] The Fleidervish model with slowly inactivating sodium current
\author Jan Benda
\todo verify reference, parameter values, and units

(Ilya A. Fleidervish and Alon Friedman and Michael J. Gutnick (1996):
Slow inactivation of Na+ current and slow cumulative spike adaptation
in mouse and guinea-pig neocortical neurones in
slices. J. Physiol. 493, pp. 83-97)
*/

class FleidervishSI : public HodgkinHuxley
{
 public:
  FleidervishSI( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! \copydoc SpikingNeuron::dimension()  */
  virtual int dimension( void ) const;
    /*! \copydoc SpikingNeuron::variables() */
  virtual void variables( vector< string > &varnames ) const;
    /*! \copydoc SpikingNeuron::units() */
  virtual void units( vector< string > &u ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;
};


/*! 
\class TraubHH
\brief [lib] The Traub-Miles (1991) model with the HH currents I_Na, I_K, and I_l only.
\author Jan Benda
\todo verify reference, parameter values, and units
\note Not working!

Conductances are from %Traub, scaled to Ermentrouts Na.
Potentials are from %Traub & Miles (1991)
*/

class TraubHH : public HodgkinHuxley
{
 public:
  TraubHH( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;

};


/*! 
\class TraubMiles
\brief [lib] The soma compartment of the Traub-Miles (1991) model
\author Jan Benda
\todo unit of calcium concentration
\todo verify reference, parameter values, and units

This is an implementation of the soma compartment only of the
Traub-Miles model (Roger D. Traub and Robert K. S. Wong and Richard
Miles and Hillary Michelson (1991): A model of a CA3 hippocampal
pyramidal neuron incorporating voltage-clamp data on intrinsic
conductances. J. Neurophysiol. 66, pp. 635-650).

Conductances are from %Traub, scaled to Ermentrouts Na.
Potentials are from %Traub & Miles (1991)
*/

class TraubMiles : public HodgkinHuxley
{
 public:
  TraubMiles( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! \copydoc SpikingNeuron::dimension()  */
  virtual int dimension( void ) const;
    /*! \copydoc SpikingNeuron::variables() */
  virtual void variables( vector< string > &varnames ) const;
    /*! \copydoc SpikingNeuron::units() */
  virtual void units( vector< string > &u ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;

    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns. */
  virtual void conductances( double *g ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns. */
  virtual void currents( double *c ) const;

    /*! Add parameters as options. */
  virtual void add( void );
    /*! Read out the current values from the list of Options. */
  virtual void notify( void );

 protected:

  double GCa, GAHP;
  double GCaGates, GAHPGates;
  double ECa, EAHP;
  double ICa, IAHP;

};


/*! 
\class TraubErmentrout
\brief [lib] Modification of the soma compartment Traub-Miles (1991) model
\author Jan Benda
\todo unit of calcium concentration
\todo verify reference, parameter values, and units

This is a modified version (Bard Ermentrout (1998): Linearization of
f-I curves by adaptation. Neural. Comput. 10, pp. 1721-1729) of the
soma compartment of the Traub-Miles model (Roger D. Traub and Robert
K. S. Wong and Richard Miles and Hillary Michelson (1991): A model of
a CA3 hippocampal pyramidal neuron incorporating voltage-clamp data on
intrinsic conductances. J. Neurophysiol. 66, pp. 635-650).
*/

class TraubErmentrout : public HodgkinHuxley
{
 public:
  TraubErmentrout( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! \copydoc SpikingNeuron::dimension()  */
  virtual int dimension( void ) const;
    /*! \copydoc SpikingNeuron::variables() */
  virtual void variables( vector< string > &varnames ) const;
    /*! \copydoc SpikingNeuron::units() */
  virtual void units( vector< string > &u ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;

    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns. */
  virtual void conductances( double *g ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns. */
  virtual void currents( double *c ) const;

    /*! Add parameters as options. */
  virtual void add( void );
    /*! Read out the current values from the list of Options. */
  virtual void notify( void );

 protected:

  double GCa, GM, GAHP;
  double GCaGates, GMGates, GAHPGates;
  double ECa, EM, EAHP;
  double ICa, IM, IAHP;
  double TauW;

};


/*! 
\class WangBuzsaki
\brief [lib] Wang-Buzsaki (1996) model
\author Jan Benda
\todo verify reference, parameter values, and units

(Xiao-Jing Wang and Gy&ouml;rgy Buzs&aacute;ki (1996): Gamma
oscillation by synaptic inhibition in a hippocampal interneuronal
network model. J. Neurosci. 16, pp. 6402-6413)
*/

class WangBuzsaki : public HodgkinHuxley
{
 public:
  WangBuzsaki( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! \copydoc SpikingNeuron::dimension()  */
  virtual int dimension( void ) const;
    /*! \copydoc SpikingNeuron::variables() */
  virtual void variables( vector< string > &varnames ) const;
    /*! \copydoc SpikingNeuron::units() */
  virtual void units( vector< string > &u ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;

};


/*! 
\class WangBuzsakiAdapt
\brief [lib] The Wang-Buzsaki model with an additional adaptation current.
\author Jan Benda
\todo verify reference, parameter values, and units
*/

class WangBuzsakiAdapt : public WangBuzsaki
{
 public:
  WangBuzsakiAdapt( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! \copydoc SpikingNeuron::dimension()  */
  virtual int dimension( void ) const;
    /*! \copydoc SpikingNeuron::variables() */
  virtual void variables( vector< string > &varnames ) const;
    /*! \copydoc SpikingNeuron::units() */
  virtual void units( vector< string > &u ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;

    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns. */
  virtual void conductances( double *g ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns. */
  virtual void currents( double *c ) const;

    /*! Add parameters as options. */
  virtual void add( void );
    /*! Read out the current values from the list of Options. */
  virtual void notify( void );

 protected:

  double EA;
  double GA;
  double GAGates;
  double Atau;
  double IA;

};


/*! 
\class Crook
\brief [lib] The two-compartment %Crook model with adaptation currents.
\author Jan Benda
\todo unit of calcium concentration
\todo verify reference, parameter values, and units

(Sharon M. Crook and G. Bard Ermentrout and James M. Bower (1998):
Spike frequency adaptation affects the synchronization properties of
networks of cortical oscillators. Neural. Comput. 10, pp. 837-854)
*/

class Crook : public HodgkinHuxley
{
 public:
  Crook( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! \copydoc SpikingNeuron::dimension()  */
  virtual int dimension( void ) const;
    /*! \copydoc SpikingNeuron::variables() */
  virtual void variables( vector< string > &varnames ) const;
    /*! \copydoc SpikingNeuron::units() */
  virtual void units( vector< string > &u ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;

    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns. */
  virtual void conductances( double *g ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns. */
  virtual void currents( double *c ) const;

    /*! Add parameters as options. */
  virtual void add( void );
    /*! Read out the current values from the list of Options. */
  virtual void notify( void );

 protected:

  double ECa;
  double GCa, GKAHP, GKM, GLD, GDS;
  double GCaGates, GKAHPGates, GKMGates, GDSGates, GSDGates;
  double ICa, IKAHP, IKM, IDS, ILD, ISD;
  double SFrac, CaA, CaTau;

};


/*! 
\class MilesDai
\brief [lib] The Miles-Dai model for spinal motoneurones with slowly inactivating sodium current
\author Jan Benda

The two-compartment model is from 
G. B. Miles and Y. Dai and R. M. Brownstone (2005):
Mechanisms underlying the early phase of spike frequency adaptation in mouse spinal motoneurones.
J. Physiol. 566, pp. 519-532.
However, all potentials are shifted by -60mV, all conductances are in muS,
and capacitances in nF.
*/

class MilesDai : public HodgkinHuxley
{
 public:
  MilesDai( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! \copydoc SpikingNeuron::dimension()  */
  virtual int dimension( void ) const;
    /*! \copydoc SpikingNeuron::variables() */
  virtual void variables( vector< string > &varnames ) const;
    /*! \copydoc SpikingNeuron::units() */
  virtual void units( vector< string > &u ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;

    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns. */
  virtual void conductances( double *g ) const;
    /*! Returns the unit of the conductances returned by conductances(),
        i.e. muS */
  virtual string conductanceUnit( void ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns. */
  virtual void currents( double *c ) const;
    /*! Returns the unit of the currents returned by currents(), i.e. uA. */
  virtual string currentUnit( void ) const;
    /*! Returns the unit of the input (the stimulus), i.e. nA. */
  virtual string inputUnit( void ) const;

    /*! Add parameters as options. */
  virtual void add( void );
    /*! Read out the current values from the list of Options. */
  virtual void notify( void );

 protected:

  double ECa;
  double GCa, GKAHP, GLD, GDS;
  double GCaGates, GKAHPGates;
  double ICa, IKAHP, IDS, ILD, ISD;
  double CaA, CaTau, CD;

};


/*! 
\class WangIKNa
\brief [lib] The %Wang et al. 2003 model with a sodium activated potassium current.
\author Jan Benda
\todo unit of calcium and sodium concentration
\todo verify reference, parameter values, and units

(X. J. Wang and Y. Liu and M.V. Sanchez-Vives and D.A. McCormick
(2003): Adaptation and temporal decorrelation by single neurons in the
primary visual cortex. J. Neurophysiol. 89, pp. 3279-3293)
*/

class WangIKNa : public HodgkinHuxley
{
 public:
  WangIKNa( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! \copydoc SpikingNeuron::dimension()  */
  virtual int dimension( void ) const;
    /*! \copydoc SpikingNeuron::variables() */
  virtual void variables( vector< string > &varnames ) const;
    /*! \copydoc SpikingNeuron::units() */
  virtual void units( vector< string > &u ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;

    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns. */
  virtual void conductances( double *g ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns. */
  virtual void currents( double *c ) const;

    /*! Add parameters as options. */
  virtual void add( void );
    /*! Read out the current values from the list of Options. */
  virtual void notify( void );

 protected:

  double ECa;
  double GCaS, GKCaS, GKNa, GDS, GLD, GCaD, GKCaD;
  double GCaSGates, GKCaSGates, GKNaGates, GDSGates, GCaDGates, GKCaDGates, GSDGates;
  double ICaS, IKCaS, IKNa, IDS, ILD, ICaD, IKCaD, ISD;
  double CaSA, CaSTau, CaDA, CaDTau;

};


/*! 
\class Edman
\brief [lib] The %Edman model for a lobster stretch receptor neurone with a slowly inactivating sodium current.
\author Jan Benda

(A. Edman and S. Gestrelius and W. Grampp (1987):
Analysis of gated membrane currents and mechanisms of firing control
in the rapidly adapting lobster stretch receptor neurone.
J. Physiol. 384, pp. 649-669)
*/

class Edman : public SpikingNeuron
{
 public:
  Edman( void );

    /*! \copydoc SpikingNeuron::name() */
  virtual string name( void ) const;
    /*! \copydoc SpikingNeuron::dimension()  */
  virtual int dimension( void ) const;
    /*! \copydoc SpikingNeuron::variables() */
  virtual void variables( vector< string > &varnames ) const;
    /*! \copydoc SpikingNeuron::units() */
  virtual void units( vector< string > &u ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;

    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns. */
  virtual void conductances( double *g ) const;
    /*! Returns the unit of the conductances returned by conductances(),
        i.e. cm^3/s. */
  virtual string conductanceUnit( void ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns. */
  virtual void currents( double *c ) const;
    /*! Returns the unit of the currents returned by currents(), i.e. uA. */
  virtual string currentUnit( void ) const;
    /*! Returns the unit of the input (the stimulus), i.e. nA. */
  virtual string inputUnit( void ) const;

    /*! Add parameters as options. */
  virtual void add( void );
    /*! Read out the current values from the list of Options. */
  virtual void notify( void );

 protected:

  double A, Vol;
  double C, Km, T;
  double GNa, GK, GLNa, GLK, GLCl, GP;
  double Narest, Krest, ClI;
  double NaO, KO, ClO;
  double Vm, Vh, Vl, Vn, Vr;
  double Tmmax, Thmax, Tlmax, Tnmax, Trmax;
  static const double Faraday = 96485.0;        // C/mol
  static const double GasConst = 8.3144;        // J/K/mol
  static const double eCharge = 1.60217653e-19; // C
  static const double kBoltz = 1.3806505e-23;   // J/K
  double FRT, F2RT, ekT;
  double INa, IK, ILNa, ILK, ILCl, IP;
  double GNaGates, GKGates, GLNaA, GLKA, GLClA, GPA;

};


}; /* namespace ephys */

#endif /* ! _RELACS_EPHYS_NEURONMODELS_H_ */
