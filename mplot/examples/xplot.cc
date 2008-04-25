/*
  xplot.cc
  

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

#include <qgroupbox.h>
#include <qapplication.h>
#include <relacs/map.h>
#include <relacs/plot.h>
#include <cstdio>


int main( int argc, char **argv )
{
  QApplication a( argc, argv );

  QGroupBox g( 1, Qt::Horizontal, "My Plot" );
  a.setMainWidget( &g );
  Plot p( &g );

  const int n = 19;
  vector< double > xd( n ), yd( n );
  //  vector< float > zd( n );
  MapD zd( n );
  for ( int k = 0; k<n; k++ )
    {
      xd[k] = (k-n/2)*1.4;
      yd[k] = xd[k]*xd[k]*0.1 - 2.0;
      zd.x( k ) = xd[k];
      zd[k] = 3.0*sin( 6.0*20.0*xd[k] );
    }

  //  p.setRange( -6.5, 6.5, -10.5, 0.5 );
  //  p.setAutoScaleXY();
  //  p.setRMarg( 3.0 );
  //  p.setBMarg( 2.0 );
  p.setXTics();
  p.setYTics();
  p.setX2Tics();
  p.setY2Tics();
  p.setXLabel( "xlabel" );
  p.setXLabelPos( 1.0, Plot::Graph, -1.0, Plot::FirstAxis, Plot::Right, 0.0 );
  //  p.setXRange( -6.0, 6.0 );
  p.setYLabel( "ylabel" );
  p.setYLabelPos( -0.6, Plot::FirstAxis, 0.5, Plot::Graph, Plot::Center, -90.0 );
  //  p.setYRange( -4.0, 4.0 );
  p.setX2Label( "x2label" );
  p.setY2Label( "y2label" );
  p.setY2Range( Plot::AutoScale, Plot::AutoMinScale );
  p.setTitle( "title" );
  p.setTitlePos( 0.0, Plot::Graph, 0.0, Plot::SecondAxis, Plot::Left, 0.0 );
  p.setLabel( "label 1", 0.3, Plot::Graph, 0.3, Plot::Graph, Plot::Center, -45.0,
	      Plot::White, 1.0, Plot::DefaultF, Plot::Blue, 2, Plot::Red );
  p.setLabel( "label 2", 2.0, Plot::First, 2.0, Plot::First, Plot::Center, 45.0 );
  p.plotVLine( -2.0, Plot::Green, 4, Plot::Solid );
  p.plot( xd, yd, Plot::Red, 2, Plot::Solid, 
	  Plot::TriangleUpDot, 9, Plot::Red, Plot::Black );
  p.plot( zd, 1.0, Plot::Orange, 2, Plot::LongDash, 
	  Plot::Diamond, 9, Plot::Orange, Plot::Orange );
  p.back().setAxis( 1, 1 );
  p.plotHLine( -2.0, Plot::LineStyle( Plot::Blue, 2, Plot::Solid ) );
  p.plotLine( -2.0, -4.0, 0, 0 );

  g.show();

  return a.exec();
}
