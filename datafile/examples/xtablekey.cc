/*
  xtablekey.cc
  

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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include "tablekey.h"
using namespace std;


int main( int argc, char *argv[] )
{
  TableKey key;

  key.addLabel( "Stimulus" );
  key.addLabel( "Stimulus" );
  key.addNumber( "n", "-", "%2.0f" );
  key.addNumber( "I", "mV/cm", "%5.2f" );
  key.addLabel( "Firing Frequency" );
  key.addNumber( "f0", "Hz", "%5.2f" );
  key.addNumber( "fpeakfff", "Hz", "%5.2f" );
  key.addNumber( "fs", "Hzzzzzz", "%5.2f" );
  key.addLabel( "f-I curves" );
  key.addLabel( "Time Constants" );
  key.addNumber( "tau", "ms", "%5.2f" );

  cout << key;
  vector< double > v( 6, 2.5678 );
  key.save( cout, v, 0 ) << endl;

  ofstream df( "tmp.dat" );
  key.saveKey( df );
  df.close();

  ifstream sf( "tmp.dat" );
  StrQueue sq;
  sq.load( sf );
  sf.close();

  TableKey key2;
  key2.loadKey( sq );
  cout << '\n' << key2 << endl;
  key2.saveMetaData( cout );


  return 0;
}
