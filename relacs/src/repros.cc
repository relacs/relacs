/*
  repros.cc
  Container handling RePros

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
#include <QLabel>
#include <QToolTip>
#include <QApplication>
#include <relacs/messagebox.h>
#include <relacs/scaledimage.h>
#include <relacs/optdialog.h>
#include <relacs/relacswidget.h>
#include <relacs/repro.h>
#include <relacs/defaultrepro.h>
#include <relacs/macros.h>
#include <relacs/repros.h>

namespace relacs {


class ReProsEvent : public QEvent
{

public:

  ReProsEvent( const string &message )
    : QEvent( Type( User+1 ) ),
      Message( message )
  {
  }

  string Message;

};


RePros::RePros( RELACSWidget *rw, QWidget *parent )
  : QTabWidget( parent ),
    RW( rw )
{
  CurrentRePro = -1;
  ActionRePro = -1;

  MacroName = "RePro";
  MacroParam = "";

  CurrentView = 0;
  PreviousView = 0;

  DialogOpt.addBoolean( "default", "Set values as default", true, 1 );
  DialogOpt.addBoolean( "overwrite", "Changes overwrite macro options", false, 1 );

  int n = 0;
  for ( int k=0; k<Plugins::plugins(); k++ )
    if ( Plugins::type( k ) & RELACSPlugin::ReProId ) {
      void *mp = Plugins::create( k );
      RePro *rp = (RePro*)mp;
      rp->setRELACSWidget( RW );
      if ( rp->widget() == 0 ) {
	QImage image( rp->headerImageFile().c_str() );
	ScaledImage *si = new ScaledImage( image );
	rp->setWidget( si );
      }
      rp->disable();
      addTab( rp->widget(), rp->name().c_str() );
      ReProData *rd = new ReProData( Plugins::ident( k ), rp, DialogOpt,
				     this, RW );
      RPs.push_back( rd );
      connect( rd, SIGNAL( stopRePro( void ) ),
	       this, SIGNAL( stopRePro( void ) ) );
      connect( rd, SIGNAL( startRePro( RePro*, int, bool ) ),
	       this, SIGNAL( startRePro( RePro*, int, bool ) ) );
      connect( rd, SIGNAL( noMacro( RePro* ) ),
	       this, SIGNAL( noMacro( RePro* ) ) );
      n++;
    }

  if ( n <= 0 ) {
    MessageBox::warning( "RELACS Warning !", "No RePros found!<br>Activating Default RePro.", 4.0, this );
    RePro *rp = new DefaultRePro();
    rp->setRELACSWidget( RW );
    if ( rp->widget() == 0 ) {
      QImage image( rp->headerImageFile().c_str() );
      ScaledImage *si = new ScaledImage( image );
      rp->setWidget( si );
    }
    rp->disable();
    addTab( rp->widget(), rp->name().c_str() );
    ReProData *rd = new ReProData( rp->name(), rp, DialogOpt, this, RW );
    RPs.push_back( rd );
    connect( rd, SIGNAL( stopRePro( void ) ),
	     this, SIGNAL( stopRePro( void ) ) );
    connect( rd, SIGNAL( startRePro( RePro*, int, bool ) ),
	     this, SIGNAL( startRePro( RePro*, int, bool ) ) );
    connect( rd, SIGNAL( noMacro( RePro* ) ),
	     this, SIGNAL( noMacro( RePro* ) ) );
    n++;
  }

  // check for duplicate RePro names:
  for ( unsigned int k=0; k<RPs.size(); k++ ) {
    if ( RPs[k]->repro() != 0 ) {
      for ( unsigned int j=k+1; j<RPs.size(); j++ ) {
	if ( RPs[j]->repro() != 0 &&
	     RPs[k]->repro()->name() == RPs[j]->repro()->name() ) {
	  RPs[j]->repro()->setLongUniqueName();
	  setTabText( indexOf( RPs[j]->repro()->widget() ),
		      RPs[j]->repro()->uniqueName().c_str() );
	}
      }
    }
  }

  setTabPosition( North );         // tab bar on top
  setElideMode( Qt::ElideRight );  // ellipses at the right
  //  setUseScrollButtons( false );
  setElideMode( Qt::ElideNone );   // no ellipses
  setUsesScrollButtons( true );
}


RePros::~RePros( void )
{
  for ( unsigned int k=0; k<RPs.size(); k++ ) {
    if ( RPs[k]->repro() != 0 ) {
      Plugins::destroy( RPs[k]->name(), RELACSPlugin::ReProId );
      delete RPs[k]->repro();
    }
    delete RPs[k];
  }
  RPs.clear();
}


int RePros::currentRePro( void ) const
{
  return CurrentRePro;
}


void RePros::activateRePro( RePro *repro, int macroaction )
{
  if ( repro == 0 ) {
    CurrentRePro = -1;
    ActionRePro = -1;
    return;
  }

  CurrentRePro = index( repro );
  if ( ( macroaction & Macro::ExplicitFallBack ) == 0 || ActionRePro < 0 )
    ActionRePro = CurrentRePro;
  raise( CurrentRePro );
}


void RePros::setMacro( const string &name, const string &param )
{
  MacroName = name;
  MacroParam = param;
}


string RePros::macroName( void ) const
{
  return MacroName;
}


string RePros::macroParam( void ) const
{
  return MacroParam;
}


QLabel *RePros::display( QWidget *parent )
{
  Message = new QLabel( "Xg", parent );
  Message->setFixedHeight( 2*Message->sizeHint().height() );
  Message->setTextFormat( Qt::AutoText );
  Message->setIndent( 14 );
  Message->setText( "No Cell" );
  Message->setToolTip( "Infos from the current research program" );
  return Message;
}


void RePros::message( const string &msg )
{
  Str s = msg;
  s.eraseMarkup();
  RW->printlog( ( CurrentRePro >= 0 ? RPs[CurrentRePro]->repro()->uniqueName() + ": " : "" ) + s );
  QApplication::postEvent( this, new ReProsEvent( msg ) );
}


void RePros::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {
  case 1: {
    ReProsEvent *re = dynamic_cast<ReProsEvent*>( qce );
    Message->setText( re->Message.c_str() );
    break;
  }
  default:
    QWidget::customEvent( qce );
  }
}


void RePros::addMenu( QMenu *menu, bool doxydoc )
{
  menu->addAction( "&Options...", this, SLOT( dialog() ), Qt::CTRL + Qt::Key_O );
  menu->addAction( "&View", this, SLOT( raise() ), Qt::CTRL + Qt::Key_V );
  menu->addAction( "&Help...", this, SLOT( help() ), Qt::CTRL + Qt::Key_H );
  menu->addAction( "&Screenshot", this, SLOT( screenshot() ), Qt::CTRL + Qt::Key_S );
  menu->addSeparator();
  for ( unsigned int k=0; k<RPs.size(); k++ )
    RPs[k]->addMenu( menu, k, doxydoc );
}


void RePros::assignTracesEvents( const InList &il, const EventList &el )
{
  for ( unsigned int k=0; k<RPs.size(); k++ )
    RPs[k]->repro()->assignTracesEvents( il, el );
}


void RePros::assignTracesEvents( void )
{
  for ( unsigned int k=0; k<RPs.size(); k++ )
    RPs[k]->repro()->assignTracesEvents();
}


void RePros::modeChanged( void )
{
  for ( unsigned int k=0; k<RPs.size(); k++ )
    RPs[k]->repro()->modeChanged();
}


void RePros::help( void )
{
  if ( ActionRePro < 0 || ActionRePro >= (int)RPs.size() )
    return;
  RPs[ActionRePro]->help();
}


void RePros::screenshot( void )
{
  if ( ActionRePro < 0 || ActionRePro >= (int)RPs.size() )
    return;
  RPs[ActionRePro]->repro()->saveWidget();
}


void RePros::dialog( void )
{
  if ( ActionRePro < 0 || ActionRePro >= (int)RPs.size() )
    return;
  RPs[ActionRePro]->dialog();
}


void RePros::notifyStimulusData( void )
{
  for ( unsigned int k=0; k<RPs.size(); k++ )
    RPs[k]->repro()->notifyStimulusData();
}


void RePros::notifyMetaData( void )
{
  for ( unsigned int k=0; k<RPs.size(); k++ )
    RPs[k]->repro()->notifyMetaData();
}


void RePros::setSettings( void )
{
  for ( unsigned int k=0; k<RPs.size(); k++ )
    RPs[k]->repro()->setSettings();
}


void RePros::sessionStarted( void )
{
  for ( unsigned int k=0; k<RPs.size(); k++ )
    RPs[k]->repro()->sessionStarted();
}


void RePros::sessionStopped( bool saved )
{
  for ( unsigned int k=0; k<RPs.size(); k++ )
    RPs[k]->repro()->sessionStopped( saved );
}


int RePros::index( const string &name ) const
{
  if ( name.empty() )
    return -1;

  Str id = name;
  id.lower();

  for ( unsigned int k=0; k<RPs.size(); k++ ) {
    Str idr = RPs[k]->name();
    idr.lower();
    if ( idr == id )
      return k;
  }

  return -1;
}


int RePros::index( const RePro *repro ) const
{
  for ( unsigned int k=0; k<RPs.size(); k++ )
    if ( RPs[k]->repro() == repro )
      return k;

  return -1;
}


RePro *RePros::repro( int index ) const
{
  if ( index >= 0 && index < (int)RPs.size() )
    return RPs[index]->repro();
  return 0;
}


RePro *RePros::repro( const string &name ) const
{
  return repro( index( name ) );
}


int RePros::nameIndex( const string &name ) const
{
  if ( name.empty() )
    return -1;

  Str id = name;
  id.lower();

  for ( unsigned int k=0; k<RPs.size(); k++ ) {
    Str idr = RPs[k]->repro()->uniqueName();
    idr.lower();
    if ( idr == id )
      return k;
  }
  int n = id.find( '[' );
  if ( n >= 0 )
    id.resize( n );
  for ( unsigned int k=0; k<RPs.size(); k++ ) {
    Str idr = RPs[k]->repro()->name();
    idr.lower();
    if ( idr == id )
      return k;
  }

  return -1;
}


RePro *RePros::nameRepro( const string &name ) const
{
  return repro( nameIndex( name ) );
}


void RePros::raise( int index )
{
  if ( index >= 0 )
    raise( RPs[index]->repro() );
}


void RePros::raise( RePro *repro )
{
  if ( repro != CurrentView )
    PreviousView = CurrentView;
  CurrentView = repro;
  removeTab( indexOf( repro->widget() ) );
  insertTab( 0, repro->widget(), repro->uniqueName().c_str() );
  setCurrentWidget( repro->widget() );
}


void RePros::raise( void )
{
  raise( PreviousView );
}


void RePros::reload( RePro *repro )
{
  RPs[ index( repro ) ]->reload();
}


void RePros::help( RePro *repro )
{
  RPs[ index( repro ) ]->help();
}


Options &RePros::dialogOptions( void )
{
  return DialogOpt;
}


ostream &operator<<( ostream &str, const RePros &repros )
{
  for ( unsigned int k=0; k < repros.RPs.size(); k++ ) {
    str << "RePro " << k << ": " << repros.RPs[k]->name() << '\n';
  }
  return str;
}


ReProData::ReProData( const string &name, RePro *repro, Options &dopt,
		      RePros *rps, RELACSWidget *rw )
{
  Name = name;
  RP = repro;
  CO.clear();
  DO = &dopt;
  RPs = rps;
  RW = rw;
  connect( this, SIGNAL( reloadRePro( const string& ) ),
	   RPs, SIGNAL( reloadRePro( const string& ) ) );
}


void ReProData::addMenu( QMenu *menu, int inx, bool doxydoc )
{
  string mt = "&";
  if ( inx == 0 )
    mt += '0';
  else if ( inx < 10 )
    mt += ( '1' + inx - 1 );
  else
    mt += ( 'a' + inx - 10 );
  mt += " ";
  mt += RP->uniqueName();

  QMenu *pop = menu->addMenu( mt.c_str() );

  pop->addAction( "&Run", this, SLOT( start() ) );
  pop->addAction( "&Options...", this, SLOT( dialog() ) );
  pop->addAction( "&View", this, SLOT( raise() ) );
  pop->addAction( "&Load", this, SLOT( reload() ) );
  pop->addAction( "&Help...", this, SLOT( help() ) );
  pop->addAction( "&Screenshot", RP, SLOT( saveWidget() ) );
  if ( doxydoc )
    pop->addAction( "&Doxygen", RP, SLOT( saveDoxygenOptions() ) );
}


string ReProData::name( void ) const
{
  return Name;
}


RePro *ReProData::repro( void )
{
  return RP;
}


void ReProData::start( void )
{
  emit stopRePro();

  RP->Options::setDefaults();
  RP->Options::read( CO );

  emit noMacro( RP );
  emit startRePro( RP, Macro::NoMacro, true );
}


void ReProData::dialog( void )
{
  if ( RP->dialogOpen() )
    return;

  RP->Options::setDefaults();
  RP->Options::read( CO, 0, RePro::CurrentFlag );

  RP->dialog();

  connect( (QWidget*)RP, SIGNAL( dialogAccepted( void ) ),
	   this, SLOT( acceptDialog( void ) ) );
  connect( (QWidget*)RP, SIGNAL( dialogAction( int ) ),
	   this, SLOT( dialogAction( int ) ) );
  connect( (QWidget*)RP, SIGNAL( dialogClosed( int ) ),
	   this, SLOT( dialogClosed( int ) ) );
}


void ReProData::acceptDialog( void )
{
  Options newopt( *((Options*)RP), OptDialog::changedFlag() );

  if ( DO->boolean( "overwrite" ) )
    RP->overwriteOptions().readAppend( newopt );
  else
    RP->overwriteOptions().clear();

  if ( DO->boolean( "default" ) ) {
    RP->Options::setToDefaults();
    CO.clear();
  }
  else
    CO.readAppend( newopt );
}


void ReProData::dialogAction( int r )
{
  if ( r == 2 )
    start();
}


void ReProData::dialogClosed( int r )
{
  disconnect( (QWidget*)RP, SIGNAL( dialogAccepted( void ) ),
	      this, SLOT( acceptDialog( void ) ) );
  disconnect( (QWidget*)RP, SIGNAL( dialogAction( int ) ),
	      this, SLOT( dialogAction( int ) ) );
  disconnect( (QWidget*)RP, SIGNAL( dialogClosed( int ) ),
	      this, SLOT( dialogClosed( int ) ) );
}


void ReProData::raise( void )
{
  RPs->raise( RP );
}


void ReProData::reload( void )
{
  if ( RPs->index( RP ) == RPs->currentRePro() ) {
    MessageBox::warning( "RELACS RePros", "Cannot reload a running RePro!", RPs );
    return;
  }

  // just copying a new library into plugins/ makes 
  // starting every other RePro crash!
  // however, moving libraries is save!
  // the save way is to stop the RePro, move the new library into plugins/
  // and then reload it.

  // remove repro from repros list:
  int index = RPs->indexOf( RP->widget() );
  RPs->removeTab( index );
  delete RP;

  if ( Plugins::destroy( Name, RELACSPlugin::ReProId ) <= 0 )
    Plugins::reopen( Plugins::fileID( Name ) );
  else
    RW->printlog( "! warning: ReProData::reload() -> unable to reopen plugin!" );

  void *mp = Plugins::create( Name, RELACSPlugin::ReProId );
  if ( mp != 0 ) {
    RP = (RePro*)mp;
    RP->setRELACSWidget( RW );
    if ( RP->widget() == 0 ) {
      QImage image( RP->headerImageFile().c_str() );
      ScaledImage *si = new ScaledImage( image );
      RP->setWidget( si );
    }
    RP->disable();
    RPs->insertTab( index, RP->widget(), RP->uniqueName().c_str() );
    emit reloadRePro( Name );
    RW->printlog( "ReProData::reload() -> loaded repro " + RP->uniqueName() );
  }
  else
    RW->printlog( "! error: ReProData::reload() -> cannot recreate RePro " +
		  Name );
}


void ReProData::help( void )
{
  RP->help();
}


}; /* namespace relacs */

#include "moc_repros.cc"

