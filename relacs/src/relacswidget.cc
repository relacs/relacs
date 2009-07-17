/*
  relacswidget.cc
  Coordinates RePros, session, input, output, plotting, and saving. 

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

#include <unistd.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <qlabel.h>
#include <qtabwidget.h>
#include <qkeycode.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <qtooltip.h>
#include <qlayout.h>
#include <qtextbrowser.h>
#include <relacs/relacswidget.h>
#include <relacs/plugins.h>
#include <relacs/defaultsession.h>
#include <relacs/aisim.h>
#include <relacs/aosim.h>
#include <relacs/attsim.h>
#include <relacs/attenuate.h>
#include <relacs/filter.h>
#include <relacs/model.h>
#include <relacs/messagebox.h>
#include <relacs/optdialog.h>

namespace relacs {


const string RELACSWidget::ModeStr[5] = { "Idle", "Acquisition", "Simulation", "Browsing", "Analysis" };


void* createAISim( void )
{
  return new AISim();
}


void* createAOSim( void )
{
  return new AOSim();
}


void* createAttSim( void )
{
  return new AttSim();
}


RELACSWidget::RELACSWidget( const string &pluginrelative,
			    const string &pluginhome,
			    const string &pluginhelp,
			    const string &coreconfigfiles,
			    const string &pluginconfigfiles,
			    const string &docpath,
			    const string &iconpath,
			    QSplashScreen *splash,
			    ModeTypes mode, QWidget *parent, const char *name )
  : QMainWindow( parent, name ),
    ConfigClass( "RELACS", RELACSPlugin::Core ),
    Mode( mode ),
    SS( this ),
    MTDT( this ),
    SignalTime( -1.0 ),
    ReadLoop( this ),
    WriteLoop( this ),
    LogFile( 0 ),
    InfoFile( 0 ),
    InfoFileMacro( "" ),
    IsFullScreen( false ),
    GUILock( 0 ),
    DataMutex( true ),  // recursive, because of activateGains()!
    DataMutexCount( 0 ),
    AIMutex( true ),  // recursive, because of activateGains()???
    SignalMutex( false ),
    RunData( false ),
    RunDataMutex( false ),
    MinTraceTime( 0.0 ),
    DeviceMenu( 0 ),
    Help( false )
{
  printlog( "this is RELACS, version " + string( RELACSVERSION ) );

  // setup configuration files:
  CFG.clearGroups();
  CFG.addGroup( coreconfigfiles );
  CFG.addGroup( pluginconfigfiles );
  DocPath = docpath;
  DocPath.preventSlash();
  setenv( "RELACSDOCPATH", DocPath.c_str(), 1 );
  IconPath = iconpath;
  IconPath.preventSlash();

  // configuration parameter for RELACS:
  addConfig();
  addLabel( "input data", 0, Parameter::TabLabel );
  addInteger( "inputtraces", "Number of input-traces", 1 );
  addNumber( "inputsamplerate", "Input sampling rate", 20000.0, 1.0, 1000000.0, 1000.0, "Hz", "kHz" ); // Hertz, -> 2.4MB pro minute and channel
  addNumber( "inputtracecapacity", "Ring buffer has capacity for ", 600.0, 1.0, 1000000.0, 1.0, "s" );
  addBoolean( "inputunipolar", "Unipolar input", false );
  addText( "inputtraceid", "Input trace identifier", "V-1" );
  addNumber( "inputtracescale", "Input trace scale", 1.0 );
  addText( "inputtraceunit", "Input trace unit", "V" );
  addInteger( "inputtracedevice", "Input trace device", 0 );
  addInteger( "inputtracechannel", "Input trace channel", 0 );
  addText( "inputtracereference", "Input trace reference", InData::referenceStr( InData::RefGround ) );
  addInteger( "inputtracegain", "Input trace gain", 0 );
  addLabel( "output data", 0, Parameter::TabLabel );
  addNumber( "maxoutputrate", "Default maximum output sampling rate", 100000.0, 1.0, 10000000.0, 1000.0, "Hz", "kHz" );
  addText( "outputtraceid", "Output trace identifier", "out-1" );
  addInteger( "outputtracechannel", "Output trace channel", 0 );
  addText( "outputtracedevice", "Output trace device", "ao-1" );
  addNumber( "outputtracescale", "Output trace scale factor to Volt", 1.0, -10000000.0, 10000000.0, 0.1 );
  addText( "outputtraceunit", "Output trace unit", "V" );
  addNumber( "outputtracemaxrate", "Maximum output sampling rate", 0.0, 0.0, 10000000.0, 1000.0, "Hz", "kHz" );
  addNumber( "outputtracedelay", "Signal delay", 0.0, 0.0, 10.0, 0.00001, "s", "ms" );

  // main widget:
  setCaption( "RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation: Version " + QString( RELACSVERSION ) );
  MainWidget = new QWidget( this );
  QGridLayout *mainlayout = new QGridLayout( MainWidget, 2, 2, 5, 5, "RELACSWidget::MainLayout" );
  setCentralWidget( MainWidget );

  // filter and detectors:
  FD = new FilterDetectors( this, MainWidget, "RELACSWidget::FilterDetectors" );

  // macros:
  MC = new Macros( this, MainWidget, "RELACSWidget::Macros" );

  // data acquisition:
  AQ = 0;
  AQD = new Acquire();

  // simulator:
  SIM = new Simulator();

  // devices:
  ADV = new AllDevices();
  DV = new Devices();
  AID = new AIDevices();
  AOD = new AODevices();
  DIOD = new DIODevices();

  // attenuators:
  ATD = new AttDevices();
  ATI = new AttInterfaces( AOD );

  // load config:
  SF = 0;
  CFG.read( RELACSPlugin::Core );
  CFG.configure( RELACSPlugin::Core );

  // loading plugins:
  Plugins::add( "AISim", RELACSPlugin::AnalogInputId, createAISim, VERSION );
  Plugins::add( "AOSim", RELACSPlugin::AnalogOutputId, createAOSim, VERSION );
  Plugins::add( "AttSim", RELACSPlugin::AttenuatorId, createAttSim, VERSION );
  StrQueue pluginhomes( pluginhome, "|" );
  pluginhomes.strip();
  for ( int k=0; k<SS.Options::size( "pluginpathes" ); k++ ) {
    string pluginlib = SS.text( "pluginpathes", k );
    if ( !pluginlib.empty() ) {
      Plugins::openPath( pluginlib, pluginrelative, pluginhomes );
    }
  }

  /*
  saveDoxygenOptions();
  ::exit( 0 );
  */

  if ( Plugins::empty() ) {
    printlog(  "! error: No valid plugins found. Exit now." );
    MessageBox::error( "RELACS Error !", "No valid plugins found.<br>\nExit now.", this );
    qApp->exit( 1 );
    ::exit( 1 ); // do we need that?
  }
  else {
    if ( !Plugins::libraryErrors().empty() ) {
      Str ws = "Errors in loading library\n";
      string s = Plugins::libraryErrors();
      s.insert( 0, "<ul><li>" );
      string::size_type p = s.find( "\n" );
      while ( p != string::npos ) {
	s.insert( p, "</li>" );
	p += 6;
	string::size_type n = s.find( "\n", p );
	if ( n == string::npos )
	  break;
	s.insert( p, "<li>" );
	p = n + 4;
      }
      ws += s + "</ul>";
      printlog( "! warning: " + ws.erasedMarkup() );
      MessageBox::warning( "RELACS Warning !", ws, this );
    }
    if ( !Plugins::classErrors().empty() ) {
      Str ws = "Errors in following plugins (recompile them!):\n";
      string s = Plugins::classErrors();
      s.insert( 0, "<ul><li>" );
      string::size_type p = s.find( "\n" );
      while ( p != string::npos ) {
	s.insert( p, "</li>" );
	p += 6;
	string::size_type n = s.find( "\n", p );
	if ( n == string::npos )
	  break;
	s.insert( p, "<li>" );
	p = n + 4;
      }
      ws += s + "</ul>";
      printlog( "! warning: " + ws.erasedMarkup() );
      MessageBox::warning( "RELACS Warning !", ws, this );
    }
  }

  // assemble help pathes:
  Parameter &p = SS[ "pluginhelppathes" ];
  for ( int k=0; k<p.size(); k++ ) {
    Str path = p.text( k );
    path.preventSlash();
    HelpPathes.add( path );
  }
  HelpPathes.append( pluginhelp, "|" );
  HelpPathes.strip();
  for ( int k=0; k<HelpPathes.size(); k++ )
    HelpPathes[k].preventSlash();

  // session, control tabwidget:
  QTabWidget *cw = new QTabWidget( MainWidget );

