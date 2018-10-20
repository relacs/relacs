/*
  relacswidget.cc
  Coordinates RePros, session, input, output, plotting, and saving.

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

#include <unistd.h>
#include <cstdlib>
#include <string>
#include <iostream>
#include <fstream>
#include <QLabel>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolTip>
#include <QLayout>
#include <QTextBrowser>
#include <relacs/outdatainfo.h>
#include <relacs/plugins.h>
#include <relacs/defaultsession.h>
#include <relacs/aisim.h>
#include <relacs/aosim.h>
#include <relacs/attsim.h>
#include <relacs/attenuate.h>
#include <relacs/acquire.h>
#include <relacs/audiomonitor.h>
#include <relacs/control.h>
#include <relacs/controltabs.h>
#include <relacs/databrowser.h>
#include <relacs/filter.h>
#include <relacs/filterdetectors.h>
#include <relacs/inputconfig.h>
#include <relacs/outputconfig.h>
#include <relacs/macros.h>
#include <relacs/model.h>
#include <relacs/messagebox.h>
#include <relacs/relacsdevices.h>
#include <relacs/repros.h>
#include <relacs/savefiles.h>
#include <relacs/session.h>
#include <relacs/simulator.h>
#include <relacs/optdialog.h>
#include <relacs/relacswidget.h>
#include <relacs/deviceselector.h>
#include <relacs/filterselector.h>
#include <relacs/macroeditor.h>

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
			    const string &cfgexamplespath,
			    const string &iconpath,
			    bool doxydoc,
			    ModeTypes mode, QWidget *parent )
  : QMainWindow( parent ),
    ConfigClass( "RELACS", RELACSPlugin::Core ),
    Mode( mode ),
    SS( this ),
    MTDT( this ),
    CW( 0 ),
    ShowFull( 0 ),
    ShowTab( 0 ),
    ReadLoop( this ),
    WriteLoop( this ),
    DataRun( false ),
    LogFile( 0 ),
    IsFullScreen( false ),
    IsMaximized( false ),
    DeviceMenu( 0 ),
    Help( false ),
    HandlingEvent( false ),
    Doxydoc(doxydoc)
{
  printlog( "This is RELACS, version " + string( RELACSVERSION ) + ", compiled at " + string( __DATE__ ) );

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
  newSection( "input data" );
  addNumber( "inputsamplerate", "Input sampling rate", 20000.0, 1.0, 1000000.0, 1000.0, "Hz", "kHz" ); // Hertz, -> 2.4MB pro minute and channel
  addNumber( "inputtracecapacity", "Ring buffer has capacity for ", 600.0, 1.0, 1000000.0, 1.0, "s" );
  addBoolean( "inputunipolar", "Unipolar input", false );
  addText( "inputtraceid", "Input trace identifier", "V-1" );
  addNumber( "inputtracescale", "Input trace scale", 1.0 );
  addText( "inputtraceunit", "Input trace unit", "V" );
  addText( "inputtracedevice", "Input trace device", "ai-1" );
  addInteger( "inputtracechannel", "Input trace channel", 0 );
  addText( "inputtracereference", "Input trace reference", InData::referenceStr( InData::RefGround ) );
  addNumber( "inputtracemaxvalue", "Input trace maximum value", 10.0, 0.0, 10000000.0, 0.001 );
  addBoolean( "inputtracecenter", "Input trace center vertically", true );

  newSection( OutputConfig::OptionNames::GROUP_NAME );
  addText   ( OutputConfig::OptionNames::ID, "Output trace identifier", "" );
  addText   ( OutputConfig::OptionNames::DEVICE, "Output trace device", "ao-1" );
  addInteger( OutputConfig::OptionNames::CHANNEL, "Output trace channel", 0 );
  addNumber ( OutputConfig::OptionNames::SCALE, "Output trace scale factor to Volt", 1.0, -10000000.0, 10000000.0, 0.1 );
  addText   ( OutputConfig::OptionNames::UNIT, "Output trace unit", "V" );
  addNumber ( OutputConfig::OptionNames::MAX_RATE, "Maximum output sampling rate", 0.0, 0.0, 10000000.0, 1000.0, "Hz", "kHz" );
  addText   ( OutputConfig::OptionNames::MODALITY, "Output trace modality", "voltage" );

  // main widget:
  setWindowTitle( "RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation: Version " + QString( RELACSVERSION ) );
  MainWidget = new QWidget;
  MainLayout = new QGridLayout;
  MainLayout->setContentsMargins( 4, 4, 4, 4 );
  MainLayout->setSpacing( 4 );
  MainWidget->setLayout( MainLayout );
  MainWidget->setBackgroundRole( QPalette::Window );
  MainWidget->setAutoFillBackground( true );
  setCentralWidget( MainWidget );

  // macros:
  MC = new Macros( this );

  // filter and detectors:
  FD = new FilterDetectors( this );

  // data acquisition:
  AQ = 0;
  AQD = new Acquire();

  // simulator:
  SIM = new Simulator();

  // setup AudioMonitor:
  AM = new AudioMonitor();

  // devices:
  ADV = new AllDevices();
  DV = new Devices();
  AID = new AIDevices();
  AOD = new AODevices();
  DIOD = new DIODevices();
  TRIGD = new TriggerDevices();

  // attenuators:
  ATD = new AttDevices();
  ATI = new AttInterfaces();

  // load config:
  SF = 0;
  int r = CFG.read( RELACSPlugin::Core );

  if ( r == 0 ) {
    printlog(  "! error: No configuration files found. Exit now." );
    printlog(  "! error: Make sure to have a relacs.cfg and a relacsplugins.cfg file in the working directory." );
    printlog(  "! error: You find examples in " + cfgexamplespath + " and subdirectories." );
    MessageBox::error( "RELACS Error !", "No configuration files found.<br>\nExit now.<br>Make sure to have a relacs.cfg and a relacsplugins.cfg file in the working directory.<br>You find examples in " + cfgexamplespath + " and subdirectories.", this );
    qApp->exit( 1 );
    ::exit( 1 ); // do we need that?
  }
  CFG.configure( RELACSPlugin::Core );

  // loading plugins:
  Plugins::add( "AISim[relacs]", RELACSPlugin::AnalogInputId, createAISim, VERSION );
  Plugins::add( "AOSim[relacs]", RELACSPlugin::AnalogOutputId, createAOSim, VERSION );
  Plugins::add( "AttSim[relacs]", RELACSPlugin::AttenuatorId, createAttSim, VERSION );
  StrQueue pluginhomes( pluginhome, "|" );
  pluginhomes.strip();
  for ( int k=0; k<SS.Options::size( "pluginpathes" ); k++ ) {
    string pluginlib = SS.text( "pluginpathes", k );
    if ( !pluginlib.empty() ) {
      Plugins::openPath( pluginlib, pluginrelative, pluginhomes );
    }
  }

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

  OrgBackground = palette().color( QPalette::Window );

  // session, control tabwidget:
  CW = new ControlTabs( this );
  CW->createControls();

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
  RP = new RePros( this );
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

  // setup PlotTrace:
  PT = new PlotTrace( this );

  // status bar:
  // RePro message:
  QLabel *rl = RP->display();
  statusBar()->addPermanentWidget( rl, 200 );
  int statusbarheight = rl->height();
  // Session message:
  SN = new Session( this, statusbarheight, statusBar() );
  statusBar()->addWidget( SN, 0 );
  // SaveFiles:
  SF = new SaveFiles( this, statusbarheight, statusBar() );
  SS.notify(); // initialize SF from the Settings
  // data browser:
  DataBrowser *db = new DataBrowser( SF->dataIndex(), this );
  connect( db, SIGNAL( displayIndex( const string&, const deque<int>&, const deque<int>&, double ) ),
	   PT, SLOT( displayIndex( const string&, const deque<int>&, const deque<int>&, double ) ) );
  connect( db, SIGNAL( dataView() ), this, SLOT( showDataOnly() ) );
  CW->addTab( db, "Data-Browser" );
  statusBar()->addWidget( SF, 0 );
  // Simulation:
  SimLabel = new QLabel( this );
  SimLabel->setTextFormat( Qt::PlainText );
  SimLabel->setAlignment( Qt::AlignRight | Qt::AlignVCenter );
  SimLabel->setIndent( 2 );
  SimLabel->setFixedHeight( statusbarheight );
  SimLabel->setToolTip( "The load of the simulation" );
  statusBar()->addWidget( SimLabel, 0 );


  // menubar:

  // file:
  QMenu *filemenu = menuBar()->addMenu( "&File" );
  AcquisitionAction = filemenu->addAction( "&Online (acquire)",
					   (QWidget*)this, SLOT( startAcquisition() ),
					   Qt::CTRL + Qt::SHIFT + Qt::Key_O );
  SimulationAction = filemenu->addAction( "Sim&ulate",
					  (QWidget*)this, SLOT( startSimulation() ),
					  Qt::CTRL + Qt::SHIFT + Qt::Key_S );
  IdleAction = filemenu->addAction( "Idle (sto&p)",
				    (QWidget*)this, SLOT( stopActivity() ),
				    Qt::CTRL + Qt::SHIFT + Qt::Key_I );
  filemenu->addSeparator();
  SN->addActions( filemenu );
  MTDT.addActions( filemenu );
  filemenu->addSeparator();
  filemenu->addAction( "&Input traces...", this, SLOT( editInputTraces() ) );
  filemenu->addAction( "&Output traces...", this, SLOT( editOutputTraces() ) );
  filemenu->addAction( "&Devices", this, SLOT(editDevices()));
  filemenu->addAction( "&Filters", this, SLOT(editFilters()));
  filemenu->addAction( "&Macros", this, SLOT(editMacros()));
  filemenu->addAction( "Settings...", &SS, SLOT( dialog() ) );
  filemenu->addAction( "&Save settings", (QWidget*)this, SLOT( saveConfig() ) );
  filemenu->addSeparator();
  MuteAction = filemenu->addAction( "&Mute audio monitor",
				    (QWidget*)this, SLOT( muteAudioMonitor() ),
				    Qt::Key_M );
  filemenu->addAction( "&Audio monitor...", AM, SLOT( dialog() ) );
  filemenu->addAction( "&Quit", (QWidget*)this, SLOT( quit() ), Qt::ALT + Qt::Key_Q );

  // plugins:
  QMenu *pluginmenu = menuBar()->addMenu( "&Plugins" );
  if ( MD != 0 ) {
    MD->addActions( pluginmenu, doxydoc );
    pluginmenu->addSeparator();
  }
  CW->addMenu( pluginmenu, doxydoc );

  // devices:
  DeviceMenu = menuBar()->addMenu( "Dev&ices" );

  // filters:
  QMenu *filtermenu = menuBar()->addMenu( "&Detectors/Filters" );
  FD->addMenu( filtermenu, doxydoc );

  // repros:
  QMenu *repromenu = menuBar()->addMenu( "&RePros" );
  RP->addMenu( repromenu, doxydoc );

  // macros:
  QMenu *macromenu = menuBar()->addMenu( "&Macros" );
  MC->setMenu( macromenu );
  MC->setRePros( RP );
  MC->setControls( CW );
  MC->load( "", true );
  MC->load();
  MC->check();
  MC->create();
  connect( RP, SIGNAL( noMacro( RePro* ) ),
	   MC, SLOT( noMacro( RePro* ) ) );
  connect( RP, SIGNAL( reloadRePro( const string& ) ),
	   MC, SLOT( reloadRePro( const string& ) ) );

  // view:
  QMenu *viewmenu = menuBar()->addMenu( "&View" );
  viewmenu->addAction( "Show &filters",
		       (QWidget*)this, SLOT( showFilters() ),
		       Qt::CTRL + Qt::Key_1 );
  viewmenu->addAction( "Show &traces",
		       (QWidget*)this, SLOT( showTraces() ),
		       Qt::CTRL + Qt::Key_2 );
  viewmenu->addAction( "Show &controls",
		       (QWidget*)this, SLOT( showControls() ),
		       Qt::CTRL + Qt::Key_3 );
  viewmenu->addAction( "Show &RePros",
		       (QWidget*)this, SLOT( showRePros() ),
		       Qt::CTRL + Qt::Key_4 );
  viewmenu->addAction( "Show &data",
		       (QWidget*)this, SLOT( showData() ),
		       Qt::CTRL + Qt::Key_5 );
  MaximizedAction = viewmenu->addAction( "&Maximize window",
					 (QWidget*)this, SLOT( maximizeScreen() ),
					 Qt::CTRL + Qt::SHIFT + Qt::Key_M );
  FullscreenAction = viewmenu->addAction( "&Full screen",
					  (QWidget*)this, SLOT( fullScreen() ),
					  Qt::CTRL + Qt::SHIFT + Qt::Key_F );
  viewmenu->addSeparator();
  PT->addMenu( viewmenu );

  // help:
  QMenu *helpmenu = menuBar()->addMenu( "&Help" );
  helpmenu->addAction( "&Help...", (QWidget*)this, SLOT( help() ) );
  helpmenu->addAction( "&About...", (QWidget*)this, SLOT( about() ) );

  // layout:
  int wd = FD->sizeHint().width();
  int wc = CW->sizeHint().width();
  int w = wc > wd ? wc : wd;
  FD->setMaximumWidth( w );
  CW->setMaximumWidth( w );
  MainLayout->addWidget( FD, 0, 0 );
  MainLayout->addWidget( PT->widget(), 0, 1 );
  MainLayout->addWidget( CW, 1, 0 );
  MainLayout->addWidget( RP, 1, 1 );
  MainLayout->addWidget( MC, 2, 0, 1, 2 );
  MainLayout->setColumnStretch( 0, 1 );
  MainLayout->setColumnStretch( 1, 100 );
  MainLayout->setRowStretch( 0, 2 );
  MainLayout->setRowStretch( 1, 3 );
  w *= 3/2;
  if ( PT->widget()->minimumWidth() < w || RP->minimumWidth() < w ) {
    PT->widget()->setMinimumWidth( w );
    RP->setMinimumWidth( w );
  }

  // miscellaneous:
  setFocusPolicy( Qt::StrongFocus );
  window()->setFocus();
  KeyTime = new KeyTimeOut( window() );

}


RELACSWidget::~RELACSWidget( void )
{
  if ( MD != 0 ) {
    Plugins::destroy( MD->name(), RELACSPlugin::ModelId );
    delete MD;
  }
  delete FD;
  delete CW;
  delete RP;
  delete PT;
  Plugins::close();
  delete AQD;
  delete SIM;
  delete ADV;
  delete DV;
  delete ATD;
  delete ATI;
  delete TRIGD;
  delete DIOD;
  delete AID;
  delete AOD;
  delete KeyTime;
}


void RELACSWidget::printlog( const string &message ) const
{
  cerr << QTime::currentTime().toString().toStdString() << " "
       << message << endl;
  if ( LogFile != 0 )
    *LogFile << QTime::currentTime().toString().toStdString() << " "
	     << message << endl;
}


void RELACSWidget::init ( void )
{
  MC->warning();
  if ( MC->fatal() )
    ::exit( 1 );

  QFont f( fontInfo().family(), fontInfo().pointSize()*4/3, QFont::Bold );
  SimLabel->setFont( f );
  SimLabel->setFixedWidth( SimLabel->fontMetrics().boundingRect( "100%" ).width() + 8 );

  MTDT.clear();
  if ( acquisition() )
    startFirstAcquisition( false );
  else if ( simulation() )
    startFirstAcquisition( true );
}


int RELACSWidget::openHardware( int n, int errorlevel )
{
  Str errors = "";
  Str warnings = "";
  int error = 0;

  // activate devices:
  DV->create( *ADV, n );
  errors += DV->errors();
  warnings += DV->warnings();
  if ( ! DV->ok() )
    error |= 1;

  // activate analog input devices:
  if ( n == 0 )
    AID->create( *ADV, n );
  else
    AID->create( *ADV, 1, "AISim" );
  errors += AID->errors();
  warnings += AID->warnings();
  if ( ! AID->ok() ) {
    error |= 3;
    errors += "No analog input device opened!\n";
  }

  // activate analog output devices:
  if ( n == 0 )
    AOD->create( *ADV, n );
  else
    AOD->create( *ADV, 1, "AOSim" );
  errors += AOD->errors();
  warnings += AOD->warnings();
  if ( ! AOD->ok() ) {
    error |= 3;
    errors += "No analog output device opened!\n";
  }

  // activate digital I/O devices:
  DIOD->create( *ADV, n );
  errors += DIOD->errors();
  warnings += DIOD->warnings();
  if ( ! DIOD->ok() )
    error |= 1;

  // activate trigger devices:
  TRIGD->create( *ADV, n );
  errors += TRIGD->errors();
  warnings += TRIGD->warnings();
  if ( ! TRIGD->ok() )
    error |= 1;

  // activate attenuators:
  if ( n == 0 )
    ATD->create( *ADV, n );
  else
    ATD->create( *ADV, 1, "AttSim" );
  errors += ATD->errors();
  warnings += ATD->warnings();
  if ( ! ATD->ok() )
    error |= 3;

  ATI->create( *ADV, 0 );
  errors += ATI->errors();
  warnings += ATI->warnings();
  if ( ! ATI->ok() )
    error |= 3;

  if ( !warnings.empty() ) {
    Str ws = "Warnings in activating devices:\n";
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
    printlog( "! warning: " + ws.erasedMarkup() );
    MessageBox::warning( "RELACS Warning !", ws, false, 0.0, this );
  }
  if ( errorlevel > 0 ) {
    if ( !errors.empty() ) {
      Str ws = "Errors in activating devices:\n";
      errors.insert( 0, "<ul><li>" );
      int p = errors.find( "\n" );
      while ( p >= 0 ) {
	errors.insert( p, "</li>" );
	p += 6;
	int n = errors.find( "\n", p );
	if ( n < 0 )
	  break;
	errors.insert( p, "<li>" );
	p = n + 4;
      }
      ws += errors + "</ul>";
      if ( errorlevel > 1 ) {
	ws += "Can't switch to <b>" + modeStr() + "</b>-mode!";
	printlog( "! warning: " + ws.erasedMarkup() );
	MessageBox::error( "RELACS Warning !", ws, true, 0.0, this );
      }
      else {
	printlog( ws.erasedMarkup() );
	MessageBox::information( "RELACS Info !", ws, false, 0.0, this );
      }
    }
  }
  return error;
}


int RELACSWidget::setupHardware( int n )
{
  openHardware( n, 0 );
  // We need to call this twice to make sure that devices that depend on other devices get a second chance:
  int r = openHardware( n, 1 );

  // setup Acquire:
  if ( r < 2 ) {

    AQ = n == 0 ? AQD : SIM;
    OutData::setAcquire( AQ );

    for ( int k=0; k<AID->size(); k++ ) {
      AQ->addInput( &(*AID)[k] );
    }
    for ( int k=0; k<AOD->size(); k++ ) {
      AQ->addOutput( &(*AOD)[k] );
    }

    for ( int k=0; k<ATI->size(); k++ ) {
      AQ->addAttLine( &(*ATI)[k] );
      (*ATI)[k].init();
    }

    AQ->setAdjustFlag( 4096 );

    AQ->initSync();
    printlog( "Synchronization method: " + AQ->syncModeStr() );

    if ( n == 1 ) {
      connect( &SimLoad, SIGNAL( timeout() ),
	       this, SLOT( simLoadMessage() ) );
      SimLoad.start( 1000 );
    }

    return 0;
  }
  else {

    // clear devices:
    CW->clearDevices();
    ADV->clear();
    DV->clear();
    ATD->clear();
    ATI->clear();
    TRIGD->clear();
    DIOD->clear();
    AID->clear();
    AOD->clear();
    AQ = 0;

    return 1;
  }
}


void RELACSWidget::clearHardware( void )
{
  CW->clearDevices();
  AQD->clear();
  SIM->clear();
  ADV->clear();
  DV->clear();
  ATD->clear();
  ATI->clear();
  TRIGD->clear();
  DIOD->clear();
  AID->clear();
  AOD->clear();
}


void RELACSWidget::closeHardware( void )
{
  AQD->clear();
  SIM->clear();
  ADV->close();
  string warnings = ADV->warnings();
  if ( ! warnings.empty() ) {
    Str ws = "errors in closing devices:\n";
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
    printlog( "! warning: " + ws.erasedMarkup() );
    MessageBox::warning( "RELACS Warning !", ws, true, 0.0, this );
  }
  DeviceMenu->clear();
}


void RELACSWidget::setupInTraces( void )
{
  IRawData.clear();
  int nid = Options::size( "inputtraceid" );
  int j = 0;
  for ( int k=0; k<nid; k++ ) {
    bool failed = false;
    string traceid = text( "inputtraceid", k, "" );
    if ( traceid.empty() )
      continue;
    Str ws;

    InData id;
    id.setIdent( traceid );
    id.setTrace( k );
    id.setSampleRate( number( "inputsamplerate", 1000.0 ) );
    id.setStartSource( 0 );
    bool unipolar = boolean( "inputunipolar", false );
    id.setUnipolar( unipolar );
    double scale = number( "inputtracescale", k, 1.0 );
    if ( fabs( scale ) < 1e-8 ) {
      Str ss = "inputtracescale for input trace <b>" + traceid + "</b> is zero, set to 1";
      printlog( "! warning: " + ss.erasedMarkup() );
      MessageBox::warning( "RELACS Warning !", ss, true, 0.0, this );
      scale = 1.0;
    }
    id.setUnit( scale, text( "inputtraceunit", k, "V" ) );
    int channel = integer( "inputtracechannel", k, -1 );
    if ( channel < 0 ) {
      ws += ", invalid channel number <b>" + Str( channel ) + "</b>";
      failed = true;
    }
    id.setChannel( channel );
    Str ds = text( "inputtracedevice", k, "" );
    int devi = (int)::rint( ds.number( -1 ) );
    if ( devi < 0 || devi >= AQ->inputsSize() )
      devi = AQ->inputIndex( ds );
    if ( devi < 0 ) {
      ws += ", unknown device <b>" + ds + "</b>";
      failed = true;
    }
    id.setDevice( devi );
    id.setContinuous();
    int m = SaveFiles::SaveTrace | PlotTraceMode;
    if ( boolean( "inputtracecenter", k, false ) )
      m |= PlotTraceCenterVertically;
    id.setMode( m );
    id.setReference( text( "inputtracereference", k, InData::referenceStr( InData::RefGround ) ) );
    double maxval = number( "inputtracemaxvalue", k, -1.0 );
    if ( maxval <= 0.0 ) {
      ws += ", invalid maximum value <b>" + Str( maxval ) + "</b>";
      failed = true;
    }
    id.setMinValue( unipolar ? 0.0 : -maxval );
    id.setMaxValue( maxval );
    if ( channel < InData::ParamChannel ) {
      int gainindex = AQ->inputDevice( devi )->gainIndex( unipolar, id.getVoltage( maxval ) );
      id.setGainIndex( gainindex );
    }
    else
      id.setGainIndex( 0 );
    if ( failed ) {
      ws.erase( 0, 2 );
      ws += " for input trace <b>" + traceid + "</b>!<br> Skipped this input trace.";
      printlog( "! warning: " + ws.erasedMarkup() );
      MessageBox::warning( "RELACS Warning !", ws, true, 0.0, this );
      continue;
    }
    IRawData.push( id );
    IRawData[j].reserve( id.indices( number( "inputtracecapacity", 0, 1000.0 ) ) );
    IRawData[j].setWriteBufferCapacity( 100.0*id.indices( AQ->updateTime() ) );
    PT->addTraceStyle( true, integer( "inputtraceplot", j, j ), Plot::Green );
    j++;
  }
}


void RELACSWidget::setupOutTraces( void )
{
  AQ->clearOutTraces();
  int nod = Options::size( "outputtraceid" );
  int chan = 0;
  int lastdevi = 0;
  for ( int k=0; k<nod; k++ ) {
    string traceid = text( "outputtraceid", k, "" );
    if ( traceid.empty() )
      continue;
    Str ds = text( "outputtracedevice", k, "" );
    int devi = (int)::rint( ds.number( -1 ) );
    if ( devi < 0 || devi >= AQ->outputsSize() )
      devi = AQ->outputIndex( ds );
    if ( devi < 0 ) {
      Str ws = "Device <b>" + ds + "</b> for output trace <b>" + traceid + "</b> not known!<br> Skip this output trace.";
      printlog( "! warning: " + ws.erasedMarkup() );
      MessageBox::warning( "RELACS Warning !", ws, true, 0.0, this );
      continue;
    }
    if ( devi != lastdevi ) {
      lastdevi = devi;
      chan = 0;
    }
    double scale = number( "outputtracescale", k, 1.0 );
    if ( fabs( scale ) < 1e-8 ) {
      Str ws = "outputtracescale for output trace <b>" + traceid + "</b> is zero, set to 1";
      printlog( "! warning: " + ws.erasedMarkup() );
      MessageBox::warning( "RELACS Warning !", ws, true, 0.0, this );
      scale = 1.0;
    }
    AQ->addOutTrace( traceid, devi,
		     integer( "outputtracechannel", k, chan ),
		     scale,
		     text( "outputtraceunit", k, "V" ),
		     0.0,
		     number( "outputtracemaxrate", k, 0.0 ),
		     text( "outputtracemodality", k, "unknown" ) );
    chan++;
  }

  AQ->addOutTraces();

  // XXX this should go into SaveFile!
  SF->lock();
  SF->Options::erase( SF->TraceFlag );
  for ( int k=0; k<AQ->outTracesSize(); k++ )
    SF->addNumber( AQ->outTraceName( k ), AQ->outTraceName( k ), 
		   AQ->outTrace( k ).value(), -1.0e10, 1.0e10, 0.0001,
		   AQ->outTrace( k ).unit() ).setFormat( "%9.3f" ).setFlags( SF->TraceFlag );
  SF->unlock();
}


///// Data thread ///////////////////////////////////////////////////////////

int RELACSWidget::getData( InList &data, EventList &events, double &signaltime,
			   double mintracetime, double prevsignal )
{
  DerivedDataMutex.lockForRead();
  bool interrupted = false;
  if ( mintracetime > 0.0 ) {
    // do wee need to wait for a new signal?
    if ( prevsignal >= -1.0 ) {
      while ( IData.success() &&
	      SignalTime <= prevsignal+1.0e-8 &&
	      AQ->isReadRunning() ) { 
	UpdateDataWait.wait( &DerivedDataMutex );
	if ( ! AQ->isWriteRunning() )
	  break;
      }
      if ( SignalTime > prevsignal+1.0e-8 )
	mintracetime += SignalTime;
      else
	mintracetime = 0.0;
    }
    
    // do we need to wait for more data?
    while ( IData.success() &&
	    IData.currentTimeRaw() < mintracetime+1.0e-8 &&
	    AQ->isReadRunning() ) {
      UpdateDataWait.wait( &DerivedDataMutex );
    }
    
    interrupted = ( IData.currentTimeRaw() < mintracetime || mintracetime == 0.0 );
  }

  // update data:
  signaltime = SignalTime;
  data.update();
  events.update();
  // check data:
  bool error = IData.failed();
  DerivedDataMutex.unlock();
  return error ? -1 : ( interrupted ? 0 : 1 );
}


int RELACSWidget::updateData( void )
// called continuously from ReadThread::run()
{
  double signaltime = -1.0;
  int r = AQ->waitForData( signaltime );
  if ( r < 0 ) {
    // error handling:
    AQ->stopRead();
    AQ->lockRead();
    string es = IRawData.errorText();
    printlog( "! error in reading acquired data: " + es );
    QCoreApplication::postEvent( this, new RelacsWidgetEvent( 3, "Error in analog input: " + es ) );
    IRawData.clearError();
    AQ->unlockRead();
    double aitimeout = SS.number( "aitimeout", 10.0 );
    if ( DataTime.restart() < (int)(1000.0*aitimeout) ) {
      QCoreApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+4 ) ) );
      return -1;
    }
    // try to restart analog input:
    AQ->restartRead();
    AQ->lockRead();
    bool failed = IRawData.failed();
    es = IRawData.errorText();
    AQ->unlockRead();
    if ( failed ) {
      printlog( "! error in restarting analog input: " + es );
      QCoreApplication::postEvent( this, new RelacsWidgetEvent( 3, "Error in restarting analog input: " + es ) );
      QCoreApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+4 ) ) );
      return -1;
    }
    else {
      DataRunLock.lock();
      bool dr = DataRun;
      DataRunLock.unlock();
      return dr ? 1 : 0;
    }
  }
  else if ( r > 0 ) {
    // update derived data:
    DerivedDataMutex.lockForWrite();
    if ( signaltime >= 0.0 )
      SignalTime = signaltime;
    AQ->lockRead();
    for ( deque<InList*>::iterator dp = UpdateRawData.begin(); dp != UpdateRawData.end(); ++dp )
      (*dp)->updateRaw();
    for ( deque<EventList*>::iterator ep = UpdateRawEvents.begin(); ep != UpdateRawEvents.end(); ++ep )
      (*ep)->updateRaw();
    AQ->unlockRead();
    Str fdw = FD->filter( signaltime );
    if ( !fdw.empty() )
      printlog( "! error: " + fdw.erasedMarkup() );
    AM->updateDerivedTraces(); // XXX is this really good?
    DerivedDataMutex.unlock();

    // save data:
    SF->saveTraces();

    // notify other plugins about available data:
    UpdateDataWait.wakeAll();
  }
  DataRunLock.lock();
  bool dr = DataRun;
  DataRunLock.unlock();
  return dr ? r : 0;
}


void RELACSWidget::wakeAll( void )
{
  UpdateDataWait.wakeAll();
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

int RELACSWidget::activateGains( void )
{
  int r = AQ->activateGains();
  if ( r<0 || IRawData.failed() ) {
    printlog( "! error in restarting analog input for changing gains: " + IRawData.errorText() );
    QCoreApplication::postEvent( this, new RelacsWidgetEvent( 3, "Error in restarting analog input for changing gains: " + IRawData.errorText() ) );
    QCoreApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+4 ) ) );
    r = -1;
  }
  else if ( ! ReadLoop.isRunning() ) {
    DataRun = true;
    DataTime.restart();
    ReadLoop.start();
    r = 0;
  }
  FD->scheduleAdjust();
  return r;
}


int RELACSWidget::write( OutData &signal, bool setsignaltime, bool blocking )
{
  if ( SF->signalPending() && SF->saving() )
    printlog( "! warning in write() -> previous signal still pending in SaveFiles !" );
  int r = AQ->write( signal, setsignaltime );
  if ( r >= 0 ) {
    SF->save( signal );
    FD->scheduleAdjust();
    if ( ! ReadLoop.isRunning() ) {
      DataRun = true;
      DataTime.restart();
      ReadLoop.start();
    }
    // update device menu:
    QCoreApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+2 ) ) );
    if ( blocking ) {
      WriteLoop.run();
      if ( WriteLoop.failed() )
	r = -1;
    }
    else
      WriteLoop.start();
  }
  else {
    printlog( "! failed to write signal: " + signal.errorText() );
    QCoreApplication::postEvent( this, new RelacsWidgetEvent( 3, "Error in analog output: " + signal.errorText() ) );
    if ( IRawData.failed() )
      printlog( "! error in restarting analog input: " + IRawData.errorText() );
  }
  return r;
}


int RELACSWidget::write( OutList &signal, bool setsignaltime, bool blocking )
{
  if ( SF->signalPending() && SF->saving() )
    printlog( "! warning in write() -> previous signal still pending in SaveFiles !" );
  int r = AQ->write( signal, setsignaltime );
  if ( r >= 0 ) {
    SF->save( signal );
    FD->scheduleAdjust();
    if ( ! ReadLoop.isRunning() ) {
      DataRun = true;
      DataTime.restart();
      ReadLoop.start();
    }
    // update device menu:
    QCoreApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+2 ) ) );
    if ( blocking ) {
      WriteLoop.run();
      if ( WriteLoop.failed() )
	r = -1;
    }
    else
      WriteLoop.start();
  }
  else {
    printlog( "! failed to write signals: " + signal.errorText() );
    if ( IRawData.failed() )
      printlog( "! error in restarting analog input: " + IRawData.errorText() );
  }
  return r;
}


int RELACSWidget::directWrite( OutData &signal, bool setsignaltime )
{
  if ( SF->signalPending() && SF->saving() )
    printlog( "! warning in write() -> previous signal still pending in SaveFiles !" );
  int r = AQ->directWrite( signal, setsignaltime );
  if ( r == 0 ) {
    if ( ! ReadLoop.isRunning() ) {
      DataRun = true;
      DataTime.restart();
      ReadLoop.start();
    }
    SF->save( signal );
    FD->scheduleAdjust();
    // update device menu:
    QCoreApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+2 ) ) );
  }
  else {
    printlog( "! failed to write signal: " + signal.errorText() );
    if ( IRawData.failed() )
      printlog( "! error in restarting analog input: " + IRawData.errorText() );
  }
  return r;
}


int RELACSWidget::directWrite( OutList &signal, bool setsignaltime )
{
  if ( SF->signalPending() && SF->saving() )
    printlog( "! warning in write() -> previous signal still pending in SaveFiles !" );
  int r = AQ->directWrite( signal, setsignaltime );
  if ( r == 0 ) {
    if ( ! ReadLoop.isRunning() ) {
      DataRun = true;
      DataTime.restart();
      ReadLoop.start();
    }
    SF->save( signal );
    FD->scheduleAdjust();
    // update device menu:
    QCoreApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+2 ) ) );
  }
  else {
    printlog( "! failed to write signals: " + signal.errorText() );
    if ( IRawData.failed() )
      printlog( "! error in restarting analog input: " + IRawData.errorText() );
  }
  return r;
}


int RELACSWidget::stopWrite( void )
{
  // stop analog output:
  int r = AQ->stopWrite();
  return r;
}


void RELACSWidget::notifyStimulusData( void )
{
  if ( CW == 0 )
    return;
  if ( MD != 0 )
    MD->notifyStimulusData();
  FD->notifyStimulusData();
  CW->notifyStimulusData();
  RP->notifyStimulusData();
}


void RELACSWidget::notifyMetaData( void )
{
  if ( CW == 0 )
    return;
  if ( MD != 0 )
    MD->notifyMetaData();
  FD->notifyMetaData();
  CW->notifyMetaData();
  RP->notifyMetaData();
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
  printlog( "Starting RePro \"" + CurrentRePro->name() + "\"" );

  ReProRunning = true;
  SN->incrReProCount();

  Options macrostack;
  if ( (macroaction & Macro::NoMacro) == 0 )
    MC->macroStack( macrostack );

  // XXX lock SF ?
  SF->holdOn();
  CurrentRePro->setSaving( saving );
  SF->save( *CurrentRePro, macrostack );
  // XXX unlock SF ?
  CurrentRePro->start( QThread::HighPriority );
}


void RELACSWidget::stopRePro( void )
{
  if ( ! ReProRunning )
    return;

  // wait on RePro to stop:
  if ( CurrentRePro->isRunning() ) {
    // dispatch all posted events (that usually paint the RePro...)
    // as long as the RePro is normally running, so that it has
    // still all internal variables available:
    QCoreApplication::sendPostedEvents();
    // request and wait for the RePro to properly terminate:
    CurrentRePro->requestStop();
    // the RePro may wait for an event to be processed:
    while ( CurrentRePro->isRunning() ) {
      qApp->processEvents( QEventLoop::AllEvents, 10 );
      CurrentRePro->requestStop();
    }
    CurrentRePro->wait();
  }

  ReProRunning = false;
  window()->setFocus();

  // XXX last stimulus still not saved?
  if ( SF->signalPending() )
    SF->clearSignal();

  // update device menu:
  QCoreApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+2 ) ) );

  // wake up controls waiting on RePros to finish:
  ReProAfterWait.wakeAll();
}


void RELACSWidget::customEvent( QEvent *qce )
{
  switch ( qce->type() - QEvent::User ) {

  case 1: {
    MC->startNextRePro( true );
    break;
  }

  case 2: {
    DV->updateMenu();
    AID->updateMenu();
    // no locking needed here, since this does not affect the AO device:
    AOD->updateMenu();
    ATD->updateMenu();
    ATI->updateMenu();
    TRIGD->updateMenu();
    break;
  }

  case 3: {
    RelacsWidgetEvent *rwe = dynamic_cast<RelacsWidgetEvent*>( qce );
    MessageBox::warning( "RELACS Error !", rwe->text(), 10.0, this );
    break;
  }

  case 4: {
    stopActivity();
    break;
  }

  case 5: {
    RelacsWidgetEvent *rwe = dynamic_cast<RelacsWidgetEvent*>( qce );
    MessageBox::warning( "RELACS Warning !", rwe->text(), 10.0, this );
    break;
  }

  }
}


void RELACSWidget::updateRePro( void )
{
  ReProSleepWait.wakeAll();
}


void RELACSWidget::startedMacro( const string &ident, const string &param )
{
  RP->setMacro( ident, param );
  printlog( "Starting Macro \"" + ident + "\"" );
}


void RELACSWidget::startSession( bool startmacro )
{
  stopRePro();

  printlog( "Start new session" );

  // open files:
  SF->openFiles();
  if ( ! SF->filesOpen() )
    QCoreApplication::postEvent( this, new RelacsWidgetEvent( 5, "No data are saved!" ) );

  QPalette p( palette() );
  p.setColor( QPalette::Window, QColor( 255, 96, 96 ) );
  MainWidget->setPalette( p );
  p.setColor( QPalette::Window, OrgBackground );
  FD->setPalette( p );
  PT->widget()->setPalette( p );
  CW->setPalette( p );
  RP->setPalette( p );
  MC->setPalette( p );

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

  SessionStartWait.wakeAll();

  if ( MD != 0 )
    MD->sessionStarted();
  FD->sessionStarted();
  CW->sessionStarted();
  RP->sessionStarted();

  if ( startmacro )
    MC->startSession();
}


void RELACSWidget::preStopSession( void )
{
  SessionPrestopWait.wakeAll();
  MC->fallBack( false );
}


void RELACSWidget::resumeSession( void )
{
  printlog( "Resume session" );
}


  void RELACSWidget::stopSession( bool saved, bool stopmacro )
{
  printlog( "Stop session" );

  MTDT.update();
  if ( MD != 0 && simulation() )
    MTDT.add( "Simulation", MD->metaData() );
  MTDT.save();

  if ( MD != 0 )
    MD->sessionStopped( saved );
  FD->sessionStopped( saved );
  CW->sessionStopped( saved );
  RP->sessionStopped( saved );

  if ( saved )
    SF->completeFiles();
  else
    SF->deleteFiles();

  CurrentRePro->setSaving( SF->filesOpen() );

  MTDT.remove();

  QPalette p( palette() );
  p.setColor( QPalette::Window, OrgBackground );
  MainWidget->setPalette( p );

  if ( LogFile != 0 ) {
    delete LogFile;
    LogFile = 0;
  }

  SessionStopWait.wakeAll();

  if ( saved && stopmacro )
    MC->stopSession();

  SF->setPath( SF->defaultPath() );
}


Session *RELACSWidget::session( void )
{
  return SN;
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
  CW->requestStop();
  wakeAll();
  CW->wait( 0.2 );
  PT->stop();

  // stop simulation and data acquisition:
  SimLoad.stop();
  if ( AQ != 0 ) {
    DataRunLock.lock();
    DataRun = false;
    DataRunLock.unlock();
    AQ->stop();
    ReadLoop.wait();
  }

  // process pending events posted from threads.
  qApp->processEvents();
}


void RELACSWidget::stopActivity( void )
{
  doStopActivity( true );
}


void RELACSWidget::doStopActivity( bool sessiondialog )
{
  printlog( "Stopping " + modeStr() + "-mode" );

  // finish session and repro:
  if ( sessiondialog )
    SN->stopTheSession();
  else
    SN->doStopTheSession( false, false );
  // shutdown macros:
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
  if ( ! idle() )
    stopActivity();
  clearActivity();
  printlog( "Quitting RELACS" );
  qApp->quit();
}


void RELACSWidget::shutdown( void )
{
  if ( ! idle() )
    doStopActivity( false );
  clearActivity();
  printlog( "Quitting RELACS" );
  qApp->quit();
}


void RELACSWidget::closeEvent( QCloseEvent *ce )
{
  quit();
  ce->accept();
}


void RELACSWidget::editInputTraces( void )
{
  InputConfig *ic = new InputConfig( section( "input data" ), this );
  OptDialog *od = new OptDialog( false, this );
  od->setCaption( "Analog input traces" );
  od->addWidget( ic );
  od->addButton( "&Ok", OptDialog::Accept, 2 );
  od->addButton( "&Apply", OptDialog::Accept, 1, false );
  od->addButton( "&Cancel" );
  QObject::connect( od, SIGNAL( buttonClicked( int ) ), ic, SLOT( dialogClosed( int ) ) );
  QObject::connect( ic, SIGNAL( newInputSettings() ), this, SLOT( restartAcquisition() ) );
  od->exec();
}


void RELACSWidget::editOutputTraces( void )
{
  OutputConfig* oc = new OutputConfig(section("output data"), this);
  OptDialog* od = new OptDialog(false, this);
  od->setCaption("Analog output traces");
  od->addWidget(oc);
  od->addButton( "&Ok", OptDialog::Accept, OutputConfig::CODE_OK );
  od->addButton( "&Apply", OptDialog::Accept, OutputConfig::CODE_APPLY, false );
  od->addButton( "&Cancel" );
  QObject::connect( od, SIGNAL( buttonClicked( int ) ), oc, SLOT( dialogClosed( int ) ) );
  QObject::connect( oc, SIGNAL( newOutputSettings() ), this, SLOT( restartAcquisition() ) );
  od->exec();
}

void RELACSWidget::editDevices()
{
  std::map<int, ConfigClass*> deviceLists;
  /* emplace is not well supported yet ... 
  deviceLists.emplace(DV->pluginId(), DV);
  deviceLists.emplace(AID->pluginId(), AID);
  deviceLists.emplace(AOD->pluginId(), AOD);
  deviceLists.emplace(DIOD->pluginId(), DIOD);
  deviceLists.emplace(TRIGD->pluginId(), TRIGD);
  deviceLists.emplace(ATD->pluginId(), ATD);
  deviceLists.emplace(ATI->pluginId(), ATI);
  */
  deviceLists[DV->pluginId()] = DV;
  deviceLists[AID->pluginId()] = AID;
  deviceLists[AOD->pluginId()] = AOD;
  deviceLists[DIOD->pluginId()] = DIOD;
  deviceLists[TRIGD->pluginId()] = TRIGD;
  deviceLists[ATD->pluginId()] = ATD;
  deviceLists[ATI->pluginId()] = ATI;

  DeviceSelector* oc = new DeviceSelector(deviceLists, this);
  OptDialog* od = new OptDialog(false, this);
  od->setCaption("Active devices");
  od->addWidget(oc);
  od->addButton( "&Ok", OptDialog::Accept, DeviceSelector::CODE_OK );
  od->addButton( "&Apply", OptDialog::Accept, DeviceSelector::CODE_APPLY, false );
  od->addButton( "&Cancel" );
  QObject::connect( od, SIGNAL( buttonClicked( int ) ), oc, SLOT( dialogClosed( int ) ) );
  QObject::connect( oc, SIGNAL( newDeviceSettings() ), this, SLOT( restartAcquisition() ) );
  od->exec();
}

