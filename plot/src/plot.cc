/*
  plot.cc
  Plotting various data in a single widget.

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

#include <cmath>
#include <iostream>
#include <algorithm>
#include <qbitmap.h>
#include <qpainter.h>
#include <qcursor.h>
#include <qapplication.h>
#include <relacs/str.h>
#include <relacs/multiplot.h>
#include <relacs/plot.h>

namespace relacs {


const double Plot::AutoScale = MAXDOUBLE;
const double Plot::AutoMinScale = 0.1*MAXDOUBLE;
const double Plot::ExactScale = 0.01*MAXDOUBLE;
const double Plot::ExactMinScale = 0.001*MAXDOUBLE;
const double Plot::DynamicScale = 0.1*MAXDOUBLE;
const double Plot::AnyScale = 0.001*MAXDOUBLE;


bool Plot::RGBColor::operator==( const RGBColor &c )
{
  return ( Red == c.Red && Green == c.Green && Blue == c.Blue );
}


Plot::RGBColor Plot::RGBColor::lighten( double f ) const
{
  return RGBColor( int( Red*f ), int( Green*f ), int( Blue*f ) );
}


Plot::Plot( KeepMode keep, QWidget *parent, const char *name )
  : QWidget( parent, name, WNoAutoErase ),
    PMutex( true )
{
  construct( keep );
}


Plot::Plot( QWidget *parent, const char *name )
  : QWidget( parent, name, WNoAutoErase ),
    PMutex( true )
{
  construct( Copy );
}


Plot::Plot( KeepMode keep, bool subwidget, int id,
	    MultiPlot *mp, const char *name )
  : QWidget( 0, name ),
    PMutex( true )
{
  construct( keep, subwidget, id, mp );
}


void Plot::construct( KeepMode keep, bool subwidget, int id, MultiPlot *mp )
{
  Keep = keep;
  SubWidget = subwidget;
  Id = id;
  MP = mp;
  setMouseTracking( false );
  MouseMenu = 0;
  MouseMenuClick = false;
  MouseAction = 2;
  MouseDrawRect = false;
  MouseX1 = 0xffff;
  MouseY1 = 0xffff;
  MouseXMax = false;
  MouseX2 = 0xffff;
  MouseY2 = 0xffff;
  MouseYMax = false;
  MouseGrabbed = false;
  MouseMoveX = false;
  MouseZoomXMin = false;
  MouseZoomXMax = false;
  MouseMoveY = false;
  MouseZoomYMin = false;
  MouseZoomYMax = false;
  MouseMoveXY = false;
  MouseZoomOut = false;
  LastMouseEvent.clear();
  MouseYShrink = 1.0;

  XOrigin = 0.0;
  YOrigin = 0.0;
  XSize = 1.0;
  YSize = 1.0;

  ScreenX1 = 0;
  ScreenY1 = height() - 1;
  ScreenX2 = width() - 1;
  ScreenY2 = 0;

  FontSize = fontMetrics().height();
  FontWidth = fontMetrics().width( "00" ) - fontMetrics().width( "0" );
  FontHeight = fontMetrics().ascent();

  for ( int k=0; k<MaxAxis; k++ ) {
    XMin[k] = -10.0;
    XMax[k] = 10.0;
    YMin[k] = -10.0;
    YMax[k] = 10.0;
    XMinRange[k] = AutoScale;
    XMaxRange[k] = AutoScale;
    YMinRange[k] = AutoScale;
    YMaxRange[k] = AutoScale;
    XMinFB[k] = -10.0;
    XMaxFB[k] = 10.0;
    YMinFB[k] = -10.0;
    YMaxFB[k] = 10.0;
    XMinPrev[k] = -10.0;
    XMaxPrev[k] = 10.0;
    YMinPrev[k] = -10.0;
    YMaxPrev[k] = 10.0;

    XTics[k] = 0;
    YTics[k] = 0;
    XTicsIncr[k] = 2.0;
    YTicsIncr[k] = 2.0;
    XTicsMinIncr[k] = AutoScale;
    YTicsMinIncr[k] = AutoScale;
    XTicsStart[k] = -10.0;
    YTicsStart[k] = -10.0;
    XTicsIncrAutoScale[k] = 2;
    YTicsIncrAutoScale[k] = 2;
    XTicsStartAutoScale[k] = true;
    YTicsStartAutoScale[k] = true;

    XTicsFormat[k] = "%g";
    YTicsFormat[k] = "%g";

    XGrid[k] = false;
    XGridStyle[k] = LineStyle( White, 1, Dotted );
    YGrid[k] = false;
    YGridStyle[k] = LineStyle( White, 1, Dotted ); 
  }

  XTics[0] = 1;
  YTics[0] = 1;

  TicsColor = Black;
  TicsLen = 4;
  TicsPos = Out;
  TicsWidth = 1;
  TicsLabelSize = 1.0;
  TicsLabelFont = DefaultF;
  TicsLabelColor = Black;
  X1TicsLen = 0;
  Y1TicsLen = 0;
  X2TicsLen = 0;
  Y2TicsLen = 0;
  X1TicsMarg = 0;
  Y1TicsMarg = 0;
  X2TicsMarg = 0;
  Y2TicsMarg = 0;

  XGrid[0] = true;
  YGrid[0] = true;

  XLabel[0].setXPos( 1.0, Graph );
  XLabel[0].setYPos( -1.0, FirstAxis );
  XLabel[0].Just = Right;
  XLabel[0].Angle = 0.0;
  XLabel[0].LColor = Black;

  YLabel[0].setXPos( 0.0, FirstMargin );
  YLabel[0].setYPos( 0.0, SecondAxis );
  YLabel[0].Just = Left;
  YLabel[0].Angle = 0.0;
  /*
  YLabel[0].setXPos( 0.0, FirstAxis );
  YLabel[0].setYPos( 0.5, Graph );
  YLabel[0].Just = Center;
  YLabel[0].Angle = -90.0;
  */
  YLabel[0].LColor = Black;

  XLabel[1].setXPos( 1.0, Graph );
  XLabel[1].setYPos( 0.0, SecondAxis );
  XLabel[1].Just = Right;
  XLabel[1].Angle = 0.0;
  XLabel[1].LColor = Black;

  YLabel[1].setXPos( 1.0, SecondAxis );
  YLabel[1].setYPos( 0.5, Graph );
  YLabel[1].Just = Center;
  YLabel[1].Angle = -90.0;
  YLabel[1].LColor = Black;

  Title.setXPos( 1.0, Graph );
  Title.setYPos( 0.0, SecondAxis );
  Title.Just = Right;
  Title.Angle = 0.0;
  Title.LColor = Black;

  DefaultLabel.setXPos( 0.0, First );
  DefaultLabel.setYPos( 0.0, First );
  DefaultLabel.Just = Left;
  DefaultLabel.Angle = 0.0;
  DefaultLabel.LColor = White;

  LMarg = 10;
  RMarg = 10;
  BMarg = 10;
  TMarg = 10;
  LMargAutoScale = true;
  RMargAutoScale = true;
  BMargAutoScale = true;
  TMargAutoScale = true;
  ScreenBorder = 1;

  Border = 15;
  BorderStyle = LineStyle( Black, 0, Solid );
  PlotColor = Black;
  BackgroundColor = WidgetBackground;

  QColor pbc = paletteBackgroundColor();
  addColor( RGBColor( pbc.red(), pbc.green(), pbc.blue() ) );  // WidgetBackground
  addColor( RGBColor( 0, 0, 0 ) );  // Black
  addColor( RGBColor( 127, 127, 127 ) );  // Gray
  addColor( RGBColor( 255, 255, 255 ) );  // White
  addColor( RGBColor( 255, 0, 0 ) );  // Red
  addColor( RGBColor( 0, 255, 0 ) );  // Green
  addColor( RGBColor( 0, 0, 255 ) );  // Blue
  addColor( RGBColor( 255, 255, 0 ) );  // Yellow
  addColor( RGBColor( 255, 0, 255 ) );  // Magenta
  addColor( RGBColor( 0, 255, 255 ) );  // Cyan
  addColor( RGBColor( 255, 165, 0 ) );  // Orange
  addColor( RGBColor( 255, 140, 0 ) );  // DarkOrange
  addColor( RGBColor( 255, 69, 0 ) );  // OrangeRed
  addColor( RGBColor( 255, 215, 0 ) );  // Gold
  addColor( RGBColor( 127, 255, 0 ) );  // Chartreuse
  addColor( RGBColor( 255, 0, 127 ) );  // DeepPink
  addColor( RGBColor( 0, 191, 255 ) );  // DeepSkyBlue
  addColor( RGBColor( 0, 255, 127 ) );  // SpringGreen
  addColor( RGBColor( 0, 100, 0 ) );  // DarkGreen
  addColor( RGBColor( 0, 139, 139 ) );  // DarkCyan
  addColor( RGBColor( 0, 206, 209 ) );  // DarkTurquoise

  QtDash[Solid] = Qt::SolidLine; 
  QtDash[LongDash] = Qt::DashLine; 
  QtDash[ShortDash] = Qt::DashLine; 
  QtDash[WideDotted] = Qt::DotLine; 
  QtDash[Dotted] = Qt::DotLine; 
  QtDash[DashDot] = Qt::DashDotLine; 
  QtDash[DashDotDot] = Qt::DashDotDotLine; 

  setSizePolicy( QSizePolicy( QSizePolicy::Expanding, 
			      QSizePolicy::Expanding ) );

  NewData = true;
  ShiftData = false;
  ShiftXPix = 0;

  if ( SubWidget )
    PixMap = 0;
  else
    PixMap = new QPixmap; 

  DMutex = 0;
}


Plot::~Plot( void )
{
  clear();

  PMutex.lock();
  if ( PixMap != 0 )
    delete PixMap;
  PMutex.unlock();
}


void Plot::lock( void )
{
  PMutex.lock();
}


void Plot::unlock( void )
{
  PMutex.unlock();
}


void Plot::setDataMutex( QMutex *mutex )
{
  if ( DMutex == 0 )
    DMutex = mutex;
}


void Plot::clearDataMutex( void )
{
  DMutex = 0;
}


void Plot::lockData( void )
{
  if ( DMutex != 0 )
    DMutex->lock();
}


void Plot::unlockData( void )
{
  if ( DMutex != 0 )
    DMutex->unlock();
}


void Plot::setOrigin( double x, double y )
{
  XOrigin = x;
  YOrigin = y;
  NewData = true;
  if ( SubWidget && MP != 0 )
    MP->setDrawBackground();
}


void Plot::setSize( double w, double h )
{
  XSize = w;
  YSize = h;
  NewData = true;
  if ( SubWidget && MP != 0 )
    MP->setDrawBackground();
}


void Plot::scale( int width, int height )
{
  if ( ! SubWidget )
    return;

  int x = (int)::rint( width * XOrigin );
  int y = (int)::rint( height * YOrigin );
  int w = (int)::rint( width * XSize );
  int h = (int)::rint( height * YSize );

  if ( ScreenX1 != x || ScreenX2 != x + w || 
       ScreenY1 != height - y || ScreenY2 != height - y - h) {
    ScreenX1 = x;
    ScreenX2 = x + w;
    ScreenY1 = height - y;
    ScreenY2 = height - y - h;
  }
}


bool Plot::inside( int xpixel, int ypixel )
{
  return ( xpixel >= ScreenX1 && xpixel <= ScreenX2 &&
	   ypixel >= ScreenY2 && ypixel <= ScreenY1 );
}


int Plot::addColor( const RGBColor &rgb )
{
  for ( unsigned int k=0; k<Colors.size(); k++ ) {
    if ( Colors[k] == rgb )
      return k;
  }
  Colors.push_back( rgb );
  return Colors.size()-1;
}


int Plot::addColor( int r, int g, int b )
{
  return addColor( RGBColor( r, g, b ) );
}


Plot::RGBColor Plot::color( int c )
{
  if ( c < 0 || c > int( Colors.size() ) )
    c = 0;
  return Colors[c];
}


void Plot::setLMarg( double lmarg )
{
  LMargAutoScale = ( lmarg >= AnyScale );
  LMarg = (int)::rint( lmarg * FontWidth );
}


void Plot::setRMarg( double rmarg )
{
  RMargAutoScale = ( rmarg >= AnyScale );
  RMarg = (int)::rint( rmarg * FontWidth );
}


void Plot::setTMarg( double tmarg )
{
  TMargAutoScale = ( tmarg >= AnyScale );
  TMarg = (int)::rint( tmarg * FontWidth );
}


void Plot::setBMarg( double bmarg )
{
  BMargAutoScale = ( bmarg >= AnyScale );
  BMarg = (int)::rint( bmarg * FontWidth );
}


void Plot::setScreenBorder( int marg )
{
  ScreenBorder = marg > 0 ? marg : 0;
}


void Plot::setXRange( double xmin, double xmax )
{
  if ( xmin <= xmax ||
       xmin >= AnyScale || xmax >= AnyScale ) {
    XMinRange[0] = xmin;
    XMaxRange[0] = xmax;
  }
  else {
    XMinRange[0] = xmax;
    XMaxRange[0] = xmin;
  }
}


void Plot::setYRange( double ymin, double ymax )
{
  if ( ymin <= ymax || ymin >= AnyScale || ymax >= AnyScale ) {
    YMinRange[0] = ymin;
    YMaxRange[0] = ymax;
  }
  else {
    YMinRange[0] = ymax;
    YMaxRange[0] = ymin;
  }
}


void Plot::setXFallBackRange( double xmin, double xmax )
{
  if ( xmin <= xmax ) {
    XMinFB[0] = xmin;
    XMaxFB[0] = xmax;
  }
  else {
    XMinFB[0] = xmax;
    XMaxFB[0] = xmin;
  }
}


void Plot::setYFallBackRange( double ymin, double ymax )
{
  if ( ymin <= ymax ) {
    YMinFB[0] = ymin;
    YMaxFB[0] = ymax;
  }
  else {
    YMinFB[0] = ymax;
    YMaxFB[0] = ymin;
  }
}


void Plot::setAutoScaleX( void )
{
  XMinRange[0] = AutoScale;
  XMaxRange[0] = AutoScale;
}


void Plot::setAutoScaleY( void )
{
  YMinRange[0] = AutoScale;
  YMaxRange[0] = AutoScale;
}


void Plot::setAutoScaleXY( void )
{
  XMinRange[0] = AutoScale;
  XMaxRange[0] = AutoScale;
  YMinRange[0] = AutoScale;
  YMaxRange[0] = AutoScale;
}


void Plot::noAutoScaleX( void )
{
  XMinRange[0] = XMin[0];
  XMaxRange[0] = XMax[0];
}


void Plot::noAutoScaleY( void )
{
  YMinRange[0] = YMin[0];
  YMaxRange[0] = YMax[0];
}


void Plot::noAutoScaleXY( void )
{
  XMinRange[0] = XMin[0];
  XMaxRange[0] = XMax[0];
  YMinRange[0] = YMin[0];
  YMaxRange[0] = YMax[0];
}


void Plot::setX2Range( double xmin, double xmax )
{
  if ( xmin <= xmax || xmin >= AnyScale || xmax >= AnyScale ) {
    XMinRange[1] = xmin;
    XMaxRange[1] = xmax;
  }
  else {
    XMinRange[1] = xmax;
    XMaxRange[1] = xmin;
  }
}


void Plot::setY2Range( double ymin, double ymax )
{
  if ( ymin <= ymax || ymin >= AnyScale || ymax >= AnyScale ) {
    YMinRange[1] = ymin;
    YMaxRange[1] = ymax;
  }
  else {
    YMinRange[1] = ymax;
    YMaxRange[1] = ymin;
  }
}


void Plot::setX2FallBackRange( double xmin, double xmax )
{
  if ( xmin <= xmax ) {
    XMinFB[1] = xmin;
    XMaxFB[1] = xmax;
  }
  else {
    XMinFB[1] = xmax;
    XMaxFB[1] = xmin;
  }
}


void Plot::setY2FallBackRange( double ymin, double ymax )
{
  if ( ymin <= ymax ) {
    YMinFB[1] = ymin;
    YMaxFB[1] = ymax;
  }
  else {
    YMinFB[1] = ymax;
    YMaxFB[1] = ymin;
  }
}


void Plot::setAutoScaleX2( void )
{
  XMinRange[1] = AutoScale;
  XMaxRange[1] = AutoScale;
}


void Plot::setAutoScaleY2( void )
{
  YMinRange[1] = AutoScale;
  YMaxRange[1] = AutoScale;
}


void Plot::setAutoScaleX2Y2( void )
{
  XMinRange[1] = AutoScale;
  XMaxRange[1] = AutoScale;
  YMinRange[1] = AutoScale;
  YMaxRange[1] = AutoScale;
}


void Plot::noAutoScaleX2( void )
{
  XMinRange[1] = XMin[1];
  XMaxRange[1] = XMax[1];
}


void Plot::noAutoScaleY2( void )
{
  YMinRange[1] = YMin[1];
  YMaxRange[1] = YMax[1];
}


void Plot::noAutoScaleX2Y2( void )
{
  XMinRange[1] = XMin[1];
  XMaxRange[1] = XMax[1];
  YMinRange[1] = YMin[1];
  YMaxRange[1] = YMax[1];
}


void Plot::setTicsStyle( Tics pos, int color, int len, int width )
{
  TicsPos = pos;
  if ( color == Transparent )
    TicsColor = BorderStyle.color();
  else
    TicsColor = color;
  if ( len >= 0 )
    TicsLen = len;
  else
    TicsLen = 5;
  if ( width > 0 )
    TicsWidth = width;
  else
    TicsWidth = 0;
}


void Plot::setXTics( double incr, int pos ) 
{ 
  XTics[0] = pos;
  if ( incr == AutoScale ) 
    XTicsIncrAutoScale[0] = 1; 
  else if ( incr == DynamicScale ) 
    XTicsIncrAutoScale[0] = 2; 
  else { 
    XTicsIncrAutoScale[0] = 0; 
    XTicsIncr[0] = incr;
  }
  XTicsStartAutoScale[0] = true;
}


void Plot::setYTics( double incr, int pos ) 
{ 
  YTics[0] = pos;
  if ( incr == AutoScale ) 
    YTicsIncrAutoScale[0] = 1; 
  else if ( incr == DynamicScale ) 
    YTicsIncrAutoScale[0] = 2; 
  else {
    YTicsIncrAutoScale[0] = 0; 
    YTicsIncr[0] = incr; 
  }
  YTicsStartAutoScale[0] = true;
}


void Plot::setXTics( double start, double incr, int pos )
{
  setXTics( incr, pos );
  XTicsStartAutoScale[0] = ( start >= AnyScale );
  if ( !XTicsStartAutoScale[0] )
    XTicsStart[0] = start;
}


void Plot::setYTics( double start, double incr, int pos )
{
  setYTics( incr, pos );
  YTicsStartAutoScale[0] = ( start >= AnyScale );
  if ( !YTicsStartAutoScale[0] )
    YTicsStart[0] = start;
}


void Plot::setXTics( int pos )
{
  XTics[0] = pos;
}


void Plot::setYTics( int pos )
{
  YTics[0] = pos;
}


