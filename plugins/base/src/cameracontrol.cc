/*
  base/cameracontrol.cc
  Camera control plugin

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

#include <relacs/base/cameracontrol.h>
using namespace relacs;

namespace base {


CameraControl::CameraControl( void )
  : Control( "CameraControl", "base", "Fabian Sinz", "1.0", "Mai 29, 2012" )
{
  // add some options:
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
 
 //  camera object
  Cam = 0;
  


  // layout:
  QVBoxLayout *vb = new QVBoxLayout;
  // QHBoxLayout *bb;

  setLayout( vb );
  vb->setSpacing( 4 );
  SW.setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
  vb->addWidget( &SW );


}

void CameraControl::initDevices( void )
{
  for ( unsigned int k=0; k<10; k++ ) {
     Str ns( k+1, 0 );
     //Rob = dynamic_cast< Manipulator* >( device( "robot-" + ns ) );
     Cam = dynamic_cast< ::misc::USBCamera* >( device( "camera-" + ns ) );
     if ( Cam != 0 ){
       cerr << "USB Camera initizalized" << endl;
     }

  }
  
}

void CameraControl::main( void )
{
  // get options:
  // double duration = number( "duration" );
}


addControl( CameraControl, base );

}; /* namespace base */

#include "moc_cameracontrol.cc"