#if QT_VERSION >= 300
  OrgBackground = paletteBackgroundColor();
#else
  OrgBackground = backgroundColor();
#endif

  // Control plugins:
  CN.clear();
  CN.reserve( 10 );
  Parameter &cp = SS[ "controlplugin" ];
  for ( int k=0; k<cp.size(); k++ ) {
    string cm = cp.text( k, "" );
    if ( ! cm.empty() ) {
      Control *cn = (Control *)Plugins::create( cm, RELACSPlugin::ControlId );
      if ( cn == 0 ) {
	printlog( "! warning: Contol plugin " + cm + " not found!" );
	MessageBox::warning( "RELACS Warning !", 
			     "Contol plugin <b>" + cm + "</b> not found!",
			     this );
      }
      else {
	cn->reparent( this, QPoint( 0, 0 ) );
	cw->addTab( cn, cn->title().c_str() );
	cn->setRELACSWidget( this );
	CN.push_back( cn );
      }
    }
  }

  // model plugin:
  MD = 0;
  string ms = SS.text( "modelplugin", "" );
  if ( ms.empty() || ms == "none" ) {
    Str es = "<b>No model plugin found!</b><br>";
    printlog( "! warning: " + es.erasedMarkup() );
    MessageBox::warning( "RELACS Warning !", es, this );
  }
  else {
    MD = (Model *)Plugins::create( ms, RELACSPlugin::ModelId );
    if ( MD == 0 ) {
      Str es = "Model plugin \"<b>";
      es += ms;
      es += "</b>\" not found!<br>\n";
      printlog( "! warning: " + es.erasedMarkup() );
      MessageBox::warning( "RELACS Warning !", es, this );
    }
    else
      MD->setRELACSWidget( this );
  }
  SIM->setModel( MD );

  // Load filter and detector plugins:
  Str fdw = FD->createFilters();
  if ( !fdw.empty() ) {
    printlog( "! error: " + fdw.erasedMarkup() );
    MessageBox::error( "RELACS Error !", fdw, this );
  }

  // setup RePros:
  ReProRunning = false;
  RP = new RePros( this, MainWidget, "RELACSWidget::RePros" );
  if ( RP->size() <= 0 ) {
    printlog( "! error: No RePros found! Exit now!" );
    MessageBox::error( "RELACS Error !", "No RePros found!<br>Exit now!", this );
    ::exit( 1 );
  }
  connect( RP, SIGNAL( stopRePro( void ) ), 
	   this, SLOT( stopRePro( void ) ) );
  connect( RP, SIGNAL( startRePro( RePro*, int, bool ) ), 
	   this, SLOT( startRePro( RePro*, int, bool ) ) );
  CurrentRePro = 0;

  // setup Macros:
  MC->setRePros( RP );
  MC->load( "", true );
  MC->load();
  MC->check();
  MC->buttons();
  connect( MC, SIGNAL( stopRePro( void ) ), 
	   this, SLOT( stopRePro( void ) ) );
  connect( MC, SIGNAL( startRePro( RePro*, int, bool ) ), 
	   this, SLOT( startRePro( RePro*, int, bool ) ) );
  connect( RP, SIGNAL( noMacro( RePro* ) ), 
	   MC, SLOT( noMacro( RePro* ) ) );
  connect( RP, SIGNAL( reloadRePro( const string& ) ),
	   MC, SLOT( reloadRePro( const string& ) ) );

  // setup PlotTrace:
  PT = new PlotTrace( this, MainWidget, "RELACSWidget::PT" );
  PT->setDataMutex( &DataMutex );

  // status bar:
  // RePro message:
  QLabel *rl = RP->display( statusBar() );
  statusBar()->addWidget( rl, 200, true );
  int statusbarheight = rl->height();
  // Session message:
  SN = new Session( this, statusbarheight, statusBar(), "RELACSWidget::SN" );
  //  SN = new Session( this, statusbarheight, this, "RELACSWidget::SN" );
  statusBar()->addWidget( SN, 0, false );
  // SaveFiles:
  SF = new SaveFiles( this, statusbarheight, statusBar(), "RELACSWidget::SF" );
  SS.notify(); // initialize SF from the Settings
  statusBar()->addWidget( SF, 0, false );
  // Simulation:
  SimLabel = new QLabel( this );
  SimLabel->setTextFormat( PlainText );
  SimLabel->setAlignment( AlignRight | AlignVCenter );
  SimLabel->setIndent( 2 );
  SimLabel->setFixedHeight( statusbarheight );
  QToolTip::add( SimLabel, "The load of the simulation" );
  statusBar()->addWidget( SimLabel, 0, false );
  
  // menubar:
  QPopupMenu* popup;
  // file:
  QPopupMenu *filemenu = new QPopupMenu;

  AcquisitionAction = new QAction( this );
  AcquisitionAction->setMenuText( "&Online (acquire)" );
  AcquisitionAction->setAccel( CTRL+SHIFT+Key_O );
  connect( AcquisitionAction, SIGNAL( activated() ), 
	   this, SLOT( startAcquisition() ) );
  AcquisitionAction->addTo( filemenu );

  SimulationAction = new QAction( this );
  SimulationAction->setMenuText( "&Simulate" );
  SimulationAction->setAccel( CTRL+SHIFT+Key_S );
  connect( SimulationAction, SIGNAL( activated() ), 
	   this, SLOT( startSimulation() ) );
  SimulationAction->addTo( filemenu );

  IdleAction = new QAction( this );
  IdleAction->setMenuText( "&Idle (stop)" );
  IdleAction->setAccel( CTRL+SHIFT+Key_I );
  connect( IdleAction, SIGNAL( activated() ),
	   this, SLOT( stopActivity() ) );
  IdleAction->addTo( filemenu );

  filemenu->insertSeparator();

  SN->addActions( filemenu );
  MTDT.addActions( filemenu );
  filemenu->insertSeparator();
  filemenu->insertItem( "Settings...", &SS, SLOT( dialog() ) );
  filemenu->insertItem( "Save Settings", this, SLOT( saveConfig() ) );
  filemenu->insertItem( "&Quit", this, SLOT( quit() ), ALT+Key_Q );
  menuBar()->insertItem( "&File", filemenu );

  // plugins:
  QPopupMenu *pluginmenu = new QPopupMenu( this );
  if ( MD != 0 ) {
    MD->addActions( pluginmenu );
    pluginmenu->insertSeparator();
  }
  if ( ! CN.empty() ) {
    for ( unsigned int k=0; k<CN.size(); k++ )
      CN[k]->addActions( pluginmenu );
  }
  menuBar()->insertItem( "&Plugins", pluginmenu );

  // devices:
  DeviceMenu = new QPopupMenu( this );
  menuBar()->insertItem( "De&vices", DeviceMenu );

  // filters:
  menuBar()->insertItem( "&Detectors/Filters", FD->menu() );

  // repros:
  menuBar()->insertItem( "&RePros", RP->menu() );

  // macros:
  menuBar()->insertItem( "&Macros", MC->menu() );

  // view:
  QPopupMenu *viewmenu = new QPopupMenu;

  FullscreenAction = new QAction( this );
  FullscreenAction->setMenuText( "&Full-Screen Mode" );
  FullscreenAction->setAccel( CTRL+SHIFT+Key_F );
  connect( FullscreenAction, SIGNAL( activated() ), this, SLOT( fullScreen() ) );
  FullscreenAction->addTo( viewmenu );

  viewmenu->insertSeparator();
  PT->addMenu( viewmenu );

  menuBar()->insertItem( "&View", viewmenu );

  // help:
  popup = new QPopupMenu;
  popup->insertItem( "&Help...", this, SLOT( help() ) );
  popup->insertItem( "&About...", this, SLOT( about() ) );
  menuBar()->insertItem( "&Help", popup );

  // layout:
  /*
  int wd = FD->minimumSizeHint().width();
  int wc = cw->minimumSizeHint().width();
  */
  int wd = FD->sizeHint().width();
  int wc = cw->sizeHint().width();
  int w = wc > wd ? wc : wd;
  FD->setMaximumWidth( w );
  cw->setMaximumWidth( w );
  mainlayout->addWidget( FD, 0, 0 );
  mainlayout->addWidget( PT, 0, 1 );
  mainlayout->addWidget( cw, 1, 0 );
  mainlayout->addWidget( RP, 1, 1 );
  mainlayout->addMultiCellWidget( MC, 2, 2, 0, 1 );
  mainlayout->setColStretch( 0, 1 );
  mainlayout->setColStretch( 1, 100 );
  mainlayout->setRowStretch( 0, 2 );
  mainlayout->setRowStretch( 1, 3 );
  wd = FD->width();
  wc = cw->width();
  w = wc > wd ? wc : wd;
  w = 3*w/2;
  if ( PT->minimumWidth() < w || RP->minimumWidth() < w ) {
    PT->setMinimumWidth( w );
    RP->setMinimumWidth( w );
  }

  if ( splash != 0 )
    msleep( 2000 );

  // miscellaneous:
  setFocusPolicy( QWidget::StrongFocus );
  KeyTime = new KeyTimeOut( topLevelWidget() );

}


