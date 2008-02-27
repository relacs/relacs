/*
  containerfuncs.h
  Collection of basic mathematical functions 

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _CONTAINERFUNCS_H_
#define _CONTAINERFUNCS_H_

#include <cmath>

/*!
\namespace numerics
\author Jan Benda
\version 0.5
\brief Collection of basic mathematical functions 
       for scalars, arrays, and matrices.
\todo add gamma and bessel functions from the glibc library
 */

namespace numerics
{
    /*! Computes the appropriate width \a width and precision \a prec
        for the output of numbers as text with minimal precision \a step
        and the largest number \a max. */
  template< typename T >
    void numberFormat( T step, T max, int &width, int &prec );

  /*! Returns sin( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container sin( const Container &vec );
    /*! Returns cos( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container cos( const Container &vec );
    /*! Returns tan( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container tan( const Container &vec );

    /*! Returns asin( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container asin( const Container &vec );
    /*! Returns acos( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container acos( const Container &vec );
    /*! Returns atan( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container atan( const Container &vec );
    /*! Returns atan( x, y ) for each element of \a x
        divided by \a y.
        \a Container1 and \a Container2 
        can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's.
        The dimensions of \a Container1 and \a Container2
        may differ. */
  template < typename Container1, typename Container2 >
    Container1 atan( const Container1 &x, const Container2 &y );

    /*! Returns sinh( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container sinh( const Container &vec );
    /*! Returns cosh( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container cosh( const Container &vec );
    /*! Returns tanh( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container tanh( const Container &vec );

    /*! Returns asinh( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container asinh( const Container &vec );
    /*! Returns acosh( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container acosh( const Container &vec );
    /*! Returns atanh( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container atanh( const Container &vec );

    /*! Returns exp( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container exp( const Container &vec );
    /*! Returns log( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container log( const Container &vec );
    /*! Returns log10( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container log10( const Container &vec );

    /*! Returns the error function erf( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container erf( const Container &vec );
    /*! Returns erfc( x ) = 1.0 - erf( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container erfc( const Container &vec );

    /*! Returns the square root sqrt( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container sqrt( const Container &vec );
    /*! Returns the cube root cbrt( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container cbrt( const Container &vec );
    /*! Returns the hypotenuse hypot( x, y ) = sqrt( x*x + y*y )
        for each element of \a x and \a y.
        \a Container1 and \a Container2 
        can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's.
        The dimensions of \a Container1 and \a Container2
        may differ. */
  template < typename Container1, typename Container2 >
    Container1 hypot( const Container1 &x, const Container2 &y );

    /*! Returns the square of each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container square( const Container &vec );
    /*! Returns the cube of each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container cube( const Container &vec );
    /*! Returns pow( x, y ) for each element of \a x
        raised to the power of \a y.
        \a Container1 and \a Container2 
        can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's.
        The dimensions of \a Container1 and \a Container2
        may differ. */
  template < typename Container1, typename Container2 >
    Container1 pow( const Container1 &x, const Container2 &y );

    /*! Returns ceil( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container ceil( const Container &vec );
    /*! Returns floor( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container floor( const Container &vec );

    /*! Returns abs( x ) for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container abs( const Container &vec );

    /*! Returns sin(2*pi*f*x)
        computed from the x-values of the container \a vec.
        \a Container contains numbers
	(\c float 's, \c double 's, or \c long \c double 's.). */
  template < typename Container >
    Container sin( const Container &vec, double f );
    /*! Returns cos(2*pi*f*x)
        computed from the x-values of the container \a vec.
        \a Container contains numbers
	(\c float 's, \c double 's, or \c long \c double 's.). */
  template < typename Container >
    Container cos( const Container &vec, double f );

    /*! Returns the standard normal distribution exp( -0.5*x^2 )/sqrt(2*pi) 
        for each element of \a vec.
        \a Container can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's. */
  template < typename Container >
    Container gauss( const Container &vec );
    /*! Returns the normal distribution
        exp( -0.5*x^2/y^2 )/sqrt(2*pi)/y for each element of \a x and \a y.
        \a Container1 and \a Container2 
        can be a scalar (\c float, \c double, \c long \c double ),
        a vector, a matrix, or any higher dimensional structure of 
        \c float 's, \c double 's, or \c long \c double 's.
        The dimensions of \a Container1 and \a Container2
        may differ. */
  template < typename Container1, typename Container2 >
    Container1 gauss( const Container1 &x, const Container2 &y );
    /*! Returns the normal distribution
        exp( -0.5*(x-m)^2/s^2 )/sqrt(2*pi)/s for each element of \a x.
        \a Container contains numbers
	(\c float 's, \c double 's, or \c long \c double 's.). */
  template < typename Container >
    Container gauss( const Container &x, double s, double m );
    /*! Returns the alpha function y*exp(-y) with
        y = (x-offs)/tau for each element of \a x.
        \a Container contains numbers
	(\c float 's, \c double 's, or \c long \c double 's.). */
  template < typename Container >
    Container alpha( const Container &x, double tau, double offs=0.0 );

