/*
  plot.h
  Plotting various data in a single widget.

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

#ifndef _RELACS_PLOT_H_
#define _RELACS_PLOT_H_ 1


#include <values.h>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <QWidget>
#include <QMutex>
#include <QReadWriteLock>
#include <QAction>
#include <QMenu>
#include <QPen>
#include <QBrush>
#include <relacs/array.h>
#include <relacs/map.h>
#include <relacs/sampledata.h>
#include <relacs/eventdata.h>

#ifdef HAVE_LIBRELACSDAQ
#include <relacs/indata.h>
#endif

#ifdef HAVE_LIBRELACSSHAPES
#include <relacs/polygon.h>
#include <relacs/transform.h>
#include <relacs/shape.h>
#endif

namespace relacs {


/*! 
\class Plot
\author Jan Benda
\brief Plotting various data in a single widget.

\bug autoscale with zero interval
\bug check initTics algorithmns. Make them iterative!
\todo execute init* routines immediately in draw(), only draw* routines should be posted.
\todo logarithmic axis
\todo add axis as an own plot object in addition to border
\todo more colors and gradients (index is index to either a color, a gradient? or a picture!
*/


class MultiPlot;


class Plot : public QWidget
{
  Q_OBJECT

  friend class MultiPlot;

public:

    /*! Different possibilites to keep data. */
  enum KeepMode { 
      /*! Keep only pointer to the data. */
    Pointer,
      /*! Keep a copy of the data. */
    Copy
  };

    /*! Some predefined colors. */
  enum Color {
    Transparent=-1,
    WidgetBackground=0, 
    TextForeground, 
    Black, Gray, White, Red, Green, Blue, Yellow,
    Magenta, Cyan, Orange, DarkOrange, OrangeRed, Gold,
    Chartreuse, DeepPink, DeepSkyBlue, SpringGreen, DarkGreen,
    DarkCyan, DarkTurquoise    
  };

    /*! Dash styles for drawing lines. */
  enum Dash {
    Solid=0, LongDash, ShortDash, WideDotted, Dotted, DashDot, DashDotDot
  };

    /*! Different point types. */
  enum Points {
    Circle, CircleDot, Diamond, DiamondDot, Square, SquareDot, 
    TriangleUp, TriangleUpDot, TriangleDown, TriangleDownDot, 
    TriangleLeft, TriangleLeftDot, TriangleRight, TriangleRightDot,
    TriangleNorth, TriangleSouth, TriangleWest, TriangleEast,
    CircleNorth, CircleSouth, CircleWest, CircleEast,
    SquareNorth, SquareSouth, SquareWest, SquareEast,
    Dot, StrokeUp, StrokeVertical, StrokeHorizontal, Box
  };

    /*! Some predefined gradients. */
  enum Gradient {
    GrayGradient=0,
    BlueGreenRedGradient,
    BlackBlueGreenRedWhiteGradient,
    BlackMagentaRedYellowWhiteGradient,
    BlueRedYellowWhiteGradient,
    BlueRedGradient,
    BlueMagentaRedGradient
  };

  class HSVGradientColor {
  public:
  HSVGradientColor( int hue, int sat, int val, double frac, bool hsv=true )
    : Hue( hue ), Sat( sat ), Val( val ), Frac( frac ), Hsv( hsv ) {};
    int Hue;
    int Sat;
    int Val;
    double Frac;
    bool Hsv;
  };

    /*! Positions for tic marks. */
  enum Tics {
    In=0, Out=1, Centered=2
  };

    /*! Different fonts. */
  enum Fonts {
    DefaultF, Helvetica, Times, Courier, Symbols
  };

    /*! Different coordinate systems for labels and keys. */
  enum Coordinates {
    First, FirstX, FirstY, Second, SecondX, SecondY, 
    Graph, GraphX, GraphY, Screen, 
    FirstAxis, SecondAxis, FirstMargin, SecondMargin,
    Pixel
  };

    /*! Justification of labels. */
  enum Justification {
    Left, Right, Center
  };

    /*! Autoscale ranges, margins, and tic mark increments. */
  static const double AutoScale;
    /*! Autoscale ranges to integer tic marks
        but use fallback ranges as minimum ranges. */
  static const double AutoMinScale;
    /*! Autoscale ranges. */
  static const double ExactScale;
    /*! Autoscale ranges but use fallback ranges as minimum ranges. */
  static const double ExactMinScale;
    /*! Adjust tic mark increments dynamically to plot size. */
  static const double DynamicScale;
    /*! A value greater or equal to \a AnyScale 
        has a special autoscale meaning. */
  static const double AnyScale;

  static const int MaxAxis = 2;
    /*! Possible axis combinations. */
  enum Axis { X1Y1=0, X1Y2=1, X2Y1=2, X2Y2=3 };


  /*! 
    \class RGBColor
    \author Jan Benda
    \brief An RGB-Color. 
  */
  class RGBColor
  {
  public:
    RGBColor( const RGBColor &rgb ) { Red=rgb.Red; Green=rgb.Green; Blue=rgb.Blue; };
    RGBColor( int r, int g, int b ) { Red=r; Green=g; Blue=b; };
    RGBColor( void ) { Red=0; Green=0; Blue=0; };
    bool operator==( const RGBColor &c );
    unsigned char red( void ) const { return Red; };
    void setRed( int r ) { Red = r; };
    unsigned char green( void ) const { return Green; };
    void setGreen( int g ) { Green = g; };
    unsigned char blue( void ) const { return Blue; };
    void setBlue( int b ) { Blue = b; };
      /*! Multiply each color with \a f. */
    RGBColor lighten( double f ) const;
  private:
    unsigned char Red;
    unsigned char Green;
    unsigned char Blue;
  };

  /*! 
    \class LineStyle
    \author Jan Benda
    \brief Style of a plotted line (color, width, and dash-type).
  */
  
  class LineStyle
  {
    
  public:
    
    LineStyle( void ) : Color( Plot::Transparent ), Width( 0 ), 
			Dash( Plot::Solid ) {};
    LineStyle( int color, int width=1, Plot::Dash dash=Plot::Solid ) 
      : Color( color ), Width( width ), Dash( dash ) {};
    
    int color( void ) const { return Color; };
    void setColor( int color ) { Color = color; };
    
    int width( void ) const { return Width; };
    void setWidth( int width ) { Width = width >= 0 ? width : 0; };
    
    Plot::Dash dash( void ) const { return Dash; };
    void setDash( Plot::Dash dash ) { Dash = dash; };
    
    
  private:
    
    int Color;
    int Width;
    Plot::Dash Dash;
    
  };


  /*! 
    \class PointStyle
    \author Jan Benda
    \brief Style of a plotted point (point type, size, color, and fill color).
  */

  class PointStyle
  {

  public:

    PointStyle( void )
      : Type( Plot::Circle ), Size( 6 ), 
	Color( Plot::Transparent ), Fill( Plot::Transparent ) {};
    PointStyle( Plot::Points type, int size, int color, int fill=-2 )
      : Type( type ), Size( size > 0 ? size : 0 ), 
	Color( color >= Plot::Transparent ? color : Plot::Transparent ), 
        Fill( fill == -2 ? Color : fill ) {};
    
    Plot::Points type( void ) const { return Type; };
    void setType( Plot::Points type ) { Type = type; };
    
    int size( void ) const { return Size; };
    void setSize( int size ) { Size = size > 0 ? size : 0; };
    
    int color( void ) const { return Color; };
    void setColor( int color ) { Color = color >= Plot::Transparent ? color : Plot::Transparent; };
    
    int fillColor( void ) const { return Fill; };
    void setFillColor( int fill ) { Fill = fill >= Plot::Transparent ? fill : Plot::Transparent; };


  private:

    Plot::Points Type;
    int Size;
    int Color;
    int Fill;
  };
  

  /*! 
    \class Position
    \author Jan Benda
    \brief Position of a point or a label.
  */

  class Position
  {

  public:

      /*! Constructor. */
    Position( void )
      : XPos( 0.0 ), XCoor( Plot::First ), YPos( 0.0 ), YCoor( Plot::First ) {};
    Position( double x, Coordinates xcoor, double y, Coordinates ycoor )
      : XPos( x ), XCoor( xcoor ), YPos( y ), YCoor( ycoor ) {};

    double xpos( void ) const { return XPos; };
    void setXPos( double x, Coordinates xcoor=Plot::First ) { XPos = x; XCoor = xcoor; };

    Coordinates xcoor( void ) const { return XCoor; };
    void setXCoor( Coordinates xcoor ) { XCoor = xcoor; };

    double ypos( void ) const { return YPos; };
    void setYPos( double y, Coordinates ycoor=Plot::First ) { YPos = y; YCoor = ycoor; };

    Coordinates ycoor( void ) const { return YCoor; };
    void setYCoor( Coordinates ycoor ) { YCoor = ycoor; };


  private:

      /*! x value of position. */
    double XPos;
      /*! Coordinate system of x value. */
    Coordinates XCoor;
      /*! y value of position. */
    double YPos;
      /*! Coordinate system of y value. */
    Coordinates YCoor;

  };


  /*! 
    \class DataElement
    \author Jan Benda
    \brief Manages a single 2-D data item for plotting lines.
  */

  class DataElement
  {
    friend class Plot;

  public:

    enum DataTypes { SinglePoint, TwoPoints, Map, Events };

    DataElement( DataTypes dt );
    virtual ~DataElement( void );

