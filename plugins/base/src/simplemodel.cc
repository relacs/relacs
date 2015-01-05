/*
  base/simplemodel.cc
  A toy model for testing.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

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

#include <relacs/random.h>
#include <relacs/base/simplemodel.h>
using namespace relacs;

namespace base {


SimpleModel::SimpleModel( void )
  : Model( "SimpleModel", "base", "Jan Benda", "1.2", "Oct 10, 2013" )
{
  // define options:
  addNumber( "stimulusgain", "Gain of stimulus", 1.0, 0.0, 100000.0, 1.0, "", "", "%.2f" );
  addNumber( "noisegain", "Amplitude of white noise", 0.0, 0.0, 100000.0, 1.0, "", "", "%.2f" );
  addNumber( "sinegain", "Amplitude of sine wave", 0.0, 0.0, 100000.0, 1.0, "", "", "%.2f" );
  addNumber( "sinefreq", "Frequency of sine wave", 1000.0, 0.0, 10000000.0, 10.0, "Hz", "Hz", "%.1f" ).setActivation( "sinegain", "0.0", false );
}


SimpleModel::~SimpleModel( void )
{
}


void SimpleModel::preConfig( void )
{
  if ( traces() > 0 ) {
    setUnit( "stimulusgain", trace( 0 ).unit() );
    setUnit( "noisegain", trace( 0 ).unit() );
    setUnit( "sinegain", trace( 0 ).unit() );
  }
}


void SimpleModel::main( void )
{
  // read out options:
  double stimulusgain = number( "stimulusgain" );
  double noisegain = number( "noisegain" );
  double sinegain = number( "sinegain" );
  double sinefreq = number( "sinefreq" );

  // integrate:
  while ( ! interrupt() ) {
    double v = 0.0;
    v += stimulusgain * signal( time( 0 ) );
    v += noisegain * rnd.gaussian();
    v += sinegain * ::sin( 6.28318530717959*sinefreq*time( 0 ) );
    for ( int k=0; k<traces(); k++ ) {
      if ( trace( k ).source() == 0 )
	push( k, v );
    }
  }
}


addModel( SimpleModel, base );

}; /* namespace base */
