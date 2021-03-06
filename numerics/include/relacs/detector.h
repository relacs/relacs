/*
  detector.h
  Some basic event detector algorithms.

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

#ifndef _RELACS_DETECTOR_H_
#define _RELACS_DETECTOR_H_ 1

#include <relacs/eventdata.h>
#include <relacs/eventlist.h>

namespace relacs {


/*!
\class Detector
\author Jan Benda
\version 1.0
\brief Some basic event detector algorithms.

The Detector class provides several algorithms for incremental
detection of events (e.g. peaks, troughs, zero crossings, etc.)
on a range of input data.
The input data are traversed by two iterators.
One of type \a DataIter returning the data values on which the events are to be detected,
the other of \a  TimeIter returning the corresponding times.
Usually, you should provide const types for both \a DataIter and \a TimeIter.
*/

template < typename DataIter, typename TimeIter >
class Detector
{

public:

    /*! Constructor. */
  Detector( void );
    /*! Constructs Detector with history size \a n. */
  Detector( int n );
    /*! Destructor. */
  ~Detector( void );

    /*! Init the detector.
        \param[in] first iterator pointing to the first data element.
        \param[in] last iterator pointing behind the last data element,
	           if there aren't any data yet, then \a last should equal \a first.
        \param[in] firsttime iterator pointing to the time of the first data element. */
  void init( DataIter first, DataIter last,
	     TimeIter firsttime );

    /*! The peak and trough finding algorithm from B. Todd and D. Andrews 
        ("The identification of peaks in physiological signals.",
	Computers and Biomedical Research, 32, 322-335, 1999).
	The peaks are stored in \a outevents[0], the troughs in \a outevents[1].
	A peak is detected if it is higher than \a threshold relative
	to the following minimum.
	A trough is detected if it is lower than \a threshold relative
	to the following maximum. 
	All thresholds (\a threshold \a minthresh, \a maxthresh) 
	have to be positive numbers!
        \param[in] first iterator pointing to the first *accessible* data element.
        \param[in] last iterator pointing behind the last data element.
        \param[out] outevents \a outevents[0] gets the peak times and \a outevents[1] the troughs. 
	\param threshold the current value of the threshold
	\param[in] minthresh the minimum value for the threshold
	\param[in] maxthresh the maximum value for the threshold
	\param check a clas that implements a checkPeak() and a checkTrough() function.
	For details see the AcceptEvent implementation.
    */
  template < class Check >
  void peakTrough( DataIter first, DataIter last,
		   EventList &outevents,
		   double &threshold, double minthresh, double maxthresh,
		   Check &check );
  template < class Check >
  void peakTroughHist( DataIter first, DataIter last,
		       EventList &outevents,
		       double &threshold, double minthresh, double maxthresh,
		       Check &check );
  template < class Check >
  void peak( DataIter first, DataIter last,
	     EventData &outevents,
	     double &threshold, double minthresh, double maxthresh,
	     Check &check );
  template < class Check >
  void peakHist( DataIter first, DataIter last,
		 EventData &outevents,
		 double &threshold, double minthresh, double maxthresh,
		 Check &check );
  template < class Check >
  void trough( DataIter first, DataIter last,
	       EventData &outevents,
	       double &threshold, double minthresh, double maxthresh,
	       Check &check );
  template < class Check >
  void troughHist( DataIter first, DataIter last,
		   EventData &outevents,
		   double &threshold, double minthresh, double maxthresh,
		   Check &check );
  template < class Check >
  void dynamicPeakTrough( DataIter first, DataIter last,
			  EventList &outevents,
			  double &threshold, double minthresh, double maxthresh,
			  double delay, double decay, Check &check );
  template < class Check >
  void dynamicPeakTroughHist( DataIter first, DataIter last,
			      EventList &outevents,
			      double &threshold, double minthresh, double maxthresh,
			      double delay, double decay, Check &check );
  template < class Check >
  void dynamicPeak( DataIter first, DataIter last,
		    EventData &outevents,
		    double &threshold, double minthresh, double maxthresh,
		    double delay, double decay, Check &check );
  template < class Check >
  void dynamicPeakHist( DataIter first, DataIter last,
			EventData &outevents,
			double &threshold, double minthresh, double maxthresh,
			double delay, double decay, Check &check );
  template < class Check >
  void dynamicTrough( DataIter first, DataIter last,
		      EventData &outevents,
		      double &threshold, double minthresh, double maxthresh,
		      double delay, double decay, Check &check );
  template < class Check >
  void dynamicTroughHist( DataIter first, DataIter last,
			  EventData &outevents,
			  double &threshold, double minthresh, double maxthresh,
			  double delay, double decay, Check &check );

     /*! Detect events crossing the threshold with a positive slope
         in a single trace of the analog data refered to by \a events. */
  template < class Check >
  void rising( DataIter first, DataIter last,
	       EventData &outevents,
	       double &threshold, double minthresh, double maxthresh,
	       Check &check );
    /*! Detect events crossing the threshold with a negative slope
        in a single trace of the analog data refered to by \a events. */
  template < class Check >
  void falling( DataIter first, DataIter last,
		EventData &outevents,
		double &threshold, double minthresh, double maxthresh,
		Check &check );

    /*! Detect events crossing the threshold with a positive slope
        in a single trace of the analog data refered to by \a events. 
	The threshold is dynamic. If there are no events detected
	after a delay of \a delay seconds the
	threshold decays to \a minthresh with a decay time constant of
	\a decay seconds. 
	\a minthresh can be greater than \a maxthresh. */
  template < class Check >
  void dynamicRising( DataIter first, DataIter last,
		      EventData &outevents,
		      double &threshold, double minthresh, double maxthresh,
		      double delay, double decay, Check &check );
    /*! Detect events crossing the threshold with a negative slope
        in a single trace of the analog data refered to by \a events. 
	The threshold is dynamic. If there are no events detected
	after a delay of \a delay seconds the
	threshold decays to \a minthresh with a decay time constant of
	\a decay seconds. 
	\a minthresh can be greater than \a maxthresh. */
  template < class Check >
  void dynamicFalling( DataIter first, DataIter last,
		       EventData &outevents,
		       double &threshold, double minthresh, double maxthresh,
		       double delay, double decay, Check &check );

    /*! Detect the largest local maxima above succeding \a threshold crossings.
        All local maxima are stored in \a BadEvents. */
  template < class Check >
  void thresholdPeakHist( DataIter first, DataIter last,
			  EventData &outevents,
			  double &threshold, double minthresh, double maxthresh,
			  Check &check );
    /*! Detect smallest local minima below succeding \a threshold crossings.
        All local minima are stored in \a BadEvents. */
  template < class Check >
  void thresholdTroughHist( DataIter first, DataIter last,
			    EventData &outevents,
			    double &threshold, double minthresh, double maxthresh,
			    Check &check );

  const EventData &goodEvents( void ) const { return GoodEvents; };
  const EventData &badEvents( void ) const { return BadEvents; };
  void setHistorySize( int n ) { GoodEvents.reserve( n ); BadEvents.reserve( n ); };
  int historySize( void ) const { return GoodEvents.capacity(); };
  void clearHistory( void ) { GoodEvents.clear(); BadEvents.clear(); };


 private:

    /*! If \a threshold excceds the interval [\a minthresh, \a maxthresh]
        it is set to the boundaries of that interval.
        \a minthresh can also be greater than \a maxthresh. */
  void checkThresh( double &threshold,
		    double minthresh, double maxthresh );

  int Dir;
  DataIter Index;
  TimeIter IndexTime;
  DataIter MinIndex;
  TimeIter MinTime;
  DataIter MaxIndex;
  TimeIter MaxTime;
  double MinValue;
  double MaxValue;
  bool EventPending;

  bool ResumePeak;
  bool ResumeTrough;
  bool Resume;
  DataIter ResumeEvent;
  TimeIter ResumeTime;
  DataIter LastEvent;
  TimeIter LastTime;
  double PreviousEvent;
  double PreviousTime;

  EventData GoodEvents;
  EventData BadEvents;
  double EventSize;

};


