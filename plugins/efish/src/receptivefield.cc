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

#include <cmath>
#include <relacs/relacswidget.h>
#include <relacs/efish/receptivefield.h>

#define PI 3.14159265

using namespace relacs;

namespace efish {


ReceptiveField::ReceptiveField( void )
  : RePro( "ReceptiveField", "efish", "Jan Grewe", "1.0", "Dec 12, 2017" )
{
  newSection( "Quick Search" );
  addNumber( "xmin", "Minimum x position", 0.0, -1000., 1000., 0.1, "mm" );
  addNumber( "xmax", "Maximum x position", 0.0, -1000., 1000., 0.1, "mm" );
  addNumber( "xstep", "Desired step size of the x-grid", 1.0, 0.1, 1000., 0.1, "mm" );

  addNumber( "ymin", "Minimum y position", 0.0, -1000., 1000., 0.1, "mm" );
  addNumber( "ymax", "Maximum y position", 0.0, -1000., 1000., 0.1, "mm" );
  addNumber( "ystep", "Desired step size of the y-grid", 1.0, 0.1, 1000., 0.1, "mm" );
  addNumber( "ystart", "Starting y-position for the xrange search", 0.0, -1000., 1000., 0.1, "mm");

  addNumber( "zmin", "Minimum z position", 0.0, -1000., 1000., 0.1, "mm" );
  addNumber( "zmax", "Maximum z position", 0.0, -1000., 1000., 0.1, "mm" );
  addNumber( "zstep", "Desired step size of the z-grid", 1.0, 0.1, 1000., 0.1, "mm" );

  //addNumber( "zposition", "Z-position, i.e. distance to fish, for the xrange search", 0.0, -1000., 1000., 0.1, "mm");
  addBoolean( "followmidline", "Auto-adjust y to follow the fish midline.", true);

  newSection( "Stimulation" );
  addNumber( "deltaf", "Difference frequency of the local stimulus relative to the fish EODf.",
             50.0, 0.0, 1000., 1., "Hz" );
  addNumber( "amplitude", "Amplitude of the local stimulus used for finding the receptive field.",
             0.001, 0.0, 10., .0001, "V", "mV" );
  addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" );
  addNumber( "pause", "Pause between stimulus presentations", 1.0, 0.001, 1000.0, 0.001, "s", "ms" );
  addNumber( "repeats", "Number of stimulus repeats at each position", 5.0, 1., 100., 1., "");
  addNumber( "binwidth", "Bin-width used to estimate the firing rate.", 0.01, 0.001, 1.0, 0.001, "s", "ms" );
  
  newSection( "Robot setup" );
  addSelection( "xmapping", "Mapping of x-axis to robot axis", "y|z|x" );
  addBoolean( "xinvert", "Select to map 0 position in relacs to max position of the robot.", true);
  addSelection( "ymapping", "Mapping of y-axis to robot axis", "z|x|y");
  addBoolean( "yinvert", "Select to map 0 position in relacs to max position of the robot.", false);
  addSelection( "zmapping", "Mapping of z-axis to robot axis", "x|y|z");
  addBoolean( "zinvert", "Select to map 0 position in relacs to max position of the robot.", false);

  addNumber( "safex", "Safe x position (robot coords) into which the robot is reset", 350.0, 0.0, 600.,  1.0, "mm" );
  addNumber( "safey", "Safe y position (robot coords) into which the robot is reset", 0.0, 0.0, 450.,  1.0, "mm" );
  addNumber( "safez", "Safe z position (robot coords) into which the robot is reset", 0.0, 0.0, 400.,  1.0, "mm" );
  
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


bool ReceptiveField::moveToPosition( double x, double y, double z) {
  Point destination( fish_head );
  Point temp_dest( x, y, z );
  for (size_t i = 0; i < axis_map.size(); i++ ) {
    destination[axis_map[i]] +=  axis_invert[i] * temp_dest[i];
  }
  if ( robot->stopped() )
    return false;
  //robot->powerAxes( true );
  //sleep( 0.75 );
  robot->PF_up_and_over( destination );
  robot->wait();
  sleep( 0.1 );
  robot->powerAxes( false );
  return true;
}


double ReceptiveField::getYSlope( void ) {
  double slope = (fish_tail[axis_map[1]] - fish_head[axis_map[1]]) /
    (fish_tail[axis_map[0]] - fish_head[axis_map[0]]) * axis_invert[axis_map[1]];
  return slope;
}


bool ReceptiveField::rangeSearch( LinearRange &range, double xy_pos, double z_pos,
                                  std::vector<double> &avg_rates, OutData &signal,
				  bool x_search, bool adjust_y, bool simulation ) {
  std::vector<double> rates(repeats);
  EventList spike_trains;
  double period = 1./deltaf;
  double best_y = 30.;
  double best_x = 60.;

  double y_slope = 0.0;
  if ( adjust_y ) {
    y_slope = getYSlope();
  }
  if ((int)avg_rates.size() != range.size()) {
    avg_rates.resize(range.size());
  }
  for(int i =0; i < range.size(); i ++) {
    double x = x_search ? range[i] : xy_pos;
    double y = x_search ? xy_pos : range[i];
    double y_corrector = y_slope * x;
    plotRate(posPlot, x, y);
    // go, robi, go
    if ( !moveToPosition(x, y, z_pos) )
      return false;
    if ( adjust_y ) {
      if ( !moveToPosition(x, y + y_corrector, z_pos) )
        return false;
    }
    // do the measurement
    spike_trains.clear();
    for ( int j = 0; j < repeats; j++ ) {
      int trial = 0;
      if ( presentStimulus( x, y, z_pos, j, signal ) != 0 ) {
        return false;
      }
      getSpikes( spike_trains );
      SampleDataD rate( (int)(period/this->binwidth), 0.0, this->binwidth );
      getRate( rate, spike_trains.back(), trial, period, duration );
      if ( simulation ) {
	double best = x_search ? best_x : best_y;
	double tuning_width = (range[range.size()-1] - range[0]);
	double shift = 2* PI * best / tuning_width;
	double modulator = std::cos(range[i] * PI * 1./tuning_width + shift ) + 1.;
	rates[j] = modulator * rate.stdev( rate.rangeFront(), rate.rangeBack() );
      } else {
	rates[j] = rate.stdev( rate.rangeFront(), rate.rangeBack() );
      }
      double x_val = x_search ? range[i] : rates[j];
      double y_val = x_search ? rates[j] : range[i];
      plotRate( x_search ? xPlot : yPlot, x_val, y_val );
    }
    double avg = 0.0;
    for(double r : rates)
      avg += (r/repeats);
    plotAvgRate( x_search ? xPlot : yPlot, x_search ? range[i] : avg, x_search ? avg : range[i]);
    avg_rates[i] = avg;
  }
  return true;
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
    return Failed;
  }

  sleep( this->pause );
  if ( interrupt() ) {
    return Aborted;
  }
  return 0;
}


void ReceptiveField::getRate( SampleDataD &rate, const EventData &spike_train, int &start_trial,
              double period, double duration ) {
  for ( int j = 0; j < (int)(duration/period); j++ ) {
    spike_train.addRate( rate, start_trial, rate.stepsize(), j * period );
  }
}


void ReceptiveField::getSpikes( EventList & spike_trains ) {
  for ( int k=0; k<MaxTraces; k++ ) {
    if ( SpikeEvents[k] >= 0 ) {
      spike_trains.push( events( SpikeEvents[k] ), signalTime(), signalTime() + duration );
      break;
    }
  }
}


void ReceptiveField::analyze( const EventList &spike_trains ) {
  SampleDataD avgRate();
  for ( int i=0; i<spike_trains.size(); i++ ) {
    
  }
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


void ReceptiveField::prepareStimulus( OutData &signal ) {
  double eodf = events( LocalEODEvents[0] ).frequency( currentTime() - 0.5, currentTime() );
  signal.setTrace( LocalEField[0] ); //FIXME: will need to be switched to LocalEField[0]
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


int map_axis( const string &s ){
  vector<string> axes = {"x", "y", "z"};
  for ( int i = 0; i < (int)axes.size(); i++ ) {
    if ( axes[i].compare(s) == 0 )
      return i;
  }
  return -1;
}


int ReceptiveField::main( void )
{
  // grid settings
  double xmin = number( "xmin" );
  double xmax = number( "xmax" );
  double ymin = number( "ymin" );
  double ymax = number( "ymax" );
  double zmin = number( "zmin" );
  double zmax = number( "zmax" );

  double xstep = number( "xstep" );
  double ystep = number( "ystep" );
  double zstep = number( "zstep" );

  double ystart = number( "ystart" );
  double z_pos = number( "zstart" );

  double safex = number( "safex" );
  double safey = number( "safey" );
  double safez = number( "safez" );
  bool adjust_y = boolean( "followmidline" );

  // Stimulus settings
  this->amplitude = number( "amplitude" );
  this->repeats = number( "repeats" );
  this->duration = number( "duration" );
  this->deltaf = number( "deltaf" );
  this->pause = number( "pause" );
  this->binwidth = number( "binwidth" );

  robot = dynamic_cast<misc::XYZRobot*>( device( "robot-2" ) );
  if ( robot == 0 ) {
    warning( "No Robot! please add 'XYZRobot' to the controlplugins int he config file." );
    return Failed;
  }
  if ( !robot->isOpen() )
    robot->start_mirob();
  if ( robot->stopped() ) {
    warning( "Robot can not move or desired point is forbidden!" );
    return Failed;
  }
  if ( !robot->PF_up_and_over( {safex, safey, safez } ) ) {
    warning( "Robot can not move or desired point is forbidden!" );
    return Failed;
  }
  robot->wait();
  if ( interrupt() ) {
    robot->PF_up_and_over( {safex, safey, safez } );
    robot->wait();
    // robot->PF_up_and_over( {0., 0., 0. } );
    // robot->wait();
    //    robot->close_mirob();
    return Aborted;
  }

  fish_head = robot->get_fish_head();
  fish_tail = robot->get_fish_tail();

  // axis setup
  axis_map = {0, 1, 2};
  axis_invert = {1, 1, 1};
  axis_map[0] = map_axis( text("xmapping") );
  axis_map[1] = map_axis( text("ymapping") );
  axis_map[2] = map_axis( text("zmapping") );
  axis_invert[0] = boolean("xinvert") ? -1 : 1;
  axis_invert[1] = boolean("yinvert") ? -1 : 1;
  axis_invert[2] = boolean("zinvert") ? -1 : 1;

  OutData signal;
  prepareStimulus( signal );

  resetPlots( xmin, xmax, ymin, ymax );

  LinearRange xrange( xmin, xmax, xstep );
  LinearRange yrange( ymin, ymax, ystep );
  LinearRange zrange( zmin, zmax, zstep );

  std::vector<double> avg_rates_x( xrange.size() );
  std::vector<double> avg_rates_y( yrange.size() );

  moveToPosition( xmin, ystart, z_pos );
  // robot->PF_up_and_over( fish_head );
  // robot->wait();
  for(int i = 0; i < zrange.size(); i ++) {
    z_pos = zrange[i];
    for (int j = 0; j < yrange.size(); j++) {
      ystart = yrange[j];
      if (!rangeSearch( xrange, ystart, z_pos, avg_rates_x, signal, true , adjust_y, false )) {
	robot->PF_up_and_over( {safex, safey, safez } );
	robot->wait();
	return Failed;
      }
    }
  }
  /*
  double bestX, bestY;
  if ( bestXPos( avg_rates_x, xrange, bestX ) ) {
    plotBestX( xPlot, bestX );
    if (!rangeSearch( yrange, bestX, z_pos, avg_rates_y, signal, false, adjust_y, false ) ) {
      robot->PF_up_and_over( {safex, safey, safez } );
      robot->wait();
      return Failed;
    }
    if (bestXPos( avg_rates_y, yrange, bestY )) {
      plotBestY( yPlot, bestY );
    }
  }
  metaData().setNumber("Cell properties>X Position", bestX);
  metaData().setNumber("Cell properties>Y Position", bestY);
  double y_correction = 0.0;
  if ( adjust_y ) {
    double slope = getYSlope();
    y_correction = slope * bestX;
  }
  moveToPosition( bestX, bestY + y_correction, z_pos );
  cerr << "Best position x: " << bestX << "\t y:" << (bestY + y_correction) << "\t z:" << z_pos << endl;
  // TODO record a second of baseline at this point
  // sleep( 2.0 );
  // robot->PF_up_and_over( { safex, safey, safez } );
  // robot->wait();
  // robot->PF_up_and_over( { 0., 0., 0. } );
  // robot->wait();
  // robot->close_mirob();
  */
  return Completed;
}


addRePro( ReceptiveField, efish );

}; /* namespace efish */

#include "moc_receptivefield.cc"
