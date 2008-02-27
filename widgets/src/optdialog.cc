/*
  optdialog.cc
  A Dialog for editing Options.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <qlayout.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include "optdialog.h"
#include "optdialogbutton.h"


OptDialog::OptDialog( QWidget *parent, char *name )
  : QDialog( parent, name, TRUE,
	     WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu ),
    OWs( 0 ), Tabs( 0 )
{
  construct();
  setCaption( "Dialog" );
}


OptDialog::OptDialog( bool modal, QWidget *parent, char *name )
  : QDialog( parent, name, modal,
	     WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu ),
    OWs( 0 ), Tabs( 0 )
{
  construct();
  setCaption( "Dialog" );
}


OptDialog::OptDialog( Options &opt, const string &title,
		      QMutex *mutex, QWidget *parent, char *name )
  : QDialog( parent, name, TRUE,
	     WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu ),
    OWs( 0 ), Tabs( 0 )
{
  construct();
  setCaption( title );
  addOptions( opt, 0, 0, 0, mutex );
}


OptDialog::OptDialog( Options &opt, const string &title, bool modal,
		      QMutex *mutex, QWidget *parent, char *name )
  : QDialog( parent, name, modal,
	     WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu ),
    OWs( 0 ), Tabs( 0 )
{
  construct();
  setCaption( title );
  addOptions( opt, 0, 0, 0, mutex );
}


OptDialog::~OptDialog( void )
{
  clearButtons();
  for ( unsigned int k=0; k<OWs.size(); k++ )
    delete OWs[k];
  OWs.clear();
}


void OptDialog::construct( void )
{
  QVBoxLayout *l = new QVBoxLayout( this, 4, 10 );
  l->setAutoAdd( true );
  //  l->setResizeMode( QLayout::Fixed );
  DialogBox = new QVBox( this );
  ButtonBox = new QHBox( this );
  DefaultButtons = false;
  addButton( "&Ok", Accept, 1 );
  addButton( "&Reset", Reset );
  addButton( "&Defaults", Defaults );
  addButton( "&Close" );
  DefaultButtons = true;
  RejectCode = 0;
}


void OptDialog::setCaption( const string &title )
{
  QDialog::setCaption( title.c_str() );
}


OptWidget *OptDialog::addOptions( Options &opt, int selectmask, int romask,
				  int style, QMutex *mutex )
{
  Tabs = 0;

  OptWidget *ow = new OptWidget( &opt, selectmask, romask, false,
				 style, mutex, DialogBox );
  OWs.push_back( ow );

  return ow;
}


OptWidget *OptDialog::addTabOptions( const string &label, Options &opt,
				     int selectmask, int romask, int style,
				     QMutex *mutex )
{
  QWidget *w = new QWidget( DialogBox );
  QVBoxLayout *l = new QVBoxLayout( w );

  OptWidget *ow = new OptWidget( &opt, selectmask, romask, false,
				 style, mutex, w );
  OWs.push_back( ow );

  l->addWidget( ow );
  l->addStretch( 10 );

  if ( Tabs == 0 )
    Tabs = new QTabWidget( DialogBox );

  Tabs->addTab( w, label.c_str() );

  return ow;

}


void OptDialog::addWidget( QWidget *widget )
{
  Tabs = 0;
  widget->reparent( DialogBox, QPoint( 0, 0 ) );
}


void OptDialog::addTabWidget( const string &label, QWidget *widget )
{
  if ( Tabs == 0 )
    Tabs = new QTabWidget( DialogBox );

  Tabs->addTab( widget, label.c_str() );
}


void OptDialog::setSpacing( int pixel )
{
  for ( unsigned int k=0; k<OWs.size(); k++ )
    OWs[k]->setSpacing( pixel );
}


void OptDialog::setMargin( int pixel )
{
  for ( unsigned int k=0; k<OWs.size(); k++ )
    OWs[k]->setMargin( pixel );
}


int OptDialog::rejectCode( void ) const
{
  return RejectCode;
}


void OptDialog::setRejectCode( int code )
{
  RejectCode = code;
}


void OptDialog::clearButtons( void )
{
  for ( unsigned int k=0; k<Buttons.size(); k++ )
    delete Buttons[k];
  Buttons.clear();
  DefaultButtons = false;
}


void OptDialog::addButton( const string &title, OptDialog::Action action,
			   int rcode, bool close, bool dflt )
{
  if ( DefaultButtons ) {
    clearButtons();
  }
  OptDialogButton *odb = new OptDialogButton( title, action, rcode, close, dflt );
  connect( odb, SIGNAL( accepted( void ) ), this, SIGNAL( valuesChanged( void ) ) );
  connect( odb, SIGNAL( clicked( int ) ), this, SIGNAL( buttonClicked( int ) ) );
  connect( odb, SIGNAL( done( int ) ), this, SLOT( done( int ) ) );
  Buttons.push_back( odb );
}


void OptDialog::createButtons( void )
{
  // check for default button:
  bool dflt = false;
  for ( unsigned int k=0; k<Buttons.size(); k++ ) {
    if ( Buttons[k]->defaultButton() ) {
      dflt = true;
      break;
    }
  }

  // set default button:
  if ( ! dflt ) {
    for ( unsigned int k=0; k<Buttons.size(); k++ ) {
      if ( Buttons[k]->accept() ) {
	Buttons[k]->setDefaultButton();
	break;
      }
    }
  }

  QLabel *label;

  if ( Buttons.size() == 1 ) {
    label = new QLabel( ButtonBox );
    Buttons[0]->create( OWs, RejectCode, ButtonBox );
    label = new QLabel( ButtonBox );
  }
  else {
    for ( unsigned int k=0; k<Buttons.size(); k++ ) {
      if ( k > 0 )  // just some stretchable space
	label = new QLabel( ButtonBox );
      Buttons[k]->create( OWs, RejectCode, ButtonBox );
    }
  }
}


int OptDialog::exec( void )
{
  createButtons();

  if ( isModal() ) {
    return QDialog::exec();
  }
  else {
    QDialog::show();
    return RejectCode;
  }
}


void OptDialog::done( int r )
{
  QDialog::done( r );
  emit dialogClosed( r );
}


void OptDialog::reject( void )
{
  done( RejectCode );
}


OptDialogButton::OptDialogButton( void )
  : Title( "" ),
    Action( OptDialog::NoAction ),
    ReturnCode( OptDialog::NoReturn ),
    Close( true ),
    OWs( 0 ),
    PB( 0 )
{
}


OptDialogButton::OptDialogButton( const OptDialogButton &odb )
  : Title( odb.Title ),
    Action( odb.Action ),
    ReturnCode( odb.ReturnCode ),
    Close( odb.Close ),
    OWs( 0 ),
    PB( odb.PB )
{
}


OptDialogButton::OptDialogButton( const string &title, OptDialog::Action action,
				  int rcode, bool close, bool dflt )
  : Title( title ),
    Action( action ),
    ReturnCode( rcode ),
    Close( close ),
    Default( dflt ),
    OWs( 0 ),
    PB( 0 )
{
}


OptDialogButton::~OptDialogButton( void )
{
}


bool OptDialogButton::defaultButton( void ) const
{
  return Default;
}


void OptDialogButton::setDefaultButton( bool d )
{
  Default = d;
}


bool OptDialogButton::accept( void ) const
{
  return ( Action == OptDialog::Accept );
}


void OptDialogButton::create( vector<OptWidget*> ows, int rejectc,
			      QWidget *parent, char *name )
{
  OWs = ows;
  PB = new QPushButton( Title.c_str(), parent, name );
  PB->setDefault( Default );
  connect( PB, SIGNAL( clicked() ), 
	   this, SLOT( clicked() ) );
  if ( ReturnCode == OptDialog::ReturnReject )
    ReturnCode = rejectc;
}


void OptDialogButton::clicked( void )
{
  switch ( Action ) {
  case OptDialog::Accept :
    for ( unsigned int k=0; k<OWs.size(); k++ )
      OWs[k]->accept( false );
    emit accepted();
    for ( unsigned int k=0; k<OWs.size(); k++ ) {
      OWs[k]->options()->delFlags( OptDialog::changedFlag() );
    }
    break;  
  case OptDialog::Reset :
    for ( unsigned int k=0; k<OWs.size(); k++ )
      OWs[k]->reset();
    break;  
  case OptDialog::Defaults :
    for ( unsigned int k=0; k<OWs.size(); k++ )
      OWs[k]->resetDefault();
    break;  
  case OptDialog::NoAction :
    break;
  }

  if ( ReturnCode != OptDialog::NoReturn ) {
    emit clicked( ReturnCode );
    if ( Close )
      emit done( ReturnCode );
  }
}