    /*! Returns a straight line with abscissa \a abscissa and 
        slope \a slope
        computed from the x-values of the container \a vec.
        \a Container contains numbers
	(\c float 's, \c double 's, or \c long \c double 's.). */
  template < typename Container >
    Container line( const Container &vec, double abscissa, double slope );
    /*! Returns a rectangular pulse pattern with period \a period, 
        duration of the rectangle \a width, and maximum value 1.0
        computed from the x-values of the container \a vec.
	The up- and downstrokes have a width of \a ramp.
        \a Container contains numbers
	(\c float 's, \c double 's, or \c long \c double 's.). */
  template < typename Container >
    Container rectangle( const Container &vec, double period, double width, double ramp=0.0 );
    /*! Returns a sawtooth with period \a period and maximum value 1.0
        computed from the x-values of the container \a vec.
	The downstroke has a width of \a ramp.
        \a Container contains numbers
	(\c float 's, \c double 's, or \c long \c double 's.). */
  template < typename Container >
    Container sawUp( const Container &vec, double period, double ramp=0.0 );
    /*! Returns a sawtooth with period \a period and maximum value 1.0
        computed from the x-values of the container \a vec.
	The upstroke has a width of \a ramp.
        \a Container contains numbers
	(\c float 's, \c double 's, or \c long \c double 's.). */
  template < typename Container >
    Container sawDown( const Container &vec, double period, double ramp=0.0 );
    /*! Returns a triangular sawtooth with period \a period and maximum value 1.0
        computed from the x-values of the container \a vec.
        \a Container contains numbers
	(\c float 's, \c double 's, or \c long \c double 's.). */
  template < typename Container >
    Container triangle( const Container &vec, double period );


  template< typename T >
    void numberFormat( T step, T max, int &width, int &prec )
    {
      // precision:
      T minprec = 5.0e-9;
      T val = fabs( step );
      for ( prec=0; prec<8; prec++ ) {
	val -= floor( val + minprec );
	if ( val < minprec )
	  break;
	val *= 10.0;
	minprec *= 10.0;
      }
      
      // width:
      width = int( ceil( log10( floor( fabs( max ) ) + 1.0 ) ) );
      if ( prec > 0 )
	width += 1 + prec;
      if ( max < 0.0 )
	width++;
    }


#define CONTAINERFUNC1( func, cfunc )				      \
  template < typename Container >				      \
    Container func( const Container &x )			      \
  {								      \
    Container y( x );						      \
    typedef typename Container::iterator ForwardIter;		      \
    for ( ForwardIter iter = y.begin(); iter != y.end(); ++iter ) {   \
      *iter = func( *iter );					      \
    }								      \
    return y;							      \
  }								      \
  								      \
  template <>							      \
    inline float func<float>( const float &x )				      \
    {								      \
      return cfunc( x );					      \
    }								      \
								      \
  template <>							      \
    inline double func<double>( const double &x )			      \
    {								      \
      return cfunc( x );					      \
    }								      \
								      \
  template <>							      \
    inline long double func<long double>( const long double &x )		      \
    {								      \
      return cfunc( x );					      \
    }

#define CONTAINERFUNC2( func, cfunc ) \
  template < typename Container1, typename Container2 >			\
    Container1 func( const Container1 &x, const Container2 &y )		\
  {									\
    Container1 z( x );							\
    typedef typename Container1::iterator ForwardIter1;			\
    typedef typename Container2::const_iterator ForwardIter2;		\
    ForwardIter1 iter1 = z.begin();					\
    ForwardIter2 iter2 = y.begin();					\
    while ( iter1 != z.end() && iter2 != y.end() ) {			\
      *iter1 = func( *iter1, *iter2 );					\
      ++iter1;								\
      ++iter2;								\
    }									\
    return z;								\
  }									\
									\
  template < typename Container1 >					\
    Container1 func( const Container1 &x, const float &y )		\
  {									\
    Container1 z( x );							\
    typedef typename Container1::iterator ForwardIter1;			\
    for ( ForwardIter1 iter1 = z.begin(); iter1 != z.end(); ++iter1 ) { \
      *iter1 = func( *iter1, y );					\
    }									\
    return z;								\
  }									\
									\
  template < typename Container1 >					\
    Container1 func( const Container1 &x, const double &y )		\
  {									\
    Container1 z( x );							\
    typedef typename Container1::iterator ForwardIter1;			\
    for ( ForwardIter1 iter1 = z.begin(); iter1 != z.end(); ++iter1 ) { \
      *iter1 = func( *iter1, y );					\
    }									\
    return z;								\
  }									\
									\
  template < typename Container1 >					\
    Container1 func( const Container1 &x, const long double &y )	\
  {									\
    Container1 z( x );							\
    typedef typename Container1::iterator ForwardIter1;			\
    for ( ForwardIter1 iter1 = z.begin(); iter1 != z.end(); ++iter1 ) { \
      *iter1 = func( *iter1, y );					\
    }									\
    return z;								\
  }									\
									\
  template < typename Container2 >					\
    Container2 func( const float &x, const Container2 &y )		\
  {									\
    Container2 z( y );							\
    typedef typename Container2::iterator ForwardIter2;			\
    for ( ForwardIter2 iter2 = z.begin(); iter2 != z.end(); ++iter2 ) { \
      *iter2 = func( x, *iter2 );					\
    }									\
    return z;								\
  }									\
									\
  template < typename Container2 >					\
    Container2 func( const double &x, const Container2 &y )		\
  {									\
    Container2 z( y );							\
    typedef typename Container2::iterator ForwardIter2;			\
    for ( ForwardIter2 iter2 = z.begin(); iter2 != z.end(); ++iter2 ) { \
      *iter2 = func( x, *iter2 );					\
    }									\
    return z;								\
  }									\
									\
  template < typename Container2 >					\
    Container2 func( const long double &x, const Container2 &y )	\
  {									\
    Container2 z( y );							\
    typedef typename Container2::iterator ForwardIter2;			\
    for ( ForwardIter2 iter2 = z.begin(); iter2 != z.end(); ++iter2 ) { \
      *iter2 = func( x, *iter2 );					\
    }									\
    return z;								\
  }									\
  									\
  inline float func( float x, float y )					\
  {									\
    return cfunc( x, y );						\
  }									\
  									\
  inline double func( float x, double y )				\
  {									\
    return cfunc( x, y );						\
  }									\
  									\
  inline long double func( float x, long double y )			\
  {									\
    return cfunc( x, y );						\
  }									\
  									\
  inline double func( double x, float y )				\
  {									\
    return cfunc( x, y );						\
  }									\
  									\
  inline double func( double x, double y )				\
  {									\
    return cfunc( x, y );						\
  }									\
  									\
  inline long double func( double x, long double y )			\
  {									\
    return cfunc( x, y );						\
  }									\
  									\
  inline long double func( long double x, float y )			\
  {									\
    return cfunc( x, y );						\
  }									\
  									\
  inline long double func( long double x, double y )			\
  {									\
    return cfunc( x, y );						\
  }									\
  									\
  inline long double func( long double x, long double y )		\
  {									\
    return cfunc( x, y );						\
  }