RELACSWidget::~RELACSWidget( void )
{
  stopThreads();
  saveConfig();
  Plugins::close();
}


void RELACSWidget::lockGUI( void )
{
  qApp->lock(); 
}


void RELACSWidget::unlockGUI( void )
{
  qApp->unlock(); 
}


void RELACSWidget::printlog( const string &message ) const
{
  cerr << QTime::currentTime().toString() << " "
       << message << endl;
  if ( LogFile != 0 )
    *LogFile << QTime::currentTime().toString() << " "
	     << message << endl;
}


void RELACSWidget::init ( void )
{
  MC->warning();
  if ( MC->fatal() ) {
    ::exit( 1 );
  }

  QFont f( fontInfo().family(), fontInfo().pointSize()*4/3, QFont::Bold );
  SimLabel->setFont( f );
  SimLabel->setFixedWidth( SimLabel->fontMetrics().boundingRect( "100%" ).width() + 8 );

  MTDT.clear();
  if ( acquisition() )
    startFirstAcquisition();
  else if ( simulation() )
    startFirstSimulation();
}


int RELACSWidget::setupHardware( int n )
{
  Str warnings;
  bool Fatal = false;

  // activate devices:
  DV->create( *ADV, n );
  warnings += DV->warnings();

  // activate analog input devices:
  if ( n == 0 )
    AID->create( *ADV, n );
  else
    AID->create( *ADV, 1, "AISim" );
  warnings += AID->warnings();
  if ( ! AID->ok() ) {
    Fatal = true;
    warnings += "No analog input device opened!\n";
  }

  // activate analog output devices:
  if ( n == 0 )
    AOD->create( *ADV, n );
  else
    AOD->create( *ADV, 1, "AOSim" );
  warnings += AOD->warnings();
  if ( ! AOD->ok() ) {
    Fatal = true;
    warnings += "No analog output device opened!\n";
  }

  // activate digital I/O devices:
  DIOD->create( *ADV, n );
  warnings += DIOD->warnings();

  // activate attenuators:
  if ( n == 0 )
    ATD->create( *ADV, n );
  else
    ATD->create( *ADV, 1, "AttSim" );
  warnings += ATD->warnings();
  if ( ! ATD->ok() )
    Fatal = true;

  ATI->create( *ADV, 0 );
  warnings += ATI->warnings();
  if ( ! ATI->ok() )
    Fatal = true;

  if ( !warnings.empty() ) {
    Str ws = "Errors in activating devices:\n";
    warnings.insert( 0, "<ul><li>" );
    int p = warnings.find( "\n" );
    while ( p >= 0 ) {
      warnings.insert( p, "</li>" );
      p += 6;
      int n = warnings.find( "\n", p );
      if ( n < 0 )
	break;
      warnings.insert( p, "<li>" );
      p = n + 4;
    }
    ws += warnings + "</ul>";
    if ( Fatal ) {
      ws += "Can't switch to <b>" + modeStr() + "</b>-mode!";
      printlog( "! warning: " + ws.erasedMarkup() );
      MessageBox::warning( "RELACS Warning !", ws, true, 0.0, this );
    }
    else {
      printlog( ws.erasedMarkup() );
      MessageBox::information( "RELACS Info !", ws, false, 0.0, this );
    }
  }

  // setup Acquire:
  if ( ! Fatal ) {

    AQ = n == 0 ? AQD : SIM;

    for ( int k=0; k<AID->size(); k++ ) {
      AQ->addInput( &(*AID)[k], InData::RefCommon );
    }
    for ( int k=0; k<AOD->size(); k++ ) {
      AQ->addOutput( &(*AOD)[k] );
    }

    for ( int k=0; k<ATI->size(); k++ ) {
      AQ->addAttLine( &(*ATI)[k] );
      (*ATI)[k].init();
    }

    AQ->setAdjustFlag( 4096 );

    AQ->init();
    printlog( "Synchronization method: " + AQ->syncModeStr() );

    if ( n == 1 ) {
      connect( &SimLoad, SIGNAL( timeout() ), this, SLOT( simLoadMessage() ) );
      SimLoad.start( 1000, false );
    }

    int menuindex = 0;
    DV->addMenu( DeviceMenu, menuindex );
    AID->addMenu( DeviceMenu, menuindex );
    AOD->addMenu( DeviceMenu, menuindex );
    ATD->addMenu( DeviceMenu, menuindex );
    ATI->addMenu( DeviceMenu, menuindex );

    return 0;
  }
  else {

    // clear devices:
    ADV->clear();
    DV->clear();
    ATD->clear();
    ATI->clear();
    AID->clear();
    AOD->clear();
    AQ = 0;

    return 1;
  }
}