void Plot::setX2Tics( double incr, int pos ) 
{ 
  XTics[1] = pos;
  if ( incr == AutoScale ) 
    XTicsIncrAutoScale[1] = 1; 
  else if ( incr == DynamicScale ) 
    XTicsIncrAutoScale[1] = 2; 
  else { 
    XTicsIncrAutoScale[1] = 0; 
    XTicsIncr[1] = incr;
  }
  XTicsStartAutoScale[1] = true;
}


void Plot::setY2Tics( double incr, int pos ) 
{ 
  YTics[1] = pos;
  if ( incr == AutoScale ) 
    YTicsIncrAutoScale[1] = 1; 
  else if ( incr == DynamicScale ) 
    YTicsIncrAutoScale[1] = 2; 
  else {
    YTicsIncrAutoScale[1] = 0; 
    YTicsIncr[1] = incr; 
  }
  YTicsStartAutoScale[1] = true;
}


void Plot::setX2Tics( double start, double incr, int pos )
{
  setX2Tics( incr, pos );
  XTicsStartAutoScale[1] = ( start >= AnyScale );
  if ( !XTicsStartAutoScale[1] )
    XTicsStart[1] = start;
}


void Plot::setY2Tics( double start, double incr, int pos )
{
  setY2Tics( incr, pos );
  YTicsStartAutoScale[1] = ( start >= AnyScale );
  if ( !YTicsStartAutoScale[1] )
    YTicsStart[1] = start;
}


void Plot::setX2Tics( int pos )
{
  XTics[1] = pos;
}


void Plot::setY2Tics( int pos )
{
  YTics[1] = pos;
}


void Plot::noTics( void )
{
  for ( int k=0; k<MaxAxis; k++ ) {
    XTics[k] = 0;
    YTics[k] = 0;
  }
}


void Plot::setMinXTics( double min ) 
{ 
  XTicsMinIncr[0] = min; 
}


void Plot::setMinYTics( double min ) 
{ 
  YTicsMinIncr[0] = min; 
}


void Plot::setMinX2Tics( double min ) 
{ 
  XTicsMinIncr[1] = min; 
}


void Plot::setMinY2Tics( double min ) 
{ 
  YTicsMinIncr[1] = min; 
}


void Plot::setXGrid( int color, int width, Dash dash )
{
  XGrid[0] = true;
  XGridStyle[0] = LineStyle( color, width, dash );
}


void Plot::setXGrid( const LineStyle &style )
{
  XGrid[0] = true;
  XGridStyle[0] = style;
}


void Plot::setYGrid( int color, int width, Dash dash )
{
  YGrid[0] = true;
  YGridStyle[0] = LineStyle( color, width, dash );
}


void Plot::setYGrid( const LineStyle &style )
{
  YGrid[0] = true;
  YGridStyle[0] = style;
}


void Plot::setXYGrid( int color, int width, Dash dash )
{
  XGrid[0] = true;
  XGridStyle[0] = LineStyle( color, width, dash );

  YGrid[0] = true;
  YGridStyle[0] = LineStyle( color, width, dash );
}


void Plot::setXYGrid( const LineStyle &style )
{
  XGrid[0] = true;
  XGridStyle[0] = style;

  YGrid[0] = true;
  YGridStyle[0] = style;
}


void Plot::setX2Grid( int color, int width, Dash dash )
{
  XGrid[1] = true;
  XGridStyle[1] = LineStyle( color, width, dash );
}


void Plot::setX2Grid( const LineStyle &style )
{
  XGrid[1] = true;
  XGridStyle[1] = style;
}


void Plot::setY2Grid( int color, int width, Dash dash )
{
  YGrid[1] = true;
  YGridStyle[1] = LineStyle( color, width, dash );
}


void Plot::setY2Grid( const LineStyle &style )
{
  YGrid[1] = true;
  YGridStyle[1] = style;
}


void Plot::setX2Y2Grid( int color, int width, Dash dash )
{
  XGrid[1] = true;
  XGridStyle[1] = LineStyle( color, width, dash );

  YGrid[1] = true;
  YGridStyle[1] = LineStyle( color, width, dash );
}


void Plot::setX2Y2Grid( const LineStyle &style )
{
  XGrid[1] = true;
  XGridStyle[1] = style;

  YGrid[1] = true;
  YGridStyle[1] = style;
}


void Plot::noGrid( void )
{
  for ( int k=0; k<MaxAxis; k++ ) {
    XGrid[k] = false;
    YGrid[k] = false;
  }
}


void Plot::setFormat( const string &format )
{
  for ( int k=0; k<MaxAxis; k++ ) {
    XTicsFormat[k] = format; 
    YTicsFormat[k] = format;
  }
}


int Plot::xPixel( double xpos, Plot::Coordinates xcoor ) const
{
  if ( xcoor == First || xcoor == FirstX || xcoor == FirstY )
    return (int)::rint( ( xpos - XMin[0] )*( PlotX2 - PlotX1 )/( XMax[0] - XMin[0] ) + PlotX1 );
  else if ( xcoor == Second || xcoor == SecondX || xcoor == SecondY )
    return (int)::rint( ( xpos - XMin[1] )*( PlotX2 - PlotX1 )/( XMax[1] - XMin[1] ) + PlotX1 );
  else if ( xcoor == Graph || xcoor == GraphX || xcoor == GraphY )
    return (int)::rint( xpos * ( PlotX2 - PlotX1 ) + PlotX1 );
  else if ( xcoor == Screen )
    return (int)::rint( xpos * ( ScreenX2 - ScreenX1 ) + ScreenX1 );
  else if ( xcoor == FirstAxis )
    return (int)::rint( xpos * FontWidth + PlotX1 - BorderStyle.width() - Y1TicsMarg );
  else if ( xcoor == SecondAxis )
    return (int)::rint( xpos * FontWidth + PlotX2 + BorderStyle.width() + Y2TicsMarg );
  else if ( xcoor == FirstMargin )
    return (int)::rint( xpos * FontWidth + ScreenX1 + ScreenBorder );
  else if ( xcoor == SecondMargin )
    return (int)::rint( xpos * FontWidth + ScreenX2 - ScreenBorder );
  return -1;
}


int Plot::xPixel( const Position &pos ) const
{
  return xPixel( pos.xpos(), pos.xcoor() );
}


int Plot::yPixel( double ypos, Plot::Coordinates ycoor ) const
{
  if ( ycoor == First || ycoor == FirstX || ycoor == FirstY )
    return (int)::rint( ( ypos - YMin[0] )*( PlotY2 - PlotY1 )/( YMax[0] - YMin[0] ) + PlotY1 );
  else if ( ycoor == Second || ycoor == SecondX || ycoor == SecondY )
    return (int)::rint( ( ypos - YMin[1] )*( PlotY2 - PlotY1 )/( YMax[1] - YMin[1] ) + PlotY1 );
  else if ( ycoor == Graph || ycoor == GraphX || ycoor == GraphY )
    return (int)::rint( ypos * ( PlotY2 - PlotY1 ) + PlotY1 );
  else if ( ycoor == Screen )
    return (int)::rint( ypos * ( ScreenY2 - ScreenY1 ) + ScreenY1 );
  else if ( ycoor == FirstAxis )
    return (int)::rint( - ypos * FontHeight + PlotY1 + BorderStyle.width() + X1TicsMarg );
  else if ( ycoor == SecondAxis )
    return (int)::rint( - ypos * FontHeight + PlotY2 - BorderStyle.width() - X2TicsMarg );
  else if ( ycoor == FirstMargin )
    return (int)::rint( - ypos * FontHeight + ScreenY1 - ScreenBorder );
  else if ( ycoor == SecondMargin )
    return (int)::rint( - ypos * FontHeight + ScreenY2 + ScreenBorder );
  return -1;
}


int Plot::yPixel( const Position &pos ) const
{
  return yPixel( pos.ypos(), pos.ycoor() );
}


int Plot::fontPixel( double w ) const
{
  return (int)::rint( FontWidth*w );
}


Plot::Label::Label( void )
{
  Just = Plot::Left;
  Angle = 0.0;
  Text = "";
  LColor = Plot::Black;
  LFont = Plot::DefaultF;
  LSize = 1.0;
  BColor = Plot::Transparent;
  FColor = Plot::Transparent;
  FWidth = 0;
}


void Plot::setXLabel( const string &label, int color, double size, 
		      Fonts font, int bcolor, int fwidth, int fcolor )
{
  XLabel[0].setXPos( 1.0, Graph );
  XLabel[0].setYPos( -1.0, FirstAxis );
  XLabel[0].Just = Right;
  XLabel[0].Angle = 0.0;
  XLabel[0].Text = label;
  XLabel[0].LColor = color != Transparent ? color : TicsColor;
  XLabel[0].LFont = font;
  XLabel[0].LSize = size > 0.0 ? size : 0.1;
  XLabel[0].BColor = bcolor;
  XLabel[0].FColor = fcolor;
  XLabel[0].FWidth = fwidth > 0 ? fwidth : 0;
}


void Plot::setXLabelPos( double xpos, Coordinates xcoor,
			 double ypos, Coordinates ycoor,
			 Justification just, double angle )
{
  XLabel[0].setXPos( xpos, xcoor );
  XLabel[0].setYPos( ypos, ycoor );
  XLabel[0].Just = just;
  XLabel[0].Angle = angle;
}


void Plot::setYLabel( const string &label, int color, double size, 
		      Fonts font, int bcolor, int fwidth, int fcolor )
{
  /*
  YLabel[0].setXPos( 0.0, FirstMargin );
  YLabel[0].setYPos( 0.0, SecondAxis );
  YLabel[0].Just = Left;
  YLabel[0].Angle = 0.0;
  */

  YLabel[0].setXPos( 0.0, FirstAxis );
  YLabel[0].setYPos( 0.5, Graph );
  YLabel[0].Just = Center;
  YLabel[0].Angle = -90.0;

  YLabel[0].Text = label;
  YLabel[0].LColor = color != Transparent ? color : TicsColor;
  YLabel[0].LFont = font;
  YLabel[0].LSize = size > 0.0 ? size : 0.1;
  YLabel[0].BColor = bcolor;
  YLabel[0].FColor = fcolor;
  YLabel[0].FWidth = fwidth > 0 ? fwidth : 0;
}


void Plot::setYLabelPos( double xpos, Coordinates xcoor,
			 double ypos, Coordinates ycoor,
			 Justification just, double angle )
{
  YLabel[0].setXPos( xpos, xcoor );
  YLabel[0].setYPos( ypos, ycoor );
  YLabel[0].Just = just;
  YLabel[0].Angle = angle;
}


void Plot::setX2Label( const string &label, int color, double size, 
		       Fonts font, int bcolor, int fwidth, int fcolor )
{
  XLabel[1].setXPos( 1.0, Graph );
  XLabel[1].setYPos( 0.0, SecondAxis );
  XLabel[1].Just = Right;
  XLabel[1].Angle = 0.0;
  XLabel[1].Text = label;
  XLabel[1].LColor = color != Transparent ? color : TicsColor;
  XLabel[1].LFont = font;
  XLabel[1].LSize = size > 0.0 ? size : 0.1;
  XLabel[1].BColor = bcolor;
  XLabel[1].FColor = fcolor;
  XLabel[1].FWidth = fwidth > 0 ? fwidth : 0;
}


void Plot::setX2LabelPos( double xpos, Coordinates xcoor,
			  double ypos, Coordinates ycoor,
			  Justification just, double angle )
{
  XLabel[1].setXPos( xpos, xcoor );
  XLabel[1].setYPos( ypos, ycoor );
  XLabel[1].Just = just;
  XLabel[1].Angle = angle;
}


void Plot::setY2Label( const string &label, int color, double size, 
		       Fonts font, int bcolor, int fwidth, int fcolor )
{
  YLabel[1].setXPos( 1.0, SecondAxis );
  YLabel[1].setYPos( 0.5, Graph );
  YLabel[1].Just = Center;
  YLabel[1].Angle = -90.0;

  YLabel[1].Text = label;
  YLabel[1].LColor = color != Transparent ? color : TicsColor;
  YLabel[1].LFont = font;
  YLabel[1].LSize = size > 0.0 ? size : 0.1;
  YLabel[1].BColor = bcolor;
  YLabel[1].FColor = fcolor;
  YLabel[1].FWidth = fwidth > 0 ? fwidth : 0;
}


void Plot::setY2LabelPos( double xpos, Coordinates xcoor,
			  double ypos, Coordinates ycoor,
			  Justification just, double angle )
{
  YLabel[1].setXPos( xpos, xcoor );
  YLabel[1].setYPos( ypos, ycoor );
  YLabel[1].Just = just;
  YLabel[1].Angle = angle;
}


void Plot::setTitle( const string &title, int color, double size, 
		     Fonts font, int bcolor, int fwidth, int fcolor )
{
  Title.setXPos( 1.0, Graph );
  Title.setYPos( 0.0, SecondAxis );
  Title.Just = Right;
  Title.Angle = 0.0;
  Title.Text = title;
  Title.LColor = color != Transparent ? color : TicsColor;
  Title.LFont = font;
  Title.LSize = size > 0.0 ? size : 0.1;
  Title.BColor = bcolor;
  Title.FColor = fcolor;
  Title.FWidth = fwidth > 0 ? fwidth : 0;
}


void Plot::setTitlePos( double xpos, Coordinates xcoor,
			double ypos, Coordinates ycoor,
			Justification just, double angle )
{
  Title.setXPos( xpos, xcoor );
  Title.setYPos( ypos, ycoor );
  Title.Just = just;
  Title.Angle = angle;
}


int Plot::setLabel( const string &label, double x, double y, 
		    Justification just, double angle )
{
  Label l = DefaultLabel;

  l.setXPos( x );
  l.setYPos( y );
  l.Just = just;
  l.Angle = angle;
  l.Text = label;

  Labels.push_back( l );
  return Labels.size()-1;
}


int Plot::setLabel( const string &label, double x, Coordinates xcoor,
	      double y, Coordinates ycoor, Justification just, double angle )
{
  Label l = DefaultLabel;

  l.setXPos( x, xcoor );
  l.setYPos( y, ycoor );
  l.Just = just;
  l.Angle = angle;
  l.Text = label;

  Labels.push_back( l );
  return Labels.size()-1;
}


int Plot::setLabel( const string &label, double x, Coordinates xcoor,
		    double y, Coordinates ycoor, Justification just,
		    double angle, int color, double size, 
		    Fonts font, int bcolor, int fwidth, int fcolor )
{
  Label l;

  l.setXPos( x, xcoor );
  l.setYPos( y, ycoor );
  l.Just = just;
  l.Angle = angle;
  l.Text = label;
  l.LColor = color != Transparent ? color : TicsColor;
  l.LFont = font;
  l.LSize = size > 0.0 ? size : 0.1;
  l.BColor = bcolor;
  l.FColor = fcolor;
  l.FWidth = fwidth > 0 ? fwidth : 0;

  Labels.push_back( l );
  return Labels.size()-1;
}


int Plot::setLabel( int index, const string &label )
{
  if ( index < 0 || index >= int( Labels.size() ) )
    return -1;

  Labels[index].Text = label;

  return index;
}


QSize Plot::sizeHint( void ) const
{
  QSize qs( LMarg+RMarg+180, TMarg+BMarg+150 );
  return qs;
}


QSize Plot::minimumSizeHint( void ) const
{
  QSize qs( LMarg+RMarg+120, TMarg+BMarg+100 );
  return qs;
}


void Plot::resizeEvent( QResizeEvent *qre )
{
  PMutex.lock();
  NewData = true;
  PMutex.unlock();

  if ( SubWidget )
    return;

  PMutex.lock();

  ScreenX1 = 0;
  ScreenY1 = height() - 1;
  ScreenX2 = width() - 1;
  ScreenY2 = 0;

  PixMap->resize( ScreenX2 - ScreenX1, ScreenY1 - ScreenY2 );

  PMutex.unlock();
}


void Plot::init( void )
{
  for ( PDataType::iterator d = PData.begin(); d != PData.end(); ++d ) {
    if ( (*d)->init() )
      NewData = true;
  }
}


void Plot::initXRange( int axis )
{
  XMin[axis] = XMinRange[axis];
  XMax[axis] = XMaxRange[axis];
  if ( XMinRange[axis] >= AnyScale || XMaxRange[axis] >= AnyScale ) {
    double xmin = XMinFB[axis];
    double xmax = XMaxFB[axis];
    double ymin[MaxAxis], ymax[MaxAxis];
    for ( int k=0; k<MaxAxis; k++ ) {
      ymin[k] = YMin[k] >= AnyScale ? -MAXDOUBLE : YMin[k];
      ymax[k] = YMax[k] >= AnyScale ? MAXDOUBLE : YMax[k];
    }
    PDataType::iterator d;
    for ( d = PData.begin(); d != PData.end(); ++d ) {
      if ( (*d)->XAxis == axis ) {
	double nxmin, nxmax;
	(*d)->xminmax( nxmin, nxmax, ymin[(*d)->YAxis], ymax[(*d)->YAxis] );
	if ( nxmin != AutoScale && nxmax != AutoScale ) {
	  xmin = nxmin;
	  xmax = nxmax;
	  for ( ++d; d != PData.end(); ++d ) {
	    if ( (*d)->XAxis == axis ) {
	      (*d)->xminmax( nxmin, nxmax, ymin[(*d)->YAxis], ymax[(*d)->YAxis] );
	      if ( nxmin != AutoScale && nxmin < xmin )
		xmin = nxmin;
	      if ( nxmax != AutoScale && nxmax > xmax )
		xmax = nxmax;
	    }
	  }
	  break;
	}
      }
    }
    if ( XMinRange[axis] >= AnyScale ) {
      if ( XMinRange[axis] == AutoScale || XMinRange[axis] == ExactScale ||
	   xmin < XMinFB[axis] )
	XMin[axis] = xmin;
      else
	XMin[axis] = XMinFB[axis];
    }
    if ( XMaxRange[axis] >= AnyScale ) {
      if ( XMaxRange[axis] == AutoScale || XMaxRange[axis] == ExactScale ||
	   ( xmax > XMaxFB[axis] && xmax < AnyScale ) )
	XMax[axis] = xmax;
      else
	XMax[axis] = XMaxFB[axis];
    }
  }

  if ( ::fabs( XMax[axis] - XMin[axis] ) < 1.0e-8 &&
       ( XMaxRange[axis] >= AnyScale || XMinRange[axis] >= AnyScale ) ) {
    if ( XMaxRange[axis] >= AnyScale && XMinRange[axis] < AnyScale )
      XMax[axis] = XMin[axis] + ::fabs( XMaxFB[axis] - XMinFB[axis] );
    else if ( XMaxRange[axis] < AnyScale && XMinRange[axis] >= AnyScale )
      XMin[axis] = XMax[axis] - ::fabs( XMaxFB[axis] - XMinFB[axis] );
    else {
      if ( XMin[axis] >= XMinFB[axis] && XMin[axis] <= XMaxFB[axis] ) {
	XMin[axis] = XMinFB[axis];
	XMax[axis] = XMaxFB[axis];
      }
      else {
	XMin[axis] -= 0.5 * ::fabs( XMaxFB[axis] - XMinFB[axis] );
	XMax[axis] = XMin[axis] + ::fabs( XMaxFB[axis] - XMinFB[axis] );
      }
    }
  }

  if ( XMin[axis] > XMax[axis] )
    swap( XMin[axis], XMax[axis] );
}


