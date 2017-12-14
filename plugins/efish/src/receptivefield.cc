/*
  efish/receptivefield.cc
  Locates the receptive field of a p-unit electrosensory afferent using the mirob roboter.

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

#include <relacs/efish/receptivefield.h>
#include <relacs/relacswidget.h>
#include <cmath>

using namespace relacs;

namespace efish {


ReceptiveField::ReceptiveField( void )
  : RePro( "ReceptiveField", "efish", "Jan Grewe", "1.0", "Dec 12, 2017" )
{
  newSection( "Quick Search" );
  addNumber( "xmin", "Minimum x position", 0.0, -1000., 1000., 0.1, "mm" );
  addNumber( "xmax", "Maximum x position", 0.0, -1000., 1000., 0.1, "mm" );
  addNumber( "xstep", "Desired step size of the x-grid ", 1.0, 0.1, 1000., 0.1, "mm" );

  addNumber( "ymin", "Minimum y position", 0.0, -1000., 1000., 0.1, "mm" );
  addNumber( "ymax", "Maximum y position", 0.0, -1000., 1000., 0.1, "mm" );
  addNumber( "ystep", "Desired step size of the y-grid ", 1.0, 0.1, 1000., 0.1, "mm" );
  addNumber( "ystart", "Starting y-position for the xrange search", 0.0, -1000., 1000., 0.1, "mm");

  newSection( "Stimulation" );
  addNumber( "deltaf", "Difference frequency of the local stimulus relative to the fish EODf.",
             50.0, 0.0, 1000., 1., "Hz" );
  addNumber( "amplitude", "Amplitude of the local stimulus used for finding the receptive field.",
             0.001, 0.0, 10., .0001, "V", "mV" );
  addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "pause", "Pause between stimulus presentations", 1.0, 0.001, 1000.0, 0.001, "s", "ms" );
  addNumber( "repeats", "Number of stimulus repeats at each position", 5.0, 1., 100., 1., "");

  newSection( "Axis mapping" );
  addSelection( "xmapping", "Mapping of x-axis to robot axis", "x|y|z" );
  addBoolean( "xinvert", "Select to map 0 position in relacs to max position of the robot.", true);
  addSelection( "ymapping", "Mapping of y-axis to robot axis", "z|x|y");
  addBoolean( "yinvert", "Select to map 0 position in relacs to max position of the robot.", false);
  
  QVBoxLayout *vb = new QVBoxLayout;
  QHBoxLayout *hb = new QHBoxLayout;

  posPlot.lock();
  posPlot.setXLabel( "x-Position [mm]" );
  posPlot.setYRange( 0., 250. );
  posPlot.setYLabel( "y-Position [mm]" );
  posPlot.setLMarg( 6 );
  posPlot.setRMarg( 1 );
  posPlot.setTMarg( 3 );
  posPlot.setBMarg( 4 );
  posPlot.unlock();

  xPlot.lock();
  xPlot.setXLabel( "x-Position [mm]" );
  xPlot.setAutoScaleY();
  xPlot.setYLabel( "Firing rate [Hz]" );
  xPlot.setLMarg( 6 );
  xPlot.setRMarg( 1 );
  xPlot.setTMarg( 3 );
  xPlot.setBMarg( 4 );
  xPlot.unlock();

  yPlot.lock();
  yPlot.setYLabel( "y-Position [mm]" );
  yPlot.setXRange( Plot::AutoScale, Plot::AutoScale );
  yPlot.setXLabel( "Firing rate [Hz]" );
  yPlot.setLMarg( 6 );
  yPlot.setRMarg( 1 );
  yPlot.setTMarg( 3 );
  yPlot.setBMarg( 4 );
  yPlot.unlock();

  vb->addWidget( &posPlot );
  hb->addWidget( &xPlot );
  hb->addWidget( &yPlot );
  vb->addLayout( hb );
  setLayout( vb );
}


void ReceptiveField::resetPlots( double xmin, double xmax, double ymin, double ymax )
{
  xPlot.lock();
  xPlot.clear();
  xPlot.setXRange( xmin, xmax );
  xPlot.setYRange(0., 250.);
  //xPlot.setAutoScaleY();
  xPlot.draw();
  xPlot.unlock();

  yPlot.lock();
  yPlot.clear();
  yPlot.setYRange( ymin, ymax );
  yPlot.setXRange( 0., 250. );
  yPlot.draw();
  yPlot.unlock();

  posPlot.lock();
  posPlot.clear();
  double xrange = xmax - xmin;
  double yrange = ymax - ymin;
  posPlot.setXRange( xmin - 0.1 * xrange, xmax + 0.1 * xrange );
  posPlot.setYRange( ymin - 0.1 * yrange, ymax + 0.1 * yrange );
  posPlot.draw();
  posPlot.unlock();
}


double fRand(double fMin, double fMax)
{
  double f = (double)std::rand() / RAND_MAX;
  return fMin + f * (fMax - fMin);
}

void plotRate(Plot &p, double x, double y) {
  p.lock();
  p.plotPoint(x, Plot::First, y, Plot::First, 0, Plot::Circle, 5, Plot::Pixel,
              Plot::Red, Plot::Red);
  p.draw();
  p.unlock();
}

void plotAvgRate(Plot &p, double x, double y) {
  p.lock();
  p.plotPoint(x, Plot::First, y, Plot::First, 0, Plot::Circle, 5, Plot::Pixel,
              Plot::Yellow);
  p.draw();
  p.unlock();
}

bool bestXPos(std::vector<double> &averages, LinearRange &range, double &bestPos) {
  if ( (int)averages.size() != range.size() )
    return false;
  std::vector<double>::iterator result;
  result = std::max_element(averages.begin(), averages.end());
  size_t pos = std::distance(averages.begin(), result);
  bestPos = range[pos];
  return true;
}

void ReceptiveField::xRangeSearch(LinearRange &range, double y_pos, double z_pos,
                                  std::vector<double> &avg_rates, OutData &signal)
{
  std::vector<double> rates(repeats);
  double best_x = 50.; // just for fun
  if ((int)avg_rates.size() != range.size()) {
    avg_rates.resize(range.size());
  }
  for(int i =0; i < range.size(); i ++) {
    plotRate(posPlot, range[i], y_pos);
    // go, robi, go
    // do the measurement
    sleep(0.1);
    for (int j = 0; j < repeats; j++) {
      presentStimulus( range[i],y_pos, z_pos, j, signal );
      rates[j] = ((range[range.size()-1] - range[0]) - std::abs(range[i] - best_x)) *
        1.5 + fRand(0., 50.);
      plotRate(xPlot, range[i], rates[j]);
    }
    double avg = 0.0;
    for(double r : rates)
      avg += (r/repeats);
    plotAvgRate(xPlot, range[i], avg);
    avg_rates[i] = avg;
  }
}

void ReceptiveField::yRangeSearch(LinearRange &range, double x_pos, double z_pos,
                                  std::vector<double> &avg_rates, OutData &signal)
{
  std::vector<double> rates(repeats);
  double best_y = 20.;
  if ((int)avg_rates.size() != range.size()) {
    avg_rates.resize(range.size());
  }
  for(int i =0; i < range.size(); i ++) {
    plotRate(posPlot, x_pos, range[i]);
    // go, robi, go
    // do the measurement
    sleep( 0.1 );

    for (int j = 0; j < repeats; j++) {
      presentStimulus( x_pos, range[i], z_pos, j, signal );
      rates[j] = ((range[range.size()-1] - range[0]) - std::abs(range[i] - best_y)) *
        1.5 + fRand( 0., 50. );
      plotRate( yPlot, rates[j], range[i] );
    }
    double avg = 0.0;
    for(double r : rates)
      avg += (r/repeats);
    plotAvgRate( yPlot, avg, range[i] );
    avg_rates[i] = avg;
  }
}

int ReceptiveField::presentStimulus(double x_pos, double y_pos, double z_pos, int repeat, OutData &signal) {
  Str m = "x: <b>" + Str( x_pos, 0, 0, 'f' ) + "mm</b>";
  m += "  y: <b>" + Str( y_pos ,0, 0, 'f' ) + "mm</b>";
  m += "  repeat: <b>" + Str( repeat ) + "</b>";
  message( m );
  signal.description().setNumber( "x_pos", x_pos, "mm" );
  signal.description().setNumber( "y_pos", y_pos, "mm" );
  signal.description().setNumber( "z_pos", z_pos, "mm" );

  write( signal );
  if ( !signal.success() ) {
    string w = "Output of stimulus failed!<br>Error code is <b>";
    w += Str( signal.error() ) + "</b>: <b>" + signal.errorText() + "</b>";
    warning( w, 4.0 );
    //save();
    //stop();
    return Failed;
  }

  sleep( this->pause );
  if ( interrupt() ) {
    //save();
    //stop();
    return Aborted;
  }
  return 0;
}

void plotBestX(Plot &p, double x) {
  p.lock();
  p.plotVLine(x, Plot::White, 2, Plot::Solid);
  p.draw();
  p.unlock();
}

void plotBestY(Plot &p, double y) {
  p.lock();
  p.plotHLine(y, Plot::White, 2, Plot::Solid);
  p.draw();
  p.unlock();
}

void ReceptiveField::setupStimulus( OutData &signal ) {
  double eodf = events( LocalEODEvents[0] ).frequency( currentTime() - 0.5, currentTime() );
  signal.setTrace( LocalEField[0] );
  signal.sineWave( this->duration, 0.0, eodf + this->deltaf );
  signal.setIntensity( this->amplitude );

  Options opts;
  Parameter &p1 = opts.addNumber( "dur", this->duration, "s" );
  Parameter &p2 = opts.addNumber( "ampl", this->amplitude, "V" );
  Parameter &p3 = opts.addNumber( "deltaf", this->deltaf, "Hz" );
  Parameter &p4 = opts.addNumber( "freq", eodf + this->deltaf, "Hz" );
  Parameter &p5 = opts.addNumber( "x_pos", 0.0, "mm" );
  Parameter &p6 = opts.addNumber( "y_pos", 0.0, "mm" );
  Parameter &p7 = opts.addNumber( "z_pos", 0.0, "mm" );

  signal.setMutable( p1 );
  signal.setMutable( p2 );
  signal.setMutable( p3 );
  signal.setMutable( p4 );
  signal.setMutable( p5 );
  signal.setMutable( p6 );
  signal.setMutable( p7 );
  signal.setDescription( opts );
}


int ReceptiveField::main( void )
{
  // grid settings
  double xmin = number( "xmin" );
  double xmax = number( "xmax" );
  double ymin = number( "ymin" );
  double ymax = number( "ymax" );
  double xstep = number( "xstep" );
  double ystep = number( "ystep" );
  double ystart = number( "ystart" );
  double z_pos = 0.0;

  // stimulus settings
  this->amplitude = number( "amplitude" );
  this->repeats = number( "repeats" );
  this->duration = number( "duration" );
  this->deltaf = number( "deltaf" );
  this->pause = number( "pause" );

  OutData signal;
  setupStimulus( signal );

  resetPlots( xmin, xmax, ymin, ymax );

  LinearRange xrange( xmin, xmax, xstep );
  LinearRange yrange( ymin, ymax, ystep );
  std::vector<double> avg_rates_x( xrange.size() );
  std::vector<double> avg_rates_y( yrange.size() );

  xRangeSearch( xrange, ystart, z_pos, avg_rates_x, signal );
  double bestX, bestY;
  if ( bestXPos( avg_rates_x, xrange, bestX ) ) {
    plotBestX( xPlot, bestX );
    yRangeSearch( yrange, bestX, z_pos, avg_rates_y, signal );
    if (bestXPos( avg_rates_y, yrange, bestY )) {
      plotBestY( yPlot, bestY );
    }
  }
  metaData().setNumber("Cell properties>x position", bestX);
  metaData().setNumber("Cell properties>y position", bestY);
  return Completed;
}


addRePro( ReceptiveField, efish );

}; /* namespace efish */

#include "moc_receptivefield.cc"