void RELACSWidget::editFilters()
{
  FilterSelector* fc = new FilterSelector(this);

  fc->setInputTraces(Options::section("input data"));
  fc->setFilters(FD);

  OptDialog* od = new OptDialog(false, this);
  od->setCaption("Filter configuration");
  od->addWidget(fc);
  od->addButton( "&Ok", OptDialog::Accept, 2 );
  od->addButton( "&Apply", OptDialog::Accept, 1, false );
  od->addButton( "&Cancel" );
  QObject::connect( od, SIGNAL( buttonClicked( int ) ), fc, SLOT( dialogClosed( int ) ) );
  QObject::connect( fc, SIGNAL( newFilterSettings() ), this, SLOT( restartAcquisition() ) );
  od->exec();
}

void RELACSWidget::editMacros()
{
  MacroEditor* mc = new MacroEditor(MC, this);

  mc->setRepros(RP);
  mc->setFilterDetectors(FD);
  mc->load();

  OptDialog* od = new OptDialog(false, this);
  od->setCaption("Macro Editor");
  od->addWidget(mc);
  od->addButton("&Ok", OptDialog::Accept, 2);
  od->addButton("&Apply", OptDialog::Accept, 1, false);
  od->addButton("&Cancel");
  QObject::connect(od, SIGNAL(buttonClicked(int)), mc, SLOT(dialogClosed(int)));
  // QObject::connect(mc, SIGNAL(macroDefinitionsChanged()), this, SLOT(restartAcquisition());
  od->exec();
}


