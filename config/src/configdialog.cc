/*
  configdialog.cc
  Config widget with dialogs.

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

#include <cmath>
#include <qgroupbox.h>
#include <qthread.h>
#include <qimage.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qmime.h>
#include <qstringlist.h>
#include <qtextbrowser.h>
#include <relacs/optdialog.h>
#include <relacs/configdialog.h>


ConfigDialog::ConfigDialog( const string &configident, int configgroup,
			    const string &name, 
			    const string &title, const string &author, 
			    const string &version, const string &date )
  : QWidget(),
    Config( configident, configgroup ),
    HelpPathes( 0 ),
    CDMutex( false )
{
  BoxLayout = new QBoxLayout( this, QBoxLayout::TopToBottom );
  BoxLayout->setAutoAdd( true );
  Name = name.empty() ? configident : name;
  QObject::setName( Name.c_str() );
  Title = title.empty() ? Name : title;
  Author = author;
  Version = version;
  Date = date;
  DialogSelectMask = 0;
  DialogROMask = 0;
  DialogStyle = OptWidget::NameBehindStyle | OptWidget::HighlightNameStyle;
  DialogCaption = "";
  Dialog = false;
  UseHeader = true;
  UseHelp = true;
  HelpCaption = "";
  Help = false;
}


ConfigDialog::~ConfigDialog( void )
{
}


QBoxLayout *ConfigDialog::boxLayout( void )
{ 
  return BoxLayout;
}


string ConfigDialog::name( void ) const
{
  return Name;
}


void ConfigDialog::setName( const string &name )
{
  Name = name;
}


string ConfigDialog::title( void ) const
{
  return Title;
}


void ConfigDialog::setTitle( const string &title )
{
  Title = title;
}


string ConfigDialog::author( void ) const
{
  return Author;
}


void ConfigDialog::setAuthor( const string &author )
{
  Author = author;
}


string ConfigDialog::version( void ) const
{
  return Version;
}


void ConfigDialog::setVersion( const string &version )
{
  Version = version;
}


string ConfigDialog::date( void ) const
{
  return Date;
}


void ConfigDialog::setDate( const string &date )
{
  Date = date;
}


bool ConfigDialog::dialogHeader( void ) const
{
  return UseHeader;
}


void ConfigDialog::setDialogHeader( bool d )
{
  UseHeader = d;
}


string ConfigDialog::dialogCaption( void ) const
{
  if ( DialogCaption.empty() )
    return title() + " Settings";
  else
    return DialogCaption;
}


void ConfigDialog::setDialogCaption( const string &caption )
{
  DialogCaption = caption;
}


bool ConfigDialog::dialogHelp( void ) const
{
  return UseHelp;
}


void ConfigDialog::setDialogHelp( bool d )
{
  UseHelp = d;
}


string ConfigDialog::helpPath( int inx ) const
{
  if ( inx >= 0 && inx < helpPathes() )
    return HelpPathes[inx];
  else
    return "";
}


int ConfigDialog::helpPathes( void ) const
{
  return HelpPathes.size();
}


void ConfigDialog::clearHelpPathes( void )
{
  HelpPathes.clear();
}


void ConfigDialog::setHelpPath( const string &path )
{
  HelpPathes.clear();
  HelpPathes.push_back( path );
}


void ConfigDialog::addHelpPath( const string &path )
{
  HelpPathes.push_back( path );
}


string ConfigDialog::helpFileName( void ) const
{
  return name() + ".html";
}


int ConfigDialog::dialogSelectMask( void ) const
{
  return DialogSelectMask;
}


void ConfigDialog::setDialogSelectMask( int mask )
{
  DialogSelectMask = mask;
}


void ConfigDialog::addDialogSelectMask( int mask )
{
  DialogSelectMask |= mask;
}


int ConfigDialog::dialogReadOnlyMask( void ) const
{
  return DialogROMask;
}


void ConfigDialog::setDialogReadOnlyMask( int mask )
{
  DialogROMask = mask;
}


void ConfigDialog::addDialogReadOnlyMask( int mask )
{
  DialogROMask |= mask;
}


int ConfigDialog::dialogStyle( void ) const
{
  return DialogStyle;
}


void ConfigDialog::setDialogStyle( int style )
{
  DialogStyle = style;
}


void ConfigDialog::addDialogStyle( int style )
{
  DialogStyle |= style;
}


bool ConfigDialog::dialogOpen( void ) const
{
  return Dialog;
}


void ConfigDialog::setDialogOpen( bool open )
{
  Dialog = open;
}


string ConfigDialog::helpCaption( void ) const
{
  if ( HelpCaption.empty() )
    return title() + " Help";
  else
    return HelpCaption;
}


void ConfigDialog::setHelpCaption( const string &caption )
{
  HelpCaption = caption;
}


bool ConfigDialog::helpOpen( void ) const
{
  return Help;
}


void ConfigDialog::setHelpOpen( bool open )
{
  Help = open;
}


void ConfigDialog::dialog( void )
{
  if ( Dialog )
    return;

  // create and exec dialog:
  Dialog = true;
  OptDialog *od = new OptDialog( false, this );
  od->setCaption( dialogCaption() );
  if ( UseHeader ) {
    QGroupBox *gb = new QGroupBox( 2 + UseHelp, Qt::Horizontal, this );
    gb->setFrameStyle( QFrame::Panel | QFrame::Sunken );
    QLabel *pic = new QLabel( gb );
    QImage image( "doc/web/relacstux.png" );
    pic->setPixmap( QPixmap( image.scale( 150, 150, QImage::ScaleMin ) ) );
    QLabel *rt = new QLabel( gb );
    string s = "<p align=\"center\">";
    if ( ! name().empty() )
      s += "<b>" + name() + "</b><br>";
    if ( ! version().empty() )
      s += "version " + version();
    if ( ! date().empty() )
      s += " (" + date() + ")<br>";
    if ( ! author().empty() )
      s += "by <b>" + author() + "</b>";
    s += "</p>";
    rt->setText( s.c_str() );
    if ( UseHelp ) {
      QPushButton *pb = new QPushButton( "&Help", gb );
      pb->setFixedSize( pb->sizeHint() );
      connect( pb, SIGNAL( clicked( void ) ), this, SLOT( help( void ) ) );
    }
    od->addWidget( gb );
  }
  if ( empty() ) {
    QLabel *ml = new QLabel( string( "There are <b>no</b> options for <b>" +
				     name() + "</b>!" ).c_str(), this );
    od->addWidget( ml );
    od->addButton( "&Ok" );
    connect( od, SIGNAL( dialogClosed( int ) ),
	     this, SLOT( dClosed( int ) ) );
  }
  else {
    od->addOptions( *this, DialogSelectMask, DialogROMask, DialogStyle, mutex() );
    od->setSpacing( int(9.0*::exp(-double(Options::size())/14.0))+1 );
    od->setMargin( 10 );
    od->setRejectCode( 0 );
    od->addButton( "&Ok", OptDialog::Accept, 1 );
    od->addButton( "&Apply", OptDialog::Accept );
    od->addButton( "&Reset", OptDialog::Reset );
    od->addButton( "&Close" );
    connect( od, SIGNAL( dialogClosed( int ) ),
	     this, SLOT( dClosed( int ) ) );
    connect( od, SIGNAL( buttonClicked( int ) ),
	     this, SIGNAL( dialogAction( int ) ) );
    connect( od, SIGNAL( valuesChanged( void ) ),
	     this, SIGNAL( dialogAccepted( void ) ) );
  }
  od->exec();
}


void ConfigDialog::dClosed( int r )
{
  Dialog = false;
  emit dialogClosed( r );
}


void ConfigDialog::help( void )
{
  if ( Help )
    return;

  Help = true;

  // create and exec dialog:
  OptDialog *od = new OptDialog( false, this );
  od->setCaption( helpCaption() );
  QTextBrowser *hb = new QTextBrowser( this );
  QStringList fpl;
  for ( int k=0; k<helpPathes(); k++ )
    fpl.push_back( helpPath( k ).c_str() );
  hb->mimeSourceFactory()->setFilePath( fpl );
  string helpfile = helpFileName();
  hb->setSource( helpfile.c_str() );
  if ( hb->mimeSourceFactory()->data( helpfile.c_str() ) == 0 ) {
    string helptext = "Sorry, can't find any help text for <br><h2>"
      + name() + "</h2>.<br><br>No file <code>" + helpfile
      + "</code> found in any of the directories<br>";
    for ( int k=0; k<helpPathes(); k++ )
      helptext += "<code>" + helpPath( k ) + "</code><br>";
    hb->setText( helptext.c_str() );
  }
  hb->setMinimumSize( 600, 400 );
  od->addWidget( hb );
  od->addButton( "&Ok" );
  connect( od, SIGNAL( dialogClosed( int ) ),
	   this, SLOT( hClosed( int ) ) );
  od->exec();
}


void ConfigDialog::hClosed( int r )
{
  Help = false;
}


void ConfigDialog::lock() const
{
  CDMutex.lock();
}


void ConfigDialog::unlock() const
{
  CDMutex.unlock();
}


QMutex *ConfigDialog::mutex( void )
{
  return &CDMutex;
}


#include "moc_configdialog.cc"
