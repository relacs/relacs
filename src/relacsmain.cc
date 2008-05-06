/*
  relacsmain.cc
  

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#include <string>
#include <getopt.h>
#include <qdatetime.h>
#include <qapplication.h>
#include <relacs/relacswidget.h>

using namespace std;


int main( int argc, char **argv )
{
  int mode = relacs::RELACSWidget::AcquisitionMode;
  bool fullscreen = false;
  string configbase = "relacs";

  static struct option longoptions[] = {
    { "version", 0, 0, 0 },
    { "help", 0, 0, 0 }
  };
  optind = 0;
  opterr = 0;
  int longindex = 0;
  char c;
  while ( (c = getopt_long( argc, argv, "f3s:", longoptions, &longindex )) >= 0 ) {
    switch ( c ) {
    case 0: switch ( longindex ) {
      case 0:
	cout << "RELACS " << RELACSVERSION << endl;
	cout << "Copyright (C) 2008 Jan Benda\n";
	exit( 0 );
	break;
      case 1:
	cout << "relacsmain should not be called directly\n"
	     << "Use 'relacs' instead!\n";
	exit( 0 );
	break;
      }
      break;

    case 'f':
      fullscreen = true;
      break;

    case '3':
      mode = relacs::RELACSWidget::SimulationMode;
      break;

    case 's':
      configbase = optarg;
      break;
      
      break;
    }
  }

  QApplication::setColorSpec( QApplication::CustomColor );
  QApplication a( argc, argv );

  relacs::RELACSWidget relacs( configbase, mode );

  if ( fullscreen )
    relacs.fullScreen();

  relacs.show();
  relacs.init();

  a.setMainWidget( &relacs );

  return a.exec();
}
