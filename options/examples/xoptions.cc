/*
  xoptions.cc
  

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
#include <relacs/options.h>
using namespace relacs;


int main( int argc, char *argv[] )
{
  Options opt;
  string s;

  opt.addText( "comment", "this is a special stimulus" );
  opt.addNumber( "duration", "Duration of Signal",
		 0.2, 0.01, 1000.0, 0.01, "seconds", "ms" );
  opt.addInteger( "repeats", "Repeats", 10 );
  opt.addBoolean( "sinewave", "Use Sine Wave", true );
  opt.addDate( "recordingdate", 2009, 6, 20 );
  opt.addTime( "recordingtime", 16, 42, 13 );
  opt.insertNumber( "pause", "repeats", "Pause between Signals",
		    0.1, 0.01, 1000.0, 0.01, "seconds", "ms", "%5.1f" );
  opt.addSection( "Analysis", "analysis/basics" );
  opt.addNumber( "win", "Length of analysis window",
		 0.1, 0.01, 1000.0, 0.01, "seconds", "ms" );
  opt.addBoolean( "type", "Full analysis", true );
  opt.addText( "Nasty |x|>1", "some special value" );
  opt.addSection( "Results" );
  opt.addInteger( "numres", "Number of results reported", 7 );
  opt.addSubSection( "Color", "color/rgb" );
  opt.addSelection( "background", "Background color", "red|green|blue" );
  opt.addText( "foreground", "Foreground color", "" );
  opt.addSubSection( "Animal" );
  opt.addSelection( "mammal", "Mammal", "Monkey|~|Elephant" );
  opt.addSelection( "fish", "Fish", "Trout|Pike|Carp" );

  string os = opt.save();
  cout << os << '\n';
  cout << '\n';
  opt.saveXML( cout );
  cout << '\n';
  opt.save( cout, "", 0, true );
  cout << '\n';

  cout << "Value of parameter 'comment': " << opt.text( "comment|repeats" ) << '\n';
  cout << "Value of parameter 'pause': " << opt.number( "Pause|pause" ) << '\n';
  cout << "Value of parameter 'repeats': " << opt.integer( "repeats|" ) << '\n';
  cout << "Value of parameter 'sinewave': " << opt.boolean( "||sinewave" ) << '\n';
  cout << "Value of parameter 'recordingtime': " << opt.text( "rec|Recording>time|recordingtime" ) << '\n';
  cout << "Value of parameter 'type': " << opt.boolean( "Analysis>type" ) << '\n';
  cout << "Value of parameter 'Nasty |x|<1': " << opt.text( "Nasty |x|>1" ) << '\n';
  cout << "Value of parameter 'Analysis>numres': " << opt.integer( "Analysis>numres" ) << '\n';
  cout << "Value of parameter 'Results>numres': " << opt.integer( "Results>numres" ) << '\n';
  cout << "Value of parameter 'Results>Color>background': " << opt.text( "Results>Color>background" ) << '\n';
  cout << "Value of parameter 'Results>mammal 0': *" << opt.text( "Results>mammal", 0 ) << "*\n";
  cout << "Value of parameter 'Results>mammal 1': *" << opt.text( "Results>mammal", 1 ) << "*\n";
  cout << "Value of parameter 'Results>mammal 2': *" << opt.text( "Results>mammal", 2 ) << "*\n";
  cout << '\n';

  // read in values from string:
  cout << os << '\n';
  opt.read( os );
  opt.save( cout, "", 0, true );
  cout << '\n';

  return 0;

  ofstream ff( "tmp.dat" );
  opt.save( ff, "", 0, true );
  ff.close();

  Options opt2;
  ifstream sf( "tmp.dat" );
  opt2.load( sf );
  opt2.save( cout, "", 0, true );
  cout << '\n';

  cout << "enter some options (ident1=value1; ident2=value2; ...): ";
  getline( cin, s );
  opt.load( s );
  opt.save( cout, "", -1, false );

  while ( true ) {
    cout << '\n';
    cout << "enter some options that should be read by the first options: ";
    getline( cin, s );
    opt.read( s );
    cout << opt;
  }

  return 0;
}