FilterDetectors *RELACSWidget::filterDetectors( void )
{
  return FD;
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
  CW->modeChanged();
  SN->modeChanged();
  RP->modeChanged();
}


void RELACSWidget::clearActivity( void )
{
  CFG.save();
  clearHardware();
  IRawData.clearBuffer();
  IData.clear();
  ERawData.clear();
  EData.clear();
  SimLabel->hide();
  MTDT.clear();
}


void RELACSWidget::startAcquisition( void )
{
  if ( ! idle() )
    stopActivity();
  clearActivity();
  startFirstAcquisition( false );
}


void RELACSWidget::startSimulation( void )
{
  if ( ! idle() )
    stopActivity();
  clearActivity();
  startFirstAcquisition( true );
}


void RELACSWidget::startFirstAcquisition( bool simulation )
{
  setMode( simulation ? SimulationMode : AcquisitionMode );

  // hardware:
  if ( setupHardware( simulation ? 1 : 0 ) ) {
    startIdle();
    return;
  }

  // analog input and output traces:
  PT->clearStyles();
  double ptime = SS.number( "processinterval", 0.1 );
  AQ->setBufferTime( SS.number( "readinterval", 0.01 ) );
  AQ->setUpdateTime( ptime );
  setupInTraces();
  if ( IRawData.empty() ) {
    printlog( "! error: No valid input traces configured!" );
    MessageBox::error( "RELACS Error !", "No valid input traces configured!", this );
    startIdle();
    return;
  }
  setupOutTraces();
  int r = AQ->testRead( IRawData );
  if ( r < 0 ) {
    printlog( "! error in testing data acquisition: " + IRawData.errorText() );
    MessageBox::warning( "RELACS Warning !",
			 "error in testing data acquisition: " + IRawData.errorText(),
			 true, 0.0, this );
    startIdle();
    return;
  }

  // basic events:
  FD->clearIndices();
  SignalTime = -1.0;
  AQ->addStimulusEvents( IRawData, ERawData );
  FD->createStimulusEvents( ERawData, PT->eventStyles() );
  AQ->addRestartEvents( IRawData, ERawData );
  FD->createRestartEvents( ERawData, PT->eventStyles() );
  FD->createRecordingEvents( IRawData, ERawData, PT->eventStyles() );

  // derived (filtered) traces and events:
  IData.assign( &IRawData );
  EData.assign( &ERawData );
  Str fdw = FD->createTracesEvents( IData, EData,
				    PT->traceStyles(), PT->eventStyles() );
  if ( !fdw.empty() ) {
    printlog( "! error: " + fdw.erasedMarkup() );
    MessageBox::error( "RELACS Error !", fdw, this );
    startIdle();
    return;
  }

  // files:
  SF->setPath( SF->defaultPath() );

  // copy traces:
  UpdateRawData.clear();
  UpdateRawEvents.clear();
  UpdateRawData.push_back( &IData );
  UpdateRawEvents.push_back( &EData );

  FD->setTracesEvents( IData, EData );
  SF->setTracesEvents( IData, EData );
  AM->assignTraces( IData, UpdateRawData ); // XXX is this really good in this way???
  CW->assignTracesEvents( IData, EData );
  PT->assignTracesEvents( IData, EData );
  RP->assignTracesEvents( IData, EData );
  if ( simulation ) {
    MD->assignTracesEvents( IData, EData );
    MD->addTracesEvents( UpdateRawData, UpdateRawEvents ); // XXX is this ever used?
  }

  // plots:
  PT->updateMenu();
  PT->resize();

  // simulation status widget:
  if ( simulation ) {
    SimLabel->setText( "" );
    SimLabel->show();
  }

  CFG.preConfigure( RELACSPlugin::Plugins );
  // XXX before configuring, something like AQ->init would be nice
  // in order to set the IRawData gain factors.
  CFG.read( RELACSPlugin::Plugins );
  CFG.configure( RELACSPlugin::Plugins );

  // device menu:
  int menuindex = 0;
  DV->addMenu( DeviceMenu, menuindex );
  AID->addMenu( DeviceMenu, menuindex );
  AOD->addMenu( DeviceMenu, menuindex );
  DIOD->addMenu( DeviceMenu, menuindex );
  TRIGD->addMenu( DeviceMenu, menuindex );
  ATD->addMenu( DeviceMenu, menuindex );
  ATI->addMenu( DeviceMenu, menuindex );

  // init plugins:
  CW->initDevices();
  RP->setSettings();

  // macros:
  MC->checkOptions();
  MC->warning();

  // update layout:
  int wd = FD->sizeHint().width();
  int wc = CW->sizeHint().width();
  int w = wc > wd ? wc : wd;
  FD->setMaximumWidth( w );
  CW->setMaximumWidth( w );

  // start data aquisition:
  r = AQ->read( IRawData );
  if ( simulation && r < 0 ) {
    // give it a second chance with the adjusted input parameter:
    r = AQ->read( IRawData );
  }
  if ( r < 0 ) {
    printlog( "! error in starting data acquisition: " + IRawData.errorText() );
    MessageBox::warning( "RELACS Warning !",
			 "error in starting data acquisition: " + IRawData.errorText(),
			 true, 0.0, this );
    startIdle();
    return;
  }

  // check success:
  if ( simulation ) {
    // XXX why this here and why not return with startIdle() ?
    for ( int k=0; k<IRawData.size(); k++ ) {
      if ( IRawData[k].failed() ) {
	printlog( "Error in starting simulation of trace " + IRawData[k].ident() + ": "
		  + IRawData[k].errorText() );
	stopActivity();
	return;
      }
    }
  }

  // initialize filters:
  FD->setAdjustFlag( AQ->adjustFlag() );
  fdw = FD->init();  // init filters/detectors before RePro!
  if ( ! fdw.empty() ) {
    printlog( "! error in initializing filter: " + fdw.erasedMarkup() );
    MessageBox::warning( "RELACS Warning !",
			 "error in initializing filter: " + fdw,
			 true, 0.0, this );
    startIdle();
    return;
  }

  DataRun = true;
  DataTime.start();
  ReadLoop.start();

  IData.assign();
  PT->assignTracesEvents();
  CW->assignTracesEvents();
  RP->assignTracesEvents();
  if ( simulation )
    MD->assignTracesEvents();
  AM->assignTraces();
  AID->updateMenu();

  CW->start();
  PT->start( ptime );

  // get first RePro and start it:
  MC->startUp();

  AcquisitionAction->setEnabled( simulation );
  SimulationAction->setEnabled( ! simulation );
  IdleAction->setEnabled( true );

  if ( simulation )
    printlog( "Simulation-mode started" );
  else
    printlog( "Acquisition-mode started" );
}


