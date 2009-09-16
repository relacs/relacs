/*
  base/lowpass.cc
  A simple first order low pass filter

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

#include <relacs/base/lowpass.h>
using namespace relacs;

namespace base {


LowPass::LowPass( const string &ident, int mode )
  : Filter( ident, mode, SingleAnalogFilter, 1,
	    "LowPass", "LowPass", "Base",
	    "Jan Benda", "0.1", "May 3 2007" ),
    LFW( (QWidget*)this )
{
  // parameter:
  Tau = 1.0;

  // options:
  addNumber( "tau", "Filter time constant", Tau, 0.0, 10000.0, 0.001, "s", "ms", "%.1f" );

  LFW.assign( ((Options*)this), 0, 0, true, 0, mutex() );
  LFW.setVerticalSpacing( 4 );
  LFW.setMargin( 4 );
  boxLayout()->addWidget( &LFW );
}


LowPass::~LowPass( void )
{
}


int LowPass::init( const InData &indata, InData &outdata )
{
  outdata.setMinValue( -100.0 );
  outdata.setMaxValue( 100.0 );
  Index = indata.begin();
  X = 0.0;
  DeltaT = indata.sampleInterval();
  TFac = DeltaT/Tau;
  return 0;
}


void LowPass::notify( void )
{
  double tau = number( "tau" );
  if ( tau > 0.0 ) {
    Tau = tau;
    TFac = DeltaT/Tau;
  }
  else
    setNumber( "tau", Tau );
  LFW.updateValues( OptWidget::changedFlag() );
}


int LowPass::filter( const InData &indata, InData &outdata )
{
  for ( ; Index < indata.end(); ++Index ) {
    X += TFac * ( *Index - X );
    outdata.push( X );
  }
  return 0;
}


addFilter( LowPass );

}; /* namespace base */

#include "moc_lowpass.cc"
