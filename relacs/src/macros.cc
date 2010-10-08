/*
  macros.cc
  A single Macro

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

#include <cstdio>
#include <QDateTime>
#include <QApplication>
#include <QPainter>
#include <QBitmap>
#include <QTextBrowser>
#include <QFileDialog>
#include <relacs/str.h>
#include <relacs/strqueue.h>
#include <relacs/optdialog.h>
#include <relacs/messagebox.h>
#include <relacs/rangeloop.h>
#include <relacs/relacswidget.h>
#include <relacs/repro.h>
#include <relacs/repros.h>
#include <relacs/filter.h>
#include <relacs/filterdetectors.h>
#include <relacs/macros.h>

namespace relacs {


const string Macro::StartUpIdent = "startup";
const string Macro::ShutDownIdent = "shutdown";
const string Macro::FallBackIdent = "fallback";
const string Macro::StartSessionIdent = "startsession";
const string Macro::StopSessionIdent = "stopsession";
const string Macro::NoButtonIdent = "nobutton";
const string Macro::NoKeyIdent = "nokey";
const string Macro::NoMenuIdent = "nomenu";
const string Macro::KeepIdent = "keep";
const string Macro::OverwriteIdent = "overwrite";


Macros::Macros( RELACSWidget *rw, QWidget *parent )
  : QWidget( parent ),
    ConfigClass( "Macros", RELACSPlugin::Core ),
    RW( rw ), 
    RPs( 0 ), 
    MCs(),
    CurrentMacro( -1 ), 
    CurrentCommand( 0 ),
    Stack(), 
    ResumePos(), 
    ResumeMacroOnly( false ),
    ThisCommandOnly( false ), 
    ThisMacroOnly( false ), 
    Enable( false ),
    Warnings( "" ), 
    StartUpIndex( 0 ), 
    ShutDownIndex( -1 ), 
    FallBackIndex( 0 ), 
    StartSessionIndex( 1 ), 
    StopSessionIndex( -1 ),
    MacroFile( "" ),
    Menu( 0 ), 
    SwitchMenu( 0 ), 
    ButtonLayout( 0 ),
    Fatal( false )
{
  ButtonLayout = new QGridLayout( this );
  ButtonLayout->setContentsMargins( 0, 0, 0, 0 );
  ButtonLayout->setSpacing( 0 );
  setLayout( ButtonLayout );

  addText( "file", "Configuration file", "macros.cfg" );
  addText( "mainfile", "Main configuration file", "" );
  addBoolean( "fallbackonreload", "Start fallback macro when loading macros", true );

  createIcons();
}


Macros::~Macros( void )
{
  clear( false );
}


void Macros::createIcons( void )
{
  int my = fontInfo().pixelSize() - 2;
  int mx = my;

  SessionIcon = QPixmap( mx+2, my+2 );
  QPainter p;
  p.begin( &SessionIcon );
  p.eraseRect( SessionIcon.rect() );
  p.setPen( QPen( Qt::black, 1 ) );
  p.setBrush( Qt::black );
  QPolygon pa( 3 );
  pa.setPoint( 0, mx/3, 0 );
  pa.setPoint( 1, mx/3, my );
  pa.setPoint( 2, mx, my/2 );
  p.drawPolygon( pa );
  p.end();
  SessionIcon.setMask( SessionIcon.createHeuristicMask() );

  BaseIcon = QPixmap( mx+2, my+2 );
  p.begin( &BaseIcon );
  p.eraseRect( BaseIcon.rect() );
  p.setPen( QPen( Qt::black, 1 ) );
  p.setBrush( Qt::red );
  p.drawEllipse( mx/4, (my-mx*3/4)/2, mx*3/4, mx*3/4 );
  p.end();
  BaseIcon.setMask( BaseIcon.createHeuristicMask() );

  StackIcon = QPixmap( mx+2, my+2 );
  p.begin( &StackIcon );
  p.eraseRect( StackIcon.rect() );
  p.setPen( QPen( Qt::black, 1 ) );
  p.setBrush( Qt::yellow );
  p.drawEllipse( mx/4, (my-mx*3/4)/2, mx*3/4, mx*3/4 );
  p.end();
  StackIcon.setMask( StackIcon.createHeuristicMask() );

  RunningIcon = QPixmap( mx+2, my+2 );
  p.begin( &RunningIcon );
  p.eraseRect( RunningIcon.rect() );
  p.setPen( QPen( Qt::black, 1 ) );
  p.setBrush( Qt::green );
  p.drawEllipse( mx/4, (my-mx*3/4)/2, mx*3/4, mx*3/4 );
  p.end();
  RunningIcon.setMask( RunningIcon.createHeuristicMask() );

  IdleIcon = QPixmap( mx+2, my+2 );
  p.begin( &IdleIcon );
  p.eraseRect( IdleIcon.rect() );
  p.end();
  IdleIcon.setMask( IdleIcon.createHeuristicMask() );

  EnabledIcon = QPixmap( mx, mx );
  p.begin( &EnabledIcon );
  p.eraseRect( EnabledIcon.rect() );
  p.setPen( QPen( Qt::black, 1 ) );
  p.setBrush( Qt::green );
  p.drawEllipse( 0, 0, mx-1, mx-1 );
  p.end();
  EnabledIcon.setMask( EnabledIcon.createHeuristicMask() );

  DisabledIcon = QPixmap( mx, mx );
  p.begin( &DisabledIcon );
  p.eraseRect( DisabledIcon.rect() );
  p.setPen( QPen( Qt::black, 1 ) );
  p.setBrush( Qt::red );
  p.drawEllipse( 0, 0, mx-1, mx-1 );
  p.end();
  DisabledIcon.setMask( DisabledIcon.createHeuristicMask() );
}


void Macros::clear( bool keep )
{
  // clear buttons:
  for ( unsigned int k=0; k<MCs.size(); k++ ) {
    if ( MCs[k]->button() != 0 ) {
      MCs[k]->button()->hide();
      ButtonLayout->removeWidget( MCs[k]->button() );
    }
  }

  // clear menu:
  if ( Menu != 0 )
    Menu->clear();
  SwitchMenu = 0;
  SwitchActions.clear();

  // clear macros:
  MacrosType::iterator mp = MCs.begin();
  while ( mp != MCs.end() ) {
    if ( ! ( (*mp)->keep() && keep ) ) {
      delete (*mp);
      mp = MCs.erase( mp );
    }
    else 
      ++mp;
  }
}


void Macros::load( const string &file, bool main )
{
  clear();

  string macrofile = file;
  if ( main ) {
    macrofile = text( "mainfile" );
    if ( macrofile.empty() ||
	 macrofile == "none" )
      return;
  }
  if ( macrofile.empty() )
    macrofile = text( "file" );

  // read in file:
  ifstream macroFile( macrofile.c_str() );
  if ( !macroFile ) {
    Warnings += "Could not read file \"<b>";
    Warnings += macrofile + "</b>\".\n";
    return;
  }

  MacroFile = macrofile;
  int linenum = 0;
  string line = "";
  bool appendable = false;
  bool appendmacro = false;
  bool appendparam = false;
  while ( getline( macroFile, line ) ) {
    linenum++;
    Str ls = line;
    ls.stripComment( "#" );
    int mpos = ls.findFirstNot( Str::WhiteSpace );
    // line empty:
    if ( mpos < 0 ) {
      appendable = false;
      continue;
    }
    // macro:
    if ( ls[mpos] == '$' ) {
      ls.erase( 0, mpos + 1 );
      ls.strip();
      if ( !ls.empty() ) {
	MCs.push_back( new Macro( ls, this ) );
	appendable = true;
	appendmacro = true;
	appendparam = true;
      }
      else {
	Warnings += "Macro name expected in line <b>" + Str( linenum )
	  + "</b>: \"<b>" + line + "</b>\".\n";
      }
    }
    // repro, macro, filter, detector, shell, message, browse command:
    else {
      bool enabled = true;
      if ( ls[mpos] == '!' )
	enabled = false;
      int pos = ls.findFirst( ':' );
      Str name = ls.mid( mpos + ( enabled ? 0 : 1 ), pos-1 );
      bool repro = name.eraseFirst( "repro", 0, false, 3, Str::WhiteSpace );
      bool macro = name.eraseFirst( "macro", 0, false, 3, Str::WhiteSpace );
      bool filter = name.eraseFirst( "filter", 0, false, 3, Str::WhiteSpace );
      bool detector = name.eraseFirst( "detector", 0, false, 3, Str::WhiteSpace );
      bool switchm = name.eraseFirst( "switch", 0, false, 3, Str::WhiteSpace );
      bool startsession = name.eraseFirst( "startsession", 0, false, 3, Str::WhiteSpace );
      bool shell = name.eraseFirst( "shell", 0, false, 3, Str::WhiteSpace );
      bool message = name.eraseFirst( "message", 0, false, 3, Str::WhiteSpace );
      bool browse = name.eraseFirst( "browse", 0, false, 3, Str::WhiteSpace );
      double timeout = 0.0;
      if ( message ) {
	// XXX for that we need nice functions in Str!
	int n=0;
	sscanf( name.c_str(), " %lf%n", &timeout, &n );
	if ( n > 0 )
	  name.erase( 0, n );
      }
      name.strip( Str::WhiteSpace );
      Str params = "";
      if ( pos >= 0 ) {
	params = ls.substr( pos+1 );
	params.strip();
      }
      if ( appendable && 
	   !( repro || macro || filter || detector || switchm ||
	      startsession || shell || message || browse ) &&
	   ( pos < 0 && ( ( appendparam && name.find( '=' ) >= 0 ) ||
			  ( !appendparam && mpos > 0 ) ) ) ) {
	if ( appendmacro )
	  MCs.back()->addParams( name );
	else {
	  if ( appendparam )
	    MCs.back()->Commands.back()->Params.provideLast( ';' );
	  MCs.back()->Commands.back()->Params.provideLast( ' ' );
	  MCs.back()->Commands.back()->Params += name;
	}
      }
      else if ( !startsession && !shell && !filter && !detector &&
		!message && !browse &&
		name.empty() ) {
	Warnings += "Missing name of action in line <b>"
	  + Str( linenum ) + "</b>: \"<b>" + line + "</b>\".\n";
	appendable = false;
      }
      else if ( startsession || !name.empty() || !params.empty() ) {
	MacroCommand *mc = new MacroCommand( name, params, 
					     enabled, macro ? 0 : -1, 
					     filter, detector, switchm, startsession,
					     shell, message, timeout, 
					     browse, this );
	MCs.back()->Commands.push_back( mc );
	if ( startsession || browse || switchm )
	  appendable = false;
	else if ( shell || message ) {
	  appendable = true;
	  appendmacro = false;
	  appendparam = false;
	}
	else {
	  appendable = true;
	  appendmacro = false;
	  appendparam = true;
	}
      }
      else {
	Warnings += "Incomplete or empty specification of action in line <b>"
	  + Str( linenum ) + "</b>: \"<b>" + line + "</b>\".\n";
	appendable = false;
      }
    }
  }
  macroFile.close();
}


bool Macros::check( void )
{
  Fatal = false;
  const int maxpasses = 2;

  for ( int pass = 0; pass < maxpasses; pass++ ) {
 
    // check all macros:
    for ( MacrosType::iterator mp = MCs.begin();
	  mp != MCs.end(); ) {

      // check the commands:
      for ( vector<MacroCommand*>::iterator cp = (*mp)->Commands.begin(); 
	    cp != (*mp)->Commands.end(); ) {
	
	if ( (*cp)->Macro >= 0 ) {
	  if ( index( (*cp)->Name ) < 0 ) {
	    Warnings += "Removed unknown Macro \"<b>";
	    Warnings += (*cp)->Name;
	    Warnings += "</b>\" in Macro \"<b>";
	    Warnings += (*mp)->Name;
	    Warnings += "</b>\".\n";
	    cp = (*mp)->Commands.erase( cp );
	  }
	  else
	    ++cp;
	}

	else if ( (*cp)->Filter > 0 ) {
	  if ( ! (*cp)->Name.empty() &&
	       ! RW->FD->exist( (*cp)->Name ) ) {
	    Warnings += "Removed unknown Filter \"<b>";
	    Warnings += (*cp)->Name;
	    Warnings += "</b>\" in Macro \"<b>";
	    Warnings += (*mp)->Name;
	    Warnings += "</b>\".\n";
	    cp = (*mp)->Commands.erase( cp );
	  }
	  else
	    ++cp;
	}

	else if ( (*cp)->Detector > 0 ) {
	  if ( ! (*cp)->Name.empty() &&
	       ! RW->FD->exist( (*cp)->Name ) ) {
	    Warnings += "Removed unknown Detector \"<b>";
	    Warnings += (*cp)->Name;
	    Warnings += "</b>\" in Macro \"<b>";
	    Warnings += (*mp)->Name;
	    Warnings += "</b>\".\n";
	    cp = (*mp)->Commands.erase( cp );
	  }
	  else
	    ++cp;
	}

	else if ( (*cp)->Switch ) {
	  ifstream f( (*cp)->Name.c_str() );
	  if ( ! f.good() ) {
	    Warnings += "Removed switch to unknown file \"<b>";
	    Warnings += (*cp)->Name;
	    Warnings += "</b>\" in Macro \"<b>";
	    Warnings += (*mp)->Name;
	    Warnings += "</b>\".\n";
	    cp = (*mp)->Commands.erase( cp );
	  }
	  else
	    ++cp;
	}

	else if ( (*cp)->StartSession ) {
	  ++cp;
	}

	else if ( (*cp)->Shell ) {
	  ++cp;
	}
	
	else if ( (*cp)->Message ) {
	  if ( (*cp)->Params.empty() ) {
	    (*cp)->Params = (*cp)->Name;
	    (*cp)->Name = "RELACS Message";
	  }
	  if ( (*cp)->Name.empty() ) {
	    (*cp)->Name = "RELACS Message";
	  }
	  ++cp;
	}
	
	else if ( (*cp)->Browse ) {
	  if ( (*cp)->Params.empty() ) {
	    (*cp)->Params = (*cp)->Name;
	    (*cp)->Name = "RELACS Info";
	  }
	  if ( (*cp)->Name.empty() ) {
	    (*cp)->Name = "RELACS Info";
	  }
	  ++cp;
	}
	
	else {
	  // find RePro:
	  RePro *repro = RPs->nameRepro( (*cp)->Name );
	  if ( repro == 0 ) {
	    Warnings += "Removed unknown RePro \"<b>";
	    Warnings += (*cp)->Name;
	    Warnings += "</b>\" in Macro \"<b>";
	    Warnings += (*mp)->Name;
	    Warnings += "</b>\".\n";
	    cp = (*mp)->Commands.erase( cp );
	  }
	  else {
	    (*cp)->RP = repro;
	    if ( pass == 0 ) {
	      // expand ranges:
	      Str ps = (*cp)->Params;
	      vector <RangeLoop> rls;
	      vector <int> lb;
	      vector <int> rb;
	      // find ranges:
	      int o = ps.find( "(" );
	      if ( o > 0 && ps[o-1] == 'd' )
		o = -1;
	      while ( o >= 0 ) {
		int c = ps.findBracket( o, "(", "" );
		if ( c > 0 ) {
		  lb.push_back( o );
		  rb.push_back( c );
		  rls.push_back( RangeLoop( ps.mid( o+1, c-1 ) ) );
		  o = ps.find( "(", c+1 );
		}
		else
		  o = -1;
	      }
	      if ( rls.empty() )
		++cp;
	      else  {
		// erase original repro:
		MacroCommand omc( *(*cp) );
		cp = (*mp)->Commands.erase( cp );
		// loop ranges:
		for ( unsigned int k=0; k<rls.size(); k++ )
		  rls[k].reset();
		while ( !rls[0] ) {
		  // create parameter:
		  string np = ps;
		  for ( int j=int(rls.size())-1; j>=0; j-- ) {
		    np.replace( lb[j], rb[j] - lb[j] + 1, Str( *rls[j] ) );
		  }
		  // add repro:
		  omc.Params = np;
		  if ( cp != (*mp)->Commands.end() ) {
		    cp = (*mp)->Commands.insert( cp, new MacroCommand( omc ) );
		    ++cp;
		  }
		  else {
		    (*mp)->Commands.push_back( new MacroCommand( omc ) );
		    cp = (*mp)->Commands.end();
		  }
		    
		  // increment range looops:
		  for ( int k = int(rls.size())-1; k >= 0; k-- ) {
		    ++rls[k];
		    if ( !rls[k] )
		      break;
		    else if ( k > 0 )
		      rls[k].reset();
		  }
		}
	      }
	    }
	    else if ( pass == 1 ) {
	      // check options:
	      Options prjopt;
	      string error = repro->checkOptions( (*mp)->expandParams( (*cp)->Params, prjopt ) );
	      if ( error.size() > 0 ) {
		Warnings += "Invalid options for RePro \"<b>";
		Warnings += repro->name();
		Warnings += "</b>\" from Macro \"<b>";
		Warnings += (*mp)->Name;
		Warnings += "</b>\":<br>";
		Warnings += error;
		Warnings += ".\n";
	      }
	      ++cp;
	    }
	    else
	      ++cp;
	  }
	}
      }  // commands
      
      if ( (*mp)->Commands.size() == 0 ) {
	Warnings += "Removed empty Macro \"<b>" + (*mp)->Name + "</b>\".\n";
	mp = MCs.erase( mp );
      }
      else {
	// check doublets of overwrite macro:
	if ( (*mp)->Overwrite ) {
	  MacrosType::iterator mpp = MCs.begin();
	  while ( mpp != mp && mpp != MCs.end() ) {
	    if ( (*mpp)->Name == (*mp)->Name ) {
	      int mk = mp - MCs.begin();
	      mpp = MCs.erase( mpp );
	      mp = MCs.begin() + mk - 1;
	    }
	    ++mpp;
	  }
	}

	// check doublets of keep macros:
	MacrosType::iterator mpp = MCs.begin();
	while ( mpp != mp && mpp != MCs.end() && (*mpp)->Keep ) {
	  if ( (*mpp)->Name == (*mp)->Name &&
	       (*mp)->Keep ) {
	    mp = MCs.erase( mp );
	    --mp;
	    break;
	  }
	  ++mpp;
	}
	  
	++mp;
      }
      
    }  // macros
    
  } // passes

  // set macros indices:
  for ( MacrosType::iterator mp = MCs.begin();
	mp != MCs.end(); ++mp )
    for ( vector<MacroCommand*>::iterator cp = (*mp)->Commands.begin(); 
	  cp != (*mp)->Commands.end(); ++cp )
      if ( (*cp)->Macro >= 0 )
	(*cp)->Macro = index( (*cp)->Name );
  
  // set RePros back to default values:
  for ( int k=0; k<RPs->size(); k++ )
    RPs->repro( k )->setDefaults();

  // no macros?
  if ( MCs.empty() ) {

    if ( !Warnings.empty() )
      Warnings += "\n";
    Warnings += "No Macros specified! Trying to create Macros from RePros...\n";

    for ( int k=0; k<RPs->size(); k++ ) {
      MCs.push_back( new Macro( RPs->repro( k )->name(), this ) );
      MCs.back()->Commands.push_back( new MacroCommand( RPs->repro( k ),
							"", this ) );
    }

  }

  // no macros?
  if ( MCs.empty() ) {
    if ( !Warnings.empty() )
      Warnings += "\n";
    Warnings += "No Macros!\n";
    Fatal = true;
  }

  // what about the default startup, etc. settings?
  StartUpIndex = -1;
  ShutDownIndex = -1;
  FallBackIndex = -1;
  StartSessionIndex = -1;
  StopSessionIndex = -1;
  for ( unsigned int k=0; k<MCs.size(); k++ ) {
    if ( MCs[k]->startUp() ) {
      if ( StartUpIndex >= 0 && StartUpIndex < (int)k )
	MCs[StartUpIndex]->noStartUp();
      StartUpIndex = k;
    }
    if ( MCs[k]->shutDown() ) {
      if ( ShutDownIndex >= 0 && ShutDownIndex < (int)k )
	MCs[ShutDownIndex]->noShutDown();
      ShutDownIndex = k;
    }
    if ( MCs[k]->fallBack() ) {
      if ( FallBackIndex >= 0 && FallBackIndex < (int)k )
	MCs[FallBackIndex]->noFallBack();
      FallBackIndex = k;
    }
    if ( MCs[k]->startSession() ) {
      if ( StartSessionIndex >= 0 && StartSessionIndex < (int)k )
	MCs[StartSessionIndex]->noStartSession();
      StartSessionIndex = k;
    }
    if ( MCs[k]->stopSession() ) {
      if ( StopSessionIndex >= 0 && StopSessionIndex < (int)k )
	MCs[StopSessionIndex]->noStopSession();
      StopSessionIndex = k;
    }
  }

  // no fallback macro?
  for ( unsigned int k=0; k<MCs.size() && FallBackIndex < 0; k++ ) {
    vector<MacroCommand*>::iterator cp;
    for ( cp = MCs[k]->Commands.begin(); 
	  cp != MCs[k]->Commands.end(); 
	  ++cp ) {
      if ( (*cp)->RP != 0 ) {
	MCs[k]->setFallBack();
	FallBackIndex = k;
	break;
      } 
    }
  }

  // no RePro within fallback macro?
  if ( FallBackIndex >= 0 && (int)MCs.size() > FallBackIndex ) {
    RePro *lr = 0;
    vector<MacroCommand*>::iterator cp;
    for ( cp = MCs[FallBackIndex]->Commands.begin(); 
	  cp != MCs[FallBackIndex]->Commands.end(); 
	  ++cp ) {
      if ( (*cp)->RP != 0 )
	lr = (*cp)->RP;
    }
    if ( lr == 0 ) {
      if ( !Warnings.empty() )
        Warnings += "\n";
      Warnings += "No RePro found in FallBack Macro!\n";
      MCs[FallBackIndex]->noFallBack();
      FallBackIndex = -1;
    }
  }

  // no fallback macro?
  if ( FallBackIndex < 0 ) {
    if ( !Warnings.empty() )
      Warnings += "\n";
    Warnings += "No FallBack RePro found!\n";
    Fatal = true;
  }

  // set macro and command indices:
  for ( unsigned int m=0; m<MCs.size(); m++ ) {
    MCs[m]->MacroNum = m;
    for ( unsigned int c=0; c<MCs[m]->Commands.size(); c++ ) {
      MCs[m]->Commands[c]->MacroNum = m;
      MCs[m]->Commands[c]->CommandNum = c;
    }
  }

  return Fatal;
}


void Macros::warning( void )
{
  if ( !Warnings.empty() ) {
    string s = Warnings;
    s.insert( 0, "<li>" );
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
    MessageBox::warning( "RELACS: Macros", "<ul>" + s + "</ul>", 0.0 );

    Warnings.eraseMarkup();
    RW->printlog( "! warning in Macros: " + Warnings );
  }
  Warnings = "";
}


void Macros::create( void )
{
  // base menu:
  Menu->addAction( "&Reload", this, SLOT( reload( void ) ) );
  Menu->addAction( "&Load...", this, SLOT( selectMacros( void ) ) );
  if ( Options::size( "file" ) > 1 ) {
    SwitchMenu = Menu->addMenu( "&Switch" );
    for ( int k=0; k<Options::size( "file" ); k++ )
      SwitchActions.push_back( SwitchMenu->addAction( text( "file", k ).c_str() ) );
    connect( SwitchMenu, SIGNAL( triggered( QAction* ) ), 
	     this, SLOT( switchMacro( QAction* ) ) );
  }
  Menu->addAction( "&Skip RePro", this, SLOT( startNextRePro( void ) ),
		   QKeySequence( Qt::Key_S ) );
  Menu->addAction( "&Break", this, SLOT( softBreak( void ) ),
		   QKeySequence( Qt::Key_B ) );
  ResumeAction = Menu->addAction( "Resume", this, SLOT( resume( void ) ),
				  QKeySequence( Qt::Key_R ) );
  ResumeNextAction = Menu->addAction( "Resume Next", this,
				      SLOT( resumeNext( void ) ),
				      QKeySequence( Qt::Key_N ) );
  ResumeAction->setEnabled( false );
  ResumeNextAction->setEnabled( false );
  Menu->addSeparator();

  // count macro buttons:
  int nb = 0;
  for ( unsigned int k=0; k<MCs.size(); k++ )
    if ( MCs[k]->Button )
      nb++;
  
  // number of buttons in a row:
  int cols = nb;
  const int maxcols = 6;
  if ( nb > maxcols ) {
    int r = (nb-1)/maxcols + 1;
    cols = (nb-1)/r + 1;
  }
  
  // create buttons and menus:
  int fkc = 0;
  int mk = 0;
  int row=0;
  int col=0;
  for ( unsigned int k=0; k<MCs.size(); k++ ) {
    
    string mt = "";
    if ( MCs[k]->Menu ) {
      mt += "&";
      if ( mk == 0 )
	mt += '0';
      else if ( mk < 10 )
	mt += ( '1' + mk - 1 );
      else
	mt += ( 'a' + mk - 10 );
      mt += " ";
      mk++;
    }
    mt += MCs[k]->Name;
    MCs[k]->MAction = new QAction( mt.c_str(), this );

    // key code:
    Str keys = "";
    if ( MCs[k]->Key ) {
      if ( MCs[k]->fallBack() ) {
	MCs[k]->KeyCode = Qt::Key_Escape;
	keys = " (ESC)";
      }
      else if ( fkc < 12 ) {
	MCs[k]->KeyCode = Qt::Key_F1 + fkc;
	fkc++;
	keys = Str( fkc, " (F%d)" );
      }
      else
	MCs[k]->KeyCode = 0;
    }
    else
      MCs[k]->KeyCode = 0;

    // menu:
    MCs[k]->addMenu( Menu );
      
    // button:
    MCs[k]->addButton( keys );
    if ( MCs[k]->Button ) {
      ButtonLayout->addWidget( MCs[k]->PushButton, row, col );
      col++;
      if ( col >= cols ) {
	col=0;
	row++;
      }
    }

  }

  setFixedHeight( minimumSizeHint().height() );
  update();
}


void Macros::setMenu( QMenu *menu )
{
  Menu = menu;
}


void Macros::startMacro( int macro, int command, bool saving, 
			 vector<MacroPos> *newstack )
{
  clearStackButtons();
  clearButton();

  if ( macro >= 0 && macro < (int)MCs.size() )
    CurrentMacro = macro;
  else
    CurrentMacro = FallBackIndex;

  CurrentCommand = command - 1;

  if ( newstack != 0 ) {
    Stack = *newstack;
    stackButtons();
  }

  ThisCommandOnly = false;
  ThisMacroOnly = false;

  runButton();

  RW->startedMacro( MCs[CurrentMacro]->Name, variablesStr( CurrentMacro ) );

  startNextRePro( saving );
}


void Macros::startNextRePro( bool saving )
{
  if ( RW->idle() )
    return;

  emit stopRePro();

  do {
    CurrentCommand++;

    do {
      if ( ThisCommandOnly || CurrentMacro < 0 ) {
	clearStackButtons();
	CurrentMacro = FallBackIndex;
	CurrentCommand = 0;
	ThisCommandOnly = false;
	runButton();
	RW->startedMacro( MCs[CurrentMacro]->Name, variablesStr( CurrentMacro ) );
      }
      else if ( CurrentCommand >= (int)MCs[CurrentMacro]->Commands.size() ) {
	if ( ThisMacroOnly && Stack.size() == 1 ) {
	  ThisMacroOnly = false;
	  clearStackButtons();
	  if ( CurrentMacro == ShutDownIndex )
	    return;
	  CurrentMacro = FallBackIndex;
	  CurrentCommand = 0;
	  runButton();
	  RW->startedMacro( MCs[CurrentMacro]->Name, variablesStr( CurrentMacro ) );
	}
	else if ( Stack.size() > 0 ) {
	  clearButton();
	  MacroPos mc( Stack.back() );
	  CurrentMacro = mc.MacroID;
	  CurrentCommand = mc.CommandID;
	  MCs[CurrentMacro]->Variables = mc.MacroVariables;
	  MCs[CurrentMacro]->Project = mc.MacroProject;
	  Stack.pop_back();
	  runButton();
	}
	else {
	  clearButton();
	  if ( CurrentMacro == ShutDownIndex )
	    return;
	  CurrentMacro = FallBackIndex;
	  CurrentCommand = 0;
	  runButton();
	  RW->startedMacro( MCs[CurrentMacro]->Name, variablesStr( CurrentMacro ) );
	}
      }
    } while ( CurrentCommand < 0 || 
	      CurrentCommand >= (int)MCs[CurrentMacro]->Commands.size() );
    
    if ( MCs[CurrentMacro]->Commands[CurrentCommand]->Enabled || Enable ) {
      if ( Enable && MCs[CurrentMacro]->Commands[CurrentCommand]->RP == 0 )
	Enable = false;
      // execute macro:
      if ( MCs[CurrentMacro]->Commands[CurrentCommand]->Macro >= 0 ) {
	int newmacro = MCs[CurrentMacro]->Commands[CurrentCommand]->Macro;
	// put next command on the stack:
	if ( MCs[newmacro]->Button )
	  stackButton();
	Stack.push_back( MacroPos( CurrentMacro, CurrentCommand + 1, 
				   MCs[CurrentMacro]->Variables,
				   MCs[CurrentMacro]->Project ) );
	// execute the requested macro:
	Options prjopts;
	MCs[newmacro]->Variables.setDefaults();
	MCs[newmacro]->Variables.read( MCs[CurrentMacro]->expandParams( MCs[CurrentMacro]->Commands[CurrentCommand]->Params, prjopts ) );
	MCs[newmacro]->Project.read( prjopts );
	CurrentMacro = newmacro;
	CurrentCommand = -1;
	runButton();
      }
      // execute shell command:
      else if ( MCs[CurrentMacro]->Commands[CurrentCommand]->Shell ) {
	string com = "nice ";
	com += MCs[CurrentMacro]->Commands[CurrentCommand]->Name;
	com += " ";
	com += MCs[CurrentMacro]->Commands[CurrentCommand]->Params;
	RW->printlog( "execute \"" + com + "\"" );
	system( com.c_str() );
      }
      // filter:
      else if ( MCs[CurrentMacro]->Commands[CurrentCommand]->Filter > 0 ) {
	if ( MCs[CurrentMacro]->Commands[CurrentCommand]->Filter == 2 &&
	     MCs[CurrentMacro]->Commands[CurrentCommand]->Name.empty() ) {
	  RW->printlog( "filter \"ALL\": auto-configure " + 
			Str( MCs[CurrentMacro]->Commands[CurrentCommand]->AutoConfigureTime ) + "s" );
	  RW->FD->autoConfigure( MCs[CurrentMacro]->Commands[CurrentCommand]->AutoConfigureTime );
	}
	else {
	  Filter *filter = RW->FD->filter( MCs[CurrentMacro]->Commands[CurrentCommand]->Name );
	  if ( filter != 0 ) {
	    if ( MCs[CurrentMacro]->Commands[CurrentCommand]->Filter == 1 ) {
	      RW->printlog( "filter \"" + filter->ident() + "\": save \"" + 
			    MCs[CurrentMacro]->Commands[CurrentCommand]->Params + "\"" );
	      filter->save( MCs[CurrentMacro]->Commands[CurrentCommand]->Params );
	    }
	    else {
	      RW->printlog( "filter \"" + filter->ident() + "\": auto-configure " + 
			    Str( MCs[CurrentMacro]->Commands[CurrentCommand]->AutoConfigureTime ) + "s" );
	      RW->FD->autoConfigure( filter, MCs[CurrentMacro]->Commands[CurrentCommand]->AutoConfigureTime );
	    }
	  }
	}
      }
      // detector:
      else if ( MCs[CurrentMacro]->Commands[CurrentCommand]->Detector > 0 ) {
	if ( MCs[CurrentMacro]->Commands[CurrentCommand]->Detector == 2 &&
	     MCs[CurrentMacro]->Commands[CurrentCommand]->Name.empty() ) {
	  RW->printlog( "detector \"ALL\": auto-configure " + 
			Str( MCs[CurrentMacro]->Commands[CurrentCommand]->AutoConfigureTime ) + "s" );
	  RW->FD->autoConfigure( MCs[CurrentMacro]->Commands[CurrentCommand]->AutoConfigureTime );
	}
	else {
	  Filter *filter = RW->FD->detector( MCs[CurrentMacro]->Commands[CurrentCommand]->Name );
	  if ( filter != 0 ) {
	    if ( MCs[CurrentMacro]->Commands[CurrentCommand]->Detector == 1 ) {
	      RW->printlog( "detector \"" + filter->ident() + "\" save: \"" + 
			    MCs[CurrentMacro]->Commands[CurrentCommand]->Params + "\"" );
	      filter->save( MCs[CurrentMacro]->Commands[CurrentCommand]->Params );
	    }
	    else {
	      RW->printlog( "detector \"" + filter->ident() + "\": auto-configure " + 
			    Str( MCs[CurrentMacro]->Commands[CurrentCommand]->AutoConfigureTime ) + "s" );
	      RW->FD->autoConfigure( filter, MCs[CurrentMacro]->Commands[CurrentCommand]->AutoConfigureTime );
	    }
	  }
	}
      }
      // switch macros:
      else if ( MCs[CurrentMacro]->Commands[CurrentCommand]->Switch ) {
	RW->printlog( "switch to macro file \"" + 
		      MCs[CurrentMacro]->Commands[CurrentCommand]->Name + "\"" );
	loadMacros( MCs[CurrentMacro]->Commands[CurrentCommand]->Name );
	if ( boolean( "fallbackonreload" ) )
	  return;
      }
      // start session:
      else if ( MCs[CurrentMacro]->Commands[CurrentCommand]->StartSession ) {
	RW->SN->startTheSession( false );
      }
      // message:
      else if ( MCs[CurrentMacro]->Commands[CurrentCommand]->Message ) {
	Str msg = MCs[CurrentMacro]->Commands[CurrentCommand]->Params;
	int i = msg.find( "$(" );
	while ( i >= 0 ) {
	  int c = msg.findBracket( i+1, "(", "" );
	  if ( c < 1 )
	    c = msg.size();
	  string cs( msg.substr( i+2, c-i-2 ) );
	  FILE *p = popen( cs.c_str(), "r" );
	  Str ns = "";
	  char ls[1024];
	  while ( fgets( ls, 1024, p ) != NULL )
	    ns += ls;
	  pclose( p );
	  ns.strip();
	  msg.replace( i, c-i+1, ns );
	  i = msg.find( "$(", i+3 );
	}
	if ( !msg.empty() ) {
	  MessageBox::information( MCs[CurrentMacro]->Commands[CurrentCommand]->Name, msg, 
				   MCs[CurrentMacro]->Commands[CurrentCommand]->TimeOut,
				   this );
	  msg.eraseMarkup();
	  RW->printlog( "message " +
			MCs[CurrentMacro]->Commands[CurrentCommand]->Name +
			": " + msg );
	}
      }
      else if ( MCs[CurrentMacro]->Commands[CurrentCommand]->Browse ) {
	Str file = MCs[CurrentMacro]->Commands[CurrentCommand]->Params;
	int i = file.find( "$(" );
	while ( i >= 0 ) {
	  int c = file.findBracket( i+1, "(", "" );
	  if ( c < 1 )
	    c = file.size();
	  string cs( file.substr( i+2, c-i-2 ) );
	  FILE *p = popen( cs.c_str(), "r" );
	  Str ns = "";
	  char ls[1024];
	  while ( fgets( ls, 1024, p ) != NULL )
	    ns += ls;
	  pclose( p );
	  ns.strip();
	  file.replace( i, c-i+1, ns );
	  i = file.find( "$(", i+3 );
	}
	file.expandPath();
	if ( !file.empty() ) {
	  RW->printlog( "browse " +
			MCs[CurrentMacro]->Commands[CurrentCommand]->Name +
			": " + file );
	  // create and exec dialog:
	  OptDialog *od = new OptDialog( false, this );
	  od->setCaption( MCs[CurrentMacro]->Commands[CurrentCommand]->Name );
	  QTextBrowser *hb = new QTextBrowser( this );
	  QStringList fpl;
	  fpl.push_back( file.dir().c_str() );
	  hb->setSearchPaths( fpl );
	  hb->setSource( QUrl::fromLocalFile( file.notdir().c_str() ) );
	  if ( hb->toHtml().isEmpty() ) {
	    hb->setText( string( "Sorry, can't find file <b>" + file + "</b>." ).c_str() );
	  }
	  hb->setMinimumSize( 600, 400 );
	  od->addWidget( hb );
	  od->addButton( "&Ok" );
	  od->exec();
	}
      }
    }

  } while ( CurrentCommand < 0 ||
	    MCs[CurrentMacro]->Commands[CurrentCommand]->RP == 0 ||
	    ! ( Enable || MCs[CurrentMacro]->Commands[CurrentCommand]->Enabled ) );

  if ( Enable )
    Enable = false;

  // start RePro:
  RePro *repro = MCs[CurrentMacro]->Commands[CurrentCommand]->RP;
  Options prjopt;
  repro->Options::setDefaults();
  repro->Options::read( MCs[CurrentMacro]->expandParams( MCs[CurrentMacro]->Commands[CurrentCommand]->Params, prjopt ) );
  repro->Options::read( repro->overwriteOptions() );
  repro->Options::read( MCs[CurrentMacro]->Commands[CurrentCommand]->CO );
  repro->projectOptions().read( prjopt );
  repro->projectOptions().read( MCs[CurrentMacro]->Commands[CurrentCommand]->CO );
  if ( repro->projectOptions().text( "project" ).empty() ) {
    string ps = MCs[CurrentMacro]->Project.text( "project" );
    if ( ps.empty() ) {
      for ( int k=(int)Stack.size()-1; k>=0; k-- ) {
	ps = Stack[k].MacroProject.text( "project" );
	if ( ! ps.empty() )
	  break;
      }
    }
    repro->projectOptions().setText( "project", ps );
    string es = MCs[CurrentMacro]->Project.text( "experiment" );
    if ( es.empty() ) {
      for ( int k=(int)Stack.size()-1; k>=0; k-- ) {
	es = Stack[k].MacroProject.text( "experiment" );
	if ( ! es.empty() )
	  break;
      }
    }
    if ( es.empty() )
      es = repro->name();
    repro->projectOptions().setText( "experiment", es );
  }

  emit startRePro( repro, MCs[CurrentMacro]->Action, saving );
}


void Macros::startNextRePro( void )
{
  startNextRePro( true );
}


void Macros::clearButton( void )
{
  if ( CurrentMacro >= 0 && CurrentMacro < (int)MCs.size() &&
       MCs[CurrentMacro]->PushButton != 0 ) {
    if ( MCs[CurrentMacro]->startSession() )
      MCs[CurrentMacro]->PushButton->setIcon( SessionIcon );
    else
      MCs[CurrentMacro]->PushButton->setIcon( IdleIcon );
  }
}


void Macros::runButton( void )
{
  if ( CurrentMacro >= 0 && CurrentMacro < (int)MCs.size() &&
       MCs[CurrentMacro]->PushButton != 0 )
    MCs[CurrentMacro]->PushButton->setIcon( RunningIcon );
}


void Macros::stackButton( void )
{
  if ( CurrentMacro >= 0 && CurrentMacro < (int)MCs.size() &&
       MCs[CurrentMacro]->PushButton != 0 ) {
    if ( Stack.empty() )
      MCs[CurrentMacro]->PushButton->setIcon( BaseIcon );
    else
      MCs[CurrentMacro]->PushButton->setIcon( StackIcon );
  }
}


void Macros::stackButtons( void )
{
  for ( int k = 0; k < (int)Stack.size(); k++ ) {
    int macro = Stack[k].MacroID;
    if ( macro >= 0 && macro < (int)MCs.size() &&
	 MCs[macro]->PushButton != 0 ) {
      if ( k==0 )
	MCs[macro]->PushButton->setIcon( BaseIcon );
      else
	MCs[macro]->PushButton->setIcon( StackIcon );
    }
  }
}


void Macros::clearStackButtons( void )
{
  for ( int k = (int)Stack.size()-1; k >= 0; k-- ) {
    int macro = Stack[k].MacroID;
    if ( macro >= 0 && macro < (int)MCs.size() &&
	 MCs[macro]->PushButton != 0 ) {
      if ( MCs[macro]->startSession() )
	MCs[macro]->PushButton->setIcon( SessionIcon );
      else
	MCs[macro]->PushButton->setIcon( IdleIcon );
    }
  }
  Stack.clear();
  clearButton();
}


void Macros::loadMacros( const string &file )
{
  load( file );
  check();
  warning();
  create();

  if ( boolean( "fallbackonreload" ) ) {
    ResumePos.clear();
    ResumeStack.clear();
    fallBack();
  }
}


void Macros::selectMacros( void )
{
  QFileDialog* fd = new QFileDialog( 0 );
  fd->setFileMode( QFileDialog::ExistingFile );
  fd->setWindowTitle( "Open Macro File" );
  //  fd->setDirectory( Str( (*OP).text( 0 ) ).dir().c_str() );
  QString types( "Macro files (m*.cfg);;"
		 "Config files (*.cfg);;"
		 "Any files (*)" );
  fd->setFilter( types );
  fd->setViewMode( QFileDialog::List );
  if ( fd->exec() == QDialog::Accepted ) {
    Str filename = "";
    QStringList qsl = fd->selectedFiles();
    if ( qsl.size() > 0 )
      loadMacros( qsl[0].toLatin1().data() );
  }
}


void Macros::switchMacro( QAction *action )
{
  for ( unsigned int k=0;
	k<SwitchActions.size() && (int)k < Options::size( "file" );
	k++ ) {
    if ( action == SwitchActions[k] ) {
      loadMacros( text( "file", k ) );
      break;
    }
  }
}


void Macros::reload( void )
{
  loadMacros( MacroFile );
}


void Macros::reloadRePro( const string &name )
{
  // find RePro:
  RePro *repro = RPs->nameRepro( name );
  if ( repro == 0 ) {
    RW->printlog( "! warning: Macros::reloadRePro() -> RePro " +
		  name + " not found!" );
  }

  for ( MacrosType::iterator mp = MCs.begin();
	mp != MCs.end(); ++mp )
    for ( vector<MacroCommand*>::iterator cp = (*mp)->Commands.begin(); 
	  cp != (*mp)->Commands.end();
	  ++cp )
      if ( (*cp)->Macro < 0 && (*cp)->Filter==0 && (*cp)->Detector==0 &&
	   !(*cp)->Switch && !(*cp)->Shell && !(*cp)->Message &&
	   (*cp)->Name == name )
	(*cp)->RP = repro;

}


void Macros::store( void )
{
  // store macro position:
  if ( CurrentMacro >= 0 && CurrentMacro < (int)MCs.size() ) {
    ResumePos.set( CurrentMacro, CurrentCommand,
		   MCs[CurrentMacro]->Variables, MCs[CurrentMacro]->Project );
    ResumeStack = Stack;
    ResumeMacroOnly = ThisMacroOnly;
    ResumeAction->setEnabled( true );
    ResumeNextAction->setEnabled( true );
  }
}


void Macros::softBreak( void )
{
  if ( RW->idle() )
    return;

  if ( CurrentMacro != FallBackIndex && CurrentMacro >= 0 ) {
    store();
    // request stop of current repro:
    MCs[CurrentMacro]->Commands[CurrentCommand]->RP->setSoftStop();
    ThisCommandOnly = true;
  }
}


void Macros::hardBreak( void )
{
  if ( CurrentMacro != FallBackIndex && CurrentMacro >= 0 ) {
    store();
    // start fallback macro:
    fallBack();
  }
}


void Macros::resume( void )
{
  if ( RW->idle() )
    return;

  if ( ResumePos.defined() ) {
    // resume:
    MCs[ResumePos.MacroID]->Variables = ResumePos.MacroVariables;
    MCs[ResumePos.MacroID]->Project = ResumePos.MacroProject;

    startMacro( ResumePos.MacroID, ResumePos.CommandID, 
		true, &ResumeStack );
    ThisMacroOnly = ResumeMacroOnly;

    // clear resume:
    ResumePos.clear();
    ResumeAction->setEnabled( false );
    ResumeNextAction->setEnabled( false );
  }
}


void Macros::resumeNext( void )
{
  if ( RW->idle() )
    return;

  if ( ResumePos.defined() ) {
    // resume:
    MCs[ResumePos.MacroID]->Variables = ResumePos.MacroVariables;
    MCs[ResumePos.MacroID]->Project = ResumePos.MacroProject;

    startMacro( ResumePos.MacroID, ResumePos.CommandID + 1, 
		true, &ResumeStack );
    ThisMacroOnly = ResumeMacroOnly;

    // clear resume:
    ResumePos.clear();
    ResumeAction->setEnabled( false );
    ResumeNextAction->setEnabled( false );
  }
}


void Macros::noMacro( RePro *repro )
{
  clearStackButtons();
  clearButton();

  CurrentMacro = -1;

  RW->startedMacro( "RePro", "" );
}


int Macros::index( const string &macro ) const
{
  if ( macro.empty() )
    return -1;

  Str id = macro;
  id.lower();

  for ( unsigned int k=0; k<MCs.size(); k++ ) {
    Str idr = MCs[k]->Name;
    idr.lower();
    if ( idr == id )
      return k;
  }

  return -1;
}


string Macros::options( void ) const
{
  if ( CurrentMacro >= 0 && CurrentCommand >= 0 ) {
    Options prjopt;
    return MCs[CurrentMacro]->expandParams( MCs[CurrentMacro]->Commands[CurrentCommand]->Params, prjopt );
  }
  else {
    return "";
  }
}


Options &Macros::variables( int macro )
{
  return MCs[macro]->Variables;
}


string Macros::variablesStr( int macro )
{
  string s = "";
  for ( int k=0; k<MCs[macro]->Variables.size(); k++ ) {
    if ( k > 0 )
      s += "; ";
    s += MCs[macro]->Variables[k].save();
  }
  return s;
}


Options &Macros::project( int macro )
{
  return MCs[macro]->Project;
}


string Macros::expandParams( int macro, const Str &params,
			     Options &prjopt ) const
{
  return MCs[macro]->expandParams( params, prjopt );
}


void Macros::saveConfig( ofstream &str )
{
  string sm = MacroFile;
  for ( int k=0; k<Options::size( "file" ); k++ )
    if ( MacroFile != text( "file", k ) )
      sm += '|' + text( "file", k );
  setText( "file", sm );
  setToDefault( "file" );
  if ( SwitchMenu != 0 ) {
    SwitchMenu->clear();
    SwitchActions.clear();
    for ( int k=0; k<Options::size( "file" ); k++ )
      SwitchActions.push_back( SwitchMenu->addAction( text( "file", k ).c_str() ) );
  }
  ConfigClass::saveConfig( str );
}


ostream &operator<< ( ostream &str, const Macros &macros )
{
  for ( unsigned int k=0; k<macros.MCs.size(); k++ )
    str << *macros.MCs[k];
  return str;
}


Macros::MacroPos::MacroPos( void )
  : MacroID( -1 ),
    CommandID( -1 ),
    MacroVariables(),
    MacroProject()
{
}


Macros::MacroPos::MacroPos( int macro, int command,
			    Options &var, Options &prj )
  : MacroID( macro ),
    CommandID( command ),
    MacroVariables( var ),
    MacroProject( prj )
{
}


Macros::MacroPos::MacroPos( const MacroPos &mp )
  : MacroID( mp.MacroID ),
    CommandID( mp.CommandID ), 
    MacroVariables( mp.MacroVariables ),
    MacroProject( mp.MacroProject )
{
}


void Macros::MacroPos::set( int macro, int command,
			    Options &var, Options &prj ) 
{
  MacroID = macro;
  CommandID = command;
  MacroVariables = var;
  MacroProject = prj;
}


void Macros::MacroPos::clear( void ) 
{
  MacroID = -1;
  CommandID = -1;
  MacroVariables.clear();
  MacroProject.clear();
}


bool Macros::MacroPos::defined( void )
{
  return ( MacroID >= 0 && CommandID >= 0 );
}


Macro::Macro( void )
  : Name( "" ), Action( 0 ), 
  Button( true ), Menu( true ), Key( true ),
  Keep( false ), Overwrite( false ),
  PushButton( 0 ), MAction( NULL ), PMenu( 0 ), RunAction( 0 ), BottomAction( 0 ),
  MacroNum( -1 ), MC( 0 ), Commands(), DialogOpen( false )
{
}


Macro::Macro( Str name, Macros *mc ) 
  : Action( 0 ), Button( true ), Menu( true ), Key( true ), 
  Keep( false ), Overwrite( false ),
  PushButton( 0 ), MAction( NULL ), PMenu( 0 ), RunAction( 0 ), BottomAction( 0 ),
  MacroNum( -1 ), MC( mc ), Commands(), DialogOpen( false )
{
  Project.clear();
  Project.addText( "project", "Project", "" );
  Project.addText( "experiment", "Experiment", "" );

  int cp = name.find( ':' );
  if ( cp > 0 ) {
    Variables.clear();
    addParams( name.substr( cp+1 ) );
    name.erase( cp );
  }

  if ( name.erase( StartUpIdent, 0, false, 3, Str::WordSpace ) > 0 )
    Action |= StartUp;
  if ( name.erase( ShutDownIdent, 0, false, 3, Str::WordSpace ) > 0 )
    Action |= ShutDown;
  if ( name.erase( FallBackIdent, 0, false, 3, Str::WordSpace ) > 0 ) {
    Action |= FallBack;
    Action |= ExplicitFallBack;
  }
  if ( name.erase( StartSessionIdent, 0, false, 3, Str::WordSpace ) > 0 )
    Action |= StartSession;
  if ( name.erase( StopSessionIdent, 0, false, 3, Str::WordSpace ) > 0 )
    Action |= StopSession;
  if ( name.erase( NoButtonIdent, 0, false, 3, Str::WordSpace ) > 0 ) {
    Button = false;
    Key = false;
  }
  if ( name.erase( NoKeyIdent, 0, false, 3, Str::WordSpace ) > 0 )
    Key = false;
  if ( name.erase( NoMenuIdent, 0, false, 3, Str::WordSpace ) > 0 ) {
    Menu = false;
    Button = false;
    Key = false;
  }
  if ( name.erase( KeepIdent, 0, false, 3, Str::WordSpace ) > 0 )
    Keep = true;
  if ( name.erase( OverwriteIdent, 0, false, 3, Str::WordSpace ) > 0 )
    Overwrite = true;
  Name = name.stripped( Str::WordSpace );
}


Macro::Macro( const Macro &macro ) 
  : Name( macro.Name ), 
  Variables( macro.Variables ),
  Project( macro.Project ),
  Action( macro.Action ), 
  Button( macro.Button ), Menu( macro.Menu ), Key( macro.Key ),
  Keep( false ), Overwrite( false ),
  PushButton( macro.PushButton ), MAction( macro.MAction ), 
  PMenu( macro.PMenu ), RunAction( macro.RunAction ), BottomAction( macro.BottomAction ),
  MacroNum( macro.MacroNum ), MC( macro.MC ), Commands( macro.Commands ),
  DialogOpen( macro.DialogOpen )
{
}


string Macro::name( void ) const
{
  return Name;
}


bool Macro::keep( void ) const
{
  return Keep;
}


void Macro::addParams( const Str &param )
{
  Variables.load( param, "=", ";" );
  Variables.setToDefaults();
  Options::iterator p = Variables.find( "project" );
  if ( p != Variables.end() ) {
    Project.setText( "project", p->text() );
    Variables.erase( p );
  }
  p = Variables.find( "experiment" );
  if ( p != Variables.end() ) {
    Project.setText( "experiment", p->text() );
    Variables.erase( p );
  }
}


string Macro::expandParams( const Str &params, Options &prjopt ) const
{
  StrQueue sq( params.stripped().preventLast( ";" ), ";" );
  for ( StrQueue::iterator sp=sq.begin(); sp != sq.end(); ++sp ) {
    // get identifier:
    string name = (*sp).ident( 0, "=", Str::WhiteSpace );
    if ( ! name.empty() ) {
      string value = (*sp).value();
      if ( value[0] == '$' ) {
	const Parameter &p = Variables[ value.substr( 1 ) ];
	if ( p.isNotype() ) {
	  if ( value.find( "rand" ) == 1 ) {
	    Str range = value.substr( 6 );
	    int p = range.find( ')' );
	    string unit;
	    if ( p >=0 ) {
	      unit = range.substr( p+1 );
	      range.erase( p );
	    }
	    double rnd = double( rand() ) / double( RAND_MAX );
	    p = range.find( ".." );
	    if ( p > 0 ) {
	      double min = range.number();
	      range.erase( 0, p+2 );
	      double max = range.number();
	      rnd = (max-min) * rnd + min;
	    }
	    else {
	      StrQueue sq( range, "," );
	      rnd = sq[ (int)floor( rnd*(sq.size()-1.0e-8) ) ].number();
	    }
	    (*sp) = name + "=" + Str( rnd ) + unit;
	  }
	  else
	    MC->RW->printlog( "! warning in Macro::expandParams(): " + value +
			      " is not defined as a variable!" );
	}
	else if ( p.isNumber() )
	  (*sp) = name + "=" + p.text( "%g%u" );
	else
	  (*sp) = name + "=" + p.text();
      }
    }
  }
  for ( StrQueue::iterator sp=sq.begin(); sp != sq.end(); ) {
    string name = (*sp).ident( 0, "=", Str::WhiteSpace );
    if ( name == "project" || name == "experiment" ) {
      prjopt.load( *sp );
      sp = sq.erase( sp );
    }
    else
      ++sp;
  }
  string newparams;
  sq.copy( newparams, ";" );
  return newparams;
}


void Macro::addMenu( QMenu *menu )
{
  if ( Menu ) {
    QMenu *firstpop = menu->addMenu( MAction->text() );
    RunAction = firstpop->addAction( menuStr().c_str(), this, SLOT( run() ) );
    if ( Key )
      RunAction->setShortcut( KeyCode );
    int n=2;
    firstpop->addAction( "&Options", this, SLOT( dialog() ) );	
    n++;
    PMenu = firstpop;
    firstpop->addSeparator();
    BottomAction = 0;

    QMenu *pop = firstpop;

    for ( unsigned int j=0; j<Commands.size(); j++, n++ ) {

      Commands[j]->addMenu( pop );

      if ( n > 20 ) {
	pop->addSeparator();
	QMenu *nextpop = pop->addMenu( "More..." );
	pop = nextpop;
	if ( BottomAction == 0 )
	  BottomAction = nextpop->menuAction();
	n = 0;
      }
    }
    if ( BottomAction == 0 && Commands.size() > 0 )
      BottomAction = Commands.back()->SubMenu->menuAction();

  }
}


string Macro::menuStr( void ) const
{
  string s = "&Run Macro " + Name;
  int nc = 10 + Name.size();
  int k = 0;
  int i = 0;
  for ( i=0; nc < Macros::MenuWidth && i<Variables.size(); i++, k++ ) {
    if ( k>0 ) {
      s += "; ";
      nc += 2;
    }
    else {
      s += ": ";
      nc += 2;
    }
    string vs = Variables[i].save();
    s += vs;
    nc += vs.size();
  }
  if ( i < Variables.size() ) {
    s += " ...";
    return s;
  }
  for ( i=0; nc < Macros::MenuWidth && i<Project.size(); i++, k++ ) {
    if ( ! Project[i].text().empty() ) {
      if ( k>0 ) {
	s += "; ";
	nc += 2;
      }
      else {
	s += ": ";
	nc += 2;
      }
      string vs = Project[i].save();
      s += vs;
      nc += vs.size();
    }
  }
  if ( i < Project.size() )
    s += " ...";
  return s;
}


void Macro::addButton( const string &keys )
{
  if ( Button ) {
    MacroButton *button = new MacroButton( Name + keys, MC );
    button->show();
    if ( startSession() )
      button->setIcon( MC->SessionIcon );
    else
      button->setIcon( MC->IdleIcon );
    button->setMinimumSize( button->sizeHint() );
    connect( button, SIGNAL( clicked() ), this, SLOT( launch() ) );
    connect( button, SIGNAL( rightClicked() ), this, SLOT( popup() ) );
    if ( Key ) {
      MAction->setShortcut( KeyCode + Qt::SHIFT );
      connect( MAction, SIGNAL( triggered() ), this, SLOT( popup() ) );
    }
    PushButton = button;
  }
  else
    PushButton = 0;
}


ostream &operator<< ( ostream &str, const Macro &macro )
{
  str << "Macro " << macro.MacroNum+1 << ": " << macro.Name 
      << ( macro.startUp() ? " startup" : "" )
      << ( macro.shutDown() ? " shutdown" : "" )
      << ( macro.fallBack() ? " fallback" : "" ) 
      << ( macro.startSession() ? " startsession" : "" )
      << ( macro.stopSession() ? " stopsession" : "" )
      << ( macro.Button ? "" : " nobutton" )
      << ( macro.Menu ? "" : " nomenu" );
  if ( macro.MAction != NULL )
    str << "Action: " << macro.MAction->shortcut().toString().toLatin1().data();
  str << " -> " << macro.Variables.save() << '\n';
  for ( unsigned int j=0; j<macro.Commands.size(); j++ )
    str << *macro.Commands[j];
  return str;
}


MacroButton *Macro::button( void )
{
  return PushButton;
}


void Macro::clear( void )
{
  delete MAction;
  MAction = 0;
  Key = 0;
  if ( PushButton != 0 ) {
    delete PushButton;
    PushButton = 0;
  }
}


void Macro::dialog( void )
{
  if ( DialogOpen )
    return;

  DialogOpen = true;
  // create and exec dialog:
  OptDialog *od = new OptDialog( false, MC );
  od->setCaption( "Macro " + Name + " Variables" );
  if ( ! Variables.empty() )
    od->addOptions( Variables );
  od->addOptions( Project );
  od->setVerticalSpacing( int(9.0*exp(-double(Variables.size())/14.0))+1 );
  od->setRejectCode( 0 );
  od->addButton( "&Ok", OptDialog::Accept, 1 );
  od->addButton( "&Apply", OptDialog::Accept, 1, false );
  od->addButton( "&Run", OptDialog::Accept, 2, false );
  od->addButton( "&Reset", OptDialog::Defaults );
  od->addButton( "&Close" );
  connect( od, SIGNAL( dialogClosed( int ) ),
	   this, SLOT( dialogClosed( int ) ) );
  connect( od, SIGNAL( buttonClicked( int ) ),
	   this, SLOT( dialogAction( int ) ) );
  connect( od, SIGNAL( valuesChanged( void ) ),
	   this, SLOT( acceptDialog( void ) ) );
  od->exec();
}


void Macro::acceptDialog( void )
{
  Variables.setToDefaults();
  Project.setToDefaults();
  // update menu:
  RunAction->setText( menuStr().c_str() );
}


void Macro::dialogAction( int r )
{
  if ( r == 2 )
    MC->startMacro( MacroNum );
  // Note: in case of a switch command, *this does not exist anymore!
}


void Macro::dialogClosed( int r )
{
  DialogOpen = false;
}


void Macro::run( void )
{
  if ( KeyCode == Qt::Key_Escape &&
       qApp->focusWidget() != MC->window() ) {
    MC->window()->setFocus();
  }
  else
    launch();
}


void Macro::launch( void )
{
  MC->window()->setFocus();
  if ( fallBack() )
    MC->store();
  Variables.setDefaults();
  MC->startMacro( MacroNum );
  // Note: in case of a switch command, *this does not exist anymore!
}


void Macro::popup( void )
{
  if ( BottomAction != 0 ) {
    QPoint p = PushButton->mapToGlobal( QPoint( 0, -30 ) );
    PMenu->popup( p, BottomAction );
  }
}


MacroCommand::MacroCommand( void ) 
  : Name( "" ),
    Params( "" ),
    RP( 0 ),
    CO(),
    PO(),
    DO( 0 ),
    Macro( -1 ),
    Filter( 0 ),
    Detector( 0 ),
    AutoConfigureTime( 0.0 ),
    Switch( false ),
    StartSession( false ),
    Shell( false ),
    Message( false ),
    TimeOut( 0.0 ),
    Browse( false ),
    Enabled( true ),
    EnabledAction( 0 ),
    MacroNum( 0 ),
    CommandNum( 0 ),
    MCs( 0 ),
    DialogOpen( false ),
    MacroVars(),
    MacroProject(),
    MenuShortcut( "" ),
    SubMenu( 0 )
{
}


MacroCommand::MacroCommand( const string &name, const string &params, 
			    bool enabled, int macro, bool filter,
			    bool detector, bool switchm, bool startsession,
			    bool shell, bool mes, double to, bool browse,
			    Macros *mc ) 
  : Name( name ),
    Params( params ),
    RP( 0 ),
    CO(), 
    PO(), 
    DO( 0 ),
    Macro( macro ),
    Filter( filter ? 1 : 0 ),
    Detector( detector ? 1 : 0 ),
    AutoConfigureTime( 0.0 ),
    Switch( switchm ),
    StartSession( startsession ),
    Shell( shell ),
    Message( mes ),
    TimeOut( to ),
    Browse( browse ),
    Enabled( enabled ),
    EnabledAction( 0 ),
    MacroNum( 0 ),
    CommandNum( 0 ),
    MCs( mc ),
    DialogOpen( false ),
    MacroVars(),
    MacroProject(),
    MenuShortcut( "" ),
    SubMenu( 0 )
{
  if ( Filter > 0 ) {
    if ( Params.eraseFirst( "save", 0, false, 3, Str::WhiteSpace ) )
      Filter = 1;
    else if ( Params.eraseFirst( "autoconf", 0, false, 3, Str::WhiteSpace ) ) {
      Filter = 2;
      AutoConfigureTime = Params.number( 1.0 );
      Params.clear();
    }
  }
  if ( Detector > 0 ) {
    if ( Params.eraseFirst( "save", 0, false, 3, Str::WhiteSpace ) )
      Detector = 1;
    else if ( Params.eraseFirst( "autoconf", 0, false, 3, Str::WhiteSpace ) ) {
      Detector = 2;
      AutoConfigureTime = Params.number( 1.0 );
      Params.clear();
    }
  }
}


MacroCommand::MacroCommand( RePro *repro, const string &params, Macros *mc )
  : Name( repro->name() ),
    Params( params ),
    RP( repro ),
    CO(), 
    PO(), 
    DO( 0 ),
    Macro( -1 ),
    Filter( 0 ),
    Detector( 0 ),
    AutoConfigureTime( 0.0 ),
    Switch( false ),
    StartSession( false ),
    Shell( false ),
    Message( false ),
    TimeOut( 0.0 ),
    Browse( false ),
    Enabled( true ),
    EnabledAction( 0 ),
    MacroNum( 0 ),
    CommandNum( 0 ),
    MCs( mc ),
    DialogOpen( false ),
    MacroVars(),
    MacroProject(),
    MenuShortcut( "" ),
    SubMenu( 0 )
{
}


MacroCommand::MacroCommand( const MacroCommand &com ) 
  : Name( com.Name ),
    Params( com.Params ),
    RP( com.RP ),
    CO( com.CO ),
    PO( com.PO ),
    DO( com.DO ),
    Macro( com.Macro ),
    Filter( com.Filter ), 
    Detector( com.Detector ), 
    AutoConfigureTime( com.AutoConfigureTime ),
    Switch( com.Switch ), 
    StartSession( com.StartSession ), 
    Shell( com.Shell ), 
    Message( com.Message ),
    TimeOut( com.TimeOut ),
    Browse( com.Browse ),
    Enabled( com.Enabled ),
    EnabledAction( com.EnabledAction ),
    MacroNum( com.MacroNum ),
    CommandNum( com.CommandNum ),
    MCs( com.MCs ),
    DialogOpen( com.DialogOpen ),
    MacroVars( com. MacroVars ),
    MacroProject( com.MacroProject ),
    MenuShortcut( com.MenuShortcut ),
    SubMenu( com.SubMenu )
{
}


void MacroCommand::addMenu( QMenu *menu )
{
  string s = "";
  if ( CommandNum < 36 ) {
    s = "&";
    if ( CommandNum == 0 )
      s += '0';
    else if ( CommandNum < 10 )
      s += ( '1' + CommandNum - 1 );
    else
      s += ( 'a' + CommandNum - 10 );
    s += " ";
  }
  else
    s = "  ";
  MenuShortcut = s;
  if ( Macro >= 0 ) {
    s += "Macro " + Name;
    if ( !Params.empty() ) {
      s += ": ";
      int index = 0;
      int nc = 10 + Name.size();
      for ( int i=0; nc+index < Macros::MenuWidth && index >= 0; i++ ) {
	index = Params.find( ';', index+1 );
      }
      if ( index < 0 )
	s += ": " + Params;
      else
	s += ": " + Params.substr( 0, index ) + " ...";
    }
  }
  else if ( Shell )
    s += "Shell " + Name;
  else if ( Filter ) {
    s += "Filter " + Name + ": ";
    if ( Filter == 1 )
      s += "save";
    else
      s += "auto-configure " + Str( AutoConfigureTime ) + "s";
  }
  else if ( Detector ) {
    s += "Detector " + Name + ": ";
    if ( Detector == 1 )
      s += "save";
    else
      s += "auto-configure " + Str( AutoConfigureTime ) + "s";
  }
  else if ( Switch )
    s += "Switch to " + Name;
  else if ( StartSession )
    s += "Start Session";
  else if ( Message ) {
    s += "Message " + Name;
    if ( !Params.empty() ) {
      Str ps = Params;
      ps.eraseMarkup();
      if ( ps.size() > 40 ) {
	ps.erase( 36 );
	ps += " ...";
      }
      s += ": " + ps;
    }
  }
  else if ( Browse ) {
    s += "Browse " + Params;
  }
  else {
    s += "RePro " + Name;
    if ( !Params.empty() ) {
      s += ": ";
      int index = 0;
      int nc = 10 + Name.size();
      for ( int i=0; nc+index < Macros::MenuWidth && index >= 0; i++ ) {
	index = Params.find( ';', index+1 );
      }
      if ( index < 0 )
	s += ": " + Params;
      else
	s += ": " + Params.substr( 0, index ) + " ...";
    }
  }

  SubMenu = menu->addMenu( s.c_str() );
  SubMenu->menuAction()->setIcon( Enabled ? MCs->EnabledIcon : MCs->DisabledIcon );

  if ( CommandNum+1 < (int)MCs->MCs[MacroNum]->Commands.size() ) {
    SubMenu->addAction( "&Start macro here", this, SLOT( start() ) );
    SubMenu->addAction( "&Run only this", this, SLOT( run() ) );
  }
  else {
    SubMenu->addAction( "&Run", this, SLOT( run() ) );
  }
  if ( RP != 0 ) {
    SubMenu->addAction( "&Options...", this, SLOT( dialog() ) );
    SubMenu->addAction( "&View", this, SLOT( view() ) );
    SubMenu->addAction( "&Load", this, SLOT( reload() ) );
    SubMenu->addAction( "&Help...", this, SLOT( help() ) );
  }
  else if ( Macro >= 0 &&
	    ! Params.empty() ) {
    SubMenu->addAction( "&Options...", this, SLOT( dialog() ) );
  }
  EnabledAction = SubMenu->addAction( Enabled ? "&Disable" : "&Enable",
				      this, SLOT( enable() ) );
}


void MacroCommand::start( void )
{
  MCs->MCs[MacroNum]->Variables.setDefaults();
  MCs->Enable = true;
  MCs->startMacro( MacroNum, CommandNum );
  // Note: in case of a switch command, *this does not exist anymore?
}


void MacroCommand::run( void )
{
  MCs->MCs[MacroNum]->Variables.setDefaults();
  MCs->Enable = true;
  MCs->startMacro( MacroNum, CommandNum );
  // XXX Note: in case of a switch command, *this does not exist anymore? XXX
  if ( Macro >= 0 )
    MCs->ThisMacroOnly = true;
  else
    MCs->ThisCommandOnly = true;
}


void MacroCommand::dialog( void )
{
  if ( DialogOpen || ( RP != 0 && RP->dialogOpen() ) )
    return;

  DialogOpen = true;
  DO = &MCs->RPs->dialogOptions();

  if ( Macro >= 0 ) {
    // Macro dialog:
    Options prjopt;
    MacroVars = MCs->variables( Macro );
    MacroVars.setDefaults();
    MacroVars.read( MCs->expandParams( MacroNum, Params, prjopt ) );
    MacroProject = MCs->project( Macro );
    MacroProject.read( prjopt );
    // create and exec dialog:
    OptDialog *od = new OptDialog( false, MCs );
    od->setCaption( "Macro " + Name + " Variables" );
    od->addOptions( MacroVars );
    od->addOptions( MacroProject );
    od->setVerticalSpacing( int(9.0*exp(-double(MacroVars.size())/14.0))+1 );
    od->setRejectCode( 0 );
    od->addButton( "&Ok", OptDialog::Accept, 1 );
    od->addButton( "&Apply", OptDialog::Accept, 1, false );
    od->addButton( "&Run", OptDialog::Accept, 2, false );
    od->addButton( "&Defaults", OptDialog::Defaults );
    od->addButton( "&Close" );
    connect( od, SIGNAL( dialogClosed( int ) ),
	     this, SLOT( dialogClosed( int ) ) );
    connect( od, SIGNAL( buttonClicked( int ) ),
	     this, SLOT( dialogAction( int ) ) );
    connect( od, SIGNAL( valuesChanged( void ) ),
	     this, SLOT( acceptDialog( void ) ) );
    od->exec();
  }
  else {
    // RePro dialog:
    Options prjopt;
    RP->Options::setDefaults();
    RP->Options::read( MCs->expandParams( MacroNum, Params, prjopt ), RePro::MacroFlag );
    RP->Options::read( RP->overwriteOptions(), 0, RePro::OverwriteFlag );
    RP->Options::read( CO, 0, RePro::CurrentFlag );
    RP->projectOptions().read( prjopt );
    RP->projectOptions().read( PO );
    
    RP->dialog();
    
    connect( (ConfigDialog*)RP, SIGNAL( dialogAccepted( void ) ),
	     this, SLOT( acceptDialog( void ) ) );
    connect( (ConfigDialog*)RP, SIGNAL( dialogAction( int ) ),
	     this, SLOT( dialogAction( int ) ) );
    connect( (ConfigDialog*)RP, SIGNAL( dialogClosed( int ) ),
	     this, SLOT( dialogClosed( int ) ) );
  }
}


void MacroCommand::acceptDialog( void )
{
  if ( Macro >= 0 ) {
    Options prjopt;
    Options po( MCs->expandParams( MacroNum, Params, prjopt ) );
    po.readAppend( MacroVars, OptDialog::changedFlag() );
    po.readAppend( MacroProject, OptDialog::changedFlag() );
    Params = po.save( "; " );
    // update menu:
    Str s = MenuShortcut;
    s += "Macro " + Name;
    if ( !Params.empty() ) {
      s += ": ";
      int index = 0;
      int nc = 10 + Name.size();
      for ( int i=0; nc+index < Macros::MenuWidth && index >= 0; i++ ) {
	index = Params.find( ';', index+1 );
      }
      if ( index < 0 )
	s += ": " + Params;
      else
	s += ": " + Params.substr( 0, index ) + " ...";
    }
    SubMenu->menuAction()->setText( s.c_str() );
  }
  else {
    Options newopt( *((Options*)RP), OptDialog::changedFlag() );
    if ( DO->boolean( "overwrite" ) ) {
      RP->overwriteOptions().readAppend( newopt );
    }
    if ( DO->boolean( "default" ) ) {
      Options prjopt;
      RP->Options::setToDefaults();
      RP->Options::read( MCs->expandParams( MacroNum, Params, prjopt ) );
      RP->Options::read( RP->overwriteOptions() );
      CO.assign( *((Options*)RP), Options::NonDefault );
      RP->Options::setDefaults();
      CO.read( *((Options*)RP) );
    }
    else {
      CO.readAppend( newopt );
    }
    PO = RP->projectOptions();
  }
}


void MacroCommand::dialogAction( int r )
{
  // run:
  if ( r == 2 )
    MCs->startMacro( MacroNum, CommandNum );
  // Note: in case of a switch command, *this does not exist anymore?

  // defaults:
  if ( r == 3 )
    CO.clear();
}


void MacroCommand::dialogClosed( int r )
{
  DialogOpen = false;
  disconnect( (ConfigDialog*)RP, SIGNAL( dialogAccepted( void ) ),
	      this, SLOT( acceptDialog( void ) ) );
  disconnect( (ConfigDialog*)RP, SIGNAL( dialogAction( int ) ),
	      this, SLOT( dialogAction( int ) ) );
  disconnect( (ConfigDialog*)RP, SIGNAL( dialogClosed( int ) ),
	      this, SLOT( dialogClosed( int ) ) );
}


void MacroCommand::view( void )
{
  MCs->RPs->raise( RP );
}


void MacroCommand::reload( void )
{
  MCs->RPs->reload( RP );
}


void MacroCommand::help( void )
{
  MCs->RPs->help( RP );
}


void MacroCommand::enable( void )
{
  Enabled = ! Enabled;
  EnabledAction->setText( Enabled ? "&Disable" : "&Enable" );
  SubMenu->menuAction()->setIcon( Enabled ? MCs->EnabledIcon : MCs->DisabledIcon );
}


ostream &operator<< ( ostream &str, const MacroCommand &command )
{
  str << "  " << command.CommandNum+1 << " ";
  if ( command.Macro >= 0 )
    str << "Macro";
  else if ( command.Shell )
    str << "Shell";
  else if ( command.Filter )
    str << "Filter " << ( command.Filter == 1 ? "save" : "auto-configure" );
  else if ( command.Detector )
    str << "Detector " << ( command.Detector == 1 ? "save" : "auto-configure" );
  else if ( command.Switch )
    str << "Switch";
  else if ( command.StartSession )
    str << "StartSession";
  else if ( command.Message ) {
    str << "Message";
    if ( command.TimeOut > 0.0 )
      str << " (timeout " << command.TimeOut << ")";
  }
  else if ( command.Browse )
    str << "Browse";
  else
    str << "RePro";
  str << ": " << command.Name 
      << " -> " << command.Params << '\n';
  return str;
}


MacroButton::MacroButton( const string &title, QWidget *parent )
  : QPushButton( title.c_str(), parent )
{
}


void MacroButton::mouseReleaseEvent( QMouseEvent *qme )
{
  QPushButton::mouseReleaseEvent( qme );
  if ( qme->button() == Qt::RightButton ) {
    emit rightClicked();
  }
}


}; /* namespace relacs */

#include "moc_macros.cc"