void Plot::initYRange( int axis )
{
  YMin[axis] = YMinRange[axis];
  YMax[axis] = YMaxRange[axis];
  if ( YMinRange[axis] >= AnyScale || YMaxRange[axis] >= AnyScale ) {
    double xmin[MaxAxis], xmax[MaxAxis];
    for ( int k=0; k<MaxAxis; k++ ) {
      xmin[k] = XMin[k] >= AnyScale ? -MAXDOUBLE : XMin[k];
      xmax[k] = XMax[k] >= AnyScale ? MAXDOUBLE : XMax[k];
    }
    double ymin = YMinFB[axis];
    double ymax = YMaxFB[axis];
    PDataType::iterator d;
    for ( d = PData.begin(); d != PData.end(); ++d ) {
      if ( (*d)->YAxis == axis ) {
	double nymin, nymax;
	(*d)->yminmax( xmin[(*d)->XAxis], xmax[(*d)->XAxis], nymin, nymax );
	if ( nymin != AutoScale && nymax != AutoScale ) {
	  ymin = nymin;
	  ymax = nymax;
	  for ( ++d; d != PData.end(); ++d ) {
	    if ( (*d)->YAxis == axis ) {
	      (*d)->yminmax( xmin[(*d)->XAxis], xmax[(*d)->XAxis], nymin, nymax );
	      if ( nymin != AutoScale && nymin < ymin )
		ymin = nymin;
	      if ( nymax != AutoScale && nymax > ymax )
		ymax = nymax;
	    }
	  }
	  break;
	}
      }
    }
    if ( YMinRange[axis] >= AnyScale ) {
      if ( YMinRange[axis] == AutoScale || YMinRange[axis] == ExactScale ||
	   ymin < YMinFB[axis] )
	YMin[axis] = ymin;
      else
	YMin[axis] = YMinFB[axis];
    }
    if ( YMaxRange[axis] >= AnyScale ) {
      if ( YMaxRange[axis] == AutoScale || YMaxRange[axis] == ExactScale ||
	   ymax > YMaxFB[axis] )
	YMax[axis] = ymax;
      else
	YMax[axis] = YMaxFB[axis];
    }
  }

  if ( ::fabs( YMax[axis] - YMin[axis] ) < 1.0e-8 && 
       ( YMaxRange[axis] >= AnyScale || YMinRange[axis] >= AnyScale ) ) {
    if ( YMaxRange[axis] >= AnyScale && YMinRange[axis] < AnyScale )
      YMax[axis] = YMin[axis] + ::fabs( YMaxFB[axis] - YMinFB[axis] );
    else if ( YMaxRange[axis] < AnyScale && YMinRange[axis] >= AnyScale )
      YMin[axis] = YMax[axis] - ::fabs( YMaxFB[axis] - YMinFB[axis] );
    else {
      if ( YMin[axis] >= YMinFB[axis] && YMin[axis] <= YMaxFB[axis] ) {
	YMin[axis] = YMinFB[axis];
	YMax[axis] = YMaxFB[axis];
      }
      else {
	YMin[axis] -= 0.5 * ::fabs( YMaxFB[axis] - YMinFB[axis] );
	YMax[axis] = YMin[axis] + ::fabs( YMaxFB[axis] - YMinFB[axis] );
      }
    }
  }

  if ( YMin[axis] > YMax[axis] )
    swap( YMin[axis], YMax[axis] );
}


void Plot::initRange( void )
{
  for ( int k=0; k<MaxAxis; k++ ) {
    initXRange( k );
    initYRange( k );
  }
}


double Plot::autoTics( double val, double min )
{
  if ( val <= 0.0 )
    return 1.0;

  double valEE = ::pow( 10.0, ::floor( ::log10( val ) ) );
  val /= valEE;
  for ( ; ; ) {
    if ( val <= 1.0 && 1.0 * valEE >= min )
      return  1.0 * valEE;
    else if ( val <= 2.0 && 2.0 * valEE >= min )
      return 2.0 * valEE;
    else if ( val <= 5.0 && 5.0 * valEE >= min )
      return 5.0 * valEE;
    else {
      val *= 0.1;
      valEE *= 10.0;
    }
  }
}


double Plot::ticsStart( double min, double incr )
{
  return ::ceil( min/incr )*incr;
}


void Plot::initTics( void )
{
  // margins for tic marks:
  if ( TicsPos == Out ) {
    X1TicsLen = X1TicsMarg = XTics[0] > 0 || XTics[1] > 1 ? TicsLen : 0;
    Y1TicsLen = Y1TicsMarg = YTics[0] > 0 || YTics[1] > 1 ? TicsLen : 0;
    X2TicsLen = X2TicsMarg = XTics[1] > 0 || XTics[0] > 1 ? TicsLen : 0;
    Y2TicsLen = Y2TicsMarg = YTics[1] > 0 || YTics[0] > 1 ? TicsLen : 0;
  }
  else if ( TicsPos == Centered ) {
    X1TicsLen = X1TicsMarg = XTics[0] > 0 || XTics[1] > 1 ? TicsLen/2 : 0;
    Y1TicsLen = Y1TicsMarg = YTics[0] > 0 || YTics[1] > 1 ? TicsLen/2 : 0;
    X2TicsLen = X2TicsMarg = XTics[1] > 0 || XTics[0] > 1 ? TicsLen/2 : 0;
    Y2TicsLen = Y2TicsMarg = YTics[1] > 0 || YTics[0] > 1 ? TicsLen/2 : 0;
  }
  else {
    X1TicsLen = X1TicsMarg = 0;
    Y1TicsLen = Y1TicsMarg = 0;
    X2TicsLen = X2TicsMarg = 0;
    Y2TicsLen = Y2TicsMarg = 0;
  }


  for ( int k=0; k<MaxAxis; k++ ) {

    // y tic marks:
    if ( YTicsIncrAutoScale[k] ) {
      double ph = double( screenHeight() );
      ph -= BMargAutoScale ? X1TicsMarg : BMarg;
      ph -= TMargAutoScale ? X2TicsMarg : TMarg;
      if ( ph > FontHeight ) {
	double miny1ticsincr = ( YMax[k] - YMin[k] ) * FontSize * TicsLabelSize / ph;
	if ( YTicsIncrAutoScale[k] == AutoScale )
	  YTicsIncr[k] = autoTics( ( YMax[k] - YMin[k] )/10.0, miny1ticsincr );
	else
	  YTicsIncr[k] = autoTics( 1.2*miny1ticsincr, miny1ticsincr );
      }
      else
	YTicsIncr[k] = 10.0*(YMax[k] - YMin[k]);  // no room for tic marks!
      if ( YTicsMinIncr[k] > 0.0 && YTicsMinIncr[k] < AnyScale &&
	   YTicsIncr[k] < YTicsMinIncr[k] )
	YTicsIncr[k] = YTicsMinIncr[k];
    }
    if ( YTicsStartAutoScale[k] )
      YTicsStart[k] = ticsStart( YMin[k], YTicsIncr[k] );

    // autoscale y range:
    if ( YMaxRange[k] == AutoScale || 
	 ( YMaxRange[k] == AutoMinScale && YMax[k] > YMaxFB[k] ) ) {
      for ( double y=YTicsStart[k]; ; y+=YTicsIncr[k] ) {
	if ( y >= YMax[k] ) {
	  YMax[k] = y;
	  break;
	}
      }
    }
    if ( YMinRange[k] == AutoScale || 
	 ( YMinRange[k] == AutoMinScale && YMin[k] >= YMinFB[k] ) ) {
      for ( double y=YTicsStart[k]; ; y-=YTicsIncr[k] ) {
	if ( y <= YMin[k] ) {
	  YMin[k] = y;
	  if ( YTicsStartAutoScale[k] )
	    YTicsStart[k] = y;
	  break;
	}
      }
    }
  
    // margins for y1 tic labels:
    if ( YTics[k] ) {
      unsigned int l = 0;
      string yticstr = "";
      for ( double y=YTicsStart[k]; y<=YMax[k]; y+=YTicsIncr[k] ) {
	if ( ::fabs( y ) < 0.001*YTicsIncr[k] )
	  y = 0.0;
	Str yt( y, YTicsFormat[k] );
	if ( yt.length() > l ) {
	  l = yt.length();
	  yticstr = yt;
	}
      }
      if ( k == 1 )
	Y2TicsMarg += 2 + (int)::ceil( fontMetrics().width( yticstr.c_str() ) * TicsLabelSize );
      else
	Y1TicsMarg += 2 + (int)::ceil( fontMetrics().width( yticstr.c_str() ) * TicsLabelSize );
    }

    // x tic marks:
    if ( XTicsIncrAutoScale[k] ) {
      double minx1ticsincr;
      double dw;
      int pw = screenWidth();
      pw -= LMargAutoScale ? Y1TicsMarg : LMarg;
      pw -= RMargAutoScale ? Y2TicsMarg : RMarg;
      if ( pw >= FontWidth * TicsLabelSize ) {
	dw = FontWidth * TicsLabelSize * ( XMax[k] - XMin[k] )/pw;
	if ( XTics[k] || XGrid[k] ) {
	  minx1ticsincr = dw;
	  unsigned int ll = 0;
	  unsigned int l = 0;
	  double incr = 0.0;
	  double oincr;
	  do {
	    ll = l;
	    l = 0;
	    oincr = incr;
	    incr = autoTics( minx1ticsincr + dw, minx1ticsincr );
	    if ( incr <= oincr )
	      break;
	    double start=ticsStart( XMin[k], incr );
	    for ( double x=start; x<=XMax[k]; x+=incr ) {
	      Str xt( x, XTicsFormat[k] );
	      if ( xt.length() > l )
		l = xt.length();
	    }
	    if ( start > XMax[k] ||
		 ( start < XMin[k] && start+incr > XMax[k] ) )
	      minx1ticsincr = 10.0*( XMax[k] - XMin[k] );
	    else
	      minx1ticsincr = l*dw;
	  } while ( l != ll );
	}
	else
	  minx1ticsincr = 4*dw;
      }
      else {
	minx1ticsincr = 10.0*( XMax[k] - XMin[k] );
	dw = minx1ticsincr;
      }
    
      if ( XTicsIncrAutoScale[k] == AutoScale )
	XTicsIncr[k] = autoTics( ( XMax[k] - XMin[k] )/10.0, minx1ticsincr );
      else
	XTicsIncr[k] = autoTics( minx1ticsincr + dw, minx1ticsincr );
      if ( XTicsMinIncr[k] > 0.0 && XTicsMinIncr[k] < AnyScale &&
	   XTicsIncr[k] < XTicsMinIncr[k] )
	XTicsIncr[k] = XTicsMinIncr[k];
    }
    if ( XTicsStartAutoScale[k] )
      XTicsStart[k] = ticsStart( XMin[k], XTicsIncr[k] );

    // autoscale x range:
    if ( XMaxRange[k] == AutoScale || 
	 ( XMaxRange[k] == AutoMinScale && XMax[k] > XMaxFB[k] ) ) {
      for ( double x=XTicsStart[k]; ; x+=XTicsIncr[k] ) {
	if ( x >= XMax[k] ) {
	  XMax[k] = x;
	  break;
	}
      }
    }
    if ( XMinRange[k] == AutoScale || 
	 ( XMinRange[k] == AutoMinScale && XMin[k] >= XMinFB[k] ) ) {
      for ( double x=XTicsStart[k]; ; x-=XTicsIncr[k] ) {
	if ( x <= XMin[k] ) {
	  XMin[k] = x;
	  if ( XTicsStartAutoScale[k] )
	    XTicsStart[k] = x;
	  break;
	}
      }
    }
  
    // margins for x tic labels:
    if ( XTics[k] ) {
      if ( k == 1 )
	X2TicsMarg += (int)::ceil( FontHeight * TicsLabelSize ) + FontSize - FontHeight;
      else
	X1TicsMarg += (int)::ceil( FontHeight * TicsLabelSize );
    }

  }
  
  // adjust margins for x tic labels:
  int xticminmarg = (int)::ceil( 0.5 * FontSize );
  if ( X1TicsMarg < xticminmarg )
    X1TicsMarg = xticminmarg;
  if ( X2TicsMarg < xticminmarg )
    X2TicsMarg = xticminmarg;

  // adjust margins for y tic labels:
  int yticminmarg = (int)::ceil( TicsLabelSize * FontWidth * 1.5 );
  if ( Y1TicsMarg < yticminmarg )
    Y1TicsMarg = yticminmarg;
  if ( Y2TicsMarg < yticminmarg )
    Y2TicsMarg = yticminmarg;
}


void Plot::initBorder( void )
{
  // set x- and ylabel margins:
  int X1LabelMarg = 0;
  int Y1LabelMarg = 0;
  int X2LabelMarg = 0;
  int Y2LabelMarg = 0;

  // check y1 label left to y1axis:
  if ( LMargAutoScale && YLabel[0].xcoor() == FirstAxis &&
       YLabel[0].Text.length() > 0 && YLabel[0].LColor != Transparent )
    Y1LabelMarg = 2 + (int)::ceil( YLabel[0].LSize * FontSize );
  // check y1 label above x2axis:
  if ( TMargAutoScale && YLabel[0].ycoor() == SecondAxis &&
       YLabel[0].Text.length() > 0 && YLabel[0].LColor != Transparent )
    X2LabelMarg = (int)::ceil( YLabel[0].LSize * FontSize );

  // check y2 label right to y2axis:
  if ( RMargAutoScale && YLabel[1].xcoor() == SecondAxis &&
       YLabel[1].Text.length() > 0 && YLabel[1].LColor != Transparent )
    Y2LabelMarg = 2 + (int)::ceil( YLabel[1].LSize * FontSize );
  // check y2 label above x2axis:
  if ( TMargAutoScale && YLabel[1].ycoor() == SecondAxis &&
       YLabel[1].Text.length() > 0 && YLabel[1].LColor != Transparent ) {
    int marg = (int)::ceil( YLabel[1].LSize * FontSize );
    if ( X2LabelMarg < marg )
      X2LabelMarg = marg;
  }

  // check x1 label:
  if ( BMargAutoScale && XLabel[0].ycoor() == FirstAxis &&
       XLabel[0].Text.length() > 0 && XLabel[0].LColor != Transparent )
    X1LabelMarg = 2 + (int)::ceil( XLabel[0].LSize * FontSize );
  // check x2 label:
  if ( TMargAutoScale && XLabel[1].ycoor() == SecondAxis &&
       XLabel[1].Text.length() > 0 && XLabel[1].LColor != Transparent ) {
    int marg = 2 + (int)::ceil( XLabel[1].LSize * FontSize );
    if (  X2LabelMarg < marg )
      X2LabelMarg = marg;
  }

  // check title:
  if ( TMargAutoScale && Title.ycoor() == SecondAxis &&
       Title.Text.length() > 0 && Title.LColor != Transparent ) {
    int tmarg = (int)::ceil( Title.LSize * FontSize );
    X2LabelMarg = tmarg > X2LabelMarg ? tmarg : X2LabelMarg;
  }

  // set margins:
  if ( LMargAutoScale )
    LMarg = BorderStyle.width() + Y1TicsMarg + Y1LabelMarg + ScreenBorder;
  if ( RMargAutoScale )
    RMarg = BorderStyle.width() + Y2TicsMarg + Y2LabelMarg + ScreenBorder;
  if ( BMargAutoScale )
    BMarg = BorderStyle.width() + X1TicsMarg + X1LabelMarg + ScreenBorder;
  if ( TMargAutoScale )
    TMarg = BorderStyle.width() + X2TicsMarg + X2LabelMarg + ScreenBorder;

  // set plot coordinates:
  PlotX1 = ScreenX1 + LMarg;
  PlotY1 = ScreenY1 - BMarg;
  PlotX2 = ScreenX2 - RMarg;
  PlotY2 = ScreenY2 + TMarg;
}


void Plot::initLines( void )
{
  for ( PDataType::iterator d = PData.begin(); d != PData.end(); ++d )
    (*d)->setRange( XMin, XMax, YMin, YMax, PlotX1, PlotX2, PlotY1, PlotY2 );
}


void Plot::drawBorder( QPainter &paint )
{
  // draw background:
  if ( BackgroundColor != Transparent ) {
    RGBColor c = color( BackgroundColor );
    QColor qcolor( c.red(), c.green(), c.blue() );
    paint.fillRect( ScreenX1, ScreenY2, PlotX1, ScreenY1 - ScreenY2 + 1, qcolor );
    paint.fillRect( PlotX2 + 1, ScreenY2, ScreenX2 - PlotX2, ScreenY1 - ScreenY2 + 1, qcolor );
    paint.fillRect( PlotX1, ScreenY1, PlotX2 - PlotX1 + 1, PlotY1 - ScreenY1, qcolor );
    paint.fillRect( PlotX1, ScreenY2, PlotX2 - PlotX1 + 1, PlotY2 - ScreenY2, qcolor );
  }

  if ( ! NewData && ShiftData ) {

    // draw plot background:
    if ( PlotColor != Transparent ) {
      RGBColor c = color( PlotColor );
      QColor qcolor( c.red(), c.green(), c.blue() );
      if ( ShiftXPix > 0 )
	paint.fillRect( PlotX2-ShiftXPix, PlotY2, ShiftXPix, PlotY1-PlotY2+1, qcolor );
      else
	paint.fillRect( PlotX1, PlotY2, -ShiftXPix, PlotY1-PlotY2+1, qcolor );
    }

  }
  else {

    // draw plot background:
    if ( PlotColor != Transparent ) {
      RGBColor c = color( PlotColor );
      QColor qcolor( c.red(), c.green(), c.blue() );
      paint.fillRect( PlotX1, PlotY2, PlotX2-PlotX1+1, PlotY1-PlotY2+1, qcolor );
    }

    // grid:
    for ( int k=0; k<MaxAxis; k++ ) {
      
      // draw horizontal grid lines:
      if ( YGrid[k] && YGridStyle[k].width() > 0 && 
	   YGridStyle[k].color() != Transparent ) {
	RGBColor c = color( YGridStyle[k].color() );
	QColor qcolor( c.red(), c.green(), c.blue() );
	Qt::PenStyle dash = QtDash.find( YGridStyle[k].dash() )->second;
	paint.setPen( QPen( qcolor, YGridStyle[k].width(), dash ) );
	for ( double y=YTicsStart[k]; y<=YMax[k]; y+=YTicsIncr[k] ) {
	  if ( ::fabs( y ) < 0.001*YTicsIncr[k] )
	    y = 0.0;
	  int yp = PlotY1 + (int)::rint( double(PlotY2-PlotY1)/(YMax[k]-YMin[k])*(y-YMin[k]) );
	  if ( ::abs( PlotY1 - yp ) > YGridStyle[k].width() &&
	       ::abs( PlotY2 - yp ) > YGridStyle[k].width() )
	    paint.drawLine( PlotX1, yp, PlotX2, yp );
	}
      }
      
      // draw vertical grid lines:
      if ( XGrid[k] && XGridStyle[k].width() > 0 && 
	   XGridStyle[k].color() != Transparent ) {
	RGBColor c = color( XGridStyle[k].color() );
	QColor qcolor( c.red(), c.green(), c.blue() );
	Qt::PenStyle dash = QtDash.find( XGridStyle[k].dash() )->second;
	paint.setPen( QPen( qcolor, XGridStyle[k].width(), dash ) );
	for ( double x=XTicsStart[k]; x<=XMax[k]; x+=XTicsIncr[k] ) {
	  if ( ::fabs( x ) < 0.001*XTicsIncr[k] )
	    x = 0.0;
	  int xp = PlotX1 + (int)::rint( double(PlotX2-PlotX1)/(XMax[k]-XMin[k])*(x-XMin[k]) );
	  if ( ::abs( PlotX1 - xp ) > XGridStyle[k].width() &&
	       ::abs( PlotX2 - xp ) > XGridStyle[k].width() )
	    paint.drawLine( xp, PlotY1, xp, PlotY2 );
	}
      }
      
    }
  }
  
  // draw border:
  if ( Border > 0 && BorderStyle.width() > 0 && 
       BorderStyle.color() != Transparent ) {
    RGBColor c = color( BorderStyle.color() );
    QColor qcolor( c.red(), c.green(), c.blue() );
    Qt::PenStyle dash = QtDash.find( BorderStyle.dash() )->second;
    paint.setPen( QPen( qcolor, BorderStyle.width(), dash ) );
    int bwh1 = BorderStyle.width()/2;
    int bwh2 = BorderStyle.width()-bwh1;
    if ( Border & 1 )
      paint.drawLine( PlotX1-BorderStyle.width(), PlotY1+bwh1+1, PlotX2+BorderStyle.width(), PlotY1+bwh1+1 );
    if ( Border & 2 )
      paint.drawLine( PlotX1-bwh2, PlotY1+BorderStyle.width()+1, PlotX1-bwh2, PlotY2-BorderStyle.width()+1 );  
    if ( Border & 4 )
      paint.drawLine( PlotX1-BorderStyle.width(), PlotY2-bwh2+1, PlotX2+BorderStyle.width(), PlotY2-bwh2+1 );  
    if ( Border & 8 )
      paint.drawLine( PlotX2+bwh1, PlotY2-BorderStyle.width()+1, PlotX2+bwh1, PlotY1+BorderStyle.width()+1 );  
  }
  paint.flush();
}


