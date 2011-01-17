/*
  xdetector.cc
  Example for using the Detector class.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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
#include <relacs/array.h>
#include <relacs/sampledata.h>
#include <relacs/eventdata.h>
#include <relacs/detector.h>
using namespace std;
using namespace relacs;


class AcceptSampleDataEvent
{

public:

  int checkEvent( SampleDataD::const_iterator first, SampleDataD::const_iterator last,
		  SampleDataD::const_iterator event, SampleDataD::const_range_iterator eventtime,
		  SampleDataD::const_iterator index, SampleDataD::const_range_iterator indextime,
		  SampleDataD::const_iterator prevevent, SampleDataD::const_range_iterator prevtime,
		  EventData &outevents,
		  double &threshold,
		  double &minthresh, double &maxthresh,
		  double &time, double &size, double &width )
  {
    time = *eventtime;
    size = *event;
    width = 0.0;
    return 1; 
  }

};


class AcceptArrayEvent
{

public:

  int checkEvent( ArrayD::const_iterator first, ArrayD::const_iterator last,
		  ArrayD::const_iterator event, ArrayD::const_iterator eventtime,
		  ArrayD::const_iterator index, ArrayD::const_iterator indextime,
		  ArrayD::const_iterator prevevent, ArrayD::const_iterator prevtime,
		  EventData &outevents,
		  double &threshold,
		  double &minthresh, double &maxthresh,
		  double &time, double &size, double &width )
  {
    time = *eventtime;
    size = *event;
    width = 0.0;
    return 1; 
  }

};


class AcceptEventDataEvent
{

public:

  int checkEvent( EventSizeIterator first, EventSizeIterator last,
		  EventSizeIterator event, EventIterator eventtime,
		  EventSizeIterator index, EventIterator indextime,
		  EventSizeIterator prevevent, EventIterator prevtime,
		  EventData &outevents,
		  double &threshold,
		  double &minthresh, double &maxthresh,
		  double &time, double &size, double &width )
  {
    time = *eventtime;
    size = *event;
    width = 0.0;
    return 1; 
  }

};


int main( int argc, char **argv )
{
  double threshold = 0.5;
 
  // Detect peaks in a SampleData:
  SampleDataD signal = sin( 0.0, 1.0, 0.001, 10.0 );
  signal.save( "signal.dat" );
  Detector< SampleDataD::const_iterator, SampleDataD::const_range_iterator > DS;
  DS.init( signal.begin(), signal.end(), signal.range().begin() );
  EventData outevents( 1000 );
  AcceptSampleDataEvent checksd;
  DS.peak( signal.begin(), signal.end(), outevents,
	   threshold, threshold, threshold, checksd );
  cout << "SampleData: detected " << outevents.size() << " events.\n";
 
  // Detect peaks in a SampleData using the peaks() function:
  outevents.clear();
  peaks( signal, outevents, threshold, checksd );
  cout << "SampleData::peaks(): detected " << outevents.size() << " events.\n";

  // Detect peaks in Arrays:
  ArrayD time( LinearRange( 0.0, 1.0, 0.001 ) );
  ArrayD volt = sin( time, 10.0 );
  Detector< ArrayD::const_iterator, ArrayD::const_iterator > DA;
  DA.init( volt.begin(), volt.end(), time.begin() );
  outevents.clear();
  AcceptArrayEvent checkad;
  DA.peak( volt.begin(), volt.end(), outevents,
	   threshold, threshold, threshold, checkad );
  cout << "Array: detected " << outevents.size() << " events.\n";

  // Detect peaks in EventData:
  EventData events( time, volt );
  Detector< EventSizeIterator, EventIterator > DE;
  DE.init( EventSizeIterator( events.begin() ), EventSizeIterator( events.end() ),
	   EventIterator( events.begin() ) );
  outevents.clear();
  AcceptEventDataEvent checked;
  DE.peak( events.begin(), events.end(), outevents,
	   threshold, threshold, threshold, checked );
  cout << "EventData: detected " << outevents.size() << " events.\n";

}