  CONTAINERFUNC1( sin, ::sin );
  CONTAINERFUNC1( cos, ::cos );
  CONTAINERFUNC1( tan, ::tan );

  CONTAINERFUNC1( asin, ::asin );
  CONTAINERFUNC1( acos, ::acos );
  CONTAINERFUNC1( atan, ::atan );
  CONTAINERFUNC2( atan, ::atan2 );

  CONTAINERFUNC1( sinh, ::sinh );
  CONTAINERFUNC1( cosh, ::cosh );
  CONTAINERFUNC1( tanh, ::tanh );

  CONTAINERFUNC1( asinh, ::asinh );
  CONTAINERFUNC1( acosh, ::acosh );
  CONTAINERFUNC1( atanh, ::atanh );

  CONTAINERFUNC1( exp, ::exp );
  CONTAINERFUNC1( log, ::log );
  CONTAINERFUNC1( log10, ::log10 );

  CONTAINERFUNC1( erf, ::erf );
  CONTAINERFUNC1( erfc, ::erfc );

  CONTAINERFUNC1( sqrt, ::sqrt );
  CONTAINERFUNC1( cbrt, ::cbrt );
  CONTAINERFUNC2( hypot, ::hypot );

#define NUMSQUARE( x ) (x)*(x)
  CONTAINERFUNC1( square, NUMSQUARE );
#undef NUMSQUARE
#define NUMCUBE( x ) (x)*(x)*(x)
  CONTAINERFUNC1( cube, NUMCUBE );
#undef NUMCUBE
  CONTAINERFUNC2( pow, ::pow );

  CONTAINERFUNC1( ceil, ::ceil );
  CONTAINERFUNC1( floor, ::floor );

  CONTAINERFUNC1( abs, ::fabs );


  template < typename Container >
    Container sin( const Container &x, double f )
  {
    Container y( x );
    typedef typename Container::iterator ForwardIter;
    for ( ForwardIter iter = y.begin(); iter != y.end(); ++iter ) {
      *iter = sin( 6.28318530717959*f*(*iter) );
    }
    return y;
  }


