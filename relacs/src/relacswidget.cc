/*
  relacswidget.cc
  Coordinates RePros, session, input, output, plotting, and saving. 

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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
#include <relacs/control.h>
#include <relacs/controltabs.h>
#include <relacs/databrowser.h>
#include <relacs/dataindex.h>
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
			    QSplashScreen *splash,
			    ModeTypes mode, QWidget *parent )
  : QMainWindow( parent ),
    ConfigClass( "RELACS", RELACSPlugin::Core ),
    Mode( mode ),
    SS( this ),
    MTDT( this ),
    SignalTime( -1.0 ),
    CurrentTime( 0.0 ),
    ReadLoop( this ),
    WriteLoop( this ),
    LogFile( 0 ),
    InfoFile( 0 ),
    InfoFileMacro( "" ),
    IsFullScreen( false ),
    IsMaximized( false ),
    AIMutex( QMutex::Recursive ),  // recursive, because of activateGains()???
    SignalMutex(),
    RunData( false ),
    RunDataMutex(),
    MinTraceTime( 0.0 ),
    DeviceMenu( 0 ),
    Help( false ),
    HandlingEvent( false )
{
  printlog( "this is RELACS, version " + string( RELACSVERSION ) );

  Thread = new UpdateThread( this );

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
  addNumber( "inputsamplerate", "Input sampling rate", 20000.0, 1.0, 1000000.0, 1000.0, "Hz", "kHz" ); // Hertz, -> 2.4MB pro minute and channel
  addNumber( "inputtracecapacity", "Ring buffer has capacity for ", 600.0, 1.0, 1000000.0, 1.0, "s" );
  addBoolean( "inputunipolar", "Unipolar input", false );
  addText( "inputtraceid", "Input trace identifier", "V-1" );
  addNumber( "inputtracescale", "Input trace scale", 1.0 );
  addText( "inputtraceunit", "Input trace unit", "V" );
  addText( "inputtracedevice", "Input trace device", "ai-1" );
  addInteger( "inputtracechannel", "Input trace channel", 0 );
  addText( "inputtracereference", "Input trace reference", InData::referenceStr( InData::RefGround ) );
  addInteger( "inputtracegain", "Input trace gain", 0 );
  addBoolean( "inputtracecenter", "Input trace center vertically", true );
  addLabel( "output data", 0, Parameter::TabLabel );
  addNumber( "maxoutputrate", "Default maximum output sampling rate", 100000.0, 1.0, 10000000.0, 1000.0, "Hz", "kHz" );
  addText( "outputtraceid", "Output trace identifier", "" );
  addInteger( "outputtracechannel", "Output trace channel", 0 );
  addText( "outputtracedevice", "Output trace device", "ao-1" );
  addNumber( "outputtracescale", "Output trace scale factor to Volt", 1.0, -10000000.0, 10000000.0, 0.1 );
  addText( "outputtraceunit", "Output trace unit", "V" );
  addNumber( "outputtracemaxrate", "Maximum output sampling rate", 0.0, 0.0, 10000000.0, 1000.0, "Hz", "kHz" );
  addNumber( "outputtracedelay", "Signal delay", 0.0, 0.0, 10.0, 0.00001, "s", "ms" );
  addText( "outputtracemodality", "Output trace modality", "voltage" );

  // main widget:
  setWindowTitle( "RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation: Version " + QString( RELACSVERSION ) );
  MainWidget = new QWidget;
  QGridLayout *mainlayout = new QGridLayout;
  mainlayout->setContentsMargins( 4, 4, 4, 4 );
  mainlayout->setSpacing( 4 );
  MainWidget->setLayout( mainlayout );
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

  // data index:
  DI = new DataIndex;
  // data browser:
  //  CW->addTab( new DataBrowser( DI, this ), "Data-Browser" );

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
  SimulationAction = filemenu->addAction( "&Simulate",
					  (QWidget*)this, SLOT( startSimulation() ),
					  Qt::CTRL + Qt::SHIFT + Qt::Key_S );
  IdleAction = filemenu->addAction( "&Idle (stop)",
				    (QWidget*)this, SLOT( stopActivity() ),
				    Qt::CTRL + Qt::SHIFT + Qt::Key_I );
  filemenu->addSeparator();
  SN->addActions( filemenu );
  MTDT.addActions( filemenu );
  filemenu->addSeparator();
  filemenu->addAction( "Settings...", &SS, SLOT( dialog() ) );
  filemenu->addAction( "Save Settings", (QWidget*)this, SLOT( saveConfig() ) );
  filemenu->addAction( "&Quit", (QWidget*)this, SLOT( quit() ), Qt::ALT + Qt::Key_Q );
  //  filemenu->addAction( "&Channels", this, SLOT( channels() ) );

  // plugins:
  QMenu *pluginmenu = menuBar()->addMenu( "&Plugins" );
  if ( MD != 0 ) {
    MD->addActions( pluginmenu, doxydoc );
    pluginmenu->addSeparator();
  }
  CW->addMenu( pluginmenu, doxydoc );

  // devices:
  DeviceMenu = menuBar()->addMenu( "De&vices" );

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
  MaximizedAction = viewmenu->addAction( "&Maximize window",
					 (QWidget*)this, SLOT( maximizeScreen() ),
					 Qt::CTRL + Qt::SHIFT + Qt::Key_M );
  FullscreenAction = viewmenu->addAction( "&Full-Screen Mode",
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
  mainlayout->addWidget( FD, 0, 0 );
  mainlayout->addWidget( PT->widget(), 0, 1 );
  mainlayout->addWidget( CW, 1, 0 );
  mainlayout->addWidget( RP, 1, 1 );
  mainlayout->addWidget( MC, 2, 0, 1, 2 );
  mainlayout->setColumnStretch( 0, 1 );
  mainlayout->setColumnStretch( 1, 100 );
  mainlayout->setRowStretch( 0, 2 );
  mainlayout->setRowStretch( 1, 3 );
  w *= 3/2;
  if ( PT->widget()->minimumWidth() < w || RP->minimumWidth() < w ) {
    PT->widget()->setMinimumWidth( w );
    RP->setMinimumWidth( w );
  }
  if ( splash != 0 )
    Thread->msleep( 2000 );

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
  delete DI;
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


int RELACSWidget::openHardware( int n, int errorlevel )
{
  Str warnings = "";
  int error = 0;

  // activate devices:
  DV->create( *ADV, n );
  warnings += DV->warnings();
  if ( ! DV->ok() )
    error |= 1;

  // activate analog input devices:
  if ( n == 0 )
    AID->create( *ADV, n );
  else
    AID->create( *ADV, 1, "AISim" );
  warnings += AID->warnings();
  if ( ! AID->ok() ) {
    error |= 3;
    warnings += "No analog input device opened!\n";
  }

  // activate analog output devices:
  if ( n == 0 )
    AOD->create( *ADV, n );
  else
    AOD->create( *ADV, 1, "AOSim" );
  warnings += AOD->warnings();
  if ( ! AOD->ok() ) {
    error |= 3;
    warnings += "No analog output device opened!\n";
  }

  // activate digital I/O devices:
  DIOD->create( *ADV, n );
  warnings += DIOD->warnings();
  if ( ! DIOD->ok() )
    error |= 1;

  // activate trigger devices:
  TRIGD->create( *ADV, n );
  warnings += TRIGD->warnings();
  if ( ! TRIGD->ok() )
    error |= 1;

  // activate attenuators:
  if ( n == 0 )
    ATD->create( *ADV, n );
  else
    ATD->create( *ADV, 1, "AttSim" );
  warnings += ATD->warnings();
  if ( ! ATD->ok() )
    error |= 3;

  ATI->create( *ADV, 0 );
  warnings += ATI->warnings();
  if ( ! ATI->ok() )
    error |= 3;

  if ( errorlevel > 0 && !warnings.empty() ) {
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
    if ( errorlevel > 1 ) {
      ws += "Can't switch to <b>" + modeStr() + "</b>-mode!";
      printlog( "! warning: " + ws.erasedMarkup() );
      MessageBox::warning( "RELACS Warning !", ws, true, 0.0, this );
    }
    else {
      printlog( ws.erasedMarkup() );
      MessageBox::information( "RELACS Info !", ws, false, 0.0, this );
    }
  }
  return error;
}


int RELACSWidget::setupHardware( int n )
{
  openHardware( n, 0 );
  int r = openHardware( n, 1 );

  // setup Acquire:
  if ( r < 2 ) {

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
  ADV->close();
  DeviceMenu->clear();
}


void RELACSWidget::setupInTraces( void )
{
  IL.clear();
  int nid = Options::size( "inputtraceid" );
  for ( int k=0; k<nid; k++ ) {
    bool failed = false;
    string traceid = text( "inputtraceid", k, "" );
    if ( traceid.empty() )
      continue;
    Str ws;
    InData id;
    id.setIdent( traceid );
    id.setUnit( number( "inputtracescale", k, 1.0 ),
		text( "inputtraceunit", k, "V" ) );
    id.setSampleRate( number( "inputsamplerate", 1000.0 ) );
    id.setStartSource( 0 );
    id.setUnipolar( boolean( "inputunipolar", false ) );
    int channel = integer( "inputtracechannel", k, -1 );
    if ( channel < 0 ) {
      ws += ", undefined channel number";
      failed = true;
    }
    id.setChannel( channel );
    Str ds = text( "inputtracedevice", k, "" );
    int devi = (int)::rint( ds.number( -1 ) );
    if ( devi < 0 || devi >= AQ->inputsSize() )
      devi = AQ->inputIndex( ds );
    if ( devi < 0 ) {
      ws += ", device <b>" + ds + "</b> not known";
      failed = true;
    }
    id.setDevice( devi );
    id.setContinuous();
    int m = SaveFiles::SaveTrace | PlotTraceMode;
    if ( boolean( "inputtracecenter", k, false ) )
      m |= PlotTraceCenterVertically;
    id.setMode( m );
    id.setReference( text( "inputtracereference", k, InData::referenceStr( InData::RefGround ) ) );
    int gain = integer( "inputtracegain", k, -1 );
    if ( gain < 0 ) {
      ws += ", undefined gain";
      failed = true;
    }
    id.setGainIndex( gain );
    if ( failed ) {
      ws.erase( 0, 2 );
      ws += " for output trace <b>" + traceid + "</b>!<br> Skip this output trace.";
      printlog( "! warning: " + ws.erasedMarkup() );
      MessageBox::warning( "RELACS Warning !", ws, true, 0.0, this );
      continue;
    }
    IL.push( id );
    IL[k].reserve( id.indices( number( "inputtracecapacity", 0, 1000.0 ) ) );
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
    AQ->addOutTrace( traceid, devi,
		     integer( "outputtracechannel", k, chan ),
		     number( "outputtracescale", k, 1.0 ),
		     text( "outputtraceunit", k, "V" ),
		     false,
		     number( "outputtracemaxrate", k, 0.0 ),
		     number( "outputtracedelay", k, 0.0 ),
		     text( "outputtracemodality", k, "unknown" ) );
    chan++;
  }

  AQ->addOutTraces();

  SF->lock();
  SF->Options::erase( SF->TraceFlag );
  for ( int k=0; k<AQ->outTracesSize(); k++ )
    SF->addNumber( AQ->outTraceName( k ), AQ->outTraceName( k ), 0.0,
		   -1.0e10, 1.0e10, 0.0001, AQ->outTrace( k ).unit() ).setFormat( "%9.3f" ).setFlags( SF->TraceFlag );
  SF->unlock();
}


///// Data thread ///////////////////////////////////////////////////////////

void RELACSWidget::updateData( void )
{
  writeLockData();
  // check data:
  if ( IL.failed() ) {
    AIErrorMsg = "Error in acquisition: " + IL.errorText();
    //    QCoreApplication::postEvent( this, new QEvent( QEvent::User+4 ) ); // this causes a lot of trouble!
    IL.clearError();    
  }
  // read data:
  lockAI();
  AQ->convertData();
  unlockAI();
  CurrentTime = IL.currentTime();
  if ( CurrentTime < 0.0 ) { // XXX DEBUG
    cerr << "RELACSWidget::updateData(): CurrentTime=" << Str( CurrentTime ) << " smaller than zero!\n";
    cerr << IL << '\n';
  }
  double ct = CurrentTime;
  unlockData();
  // do we need to wait for more data?
  MinTraceMutex.lock();
  double mintime = MinTraceTime;
  MinTraceMutex.unlock();
  while ( IL.success() && 
	  mintime > 0.0 && ct < mintime &&
	  ( simulation() || ReadLoop.isRunning() ) ) {
    RunDataMutex.lock();
    bool rd = RunData;
    RunDataMutex.unlock();
    if ( ! rd )
      break;
    // XXX wait needs a locked mutex!
    QMutex mutex;
    mutex.lock();
    if ( acquisition() )
      ReadDataWait.wait( &mutex );
    else
      ReadDataWait.wait( &mutex, 1 );
    mutex.unlock(); // XXX
    writeLockData();
    lockAI();
    AQ->convertData();
    unlockAI();
    CurrentTime = IL.currentTime();
    if ( CurrentTime < 0.0 ) { // XXX DEBUG
      cerr << "RELACSWidget::updateData(): CurrentTime=" << Str( CurrentTime ) << " smaller than zero!\n";
      cerr << IL << '\n';
    }
    ct = CurrentTime;
    unlockData();
    MinTraceMutex.lock();
    if ( mintime != MinTraceTime )
      printlog( "! warning in RELACSWidget::updateData() -> mintime=" + Str( mintime ) + " < MinTraceTime=" + Str( MinTraceTime ) + ", currentTime=" + Str( ct ) );
    mintime = MinTraceTime;
    MinTraceMutex.unlock();
  }
  setMinTraceTime( 0.0 );
  // update derived data:
  writeLockData();
  AQ->readSignal( SignalTime, IL, ED ); // we probably get the latest signal start here
  AQ->readRestart( IL, ED );
  ED.setRangeBack( ct );
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
  PT->plot();
}


void RELACSWidget::run( void )
{
  bool rd = true;
  double updateinterval = IL[0].updateTime();
  signed long ui = (unsigned long)::rint( 1000.0*updateinterval );
  QTime updatetime;
  updatetime.start();
  Thread->msleep( 1 );

  do {
    int ei = updatetime.elapsed();
    int di = ui - ei;
    if ( di < 2 )
      di = 2;
    // XXX wait needs a locked mutex!
    QMutex mutex;
    mutex.lock();
    ThreadSleepWait.wait( &mutex, di );
    mutex.unlock(); // XXX
    updatetime.restart();
    updateData();
    UpdateDataWait.wakeAll();
    processData();
    ProcessDataWait.wakeAll();
    RunDataMutex.lock();
    rd = RunData;
    RunDataMutex.unlock();
  } while( rd && ( simulation() || ReadLoop.isRunning() ) );

  if ( ! rd )
    return;

  // stop all activity:
  UpdateDataWait.wakeAll();
  stopRePro();
  CW->requestStop();
  wakeAll();
  CW->wait( 0.2 );
  ReadLoop.stop();
  WriteLoop.stop();
  ThreadSleepWait.wakeAll();
  ReadDataWait.wakeAll();
  SimLoad.stop();
  if ( AQ != 0 ) {
    lockSignals();
    lockAI();
    AQ->stop();
    unlockAI();
    unlockSignals();
  }
  qApp->processEvents();
  closeHardware();
  RP->activateRePro( 0 );
  AcquisitionAction->setEnabled( true );
  SimulationAction->setEnabled( true );
  IdleAction->setEnabled( false );
  setMode( IdleMode );
  RP->message( "<b>Idle-mode</b>" );
}


void RELACSWidget::setMinTraceTime( double t )
{
  MinTraceMutex.lock();
  MinTraceTime = t;
  MinTraceMutex.unlock();
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

void RELACSWidget::activateGains( bool datalocked )
{
  if ( datalocked )
    unlockData();
  writeLockData();
  lockAI();
  AQ->activateGains();
  unlockAI();
  AQ->readRestart( IL, ED );
  FD->adjust( IL, ED, AQ->adjustFlag() );
  unlockData();
  if ( datalocked )
    readLockData();
}


class OutDataEvent : public QEvent
{

public:

  OutDataEvent( const OutData &signal, const Acquire *aq )
    : QEvent( Type( User+5 ) ),
      Signal( signal )
  {
    int inx = -1;
    if ( ! signal.traceName().empty() )
      inx = aq->outTraceIndex( signal.traceName() );
    else
      inx = signal.trace();
    if ( inx >= 0 )
      Signal.setTraceName( aq->outTraceName( inx ) );
  }

  OutDataInfo Signal;
};


class OutListEvent : public QEvent
{

public:

  OutListEvent( const OutList &signal )
    : QEvent( Type( User+6 ) )
  {
    for ( int k=0; k<signal.size(); k++ )
      Signals.push_back( signal[k] );
  }

  deque< OutDataInfo > Signals;
};


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
      UpdateDataWait.wait();
    } while ( SF->signalPending() );
    CurrentRePro->lockAll();
  }
  */
  if ( SF->signalPending() )
    printlog( "! warning in write() -> previous signal still pending in SaveFiles !" );
  lockSignals();
  int r = AQ->setupWrite( signal );
  if ( r >= 0 ) {
    SF->unlock();    // we assume that IL data are read locked while calling write()
    MTDT.unlock();   // we need to unlock all, so that the data lock can be freed.
    unlockData();
    writeLockData(); // IL data need to be write locked, because data might be truncated in Acquire::restartRead()
    lockAI();
    r = AQ->startWrite( signal );
    unlockAI();
    unlockData();
    readLockData();  // put the data lock back into read lock state
    MTDT.lock();     // and the other locks as well
    SF->lock();
  }
  unlockSignals();
  if ( r == 0 ) {
    WriteLoop.start( signal.writeTime() );
    lockSignals();
    SF->save( signal );
    QCoreApplication::postEvent( this, new OutDataEvent( signal, AQ ) );
    unlockSignals();
    AQ->readSignal( SignalTime, IL, ED ); // if acquisition was restarted we here get the signal start
    AQ->readRestart( IL, ED );
    FD->adjust( IL, ED, AQ->adjustFlag() );
    // update device menu:
    QCoreApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+2 ) ) );
    SF->setNumber( AQ->outTraceName( signal.trace() ), signal.back() );
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
      UpdateDataWait.wait();
    } while ( SF->signalPending() );
    CurrentRePro->lockAll();
  }
  */
  if ( SF->signalPending() )
    printlog( "! warning in write() -> previous signal still pending in SaveFiles !" );
  lockSignals();
  int r = AQ->setupWrite( signal );
  if ( r >= 0 ) {
    SF->unlock();    // we assume that IL data are read locked while calling write()
    MTDT.unlock();   // we need to unlock all, so that the data lock can be freed.
    unlockData();
    writeLockData(); // IL data need to be write locked, because data might be truncated in Acquire::restartRead()
    lockAI();
    r = AQ->startWrite( signal );
    unlockAI();
    unlockData();
    readLockData();  // put the data lock back into read lock state
    MTDT.lock();     // and the other locks as well
    SF->lock();
  }
  unlockSignals();
  if ( r == 0 ) {
    WriteLoop.start( signal[0].writeTime() );
    lockSignals();
    SF->save( signal );
    QCoreApplication::postEvent( this, new OutListEvent( signal ) );
    unlockSignals();
    AQ->readSignal( SignalTime, IL, ED ); // if acquisition was restarted we here get the signal start
    AQ->readRestart( IL, ED );
    FD->adjust( IL, ED, AQ->adjustFlag() );
    // update device menu:
    QCoreApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+2 ) ) );
    for ( int k=0; k<signal.size(); k++ )
      SF->setNumber( AQ->outTraceName( signal[k].trace() ), signal[k].back() );
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
      UpdateDataWait.wait();
    } while ( SF->signalPending() );
    CurrentRePro->lockAll();
  }
  */
  if ( SF->signalPending() )
    printlog( "! warning in write() -> previous signal still pending in SaveFiles !" );
  lockSignals();
  SF->unlock();    // we assume that IL data are read locked while calling write()
  MTDT.unlock();   // we need to unlock all, so that the data lock can be freed.
  unlockData();
  writeLockData(); // IL data need to be write locked, because data might be truncated in Acquire::restartRead()
  lockAI();
  int r = AQ->directWrite( signal );
  unlockAI();
  unlockData();
  readLockData();  // put the data lock back into read lock state
  MTDT.lock();     // and the other locks as well
  SF->lock();
  unlockSignals();
  if ( r == 0 ) {
    lockSignals();
    SF->save( signal );
    QCoreApplication::postEvent( this, new OutDataEvent( signal, AQ ) );
    unlockSignals();
    AQ->readSignal( SignalTime, IL, ED ); // if acquisition was restarted we here get the signal start
    AQ->readRestart( IL, ED );
    FD->adjust( IL, ED, AQ->adjustFlag() );
    // update device menu:
    QCoreApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+2 ) ) );
    SF->setNumber( AQ->outTraceName( signal.trace() ), signal.back() );
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
      UpdateDataWait.wait();
    } while ( SF->signalPending() );
    CurrentRePro->lockAll();
  }
  */
  if ( SF->signalPending() )
    printlog( "! warning in write() -> previous signal still pending in SaveFiles !" );
  lockSignals();
  SF->unlock();    // we assume that IL data are read locked while calling write()
  MTDT.unlock();   // we need to unlock all, so that the data lock can be freed.
  unlockData();
  writeLockData(); // IL data need to be write locked, because data might be truncated in Acquire::restartRead()
  lockAI();
  int r = AQ->directWrite( signal );
  unlockAI();
  unlockData();
  readLockData();  // put the data lock back into read lock state
  MTDT.lock();     // and the other locks as well
  SF->lock();
  unlockSignals();
  if ( r == 0 ) {
    lockSignals();
    SF->save( signal );
    QCoreApplication::postEvent( this, new OutListEvent( signal ) );
    unlockSignals();
    AQ->readSignal( SignalTime, IL, ED ); // if acquisition was restarted we here get the signal start
    AQ->readRestart( IL, ED );
    FD->adjust( IL, ED, AQ->adjustFlag() );
    // update device menu:
    QCoreApplication::postEvent( this, new QEvent( QEvent::Type( QEvent::User+2 ) ) );
    for ( int k=0; k<signal.size(); k++ )
      SF->setNumber( AQ->outTraceName( signal[k].trace() ), signal[k].back() );
  }
  else
    printlog( "! failed to write signals: " + signal.errorText() );
  if ( IL.failed() )
    printlog( "! error in restarting analog input: " + IL.errorText() );
  return r;
}