void RELACSWidget::startIdle( void )
{
  MTDT.clear();
  CFG.preConfigure( RELACSPlugin::Plugins );
  CFG.read( RELACSPlugin::Plugins );
  CFG.configure( RELACSPlugin::Plugins );
  CW->initDevices();

  // macros:
  MC->checkOptions();
  MC->warning();

  // update layout:
  int wd = FD->sizeHint().width();
  int wc = CW->sizeHint().width();
  int w = wc > wd ? wc : wd;
  FD->setMaximumWidth( w );
  CW->setMaximumWidth( w );

  RP->setSettings();
  //  XXX IRawData->clearBuffer();
  //  XXX ERawData.clear();
  RP->activateRePro( 0 );
  AcquisitionAction->setEnabled( true );
  SimulationAction->setEnabled( true );
  IdleAction->setEnabled( false );
  setMode( IdleMode );
  RP->message( "<b>Idle-mode</b>" );
}



void RELACSWidget::restartAcquisition( void )
{
  ModeTypes mode = Mode;
  if ( ! idle() )
    stopActivity();
  clearActivity();

  // reload filters
  {
    FD->clear();
    FD->createFilters();
    FD->addMenu(nullptr, Doxydoc);
  }

  if ( mode == AcquisitionMode )
    startFirstAcquisition( false );
  else if ( mode == SimulationMode )
    startFirstAcquisition( true );
  else
    startIdle();
}


