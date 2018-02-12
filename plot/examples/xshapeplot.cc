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

  setMinimumSize( QSize(1400, 800) );
  (*this)[0].setXLabel( "x-coord" );
  (*this)[0].setXLabelPos( 1.0, Plot::Graph, -1.0, Plot::FirstAxis, Plot::Right, 0.0 );
  (*this)[0].setXRange( -4.0, 4.0 );
  (*this)[0].setXTics( 1.0 );
  (*this)[0].setYLabel( "y-coord" );
  (*this)[0].setYLabelPos( -0.6, Plot::FirstAxis, 0.5, Plot::Graph, Plot::Center, -90.0 );
  (*this)[0].setYRange( -4.0, 4.0 );
  (*this)[0].setYTics( 1.0 );
  (*this)[0].noGrid();
  Transform projxy = Transform::ProjectXY;
  projxy(3, 2) = 0.2;
  (*this)[0].setProjection( projxy );

  (*this)[1].setXLabel( "x-coord" );
  (*this)[1].setXLabelPos( 1.0, Plot::Graph, -1.0, Plot::FirstAxis, Plot::Right, 0.0 );
  (*this)[1].setXRange( -4.0, 4.0 );
  (*this)[1].setXTics( 1.0 );
  (*this)[1].setYLabel( "z-coord" );
  (*this)[1].setYLabelPos( -0.6, Plot::FirstAxis, 0.5, Plot::Graph, Plot::Center, -90.0 );
  (*this)[1].setYRange( -4.0, 4.0 );
  (*this)[1].setYTics( 1.0 );
  (*this)[1].noGrid();
  Transform projxz = Transform::ProjectXZ;
  projxz(3, 1) = 0.2;
  (*this)[1].setProjection( projxz );

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
  cbd1.rotateZ( A );
  cbd1.rotateY( B );
  cbd1.rotateX( C );
  cbd1.translateX( 2.0 * ::sin(X) );
  z.add( cbd1 );

  Cuboid cbd2;
  cbd2.translate( 0.0, -0.5, -0.5 );
  cbd2.scaleX( -0.5 );
  cbd2.scaleY( 1.6 );
  cbd2.scaleZ( 1.6 );
  cbd2.rotateZ( A );
  cbd2.rotateY( B );
  cbd2.rotateX( C );
  cbd2.translateX( 2.0 * ::sin(X) );
  z.add( cbd2 );

  Cylinder clnd;
  clnd.scale( 0.5, 1.0, 1.0 );
  clnd.translateX( 2.0 );
  clnd.rotateZ( A );
  clnd.rotateY( B );
  clnd.rotateX( C );
  clnd.translateX( 2.0 * ::sin(X) );
  z.add( clnd );

  Sphere sphr1;
  sphr1.scale( 0.5 );
  sphr1.translateY( 1.5 );
  sphr1.rotateZ( A );
  sphr1.rotateY( B );
  sphr1.rotateX( C );
  sphr1.translateX( 2.0 * ::sin(X) );
  z.add( sphr1 );

  Sphere sphr2;
  sphr2.scale( 0.5, 1.0, 0.5 );
  sphr2.translateZ( 1.5 );
  sphr2.rotateZ( A );
  sphr2.rotateY( B );
  sphr2.rotateX( C );
  sphr2.translateX( 2.0 * ::sin(X) );
  z.add( sphr2 );

  (*this)[0].clearPolygons();
  (*this)[0].plot( z, Plot::LineStyle( Plot::Red, 2, Plot::Solid ) );
  (*this)[1].clearPolygons();
  (*this)[1].plot( z, Plot::LineStyle( Plot::Red, 2, Plot::Solid ) );
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
