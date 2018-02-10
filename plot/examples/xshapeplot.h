#ifndef _SHAPEPLOT_H_
#define _SHAPEPLOT_H_


#include <cmath>
#include <QObject>
#include <QTimer>
#include <relacs/shape.h>
#include <relacs/multiplot.h>
#include <xshapeplot.h>
using namespace relacs;


class PlotShapes : public MultiPlot
{
  Q_OBJECT

public:
  PlotShapes( int n );
  ~PlotShapes( void ) {};

public slots:
  void update( void );

private:

  double A, B, C;

};

#endif
