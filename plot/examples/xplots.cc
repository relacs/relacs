/*
  xplots.cc
  

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

#include <qgroupbox.h>
#include <qapplication.h>
#include <relacs/multiplot.h>
using namespace relacs;


int main( int argc, char **argv )
{
  QApplication a( argc, argv );

  QGroupBox g( 1, Qt::Horizontal, "My Plots" );
  a.setMainWidget( &g );
  MultiPlot p( 3, &g );

  const int n = 19;
  vector< double > xd( n ), yd( n );
  for ( int k = 0; k<n; k++ )
    {
      xd[k] = (k-n/2)*1.4;
      yd[k] = xd[k]*xd[k]*0.1 - 8.0;
    }

  p[0].setXTics();
  p[0].setYTics();
  p[0].setXLabel( "xlabel" );
  p[0].setXLabelPos( 1.0, Plot::Graph, -1.0, Plot::FirstAxis, Plot::Right, 0.0 );
  p[0].setYLabel( "ylabel" );
  p[0].setYLabelPos( -0.6, Plot::FirstAxis, 0.5, Plot::Graph, Plot::Center, -90.0 );
  p[0].setTitle( "title" );
  p[0].setLabel( "label", 0.5, Plot::Graph, 0.5, Plot::Graph, Plot::Center, 45.0 );
  p[0].plotVLine( -2.0, Plot::Green, 4, Plot::Solid );
  p[0].plot( xd, yd, Plot::Red, 2, Plot::Solid, 
	  Plot::TriangleUpDot, 9, Plot::Red, Plot::Black );
  p[0].plotHLine( -4.0, Plot::Blue, 2, Plot::Solid );
  p[0].plotLine( -2.0, -4.0, 0, 0 );
  p[0].setOrigin( 0.0, 0.67 );
  p[0].setSize( 1.0, 0.33 );

  p[1].setXRange( -10.0, 0.0 );
  p[1].plot( xd, yd, Plot::Red, 2, Plot::Solid, 
	  Plot::Circle, 9, Plot::Red, Plot::Blue );
  p[1].setOrigin( 0.0, 0.33 );
  p[1].setSize( 1.0, 0.33 );

  p[2].setXRange( 0.0, 10.0 );
  p[2].plot( xd, yd, Plot::Red, 2, Plot::Solid, 
	  Plot::DiamondDot, 9, Plot::Green, Plot::Black );
  p[2].setOrigin( 0.0, 0.0 );
  p[2].setSize( 1.0, 0.33 );

  g.show();

  return a.exec();
}
