/*
  relacswidget.cc
  Coordinates RePros, session, input, output, plotting, and saving. 

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
#include <relacs/relacswidget.h>
#include <relacs/plugins.h>
#include <relacs/defaultsession.h>
#include <relacs/aisim.h>
#include <relacs/aosim.h>
#include <relacs/attsim.h>
#include <relacs/attenuate.h>
#include <relacs/acquire.h>
#include <relacs/control.h>
#include <relacs/databrowser.h>
#include <relacs/filter.h>
#include <relacs/filterdetectors.h>
#include <relacs/macros.h>
#include <relacs/model.h>
#include <relacs/messagebox.h>
#include <relacs/relacsdevices.h>
#include <relacs/repros.h>
#include <relacs/savefiles.h>
#include <relacs/session.h>
#include <relacs/simulator.h>
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

  // filter and detectors:
  FD = new FilterDetectors( this );

  // macros:
  MC = new Macros( this );

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
  CW = new QTabWidget;

  OrgBackground = palette().color( QPalette::Window );

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
	if ( cn->widget() != 0 )
	  CW->addTab( cn->widget(), cn->name().c_str() );
	cn->setRELACSWidget( this );
	CN.push_back( cn );
      }
    }
  }

  // data browser:
  DB = new DataBrowser( this );
  CW->addTab( DB, "Data-Browser" );

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
  QWidget::connect( RP, SIGNAL( stopRePro( void ) ), 
		    (QWidget*)this, SLOT( stopRePro( void ) ) );
  QWidget::connect( RP, SIGNAL( startRePro( RePro*, int, bool ) ), 
		    (QWidget*)this, SLOT( startRePro( RePro*, int, bool ) ) );
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

  // plugins:
  QMenu *pluginmenu = menuBar()->addMenu( "&Plugins" );
  if ( MD != 0 ) {
    MD->addActions( pluginmenu, doxydoc );
    pluginmenu->addSeparator();
  }
  if ( ! CN.empty() ) {
    for ( unsigned int k=0; k<CN.size(); k++ )
      CN[k]->addActions( pluginmenu, doxydoc );
  }

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
  QWidget::connect( MC, SIGNAL( stopRePro( void ) ), 
		    (QWidget*)this, SLOT( stopRePro( void ) ) );
  QWidget::connect( MC, SIGNAL( startRePro( RePro*, int, bool ) ), 
		    (QWidget*)this, SLOT( startRePro( RePro*, int, bool ) ) );
  QWidget::connect( RP, SIGNAL( noMacro( RePro* ) ), 
		    MC, SLOT( noMacro( RePro* ) ) );
  QWidget::connect( RP, SIGNAL( reloadRePro( const string& ) ),
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
  /*
  int wd = FD->minimumSizeHint().width();
  int wc = CW->minimumSizeHint().width();
  */
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
    msleep( 2000 );

  // miscellaneous:
  setFocusPolicy( Qt::StrongFocus );
  window()->setFocus();
  KeyTime = new KeyTimeOut( window() );

}