//-------------------------Keyboard Interaction----------------------------//


void RELACSWidget::keyPressEvent( QKeyEvent *event )
{
  // unused events are propagated back to parent widgets,
  // therefore we have to prevent this additional call:
  if ( HandlingEvent ) {
    event->ignore();
    return;
  }
  HandlingEvent = true;

  // PlotTrace:
  // note: because keyPressEvent() is protected, we need to call
  // this function via sendEvent().
  // This has the advantage, that all the eventFilters, etc. are called as well.
  QCoreApplication::sendEvent( PT->widget(), event );

  // RePro:
  if ( ! event->isAccepted() &&
       CurrentRePro != 0 &&
       CurrentRePro->widget() != 0  ) {
    // even RePros without a widget get one automatically assigned by RePros.
    // So, via the RELACSPlugin::eventFilter() they also get their
    // keyPressEvent() called!
    QCoreApplication::sendEvent( CurrentRePro->widget(), event );
  }

  // Controls:
  if ( ! event->isAccepted() )
    QCoreApplication::sendEvent( CW, event );

  // Filter and Detectors:
  if ( ! event->isAccepted() )
    QCoreApplication::sendEvent( FD, event );

  HandlingEvent = false;
}


void RELACSWidget::keyReleaseEvent( QKeyEvent *event )
{
  // unused events are propagated back to parent widgets,
  // therefore we have to prevent this additional call:
  if ( HandlingEvent ) {
    event->ignore();
    return;
  }
  HandlingEvent = true;

  // PlotTrace:
  QCoreApplication::sendEvent( PT->widget(), event );

  // RePro:
  if ( ! event->isAccepted() &&
       CurrentRePro != 0 &&
       CurrentRePro->widget() != 0  )
    QCoreApplication::sendEvent( CurrentRePro->widget(), event );

  // Controls:
  if ( ! event->isAccepted() )
    QCoreApplication::sendEvent( CW, event );

  // Filter and Detectors:
  if ( ! event->isAccepted() )
    QCoreApplication::sendEvent( FD, event );

  HandlingEvent = false;
}