    void setAxis( Plot::Axis axis );
    void setAxis( int xaxis, int yaxis );
    void setLineIndex( long inx );
    long lineIndex() const;
    void setPointIndex( long inx );
    long pointIndex() const;
    void setLine( const Plot::LineStyle &style );
    void setLine( int lcolor=Transparent, int lwidth=1,
		  Plot::Dash ldash=Solid );
    void setPoint( const Plot::PointStyle &style );
    void setPoint( Points ptype=Circle, int psize=10,
		   int pcolor=Transparent, int pfill=Transparent );
    void setStyle( const Plot::LineStyle &lstyle, 
		   const Plot::PointStyle &pstyle );
    void setStyle( int lcolor=Transparent, int lwidth=1,
		   Plot::Dash ldash=Solid, 
		   Points ptype=Circle, int psize=10,
		   int pcolor=Transparent, int pfill=Transparent );
      /*! The index of the first data point in the x-range \a x1, \a x2
          and the y-range \a y1, \a y2. */
    virtual long first( double x1, double y1, double x2, double y2 ) const =0;
      /*! The index of the last data point in the x-range \a x1, \a x2
          and the y-range \a y1, \a y2. */
    virtual long last( double x1, double y1, double x2, double y2 ) const =0;
      /*! Returns the \a x coordinate and \a y coordinate of the
          data point at index\ a index. */
    virtual void point( long index, double &x, double &y ) const =0;
    virtual void errors( long index, double &up, double &down ) const {};
    virtual void vector( long index, double &a, double &l ) const {};
      /*! Can be reimplemented for some initialization
	  before initializing and drawing the plot.
          \return \c true if the data changed */
    virtual bool init( void ) { return false; };
      /*! Returns a sensible x-range \a xmin and \a xmax for
	  a given y-range \a ymin, \a ymax. */
    virtual void xminmax( double &xmin, double &xmax, double ymin, double ymax ) const { xmin=-10.0; xmax=10.0; };
      /*! Returns a sensible y-range \a ymin and \a ymax for
	  a given x-range \a xmin, \a xmax. */
    virtual void yminmax( double xmin, double xmax, double &ymin, double &ymax ) const { ymin=-10.0; ymax=10.0; };
      /*! Passes the current ranges and pixel positions to the data element
          after initializing but before drawing the plot. */
    virtual void setRange( double xmin[MaxAxis], double xmax[MaxAxis], double ymin[MaxAxis], double ymax[MaxAxis],
			   int xpmin, int xpmax, int ypmin, int ypmax ) {};

  protected:

    bool Own;
    int XAxis;
    int YAxis;
    int LineIndex;
    int PointIndex;
    DataTypes DataType;
    Plot::LineStyle Line;
    Plot::PointStyle Point;

  };


  /*! 
    \class SurfaceElement
    \author Jan Benda
    \brief Manages a single 3-D data item for plotting surfaces.
  */

  class SurfaceElement
  {
    friend class Plot;

  public:

    SurfaceElement( void );
    virtual ~SurfaceElement( void );

    void setAxis( Plot::Axis axis );
    void setAxis( int xaxis, int yaxis );
    int gradient( void ) const;
    void setGradient( int gradient );
      /*! The index of the first column in the x-range \a x1, \a x2. */
    virtual long firstX( double x1, double x2 ) const =0;
      /*! The index of the last column in the x-range \a x1, \a x2. */
    virtual long lastX( double x1, double x2 ) const =0;
      /*! The index of the first row in the y-range \a y1, \a y2. */
    virtual long firstY( double y1, double y2 ) const =0;
      /*! The index of the last column in the y-range \a y1, \a y2. */
    virtual long lastY( double y1, double y2 ) const =0;
      /*! Returns in \a x1, \a y1, \a x2, and \a y2 the rectangle
	  that gets the value \a z of the
          data point at row index \a rindex and column index\ a cindex. */
    virtual void point( long rindex, long cindex,
			double &x1, double &y1, double &x2, double &y2,
			double &z ) const =0;
      /*! Can be reimplemented for some initialization
	  before initializing and drawing the plot.
          \return \c true if the data changed */
    virtual bool init( void ) { return true; };
      /*! Returns a sensible x-range \a xmin and \a xmax for
	  a given y-range \a ymin, \a ymax. */
    virtual void xminmax( double &xmin, double &xmax, double ymin, double ymax ) const { xmin=-10.0; xmax=10.0; };
      /*! Returns a sensible y-range \a ymin and \a ymax for
	  a given x-range \a xmin, \a xmax. */
    virtual void yminmax( double xmin, double xmax, double &ymin, double &ymax ) const { ymin=-10.0; ymax=10.0; };
      /*! Passes the current ranges and pixel positions to the data element
          after initializing but before drawing the plot. */
    virtual void setRange( double xmin[MaxAxis], double xmax[MaxAxis], double ymin[MaxAxis], double ymax[MaxAxis],
			   int xpmin, int xpmax, int ypmin, int ypmax ) {};

  protected:

    bool Own;
    int XAxis;
    int YAxis;
    int GradientIndex;

  };


#ifdef HAVE_LIBRELACSSHAPES
  /*! 
    \class PolygonElement
    \author Jan Benda
    \brief Manages a single closed polygon that can be stroked and filled.
  */

  class PolygonElement
  {
    friend class Plot;

  public:

    PolygonElement( const vector<double> &x, const vector<double> &y,
		    const Polygon &poly, int id, double distance,
		    const QPen &pen, const QBrush &brush );
    ~PolygonElement( void );

    void setAxis( Plot::Axis axis );
    void setAxis( int xaxis, int yaxis );
      /*! Returns a sensible x-range \a xmin and \a xmax for
	  a given y-range \a ymin, \a ymax. */
    void xminmax( double &xmin, double &xmax, double ymin, double ymax ) const;
      /*! Returns a sensible y-range \a ymin and \a ymax for a given
	  x-range \a xmin, \a xmax. */
    void yminmax( double xmin, double xmax, double &ymin, double &ymax ) const;

  protected:

    int Id;
    int XAxis;
    int YAxis;
    vector<double> X;
    vector<double> Y;
    Polygon Poly;
    double Distance;
    QPen Pen;
    QBrush Brush;
  };
#endif


    /*! Constructs a plot with KeepMode \a keep.
        If you set \a keep to Plot::Pointer and
	you are using multible threads, then
	you need to provide a mutex for locking the data
	via setDataMutex(). */
  Plot( KeepMode keep, QWidget *parent=0 );
  Plot( QWidget *parent=0 );
  Plot( KeepMode keep, bool subwidget, int id=0, MultiPlot *mp=0 );
  ~Plot( void );

  void keepData( void ) { Keep = Copy; };
  void keepPointer( void ) { Keep = Pointer; };
  KeepMode keep( void ) const { return Keep; };

    /*! Lock the plot mutex. */
  void lock( void );
    /*! Try to lock the plot mutex for at maximum \a timeoutms milliseconds. */
  bool tryLock( int timeoutms=1 );
    /*! Unlock the plot mutex. */
  void unlock( void );

    /*! Provide a mutex that is used by Plot to lock
        access to data while they are plotted. 
        If you want to change the mutex, you first have to
        disable the mutex by calling clearDataMutex().
        \sa setDataMutex(QReadWriteLock*), lockData() */
  void setDataMutex( QMutex *mutex );
    /*! Provide a mutex that is used by Plot to lock
        reading access to data while they are plotted. 
        If you want to change the mutex, you first have to
        disable the mutex by calling clearDataMutex().
        \sa setDataMutex(QMutex*), lockData() */
  void setDataMutex( QReadWriteLock *mutex );
    /*! Disables the data mutex. \sa setDataMutex() */
  void clearDataMutex( void );

    /*! Lock the data mutex if it was set by setDataMutex() before.
        \sa tryLockData(), unlockData() */
  void lockData( void );
    /*! Try to lock the data mutex for reading.
        Returns \c true if the lock was obtained within \a timeout milliseconds.
	\sa lockData(), setDataMutex(), clearDataMutex() */
  bool tryLockData( int timeout=1 );
    /*! Unlock the data mutex if it was set by setDataMutex() before.
        \sa lockData(), tryLockData() */
  void unlockData( void );
    /*! Returns \c true if \a this has the same data mutex as \a p.
        \sa setDataMutex() */
  bool equalDataMutex( const Plot &p ) const;
    /*! Returns \c true if no data mutex is set.
        \sa setDataMutex() */
  bool noDataMutex( void ) const;

    /*! If this Plot is part of a MultiPlot, then this function specifies
        the position of the Plot within the MultiPlot.
        \param[in] x the x-coordinate of the lower-left corner
	\param[in] y the x-coordinate of the lower-left corner
        the coordinates range from 0 to 1 for the entire plot area of the MultiPlot.
	\sa setSize() */
  void setOrigin( double x, double y );
    /*! Return the origin of this Plot. \sa setOrigin() */
  void origin( double &x, double &y ) const;
    /*! If this Plot is part of a MultiPlot, then this function specifies
        the size of the Plot within the MultiPlot.
        \param[in] w the width of the plot
	\param[in] h the height of the plot
        \a w and \a h range from 0 to 1 for the entire plot area of the MultiPlot.
	\sa setOrigin() */
  void setSize( double w, double h );
    /*! Return the size of this Plot. \sa setSize() */
  void size( double &x, double &y ) const;
    /*! Used by MultiPlot to provide the size used by the MultiPlot. */
  void scale( int width, int height );
    /*! If this plot is part of a MultiPlot, this Plot can be hidden
        (not drawn) by setting \a skip to \c true. */
  void setSkip( bool skip=true );
    /*! Returns \c true if this Plot is not to be drawn. \sa setSkip() */
  bool skip( void ) const;
    /*! True if \a xpixel, \a ypixel is inside the plot screen
        handled by this widget. */
  bool inside( int xpixel, int ypixel );

