/*
  xsurfaceplot.cc
  

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

#include <cmath>
#include <QApplication>
#include <relacs/sampledata.h>
#include <relacs/plot.h>
using namespace relacs;


int main( int argc, char **argv )
{
  QApplication a( argc, argv );

  Plot p( 0 );

  // data:
  SampleData< SampleDataD > surface( 0.0, 99.9, 0.1 );
  for ( int r=0; r<surface.size(); r++ ) {
    surface[r] = SampleDataD( -6.0, 5.95, 0.05 );
    for ( int c=0; c<surface[r].size(); c++ )
      //      surface[r][c] = 10.0*(double(r)/double(surface.size())) * (double(c)/double(surface[r].size()));
    surface[r][c] = 5.0+5.0*sin( 2.0*M_PI*0.03*surface.pos( r ) ) * sin( 2.0*M_PI*0.1*surface[r].pos( c ) );
  }

  // plot:
  p.setZRange( 0.0, 10.0 );
  p.setXFallBackRange( 0.0, 10.0 );
  p.setXTics();
  p.setYTics();
  p.setXLabel( "xlabel" );
  p.setXLabelPos( 1.0, Plot::Graph, -1.0, Plot::FirstAxis, Plot::Right, 0.0 );
  p.setYFallBackRange( 0.0, 1.0 );
  p.setYLabel( "ylabel" );
  p.setYLabelPos( -0.6, Plot::FirstAxis, 0.5, Plot::Graph, Plot::Center, -90.0 );
  p.setTitle( "title" );
  p.setTitlePos( 0.0, Plot::Graph, 0.0, Plot::SecondAxis, Plot::Left, 0.0 );
  p.plot( surface, 1.0, Plot::BlueRedYellowWhiteGradient );

  p.show();

  return a.exec();
}