void RELACSWidget::displayData( void )
{
  showFull();
  PT->displayData();
}


void RELACSWidget::showFilters( void )
{
  if ( ShowFull == 1 )
    showFull();
  else {
    restoreWidgets();
    ShowFull = 1;
    MainLayout->setColumnStretch( 0, 0 );
    MainLayout->setColumnStretch( 1, 0 );
    MainLayout->setRowStretch( 0, 0 );
    MainLayout->setRowStretch( 1, 0 );
    FD->setMaximumWidth( 16777215 );
    FD->show();
    PT->widget()->hide();
    CW->hide();
    RP->hide();
  }
}


void RELACSWidget::showTraces( void )
{
  if ( ShowFull == 2 )
    showFull();
  else {
    restoreWidgets();
    ShowFull = 2;
    MainLayout->setColumnStretch( 0, 0 );
    MainLayout->setColumnStretch( 1, 0 );
    MainLayout->setRowStretch( 0, 0 );
    MainLayout->setRowStretch( 1, 0 );
    FD->hide();
    PT->widget()->show();
    CW->hide();
    RP->hide();
  }
}


void RELACSWidget::showControls( void )
{
  if ( ShowFull == 3 )
    showFull();
  else {
    restoreWidgets();
    ShowFull = 3;
    MainLayout->setColumnStretch( 0, 0 );
    MainLayout->setColumnStretch( 1, 0 );
    MainLayout->setRowStretch( 0, 0 );
    MainLayout->setRowStretch( 1, 0 );
    FD->hide();
    PT->widget()->hide();
    CW->setMaximumWidth( 16777215 );
    CW->show();
    RP->hide();
  }
}