    /*! \return the width of the plot area (the part spanned by the axis) in pixels.
        Only defined after the first draw().
        \sa  pixelPlotHeight(), pixelScreenWidth() */
  int pixelPlotWidth( void ) const;
    /*! \return the height of the plot area (the part spanned by the axis) in pixels.
        Only defined after the first draw().
        \sa  pixelPlotWidth(), pixelScreenHeight() */
  int pixelPlotHeight( void ) const;
    /*! \return the width of the plot screen (including the margins
        around the axis) in pixels. \sa pixelScreenHeight(), pixelPlotWidth() */
  int pixelScreenWidth( void ) const;
    /*! \return the height of the plot screen (including the margins
        around the axis) in pixels. \sa pixelScreenWidth(), pixelPlotHeight() */
  int pixelScreenHeight( void ) const;

    /*! Computes the pixel coordinate for the specified x position. 
        \param[in] xpos the position
        \param[in] xcoor the coordinate system
        \return the pixel coordinate
        \sa yPixel() */
  int xPixel( double xpos, Coordinates xcoor=FirstX ) const;
    /*! Computes the x-pixel coordinate for the specified position. 
        \param[in] pos the position
        \return the pixel coordinate
        \sa yPixel() */
  int xPixel( const Position &pos ) const;
    /*! Computes the pixel coordinate for the specified y position. 
        \param[in] ypos the position
        \param[in] ycoor the coordinate system
        \return the pixel coordinate
        \sa xPixel() */
  int yPixel( double ypos, Plot::Coordinates ycoor=FirstY ) const;
    /*! Computes the y-pixel coordinate for the specified position. 
        \param[in] pos the position
        \return the pixel coordinate
        \sa xPixel() */
  int yPixel( const Position &pos ) const;
    /*! The number of pixels corresponding to \a w times the current font  width
        as used by setLMarg(), setRMarg(), setTMarg(), setBMarg() 
        and the axis coordinates. */
  int fontPixel( double w ) const;

    /*! Set the size of the standard fontb to \a pixel pixels. */
  void setFontSize( double pixel );

  int addColor( const RGBColor &rgb );
  int addColor( int r, int g, int b );
  RGBColor color( int c );

  double lmarg( void ) const { return LMarg; };
  double rmarg( void ) const { return RMarg; };
  double tmarg( void ) const { return TMarg; };
  double bmarg( void ) const { return BMarg; };

  void setLMarg( double lmarg );
  void setRMarg( double rmarg );
  void setTMarg( double tmarg );
  void setBMarg( double bmarg );
  void setScreenBorder( int marg );

  void setBackgroundColor( int c ) { BackgroundColor = c; };
  void setPlotColor( int c ) { PlotColor = c; };

  void setAutoScaleLMarg( void ) { LMargAutoScale = true; };
  void setAutoScaleRMarg( void ) { RMargAutoScale = true; };
  void setAutoScaleTMarg( void ) { TMargAutoScale = true; };
  void setAutoScaleBMarg( void ) { BMargAutoScale = true; };

  double xminRange( void ) const { return XMin[0]; };
  double xmaxRange( void ) const { return XMax[0]; };
  double yminRange( void ) const { return YMin[0]; };
  double ymaxRange( void ) const { return YMax[0]; };
  double zminRange( void ) const { return ZMin; };
  double zmaxRange( void ) const { return ZMax; };

  void setXRange( double xmin, double xmax );
  void setYRange( double ymin, double ymax );
  void setZRange( double zmin, double zmax );

  void setXFallBackRange( double xmin, double xmax );
  void setYFallBackRange( double ymin, double ymax );

  void setAutoScaleX( void );
  void setAutoScaleY( void );
  void setAutoScaleXY( void );
  void noAutoScaleX( void );
  void noAutoScaleY( void );
  void noAutoScaleXY( void );

  double x2minRange( void ) const { return XMin[1]; };
  double x2maxRange( void ) const { return XMax[1]; };
  double y2minRange( void ) const { return YMin[1]; };
  double y2maxRange( void ) const { return YMax[1]; };

  void setX2Range( double xmin, double xmax );
  void setY2Range( double ymin, double ymax );

  void setX2FallBackRange( double xmin, double xmax );
  void setY2FallBackRange( double ymin, double ymax );

  void setAutoScaleX2( void );
  void setAutoScaleY2( void );
  void setAutoScaleX2Y2( void );
  void noAutoScaleX2( void );
  void noAutoScaleY2( void );
  void noAutoScaleX2Y2( void );

  void setTicsStyle( Tics pos, int color=Transparent, int len=5, int width=1 );

  double xticsIncr( void ) const { return XTicsIncr[0]; };
  double yticsIncr( void ) const { return YTicsIncr[0]; };
  double xticsStart( void ) const { return XTicsStart[0]; };
  double yticsStart( void ) const { return YTicsStart[0]; };

  void setXTics( double incr=DynamicScale, int pos=1 );
  void setYTics( double incr=DynamicScale, int pos=1 );
  void setXTics( double start, double incr, int pos=1 );
  void setYTics( double start, double incr, int pos=1 );
  void setXTics( int pos );
  void setYTics( int pos );
  void noXTics( void ) { XTics[0] = 0; };
  void noYTics( void ) { YTics[0] = 0; };

  double x2ticsIncr( void ) const { return XTicsIncr[1]; };
  double y2ticsIncr( void ) const { return YTicsIncr[1]; };
  double x2ticsStart( void ) const { return XTicsStart[1]; };
  double y2ticsStart( void ) const { return YTicsStart[1]; };

  void setX2Tics( double incr=DynamicScale, int pos=1 );
  void setY2Tics( double incr=DynamicScale, int pos=1 );
  void setX2Tics( double start, double incr, int pos=1 );
  void setY2Tics( double start, double incr, int pos=1 );
  void setX2Tics( int pos );
  void setY2Tics( int pos );
  void noX2Tics( void ) { XTics[1] = 0; };
  void noY2Tics( void ) { YTics[1] = 0; };

  void noTics( void );

  void setMinXTics( double min=AutoScale );
  void setMinYTics( double min=AutoScale );
  void setMinX2Tics( double min=AutoScale );
  void setMinY2Tics( double min=AutoScale );

  void setXGrid( int color=White, int width=1, Dash dash=Dotted );
  void setXGrid( const LineStyle &style );
  void setYGrid( int color=White, int width=1, Dash dash=Dotted );
  void setYGrid( const LineStyle &style );
  void setXYGrid( int color=White, int width=1, Dash dash=Dotted );
  void setXYGrid( const LineStyle &style );
  void noXGrid( void ) { XGrid[0] = false; };
  void noYGrid( void ) { YGrid[0] = false; };
  void noXYGrid( void ) { XGrid[0] = false; YGrid[0] = false; };

  void setX2Grid( int color=White, int width=1, Dash dash=Dotted );
  void setX2Grid( const LineStyle &style );
  void setY2Grid( int color=White, int width=1, Dash dash=Dotted );
  void setY2Grid( const LineStyle &style );
  void setX2Y2Grid( int color=White, int width=1, Dash dash=Dotted );
  void setX2Y2Grid( const LineStyle &style );
  void noX2Grid( void ) { XGrid[1] = false; };
  void noY2Grid( void ) { YGrid[1] = false; };
  void noX2Y2Grid( void ) { XGrid[1] = false; YGrid[1] = false; };

  void noGrid( void );

  void setFormatX( const string &format ) { XTicsFormat[0] = format; }; 
  void setFormatY( const string &format ) { YTicsFormat[0] = format; }; 
  void setFormatXY( const string &format ) { XTicsFormat[0] = format; YTicsFormat[0] = format; }; 

  void setFormatX2( const string &format ) { XTicsFormat[1] = format; }; 
  void setFormatY2( const string &format ) { YTicsFormat[1] = format; }; 
  void setFormatX2Y2( const string &format ) { XTicsFormat[1] = format; YTicsFormat[1] = format; }; 

  void setFormat( const string &format );

  void setXLabel( const string &label ) { XLabel[0].Text = label; };
  void setXLabel( const string &label, int color, double size=1.0, 
		  Fonts font=DefaultF, int bcolor=Transparent, 
		  int fwidth=0, int fcolor=Transparent );
  void setXLabelPos( double xpos, Coordinates xcoor=Graph,
		     double ypos=-1.0, Coordinates ycoor=FirstAxis,
		     Justification just=Right, double angle=0.0 );

  void setYLabel( const string &label ) { YLabel[0].Text = label; };
  void setYLabel( const string &label, int color, double size=1.0, 
		  Fonts font=DefaultF, int bcolor=Transparent, 
		  int fwidth=0, int fcolor=Transparent );
  void setYLabelPos( double xpos=0.0, Coordinates xcoor=Screen,
		     double ypos=0.0, Coordinates ycoor=SecondAxis,
		     Justification just=Left, double angle=0.0 );

  void setX2Label( const string &label ) { XLabel[1].Text = label; };
  void setX2Label( const string &label, int color, double size=1.0, 
		   Fonts font=DefaultF, int bcolor=Transparent, 
		   int fwidth=0, int fcolor=Transparent );
  void setX2LabelPos( double xpos, Coordinates xcoor=Graph,
		      double ypos=-1.0, Coordinates ycoor=FirstAxis,
		      Justification just=Right, double angle=0.0 );

  void setY2Label( const string &label ) { YLabel[1].Text = label; };
  void setY2Label( const string &label, int color, double size=1.0, 
		   Fonts font=DefaultF, int bcolor=Transparent, 
		   int fwidth=0, int fcolor=Transparent );
  void setY2LabelPos( double xpos=0.0, Coordinates xcoor=Screen,
		      double ypos=0.0, Coordinates ycoor=SecondAxis,
		      Justification just=Left, double angle=0.0 );

