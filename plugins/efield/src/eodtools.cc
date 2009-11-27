/*
  efield/eodtools.cc
  Functions for analyzing EODs of weakly electric fish.

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

#include <relacs/efield/eodtools.h>
using namespace relacs;

namespace efield {


EODTools::EODTools( void )
{
}


template < typename DataIter, typename TimeIter >
class AcceptEODPeaks
{
public:
  int checkEvent( const DataIter &first, const DataIter &last,
		  DataIter &event, TimeIter &eventtime,
		  DataIter &index, TimeIter &indextime,
		  DataIter &prevevent, TimeIter &prevtime,
		  EventData &outevents,
		  double &threshold,
		  double &minthresh, double &maxthresh,
		  double &time, double &size, double &width )
  { 
    double maxsize = 0.5 * *event;
    double sampleinterval = *eventtime - *(eventtime - 1);

    // previous maxsize crossing time:
    DataIter id = event;
    TimeIter it = eventtime;
    double ival = *id;
    double pval = ival;
    for ( --id, --it; id != first; --id, --it ) {
      ival = *id;
      if ( ival < maxsize ) {
	double m = sampleinterval / ( pval - ival );
	time = *it + m * ( maxsize - ival );
	break;
      }
      pval = ival;
    }

    /*
    // next maxsize crossing time:
    id = event;
    ival = *id;
    pval = ival;
    for ( ++id, ++it; id != last; ++id, ++it ) {
      ival = *id;
      if ( ival < maxsize ) {
	double m = sampleinterval / ( ival - pval );
	double t = *it + m * ( maxsize - ival );
	time = 0.5 * ( time + t );
	break;
      }
      pval = ival;
    }
    */

    // peak:
    double y2 = *event;
    double y3 = *(event+1);
    --event;
    double y1 = *event;
    double a = y3 - 4.0*y2 + 3.0*y1;
    double b = 2.0*y3 - 4.0*y2 + 2.0*y1;
    // peak time:
    // time = *eventtime + event.sampleInterval()*a/b;  // very noisy!
    // peak size:
    size = y1 - 0.25*a*a/b;
    
    // width:
    width = 0.0;
    
    // accept:
    return 1; 
  }

};


void EODTools::eodPeaks( const InData &data, double time, double duration,
			 double threshold, EventData &peaks )
{
  InDataIterator first = data.begin() + time;
  InDataIterator last = data.begin() + time + duration;
  peaks.reserve( long( duration * 10000.0 ) );
  AcceptEODPeaks< InDataIterator, InDataTimeIterator > accept;
  Detector< InDataIterator, InDataTimeIterator > D;
  D.init( first, last, data.timeBegin() + time );
  D.peak( first, last, peaks, threshold,
	  threshold, threshold,
	  accept  );
}


class AcceptEODTroughs
{
public:
  int checkEvent( const InDataIterator &first, 
		  const InDataIterator &last,
		  InDataIterator &event, 
		  InDataTimeIterator &eventtime, 
		  InDataIterator &index,
		  InDataTimeIterator &indextime,
		  InDataIterator &prevevent,
		  InDataTimeIterator &prevtime,
		  EventData &outevents, 
		  double &threshold,
		  double &minthresh, double &maxthresh,
		  double &time, double &size, double &width )
  {
    // time:
    time = *eventtime;

    // peak:
    double y2 = *event;
    double y3 = *(event+1);
    --event;
    double y1 = *event;
    double a = y3 - 4.0*y2 + 3.0*y1;
    double b = 2.0*y3 - 4.0*y2 + 2.0*y1;
    // peak size:
    size = y1 - 0.25*a*a/b;
    
    // width:
    width = 0.0;
    
    // accept:
    return 1; 
  }

};


void EODTools::eodTroughs( const InData &data, double time, double duration,
			   double threshold, EventData &troughs )
{
  InDataIterator first = data.begin() + time;
  InDataIterator last = data.begin() + time + duration;
  if ( last  > data.end() )
    last = data.end();
  troughs.reserve( long( duration * 10000.0 ) );
  troughs.setSource( 1 );
  AcceptEODTroughs accept;
  Detector< InDataIterator, InDataTimeIterator > D;
  D.init( first, last, data.timeBegin() + time );
  D.trough( first, last, troughs, threshold,
	    threshold, threshold, accept );
}


double EODTools::meanTroughs( const InData &data, double time, double duration,
			      double threshold )
{
  EventData troughs( 0, true );
  eodTroughs( data, time, duration, threshold, troughs );
  double size = 0.0;
  for ( int k = 0; k<troughs.size(); k++ ) {
    size += ( troughs.eventSize( k ) - size )/(k+1);
  }

  return size;
}


double EODTools::eodAmplitude( const InData &eodd, const EventData &eode,
			       double time, double duration )
{
  double up = eode.meanSize( time, duration );
  double down = meanTroughs( eodd, time, duration, 0.2 * up );
  return up - down;
}


void EODTools::beatPeakTroughs( const InData &eodd, const EventData &bpe,
				const EventData &bte,
				double time, double duration, double offset,
				double &upperpeak, double &uppertrough,
				double &lowerpeak, double &lowertrough )
{
  // mean upper beat peaks:
  upperpeak = bpe.meanSize( time + offset, duration - 2.0 * offset );
  
  // mean upper beat troughs:
  uppertrough = bte.meanSize( time + offset, duration - 2.0 * offset );
  
  // EOD troughs:
  EventData troughs( 0, true );
  eodTroughs( eodd, time, duration,
	      0.2 * 0.5 * ( upperpeak + uppertrough ), troughs );

  // EOD lower beat:
  double threshold = 0.3 * fabs( upperpeak - uppertrough );
  int n = bpe.count( time, duration );
  if ( n < duration * 100.0 )
    n = (int)(duration * 100.0);
  if ( n < 1 )
    n = 1;
  EventData beatpeaks( 10 * n, true );
  EventData beattroughs( 10 * n, true );
  EventList lowerbeat;
  lowerbeat.add( &beattroughs );
  lowerbeat.add( &beatpeaks );
  EventSizeIterator first = troughs.begin();
  EventSizeIterator last = troughs.end();
  EventIterator firsttime = troughs.begin();
  AcceptEvent< EventSizeIterator, EventIterator > accept;
  Detector< EventSizeIterator, EventIterator > D;
  D.init( first, last, firsttime );
  D.peakTrough( first, last,
		lowerbeat, threshold, threshold, threshold,
		accept );

  // mean lower beat peaks:
  lowerpeak = beatpeaks.meanSize( time + offset, duration - 2.0 * offset );
  
  // mean lower beat troughs:
  lowertrough = beattroughs.meanSize( time + offset, duration - 2.0 * offset );

}


void EODTools::beatAmplitudes( const InData &eodd, const EventData &bpe,
			       const EventData &bte,
			       double time, double duration, double offset,
			       double &min, double &max )
{
  double up, ut;
  double lp, lt;

  beatPeakTroughs( eodd, bpe, bte, time, duration, offset,
		   up, ut, lp, lt );
  max = up - lp;
  min = ut - lt;
}


double EODTools::beatContrast( const InData &eodd, const EventData &bpe,
			       const EventData &bte,
			       double time, double duration, double offset )
{
  double up, ut;
  double lp, lt;

  beatPeakTroughs( eodd, bpe, bte, time, duration, offset,
		   up, ut, lp, lt );
  double max = up - lp;
  double min = ut - lt;

  double c = ( max - min ) / ( max + min );

  return c;
}


}; /* namespace efield */