void RELACSWidget::showRePros( void )
{
  if ( ShowFull == 4 )
    showFull();
  else {
    restoreWidgets();
    ShowFull = 4;
    MainLayout->setColumnStretch( 0, 0 );
    MainLayout->setColumnStretch( 1, 0 );
    MainLayout->setRowStretch( 0, 0 );
    MainLayout->setRowStretch( 1, 0 );
    FD->hide();
    PT->widget()->hide();
    CW->hide();
    RP->show();
  }
}


void RELACSWidget::showData( void )
{
  if ( ShowFull == 5 )
    showFull();
  else 
    showDataOnly();
}


void RELACSWidget::showDataOnly( void )
{
  if ( ShowFull != 5 ) {
    restoreWidgets();
    ShowFull = 5;
    MainLayout->setColumnStretch( 0, 1 );
    MainLayout->setColumnStretch( 1, 100 );
    MainLayout->setRowStretch( 0, 0 );
    MainLayout->setRowStretch( 1, 0 );
    FD->hide();
    PT->widget()->show();
    CW->show();
    RP->hide();
    MainLayout->removeWidget( CW );
    MainLayout->removeWidget( FD );
    MainLayout->addWidget( CW, 0, 0 );
    ShowTab = CW->currentIndex();
    CW->setCurrentIndex(CW->count()-1);
  }
}


