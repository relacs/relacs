/*
  repros.cc
  Container handling RePros

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

#include <QDateTime>
#include <QLabel>
#include <QToolTip>
#include <QApplication>
#include <relacs/messagebox.h>
#include <relacs/optdialog.h>
#include <relacs/relacswidget.h>
#include <relacs/repro.h>
#include <relacs/defaultrepro.h>
#include <relacs/macros.h>
#include <relacs/repros.h>

namespace relacs {


RePros::RePros( RELACSWidget *rw, QWidget *parent, const char *name )
  : QTabWidget( parent, name ), Menu( 0 )
{
  create( rw );
}


RePros::~RePros( void )
{
  // XXX das ist noch nicht alles ???
  for ( unsigned int k=0; k<RPs.size(); k++ )
    delete RPs[k];
  RPs.clear();
}


int RePros::create( RELACSWidget *rw )
{
  RW = rw;

  CurrentRePro = -1;
  ActionRePro = -1;

  MacroName = "RePro";
  MacroParam = "";

  CurrentView = 0;
  PreviousView = 0;

  DialogOpt.addSeparator();
  DialogOpt.addBoolean( "default", "Set values as default", true );
  DialogOpt.addBoolean( "overwrite", "Changes overwrite macro options", false );

  int n = 0;
  for ( int k=0; k<Plugins::plugins(); k++ )
    if ( Plugins::type( k ) & RELACSPlugin::ReProId ) {
      void *mp = Plugins::create( k );
      RePro *rp = (RePro*)mp;
      rp->setRELACSWidget( RW );
      addTab( rp, rp->title().c_str() );
      ReProData *rd = new ReProData( Plugins::ident( k ), rp, DialogOpt );
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
    addTab( rp, rp->title().c_str() );
    ReProData *rd = new ReProData( rp->name(), rp, DialogOpt );
    RPs.push_back( rd );
    connect( rd, SIGNAL( stopRePro( void ) ),
	     this, SIGNAL( stopRePro( void ) ) );
    connect( rd, SIGNAL( startRePro( RePro*, int, bool ) ),
	     this, SIGNAL( startRePro( RePro*, int, bool ) ) );
    connect( rd, SIGNAL( noMacro( RePro* ) ),
	     this, SIGNAL( noMacro( RePro* ) ) );
    n++;
  }

  return n;
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


QLabel *RePros::display( QWidget *parent, const char *name )
{
  MessageStr = "No Cell";
  Message = new QLabel( "Xg", parent, name );
  Message->setFixedHeight( 2*Message->sizeHint().height() );
  Message->setTextFormat( AutoText );
  Message->setIndent( 14 );
  Message->setText( MessageStr.c_str() );
  QToolTip::add( Message, "Infos from the current research program" );
  return Message;
}


void RePros::message( const string &msg )
{
  Str s = msg;
  s.eraseMarkup();
  RW->printlog( ( CurrentRePro >= 0 ? RPs[CurrentRePro]->RP->name() + ": " : "" ) + s );

  MessageStr = msg;
  QApplication::postEvent( this, new QCustomEvent( QEvent::User+3 ) );
}


void RePros::customEvent( QCustomEvent *qce )
{
  Message->setText( MessageStr.c_str() );
}


QPopupMenu *RePros::menu( void )
{
  if ( Menu == 0 ) {
    Menu = new QPopupMenu( this );
    Menu->insertItem( "&Options...", this, SLOT( dialog( void ) ), CTRL + Key_O );
    Menu->insertItem( "&View", this, SLOT( raise( void ) ), CTRL + Key_V );
    Menu->insertItem( "&Help...", this, SLOT( help( void ) ), CTRL + Key_H );
    Menu->insertSeparator();
    for ( unsigned int k=0; k<RPs.size(); k++ ) {
      QPopupMenu *pop = new QPopupMenu( this );
      pop->insertItem( "&Run", k << 3 );
      pop->insertItem( "&Options...", ( k << 3 ) | 1 );
      pop->insertItem( "&View", ( k << 3 ) | 2 );
      pop->insertItem( "&Load", ( k << 3 ) | 3 );
      pop->insertItem( "&Help...", ( k << 3 ) | 4 );
      connect( pop, SIGNAL( activated( int ) ),
	       this, SLOT( select( int ) ) );
      string mt = "&";
      if ( k == 0 )
	mt += '0';
      else if ( k < 10 )
	mt += ( '1' + k - 1 );
      else
	mt += ( 'a' + k - 10 );
      mt += " ";
      mt += RPs[k]->RP->name();
      Menu->insertItem( mt.c_str(), pop );
    }
  }

  return Menu;
}


void RePros::select( int id )
{
  int index = id >> 3;
  if ( index < 0 || index >= (int)RPs.size() )
    return;

  int action = id & 7;

  if ( action == 1 )
    RPs[index]->dialog();
  else if ( action == 2 )
    raise( index );
  else if ( action == 3 )
    reload( index );
  else if ( action == 4 )
    help( index );
  else
    RPs[index]->start();
}


void RePros::modeChanged( void )
{
  for ( unsigned int k=0; k<RPs.size(); k++ )
    RPs[k]->RP->modeChanged();
}


void RePros::reload( int index )
{
  if ( index < 0 ) {
    RW->printlog( "! warning: RePros::reload() -> invalid index!" );
    return;
  }

  if ( index == CurrentRePro ) {
    MessageBox::warning( "RELACS RePros", "Cannot reload a running RePro!", this );
    return;
  }

  // just copying a new library into plugins/ makes 
  // starting every other RePro crash!
  // however, moving libraries is save!
  // the save way is to stop the RePro, move the new library into plugins/
  // and then reload it.

  // remove repro from repros list:
  removePage( RPs[index]->RP );
  delete RPs[index]->RP;

  if ( Plugins::destroy( RPs[index]->Name, RELACSPlugin::ReProId ) <= 0 ) {
    Plugins::reopen( Plugins::fileID( RPs[index]->Name ) );
  }
  else {
    RW->printlog( "! warning: RePros::reload() -> unable to reopen plugin!" );
  }

  void *mp = Plugins::create( RPs[index]->Name, RELACSPlugin::ReProId );
  if ( mp != 0 ) {
    RePro *rp = (RePro*)mp;
    rp->setRELACSWidget( RW );
    insertTab( rp, rp->title().c_str(), index );
    RPs[index]->RP = rp;
    emit reloadRePro( RPs[index]->Name );
    RW->printlog( "RePros::reload() -> loaded repro " + rp->name() );
  }
  else
    RW->printlog( "! error: RePros::reload() -> cannot recreate RePro " +
		  RPs[index]->Name );
}


void RePros::help( int index )
{
  if ( index < 0 ) {
    RW->printlog( "! warning: RePros::help() -> invalid index!" );
    return;
  }

  RPs[index]->RP->help();
}


void RePros::help( void )
{
  if ( ActionRePro < 0 || ActionRePro >= (int)RPs.size() )
    return;
  RPs[ActionRePro]->RP->help();
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
    RPs[k]->RP->notifyStimulusData();
}


void RePros::notifyMetaData( const string &section )
{
  for ( unsigned int k=0; k<RPs.size(); k++ )
    RPs[k]->RP->notifyMetaData( section );
}


void RePros::sessionStarted( void )
{
  for ( unsigned int k=0; k<RPs.size(); k++ )
    RPs[k]->RP->sessionStarted();
}


void RePros::sessionStopped( bool saved )
{
  for ( unsigned int k=0; k<RPs.size(); k++ )
    RPs[k]->RP->sessionStopped( saved );
}


int RePros::index( const string &name ) const
{
  if ( name.empty() )
    return -1;

  Str id = name;
  id.lower();

  for ( unsigned int k=0; k<RPs.size(); k++ ) {
    Str idr = RPs[k]->Name;
    idr.lower();
    if ( idr == id )
      return k;
  }

  return -1;
}


int RePros::index( const RePro *repro ) const
{
  for ( unsigned int k=0; k<RPs.size(); k++ )
    if ( RPs[k]->RP == repro )
      return k;

  return -1;
}


RePro *RePros::repro( int index ) const
{
  if ( index >= 0 && index < (int)RPs.size() )
    return RPs[index]->RP;
  return 0;
}


int RePros::nameIndex( const string &name ) const
{
  if ( name.empty() )
    return -1;

  Str id = name;
  id.lower();

  for ( unsigned int k=0; k<RPs.size(); k++ ) {
    Str idr = RPs[k]->RP->name();
    idr.lower();
    if ( idr == id )
      return k;
  }

  return -1;
}


void RePros::raise( int index )
{
  if ( index >= 0 )
    raise( RPs[index]->RP );
}


void RePros::raise( RePro *repro )
{
  if ( repro != CurrentView )
    PreviousView = CurrentView;
  CurrentView = repro;
  showPage( repro );
}


ostream &operator<<( ostream &str, const RePros &repros )
{
  for ( unsigned int k=0; k < repros.RPs.size(); k++ ) {
    str << "RePro " << k << ": " << repros.RPs[k]->Name << '\n';
  }
  return str;
}


ReProData::ReProData( const string &name, RePro *repro, Options &dopt )
{
  Name = name;
  RP = repro;
  CO.clear();
  DO = &dopt;
}


void ReProData::start( void )
{
  emit stopRePro();

  RP->Options::setDefaults();
  RP->Options::read( CO );
  RP->getProjectOptions();

  emit noMacro( RP );
  emit startRePro( RP, Macro::NoMacro, true );
}


void ReProData::dialog( void )
{
  if ( RP->dialogOpen() )
    return;

  RP->Options::setDefaults();
  RP->Options::read( CO, 0, RePro::CurrentFlag );
  RP->getProjectOptions();

  RP->dialog();

  connect( RP, SIGNAL( dialogAccepted( void ) ),
	   this, SLOT( acceptDialog( void ) ) );
  connect( RP, SIGNAL( dialogAction( int ) ),
	   this, SLOT( dialogAction( int ) ) );
  connect( RP, SIGNAL( dialogClosed( int ) ),
	   this, SLOT( dialogClosed( int ) ) );
}


void ReProData::acceptDialog( void )
{
  Options newopt( *((Options*)RP), OptDialog::changedFlag() );
  if ( DO->boolean( "overwrite" ) ) {
    RP->overwriteOptions().readAppend( newopt );
  }
  else {
    RP->overwriteOptions().clear();
  }
  if ( DO->boolean( "default" ) ) {
    RP->Options::setToDefaults();
    CO.clear();
  }
  else {
    CO.readAppend( newopt );
  }
  RP->setProjectOptions();
}


void ReProData::dialogAction( int r )
{
  if ( r == 2 )
    start();
}


void ReProData::dialogClosed( int r )
{
  disconnect( RP, SIGNAL( dialogAccepted( void ) ),
	      this, SLOT( acceptDialog( void ) ) );
  disconnect( RP, SIGNAL( dialogAction( int ) ),
	      this, SLOT( dialogAction( int ) ) );
  disconnect( RP, SIGNAL( dialogClosed( int ) ),
	      this, SLOT( dialogClosed( int ) ) );
}



}; /* namespace relacs */

#include "moc_repros.cc"

