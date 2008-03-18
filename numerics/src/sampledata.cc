/*
  sampledata.cc
  A template defining an one-dimensional Array of data with an associated Range.

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

#include <relacs/sampledata.h>


SampleData<> sin( const LinearRange &r, double f )
{
  SampleData<> y( r );
  typedef SampleData<>::iterator ForwardIter1;
  typedef LinearRange::const_iterator ForwardIter2;
  ForwardIter1 iter1 = y.begin();
  ForwardIter1 end1 = y.end();
  ForwardIter2 iter2 = r.begin();
  while ( iter1 != end1 ) {
    *iter1 = ::sin( 6.28318530717959*f*(*iter2) );
    ++iter1;
    ++iter2;
  }
  return y;
}


SampleData<> sin( int n, double offset, double stepsize, double f )
{
  return sin( LinearRange( n, offset, stepsize ), f );
}


SampleData<> sin( double l, double r, double stepsize, double f )
{
  return sin( LinearRange( l, r, stepsize ), f );
}


SampleData<> cos( const LinearRange &r, double f )
{
  SampleData<> y( r );
  typedef SampleData<>::iterator ForwardIter1;
  typedef LinearRange::const_iterator ForwardIter2;
  ForwardIter1 iter1 = y.begin();
  ForwardIter1 end1 = y.end();
  ForwardIter2 iter2 = r.begin();
  while ( iter1 != end1 ) {
    *iter1 = ::cos( 6.28318530717959*f*(*iter2) );
    ++iter1;
    ++iter2;
  }
  return y;
}


SampleData<> cos( int n, double offset, double stepsize, double f )
{
  return cos( LinearRange( n, offset, stepsize ), f );
}


SampleData<> cos( double l, double r, double stepsize, double f )
{
  return cos( LinearRange( l, r, stepsize ), f );
}


SampleData<> gauss( const LinearRange &r )
{
  SampleData<> y( r );
  typedef SampleData<>::iterator ForwardIter1;
  typedef LinearRange::const_iterator ForwardIter2;
  ForwardIter1 iter1 = y.begin();
  ForwardIter1 end1 = y.end();
  ForwardIter2 iter2 = r.begin();
  while ( iter1 != end1 ) {
    *iter1 = ::exp( -0.5*(*iter2)*(*iter2) ) * 0.398942280401433;
    ++iter1;
    ++iter2;
  }
  return y;
}


SampleData<> gauss( int n, double offset, double stepsize )
{
  return gauss( LinearRange( n, offset, stepsize ) );
}


SampleData<> gauss( double l, double r, double stepsize )
{
  return gauss( LinearRange( l, r, stepsize ) );
}


SampleData<> gauss( const LinearRange &r, double s, double m )
{
  double n = 0.398942280401433 / s;
  SampleData<> y( r );
  typedef SampleData<>::iterator ForwardIter1;
  typedef LinearRange::const_iterator ForwardIter2;
  ForwardIter1 iter1 = y.begin();
  ForwardIter1 end1 = y.end();
  ForwardIter2 iter2 = r.begin();
  while ( iter1 != end1 ) {
    double xx = ((*iter2) - m)/s;
    *iter1 = n * ::exp( -0.5*xx*xx);
    ++iter1;
    ++iter2;
  }
  return y;
}


SampleData<> gauss( int n, double offset, double stepsize, double s, double m )
{
  return gauss( LinearRange( n, offset, stepsize ), s, m );
}


SampleData<> gauss( double l, double r, double stepsize, double s, double m )
{
  return gauss( LinearRange( l, r, stepsize ), s, m );
}


SampleData<> alpha( const LinearRange &r, double tau, double offs )
{
  SampleData<> y( r );
  typedef SampleData<>::iterator ForwardIter1;
  typedef LinearRange::const_iterator ForwardIter2;
  ForwardIter1 iter1 = y.begin();
  ForwardIter1 end1 = y.end();
  ForwardIter2 iter2 = r.begin();
  while ( iter1 != end1 ) {
    double xx = ((*iter2) - offs)/tau;
    *iter1 = xx > 0.0 ? xx * ::exp( -xx ) : 0.0;
    ++iter1;
    ++iter2;
  }
  return y;
}


SampleData<> alpha( int n, double offset, double stepsize, double tau, double offs )
{
  return alpha( LinearRange( n, offset, stepsize ), tau, offs );
}


SampleData<> alpha( double l, double r, double stepsize, double tau, double offs )
{
  return alpha( LinearRange( l, r, stepsize ), tau, offs );
}


SampleData<> line( const LinearRange &r, double abscissa, double slope )
{
  SampleData<> y( r );
  typedef SampleData<>::iterator ForwardIter1;
  typedef LinearRange::const_iterator ForwardIter2;
  ForwardIter1 iter1 = y.begin();
  ForwardIter1 end1 = y.end();
  ForwardIter2 iter2 = r.begin();
  while ( iter1 != end1 ) {
    *iter1 = abscissa + slope * (*iter2);
    ++iter1;
    ++iter2;
  }
  return y;
}

SampleData<> line( int n, double offset, double stepsize, double abscissa, double slope )
{
  return line( LinearRange( n, offset, stepsize ), abscissa, slope );
}


SampleData<> line( double l, double r, double stepsize, double abscissa, double slope )
{
  return line( LinearRange( l, r, stepsize ), abscissa, slope );
}


SampleData<> rectangle( const LinearRange &r,
			double period, double width, double ramp )
{
  SampleData<> y( r );
  typedef SampleData<>::iterator ForwardIter1;
  typedef LinearRange::const_iterator ForwardIter2;
  ForwardIter1 iter1 = y.begin();
  ForwardIter1 end1 = y.end();
  ForwardIter2 iter2 = r.begin();
  if ( ramp <= 0.0 ) {
    while ( iter1 != end1 ) {
      *iter1 = ::fmod( (*iter2), period ) < width ? 1.0 : 0.0;
      ++iter1;
      ++iter2;
    }
  }
  else {
    if ( ramp > width )
      ramp = width;
    if ( ramp > period - width )
      ramp = period - width;
    while ( iter1 != end1 ) {
      double p = ::fmod( (*iter2), period );
      *iter1 = p < width ? ( p < ramp ? p / ramp : 1.0 ) : ( p < width + ramp ? ( width + ramp - p ) / ramp : 0.0 );
      ++iter1;
      ++iter2;
    }
  }
  return y;
}


SampleData<> rectangle( int n, double offset, double stepsize,
			double period, double width, double ramp )
{
  return rectangle( LinearRange( n, offset, stepsize ), period, width, ramp );
}


SampleData<> rectangle( double l, double r, double stepsize,
			double period, double width, double ramp=0.0 )
{
  return rectangle( LinearRange( l, r, stepsize ), period, width, ramp );
}


SampleData<> sawUp( const LinearRange &r, double period, double ramp )
{
  SampleData<> y( r );
  typedef SampleData<>::iterator ForwardIter1;
  typedef LinearRange::const_iterator ForwardIter2;
  ForwardIter1 iter1 = y.begin();
  ForwardIter1 end1 = y.end();
  ForwardIter2 iter2 = r.begin();
  if ( ramp <= 0 ) {
    while ( iter1 != end1 ) {
      *iter1 = ::fmod( (*iter2), period ) / period;
      ++iter1;
      ++iter2;
    }
  }
  else {
    if ( ramp > 0.5*period )
      ramp = 0.5*period;
    while ( iter1 != end1 ) {
      double p = ::fmod( (*iter2), period );
      *iter1 = p < period-ramp ? p / ( period - ramp ) : ( period - p ) / ramp;
      ++iter1;
      ++iter2;
    }
  }
  return y;
}


SampleData<> sawUp( int n, double offset, double stepsize, double period, double ramp )
{
  return sawUp( LinearRange( n, offset, stepsize ), period, ramp );
}


SampleData<> sawUp( double l, double r, double stepsize, double period, double ramp )
{
  return sawUp( LinearRange( l, r, stepsize ), period, ramp );
}


SampleData<> sawDown( const LinearRange &r, double period, double ramp )
{
  SampleData<> y( r );
  typedef SampleData<>::iterator ForwardIter1;
  typedef LinearRange::const_iterator ForwardIter2;
  ForwardIter1 iter1 = y.begin();
  ForwardIter1 end1 = y.end();
  ForwardIter2 iter2 = r.begin();
  if ( ramp <= 0.0 ) {
    while ( iter1 != end1 ) {
      *iter1 = 1.0 - ::fmod( (*iter2), period ) / period;
      ++iter1;
      ++iter2;
    }
  }
  else {
    if ( ramp > 0.5*period )
      ramp = 0.5*period;
    while ( iter1 != end1 ) {
      double p = ::fmod( (*iter2), period );
      *iter1 = p < ramp ? p/ramp : ( period - p ) / ( period - ramp );
      ++iter1;
      ++iter2;
    }
  }
  return y;
}


SampleData<> sawDown( int n, double offset, double stepsize, double period, double ramp )
{
  return sawDown( LinearRange( n, offset, stepsize ), period, ramp );
}


SampleData<> sawDown( double l, double r, double stepsize, double period, double ramp )
{
  return sawDown( LinearRange( l, r, stepsize ), period, ramp );
}


SampleData<> triangle( const LinearRange &r, double period )
{
  SampleData<> y( r );
  typedef SampleData<>::iterator ForwardIter1;
  typedef LinearRange::const_iterator ForwardIter2;
  ForwardIter1 iter1 = y.begin();
  ForwardIter1 end1 = y.end();
  ForwardIter2 iter2 = r.begin();
  while ( iter1 != end1 ) {
    double p = ::fmod( (*iter2), period );
    *iter1 = p < 0.5*period ? 2.0 * p / period : 2.0 - 2.0 * p / period;
    ++iter1;
    ++iter2;
  }
  return y;
}


SampleData<> triangle( int n, double offset, double stepsize, double period )
{
  return triangle( LinearRange( n, offset, stepsize ), period );
}


SampleData<> triangle( double l, double r, double stepsize, double period )
{
  return triangle( LinearRange( l, r, stepsize ), period );
}




