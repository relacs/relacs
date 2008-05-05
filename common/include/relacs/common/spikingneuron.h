/*
  spikingneuron.h
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

#ifndef _RELACS_COMMON_SPIKINGNEURON_H_
#define _RELACS_COMMON_SPIKINGNEURON_H_ 1

#include <string>
#include <relacs/options.h>
using namespace std;
using namespace relacs;


/*! 
\class SpikingNeuron
\brief Base class for a spiking (point-) neuron.
\author Jan Benda
\author Alexander Wolf
\version 1.2 (May 5, 2008)
\version 1.1 (Jan 10, 2006)
*/

class SpikingNeuron
{

 public:

  SpikingNeuron( void );
  virtual ~SpikingNeuron( void );

    /*! The name of the model. */
  virtual string name( void ) const = 0;
    /*! The dimension of the system. */
  virtual int dimension( void ) const = 0;
    /*! Returns in \a varnames the names of each of the dimension variables. */
  virtual void variables( vector< string > &varnames ) const = 0;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n ) = 0;
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const = 0;
    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return.
        The default implementation returns an empty vector. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns.
        The default implementation returns nothing. */
  virtual void conductances( double *g ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return.
        The default implementation returns an empty vector. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns.
        The default implementation returns nothing. */
  virtual void currents( double *c ) const;
    /*! Add some options to \a o. */
  virtual void add( Options &o ) const {};
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


/*! 
\class Stimulus
\brief Just returns the stimulus.
\author Jan Benda
*/

class Stimulus : public SpikingNeuron
{
 public:
  Stimulus( void );

    /*! The name of the model. */
  virtual string name( void ) const;
    /*! The dimension of the system. */
  virtual int dimension( void ) const;
    /*! Returns in \a varnames the names of each of the dimension variables. */
  virtual void variables( vector< string > &varnames ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;
    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return.
        The default implementation returns an empty vector. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns.
        The default implementation returns nothing. */
  virtual void conductances( double *g ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return.
        The default implementation returns an empty vector. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns.
        The default implementation returns nothing. */
  virtual void currents( double *c ) const;
    /*! Add some options to \a o. */
  virtual void add( Options &o ) const;
    /*! Read values op options \a o.
        Prepend \a label to the search strings. */
  virtual void read( const Options &o, const string &label=0 );

};


/*! 
\class FitzhughNagumo
\brief The Fitzhugh-Nagumo model
\author Jan Benda
*/

class FitzhughNagumo : public SpikingNeuron
{
 public:
  FitzhughNagumo( void );

    /*! The name of the model. */
  virtual string name( void ) const;
    /*! The dimension of the system. */
  virtual int dimension( void ) const;
    /*! Returns in \a varnames the names of each of the dimension variables. */
  virtual void variables( vector< string > &varnames ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;
    /*! Add some options to \a o. */
  virtual void add( Options &o ) const;
    /*! Read values op options \a o.
        Prepend \a label to the search strings. */
  virtual void read( const Options &o, const string &label=0 );

 protected:

  double Phi, A, B, TimeScale, Scale;

};


/*! 
\class MorrisLecar
\brief The Morris-Lecar model.
\author Jan Benda
*/

class MorrisLecar : public SpikingNeuron
{
 public:
  MorrisLecar( void );

    /*! The name of the model. */
  virtual string name( void ) const;
    /*! The dimension of the system. */
  virtual int dimension( void ) const;
    /*! Returns in \a varnames the names of each of the dimension variables. */
  virtual void variables( vector< string > &varnames ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;
    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return.
        The default implementation returns an empty vector. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns.
        The default implementation returns nothing. */
  virtual void conductances( double *g ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return.
        The default implementation returns an empty vector. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns.
        The default implementation returns nothing. */
  virtual void currents( double *c ) const;
    /*! Add some options to \a o. */
  virtual void add( Options &o ) const;
    /*! Read values op options \a o.
        Prepend \a label to the search strings. */
  virtual void read( const Options &o, const string &label=0 );

 protected:

  double ECa, EK, EL;
  double GCa, GK, GL;
  double GCaM, GKW;
  double ICa, IK, IL;
  double MVCa, MKCa, MVK, MKK, MPhiK;
  double C, TimeScale;

};


/*! 
\class HodgkinHuxley
\brief The original Hodgkin-Huxley model
\author Jan Benda
*/

class HodgkinHuxley : public SpikingNeuron
{
 public:
  HodgkinHuxley( void );

    /*! The name of the model. */
  virtual string name( void ) const;
    /*! The dimension of the system. */
  virtual int dimension( void ) const;
    /*! Returns in \a varnames the names of each of the dimension variables. */
  virtual void variables( vector< string > &varnames ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;
    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return.
        The default implementation returns an empty vector. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns.
        The default implementation returns nothing. */
  virtual void conductances( double *g ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return.
        The default implementation returns an empty vector. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns.
        The default implementation returns nothing. */
  virtual void currents( double *c ) const;
    /*! Add some options to \a o. */
  virtual void add( Options &o ) const;
    /*! Read values op options \a o.
        Prepend \a label to the search strings. */
  virtual void read( const Options &o, const string &label=0 );

 protected:

  double C, PT, ENa, EK, EL;
  double GNa, GK, GL;
  double GNaM3H, GKN4;
  double INa, IK, IL;

};


/*! 
\class TraubHH
\brief Simplified %Traub-Miles (1991) model with the HH currents I_Na, I_K, and I-l only.
\author Jan Benda

Conductances are from %Traub, scaled to Ermentrouts Na.
Potentials are from %Traub & Miles (1991)
*/

class TraubHH : public HodgkinHuxley
{
 public:
  TraubHH( void );

    /*! The name of the model. */
  virtual string name( void ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;

};


/*! 
\class Traub
\brief %Traub-Miles (1991) soma model.
\author Jan Benda

Conductances are from %Traub, scaled to Ermentrouts Na.
Potentials are from %Traub & Miles (1991)
*/

class Traub : public HodgkinHuxley
{
 public:
  Traub( void );

    /*! The name of the model. */
  virtual string name( void ) const;
    /*! The dimension of the system. */
  virtual int dimension( void ) const;
    /*! Returns in \a varnames the names of each of the dimension variables. */
  virtual void variables( vector< string > &varnames ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;
    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return.
        The default implementation returns an empty vector. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns.
        The default implementation returns nothing. */
  virtual void conductances( double *g ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return.
        The default implementation returns an empty vector. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns.
        The default implementation returns nothing. */
  virtual void currents( double *c ) const;
    /*! Add some options to \a o. */
  virtual void add( Options &o ) const;
    /*! Read values op options \a o.
        Prepend \a label to the search strings. */
  virtual void read( const Options &o, const string &label=0 );

 protected:

  double GCa, GAHP;
  double GCaS5R, GAHPQ;
  double ECa, EAHP;
  double ICa, IAHP;

};


/*! 
\class TraubErmentrout
\brief %Traub-Miles (1991) model modified by Ermentrout.
\author Jan Benda
*/

class TraubErmentrout : public HodgkinHuxley
{
 public:
  TraubErmentrout( void );

    /*! The name of the model. */
  virtual string name( void ) const;
    /*! The dimension of the system. */
  virtual int dimension( void ) const;
    /*! Returns in \a varnames the names of each of the dimension variables. */
  virtual void variables( vector< string > &varnames ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;
    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return.
        The default implementation returns an empty vector. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns.
        The default implementation returns nothing. */
  virtual void conductances( double *g ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return.
        The default implementation returns an empty vector. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns.
        The default implementation returns nothing. */
  virtual void currents( double *c ) const;
    /*! Add some options to \a o. */
  virtual void add( Options &o ) const;
    /*! Read values op options \a o.
        Prepend \a label to the search strings. */
  virtual void read( const Options &o, const string &label=0 );

 protected:

  double GCa, GM, GAHP;
  double GCaS, GMW, GAHPQ;
  double ECa, EM, EAHP;
  double ICa, IM, IAHP;
  double TauW;

};


/*! 
\class Wang
\brief The %Wang model.
\author Jan Benda
*/

class Wang : public HodgkinHuxley
{
 public:
  Wang( void );

    /*! The name of the model. */
  virtual string name( void ) const;
    /*! The dimension of the system. */
  virtual int dimension( void ) const;
    /*! Returns in \a varnames the names of each of the dimension variables. */
  virtual void variables( vector< string > &varnames ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;

};


/*! 
\class WangAdapt
\brief The %Wang model with an additional adaptation current.
\author Jan Benda
*/

class WangAdapt : public Wang
{
 public:
  WangAdapt( void );

    /*! The name of the model. */
  virtual string name( void ) const;
    /*! The dimension of the system. */
  virtual int dimension( void ) const;
    /*! Returns in \a varnames the names of each of the dimension variables. */
  virtual void variables( vector< string > &varnames ) const;
    /*! Computes the derivative \a dxdt at time \a t
        with stimulus \a s given the state \a x. */
  virtual void operator()(  double t, double s, double *x, double *dxdt, int n );
    /*! Initialize the state \a x with usefull inital conditions. */
  virtual void init( double *x ) const;
    /*! Returns in \a conductancenames the names of the individual 
        ionic conductances that conductances( double * ) would return.
        The default implementation returns an empty vector. */
  virtual void conductances( vector< string > &conductancenames ) const;
    /*! Returns in \a g the values of the individual ionic conductances.
        The number of conductances is defined by the size of 
        \a conductancenames the function conductances() returns.
        The default implementation returns nothing. */
  virtual void conductances( double *g ) const;
    /*! Returns in \a currentnames the names of the individual ionic currents
        that currents( double * ) would return.
        The default implementation returns an empty vector. */
  virtual void currents( vector< string > &currentnames ) const;
    /*! Returns in \a c the values of the individual ionic currents.
        The number of currents is defined by the size of \a currentnames
        the function currents() returns.
        The default implementation returns nothing. */
  virtual void currents( double *c ) const;
    /*! Add some options to \a o. */
  virtual void add( Options &o ) const;
    /*! Read values op options \a o.
        Prepend \a label to the search strings. */
  virtual void read( const Options &o, const string &label=0 );

 protected:

  double EA;
  double GA;
  double GAA;
  double Atau;
  double IA;

};


#endif /* ! _RELACS_COMMON_NEURONMODELS_H_ */