int RELACSWidget::stopWrite( void )
{
  // stop analog output:
  WriteLoop.stop();
  lockSignals();
  int r = AQ->stopWrite();                
  unlockSignals();
  return r;
}


void RELACSWidget::notifyStimulusData( void )
{
  if ( MD != 0 )
    MD->notifyStimulusData();
  FD->notifyStimulusData();
  CW->notifyStimulusData();
  RP->notifyStimulusData();
}


void RELACSWidget::notifyMetaData( const string &section )
{
  if ( MD != 0 )
    MD->notifyMetaData( section );
  FD->notifyMetaData( section );
  CW->notifyMetaData( section );
  RP->notifyMetaData( section );
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
    *InfoFile << QTime::currentTime().toString().toStdString();
    *InfoFile << "   " << CurrentRePro->name() << ": " << MC->options();
  }
  DI->addRepro( *CurrentRePro );

  ReProRunning = true;
  SN->incrReProCount();
  
  readLockData();
  SF->holdOn();
  CurrentRePro->setSaving( saving );
  SF->save( *CurrentRePro );
  unlockData();
  /* XXX
  // make sure all paintEvents are finished:???
  if ( CurrentRePro->widget() != 0 )
    QCoreApplication::sendPostedEvents( CurrentRePro->widget(), 0 );
  */
  CurrentRePro->start( QThread::HighPriority );
}