void RELACSWidget::clearHardware( void )
{
  AQD->clear();
  SIM->clear();
  ADV->clear();
  DV->clear();
  ATD->clear();
  ATI->clear();
  AID->clear();
  AOD->clear();
}


void RELACSWidget::closeHardware( void )
{
  ADV->close();
  DeviceMenu->clear();
}


void RELACSWidget::setupInTraces( void )
{
  IL.clear();
  int nid = integer( "inputtraces", 0, 1 );
  for ( int k=0; k<nid; k++ ) {
    InData id;
    IL.push( id );
    IL[k].setIdent( text( "inputtraceid", k, "trace-" + Str( k+1 ) ) );
    IL[k].setUnit( number( "inputtracescale", k, 1.0 ),
		   text( "inputtraceunit", k, "" ) );
    IL[k].setSampleRate( number( "inputsamplerate", 1000.0 ) );
    IL[k].setStartSource( 0 );
    IL[k].setUnipolar( boolean( "inputunipolar", false ) );
    IL[k].setChannel( integer( "inputtracechannel", k, k ) );
    IL[k].setDevice( integer( "inputtracedevice", k, 0 ) );
    IL[k].setContinuous();
    IL[k].setMode( SaveFiles::SaveTrace | PlotTraceMode );
    IL[k].setReference( text( "inputtracereference", k, InData::referenceStr( InData::RefGround ) ) );
    IL[k].setGainIndex( integer( "inputtracegain", k, 0 ) );
    IL[k].reserve( IL[k].indices( number( "inputtracecapacity", 0, 1000.0 ) ) );
    TraceStyles.push_back( PlotTrace::TraceStyle() );
    TraceStyles[k].PlotWindow = integer( "inputtraceplot", k, k );
  }
}


void RELACSWidget::setupOutTraces( void )
{
  AQ->clearOutTraces();
  OutData::setDefaultMaxSampleRate( number( "maxoutputrate", 100000.0 ) );
  int nod = Options::size( "outputtraceid" );
  int chan = 0;
  int lastdi = 0;
  for ( int k=0; k<nod; k++ ) {
    Str ds = text( "outputtracedevice", k, "" );
    int di = (int)::rint( ds.number( -1 ) );
    if ( di < 0 || di >= AQ->outputsSize() )
      di = AQ->outputIndex( ds );
    if ( di != lastdi ) {
      lastdi = di;
      chan = 0;
    }
    AQ->addOutTrace( text( "outputtraceid", k, "out-" + Str( k+1 ) ),
		     di,
		     integer( "outputtracechannel", k, chan ),
		     number( "outputtracescale", k, 1.0 ),
		     text( "outputtraceunit", k, "V" ),
		     false,
		     number( "outputtracemaxrate", k, 0.0 ),
		     number( "outputtracedelay", k, 0.0 ) );
    chan++;
  }

  AQ->addOutTraces();
}


///// Data thread ///////////////////////////////////////////////////////////

void RELACSWidget::updateData( void )
{
  // read data:
  writeLockData();
  lockAI();
  AQ->convertData();
  unlockAI();
  // do we need to wait for more data?
  MinTraceMutex.lock();
  double mintime = MinTraceTime;
  MinTraceMutex.unlock();
  while ( IL.success() && IL[0].currentTime() < mintime ) {
    if ( acquisition() )
      ReadDataWait.wait();
    else
      ReadDataWait.wait( 1 );
    lockAI();
    AQ->convertData();
    unlockAI();
  }
  setMinTraceTime( 0.0 );
  // update derived data:
  AQ->readSignal( SignalTime, IL, ED ); // we probably get the latest signal start here
  AQ->readRestart( IL, ED );
  ED.setRangeBack( IL[0].currentTime() );
  Str fdw = FD->filter( IL, ED );
  if ( !fdw.empty() )
    printlog( "! error: " + fdw.erasedMarkup() );
  unlockData();
}


void RELACSWidget::processData( void )
{
  readLockData();
  SF->save( IL, ED );
  unlockData();
  PT->plot( IL, ED );
}


void RELACSWidget::run( void )
{
  bool rd = true;
  double updateinterval = IL[0].updateTime();
  signed long ui = (unsigned long)::rint( 1000.0*updateinterval );
  QTime updatetime;
  updatetime.start();
  QThread::msleep( 1 );

  do {
    int ei = updatetime.elapsed();
    int di = ui - ei;
    if ( di < 2 )
      di = 2;
    ThreadSleepWait.wait( di );
    updatetime.restart();
    updateData();
    processData();
    DataSleepWait.wakeAll();
    RunDataMutex.lock();
    rd = RunData;
    RunDataMutex.unlock();
  } while( rd );
}


void RELACSWidget::setMinTraceTime( double t )
{
  MinTraceMutex.lock();
  MinTraceTime = t;
  MinTraceMutex.unlock();
}


void RELACSWidget::wakeAll( void )
{
  DataSleepWait.wakeAll();
  ReProSleepWait.wakeAll();
  ReProAfterWait.wakeAll();
  SessionStartWait.wakeAll();
  SessionStopWait.wakeAll();
  SessionPrestopWait.wakeAll();
}


void RELACSWidget::simLoadMessage( void )
{
  if ( MD != 0 ) {
    SimLabel->setText( string( Str( 100.0*MD->load(), 0, 0, 'f' ) + "%" ).c_str() );
  }
}


////// RePros ///////////////////////////////////////////////////////////

void RELACSWidget::activateGains( void )
{
  writeLockData();
  lockAI();
  AQ->activateGains();
  unlockAI();
  AQ->readRestart( IL, ED );
  FD->adjust( IL, ED, AQ->adjustFlag() );
  unlockData();
}


