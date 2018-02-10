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
#include <relacs/matrix.h>
#include <xshapeplot.h>
using namespace relacs;

PlotShapes::PlotShapes( int n )
  : MultiPlot( n )
{
  A = 0.0;
  B = 0.0;
  C = 0.0;

  setMinimumSize( QSize(800, 800) );
  (*this)[0].setXLabel( "x-coord" );
  (*this)[0].setXLabelPos( 1.0, Plot::Graph, -1.0, Plot::FirstAxis, Plot::Right, 0.0 );
  (*this)[0].setXRange( -4.0, 4.0 );
  (*this)[0].setXTics( 1.0 );
  (*this)[0].setYLabel( "y-coord" );
  (*this)[0].setYLabelPos( -0.6, Plot::FirstAxis, 0.5, Plot::Graph, Plot::Center, -90.0 );
  (*this)[0].setYRange( -2.0, 2.0 );
  (*this)[0].setYTics( 1.0 );

  (*this)[1].setXLabel( "x-coord" );
  (*this)[1].setXLabelPos( 1.0, Plot::Graph, -1.0, Plot::FirstAxis, Plot::Right, 0.0 );
  (*this)[1].setXRange( -4.0, 4.0 );
  (*this)[1].setXTics( 1.0 );
  (*this)[1].setYLabel( "z-coord" );
  (*this)[1].setYLabelPos( -0.6, Plot::FirstAxis, 0.5, Plot::Graph, Plot::Center, -90.0 );
  (*this)[1].setYRange( -2.0, 2.0 );
  (*this)[1].setYTics( 1.0 );

  QTimer *timer = new QTimer(this);
  connect(timer, SIGNAL(timeout()), this, SLOT(update()));
  timer->start(50);
}

void PlotShapes::update( void )
{
  Cuboid cbd;
  A += 0.01 * 2.0 * M_PI;
  B += 0.003 * 2.0 * M_PI;
  C += 0.001 * 2.0 * M_PI;
  cbd.scaleX( 2.0 );
  cbd.scaleZ( 0.5 );
  cbd.rotateZ( A );
  cbd.rotateY( B );
  cbd.rotateX( C );

  (*this)[0].clearData();
  (*this)[0].plot( cbd, Matrix::ProjectXY, Plot::LineStyle( Plot::Red, 2, Plot::Solid ) );
  (*this)[1].clearData();
  (*this)[1].plot( cbd, Matrix::ProjectXZ, Plot::LineStyle( Plot::Red, 2, Plot::Solid ) );
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