void Plot::drawTicMarks( QPainter &paint, int axis )
{
  // x tic marks:
  if ( XTics[axis] && TicsLen > 0 && TicsWidth > 0 && TicsColor != Transparent ) {
    RGBColor c = color( TicsColor );
    QColor qcolor( c.red(), c.green(), c.blue() );
    paint.setPen( QPen( qcolor, TicsWidth ) );
    for ( double x=XTicsStart[axis]; x<=XMax[axis]; x+=XTicsIncr[axis] )	{
      if ( ::fabs( x ) < 0.001*XTicsIncr[axis] )
	x = 0.0;
      int xp = PlotX1 + (int)::rint( double(PlotX2-PlotX1)/(XMax[axis]-XMin[axis])*(x-XMin[axis]) );
      if ( XTics[axis] > axis  ) {
	if ( TicsPos == Out )
	  paint.drawLine( xp, PlotY1+BorderStyle.width(),
			  xp, PlotY1+BorderStyle.width() + TicsLen );
	else if ( TicsPos == In )
	  paint.drawLine( xp, PlotY1+1,
			  xp, PlotY1 + 1 - TicsLen );
	else
	  paint.drawLine( xp, PlotY1+1+BorderStyle.width()/2 - TicsLen/2,
			  xp, PlotY1+1+BorderStyle.width()/2 + TicsLen/2 );
      }
      if ( XTics[axis] > 1-axis ) {
	if ( TicsPos == Out )
	  paint.drawLine( xp, PlotY2-BorderStyle.width(),
			  xp, PlotY2-BorderStyle.width() - TicsLen );
	else if ( TicsPos == In )
	  paint.drawLine( xp, PlotY2-1,
			  xp, PlotY2 - 1 + TicsLen );
	else
	  paint.drawLine( xp, PlotY2-1-BorderStyle.width()/2 + TicsLen/2,
			  xp, PlotY2-1-BorderStyle.width()/2 - TicsLen/2 );
      }
    }
  }

  // y tic marks:
  if ( YTics[axis] && TicsLen > 0 && TicsWidth > 0 && TicsColor != Transparent ) {
    RGBColor c = color( TicsColor );
    QColor qcolor( c.red(), c.green(), c.blue() );
    paint.setPen( QPen( qcolor, TicsWidth ) );
    for ( double y=YTicsStart[axis]; y<=YMax[axis]; y+=YTicsIncr[axis] )	{
      if ( ::fabs( y ) < 0.001*YTicsIncr[axis] )
	y = 0.0;
      int yp = PlotY1 + (int)::rint( double(PlotY2-PlotY1)/(YMax[axis]-YMin[axis])*(y-YMin[axis]) );
      if ( YTics[axis] > axis  ) {
	if ( TicsPos == Out )
	  paint.drawLine( PlotX1-BorderStyle.width(), yp,
			  PlotX1-BorderStyle.width() - TicsLen, yp );
	else if ( TicsPos == In )
	  paint.drawLine( PlotX1, yp,
			  PlotX1 + TicsLen, yp );
	else
	  paint.drawLine( PlotX1-BorderStyle.width()/2 + TicsLen/2, yp,
			  PlotX1-BorderStyle.width()/2 - TicsLen/2, yp );
      }
      if ( YTics[axis] > 1-axis ) {
	if ( TicsPos == Out )
	  paint.drawLine( PlotX2+BorderStyle.width(), yp,
			  PlotX2+BorderStyle.width() + TicsLen, yp );
	else if ( TicsPos == In )
	  paint.drawLine( PlotX2, yp,
			  PlotX2 - TicsLen, yp );
	else
	  paint.drawLine( PlotX2+BorderStyle.width()/2 - TicsLen/2, yp,
			  PlotX2+BorderStyle.width()/2 + TicsLen/2, yp );
      }
    }
  }
}


void Plot::drawTicLabels( QPainter &paint, int axis )
{
  // x tic labels:
  if ( XTics[axis] && TicsLabelColor != Transparent ) {
    RGBColor c = color( TicsLabelColor );
    QColor qcolor( c.red(), c.green(), c.blue() );
    paint.setPen( QPen( qcolor ) );
    for ( double x=XTicsStart[axis]; x<=XMax[axis]; x+=XTicsIncr[axis] ) {
      if ( ::fabs( x ) < 0.001*XTicsIncr[axis] )
	x = 0.0;
      int xp = PlotX1 + (int)::rint( double(PlotX2-PlotX1)/(XMax[axis]-XMin[axis])*(x-XMin[axis]) );
      QString xt;
      xt.sprintf( XTicsFormat[axis].c_str(), x );
      int w = fontMetrics().width( xt );
      if ( axis == 1 )
	paint.drawText( xp-w/2, PlotY2-BorderStyle.width()-X2TicsLen-2, xt );
      else
	paint.drawText( xp-w/2, PlotY1+BorderStyle.width()+X1TicsMarg, xt );
    }
  }

  // y tic labels:
  if ( YTics[axis] && TicsLabelColor != Transparent ) {
    RGBColor c = color( TicsLabelColor );
    QColor qcolor( c.red(), c.green(), c.blue() );
    paint.setPen( qcolor );
    for ( double y=YTicsStart[axis]; y<=YMax[axis]; y+=YTicsIncr[axis] ) {
      if ( ::fabs( y ) < 0.001*YTicsIncr[axis] )
	y = 0.0;
      int yp = PlotY1 + (int)::rint( double(PlotY2-PlotY1)/(YMax[axis]-YMin[axis])*(y-YMin[axis]) );
      QString yt;
      yt.sprintf( YTicsFormat[axis].c_str(), y );
      int w = fontMetrics().width( yt );
      int h = (int)::ceil( TicsLabelSize*FontHeight );
      if ( axis == 1 )
	paint.drawText( PlotX2+BorderStyle.width()+Y2TicsLen+2, yp+h/2, yt );
      else
	paint.drawText( PlotX1-BorderStyle.width()-Y1TicsLen-2-w, yp+h/2, yt );
    }
  }
}


void Plot::drawAxis( QPainter &paint )
{
  for ( int k=0; k<MaxAxis; k++ ) {
    drawTicMarks( paint, k );
    drawTicLabels( paint, k );
  }
  paint.flush();
}


void Plot::drawLabel( QPainter &paint, const Label &label )
{
  if ( label.Text.length() > 0 && label.LColor != Transparent ) {
    int xp = xPixel( label );
    int yp = yPixel( label );
    int w = fontMetrics().width( label.Text.c_str() );
    paint.save();
    paint.translate( xp, yp );
    paint.rotate( label.Angle );

    if ( label.FColor != Transparent && label.FWidth > 0 ) {
      int fw = label.FWidth;
      int fw2 = label.FWidth / 2;
      RGBColor fc = color( label.FColor );
      QColor qfcolor( fc.red(), fc.green(), fc.blue() );
      paint.setPen( QPen( qfcolor, fw ) );
      if ( label.BColor != Transparent ) {
	RGBColor bc = Plot::color( label.BColor );
	QColor qbcolor( bc.red(), bc.green(), bc.blue() );
	paint.setBrush( QBrush( qbcolor ) );
      }
      else
	paint.setBrush( QBrush( Qt::black, Qt::NoBrush ) );
      if ( label.Just == Right )
	paint.drawRect( -w-fw2, -FontHeight-fw2, w+fw, FontSize+fw );
      else if ( label.Just == Center )
	paint.drawRect( -w/2-fw2, -FontHeight-fw2, w+fw, FontSize+fw );
      else
	paint.drawRect( -fw2, -FontHeight-fw2, w+fw, FontSize+fw );
    }
    else if ( label.BColor != Transparent ) {
      RGBColor bc = color( label.BColor );
      QColor qbcolor( bc.red(), bc.green(), bc.blue() );
      if ( label.Just == Right )
	paint.fillRect( -w, -FontHeight, w, FontSize, qbcolor );
      else if ( label.Just == Center )
	paint.fillRect( -w/2, -FontHeight, w, FontSize, qbcolor );
      else
	paint.fillRect( 0, -FontHeight, w, FontSize, qbcolor );
    }

    RGBColor c = color( label.LColor );
    QColor qcolor( c.red(), c.green(), c.blue() );
    paint.setPen( qcolor );
    if ( label.Just == Right )
      paint.drawText( -w, 0, label.Text.c_str() );
    else if ( label.Just == Center )
      paint.drawText( -w/2, 0, label.Text.c_str() );
    else
      paint.drawText( 0, 0, label.Text.c_str() );
    paint.restore();
  }
  paint.flush();
}


void Plot::drawLabels( QPainter &paint )
{
  for ( int k=0; k<MaxAxis; k++ ) {
    drawLabel( paint, XLabel[k] );
    drawLabel( paint, YLabel[k] );
  }
  drawLabel( paint, Title );
  for ( unsigned int k=0; k<Labels.size(); k++ )
    drawLabel( paint, Labels[k] );
}


void Plot::drawLine( QPainter &paint, DataElement *d )
{
  if ( d->Line.color() != Transparent && d->Line.width() > 0 ) {
    // set pen:
    RGBColor c = color( d->Line.color() );
    QColor qcolor( c.red(), c.green(), c.blue() );
    Qt::PenStyle dash = QtDash.find( d->Line.dash() )->second;
    paint.setPen( QPen( qcolor, d->Line.width(), dash ) );

    // axis:
    int xaxis = d->XAxis;
    int yaxis = d->YAxis;
    
    // init data:
    // XXX needs to be replicated in drawPoints()!
    long f = 0;
    long l = 0;
    if ( NewData ) {
      f = d->first( XMin[xaxis], YMin[yaxis], XMax[xaxis], YMax[yaxis] );
      l = d->last( XMin[xaxis], YMin[yaxis], XMax[xaxis], YMax[yaxis] );
    }
    else {
      if ( ShiftData ) {
	if ( ShiftX[xaxis] > 0.0 ) {
	  f = d->first( XMax[xaxis]-ShiftX[xaxis], YMin[yaxis], XMax[xaxis], YMax[yaxis] );
	  l = d->last( XMax[xaxis]-ShiftX[xaxis], YMin[yaxis], XMax[xaxis], YMax[yaxis] );
	}
	else {
	  f = d->first( XMin[xaxis], YMin[yaxis], XMin[xaxis]-ShiftX[xaxis], YMax[yaxis] );
	  l = d->last( XMin[xaxis], YMin[yaxis], XMin[xaxis]-ShiftX[xaxis], YMax[yaxis] );
	}
      }
      else {
	f = d->lineIndex();
	l = d->last( XMin[xaxis], YMin[yaxis], XMax[xaxis], YMax[yaxis] );
      }
    }
    if ( f >= l )
      return;
    long k = f;
    double x, y;
    double ox = 0.0, oy = 0.0;
    double nx, ny;
    bool previn = true;
    double slope;
    int xp, yp;
    
    // find first point inside plot:
    d->point( k, x, y );
    for ( k++; 
	  k<l && ( x < XMin[xaxis] || x > XMax[xaxis] || y < YMin[yaxis] || y > YMax[yaxis] ); 
	  k++ ) {
      ox = x;
      oy = y;
      d->point( k, x, y );
      // missing: what's about lines intersecting the plot but with both
      // sides outside the plot?
      // if ( intersect ) { drawLine() };
    }

    // draw first point:
    if ( k > f+1 && k < l ) {
      // previous point is outside plot
      // find margin point:
      if ( ox != x ) {
	slope = (oy-y)/(ox-x);
	if ( ox < XMin[xaxis] ) {
	  ox = XMin[xaxis];
	  oy = slope*(ox-x)+y;
	}
	else if ( ox > XMax[xaxis] ) {
	  ox = XMax[xaxis];
	  oy = slope*(ox-x)+y;
	}
	if ( oy < YMin[yaxis] ) {
	  oy = YMin[yaxis];
	  ox = (oy-y)/slope+x;
	}
	else if ( oy > YMax[yaxis] ) {
	  oy = YMax[yaxis];
	  ox = (oy-y)/slope+x;
	}
      }
      else {
	if ( oy < YMin[yaxis] )
	  oy = YMin[yaxis];
	else if ( oy > YMax[yaxis] )
	  oy = YMax[yaxis];
      }
      // move to margin point:
      xp = PlotX1 + (int)::rint( double(PlotX2-PlotX1)/(XMax[xaxis]-XMin[xaxis])*(ox-XMin[xaxis]) );
      yp = PlotY1 + (int)::rint( double(PlotY2-PlotY1)/(YMax[yaxis]-YMin[yaxis])*(oy-YMin[yaxis]) );
      paint.moveTo( xp, yp );
      // draw line to first point:
      xp = PlotX1 + (int)::rint( double(PlotX2-PlotX1)/(XMax[xaxis]-XMin[xaxis])*(x-XMin[xaxis]) );
      yp = PlotY1 + (int)::rint( double(PlotY2-PlotY1)/(YMax[yaxis]-YMin[yaxis])*(y-YMin[yaxis]) );
      paint.lineTo( xp, yp );
    }
    else {
      // this is the first point to draw
      xp = PlotX1 + (int)::rint( double(PlotX2-PlotX1)/(XMax[xaxis]-XMin[xaxis])*(x-XMin[xaxis]) );
      yp = PlotY1 + (int)::rint( double(PlotY2-PlotY1)/(YMax[yaxis]-YMin[yaxis])*(y-YMin[yaxis]) );
      paint.moveTo( xp, yp );
    }

    // draw remaining line:
    for ( ; k<l; k++ ) {
      ox = x;
      oy = y;
      d->point( k, x, y );
      if ( x < XMin[xaxis] || x > XMax[xaxis] || y < YMin[yaxis] || y > YMax[yaxis] ) {
	// this point is outside the plot
	if ( previn ) {
	  // the previous point was inside the plot
	  // find margin point:
	  if ( ox != x ) {
	    slope = (oy-y)/(ox-x);
	    if ( x < XMin[xaxis] ) {
	      nx = XMin[xaxis];
	      ny = slope*(nx-x)+y;
	    }
	    else if ( x > XMax[xaxis] ) {
	      nx = XMax[xaxis];
	      ny = slope*(nx-x)+y;
	    }
	    else {
	      nx = x;
	      ny = y;
	    }
	    if ( ny < YMin[yaxis] ) {
	      ny = YMin[yaxis];
	      nx = (ny-y)/slope+x;
	    }
	    else if ( ny > YMax[yaxis] ) {
	      ny = YMax[yaxis];
	      nx = (ny-y)/slope+x;
	    }
	  }
	  else {
	    nx = x;
	    ny = y;
	    if ( ny < YMin[yaxis] )
	      ny = YMin[yaxis];
	    else if ( ny > YMax[yaxis] )
	      ny = YMax[yaxis];
	  }
	  // line to margin point:
	  xp = PlotX1 + (int)::rint( double(PlotX2-PlotX1)/(XMax[xaxis]-XMin[xaxis])*(nx-XMin[xaxis]) );
	  yp = PlotY1 + (int)::rint( double(PlotY2-PlotY1)/(YMax[yaxis]-YMin[yaxis])*(ny-YMin[yaxis]) );
	  paint.lineTo( xp, yp );
	}
	else {
	  // the previous point was outside the plot, too
	  // check for intersection!
	}
	previn = false;
      }
      else {
	// this point is inside the plot
	if ( !previn ) {
	  // the previous point was outside the plot
	  // find margin point:
	  if ( ox != x ) {
	    slope = (oy-y)/(ox-x);
	    if ( ox < XMin[xaxis] ) {
	      ox = XMin[xaxis];
	      oy = slope*(ox-x)+y;
	    }
	    else if ( ox > XMax[xaxis] ) {
	      ox = XMax[xaxis];
	      oy = slope*(ox-x)+y;
	    }
	    if ( oy < YMin[yaxis] ) {
	      oy = YMin[yaxis];
	      ox = (oy-y)/slope+x;
	    }
	    else if ( oy > YMax[yaxis] ) {
	      oy = YMax[yaxis];
	      ox = (oy-y)/slope+x;
	    }
	  }
	  else {
	    if ( oy < YMin[yaxis] )
	      oy = YMin[yaxis];
	    else if ( oy > YMax[yaxis] )
	      oy = YMax[yaxis];
	  }
	  // move to margin point:
	  xp = PlotX1 + (int)::rint( double(PlotX2-PlotX1)/(XMax[xaxis]-XMin[xaxis])*(ox-XMin[xaxis]) );
	  yp = PlotY1 + (int)::rint( double(PlotY2-PlotY1)/(YMax[yaxis]-YMin[yaxis])*(oy-YMin[yaxis]) );
	  paint.moveTo( xp, yp );
	}
	// line to the point:
	xp = PlotX1 + (int)::rint( double(PlotX2-PlotX1)/(XMax[xaxis]-XMin[xaxis])*(x-XMin[xaxis]) );
	yp = PlotY1 + (int)::rint( double(PlotY2-PlotY1)/(YMax[yaxis]-YMin[yaxis])*(y-YMin[yaxis]) );
	paint.lineTo( xp, yp );
	previn = true;
      }
    }
    paint.flush();
    d->setLineIndex( l );
  }
}


