/*
  kernel.cc
  Base class of all kernel functions.

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
#include "kernel.h"


double Kernel::value( double x ) const
{
  return operator()( x );
}


void Kernel::setScale( double scale )
{
  Scale = scale;
}


double Kernel::mean( void ) const
{
  double l = left();
  double r = right();
  double d = (r-l)/400.0;
  
  double m = 0.0;
  double x = l;
  for ( int k=1; x <= r; k++, x += d )
    m += ( value( x ) - m ) / k;

  return m;
}


double Kernel::stdev( void ) const
{
  double l = left();
  double r = right();
  double d = (r-l)/400.0;
  
  double m = 0.0;
  double x = l;
  for ( int k=1; x <= r; k++, x += d )
    m += ( value( x ) - m ) / k;

  double v = 0.0;
  x = l;
  for ( int k=1; x <= r; k++, x += d ) {
    double s = value( x ) - m;
    v += ( s*s - v ) / k;
  }

  return ::sqrt( v );
}


void Kernel::setStdev( double stdev )
{
  setScale( stdev );
}


double Kernel::max( void ) const
{
  double l = left();
  double r = right();
  double d = (r-l)/400.0;
  
  double m = 0.0;
  for ( double x = l; x <= r; x += d ) {
    double v = value( x );
    if ( m > v )
      m = v;
  }

  return m;
}


double Kernel::left( void ) const
{
  return -0.5*Scale;
}


double Kernel::right( void ) const
{
  return 0.5*Scale;
}


RectKernel::RectKernel( void )
  : Kernel( 1.0 ), Height( 1.0 )
{
}


RectKernel::RectKernel( double stdev )
{
  setStdev( stdev );
}


RectKernel::~RectKernel( void )
{
}


double RectKernel::operator()( double x ) const
{
  return ( x >= -0.5*scale() && x <= 0.5*scale() ) ? Height : 0.0;
}


double RectKernel::value( double x ) const
{
  return operator()( x );
}


void RectKernel::setScale( double scale )
{
  Kernel::setScale( scale );
  Height = 1.0/scale;
}


double RectKernel::mean( void ) const
{
  return 0.0;
}


double RectKernel::stdev( void ) const
{
  return scale() / ::sqrt( 12.0 );
}


void RectKernel::setStdev( double stdev )
{
  setScale( stdev * ::sqrt( 12.0 ) );
}


double RectKernel::max( void ) const
{
  return Height;
}


TriangularKernel::TriangularKernel( void )
  : Kernel( 1.0 ), Max( 1.0 )
{
}


TriangularKernel::TriangularKernel( double stdev )
{
  setStdev( stdev );
}


TriangularKernel::~TriangularKernel( void )
{
}


double TriangularKernel::operator()( double x ) const
{
  return ( x >= -Max && x <= Max ) ? 1/Max - ::fabs( x / scale() ) : 0.0;
}


double TriangularKernel::value( double x ) const
{
  return operator()( x );
}


void TriangularKernel::setScale( double scale )
{
  Kernel::setScale( scale );
  Max = ::sqrt( scale );
}


double TriangularKernel::mean( void ) const
{
  return 0.0;
}


double TriangularKernel::stdev( void ) const
{
  return ::sqrt( scale() / 6.0 );
}


void TriangularKernel::setStdev( double stdev )
{
  setScale( 6.0 * stdev * stdev );
}


double TriangularKernel::max( void ) const
{
  return 1.0/Max;
}


double TriangularKernel::left( void ) const
{
  return -Max;
}


double TriangularKernel::right( void ) const
{
  return Max;
}


EpanechnikovKernel::EpanechnikovKernel( void )
{
  setScale( 1.0 );
}


EpanechnikovKernel::EpanechnikovKernel( double stdev )
{
  setStdev( stdev );
}


EpanechnikovKernel::~EpanechnikovKernel( void )
{
}


double EpanechnikovKernel::operator()( double x ) const
{
  double y = x / Stdev;
  return ( x >= -Max && x <= Max ) ? Fac * ( 1.0 - 0.2*y*y ) : 0.0;
}


double EpanechnikovKernel::value( double x ) const
{
  return operator()( x );
}


void EpanechnikovKernel::setScale( double scale )
{
  setStdev( ::pow( 0.75, 1.0/3.0 ) * ::pow( scale, 2.0/3.0 ) / sqrt( 5.0 ) );
}


double EpanechnikovKernel::mean( void ) const
{
  return 0.0;
}


double EpanechnikovKernel::stdev( void ) const
{
  return Stdev;
}


void EpanechnikovKernel::setStdev( double stdev )
{
  Stdev = stdev;
  Max = ::sqrt( 5.0 ) * stdev;
  Fac = 0.75 / Max;
  Kernel::setScale( ::sqrt( 4.0/3.0 ) * ::pow( 5.0, 0.75 ) * ::pow( stdev, 1.5 ) );
}


double EpanechnikovKernel::max( void ) const
{
  return Fac;
}


double EpanechnikovKernel::left( void ) const
{
  return -Max;
}


double EpanechnikovKernel::right( void ) const
{
  return Max;
}


GaussKernel::GaussKernel( void )
  : Kernel()
{
  setScale( scale() );
}


GaussKernel::GaussKernel( double stdev )
{
  setStdev( stdev );
}


GaussKernel::~GaussKernel( void )
{
}


double GaussKernel::operator()( double x ) const
{
  double z = x/scale();
  return Norm * exp( -0.5*z*z );
}


double GaussKernel::value( double x ) const
{
  return operator()( x );
}


void GaussKernel::setScale( double scale )
{
  Kernel::setScale( scale );
  Norm = 1.0/sqrt( 2.0*M_PI )/scale;
}


double GaussKernel::mean( void ) const
{
  return 0.0;
}


double GaussKernel::stdev( void ) const
{
  return scale();
}


void GaussKernel::setStdev( double stdev )
{
  setScale( stdev );
}


double GaussKernel::max( void ) const
{
  return Norm;
}


double GaussKernel::left( void ) const
{
  return -4.0*scale();
}


double GaussKernel::right( void ) const
{
  return 4.0*scale();
}


GammaKernel::GammaKernel( void )
  : Kernel(), Order( 1 )
{
  setScale( scale() );
}


GammaKernel::GammaKernel( double stdev, int order )
{
  setOrder( order );
  setStdev( stdev );
}


GammaKernel::~GammaKernel( void )
{
}


double GammaKernel::operator()( double x ) const
{
  if ( x < 0.0 )
    return 0.0;

  double z = x/scale();
  if ( Order <= 1 )
    return Norm * exp( -z );
  else if ( Order == 2 )
    return Norm * z * exp( -z );
  else if ( Order == 3 )
    return Norm * z * z * exp( -z );
  else
    return Norm * pow( z, Order-1 ) * exp( -z );
}


double GammaKernel::value( double x ) const
{
  return operator()( x );
}


void GammaKernel::setScale( double scale )
{
  Kernel::setScale( scale );
  double fac = 1.0;
  for ( int k=2; k<Order; k++ )
    fac *= k;
  Norm = 1.0/fac/scale;
}


double GammaKernel::mean( void ) const
{
  return scale() * Order;
}


double GammaKernel::stdev( void ) const
{
  return scale() * ::sqrt( Order );
}


void GammaKernel::setStdev( double stdev )
{
  setScale( stdev / ::sqrt( Order ) );
}


double GammaKernel::max( void ) const
{
  if ( Order <= 1 )
    return Norm;
  else if ( Order == 2 )
    return Norm * exp( -1.0 );
  else if ( Order == 3 )
    return Norm * 4.0 * exp( -2.0 );
  else
    return Norm * pow( Order-1, Order-1 ) * exp( 1.0-Order );
}


double GammaKernel::left( void ) const
{
  return 0.0;
}


double GammaKernel::right( void ) const
{
  return 5.0*stdev();
}

