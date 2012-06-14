/*
  base/envelope.cc
  Computes the envelope of a signal

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

#include <relacs/base/envelope.h>
using namespace relacs;

namespace base {


Envelope::Envelope( const string &ident, int mode )
  : Filter( ident, mode, SingleAnalogFilter, 1,
            "Envelope", "base", "Jan Benda", "1.0", "Jun 14, 2012" )
{
  // parameter:
  DeMean = true;
  Rectification = 0;
  Tau = 0.01;
  MeanTau = 1.0;

  // options:
  addLabel( "Envelope filter", 1, OptWidget::LabelBold );
  addBoolean( "demean", "Remove mean", DeMean );
  addNumber( "demeantau", "Time constant for computing mean", MeanTau, 0.0, 10000.0, 0.01, "s", "ms", "%.0f", 2 ).setActivation( "demean", "true" );
  addSelection( "rectification", "Rectification", "truncate|rectify|square" );
  addNumber( "tau", "Time constant for computing envelope", Tau, 0.0, 10000.0, 0.001, "s", "ms", "%.1f", 2 );
  setDialogSelectMask( 2 );

  EFW.assign( ((Options*)this), 0, 0, true, 0, mutex() );
  setWidget( &EFW );
}


int Envelope::init( const InData &indata, InData &outdata )
{
  Index = indata.begin();
  X = 0.0;
  Mean = 0.0;
  DeltaT = indata.sampleInterval();
  TFac = DeltaT/Tau;
  MeanTFac = DeltaT/MeanTau;
  return 0;
}


int Envelope::adjust( const InData &indata, InData &outdata )
{
  outdata.setMinValue( 0.0 );
  outdata.setMaxValue( indata.maxValue() );
  return 0;
}


void Envelope::notify( void )
{
  DeMean = boolean( "demean" );
  Rectification = index( "rectification" );
  double tau = number( "tau" );
  if ( tau > 0.0 ) {
    Tau = tau;
    TFac = DeltaT/Tau;
  }
  else
    setNumber( "tau", Tau );
  double meantau = number( "demeantau" );
  if ( meantau > tau ) {
    MeanTau = meantau;
    MeanTFac = DeltaT/MeanTau;
  }
  else {
    if ( MeanTau <= Tau )
      MeanTau = 5.0*Tau;
    setNumber( "demeantau", MeanTau );
  }
  EFW.updateValues( OptWidget::changedFlag() );
  // XXX updateValues does nothing, since DisableUpdate is set!
  // same in all other filters and detectors!
}


int Envelope::filter( const InData &indata, InData &outdata )
{
  if ( Rectification == 1 ) {
    // rectify:
    for ( ; Index < indata.end(); ++Index ) {
      float x = *Index;
      Mean += MeanTFac * ( x - Mean );
      if ( DeMean )
	x -= Mean;
      if ( x < 0 )
	x = -x;
      X += TFac * ( x - X );
      outdata.push( X );
    }
  }
  else if ( Rectification == 2 ) {
    // square:
    for ( ; Index < indata.end(); ++Index ) {
      float x = *Index;
      Mean += MeanTFac * ( x - Mean );
      if ( DeMean )
	x -= Mean;
      x *= x;
      X += TFac * ( x - X );
      x = ::sqrt( X );
      outdata.push( x );
    }
  }
  else {
    // truncate:
    for ( ; Index < indata.end(); ++Index ) {
      float x = *Index;
      Mean += MeanTFac * ( x - Mean );
      if ( DeMean )
	x -= Mean;
      if ( x < 0 )
	x = 0.0;
      X += TFac * ( x - X );
      outdata.push( X );
    }
  }
  return 0;
}


addFilter( Envelope, base );

}; /* namespace base */

#include "moc_envelope.cc"