void Plot::drawPoints( QPainter &paint, DataElement *d )
{
  if ( ( d->Point.color() != Transparent || 
	 d->Point.fillColor() != Transparent ) && 
       ( d->Point.size() > 0 || d->Point.type() == Box ) ) {
    // single point pixmap:
    int offs = d->Point.size();
    if ( offs <= 0 )
      offs = 1;
    QPixmap point( 2*offs, 2*offs );
    QPainter ppaint( &point );
    QBitmap mask( 2*offs, 2*offs );
    mask.fill( Qt::color0 );
    QPainter mpaint( &mask );

    // set pen:
    if ( d->Point.color() != Transparent ) {
      RGBColor c = Plot::color( d->Point.color() );
      QColor qcolor( c.red(), c.green(), c.blue() );
      paint.setPen( QPen( qcolor, d->Line.width(), Qt::SolidLine ) );
      ppaint.setPen( QPen( qcolor, d->Line.width(), Qt::SolidLine ) );
      mpaint.setPen( QPen( Qt::color1, d->Line.width(), Qt::SolidLine ) );
    }
    else {
      paint.setPen( QPen( Qt::black, 0, Qt::NoPen ) );
      ppaint.setPen( QPen( Qt::black, 0, Qt::NoPen ) );
      mpaint.setPen( QPen( Qt::color0, 0, Qt::NoPen ) );
    }
    
    // set brush:
    if ( d->Point.fillColor() != Transparent ) {
      RGBColor c = Plot::color( d->Point.fillColor() );
      QColor qcolor( c.red(), c.green(), c.blue() );
      paint.setBrush( QBrush( qcolor ) );
      ppaint.setBrush( QBrush( qcolor ) );
      mpaint.setBrush( QBrush( Qt::color1 ) );
    }
    else {
      paint.setBrush( QBrush( Qt::black, Qt::NoBrush ) );
      ppaint.setBrush( QBrush( Qt::black, Qt::NoBrush ) );
      mpaint.setBrush( QBrush( Qt::color0 ) );
    }

    // axis:
    int xaxis = d->XAxis;
    int yaxis = d->YAxis;

    // index range:
    long f = d->first( XMin[xaxis], YMin[yaxis], XMax[xaxis], YMax[yaxis] );
    long l = d->last( XMin[xaxis], YMin[yaxis], XMax[xaxis], YMax[yaxis] );
    if ( ! NewData )
      f = d->pointIndex();

    // draw Box:
    if ( d->Point.type() == Box ) {
      // XXX improve single point plot!
      if ( l-f >= 2 ) {
	double x, y;
	int WP = d->Point.size();
	if ( WP <= 0 ) {
	  double x1, x2;
	  d->point( f, x1, y );
	  d->point( l-1, x2, y );
	  double w = (x2-x1)/(l-f-1);
	  WP = (int)::rint( double(PlotX2-PlotX1)/(XMax[xaxis]-XMin[xaxis])*w );
	}
	int WPL = WP/2;
	int WPR = WP - WPL;
	y = 0.0;
	if ( y < YMin[yaxis] )
	  y = YMin[yaxis];
	int Y0 = PlotY1 + (int)::rint( double(PlotY2-PlotY1)/(YMax[yaxis]-YMin[yaxis])*(y-YMin[yaxis]) );
	QPointArray pa( 4 );
	for ( long k=f; k<l; k++ ) {
	  d->point( k, x, y );
	  if ( XMin[xaxis] <= x && XMax[xaxis] >= x && YMin[yaxis] <= y ) {
	    if ( y > YMax[yaxis] )
	      y = YMax[yaxis];
	    int xp = PlotX1 + (int)::rint( double(PlotX2-PlotX1)/(XMax[xaxis]-XMin[xaxis])*(x-XMin[xaxis]) );
	    int yp = PlotY1 + (int)::rint( double(PlotY2-PlotY1)/(YMax[yaxis]-YMin[yaxis])*(y-YMin[yaxis]) );
	    pa.setPoint( 0, xp - WPL, yp );
	    pa.setPoint( 1, xp + WPR, yp );
	    pa.setPoint( 2, xp + WPR, Y0 );
	    pa.setPoint( 3, xp - WPL, Y0 );
	    paint.drawPolygon( pa );
	  }
	}
      }
    }
    else {
      // draw point:
      switch ( d->Point.type() ) {

      case Circle: {
	int r = (int)::rint( d->Point.size()*0.564 );
	ppaint.drawEllipse( offs - r, offs - r, 2*r, 2*r );
	mpaint.drawEllipse( offs - r, offs - r, 2*r, 2*r );
      }
	break;

      case CircleDot: {
	int r = (int)::rint( d->Point.size()*0.564 );
	ppaint.drawEllipse( offs - r, offs - r, 2*r, 2*r );
	ppaint.drawPoint( offs, offs ); 
	mpaint.drawEllipse( offs - r, offs - r, 2*r, 2*r );
	mpaint.drawPoint( offs, offs ); 
      }
	break;

      case Diamond: {
	QPointArray pa( 4 );
	int c = (int)::rint( d->Point.size() / sqrt( 2.0 ) );
	pa.setPoint( 0, offs - c, offs );
	pa.setPoint( 1, offs, offs + c );
	pa.setPoint( 2, offs + c, offs );
	pa.setPoint( 3, offs, offs - c );
	ppaint.drawPolygon( pa );
	mpaint.drawPolygon( pa );
      }
	break;

      case DiamondDot: {
	QPointArray pa( 4 );
	int c = (int)::rint( d->Point.size() / sqrt( 2.0 ) );
	pa.setPoint( 0, offs - c, offs );
	pa.setPoint( 1, offs, offs + c );
	pa.setPoint( 2, offs + c, offs );
	pa.setPoint( 3, offs, offs - c );
	ppaint.drawPolygon( pa );
	ppaint.drawPoint( offs, offs ); 
	mpaint.drawPolygon( pa );
	mpaint.drawPoint( offs, offs ); 
      }
	break;

      case Square: {
	int r = d->Point.size()/2;
	ppaint.drawRect( offs - r, offs - r, 
			 d->Point.size(), d->Point.size() );
	mpaint.drawRect( offs - r, offs - r, 
			 d->Point.size(), d->Point.size() );
      }
	break;

      case SquareDot: {
	int r = d->Point.size()/2;
	ppaint.drawRect( offs - r, offs - r, 
			 d->Point.size(), d->Point.size() );
	ppaint.drawPoint( offs, offs ); 
	mpaint.drawRect( offs - r, offs - r, 
			 d->Point.size(), d->Point.size() );
	mpaint.drawPoint( offs, offs ); 
      }
	break;

      case TriangleUp: {
	QPointArray pa( 3 );
	int a = (int)::rint( d->Point.size() / sqrt( sqrt( 3.0 ) ) );
	int c = (int)::rint( d->Point.size() * sqrt( sqrt( 3.0 ) ) / 3 );
	pa.setPoint( 0, offs - a, offs + c );
	pa.setPoint( 1, offs, offs - 2*c );
	pa.setPoint( 2, offs + a, offs + c );
	ppaint.drawPolygon( pa );
	mpaint.drawPolygon( pa );
      }
	break;

      case TriangleUpDot: {
	QPointArray pa( 3 );
	int a = (int)::rint( d->Point.size() / sqrt( sqrt( 3.0 ) ) );
	int c = (int)::rint( d->Point.size() * sqrt( sqrt( 3.0 ) ) / 3.0 );
	pa.setPoint( 0, offs - a, offs + c );
	pa.setPoint( 1, offs, offs - 2*c );
	pa.setPoint( 2, offs + a, offs + c );
	ppaint.drawPolygon( pa );
	ppaint.drawPoint( offs, offs ); 
	mpaint.drawPolygon( pa );
	mpaint.drawPoint( offs, offs ); 
      }
	break;

      case TriangleDown: {
	QPointArray pa( 3 );
	int a = (int)::rint( d->Point.size() / sqrt( sqrt( 3.0 ) ) );
	int c = (int)::rint( d->Point.size() * sqrt( sqrt( 3.0 ) ) / 3 );
	pa.setPoint( 0, offs - a, offs - c );
	pa.setPoint( 1, offs, offs + 2*c );
	pa.setPoint( 2, offs + a, offs - c );
	ppaint.drawPolygon( pa );
	mpaint.drawPolygon( pa );
      }
	break;

      case TriangleDownDot: {
	QPointArray pa( 3 );
	int a = (int)::rint( d->Point.size() / sqrt( sqrt( 3.0 ) ) );
	int c = (int)::rint( d->Point.size() * sqrt( sqrt( 3.0 ) ) / 3 );
	pa.setPoint( 0, offs - a, offs - c );
	pa.setPoint( 1, offs, offs + 2*c );
	pa.setPoint( 2, offs + a, offs - c );
	ppaint.drawPolygon( pa );
	ppaint.drawPoint( offs, offs ); 
	mpaint.drawPolygon( pa );
	mpaint.drawPoint( offs, offs ); 
      }
	break;

      case TriangleLeft: {
	QPointArray pa( 3 );
	int a = (int)::rint( d->Point.size() / sqrt( sqrt( 3.0 ) ) );
	int c = (int)::rint( d->Point.size() * sqrt( sqrt( 3.0 ) ) / 3 );
	pa.setPoint( 0, offs + c, offs - a );
	pa.setPoint( 1, offs - 2*c, offs );
	pa.setPoint( 2, offs + c, offs + a );
	ppaint.drawPolygon( pa );
	mpaint.drawPolygon( pa );
      }
	break;

      case TriangleLeftDot: {
	QPointArray pa( 3 );
	int a = (int)::rint( d->Point.size() / sqrt( sqrt( 3.0 ) ) );
	int c = (int)::rint( d->Point.size() * sqrt( sqrt( 3.0 ) ) / 3 );
	pa.setPoint( 0, offs + c, offs - a );
	pa.setPoint( 1, offs - 2*c, offs );
	pa.setPoint( 2, offs + c, offs + a );
	ppaint.drawPolygon( pa );
	ppaint.drawPoint( offs, offs ); 
	mpaint.drawPolygon( pa );
	mpaint.drawPoint( offs, offs ); 
      }
	break;

      case TriangleRight: {
	QPointArray pa( 3 );
	int a = (int)::rint( d->Point.size() / sqrt( sqrt( 3.0 ) ) );
	int c = (int)::rint( d->Point.size() * sqrt( sqrt( 3.0 ) ) / 3 );
	pa.setPoint( 0, offs - c, offs - a );
	pa.setPoint( 1, offs + 2*c, offs );
	pa.setPoint( 2, offs - c, offs + a );
	ppaint.drawPolygon( pa );
	mpaint.drawPolygon( pa );
      }
	break;

      case TriangleRightDot: {
	QPointArray pa( 3 );
	int a = (int)::rint( d->Point.size() / sqrt( sqrt( 3.0 ) ) );
	int c = (int)::rint( d->Point.size() * sqrt( sqrt( 3.0 ) ) / 3 );
	pa.setPoint( 0, offs - c, offs - a );
	pa.setPoint( 1, offs + 2*c, offs );
	pa.setPoint( 2, offs - c, offs + a );
	ppaint.drawPolygon( pa );
	ppaint.drawPoint( offs, offs ); 
	mpaint.drawPolygon( pa );
	mpaint.drawPoint( offs, offs ); 
      }
	break;

      case TriangleNorth: {
	QPointArray pa( 3 );
	int a = (int)::rint( offs / sqrt( 3.0 ) );
	pa.setPoint( 0, offs - a, offs );
	pa.setPoint( 1, offs, offs + offs );
	pa.setPoint( 2, offs + a, offs );
	ppaint.drawPolygon( pa );
	mpaint.drawPolygon( pa );
      }
	break;

      case TriangleSouth: {
	QPointArray pa( 3 );
	int a = (int)::rint( offs / sqrt( 3.0 ) );
	pa.setPoint( 0, offs - a, offs );
	pa.setPoint( 1, offs, offs - offs );
	pa.setPoint( 2, offs + a, offs );
	ppaint.drawPolygon( pa );
	mpaint.drawPolygon( pa );
      }
	break;

      case TriangleWest: {
	QPointArray pa( 3 );
	int a = (int)::rint( offs / sqrt( 3.0 ) );
	pa.setPoint( 0, offs, offs - a );
	pa.setPoint( 1, offs + offs, offs );
	pa.setPoint( 2, offs, offs + a );
	ppaint.drawPolygon( pa );
	mpaint.drawPolygon( pa );
      }
	break;

      case TriangleEast: {
	QPointArray pa( 3 );
	int a = (int)::rint( offs / sqrt( 3.0 ) );
	pa.setPoint( 0, offs, offs - a );
	pa.setPoint( 1, offs - offs, offs );
	pa.setPoint( 2, offs, offs + a );
	ppaint.drawPolygon( pa );
	mpaint.drawPolygon( pa );
      }
	break;

      case CircleNorth: {
	int r = (int)::rint( offs * 0.606 ); // *sqrt( 2 / pi / sqrt( 3 ) )
	ppaint.drawPie( offs - r, offs - r, 2*r, 2*r, 16*180+1, 16*180-2 );
	mpaint.drawPie( offs - r, offs - r, 2*r, 2*r, 16*180+1, 16*180-2 );
      }
	break;

      case CircleSouth: {
	int r = (int)::rint( offs * 0.606 ); // *sqrt( 2 / pi / sqrt( 3 ) )
	ppaint.drawPie( offs - r, offs - r, 2*r, 2*r, 0, 16*180+1 );
	mpaint.drawPie( offs - r, offs - r, 2*r, 2*r, 0, 16*180+1 );
      }
	break;

      case CircleWest: {
	int r = (int)::rint( offs * 0.606 ); // *sqrt( 2 / pi / sqrt( 3 ) )
	ppaint.drawPie( offs - r, offs - r, 2*r, 2*r, -16*90, 16*180+1 );
	mpaint.drawPie( offs - r, offs - r, 2*r, 2*r, -16*90, 16*180+1 );
      }
	break;

      case CircleEast: {
	int r = (int)::rint( offs * 0.606 ); // *sqrt( 2 / pi / sqrt( 3 ) )
	ppaint.drawPie( offs - r, offs - r, 2*r, 2*r, 16*90, 16*180+1 );
	mpaint.drawPie( offs - r, offs - r, 2*r, 2*r, 16*90, 16*180+1 );
      }
	break;

      case SquareNorth: {
	int r = (int)::rint( offs / sqrt( sqrt( 3.0 ) ) );
	ppaint.drawRect( offs - r/2, offs, r, r );
	mpaint.drawRect( offs - r/2, offs, r, r );
      }
	break;

      case SquareSouth: {
	int r = (int)::rint( offs / sqrt( sqrt( 3.0 ) ) );
	ppaint.drawRect( offs - r/2, offs, r, -r );
	mpaint.drawRect( offs - r/2, offs, r, -r );
      }
	break;

      case SquareWest: {
	int r = (int)::rint( offs / sqrt( sqrt( 3.0 ) ) );
	ppaint.drawRect( offs, offs - r/2, r, r );
	mpaint.drawRect( offs, offs - r/2, r, r );
      }
	break;

      case SquareEast: {
	int r = (int)::rint( offs / sqrt( sqrt( 3.0 ) ) );
	ppaint.drawRect( 0, offs - r/2, r, r );
	mpaint.drawRect( 0, offs - r/2, r, r );
      }
	break;

      case Dot:
	ppaint.drawPoint( offs, offs ); 
	mpaint.drawPoint( offs, offs ); 
	break;

      case StrokeUp:
	ppaint.drawLine( offs, offs, offs, offs - d->Point.size() ); 
	mpaint.drawLine( offs, offs, offs, offs - d->Point.size() ); 
	break;

      case StrokeVertical: {
	int r = d->Point.size()/2;
	ppaint.drawLine( offs, offs - r, offs, offs + r ); 
	mpaint.drawLine( offs, offs - r, offs, offs + r ); 
      }
	break;

      case StrokeHorizontal: {
	int r = d->Point.size()/2;
	ppaint.drawLine( offs - r, offs, offs + r, offs ); 
	mpaint.drawLine( offs - r, offs, offs + r, offs ); 
      }
	break;

      default:
	cerr << "point type not supported!\n";
      }
      point.setMask( mask );

      // draw points:
      for ( long k=f; k<l; k++ ) {
	double x, y;
	d->point( k, x, y );
	if ( XMin[xaxis] <= x && XMax[xaxis] >= x && YMin[yaxis] <= y && YMax[yaxis] >= y ) {
	  int xp = PlotX1 + (int)::rint( double(PlotX2-PlotX1)/(XMax[xaxis]-XMin[xaxis])*(x-XMin[xaxis]) );
	  int yp = PlotY1 + (int)::rint( double(PlotY2-PlotY1)/(YMax[yaxis]-YMin[yaxis])*(y-YMin[yaxis]) );
	  paint.drawPixmap( xp-offs, yp-offs, point );
	}
      }

    }
    paint.flush();
    d->setPointIndex( l );
  }
}


void Plot::drawData( QPainter &paint )
{
  for ( PDataType::iterator d = PData.begin(); d != PData.end(); ++d ) {
    drawLine( paint, *d );
    drawPoints( paint, *d );
  }
}