/*! 
\class AcceptEvent
\author Jan Benda
\version 1.0
\brief Provides check functions for the Detector algorithms 
       that accept each detected event.
*/


template < typename DataIter, typename TimeIter >
class AcceptEvent
{

public:

    /*! The constructor. */
  AcceptEvent( void ) {};
    /*! The destructor. */
  ~AcceptEvent( void ) {};


  int checkEvent( DataIter first, DataIter last,
		  DataIter event, TimeIter eventtime,
		  DataIter index, TimeIter indextime,
		  DataIter prevevent, TimeIter prevtime,
		  EventData &outevents,
		  double &threshold,
		  double &minthresh, double &maxthresh,
		  double &time, double &size, double &width );
  int checkPeak( DataIter first, DataIter last,
		 DataIter event, TimeIter eventtime,
		 DataIter index, TimeIter indextime,
		 DataIter prevevent, TimeIter prevtime,
		 EventList &outevents,
		 double &threshold,
		 double &minthresh, double &maxthresh,
		 double &time, double &size, double &width );
  int checkTrough( DataIter first, DataIter last,
		   DataIter event, TimeIter eventtime,
		   DataIter index, TimeIter indextime,
		   DataIter prevevent, TimeIter prevtime,
		   EventList &outevents,
		   double &threshold,
		   double &minthresh, double &maxthresh,
		   double &time, double &size, double &width );

};


template < typename DataIter, typename TimeIter >
Detector< DataIter, TimeIter >::Detector( void )
  : ResumePeak( false ),
    ResumeTrough( false ),
    Resume( false ),
    PreviousEvent( 0.0 ),
    PreviousTime( 0.0 ),
    GoodEvents( 0, true ),
    BadEvents( 0, true )
{
  GoodEvents.setCyclic();
  BadEvents.setCyclic();
  GoodEvents.setIdent( "GoodEvents" );
  BadEvents.setIdent( "BadEvents" );
}


template < typename DataIter, typename TimeIter >
Detector< DataIter, TimeIter >::Detector( int n )
  : ResumePeak( false ),
    ResumeTrough( false ),
    Resume( false ),
    PreviousEvent( 0.0 ),
    PreviousTime( 0.0 ),
    GoodEvents( n, true ),
    BadEvents( n, true )
{
  GoodEvents.setCyclic();
  BadEvents.setCyclic();
}


template < typename DataIter, typename TimeIter >
Detector< DataIter, TimeIter >::~Detector( void )
{
}


template < typename DataIter, typename TimeIter >
void Detector< DataIter, TimeIter >::init( DataIter first,
					   DataIter last,
					   TimeIter firsttime )
{
  Dir = 0;
  Index = first;
  IndexTime = firsttime;
  MinIndex = first;
  MinTime = firsttime;
  MaxIndex = first;
  MaxTime = firsttime;
  MinValue = first < last ? *MinIndex : 0.0;
  MaxValue = MinValue;
  EventPending = false;

  ResumePeak = false;
  ResumeTrough = false;
  Resume = false;
  ResumeEvent = first;
  ResumeTime = firsttime;
  LastEvent = first;
  LastTime = firsttime;
  PreviousEvent = 0.0;
  PreviousTime = first < last ? *firsttime : 0.0;

  GoodEvents.clear();
  BadEvents.clear();
  EventSize = 0.0;
}