int RELACSWidget::write( OutData &signal )
{
  if ( AQ->readSignal( SignalTime, IL, ED ) || // we should get the start time of the latest signal here
       SF->signalPending() )                   // the signal time might not have been transferred to SF
    SF->save( IL, ED );
  /*
  // last stimulus still not saved?
  if ( SF->signalPending() ) {
    CurrentRePro->unlockAll();
    // force data updates:
    ThreadSleepWait.wakeAll();
    do {
      // wait for data updates:
      DataSleepWait.wait();
    } while ( SF->signalPending() );
    CurrentRePro->lockAll();
  }
  */
  if ( SF->signalPending() )
    printlog( "! warning in write() -> previous signal still pending in SaveFiles !" );
  lockSignals();
  lockAI();
  int r = AQ->write( signal );
  unlockAI();
  unlockSignals();
  if ( r == 0 ) {
    WriteLoop.start( signal.writeTime() );
    lockSignals();
    SF->save( signal );
    unlockSignals();
    lockAI();
    AQ->readSignal( SignalTime, IL, ED ); // if acquisition was restarted we here get the signal start
    unlockAI();
    AQ->readRestart( IL, ED );
    // update device menu:
    QApplication::postEvent( this, new QCustomEvent( QEvent::User+2 ) );
  }
  else
    printlog( "! failed to write signal: " + signal.errorText() );
  if ( IL.failed() )
    printlog( "! error in restarting analog input: " + IL.errorText() );
  return r;
}


int RELACSWidget::write( OutList &signal )
{
  if ( AQ->readSignal( SignalTime, IL, ED ) || // we should get the start time of the latest signal here
       SF->signalPending() )                   // the signal time might not have been transferred to SF
    SF->save( IL, ED );
  /*
  // last stimulus still not saved?
  if ( SF->signalPending() ) {
    CurrentRePro->unlockAll();
    // force data updates:
    ThreadSleepWait.wakeAll();
    do {
      // wait for data updates:
      DataSleepWait.wait();
    } while ( SF->signalPending() );
    CurrentRePro->lockAll();
  }
  */
  if ( SF->signalPending() )
    printlog( "! warning in write() -> previous signal still pending in SaveFiles !" );
  lockSignals();
  lockAI();
  int r = AQ->write( signal );
  unlockAI();
  unlockSignals();
  if ( r == 0 ) {
    WriteLoop.start( signal[0].writeTime() );
    lockSignals();
    SF->save( signal );
    unlockSignals();
    lockAI();
    AQ->readSignal( SignalTime, IL, ED ); // if acquisition was restarted we here get the signal start
    unlockAI();
    AQ->readRestart( IL, ED );
    // update device menu:
    QApplication::postEvent( this, new QCustomEvent( QEvent::User+2 ) );
  }
  else
    printlog( "! failed to write signals: " + signal.errorText() );
  if ( IL.failed() )
    printlog( "! error in restarting analog input: " + IL.errorText() );
  return r;
}


int RELACSWidget::directWrite( OutData &signal )
{
  if ( AQ->readSignal( SignalTime, IL, ED ) || // we should get the start time of the latest signal here
       SF->signalPending() )                   // the signal time might not have been transferred to SF
    SF->save( IL, ED );
  /*
  // last stimulus still not saved?
  if ( SF->signalPending() ) {
    CurrentRePro->unlockAll();
    // force data updates:
    ThreadSleepWait.wakeAll();
    do {
      // wait for data updates:
      DataSleepWait.wait();
    } while ( SF->signalPending() );
    CurrentRePro->lockAll();
  }
  */
  if ( SF->signalPending() )
    printlog( "! warning in write() -> previous signal still pending in SaveFiles !" );
  lockSignals();
  lockAI();
  int r = AQ->directWrite( signal );
  unlockAI();
  unlockSignals();
  if ( r == 0 ) {
    lockSignals();
    SF->save( signal );
    unlockSignals();
    lockAI();
    AQ->readSignal( SignalTime, IL, ED ); // if acquisition was restarted we here get the signal start
    unlockAI();
    AQ->readRestart( IL, ED );
    // update device menu:
    QApplication::postEvent( this, new QCustomEvent( QEvent::User+2 ) );
  }
  else
    printlog( "! failed to write signal: " + signal.errorText() );
  if ( IL.failed() )
    printlog( "! error in restarting analog input: " + IL.errorText() );
  return r;
}


int RELACSWidget::directWrite( OutList &signal )
{
  if ( AQ->readSignal( SignalTime, IL, ED ) || // we should get the start time of the latest signal here
       SF->signalPending() )                   // the signal time might not have been transferred to SF
    SF->save( IL, ED );
  /*
  // last stimulus still not saved?
  if ( SF->signalPending() ) {
    CurrentRePro->unlockAll();
    // force data updates:
    ThreadSleepWait.wakeAll();
    do {
      // wait for data updates:
      DataSleepWait.wait();
    } while ( SF->signalPending() );
    CurrentRePro->lockAll();
  }
  */
  if ( SF->signalPending() )
    printlog( "! warning in write() -> previous signal still pending in SaveFiles !" );
  lockSignals();
  lockAI();
  int r = AQ->directWrite( signal );
  unlockAI();
  unlockSignals();
  if ( r == 0 ) {
    lockSignals();
    SF->save( signal );
    unlockSignals();
    lockAI();
    AQ->readSignal( SignalTime, IL, ED ); // if acquisition was restarted we here get the signal start
    unlockAI();
    AQ->readRestart( IL, ED );
    // update device menu:
    QApplication::postEvent( this, new QCustomEvent( QEvent::User+2 ) );
  }
  else
    printlog( "! failed to write signals: " + signal.errorText() );
  if ( IL.failed() )
    printlog( "! error in restarting analog input: " + IL.errorText() );
  return r;
}


void RELACSWidget::noSaving( void )
{
  SF->save( false, IL, ED );
}


void RELACSWidget::plotToggle( bool on, bool fixed, double length, double offs )
{
  PT->setState( on, fixed, length, offs );
}


void RELACSWidget::notifyStimulusData( void )
{
  if ( MD != 0 )
    MD->notifyStimulusData();
  FD->notifyStimulusData();
  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->notifyStimulusData();
  RP->notifyStimulusData();
}


void RELACSWidget::notifyMetaData( const string &section )
{
  if ( MD != 0 )
    MD->notifyMetaData( section );
  FD->notifyMetaData( section );
  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->notifyMetaData( section );
  RP->notifyMetaData( section );
}


void RELACSWidget::saveDoxygenOptions( void )
{
  for ( int k=0; k < Plugins::plugins(); k++ ) {
    if ( Plugins::type( k ) == RELACSPlugin::ModelId ||
	 Plugins::type( k ) == RELACSPlugin::FilterId ||
	 Plugins::type( k ) == RELACSPlugin::ControlId ||
	 Plugins::type( k ) == RELACSPlugin::ReProId ) {
      cout << "\n";
      RELACSPlugin *rxp = (RELACSPlugin *)Plugins::create( k );
      if ( Plugins::type( k ) == RELACSPlugin::ModelId )
	cout << "Model ";
      else if ( Plugins::type( k ) == RELACSPlugin::FilterId )
	cout << "Filter ";
      else if ( Plugins::type( k ) == RELACSPlugin::ReProId )
	cout << "RePro ";
      else if ( Plugins::type( k ) == RELACSPlugin::ControlId )
	cout << "Control ";
      else
	cout << "! Invalid Plugin Type ";
      cout << rxp->name() << " [" << rxp->pluginSet() << "]\n";
      cout << "\\author " << rxp->author() << '\n';
      cout << "\\version " << rxp->version() << " (" << rxp->date() << ")\n";
      cout << "\\par Options\n";
      rxp->save( cout, "- \\c %i=%s: %r (\\c %t)\n", "- \\c %i=%g%u: %r (\\c %t)\n", "- \\c %i=%b: %r (\\c %t)\n", "%i\n" );
      delete rxp;
      Plugins::destroy( k );
    }
  }
  ::exit( 0 );
}


