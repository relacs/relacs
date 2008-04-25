/*
  random.h
  The base class for random number generators.

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

#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <cstdlib>
#include <cmath>
#include <string>
#include <ctime>
using namespace std;


/*!
\class RandomBase
\author Jan Benda
\version 1.0
\brief The base class for random number generators.


A global variable \a rnd is defined with the best available
random generator available on your system.
\code
  double v = rnd();  // a random number from the uniform distribution [0,1)
  double g = rnd.gaussian(); // a random number from a gaussian distribution.
\endcode
 */

class RandomBase
{

public:

  RandomBase( void ) {};
  RandomBase( unsigned long seed ) {};
  virtual ~RandomBase( void ) {};

    /*! Set the seed of the random number generator to \a seed.
        If \a seed is 0, then the system time is used to generate a seed
        to imitate real randomness.
        Returns the seed. */
  virtual unsigned long setSeed( unsigned long seed ) = 0;

    /*! Returns a uniformly distributed random integer between min() and max(). */
  virtual unsigned long integer( void ) = 0;
    /*! The minimum value integer() returns. */
  virtual unsigned long min( void ) const = 0;
    /*! The maximum value integer() returns. */
  virtual unsigned long max( void ) const = 0;
    /*! Returns an uniformly distributed integer random number between zero
        and \a n.
        The range includes 0 but excludes \a n.
        This operator conforms to the STL RandomNumberGenerator specification.
        This function is NOT virtual in order to speed up computation. */
  unsigned long operator()( unsigned long n );

    /*! Returns an uniformly distributed random number between zero and one.
        The range includes 0.0 but excludes 1.0. */
  double operator()( void );
    /*! Returns an uniformly distributed random number between zero and one.
        The range includes 0.0 but excludes 1.0. */
  virtual double uniform( void ) = 0;

    /*! Returns a unit gaussian distributed random number. */
  virtual double gaussian( void ) = 0;
    /*! Returns an exponential distributed random number with mean one. */
  virtual double exponential( void );
    /*! Returns a gamma distributed random number 
        p(x) dx = x^(a-1) exp(-x)/Gamma(a) dx. */
  virtual double gamma( int a );

    /*! The name of the random number generator. */
  virtual string name( void ) = 0;

};


/*!
\class RandomStd
\author Jan Benda
\version 1.0
\brief An implementation of RandomBase 
  with the random number generator from the standard C library.
 */

class RandomStd : public RandomBase
{

public:

  RandomStd( void );
  RandomStd( unsigned long seed );
  virtual ~RandomStd( void );

    /*! Set the seed of the random number generator to \a seed.
        If \a seed is 0, then the system time is used to generate a seed
        to imitate real randomness.
        Returns the seed. */
  virtual unsigned long setSeed( unsigned long seed );

    /*! Returns a uniformly distributed random integer between min() and max(). */
  virtual unsigned long integer( void );
    /*! The minimum value integer() returns. */
  virtual unsigned long min( void ) const;
    /*! The maximum value integer() returns. */
  virtual unsigned long max( void ) const;
    /*! Returns an uniformly distributed integer random number between zero
        and \a n.
        The range includes 0 but excludes \a n.
        This operator conforms to the STL RandomNumberGenerator specification.
        This function is NOT virtual in order to speed up computation. */
  unsigned long operator()( unsigned long n )
    { return (unsigned long)::ceil( double(rand())/double(RAND_MAX) * n ); };

    /*! Returns a uniformly distributed random number between zero and one.
        The range includes 0.0 but excludes 1.0.
        This function is NOT virtual in order to speed up computation. */
  inline double operator()( void )
    { return double(rand())/double(RAND_MAX); };
    /*! Returns a uniformly distributed random number between zero and one.
        The range includes 0.0 but excludes 1.0. */
  virtual double uniform( void );

    /*! Returns a unit gaussian distributed random number. */
  virtual double gaussian( void );
    /*! Returns an exponential distributed random number with mean one. */
  virtual double exponential( void );
    /*! Returns a gamma distributed random number 
        p(x) dx = x^(a-1) exp(-x)/Gamma(a) dx. */
  virtual double gamma( int a );

    /*! The name of the random number generator ("rand"). */
  virtual string name( void );


private:

  int ISet;
  double GSet;

};


/*!
\class Ran3
\author Jan Benda
\version 1.0
\brief An implementation of RandomBase 
  with the ran3 random number generator from the numerical recipes.
 */

class Ran3 : public RandomBase
{

public:

  Ran3( void );
  Ran3( unsigned long seed );
  virtual ~Ran3( void );

    /*! Set the seed of the random number generator to \a seed.
        If \a seed is 0, then the system time is used to generate a seed
        to imitate real randomness.
        Returns the seed. */
  virtual unsigned long setSeed( unsigned long seed );

