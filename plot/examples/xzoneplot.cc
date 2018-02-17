/*
  xzoneplot.cc
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

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

#include <QApplication>
#include <relacs/transform.h>
#include <xzoneplot.h>
using namespace relacs;

PlotShapes::PlotShapes( int n )
  : MultiPlot( n, 2 )
{
  A = 0.0;
  B = 0.0;
  C = 0.0;
  X = 0.0;

  Point lightsource( -0.5, -1.0, 1.0 );

  double range = 5.0;

  setMinimumSize( QSize(1500, 800) );
  (*this)[0].setXLabel( "x-coord" );
  (*this)[0].setXLabelPos( 1.0, Plot::Graph, -1.0, Plot::FirstAxis, Plot::Right, 0.0 );
  (*this)[0].setXRange( -range, range );
  (*this)[0].setXTics( 1.0 );
  (*this)[0].setYLabel( "y-coord" );
  (*this)[0].setYLabelPos( -0.6, Plot::FirstAxis, 0.5, Plot::Graph, Plot::Center, -90.0 );
  (*this)[0].setYRange( -range, range );
  (*this)[0].setYTics( 1.0 );
  (*this)[0].noGrid();
  (*this)[0].setLightSource( lightsource, 0.5 );

  (*this)[1].setXLabel( "x-coord" );
  (*this)[1].setXLabelPos( 1.0, Plot::Graph, -1.0, Plot::FirstAxis, Plot::Right, 0.0 );
  (*this)[1].setXRange( -range, range );
  (*this)[1].setXTics( 1.0 );
  (*this)[1].setYLabel( "z-coord" );
  (*this)[1].setYLabelPos( -0.6, Plot::FirstAxis, 0.5, Plot::Graph, Plot::Center, -90.0 );
  (*this)[1].setYRange( -range, range );
  (*this)[1].setYTics( 1.0 );
  (*this)[1].noGrid();
  (*this)[1].setLightSource( lightsource, 0.7 );

  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(30);
}

void PlotShapes::update( void )
{
  A += 0.005 * 2.0 * M_PI;
  B += 0.0015 * 2.0 * M_PI;
  C += 0.0005 * 2.0 * M_PI;

  Zone z( "zone" );

  Cuboid cbd1( "cbd1" );
  cbd1.scale( 2.0, 1.0, 1.5 );
  z.add( cbd1 );

  Cuboid cbd2( "cbd2" );
  cbd2.translate( 1.5, 0.5, 1.0 );
  z.add( cbd2 );

  Cuboid cbd3( "cbd3" );
  cbd3.translate( -0.5, -0.5, 1.0 );
  z.subtract( cbd3 );

  Sphere sphr1( "sphr1" );
  sphr1.translateX( 4.0 );
  z.add( sphr1 );

  Sphere sphr2( "sphr2" );
  sphr2.translate( 4.0, 1.0, 0.0 );
  z.add( sphr2 );

  Sphere sphr3( "sphr3" );
  sphr3.translate( 4.0, -1.0, 0.0 );
  z.subtract( sphr3 );

  z.rotateZ( A );
  z.rotateY( B );
  z.rotateX( C );

  Point viewxz = -Point::UnitY*20.0;

  (*this)[0].clearPolygons();
  (*this)[0].setViewPoint( viewxz );
  (*this)[0].plot( z, 30, Plot::Red, 0.8, Plot::Red, 1 );

  (*this)[1].clearPolygons();
  (*this)[1].setViewPoint( viewxz );
  (*this)[1].plot( z, 60, Plot::Orange );
  draw();
}


int main( int argc, char **argv )
{
  QApplication a( argc, argv );

  PlotShapes p( 2 );
  p.show();

  return a.exec();
}



#include "moc_xzoneplot.cc"