void Plot::drawMouse( QPainter &paint )
{
  if ( MouseDrawRect ) {
    QColor qcolor( 192, 192, 192 ); // 75% gray
    paint.setPen( QPen( qcolor, 1, Qt::DotLine ) );
    paint.setBrush( QBrush( Qt::black, Qt::NoBrush ) );
    if ( MouseXMax ) {
      paint.drawLine( PlotX1, MouseY1, PlotX2, MouseY1 );
      paint.drawLine( PlotX1, MouseY2, PlotX2, MouseY2 );
      paint.drawLine( MouseX1, MouseY1, MouseX1, MouseY2 );
    }
    else if ( MouseYMax ) {
      paint.drawLine( MouseX1, PlotY1, MouseX1, PlotY2 );
      paint.drawLine( MouseX2, PlotY1, MouseX2, PlotY2 );
      paint.drawLine( MouseX1, MouseY1, MouseX2, MouseY1 );
    }
    else {
      int x = MouseX1;
      int w = MouseX2 - MouseX1 + 1;
      if ( MouseX2 < MouseX1 ) {
	x = MouseX2;
	w = MouseX1 - MouseX2 + 1;
      }
      int y = MouseY1;
      int h = MouseY2 - MouseY1 + 1;
      if ( MouseY2 < MouseY1 ) {
	y = MouseY2;
	h = MouseY1 - MouseY2 + 1;
      }
      paint.drawRect( x, y, w, h );
    }
    paint.flush();
  }
  else if ( ! MouseXPos.empty() ) {
    vector< double > xpos( MouseXPos.size() );
    vector< double > ypos( MouseXPos.size() );
    QColor qcolor( 192, 192, 192 ); // 75% gray
    paint.setPen( QPen( qcolor, 1, Qt::SolidLine ) );
    paint.setBrush( QBrush( Qt::black, Qt::NoBrush ) );
    // draw cross for all points:
    for ( unsigned int k=0; k<MouseXPos.size(); k++ ) {
      xpos[k] = MouseXPos[k];
      ypos[k] = MouseYPos[k];
      int xaxis = 0;
      int yaxis = 0;
      if ( xpos[k] == AutoScale || ypos[k] == AutoScale ) {
	if ( MouseDInx[k] < 0 ||
	     MouseDInx[k] >= (int)PData.size() )
	  continue;
	int xdaxis = PData[MouseDInx[k]]->XAxis;
	int ydaxis = PData[MouseDInx[k]]->YAxis;
	if ( MousePInx[k] < 0 ||
	     MousePInx[k] >= PData[MouseDInx[k]]->last( XMin[xdaxis], YMin[ydaxis],
							XMax[xdaxis], YMax[ydaxis] ) )
	  continue;
	double x, y;
	PData[MouseDInx[k]]->point( MousePInx[k], x, y );
	if ( xpos[k] == AutoScale ) {
	  xpos[k] = x;
	  xaxis = xdaxis;
	}
	if ( ypos[k] == AutoScale ) {
	  ypos[k] = y;
	  yaxis = ydaxis;
	}
      }
      int xpix = (int)::rint( double( PlotX2 - PlotX1 ) / ( XMax[xaxis] - XMin[xaxis] ) * ( xpos[k] - XMin[xaxis] ) + PlotX1 );
      int ypix = (int)::rint( double( PlotY2 - PlotY1 ) / ( YMax[yaxis] - YMin[yaxis] ) * ( ypos[k] - YMin[yaxis] ) + PlotY1 );
      paint.drawLine( xpix, PlotY1, xpix, PlotY2 );
      paint.drawLine( PlotX1, ypix, PlotX2, ypix );
    }
    RGBColor pc = color( PlotColor );
    QColor qpcolor( pc.red(), pc.green(), pc.blue() );

    // print coordinates of current point:
    if ( ! MouseXPos.empty() && 
	 xpos.back() != AutoScale && ypos.back() != AutoScale ) {
      Str xlabel = XTicsFormat[0];
      xlabel.format( xpos.back() );
      Str ylabel = YTicsFormat[0];
      ylabel.format( ypos.back() );
      string label = "(" + xlabel + " | " + ylabel + ")";
      int offs = 2;
      int lx = PlotX1 + offs;
      int ly = PlotY1 + offs;
      int flags = Qt::AlignTop | Qt::AlignLeft | Qt::DontClip;
      QRect lr = paint.boundingRect( lx, ly, 300, FontSize, 
				     flags, label.c_str() );
      QRect fr = lr;
      fr.addCoords( -offs, -offs, offs, offs );
      paint.fillRect( fr, qpcolor );
      paint.drawText( lr, flags, label.c_str() );
    }

    // print differences of last two points:
    if ( MouseXPos.size() > 1 && 
	 xpos[xpos.size()-1] != AutoScale && ypos[ypos.size()-1] != AutoScale &&
	 xpos[xpos.size()-2] != AutoScale && ypos[ypos.size()-2] != AutoScale ) {
      Str xlabel = XTicsFormat[0];
      xlabel.format( xpos[xpos.size()-1] - xpos[xpos.size()-2] );
      Str ylabel = YTicsFormat[0];
      ylabel.format( ypos[ypos.size()-1] - ypos[ypos.size()-2] );
      string label = "D=" + xlabel + " | " + ylabel;
      int offs = 2;
      int lx = PlotX2 - offs;
      int ly = PlotY1 + offs;
      int flags = Qt::AlignTop | Qt::AlignRight | Qt::DontClip;
      QRect lr = paint.boundingRect( lx, ly, 0, FontSize, 
				     flags, label.c_str() );
      QRect fr = lr;
      fr.addCoords( -offs, -offs, offs, offs );
      paint.fillRect( fr, qpcolor );
      paint.drawText( lr, flags, label.c_str() );
    }

    paint.flush();
  }
}


void Plot::draw( QPaintDevice *qpm )
{
  // the order of locking is important here!
  // if the data are not available there is no need to lock the plot.
  if ( ! SubWidget )
    lockData();
  PMutex.lock();

  QColor pbc = paletteBackgroundColor();
  Colors[WidgetBackground] = RGBColor( pbc.red(), pbc.green(), pbc.blue() );

  init();
  initRange();
  initTics();
  initBorder();
  initLines();

  // check for changes:
  ShiftData = false;
  ShiftXPix = 0;
  for ( int k=0; k<MaxAxis; k++ ) {
    ShiftX[k] = 0.0;
    // check whether axis is in use:
    bool havexaxis = false;
    bool haveyaxis = false;
    for ( PDataType::iterator d = PData.begin(); d != PData.end(); ++d ) {
      if ( (*d)->XAxis == k ) {
	havexaxis = true;
	if ( haveyaxis )
	  break;
      }
      if ( (*d)->YAxis == k ) {
	haveyaxis = true;
	if ( havexaxis )
	  break;
      }
    }

    if ( havexaxis ) {
      if ( ::fabs( ::fabs( XMax[k] - XMin[k] ) - ::fabs( XMaxPrev[k] - XMinPrev[k] ) ) > 1.0e-8 )
	NewData = true;
      else {
	if ( XMin[k] != XMinPrev[k] ) {
	  ShiftData = true;
	  int dx = (int)::rint( double(PlotX2-PlotX1)/(XMax[k]-XMin[k])*(XMin[k]-XMinPrev[k]) );
	  if ( ShiftXPix == 0 )
	    ShiftXPix = dx;
	  else if ( dx != ShiftXPix )
	    NewData = true;
	  ShiftX[k] = XMin[k]-XMinPrev[k];
	}
	else {
	  if ( ShiftData )
	    NewData = true;
	}
      }
    }

    if ( haveyaxis ) {
      if ( YMin[k] != YMinPrev[k] )
	NewData = true;
      if ( YMax[k] != YMaxPrev[k] )
	NewData = true;
    }
  }

  // XXX remove, once thes optimiziation for shifted data is working:
  if ( ShiftData ) {
    ShiftData = false;
    NewData = true;
  }

  if ( ! NewData && ShiftData ) {
    if ( ShiftXPix >= PlotX2 - PlotX1 )
      NewData = true;
    else {
      if ( ShiftX > 0 ) {
	QPixmap pxm( PlotX2-PlotX1+1-ShiftXPix, PlotY2-PlotY1 );
	bitBlt( &pxm, 0, 0,
		qpm, PlotX1+ShiftXPix, PlotY1, PlotX2-PlotX1+1-ShiftXPix, PlotY2-PlotY1,
		CopyROP, true );
	bitBlt( qpm, PlotX1, PlotY1,
		&pxm, 0, 0, PlotX2-PlotX1+1-ShiftXPix, PlotY2-PlotY1,
		CopyROP, true );
      }
      else {
	QPixmap pxm( PlotX2-PlotX1+1+ShiftXPix, PlotY2-PlotY1 );
	bitBlt( &pxm, 0, 0,
		qpm, PlotX1, PlotY1, PlotX2-PlotX1+1+ShiftXPix, PlotY2-PlotY1,
		CopyROP, true );
	bitBlt( qpm, PlotX1-ShiftXPix, PlotY1,
		&pxm, 0, 0, PlotX2-PlotX1+1+ShiftXPix, PlotY2-PlotY1,
		CopyROP, true );
      }
    }
  }

  QPainter paint( qpm );
  // the painter coordinate system has its origin in 
  // the upper left corner of the widget!
  if ( NewData || ShiftData ) {
    drawBorder( paint );
    drawAxis( paint );
  }
  drawData( paint );
  if ( NewData || ShiftData )
    drawLabels( paint );
  drawMouse( paint );

  // remember current ranges:
  for ( int k=0; k<MaxAxis; k++ ) {
    XMinPrev[k] = XMin[k];
    XMaxPrev[k] = XMax[k];
    YMinPrev[k] = YMin[k];
    YMaxPrev[k] = YMax[k];
  }
  NewData = false;

  PMutex.unlock();
  if ( ! SubWidget )
    unlockData();
}


void Plot::draw( void )
{
  if ( SubWidget ) {
    if ( MP != 0 )
      MP->draw();
  }
  else {
    QApplication::postEvent( this, new QPaintEvent( rect(), false ) );
  }
}


void Plot::paintEvent( QPaintEvent *qpe )
{
  if ( !SubWidget ) {
    draw( PixMap );
    PMutex.lock();
    bitBlt( this, 0, 0, PixMap, 0, 0, PixMap->width(), PixMap->height() );
    PMutex.unlock();
  }
}


Plot::RangeCopy::RangeCopy( void ) 
{
  for ( int k=0; k<MaxAxis; k++ ) {
    XMin[k] = -10.0;
    XMax[k] = 10.0;
    YMin[k] = -10.0;
    YMax[k] = 10.0;
  }
}


Plot::RangeCopy::RangeCopy( const RangeCopy &rc ) 
{
  for ( int k=0; k<MaxAxis; k++ ) {
    XMin[k] = rc.XMin[k];
    XMax[k] = rc.XMax[k];
    YMin[k] = rc.YMin[k];
    YMax[k] = rc.YMax[k];
  }
}


Plot::RangeCopy::RangeCopy( const Plot *p ) 
{
  for ( int k=0; k<MaxAxis; k++ ) {
    XMin[k] = p->XMinRange[k];
    XMax[k] = p->XMaxRange[k];
    YMin[k] = p->YMinRange[k];
    YMax[k] = p->YMaxRange[k];
  }
}


bool Plot::ranges( void ) const
{
  return ( ! MouseRangeStack.empty() );
}


void Plot::pushRanges( void )
{
  MouseRangeStack.push_back( RangeCopy( this ) );
}


void Plot::popRanges( void )
{
  if ( ! MouseRangeStack.empty() ) {
    for ( int k=0; k<MaxAxis; k++ ) {
      XMinRange[k] = XMin[k] = MouseRangeStack.back().XMin[k];
      XMaxRange[k] = XMax[k] = MouseRangeStack.back().XMax[k];
      YMinRange[k] = YMin[k] = MouseRangeStack.back().YMin[k];
      YMaxRange[k] = YMax[k] = MouseRangeStack.back().YMax[k];
    }
    MouseRangeStack.pop_back();
  }
}


Plot::MouseEvent::MouseEvent( void )
  : XPixel( 0xffff ),
    YPixel( 0xffff ),
    XCoor( Plot::First ),
    YCoor( Plot::First ), 
    Mode( 0 ),
    Init( false ),
    Used( false )
{
  for ( int k=0; k<MaxAxis; k++ ) {
    XPos[k] = Plot::AutoScale;
    YPos[k] = Plot::AutoScale;
  }
}


void Plot::MouseEvent::clear( void )
{
  XPixel = 0xffff;
  YPixel = 0xffff; 
  for ( int k=0; k<MaxAxis; k++ ) {
    XPos[k] = Plot::AutoScale;
    YPos[k] = Plot::AutoScale;
  }
  Mode = 0;
}


void Plot::readMouse( QMouseEvent *qme, MouseEvent &me, bool move )
{
  me.XPixel = qme->x();
  for ( int k=0; k<MaxAxis; k++ )
    me.XPos[k] = double( me.XPixel - PlotX1 ) / double( PlotX2 - PlotX1 ) * ( XMax[k] - XMin[k] ) + XMin[k];
  if ( me.XPixel < PlotX1 )
    me.XCoor = FirstMargin;    
  else if ( me.XPixel > PlotX2 )
    me.XCoor = SecondMargin;    
  else
    me.XCoor = First;

  me.YPixel = qme->y();
  for ( int k=0; k<MaxAxis; k++ )
    me.YPos[k] = double( me.YPixel - PlotY1 ) / double( PlotY2 - PlotY1 ) * ( YMax[k] - YMin[k] ) + YMin[k];
  if ( me.YPixel > PlotY1 )
    me.YCoor = FirstMargin;    
  else if ( me.YPixel < PlotY2 )
    me.YCoor = SecondMargin;    
  else
    me.YCoor = First;

  me.Mode = 0;
  ButtonState button = move ? qme->state() : qme->button();
  if ( button & LeftButton )
    me.Mode |= 1;
  if ( button & RightButton )
    me.Mode |= 2;
  if ( button & MidButton )
    me.Mode |= 4;
  if ( qme->state() & ShiftButton )
    me.Mode |= 8;
  if ( qme->state() & ControlButton )
    me.Mode |= 16;
  if ( qme->state() & AltButton )
    me.Mode |= 32;
}


void Plot::mousePressEvent( QMouseEvent *qme )
{
  if ( ! SubWidget )
    lockData();
  PMutex.lock();
  MouseEvent me;
  readMouse( qme, me, false );
  me.Mode |= 64;
  mouseEvent( me );
  PMutex.unlock();
  if ( ! SubWidget )
    unlockData();
}


void Plot::mouseReleaseEvent( QMouseEvent *qme )
{
  if ( ! SubWidget )
    lockData();
  PMutex.lock();
  MouseEvent me;
  readMouse( qme, me, false );
  me.Mode |= 128;
  mouseEvent( me );
  PMutex.unlock();
  if ( ! SubWidget )
    unlockData();
}


void Plot::mouseMoveEvent( QMouseEvent *qme )
{
  if ( ! SubWidget )
    lockData();
  PMutex.lock();
  MouseEvent me;
  readMouse( qme, me, true );
  me.Mode |= 256;
  mouseEvent( me );
  PMutex.unlock();
  if ( ! SubWidget )
    unlockData();
}


void Plot::mouseDoubleClickEvent( QMouseEvent *qme )
{
  if ( ! SubWidget )
    lockData();
  PMutex.lock();
  MouseEvent me;
  readMouse( qme, me, false );
  me.Mode |= 512;
  mouseEvent( me );
  PMutex.unlock();
  if ( ! SubWidget )
    unlockData();
}


void Plot::mouseZoomMoveFirstX( MouseEvent &me )
{
  // move and zoom x1 axis:
  if ( me.pressed() && me.left() &&
       me.xCoor() == First && me.yCoor() == FirstMargin ) {
    LastMouseEvent = me;
    me.setUsed();
  }
  if ( me.moved() && me.left() ) {
    if ( ! MouseGrabbed &&
	 ! MouseZoomXMin && ! MouseZoomXMax && ! MouseMoveX &&
	 me.xCoor() == First && me.yCoor() == FirstMargin ) {
      pushRanges();
      if ( ! LastMouseEvent.valid() )
	LastMouseEvent = me;
      if ( LastMouseEvent.xPos() < XMin[0] + 0.333*( XMax[0] - XMin[0] ) )
	MouseZoomXMin = true;
      else if ( LastMouseEvent.xPos() > XMin[0] + 0.667*( XMax[0] - XMin[0] ) )
	MouseZoomXMax = true;
      else
	MouseMoveX = true;
      MouseGrabbed = true;
    }
    // move x1 axis:
    if ( MouseMoveX ) {
      double d[MaxAxis];
      for ( int k=0; k<MaxAxis; k++ )
	d[k] = me.xPos( k ) - LastMouseEvent.xPos( k );
      LastMouseEvent = me;
      for ( int k=0; k<MaxAxis; k++ ) {
	LastMouseEvent.XPos[k] -= d[k];
	XMinRange[k] = XMin[k] = XMin[k] - d[k];
	XMaxRange[k] = XMax[k] = XMax[k] - d[k];
      }
      emit changedRange();
      emit changedRange( Id );
      draw();
      me.setUsed();
    }
    // zoom x1 min:
    else if ( MouseZoomXMin ) {
      if ( me.xPixel() < PlotX2 && 
	   me.xPixel() != LastMouseEvent.xPixel() ) {
	double d[MaxAxis];
	for ( int k=0; k<MaxAxis; k++ ) {
	  d[k] = me.xPos( k ) - LastMouseEvent.xPos( k );
	  XMinRange[k] = XMin[k] = XMin[k] - d[k] * (XMax[k]-XMin[k])/(XMax[k]-LastMouseEvent.XPos[k]);
	}
	LastMouseEvent = me;
	for ( int k=0; k<MaxAxis; k++ )
	  LastMouseEvent.XPos[k] -= d[k];
	emit changedRange();
	emit changedRange( Id );
	draw();
      }
      me.setUsed();
    }
    // zoom x1 max:
    else if ( MouseZoomXMax ) {
      if ( me.xPixel() > PlotX1 && 
	   me.xPixel() != LastMouseEvent.xPixel() ) {
	double d[MaxAxis];
	for ( int k=0; k<MaxAxis; k++ ) {
	  d[k] = me.xPos( k ) - LastMouseEvent.xPos( k );
	  XMaxRange[k] = XMax[k] = XMax[k] - d[k] * (XMax[k]-XMin[k])/(LastMouseEvent.XPos[k]-XMin[k]);
	}
	LastMouseEvent = me;
	for ( int k=0; k<MaxAxis; k++ )
	  LastMouseEvent.XPos[k] -= d[k];
	emit changedRange();
	emit changedRange( Id );
	draw();
      }
      me.setUsed();
    }
  }
  if ( me.released() && me.left() ) {
    // stop moving and zooming x1 axis:
    if ( MouseMoveX || MouseZoomXMin || MouseZoomXMax ) {
      MouseMoveX = false;
      MouseZoomXMin = false;
      MouseZoomXMax = false;
      MouseGrabbed = false;
      LastMouseEvent.clear();
      me.setUsed();
    }
    else if ( me.xCoor() == First && me.yCoor() == FirstMargin ) {
      if ( MouseZoomOut ) {
	// restore previous ranges:
	if ( ranges() ) {
	  popRanges();
	  if ( ! ranges() )
	    MouseZoomOut = false;
	  emit changedRange();
	  emit changedRange( Id );
	  draw();
	  me.setUsed();
	}
      }
      else {
	// zoom at point:
	pushRanges();
	for ( int k=0; k<MaxAxis; k++ ) {
	  double xw = 0.5*( XMax[k] - XMin[k] );
	  double xmin = me.xPos( k ) - 0.5*xw;
	  if ( xmin < XMin[k] )
	    xmin = XMin[k];
	  double xmax = xmin + xw;
	  XMinRange[k] = XMin[k] = xmin;
	  XMaxRange[k] = XMax[k] = xmax;
	}
	emit changedRange();
	emit changedRange( Id );
	draw();
	me.setUsed();
      }
    }
  }
  // right click on x-axis:
  if ( me.pressed() && me.right() &&
       me.xCoor() == First && me.yCoor() == FirstMargin ) {
    if ( ! ranges() || MouseZoomOut ) {
      // zoom out:
      pushRanges();
      for ( int k=0; k<MaxAxis; k++ ) {
	double xw = XMax[k] - XMin[k];
	double xmin = me.xPos( k ) - xw;
	double xmax = xmin + 2.0*xw;
	XMinRange[k] = XMin[k] = xmin;
	XMaxRange[k] = XMax[k] = xmax;
      }
      MouseZoomOut = true;
    }
    else {
      // restore previous xrange:
      popRanges();
    }
    emit changedRange();
    emit changedRange( Id );
    draw();
    me.setUsed();
  }
}


