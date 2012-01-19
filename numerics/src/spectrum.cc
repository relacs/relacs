/*
  spectrum.cc
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#include <relacs/spectrum.h>

namespace relacs {


double bartlett( int j, int n )
{
  double a = 2.0/(n-1);
  double w = j*a;
  if ( w > 1.0 )
    w = 2.0 - w;
  return w;
}


double blackman( int j, int n )
{
  double a = 2.0*M_PI/(n-1);
  return 0.42 - 0.5 * ::cos(a*j) + 0.08 * ::cos(2.0*a*j);
}


double blackmanHarris( int j, int n )
{
  double a = 2.0*M_PI/(n-1);
  return 0.35875 - 0.48829 * ::cos(a*j) + 0.14128 * ::cos(2.0*a*j) - 0.01168 * ::cos(3.0*a*j);
}


double hamming( int j, int n )
{
  double a = 2.0*M_PI/(n-1);
  return 0.54 - 0.46 * ::cos(a*j);
}


double hanning( int j, int n )
{
  double a = 2.0*M_PI/(n-1);
  return 0.5 - 0.5 * ::cos(a*j);
}


double parzen( int j, int n )
{
  double a = (n-1)/2.0;
  double w = (j-a)/(a+1);
  if ( w > 0.0 ) 
    w = 1.0 - w;
  else 
    w = 1.0 + w;
  return w;
}


double square( int j, int n )
{
  return 1.0;
}


double welch( int j, int n )
{
  double a = (n-1)/2.0;
  double w = (j-a)/(a+1);
  return 1 - w*w;
}


}; /* namespace relacs */

