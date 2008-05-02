/*
  basisfunction.h
  Base class of basis functions.

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

#ifndef _RELACS_BASISFUNCTION_H_
#define _RELACS_BASISFUNCTION_H_ 1

#include <relacs/array.h>

namespace relacs {


/*! 
\class BasisFunction
\author Jan Benda
\version 1.0
\brief Base class of all basis functions.
*/

class BasisFunction
{

public:

    /*! Constructs a BasisFunction. */
  BasisFunction( void ) {}
    /*! Destructor. */
  virtual ~BasisFunction( void ) {}

    /*! Returns the values of the first \a y.size() 
        basis functions at position \a x in \a y.
        This operator is NOT virtual in order to speed up
        computation when using templates. 
        If you need a virtual function as an interface, implement basis(). */
  void operator()( double x, ArrayD &y ) const { return basis( x, y ); };
    /*! Returns the values of the first \a y.size() 
        basis functions at position \a x in \a y.
        This function IS virtual and might slow things down. 
        If you need a NON virtual function as an interface,
	implement operator(). */
  virtual void basis( double x, ArrayD &y ) const;

    /*! Returns the linear combination of the first c.size() basis functions
        at position \a x with coefficients \a c.
        This operator is NOT virtual in order to speed up
        computation when using templates. 
        If you need a virtual function as an interface, implement value(). */
  double operator()( const ArrayD &c, double x ) const { return value( c, x ); };
    /*! Returns the linear combination of the first c.size() basis functions
        at position \a x with coefficients \a c.
        This function IS virtual and might slow things down. 
        If you need a NON virtual function as an interface,
	implement operator(). */
  virtual double value( const ArrayD &c, double x ) const;

};


/*! 
\class Polynom
\author Jan Benda
\version 1.0
\brief A polynomial basis function.


\f[ f_i(x) = x^i \f]
\f[ f(x) = \sum_{i=0}^n c_i x^i \f]
*/

class Polynom : public BasisFunction
{

public:

  Polynom( void );
  virtual ~Polynom( void );

    /*! Returns the values of the first \a y.size() 
        basis functions at position \a x in \a y.
        This operator is NOT virtual in order to speed up
        computation when using templates. 
        If you need a virtual function as an interface, implement basis(). */
  void operator()( double x, ArrayD &y ) const;
    /*! Returns the values of the first \a y.size() 
        basis functions at position \a x in \a y.
        This function IS virtual and might slow things down. 
        If you need a NON virtual function as an interface,
	implement operator(). */
  virtual void basis( double x, ArrayD &y ) const;

    /*! Returns the linear combination of the first c.size() basis functions
        at position \a x with coefficients \a c.
        This operator is NOT virtual in order to speed up
        computation when using templates. 
        If you need a virtual function as an interface, implement value(). */
  double operator()( const ArrayD &c, double x ) const;
    /*! Returns the linear combination of the first c.size() basis functions
        at position \a x with coefficients \a c.
        This function IS virtual and might slow things down. 
        If you need a NON virtual function as an interface,
	implement operator(). */
  virtual double value( const ArrayD &c, double x ) const;

};


/*! 
\class Sine
\author Jan Benda
\version 1.0
\brief A sine basis function.


\f[ f_i(x) = sin( 2 \pi f i x + \varphi ) \f]
\f[ f(x) = \sum_{i=0}^n c_i sin( 2 \pi f i x + \varphi ) \f]
*/

class Sine : public BasisFunction
{

public:

  Sine( void );
  Sine( double freq, double phase=0.0 );
  virtual ~Sine( void );

    /*! The frequency of the sine functions. */
  double frequency( void ) const;
    /*! Set the frequency of the sine functions to \a freq. */
  void setFrequency( double freq );

    /*! The phase of the sine functions. */
  double phase( void ) const;
    /*! Set the phase of the sine functions to \a phase. */
  void setPhase( double phase );

    /*! Returns the values of the first \a y.size() 
        basis functions at position \a x in \a y.
        This operator is NOT virtual in order to speed up
        computation when using templates. 
        If you need a virtual function as an interface, implement basis(). */
  void operator()( double x, ArrayD &y ) const;
    /*! Returns the values of the first \a y.size() 
        basis functions at position \a x in \a y.
        This function IS virtual and might slow things down. 
        If you need a NON virtual function as an interface,
	implement operator(). */
  virtual void basis( double x, ArrayD &y ) const;

    /*! Returns the linear combination of the first c.size() basis functions
        at position \a x with coefficients \a c.
        This operator is NOT virtual in order to speed up
        computation when using templates. 
        If you need a virtual function as an interface, implement value(). */
  double operator()( const ArrayD &c, double x ) const;
    /*! Returns the linear combination of the first c.size() basis functions
        at position \a x with coefficients \a c.
        This function IS virtual and might slow things down. 
        If you need a NON virtual function as an interface,
	implement operator(). */
  virtual double value( const ArrayD &c, double x ) const;


private:

  double A;
  double P;

};


}; /* namespace relacs */

#endif /* ! _RELACS_BASISFUNCTION_H_ */
