/*
  optdialog.cc
  A Dialog for editing Options.

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

#include <QLayout>
#include <QLabel>
#include <QPushButton>
#include <relacs/optdialog.h>
#include <relacs/optdialogbutton.h>

namespace relacs {


OptDialog::OptDialog( QWidget *parent )
  : QDialog( parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint ),
    OWs( 0 ), Tabs( 0 )
		    // WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu
{
  construct();
  setModal( true );
  setCaption( "Dialog" );
}


OptDialog::OptDialog( bool modal, QWidget *parent )
  : QDialog( parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint ),
    OWs( 0 ), Tabs( 0 )
{
  construct();
  setModal( modal );
  setCaption( "Dialog" );
}


OptDialog::OptDialog( Options &opt, const string &title,
		      QMutex *mutex, QWidget *parent )
  : QDialog( parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint ),
    OWs( 0 ), Tabs( 0 )
{
  construct();
  setModal( true );
  setCaption( title );
  addOptions( opt, 0, 0, 0, mutex );
}


OptDialog::OptDialog( Options &opt, const string &title, bool modal,
		      QMutex *mutex, QWidget *parent )
  : QDialog( parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint ),
    OWs( 0 ), Tabs( 0 )
{
  construct();
  setModal( modal );
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
  QVBoxLayout *l = new QVBoxLayout;
  setLayout( l );
  DialogBox = new QVBoxLayout;
  l->addLayout( DialogBox );
  ButtonBox = new QHBoxLayout;
  l->addLayout( ButtonBox );
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
  QDialog::setWindowTitle( title.c_str() );
}


OptWidget *OptDialog::addOptions( Options &opt, int selectmask, int romask,
				  int style, QMutex *mutex )
{
  Tabs = 0;

  OptWidget *ow = new OptWidget( &opt, selectmask, romask, false,
				 style, mutex );
  OWs.push_back( ow );
  DialogBox->addWidget( ow );

  return ow;
}


OptWidget *OptDialog::addTabOptions( const string &label, Options &opt,
				     int selectmask, int romask, int style,
				     QMutex *mutex )
{
  OptWidget *ow = new OptWidget( &opt, selectmask, romask, false,
				 style, mutex );
  OWs.push_back( ow );

  if ( Tabs == 0 ) {
    Tabs = new QTabWidget;
    DialogBox->addWidget( Tabs );
  }

  Tabs->addTab( ow, label.c_str() );

  return ow;

}


void OptDialog::addWidget( QWidget *widget )
{
  Tabs = 0;
  DialogBox->addWidget( widget );
}


void OptDialog::addTabWidget( const string &label, QWidget *widget )
{
  if ( Tabs == 0 ) {
    Tabs = new QTabWidget;
    DialogBox->addWidget( Tabs );
  }

  Tabs->addTab( widget, label.c_str() );
}


void OptDialog::setVerticalSpacing( int pixel )
{
  for ( unsigned int k=0; k<OWs.size(); k++ )
    OWs[k]->setVerticalSpacing( pixel );
}


void OptDialog::setHorizontalSpacing( int pixel )
{
  for ( unsigned int k=0; k<OWs.size(); k++ )
    OWs[k]->setHorizontalSpacing( pixel );
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
    label = new QLabel;
    ButtonBox->addWidget( label );
    Buttons[0]->create( OWs, RejectCode, ButtonBox );
    label = new QLabel;
    ButtonBox->addWidget( label );
  }
  else {
    for ( unsigned int k=0; k<Buttons.size(); k++ ) {
      if ( k > 0 ) {  // just some stretchable space
	label = new QLabel;
	ButtonBox->addWidget( label );
      }
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
			      QHBoxLayout *layout )
{
  OWs = ows;
  PB = new QPushButton( Title.c_str() );
  layout->addWidget( PB );
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


}; /* namespace relacs */

#include "moc_optdialog.cc"

