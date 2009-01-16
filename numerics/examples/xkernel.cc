/*
  xkernel.cc
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <iostream>
#include <relacs/kernel.h>
using namespace std;
using namespace relacs;

double scale = 1.0;


void print( Kernel *k )
{
  cerr << "left: " << k->left();
  cerr << " right: " << k->right();
  cerr << endl;
  for ( double x=k->left(); x<=k->right(); x+=0.02 ) {
    cout << x;
    cout << "  " << k->value( x );
    cout << endl;
  }
  cout << endl;
  cout << endl;
}


int main( void )
{
  RectKernel rek( scale );
  GaussKernel gak( scale );
  GammaKernel gmk1( scale, 1 );
  GammaKernel gmk2( scale, 2 );
  GammaKernel gmk3( scale, 4 );

  print( &rek );
  print( &gak );
  print( &gmk1 );
  print( &gmk2 );
  print( &gmk3 );

  return 0;
}