  template < typename Container >
    Container cos( const Container &x, double f )
  {
    Container y( x );
    typedef typename Container::iterator ForwardIter;
    for ( ForwardIter iter = y.begin(); iter != y.end(); ++iter ) {
      *iter = cos( 6.28318530717959*f*(*iter) );
    }
    return y;
  }


#define NUMGAUSS( x ) ::exp( -0.5*(x)*(x) )*0.398942280401433
  CONTAINERFUNC1( gauss, NUMGAUSS );
#undef NUMGAUSS
#define NUMGAUSS( x, y ) ::exp( -0.5*(x)*(x)/(y)/(y) )*0.398942280401433/y
  CONTAINERFUNC2( gauss, NUMGAUSS );
#undef NUMGAUSS

  template < typename Container >
    Container gauss( const Container &x, double s, double m )
  {
    double n = 0.398942280401433 / s;
    Container y( x );
    typedef typename Container::iterator ForwardIter;
    for ( ForwardIter iter = y.begin(); iter != y.end(); ++iter ) {
      double xx = ((*iter) - m)/s;
      *iter = n * ::exp( -0.5*xx*xx);
    }
    return y;
  }


  template < typename Container >
    Container alpha( const Container &x, double tau, double offs )
  {
    Container y( x );
    typedef typename Container::iterator ForwardIter;
    for ( ForwardIter iter = y.begin(); iter != y.end(); ++iter ) {
      double xx = ((*iter) - offs)/tau;
      *iter = xx > 0.0 ? xx * ::exp( -xx ) : 0.0;
    }
    return y;
  }


  template < typename Container >
    Container line( const Container &x, double abscissa, double slope )
  {
    Container y( x );
    typedef typename Container::iterator ForwardIter;
    for ( ForwardIter iter = y.begin(); iter != y.end(); ++iter ) {
      *iter = abscissa + slope * (*iter);
    }
    return y;
  }


  template < typename Container >
    Container rectangle( const Container &x, double period, double width, double ramp )
  {
    Container y( x );
    typedef typename Container::iterator ForwardIter;
    if ( ramp <= 0.0 ) {
      for ( ForwardIter iter = y.begin(); iter != y.end(); ++iter ) {
	*iter = ::fmod( (*iter), period ) < width ? 1.0 : 0.0;
      }
    }
    else {
      if ( ramp > width )
	ramp = width;
      if ( ramp > period - width )
	ramp = period - width;
      for ( ForwardIter iter = y.begin(); iter != y.end(); ++iter ) {
	double p = ::fmod( (*iter), period );
	*iter = p < width ? ( p < ramp ? p / ramp : 1.0 ) : ( p < width + ramp ? ( width + ramp - p ) / ramp : 0.0 );
      }
    }
    return y;
  }


  template < typename Container >
    Container sawUp( const Container &x, double period, double ramp )
  {
    Container y( x );
    typedef typename Container::iterator ForwardIter;
    if ( ramp <= 0.0 ) {
      for ( ForwardIter iter = y.begin(); iter != y.end(); ++iter ) {
	*iter = ::fmod( (*iter), period ) / period;
      }
    }
    else {
      if ( ramp > 0.5*period )
	ramp = 0.5*period;
      for ( ForwardIter iter = y.begin(); iter != y.end(); ++iter ) {
	double p = ::fmod( (*iter), period );
	*iter = p < period-ramp ? p / ( period - ramp ) : ( period - p ) / ramp;
      }
    }
    return y;
  }


  template < typename Container >
    Container sawDown( const Container &x, double period, double ramp )
  {
    Container y( x );
    typedef typename Container::iterator ForwardIter;
    if ( ramp <= 0.0 ) {
      for ( ForwardIter iter = y.begin(); iter != y.end(); ++iter ) {
	*iter = 1.0 - ::fmod( (*iter), period ) / period;
      }
    }
    else {
      if ( ramp > 0.5*period )
	ramp = 0.5*period;
      for ( ForwardIter iter = y.begin(); iter != y.end(); ++iter ) {
	double p = ::fmod( (*iter), period );
	*iter = p < ramp ? p/ramp : ( period - p ) / ( period - ramp );
      }
    }
    return y;
  }


  template < typename Container >
    Container triangle( const Container &x, double period )
  {
    Container y( x );
    typedef typename Container::iterator ForwardIter;
    for ( ForwardIter iter = y.begin(); iter != y.end(); ++iter ) {
      double p = ::fmod( (*iter), period );
      *iter = p < 0.5*period ? 2.0 * p / period : 2.0 - 2.0 * p / period;
    }
    return y;
  }


#undef CONTAINERFUNC1
#undef CONTAINERFUNC2

}; // namespace numerics


#endif