    /*! Returns a uniformly distributed random integer between min() and max(). */
  virtual unsigned long integer( void );
    /*! The minimum value integer() returns. */
  virtual unsigned long min( void ) const;
    /*! The maximum value integer() returns. */
  virtual unsigned long max( void ) const;
    /*! Returns an uniformly distributed integer random number between zero
        and \a n.
        The range includes 0 but excludes \a n.
        This operator conforms to the STL RandomNumberGenerator specification.
        This function is NOT virtual in order to speed up computation. */
  unsigned long operator()( unsigned long n )
    { return (unsigned long)::ceil( integer() * FAC * n ); };

    /*! Returns a uniformly distributed random number between zero and one.
        The range includes 0.0 but excludes 1.0.
        This function is NOT virtual in order to speed up computation. */
  double operator()( void )
    { return integer() * FAC; };
    /*! Returns a uniformly distributed random number between zero and one.
        The range includes 0.0 but excludes 1.0. */
  virtual double uniform( void );

    /*! Returns a unit gaussian distributed random number. */
  virtual double gaussian( void );
    /*! Returns an exponential distributed random number with mean one. */
  virtual double exponential( void );
    /*! Returns a gamma distributed random number 
        p(x) dx = x^(a-1) exp(-x)/Gamma(a) dx. */
  virtual double gamma( int a );

    /*! The name of the random number generator ("ran3"). */
  virtual string name( void );


private:

  long Idum;
  int Inext;
  int Inextp;
  long Ma[56];
  int Iff;

  static const long MBIG;
  static const long MSEED;
  static const long MZ;
  static const double FAC;

  int ISet;
  double GSet;

};


#ifdef HAVE_LIBRAND55


#include <limits.h>
#include <rand55.h>


/*!
\class Rand55
\author Jan Benda
\version 1.0
\brief An implementation of RandomBase 
  with the rand55 random number generator by Thomas Fricke
 */

class Rand55 : public RandomBase
{

public:

  Rand55( void );
  Rand55( unsigned long seed );
  virtual ~Rand55( void );

    /*! Set the seed of the random number generator to \a seed.
        If \a seed is 0, then the system time is used to generate a seed
        to imitate real randomness.
        Returns the seed. */
  virtual unsigned long setSeed( unsigned long seed );

    /*! Returns a uniformly distributed random integer between min() and max(). */
  virtual unsigned long integer( void );
    /*! The minimum value integer() returns. */
  virtual unsigned long min( void ) const;
    /*! The maximum value integer() returns. */
  virtual unsigned long max( void ) const;
    /*! Returns an uniformly distributed integer random number between zero
        and \a n.
        The range includes 0 but excludes \a n.
        This operator conforms to the STL RandomNumberGenerator specification.
        This function is NOT virtual in order to speed up computation. */
  unsigned long operator()( unsigned long n )
    { return (unsigned long)::ceil( drand55() * n ); };

    /*! Returns a uniformly distributed random number between zero and one.
        The range includes 0.0 but excludes 1.0
        This function is NOT virtual in order to speed up computation. */
  inline double operator()( void )
    { return drand55(); };
    /*! Returns a uniformly distributed random number between zero and one.
        The range includes 0.0 but excludes 1.0. */
  virtual double uniform( void );

    /*! Returns a unit gaussian distributed random number. */
  virtual double gaussian( void );
    /*! Returns an exponential distributed random number with mean one. */
  virtual double exponential( void );

    /*! The name of the random number generator ("rand55"). */
  virtual string name( void );

};


#endif


#ifdef HAVE_LIBGSL


#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>


/*!
\class RandomGSL
\author Jan Benda
\version 1.0
\brief An implementation of RandomBase 
  for the random number generators from the GSL library.
 */

class RandomGSL : public RandomBase
{

public:

    /*! Construct the random number generator.
        There are many different random number generators available
        that can be passed to this class:
	gsl_rng_mt19937, gsl_gsl_rng_ranlxs0, gsl_rng_ranlxs1, rng_ranlxs2,
        gsl_rng_ranlxd1, gsl_rng_ranlxd2, gsl_rng_ranlux, gsl_rng_ranlux389,
	gsl_rng_cmrg, gsl_rng_mrg, gsl_rng_taus, gsl_rng_taus2, gsl_rng_gfsr4,
        gsl_rng_rand, gsl_rng_random_bsd, gsl_rng_random_libc5, gsl_rng_random_glibc2,
        gsl_rng_rand48, gsl_rng_ranf, gsl_rng_ranmar, gsl_rng_r250, gsl_rng_tt800,
        gsl_rng_vax, gsl_rng_transputer, gsl_rng_randu, gsl_rng_minstd,
        gsl_rng_uni, gsl_rng_uni32, gsl_rng_slatec, gsl_rng_zuf,
        gsl_rng_borosh13, gsl_rng_coveyou, gsl_rng_fishman18, 
	gsl_rng_fishman20, gsl_rng_fishman2x, gsl_rng_knuthran2, 
	gsl_rng_knuthran, gsl_rng_lecuyer21, gsl_rng_waterman14.
        See the GSL documentation for details. */
  RandomGSL( const gsl_rng_type *rt=gsl_rng_mt19937 );
      /*! This constructor creates a random number generator with a specified seed. */
  RandomGSL( unsigned long seed, const gsl_rng_type *rt=gsl_rng_mt19937 );
  virtual ~RandomGSL( void );

