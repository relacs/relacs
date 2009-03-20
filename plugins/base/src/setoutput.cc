/*
  base/setoutput.cc
  Set an output to a specified value

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

#include <relacs/base/setoutput.h>
using namespace relacs;

namespace base {


SetOutput::SetOutput( void )
  : RePro( "SetOutput", "SetOutput", "Base",
	   "Jan Benda", "0.0", "Mar 20, 2009" )
{
  // add some options:
  addSelection( "outtrace", "Output trace", "V-1" );
  addNumber( "value", "Value to be writen to output trace", 0.0, -100000.0, 100000.0, 0.1 );
}


void SetOutput::config( void )
{
  string ots = "";
  for ( int k=0; k<outTracesSize(); k++ ) {
    if ( k > 0 )
      ots += '|';
    ots += outTraceName( k );
  }
  setText( "outtrace", ots );
  setToDefault( "outtrace" );
}


void SetOutput::notify( void )
{
  int outtrace = index( "outtrace" );
  setUnit( "value", outTrace( outtrace ).unit() );
}


int SetOutput::main( void )
{
  // get options:
  int outtrace = index( "outtrace" );
  double value = number( "value" );

  double sample = trace( 0 ).sampleInterval();

  OutData signal( 10, sample );
  signal = value;
  signal.setIdent( "const" );
  signal.setTrace( outtrace );

  write( signal );
  if ( signal.failed() ) {
    warning( signal.errorText() );
    return Failed;
  }
  sleep( 10.0*signal.length() );
  return Completed;
}


addRePro( SetOutput );

}; /* namespace base */

#include "moc_setoutput.cc"