  void setTitle( const string &title ) { Title.Text = title; };
  void setTitle( const string &title, int color, double size=1.0, 
		 Fonts font=DefaultF, int bcolor=Transparent, 
		 int fwidth=0, int fcolor=Transparent );
  void setTitlePos( double xpos=1.0, Coordinates xcoor=Graph,
		    double ypos=0.0, Coordinates ycoor=SecondAxis,
		    Justification just=Right, double angle=0.0 );

  int setLabel( const string &label, double x, double y, 
		Justification just=Left, double angle=0.0 );
  int setLabel( const string &label, double x, Coordinates xcoor,
		double y, Coordinates ycoor, Justification just=Left,
		double angle=0.0 );
  int setLabel( const string &label, double x, Coordinates xcoor,
		double y, Coordinates ycoor, Justification just,
		double angle, int color, double size=1.0, 
		Fonts font=DefaultF, int bcolor=Transparent, 
		int fwidth=0, int fcolor=Transparent );
  int setLabel( int index, const string &label );
    /*! Remove all labels from the plot. */
  void clearLabels( void );
    /*! Remove label with index \a index from the plot. */
  void clearLabel( int index );

    /*! Plot data point \a x, \a y. */
  int plotPoint( double x, Coordinates xcoor, 
		 double y, Coordinates ycoor, int lwidth,
		 Points ptype, double size, Coordinates sizecoor,
		 int pcolor=Red, int pfill=Transparent );

    /*! Plot a vertical line at position \a x of the first coordinate system. */
  int plotVLine( double x, const LineStyle &line );
  int plotVLine( double x, int lcolor=White, int lwidth=1, Dash ldash=Solid )
    { return plotVLine( x, LineStyle( lcolor, lwidth, ldash ) ); };
    /*! Plot a horizontal line at height \a y of the first coordinate system. */
  int plotHLine( double y, const LineStyle &line );
  int plotHLine( double y, int lcolor=White, int lwidth=1, Dash ldash=Solid )
    { return plotHLine( y, LineStyle( lcolor, lwidth, ldash ) ); };
    /*! Plot a line connecting the points (\a x1, \a y1) and (\a x2, \a y2) 
        of the first coordinate system. */
  int plotLine( double x1, double y1, double x2, double y2, 
		const LineStyle &line );
  int plotLine( double x1, double y1, double x2, double y2, 
		int lcolor=White, int lwidth=1, Dash ldash=Solid )
    { return plotLine( x1, y1, x2, y2, LineStyle( lcolor, lwidth, ldash ) ); };

    /*! Plot the pairs given in \a x and \a y. */
  template< typename T, typename R >
  int plot( const T &x, const R &y,
	    const LineStyle &line, const PointStyle &point=PointStyle() );
  template< typename T, typename R >
  int plot( const T &x, const R &y,
	    const PointStyle &point, const LineStyle &line=LineStyle() )
    { return plot( x, y, line, point ); };
  template< typename T, typename R >
  int plot( const T &x, const R &y, 
	    int lcolor, int lwidth=1, Dash ldash=Solid, 
	    Points ptype=Circle, int psize=10, int pcolor=Transparent, int pfill=Transparent )
    { return plot( x, y, LineStyle( lcolor, lwidth, ldash ), 
		   PointStyle( ptype, psize, pcolor, pfill ) ); };

    /*! Plot the pairs given in \a x and \a y. 
        The original x-data are scaled by \a xscale. */
  template< typename T, typename R >
  int plot( const T &x, const R &y, double xscale,
	    const LineStyle &line, const PointStyle &point=PointStyle() );
  template< typename T, typename R >
  int plot( const T &x, const R &y, double xscale,
	    const PointStyle &point, const LineStyle &line=LineStyle() )
    { return plot( x, xscale, y, line, point ); };
  template< typename T, typename R >
  int plot( const T &x, const R &y, double xscale, 
	    int lcolor, int lwidth=1, Dash ldash=Solid, 
	    Points ptype=Circle, int psize=10, int pcolor=Transparent, int pfill=Transparent )
    { return plot( x, xscale, y, LineStyle( lcolor, lwidth, ldash ), 
		   PointStyle( ptype, psize, pcolor, pfill ) ); };

    /*! Plot the Map \a data. 
        The original x-data are scaled by \a xscale. */
  template< typename T >
  int plot( const Map<T> &data, double xscale,
	    const LineStyle &line, const PointStyle &point=PointStyle() );
  template< typename T >
  int plot( const Map<T> &data, double xscale,
	    const PointStyle &point, const LineStyle &line=LineStyle() )
    { return plot( data, xscale, line, point ); };
  template< typename T >
  int plot( const Map<T> &data, double xscale,
	    int lcolor, int lwidth=1, Dash ldash=Solid, 
	    Points ptype=Circle, int psize=10, int pcolor=Transparent, int pfill=Transparent )
    { return plot( data, xscale, 
		   LineStyle( lcolor, lwidth, ldash ), 
		   PointStyle( ptype, psize, pcolor, pfill ) ); };

    /*! Plot the SampleData \a data. 
        The original x-data are scaled by \a xscale. */
  template< typename T >
  int plot( const SampleData<T> &data, double xscale,
	    const LineStyle &line, const PointStyle &point=PointStyle() );
  template< typename T >
  int plot( const SampleData<T> &data, double xscale,
	    const PointStyle &point, const LineStyle &line=LineStyle() )
    { return plot( data, xscale, line, point ); };
  template< typename T >
  int plot( const SampleData<T> &data, double xscale,
	    int lcolor, int lwidth=1, Dash ldash=Solid, 
	    Points ptype=Circle, int psize=10, int pcolor=Transparent, int pfill=Transparent )
    { return plot( data, xscale, 
		   LineStyle( lcolor, lwidth, ldash ), 
		   PointStyle( ptype, psize, pcolor, pfill ) ); };

    /*! Plot the events given in \a x at hight \a y. */
  template< typename T >
  int plot( const T &x, double tscale, 
	    double y, Coordinates ycoor, int lwidth,
	    Points ptype, double size, Coordinates sizecoor,
	    int pcolor=Red, int pfill=Transparent );

    /*! Plot the events given in \a x at hight \a y. */
  int plot( const EventData &x, int origin, double offset, double tscale, 
	    double y, Coordinates ycoor, int lwidth,
	    Points ptype, double size, Coordinates sizecoor,
	    int pcolor=Red, int pfill=Transparent );

#ifdef HAVE_LIBRELACSDAQ
    /*! Plot InData \a data. 
        The original time is scaled by \a tscale. */
  int plot( const InData &data, int origin, double offset, double tscale,
	    const LineStyle &line, const PointStyle &point=PointStyle() );
  int plot( const InData &data, int origin, double offset, double tscale,
	    const PointStyle &point, const LineStyle &line=LineStyle() )
    { return plot( data, origin, offset, tscale, line, point ); };
  int plot( const InData &data, int origin, double offset, double tscale,
	    int lcolor, int lwidth=1, Dash ldash=Solid, 
	    Points ptype=Circle, int psize=10, int pcolor=Transparent, int pfill=Transparent )
    { return plot( data, origin, offset, tscale, 
		   LineStyle( lcolor, lwidth, ldash ), 
		   PointStyle( ptype, psize, pcolor, pfill ) ); };

  int plot( const EventData &events, const InData &data, int origin, double offset, double tscale, 
	    int lwidth, Points ptype, double size, Coordinates sizecoor,
	    int pcolor=Red, int pfill=Transparent );

    /*! Plot the OutData \a data. 
        The original x-data are scaled by \a xscale. */
  int plot( const OutData &data, double xscale,
	    const LineStyle &line, const PointStyle &point=PointStyle() );
  int plot( const OutData &data, double xscale,
	    const PointStyle &point, const LineStyle &line=LineStyle() )
    { return plot( data, xscale, line, point ); };
  int plot( const OutData &data, double xscale,
	    int lcolor, int lwidth=1, Dash ldash=Solid, 
	    Points ptype=Circle, int psize=10, int pcolor=Transparent, int pfill=Transparent )
    { return plot( data, xscale, 
		   LineStyle( lcolor, lwidth, ldash ), 
		   PointStyle( ptype, psize, pcolor, pfill ) ); };
#endif

    /*! Reference to the data element \a i. */
  DataElement &operator[]( int i ) { return *LineData[i]; };
    /*! Const reference to the data element \a i. */
  const DataElement &operator[]( int i ) const { return *LineData[i]; };
    /*! Reference to the last data element in the list. */
  DataElement &back( void ) { return *LineData.back(); };
    /*! Const reference to the last data element in the list. */
  const DataElement &back( void ) const { return *LineData.back(); };
    /*! Reference to the first data element in the list. */
  DataElement &front( void ) { return *LineData.front(); };
    /*! Const reference to the first data element in the list. */
  const DataElement &front( void ) const { return *LineData.front(); };

