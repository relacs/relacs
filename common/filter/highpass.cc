/*
  highpass.cc
  A simple first order high pass filter

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <relacs/common/highpass.h>


HighPass::HighPass( const string &ident, int mode )
  : Filter( ident, mode, SingleAnalogFilter, 1,
	    "HighPass", "HighPass", "Common",
	    "Jan Benda", "0.1", "Jan 24 2008" ),
    LFW( (QWidget*)this )
{
  // parameter:
  Tau = 0.001;

  // options:
  addNumber( "tau", "Time constant", Tau, 0.0, 10000.0, 0.0001, "s", "ms", "%.1f" );

  LFW.assign( ((Options*)this), 0, 0, true, 0, mutex() );
  LFW.setSpacing( 4 );
  LFW.setMargin( 4 );
}


HighPass::~HighPass( void )
{
}


int HighPass::init( const InData &indata, InData &outdata )
{
  outdata.setMinValue( -100.0 );
  outdata.setMaxValue( 100.0 );
  Index = indata.begin();
  X = 0.0;
  DeltaT = indata.sampleInterval();
  TFac = DeltaT/Tau;
  return 0;
}


void HighPass::notify( void )
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


int HighPass::filter( const InData &indata, InData &outdata )
{
  for ( ; Index < indata.end(); ++Index ) {
    float y = *Index;
    X += TFac * ( y - X );
    outdata.push( y - X );
  }
  return 0;
}


addFilter( HighPass );

#include "moc_highpass.cc"