template < typename DataIter, typename TimeIter >
void Detector< DataIter, TimeIter >::checkThresh( double &threshold,
						  double minthresh,
						  double maxthresh )
{
  if ( minthresh < maxthresh ) {
    if ( threshold > maxthresh )
      threshold = maxthresh;
    else if ( threshold < minthresh )
      threshold = minthresh;
  }
  else {
    if ( threshold > minthresh )
      threshold = minthresh;
    else if ( threshold < maxthresh )
      threshold = maxthresh;
  }
}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::peakTrough( DataIter first,
						 DataIter last,
						 EventList &outevents,
						 double &threshold,
						 double minthresh,
						 double maxthresh,
						 Check &check )
  {
  if ( Index < first || Index >= last )
    return;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( ResumePeak && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkPeak( first, last,
			     ResumeEvent, ResumeTime,
			     Index, IndexTime,
			     LastEvent, LastTime,
			     outevents, threshold, 
			     minthresh, maxthresh,
			     time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents[0].push( time, size, width );
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
    }
    else if ( r < 0 ) {
      ResumePeak = true;
      return;
    }
    else
      outevents[0].updateMeanQuality();
  }
  ResumePeak = false;

  if ( ResumeTrough && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkTrough( first, last,
			       ResumeEvent, ResumeTime,
			       Index, IndexTime,
			       LastEvent, LastTime,
			       outevents, threshold, 
			       minthresh, maxthresh,
			       time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents[1].push( time, size, width );
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
    }
    else if ( r < 0 ) {
      ResumeTrough = true;
      return;
    }
    else
      outevents[1].updateMeanQuality();
  }
  ResumeTrough = false;

  // loop through the new read data:
  for ( ; Index < last; ++Index, ++IndexTime ) {
    // rising?
    if ( Dir > 0 ) {
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( MaxValue >= *Index + threshold ) {
	// there was a peak:
	DataIter event = MaxIndex;
	TimeIter eventtime = MaxTime;
	if ( event >= first ) {
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  int r = check.checkPeak( first, last,
				   event, eventtime,
				   Index, IndexTime,
				   LastEvent, LastTime,
				   outevents, threshold, 
				   minthresh, maxthresh, 
				   time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents[0].push( time, size, width );
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	  }
	  else if ( r < 0 ) {
	    ResumePeak = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	  }
	  else
	    outevents[0].updateMeanQuality();
	}
	// change direction:
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
	Dir = -1;
	if ( ResumePeak )
	  break;
      }
    }
    // falling?
    else if ( Dir < 0 ) {
      if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
      else if ( *Index >= MinValue + threshold ) {
	// there was a trough:
	DataIter event = MinIndex;
	TimeIter eventtime = MinTime;
	if ( event >= first ) {
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  int r = check.checkTrough( first, last,
				     event, eventtime,
				     Index, IndexTime,
				     LastEvent, LastTime,
				     outevents, threshold, 
				     minthresh, maxthresh,
				     time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents[1].push( time, size, width );
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	  }
	  else if ( r < 0 ) {
	    ResumeTrough = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	  }
	  else
	    outevents[1].updateMeanQuality();
	}
	// change direction:
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
	Dir = 1;
	if ( ResumeTrough )
	  break;
      }
    }
    // don't know:
    else {
      if ( MaxValue >= *Index + threshold )
	Dir = -1;               // falling
      else if ( *Index >= MinValue + threshold )
	Dir = 1;                // rising
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
    }
    
  }  

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::peakTroughHist( DataIter first,
						     DataIter last,
						     EventList &outevents,
						     double &threshold,
						     double minthresh,
						     double maxthresh,
						     Check &check )
{
  if ( Index < first || Index >= last )
    return;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( ResumePeak && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkPeak( first, last,
			     ResumeEvent, ResumeTime,
			     Index, IndexTime,
			     LastEvent, LastTime,
			     outevents, threshold, 
			     minthresh, maxthresh,
			     time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents[0].push( time, size, width );
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
      GoodEvents.push( time, EventSize );
    }
    else if ( r < 0 ) {
      ResumePeak = true;
      return;
    }
    else {
      outevents[0].updateMeanQuality();
      BadEvents.push( time, EventSize );
    }
  }
  ResumePeak = false;

  if ( ResumeTrough && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkTrough( first, last,
			       ResumeEvent, ResumeTime,
			       Index, IndexTime,
			       LastEvent, LastTime,
			       outevents, threshold, 
			       minthresh, maxthresh,
			       time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents[1].push( time, size, width );
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
      GoodEvents.push( time, EventSize );
    }
    else if ( r < 0 ) {
      ResumeTrough = true;
      return;
    }
    else {
      outevents[1].updateMeanQuality();
      BadEvents.push( time, EventSize );
    }
  }
  ResumeTrough = false;

  // loop through the new read data:
  for ( ; Index < last; ++Index, ++IndexTime ) {
    // rising?
    if ( Dir > 0 ) {
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( MaxValue >= *Index + threshold ) {
	// there was a peak:
	DataIter event = MaxIndex;
	TimeIter eventtime = MaxTime;
	if ( event >= first ) {
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  EventSize = MaxValue - MinValue;
	  int r = check.checkPeak( first, last,
				   event, eventtime,
				   Index, IndexTime,
				   LastEvent, LastTime,
				   outevents, threshold, 
				   minthresh, maxthresh, 
				   time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents[0].push( time, size, width );
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	    GoodEvents.push( time, EventSize );
	  }
	  else if ( r < 0 ) {
	    ResumePeak = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	  }
	  else {
	    outevents[0].updateMeanQuality();
	    BadEvents.push( time, EventSize );
	  }
	}
	// change direction:
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
	Dir = -1;
	if ( ResumePeak )
	  break;
      }
      else if ( Index > first + 1 ) {
	DataIter data1 = Index-1;
	DataIter data2 = Index-2;
	if ( *Index > *data1 && *data1 < *data2 ) {
	  // local minimum:
	  TimeIter time1 = IndexTime-1;
	  BadEvents.push( *time1, *data1 - MinValue );
	}
      }
    }
    // falling?
    else if ( Dir < 0 ) {
      if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
      else if ( *Index >= MinValue + threshold ) {
	// there was a trough:
	DataIter event = MinIndex;
	TimeIter eventtime = MinTime;
	if ( event >= first ) {
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  EventSize = MaxValue - MinValue;
	  int r = check.checkTrough( first, last,
				     event, eventtime,
				     Index, IndexTime,
				     LastEvent, LastTime,
				     outevents, threshold, 
				     minthresh, maxthresh,
				     time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents[1].push( time, size, width );
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	    GoodEvents.push( time, EventSize );
	  }
	  else if ( r < 0 ) {
	    ResumeTrough = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	  }
	  else {
	    outevents[1].updateMeanQuality();
	    BadEvents.push( time, EventSize );
	  }
	}
	// change direction:
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
	Dir = 1;
	if ( ResumeTrough )
	  break;
      }
      else if ( Index > first + 1 ) {
	DataIter data1 = Index-1;
	DataIter data2 = Index-2;
	if ( *Index < *data1 && *data1 > *data2 ) {
	  // local maximum:
	  TimeIter time1 = IndexTime-1;
	  BadEvents.push( *time1, *data1 - MinValue );
	}
      }
    }
    // don't know:
    else {
      if ( MaxValue >= *Index + threshold )
	Dir = -1;               // falling
      else if ( *Index >= MinValue + threshold )
	Dir = 1;                // rising
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
    }
    
  }  

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::peak( DataIter first,
					   DataIter last,
					   EventData &outevents,
					   double &threshold,
					   double minthresh,
					   double maxthresh,
					   Check &check )
{
  if ( Index < first || Index >= last )
    return;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( Resume && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkEvent( first, last,
			      ResumeEvent, ResumeTime,
			      Index, IndexTime,
			      LastEvent, LastTime,
			      outevents, threshold, 
			      minthresh, maxthresh,
			      time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents.push( time, size, width );
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
    }
    else if ( r < 0 ) {
      Resume = true;
      return;
    }
    else
      outevents.updateMeanQuality();
  }
  Resume = false;

  // loop through the new read data:
  for ( ; Index < last; ++Index, ++IndexTime ) {
    // rising?
    if ( Dir > 0 ) {
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( MaxValue >= *Index + threshold ) {
	// there was a peak:
	DataIter event = MaxIndex;
	TimeIter eventtime = MaxTime;
	if ( event >= first ) {
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  int r = check.checkEvent( first, last,
				    event, eventtime,
				    Index, IndexTime,
				    LastEvent, LastTime,
				    outevents, threshold, 
				    minthresh, maxthresh,
				    time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents.push( time, size, width );
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	  }
	  else if ( r < 0 ) {
	    Resume = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	  }
	  else
	    outevents.updateMeanQuality();
	}
	// change direction:
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
	Dir = -1;
	if ( Resume )
	  break;
      }
    }
    // falling?
    else if ( Dir < 0 ) {
      if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
      else if ( *Index >= MinValue + threshold ) {
	// there was a trough:
	LastEvent = MinIndex;
	LastTime = MinTime;
	// change direction:
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
	Dir = 1;
      }
    }
    // don't know:
    else {
      if ( MaxValue >= *Index + threshold )
	Dir = -1;               // falling
      else if ( *Index >= MinValue + threshold )
	Dir = 1;                // rising
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
    }
    
  }  

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::peakHist( DataIter first,
					       DataIter last,
					       EventData &outevents,
					       double &threshold,
					       double minthresh,
					       double maxthresh,
					       Check &check )
{
  if ( Index < first || Index >= last )
    return;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( Resume && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkEvent( first, last,
			      ResumeEvent, ResumeTime,
			      Index, IndexTime,
			      LastEvent, LastTime,
			      outevents, threshold, 
			      minthresh, maxthresh,
			      time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents.push( time, size, width );
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
      GoodEvents.push( time, EventSize );
    }
    else if ( r < 0 ) {
      Resume = true;
      return;
    }
    else {
      outevents.updateMeanQuality();
      BadEvents.push( time, EventSize );
    }
  }
  Resume = false;

  // loop through the new read data:
  for ( ; Index < last; ++Index, ++IndexTime ) {
    // rising?
    if ( Dir > 0 ) {
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( MaxValue >= *Index + threshold ) {
	// there was a peak:
	DataIter event = MaxIndex;
	TimeIter eventtime = MaxTime;
	if ( event >= first ) {
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  EventSize = MaxValue - MinValue;
	  int r = check.checkEvent( first, last,
				    event, eventtime,
				    Index, IndexTime,
				    LastEvent, LastTime,
				    outevents, threshold, 
				    minthresh, maxthresh,
				    time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents.push( time, size, width );
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	    GoodEvents.push( time, EventSize );
	  }
	  else if ( r < 0 ) {
	    Resume = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	  }
	  else {
	    outevents.updateMeanQuality();
	    BadEvents.push( time, EventSize );
	  }
	}
	// change direction:
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
	Dir = -1;
	if ( Resume )
	  break;
      }
    }
    // falling?
    else if ( Dir < 0 ) {
      if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
      else if ( *Index >= MinValue + threshold ) {
	// there was a trough:
	LastEvent = MinIndex;
	LastTime = MinTime;
	// change direction:
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
	Dir = 1;
      }
      else if ( Index > first + 1 ) {
	DataIter data1 = Index-1;
	DataIter data2 = Index-2;
	if ( *Index < *data1 && *data1 > *data2 ) {
	  // local maximum:
	  TimeIter time1 = IndexTime-1;
	  BadEvents.push( *time1, *data1 - MinValue );
	}
      }
    }
    // don't know:
    else {
      if ( MaxValue >= *Index + threshold )
	Dir = -1;               // falling
      else if ( *Index >= MinValue + threshold )
	Dir = 1;                // rising
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
    }
    
  }  

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::trough( DataIter first,
					     DataIter last,
					     EventData &outevents,
					     double &threshold,
					     double minthresh, double maxthresh,
					     Check &check )
{
  if ( Index < first || Index >= last )
    return;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( Resume && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkEvent( first, last,
			      ResumeEvent, ResumeTime,
			      Index, IndexTime,
			      LastEvent, LastTime,
			      outevents, threshold, 
			      minthresh, maxthresh,
			      time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents.push( time, size, width );
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
    }
    else if ( r < 0 ) {
      Resume = true;
      return;
    }
    else
      outevents.updateMeanQuality();
  }
  Resume = false;

  // loop through the new read data:
  for ( ; Index < last; ++Index, ++IndexTime ) {
    // rising?
    if ( Dir > 0 ) {
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( MaxValue >= *Index + threshold ) {
	// there was a peak:
	LastEvent = MaxIndex;
	LastTime = MaxTime;
	// change direction:
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
	Dir = -1;
      }
    }
    // falling?
    else if ( Dir < 0 ) {
      if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
      else if ( *Index >= MinValue + threshold ) {
	// there was a trough:
	DataIter event = MinIndex;
	TimeIter eventtime = MinTime;
	if ( event >= first ) {
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  int r = check.checkEvent( first, last,
				    event, eventtime,
				    Index, IndexTime,
				    LastEvent, LastTime,
				    outevents,
				    threshold, minthresh, maxthresh,
				    time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents.push( time, size, width );
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	  }
	  else if ( r < 0 ) {
	    Resume = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	  }
	  else
	    outevents.updateMeanQuality();
	}
	// change direction:
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
	Dir = 1;
	if ( Resume )
	  break;
      }
    }
    // don't know:
    else {
      if ( MaxValue >= *Index + threshold )
	Dir = -1;               // falling
      else if ( *Index >= MinValue + threshold )
	Dir = 1;                // rising
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
    }
    
  }  

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::troughHist( DataIter first,
						 DataIter last,
						 EventData &outevents,
						 double &threshold,
						 double minthresh, double maxthresh,
						 Check &check )
{
  if ( Index < first || Index >= last )
    return;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( Resume && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkEvent( first, last,
			      ResumeEvent, ResumeTime,
			      Index, IndexTime,
			      LastEvent, LastTime,
			      outevents, threshold, 
			      minthresh, maxthresh,
			      time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents.push( time, size, width );
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
      GoodEvents.push( time, EventSize );
    }
    else if ( r < 0 ) {
      Resume = true;
      return;
    }
    else {
      outevents.updateMeanQuality();
      BadEvents.push( time, EventSize );
    }
  }
  Resume = false;

  // loop through the new read data:
  for ( ; Index < last; ++Index, ++IndexTime ) {
    // rising?
    if ( Dir > 0 ) {
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( MaxValue >= *Index + threshold ) {
	// there was a peak:
	LastEvent = MaxIndex;
	LastTime = MaxTime;
	// change direction:
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
	Dir = -1;
      }
      else if ( Index > first + 1 ) {
	DataIter data1 = Index-1;
	DataIter data2 = Index-2;
	if ( *Index > *data1 && *data1 < *data2 ) {
	  // local minimum:
	  TimeIter time1 = IndexTime-1;
	  BadEvents.push( *time1, *data1 - MinValue );
	}
      }
    }
    // falling?
    else if ( Dir < 0 ) {
      if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
      else if ( *Index >= MinValue + threshold ) {
	// there was a trough:
	DataIter event = MinIndex;
	TimeIter eventtime = MinTime;
	if ( event >= first ) {
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  EventSize = MaxValue - MinValue;
	  int r = check.checkEvent( first, last,
				    event, eventtime,
				    Index, IndexTime,
				    LastEvent, LastTime,
				    outevents,
				    threshold, minthresh, maxthresh,
				    time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents.push( time, size, width );
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	    GoodEvents.push( time, EventSize );
	  }
	  else if ( r < 0 ) {
	    Resume = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	  }
	  else {
	    outevents.updateMeanQuality();
	    BadEvents.push( time, EventSize );
	  }
	}
	// change direction:
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
	Dir = 1;
	if ( Resume )
	  break;
      }
    }
    // don't know:
    else {
      if ( MaxValue >= *Index + threshold )
	Dir = -1;               // falling
      else if ( *Index >= MinValue + threshold )
	Dir = 1;                // rising
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
    }
    
  }  

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::dynamicPeakTrough( DataIter first,
							DataIter last,
							EventList &outevents,
							double &threshold,
							double minthresh,
							double maxthresh,
							double delay,
							double decay,
							Check &check )
{
  if ( Index < first || Index >= last )
    return;

  double currenttime = 0.0;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( ResumePeak && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkPeak( first, last,
			     ResumeEvent, ResumeTime,
			     Index, IndexTime,
			     LastEvent, LastTime,
			     outevents, threshold, 
			     minthresh, maxthresh,
			     time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents[0].push( time, size, width );
      PreviousEvent = time;
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
    }
    else if ( r < 0 ) {
      ResumePeak = true;
      return;
    }
    else
      outevents[0].updateMeanQuality();
  }
  ResumePeak = false;

  if ( ResumeTrough && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkTrough( first, last,
			       ResumeEvent, ResumeTime,
			       Index, IndexTime,
			       LastEvent, LastTime,
			       outevents, threshold, 
			       minthresh, maxthresh,
			       time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents[1].push( time, size, width );
      PreviousEvent = time;
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
    }
    else if ( r < 0 ) {
      ResumeTrough = true;
      return;
    }
    else
      outevents[1].updateMeanQuality();
  }
  ResumeTrough = false;

  // loop through the new read data:
  for ( ; Index < last; ++Index, ++IndexTime ) {

    currenttime = *IndexTime;
    // no recent events?
    if ( currenttime - PreviousEvent > delay
	 && Index > first ) {
      double dt = currenttime - PreviousTime;
      double tau = 1.0;
      if ( decay > 0.0 ) {
	if ( dt > 0.01 * decay )
	  dt /= ceil( dt / ( 0.01 * decay ) );
	tau = dt / decay;
      }
      // decay threshold to minthresh:
      for ( double t = PreviousTime; t < currenttime; t += dt )
	threshold += ( minthresh - threshold ) * tau;
    }

    // rising?
    if ( Dir > 0 ) {
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( MaxValue >= *Index + threshold ) {
	// there was a peak:
	DataIter event = MaxIndex;
	TimeIter eventtime = MaxTime;
	if ( event >= first ) {
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  int r = check.checkPeak( first, last,
				   event, eventtime,
				   Index, IndexTime,
				   LastEvent, LastTime,
				   outevents,
				   threshold, minthresh, maxthresh,
				   time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents[0].push( time, size, width );
	    PreviousEvent = time;
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	  }
	  else if ( r < 0 ) {
	    ResumePeak = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	  }
	  else
	    outevents[0].updateMeanQuality();
	  currenttime = *IndexTime;
	}
	// change direction:
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
	Dir = -1;
	if ( ResumePeak )
	  break;
      }
    }
    // falling?
    else if ( Dir < 0 ) {
      if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
      else if ( *Index >= MinValue + threshold ) {
	// there was a trough:
	DataIter event = MinIndex;
	TimeIter eventtime = MinTime;
	if ( event >= first ) {
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  int r = check.checkTrough( first, last,
				     event, eventtime,
				     Index, IndexTime,
				     LastEvent, LastTime,
				     outevents,
				     threshold, minthresh, maxthresh,
				     time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents[1].push( time, size, width );
	    PreviousEvent = time;
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	  }
	  else if ( r < 0 ) {
	    ResumeTrough = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	  }
	  else
	    outevents[1].updateMeanQuality();
	  currenttime = *IndexTime;
	}
	// change direction:
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
	Dir = 1;
	if ( ResumeTrough )
	  break;
      }
    }
    // don't know:
    else {
      if ( MaxValue >= *Index + threshold )
	Dir = -1;               // falling
      else if ( *Index >= MinValue + threshold )
	Dir = 1;                // rising
      if ( MaxValue < *Index ){
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
    }
   
    PreviousTime = currenttime;
 
  }  

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::dynamicPeakTroughHist( DataIter first,
							    DataIter last,
							    EventList &outevents,
							    double &threshold,
							    double minthresh,
							    double maxthresh,
							    double delay,
							    double decay,
							    Check &check )
{
  if ( Index < first || Index >= last )
    return;

  double currenttime = 0.0;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( ResumePeak && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkPeak( first, last,
			     ResumeEvent, ResumeTime,
			     Index, IndexTime,
			     LastEvent, LastTime,
			     outevents, threshold, 
			     minthresh, maxthresh,
			     time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents[0].push( time, size, width );
      PreviousEvent = time;
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
      GoodEvents.push( time, EventSize );
    }
    else if ( r < 0 ) {
      ResumePeak = true;
      return;
    }
    else {
      outevents[0].updateMeanQuality();
      BadEvents.push( time, EventSize );
    }
  }
  ResumePeak = false;

  if ( ResumeTrough && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkTrough( first, last,
			       ResumeEvent, ResumeTime,
			       Index, IndexTime,
			       LastEvent, LastTime,
			       outevents, threshold, 
			       minthresh, maxthresh,
			       time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents[1].push( time, size, width );
      PreviousEvent = time;
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
      GoodEvents.push( time, EventSize );
    }
    else if ( r < 0 ) {
      ResumeTrough = true;
      return;
    }
    else {
      outevents[1].updateMeanQuality();
      BadEvents.push( time, EventSize );
    }
  }
  ResumeTrough = false;

  // loop through the new read data:
  for ( ; Index < last; ++Index, ++IndexTime ) {

    currenttime = *IndexTime;
    // no recent events?
    if ( currenttime - PreviousEvent > delay
	 && Index > first ) {
      double dt = currenttime - PreviousTime;
      double tau = 1.0;
      if ( decay > 0.0 ) {
	if ( dt > 0.01 * decay )
	  dt /= ceil( dt / ( 0.01 * decay ) );
	tau = dt / decay;
      }
      // decay threshold to minthresh:
      for ( double t = PreviousTime; t < currenttime; t += dt )
	threshold += ( minthresh - threshold ) * tau;
    }

    // rising?
    if ( Dir > 0 ) {
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( MaxValue >= *Index + threshold ) {
	// there was a peak:
	DataIter event = MaxIndex;
	TimeIter eventtime = MaxTime;
	if ( event >= first ) {
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  EventSize = MaxValue - MinValue;
	  int r = check.checkPeak( first, last,
				   event, eventtime,
				   Index, IndexTime,
				   LastEvent, LastTime,
				   outevents,
				   threshold, minthresh, maxthresh,
				   time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents[0].push( time, size, width );
	    PreviousEvent = time;
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	    GoodEvents.push( time, EventSize );
	  }
	  else if ( r < 0 ) {
	    ResumePeak = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	  }
	  else {
	    outevents[0].updateMeanQuality();
	    BadEvents.push( time, EventSize );
	  }
	  currenttime = *IndexTime;
	}
	// change direction:
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
	Dir = -1;
	if ( ResumePeak )
	  break;
      }
      else if ( Index > first + 1 ) {
	DataIter data1 = Index-1;
	DataIter data2 = Index-2;
	if ( *Index > *data1 && *data1 < *data2 ) {
	  // local minimum:
	  TimeIter time1 = IndexTime-1;
	  BadEvents.push( *time1, *data1 - MinValue );
	}
      }
    }
    // falling?
    else if ( Dir < 0 ) {
      if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
      else if ( *Index >= MinValue + threshold ) {
	// there was a trough:
	DataIter event = MinIndex;
	TimeIter eventtime = MinTime;
	if ( event >= first ) {
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  EventSize = MaxValue - MinValue;
	  int r = check.checkTrough( first, last,
				     event, eventtime,
				     Index, IndexTime,
				     LastEvent, LastTime,
				     outevents,
				     threshold, minthresh, maxthresh,
				     time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents[1].push( time, size, width );
	    PreviousEvent = time;
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	    GoodEvents.push( time, EventSize );
	  }
	  else if ( r < 0 ) {
	    ResumeTrough = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	  }
	  else {
	    outevents[1].updateMeanQuality();
	    BadEvents.push( time, EventSize );
	  }
	  currenttime = *IndexTime;
	}
	// change direction:
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
	Dir = 1;
	if ( ResumeTrough )
	  break;
      }
      else if ( Index > first + 1 ) {
	DataIter data1 = Index-1;
	DataIter data2 = Index-2;
	if ( *Index < *data1 && *data1 > *data2 ) {
	  // local maximum:
	  TimeIter time1 = IndexTime-1;
	  BadEvents.push( *time1, *data1 - MinValue );
	}
      }
    }
    // don't know:
    else {
      if ( MaxValue >= *Index + threshold )
	Dir = -1;               // falling
      else if ( *Index >= MinValue + threshold )
	Dir = 1;                // rising
      if ( MaxValue < *Index ){
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
    }
   
    PreviousTime = currenttime;
 
  }  

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::dynamicPeak( DataIter first,
						  DataIter last,
						  EventData &outevents,
						  double &threshold,
						  double minthresh,
						  double maxthresh,
						  double delay,
						  double decay,
						  Check &check )
{
  if ( Index < first || Index >= last )
    return;

  double currenttime = 0.0;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( Resume && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkEvent( first, last,
			      ResumeEvent, ResumeTime,
			      Index, IndexTime,
			      LastEvent, LastTime,
			      outevents, threshold, 
			      minthresh, maxthresh,
			      time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents.push( time, size, width );
      PreviousEvent = time;
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
    }
    else if ( r < 0 ) {
      Resume = true;
      return;
    }
    else
      outevents.updateMeanQuality();
  }
  Resume = false;

  // loop through the new read data:
  for ( ; Index < last; ++Index, ++IndexTime ) {

    currenttime = *IndexTime;
    // no recent events?
    if ( currenttime - PreviousEvent > delay
	 && Index > first ) {
      double dt = currenttime - PreviousTime;
      double tau = 1.0;
      if ( decay > 0.0 ) {
	if ( dt > 0.01 * decay )
	  dt /= ceil( dt / ( 0.01 * decay ) );
	tau = dt / decay;
      }
      // decay threshold to minthresh:
      for ( double t = PreviousTime; t < currenttime; t += dt )
	threshold += ( minthresh - threshold ) * tau;
    }

    // rising?
    if ( Dir > 0 ) {
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( MaxValue >= *Index + threshold ) {
	// there was a peak:
	DataIter event = MaxIndex;
	TimeIter eventtime = MaxTime;
	if ( event >= first ) {
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  int r = check.checkEvent( first, last,
				    event, eventtime,
				    Index, IndexTime,
				    LastEvent, LastTime,
				    outevents,
				    threshold, minthresh, maxthresh,
				    time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents.push( time, size, width );
	    PreviousEvent = time;
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	  }
	  else if ( r < 0 ) {
	    Resume = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	  }
	  else
	    outevents.updateMeanQuality();
	  currenttime = *IndexTime;
	}
	// change direction:
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
	Dir = -1;
	if ( Resume )
	  break;
      }
    }
    // falling?
    else if ( Dir < 0 ) {
      if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
      else if ( *Index >= MinValue + threshold ) {
	// there was a trough:
	LastEvent = MinIndex;
	LastTime = MinTime;
	// change direction:
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
	Dir = 1;
      }
    }
    // don't know:
    else {
      if ( MaxValue >= *Index + threshold )
	Dir = -1;               // falling
      else if ( *Index >= MinValue + threshold )
	Dir = 1;                // rising
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
    }
   
    PreviousTime = currenttime;
    
  }  

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::dynamicPeakHist( DataIter first,
						      DataIter last,
						      EventData &outevents,
						      double &threshold,
						      double minthresh,
						      double maxthresh,
						      double delay,
						      double decay,
						      Check &check )
{
  if ( Index < first || Index >= last )
    return;

  double currenttime = 0.0;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( Resume && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkEvent( first, last,
			      ResumeEvent, ResumeTime,
			      Index, IndexTime,
			      LastEvent, LastTime,
			      outevents, threshold, 
			      minthresh, maxthresh,
			      time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents.push( time, size, width );
      PreviousEvent = time;
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
      GoodEvents.push( time, EventSize );
    }
    else if ( r < 0 ) {
      Resume = true;
      return;
    }
    else {
      outevents.updateMeanQuality();
      BadEvents.push( time, EventSize );
    }
  }
  Resume = false;

  // loop through the new read data:
  for ( ; Index < last; ++Index, ++IndexTime ) {

    // update threshold:
    currenttime = *IndexTime;
    // no recent events?
    if ( currenttime - PreviousEvent > delay
	 && Index > first ) {
      double dt = currenttime - PreviousTime;
      double tau = 1.0;
      if ( decay > 0.0 ) {
	if ( dt > 0.01 * decay )
	  dt /= ceil( dt / ( 0.01 * decay ) );
	tau = dt / decay;
      }
      // decay threshold to minthresh:
      for ( double t = PreviousTime; t < currenttime; t += dt )
	threshold += ( minthresh - threshold ) * tau;
    }

    // rising?
    if ( Dir > 0 ) {
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( MaxValue >= *Index + threshold ) {
	// there was a peak:
	DataIter event = MaxIndex;
	TimeIter eventtime = MaxTime;
	if ( event >= first ) {
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  EventSize = MaxValue - MinValue;
	  int r = check.checkEvent( first, last,
				    event, eventtime,
				    Index, IndexTime,
				    LastEvent, LastTime,
				    outevents,
				    threshold, minthresh, maxthresh,
				    time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents.push( time, size, width );
	    PreviousEvent = time;
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	    GoodEvents.push( time, EventSize );
	  }
	  else if ( r < 0 ) {
	    Resume = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	  }
	  else {
	    outevents.updateMeanQuality();
	    BadEvents.push( time, EventSize );
	  }
	  currenttime = *IndexTime;
	}
	// change direction:
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
	Dir = -1;
	if ( Resume )
	  break;
      }
    }
    // falling?
    else if ( Dir < 0 ) {
      if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
      else if ( *Index >= MinValue + threshold ) {
	// there was a trough:
	LastEvent = MinIndex;
	LastTime = MinTime;
	// change direction:
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
	Dir = 1;
      }
      else if ( Index > first + 1 ) {
	DataIter data1 = Index-1;
	DataIter data2 = Index-2;
	if ( *Index < *data1 && *data1 > *data2 ) {
	  // local maximum:
	  TimeIter time1 = IndexTime-1;
	  BadEvents.push( *time1, *data1 - MinValue );
	}
      }
    }
    // don't know:
    else {
      if ( MaxValue >= *Index + threshold )
	Dir = -1;               // falling
      else if ( *Index >= MinValue + threshold )
	Dir = 1;                // rising
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
    }
   
    PreviousTime = currenttime;
    
  }  

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::dynamicTrough( DataIter first,
						    DataIter last,
						    EventData &outevents,
						    double &threshold,
						    double minthresh,
						    double maxthresh,
						    double delay, double decay,
						    Check &check )
{
  if ( Index < first || Index >= last )
    return;

  double currenttime = 0.0;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( Resume && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkEvent( first, last,
			      ResumeEvent, ResumeTime,
			      Index, IndexTime,
			      LastEvent, LastTime,
			      outevents, threshold, 
			      minthresh, maxthresh,
			      time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents.push( time, size, width );
      PreviousEvent = time;
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
    }
    else if ( r < 0 ) {
      Resume = true;
      return;
    }
    else
      outevents.updateMeanQuality();
  }
  Resume = false;

  // loop through the new read data:
  for ( ; Index < last; ++Index, ++IndexTime ) {

    currenttime = *IndexTime;
    // no recent events?
    if ( currenttime - PreviousEvent > delay
	 && Index > first ) {
      double dt = currenttime - PreviousTime;
      double tau = 1.0;
      if ( decay > 0.0 ) {
	if ( dt > 0.01 * decay )
	  dt /= ceil( dt / ( 0.01 * decay ) );
	tau = dt / decay;
      }
      // decay threshold to minthresh:
      for ( double t = PreviousTime; t < currenttime; t += dt )
	threshold += ( minthresh - threshold ) * tau;
    }

    // rising?
    if ( Dir > 0 ) {
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( MaxValue >= *Index + threshold ) {
	// there was a peak:
	LastEvent = MaxIndex;
	LastTime = MaxTime;
	// change direction:
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
	Dir = -1;
      }
    }
    // falling?
    else if ( Dir < 0 ) {
      if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
      else if ( *Index >= MinValue + threshold ) {
	// there was a trough:
	DataIter event = MinIndex;
	TimeIter eventtime = MinTime;
	if ( event >= first ) {
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  int r = check.checkEvent( first, last,
				    event, eventtime,
				    Index, IndexTime,
				    LastEvent, LastTime,
				    outevents,
				    threshold, minthresh, maxthresh,
				    time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents.push( time, size, width );
	    PreviousEvent = time;
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	  }
	  else if ( r < 0 ) {
	    Resume = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	  }
	  else
	    outevents.updateMeanQuality();
	  currenttime = *IndexTime;
	}
	// change direction:
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
	Dir = 1;
	if ( Resume )
	  break;
      }
    }
    // don't know:
    else {
      if ( MaxValue >= *Index + threshold )
	Dir = -1;               // falling
      else if ( *Index >= MinValue + threshold )
	Dir = 1;                // rising
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
    }
   
    PreviousTime = currenttime;
    
  }  

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::dynamicTroughHist( DataIter first,
							DataIter last,
							EventData &outevents,
							double &threshold,
							double minthresh,
							double maxthresh,
							double delay, double decay,
							Check &check )
{
  if ( Index < first || Index >= last )
    return;

  double currenttime = 0.0;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( Resume && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkEvent( first, last,
			      ResumeEvent, ResumeTime,
			      Index, IndexTime,
			      LastEvent, LastTime,
			      outevents, threshold, 
			      minthresh, maxthresh,
			      time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents.push( time, size, width );
      PreviousEvent = time;
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
      GoodEvents.push( time, EventSize );
    }
    else if ( r < 0 ) {
      Resume = true;
      return;
    }
    else {
      outevents.updateMeanQuality();
      BadEvents.push( time, EventSize );
    }
  }
  Resume = false;

  // loop through the new read data:
  for ( ; Index < last; ++Index, ++IndexTime ) {

    currenttime = *IndexTime;
    // no recent events?
    if ( currenttime - PreviousEvent > delay
	 && Index > first ) {
      double dt = currenttime - PreviousTime;
      double tau = 1.0;
      if ( decay > 0.0 ) {
	if ( dt > 0.01 * decay )
	  dt /= ceil( dt / ( 0.01 * decay ) );
	tau = dt / decay;
      }
      // decay threshold to minthresh:
      for ( double t = PreviousTime; t < currenttime; t += dt )
	threshold += ( minthresh - threshold ) * tau;
    }

    // rising?
    if ( Dir > 0 ) {
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( MaxValue >= *Index + threshold ) {
	// there was a peak:
	LastEvent = MaxIndex;
	LastTime = MaxTime;
	// change direction:
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
	Dir = -1;
      }
      else if ( Index > first + 1 ) {
	DataIter data1 = Index-1;
	DataIter data2 = Index-2;
	if ( *Index > *data1 && *data1 < *data2 ) {
	  // local minimum:
	  TimeIter time1 = IndexTime-1;
	  BadEvents.push( *time1, *data1 - MinValue );
	}
      }
    }
    // falling?
    else if ( Dir < 0 ) {
      if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
      else if ( *Index >= MinValue + threshold ) {
	// there was a trough:
	DataIter event = MinIndex;
	TimeIter eventtime = MinTime;
	if ( event >= first ) {
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  EventSize = MaxValue - MinValue;
	  int r = check.checkEvent( first, last,
				    event, eventtime,
				    Index, IndexTime,
				    LastEvent, LastTime,
				    outevents,
				    threshold, minthresh, maxthresh,
				    time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents.push( time, size, width );
	    PreviousEvent = time;
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	    GoodEvents.push( time, EventSize );
	  }
	  else if ( r < 0 ) {
	    Resume = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	  }
	  else {
	    outevents.updateMeanQuality();
	    BadEvents.push( time, EventSize );
	  }
	  currenttime = *IndexTime;
	}
	// change direction:
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
	Dir = 1;
	if ( Resume )
	  break;
      }
    }
    // don't know:
    else {
      if ( MaxValue >= *Index + threshold )
	Dir = -1;               // falling
      else if ( *Index >= MinValue + threshold )
	Dir = 1;                // rising
      if ( MaxValue < *Index ) {
	MaxIndex = Index;       // maximum element
	MaxTime = IndexTime;
	MaxValue = *Index;
      }
      else if ( *Index < MinValue ) {
	MinIndex = Index;       // minimum element
	MinTime = IndexTime;
	MinValue = *Index;
      }
    }
   
    PreviousTime = currenttime;
    
  }  

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::rising( DataIter first,
					     DataIter last,
					     EventData &outevents,
					     double &threshold,
					     double minthresh, double maxthresh,
					     Check &check )
{
  if ( Index < first || Index >= last )
    return;

  DataIter lastindex( last );
  --lastindex;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( Resume && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkEvent( first, last,
			      ResumeEvent, ResumeTime,
			      Index, IndexTime,
			      LastEvent, LastTime,
			      outevents, threshold, 
			      minthresh, maxthresh,
			      time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents.push( time, size, width );
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
    }
    else if ( r < 0 ) {
      Resume = true;
      return;
    }
    else
      outevents.updateMeanQuality();
  }
  Resume = false;

  // loop through the new read data:
  for ( ; Index < lastindex; ++Index, ++IndexTime ) {

    // threshold crossed?
    if ( *Index <= threshold &&
	 *(Index+1) > threshold ) {
      ++Index;
      ++IndexTime;
      // this is an event:
      DataIter event = Index;
      TimeIter eventtime = IndexTime;
      // check this event:
      double time = *eventtime;
      double size = 0.0;
      double width = 0;
      int r = check.checkEvent( first, last,
				event, eventtime,
				Index, IndexTime,
				LastEvent, LastTime,
				outevents, threshold, 
				minthresh, maxthresh,
				time, size, width );
      if ( r > 0 ) {
	// this really is an event:
	outevents.push( time, size, width );
	checkThresh( threshold, minthresh, maxthresh );
	LastEvent = event;
	LastTime = eventtime;
      }
      else if ( r < 0 ) {
	Resume = true;
	ResumeEvent = event;
	ResumeTime = eventtime;
	break;
      }
      else
	outevents.updateMeanQuality();
    }

  }

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::falling( DataIter first,
					      DataIter last,
					      EventData &outevents,
					      double &threshold,
					      double minthresh, double maxthresh,
					      Check &check )
{
  if ( Index < first || Index >= last )
    return;

  DataIter lastindex( last );
  --lastindex;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( Resume && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkEvent( first, last,
			      ResumeEvent, ResumeTime,
			      Index, IndexTime,
			      LastEvent, LastTime,
			      outevents, threshold, 
			      minthresh, maxthresh,
			      time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents.push( time, size, width );
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
    }
    else if ( r < 0 ) {
      Resume = true;
      return;
    }
    else
      outevents.updateMeanQuality();
  }
  Resume = false;

  // loop through the new read data:
  for ( ; Index < lastindex; ++Index, ++IndexTime ) {

    // threshold crossed?
    if ( *Index >= threshold &&
	 *(Index+1) < threshold ) {
      ++Index;
      ++IndexTime;
      // this is an event:
      DataIter event = Index;
      TimeIter eventtime = IndexTime;
      // check this event:
      double time = *eventtime;
      double size = 0.0;
      double width = 0;
      int r = check.checkEvent( first, last,
				event, eventtime,
				Index, IndexTime,
				LastEvent, LastTime,
				outevents, threshold, 
				minthresh, maxthresh,
				time, size, width );
      if ( r > 0 ) {
	// this really is an event:
	outevents.push( time, size, width );
	checkThresh( threshold, minthresh, maxthresh );
	LastEvent = event;
	LastTime = eventtime;
      }
      else if ( r < 0 ) {
	Resume = true;
	ResumeEvent = event;
	ResumeTime = eventtime;
	break;
      }
      else
	outevents.updateMeanQuality();
    }

  }

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::dynamicRising( DataIter first,
						    DataIter last,
						    EventData &outevents,
						    double &threshold,
						    double minthresh,
						    double maxthresh,
						    double delay, double decay,
						    Check &check )
{
  if ( Index < first || Index >= last )
    return;

  double currenttime = 0.0;

  DataIter lastindex( last );
  --lastindex;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( Resume && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkEvent( first, last,
			      ResumeEvent, ResumeTime,
			      Index, IndexTime,
			      LastEvent, LastTime,
			      outevents, threshold, 
			      minthresh, maxthresh,
			      time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents.push( time, size, width );
      PreviousEvent = time;
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
    }
    else if ( r < 0 ) {
      Resume = true;
      return;
    }
    else
      outevents.updateMeanQuality();
  }
  Resume = false;

  // loop through the new read data:
  for ( ; Index < lastindex; ++Index, ++IndexTime ) {

    currenttime = *IndexTime;
    // no recent events?
    if ( currenttime - PreviousEvent > delay
	 && Index > first ) {
      double dt = currenttime - PreviousTime;
      double tau = 1.0;
      if ( decay > 0.0 ) {
	if ( dt > 0.01 * decay )
	  dt /= ceil( dt / ( 0.01 * decay ) );
	tau = dt / decay;
      }
      // decay threshold to minthresh:
      for ( double t = PreviousTime; t < currenttime; t += dt )
	threshold += ( minthresh - threshold ) * tau;
    }

    // threshold crossed?
    if ( *Index <= threshold &&
	 *(Index+1) > threshold ) {
      ++Index;
      ++IndexTime;
      // this is an event:
      DataIter event = Index;
      TimeIter eventtime = IndexTime;
      // check this event:
      double time = *eventtime;
      double size = 0.0;
      double width = 0;
      int r = check.checkEvent( first, last,
				event, eventtime,
				Index, IndexTime,
				LastEvent, LastTime,
				outevents, threshold, 
				minthresh, maxthresh,
				time, size, width );
      if ( r > 0 ) {
	// this really is an event:
	outevents.push( time, size, width );
	PreviousEvent = time;
	checkThresh( threshold, minthresh, maxthresh );
	LastEvent = event;
	LastTime = eventtime;
      }
      else if ( r < 0 ) {
	Resume = true;
	ResumeEvent = event;
	ResumeTime = eventtime;
	break;
      }
      else
	outevents.updateMeanQuality();
      currenttime = *IndexTime;
    }
   
    PreviousTime = currenttime;

  }

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::dynamicFalling( DataIter first,
						     DataIter last,
						     EventData &outevents,
						     double &threshold,
						     double minthresh,
						     double maxthresh,
						     double delay, double decay,
						     Check &check )
{
  if ( Index < first || Index >= last )
    return;

  double currenttime = 0.0;

  DataIter lastindex( last );
  --lastindex;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( Resume && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkEvent( first, last,
			      ResumeEvent, ResumeTime,
			      Index, IndexTime,
			      LastEvent, LastTime,
			      outevents, threshold, 
			      minthresh, maxthresh,
			      time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents.push( time, size, width );
      PreviousEvent = time;
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
    }
    else if ( r < 0 ) {
      Resume = true;
      return;
    }
    else
      outevents.updateMeanQuality();
  }
  Resume = false;
    
  // loop through the new read data:
  for ( ; Index < lastindex; ++Index, ++IndexTime ) {

    currenttime = *IndexTime;
    // no recent events?
    if ( currenttime - PreviousEvent > delay
	 && Index > first ) {
      double dt = currenttime - PreviousTime;
      double tau = 1.0;
      if ( decay > 0.0 ) {
	if ( dt > 0.01 * decay )
	  dt /= ceil( dt / ( 0.01 * decay ) );
	tau = dt / decay;
      }
      // decay threshold to minthresh:
      for ( double t = PreviousTime; t < currenttime; t += dt )
	threshold += ( minthresh - threshold ) * tau;
    }

    // threshold crossed?
    if ( *Index >= threshold &&
	 *(Index+1) < threshold ) {
      ++Index;
      ++IndexTime;
      // this is an event:
      DataIter event = Index;
      TimeIter eventtime = IndexTime;
      // check this event:
      double time = *eventtime;
      double size = 0.0;
      double width = 0;
      int r = check.checkEvent( first, last,
				event, eventtime,
				Index, IndexTime,
				LastEvent, LastTime,
				outevents, threshold, 
				minthresh, maxthresh,
				time, size, width );
      if ( r > 0 ) {
	// this really is an event:
	outevents.push( time, size, width );
	PreviousEvent = time;
	checkThresh( threshold, minthresh, maxthresh );
	LastEvent = event;
	LastTime = eventtime;
      }
      else if ( r < 0 ) {
	Resume = true;
	ResumeEvent = event;
	ResumeTime = eventtime;
	break;
      }
      else
	outevents.updateMeanQuality();
      currenttime = *IndexTime;
    }
   
    PreviousTime = currenttime;

  }

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::thresholdPeakHist( DataIter first,
							DataIter last,
							EventData &outevents,
							double &threshold,
							double minthresh,
							double maxthresh,
							Check &check )
{
  if ( Index < first || Index >= last )
    return;

  DataIter lastindex( last );
  --lastindex;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( Resume && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkEvent( first, last,
			      ResumeEvent, ResumeTime,
			      Index, IndexTime,
			      LastEvent, LastTime,
			      outevents, threshold, 
			      minthresh, maxthresh,
			      time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents.push( time, size, width );
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
      GoodEvents.push( *ResumeTime, *ResumeEvent );
    }
    else if ( r < 0 ) {
      Resume = true;
      return;
    }
    else {
      outevents.updateMeanQuality();
      BadEvents.push( *ResumeTime, *ResumeEvent );
    }
  }
  Resume = false;

  // loop through the new read data:
  for ( ; Index < lastindex; ++Index, ++IndexTime ) {

    // rising:
    if ( Dir > 0 ) {
      // peak:
      if ( *(Index+1) < *Index ) {
	Dir = -1;               // falling
	if ( *Index > threshold ) {
	  if ( ! EventPending ||
	       MaxIndex < first || *Index > *MaxIndex ) {
	    MaxIndex = Index;
	    MaxTime = IndexTime;
	    EventPending = true;
	  }
	}
	else
	  BadEvents.push( *IndexTime, *Index );  // local sub-threshold maximum
      }
    }
    // falling:
    else if ( Dir < 0 ) {
      // the last maximum is an event:
      if ( EventPending && *Index <= threshold ) {
	if ( MaxIndex >= first ) {
	  DataIter event = MaxIndex;
	  TimeIter eventtime = MaxTime;
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  int r = check.checkEvent( first, last,
				    event, eventtime,
				    Index, IndexTime,
				    LastEvent, LastTime,
				    outevents, threshold, 
				    minthresh, maxthresh,
				    time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents.push( time, size, width );
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	    GoodEvents.push( *eventtime, *event );
	  }
	  else if ( r < 0 ) {
	    Resume = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	    break;
	  }
	  else {
	    outevents.updateMeanQuality();
	    BadEvents.push( *eventtime, *event );
	  }
	}
	EventPending = false;
      }
      if ( *(Index+1) > *Index )
	Dir = 1;                // rising
    }
    // don't know:
    else {
      if ( *(Index+1) < *Index )
	Dir = -1;               // falling
      else if ( *(Index+1) > *Index )
	Dir = 1;                // rising
    }

  }

}


template < typename DataIter, typename TimeIter >
template < class Check >
void Detector< DataIter, TimeIter >::thresholdTroughHist( DataIter first,
							  DataIter last,
							  EventData &outevents,
							  double &threshold,
							  double minthresh,
							  double maxthresh,
							  Check &check )
{
  if ( Index < first || Index >= last )
    return;

  DataIter lastindex( last );
  --lastindex;

  checkThresh( threshold, minthresh, maxthresh );

  // check last event again:
  if ( Resume && ResumeEvent >= first ) {
    double time = *ResumeTime;
    double size = 0.0;
    double width = 0;
    int r = check.checkEvent( first, last,
			      ResumeEvent, ResumeTime,
			      Index, IndexTime,
			      LastEvent, LastTime,
			      outevents, threshold, 
			      minthresh, maxthresh,
			      time, size, width );
    if ( r > 0 ) {
      // this really is an event:
      outevents.push( time, size, width );
      checkThresh( threshold, minthresh, maxthresh );
      LastEvent = ResumeEvent;
      LastTime = ResumeTime;
      GoodEvents.push( *ResumeTime, *ResumeEvent );
    }
    else if ( r < 0 ) {
      Resume = true;
      return;
    }
    else {
      outevents.updateMeanQuality();
      BadEvents.push( *ResumeTime, *ResumeEvent );
    }
  }
  Resume = false;

  // loop through the new read data:
  for ( ; Index < lastindex; ++Index, ++IndexTime ) {

    // rising:
    if ( Dir > 0 ) {
      // the last minimum is an event:
      if ( EventPending && *Index >= threshold ) {
	if ( MinIndex >= first ) {
	  DataIter event = MinIndex;
	  TimeIter eventtime = MinTime;
	  // check this event:
	  double time = *eventtime;
	  double size = 0.0;
	  double width = 0;
	  int r = check.checkEvent( first, last,
				    event, eventtime,
				    Index, IndexTime,
				    LastEvent, LastTime,
				    outevents, threshold, 
				    minthresh, maxthresh,
				    time, size, width );
	  if ( r > 0 ) {
	    // this really is an event:
	    outevents.push( time, size, width );
	    checkThresh( threshold, minthresh, maxthresh );
	    LastEvent = event;
	    LastTime = eventtime;
	    GoodEvents.push( *eventtime, *event );
	  }
	  else if ( r < 0 ) {
	    Resume = true;
	    ResumeEvent = event;
	    ResumeTime = eventtime;
	    break;
	  }
	  else {
	    outevents.updateMeanQuality();
	    BadEvents.push( *eventtime, *event );
	  }
	}
	EventPending = false;
      }
      if ( *(Index+1) < *Index ) {
	Dir = -1;               // falling
      }
    }
    // falling:
    else if ( Dir < 0 ) {
      if ( *(Index+1) > *Index ) {
	Dir = 1;                // rising
	if ( *Index < threshold ) {
	  if ( ! EventPending ||
	       MinIndex < first || *Index > *MinIndex ) {
	    MinIndex = Index;
	    MinTime = IndexTime;
	    EventPending = true;
	  }
	}
	else
	  BadEvents.push( *IndexTime, *Index );  // local sub-threshold minimum
      }
    }
    // don't know:
    else {
      if ( *(Index+1) < *Index )
	Dir = -1;               // falling
      else if ( *(Index+1) > *Index )
	Dir = 1;                // rising
    }

  }

}


template < typename DataIter, typename TimeIter >
int AcceptEvent<DataIter,TimeIter>::checkEvent( DataIter first,
						DataIter last,
						DataIter event,
						TimeIter eventtime,
						DataIter index,
						TimeIter indextime,
						DataIter prevevent,
						TimeIter prevtime,
						EventData &outevents,
						double &threshold,
						double &minthresh, 
						double &maxthresh,
						double &time,
						double &size,
						double &width )
{ 
  time = *eventtime;
  size = *event;
  width = 0.0;
  return 1; 
}


template < typename DataIter, typename TimeIter >
int AcceptEvent<DataIter,TimeIter>::checkPeak( DataIter first,
					       DataIter last,
					       DataIter event,
					       TimeIter eventtime,
					       DataIter index,
					       TimeIter indextime,
					       DataIter prevevent,
					       TimeIter prevtime,
					       EventList &outevents,
					       double &threshold,
					       double &minthresh,
					       double &maxthresh,
					       double &time,
					       double &size,
					       double &width )
{ 
  time = *eventtime;
  size = *event;
  width = 0.0;
  return 1; 
}


template < typename DataIter, typename TimeIter >
int AcceptEvent<DataIter,TimeIter>::checkTrough( DataIter first,
						 DataIter last,
						 DataIter event,
						 TimeIter eventtime,
						 DataIter index,
						 TimeIter indextime,
						 DataIter prevevent,
						 TimeIter prevtime,
						 EventList &outevents,
						 double &threshold,
						 double &minthresh,
						 double &maxthresh,
						 double &time,
						 double &size,
						 double &width )
{ 
  time = *eventtime;
  size = *event;
  width = 0.0;
  return 1; 
}



}; /* namespace relacs */

#endif /* ! _RELACS_DETECTOR_H_ */