  int plot( const SampleData<SampleDataD> &data, double xscale, int gradient=0 );

#ifdef HAVE_LIBRELACSSHAPES
    /*! Return the transformation matrix that defines the (perspective) projection
        onto the x-y plane. */
  Transform projection( void ) const;
    /*! Put the view point (camera) at position \a view.
        Computes the corresponding projection matrix (rotation and perspective).
        The viewpoint needs to be set before plotting shapes. */
  void setViewPoint( const Point &view );
    /*! Return the position (direction) of the light source. */
  Point lightSource( void ) const;
    /*! Set the position (direction) of the light source to \a lightsource.
        The contrast defines how much the color is modulated (0: none, 1: maximal).
        The light source and contrast needs to be set before plotting shapes. */
  void setLightSource( const Point &lightsource, double contrast=0.5 );
    /*! Plot the Zone \a zone using the current projection matrix. */
  int plot( const Zone &zone, Color fillcolor, double alpha=1.0,
  	    int linecolor=Transparent, int width=1, Dash dash=Solid );
    /*! Plot the Shape \a shape using the current projection matrix. */
  int plot( const Shape &shape, Color fillcolor, double alpha=1.0,
	    int linecolor=Transparent, int width=1, Dash dash=Solid );
    /*! Plot the polygon \a polygon using the current projection matrix. */
  int plot( const Polygon &poly, Color fillcolor, double alpha=1.0,
	    int linecolor=Transparent, int width=1, Dash dash=Solid );
    /*! Plot the line from point \a x1 to point \a x2 using the current projection matrix. */
  int plot( const Point &x1, const Point &x2,
	    int linecolor=Transparent, int width=1, Dash dash=Solid );

    /*! Remove all 3-D polygons from the plot. */
  void clearPolygons( void );
    /*! Remove all 3-D polygons with id \a id from the plot. */
  void clearPolygons( int id );
#endif

    /*! Remove all 2-D plot data from the plot. */
  void clearData( void );
    /*! Remove 2-D plot data with index \a index from the plot. */
  void clearData( int index );
    /*! Remove surface plot data from the plot. */
  void clearSurfaceData( void );

    /*!  Remove all 2-D, 3-D (surface), and shape plot data and labels from the plot. */
  void clear( void );

    /*! Give a hint for the prefered size of this widget. */
  virtual QSize sizeHint( void ) const;
    /*! Give a hint for the minimum size of this widget. */
  virtual QSize minimumSizeHint( void ) const;

  void draw( void );

  /*!
    \class MouseEvent
    \author Jan Benda
    \brief Handling a mouse event for the Plot class.
   */
  class MouseEvent
  {

    friend class Plot;


  public:

    MouseEvent( void );
    MouseEvent( int mode );
    MouseEvent( const MouseEvent &me );

    int xPixel( void ) const { return XPixel; };
    int yPixel( void ) const { return YPixel; };

    double xPos( void ) const { return XPos[0]; };
    double yPos( void ) const { return YPos[0]; };

    double xPos( int i ) const { return XPos[i]; };
    double yPos( int i ) const { return YPos[i]; };

    Coordinates xCoor( void ) const { return XCoor; };
    Coordinates yCoor( void ) const { return YCoor; };

    bool left( void ) const { return ( ( Mode & 1 ) > 0 ); };
    bool right( void ) const { return ( ( Mode & 2 ) > 0 ); };
    bool mid( void ) const { return ( ( Mode & 4 ) > 0 ); };
    bool leftOnly( void ) const { return ( ( Mode & (1+2+4) ) == 1 ); };
    bool rightOnly( void ) const { return ( ( Mode & (1+2+4) ) == 2 ); };
    bool midOnly( void ) const { return ( ( Mode & (1+2+4) ) == 4 ); };

    bool shift( void ) const { return ( ( Mode & 8 ) > 0 ); };
    bool control( void ) const { return ( ( Mode & 16 ) > 0 ); };
    bool alt( void ) const { return ( ( Mode & 32 ) > 0 ); };
    bool shiftOnly( void ) const { return ( ( Mode & (8+16+32) ) == 8 ); };
    bool controlOnly( void ) const { return ( ( Mode & (8+16+32) ) == 16 ); };
    bool altOnly( void ) const { return ( ( Mode & (8+16+32) ) == 32 ); };
    bool noKeys( void ) const { return ( ( Mode & ( 8+16+32 ) ) == 0 ); };

    bool pressed( void ) const { return ( ( Mode & 64 ) > 0 ); };
    bool released( void ) const { return ( ( Mode & 128 ) > 0 ); };
    bool moved( void ) const { return ( ( Mode & 256 ) > 0 ); };
    bool doubleClicked( void ) const { return ( ( Mode & 512 ) > 0 ); };
    bool pressedOnly( void ) const { return ( ( Mode & (64+128+256+512) ) == 128 ); };
    bool releasedOnly( void ) const { return ( ( Mode & (64+128+256+512) ) == 128 ); };
    bool movedOnly( void ) const { return ( ( Mode & (64+128+256+512) ) == 256 ); };
    bool doubleClickedOnly( void ) const { return ( ( Mode & (64+128+256+512) ) == 512 ); };

    void clearMouseButtons( void ) { Mode &= ~(1+2+4+64+128+256+512); };

    bool init( void ) const { return Init; };
    void setInit( void ) { Init = true; };

    void clear( void );
    bool valid( void ) const { return ( XPixel != 0xffff ); };

    bool used( void ) const { return Used; };
    void setUsed( void ) { Used = true; };


  private:

    int XPixel;
    int YPixel;
    double XPos[MaxAxis];
    double YPos[MaxAxis];
    Coordinates XCoor;
    Coordinates YCoor;
    int Mode;
    bool Init;
    bool Used;

  };

    /*! Enable mouse support as handled by the default implementation
        of mouseEvent() */
  void enableMouse( void );
    /*! Disable mouse support as handled by the default implementation
        of mouseEvent() */
  void disableMouse( void );
    /*! Returns \c true if the user has scaled the x1-range with the mouse. */
  bool zoomedXRange( void );
    /*! Returns \c true if the user has scaled the y1-range with the mouse. */
  bool zoomedYRange( void );
    /*! Returns \c true if the user has scaled the x2-range with the mouse. */
  bool zoomedX2Range( void );
    /*! Returns \c true if the user has scaled the y2-range with the mouse. */
  bool zoomedY2Range( void );
    /*! Returns \c true if the user has scaled the x1-range, y1-range,
        x2-range, or the y-range with the mouse. */
  bool zoomedRange( void );
    /*! The number of elements on the zoom stack. */
  bool ranges( void ) const;  
    /*! Set the ranges according to the last zoom-stack entry and remove it from the stack. */
  void popRanges( void );
    /*! Push the current ranges on the zoom stack. */
  void pushRanges( void );
    /*! Clears the zoom stack. */
  void resetRanges( void );

    /*! This factor is used by the mouseAnalyse() for
        scaling distances in y-direction relative to distances in x direction. */
  void setYShrinkFactor( double f );

    /*! Returns \c true if this plot has grabbed the mouse cursor. */
  bool mouseGrabbed( void ) const { return MouseGrabbed; };


public slots:

    /*! Controls whether userMouseEvent() should be called 
        from mouseEvent() on mouse move events.
        \param[in] enable set this to \c true if you want to have 
        userMouseEvent() called also on move events. */
  void setUserMouseTracking( bool enable );


signals:
  
    /*! The plot range was changed by the user.
        Both the Plot mutex and the MultiPlot mutex (if applicable) are locked. */
  void changedRange( void );
    /*! The plot range was changed by the user.
        Both the Plot mutex and the MultiPlot mutex (if applicable) are locked. */
  void changedRange( int );
    /*! This signal is emitted whenever the Plot widget receives
        a resizeEvent() before processing it. */
  void resizePlot( QResizeEvent *qre );
    /*! This signal is emitted whenever a mouse event in the Plot occured.
        A slot making use of the mouse event should call me.setUsed()
        to prevent further processing by the default mouse event handling functions.
	Neither the Plot nor the data are locked during the call of userMouseEvent().
        \param[in] me the coordinates and status of the mouse event */
  void userMouseEvent( Plot::MouseEvent &me );


protected:

    /*! Redraws the plot widget. */
  void draw( QPaintDevice *qpm, bool drawdata );
    /*! Handles the resize event. */
  void resizeEvent( QResizeEvent *qre );
    /*! Paints the entire plot. */
  void paintEvent( QPaintEvent *qpe );
  void customEvent( QEvent *qce );

    /*! Handles all kinds of mouse events.
        The current implementation supports
        moving and scaling of the axis
	and scaling of the plot.
        Right click restores previous ranges.
	Simply calls mouseZoomFirstX(), mouseZoomFirstY(), mouseZoomPlot(). */
  virtual void mouseEvent( MouseEvent &me );

    /*! Opens the plot popup menu (middle click). */
  void mouseMenu( MouseEvent &me );

    /*! Set the appropriate mouse cursor. */
  void mouseSetCursor( MouseEvent &me );
    /*! Zooms and moves first x-axis. */
  void mouseZoomMoveFirstX( MouseEvent &me );
    /*! Zooms and moves first y-axis. */
  void mouseZoomMoveFirstY( MouseEvent &me );
    /*! Zooms and moves plot. */
  void mouseZoomMovePlot( MouseEvent &me, bool move );

    /*! Analyse plot. */
  void mouseAnalyse( MouseEvent &me );

    /*! Calculates the mouse coordinates from the pixel position
        of the mouse. */
  void setMouseCoordinates( MouseEvent &me );
    /*! Translates \a qme into \a me. 
        Used by the reimplementations of the Qt mouse event handlers. */
  void readMouse( QMouseEvent *qme, MouseEvent &me );

    /*! The Qt mouse event handler for a mouse press event.
        Translates the event via readMouse() and calls mouseEvent(). */
  virtual void mousePressEvent( QMouseEvent *qme );
    /*! The Qt mouse event handler for a mouse release event.
        Translates the event via readMouse() and calls mouseEvent(). */
  virtual void mouseReleaseEvent( QMouseEvent *qme );
    /*! The Qt mouse event handler for a mouse doouble click event.
        Translates the event via readMouse() and calls mouseEvent(). */
  virtual void mouseDoubleClickEvent( QMouseEvent *qme );
    /*! The Qt mouse event handler for a mouse move event.
        Translates the event via readMouse() and calls mouseEvent(). */
  virtual void mouseMoveEvent( QMouseEvent *qme );

