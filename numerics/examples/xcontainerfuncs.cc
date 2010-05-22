/*
  xcontainerfuncs.cc
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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
#include <iomanip>
#include <vector>
#include <relacs/array.h>
#include <relacs/containerfuncs.h>
using namespace std;
using namespace relacs;


int main( void )
{
  float a = 0.5;

  cerr << "scalar: sin( a ):\n";
  double b = sin( a );   // cmath
  cerr << b << "\n\n";

  cerr << "scalar: ::sin( a ):\n";
  double c = ::sin( a );   // cmath
  cerr << c << "\n\n";

  cerr << "scalar: ::sin( a ):\n";
  double d = relacs::sin( a );    // relacs
  cerr << d << "\n\n";

  vector< long double > vecla;
  vecla.push_back( 0.5 );
  vecla.push_back( 1.0 );
  vecla.push_back( 1.5 );
  cerr << "Vector vector< long double >: sin( vecla ):\n";
  vector< long double > veclb = sin( vecla );    // relacs
  for ( unsigned int k=0; k<veclb.size(); k++ )
    cerr << " " << veclb[k];
  cerr << "\n\n";

  Array< float > arrayfa;
  arrayfa.push( float( 0.5 ) );
  arrayfa.push( float( 1.0 ) );
  arrayfa.push( float( 1.5 ) );
  cerr << "Vector Array< float >: cos( arrayfa ):\n";
  Array< float > arrayfb = cos( arrayfa );    // relacs
  for ( int k=0; k<arrayfb.size(); k++ )
    cerr << " " << arrayfb[k];
  cerr << "\n\n";


  vector< Array< double > > darrayda( 3, Array<double>( 4 ) );
  darrayda[0][0] = 0.5;
  darrayda[0][2] = 1.0;
  darrayda[1][1] = 1.5;
  darrayda[1][0] = 2.0;
  darrayda[2][2] = 2.5;
  darrayda[2][0] = 3.0;
  cerr << "Matrix vector< Array< double > >: exp( darrayda ):\n";
  vector< Array< double > > darraydb = exp( darrayda );    // relacs
  for ( unsigned int j=0; j<darraydb.size(); j++ ) {
    for ( int k=0; k<darraydb[j].size(); k++ )
      cerr << " " << setprecision( 4 ) << setw( 7 ) << darraydb[j][k];
    cerr << "\n";
  }
  cerr << "\n";


  cerr << "scalar: ::pow( 2.0, a ):\n";
  a = 4.0;
  d = ::pow( 2.0, a );    // cmath
  cerr << d << "\n\n";

  cerr << "scalar: pow( 2.0, a ):\n";
  a = 4.0;
  d = pow( 2.0, a );    // relacs
  cerr << d << "\n\n";

  /*
  vecla[0] = 1.0;
  vecla[1] = 2.0;
  vecla[2] = 3.0;
  cerr << "Vector vector< long double >: pow( 2.0, vecla ):\n";
  veclb = pow( 2.0, vecla );    // relacs
  for ( unsigned int k=0; k<veclb.size(); k++ )
    cerr << " " << veclb[k];
  cerr << "\n\n";
  */

  /*
  arrayfa[0] = 1.0;
  arrayfa[1] = 2.0;
  arrayfa[2] = 3.0;
  cerr << "Vector Array< long double >: pow( arrayfa, 2.0 ):\n";
  arrayfb = pow( arrayfa, 2.0 );    // relacs
  for ( int k=0; k<arrayfb.size(); k++ )
    cerr << " " << arrayfb[k];
  cerr << "\n\n";

  cerr << "Vector Array< long double >: pow( arrayfa, vecla ):\n";
  arrayfb = pow( arrayfa, vecla );    // relacs
  for ( int k=0; k<arrayfb.size(); k++ )
    cerr << " " << arrayfb[k];
  cerr << "\n\n";
  */

  /*
  darrayda[0][0] = 1.0;
  darrayda[0][2] = 2.0;
  darrayda[1][1] = 3.0;
  darrayda[1][0] = 4.0;
  darrayda[2][2] = 5.0;
  darrayda[2][0] = 6.0;
  cerr << "Matrix vector< Array< double > >: pow( 2.0, darrayda ):\n";
  darraydb = pow( 2.0, darrayda );    // relacs
  for ( unsigned int j=0; j<darraydb.size(); j++ ) {
    for ( int k=0; k<darraydb[j].size(); k++ )
      cerr << " " << setprecision( 4 ) << setw( 7 ) << darraydb[j][k];
    cerr << "\n";
  }
  cerr << "\n";

  cerr << "Matrix vector< Array< double > >: pow( darrayda, 2.0 ):\n";
  darraydb = pow( darrayda, 2.0 );    // relacs
  for ( unsigned int j=0; j<darraydb.size(); j++ ) {
    for ( int k=0; k<darraydb[j].size(); k++ )
      cerr << " " << setprecision( 4 ) << setw( 7 ) << darraydb[j][k];
    cerr << "\n";
  }
  cerr << "\n";

  cerr << "Matrix vector< Array< double > >: pow( darrayda, darrayda ):\n";
  darraydb = pow( darrayda, darrayda );    // relacs
  for ( unsigned int j=0; j<darraydb.size(); j++ ) {
    for ( int k=0; k<darraydb[j].size(); k++ )
      cerr << " " << setprecision( 6 ) << setw( 9 ) << darraydb[j][k];
    cerr << "\n";
  }
  cerr << "\n";
  */

  return 0;
}
