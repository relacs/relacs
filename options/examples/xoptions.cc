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

  opt.setName( "Test" );
  opt.addText( "comment", "this is a special stimulus" );
  opt.addNumber( "duration", "Duration of Signal",
		 0.2, 0.01, 1000.0, 0.01, "seconds", "ms" );
  opt.addInteger( "repeats", "Repeats", 10 );
  opt.addBoolean( "sinewave", "Use Sine Wave", true );
  opt.addDate( "recordingdate", 2009, 6, 20 );
  opt.addTime( "recordingtime", 16, 42, 13 );
  opt.insertNumber( "pause", "repeats", "Pause between Signals",
		    0.1, 0.01, 1000.0, 0.01, "seconds", "ms", "%5.1f" );
  opt.newSection( "Analysis", "analysis/basics" );
  opt.addNumber( "win", "Length of analysis window",
		 0.1, 0.01, 1000.0, 0.01, "seconds", "ms" );
  opt.pushNumber( "win", 0.23 );
  opt.pushNumber( "win", 15.0, "ms" );
  opt.addBoolean( "type", "Full analysis", true );
  opt.addText( "Nasty, a = |x|>1", "some special value" );
  opt.addText( "Nasty value", "a, b [c], {d}" );
  opt.newSection( "Results" );
  opt.addInteger( "numres", "Number of results reported", 7 );
  opt.newSubSection( "Color", "color/rgb" );
  opt.addSelection( "background", "Background color", "red|green|blue" );
  opt.addText( "foreground", "Foreground color", "" );
  opt.newSubSection( "Animal" );
  opt.addSelection( "mammal", "Mammal", "Monkey|~|Elephant" );
  opt.addSelection( "fish", "Fish", "Trout|Pike|Carp" );
  opt.newSection( "Files", "analysis/files" );
  opt.addText( "master", "Master file", "main.dat" );
  opt.addText( "data", "Data file", "data.dat" );

  cout << opt.save() << '\n';
  cout << '\n';
  opt.saveXML( cout );
  cout << '\n';
  opt.save( cout, "", 0, true );
  cout << '\n';

  cout << "Value of parameter 'comment': " << opt.text( "comment|repeats" ) << '\n';
  cout << "Value of parameter 'Pause|pause': " << opt.number( "Pause|pause" ) << '\n';
  cout << "Value of parameter 'Test>pause': " << opt.number( "Test>pause" ) << '\n';
  cout << "Value of parameter 'repeats': " << opt.integer( "repeats|" ) << '\n';
  cout << "Value of parameter 'sinewave': " << opt.boolean( "||sinewave" ) << '\n';
  cout << "Value of parameter 'recordingtime': " << opt.text( "rec|Recording>time|recordingtime" ) << '\n';
  cout << "Value of parameter 'type': " << opt.boolean( "Analysis>type" ) << '\n';
  cout << "Value of parameter 'Nasty, a = |x|<1': " << opt.text( "Nasty, a = |x|>1" ) << '\n';
  cout << "Value of parameter 'Analysis>numres': " << opt.integer( "Analysis>numres" ) << '\n';
  cout << "Value of parameter 'Analysis>win, 0': " << opt.number( "Analysis>win", 0 ) << '\n';
  cout << "Value of parameter 'Analysis>win, 1': " << opt.number( "Analysis>win", 1 ) << '\n';
  cout << "Value of parameter 'Analysis>win, 2': " << opt.number( "Analysis>win", 2 ) << '\n';
  cout << "Value of parameter 'Results>numres': " << opt.integer( "Results>numres" ) << '\n';
  cout << "Value of parameter 'Results>Color>background': " << opt.text( "Results>Color>background" ) << '\n';
  cout << "Value of parameter 'Results>mammal 0': *" << opt.text( "Results>mammal", 0 ) << "*\n";
  cout << "Value of parameter 'Results>mammal 1': *" << opt.text( "Results>mammal", 1 ) << "*\n";
  cout << "Value of parameter 'Results>mammal 2': *" << opt.text( "Results>mammal", 2 ) << "*\n";
  cout << "Value of parameter 'Files>data': *" << opt.text( "Files>data" ) << "*\n";
  cout << '\n';

  // set values in opt2:
  Options opt2 = opt;
  opt2.setText( "comment", "no comment" );
  opt2.setText( "foreground", "pink" );
  opt2.setBoolean( "type", false );
  opt2.setNumber( "duration", 0.31 );
  opt2.setNumber( "Analysis>win", 0.42 );
  opt2.setText( "Nasty, a = |x|>1", "some other value" );
  opt2.setText( "Nasty value", "d, [c] {b}, a" );
  opt2.setInteger( "Results>numres", 23 );
  opt2.selectText( "Animal>mammal", "Elephant" );
  opt2.setText( "Files>master", "maindata.dat" );

  // read in values from string:
  cout << "read in values from string:\n";
  string os = opt2.save();
  cout << os << '\n';
  opt.read( os, ":" );
  opt.save( cout, "", 0, true );
  cout << '\n';

  // set default:
  cout << "set default:\n";
  opt.setDefaults();
  opt.save( cout, "", 0, true );
  cout << '\n';

  // read in values from file:
  cout << "read in values from file:\n";
  opt2.save( cout, "", 0, true );
  cout << '\n';
  {
    ofstream ff( "tmp.dat" );
    ff << "# This is the current yaml style:\n";
    opt2.save( ff, "", 0, false, true );
  }
  {
    ifstream sf( "tmp.dat" );
    opt.read( sf, ":" );
  }
  opt.save( cout, "", 0, true );
  cout << '\n';

  cout << "read in values from file without indentation:\n";
  opt.setDefaults();
  {
    ifstream sf( "read.dat" );
    opt.read( sf, ":" );
  }
  opt.save( cout, "", 0, true );
  cout << '\n';

  cout << "Value of parameter 'Nasty value': " << opt.text( "Nasty value" ) << '\n';
  cout << '\n';

  // read in values from options:
  cout << "read in values from options:\n";
  opt.setDefaults();
  opt.read( opt2 );
  opt.save( cout, "", 0, true );
  cout << '\n';

  // load options from file:
  cout << "load options from file:\n";
  Options opt3;
  {
    ifstream sf( "tmp.dat" );
    string s;
    opt3.load( sf );
  }
  opt3.save( cout, "", 0, true );
  cout << '\n';

  // load options from file without indentation:
  cout << "load options from file without indentation:\n";
  Options opt4;
  {
    ifstream sf( "read.dat" );
    string s;
    opt4.load( sf );
  }
  opt4.save( cout, "", 0, true );
  cout << '\n';

  // load options from string:
  cout << "load options from string:\n";
  Options opt5;
  opt5.load( os );
  opt5.save( cout, "", 0, true );
  cout << '\n';

  return 0;

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
