/*
  efield/eodtools.cc
  Functions for analyzing EODs of weakly electric fish.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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
  int checkEvent( DataIter first, DataIter last,
		  DataIter event, TimeIter eventtime,
		  DataIter index, TimeIter indextime,
		  DataIter prevevent, TimeIter prevtime,
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


void EODTools::eodPeaks( const InData &data, double tbegin, double tend,
			 double threshold, EventData &peaks )
{
  InDataIterator first = data.begin() + tbegin;
  if ( first  < data.begin() )
    first = data.begin();
  InDataIterator last = data.begin() + tend;
  if ( last  > data.end() )
    last = data.end();
  peaks.reserve( long( (tend-tbegin) * 10000.0 ) );
  AcceptEODPeaks< InDataIterator, InDataTimeIterator > accept;
  Detector< InDataIterator, InDataTimeIterator > D;
  D.init( first, last, data.timeBegin() + tbegin );
  D.peak( first, last, peaks, threshold,
	  threshold, threshold,
	  accept  );
}


class AcceptEODTroughs
{
public:
  int checkEvent( InDataIterator first, 
		  InDataIterator last,
		  InDataIterator event, 
		  InDataTimeIterator eventtime, 
		  InDataIterator index,
		  InDataTimeIterator indextime,
		  InDataIterator prevevent,
		  InDataTimeIterator prevtime,
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


void EODTools::eodTroughs( const InData &data, double tbegin, double tend,
			   double threshold, EventData &troughs )
{
  InDataIterator first = data.begin() + tbegin;
  if ( first  < data.begin() )
    first = data.begin();
  InDataIterator last = data.begin() + tend;
  if ( last  > data.end() )
    last = data.end();
  troughs.reserve( long( (tend-tbegin) * 10000.0 ) );
  troughs.setSource( 1 );
  AcceptEODTroughs accept;
  Detector< InDataIterator, InDataTimeIterator > D;
  D.init( first, last, data.timeBegin() + tbegin );
  D.trough( first, last, troughs, threshold,
	    threshold, threshold, accept );
}


double EODTools::meanTroughs( const InData &data, double tbegin, double tend,
			      double threshold )
{
  EventData troughs( 0, true );
  eodTroughs( data, tbegin, tend, threshold, troughs );
  double size = 0.0;
  for ( int k = 0; k<troughs.size(); k++ ) {
    size += ( troughs.eventSize( k ) - size )/(k+1);
  }

  return size;
}


double EODTools::eodAmplitude( const InData &eodd, const EventData &eode,
			       double tbegin, double tend )
{
  double up = eode.meanSize( tbegin, tend );
  double down = meanTroughs( eodd, tbegin, tend, 0.2 * up );
  return up - down;
}


void EODTools::beatPeakTroughs( const InData &eodd, const EventData &bpe,
				const EventData &bte,
				double tbegin, double tend, double offset,
				double &upperpeak, double &uppertrough,
				double &lowerpeak, double &lowertrough )
{
  // mean upper beat peaks:
  upperpeak = bpe.meanSize( tbegin + offset, tend - offset );
  
  // mean upper beat troughs:
  uppertrough = bte.meanSize( tbegin + offset, tend - offset );
  
  // EOD troughs:
  EventData troughs( 0, true );
  eodTroughs( eodd, tbegin, tend,
	      0.2 * 0.5 * ( upperpeak + uppertrough ), troughs );

  // EOD lower beat:
  double threshold = 0.3 * fabs( upperpeak - uppertrough );
  int n = bpe.count( tbegin, tend );
  if ( n < (tend-tbegin) * 100.0 )
    n = (int)((tend-tbegin) * 100.0);
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
  lowerpeak = beatpeaks.meanSize( tbegin + offset, tend - offset );
  
  // mean lower beat troughs:
  lowertrough = beattroughs.meanSize( tbegin + offset, tend - offset );

}


void EODTools::beatAmplitudes( const InData &eodd, const EventData &bpe,
			       const EventData &bte,
			       double tbegin, double tend, double offset,
			       double &min, double &max )
{
  double up, ut;
  double lp, lt;

  beatPeakTroughs( eodd, bpe, bte, tbegin, tend, offset,
		   up, ut, lp, lt );
  max = up - lp;
  min = ut - lt;
}


double EODTools::beatContrast( const InData &eodd, const EventData &bpe,
			       const EventData &bte,
			       double tbegin, double tend, double offset )
{
  double up, ut;
  double lp, lt;

  beatPeakTroughs( eodd, bpe, bte, tbegin, tend, offset,
		   up, ut, lp, lt );
  double max = up - lp;
  double min = ut - lt;

  double c = ( max - min ) / ( max + min );

  return c;
}


void EODTools::beatAmplitudes( const InData &eodd, const EventData &eode,
			       double tbegin, double tend, double offset,
			       double &uppermean, double &upperampl,
			       double &lowermean, double &lowerampl )
{
  // EOD peaks:
  upperampl = 0.0;
  uppermean = eode.meanSize( tbegin+offset, tend-offset, upperampl );
  upperampl *= ::sqrt( 2.0 );

  // EOD troughs:
  double threshold = fabs( uppermean ) - upperampl;
  EventData lowereod( eode.count( tbegin, tend ), true );
  eodTroughs( eodd, tbegin, tend, threshold, lowereod );
  lowerampl = 0.0;
  lowermean = lowereod.meanSize( tbegin+offset, tend-offset, lowerampl );
  lowerampl *= ::sqrt( 2.0 );
}


double EODTools::beatAmplitude( const InData &eodd, const EventData &eode,
				double tbegin, double tend, double offset )
{
  // EOD peaks:
  double uppersd = 0.0;
  double uppera = eode.meanSize( tbegin+offset, tend-offset, uppersd );

  // EOD troughs:
  double threshold = fabs( uppera ) - sqrt( 2.0 )*uppersd;
  EventData lowereod( eode.count( tbegin, tend ), true );
  eodTroughs( eodd, tbegin, tend, threshold, lowereod );
  double lowersd = 0.0;
  lowereod.meanSize( tbegin+offset, tend-offset, lowersd );

  return ::sqrt( 2.0 )*0.5*(uppersd + lowersd);
}


double EODTools::beatContrast( const InData &eodd, const EventData &eode,
			       double tbegin, double tend, double offset )
{
  // EOD peaks:
  double uppersd = 0.0;
  double uppera = eode.meanSize( tbegin+offset, tend-offset, uppersd );

  // EOD troughs:
  double threshold = fabs( uppera ) - sqrt( 2.0 )*uppersd;
  EventData lowereod( eode.count( tbegin, tend ), true );
  eodTroughs( eodd, tbegin, tend, threshold, lowereod );
  double lowersd = 0.0;
  double lowera = lowereod.meanSize( tbegin+offset, tend-offset, lowersd );

  return ::sqrt( 2.0 )*(uppersd + lowersd)/(fabs(uppera) + fabs(lowera));
}


}; /* namespace efield */
