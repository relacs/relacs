/*
  efield/eodtools.cc
  Functions for analyzing EODs of weakly electric fish.

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

#include <relacs/efield/eodtools.h>
using namespace relacs;

namespace efield {


EODTools::EODTools( void )
{
}


double EODTools::eodThreshold( const InData &data, double tbegin, double tend,
			       double contrast )
{
  contrast = fabs( contrast );
  if ( contrast > 0.99 )
    contrast = 0.99;
  float min=0.0, max=0.0;
  data.minMax( min, max, tbegin, tend );
  return 0.75*(max-min)*(1.0-contrast)/(1.0+contrast);
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
    // time:
    time = *eventtime;

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
    size = fabs( b ) > 1e-8 ? y1 - 0.25*a*a/b : y1;
    
    // width:
    width = 0.0;
    
    // accept:
    return 1; 
  }

};


void EODTools::eodPeaks( const InData &data, double tbegin, double tend,
			 double threshold, EventData &peaks )
{
  peaks.clear();
  if ( tend <= tbegin )
    return;
  InDataIterator first = data.begin() + tbegin;
  if ( first  < data.minBegin() )
    first = data.minBegin();
  InDataTimeIterator firsttime( first );
  InDataIterator last = data.begin() + tend;
  if ( last  > data.end() )
    last = data.end();
  peaks.reserve( long( (tend-tbegin) * 10000.0 ) );
  AcceptEODPeaks< InDataIterator, InDataTimeIterator > accept;
  Detector< InDataIterator, InDataTimeIterator > D;
  D.init( first, last, firsttime );
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
    size = fabs( b ) > 1e-8 ? y1 - 0.25*a*a/b : y1;
    
    // width:
    width = 0.0;
    
    // accept:
    return 1; 
  }

};


void EODTools::eodTroughs( const InData &data, double tbegin, double tend,
			   double threshold, EventData &troughs )
{
  troughs.clear();
  if ( tend <= tbegin )
    return;
  InDataIterator first = data.begin() + tbegin;
  if ( first < data.minBegin() )
    first = data.minBegin();
  InDataTimeIterator firsttime( first );
  InDataIterator last = data.begin() + tend;
  if ( last > data.end() )
    last = data.end();
  troughs.reserve( long( (tend-tbegin) * 10000.0 ) );
  troughs.setSource( 1 );
  AcceptEODTroughs accept;
  Detector< InDataIterator, InDataTimeIterator > D;
  D.init( first, last, firsttime );
  D.trough( first, last, troughs, threshold,
	    threshold, threshold, accept );
}


class AcceptEODPeaksTroughs
{
public:
  int checkPeak( InDataIterator first, InDataIterator last,
		 InDataIterator event, InDataTimeIterator eventtime,
		 InDataIterator index, InDataTimeIterator indextime,
		 InDataIterator prevevent, InDataTimeIterator prevtime,
		 EventList &outevents,
		 double &threshold,
		 double &minthresh, double &maxthresh,
		 double &time, double &size, double &width )
  { 
    if ( event <= first )
      return 0;

    // time:
    time = *eventtime;

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
    size = fabs( b ) > 1e-8 ? y1 - 0.25*a*a/b : y1;
    
    // width:
    width = 0.0;
    
    // accept:
    return 1; 
  }

  int checkTrough( InDataIterator first, InDataIterator last,
		   InDataIterator event, InDataTimeIterator eventtime, 
		   InDataIterator index, InDataTimeIterator indextime,
		   InDataIterator prevevent, InDataTimeIterator prevtime,
		   EventList &outevents, double &threshold,
		   double &minthresh, double &maxthresh,
		   double &time, double &size, double &width )
  {
    if ( event <= first )
      return 0;

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
    size = fabs( b ) > 1e-8 ? y1 - 0.25*a*a/b : y1;
    
    // width:
    width = 0.0;
    
    // accept:
    return 1; 
  }

};


void EODTools::eodPeaksTroughs( const InData &data, double tbegin, double tend,
				double threshold, EventData &peaks, EventData &troughs )
{
  peaks.clear();
  troughs.clear();
  if ( tend <= tbegin )
    return;
  InDataIterator first = data.begin() + tbegin;
  if ( first < data.minBegin() )
    first = data.minBegin();
  InDataTimeIterator firsttime( first );
  InDataIterator last = data.begin() + tend;
  if ( last > data.end() )
    last = data.end();
  peaks.reserve( long( (tend-tbegin) * 10000.0 ) );
  peaks.setSource( 1 );
  troughs.reserve( long( (tend-tbegin) * 10000.0 ) );
  troughs.setSource( 1 );
  EventList peaktroughs( &peaks );
  peaktroughs.add( &troughs );
  AcceptEODPeaksTroughs accept;
  Detector< InDataIterator, InDataTimeIterator > D;
  D.init( first, last, firsttime );
  D.peakTrough( first, last, peaktroughs, threshold,
		threshold, threshold, accept );
}


double EODTools::meanPeaks( const InData &data, double tbegin, double tend,
			    double threshold )
{
  EventData peaks( 0, true );
  eodPeaks( data, tbegin, tend, threshold, peaks );
  double size = 0.0;
  for ( int k = 0; k<peaks.size(); k++ )
    size += ( peaks.eventSize( k ) - size )/(k+1);
  return size;
}


double EODTools::meanTroughs( const InData &data, double tbegin, double tend,
			      double threshold )
{
  EventData troughs( 0, true );
  eodTroughs( data, tbegin, tend, threshold, troughs );
  double size = 0.0;
  for ( int k = 0; k<troughs.size(); k++ )
    size += ( troughs.eventSize( k ) - size )/(k+1);
  return size;
}


double EODTools::eodAmplitude( const InData &eodd, double tbegin, double tend )
{
  double threshold = eodThreshold( eodd, tbegin, tend, 0.0 );
  EventData peaks( 0, true );
  EventData troughs( 0, true );
  eodPeaksTroughs( eodd, tbegin, tend, threshold, peaks, troughs );
  double peaksize = 0.0;
  for ( int k = 0; k<peaks.size(); k++ )
    peaksize += ( peaks.eventSize( k ) - peaksize )/(k+1);
  double troughsize = 0.0;
  for ( int k = 0; k<troughs.size(); k++ )
    troughsize += ( troughs.eventSize( k ) - troughsize )/(k+1);
  return 0.5*(peaksize - troughsize);  // 0.5 * p-p amplitude
}


void EODTools::beatAmplitudes( const InData &eodd, double tbegin, double tend,
			       double period, double contrast,
			       double &uppermean, double &upperampl,
			       double &lowermean, double &lowerampl )
{
  // duration as integer multiples of beat period:
  double duration = tend - tbegin;
  double window = floor( duration/period )*period;
  double offset = 0.5*(duration - window);

  // threshold:
  double threshold = eodThreshold( eodd, tbegin, tend, contrast );

  // EOD peaks and troughs:
  EventData uppereod( (int)::floor( 2000.0*(tend-tbegin) ), true );
  EventData lowereod( (int)::floor( 2000.0*(tend-tbegin) ), true );
  eodPeaksTroughs( eodd, tbegin, tend, threshold, uppereod, lowereod );

  upperampl = 0.0;
  uppermean = uppereod.meanSize( tbegin+offset, tend-offset, upperampl );
  upperampl *= ::sqrt( 2.0 );  // 0.5 * p-p amplitude for sine wave

  lowerampl = 0.0;
  lowermean = lowereod.meanSize( tbegin+offset, tend-offset, lowerampl );
  lowerampl *= ::sqrt( 2.0 );  // 0.5 * p-p amplitude for sine wave
}


double EODTools::beatAmplitude( const InData &eodd, double tbegin, double tend,
				double period, double contrast )
{
  // duration as integer multiples of beat period:
  double duration = tend - tbegin;
  double window = floor( duration/period )*period;
  double offset = 0.5*(duration - window);

  // threshold:
  double threshold = eodThreshold( eodd, tbegin, tend, contrast );

  // EOD peaks and troughs:
  EventData uppereod( (int)::floor( 2000.0*(tend-tbegin) ), true );
  EventData lowereod( (int)::floor( 2000.0*(tend-tbegin) ), true );
  eodPeaksTroughs( eodd, tbegin, tend, threshold, uppereod, lowereod );

  double uppersd = 0.0;
  uppereod.meanSize( tbegin+offset, tend-offset, uppersd );

  double lowersd = 0.0;
  lowereod.meanSize( tbegin+offset, tend-offset, lowersd );

  return ::sqrt( 2.0 )*0.5*(uppersd + lowersd);  // 0.5 * p-p amplitude
}


double EODTools::beatContrast( const InData &eodd, double tbegin, double tend,
			       double period, double contrast )
{
  // duration as integer multiples of beat period:
  double duration = tend - tbegin;
  double window = floor( duration/period )*period;
  double offset = 0.5*(duration - window);

  // threshold:
  double threshold = eodThreshold( eodd, tbegin, tend, contrast );

  // EOD peaks and troughs:
  EventData uppereod( (int)::floor( 2000.0*(tend-tbegin) ), true );
  EventData lowereod( (int)::floor( 2000.0*(tend-tbegin) ), true );
  eodPeaksTroughs( eodd, tbegin, tend, threshold, uppereod, lowereod );

  double uppersd = 0.0;
  double uppera = uppereod.meanSize( tbegin+offset, tend-offset, uppersd );

  double lowersd = 0.0;
  double lowera = lowereod.meanSize( tbegin+offset, tend-offset, lowersd );

  return ::sqrt( 2.0 )*(uppersd + lowersd)/fabs(uppera - lowera);
}


}; /* namespace efield */
