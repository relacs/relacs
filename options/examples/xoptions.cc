/*
  xoptions.cc
  

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

#include <cstdlib>
#include <iostream>
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
  cout << opt;

  cout << endl;
  cout << "enter some options (ident1=value1; ident2=value2; ...): ";
  getline( cin, s );
  opt.load( s );
  opt.save( cout, "", -1, false );

  while ( true ) {
    cout << endl;
    cout << "enter some options that should be read by the first options: ";
    getline( cin, s );
    opt.read( s );
    cout << opt;
  }

  return 0;
}