  QMenu *MouseMenu;
  QAction *MouseZoom;
  QAction *MouseMove;
  QAction *MouseZoomReset;
  QAction *MouseAnalyse;
  QAction *MouseDisable;
  QAction *MouseAction;  // the current mouse action mode
  bool MouseMenuClick;

  MouseEvent LastMouseEvent;
  bool MouseGrabbed;
  bool MouseMoveX;
  bool MouseZoomXMin;
  bool MouseZoomXMax;
  bool MouseMoveY;
  bool MouseZoomYMin;
  bool MouseZoomYMax;
  bool MouseMoveXY;
  bool MouseZoomOut;
  double MouseYShrink;

  class RangeCopy
  {
  public:
    RangeCopy( void );
    RangeCopy( const RangeCopy &rc );
    RangeCopy( const Plot *p ); 

  protected:
    friend class Plot;
    double XMin[MaxAxis];
    double XMax[MaxAxis];
    double YMin[MaxAxis];
    double YMax[MaxAxis];
    double XMinRange[MaxAxis];
    double XMaxRange[MaxAxis];
    double YMinRange[MaxAxis];
    double YMaxRange[MaxAxis];

  };

  deque< RangeCopy > MouseRangeStack;


protected slots:

  void mouseSelect( QAction *action );


private:

  void construct( KeepMode keep, bool subwidget=false,
		  int id=0, MultiPlot *mp=0 );

  int screenWidth( void ) const { return ScreenX2 - ScreenX1 + 1; };
  int screenHeight( void ) const { return ScreenY1 - ScreenY2 + 1; };

  void init( void );
  void initXRange( int axis );
  void initYRange( int axis );
  void initZRange( void );
  void initRange( void );
  void initTics( void );
  void initBorder( void );
  void initLines( void );
  void drawBorder( QPainter &paint );
  void drawTicMarks( QPainter &paint, int axis );
  void drawTicLabels( QPainter &paint, int axis );
  void drawAxis( QPainter &paint );
  void drawLabels( QPainter &paint );
  void drawData( QPainter &paint );
  void drawMouse( QPainter &paint );


#ifdef HAVE_LIBRELACSSHAPES
    /*! Add a polygon \a poly for plotting. */
  void addPolygon( const Polygon &poly, int id, Color fillcolor, double alpha,
		   int linecolor, int width, Dash dash );
#endif

    /*! Keep-mode for data to be plotted. */
  KeepMode Keep;

    /*! True if Plot is used as a subwidget of the MultiPlot-class. */
  bool SubWidget;
  MultiPlot *MP;

  bool UserMouseTracking;
  bool MouseDrawRect;
  int MouseX1;
  int MouseX2;
  bool MouseXMax;
  int MouseY1;
  int MouseY2;
  bool MouseYMax;
  vector< double > MouseXPos;
  vector< double > MouseYPos;
  vector< int > MouseDInx;
  vector< int > MousePInx;
  int Id;

    /*! Basic font size. */
  int FontSize;
    /*! Width of a '0' in the basic font. */
  int FontWidth;
    /*! Maximum height of the basic font. */
  int FontHeight;

    /*! Fraction of the widget size for the x coordinate
        of the lower left corner of the plot graph. */
  double XOrigin;
    /*! Fraction of the widget size for the y coordinate
        of the lower left corner of the plot graph. */
  double YOrigin;
    /*! Fraction of the widget size for the width of the plot graph. */
  double XSize;
    /*! Fraction of the widget size for the hight of the plot graph. */
  double YSize;
    /*! \c true if this Plot is not to be drawn. */
  bool Skip;

    /*! Pixel coordinates of the lower left corner of the whole plot (inclusively). */
  int ScreenX1, ScreenY1;
    /*! Pixel coordinates of the upper right corner of the whole plot (inclusively). */
  int ScreenX2, ScreenY2;

    /*! Pixel coordinates of the lower left corner of the plot itself (inclusively). */
  int PlotX1, PlotY1;
    /*! Pixel coordinates of the upper right corner of the plot itself (inclusively). */
  int PlotX2, PlotY2;

    /*! Horizontal margins. */
  int LMarg, RMarg;
    /*! Vertical margins. */
  int BMarg, TMarg;
    /*! autoscale horizontal margins. */
  bool LMargAutoScale, RMargAutoScale;
    /*! autoscale vertical margins. */
  bool BMargAutoScale, TMargAutoScale;
    /*! Add \a ScreenBorder to margins if they are set to be auto scaled. */
  int ScreenBorder;

    /*! currently used x-ranges of the plot. */
  double XMin[MaxAxis], XMax[MaxAxis];
    /*! currently used y-ranges of the plot. */
  double YMin[MaxAxis], YMax[MaxAxis];
    /*! requested x-ranges of the plot (probably autoscaling). */
  double XMinRange[MaxAxis], XMaxRange[MaxAxis];
    /*! requested y-ranges of the plot (probably autoscaling). */
  double YMinRange[MaxAxis], YMaxRange[MaxAxis];
    /*! fall back x-ranges of the plot. */
  double XMinFB[MaxAxis], XMaxFB[MaxAxis];
    /*! fall back y-ranges of the plot. */
  double YMinFB[MaxAxis], YMaxFB[MaxAxis];
    /*! previously used x-ranges of the plot. */
  double XMinPrev[MaxAxis], XMaxPrev[MaxAxis];
    /*! previously used y-ranges of the plot. */
  double YMinPrev[MaxAxis], YMaxPrev[MaxAxis];
    /*! currently used x-range of the plot. */
  double ZMin, ZMax;
    /*! requested z-range of the plot (probably autoscaling). */
  double ZMinRange, ZMaxRange;

    /*! Draw tics? 0: don't draw, 
        1: on the corresponding axis only, 2: on both axis. */
  int XTics[MaxAxis], YTics[MaxAxis];
    /*! Increment of ticmarks. */
  double XTicsIncr[MaxAxis], YTicsIncr[MaxAxis];
    /*! Minimum increment of ticmarks. */
  double XTicsMinIncr[MaxAxis], YTicsMinIncr[MaxAxis];
    /*! Start-value of ticmarks. */
  double XTicsStart[MaxAxis], YTicsStart[MaxAxis];
    /*! Autoscale increment. */
  char XTicsIncrAutoScale[MaxAxis], YTicsIncrAutoScale[MaxAxis];
    /*! Autoscale start value. */
  bool XTicsStartAutoScale[MaxAxis], YTicsStartAutoScale[MaxAxis];

    /*! Color of tic marks. */
  int TicsColor;
    /*! Len of tic marks. */
  int TicsLen;
    /*! Position of tic marks. */
  int TicsPos;
    /*! Width of tic marks. */
  int TicsWidth;

    /*! Format for tic labels. */
  string XTicsFormat[MaxAxis], YTicsFormat[MaxAxis];
    /*! Size of tic mark labels relative to default size. */
  double TicsLabelSize;
    /*! Font for tic mark labels. */
  int TicsLabelFont;
    /*! Color of tic labels. */
  int TicsLabelColor;

    /*! Margin needed for x1 tics. */
  int X1TicsLen;
    /*! Margin needed for y1 tics. */
  int Y1TicsLen;
    /*! Margin needed for x2 tics. */
  int X2TicsLen;
    /*! Margin needed for y2 tics. */
  int Y2TicsLen;

    /*! Margin needed for x1 tics and tic labels. */
  int X1TicsMarg;
    /*! Margin needed for y1 tics and tic labels. */
  int Y1TicsMarg;
    /*! Margin needed for x2 tics and tic labels. */
  int X2TicsMarg;
    /*! Margin needed for y2 tics and tic labels. */
  int Y2TicsMarg;

    /*! Returns tics increment for about \a val distance
        and minimum distance \a min. */
  double autoTics( double val, double min=0.0 );
    /*! Returns tics start for minimum value \a min of the axis 
        and tics increment \a incr. */
  double ticsStart( double min, double incr );

    /*! Draw border: 1=bottom, 2=left, 4=top, 8=right */
  int Border;
    /*! Line-style of the border lines. */
  LineStyle BorderStyle;
    /*! Color of the plot background. */
  int PlotColor;
    /*! Color of the screen background. */
  int BackgroundColor;

    /*! Draw grids? */
  bool XGrid[MaxAxis], YGrid[MaxAxis];
    /*! Line-style of the grids. */
  LineStyle XGridStyle[MaxAxis], YGridStyle[MaxAxis];

    /*! A text label. */
  struct Label : public Position
  {
      /*! Constructor. */
    Label( void );

      /*! The label text. */
    string Text;
      /*! Justification of the label. */
    Justification Just;
      /*! Rotate label by \a Angle degrees. */
    double Angle;
      /*! Color of the label text. */
    int LColor;
      /*! Font of the label. */
    Fonts LFont;
      /*! Size of the label font. */
    double LSize;
      /*! Color of the label background. */
    int BColor;
      /*! Color of a frame. */
    int FColor;
      /*! Width of the frame. */
    int FWidth;
  };

    /*! x-axis labels. */
  Label XLabel[MaxAxis];
    /*! y-axis labels. */
  Label YLabel[MaxAxis];
    /*! Title for the plot. */
  Label Title;
    /*! Default properties for a label. */
  Label DefaultLabel;
    /*! labels. */
  deque<Label> Labels;

  void drawLabel( QPainter &paint, const Label &label );

    /*! The list of predefined colors. */
  deque< RGBColor > Colors;

