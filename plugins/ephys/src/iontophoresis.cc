/*
  ephys/iontophoresis.cc
  Current injection for iontophoresis

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

#include <QGridLayout>
#include <QTime>
#include <QProcess>
#include <relacs/ephys/iontophoresis.h>
using namespace relacs;

namespace ephys {


Iontophoresis::Iontophoresis( void )
  : RePro( "Iontophoresis", "ephys", "Jan Benda", "1.2", "Oct 9, 2014" ),
    IUnit( "nA" )
{
  // add some options:
  addNumber( "durationpos", "Duration of positive current", 1.0, 0.0, 100000.0, 0.01, "s" );
  addNumber( "amplitudepos", "Amplitude of positive current", 1.0, 0.0, 100000.0, 0.01, "nA" ).setActivation( "durationpos", ">0" );
  addNumber( "pausepos", "Pause after positive current", 1.0, 0.0, 100000.0, 0.01, "s" ).setActivation( "durationpos", ">0" );
  addNumber( "durationneg", "Duration of negative current", 1.0, 0.0, 100000.0, 0.01, "s" );
  addNumber( "amplitudeneg", "Amplitude of negative current", 1.0, 0.0, 100000.0, 0.01, "nA" ).setActivation( "durationneg", ">0" );
  addNumber( "pauseneg", "Pause after negative current", 1.0, 0.0, 100000.0, 0.01, "s" ).setActivation( "durationneg", ">0" );
  addBoolean( "fortunes", "Display fortunes", true );
  addNumber( "fortuneperiod", "Period for displaying fortunes", 10.0, 0.0, 100000.0, 1.0, "s" );

  QGridLayout *gl = new QGridLayout;

  PosTimeLabel = new QLabel( "Positive current injection:" );
  gl->addWidget( PosTimeLabel, 0, 0 );

  PosTime = new QLabel;
  gl->addWidget( PosTime, 0, 1 );

  NegTimeLabel = new QLabel( "Negative current injection:" );
  gl->addWidget( NegTimeLabel, 1, 0 );

  NegTime = new QLabel;
  gl->addWidget( NegTime, 1, 1 );

  FortuneText = new QLabel;
  gl->addWidget( FortuneText, 2, 0, 1, 2 );

  setLayout( gl );
}


void Iontophoresis::preConfig( void )
{
  if ( CurrentOutput[0] >= 0 )
    IUnit = outTrace( CurrentOutput[0] ).unit();
  else
    IUnit = outTrace( 0 ).unit();
  setUnit( "amplitudepos", IUnit );
  setUnit( "amplitudeneg", IUnit );
}


class IontophoresisEvent : public QEvent
{

public:

  IontophoresisEvent( bool pos, bool neg, bool fortunes )
    : QEvent( Type( User+11 ) ),
      PosTime( pos ),
      NegTime( neg ),
      Fortunes( fortunes )
  {
  }
  IontophoresisEvent( double pos, double neg )
    : QEvent( Type( User+12 ) ),
      PosTime( pos ),
      NegTime( neg )
  {
  }
  IontophoresisEvent( const QString &text )
    : QEvent( Type( User+13 ) ),
      Text( text )
  {
  }

  double PosTime;
  double NegTime;
  bool Fortunes;
  QString Text;
};


int Iontophoresis::main( void )
{
  // get options:
  double durationpos = number( "durationpos" );
  double amplitudepos = number( "amplitudepos" );
  double pausepos = number( "pausepos" );
  double durationneg = number( "durationneg" );
  double amplitudeneg = number( "amplitudeneg" );
  double pauseneg = number( "pauseneg" );
  bool fortunes = boolean( "fortunes" );
  double fortuneperiod = number( "fortuneperiod" );
  double pause = 0.0;

  int outtrace = CurrentOutput[0] >= 0 ? CurrentOutput[0] : 0;

  // don't print repro message:
  noMessage();

  double samplerate = trace( SpikeTrace[0] >= 0 ? SpikeTrace[0] : 0 ).sampleRate();

  // dc signal:
  lockStimulusData();
  double dccurrent = stimulusData().number( outTraceName( outtrace ) );
  unlockStimulusData();
  OutData dcsignal;
  dcsignal.setTrace( outtrace );
  dcsignal.constWave( dccurrent );
  dcsignal.setIdent( "DC=" + Str( dccurrent ) + IUnit );

  // signal:
  OutData signal( 0, 1.0/samplerate );
  signal.setTrace( outtrace );
  string msg = "";
  if ( durationpos > 1e-5 ) {
    OutData sig;
    sig.pulseWave( durationpos, 1.0/samplerate, dccurrent + amplitudepos, dccurrent );
    signal.append( sig );
    if ( pausepos > 1e-4 && durationneg > 1e-5 ) {
      sig.constWave( pausepos, 1.0/samplerate, dccurrent );
      signal.append( sig );
    }
    pause = pausepos;
    msg = "Ipos=" +  Str( dccurrent + amplitudepos ) + IUnit;
  }
  if ( durationneg > 1e-5 ) {
    OutData sig;
    sig.pulseWave( durationneg, 1.0/samplerate, dccurrent - amplitudeneg, dccurrent );
    signal.append( sig );
    pause = pauseneg;
    if ( ! msg.empty() )
      msg += ", ";
    msg += "Ineg=" +  Str( dccurrent - amplitudeneg ) + IUnit;
  }
  signal.setIdent( msg );
  message( msg );

  // plot trace:
  tracePlotSignal( signal.duration() + pause );
  QCoreApplication::postEvent( this, new IontophoresisEvent( bool( durationpos > 1e-5 ), bool( durationneg > 1e-5 ), fortunes ) );

  double totalpos = 0.0;
  double totalneg = 0.0;
  QTime calltime;
  calltime.start();

  // write stimulus:
  while ( softStop() == 0 ) {
 
    timeStamp();
    
    write( signal );
    if ( signal.failed() ) {
      warning( signal.errorText() );
      directWrite( dcsignal );
      return Failed;
    }
    if ( interrupt() ) {
      directWrite( dcsignal );
      break;
    }

    // reporting:
    if ( durationpos > 1e-8 )
      totalpos += durationpos;
    if ( durationneg > 1e-8 )
      totalneg += durationneg;
    QCoreApplication::postEvent( this, new IontophoresisEvent( totalpos, totalneg ) );

    sleepOn( signal.duration() + pause );
    if ( interrupt() ) {
      directWrite( dcsignal );
      break;
    }

    // fortune:
    if ( calltime.elapsed() > (int)::rint( 1000.0*fortuneperiod ) ) {
      QProcess p;
      p.start( "fortune" );
      p.waitForFinished( -1 );
      QString text = p.readAllStandardOutput();
      QCoreApplication::postEvent( this, new IontophoresisEvent( text ) );
      calltime.restart();
    }

  }

  return Completed;
}


void Iontophoresis::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 11: {
    IontophoresisEvent *ie = dynamic_cast<IontophoresisEvent*>( qce );
    QFont nf( PosTime->font() );
    nf.setPointSize( 2 * widget()->fontInfo().pointSize() );
    nf.setBold( false );
    PosTimeLabel->setFont( nf );
    NegTimeLabel->setFont( nf );
    nf.setBold( true );
    PosTime->setFont( nf );
    NegTime->setFont( nf );
    if ( ie->PosTime > 1e-5 ) {
      PosTimeLabel->show();
      PosTime->show();
    }
    else {
      PosTimeLabel->hide();
      PosTime->hide();
    }
    if ( ie->NegTime > 1e-5 ) {
      NegTimeLabel->show();
      NegTime->show();
    }
    else {
      NegTimeLabel->hide();
      NegTime->hide();
    }
    PosTime->setText( "00:00" );
    NegTime->setText( "00:00" );
    nf.setPointSize( 1.4 * widget()->fontInfo().pointSize() );
    nf.setBold( false );
    FortuneText->setFont( nf );
    if ( ie->Fortunes )
      FortuneText->show();
    else
      FortuneText->hide();
    break;
  }
  case 12: {
    IontophoresisEvent *ie = dynamic_cast<IontophoresisEvent*>( qce );
    int secs = ie->PosTime;
    int mins = secs / 60;
    secs -= mins * 60;
    string ts = Str( mins, 2, '0' ) + ":";
    ts += Str( secs, 2, '0' );
    PosTime->setText( ts.c_str() );
    secs = ie->NegTime;
    mins = secs / 60;
    secs -= mins * 60;
    ts = Str( mins, 2, '0' ) + ":";
    ts += Str( secs, 2, '0' );
    NegTime->setText( ts.c_str() );
    break;
  }
  case 13: {
    IontophoresisEvent *ie = dynamic_cast<IontophoresisEvent*>( qce );
    FortuneText->setText( ie->Text );
    break;
  }
  default:
    RePro::customEvent( qce );
  }
}


addRePro( Iontophoresis, ephys );

}; /* namespace ephys */

#include "moc_iontophoresis.cc"
