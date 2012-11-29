/*
  efield/robotfield.cc
  Plugin to measure the field geometry using a robot for eletrode positioning.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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


#include <QPen>
#include <relacs/outdata.h>
#include <relacs/efield/robotfield.h>
#include <relacs/tablekey.h>


using namespace relacs;
using namespace misc;
#define WIDTH 300
#define MARGIN 10

namespace efield {


RobotField::RobotField( void )
  : RePro( "RobotField", "efield", "Fabian Sinz/ Jan Benda", "1.0", "Nov 27, 2012" )
{
  // add some options:
  // addNumber( "duration", "Stimulus duration", 1.0, 0.001, 100000.0, 0.001, "s", "ms" )
  addNumber("resolution", 3, "The measured grid will have resolution^2 equidistant points.");
  addNumber("speed", 20, "Speed of the robot.");

  Stack = new QStackedLayout;
  
  //------------------------------------------------------
  Init = new QWidget;
  QVBoxLayout *iniLayout = new QVBoxLayout;

  Set = new QPushButton("Set Landmark");
  connect( Set, SIGNAL( clicked() ),
	   this, SLOT( setLandMark() ) );

  Instructions = new QLabel("-Idle-");
  Instructions->setAlignment( Qt::AlignLeft );

  View = new QGraphicsView;
  GoWhere = new QGraphicsScene(0,0,WIDTH,WIDTH);
  View->setScene(GoWhere);
  pen = new QPen;

  iniLayout->addWidget(Instructions);
  iniLayout->addWidget(View);
  iniLayout->addWidget(Set);

  Init->setLayout(iniLayout);
  Stack->addWidget(Init);

  //------------------------------------------------------
  Exec = new QWidget;
  QVBoxLayout *execLayout = new QVBoxLayout;
  execLayout->addWidget( &P );
  Exec->setLayout(execLayout);
  Stack->addWidget(Exec);

  setLayout(Stack);
}


void RobotField::customEvent( QEvent *qce )
{
  if ( qce->type() == QEvent::User+11 ) {
    Stack->setCurrentWidget( Init );
  }
  else if ( qce->type() == QEvent::User+12 ) {
    Stack->setCurrentWidget( Exec );
  }
  else if ( qce->type() == QEvent::User+13 ) {
    Instructions->setText("Move the robot to the grid point\nrepresented by the red dot and press \"Set Landmark\"");
    double resolution = number("resolution");
    int n = (int)resolution;
    if (LandmarkCounter == -1){
      Dots.clear();
      GoWhere->clear();
      pen->setColor(QColor("#000000"));
      double d = (WIDTH-2*MARGIN)/(resolution-1);
      for (double x = MARGIN; x <= WIDTH-MARGIN; x += d){
	for (double y = MARGIN; y <= WIDTH-MARGIN; y += d){
	  QGraphicsEllipseItem *tmp = new QGraphicsEllipseItem(x, y, 10, 10 );
	  tmp->setBrush( QBrush(Qt::blue));
	  tmp->setPen(*pen);
	  GoWhere->addItem(tmp);
	  Dots.append(tmp);
	}
      }

    }
    View->viewport()->update();

    if (LandmarkCounter == -1){
      Dots[n-1]->setBrush( QBrush(Qt::green));
      Dots[n*n-1]->setBrush( QBrush(Qt::green));
      Dots[n*(n-1)]->setBrush( QBrush(Qt::red));
      View->viewport()->update();
      
    }else if (LandmarkCounter == 0){
      Dots[n-1]->setBrush( QBrush(Qt::green));
      Dots[n*n-1]->setBrush( QBrush(Qt::red));
      Dots[n*(n-1)]->setBrush( QBrush(Qt::green));

      View->viewport()->update();
      
    }else if (LandmarkCounter == 1){
      Dots[n-1]->setBrush( QBrush(Qt::red));
      Dots[n*n-1]->setBrush( QBrush(Qt::green));
      Dots[n*(n-1)]->setBrush( QBrush(Qt::green));

      View->viewport()->update();
    }
    LandmarkCounter++;
  }
  else
    RePro::customEvent( qce );
}


  void RobotField::saveData( const SampleData< SampleDataD > &results )
  {
    ofstream df( addPath( "robotfield-data.dat" ).c_str(),
		 ofstream::out | ofstream::app );

    Options header;
    header.addInteger( "index", completeRuns() );
    header.addInteger( "ReProIndex", reproCount() );
    header.addNumber( "ReProTime", reproStartTime(), "s", "%0.3f" );
    header.save( df, "# " );
    df << "# status:\n";
    stimulusData().save( df, "#   " );
    df << "# settings:\n";
    settings().save( df, "#   " );
    df << '\n';

    TableKey datakey;
    datakey.addNumber( "x", "mm", "%6.2f" );
    datakey.addNumber( "y", "mm", "%6.2f" );
    datakey.addNumber( "a", "mV", "%6.3f" );
    datakey.saveKey( df );

    for ( int ix=0; ix<results.size(); ix++ ) {
      for ( int iy=0; iy<results.size(); iy++ ) {
    	datakey.save( df, 1000.0*results.pos( ix ), 0 );
    	datakey.save( df, 1000.0*results[ix].pos( iy ) );
    	datakey.save( df, results[ix][iy] );
    	df << '\n';
      }
    }
  
    df << "\n\n";
  }


void RobotField::setLandMark(){
  int coord = Rob->getCoordinateSystem();
  Rob->setCoordinateSystem(MIROB_COORD_TRANS);
  Landmarks[LandmarkCounter] = new PositionUpdate(Rob->posX(),Rob->posY(),Rob->posZ(),0);
  Rob->setCoordinateSystem(coord);
  wake();

}

//------------------------------------------------

int RobotField::main( void )
{
  // intialize
  LandmarkCounter = -1;
  double resolution = number( "resolution" );
  double speed = number( "speed" );
  postCustomEvent( 13 );
    
  Stack->setCurrentWidget( Init );

  // get the robot device
  unsigned int k   ;
  for (  k=0; k<10; k++ ) {
    Str ns( k+1, 0 );
     Rob = dynamic_cast< ::misc::Mirob* >( device( "robot-" + ns ) );
     if (Rob != 0){
       break;
     }
  }
  if (Rob == 0){
    printlog("No robot found");
    return 1;
  }
  if (!Rob->isCalibrated()){
    printlog("Calibrate the robot first!");
    return 1;

  }
  


  // wait until the edges of the grid are set
  while (LandmarkCounter < 2){
    sleepWait();
    postCustomEvent( 13 );
    if ( interrupt() )
      return Aborted;

  }
  printlog("Edges determined. Starting measurement ...");

  // ---------- stimulate and record -------------------
  postCustomEvent( 12 );
  // get offspring and update directions
  double d1[3] = {Landmarks[0]->x - Landmarks[1]->x, 
		  Landmarks[0]->y - Landmarks[1]->y,
		  Landmarks[0]->z - Landmarks[1]->z };
  double d2[3] = {Landmarks[2]->x - Landmarks[1]->x, 
		  Landmarks[2]->y - Landmarks[1]->y,
		  Landmarks[2]->z - Landmarks[1]->z };
  double b[3] = {Landmarks[1]->x, Landmarks[1]->y, Landmarks[1]->z};

  // get current coordinate system and movement state
  int coord = Rob->getCoordinateSystem();
  int state = Rob->getState();

  // set MIROB into calibrated coordinate system and absolute positioning state
  Rob->setCoordinateSystem(MIROB_COORD_TRANS);
  Rob->setState(ROBOT_POS);

  // @FABIAN:
  // create stimulus:
  double duration = 0.5;
  OutData signal;
  signal.sineWave( duration, 0.0005, 1000.0 ); // 0.5sec langer 1000Hz sinus mit 20kHz
  signal.back() = 0.0;   // last element should be zero, so that the stimululs really ends.
  signal.setTrace( GlobalEField );  // GlobalEField kommt aus efield::Traces und bezeichnet direkte Stimuluation mit den globalen stimuluselektroden

  // results:
  SampleData< SampleDataD > results( 0.0, 1.0, 1.0/resolution );   // @FABIAN: das sollten die x Werte sein
  for ( int r=0; r<results.size(); r++ )
    results[r] = SampleDataD( 0.0, 1.0, 1.0/resolution, 0.0 );   /// @FABIAN: und das die y-WErte (erster, letzter, stepsize, default-value)

  // got to different grid points and measure
  // @FABIAN: da haette ich gerne indices:
  double x,y;
  for ( int ix=0; ix<resolution; ix++ ){
    for ( int iy=0; iy<resolution; iy++ ){
      //  for (double x = 0; x <= 1.; x += 1./(resolution-1) ){
      //    for (double y = 0; y <= 1.; y += 1./(resolution-1) ){
      x = double(ix)/(resolution-1); // @FABIAN oder so aehnlich...
      y = double(iy)/(resolution-1);
      Rob->setPos(b[0] + x*d1[0] + y*d2[0],   // @FABIAN XXX ist da alles mit mutexes gelockt (im demon????)
		  b[1] + x*d1[1] + y*d2[1],
		  b[2] + x*d1[2] + y*d2[2],speed);

      cerr << "Moving to new position." << endl;

      sleep( 0.05 );                           // @FABIAN XXX eventuell mal 0.1 statt 0.05 sec ausprobieren
      while (Rob->positionQueueLength() > 0){  // @FABIAN XXX ist da alles mit mutexes gelockt (im demon????)
	sleep( 0.05 );
      }
      sleep( 0.1 );  // wait to get rid of movement artifacts
      cerr << "Position reached!" << endl;

      // printlog("Measuring signal!");
      cerr << "measuring signal" << endl;
      write( signal );
      sleep( signal.duration() + 0.01 );


      cerr << "analyse signal" << endl;
      // analyze:
      const InData &data = trace( "field" ); // "field" is the name of the trace in relacs.cfg
      cerr << "get signal" << endl;
      SampleDataD response( 0.0, signal.duration(), signal.stepsize() );   // numerics/include/relacs/sampledata.h
      cerr << "copy to response" << endl;
      data.copy( signalTime(), response );   // the response is now the measured voltage during the whole stimulus 
      // z.B.
      cerr << "compute std" << endl;
      double sn = stdev( response );
      results[ix][iy] = sn;

      cerr << "plot data" << endl;
      // plot:
      P.lock();
      P.clear();
      P.plot( results, 1.0, Plot::BlackBlueGreenRedWhiteGradient );  // @FABIAN: siehe plot/examples/xsurfaceplot.cc
      P.unlock();
      P.draw();

      cerr << "Done" << endl;
      // save:
      // hier koennten wir noch die response speichern....      

    }
  }
  Rob->setCoordinateSystem(coord);
  Rob->setState(state);

  saveData( results );

  
  return Completed;
}




addRePro( RobotField, efield );

}; /* namespace efield */

#include "moc_robotfield.cc"
