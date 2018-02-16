/*
  xshapeplot.cc
  

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
#include <xshapeplot.h>
using namespace relacs;

PlotShapes::PlotShapes( int n )
  : MultiPlot( n, 2 )
{
  A = 0.0;
  B = 0.0;
  C = 0.0;
  X = 0.0;

  Point lightsource( -0.5, -1.0, 1.0 );

  setMinimumSize( QSize(1500, 800) );
  (*this)[0].setXLabel( "x-coord" );
  (*this)[0].setXLabelPos( 1.0, Plot::Graph, -1.0, Plot::FirstAxis, Plot::Right, 0.0 );
  (*this)[0].setXRange( -4.0, 4.0 );
  (*this)[0].setXTics( 1.0 );
  (*this)[0].setYLabel( "y-coord" );
  (*this)[0].setYLabelPos( -0.6, Plot::FirstAxis, 0.5, Plot::Graph, Plot::Center, -90.0 );
  (*this)[0].setYRange( -4.0, 4.0 );
  (*this)[0].setYTics( 1.0 );
  (*this)[0].noGrid();
  (*this)[0].setLightSource( lightsource, 0.5 );

  (*this)[1].setXLabel( "x-coord" );
  (*this)[1].setXLabelPos( 1.0, Plot::Graph, -1.0, Plot::FirstAxis, Plot::Right, 0.0 );
  (*this)[1].setXRange( -4.0, 4.0 );
  (*this)[1].setXTics( 1.0 );
  (*this)[1].setYLabel( "z-coord" );
  (*this)[1].setYLabelPos( -0.6, Plot::FirstAxis, 0.5, Plot::Graph, Plot::Center, -90.0 );
  (*this)[1].setYRange( -4.0, 4.0 );
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
  X += 0.001 * 2.0 * M_PI;

  Zone z;

  Cuboid cbd1;
  cbd1.translate( 0.0, -0.5, -0.5 );
  cbd1.scaleX( 2.0 );
  cbd1.scaleZ( 0.5 );
  z.add( cbd1 );

  Cuboid cbd2;
  cbd2.scaleX( 0.5 );
  cbd2.translate( -0.5, -0.5, -0.5 );
  cbd2.scaleY( 1.6 );
  cbd2.scaleZ( 1.6 );
  z.add( cbd2 );

  Cuboid cbd3;
  cbd3.scaleX( 0.5 );
  cbd3.translate( -0.6, -0.5, -0.5 );
  z.subtract( cbd3 );

  Cylinder clnd;
  clnd.scale( 0.5, 1.0, 1.0 );
  clnd.translateX( 2.0 );
  z.add( clnd );

  Sphere sphr0;
  sphr0.scale( 0.5 );
  sphr0.translateX( 2.5 );
  //  sphr0.translateY( 1 );
  //  z.subtract( sphr0 );

  Sphere sphr1;
  sphr1.scale( 0.5 );
  sphr1.translateY( 1.5 );
  z.add( sphr1 );

  Sphere sphr2;
  sphr2.scale( 0.5, 1.0, 0.5 );
  //  sphr2.scale( 0.5 );
  sphr2.translateZ( 1.5 );
  z.add( sphr2 );

  //  z.scale( 0.5 );
  z.rotateZ( A );
  z.rotateY( B );
  z.rotateX( C );
  z.translateX( 2.0 * ::sin(X) );

  Point viewxy = Point::UnitZ*20.0;
  Point viewxz = -Point::UnitY*20.0;

  (*this)[0].clearPolygons();
  (*this)[0].setViewPoint( viewxy );
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



#include "moc_xshapeplot.cc"