    /*! Maps the dash-style into the appropriate Qt-PenStyle. */
  map< Dash, Qt::PenStyle, less<int> > QtDash;

  class PointElement : public DataElement
  {
  public:
    PointElement( double x, Coordinates xcoor, double y, Coordinates ycoor,
		  double size, Coordinates sizecoor );
    virtual ~PointElement( void ) {};

    virtual long first( double x1, double y1, double x2, double y2 ) const { return 0; };
    virtual long last( double x1, double y1, double x2, double y2 ) const { return 1; };
    virtual void point( long index, double &x, double &y ) const;
    virtual void xminmax( double &xmin, double &xmax, double ymin, double ymax ) const { xmin=AutoScale; xmax=AutoScale; };
    virtual void yminmax( double xmin, double xmax, double &ymin, double &ymax ) const { ymin=AutoScale; ymax=AutoScale; };
    virtual void setRange( double xmin[MaxAxis], double xmax[MaxAxis], double ymin[MaxAxis], double ymax[MaxAxis],
			   int xpmin, int xpmax, int ypmin, int ypmax );
    void getPos( const Position &pos, double &x, double &y ) const;
    
  protected:
    Position P;
    double X;
    double Y;
    double Size;
    Coordinates SizeCoor;
  };


  class LineElement : public DataElement
  {
  public:
    LineElement( double x1, Coordinates x1coor, double y1, Coordinates y1coor, 
		 double x2, Coordinates x2coor, double y2, Coordinates y2coor );
    virtual ~LineElement( void ) {};

    virtual long first( double x1, double y1, double x2, double y2 ) const { return 0; };
    virtual long last( double x1, double y1, double x2, double y2 ) const { return 2; };
    virtual void point( long index, double &x, double &y ) const;
    virtual void xminmax( double &xmin, double &xmax, double ymin, double ymax ) const { xmin=AutoScale; xmax=AutoScale; };
    virtual void yminmax( double xmin, double xmax, double &ymin, double &ymax ) const { ymin=AutoScale; ymax=AutoScale; };
    virtual void setRange( double xmin[MaxAxis], double xmax[MaxAxis], double ymin[MaxAxis], double ymax[MaxAxis],
		   int xpmin, int xpmax, int ypmin, int ypmax );
    void getPos( const Position &pos, double &x, double &y ) const;

  protected:
    Position P1;
    Position P2;
    double XMin, XMax;
    double YMin, YMax;
  };


  template< typename T, typename R >
  class VectorElement : public DataElement
  {
  public:
    VectorElement( const T &x, const R &y, double xscale, bool copy );
    virtual ~VectorElement( void );

    virtual long first( double x1, double y1, double x2, double y2 ) const { return 0; };
    virtual long last( double x1, double y1, double x2, double y2 ) const { return XData->size() < YData->size() ? XData->size() : YData->size(); };
    virtual void point( long index, double &x, double &y ) const { x = (*XData)[ index ]*XScale; y = (*YData)[ index ]; };
    virtual void xminmax( double &xmin, double &xmax, double ymin, double ymax ) const;
    virtual void yminmax( double xmin, double xmax, double &ymin, double &ymax ) const;

  protected:
    const T *XData;
    const R *YData;
    double XScale;
  };


  template< typename T >
  class SampleDataElement : public DataElement
  {
  public:
    SampleDataElement( const SampleData<T> &data, double xscale, bool copy );
    ~SampleDataElement( void );

    virtual long first( double x1, double y1, double x2, double y2 ) const;
    virtual long last( double x1, double y1, double x2, double y2 ) const;
    virtual void point( long index, double &x, double &y ) const;
    virtual void xminmax( double &xmin, double &xmax, double ymin, double ymax ) const;
    virtual void yminmax( double xmin, double xmax, double &ymin, double &ymax ) const;

  protected:
    const SampleData<T> *SD;
    double XScale;
  };


  template< typename T >
  class EventsElement : public DataElement
  {
  public:
    EventsElement( void );
    EventsElement( const T &x, double tscale,
		   double y, Coordinates ycoor, 
		   double size, Coordinates sizecoor,
		   bool copy );
    virtual ~EventsElement( void );

    virtual long first( double x1, double y1, double x2, double y2 ) const;
    virtual long last( double x1, double y1, double x2, double y2 ) const;
    virtual void point( long index, double &x, double &y ) const;
    virtual void xminmax( double &xmin, double &xmax, double ymin, double ymax ) const;
    virtual void yminmax( double xmin, double xmax, double &ymin, double &ymax ) const { ymin=AutoScale; ymax=AutoScale; };
    virtual void setRange( double xmin[MaxAxis], double xmax[MaxAxis], double ymin[MaxAxis], double ymax[MaxAxis],
			   int xpmin, int xpmax, int ypmin, int ypmax );

  protected:
    const T *ED;
    double TScale;
    double YPos;
    Coordinates YCoor;
    double Y;
    double Size;
    Coordinates SizeCoor;
  };


  class EventDataElement : public EventsElement< EventData >
  {
  public:
    EventDataElement( const EventData &data, int origin, double offset, double tscale, 
		      double y, Coordinates ycoor, double size, Coordinates sizecoor,
		      bool copy );
    virtual ~EventDataElement( void );

    virtual long first( double x1, double y1, double x2, double y2 ) const;
    virtual long last( double x1, double y1, double x2, double y2 ) const;
    virtual void point( long index, double &x, double &y ) const;
    virtual bool init( void );
    virtual void xminmax( double &xmin, double &xmax, double ymin, double ymax ) const;

  protected:
    int Origin;
    double Offset;
    double Reference;
  };


#ifdef HAVE_LIBRELACSDAQ

  class InDataElement : public DataElement
  {
  public:
    InDataElement( const InData &data, int origin, double offset, double tscale, bool copy );
    ~InDataElement( void );

    virtual long first( double x1, double y1, double x2, double y2 ) const;
    virtual long last( double x1, double y1, double x2, double y2 ) const;
    virtual void point( long index, double &x, double &y ) const;
    virtual bool init( void );
    virtual void xminmax( double &xmin, double &xmax, double ymin, double ymax ) const;
    virtual void yminmax( double xmin, double xmax, double &ymin, double &ymax ) const;

  protected:
    const InData *ID;
    int Origin;
    double Offset;
    double TScale;
    double Reference;
  };


  class OutDataElement : public SampleDataElement< float >
  {
  public:
    OutDataElement( const OutData &data, double xscale, bool copy );
  };


  class EventInDataElement : public EventDataElement
  {
  public:
    EventInDataElement( const EventData &events, const InData &data, int origin, double offset, double tscale, 
			   double size, Coordinates sizecoor,
			   bool copy );
    ~EventInDataElement( void );

    long first( double x1, double y1, double x2, double y2 ) const;
    long last( double x1, double y1, double x2, double y2 ) const;
    void point( long index, double &x, double &y ) const;
    void xminmax( double &xmin, double &xmax, double ymin, double ymax ) const;

  protected:
    const InData *ID;
  };

#endif

  class SampleSurfaceElement : public SurfaceElement
  {
  public:
    SampleSurfaceElement( const SampleData<SampleDataD> &data, double xscale, bool copy );
    ~SampleSurfaceElement( void );

    virtual long firstX( double x1, double x2 ) const;
    virtual long lastX( double x1, double x2 ) const;
    virtual long firstY( double y1, double y2 ) const;
    virtual long lastY( double y1, double y2 ) const;
    virtual void point( long rindex, long cindex,
			double &x1, double &y1, double &x2, double &y2,
			double &z ) const;
    virtual void xminmax( double &xmin, double &xmax, double ymin, double ymax ) const;
    virtual void yminmax( double xmin, double xmax, double &ymin, double &ymax ) const;

  protected:
    const SampleData<SampleDataD> *SD;
    double XScale;
  };


  SurfaceElement* SData;
  uchar* SurfaceData;
#ifdef HAVE_LIBRELACSSHAPES
  Transform Projection;
  Point ViewPoint;
  Point LightSource;
  double Contrast;
  typedef deque<PolygonElement*> PolygonDataType;
  PolygonDataType PolygonData;
  int MaxPolygonId;
#endif
  typedef deque<DataElement*> LineDataType;
  LineDataType LineData;
  bool DrawData;
  bool NewData;
  bool ShiftData;
  int ShiftXPix;
  double ShiftX[MaxAxis];
  mutable QMutex PMutex;
  QMutex *DMutex;
  QReadWriteLock *DRWMutex;
  QThread *GUIThread;

  int addData( DataElement *d );
  int setSurface( SurfaceElement *s );
  void drawSurface( QPainter &paint );
#ifdef HAVE_LIBRELACSSHAPES
  void drawPolygon( QPainter &paint, PolygonElement *d );
#endif
  void drawLine( QPainter &paint, DataElement *d, int addpx );
  int drawPoints( QPainter &paint, DataElement *d );

};


template< typename T, typename R >
Plot::VectorElement<T,R>::VectorElement( const T &x, const R &y,
					 double xscale, bool copy )
  : DataElement( Map )
{
  Own = copy;
  if ( copy ) {
    XData = new T( x );
    YData = new R( y );
  }
  else {
    XData = &x;
    YData = &y;
  }
  XScale = xscale;
}


template< typename T, typename R >
Plot::VectorElement<T,R>::~VectorElement( void )
{
  if ( Own ) {
    delete XData;
    delete YData;
  }
}