    /*! Set the seed of the random number generator to \a seed.
        If \a seed is 0, then the system time is used to generate a seed
        to imitate real randomness.
        Returns the seed. */
  virtual unsigned long setSeed( unsigned long seed );

    /*! Returns a uniformly distributed random integer between min() and max(). */
  virtual unsigned long integer( void );
    /*! Returns a uniformly distributed random integer 
        between 0 and \a max exclusively.
        This function is NOT virtual in order to speed up computation. */
  inline unsigned long integer( unsigned long int max )
    { return gsl_rng_uniform_int( Rng, max ); };
    /*! The minimum value integer() returns. */
  virtual unsigned long min( void ) const;
    /*! The maximum value integer() returns. */
  virtual unsigned long max( void ) const;
    /*! Returns an uniformly distributed integer random number between zero
        and \a n.
        The range includes 0 but excludes \a n.
        This operator conforms to the STL RandomNumberGenerator specification.
        This function is NOT virtual in order to speed up computation. */
  inline unsigned long operator()( unsigned long n )
    { return gsl_rng_uniform_int( Rng, n ); };

    /*! Returns a uniformly distributed random number between zero and one.
        The range includes 0.0 but excludes 1.0.
        This operator is NOT virtual in order to speed up computation. */
  inline double operator()( void )
    { return gsl_rng_uniform( Rng ); };
    /*! Returns a uniformly distributed random number between zero and one.
        The range includes 0.0 but excludes 1.0. */
  virtual double uniform( void );
    /*! Returns a uniformly distributed random number between zero and one.
        The range excludes both 0.0 and 1.0
        This function is NOT virtual in order to speed up computation. */
  inline double positive( void )
    { return gsl_rng_uniform_pos( Rng ); };

    /*! Returns a unit gaussian distributed random number. */
  virtual double gaussian( void );
    /*! Returns a gaussian distributed random number with standard deviation \a sigma. */
  inline double gaussian( double sigma )
    { return gsl_ran_gaussian( Rng, sigma ); };

    /*! Returns an exponential distributed random number with mean one. */
  virtual double exponential( void );
    /*! Returns an exponential distributed random number with mean \a mu. */
  inline double exponential( double mu )
    { return gsl_ran_exponential( Rng, mu ); };

    /*! Returns a gamma distributed random number 
        p(x) dx = x^(a-1) exp(-x)/Gamma(a) dx. */
  virtual double gamma( double a );
    /*! Returns a gamma distributed random number
        p(x) dx = x^(a-1) exp(-x/b)/Gamma(a) b^a dx. */
  inline double gamma( double a, double b )
    { return gsl_ran_gamma( Rng, a, b ); };

    /*! Returns a lognormal distributed random number
        p(x) dx =  (1 / x sqrt(2 pi sigma^2)) exp(-(ln(x) - zeta)^2/2 sigma^2) dx. */
  inline double lognormal( double zeta, double sigma )
    { return gsl_ran_lognormal( Rng, zeta, sigma ); };

    /*! Returns a Chi-squared distributed random number
        p(x) dx = (1 / Gamma(nu/2)) (x/2)^(nu/2 - 1) exp(-x/2) dx */
  inline double chisq( double nu )
    { return gsl_ran_chisq( Rng, nu ); };

    /*! Returns a F distributed random number. */
  inline double fdist( double nu1, double nu2 )
    { return gsl_ran_fdist( Rng, nu1, nu2 ); };

    /*! Returns a t distributed random number. */
  inline double tdist( double nu )
    { return gsl_ran_tdist( Rng, nu ); };

    /*! Returns a Poisson distributed random number.
        p(k) = (mu^k / k!) exp(-mu) */
  inline unsigned int poisson( double mu )
    { return gsl_ran_poisson( Rng, mu ); };

    /*! Returns a Binomial distributed random number.
        p(k) = (n! / k! (n-k)!) p^k (1-p)^(n-k) */
  inline int binomial( double p, int n )
    { return gsl_ran_binomial( Rng, p, n ); };

    /*! The name of the random number generator. */
  virtual string name( void );


protected:

  gsl_rng *Rng;

};


#endif


namespace numerics
{

#ifdef HAVE_LIBGSL

  typedef RandomGSL Random;
    /*! A global random number generator. */
  static RandomGSL rnd( gsl_rng_taus );

#else

#ifdef HAVE_LIBRAND55

  typedef Rand55 Random;
    /*! A global random number generator. */
  static Rand55 rnd;

#else

  typedef Ran3 Random;
    /*! A global random number generator. */
  static Ran3 rnd;

#endif

#endif

}; // namespace numerics


#endif
