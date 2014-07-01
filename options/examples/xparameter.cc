/*
  xparameter.cc
  

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
#include <relacs/parameter.h>
using namespace relacs;


void print( const Parameter &p )
{
  cout << "p.text()     : " << p.text( 0 );
  for ( int k=1; k<p.size(); k++ )
    cout << "|" << p.text( k );
  cout << '\n';

  if ( p.isAnyNumber() ) {
    cout << "p.number()   : " << p.number( 0 );
    for ( int k=1; k<p.size(); k++ )
      cout << "|" << p.number( k );
    cout << '\n';

    cout << "p.error()   : " << p.error( 0 );
    for ( int k=1; k<p.size(); k++ )
      cout << "|" << p.error( k );
    cout << '\n';
  }

  cout << "p.unit()     : " << p.unit() << '\n';
  cout << "p.outUnit()  : " << p.outUnit() << '\n';
  cout << "p.format()   : " << p.format() << '\n';
  cout << "p.valueType(): " << p.valueType() << '\n';
  cout << "p.text()     : " << p.text( "r='%r', i=%i, s=%s, n=%g, e=%G, u=%u, date=%04Y-%02m-%02d, time=%02H:%02M:%02S.%03U" ) << '\n';
  cout << "p            : ";
  p.save( cout, 0 );
  cout << '\n';
}


int main( int argc, char *argv[] )
{
  Parameter tp( "textid", "enter a text", "char-text" );

  cout << '\n';
  cout << "Text-Parameter set to: " << "char_text" << '\n';
  print( tp );

  cout << '\n';
  cout << "Text-Parameter set to: " << "(2.0+-0.3)Hz" << '\n';
  tp.setText( "(2.0+-0.3)Hz" );
  print( tp );

  cout << '\n';
  cout << "Text-Parameter set to: " << "aa|bb||cc" << '\n';
  tp.setText( "aa|bb||cc" );
  print( tp );

  cout << '\n';
  cout << "Text-Parameter set to: " << "2|0.4kHz|(43+-5)" << '\n';
  tp.setText( "2|0.4kHz|(43+-5)" );
  print( tp );

  cout << '\n';
  cout << "Text-Parameter setDefault()" << '\n';
  tp.setDefault();
  print( tp );


  Parameter np( "numberid", "enter a number", 234.0, 56.0, 0.0, 1000.0, 20.0, "Hz", "kHz", "%5.3f" );

  cout << '\n';
  cout << "Number-Parameter set to: " << "234.0, 56.0, 0.0, 1000.0, 20.0, \"Hz\", \"kHz\", \"%5.3f\"" << '\n';
  print( np );

  cout << '\n';
  cout << "Number-Parameter set to: " << "3400.0, -1.0, \"mHz\"" << '\n';
  np.setNumber( 3400.0, -1.0, "mHz" );
  print( np );

  cout << '\n';
  cout << "Number-Parameter added: " << "780.0, 92.0" << '\n';
  np.addNumber( 780.0, 92.0 );
  print( np );

  cout << '\n';
  cout << "Number-Parameter setDefault " << '\n';
  np.setDefault();
  print( np );

  Parameter cp( "percentageid", "enter percent", 0.3, -1.0, 0.0, 2.0, 0.01, "%", "1", "%5.1f" );
  cout << '\n';
  cout << "Percent-Parameter set to: " << "0.3, 0.0, 0.0, 2.0, 0.01, \"\", \"%\", \"%5.1f\"" << '\n';
  print( cp );

  Parameter ip( "integerid", "enter integer", 4L, -1L, 0L, 100L, 2L, "", "", -1 );
  cout << '\n';
  print( ip );

  Parameter dp( "dateid", "enter date", Parameter::Date, 2009, 6, 20 );
  cout << '\n';
  print( dp );

  Parameter tip( "timeid", "enter time", Parameter::Time, 23, 59, 3 );
  cout << '\n';
  print( tip );

  Parameter tip2( "microtimeid", "enter time", 23, 59, 3, 497 );
  cout << '\n';
  print( tip2 );

  while ( true ) {
    cout << '\n';
    cout << "enter a parameter (ident=value): ";
    Str s;
    cin >> s;
    Parameter pp( s.ident(), s.value() );
    cout << "p.valueType(): " << pp.valueType() << '\n';
    cout << "p.format()   : " << pp.format() << '\n';
    print( pp );
  }

  return 0;
  /*
  Parameter ip( "integerid", "enter a integer", 2L );
  Parameter bp( "booleanid", "enter a boolean", true );
  */
}