void Plot::mouseZoomMoveFirstY( MouseEvent &me )
{
  // move y1 axis:
  if ( me.pressed() && me.left() &&
       me.yCoor() == First && me.xCoor() == FirstMargin ) {
    LastMouseEvent = me;
    me.setUsed();
  }
  if ( me.moved() && me.left() ) {
    if ( ! MouseGrabbed &&
	 ! MouseZoomYMin && ! MouseZoomYMax && ! MouseMoveY &&
	 me.yCoor() == First && me.xCoor() == FirstMargin ) {
      pushRanges();
      if ( ! LastMouseEvent.valid() )
	LastMouseEvent = me;
      if ( me.yPos() < YMin[0] + 0.333*( YMax[0] - YMin[0] ) )
	MouseZoomYMin = true;
      else if ( me.yPos() > YMin[0] + 0.667*( YMax[0] - YMin[0] ) )
	MouseZoomYMax = true;
      else
	MouseMoveY = true;
      MouseGrabbed = true;
    }
    // move y1 axis:
    if ( MouseMoveY ) {
      double d[MaxAxis];
      for ( int k=0; k<MaxAxis; k++ )
	d[k] = me.yPos( k ) - LastMouseEvent.yPos( k );
      LastMouseEvent = me;
      for ( int k=0; k<MaxAxis; k++ ) {
	LastMouseEvent.YPos[k] -= d[k];
	YMinRange[k] = YMin[k] = YMin[k] - d[k];
	YMaxRange[k] = YMax[k] = YMax[k] - d[k];
      }
      emit changedRange();
      emit changedRange( Id );
      draw();
      me.setUsed();
    }
    // zoom y1 min:
    else if ( MouseZoomYMin ) {
      if ( me.yPixel() > PlotY2 && 
	   me.yPixel() != LastMouseEvent.yPixel() ) {
	double d[MaxAxis];
	for ( int k=0; k<MaxAxis; k++ ) {
	  d[k] = me.yPos( k ) - LastMouseEvent.yPos( k );
	  YMinRange[k] = YMin[k] = YMin[k] - d[k] * (YMax[k]-YMin[k])/(YMax[k]-LastMouseEvent.YPos[k]);
	}
	LastMouseEvent = me;
	for ( int k=0; k<MaxAxis; k++ )
	  LastMouseEvent.YPos[k] -= d[k];
	emit changedRange();
	emit changedRange( Id );
	draw();
      }
      me.setUsed();
    }
    // zoom y1 max:
    else if ( MouseZoomYMax ) {
      if ( me.yPixel() < PlotY1 && 
	   me.yPixel() != LastMouseEvent.yPixel() ) {
	double d[MaxAxis];
	for ( int k=0; k<MaxAxis; k++ ) {
	  d[k] = me.yPos( k ) - LastMouseEvent.yPos( k );
	  YMaxRange[k] = YMax[k] = YMax[k] - d[k] * (YMax[k]-YMin[k])/(LastMouseEvent.YPos[k]-YMin[k]);
	}
	LastMouseEvent = me;
	for ( int k=0; k<MaxAxis; k++ )
	  LastMouseEvent.YPos[k] -= d[k];
	emit changedRange();
	emit changedRange( Id );
	draw();
      }
      me.setUsed();
    }
  }
  if ( me.released() && me.left() ) {
    // stop moving y1 axis:
    if ( MouseMoveY || MouseZoomYMin || MouseZoomYMax ) {
      MouseMoveY = false;
      MouseZoomYMin = false;
      MouseZoomYMax = false;
      MouseGrabbed = false;
      LastMouseEvent.clear();
      me.setUsed();
    }
    else if ( me.yCoor() == First && me.xCoor() == FirstMargin ) {
      if ( MouseZoomOut ) {
	// restore previous y-range:
	if ( ranges() ) {
	  popRanges();
	  if ( ! ranges() )
	    MouseZoomOut = false;
	  emit changedRange();
	  emit changedRange( Id );
	  draw();
	  me.setUsed();
	}
      }
      else {
	// zoom at point:
	pushRanges();
	for ( int k=0; k<MaxAxis; k++ ) {
	  double yw = 0.5*( YMax[k] - YMin[k] );
	  double ymin = me.yPos( k ) - 0.5*yw;
	  if ( ymin < YMin[k] )
	    ymin = YMin[k];
	  double ymax = ymin + yw;
	  YMinRange[k] = YMin[k] = ymin;
	  YMaxRange[k] = YMax[k] = ymax;
	}
	emit changedRange();
	emit changedRange( Id );
	draw();
	me.setUsed();
      }
    }
  }
  // right click on y-axis:
  if ( me.pressed() && me.right() &&
       me.yCoor() == First && me.xCoor() == FirstMargin ) {
    if ( ! ranges() || MouseZoomOut ) {
      // zoom out:
      pushRanges();
      for ( int k=0; k<MaxAxis; k++ ) {
	double yw = YMax[k] - YMin[k];
	double ymin = me.yPos( k ) - yw;
	double ymax = ymin + 2.0*yw;
	YMinRange[k] = YMin[k] = ymin;
	YMaxRange[k] = YMax[k] = ymax;
      }
      MouseZoomOut = true;
    }
    else {
      // restore previous ranges:
      popRanges();
    }
    emit changedRange();
    emit changedRange( Id );
    draw();
    me.setUsed();
  }
}


void Plot::mouseZoomMovePlot( MouseEvent &me, bool move )
{
  // start move or zoom plot:
  if ( me.pressed() && me.left() &&
       me.xCoor() == First && me.yCoor() == First ) {
    LastMouseEvent = me;
    MouseX1 = MouseX2 = me.xPixel();
    MouseY1 = MouseY2 = me.yPixel();
    MouseXMax = false;
    MouseYMax = false;
    me.setUsed();
  }
  if ( me.moved() && me.left() ) {
    if ( ! MouseGrabbed && ! MouseDrawRect && ! MouseMoveXY &&
	 me.xCoor() == First && me.yCoor() == First ) {
      if ( move != me.alt() ) {
	pushRanges();
	MouseMoveXY = true;
	if ( ! LastMouseEvent.valid() )
	  LastMouseEvent = me;
      }
      else {
	MouseDrawRect = true;
	if ( MouseX1 == 0xffff ) {
	  MouseX1 = me.xPixel();
	  MouseY1 = me.yPixel();
	}
      }
      MouseGrabbed = true;
    }
    // draw zoom rectangle:
    if ( MouseDrawRect ) {
      if ( me.xCoor() == First )
	MouseX2 = me.xPixel();
      if ( me.yCoor() == First )
	MouseY2 = me.yPixel();
      MouseXMax = false;
      MouseYMax = false;
      if ( ::abs( MouseX2 - MouseX1 ) < ::abs( MouseY2 - MouseY1 ) ) {
	if ( ::abs( MouseX2 - MouseX1 ) < 10 )
	  MouseXMax = true;
      }
      else {
	if ( ::abs( MouseY2 - MouseY1 ) < 10 )
	  MouseYMax = true;
      }
      NewData = true;
      draw();
      me.setUsed();
    }
    // move x1 and y1 axis:
    else if ( MouseMoveXY ) {
      double dx[MaxAxis];
      double dy[MaxAxis];
      for ( int k=0; k<MaxAxis; k++ ) {
	dx[k] = me.xPos( k ) - LastMouseEvent.xPos( k );
	dy[k] = me.yPos( k ) - LastMouseEvent.yPos( k );
      }
      LastMouseEvent = me;
      for ( int k=0; k<MaxAxis; k++ ) {
	LastMouseEvent.XPos[k] -= dx[k];
	LastMouseEvent.YPos[k] -= dy[k];
	if ( ! me.control() ) {
	  XMinRange[k] = XMin[k] = XMin[k] - dx[k];
	  XMaxRange[k] = XMax[k] = XMax[k] - dx[k];
	}
	if ( ! me.shift() ) {
	  YMinRange[k] = YMin[k] = YMin[k] - dy[k];
	  YMaxRange[k] = YMax[k] = YMax[k] - dy[k];
	}
      }
      emit changedRange();
      emit changedRange( Id );
      draw();
      me.setUsed();
    }
  }
  if ( me.released() && me.left() ) {
    if ( MouseDrawRect ) {
      // zoom to rectangle selection:
      MouseDrawRect = false;
      MouseGrabbed = false;
      if ( MouseX1 != MouseX2 && MouseY1 != MouseY2 ) {
	pushRanges();
	if ( MouseX1 > MouseX2 )
	  swap( MouseX1, MouseX2 );
	if ( MouseY1 > MouseY2 )
	  swap( MouseY1, MouseY2 );
	for ( int k=0; k<MaxAxis; k++ ) {
	  double xmin = 0.0;
	  double xmax = 0.0;
	  double ymin = 0.0;
	  double ymax = 0.0;
	  if ( MouseXMax ) {
	    xmin = XMin[k];
	    xmax = XMax[k];
	  }
	  else {
	    xmin = double( MouseX1 - PlotX1 ) / double( PlotX2 - PlotX1 ) * ( XMax[k] - XMin[k] ) + XMin[k];
	    xmax = double( MouseX2 - PlotX1 ) / double( PlotX2 - PlotX1 ) * ( XMax[k] - XMin[k] ) + XMin[k];
	  }
	  if ( MouseYMax ) {
	    ymin = YMin[k];
	    ymax = YMax[k];
	  }
	  else {
	    ymin = double( MouseY1 - PlotY1 ) / double( PlotY2 - PlotY1 ) * ( YMax[k] - YMin[k] ) + YMin[k];
	    ymax = double( MouseY2 - PlotY1 ) / double( PlotY2 - PlotY1 ) * ( YMax[k] - YMin[k] ) + YMin[k];
	  }
	  XMinRange[k] = XMin[k] = xmin;
	  XMaxRange[k] = XMax[k] = xmax;
	  YMinRange[k] = YMin[k] = ymin;
	  YMaxRange[k] = YMax[k] = ymax;
	}
	MouseX1 = MouseX2 = 0xffff;
	MouseY1 = MouseY2 = 0xffff;
	me.setUsed();
      }
    }
    else if ( MouseMoveXY ) {
      // stop moving x1 - y1 axis:
      MouseMoveXY = false;
      MouseGrabbed = false;
      LastMouseEvent.clear();
      me.setUsed();
    }
    else if ( me.xCoor() == First && me.yCoor() == First ) {
      if ( MouseZoomOut && ranges() ) {
	// restore previous ranges:
	popRanges();
	if ( ! ranges() )
	  MouseZoomOut = false;
	me.setUsed();
      }
      else {
	// zoom at point:
	pushRanges();
	for ( int k=0; k<MaxAxis; k++ ) {
	  double xw = 0.5*( XMax[k] - XMin[k] );
	  double yw = 0.5*( YMax[k] - YMin[k] );
	  double xmin = me.xPos( k ) - 0.5*xw;
	  if ( xmin < XMin[k] )
	    xmin = XMin[k];
	  double xmax = xmin + xw;
	  double ymin = me.yPos( k ) - 0.5*yw;
	  if ( ymin < YMin[k] )
	    ymin = YMin[k];
	  double ymax = ymin + yw;
	  if ( ! me.control() ) {
	    XMinRange[k] = XMin[k] = xmin;
	    XMaxRange[k] = XMax[k] = xmax;
	  }
	  if ( ! me.shift() ) {
	    YMinRange[k] = YMin[k] = ymin;
	    YMaxRange[k] = YMax[k] = ymax;
	  }
	}
	me.setUsed();
      }
    }
    emit changedRange();
    emit changedRange( Id );
    draw();
  }
  if ( me.pressed() && me.right() &&
       me.yCoor() == First && me.xCoor() == First ) {
    if ( ! ranges() || MouseZoomOut ) {
      // zoom out:
      pushRanges();
      for ( int k=0; k<MaxAxis; k++ ) {
	double xw = XMax[k] - XMin[k];
	double yw = YMax[k] - YMin[k];
	double xmin = me.xPos( k ) - xw;
	double xmax = xmin + 2.0*xw;
	double ymin = me.yPos( k ) - yw;
	double ymax = ymin + 2.0*yw;
	if ( ! me.control() ) {
	  XMinRange[k] = XMin[k] = xmin;
	  XMaxRange[k] = XMax[k] = xmax;
	}
	if ( ! me.shift() ) {
	  YMinRange[k] = YMin[k] = ymin;
	  YMaxRange[k] = YMax[k] = ymax;
	}
      }
      MouseZoomOut = true;
      me.setUsed();
    }
    else {
      // restore previous ranges:
      if ( ranges() ) {
	popRanges();
	me.setUsed();
      }
    }
    emit changedRange();
    emit changedRange( Id );
    draw();
  }
  /*
  // restore original ranges:
  if ( me.doubleClicked() && me.left() &&
       me.yCoor() == First && me.xCoor() == First &&
       ranges() ) {
    xxxRanges();
    draw();
  }
  */
}


void Plot::mouseAnalyse( MouseEvent &me )
{
  if ( me.xCoor() == First && me.yCoor() == First ) {

    double xpos = me.xPos();
    double ypos = me.yPos();
    int dinx = -1;
    int pinx = -1;

    if ( ! me.alt() && ( ! me.control() || ! me.shift() ) ) {
      // find data set:
      double mindd = ( PlotX2 - PlotX1 + 1 ) + ( PlotY1 - PlotY2 + 1 );
      for ( unsigned int k=0; k<PData.size(); k++ ) {
        // axis:
	int xaxis = PData[k]->XAxis;
	int yaxis = PData[k]->YAxis;
	// find the closest data point:
	int f = PData[k]->first( XMin[xaxis], YMin[yaxis], XMax[xaxis], YMax[yaxis] );
	int l = PData[k]->last( XMin[xaxis], YMin[yaxis], XMax[xaxis], YMax[yaxis] );
	if ( f >= l )
	  continue;
	double x = 0.0;
	double y = 0.0;
	PData[k]->point( f, x, y );
	int xp = PlotX1 + (int)::rint( double(PlotX2-PlotX1)/(XMax[xaxis]-XMin[xaxis])*(x-XMin[xaxis]) );
	int yp = PlotY1 + (int)::rint( double(PlotY2-PlotY1)/(YMax[yaxis]-YMin[yaxis])*(y-YMin[yaxis]) );
	double dx = ::fabs( xp - me.xPixel() );
	double dy = MouseYShrink * ::fabs( yp - me.yPixel() );
	double mind = ::sqrt( dx*dx + dy*dy );
	int minpinx = 0;
	for ( int j=f+1; j<l; j++ ) {
	  PData[k]->point( j, x, y );
	  xp = PlotX1 + (int)::rint( double(PlotX2-PlotX1)/(XMax[xaxis]-XMin[xaxis])*(x-XMin[xaxis]) );
	  yp = PlotY1 + (int)::rint( double(PlotY2-PlotY1)/(YMax[yaxis]-YMin[yaxis])*(y-YMin[yaxis]) );
	  dx = ::fabs( xp - me.xPixel() );
	  dy = MouseYShrink * ::fabs( yp - me.yPixel() );
	  double md = ::sqrt( dx*dx + dy*dy );
	  if ( x >= XMin[xaxis] && x <= XMax[xaxis] && 
	       y >= YMin[yaxis] && y <= YMax[yaxis] && 
	       md < mind ) {
	    mind = md;
	    minpinx = j;
	  }
	}
	// take the closest one:
	if ( mind < mindd ) {
	  mindd = mind;
	  dinx = k;
	  pinx = minpinx;
	}
      }
      if ( ! me.shift() )
	xpos = AutoScale;
      if ( ! me.control() )
	ypos = AutoScale;
    }

    if ( me.init() || ( me.left() && me.pressed() ) ) {
      MouseXPos.push_back( xpos );
      MouseYPos.push_back( ypos );
      MouseDInx.push_back( dinx );
      MousePInx.push_back( pinx );
      draw();
      me.setUsed();
    }
    else if ( me.right() && me.doubleClicked() ) {
      MouseXPos.clear();
      MouseYPos.clear();
      MouseDInx.clear();
      MousePInx.clear();
      MouseXPos.push_back( xpos );
      MouseYPos.push_back( ypos );
      MouseDInx.push_back( dinx );
      MousePInx.push_back( pinx );
      draw();
      me.setUsed();
    }
    else {
      if ( me.right() && me.pressed() &&
	   MouseXPos.size() > 1 ) {
	MouseXPos.pop_back();
	MouseYPos.pop_back();
	MouseDInx.pop_back();
	MousePInx.pop_back();
	draw();
	me.setUsed();
      }
      if ( MouseXPos.back() != xpos ||
	   MouseYPos.back() != ypos ||
	   MouseDInx.back() != dinx || 
	   MousePInx.back() != pinx ) {
	MouseXPos.back() = xpos;
	MouseYPos.back() = ypos;
	MouseDInx.back() = dinx;
	MousePInx.back() = pinx;
	draw();
	me.setUsed();
      }
    }
  }
  else if ( me.init() ) {
    MouseXPos.push_back( AutoScale );
    MouseYPos.push_back( AutoScale );
    MouseDInx.push_back( -1 );
    MousePInx.push_back( -1 );
    me.setUsed();
  }
}


void Plot::mouseMenu( MouseEvent &me )
{
  if ( me.pressed() && me.mid() ) {
    if ( MouseMenu == 0 ) {
      MouseMenu = new QPopupMenu( this );
      MouseMenu->setCheckable( true );
      MouseMenu->insertItem( "&Zoom", 2 );
      MouseMenu->insertItem( "&Move", 4 );
      MouseMenu->insertItem( "&Analyse", 8 );
      MouseMenu->insertItem( "&Disable", 1 );
      MouseMenu->setItemChecked( 2, true );
      connect( MouseMenu, SIGNAL( activated( int ) ),
	       this, SLOT( mouseSelect( int ) ) );
    }
    MouseMenu->popup( QCursor::pos() );
    MouseMenuClick = true;
    me.setUsed();
  }
  else if ( MouseMenuClick ) {
    if ( me.released() )
      MouseMenuClick = false;
    me.setUsed();
  }
}


