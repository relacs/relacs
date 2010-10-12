/*
  configdialog.cc
  A dialog for a ConfigClass.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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
#include <QLayout>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QTextBrowser>
#include <relacs/configdialog.h>

using namespace std;

namespace relacs {


ConfigDialog::ConfigDialog( const string &configident, int configgroup,
			    const string &name, const string &author, 
			    const string &version, const string &date )
  : ConfigClass( configident, configgroup ),
    MainWidget( 0 ),
    HelpPathes( 0 ),
    CDMutex()
{
  Name = name.empty() ? configident : name;
  Author = author;
  Version = version;
  Date = date;
  DialogSelectMask = 0;
  DialogROMask = 0;
  DialogStyle = OptWidget::NameBehindStyle | OptWidget::HighlightNameStyle;
  DialogCaption = "";
  Dialog = false;
  UseHeader = true;
  HeaderBackgroundColor = "";
  HeaderForegroundColor = "";
  HeaderImageFile = "";
  HelpCaption = "";
  Help = false;
}


ConfigDialog::~ConfigDialog( void )
{
  HelpPathes.clear();
}


string ConfigDialog::name( void ) const
{
  return Name;
}


void ConfigDialog::setName( const string &name )
{
  Name = name;
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
    return name() + " Settings";
  else
    return DialogCaption;
}


void ConfigDialog::setDialogCaption( const string &caption )
{
  DialogCaption = caption;
}


bool ConfigDialog::dialogHelp( void ) const
{
  return ( configMode() & ConfigClass::Help );
}


void ConfigDialog::setDialogHelp( bool d )
{
  if ( d )
    addConfigMode( ConfigClass::Help );
  else
    delConfigMode( ConfigClass::Help );
}


string ConfigDialog::headerBackgroundColor( void ) const
{
  return HeaderBackgroundColor;
}


void ConfigDialog::setHeaderBackgroundColor( const string &color )
{
  if ( color.size() == 7 && color[0] == '#' )
    HeaderBackgroundColor = color;
  else
    HeaderBackgroundColor = "";
}


string ConfigDialog::headerForegroundColor( void ) const
{
  return HeaderForegroundColor;
}


void ConfigDialog::setHeaderForegroundColor( const string &color )
{
  if ( color.size() == 7 && color[0] == '#' )
    HeaderForegroundColor = color;
  else
    HeaderForegroundColor = "";
}


string ConfigDialog::headerImageFile( void ) const
{
  return HeaderImageFile;
}


void ConfigDialog::setHeaderImageFile( const string &file )
{
  HeaderImageFile = file;
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
    return name() + " Help";
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


int hextodec( char c1, char c2 )
{
  int d1 = c1 == '0' ? 0 : c1 >= '1' && c1 <= '9' ? c1-'1'+1 : c1-'a'+10;
  int d2 = c2 == '0' ? 0 : c2 >= '1' && c2 <= '9' ? c2-'1'+1 : c2-'a'+10;
  return d1*16+d2;
}


void ConfigDialog::dialogHeaderWidget( OptDialog *od )
{
  if ( UseHeader ) {
    // frame:
    QFrame *frm = new QFrame;
    QHBoxLayout *hb = new QHBoxLayout;
    frm->setLayout( hb );
    if ( ! headerBackgroundColor().empty() ) {
      frm->setAutoFillBackground( true );
      int r = hextodec( headerBackgroundColor()[1], headerBackgroundColor()[2] );
      int g = hextodec( headerBackgroundColor()[3], headerBackgroundColor()[4] );
      int b = hextodec( headerBackgroundColor()[5], headerBackgroundColor()[6] );
      QPalette p = frm->palette();
      p.setColor( QPalette::Window, QColor( r, g, b ) );
      frm->setBackgroundRole( QPalette::Window );
      frm->setPalette( p );
    }
    // background image:
    if ( ! headerImageFile().empty() ) {
      int h = od->fontMetrics().height()*11/2;
      QImage image( headerImageFile().c_str() );
      QLabel *pic = new QLabel;
      hb->addWidget( pic );
      pic->setPixmap( QPixmap::fromImage( image.scaled( h, h, Qt::KeepAspectRatio ) ) );
    }
    // infos:
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
    QLabel *rt = new QLabel;
    hb->addWidget( rt );
    rt->setText( s.c_str() );
    rt->setSizePolicy( QSizePolicy( QSizePolicy::MinimumExpanding, QSizePolicy::Fixed ) );
    if ( ! headerForegroundColor().empty() ) {
      int r = hextodec( headerForegroundColor()[1], headerForegroundColor()[2] );
      int g = hextodec( headerForegroundColor()[3], headerForegroundColor()[4] );
      int b = hextodec( headerForegroundColor()[5], headerForegroundColor()[6] );
      QPalette p = rt->palette();
      p.setColor( QPalette::WindowText, QColor( r, g, b ) );
      rt->setPalette( p );
    }
    // help button:
    if ( dialogHelp() ) {
      QPushButton *pb = new QPushButton( "&Help" );
      hb->addWidget( pb );
      pb->setFixedSize( pb->sizeHint() );
      connect( pb, SIGNAL( clicked( void ) ), this, SLOT( help( void ) ) );
    }

    frm->setFrameStyle( QFrame::Box | QFrame::Sunken );
    od->addWidget( frm );
  }
}


void ConfigDialog::dialogEmptyMessage( OptDialog *od )
{
  QLabel *ml = new QLabel( string( "There are <b>no</b> options for <b>" +
				   name() + "</b>!" ).c_str() );
  od->addWidget( ml );
  od->addButton( "&Ok" );
  connect( od, SIGNAL( dialogClosed( int ) ),
	   this, SLOT( dClosed( int ) ) );
}


void ConfigDialog::dialogOptions( OptDialog *od )
{
  od->addOptions( *this, DialogSelectMask, DialogROMask, DialogStyle, mutex() );
  od->setVerticalSpacing( int(9.0*::exp(-double(Options::size())/14.0))+1 );
}


void ConfigDialog::dialogButtons( OptDialog *od )
{
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


void ConfigDialog::dialog( void )
{
  if ( dialogOpen() || ( configMode() & ConfigClass::Dialog ) == 0 )
    return;
  setDialogOpen();

  OptDialog *od = new OptDialog( false, mainWidget() );
  od->setCaption( dialogCaption() );
  dialogHeaderWidget( od );
  if ( Options::size( DialogSelectMask ) <= 0 )
    dialogEmptyMessage( od );
  else {
    dialogOptions( od );
    dialogButtons( od );
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
  if ( Help || ! dialogHelp() )
    return;

  Help = true;

  // create and exec dialog:
  OptDialog *od = new OptDialog( false, mainWidget() );
  od->setCaption( helpCaption() );
  QTextBrowser *hb = new QTextBrowser( mainWidget() );
  QStringList fpl;
  for ( int k=0; k<helpPathes(); k++ )
    fpl.push_back( helpPath( k ).c_str() );
  hb->setSearchPaths( fpl );
  string helpfile = helpFileName();
  hb->setSource( QUrl::fromLocalFile( helpfile.c_str() ) );
  if ( hb->toHtml().isEmpty() ) {
    string helptext = "Sorry, can't find any help text for <br><h2>"
      + name() + "</h2><br><br>No file <code>" + helpfile
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


QWidget *ConfigDialog::mainWidget( void )
{
  return MainWidget;
}


void ConfigDialog::setMainWidget( QWidget *widget )
{
  MainWidget = widget;
}


void ConfigDialog::lock() const
{
  CDMutex.lock();
}


void ConfigDialog::unlock() const
{
  CDMutex.unlock();
}


bool ConfigDialog::tryLock( int timeout )
{
  return CDMutex.tryLock( timeout );
}


QMutex *ConfigDialog::mutex( void )
{
  return &CDMutex;
}


ostream &ConfigDialog::saveXML( ostream &str, int selectmask, int level, int indent ) const
{
  string indstr1( level*indent, ' ' );
  str << indstr1 << "<section>\n";
  str << indstr1 << "  <type>" << name() << "</type>\n";
  Options::saveXML( str, selectmask, level+1, indent );
  str << indstr1 << "</section>\n";
  return str;
}


}; /* namespace relacs */

#include "moc_configdialog.cc"