void RELACSWidget::checkPlugin( void* plugin, const string &ident )
{
  if ( plugin == 0 ) {
    Str es = "Plugin \"<b>";
    es += ident;
    es += "</b>\" not found!<br>\n";
    es += "Exit now!\n";
    printlog( es.erasedMarkup() );
    MessageBox::error( "RELACS Error !", es, this );
    qApp->exit( 1 );
    ::exit( 1 ); // do we need that?
  }
}


//-----------------------RePro and Macro Handling--------------------------//


void RELACSWidget::startRePro( RePro *repro, int macroaction, bool saving )
{
  // start new RePro:
  CurrentRePro = repro;
  RP->activateRePro( CurrentRePro, macroaction );

  // update info file:
  printlog( "starting RePro \"" + CurrentRePro->name() + "\"" );
  if ( ! InfoFileMacro.empty() ) {
    if ( InfoFile != 0 )
      *InfoFile << InfoFileMacro << '\n';
    InfoFileMacro = "";
  }
  if ( InfoFile != 0 ) {
    *InfoFile << QTime::currentTime().toString();
    *InfoFile << "   " << CurrentRePro->name() << ": " << MC->options();
  }

  ReProRunning = true;

  readLockData();
  SF->save( saving, IL, ED );
  SF->save( *CurrentRePro );
  CurrentRePro->setSaving( SF->saving() );
  unlockData();
  CurrentRePro->start( HighPriority );
}


void RELACSWidget::stopRePro( void )
{
  if ( ! ReProRunning )
    return;

  // wait on RePro to stop:
  if ( CurrentRePro->running() ) {
    // wait for the RePro to leave sensible code:
    CurrentRePro->lock();

    // dispatch all posted events (that usually paint the RePro...)
    // as long as the RePro is normally running, so that it has
    // still all internal variables available:
    QApplication::sendPostedEvents();
    qApp->processEvents( 100 );

    // request and wait for the RePro to properly terminate:
    CurrentRePro->requestStop();
    CurrentRePro->unlock();
    CurrentRePro->wait();
  }

  // stop analog output:
  WriteLoop.stop();
  lockSignals();
  AQ->stopWrite();                
  unlockSignals();

  ReProRunning = false;

  if ( AQ->readSignal( SignalTime, IL, ED ) ) // we should get the start time of the latest signal here
    SF->save( IL, ED );
  // last stimulus still not saved?
  if ( SF->signalPending() ) {
    // force data updates:
    ThreadSleepWait.wakeAll();
    DataSleepWait.wait();
    SF->clearSignal();
  }

  // update Session:
  ReProAfterWait.wakeAll();

  if ( InfoFile != 0 ) {
    if ( CurrentRePro->aborted() )
      *InfoFile << " (user interrupted)" << endl;
    else
      *InfoFile << ( CurrentRePro->completed() ? " (completed)" :  " (failed)" ) << endl;
  }
  if ( ! InfoFileMacro.empty() ) {
    if ( InfoFile != 0 )
      *InfoFile << InfoFileMacro << '\n';
    InfoFileMacro = "";
  }
}


void RELACSWidget::customEvent( QCustomEvent *qce )
{
  if ( qce->type() == QEvent::User+1 ) {
    MC->startNextRePro( true );
  }
  else if ( qce->type() == QEvent::User+2 ) {
    AOD->updateMenu();
    ATD->updateMenu();
    ATI->updateMenu();
  }
  else if ( qce->type() == QEvent::User+3 ) {
    MessageBox::error( "RELACS Error !", "Transfering stimulus data to hardware driver failed.", this );
  }
}


void RELACSWidget::updateRePro( void )
{
  ReProSleepWait.wakeAll();
}


void RELACSWidget::startedMacro( const string &ident, const string &param )
{
  RP->setMacro( ident, param );
  InfoFileMacro = "---------> Macro " + ident;
  if ( ! param.empty() )
    InfoFileMacro += ": " + param;
  printlog( "starting Macro \"" + ident + "\"" );
}


void RELACSWidget::startSession( bool startmacro )
{
  stopRePro();

  printlog( "start new session" );

  // open files:
  SF->openFiles( IL, ED );

#if QT_VERSION >= 300
  MainWidget->setEraseColor( QColor( 255, 96, 96 ) );
#else
  MainWidget->setBackgroundColor( QColor( 255, 96, 96 ) );
#endif

  SS.lock();
  if ( SS.boolean( "saverelacscore" ) )
    CFG.save( RELACSPlugin::Core, SF->addPath( "relacs.cfg" ) );
  if ( SS.boolean( "saverelacsplugins" ) )
    CFG.save( RELACSPlugin::Plugins, SF->addPath( "relacsplugins.cfg" ) );
  if ( SS.boolean( "saveattenuators" ) )
    ATI->save( SF->path() );
  if ( SS.boolean( "saverelacslog" ) ) {
    LogFile = new ofstream( SF->addPath( "relacs.log" ).c_str() );
    if ( ! LogFile->good() ) {
      printlog( "! warning: LogFile not good" );
      delete LogFile;
      LogFile = 0;
    }
    else {
      *LogFile << "RELACS, Version " << RELACSVERSION << "\n";
    }
  }
  SS.unlock();

  InfoFile = new ofstream( SF->addPath( "repros.dat" ).c_str() );
  if ( ! InfoFile->good() ) {
    printlog( "! warning: InfoFile not good" );
    delete InfoFile;
    InfoFile = 0;
  }
  else {
    *InfoFile << "This is RELACS, Version " << RELACSVERSION << "\n\n";
    *InfoFile << "The session was started at time "
	     << QTime::currentTime().toString() << " on "
	     << QDate::currentDate().toString() << "\n\n"
	     << "Time:      Research Program:\n";
  }

  SessionStartWait.wakeAll();

  if ( MD != 0 )
    MD->sessionStarted();
  FD->sessionStarted();
  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->sessionStarted();
  RP->sessionStarted();

  if ( startmacro )
    MC->startSession( true );
}


void RELACSWidget::preStopSession( void )
{
  SessionPrestopWait.wakeAll();
  MC->fallBack( false );
}


void RELACSWidget::resumeSession( void )
{
  printlog( "resume session" );
}


void RELACSWidget::stopSession( bool saved )
{
  printlog( "stop session" );

  MTDT.save();

  if ( saved )
    SF->completeFiles();
  else 
    SF->deleteFiles();

  if ( MD != 0 )
    MD->sessionStopped( saved );
  FD->sessionStopped( saved );
  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->sessionStopped( saved );
  RP->sessionStopped( saved );

  CurrentRePro->setSaving( SF->saving() );

#if QT_VERSION >= 300
  MainWidget->setEraseColor( OrgBackground );
#else
  MainWidget->setBackgroundColor( OrgBackground );
#endif

  if ( LogFile != 0 ) {
    delete LogFile;
    LogFile = 0;
  }

  if ( InfoFile != 0 ) {
    *InfoFile << "\n\n"
	     << "The session was stopped at time " 
	     << QTime::currentTime().toString() << " on "
	     << QDate::currentDate().toString() << '\n';
    delete InfoFile;
    InfoFile = 0;
  }

  SessionStopWait.wakeAll();

  if ( MC->stopSessionIndex() >= 0 && saved )
    MC->stopSession( false );

  SF->setPath( SF->defaultPath() );
}