void RELACSWidget::stopRePro( void )
{
  if ( ! ReProRunning )
    return;

  // wait on RePro to stop:
  if ( CurrentRePro->isRunning() ) {
    /*
    // wait for the RePro to leave sensible code:
    CurrentRePro->lock();
    // RePro needs to be unlocked here,
    // so that sendPostedEvents() does not wait on the RePro to unlock()!
    CurrentRePro->unlock();
    */

    // dispatch all posted events (that usually paint the RePro...)
    // as long as the RePro is normally running, so that it has
    // still all internal variables available:
    QCoreApplication::sendPostedEvents();
    //    qApp->processEvents( QEventLoop::AllEvents, 100 );

    // request and wait for the RePro to properly terminate:
    CurrentRePro->requestStop();
    // the RePro may wait for an event to be processed:
    while ( CurrentRePro->isRunning() )
      qApp->processEvents( QEventLoop::AllEvents, 10 );
    CurrentRePro->wait();
  }

  ReProRunning = false;
  window()->setFocus();

  if ( AQ->readSignal( SignalTime, IL, ED ) ) // we should get the start time of the latest signal here
    SF->save( IL, ED );
  // last stimulus still not saved?
  if ( SF->signalPending() ) {
    // force data updates:
    ThreadSleepWait.wakeAll();
    if ( ReadLoop.isRunning() ) {
      // XXX wait needs a locked mutex!
      QMutex mutex;
      mutex.lock();
      ProcessDataWait.wait( &mutex );
      mutex.unlock(); // XXX
    }
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
    lockSignals();
    AOD->updateMenu();
    ATD->updateMenu();
    ATI->updateMenu();
    unlockSignals();
    TRIGD->updateMenu();
    break;
  }

  case 3: {
    MessageBox::error( "RELACS Error !", "Transfering stimulus data to hardware driver failed.", this );
    break;
  }

  case 4: {
    MessageBox::warning( "RELACS Error !", AIErrorMsg, 2.0, this );
    break;
  }

  case 5: {
    OutDataEvent *ode = dynamic_cast<OutDataEvent*>( qce );
    DI->addStimulus( ode->Signal );
    break;
  }

  case 6: {
    OutListEvent *ole = dynamic_cast<OutListEvent*>( qce );
    DI->addStimulus( ole->Signals );
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

  InfoFile = new ofstream( SF->addPath( "repros.dat" ).c_str() );
  if ( ! InfoFile->good() ) {
    printlog( "! warning: InfoFile not good" );
    delete InfoFile;
    InfoFile = 0;
  }
  else {
    *InfoFile << "This is RELACS, Version " << RELACSVERSION << "\n\n";
    *InfoFile << "The session was started at time "
	     << QTime::currentTime().toString().toStdString() << " on "
	     << QDate::currentDate().toString().toStdString() << "\n\n"
	     << "Time:      Research Program:\n";
  }

  DI->addSession( SF->path() + "stimuli.dat", Options() );

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
  printlog( "resume session" );
}


void RELACSWidget::stopSession( bool saved )
{
  printlog( "stop session" );

  string modeltitle = "";
  Options modelopts;
  if ( MD != 0 && simulation() ) {
    modeltitle = "Simulation parameter";
    modelopts = MD->metaData();
  }

  MTDT.save( modeltitle, modelopts );

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

  QPalette p( palette() );
  p.setColor( QPalette::Window, OrgBackground );
  MainWidget->setPalette( p );

  if ( LogFile != 0 ) {
    delete LogFile;
    LogFile = 0;
  }

  if ( InfoFile != 0 ) {
    *InfoFile << "\n\n"
	     << "The session was stopped at time " 
	     << QTime::currentTime().toString().toStdString() << " on "
	     << QDate::currentDate().toString().toStdString() << '\n';
    delete InfoFile;
    InfoFile = 0;
  }

  DI->endSession( saved );

  SessionStopWait.wakeAll();

  if ( saved )
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

  // stop data threads:
  RunDataMutex.lock();
  RunData = false;
  RunDataMutex.unlock();
  ReadLoop.stop();
  WriteLoop.stop();
  ThreadSleepWait.wakeAll();
  ReadDataWait.wakeAll();
  if ( Thread->isRunning() )
    Thread->wait();

  // stop simulation and data acquisition:
  SimLoad.stop();
  if ( AQ != 0 ) {
    lockSignals();
    lockAI();
    AQ->stop();
    unlockAI();
    unlockSignals();
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
  printlog( "quitting RELACS" );
  qApp->quit();
}


void RELACSWidget::closeEvent( QCloseEvent *ce )
{
  quit();
  ce->accept();
}


void RELACSWidget::channels( void )
{
  InputConfig *ic = new InputConfig( this );
  OptDialog *od = new OptDialog( false, this );
  od->addWidget( ic );
  od->exec();

  OutputConfig *oc = new OutputConfig( this );
  OptDialog *od2 = new OptDialog( false, this );
  od2->addWidget( oc );
  od2->exec();
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
  CurrentTime = 0.0;
  setupInTraces();
  if ( IL.empty() ) {
    printlog( "! error: No valid input traces configured!" );
    MessageBox::error( "RELACS Error !", "No valid input traces configured!", this );
    startIdle();
    return;
  }
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
  PT->resize();
  PT->updateMenu();

  CW->initialize();

  // XXX before configuring, something like AQ->init would be nice
  // in order to set the IL gain factors.
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

  // controls:
  CW->initDevices();
  RP->setSettings();

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
  Thread->start( QThread::HighPriority );

  // reset analog output for dynamic clamp:
  lockAI();
  string wr = AQ->writeReset( true, true );
  unlockAI();
  if ( ! wr.empty() ) {
    printlog( "! warning: RELACSWidget::startFirstAcquisition() -> resetting analog output failed: " + wr );
    MessageBox::warning( "RELACS Warning !",
			 "error in resetting analog output: " + wr,
			 true, 0.0, this );
    startIdle();
    return;
  }

  CW->start();

  // get first RePro and start it:
  MC->startUp(); 

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
  CurrentTime = 0.0;
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
  PT->resize();
  PT->updateMenu();

  SimLabel->setText( "" );
  SimLabel->show();

  CW->initialize();

  // XXX before configuring, something like AQ->init would be nice
  // in order to set the IL gain factors.
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

  // controls:
  CW->initDevices();
  RP->setSettings();

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
  Thread->start( QThread::HighPriority );
  CW->start();

  // get first RePro and start it:
  MC->startUp(); 

  AcquisitionAction->setEnabled( true );
  SimulationAction->setEnabled( false );
  IdleAction->setEnabled( true );

  printlog( "Simulation-mode started" );
}


void RELACSWidget::startIdle( void )
{
  CW->initialize();
  CFG.read( RELACSPlugin::Plugins );
  CFG.configure( RELACSPlugin::Plugins );
  CW->initDevices();
  RP->setSettings();
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


//----------------------------Private Functions----------------------------//


void RELACSWidget::fullScreen( void )
{
  if ( IsFullScreen ) {
    showNormal();
    IsFullScreen = false;
    FullscreenAction->setText( "&Full-Screen Mode" );
  }
  else {
    showFullScreen();
    IsFullScreen = true;
    FullscreenAction->setText( "Exit &Full-Screen Mode" );
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
    MaximizedAction->setText( "Exit &Maximize window" );
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


UpdateThread::UpdateThread( RELACSWidget *rw )
  : QThread( rw ),
    RW( rw )
{
}


void UpdateThread::run( void )
{
  RW->run();
}


void UpdateThread::msleep( unsigned long msecs )
{
  QThread::msleep( msecs );
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

