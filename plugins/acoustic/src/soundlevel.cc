/*
  acoustic/soundlevel.cc
  Measures sound level in dB SPL

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

#include <QDateTime>
#include <QVBoxLayout>
#include <relacs/acoustic/soundlevel.h>
using namespace relacs;

namespace acoustic {


SoundLevel::SoundLevel( void )
  : Control( "SoundLevel", "acoustic", "Jan Benda", "1.0", "Jul 28, 2016" )
{
  // parameter:
  InTrace = 0;
  Origin = 0;
  Offset = 0.0;
  Duration = 1.0;
  SoundPressureScale = 1.0;

  // options:
  addSelection( "intrace", "Input trace", "V-1" ).setFlags( 8 );
  addSelection( "origin", "Analysis window", "before end of data|before signal|after signal" );
  addNumber( "offset", "Offset of analysis window", Offset, -10000.0, 10000.0, 0.1, "s", "ms" );
  addNumber( "duration", "Width of analysis window", Duration, 0.0, 100.0, 0.1, "s", "ms" );
  addNumber( "scale", "Scale for V/Pa", 1.0, 0.0, 10000.0, 0.05 );

  // layout:
  QVBoxLayout *vb = new QVBoxLayout;
  setLayout( vb );
  vb->setSpacing( 0 );
  IW = new QLabel( "0.0" );
  QFont nf( IW->font() );
  nf.setPointSizeF( 4.0*IW->fontInfo().pointSizeF() );
  nf.setBold( true );
  IW->setFont( nf );
  vb->addWidget( IW, Qt::AlignHCenter );

  QLabel *unit = new QLabel( "dB SPL" );
  nf.setPointSizeF( 0.67*IW->fontInfo().pointSizeF() );
  unit->setFont( nf );
  vb->addWidget( unit, Qt::AlignHCenter );
}


void SoundLevel::preConfig( void )
{
  Parameter &p = *find( "intrace" );
  p.setText( traceNames() );
  p.setToDefault();
  if ( p.size() <= 1 )
    p.addFlags( 16 );
  else
    p.delFlags( 16 );
}


void SoundLevel::notify( void )
{
  InTrace = index( "intrace" );
  Origin = index( "origin" );
  Offset = number( "offset" );
  Duration = number( "duration" );
  SoundPressureScale = number( "scale" );
}


void SoundLevel::main( void )
{
  sleep( Duration );

  do {

    if ( InTrace < 0 || InTrace >= traces().size() ) {
      warning( "Trace does not exist!", 4.0 );
      return;
    }

    int n = trace( InTrace ).indices( Duration );
    int offsinx = 0;
    if ( Origin == 1 )
      offsinx = trace( InTrace ).index( signalTime() - Offset - Duration );
    else if ( Origin == 2 )
      offsinx = trace( InTrace ).index( signalTime() + Offset );
    else
      offsinx = trace( InTrace ).index( currentTime() - Offset - Duration );
    if ( offsinx < trace( InTrace ).minIndex() )
      offsinx = trace( InTrace ).minIndex();
    if ( offsinx + n > trace( InTrace ).currentIndex() )
      n = trace( InTrace ).currentIndex() - offsinx;

    SampleDataD d( n, 0.0, trace( InTrace ).sampleInterval() );
    for ( int k=0; k<d.size(); k++ )
      d[k] = trace( InTrace )[ offsinx+k ];
    d -= mean( d );

    // analyze signal amplitude:
    double p = stdev( d );
    double intensity = 20.0 * log10( p / 2.0e-5 / SoundPressureScale );

    // show result:
    IW->setText( Str( intensity, "%.1f" ).c_str() );

    if ( Origin > 0 )
      waitOnReProSleep();
    else {
      QTime time;
      time.start();
      do {
	waitOnData();
      } while ( 0.001*time.elapsed() < Duration && ! interrupt() );
    }

  } while ( ! interrupt() );
}


addControl( SoundLevel, acoustic );

}; /* namespace acoustic */

#include "moc_soundlevel.cc"
