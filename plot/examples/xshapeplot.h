#ifndef _XSHAPEPLOT_H_
#define _XSHAPEPLOT_H_


#include <cmath>
#include <QObject>
#include <QTimer>
#include <relacs/shape.h>
#include <relacs/multiplot.h>
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
  double X;

};

#endif
