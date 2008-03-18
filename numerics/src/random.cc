/*
  random.cc
  The base class for random number generators.

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

#include <cmath>
#include <iostream>
#include <relacs/random.h>
using namespace std;


double RandomBase::exponential( void )
{
  double dum;
  
  do {
    dum = uniform();
  } while ( dum  ==  0.0 );
  return -::log( dum );
} 


double RandomBase::gamma( int a )
{
  int j;
  double am, e, s, v1, v2, x, y;

  if ( a < 1 )
    cerr << "Wrong order in RandomBase::gamma() !\n";
  if ( a < 6 ) { 
    x = 1.0;
    for ( j = 1; j <= a; j++ ) 
      x *=  uniform();
    x = -::log(x);
  } 
  else {
    do {
      do {
	do {
	  v1 = 2.0*uniform()-1.0;
	  v2 = 2.0*uniform()-1.0;
	} while ( v1*v1+v2*v2 > 1.0 );
	y = v2/v1;
	am = a-1;
	s = ::sqrt( 2.0*am+1.0 );
	x = s*y+am;
      } while ( x <= 0.0 );
      e = (1.0+y*y)*::exp(am*::log(x/am)-s*y);
    } while ( uniform() > e );
  }
  return x;
}


RandomStd::RandomStd( void )
  : ISet( 0 ) 
{
  setSeed( 0 );
}


RandomStd::RandomStd( unsigned long seed )
  : ISet( 0 )
{
  setSeed( seed );
}


RandomStd::~RandomStd( void )
{
}


unsigned long RandomStd::setSeed( unsigned long seed )
{
  if ( seed == 0 )
    seed = (long)( time(NULL) | 1 );
  srand( seed );
  return seed;
}


unsigned long RandomStd::min( void ) const
{
  return 0;
}


unsigned long RandomStd::max( void ) const
{
  return RAND_MAX;
}


unsigned long RandomStd::integer( void )
{
  return rand();
}


double RandomStd::uniform( void )
{
  return double(rand())/double(RAND_MAX);
}


double RandomStd::gaussian( void )
{
  if ( ISet == 0 ) {
    double v1, v2;
    double fac, rsq;
    do {
      v1 = 2.0*operator()()-1.0;
      v2 = 2.0*operator()()-1.0;
      rsq = v1*v1+v2*v2;
    } while ( rsq >=  1.0 || rsq  ==  0.0 );
    fac = ::sqrt( -2.0*::log(rsq)/rsq );
    GSet = v1*fac;
    ISet = 1;
    return v2*fac;
  } 
  else {
    ISet = 0;
    return GSet;
  }
}


double RandomStd::exponential( void )
{
  double dum;
  
  do {
    dum = operator()();
  } while ( dum  ==  0.0 );
  return -::log( dum );
} 


double RandomStd::gamma( int a )
{
  int j;
  double am, e, s, v1, v2, x, y;

  if ( a < 1 )
    cerr << "Wrong order in RandomStd::gamma() !\n";
  if ( a < 6 ) { 
    x = 1.0;
    for ( j = 1; j <= a; j++ ) 
      x *=  operator()();
    x = -::log(x);
  } 
  else {
    do {
      do {
	do {
	  v1 = 2.0*operator()()-1.0;
	  v2 = 2.0*operator()()-1.0;
	} while ( v1*v1+v2*v2 > 1.0 );
	y = v2/v1;
	am = a-1;
	s = ::sqrt( 2.0*am+1.0 );
	x = s*y+am;
      } while ( x <= 0.0 );
      e = (1.0+y*y)*::exp(am*::log(x/am)-s*y);
    } while ( operator()() > e );
  }
  return x;
}


string RandomStd::name( void )
{
  return "rand";
}


const long Ran3::MBIG = 1000000000;
const long Ran3::MSEED = 161803398;
const long Ran3::MZ = 0;
const double Ran3::FAC = 1.0/MBIG;

Ran3::Ran3( void )
  : Iff( 0 ),
    ISet( 0 )
{
  setSeed( 0 );
}


Ran3::Ran3( unsigned long seed )
  : Iff( 0 ),
    ISet( 0 )
{
  setSeed( seed );
}


Ran3::~Ran3( void )
{
}


unsigned long Ran3::setSeed( unsigned long seed )
{ 
  if ( seed == 0 )
    seed = (long)( time(NULL) | 1 );
  Idum = seed;
  return seed;
}


unsigned long Ran3::integer( void )
{
  long mj, mk;
  int i, ii, k;

  if ( Idum < 0 || Iff == 0 ) {
    Iff = 1;
    mj = labs( MSEED - labs(Idum) );
    mj %= MBIG;
    Ma[55] = mj;
    mk = 1;
    for ( i = 1; i <= 54; i++ ) {
      ii = (21*i) % 55;
      Ma[ii] = mk;
      mk = mj-mk;
      if (mk < MZ)
	mk += MBIG;
      mj = Ma[ii];
    }
    for ( k = 1; k <= 4; k++ ) {
      for ( i = 1; i <= 55; i++) {
	Ma[i] -= Ma[1+(i+30) % 55];
	if (Ma[i] < MZ)
	  Ma[i] += MBIG;
      }
    }
    Inext = 0;
    Inextp = 31;
    Idum = 1;
  }
  if ( ++Inext == 56 )
    Inext = 1;
  if ( ++Inextp == 56 )
    Inextp = 1;
  mj = Ma[Inext] - Ma[Inextp];
  if (mj < MZ)
    mj += MBIG;
  Ma[Inext] = mj;
  return mj;
}


unsigned long Ran3::min( void ) const
{
  return 0;
}


unsigned long Ran3::max( void ) const
{
  return MBIG;
}


double Ran3::uniform( void )
{
  return integer() * FAC;
}


double Ran3::gaussian( void )
{
  if ( ISet == 0 ) {
    double v1, v2;
    double fac, rsq;
    do {
      v1 = 2.0*operator()()-1.0;
      v2 = 2.0*operator()()-1.0;
      rsq = v1*v1+v2*v2;
    } while ( rsq >= 1.0 || rsq == 0.0 );
    fac = ::sqrt( -2.0*::log(rsq)/rsq );
    GSet = v1*fac;
    ISet = 1;
    return v2*fac;
  } 
  else {
    ISet  =  0;
    return GSet;
  }
}


double Ran3::exponential( void )
{
  double dum;
  
  do {
    dum = operator()();
  } while ( dum  ==  0.0 );
  return -::log( dum );
} 


double Ran3::gamma( int a )
{
  int j;
  double am, e, s, v1, v2, x, y;

  if ( a < 1 )
    cerr << "Wrong order in Ran3::gamma() !\n";
  if ( a < 6 ) { 
    x = 1.0;
    for ( j = 1; j <= a; j++ ) 
      x *=  operator()();
    x = -::log(x);
  } 
  else {
    do {
      do {
	do {
	  v1 = 2.0*operator()()-1.0;
	  v2 = 2.0*operator()()-1.0;
	} while ( v1*v1+v2*v2 > 1.0 );
	y = v2/v1;
	am = a-1;
	s = ::sqrt( 2.0*am+1.0 );
	x = s*y+am;
      } while ( x <= 0.0 );
      e = (1.0+y*y)*::exp(am*::log(x/am)-s*y);
    } while ( operator()() > e );
  }
  return x;
}


string Ran3::name( void )
{
  return "ran3";
}


#ifdef RAND55


Rand55::Rand55( void )
{
  init_rand55( 0 );
}


Rand55::Rand55( unsigned long seed )
{
  init_rand55( seed );
}


Rand55::~Rand55( void )
{
}


unsigned long Rand55::setSeed( unsigned long seed )
{
  if ( seed == 0 )
    seed = (long)( time(NULL) | 1 );
  init_rand55( seed );
  return seed;
}


unsigned long Rand55::integer( void )
{
  return rand55();
}


unsigned long Rand55::min( void ) const
{
  return 0;
}


unsigned long Rand55::max( void ) const
{
  return ULONG_MAX;
}


double Rand55::uniform( void )
{
  return drand55();
}


double Rand55::gaussian( void )
{
  return gauss_rand55();
}


double Rand55::exponential( void )
{
  return exp_rand55();
}


string Rand55::name( void )
{
  return "rand55";
}


#endif


#ifdef GSL


RandomGSL::RandomGSL( const gsl_rng_type *rt )
  : Rng( 0 )
{
  Rng = gsl_rng_alloc( rt );
  setSeed( 0 );
}


RandomGSL::RandomGSL( unsigned long seed, const gsl_rng_type *rt )
{
  Rng = gsl_rng_alloc( rt );
  setSeed( seed );
}


RandomGSL::~RandomGSL( void ) 
{
  if ( Rng != 0 )
    gsl_rng_free( Rng );
  Rng = 0;
}


unsigned long RandomGSL::setSeed( unsigned long seed )
{
  if ( seed == 0 )
    seed = (long)( time(NULL) | 1 );
  gsl_rng_set( Rng, seed );
  return seed;
}


unsigned long RandomGSL::integer( void )
{
  return gsl_rng_get( Rng );
}


unsigned long RandomGSL::min( void ) const
{
  return gsl_rng_min( Rng );
}


unsigned long RandomGSL::max( void ) const
{
  return gsl_rng_max( Rng );
}


double RandomGSL::uniform( void )
{
  return gsl_rng_uniform( Rng );
}


double RandomGSL::gaussian( void )
{
  return gsl_ran_ugaussian( Rng );
}


double RandomGSL::exponential( void )
{
  return gsl_ran_exponential( Rng, 1.0 );
}


double RandomGSL::gamma( double a )
{
  return gsl_ran_gamma( Rng, a, 1.0 );
}


string RandomGSL::name( void )
{ 
  return gsl_rng_name( Rng );
}


#endif