RELACSWidget::~RELACSWidget( void )
{
  stopThreads();
  clearActivity();
  if ( MD != 0 ) {
    Plugins::destroy( MD->name(), RELACSPlugin::ModelId );
    delete MD;
  }
  delete FD;
  for ( unsigned int k=0; k<CN.size(); k++ ) {
    Plugins::destroy( CN[k]->name(), RELACSPlugin::ControlId );
    delete CN[k];
  }
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
  cerr << QTime::currentTime().toString().toLatin1().data() << " "
       << message << endl;
  if ( LogFile != 0 )
    *LogFile << QTime::currentTime().toString().toLatin1().data() << " "
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
      QWidget::connect( &SimLoad, SIGNAL( timeout() ),
			(QWidget*)this, SLOT( simLoadMessage() ) );
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
  unlockData();
  // do we need to wait for more data?
  MinTraceMutex.lock();
  double mintime = MinTraceTime;
  MinTraceMutex.unlock();
  while ( IL.success() && CurrentTime < mintime ) {
    RunDataMutex.lock();
    bool rd = RunData;
    RunDataMutex.unlock();
    if ( ! rd )
      break;
    // XXX wait needs a locked mutex!
    QMutex mutex;
    mutex.lock();
    if ( acquisition() ) {
      ReadDataWait.wait( &mutex );
    }
    else
      ReadDataWait.wait( &mutex, 1 );
    mutex.unlock(); // XXX
    writeLockData();
    lockAI();
    AQ->convertData();
    unlockAI();
    CurrentTime = IL.currentTime();
    unlockData();
  }
  setMinTraceTime( 0.0 );
  // update derived data:
  writeLockData();
  AQ->readSignal( SignalTime, IL, ED ); // we probably get the latest signal start here
  AQ->readRestart( IL, ED );
  ED.setRangeBack( CurrentTime );
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
  QThread::msleep( 1 );

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
    lockAI();
    r = AQ->startWrite( signal );
    unlockAI();
  }
  unlockSignals();
  if ( r == 0 ) {
    WriteLoop.start( signal.writeTime() );
    lockSignals();
    SF->save( signal );
    unlockSignals();
    AQ->readSignal( SignalTime, IL, ED ); // if acquisition was restarted we here get the signal start
    AQ->readRestart( IL, ED );
    FD->adjust( IL, ED, AQ->adjustFlag() );
    // update device menu:
    QCoreApplication::postEvent( (QWidget*)this,
				 new QEvent( QEvent::Type( QEvent::User+2 ) ) );
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
    lockAI();
    r = AQ->startWrite( signal );
    unlockAI();
  }
  unlockSignals();
  if ( r == 0 ) {
    WriteLoop.start( signal[0].writeTime() );
    lockSignals();
    SF->save( signal );
    unlockSignals();
    AQ->readSignal( SignalTime, IL, ED ); // if acquisition was restarted we here get the signal start
    AQ->readRestart( IL, ED );
    FD->adjust( IL, ED, AQ->adjustFlag() );
    // update device menu:
    QCoreApplication::postEvent( (QWidget*)this,
				 new QEvent( QEvent::Type( QEvent::User+2 ) ) );
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
  lockAI();
  int r = AQ->directWrite( signal );
  unlockAI();
  unlockSignals();
  if ( r == 0 ) {
    lockSignals();
    SF->save( signal );
    unlockSignals();
    AQ->readSignal( SignalTime, IL, ED ); // if acquisition was restarted we here get the signal start
    AQ->readRestart( IL, ED );
    FD->adjust( IL, ED, AQ->adjustFlag() );
    // update device menu:
    QCoreApplication::postEvent( (QWidget*)this,
				 new QEvent( QEvent::Type( QEvent::User+2 ) ) );
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
  lockAI();
  int r = AQ->directWrite( signal );
  unlockAI();
  unlockSignals();
  if ( r == 0 ) {
    lockSignals();
    SF->save( signal );
    unlockSignals();
    AQ->readSignal( SignalTime, IL, ED ); // if acquisition was restarted we here get the signal start
    AQ->readRestart( IL, ED );
    FD->adjust( IL, ED, AQ->adjustFlag() );
    // update device menu:
    QCoreApplication::postEvent( (QWidget*)this,
				 new QEvent( QEvent::Type( QEvent::User+2 ) ) );
    for ( int k=0; k<signal.size(); k++ )
      SF->setNumber( AQ->outTraceName( signal[k].trace() ), signal[k].back() );
  }
  else
    printlog( "! failed to write signals: " + signal.errorText() );
  if ( IL.failed() )
    printlog( "! error in restarting analog input: " + IL.errorText() );
  return r;
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
    *InfoFile << QTime::currentTime().toString().toLatin1().data();
    *InfoFile << "   " << CurrentRePro->name() << ": " << MC->options();
  }
  DB->addRepro( CurrentRePro );

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
  CurrentRePro->start( HighPriority );
}


