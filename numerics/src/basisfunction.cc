/*
  basisfunction.cc
  Base class of basis functions.

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
#include <relacs/basisfunction.h>


void BasisFunction::basis( double x, ArrayD &y ) const
{
  operator()( x, y );
}


double BasisFunction::value( const ArrayD &c, double x ) const
{
  return operator()( c, x );
}


Polynom::Polynom( void )
  : BasisFunction()
{
}


Polynom::~Polynom( void )
{
}


void Polynom::operator()( double x, ArrayD &y ) const
{
  y[0] = 1.0;
  for ( int i=1; i<y.size(); i++ )
    y[i] = y[i-1]*x;
}


void Polynom::basis( double x, ArrayD &y ) const
{
  operator()( x, y );
}


double Polynom::operator()( const ArrayD &c, double x ) const
{
  int n = c.size() - 1;
  if ( n < 0 )
    return 0.0;
  double y = c[n];
  for ( int i=n-1; i>=0; i-- )
    y = y * x + c[i];
  return y;
}


double Polynom::value( const ArrayD &c, double x ) const
{
  return operator()( c, x );
}


Sine::Sine( void ) 
  : BasisFunction(),
    A( 2.0 * M_PI ),
    P( 0.0 )
{
}


Sine::Sine( double freq, double phase ) 
  : BasisFunction(),
    A( 2.0 * M_PI * freq ),
    P( phase )
{
}


Sine::~Sine( void )
{
}


double Sine::frequency( void ) const
{
  return A/2.0/M_PI;
}


void Sine::setFrequency( double freq )
{
  A = 2.0 * M_PI * freq;
}


double Sine::phase( void ) const
{
  return P;
}


void Sine::setPhase( double phase )
{
  P = phase;
}


void Sine::operator()( double x, ArrayD &y ) const
{
  y[0] = 1.0;
  for ( int i=1; i<y.size(); i++ )
    y[i] = ::sin( A*i*x + P );
}


void Sine::basis( double x, ArrayD &y ) const
{
  operator()( x, y );
}


double Sine::operator()( const ArrayD &c, double x ) const
{
  if ( c.empty() )
    return 0.0;
  double y = c[0];
  for ( int i=1; i<c.size(); i++ )
    y += c[i]*::sin( A*i*x + P );
  return y;
}


double Sine::value( const ArrayD &c, double x ) const
{
  return operator()( c, x );
}



#include "moc_basisfunction.cc"
