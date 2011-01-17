/*
  xparameter.cc
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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
#include <relacs/parameter.h>
using namespace relacs;


void print( const Parameter &p )
{
  cout << "   p.text(): " << p.text( 0 );
  for ( int k=1; k<p.size(); k++ )
    cout << "|" << p.text( k );
  cout << endl;

  if ( p.isAnyNumber() ) {
    cout << " p.number(): " << p.number( 0 );
    for ( int k=1; k<p.size(); k++ )
      cout << "|" << p.number( k );
    cout << endl;

    cout << "  p.error(): " << p.error( 0 );
    for ( int k=1; k<p.size(); k++ )
      cout << "|" << p.error( k );
    cout << endl;
  }

  cout << "   p.unit(): " << p.unit() << endl;
  cout << "p.outUnit(): " << p.outUnit() << endl;
  cout << " p.format(): " << p.format() << endl;
  cout << "   p.type(): " << p.type() << endl;
  cout << "   p.text(): " << p.text( "r='%r', i=%i, s=%s, n=%g, e=%G, u=%u, date=%Y-%m-%d, time=%H:%M:%S" ) << endl;
  cout << "          p: ";
  p.save( cout, 0, false );
  cout << endl;
}


int main( int argc, char *argv[] )
{
  Parameter tp( "textid", "enter a text", "char-text" );

  cout << endl;
  cout << "Text-Parameter set to: " << "char_text" << endl;
  print( tp );

  cout << endl;
  cout << "Text-Parameter set to: " << "(2.0+-0.3)Hz" << endl;
  tp.setText( "(2.0+-0.3)Hz" );
  print( tp );

  cout << endl;
  cout << "Text-Parameter set to: " << "aa|bb||cc" << endl;
  tp.setText( "aa|bb||cc" );
  print( tp );

  cout << endl;
  cout << "Text-Parameter set to: " << "2|0.4kHz|(43+-5)" << endl;
  tp.setText( "2|0.4kHz|(43+-5)" );
  print( tp );

  cout << endl;
  cout << "Text-Parameter setDefault()" << endl;
  tp.setDefault();
  print( tp );


  Parameter np( "numberid", "enter a number", 234.0, 56.0, 0.0, 1000.0, 20.0, "Hz", "kHz", "%5.3f" );

  cout << endl;
  cout << "Number-Parameter set to: " << "234.0, 56.0, 0.0, 1000.0, 20.0, \"Hz\", \"kHz\", \"%5.3f\"" << endl;
  print( np );

  cout << endl;
  cout << "Number-Parameter set to: " << "3400.0, -1.0, \"mHz\"" << endl;
  np.setNumber( 3400.0, -1.0, "mHz" );
  print( np );

  cout << endl;
  cout << "Number-Parameter added: " << "780.0, 92.0" << endl;
  np.addNumber( 780.0, 92.0 );
  print( np );

  cout << endl;
  cout << "Number-Parameter setDefault " << endl;
  np.setDefault();
  print( np );

  Parameter cp( "percentageid", "enter percent", 0.3, -1.0, 0.0, 2.0, 0.01, "%", "1", "%5.1f" );
  cout << endl;
  cout << "Percent-Parameter set to: " << "0.3, 0.0, 0.0, 2.0, 0.01, \"\", \"%\", \"%5.1f\"" << endl;
  print( cp );

  Parameter ip( "integerid", "enter integer", 4L, -1L, 0L, 100L, 2L, "", "", -1 );
  cout << endl;
  print( ip );

  Parameter dp( "dateid", "enter date", Parameter::Date, 2009, 6, 20 );
  cout << endl;
  print( dp );

  Parameter tip( "timeid", "enter time", Parameter::Time, 23, 59, 3 );
  cout << endl;
  print( tip );

  while ( true ) {
    cout << endl;
    Parameter pp;
    cout << "enter a parameter (ident=value): ";
    cin >> pp;
    cout << "   p.type(): " << pp.type() << endl;
    cout << " p.format(): " << pp.format() << endl;
    print( pp );
  }

  return 0;
  /*
  Parameter ip( "integerid", "enter a integer", 2L );
  Parameter bp( "booleanid", "enter a boolean", true );
  Parameter lp( "text", Parameter::Label );
  */
}