void RELACSWidget::showFull( void )
{
  restoreWidgets();
  ShowFull = 0;
  int wd = FD->sizeHint().width();
  int wc = CW->sizeHint().width();
  int w = wc > wd ? wc : wd;
  FD->setMaximumWidth( w );
  CW->setMaximumWidth( w );
  MainLayout->setColumnStretch( 0, 1 );
  MainLayout->setColumnStretch( 1, 100 );
  MainLayout->setRowStretch( 0, 2 );
  MainLayout->setRowStretch( 1, 3 );
  FD->show();
  PT->widget()->show();
  CW->show();
  RP->show();
}


void RELACSWidget::restoreWidgets( void )
{
  if ( ShowFull == 5 ) {
    MainLayout->removeWidget( CW );
    MainLayout->addWidget( FD, 0, 0 );
    MainLayout->addWidget( CW, 1, 0 );
    CW->setCurrentIndex( ShowTab );
  }
}


void RELACSWidget::fullScreen( void )
{
  if ( IsFullScreen ) {
    showNormal();
    IsFullScreen = false;
    FullscreenAction->setText( "&Fullscreen" );
  }
  else {
    showFullScreen();
    IsFullScreen = true;
    FullscreenAction->setText( "Exit &fullscreen" );
  }
}


void RELACSWidget::maximizeScreen( void )
{
  if ( IsMaximized ) {
    showNormal();
    IsMaximized = false;
    MaximizedAction->setText( "&Maximize window" );
  }
  else {
    showMaximized();
    IsMaximized = true;
    MaximizedAction->setText( "Exit &maximize window" );
  }
}


void RELACSWidget::muteAudioMonitor( void )
{
  if ( AM != 0 ) {
    if ( AM->muted() ) {
      AM->unmute();
      MuteAction->setText( "Mute audio &monitor" );
    }
    else {
      AM->mute();
      MuteAction->setText( "Unmute audio &monitor" );
    }
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
  info += "<p align=center>Neuroethology Lab<br>\n";
  info += "<p align=center>Institute for Neurobiology<br>\n";
  info += "   Eberhard Karls Universit&auml;t T&uuml;bingen</p>\n";
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
  QStringList fpl;
  fpl.push_back( DocPath.c_str() );
  hb->setSearchPaths( fpl );
  hb->setSource( QUrl::fromLocalFile( "index.html" ) );
  if ( hb->toHtml().isEmpty() )
    hb->setText( "Sorry, there is no help for <br><h2>RELACS</h2> available.<br><br> Try <c>make doc</c>." );
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
  : TimerId( 0 ),
    TopLevelWidget( tlw ),
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
    if ( TimerId != 0 )
      killTimer( TimerId );
    TimerId = startTimer( 15000 );
  }
  return false;
}


void KeyTimeOut::timerEvent( QTimerEvent *e )
{
  if ( qApp->focusWidget() != TopLevelWidget &&
       noFocusWidget() ) {
    TopLevelWidget->setFocus();
  }
  if ( TimerId != 0 )
    killTimer( TimerId );
  TimerId = 0;
}


bool KeyTimeOut::noFocusWidget( void ) const
{
  if ( NoFocusWidget != 0 ) {
    QWidget *fw = qApp->focusWidget();
    while ( fw != 0 && fw != NoFocusWidget )
      fw = fw->parentWidget();
    if ( fw != 0 && fw == NoFocusWidget )
      return false;
  }

  return true;
}


}; /* namespace relacs */

#include "moc_relacswidget.cc"