void RELACSWidget::stopRePro( void )
{
  if ( ! ReProRunning )
    return;

  // stop analog output:
  WriteLoop.stop();
  lockSignals();
  AQ->stopWrite();                
  unlockSignals();

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
    qApp->processEvents( QEventLoop::AllEvents, 100 );

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
    // XXX wait needs a locked mutex!
    QMutex mutex;
    mutex.lock();
    ProcessDataWait.wait( &mutex );
    mutex.unlock(); // XXX
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
  if ( qce->type() == QEvent::User+1 ) {
    MC->startNextRePro( true );
  }
  else if ( qce->type() == QEvent::User+2 ) {
    AOD->updateMenu();
    ATD->updateMenu();
    ATI->updateMenu();
    TRIGD->updateMenu();
  }
  else if ( qce->type() == QEvent::User+3 ) {
    MessageBox::error( "RELACS Error !", "Transfering stimulus data to hardware driver failed.", this );
  }
  else if ( qce->type() == QEvent::User+4 ) {
    MessageBox::warning( "RELACS Error !", AIErrorMsg, 2.0, this );
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
	     << QTime::currentTime().toString().toLatin1().data() << " on "
	     << QDate::currentDate().toString().toLatin1().data() << "\n\n"
	     << "Time:      Research Program:\n";
  }

  DB->addSession( SF->path() );

  SessionStartWait.wakeAll();

  if ( MD != 0 )
    MD->sessionStarted();
  FD->sessionStarted();
  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->sessionStarted();
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

  MTDT.save();

  if ( MD != 0 )
    MD->sessionStopped( saved );
  FD->sessionStopped( saved );
  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->sessionStopped( saved );
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
	     << QTime::currentTime().toString().toLatin1().data() << " on "
	     << QDate::currentDate().toString().toLatin1().data() << '\n';
    delete InfoFile;
    InfoFile = 0;
  }

  SessionStopWait.wakeAll();

  if ( MC->stopSessionIndex() >= 0 && saved )
    MC->stopSession();

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
    CN[k]->wait( 0.2 );

  // stop data threads:
  ReadLoop.stop();
  WriteLoop.stop();
  RunDataMutex.lock();
  RunData = false;
  RunDataMutex.unlock();
  ThreadSleepWait.wakeAll();
  ReadDataWait.wakeAll();
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

  // plot:
  PT->resize();
  PT->updateMenu();

  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->initialize();

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
  for ( unsigned int k=0; k<CN.size(); k++ ) {
    CN[k]->setSettings();
    CN[k]->initDevices();
  }

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

  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->initialize();

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
  for ( unsigned int k=0; k<CN.size(); k++ ) {
    CN[k]->setSettings();
    CN[k]->initDevices();
  }

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
  QThread::start( HighPriority );

  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->start();

  // get first RePro and start it:
  MC->startUp(); 

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
  // note: because keyPressEvent() is protected, we need to call
  // this function via sendEvent().
  // This has the advantage, that all the eventFilters, etc. are called as well.
  QCoreApplication::sendEvent( PT->widget(), event );
  if ( ! event->isAccepted() &&
       CurrentRePro != 0 &&
       CurrentRePro->widget() != 0  ) {
    // even RePros without a widget get one automatically assigned by RePros.
    // So, via the RELACSPlugin::eventFilter() they also get their
    // keyPressEvent() called!
    QCoreApplication::sendEvent( CurrentRePro->widget(), event );
  }
  if ( ! event->isAccepted() )
    QCoreApplication::sendEvent( CW->currentWidget(), event );
  for ( unsigned int k=0; k<CN.size() && ! event->isAccepted(); k++ ) {
    if ( CN[k]->globalKeyEvents() &&
	 CN[k]->widget() != 0 &&
	 CN[k]->widget() != CW->currentWidget() )
      QCoreApplication::sendEvent( CN[k]->widget(), event );
  }
  if ( ! event->isAccepted() ) {
    QCoreApplication::sendEvent( FD, event );
  }
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
  QCoreApplication::sendEvent( PT->widget(), event );
  if ( ! event->isAccepted() &&
       CurrentRePro != 0 &&
       CurrentRePro->widget() != 0  )
    QCoreApplication::sendEvent( CurrentRePro->widget(), event );
  if ( ! event->isAccepted() )
    QCoreApplication::sendEvent( CW->currentWidget(), event );
  for ( unsigned int k=0; k<CN.size() && ! event->isAccepted(); k++ ) {
    if ( CN[k]->globalKeyEvents() &&
	 CN[k]->widget() != 0 &&
	 CN[k]->widget() != CW->currentWidget() )
      QCoreApplication::sendEvent( CN[k]->widget(), event );
  }
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
  QWidget::connect( od, SIGNAL( dialogClosed( int ) ),
		    (QWidget*)this, SLOT( helpClosed( int ) ) );
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

