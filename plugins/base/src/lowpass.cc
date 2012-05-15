/*
  base/lowpass.cc
  A simple first order low pass filter

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

#include <relacs/base/lowpass.h>
using namespace relacs;

namespace base {


LowPass::LowPass( const string &ident, int mode )
  : Filter( ident, mode, SingleAnalogFilter, 1,
	    "LowPass", "base", "Jan Benda", "0.2", "May 12 2012" )
{
  // parameter:
  Tau = 1.0;

  // options:
  addLabel( "Low-pass filter", 1, OptWidget::LabelBold );
  addNumber( "tau", "Filter time constant", Tau, 0.0, 10000.0, 0.001, "s", "ms", "%.1f", 2 );
  setDialogSelectMask( 2 );

  LFW.assign( ((Options*)this), 0, 0, true, 0, mutex() );
  setWidget( &LFW );
}


LowPass::~LowPass( void )
{
}


int LowPass::init( const InData &indata, InData &outdata )
{
  Index = indata.begin();
  X = 0.0;
  DeltaT = indata.sampleInterval();
  TFac = DeltaT/Tau;
  return 0;
}


int LowPass::adjust( const InData &indata, InData &outdata )
{
  outdata.setMinValue( indata.minValue() );
  outdata.setMaxValue( indata.maxValue() );
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


addFilter( LowPass, base );

}; /* namespace base */

#include "moc_lowpass.cc"