//------------------------------Quit and Exit------------------------------//


void RELACSWidget::saveConfig( void )
{
  CFG.save();
}


void RELACSWidget::stopThreads( void )
{
  // stop any still running RePro:
  stopRePro();

  // stop control threads:
  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->requestStop();
  wakeAll();
  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->QThread::wait( 200 );

  // stop data threads:
  ReadLoop.stop();
  WriteLoop.stop();
  RunDataMutex.lock();
  RunData = false;
  RunDataMutex.unlock();
  QThread::wait();

  // stop simulation and data acquisition:
  SimLoad.stop();
  if ( AQ != 0 ) {
    lockAI();
    lockSignals();
    AQ->stop();
    unlockSignals();
    unlockAI();
  }

  // process pending events posted from threads.
  qApp->processEvents();
}


void RELACSWidget::stopActivity( void )
{
  printlog( "Stopping " + modeStr() + "-mode" );

  // finish session and repro:
  SN->stopTheSession();
  // shutdown macros:
  if ( MC->shutDownIndex() >= 0 )
    MC->shutDown();

  stopThreads();

  closeHardware();

  RP->activateRePro( 0 );

  AcquisitionAction->setEnabled( true );
  SimulationAction->setEnabled( true );
  IdleAction->setEnabled( false );
  setMode( IdleMode );
  RP->message( "<b>Idle-mode</b>" );
}


void RELACSWidget::quit( void )
{
  stopActivity();
  printlog( "quitting RELACS" );
  qApp->quit();
}


void RELACSWidget::closeEvent( QCloseEvent *ce )
{
  quit();
  ce->accept();
}


///// working mode: ////////////////////////////////////////////////////////

RELACSWidget::ModeTypes RELACSWidget::mode( void ) const
{
  return Mode;
}


string RELACSWidget::modeStr( void ) const
{
  return ModeStr[mode()];
}


bool RELACSWidget::acquisition( void ) const
{
  return ( Mode == AcquisitionMode );
}


bool RELACSWidget::simulation( void ) const
{
  return ( Mode == SimulationMode );
}


bool RELACSWidget::browsing( void ) const
{
  return ( Mode == BrowseMode );
}


bool RELACSWidget::analysis( void ) const
{
  return ( Mode == AnalysisMode );
}


bool RELACSWidget::idle( void ) const
{
  return ( Mode == IdleMode );
}


void RELACSWidget::setMode( ModeTypes mode )
{
  Mode = mode;
  if ( MD != 0 )
    MD->modeChanged();
  FD->modeChanged();
  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->modeChanged();
  SN->modeChanged();
  RP->modeChanged();
}


void RELACSWidget::clearActivity( void )
{
  CFG.save();
  clearHardware();
  IL.clearBuffer();
  ED.clear();
  SimLabel->hide();
  MTDT.clear();
}


void RELACSWidget::startAcquisition( void )
{
  if ( ! idle() )
    stopActivity();
  clearActivity();
  startFirstAcquisition();
}


void RELACSWidget::startFirstAcquisition( void )
{
  setMode( AcquisitionMode );

  // hardware:
  if ( setupHardware( 0 ) ) {
    startIdle();
    return;
  }

  // analog input and output traces:
  SignalTime = -1.0;
  setupInTraces();
  setupOutTraces();

  // events:
  FD->clearIndices();
  FD->createStimulusEvents( IL, ED, EventStyles );
  FD->createRestartEvents( IL, ED, EventStyles );
  FD->createRecordingEvents( IL, ED, EventStyles );
  Str fdw = FD->createTracesEvents( IL, ED, TraceStyles, EventStyles );
  if ( !fdw.empty() ) {
    printlog( "! error: " + fdw.erasedMarkup() );
    MessageBox::error( "RELACS Error !", fdw, this );
    startIdle();
    return;
  }

  // files:
  SF->setPath( SF->defaultPath() );

  // plot:
  PT->resize( IL, ED );
  PT->updateMenu();

  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->initialize();

  // XXX before configuring, something like AQ->init would be nice
  // in order to set the IL gain factors.
  CFG.read( RELACSPlugin::Plugins );
  CFG.configure( RELACSPlugin::Plugins );

  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->initDevices();

  // start data aquisition:
  setMinTraceTime( 0.0 );
  lockAI();
  AQ->setBufferTime( SS.number( "readinterval", 0.01 ) );
  AQ->setUpdateTime( SS.number( "processinterval", 0.1 ) );
  int r = AQ->read( IL );
  unlockAI();
  if ( r < 0 ) {
    printlog( "! error in starting data acquisition: " + IL.errorText() );
    MessageBox::warning( "RELACS Warning !",
			 "error in starting data acquisition: " + IL.errorText(),
			 true, 0.0, this );
    startIdle();
    return;
  }

  // reset analog output for dynamic clamp:
  lockAI();
  r = AQ->writeReset( true, true );
  unlockAI();
  if ( r < 0 )
    printlog( "! warning: RELACSWidget::startFirstAcquisition() -> resetting analog output failed" );

  AQ->readRestart( IL, ED );
  AID->updateMenu();

  fdw = FD->init( IL, ED );  // init filters/detectors before RePro!
  if ( ! fdw.empty() ) {
    printlog( "! error in initializing filter: " + fdw.erasedMarkup() );
    MessageBox::warning( "RELACS Warning !",
			 "error in initializing filter: " + fdw,
			 true, 0.0, this );
    startIdle();
    return;
  }

  ReadLoop.start();
  RunDataMutex.lock();
  RunData = true;
  RunDataMutex.unlock();
  QThread::start( HighPriority );

  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->start();

  // get first RePro and start it:
  MC->startUp( false ); 

  AcquisitionAction->setEnabled( false );
  SimulationAction->setEnabled( true );
  IdleAction->setEnabled( true );

  printlog( "Acquisition-mode started" );
}


void RELACSWidget::startSimulation( void )
{
  if ( ! idle() )
    stopActivity();
  clearActivity();
  startFirstSimulation();
}