void Plot::mouseSelect( int id )
{
  if ( id < 0 ) {
    PMutex.lock();
    MouseMenuClick = true;
    PMutex.unlock();
  }
  else {
    MouseMenuClick = false;
    if ( id != MouseAction ) {
      MouseMenu->setItemChecked( MouseAction, false );
      MouseMenu->setItemChecked( id, true );
      if ( MouseAction == 8 ) {
	PMutex.lock();
	MouseXPos.clear();
	MouseYPos.clear();
	MouseDInx.clear();
	MousePInx.clear();
	if ( ! MouseTracking ) {
	  if ( SubWidget && MP != 0 )
	    MP->setMouseTracking( false );
	  else
	    QWidget::setMouseTracking( false );
	}
	PMutex.unlock();
	draw();
      }
      MouseAction = id;
      if ( MouseAction == 8 ) {
	if ( ! MouseTracking ) {
	  if ( SubWidget && MP != 0 )
	    MP->setMouseTracking( true );
	  else
	    QWidget::setMouseTracking( true );
	}
	QPoint p = mapFromGlobal( QCursor::pos() );
	QMouseEvent qme( QEvent::MouseButtonRelease, p, 
			 Qt::LeftButton, Qt::NoButton );
	MouseEvent nme;
	if ( ! SubWidget )
	  lockData();
	PMutex.lock();
	readMouse( &qme, nme, false );
	nme.setInit();
	mouseAnalyse( nme );
	if ( ! SubWidget )
	  unlockData();
	PMutex.unlock();
      }
    }
  }
}


void Plot::mouseEvent( MouseEvent &me )
{
  emit userMouseEvent( me );
  if ( me.used() )
    return;

  mouseMenu( me );
  if ( me.used() )
    return;

  if ( MouseAction == 2 )
    mouseZoomMovePlot( me, false );
  else if ( MouseAction == 4 )
    mouseZoomMovePlot( me, true );
  else if ( MouseAction == 8 )
    mouseAnalyse( me );

  if ( ! me.used() ) {
    mouseZoomMoveFirstX( me );
    mouseZoomMoveFirstY( me );
  }
}


void Plot::setMouseTracking( bool enable )
{
  MouseTracking = enable;
  if ( SubWidget && MP != 0 )
    MP->setMouseTracking( MouseTracking );
  else
    QWidget::setMouseTracking( MouseTracking );
}


void Plot::enableMouse( void )
{
  MouseAction &= ~1;
}


void Plot::disableMouse( void )
{
  MouseAction |= 1;
}


bool Plot::zoomedXRange( void )
{
  return ( ranges() && 
	   ( XMinRange[0] != MouseRangeStack.front().XMin[0] ||
	     XMaxRange[0] != MouseRangeStack.front().XMax[0] ) );
}


bool Plot::zoomedYRange( void )
{
  return ( ranges() && 
	   ( YMinRange[0] != MouseRangeStack.front().YMin[0] ||
	     YMaxRange[0] != MouseRangeStack.front().YMax[0] ) );
}


bool Plot::zoomedX2Range( void )
{
  return ( ranges() && 
	   ( XMinRange[1] != MouseRangeStack.front().XMin[1] ||
	     XMaxRange[1] != MouseRangeStack.front().XMax[1] ) );
}


bool Plot::zoomedY2Range( void )
{
  return ( ranges() && 
	   ( YMinRange[1] != MouseRangeStack.front().YMin[1] ||
	     YMaxRange[1] != MouseRangeStack.front().YMax[1] ) );
}


bool Plot::zoomedRange( void )
{
  return ( zoomedXRange() || zoomedYRange() ||
	   zoomedX2Range() || zoomedY2Range() );
}


void Plot::setYShrinkFactor( double f )
{
  MouseYShrink = f;
}


int Plot::addData( DataElement *d )
{
  NewData = true;
  PData.push_back( d );
  return PData.size() - 1;
}

Plot::DataElement::DataElement( DataTypes dt )
  : Own( false ), 
    XAxis( 0 ),
    YAxis( 0 ),
    LineIndex( 0 ),
    PointIndex( 0 ),
    DataType( dt ),
    Line(),
    Point()  
{
}


Plot::DataElement::~DataElement( void )
{
}


void Plot::DataElement::setAxis( Plot::Axis axis )
{
  XAxis = axis & 2 ? 1 : 0;
  YAxis = axis & 1 ? 1 : 0;
}


void Plot::DataElement::setAxis( int xaxis, int yaxis )
{
  XAxis = xaxis;
  YAxis = yaxis;
}


void Plot::DataElement::setLineIndex( long inx )
{
  LineIndex = inx;
}


long Plot::DataElement::lineIndex() const
{
  return LineIndex;
}


void Plot::DataElement::setPointIndex( long inx )
{
  PointIndex = inx;
}


long Plot::DataElement::pointIndex() const
{
  return PointIndex;
}


void Plot::DataElement::setLine( const Plot::LineStyle &style )
{
  Line = style;
}


void Plot::DataElement::setLine( int lcolor, int lwidth, Plot::Dash ldash )
{
  setLine( LineStyle( lcolor, lwidth, ldash ) );
}


void Plot::DataElement::setPoint( const Plot::PointStyle &style )
{
  Point = style;
}


void Plot::DataElement::setPoint( Points ptype, int psize, int pcolor, int pfill )
{
  setPoint( PointStyle( ptype, psize, pcolor, pfill ) );
}


void Plot::DataElement::setStyle( const Plot::LineStyle &lstyle, 
				  const Plot::PointStyle &pstyle )
{ 
  Line = lstyle;
  Point = pstyle;
}


void Plot::DataElement::setStyle( int lcolor, int lwidth, Plot::Dash ldash, 
				  Plot::Points ptype, int psize, int pcolor, 
				  int pfill )
{
  Line = LineStyle( lcolor, lwidth, ldash );
  Point = PointStyle( ptype, psize, pcolor, pfill );
}


Plot::PointElement::PointElement( double x, Coordinates xcoor,
				  double y, Coordinates ycoor,
				  double size, Coordinates sizecoor )
  : DataElement( SinglePoint )
{
  Own = true;
  P.setXPos( x, xcoor );
  P.setYPos( y, ycoor );
  X = x;
  Y = y;
  Size = size;
  SizeCoor = sizecoor;
}


void Plot::PointElement::point( long index, double &x, double &y ) const
{
  x = X;
  y = Y;
}


void Plot::PointElement::setRange( double xmin[MaxAxis], double xmax[MaxAxis], 
				   double ymin[MaxAxis], double ymax[MaxAxis],
				   int xpmin, int xpmax, 
				   int ypmin, int ypmax )
{
  if ( P.xcoor() == Plot::Graph )
    X = P.xpos()*( xmax[XAxis] - xmin[XAxis] ) + xmin[XAxis];
  else
    X = P.xpos();

  if ( P.ycoor() == Plot::Graph )
    Y = P.ypos()*( ymax[YAxis] - ymin[YAxis] ) + ymin[YAxis];
  else
    Y = P.ypos();

  if ( Size > 0 ) {
    if ( SizeCoor == Plot::Graph || SizeCoor == Plot::GraphY )
      Point.setSize( (int)::rint( Size*::abs( ypmax - ypmin ) ) );
    else if ( SizeCoor == Plot::GraphX )
      Point.setSize( (int)::rint( Size*::abs( xpmax - xpmin ) ) );
    else if ( SizeCoor == Plot::First || SizeCoor == Plot::FirstY )
      Point.setSize( (int)::rint( Size*::fabs( (ypmax-ypmin)/double(ymax[0]-ymin[0]) ) ) );
    else if ( SizeCoor == Plot::FirstX )
      Point.setSize( (int)::rint( Size*::fabs( (xpmax-xpmin)/double(xmax[0]-xmin[0]) ) ) );
    else if ( SizeCoor == Plot::Second || SizeCoor == Plot::SecondY )
      Point.setSize( (int)::rint( Size*::fabs( (ypmax-ypmin)/double(ymax[1]-ymin[1]) ) ) );
    else if ( SizeCoor == Plot::SecondX )
      Point.setSize( (int)::rint( Size*::fabs( (xpmax-xpmin)/double(xmax[1]-xmin[1]) ) ) );
    else if ( SizeCoor == Plot::Pixel )
      Point.setSize( (int)::rint( Size ) );
    else
      Point.setSize( (int)::rint( Size*::fabs( (ypmax-ypmin)/double(ymax[YAxis]-ymin[YAxis]) ) ) );
  }
}


int Plot::plotPoint( double x, Coordinates xcoor, 
		     double y, Coordinates ycoor, int lwidth,
		     Points ptype, double size, Coordinates sizecoor,
		     int pcolor, int pfill )
{
  PointElement *PE = new PointElement( x, xcoor, y, ycoor, size, sizecoor );
  PE->setStyle( Transparent, lwidth, Solid, ptype, int(size), pcolor, pfill );
  return addData( PE );
}


Plot::LineElement::LineElement( double x1, Coordinates x1coor, double y1, Coordinates y1coor, 
				double x2, Coordinates x2coor, double y2, Coordinates y2coor )
  : DataElement( TwoPoints )
{
  Own = true;
  P1.setXPos( x1, x1coor );
  P1.setYPos( y1, y1coor );
  P2.setXPos( x2, x2coor );
  P2.setYPos( y2, y2coor );
}


void Plot::LineElement::point( long index, double &x, double &y ) const
{
  if ( index == 0 )
    getPos( P1, x, y );
  else
    getPos( P2, x, y );
}


void Plot::LineElement::setRange( double xmin[MaxAxis], double xmax[MaxAxis], 
				  double ymin[MaxAxis], double ymax[MaxAxis],
				  int xpmin, int xpmax, 
				  int ypmin, int ypmax )
{
  XMin = xmin[XAxis];
  XMax = xmax[XAxis];
  YMin = ymin[YAxis];
  YMax = ymax[YAxis];
}


void Plot::LineElement::getPos( const Position &pos, double &x, double &y ) const
{
  // x coordinate:
  if ( pos.xcoor() == Plot::Graph )
    x = pos.xpos()*( XMax - XMin ) + XMin;
  else
    x = pos.xpos();

  // y coordinate:
  if ( pos.ycoor() == Plot::Graph )
    y = pos.ypos()*( YMax - YMin ) + YMin;
  else
    y = pos.ypos();
}


int Plot::plotVLine( double x, const LineStyle &line )
{
  LineElement *LE = new LineElement( x, First, 0.0, Graph, 
				     x, First, 1.0, Graph );
  LE->setLine( line );
  return addData( LE );
}


int Plot::plotHLine( double y, const LineStyle &line )
{
  LineElement *LE = new LineElement( 0.0, Graph, y, First,
				     1.0, Graph, y, First );
  LE->setLine( line );
  return addData( LE );
}


int Plot::plotLine( double x1, double y1, double x2, double y2,
		    const LineStyle &line )
{
  LineElement *LE = new LineElement( x1, First, y1, First,
				     x2, First, y2, First );
  LE->setLine( line );
  return addData( LE );
}


Plot::EventDataElement::EventDataElement( const EventData &x, int origin,
					  double offset, double tscale, 
					  double y, Coordinates ycoor, 
					  double size, Coordinates sizecoor,
					  bool copy )
  : EventsElement< EventData >( x, tscale, y, ycoor, size, sizecoor, copy )
{
  Origin = origin;
  Offset = offset;
  Reference = 0.0;
}


Plot::EventDataElement::~EventDataElement( void )
{
}


long Plot::EventDataElement::first( double x1, double y1, double x2, double y2 ) const
{
  return ED->next( x1/TScale + Reference );
}


long Plot::EventDataElement::last( double x1, double y1, double x2, double y2 ) const
{
  return ED->next( x2/TScale + Reference );
}


void Plot::EventDataElement::point( long index, double &x, double &y ) const
{
  x = ( ED->operator[]( index ) - Reference ) * TScale;
  y = Y;
}


bool Plot::EventDataElement::init( void )
{
  double prevref = Reference;

  Reference = 0.0;
  if ( Origin == 1 )
    Reference = ED->rangeBack(); 
  else if ( Origin == 2 )
    Reference = ( ED->signalTime() < 0.0 ? 0.0 : ED->signalTime() ); 
  else if ( Origin == 3 )
    Reference = Offset; 

  return ( ::fabs( Reference - prevref ) > 1.0e-8 );
}


void Plot::EventDataElement::xminmax( double &xmin, double &xmax, 
				      double ymin, double ymax ) const
{
  double tmin = ED->rangeFront();
  if ( ED->size() > 0 && ED->minTime() > tmin )
    tmin = ED->minTime();
  double tmax = ED->rangeBack();

  tmin -= Reference;
  tmax -= Reference; 

  xmin = tmin * TScale;
  xmax = tmax * TScale;
}


int Plot::plot( const EventData &data, int origin, double offset, double tscale, 
		double y, Coordinates ycoor, int lwidth,
		Points ptype, double size, Coordinates sizecoor,
		int pcolor, int pfill )
{
  EventDataElement *DE = new EventDataElement( data, origin, offset, tscale, 
					       y, ycoor, size, sizecoor, Keep == Copy );
  DE->setStyle( Transparent, lwidth, Solid, ptype, int(size), pcolor, pfill );
  return addData( DE );
}


#ifdef HAVE_LIBRELACSDAQ

Plot::InDataElement::InDataElement( const InData &data, int origin,
				    double offset, double tscale,
				    bool copy )
  : DataElement( Map )
{
  Own = copy;
  if ( copy ) {
    ID = new InData( data );
  }
  else {
    ID = &data;
  }
  Origin = origin;
  Offset = offset;
  TScale = tscale;
  Reference = 0.0;
}


Plot::InDataElement::~InDataElement( void )
{
  if ( Own ) {
    delete ID;
  }
}


long Plot::InDataElement::first( double x1, double y1, double x2, double y2 ) const
{
  double t = x1/TScale + Reference; 
  long x1i = long( ::floor( t/ID->sampleInterval() ) );
  if ( x1i > ID->currentIndex() )
    return ID->currentIndex();
  if ( x1i < ID->minIndex() )
    return ID->minIndex();
  return x1i;
}


long Plot::InDataElement::last( double x1, double y1, double x2, double y2 ) const
{
  double t = x2/TScale + Reference; 
  long x2i = long( ::ceil( t/ID->sampleInterval() ) ) + 1;
  if ( x2i > ID->currentIndex() )
    return ID->currentIndex();
  if ( x2i < ID->minIndex() )
    return ID->minIndex();
  return x2i;
}


void Plot::InDataElement::point( long index, double &x, double &y ) const
{
  x = ( ID->interval( index ) - Reference ) * TScale;
  y = (*ID)[index];
}


bool Plot::InDataElement::init( void )
{
  double prevref = Reference;

  Reference = 0.0;
  if ( Origin == 1 )
    Reference = ID->currentTime(); 
  else if ( Origin == 2 )
    Reference = ( ID->signalTime() < 0.0 ? 0.0 : ID->signalTime() ); 
  else if ( Origin == 3 )
    Reference = Offset; 

  return ( ::fabs( Reference - prevref ) > 1.0e-8 );
}


void Plot::InDataElement::xminmax( double &xmin, double &xmax, 
				      double ymin, double ymax ) const
{
  double tmin = ID->minTime();
  double tmax = ID->currentTime();

  tmin -= Reference;
  tmax -= Reference; 

  xmin = tmin * TScale;
  xmax = tmax * TScale;
}


void Plot::InDataElement::yminmax( double xmin, double xmax, 
				   double &ymin, double &ymax ) const
{
  double tmin = xmin/TScale;
  double tmax = xmax/TScale;

  tmin += Reference;
  tmax += Reference; 

  long x1i = ID->indices( tmin );
  long x2i = ID->indices( tmax );

  if ( x1i < 0 )
    x1i = 0;
  if ( x2i > ID->currentIndex() )
    x2i = ID->currentIndex();
  if ( x2i > x1i ) {
    ymin = ymax = (*ID)[x1i];
    for ( long k=x1i+1; k<=x2i; k++ )
      if ( (*ID)[k] > ymax )
	ymax = (*ID)[k];
      else if ( (*ID)[k] < ymin )
	ymin = (*ID)[k];
  }
  else {
    ymin = 0.0;
    ymax = 0.0;
  }
}


int Plot::plot( const InData &data, int origin, double offset, double tscale, 
		const LineStyle &line, const PointStyle &point )
{
  InDataElement *DE = new InDataElement( data, origin, offset, tscale, Keep == Copy );
  DE->setStyle( line, point );
  return addData( DE );
}


Plot::OutDataElement::OutDataElement( const OutData &data, double tscale,
				      bool copy )
  : SampleDataElement< float >( (const SampleData<float>&)data, tscale, copy )
{
}


int Plot::plot( const OutData &data, double xscale, 
		const LineStyle &line, const PointStyle &point )
{
  OutDataElement *DE = new OutDataElement( data, xscale, Keep == Copy );
  DE->setStyle( line, point );
  return addData( DE );
}


Plot::EventInDataElement::EventInDataElement( const EventData &events, 
					      const InData &data, int origin,
					      double offset, double tscale, 
					      double size, Coordinates sizecoor,
					      bool copy )
  : EventDataElement( events, origin, offset, tscale, 0.0, First, size, sizecoor, copy )
{
  if ( copy ) {
    ID = new InData( data );
  }
  else {
    ID = &data;
  }
}


Plot::EventInDataElement::~EventInDataElement( void )
{
  if ( Own ) {
    delete ID;
  }
}


long Plot::EventInDataElement::first( double x1, double y1, double x2, double y2 ) const
{
  long i = EventDataElement::first( x1, y1, x2, y2 );
  if ( i < ED->size() ) {
    double t = (*ED)[ i ];
    if ( t < ID->minTime() )
      i = ED->next( ID->minTime() );
  }
  return i;
}


long Plot::EventInDataElement::last( double x1, double y1, double x2, double y2 ) const
{
  long i = EventDataElement::last( x1, y1, x2, y2 );
  if ( i < ED->size() ) {
    double t = (*ED)[ i ];
    if ( t < ID->minTime() )
      i = ED->next( ID->minTime() );
  }
  return i;
}


void Plot::EventInDataElement::point( long index, double &x, double &y ) const
{
  double time = ED->operator[]( index ); 
  x = ( time - Reference ) * TScale;
  int inx = ID->index( time );
  y = (*ID)[inx];
}


void Plot::EventInDataElement::xminmax( double &xmin, double &xmax, 
					   double ymin, double ymax ) const
{
  double tmin = ED->rangeFront();
  if ( ED->size() > 0 && ED->minTime() > tmin )
    tmin = ED->minTime();
  if ( tmin < ID->minTime() )
    tmin = ID->minTime();
  double tmax = ID->currentTime();

  tmin -= Reference;
  tmax -= Reference; 

  xmin = tmin * TScale;
  xmax = tmax * TScale;
}


int Plot::plot( const EventData &events, const InData &data, int origin, double offset, double tscale, 
		int lwidth, Points ptype, double size, Coordinates sizecoor,
		int pcolor, int pfill )
{
  EventInDataElement *DE = new EventInDataElement( events, data, origin, offset, tscale, 
							 size, sizecoor, Keep == Copy );
  DE->setStyle( Transparent, lwidth, Solid, ptype, int(size), pcolor, pfill );
  return addData( DE );
}

#endif


void Plot::clear( void )
{
  for ( PDataType::iterator d = PData.begin(); d != PData.end(); ++d )
    delete (*d);
  PData.clear();
  NewData = true;
}


void Plot::clear( int index )
{
  int k;
  PDataType::iterator d;
  for ( k=0, d = PData.begin(); k<index && d != PData.end(); ++d, ++k );
  if ( k == index && d != PData.end() ) {
    delete (*d);
    PData.erase( d );
  }
  NewData = true;
}


}; /* namespace relacs */

#include "moc_plot.cc"