template< typename T, typename R >
void Plot::VectorElement<T,R>::xminmax( double &xmin, double &xmax, 
					double ymin, double ymax ) const
{
  bool autoymin = ( ymin == -MAXDOUBLE );
  bool autoymax = ( ymax == MAXDOUBLE );

  int k;
  for ( k=0; k<(int)XData->size() && k<(int)YData->size(); k++ ) {
    double y = YData->operator[]( k );
    if ( finite( XData->operator[]( k ) ) && finite( y ) &&
	 ( autoymin || y >= ymin ) && 
	 ( autoymax || y <= ymax ) )
      break;
  }
  if ( k<(int)XData->size() ) {
    xmin = XData->operator[]( k );
    xmax = xmin;
    for ( k++; k<(int)XData->size() && k<(int)YData->size(); k++ ) {
      double x = XData->operator[]( k );
      double y = YData->operator[]( k );
      if ( finite( x ) && finite( y ) &&
	   ( autoymin || y >= ymin ) && 
	   ( autoymax || y <= ymax ) ) {
	if ( x < xmin )
	  xmin = x;
	else if ( x > xmax )
	  xmax = x;
      }      
    }
    xmin *= XScale;
    xmax *= XScale;
  }
  else {
    xmin = AutoScale;
    xmax = AutoScale;
  }
}


template< typename T, typename R >
void Plot::VectorElement<T,R>::yminmax( double xmin, double xmax, 
					double &ymin, double &ymax ) const
{
  bool autoxmin = ( xmin == -MAXDOUBLE );
  bool autoxmax = ( xmax == MAXDOUBLE );

  xmin /= XScale;
  xmax /= XScale;

  int k;
  for ( k=0; k<(int)XData->size() &&  k<(int)YData->size(); k++ ) {
    double x = XData->operator[]( k );
    if ( finite( x ) && finite( YData->operator[]( k ) ) &&
	 ( autoxmin || x >= xmin ) &&
	 ( autoxmax || x <= xmax ) )
      break;
  }
  if ( k<(int)YData->size() ) {
    ymin = YData->operator[]( k );
    ymax = ymin;
    for ( k++; k<(int)XData->size() && k<(int)YData->size(); k++ ) {
      double x = XData->operator[]( k );
      double y = YData->operator[]( k );
      if ( finite( x ) && finite( y ) &&
	   ( autoxmin || x >= xmin ) && 
	   ( autoxmax || x <= xmax ) ) {
	if ( y < ymin )
	  ymin = y;
	else if ( y > ymax )
	  ymax = y;
      }
    }
  }
  else {
    ymin = AutoScale;
    ymax = AutoScale;
  }
}


template< typename T, typename R >
int Plot::plot( const T &x, const R &y, 
		const LineStyle &line, const PointStyle &point )
{
  VectorElement<T,R> *DE = new VectorElement<T,R>( x, y, 1.0, Keep == Copy );
  DE->setStyle( line, point );
  return addData( DE );
}


template< typename T, typename R >
int Plot::plot( const T &x, const R &y, double xscale, 
		const LineStyle &line, const PointStyle &point )
{
  VectorElement<T,R> *DE = new VectorElement<T,R>( x, y,
						   xscale, Keep == Copy );
  DE->setStyle( line, point );
  return addData( DE );
}


template< typename T >
int Plot::plot( const Map<T> &m, double xscale, 
		const LineStyle &line, const PointStyle &point )
{
  VectorElement< Array<T>, Array<T> > *DE = 
    new VectorElement< Array<T>, Array<T> >( m.x(), m.y(),
					     xscale, Keep == Copy );
  DE->setStyle( line, point );
  return addData( DE );
}


template< typename T >
Plot::SampleDataElement<T>::SampleDataElement( const SampleData<T> &data, 
					       double xscale, 
					       bool copy )
  : DataElement( Map )
{
  Own = copy;
  if ( copy ) {
    SD = new SampleData<T>( data );
  }
  else {
    SD = &data;
  }
  XScale = xscale;
}


template< typename T >
Plot::SampleDataElement<T>::~SampleDataElement<T>( void )
{
  if ( Own ) {
    delete SD;
  }
}


template< typename T >
long Plot::SampleDataElement<T>::first( double x1, double y1, double x2, double y2 ) const
{
  long i = long( ::floor( (x1/XScale - SD->offset())/SD->stepsize() ) );
  if ( i<0 )
    i=0;
  else if ( i > SD->size() )
    i = SD->size();
  return i;
}


template< typename T >
long Plot::SampleDataElement<T>::last( double x1, double y1, double x2, double y2 ) const
{
  long i = long( ::ceil( (x2/XScale - SD->offset())/SD->stepsize() ) ) + 1;
  //  long i = SD->index( x2/XScale )+1;
  if ( i > SD->size() )
    i = SD->size();
  return i;
}


template< typename T >
void Plot::SampleDataElement<T>::point( long index, double &x, double &y ) const
{
  x = XScale*SD->pos( index );
  y = (*SD)[int(index)];
}


template< typename T >
void Plot::SampleDataElement<T>::xminmax( double &xmin, double &xmax, 
					  double ymin, double ymax ) const
{
  xmin = SD->rangeFront()*XScale;
  xmax = SD->rangeBack()*XScale;
}


template< typename T >
void Plot::SampleDataElement<T>::yminmax( double xmin, double xmax, 
					  double &ymin, double &ymax ) const
{
  int x1i = xmin == -MAXDOUBLE ? 0 : SD->index( xmin/XScale );
  int x2i = xmax == MAXDOUBLE ? SD->size()-1 : SD->index( xmax/XScale );

  if ( x1i < 0 )
    x1i = 0;
  if ( x2i >= SD->size() )
    x2i = SD->size()-1;

  if ( x2i >= x1i ) {
    while ( x1i <= x2i && ! finite( (*SD)[ x1i ] ) )
      x1i++;
    ymin = ymax = (*SD)[ x1i ];
    for ( int k=x1i+1; k<=x2i; k++ ) {
      if ( finite( (*SD)[k] ) ) {
	if ( (*SD)[k] > ymax )
	  ymax = (*SD)[k];
	else if ( (*SD)[k] < ymin )
	  ymin = (*SD)[k];
      }
    }
  }
  else {
    ymin = AnyScale;
    ymax = AnyScale;
  }
}


template< typename T >
int Plot::plot( const SampleData<T> &data, double xscale, 
		const LineStyle &line, const PointStyle &point )
{
  SampleDataElement<T> *DE = new SampleDataElement<T>( data, xscale, Keep == Copy );
  DE->setStyle( line, point );
  return addData( DE );
}


template< typename T >
Plot::EventsElement<T>::EventsElement( const T &x, double tscale,
				       double y, Coordinates ycoor, 
				       double size, Coordinates sizecoor,
				       bool copy )
  : DataElement( Events )
{
  Own = copy;
  if ( copy ) {
    ED = new T( x );
  }
  else {
    ED = &x;
  }
  TScale = tscale;
  YPos = y;
  YCoor = ycoor;
  Y = y;
  Size = size;
  SizeCoor = sizecoor;
}


template< typename T >
Plot::EventsElement<T>::~EventsElement( void )
{
  if ( Own ) {
    delete ED;
  }
}


template< typename T >
long Plot::EventsElement<T>::first( double x1, double y1, double x2, double y2 ) const
{
  int l = 0;
  int r = int(ED->size())-1;

  // there is no next event?
  if ( r < l || (*ED)[r] * TScale < x1 )
    return ED->size();
  // first event is already next event?
  else if ( (*ED)[l] * TScale >= x1 )
    return l;

  // bisect:
  while ( r-l > 1 ) {
    int h = (l+r)/2;
    if ( (*ED)[h] * TScale < x1 )
      l = h;
    else
      r = h;
  }
  return r;
}


template< typename T >
long Plot::EventsElement<T>::last( double x1, double y1, double x2, double y2 ) const
{
  int l = 0;
  int r = int(ED->size())-1;

  // there is no next event?
  if ( r < l || (*ED)[r] * TScale < x2 )
    return ED->size();
  // first event is already next event?
  else if ( (*ED)[l] * TScale >= x2 )
    return l;

  // bisect:
  while ( r-l > 1 ) {
    int h = (l+r)/2;
    if ( (*ED)[h] * TScale < x2 )
      l = h;
    else
      r = h;
  }
  return r;
}


template< typename T >
void Plot::EventsElement<T>::point( long index, double &x, double &y ) const
{
  x = (*ED)[index] * TScale;
  y = Y;
}


template< typename T >
void Plot::EventsElement<T>::xminmax( double &xmin, double &xmax, 
				      double ymin, double ymax ) const
{
  if ( ED->empty() ) {
    xmin = AutoScale;
    xmax = AutoScale;
  }
  else {
    xmin = ED->front() * TScale;
    xmax = ED->back() * TScale;
  }
}


template< typename T >
void Plot::EventsElement<T>::setRange( double xmin[MaxAxis], double xmax[MaxAxis], 
				       double ymin[MaxAxis], double ymax[MaxAxis],
				       int xpmin, int xpmax, 
				       int ypmin, int ypmax )
{
  if ( YCoor == Plot::Graph )
    Y = YPos*( ymax[YAxis] - ymin[YAxis] ) + ymin[YAxis];
  else
    Y = YPos;

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


template< typename T >
int Plot::plot( const T &x, double tscale, 
		double y, Coordinates ycoor, int lwidth,
		Points ptype, double size, Coordinates sizecoor,
		int pcolor, int pfill )
{
  EventsElement<T> *DE = new EventsElement<T>( x, tscale, y, ycoor, size, sizecoor, Keep == Copy );
  DE->setStyle( Transparent, lwidth, Solid, ptype, int(size), pcolor, pfill );
  return addData( DE );
}


}; /* namespace relacs */

#endif /* ! _RELACS_PLOT_H_ */
