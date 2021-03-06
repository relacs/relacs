/*
  efield/linearfield.cc
  Measure the electric field manually with a single electrode in one direction

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

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <relacs/tablekey.h>
#include <relacs/efield/linearfield.h>
using namespace relacs;

namespace efield {


LinearField::LinearField( void )
  : RePro( "LinearField", "efield", "Jan Benda", "1.0", "Apr 23, 2009" )
{
  // add some options:
  addNumber( "duration", "Duration of measurement", 1.0, 0.01, 1000000.0, 1.0, "s" );

  // layout:
  QHBoxLayout *hb = new QHBoxLayout;
  setLayout( hb );
  QVBoxLayout *vb = new QVBoxLayout;
  hb->addLayout( vb );

  // user interaction:
  GUIOpts.addText( "comment", "Comment", "" );
  GUIOpts.addNumber( "distance", "Distance", 0.0, -100000.0, 100000.0, 1.0, "cm" );
  O.assign( &GUIOpts, 0, 0, false, OptWidget::BreakLinesStyle + OptWidget::ExtraSpaceStyle );
  O.setVerticalSpacing( 2 );
  vb->addWidget( &O );

  // measure button:
  MeasureButton = new QPushButton( "&Measure" );
  connect( MeasureButton, SIGNAL( clicked() ),
	   this, SLOT( measure() ) );
  grabKey( Qt::ALT+Qt::Key_M );
  grabKey( Qt::Key_Return );
  grabKey( Qt::Key_Enter );
  vb->addWidget( MeasureButton );

  // finish button:
  FinishButton = new QPushButton( "&Finish" );
  connect( FinishButton, SIGNAL( clicked() ),
	   this, SLOT( finish() ) );
  grabKey( Qt::ALT+Qt::Key_F );
  grabKey( Qt::Key_Escape );
  vb->addWidget( FinishButton );

  // setting the right tab order (seems not really to be needed!)
  /*
  if ( O.lastWidget() != 0 )
    setTabOrder( O.lastWidget(), MeasureButton );
  setTabOrder( MeasureButton, FinishButton );
  */

  // plot:
  hb->addWidget( &P );

  Input = false;
}


void LinearField::measure( void )
{
  lock();
  if ( Input ) {
    Measure = true;
    O.accept( false );
    wake();
  }
  unlock();
}


void LinearField::finish( void )
{
  lock();
  if ( Input ) {
    Measure = false;
    wake();
  }
  unlock();
}


int LinearField::main( void )
{
  // get options:
  double duration = number( "duration" );

  // init:
  noMessage();
  keepFocus();

  Amplitude.clear();
  Amplitude.reserve( 1000 );

  // plot:
  P.lock();
  P.setXLabel( "Distance [cm]" );
  P.setYLabel( "RMS Amplitude [" + trace( 0 ).unit() + "]" );
  P.setYRange( 0.0, Plot::AutoScale );
  P.unlock();
  plot();

  postCustomEvent( 11 ); // O.setFocus();
  do {
    // wait for input:
    Measure = false;
    Input = true;
    sleepWait();
    Input = false;
    if ( Measure ) {
      double distance = GUIOpts.number( "distance" );
      message( "measure at " + Str( distance ) + "cm" );
      // get data:
      sleep( duration+0.1 );
      SampleDataF data( 0.0, duration, trace( 0 ).sampleInterval() );
      trace( 0 ).copy( currentTime()-duration-0.1, data );
      // analyse:
      analyze( distance, data );
      // plot:
      plot();
      // save:
      saveTrace( data );
    }
  } while ( Measure && ! interrupt() );
  saveAmplitude();
  postCustomEvent( 12 ); // clearFocus();
  return Completed;
}


void LinearField::analyze( double distance, const SampleDataF &data )
{
  double ampl = variance( data );
  Amplitude.push( distance, ampl );
}


void LinearField::plot( void )
{
  P.lock();
  P.clear();
  P.plot( Amplitude, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 10, Plot::Yellow, Plot::Yellow );
  if ( Amplitude.size() > 0 ) {
    MapD p;
    p.push( Amplitude.x().back(), Amplitude.y().back() );
    P.plot( p, 1.0, Plot::Transparent, 0, Plot::Solid, Plot::Circle, 10, Plot::Red, Plot::Yellow );
  }
  P.draw();
  P.unlock();
}


void LinearField::saveTrace( const SampleDataF &data )
{
  // create file:
  ofstream df( addPath( "linearfield-traces.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  settings().save( df, "# ", 0, Options::FirstOnly );
  GUIOpts.save( df, "# " );
  df << '\n';
  TableKey key;
  key.addNumber( "t", "ms", "%6.2f" );
  key.addNumber( "V", trace( 0 ).unit(), "%6.2f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<data.size(); k++ ) {
    key. save( df, data.pos( k )*1000.0, 0 );
    key. save( df, data[k] );
    df << '\n';
  }
  df << "\n\n";
}


void LinearField::saveAmplitude( void )
{
  if ( Amplitude.empty() )
    return;

  // create file:
  ofstream df( addPath( "linearfield.dat" ).c_str(),
	       ofstream::out | ofstream::app );
  if ( ! df.good() )
    return;

  // write header and key:
  settings().save( df, "# ", 0, Options::FirstOnly );
  df << '\n';
  TableKey key;
  key.addNumber( "x", "cm", "%6.1f" );
  key.addNumber( "A", trace( 0 ).unit(), "%6.2f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<Amplitude.size(); k++ ) {
    key. save( df, Amplitude.x( k ), 0 );
    key. save( df, Amplitude.y( k ) );
    df << '\n';
  }
  df << "\n\n";
}


void LinearField::keyPressEvent( QKeyEvent *e )
{
  e->ignore();
  if ( e->key() == Qt::Key_M && ( e->modifiers() & Qt::AltModifier ) ) {
    MeasureButton->animateClick();
    e->accept();
  }
  else if ( e->key() == Qt::Key_F && ( e->modifiers() & Qt::AltModifier ) ) {
    FinishButton->animateClick();
    e->accept();
  }
  else if ( ( e->key() == Qt::Key_Return || e->key() == Qt::Key_Enter ) && ( e->modifiers() == Qt::NoModifier ) ) {
    MeasureButton->animateClick();
    e->accept();
  }
  else if ( e->key() == Qt::Key_Escape && ( e->modifiers() == Qt::NoModifier ) ) {
    FinishButton->animateClick();
    e->accept();
  }
  else
    RePro::keyPressEvent( e );
}


void LinearField::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 11: {
    if ( O.firstWidget() != 0 )
      O.firstWidget()->setFocus( Qt::TabFocusReason );
    break;
  }
  case 12: {
    removeFocus();
    break;
  }
  default:
    RePro::customEvent( qce );
  }
}


addRePro( LinearField, efield );

}; /* namespace efield */

#include "moc_linearfield.cc"

