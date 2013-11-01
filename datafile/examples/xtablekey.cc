/*
  xtablekey.cc
  

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

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>
#include <relacs/tablekey.h>
using namespace std;
using namespace relacs;


int main( int argc, char *argv[] )
{
  TableKey key;

  key.newSection( "Stimulus" );
  key.addNumber( "n", "-", "%2.0f" );
  key.addNumber( "I", "mV/cm", "%5.1f" );
  key.newSection( "Results" );
  key.newSubSection( "Firing Frequency" );
  key.addNumber( "f0", "Hz", "%5.2f" );
  key.addNumber( "fpeakfff", "Hz", "%5.3f" );
  key.addNumber( "fs", "Hzzzzzz", "%6.4f" );
  key.newSection( "Timescales" );
  key.addNumber( "tau", "ms", "%7.5f" );
  key.addNumber( "delay", "ms", "%8.6f" );

  key.saveMetaData( cout );
  cout << '\n';

  cout << key;

  vector< double > v( 7, 2.5678 );
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

  cout << '\n';
  cout << "Column of \"n\" : " << key2.column( "n" ) << '\n';
  cout << "Column of \"Results\" : " << key2.column( "Results" ) << '\n';
  cout << "Column of \"Results>fpeakfff\" : " << key2.column( "Results>fpeakfff" ) << '\n';
  cout << "Column of \"Results>Firing Frequency>fs\" : " << key2.column( "Results>Firing Frequency>fs" ) << '\n';
  cout << "Column of \"Timescales\" : " << key2.column( "Timescales" ) << '\n';
  cout << "Column of \"Timescales>tau\" : " << key2.column( "Timescales>tau" ) << '\n';
  cout << "Column of \"Timescales>tau>delay\" : " << key2.column( "Timescales>tau>delay" ) << '\n';
  cout << '\n';

  cout << "Erase column 5:\n";
  key.erase( 5 );
  cout << key << '\n';

  cout << "Erase \"delay\":\n";
  key.erase( "delay" );
  cout << key << '\n';

  cout << "Erase \"Stimulus\":\n";
  key.erase( "Stimulus" );
  cout << key << '\n';

  return 0;
}
