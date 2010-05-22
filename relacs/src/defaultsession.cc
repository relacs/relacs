/*
  defaultsession.cc
  Minimum default Session

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

#include <relacs/defaultsession.h>

namespace relacs {


DefaultSession::DefaultSession( void )
  : Control( "DefaultSession", "Info", "Jan Benda", "1.0" )

{
  SessionButton = new QPushButton( "Start", this );
  SessionButton->setMinimumSize( SessionButton->sizeHint() );
  connect( SessionButton, SIGNAL( clicked() ),
	   this, SLOT( toggleSession() ) );
}


DefaultSession::~DefaultSession( void )
{
}


void DefaultSession::sessionStarted( void )
{
  SessionButton->setText( "Stop" );
}


void DefaultSession::sessionStopped( bool saved )
{
  SessionButton->setText( "Start" );
}


}; /* namespace relacs */

#include "moc_defaultsession.cc"

