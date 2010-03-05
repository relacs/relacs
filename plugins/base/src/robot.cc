/*
  base/robot.cc
  Control a robot

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

#include <relacs/base/robot.h>
using namespace relacs;

namespace base {


Robot::Robot( void )
  : Control( "Robot", "Robot", "base",
	     "Jan Benda", "1.0", "Mar 05, 2010" )
{
  Rob = 0;
}


void Robot::initDevices( void )
{
  for ( unsigned int k=0; k<10; k++ ) {
    Str ns( k+1, 0 );
    Rob = dynamic_cast< Manipulator* >( device( "robot-" + ns ) );
    if ( Rob != 0 )
      break;
  }
}


void Robot::keyPressEvent( QKeyEvent *e )
{
  e->accept();

  switch ( e->key() ) {

  case Key_Up:
    if ( Rob != 0 )
      Rob->stepX( -100.0 );
    break;

  case Key_Down:
    if ( Rob != 0 )
      Rob->stepX( 100.0 );
    break;

  case Key_Left:
    if ( Rob != 0 )
      Rob->stepY( 100.0 );
    break;

  case Key_Right:
    if ( Rob != 0 )
      Rob->stepY( -100.0 );
    break;

  case Key_PageUp:
    if ( Rob != 0 )
      Rob->stepZ( 100.0 );
    break;

  case Key_PageDown:
    if ( Rob != 0 )
      Rob->stepZ( -100.0 );
    break;

  default:
    Control::keyPressEvent( e );

  }
}


addControl( Robot );

}; /* namespace base */

#include "moc_robot.cc"
