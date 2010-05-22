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
#include <qdatetime.h>
#include <qapplication.h>
#include <qkeycode.h>
#include <qpainter.h>
#include <qbitmap.h>
#include <qtextbrowser.h>
#include <qfiledialog.h>
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


Macros::Macros( RELACSWidget *rw, QWidget *parent, const char *name )
  : QWidget( parent, name ),
    ConfigClass( "Macros", RELACSPlugin::Core ),
    RW( rw ), RP( 0 ), MCs(),
    CurrentMacro( -1 ), CurrentCommand( 0 ),
    Stack(), ResumePos(), ResumeMacroOnly( false ),
    ThisCommandOnly( false ), ThisMacroOnly( false ), Enable( false ),
    Warnings( "" ), 
    StartUpIndex( 0 ), ShutDownIndex( -1 ), FallBackIndex( 0 ), 
    StartSessionIndex( 1 ), StopSessionIndex( -1 ),
    MacroFile( "" ),
    Menu( 0 ), SwitchMenu( 0 ), ButtonLayout( 0 ), ButtonMenuKeys( 0 ),
    Fatal( false )
{
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
  /*
  cerr << font().pointSize() << endl;
  cerr << font().pixelSize() << endl;
  cerr << fontInfo().pointSize() << endl;
  cerr << fontInfo().pixelSize() << endl;
  cerr << fontMetrics().height() << endl;
  */

  int my = fontInfo().pixelSize() - 2;
  int mx = my;

  SessionIcon.resize( mx+2, my+2 );
  QPainter p;
  p.begin( &SessionIcon, this );
  p.eraseRect( SessionIcon.rect() );
  p.setPen( QPen( black, 1 ) );
  p.setBrush( black );
  QPointArray pa( 3 );
  pa.setPoint( 0, mx/3, 0 );
  pa.setPoint( 1, mx/3, my );
  pa.setPoint( 2, mx, my/2 );
  p.drawPolygon( pa );
  p.end();
  SessionIcon.setMask( SessionIcon.createHeuristicMask() );

  BaseIcon.resize( mx+2, my+2 );
  p.begin( &BaseIcon, this );
  p.eraseRect( BaseIcon.rect() );
  p.setPen( QPen( black, 1 ) );
  p.setBrush( red );
  p.drawEllipse( mx/4, (my-mx*3/4)/2, mx*3/4, mx*3/4 );
  p.end();
  BaseIcon.setMask( BaseIcon.createHeuristicMask() );

  StackIcon.resize( mx+2, my+2 );
  p.begin( &StackIcon, this );
  p.eraseRect( StackIcon.rect() );
  p.setPen( QPen( black, 1 ) );
  p.setBrush( yellow );
  p.drawEllipse( mx/4, (my-mx*3/4)/2, mx*3/4, mx*3/4 );
  p.end();
  StackIcon.setMask( StackIcon.createHeuristicMask() );

  RunningIcon.resize( mx+2, my+2 );
  p.begin( &RunningIcon, this );
  p.eraseRect( RunningIcon.rect() );
  p.setPen( QPen( black, 1 ) );
  p.setBrush( green );
  p.drawEllipse( mx/4, (my-mx*3/4)/2, mx*3/4, mx*3/4 );
  p.end();
  RunningIcon.setMask( RunningIcon.createHeuristicMask() );

  IdleIcon.resize( mx+2, my+2 );
  p.begin( &IdleIcon, this );
  p.eraseRect( IdleIcon.rect() );
  p.end();
  IdleIcon.setMask( IdleIcon.createHeuristicMask() );

  EnabledIcon.resize( mx, mx );
  p.begin( &EnabledIcon, this );
  p.eraseRect( EnabledIcon.rect() );
  p.setPen( QPen( black, 1 ) );
  p.setBrush( green );
  p.drawEllipse( 0, 0, mx-1, mx-1 );
  p.end();
  EnabledIcon.setMask( EnabledIcon.createHeuristicMask() );

  DisabledIcon.resize( mx, mx );
  p.begin( &DisabledIcon, this );
  p.eraseRect( DisabledIcon.rect() );
  p.setPen( QPen( black, 1 ) );
  p.setBrush( red );
  p.drawEllipse( 0, 0, mx-1, mx-1 );
  p.end();
  DisabledIcon.setMask( DisabledIcon.createHeuristicMask() );
}