void RELACSWidget::startFirstSimulation( void )
{
  setMode( SimulationMode );

  // hardware:
  if ( setupHardware( 1 ) ) {
    startIdle();
    return;
  }

  // analog input and output traces:
  SignalTime = -1.0;
  setupInTraces();
  setupOutTraces();

  // events:
  FD->clearIndices();
  FD->createStimulusEvents( IL, ED, EventStyles );
  FD->createRestartEvents( IL, ED, EventStyles );
  FD->createRecordingEvents( IL, ED, EventStyles );
  Str fdw = FD->createTracesEvents( IL, ED, TraceStyles, EventStyles );
  if ( !fdw.empty() ) {
    printlog( "! error: " + fdw.erasedMarkup() );
    MessageBox::error( "RELACS Error !", fdw, this );
    startIdle();
    return;
  }

  // files:
  SF->setPath( SF->defaultPath() );

  // plots:
  PT->resize( IL, ED );
  PT->updateMenu();

  SimLabel->setText( "" );
  SimLabel->show();

  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->initialize();

  // XXX before configuring, something like AQ->init would be nice
  // in order to set the IL gain factors.
  CFG.read( RELACSPlugin::Plugins );
  CFG.configure( RELACSPlugin::Plugins );

  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->initDevices();

  // start data aquisition:
  setMinTraceTime( 0.0 );
  lockAI();
  AQ->setBufferTime( SS.number( "readinterval", 0.01 ) );
  AQ->setUpdateTime( SS.number( "processinterval", 0.1 ) );
  int r = AQ->read( IL );
  unlockAI();
  if ( r < 0 ) {
    // give it a second chance with the adjusted input parameter:
    lockAI();
    r = AQ->read( IL );
    unlockAI();
    if ( r < 0 ) {
      printlog( "! error in starting data acquisition: " + IL.errorText() );
      MessageBox::warning( "RELACS Warning !",
			   "error in starting data acquisition: " + IL.errorText(),
			   true, 0.0, this );
      startIdle();
      return;
    }
  }
  AQ->readRestart( IL, ED );

  AID->updateMenu();

  // check success:
  for ( int k=0; k<IL.size(); k++ ) {
    if ( IL[k].failed() ) {
      printlog( "error in starting simulation of trace " + IL[k].ident() + ": "
		+ IL[k].errorText() );
      stopActivity();
      return;
    }
  }

  fdw = FD->init( IL, ED );  // init filters/detectors before RePro!
  if ( ! fdw.empty() ) {
    printlog( "! error in initializing filter: " + fdw.erasedMarkup() );
    MessageBox::warning( "RELACS Warning !",
			 "error in initializing filter: " + fdw,
			 true, 0.0, this );
    startIdle();
    return;
  }

  RunDataMutex.lock();
  RunData = true;
  RunDataMutex.unlock();
  QThread::start( HighPriority );

  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->start();

  // get first RePro and start it:
  MC->startUp( false ); 

  AcquisitionAction->setEnabled( true );
  SimulationAction->setEnabled( false );
  IdleAction->setEnabled( true );

  printlog( "Simulation-mode started" );
}


void RELACSWidget::startIdle( void )
{
  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->initialize();
  CFG.read( RELACSPlugin::Plugins );
  CFG.configure( RELACSPlugin::Plugins );
  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->initDevices();
  //  IL->clearBuffer();
  //  ED.clear();
  RP->activateRePro( 0 );
  AcquisitionAction->setEnabled( true );
  SimulationAction->setEnabled( true );
  IdleAction->setEnabled( false );
  setMode( IdleMode );
  RP->message( "<b>Idle-mode</b>" );
}



//-------------------------Keyboard Interaction----------------------------//

void RELACSWidget::keyPressEvent( QKeyEvent* e)
{
  if ( CurrentRePro != 0 )
    CurrentRePro->keyPressEvent( e );
  PT->keyPressEvent( e );
  MC->keyPressEvent( e );
  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->keyPressEvent( e );
}


void RELACSWidget::keyReleaseEvent( QKeyEvent* e)
{
  if ( CurrentRePro != 0 )
    CurrentRePro->keyReleaseEvent( e );
  PT->keyReleaseEvent( e );
  MC->keyReleaseEvent( e );
  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->keyReleaseEvent( e );
}


//----------------------------Private Functions----------------------------//


void RELACSWidget::fullScreen( void )
{
  if ( IsFullScreen ) {
    showNormal();
    IsFullScreen = false;
    FullscreenAction->setMenuText( "&Full-Screen Mode" );
  }
  else {
    showFullScreen();
    IsFullScreen = true;
    FullscreenAction->setMenuText( "Exit &Full-Screen Mode" );
  }
}


void RELACSWidget::about()
{
  Str info = "<p align=center>This is</p>";
  info += "<p align=center><b>RELACS</b></p>";
  info += "<p align=center><b>R</b>elaxed <b>El</b>ectrophysiological Data <b>A</b>cquisition, <b>C</b>ontrol, and <b>S</b>timulation</p>\n";
  info += "<p align=center>Version ";
  info += RELACSVERSION;
  info += "</p>\n";
  info += "<p align=center>(c) by Jan Benda</p>\n";
  info += "<p align=center>Department Biology II,<br>\n";
  info += "   Ludwig-Maximilian University Munich.</p>\n";
  MessageBox::information( "About RELACS", info, this ); 
}


void RELACSWidget::help( void )
{
  if ( Help )
    return;

  Help = true;

  // create and exec dialog:
  OptDialog *od = new OptDialog( false, this );
  od->setCaption( "RELACS Help" );
  QTextBrowser *hb = new QTextBrowser( this );
  hb->mimeSourceFactory()->setFilePath( DocPath.c_str() );
  hb->setSource( "index.html" );
  if ( hb->mimeSourceFactory()->data( "index.html" ) == 0 ) {
    hb->setText( "Sorry, there is no help for <br><h2>RELACS</h2> available.<br><br> Try <c>make doc</c>." );
  }
  hb->setMinimumSize( 600, 400 );
  od->addWidget( hb );
  od->addButton( "&Ok" );
  connect( od, SIGNAL( dialogClosed( int ) ),
	   this, SLOT( helpClosed( int ) ) );
  od->exec();
}


void RELACSWidget::helpClosed( int r )
{
  Help = false;
}


KeyTimeOut::KeyTimeOut( QWidget *tlw )
  : TopLevelWidget( tlw ),
    NoFocusWidget( 0 )
{
  qApp->installEventFilter( this );
}


KeyTimeOut::~KeyTimeOut( void )
{
  qApp->removeEventFilter( this );
}


void KeyTimeOut::setNoFocusWidget( QWidget *w )
{
  NoFocusWidget = w;
}


void KeyTimeOut::unsetNoFocusWidget( void )
{
  NoFocusWidget = 0;
}


bool KeyTimeOut::eventFilter( QObject *o, QEvent *e )
{
  if ( qApp->focusWidget() != TopLevelWidget &&
       noFocusWidget() &&
       ( e->type() == QEvent::KeyPress ||
	 e->type() == QEvent::MouseButtonPress ||
	 e->type() == QEvent::FocusIn ) ) {
    killTimers();
    startTimer( 15000 );
  }
  return false;
}


void KeyTimeOut::timerEvent( QTimerEvent *e )
{
  if ( qApp->focusWidget() != TopLevelWidget &&
       noFocusWidget() ) {
    TopLevelWidget->setFocus();
  }
  killTimers();
}


bool KeyTimeOut::noFocusWidget( void ) const
{
  if ( NoFocusWidget != 0 ) {
    QWidget *fw = qApp->focusWidget();
    while ( fw != 0 && fw != NoFocusWidget ) {
      fw = fw->parentWidget( true );
    }
    if ( fw != 0 && fw == NoFocusWidget )
      return false;
  }

  return true;
}


}; /* namespace relacs */

#include "moc_relacswidget.cc"

