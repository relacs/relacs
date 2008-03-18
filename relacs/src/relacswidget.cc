/*
  ../include/relacs/relacswidget.cc
  Coordinates RePros, session, input, output, plotting, and saving. 

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
#include <relacs/defaultsession.h>
#include <relacs/aisim.h>
#include <relacs/aosim.h>
#include <relacs/attsim.h>
#include <relacs/attenuate.h>
#include <relacs/filter.h>
#include <relacs/model.h>
#include <relacs/messagebox.h>
#include <relacs/optdialog.h>


const string RELACSWidget::ModeStr[5] = { "Idle", "Acquisition", "Simulation", "", "Analysis" };


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


RELACSWidget::RELACSWidget( const string &configbase, int mode,
			    QWidget *parent, const char *name )
  : QMainWindow( parent, name ),
    ConfigClass( "RELACS", RELACSPlugin::Core ),
    Mode( mode ),
    SS( this ),
    MTDT( this ),
    Setup( &MTDT ),
    LogFile( 0 ),
    InfoFile( 0 ),
    InfoFileMacro( "" ),
    IsFullScreen( false ),
    GUILock( 0 ),
    DataMutex( true ),  // recursive, because of activateGains()!
    DataMutexCount( 0 ),
    DeviceMenu( 0 ),
    Help( false )
{
  printlog( "this is RELACS, version " + string( RELACSVERSION ) );
  // setup configure:
  CFG.addGroup( configbase + ".cfg" );
  CFG.addGroup( configbase + "plugins.cfg" );

  // configuration parameter for RELACS:
  addConfig();
  addLabel( "input data", 0, Parameter::TabLabel );
  addInteger( "inputtraces", "Number of input-traces", 1 );
  addNumber( "inputsamplerate", "Input sampling rate", 20000.0, 1.0, 1000000.0, 1000.0, "Hz", "kHz" ); // Hertz, -> 2.4MB pro minute and channel
  addNumber( "inputtracecapacity", "Ring buffer has capacity for ", 600.0, 1.0, 1000000.0, 1.0, "s" );
  addNumber( "inputtracetime", "Buffer of driver can hold data for ", 1.0, 0.001, 10.0, 0.001, "s", "ms"  );
  addInteger( "inputstartsource", "Start source for input", 0 );
  addBoolean( "inputunipolar", "Unipolar input", false );
  addText( "inputtraceid", "Input trace identifier", "V-1" );
  addNumber( "inputtracescale", "Input trace scale", 1.0 );
  addNumber( "inputtraceoffset", "Input trace offset", 0.0 );
  addText( "inputtraceunit", "Input trace unit", "V" );
  addInteger( "inputtracechannel", "Input trace channel", 0 );
  addInteger( "inputtracedevice", "Input trace device", 0 );
  addInteger( "inputtracegain", "Input trace gain", 0 );
  addLabel( "output data", 0, Parameter::TabLabel );
  addNumber( "maxoutputrate", "Default maximum output sampling rate", 100000.0, 1.0, 10000000.0, 1000.0, "Hz", "kHz" );
  addText( "outputtraceid", "Output trace identifier", "out-1" );
  addInteger( "outputtracechannel", "Output trace channel", 0 );
  addText( "outputtracedevice", "Output trace device", "ao-1" );
  addNumber( "outputtracescale", "Output trace scale factor to Volt", 1.0, -10000000.0, 10000000.0, 0.1 );
  addNumber( "outputtraceoffset", "Output trace offset", 0.0, -10000000.0, 10000000.0, 0.1 );
  addText( "outputtraceunit", "Output trace unit", "V" );
  addNumber( "outputtracemaxrate", "Maximum output sampling rate", 0.0, 0.0, 10000000.0, 1000.0, "Hz", "kHz" );
  addNumber( "outputtracedelay", "Signal delay", 0.0, 0.0, 10.0, 0.00001, "s", "ms" );

  // main widget:
  setCaption( "RELACS - Realtime ELectrophysiological data Acquisition, Control, and Stimulation: Version " + QString( RELACSVERSION ) );
  MainWidget = new QWidget( this );
  QGridLayout *mainlayout = new QGridLayout( MainWidget, 2, 2, 5, 5, "RELACSWidget::MainLayout" );
  setCentralWidget( MainWidget );

  // filter and detectors:
  FD = new FilterDetectors( this, MainWidget, "RELACSWidget::FilterDetectors" );

  // macros:
  MC = new Macros( this, MainWidget, "RELACSWidget::Macros" );

  // data acquisition:
  AQD = new Acquire();

  // simulator:
  SIM = new Simulator();

  // devices:
  ADV = new AllDevices();
  DV = new Devices();
  AID = new AIDevices();
  AOD = new AODevices();

  // attenuators:
  ATD = new AttDevices();
  ATI = new AttInterfaces( AOD );

  // load config:
  CFG.read( RELACSPlugin::Core );
  CFG.configure( RELACSPlugin::Core );

  // loading plugins:
  PG = new Plugins();
  PG->add( "AISim", RELACSPlugin::AnalogInputId, createAISim, PLUGINVERSION );
  PG->add( "AOSim", RELACSPlugin::AnalogOutputId, createAOSim, PLUGINVERSION );
  PG->add( "AttSim", RELACSPlugin::AttenuatorId, createAttSim, PLUGINVERSION );
  PG->setChangeToPluginDir( SS.boolean( "changetoplugindir" ) );
  string pluginhome = SS.text( "pluginhome" );
  for ( int k=0; k<SS.Options::size( "pluginpathes" ); k++ ) {
    string pluginlib = SS.text( "pluginpathes", k );
    if ( !pluginlib.empty() ) {
      if ( pluginlib[pluginlib.size()-1] == '/' )
	pluginlib += "*.so";
      PG->openPath( pluginlib, pluginhome );
    }
  }
  if ( PG->empty() ) {
    printlog(  "! error: No valid plugins found. Exit now." );
    MessageBox::error( "RELACS Error !", "No valid plugins found.<br>\nExit now.", this );
    qApp->exit( 1 );
    ::exit( 1 ); // do we need that?
  }
  else {
    if ( !PG->libraryErrors().empty() ) {
      Str ws = "Errors in loading library\n";
      string s = PG->libraryErrors();
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
    if ( !PG->classErrors().empty() ) {
      Str ws = "Errors in following plugins (recompile them!):\n";
      string s = PG->classErrors();
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
      Control *cn = (Control *)PG->create( cm, RELACSPlugin::ControlId );
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
    MD = (Model *)PG->create( ms, RELACSPlugin::ModelId );
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
  FW = new SaveFiles( this, statusbarheight, statusBar(), "RELACSWidget::FW" );
  statusBar()->addWidget( FW, 0, false );
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

  // miscellaneous:
  setFocusPolicy( QWidget::StrongFocus );
  KeyTime = new KeyTimeOut( topLevelWidget() );
  qApp->installEventFilter( KeyTime );

}


RELACSWidget::~RELACSWidget( void )
{
  stopThreads();
  saveConfig();
  PG->close();
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
  DV->create( *PG, *ADV, n );
  warnings += DV->warnings();

  // activate analog input devices:
  if ( n == 0 )
    AID->create( *PG, *ADV, n );
  else
    AID->create( *PG, *ADV, 1, "AISim" );
  warnings += AID->warnings();
  if ( ! AID->ok() ) {
    Fatal = true;
    warnings += "No analog input device opened!\n";
  }

  // activate analog output devices:
  if ( n == 0 )
    AOD->create( *PG, *ADV, n );
  else
    AOD->create( *PG, *ADV, 1, "AOSim" );
  warnings += AOD->warnings();
  if ( ! AOD->ok() ) {
    Fatal = true;
    warnings += "No analog output device opened!\n";
  }

  // activate attenuators:
  if ( n == 0 )
    ATD->create( *PG, *ADV, n );
  else
    ATD->create( *PG, *ADV, 1, "AttSim" );
  warnings += ATD->warnings();
  if ( ! ATD->ok() )
    Fatal = true;

  ATI->create( *PG, *ADV, 0 );
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
      ws += "Can't switch to <b>" + ModeStr[mode()] + "</b>-mode!";
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
		   number( "inputtraceoffset", k, 0.0 ), 
		   text( "inputtraceunit", k, "" ) );
    IL[k].setSampleRate( number( "inputsamplerate", 1000.0 ) );
    IL[k].setStartSource( integer( "inputstartsource", 0, 0 ) );
    IL[k].setUnipolar( boolean( "inputunipolar", false ) );
    IL[k].setChannel( integer( "inputtracechannel", k, k ) );
    IL[k].setDevice( integer( "inputtracedevice", k, 0 ) );
    IL[k].setContinuous();
    IL[k].setMode( SaveFilesMode | PlotTraceMode );
    IL[k].setGainIndex( integer( "inputtracegain", k, 0 ) );
    IL[k].setUpdateTime( number( "inputtracetime", 0, 1.0 ) );
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
		     number( "outputtraceoffset", k, 0.0 ),
		     text( "outputtraceunit", k, "V" ),
		     false,
		     number( "outputtracemaxrate", k, 0.0 ),
		     number( "outputtracedelay", k, 0.0 ) );
    chan++;
  }

  AQ->addOutTraces();

}


///// Data thread ///////////////////////////////////////////////////////////

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


void RELACSWidget::updateData( void )
{
  writeLockData();
  if ( AQ->readData() < 0 )
    printlog( "! error in reading acquired data: " + IL.errorText() );
  ED.setRangeBack( IL[0].currentTime() );
  AQ->readSignal( IL, ED );
  FD->filter( IL, ED );
  if ( AQ->writeData() < 0 )
    printlog( "! error in writing data" );
  unlockData();
  DataSleepWait.wakeAll();
}


void RELACSWidget::processData( void )
{
  readLockData();
  for ( int k=0; k<AQ->inputsSize(); k++ ) {
    // XXX FW->write( AQ->aiData( k ) );
  }
  FW->write( IL, ED );
  IL.clearDeviceBuffer();
  unlockData();
  PT->plot( IL, ED );
}


void RELACSWidget::startDataThread( void )
{
  RunDataMutex.lock();
  RunData = true;
  RunDataMutex.unlock();
  QThread::start();
}


void RELACSWidget::run( void )
{
  bool rd = true;
  SS.lock();
  double updateinterval = SS.number( "updateinterval", 0.05 );
  double processinterval = SS.number( "processinterval", 0.1 );
  SS.unlock();
  signed long ui = (unsigned long)::rint( 1000.0*updateinterval );
  ui -= 1;
  int pmax = (int)::rint( processinterval/updateinterval );
  if ( pmax < 1 )
    pmax = 1;
  int pc = 0;
  QTime updatetime;
  updatetime.start();

  do {
    int ei = updatetime.restart();
    QThread::msleep( ui > ei ? ui - ei : 1 );
    updateData();
    QThread::msleep( 1 );
    pc++;
    if ( pc >= pmax ) {
      processData();
      pc = 0;
    }
    RunDataMutex.lock();
    rd = RunData;
    RunDataMutex.unlock();
  } while( rd );
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
  AQ->activateGains();
  FD->adjust( IL, ED, AQ->adjustFlag() );
  unlockData();
}


int RELACSWidget::write( OutData &signal )
{
  int r = AQ->write( signal );
  if ( r == 0 ) {
    FW->write( signal );
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
  int r = AQ->write( signal );
  if ( r == 0 ) {
    FW->write( signal );
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
  FW->write( false );
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


void RELACSWidget::notifyMetaData( void )
{
  if ( MD != 0 )
    MD->notifyMetaData();
  FD->notifyMetaData();
  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->notifyMetaData();
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
  readLockData();

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

  FW->write( saving );
  FW->write( *CurrentRePro );
  CurrentRePro->setSaving( FW->saving() );
  unlockData();
  CurrentRePro->start();
}


void RELACSWidget::stopRePro( void )
{
  if ( ! ReProRunning )
    return;

  // wait on RePro to stop:
  if ( CurrentRePro->running() ) {
    CurrentRePro->requestStop();

    // process all posted events (that usually paint the RePro...) really needed?
    //  QApplication::sendPostedEvents();
    
    // waiting for the RePro to properly terminate:
    CurrentRePro->wait();
  }

  // stop analog output:
  AQ->stopWrite();                

  ReProRunning = false;

  updateData();

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
  FW->openFiles( IL, ED );
  SN->setSaveDialog( FW->openState() );

#if QT_VERSION >= 300
  MainWidget->setEraseColor( QColor( 255, 96, 96 ) );
#else
  MainWidget->setBackgroundColor( QColor( 255, 96, 96 ) );
#endif

  SS.lock();
  if ( SS.boolean( "saverelacscore" ) )
    CFG.save( RELACSPlugin::Core, FW->addPath( "relacs.cfg" ) );
  if ( SS.boolean( "saverelacsplugins" ) )
    CFG.save( RELACSPlugin::Plugins, FW->addPath( "relacsplugins.cfg" ) );
  if ( SS.boolean( "saveattenuators" ) )
    ATI->save( FW->path() );
  if ( SS.boolean( "saverelacslog" ) ) {
    LogFile = new ofstream( FW->addPath( "relacs.log" ).c_str() );
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

  InfoFile = new ofstream( FW->addPath( "repros.dat" ).c_str() );
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
    FW->completeFiles();
  else 
    FW->deleteFiles();

  if ( MD != 0 )
    MD->sessionStopped( saved );
  FD->sessionStopped( saved );
  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->sessionStopped( saved );
  RP->sessionStopped( saved );

  CurrentRePro->setSaving( FW->saving() );

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

  FW->setPath( FW->defaultPath() );
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

  // stop simulation and data acquisition:
  SimLoad.stop();
  if ( AQ != 0 )
    AQ->stop();

  // stop data thread:
  RunDataMutex.lock();
  RunData = false;
  RunDataMutex.unlock();
  QThread::wait();
  qApp->processEvents();  // process pending events posted from threads.
}


void RELACSWidget::stopActivity( void )
{
  printlog( "Stopping " + ModeStr[ mode() ] + "-mode" );

  // finish session and repro:
  SN->stopTheSession();
  // shutdown macros:
  if ( MC->shutDownIndex() >= 0 )
    MC->shutDown();

  stopThreads();

  closeHardware();

  IL.freeDeviceBuffer();

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

int RELACSWidget::mode( void ) const
{
  return Mode;
}


string RELACSWidget::modeStr( void ) const
{
  string ms = "";
  if ( acquisition() )
    ms = "acquisition";
  else if ( simulation() )
    ms = "simulation";
  else if ( analysis() )
    ms = "analysis";
  else if ( idle() )
    ms = "idle";
  return ms;
}


bool RELACSWidget::acquisition( void ) const
{
  return ( Mode == AcquisitionMode );
}


bool RELACSWidget::simulation( void ) const
{
  return ( Mode == SimulationMode );
}


bool RELACSWidget::analysis( void ) const
{
  return ( Mode == AnalysisMode );
}


bool RELACSWidget::idle( void ) const
{
  return ( Mode == IdleMode );
}


void RELACSWidget::setMode( int mode )
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
  setupInTraces();
  setupOutTraces();

  // events:
  FD->clearIndices();
  FD->createStimulusEvents( IL, ED, EventStyles );
  FD->createRestartEvents( IL, ED, EventStyles );
  Str fdw = FD->createTracesEvents( IL, ED, TraceStyles, EventStyles );
  if ( !fdw.empty() ) {
    printlog( "! error: " + fdw.erasedMarkup() );
    MessageBox::error( "RELACS Error !", fdw, this );
  }

  // files:
  FW->setPath( FW->defaultPath() );

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
  int r = AQ->read( IL );
  if ( r < 0 )
    printlog( "! error in starting data acquisition: " + IL.errorText() );
  AID->updateMenu();

  FD->init( IL, ED );  // init filters/detectors before RePro!

  startDataThread();

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
  setupInTraces();
  setupOutTraces();

  // events:
  FD->clearIndices();
  FD->createStimulusEvents( IL, ED, EventStyles );
  FD->createRestartEvents( IL, ED, EventStyles );
  Str fdw = FD->createTracesEvents( IL, ED, TraceStyles, EventStyles );
  if ( !fdw.empty() ) {
    printlog( "! error: " + fdw.erasedMarkup() );
    MessageBox::error( "RELACS Error !", fdw, this );
  }

  // files:
  FW->setPath( FW->defaultPath() );

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
  int r = AQ->read( IL );
  if ( r < 0 )
    printlog( "! error in starting data acquisition: " + IL.errorText() );
  AID->updateMenu();

  // give it a second chance:
  for ( int k=0; k<IL.size(); k++ ) {
    if ( IL[k].failed() ) {
      r = AQ->read( IL );
      if ( r < 0 )
	printlog( "! error in starting data acquisition: " + IL.errorText() );
    }
  }

  // check success:
  for ( int k=0; k<IL.size(); k++ ) {
    if ( IL[k].failed() ) {
      printlog( "error in starting simulation of trace " + IL[k].ident() + ": "
		+ IL[k].errorText() );
      stopActivity();
      return;
    }
  }

  FD->init( IL, ED );  // init filters/detectors before RePro!

  startDataThread();

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
  e->ignore();
}


void RELACSWidget::keyReleaseEvent( QKeyEvent* e)
{
  if ( CurrentRePro != 0 )
    CurrentRePro->keyReleaseEvent( e );
  PT->keyReleaseEvent( e );
  MC->keyReleaseEvent( e );
  for ( unsigned int k=0; k<CN.size(); k++ )
    CN[k]->keyReleaseEvent( e );
  e->ignore();
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
  info += "<p align=center><b>R</b>ealtime <b>El</b>ectrophysiological Data <b>A</b>cquisition, <b>C</b>ontrol, and <b>S</b>timulation</p>\n";
  info += "<p align=center>Version ";
  info += RELACSVERSION;
  info += "</p>\n";
  info += "<p align=center>(c) by Jan Benda</p>\n";
  info += "<p align=center>Institute for Theoretical Biology,<br>\n";
  info += "   Humboldt-University Berlin.</p>\n";
  /*
  info += "<p>Using the following Research Programs:</p>\n<ul>\n";			
  for (int k=0; k<RP->size(); k++ )
    {
      info += "<li>";
      info += "<b>" + RP->repro( k )->name() + "</b>";
      info += ", Version ";
      info += RP->repro( k )->version();
      info += ", by ";
      info += RP->repro( k )->author();
      info += "</li>\n";
    }
  info += "</ul>\n";
  */
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
  hb->mimeSourceFactory()->setFilePath( "doc/html" );
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
  : TLW( tlw )
{
}


KeyTimeOut::~KeyTimeOut( void )
{
}


bool KeyTimeOut::eventFilter( QObject *o, QEvent *e )
{
  if ( qApp->focusWidget() != TLW &&
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
  if ( qApp->focusWidget() != TLW ) {
    TLW->setFocus();
  }
  killTimers();
}