void Macros::clear( bool keep )
{
  // clear buttons:
  for ( unsigned int k=0; k<MCs.size(); k++ ) {
    if ( MCs[k]->PushButton != 0 ) {
      if ( MCs[k]->Key ) {
	ButtonMenuKeys->setItemEnabled( MCs[k]->AccelId, false );
	ButtonMenuKeys->disconnectItem( MCs[k]->AccelId,
				    MCs[k]->PushButton,
				    SLOT( wasRightClicked( void ) ) );
	ButtonMenuKeys->removeItem( MCs[k]->AccelId );
	MCs[k]->AccelId = -1;
      }
      ButtonLayout->remove( MCs[k]->PushButton );
      MCs[k]->PushButton->hide();
      delete MCs[k]->PushButton;
      MCs[k]->PushButton = 0;
    }
  }
  delete ButtonLayout;
  delete ButtonMenuKeys;

  // clear macros:
  MacrosType::iterator mp = MCs.begin();
  while ( mp != MCs.end() ) {
    if ( ! ( (*mp)->Keep && keep ) ) {
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
      else if ( !startsession && !shell && !message && !browse &&
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

	else if ( (*cp)->Filter ) {
	  if ( ! RW->FD->exist( (*cp)->Name ) ) {
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

	else if ( (*cp)->Detector ) {
	  if ( ! RW->FD->exist( (*cp)->Name ) ) {
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
	  RePro *repro = RP->nameRepro( (*cp)->Name );
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
  for ( int k=0; k<RP->size(); k++ )
    RP->repro( k )->setDefaults();

  // no macros?
  if ( MCs.empty() ) {

    if ( !Warnings.empty() )
      Warnings += "\n";
    Warnings += "No Macros specified! Trying to create Macros from RePros...\n";

    for ( int k=0; k<RP->size(); k++ ) {
      MCs.push_back( new Macro( RP->repro( k )->name(), this ) );
      MCs.back()->Commands.push_back( new MacroCommand( RP->repro( k ),
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


void Macros::buttons( void )
{
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
  int rows = ( nb - 1 ) / cols + 1;

  ButtonLayout = new QGridLayout( this, rows, cols );
  ButtonMenuKeys = new QAccel( this );

  // create buttons:
  int fkc = 0;
  int row=0;
  int col=0;
  for ( unsigned int k=0; k<MCs.size(); k++ ) {

    Str keys = "";
    if ( MCs[k]->Key ) {
      if ( MCs[k]->fallBack() ) {
	MCs[k]->KeyCode = Key_Escape;
	keys = " (ESC)";
      }
      else if ( fkc < 12 ) {
	MCs[k]->KeyCode = Key_F1 + fkc;
	fkc++;
	keys = Str( fkc, " (F%d)" );
      }
      else
	MCs[k]->KeyCode = 0;
    }
    else
      MCs[k]->KeyCode = 0;

    if ( MCs[k]->Button ) {
      MacroButton *button = new MacroButton( k, MCs[k]->Name + keys, this );
      ButtonLayout->addWidget( button, row, col );
      button->show();
      if ( MCs[k]->startSession() )
	button->setIconSet( SessionIcon );
      else
	button->setIconSet( IdleIcon );
      button->setMinimumSize( button->sizeHint() );
      connect( button, SIGNAL( leftClicked( int ) ),
	       this, SLOT( launch( int ) ) );
      connect( button, SIGNAL( rightClicked( int ) ),
	       this, SLOT( popup( int ) ) );
      if ( MCs[k]->Key ) {
	MCs[k]->AccelId = ButtonMenuKeys->insertItem( MCs[k]->KeyCode + SHIFT );
 	ButtonMenuKeys->connectItem( MCs[k]->AccelId,
				     button, SLOT( wasRightClicked( void ) ) );
      }
      MCs[k]->PushButton = button;
      col++;
      if ( col >= cols ) {
	col=0;
	row++;
      }
    }
    else
      MCs[k]->PushButton = 0;

  }

  setFixedHeight( minimumSizeHint().height() );
  update();
}


QPopupMenu* Macros::menu( void )
{
  string s;

  if ( Menu == 0 )
    Menu = new QPopupMenu( this );
  else {
    Menu->clear();
    SwitchMenu = 0;
  }

  Menu->insertItem( "&Reload", this, SLOT( reload( void ) ) );
  Menu->insertItem( "&Load...", this, SLOT( selectMacros( void ) ) );
  if ( Options::size( "file" ) > 1 ) {
    SwitchMenu = new QPopupMenu( Menu );
    for ( int k=0; k<Options::size( "file" ); k++ )
      SwitchMenu->insertItem( text( "file", k ).c_str(), k );
    connect( SwitchMenu, SIGNAL( activated( int ) ), 
	     this, SLOT( switchMacro( int ) ) );
    Menu->insertItem( "&Switch", SwitchMenu );
  }
  Menu->insertItem( "&Skip RePro", this, SLOT( startNextRePro( void ) ), Key_S );
  Menu->insertItem( "&Break", this, SLOT( softBreak( void ) ), Key_B );
  Menu->insertItem( "Resume", this, SLOT( resume( void ) ), Key_R, 3 );
  Menu->insertItem( "Resume Next", this, SLOT( resumeNext( void ) ), Key_N, 4 );
  Menu->setItemEnabled( 3, false );
  Menu->setItemEnabled( 4, false );
  Menu->insertSeparator();
  for ( unsigned int k=0; k<MCs.size(); k++ ) {
    if ( MCs[k]->Menu ) {
      QPopupMenu *firstpop = new QPopupMenu( Menu );
      QPopupMenu *pop = firstpop;
      s = MCs[k]->menuStr();
      firstpop->insertItem( s.c_str(), k << 10 );
      if ( MCs[k]->Key )
	firstpop->setAccel( MCs[k]->KeyCode, k << 10 );
      int n=2;
      firstpop->insertItem( "&Options", k << 10 | 1 );	
      n++;
      MCs[k]->PMenu = firstpop;
      MCs[k]->PMenuId = ( k << 10 );
      MCs[k]->MacroNum = k;
      firstpop->insertSeparator();
      for ( unsigned int j=0; j<MCs[k]->Commands.size(); j++, n++ ) {
	if ( j < 36 ) {
	  s = "&";
	  if ( j == 0 )
	    s += '0';
	  else if ( j < 10 )
	    s += ( '1' + j - 1 );
	  else
	    s += ( 'a' + j - 10 );
	  s += " ";
	}
	else
	  s = "  ";
	MCs[k]->Commands[j]->MenuShortcut = s;
	if ( MCs[k]->Commands[j]->Macro >= 0 ) {
	  s += "Macro " + MCs[k]->Commands[j]->Name;
	  if ( !MCs[k]->Commands[j]->Params.empty() ) {
	    s += ": ";
	    int index = 0;
	    int nc = 10 + MCs[k]->Commands[j]->Name.size();
	    for ( int i=0; nc+index < MenuWidth && index >= 0; i++ ) {
	      index = MCs[k]->Commands[j]->Params.find( ';', index+1 );
	    }
	    if ( index < 0 )
	      s += ": " + MCs[k]->Commands[j]->Params;
	    else
	      s += ": " + MCs[k]->Commands[j]->Params.substr( 0, index ) + " ...";
	  }
	}
	else if ( MCs[k]->Commands[j]->Shell )
	  s += "Shell " + MCs[k]->Commands[j]->Name;
	else if ( MCs[k]->Commands[j]->Filter )
	  s += "Filter " + MCs[k]->Commands[j]->Name;
	else if ( MCs[k]->Commands[j]->Detector )
	  s += "Detector " + MCs[k]->Commands[j]->Name;
	else if ( MCs[k]->Commands[j]->Switch )
	  s += "Switch to " + MCs[k]->Commands[j]->Name;
	else if ( MCs[k]->Commands[j]->StartSession )
	  s += "Start Session";
	else if ( MCs[k]->Commands[j]->Message ) {
	  s += "Message " + MCs[k]->Commands[j]->Name;
	  if ( !MCs[k]->Commands[j]->Params.empty() ) {
	    Str ps = MCs[k]->Commands[j]->Params;
	    ps.eraseMarkup();
	    if ( ps.size() > 40 ) {
	      ps.erase( 36 );
	      ps += " ...";
	    }
	    s += ": " + ps;
	  }
	}
	else if ( MCs[k]->Commands[j]->Browse ) {
	  s += "Browse " + MCs[k]->Commands[j]->Params;
	}
	else {
	  s += "RePro " + MCs[k]->Commands[j]->Name;
	  if ( !MCs[k]->Commands[j]->Params.empty() ) {
	    s += ": ";
	    int index = 0;
	    int nc = 10 + MCs[k]->Commands[j]->Name.size();
	    for ( int i=0; nc+index < MenuWidth && index >= 0; i++ ) {
	      index = MCs[k]->Commands[j]->Params.find( ';', index+1 );
	    }
	    if ( index < 0 )
	      s += ": " + MCs[k]->Commands[j]->Params;
	    else
	      s += ": " + MCs[k]->Commands[j]->Params.substr( 0, index ) + " ...";
	  }
	}

	QPopupMenu *subpop = new QPopupMenu( pop );
	if ( j+1 < MCs[k]->Commands.size() ) {
	  subpop->insertItem( "&Start macro here", ( k << 10 ) | (j+1) << 3 );
	  subpop->insertItem( "&Run only this", ( k << 10 ) | ( (j+1) << 3 ) | 1 );
	}
	else {
	  subpop->insertItem( "&Run", ( k << 10 ) | ( (j+1) << 3 ) | 1 );
	}
	if ( MCs[k]->Commands[j]->RP != 0 ) {
	  subpop->insertItem( "&Options...", ( k << 10 ) | ( (j+1) << 3 ) | 2 );
	  subpop->insertItem( "&View", ( k << 10 ) | ( (j+1) << 3 ) | 3 );
	  subpop->insertItem( "&Load", ( k << 10 ) | ( (j+1) << 3 ) | 4 );
	  subpop->insertItem( "&Help...", ( k << 10 ) | ( (j+1) << 3 ) | 5 );
	}
	else if ( MCs[k]->Commands[j]->Macro >= 0 &&
		  ! MCs[k]->Commands[j]->Params.empty() ) {
	  subpop->insertItem( "&Options...", ( k << 10 ) | ( (j+1) << 3 ) | 2 );
	}
	subpop->insertItem( MCs[k]->Commands[j]->Enabled ? "&Disable" : "&Enable", ( k << 10 ) | ( (j+1) << 3 ) | 6 );
	connect( subpop, SIGNAL( activated( int ) ),
		 this, SLOT( select( int ) ) );
	
	pop->insertItem( MCs[k]->Commands[j]->Enabled ? EnabledIcon : DisabledIcon,
			 s.c_str(), subpop, ( k << 10 ) | ( (j+1) << 3 ) );
	MCs[k]->Commands[j]->MenuText = s;
	MCs[k]->Commands[j]->Menu = pop;
	MCs[k]->Commands[j]->MenuId = ( k << 10 ) | ( (j+1) << 3 );
	MCs[k]->Commands[j]->SubMenu = subpop;

	if ( n > 20 ) {
	  QPopupMenu *nextpop = new QPopupMenu( pop );
	  pop->insertSeparator();
	  pop->insertItem( "More...", nextpop );
	  connect( pop, SIGNAL( activated( int ) ),
		   this, SLOT( select( int ) ) );
	  pop = nextpop;
	  nextpop = 0;
	  n = 0;
	}
      }
      connect( pop, SIGNAL( activated( int ) ),
	       this, SLOT( select( int ) ) );
      string mt = "&";
      if ( k == 0 )
	mt += '0';
      else if ( k < 10 )
	mt += ( '1' + k - 1 );
      else
	mt += ( 'a' + k - 10 );
      mt += " ";
      mt += MCs[k]->Name;
      Menu->insertItem( mt.c_str(), firstpop );
    }
  }

  return Menu;
}


void Macros::select( int id )
{
  int action = id & 7;
  int command = ( id & 1023 ) >> 3;
  int macro = id >> 10;

  if ( command == 0 ) {
    if ( action == 1 ) {
      MCs[macro]->dialog( this );
    }
    else {
      if ( MCs[macro]->KeyCode == Key_Escape &&
	   qApp->focusWidget() != topLevelWidget() ) {
	topLevelWidget()->setFocus();
      }
      else
	launch( macro );
    }
  }
  else {
    command--;
    MacroCommand *mc = MCs[macro]->Commands[command];
    if ( action == 6 ) {
      mc->Enabled = !mc->Enabled;
      mc->SubMenu->changeItem( id, mc->Enabled ? "&Disable" : "&Enable" );
      mc->Menu->changeItem( mc->MenuId, mc->Enabled ? EnabledIcon : DisabledIcon, mc->MenuText.c_str() );
    }
    else if ( action == 5 ) {
      RP->help( mc->RP );
    }
    else if ( action == 4 ) {
      RP->reload( mc->RP );
    }
    else if ( action == 3 ) {
      RP->raise( mc->RP );
    }
    else if ( action == 2 ) {
      mc->dialog( macro, command, RP->dialogOptions() );
    }
    else if ( action <= 1 ) {
      if ( macro >= 0 && macro < (int)MCs.size() )
	MCs[macro]->Variables.setDefaults();
      Enable = true;
      startMacro( macro, command );
      if ( action == 1 ) {
	if ( mc->Macro >= 0 )
	  ThisMacroOnly = true;
	else
	  ThisCommandOnly = true;
      }
    }
  }
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
      else if ( MCs[CurrentMacro]->Commands[CurrentCommand]->Filter ) {
	Filter *filter = RW->FD->filter( MCs[CurrentMacro]->Commands[CurrentCommand]->Name );
	if ( filter != 0 ) {
	  RW->printlog( "filter \"" + filter->ident() + ": " + 
			MCs[CurrentMacro]->Commands[CurrentCommand]->Params + "\"" );
	  filter->save( MCs[CurrentMacro]->Commands[CurrentCommand]->Params );
	}
      }
      // detector:
      else if ( MCs[CurrentMacro]->Commands[CurrentCommand]->Detector ) {
	Filter *filter = RW->FD->detector( MCs[CurrentMacro]->Commands[CurrentCommand]->Name );
	if ( filter != 0 ) {
	  RW->printlog( "detector \"" + filter->ident() + ": " + 
			MCs[CurrentMacro]->Commands[CurrentCommand]->Params + "\"" );
	  filter->save( MCs[CurrentMacro]->Commands[CurrentCommand]->Params );
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
	  hb->mimeSourceFactory()->setFilePath( file.dir().c_str() );
	  hb->setSource( file.notdir().c_str() );
	  if ( hb->mimeSourceFactory()->data( file.notdir().c_str() ) == 0 ) {
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
      MCs[CurrentMacro]->PushButton->setIconSet( SessionIcon );
    else
      MCs[CurrentMacro]->PushButton->setIconSet( IdleIcon );
  }
}


void Macros::runButton( void )
{
  if ( CurrentMacro >= 0 && CurrentMacro < (int)MCs.size() &&
       MCs[CurrentMacro]->PushButton != 0 )
    MCs[CurrentMacro]->PushButton->setIconSet( RunningIcon );
}


void Macros::stackButton( void )
{
  if ( CurrentMacro >= 0 && CurrentMacro < (int)MCs.size() &&
       MCs[CurrentMacro]->PushButton != 0 ) {
    if ( Stack.empty() )
      MCs[CurrentMacro]->PushButton->setIconSet( BaseIcon );
    else
      MCs[CurrentMacro]->PushButton->setIconSet( StackIcon );
  }
}


void Macros::stackButtons( void )
{
  for ( int k = 0; k < (int)Stack.size(); k++ ) {
    int macro = Stack[k].MacroID;
    if ( macro >= 0 && macro < (int)MCs.size() &&
	 MCs[macro]->PushButton != 0 ) {
      if ( k==0 )
	MCs[macro]->PushButton->setIconSet( BaseIcon );
      else
	MCs[macro]->PushButton->setIconSet( StackIcon );
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
	MCs[macro]->PushButton->setIconSet( SessionIcon );
      else
	MCs[macro]->PushButton->setIconSet( IdleIcon );
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
  buttons();
  menu();

  if ( boolean( "fallbackonreload" ) ) {
    ResumePos.clear();
    ResumeStack.clear();
    fallBack();
  }
}


void Macros::selectMacros( void )
{
  QFileDialog* fd = new QFileDialog( 0, "file dialog", TRUE );
  fd->setCaption( "Open Macro File" );
  fd->setMode( QFileDialog::ExistingFile );
  //  fd->setDir( Str( (*OP).text( 0 ) ).dir().c_str() );
  QString types( "Macro files (m*.cfg);;"
		 "Config files (*.cfg);;"
		 "Any files (*)" );
  fd->setFilters( types );
  fd->setViewMode( QFileDialog::List );
  if ( fd->exec() == QDialog::Accepted ) {
    QString filename = fd->selectedFile();
    loadMacros( filename.latin1() );
  }
}


void Macros::switchMacro( int id )
{
  if ( id < 0 || id >= Options::size( "file" ) )
    return;

  loadMacros( text( "file", id ) );
}


void Macros::reload( void )
{
  loadMacros( MacroFile );
}


void Macros::reloadRePro( const string &name )
{
  // find RePro:
  RePro *repro = RP->nameRepro( name );
  if ( repro == 0 ) {
    RW->printlog( "! warning: Macros::reloadRePro() -> RePro " +
		  name + " not found!" );
  }

  for ( MacrosType::iterator mp = MCs.begin();
	mp != MCs.end(); ++mp )
    for ( vector<MacroCommand*>::iterator cp = (*mp)->Commands.begin(); 
	  cp != (*mp)->Commands.end();
	  ++cp )
      if ( (*cp)->Macro < 0 && !(*cp)->Filter && !(*cp)->Detector &&
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
    Menu->setItemEnabled( 3, true );
    Menu->setItemEnabled( 4, true );
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
    Menu->setItemEnabled( 3, false );
    Menu->setItemEnabled( 4, false );
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
    Menu->setItemEnabled( 3, false );
    Menu->setItemEnabled( 4, false );
  }
}


void Macros::launch( int number )
{
  if ( number == FallBackIndex ) {
    store();
  }
  if ( number >= 0 && number < (int)MCs.size() )
    MCs[number]->Variables.setDefaults();
  startMacro( number );
}


void Macros::popup( int number )
{
  QPoint p = MCs[number]->PushButton->mapToGlobal( QPoint( 0, -30 ) );
  MCs[number]->PMenu->popup( p, MCs[number]->PMenu->count()-1 );
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
    for ( int k=0; k<Options::size( "file" ); k++ )
      SwitchMenu->insertItem( text( "file", k ).c_str(), k );
  }
  ConfigClass::saveConfig( str );
}


ostream &operator<< ( ostream &str, const Macros &macros )
{
  for ( unsigned int k=0; k<macros.MCs.size(); k++ ) {
    str << "Macro " << k+1 << ": " << macros.MCs[k]->Name 
	<< ( macros.MCs[k]->startUp() ? " startup" : "" )
	<< ( macros.MCs[k]->shutDown() ? " shutdown" : "" )
	<< ( macros.MCs[k]->fallBack() ? " fallback" : "" ) 
	<< ( macros.MCs[k]->startSession() ? " startsession" : "" )
	<< ( macros.MCs[k]->stopSession() ? " stopsession" : "" )
	<< ( macros.MCs[k]->Button ? "" : " nobutton" )
	<< ( macros.MCs[k]->Menu ? "" : " nomenu" );
    if ( macros.MCs[k]->AccelId >= 0 )
      str << " accel: " << macros.MCs[k]->AccelId;
    str << " -> " << macros.MCs[k]->Variables.save() << '\n';
    for ( unsigned int j=0; j<macros.MCs[k]->Commands.size(); j++ ) {
      str << "  " << j+1 << " ";
      if ( macros.MCs[k]->Commands[j]->Macro >= 0 )
	str << "Macro";
      else if ( macros.MCs[k]->Commands[j]->Shell )
	str << "Shell";
      else if ( macros.MCs[k]->Commands[j]->Filter )
	str << "Filter";
      else if ( macros.MCs[k]->Commands[j]->Detector )
	str << "Detector";
      else if ( macros.MCs[k]->Commands[j]->Switch )
	str << "Switch";
      else if ( macros.MCs[k]->Commands[j]->StartSession )
	str << "StartSession";
      else if ( macros.MCs[k]->Commands[j]->Message ) {
	str << "Message";
	if ( macros.MCs[k]->Commands[j]->TimeOut > 0.0 )
	  str << " (timeout " << macros.MCs[k]->Commands[j]->TimeOut << ")";
      }
      else if ( macros.MCs[k]->Commands[j]->Browse )
	str << "Browse";
      else
	str << "RePro";
      str << ": " << macros.MCs[k]->Commands[j]->Name 
	  << " -> " << macros.MCs[k]->Commands[j]->Params << '\n';
    }
  }
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


Macro::Macro( Str name, Macros *mc ) 
  : Action( 0 ), Button( true ), Menu( true ), Key( true ), 
    Keep( false ), Overwrite( false ),
    PushButton( 0 ), AccelId( -1 ), PMenu( 0 ), PMenuId( -1 ), MacroNum( -1 ),
    MC( mc ), Commands(), DialogOpen( false )
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


void Macro::dialog( Macros *macros )
{
  if ( DialogOpen )
    return;

  DialogOpen = true;
  // create and exec dialog:
  OptDialog *od = new OptDialog( false, macros );
  od->setCaption( "Macro " + Name + " Variables" );
  if ( ! Variables.empty() )
    od->addOptions( Variables );
  od->addOptions( Project );
  od->setSpacing( int(9.0*exp(-double(Variables.size())/14.0))+1 );
  od->setMargin( 10 );
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


void Macro::acceptDialog( void )
{
  Variables.setToDefaults();
  Project.setToDefaults();
  // update menu:
  string s = menuStr();
  PMenu->changeItem( PMenuId, s.c_str() );
}


void Macro::dialogAction( int r )
{
  if ( r == 2 ) {
    MC->startMacro( MacroNum );
  }
}


void Macro::dialogClosed( int r )
{
  DialogOpen = false;
}


MacroCommand::MacroCommand( void ) 
  : Name( "" ),
    Params( "" ),
    RP( 0 ),
    CO(),
    PO(),
    DO( 0 ),
    Macro( -1 ),
    Filter( false ),
    Detector( false ),
    Switch( false ),
    StartSession( false ),
    Shell( false ),
    Message( false ),
    TimeOut( 0.0 ),
    Browse( false ),
    Enabled( true ),
    MacroNum( 0 ),
    CommandNum( 0 ),
    MC( 0 ),
    DialogOpen( false ),
    MacroVars(),
    MacroProject(),
    MenuShortcut( "" ),
    MenuText( "" ),
    Menu( 0 ),
    MenuId( -1 ),
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
    Filter( filter ),
    Detector( detector ),
    Switch( switchm ),
    StartSession( startsession ),
    Shell( shell ),
    Message( mes ),
    TimeOut( to ),
    Browse( browse ),
    Enabled( enabled ),
    MacroNum( 0 ),
    CommandNum( 0 ),
    MC( mc ),
    DialogOpen( false ),
    MacroVars(),
    MacroProject(),
    MenuShortcut( "" ),
    MenuText( "" ),
    Menu( 0 ),
    MenuId( -1 ),
    SubMenu( 0 )
{
}


MacroCommand::MacroCommand( RePro *repro, const string &params, Macros *mc )
  : Name( repro->name() ),
    Params( params ),
    RP( repro ),
    CO(), 
    PO(), 
    DO( 0 ),
    Macro( -1 ),
    Filter( false ),
    Detector( false ),
    Switch( false ),
    StartSession( false ),
    Shell( false ),
    Message( false ),
    TimeOut( 0.0 ),
    Browse( false ),
    Enabled( true ),
    MacroNum( 0 ),
    CommandNum( 0 ),
    MC( mc ),
    DialogOpen( false ),
    MacroVars(),
    MacroProject(),
    MenuShortcut( "" ),
    MenuText( "" ),
    Menu( 0 ),
    MenuId( -1 ),
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
    Switch( com.Switch ), 
    StartSession( com.StartSession ), 
    Shell( com.Shell ), 
    Message( com.Message ),
    TimeOut( com.TimeOut ),
    Browse( com.Browse ),
    Enabled( com.Enabled ),
    MacroNum( com.MacroNum ),
    CommandNum( com.CommandNum ),
    MC( com.MC ),
    DialogOpen( com.DialogOpen ),
    MacroVars( com. MacroVars ),
    MacroProject( com.MacroProject ),
    MenuShortcut( com.MenuShortcut ),
    MenuText( com.MenuText ),
    Menu( com.Menu ),
    MenuId( com.MenuId ),
    SubMenu( com.SubMenu )
{
}


void MacroCommand::dialog( int macro, int command,
			   Options &dopt )
{
  if ( DialogOpen || ( RP != 0 && RP->dialogOpen() ) )
    return;

  DialogOpen = true;
  MacroNum = macro;
  CommandNum = command;
  DO = &dopt;

  if ( Macro >= 0 ) {
    // Macro dialog:
    Options prjopt;
    MacroVars = MC->variables( Macro );
    MacroVars.setDefaults();
    MacroVars.read( MC->expandParams( MacroNum, Params, prjopt ) );
    MacroProject = MC->project( Macro );
    MacroProject.read( prjopt );
    // create and exec dialog:
    OptDialog *od = new OptDialog( false, MC );
    od->setCaption( "Macro " + Name + " Variables" );
    od->addOptions( MacroVars );
    od->addOptions( MacroProject );
    od->setSpacing( int(9.0*exp(-double(MacroVars.size())/14.0))+1 );
    od->setMargin( 10 );
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
    RP->Options::read( MC->expandParams( MacroNum, Params, prjopt ), RePro::MacroFlag );
    RP->Options::read( RP->overwriteOptions(), 0, RePro::OverwriteFlag );
    RP->Options::read( CO, 0, RePro::CurrentFlag );
    RP->projectOptions().read( prjopt );
    RP->projectOptions().read( PO );
    
    RP->dialog();
    
    connect( RP, SIGNAL( dialogAccepted( void ) ),
	     this, SLOT( acceptDialog( void ) ) );
    connect( RP, SIGNAL( dialogAction( int ) ),
	     this, SLOT( dialogAction( int ) ) );
    connect( RP, SIGNAL( dialogClosed( int ) ),
	     this, SLOT( dialogClosed( int ) ) );
  }
}


void MacroCommand::acceptDialog( void )
{
  if ( Macro >= 0 ) {
    Options prjopt;
    Options po( MC->expandParams( MacroNum, Params, prjopt ) );
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
    MenuText = s;
    Menu->changeItem( MenuId, s.c_str() );
  }
  else {
    Options newopt( *((Options*)RP), OptDialog::changedFlag() );
    if ( DO->boolean( "overwrite" ) ) {
      RP->overwriteOptions().readAppend( newopt );
    }
    if ( DO->boolean( "default" ) ) {
      Options prjopt;
      RP->Options::setToDefaults();
      RP->Options::read( MC->expandParams( MacroNum, Params, prjopt ) );
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
  if ( r == 2 ) {
    MC->startMacro( MacroNum, CommandNum );
  }
  // defaults:
  if ( r == 3 ) {
    CO.clear();
  }
}


void MacroCommand::dialogClosed( int r )
{
  DialogOpen = false;
  disconnect( RP, SIGNAL( dialogAccepted( void ) ),
	      this, SLOT( acceptDialog( void ) ) );
  disconnect( RP, SIGNAL( dialogAction( int ) ),
	      this, SLOT( dialogAction( int ) ) );
  disconnect( RP, SIGNAL( dialogClosed( int ) ),
	      this, SLOT( dialogClosed( int ) ) );
}


MacroButton::MacroButton( int number, const string &title,
			  QWidget *parent, const char *name )
  : QPushButton( title.c_str(), parent, name ), Number( number )
{
  connect( this, SIGNAL( clicked() ), this, SLOT( wasLeftClicked() ) );
}


void MacroButton::mouseReleaseEvent( QMouseEvent *qme )
{
  QPushButton::mouseReleaseEvent( qme );
  if ( qme->button() == RightButton ) {
    emit rightClicked( Number );
  }
}


void MacroButton::wasLeftClicked( void )
{
  emit leftClicked( Number );
}


void MacroButton::wasRightClicked( void )
{
  emit rightClicked( Number );
}


}; /* namespace relacs */

#include "moc_macros.cc"

