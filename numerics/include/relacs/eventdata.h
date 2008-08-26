/*
  eventdata.h
  Class for event times that can also be an infinite ring-buffer.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _RELACS_EVENTDATA_H_
#define _RELACS_EVENTDATA_H_ 1

#include <vector>
#include <iterator>
#include <cassert>
#include <iostream>
#include <relacs/array.h>
#include <relacs/random.h>
#include <relacs/linearrange.h>

using namespace std;

namespace relacs {

template < typename T > class Map;
typedef Map< double > MapD;

template < typename T > class SampleData;
typedef SampleData< double > SampleDataD;

class Kernel;


/*! 
\class EventData
\author Jan Benda
\version 2.1
\brief Class for event times that can also be an infinite ring-buffer.
\bug why does spikes.erase( spikes.begin() ) not compile?

\todo check equal size of input SampleDataD in e.g. rate, etc.
*/

  /*! Flag for the mode() of EventData, 
      indicating that the events are stimulus times. */
static const int StimulusEventMode = 0x0001;
  /*! Flag for the mode() of EventData, indicating that the events 
      are restart times of the data acquisition. */
static const int RestartEventMode = 0x0002;

class EventIterator;

class EventData
{

  friend class EventIterator;

public:

    /*! Constructs an empty EventData 
        with a stepsize() of 0.0001 seconds (0.1 ms)
	and a non-cyclic buffer. */
  EventData( void );
    /*! Constructs an EventData that can hold (has a capacity() of) \a n events
        with a stepsize() of 0.0001 seconds (0.1 ms)
	and a non-cyclic buffer.
        If \a sizebuffer is true,
	an additional buffer for the sizes of events is created.
        If \a widthbuffer is true,
	an additional buffer for the widths of events is created. */
  EventData( int n, bool sizebuffer=false, bool widthbuffer=false );
    /*! Constructs an EventData that can hold (has a capacity() of) \a n events
        beginning at time \a tbegin, ending at time \a tend,
	and with stepsize \a stepsize
	and a non-cyclic buffer.
        If \a sizebuffer is true,
	an additional buffer for the sizes of events is created.
        If \a widthbuffer is true,
	an additional buffer for the widths of events is created. */
  EventData( int n, double tbegin, double tend, double stepsize=0.0001,
	     bool sizebuffer=false, bool widthbuffer=false );
    /*! Copy constructor. 
        Copy the entire event list of \a events to \a *this. */
  EventData( const EventData &events );
    /*! Copy from \a events all events between \a tbegin and
        \a tend seconds to \a *this.
	In the copy, all event times and the signalTime() 
	are set relative to time \a tbegin. */
  EventData( const EventData &events, double tbegin, double tend );
    /*! Copy from \a events all event times between time 
        \a tbegin and time \a tend seconds.
	In the copy, all event times and the signalTime() 
	are set relative to time \a tref. */
  EventData( const EventData &events, double tbegin, double tend,
	     double tref );
    /*! Copy the event times \a times to \a *this
        and initializes the range with \a tbegin, \a tend, and \a stepsize.
        By default \a tbegin and \a tend are set to the first and last event
	of \a times. */
  EventData( const ArrayD &times, double tbegin=-HUGE_VAL,
	     double tend=HUGE_VAL, double stepsize=0.0001 );
    /*! Destructs an EventData. */
  ~EventData( void );

    /*! Number of events. 
        In cyclic() mode the returned number can be larger than the capacity()! */
  long size( void ) const;
    /*! True if there are no events. */
  bool empty( void ) const;
    /*! Resize the content of the buffers to \a nevents.
        If \a nevents equals zero, clear() is called.
	Otherwise the buffer content is only resized
	if the buffer is not cyclic.
	If a larger size is requested, new event data elements are
	initialized with \a dflt.
	New size data and width data are initialized with zero.
	\warning The capacity of the event buffer is enlarged only.
        It is never shrinked. 
	Use free() to control the buffer size.
	The range() is not changed.
	\sa clear(), size(), empty(), reserve(), free(), capacity() */ 
  void resize( long nevents, double dflt=0.0 );
    /*! Clear the buffer content.
	The range() is not changed.
        The capacity of the event buffer is not changed. */
  void clear( void );

    /*! Maximum number of elements the event buffer can hold. */
  long capacity( void ) const;
    /*! If \a n is less than or equal to capacity(),
        this call has no effect.
	Otherwise, it is a request for allocation 
	of additional memory to hold \a n events altogether. 
        If \a useSizeBuffer() is true,
	an additional buffer for the sizes of events is created.
        If \a useWidthBuffer() is true,
	an additional buffer for the widths of events is created.
	The first size() or capacity() elements of the event, size, and width buffers
	are preserved whichever is smaller.
	The remaining event data elements are initialized with \a dflt.
	New size data and width data are initialized with zero.
	The range() is not changed.
        \sa free(), capacity(), resize(), clear(), size(), empty() */
  void reserve( long n, double dflt=0.0 );
    /*! In contrast to the reserve() function, this function
        frees or allocates memory, such that capacity()
	equals exactly \a n. 
        If \a useSizeBuffer() is true,
	an additional buffer for the sizes of events is created.
        If \a useWidthBuffer() is true,
	an additional buffer for the widths of events is created.
	The first size() or capacity() elements of the event, size, and width buffers
	are preserved whichever is smaller.
	The remaining event data elements are initialized with \a dflt.
	New size data and width data are initialized with zero.
	The range() is not changed.
        \sa reserve(), capacity(), resize(), clear(), size(), empty() */
  void free( long n=0, double dflt=0.0 );

    /*! True if events are stored in a cyclic buffer. */
  bool cyclic( void ) const;
    /*! Set buffer to cyclic mode (\a cyclic = \c true)
        or non-cyclic mode (\a cyclic = \c false). */
  void setCyclic( bool cyclic=true );

    /*! True if an additional buffer for the sizes of the events exist. */
  bool sizeBuffer( void ) const;
    /*! True if an additional buffer for the sizes of the events exist 
        or is going to be used. */
  bool useSizeBuffer( void ) const;
    /*! If \a use is set to true, the next call of reserve will
        create an additional buffer for event sizes. */
  void setSizeBuffer( bool use=true );

    /*! True if an additional buffer for the widths of the events exist. */
  bool widthBuffer( void ) const;
    /*! True if an additional buffer for the widths of the events exist
        or is going to be used. */
  bool useWidthBuffer( void ) const;
    /*! If \a use is set to true, the next call of reserve will
        create an additional buffer for event widths. */
  void setWidthBuffer( bool use=true );

    /*! The range that contains the events. */
  LinearRange &range( void );
    /*! The range that contains the events. */
  const LinearRange &range( void ) const;
    /*! The beginning of the range() containing the events. */
  double offset( void ) const;
    /*! Set the beginning of the range() containing the events to \a offset. */
  void setOffset( double offset );
    /*! The length of the range() containing the events. */
  double length( void ) const;
    /*! Set the length of the range() containing the events to \a duration. */
  void setLength( double duration );
    /*! The stepsize of the range() containing the events,
        i.e. the resolution of the event times. */
  double stepsize( void ) const;
    /*! Set the stepsize (resolution) of the range() containing the events
        to \a stepsize. */
  void setStepsize( double stepsize );
    /*! The beginning of the range() containing the events. Same as offset(). */
  double rangeFront( void ) const;
    /*! Set the offset of the range containing the events
        to \a front without changing the last range element. */
  void setRangeFront( double front );
    /*! Returns the last element of the range containing the events. */
  double rangeBack( void ) const;
    /*! Resize the range containing the events
        such that the last range element equals \a back. */
  void setRangeBack( double back );

    /*! Returns the time of the signal. */
  double signalTime( void ) const;
    /*! Set the time of the signal to \a s. */
  void setSignalTime( double s );

    /*! Copy \a events to \a *this. */
  EventData &operator=( const EventData &events );
    /*! Copy the event times \a times to \a *this. */
  EventData &operator=( const ArrayD &times );

    /*! Make \a *this a copy of \a events. */
  void assign( const EventData &events );
    /*! Copy from \a events all events between time \a tbegin
        and time \a tend seconds to \a *this.
	In the copy, all event times and the signalTime()
	are set relative to time \a tbegin. */
  void assign( const EventData &events, double tbegin, double tend );
    /*! Copy from \a events all event times between time \a tbegin
        and time \a tend seconds to \a *this.
	In the copy, all event times and the signalTime() 
	are set relative to time \a tref. */
  void assign( const EventData &events, double tbegin, 
	       double tend, double tref );
    /*! Copy the event times \a times to \a *this
        and initializes the range with \a tbegin, \a tend, and \a stepsize.
        On default \a tbegin and \a tend are set to the first and last event
	of \a times. */
  void assign( const ArrayD &times, double tbegin=-HUGE_VAL,
	       double tend=HUGE_VAL, double stepsize=0.0001 );

    /*! Copy event times between time \a tbegin
        and time \a tend seconds to \a events.
        The event times in \a events are set relative to \a tbegin.
        Specify in \a nevents the size of the buffer where \a events points to.
        After executing this function \a nevents is the number
        of events copied to \a events. */
  void copy( double tbegin, double tend,
	     double *events, long &nevents ) const;
    /*! Copy event times between time \a tbegin
        and time \a tend seconds to \a events.
        The event times in \a events are set relative to \a tref.
        Specify in \a nevents the size of the buffer where \a events points to.
        After executing this function \a nevents is the number
        of events copied to \a events. */
  void copy( double tbegin, double tend, double tref, 
	     double *events, long &nevents ) const;
    /*! Copy event times between time \a tbegin
        and time \a tend seconds to \a events.
        The event times in \a events are set relative to \a tbegin. */
  void copy( double tbegin, double tend,
	     vector<double> &events ) const;
    /*! Copy event times between time \a tbegin
        and time \a tend seconds to \a events.
        The event times in \a events are set relative to \a tref. */
  void copy( double tbegin, double tend, double tref, 
	     vector<double> &events ) const;
    /*! Copy event times between time \a tbegin
        and time \a tend seconds to \a events.
        The event times in \a events are set relative to \a tbegin. */
  void copy( double tbegin, double tend, ArrayD &events ) const;
    /*! Copy event times between time \a tbegin
        and time \a tend seconds to \a events.
        The event times in \a events are set relative to \a tref. */
  void copy( double tbegin, double tend, double tref, 
	     ArrayD &events ) const;
    /*! Copy event times between time \a tbegin
        and time \a tend seconds to \a events.
        The event times in \a events are set relative to \a tbegin. */
  void copy( double tbegin, double tend, EventData &events ) const;
    /*! Copy event times between time \a tbegin
        and time \a tend seconds to \a events.
        The event times in \a events are set relative to \a tref. */
  void copy( double tbegin, double tend, double tref, 
	     EventData &events ) const;

    /*! Get the time of the \a i -th event in seconds.
        No range checking is performed. */
  inline double operator[] ( long i ) const;
    /*! Get a reference to the time of the \a i -th event in seconds.
        No range checking is performed. */
  inline double &operator[] ( long i );

    /*! Get the time of the \a i -th event in seconds.
	If an invalid index is specified, \c -HUGE_VAL is returned. */
  inline double at( long i ) const;
    /*! Get a reference to the time of the \a i -th event in seconds.
	If an invalid index is specified, 
	a reference to a dummy variable set to \c -HUGE_VAL is returned. */
  inline double &at( long i );

    /*! Get the time of the first accessible event in seconds. */
  double front( void ) const;
    /*! Get a reference to the time of the first accessible event in seconds. */
  double &front( void );
    /*! Get the time of the first plus \a n accessible event in seconds. */
  double front( long n ) const;
    /*! Get a reference to the time of the first plus \a n accessible event in seconds. */
  double &front( long n );

    /*! Get the time of the last event in seconds. */
  double back( void ) const;
    /*! Get a reference to the time of the last event in seconds. */
  double &back( void );
    /*! Get the time of the last minus \a n event in seconds. */
  double back( long n ) const;
    /*! Get a reference to the time of the last minus \a n event in seconds. */
  double &back( long n );

    /*! Const Iterator used to iterate through an EventData. */
  typedef EventIterator const_iterator;
    /*! Returns an iterator pointing to the first element of the EventData. */
  const_iterator begin( void ) const;
    /*! Returns an iterator pointing to the first element of the EventData
        following time \a time seconds. */
  const_iterator begin( double time ) const;
    /*! Returns an iterator pointing behind the last element of the EventData. */
  const_iterator end( void ) const;

    /*! Get the size of the \a i -th element of the event buffer.
	If an invalid index is specified or the sizes of events
	are not stored, \c -HUGE_VAL is returned. */
  inline double eventSize( long i ) const;
    /*! Get a reference to the size of the \a i -th element of the event buffer.
	If an invalid index is specified or the sizes of events
	are not stored, 
	a reference to a dummy variable set to \c -HUGE_VAL is returned. */
  inline double &eventSize( long i );

    /*! Get the size of the first accessible event. */
  double frontSize( void ) const;
    /*! Get a reference to the size of the first accessible event. */
  double &frontSize( void );
    /*! Get the size of the first plus \a n accessible event. */
  double frontSize( long n ) const;
    /*! Get a reference to the size of the first plus \a n accessible event. */
  double &frontSize( long n );

    /*! Get the size of the last event. */
  double backSize( void ) const;
    /*! Get a reference to the size of the last event. */
  double &backSize( void );
    /*! Get the size of the last minus \a n event. */
  double backSize( long n ) const;
    /*! Get a reference to the size of the last minus \a n event. */
  double &backSize( long n );

    /*! Get the width of the \a i -th element of the event buffer.
	If an invalid index is specified or the widths of events
	are not stored, \c -HUGE_VAL is returned. */
  inline double eventWidth( long i ) const;
    /*! Get a reference to the width of the \a i -th element of the event buffer.
	If an invalid index is specified or the widths of events
	are not stored, 
	a reference to a dummy variable set to \c -HUGE_VAL is returned. */
  inline double &eventWidth( long i );

    /*! Get the width of the first accessible event. */
  double frontWidth( void ) const;
    /*! Get a reference to the width of the first accessible event. */
  double &frontWidth( void );
    /*! Get the width of the first plus \a n accessible event. */
  double frontWidth( long n ) const;
    /*! Get a reference to the width of the first plus \a n accessible event. */
  double &frontWidth( long n );

    /*! Get the width of the last event. */
  double backWidth( void ) const;
    /*! Get a reference to the width of the last event. */
  double &backWidth( void );
    /*! Get the width of the last minus \a n event. */
  double backWidth( long n ) const;
    /*! Get a reference to the width of the last minus \a n event. */
  double &backWidth( long n );

    /*! Add a new event which occured at time \a time (seconds) and has the
        size \a size and width \a width to the buffer.
        The mean size and the mean width are updated using \a size and \a width.
        Mean rate it updated to. */
  void push( double time, double size=1.0, double width=0.0 );
    /*! Add new events which occured at times \a time (seconds) and have the
        size \a size and width \a width to the buffer.
        The mean size and the mean width are updated using \a size and \a width.
        Mean rate it updated to. */
  void push( const ArrayD &time, double size=1.0, double width=0.0 );

    /*! Insert a new event which occured at time \a time (seconds) and has the
        size \a size and width \a width to the buffer.
	The EventData must be non-cyclic (see cyclic(), setCyclic() ).
        The mean size, width, and rate are NOT updated. */
  void insert( double time, double size=0.0, double width=0.0 );
    /*! Insert the event times of \a e. */
  void insert( const EventData &e );
    /*! Erase event at index \a index.
	The EventData must be non-cyclic (see cyclic(), setCyclic() ). */
  void erase( int index );
    /*! Erase the event where \a iter points to.
	The EventData must be non-cyclic (see cyclic(), setCyclic() ). */
  void erase( const_iterator iter );
    /*! Erases the last event. */
  void pop( void );

    /*! Return in \a all the event times merged (summed up) 
        with the ones of \a e. */
  void sum( const EventData &e, EventData &all ) const;
    /*! For each time bin of width \a bin 
        add the time of the time bin to \a s 
	if an event is contained in both \a e and *this. */
  void sync( const EventData &e, EventData &s, double bin ) const;

    /*! Add \a x to all event times, signalTime() and the range(). */
  const EventData &operator+=( double x );
    /*! Subtract \a x from all event times, signalTime() and the range(). */
  const EventData &operator-=( double x );
    /*! Multiply all event times, signalTime() and the range() by \a x. */
  const EventData &operator*=( double x );
    /*! Divide all event times, signalTime() and the range() by \a x. */
  const EventData &operator/=( double x );

    /*! Insert the event times of \a e. \sa insert() */
  const EventData &operator+=( const EventData &e ) { insert( e ); return *this; };
    /*! Merge (sum up) the events of the two EventData. \sa sum() */
  EventData operator+( const EventData &e ) const { EventData x; sum( e, x ); return x; };
    /*! Return the times of the time bins of width stepsize()
        that contain at least an event in each of \a e and *this.
	\sa sync() */
  EventData operator*( const EventData &e ) const { EventData x; sync( e, x, stepsize() ); return x; };

    /*! Check whether values of members are ok. */
  bool check( void ) const;
    /*! Write all EventData variables to stream \a str (for debugging only). */
  friend ostream &operator<< ( ostream &str, const EventData &events );
    /*! If check() has detected an error, message() returns the reason. */
  string message( void ) const;

    /*! Number of events. Same as size(). */
  long currentEvent( void ) const;
    /*! The smallest possible event index that can be accesed to return an event. 
        In a non-cyclic buffer this is always 0. */
  long minEvent( void ) const;
    /*! The time of the first event that can be accesed. */
  double minTime( void ) const;

    /*! Mode of the event data.
        The mode is just a number, which can be used to label the traces. */
  int mode( void ) const;
    /*! Set mode to \a mode.
        The mode \a mode is just a number, 
	which can be used to label the event data. */
  void setMode( int mode );

    /*! Returns 1 if the events were extracted from an InData, 
        2 if the events were extracted from other events. */ 
  int source( void ) const;
    /*! Set the source of the events to \a source. */
  void setSource( int source );

    /*! The identifier string of the events. */
  const string &ident( void ) const;
    /*! Set the identifier of the events to \a ident. */
  void setIdent( const string &ident );

    /*! Minimum size of events between time \a tbegin and time \a tend seconds.
        If there are no events within this time interval or
        if the event sizes of the events are not stored 
	then the current value of the running average of the event sizes
	is returned. */
  double minSize( double tbegin, double tend ) const;
    /*! Maximum size of events between time \a tbegin and time \a tend seconds.
        If there are no events within this time interval or
        if the event sizes of the events are not stored 
	then the current value of the running average of the event sizes
	is returned. */
  double maxSize( double tbegin, double tend ) const;
    /*! Returns in \a min and \a max the minimum and maximum
        size of events between time \a tbegin and time \a tend seconds,
	respectively.
        If there are no events within this time interval or
        if the event sizes of the events are not stored 
	then \a min and \a max are set to the current value
	of the running average of the event sizes. */
  void minMaxSize( double tbegin, double tend, double &min, double &max ) const;
    /*! Mean size of events and standard deviation \a stdev
        between time \a tbegin and time \a tend seconds.
        If there are no events within this time interval zero is returned.
        If the event sizes of the events are not stored 
	then the current value of the running average of the event sizes
	is returned. */
  double meanSize( double tbegin, double tend, double &stdev ) const;
    /*! Mean size of events between time \a tbegin and time \a tend seconds.
        If there are no events within this time interval zero is returned.
        If the event sizes of the events are not stored 
	then the current value of the running average of the event sizes
	is returned. */
  double meanSize( double tbegin, double tend ) const;
    /*! Mean size of events since \a time seconds until the last event.
        If there are no events within this time interval zero is returned.
        If the event sizes of the events are not stored 
	then the current value of the running average of the event sizes
	is returned. */
  double meanSize( double time ) const { return meanSize( time, back() - time ); };
    /*! Mean size of detected events. */
  double meanSize( void ) const { return MeanSize; }
    /*! Set mean event size to \a meansize. */
  void setMeanSize( double meansize ) { MeanSize = meansize; }
    /*! Update the mean size with \a n times \a size. */
  void updateMeanSize( int n=1, double size=0.0 );
    /*! Compute a histogram \a hist over the event sizes
        between time \a tbegin and time \a tend seconds. */
  void sizeHist( double tbegin, double tend, SampleDataD &hist ) const;

    /*! Minimum width of events between time \a tbegin and time \a tend seconds.
        If there are no events within this time interval or
        if the event widths of the events are not stored 
	then the current value of the running average of the event widths
	is returned. */
  double minWidth( double tbegin, double tend ) const;
    /*! Maximum width of events between time \a tbegin and time \a tend seconds.
        If there are no events within this time interval or
        if the event widths of the events are not stored 
	then the current value of the running average of the event widths
	is returned. */
  double maxWidth( double tbegin, double tend ) const;
    /*! Returns in \a min and \a max the minimum and maximum
        width of events between time \a tbegin and time \a tend seconds,
	respectively.
        If there are no events within this time interval or
        if the event widths of the events are not stored 
	then \a min and \a max are set to the current value
	of the running average of the event widths. */
  void minMaxWidth( double tbegin, double tend, double &min, double &max ) const;
    /*! Mean width of events between time \a tbegin and time \a tend seconds.
        If there are no events within this time interval zero is returned.
        If the event widths of the events are not stored 
	then the current value of the running average of the event widths
	is returned. */
  double meanWidth( double tbegin, double tend ) const;
    /*! Mean width of the recent detected events in seconds. */
  double meanWidth( void ) const { return MeanWidth; };
    /*! Set event width to \a meanwidth seconds. */
  void setMeanWidth( double meanwidth ) { MeanWidth = meanwidth; }
    /*! Update the mean width with \a n times \a width seconds. */
  void updateMeanWidth( int n=1, double width=0.0 );
    /*! Compute a histogram \a hist over the event widths
        between time \a tbegin and time \a tend seconds. */
  void widthHist( double tbegin, double tend, SampleDataD &hist ) const;

    /*! Mean rate of detected events in Hertz. */
  double meanRate( void ) const;
    /*! Set mean event rate to \a meanrate Hertz. */
  void setMeanRate( double meanrate );
    /*! Update the mean rate with \a n times \a rate Hertz. */
  void updateMeanRate( int n=1, double rate=0.0 );

    /*! Update mean size, width, and rate
        with \a n times \a size, \a width, and \a rate, respectively. */
  void updateMean( int n=1, double size=0.0, double width=0.0, double rate=0.0 );

    /*! Mean quality of event detection. */
  double meanQuality( void ) const;
    /*! Set mean quality of event detection to \a meanquality. */
  void setMeanQuality( double meanquality );
    /*! Update the mean quality with \a good. */
  void updateMeanQuality( bool good=false );

    /*! Return the ratio value which is used to update the mean values. */
  double meanRatio( void ) const;
    /*! Set the ratio value which is used to update the mean values to \a ratio.
        The mean values are updated according to 
        nm = om*(1-r) + nv*r
        where the old and the new value of the mean are om and nm, respectively,
        the new value is nv and r is the mean ratio. 
        N = 1/r is the number of new values which are needed for the mean value
        to adapt to a new value (in the sense of a time constant of an
	exponential decay). */
  void setMeanRatio( double ratio );

    /*! Returns index of event following or equal to time \a time in seconds.
        Returns \a size() if no event is found. 
        Uses a fast bisecting method. */
  long next( double time ) const;
    /*! Returns time of event following or equal to time \a time in seconds.
        Returns \a dflt if no event is found. */
  double nextTime( double time, double dflt=-HUGE_VAL ) const;

    /*! Returns index to event preceeding or equal to time \a time in seconds.
        Returns -1 if no event is found.
        Uses a fast bisecting method. */
  long previous( double time ) const;
    /*! Returns time to event preceeding or equal to time \a time in seconds.
        Returns \a dflt if no event is found. */
  double previousTime( double time, double dflt=-HUGE_VAL ) const;

    /*! True if an event is within time \a time
        plus or minus \a distance seconds. */
  bool within( double time, double distance ) const;

    /*! Count events since time \a tbegin and time \a tend seconds. */
  long count( double tbegin, double tend ) const;
    /*! Count all events since time \a time (seconds). */
  long count( double time ) const;        

    /*! Mean event rate (Hz) as the number of events between time \a tbegin
        and time \a tend seconds divided by the width of the time window
	\a tend - \a tbegin. */
  double rate( double tbegin, double tend ) const;
    /*! Mean rate (Hz) of the last \a n events.
        If \a n < 1 or there are less than \a n events in the buffer
        zero is returned. */
  double rate( int n ) const;
    /*! Mean rate (Hz) of all events since time \a time (seconds). */
  double rate( double time ) const;        

    /*! The time course of the event rate is returned in \a rate.
	The rate is the number of events per bin.
	The width of the bins is given by \a width seconds.
        If \a width is less or equal to zero it is set to
	the stepsize of \a rate.
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void rate( SampleDataD &rate, double width=0.0, double time=0.0  ) const;
    /*! The time course of the event rate for the \a trial + 1 trial
        is added to \a rate.
	The rate is the number of events per bin.
	The width of the bins is given by \a width seconds.
        If \a width is less or equal to zero it is set to
	the stepsize of \a rate.
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void addRate( SampleDataD &rate, int &trial, double width=0.0,
		double time=0.0 ) const;

    /*! The time course of the cyclic event rate is returned in \a rate.
	The rate is the number of events per bin.
	The width of the bins is given by \a width seconds.
        If \a width is less or equal to zero it is set to
	the stepsize of \a rate.
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void cyclicRate( SampleDataD &rate, double width=0.0,
		   double time=0.0 ) const;
    /*! The time course of the cyclic event rate for the \a trial + 1 trial
        is added to \a rate.
	The rate is the number of events per bin.
	The width of the bins is given by \a width seconds.
        If \a width is less or equal to zero it is set to
	the stepsize of \a rate.
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void addCyclicRate( SampleDataD &rate, int &trial, double width=0.0,
		      double time=0.0 ) const;

    /*! The time course of the event rate is returned in \a rate.
	Each event is replaced by the \a kernel,
	which then are summed up.
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void rate( SampleDataD &rate, const Kernel &kernel, double time=0.0 ) const;
    /*! The time course of the event rate for the \a trial + 1 trial
        is added to \a rate.
	Each event is replaced by the \a kernel,
	which then are summed up.
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void addRate( SampleDataD &rate, int &trial, const Kernel &kernel,
		double time=0.0 ) const;

    /*! The time course of the cyclic event rate is returned in \a rate.
	Each event is replaced by the \a kernel,
	which then are summed up.
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void cyclicRate( SampleDataD &rate, const Kernel &kernel,
		   double time=0.0 ) const;
    /*! The time course of the cyclic event rate for the \a trial + 1 trial
        is added to \a rate.
	Each event is replaced by the \a kernel,
	which then are summed up.
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void addCyclicRate( SampleDataD &rate, int &trial, const Kernel &kernel,
		      double time=0.0 ) const;

    /*! Mean event interval (seconds) of all event intervals
        between time \a tbegin and time \a tend.
        In \a sd the standard deviation of the intervals is returned
	provided \a sd does not equal zero. */
  double interval( double tbegin, double tend, double *sd=0 ) const;
    /*! Mean event interval (seconds) of all event intervals
        since time \a time (seconds).
        In \a sd the standard deviation of the intervals is returned
	provided \a sd does not equal zero. */
  double interval( double time, double *sd=0 ) const;
    /*! Mean interval (seconds) of the last \a n events.
        If \a n < 1 or there are less than \a n events in the buffer
        zero is returned.
        In \a sd the standard deviation of the intervals is returned
	provided \a sd does not equal zero. */
  double interval( int n, double *sd ) const;
    /*! Interval (seconds) of the event interval at time \a time (seconds). */
  double intervalAt( double time ) const; 

    /*! The time course of the event intervals
        is returned in \a intervals with \a intervals.stepsize() in seconds.
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void interval( SampleDataD &intervals, double time=0.0 ) const;
    /*! The time course of the event intervals
        for the \a trial + 1 trial is added to \a intervals with
	\a intervals.stepsize() in seconds.
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void addInterval( SampleDataD &intervals, int &trial,
		    double time=0.0  ) const;

    /*! The time course of the cyclic event intervals
        is returned in \a intervals with \a intervals.stepsize() in seconds.
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void cyclicInterval( SampleDataD &intervals, double time=0.0 ) const;
    /*! The time course of the cyclic event intervals
        for the \a trial + 1 trial is added to \a intervals with
	\a intervals.stepsize() in seconds.
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void addCyclicInterval( SampleDataD &intervals, int &trial,
			  double time=0.0 ) const;

    /*! Returns in \a intrvls.x() the position of each interevent interval
        between \a tbegin and \a tend, and in \a intrvls.y() the
	interevent interval.
        The position of the interevent interval is the position of the left
	event (\pos = -1, default), the position of the right event 
	(\a pos = 1), or in between the left and the right event
	(\a pos = 0).
        \return the number of interevent intervals. */
  int intervals( double tbegin, double tend, MapD &intrvls, int pos=-1 ) const;
    /*! Adds to \a intrvls.x() the position of each interevent interval
        between \a tbegin and \a tend, and to \a intrvls.y() the
	interevent interval.
        The position of the interevent interval is the position of the left
	event (\pos = -1, default), the position of the right event 
	(\a pos = 1), or in between the left and the right event
	(\a pos = 0).
        \return the number of interevent intervals. */
  int addIntervals( double tbegin, double tend, MapD &intrvls, int pos=-1 ) const;
    /*! Write into stream \a os the position of each interevent interval
        between \a tbegin and \a tend multiplied by \a tfac in the first column,
	and the interevent interval in the second column.
        The position of the interevent interval is the position of the left
	event (\pos = -1, default), the position of the right event 
	(\a pos = 1), or in between the left and the right event
	(\a pos = 0).
	Both the position and the intervals are
	formatted as specified by \a width, \a prec, and \a frmt.
        If there aren't any intervals and \a noevents isn't empty, than
        \a noevents is printed as the only output once in each column.
        \return the number of interevent intervals. */
  int saveIntervals( double tbegin, double tend, ostream &os, int pos=-1,
		     double tfac=1.0, int width=0, int prec=5,
		     char frmt='g', const string &noevents="" ) const;

    /*! Mean event frequency (Hz) as the inverse of the mean event interval
        of all event intervals between time \a tbegin and
	time \a tend seconds. 
        In \a sd the standard deviation in Hz as the standard deviation
	of the intervals devided by the squared mean interval is returned
	provided \a sd does not equal zero. */
  double frequency( double tbegin, double tend, double *sd=0 ) const;
    /*! Mean event frequency (Hz) as the inverse of 
        the mean event interval (seconds) of all event intervals
        since time \a time (seconds).
        In \a sd the standard deviation in Hz as the standard deviation
	of the intervals devided by the squared mean interval is returned
	provided \a sd does not equal zero. */
  double frequency( double time, double *sd=0 ) const;
    /*! Mean event frequency (Hz) as the inverse of 
        the mean event interval (seconds) of the \a n recent events.
        If \a n < 1 or there are less than \a n events in the buffer
        zero is returned.
        In \a sd the standard deviation in Hz as the standard deviation
	of the intervals devided by the squared mean interval is returned
	provided \a sd does not equal zero. */
  double frequency( int n, double *sd=0 ) const;
    /*! Frequency (Hz) as the inverse of the event interval 
        at time \a time (seconds). */
  double frequencyAt( double time ) const;

    /*! The time course of the instantaneous frequency 1/ISI
        is returned in \a rate with \a rate.stepsize() in seconds.
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void frequency( SampleDataD &rate, double time=0.0 ) const;
    /*! The time course of the instantaneous frequency 1/ISI
        for the \a trial + 1 trial is added to \a rate with
	\a rate.stepsize() in seconds.
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void addFrequency( SampleDataD &rate, int &trial, double time=0.0 ) const;
    /*! The time course of the instantaneous frequency 1/ISI
        for the \a trial + 1 trial is added to \a rate with
	\a rate.stepsize() in seconds.
	In \a period the updated mean interval is returned.
        It can be used to calculate the standard deviation
        according to sd = \a rate * sqrt( \a rate * \a period - 1 ).
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void addFrequency( SampleDataD &rate, SampleDataD &period,
		     int &trial, double time=0.0 ) const;

    /*! The time course of the cyclic instantaneous frequency 1/ISI
        is returned in \a rate with \a rate.stepsize() in seconds.
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void cyclicFrequency( SampleDataD &rate, double time=0.0 ) const;
    /*! The time course of the cyclic instantaneous frequency 1/ISI
        for the \a trial + 1 trial is added to \a rate with
	\a rate.stepsize() in seconds.
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void addCyclicFrequency( SampleDataD &rate, int &trial,
			   double time=0.0 ) const;
    /*! The time course of the cyclic instantaneous frequency 1/ISI
        for the \a trial + 1 trial is added to \a rate with
	\a rate.stepsize() in seconds.
	In \a period the updated mean interval is returned.
        It can be used to calculate the standard deviation
        according to sd = \a rate * sqrt( \a rate * \a period - 1 ).
	The events between \a rate.leftMargin() and \a rate.rightMargin()
	seconds relative to time \a time (seconds) are considered. */
  void addCyclicFrequency( SampleDataD &rate, SampleDataD &period, 
			   int &trial, double time=0.0 ) const;

    /*! Returns in \a freqs.x() the position of each interevent interval
        between \a tbegin and \a tend, and in \a freqs.y() 1 divided
        by that interevent interval (the frequency).
        The position of the interevent interval is the position of the left
	event (\pos = -1, default), the position of the right event 
	(\a pos = 1), or in between the left and the right event
	(\a pos = 0).
        \return the number of interevent intervals. */
  int frequencies( double tbegin, double tend, MapD &freqs, int pos=-1 ) const;
    /*! Adds to \a freqs.x() the position of each interevent interval
        between \a tbegin and \a tend, and to \a freqs.y() 1 divided
        by that interevent interval (the frequency).
        The position of the interevent interval is the position of the left
	event (\pos = -1, default), the position of the right event 
	(\a pos = 1), or in between the left and the right event
	(\a pos = 0).
        \return the number of interevent intervals. */
  int addFrequencies( double tbegin, double tend, MapD &freqs, int pos=-1 ) const;
    /*! Write into stream \a os the position of each interevent interval
        between \a tbegin and \a tend multiplied by \a tfac in the first column,
	and 1 divided by that interevent interval (the frequency) in 
	the second column.
        The position of the interevent interval is the position of the left
	event (\pos = -1, default), the position of the right event 
	(\a pos = 1), or in between the left and the right event
	(\a pos = 0).
	Both the position and the frequency are
	formatted as specified by \a width, \a prec, and \a frmt.
        If there aren't any events and \a noevents isn't empty, than
        \a noevents is printed as the only output once in each column.
        \return the number of interevent intervals. */
  int saveFrequencies( double tbegin, double tend, ostream &os, int pos=-1,
		       double tfac=1.0, int width=0, int prec=5,
		       char frmt='g', const string &noevents="" ) const;

    /*! Compute interval histogram \a hist for the
        event intervals between time \a tbegin and 
	time \a tend seconds. Resolution and maximum interspike interval
        in \a hist are given in seconds. */
  void intervalHistogram( double tbegin, double tend,
			  SampleDataD &hist ) const;
    /*! Add event intervals between time \a tbegin and 
	time \a tend seconds to the interval
	histogram \a hist. Resolution and maximum interspike interval
        in \a hist are given in seconds. */
  void addIntervalHistogram( double tbegin, double tend,
			     SampleDataD &hist ) const;

    /*! Compute the serial correlation coefficients
        for lag zero to lag \a sc.size()-1
        of all event intervals between time \a tbegin 
	and time \a tend seconds. */
  void serialCorr( double tbegin, double tend, ArrayD &sc ) const;

    /*! Compute the fano factors for windows as defined by \a ff (in seconds)
        of all events between time \a tbegin and time \a tend seconds. */
  void fano( double tbegin, double tend, SampleDataD &ff ) const;

    /*! Compute the phase locking as the number of spikes per period \a period
        of all events between time \a tbegin and time \a tend seconds.
        \a tend is reduced to \a tbegin plus 
	the closest multiple of \a period. */
  double locking( double tbegin, double tend, double period ) const;
    /*! Compute the vector strength for a period \a period (seconds)
        of all events between time \a tbegin and time \a tend seconds
	according to 
        \f[ VS = \frac{1}{n} \sqrt{ \left( \sum_{i=1}^n \sin\varphi_i \right)^2 + \left( \sum_{i=1}^n \cos\varphi_i \right)^2} \f]
	with \f[ \varphi_i = 2 \pi ( t_i - time ) / period \f]
	for all \a n spike times \a t_i. */
  double vectorStrength( double tbegin, double tend, double period ) const;
    /*! Compute the vector phase for a period \a period (seconds)
        of all events between time \a tbegin and time \a tend seconds
	according to 
        \f[ VP = \mbox{atan} \frac{ \sum_{i=1}^n \sin\varphi_i }{ \sum_{i=1}^n \cos\varphi_i } \f]
	with \f[ \varphi_i = 2 \pi ( t_i - time ) / period \f]
	for all \a n spike times \a t_i.*/
  double vectorPhase( double tbegin, double tend, double period ) const;

    /*! Compute the event-triggered average \a ave of the trace \a trace
        for all \a n events \a t_i between time \a tbegin and time \a tend seconds:
	\f[ ave(t) = \frac{1}{n} \sum_{i=1}^n trace( t_i + t ) \f]. */
  void average( double tbegin, double tend, const SampleDataD &trace,
		SampleDataD &ave ) const;
    /*! Compute the event-triggered average \a ave and its
        standard deviation \a sd of the trace \a trace
        for all \a n events \a t_i between time \a tbegin and time \a tend seconds:
	\f[ ave(t) = \frac{1}{n} \sum_{i=1}^n trace( t_i + t ) \f]
	\f[ sd(t) = \sqrt{ \frac{1}{n} \sum_{i=1}^n \left( trace( t_i + t ) - ave(t)\right)^2} \f]. */
  void average( double tbegin, double tend, const SampleDataD &trace,
		SampleDataD &ave, SampleDataD &sd ) const;

    /*! Returns in \a psd the powerspectrum density of the events
	between \a tbegin and \a tend.
	The size of \a psd times \a step determines
	the width of the time windows used for the fourier transformations.
	The bin width for discretizing the events is set to \a step. */
  void spectrum( double tbegin, double tend, double step,
		 SampleDataD &psd ) const;

    /*! Returns in \a c the stimulus-response coherence between 
        \a stimulus and the events (the S-R coherence).
	The size of \a c times stimulus.stepsize() determines
	the width of the time windows used for the fourier transformations.
	Only events during the stimulus (between stimulus.rangeFront()
	and stimulus.rangeBack() ) are considered.
	The sampling interval of the stimulus (stimulus.stepsize())
	is used as the bin width for discretizing the events. */
  void coherence( const SampleDataD &stimulus, SampleDataD &c ) const;
    /*! Returns in \a c the coherence between 
        the events and the events in \a e
	(the response-response (R-R) coherence).
	The size of \a c times \a step determines
	the width of the time windows used for the fourier transformations.
	Only events during the \a tbegin and \a tend are considered.
	The bin width for discretizing the events is set to \a step. */
  void coherence( const EventData &e, double tbegin, double tend,
		  double step, SampleDataD &c ) const;

    /*! Latency of first event relative to \a time in seonds. 
        A negative number is returned, if there is no event. */
  double latency( double time ) const;

    /*! Generate a poisson spike train with rate \a rate Hertz
        and absoulte refractory period \a refract
        for duration \a duration seconds.
        Use the random number gnerator \a random. */
  void poisson( double rate, double refract, 
		double duration, RandomBase &random=rnd );

    /*! Write event times as a single column of text in stream \a s.
        Each event time is multiplied by \a tfac,
	formatted as specified by \a width, \a prec, and \a frmt,
	and is written in a separate line.
        If there aren't any events and \a noevents isn't empty, than
        \a noevents is printed as the only output. */
  void saveText( ostream &os, double tfac=1.0, int width=0, int prec=5,
		 char frmt='g', const string &noevents="" ) const;
    /*! Write event times as text in stream \a s.
        Two columns are written.
	The first column is the event time is multiplied by \a tfac, and is
	formatted as specified by \a width, \a prec, and \a frmt.
	The second column is an y-value as specified by \a y.
        If there aren't any events and \a noevents isn't empty, than
        a single line with \a noevents as the time of an event 
	followed by \a noy as the corresponding y-value
	is printed as the only output. */
  void savePoint( ostream &os, double y=0.0, double tfac=1.0, int width=0,
		  int prec=5, char frmt='g',
		  const string &noevents="", double noy=-1.0 ) const;
    /*! Write event times as text in stream \a s
        such that each event gets two data points.
	The y-value of the data points are set to 
	\a offs + \a lower and \a offs + \a upper.
        Each event time is multiplied by \a tfac, and is
	formatted as specified by \a width, \a prec, and \a frmt.
	Events are separated by a blank line.
        If there aren't any events and \a noevents isn't empty, than
        a single line with \a noevents as the time of an event 
	followed by \a noy as the corresponding y-value
	is printed as the only output. */
  void saveStroke( ostream &os, int offs=0, double tfac=1.0,
		   int width=0, int prec=5, char frmt='g',
		   double lower=0.1, double upper=0.9, 
		   const string &noevents="", double noy=-1.0 ) const;
    /*! Write event times as text in stream \a s
        such that each event gets four data points.
	Each such box has width \a bin and contains the corresponding event.
        The sides of the box are at multiples of \a bin.
	The y-values of the box are set to 
	\a offs + \a lower and \a offs + \a upper.
        All times are multiplied by \a tfac, and are
	formatted as specified by \a width, \a prec, and \a frmt.
	Events are separated by a blank line.
        If there aren't any events and \a noevents isn't empty, than
        a single line with \a noevents as the time of a event 
	followed by \a noy as the corresponding y-value
	is printed as the only output. */
  void saveBox( ostream &os, double bin, int offs=0, double tfac=1.0,
		int width=0, int prec=5, char frmt='g',
		double lower=0.1, double upper=0.9, 
		const string &noevents="", double noy=-1.0 ) const;


private:

    /*! Buffer for the times of events measured in seconds. */
  double *TimeBuffer;                  
    /*! Optional buffer for the sizes of events. */
  double *SizeBuffer;
    /*! True if the buffer for the event sizes should be used. */
  bool UseSizeBuffer;
    /*! Optional buffer for the widths of events. */
  double *WidthBuffer;                  
    /*! True if the buffer for the event widths should be used. */
  bool UseWidthBuffer;
    /*! Number of elements the buffers \a TimeBuffer,
        \a SizeBuffer, and \a WidthBuffer can hold. */
  long NBuffer;
    /*! Flag indicating whether EventData is in cyclic buffer mode. */
  bool Cyclic;
    /*! Current event index in \a Buffer of current window. */
  long R;
    /*! Index of buffer relative to first event. */
  long Index;
    /*! Number of completed buffer cycles. */
  long Cycles;
    /*! Mode. */
  int Mode;
    /*! An identifier for the events. */
  string Ident;
    /*! The source of the events: 0: DIO, 1: IData, 2: events. */
  int Source;

    /*! Determines how fast the mean values are updated. */
  double MeanRatio;
    /*! Mean event size. */
  double MeanSize;
    /*! Mean event width in seconds. */
  double MeanWidth;
    /*! Mean interval in seconds. */
  double MeanInterval;
  static const double MaxInterval;
    /*! Mean quality of event-detection. */
  double MeanQuality;

    /*! Error message set by check(). */
  mutable string ErrorMessage;

    /*! A dummy variable to return a value for invalid indices. */
  double Dummy;

  double SignalTime;

  LinearRange Range;

};


/*! 
  \class EventIterator
  \author Jan Benda
  \version 1.2
  \brief Iterator for EventData returning the event time.
*/
  
class EventIterator //: public iterator< random_access_iterator_tag, double, long > 
{
    
public:
    
    /*! Constructs an empty invalid iterator for an EventData. */
  EventIterator( void ) 
    : Index( 0 ), ED( 0 ) {};
    /*! Constructs an valid iterator for an EventData \a ed
        pointing to element \a index. */
  EventIterator( const EventData &ed, long index ) 
    : Index( index ), ED( &ed ) {};
    /*! Copy constructor. */
  EventIterator( const EventIterator &p )
    : Index( p.Index ), ED( p.ED ) {};
    /*! Destructor. */
  ~EventIterator( void ) {};
    
    /*! Assigns \a p to this. */
  EventIterator &operator=( const EventIterator &p );
    
    /*! Returns true if both iterators point to the same element 
        of the same instance of an EventData. */
  inline bool operator==( const EventIterator &p ) const
    { return ( ED == p.ED && Index == p.Index ); };
    /*! Returns true if the iterators do not point to the same element 
        of the same instance of an EventData. */
  inline bool operator!=( const EventIterator &p ) const
    { return ( ED != p.ED || Index != p.Index ); };
    /*! Returns true if \a this points to an element preceeding the 
        element where \a points to. */
  inline bool operator<( const EventIterator &p ) const
    { return ( ED == p.ED && Index < p.Index ); };
    /*! Returns true if \a this points to an element succeeding the 
        element where \a p points to. */
  inline bool operator>( const EventIterator &p ) const
    { return ( ED == p.ED && Index > p.Index ); };
    /*! Returns true if \a this points to an element preceeding 
        or equaling the element where this points to. */
  inline bool operator<=( const EventIterator &p ) const
    { return ( ED == p.ED && Index <= p.Index ); };
    /*! Returns true if \a this points to an element succeeding
        or preceeding the element where this points to. */
  inline bool operator>=( const EventIterator &p ) const
    { return ( ED == p.ED && Index >= p.Index ); };
    /*! Returns true if this is an valid iterator, i.e.
        it points to an existing element. */
  inline bool operator!( void ) const
    { return ( ED != 0 && Index >= ED->minEvent() && Index < ED->currentEvent() ); };
    
    /*! Increments the iterator to the next element. */
  inline const EventIterator &operator++( void )
    { Index++; return *this; };
    /*! Decrements the iterator to the previous element. */
  inline const EventIterator &operator--( void )
    { Index--; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const EventIterator &operator+=( unsigned int incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const EventIterator &operator+=( signed int incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const EventIterator &operator+=( unsigned long incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const EventIterator &operator+=( signed long incr )
    { Index += incr; return *this; };
    /*! Sets the iterator to the element following the time
        of the event where the iterator is currrently pointing to
        plus \a time seconds. */
  inline const EventIterator &operator+=( double time )
    { Index = ED->next( (*ED)[Index] + time ); return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const EventIterator &operator-=( unsigned int decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const EventIterator &operator-=( signed int decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const EventIterator &operator-=( unsigned long decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const EventIterator &operator-=( signed long decr )
    { Index -= decr; return *this; };
    /*! Sets the iterator to the element following the time
        of the event where the iterator is currrently pointing to
        minus \a time seconds. */
  inline const EventIterator &operator-=( double time )
    { Index = ED->next( (*ED)[Index] - time ); return *this; };
    /*! Adds \a incr to the iterator. */
  inline EventIterator operator+( unsigned int incr ) const
    { EventIterator p( *this ); p.Index += incr; return p; };
    /*! Adds \a incr to the iterator. */
  inline EventIterator operator+( signed int incr ) const
    { EventIterator p( *this ); p.Index += incr; return p; };
    /*! Adds \a incr to the iterator. */
  inline EventIterator operator+( unsigned long incr ) const
    { EventIterator p( *this ); p.Index += incr; return p; };
    /*! Adds \a incr to the iterator. */
  inline EventIterator operator+( signed long incr ) const
    { EventIterator p( *this ); p.Index += incr; return p; };
    /*! Returns an iterator pointing to the element following the time
        of the event where the iterator is currrently pointing to
        plus \a time seconds. */
  inline EventIterator operator+( double time ) const
    { EventIterator p( *this ); p.Index = ED->next( (*ED)[Index] + time ); return p; };
    /*! Subtracts \a decr from the iterator. */
  inline EventIterator operator-( unsigned int decr ) const
    { EventIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts \a decr from the iterator. */
  inline EventIterator operator-( signed int decr ) const
    { EventIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts \a decr from the iterator. */
  inline EventIterator operator-( unsigned long decr ) const
    { EventIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts \a decr from the iterator. */
  inline EventIterator operator-( signed long decr ) const
    { EventIterator p( *this ); p.Index -= decr; return p; };
    /*! Returns an iterator pointing to the element following the time
        of the event where the iterator is currrently pointing to
        minus \a time seconds. */
  inline EventIterator operator-( double time ) const
    { EventIterator p( *this ); p.Index = ED->next( (*ED)[Index] - time ); return p; };
    /*! Returns the number of elements between the two iterators. */
  inline int operator-( const EventIterator &p ) const
    { if ( ED == p.ED ) return Index - p.Index; return 0; };
    
    /*! Returns the time of the event where the iterator points to. */
  inline double operator*( void ) const
    { assert( ED != 0 ); return (*ED)[ Index ]; };
    /*! Returns the time of the event where the iterator + n points to. */
  inline double operator[]( long n ) const
    { assert( ED != 0 ); return (*ED)[ Index+n ]; };

    /*! Returns the index of the element where the iterator points to. */
  inline long index( void ) const { return Index; };
    /*! The time in seconds of the current event relative to time zero. */
  inline double time( void ) const
    { return ED->operator[]( Index ); };

    /*! The event data where the iterator points in. */
  inline const EventData &events( void ) const
    { return *ED; };
    
    
protected:

  long Index;    
  const EventData *ED;
    
};


/*! 
  \class EventFrequencyIterator
  \author Jan Benda
  \version 1.2
  \brief Input Iterator for EventData which returns the event frequency.
*/

class EventFrequencyIterator : public EventIterator
{
    
public:
    
    /*! Constructs an empty invalid iterator for an EventData. */
  EventFrequencyIterator( void ) 
    : EventIterator() {};
    /*! Constructs an valid iterator for an EventData \a ed
        pointing to element \a index. */
  EventFrequencyIterator( const EventData &ed, long index ) 
    : EventIterator( ed, index ) {};
    /*! Copy constructor. */
  EventFrequencyIterator( const EventFrequencyIterator &p )
    : EventIterator( p ) {};
    /*! Cast constructor. */
  EventFrequencyIterator( const EventIterator &p )
    : EventIterator( p ) {};
    /*! Destructor. */
  ~EventFrequencyIterator( void ) {};

    /*! Returns true if this is an valid iterator, i.e.
        it points to an existing element. */
  bool operator!( void ) const
    { return ( ED != 0 && Index >= ED->minEvent()+1 && Index < ED->currentEvent() ); };
    
    /*! Returns the frequency of the preceeding event interval. */
  inline double operator*( void ) const;
    /*! Returns the frequency of the event interval [*i+n-1, *i+n]. */
  inline double operator[]( long n ) const;
    
};


/*! 
  \class EventSizeIterator
  \author Jan Benda
  \version 1.2
  \brief Input Iterator for EventData which returns the event size.
*/

class EventSizeIterator : public EventIterator
{
    
public:
    
    /*! Constructs an empty invalid iterator for an EventData. */
  EventSizeIterator( void ) 
    : EventIterator() {};
    /*! Constructs an valid iterator for an EventData \a ed
        pointing to element \a index. */
  EventSizeIterator( const EventData &ed, long index ) 
    : EventIterator( ed, index ) {};
    /*! Copy constructor. */
  EventSizeIterator( const EventSizeIterator &p )
    : EventIterator( p ) {};
    /*! Cast constructor. */
  EventSizeIterator( const EventIterator &p )
    : EventIterator( p ) {};
    /*! Destructor. */
  ~EventSizeIterator( void ) {};
    
    /*! Returns the size of the event.
        If there is an extra buffer for event sizes,
	the size is taken from that buffer.
	Otherwise the value of the data element
	where the iterator points to is returned. */
  inline double operator*( void ) const;
    /*! Returns the size of the \a n th event.
        If there is an extra buffer for event sizes,
	the size is taken from that buffer.
	Otherwise the value of the data element
	where the iterator points to is returned. */
  inline double operator[]( long n ) const;
    
};


///////////////////// INLINE FUNCTIONS /////////////////////////////////////

inline double EventData::operator[] ( long i ) const
{ 
  i -= Index;

  if ( i < 0 ) {
    if ( Cyclic ) {
      i += NBuffer;
#ifndef NDEBUG
      if ( !( i >= R && i < NBuffer ) ) {
	cerr << "! error in " << Ident << "::operator[] const i=" << i << " < R = " << R << endl << *this << endl;
	assert( i >= R && i < NBuffer );
      }
#endif
    }
    else {
#ifndef NDEBUG
      cerr << "! error in " << Ident << "::operator[] const i=" << i << " < 0 " << endl << *this << endl;
      assert( i >= 0 );
#endif
    }
  }
#ifndef NDEBUG
  else {
    if ( !( i < R ) ) {
      cerr << "! error in " << Ident << "::operator[] const i=" << i << " >= R = " << R << endl << *this << endl;
      assert( i < R );
    }
  }
#endif

  return TimeBuffer[i];
}


inline double &EventData::operator[] ( long i )
{ 
  i -= Index;

  if ( i < 0 ) {
    if ( Cyclic ) {
      i += NBuffer;
#ifndef NDEBUG
      if ( !( i >= R && i < NBuffer ) ) {
	cerr << "! error in " << Ident << "::operator[] const i=" << i << " < R = " << R << endl << *this << endl;
	assert( i >= R && i < NBuffer );
      }
#endif
    }
    else {
#ifndef NDEBUG
      cerr << "! error in " << Ident << "::operator[] const i=" << i << " < 0 " << endl << *this << endl;
      assert( i >= 0 );
#endif
    }
  }
#ifndef NDEBUG
  else {
    if ( !( i < R ) ) {
      cerr << "! error in " << Ident << "::&operator[] i=" << i << " >= R = " << R << endl << *this << endl;
      assert( i < R );
    }
  }
#endif

  return TimeBuffer[i];
}


inline double EventData::at( long i ) const
{
  i -= Index;

  if ( i < 0 ) {
    if ( Cyclic ) {
      i += NBuffer;
      if ( !( i >= R && i < NBuffer ) ) {
#ifndef NDEBUG
	cerr << "! error in " << Ident << "::at() const i=" << i << " < R = " << R << endl << *this << endl;
	assert( i >= R && i < NBuffer );
#endif
	return -HUGE_VAL;
      }
    }
    else {
#ifndef NDEBUG
      cerr << "! error in " << Ident << "::at() const i=" << i << " < 0" << endl << *this << endl;
      assert( i >= 0 );
#endif
      return -HUGE_VAL;
    }
  }
  else {
    if ( !( i < R ) ) {
#ifndef NDEBUG
      cerr << "! error in " << Ident << "::at() const i=" << i << " >= R = " << R << endl << *this << endl;
      assert( i < R );
#endif
      return -HUGE_VAL;
    }
  }

  return TimeBuffer[i];
}


inline double &EventData::at( long i )
{
  i -= Index;

  if ( i < 0 ) {
    if ( Cyclic ) {
      i += NBuffer;
      if ( !( i >= R && i < NBuffer ) ) {
#ifndef NDEBUG
	cerr << "! error in " << Ident << "::at() const i=" << i << " < R = " << R << endl << *this << endl;
	assert( i >= R && i < NBuffer );
#endif
	Dummy = -HUGE_VAL;
	return Dummy;
      }
    }
    else {
#ifndef NDEBUG
      cerr << "! error in " << Ident << "::at() const i=" << i << " < 0" << endl << *this << endl;
      assert( i >= 0 );
#endif
      Dummy = -HUGE_VAL;
      return Dummy;
    }
  }
  else {
    if ( !( i < R ) ) {
#ifndef NDEBUG
      cerr << "! error in " << Ident << "::&at() i=" << i << " >= R = " << R << endl << *this << endl;
      assert( i < R );
#endif
      Dummy = -HUGE_VAL;
      return Dummy;
    }
  }

  return TimeBuffer[i];
}


inline double EventData::eventSize( long i ) const
{
  if ( SizeBuffer == 0 )
    return -HUGE_VAL;

  i -= Index;

  if ( i < 0 ) {
    if ( Cyclic ) {
      i += NBuffer;
      if ( !( i >= R && i < NBuffer ) ) {
#ifndef NDEBUG
	cerr << "! error in " << Ident << "::eventSize() const i=" << i << " < R = " << R << endl << *this << endl;
	assert( i >= R && i < NBuffer );
#endif
	return -HUGE_VAL;
      }
    }
    else {
#ifndef NDEBUG
      cerr << "! error in " << Ident << "::eventSize() const i=" << i << " < 0" << endl << *this << endl;
      assert( i >= 0 );
#endif
      return -HUGE_VAL;
    }
  }
  else {
    if ( !( i < R ) ) {
#ifndef NDEBUG
      cerr << "! error in " << Ident << "::eventSize() const i=" << i << " >= R = " << R << endl << *this << endl;
      assert( i < R );
#endif
      return -HUGE_VAL;
    }
  }

  return SizeBuffer[i];
}


inline double &EventData::eventSize( long i )
{
  if ( SizeBuffer == 0 ) {
    Dummy = -HUGE_VAL;
    return Dummy;
  }

  i -= Index;

  if ( i < 0 ) {
    if ( Cyclic ) {
      i += NBuffer;
      if ( !( i >= R && i < NBuffer ) ) {
#ifndef NDEBUG
	cerr << "! error in " << Ident << "::eventSize() const i=" << i << " < R = " << R << endl << *this << endl;
	assert( i >= R && i < NBuffer );
#endif
	Dummy = -HUGE_VAL;
	return Dummy;
      }
    }
    else {
#ifndef NDEBUG
      cerr << "! error in " << Ident << "::eventSize() const i=" << i << " < 0" << endl << *this << endl;
      assert( i >= 0 );
#endif
      Dummy = -HUGE_VAL;
      return Dummy;
    }
  }
  else {
    if ( !( i < R ) ) {
#ifndef NDEBUG
      cerr << "! error in " << Ident << "::&eventSize() i=" << i << " >= R = " << R << endl << *this << endl;
      assert( i < R );
#endif
      Dummy = -HUGE_VAL;
      return Dummy;
    }
  }

  return SizeBuffer[i];
}


inline double EventData::eventWidth( long i ) const
{
  if ( WidthBuffer == 0 )
    return -HUGE_VAL;

  i -= Index;

  if ( i < 0 ) {
    if ( Cyclic ) {
      i += NBuffer;
      if ( !( i >= R && i < NBuffer ) ) {
#ifndef NDEBUG
	cerr << "! error in " << Ident << "::eventWidth() const i=" << i << " < R = " << R << endl << *this << endl;
	assert( i >= R && i < NBuffer );
#endif
	return -HUGE_VAL;
      }
    }
    else {
#ifndef NDEBUG
      cerr << "! error in " << Ident << "::eventWidth() const i=" << i << " < 0" << endl << *this << endl;
      assert( i >= 0 );
#endif
      return -HUGE_VAL;
    }
  }
  else {
    if ( !( i < R ) ) {
#ifndef NDEBUG
      cerr << "! error in " << Ident << "::eventWidth() const i=" << i << " >= R = " << R << endl << *this << endl;
      assert( i < R );
#endif
      return -HUGE_VAL;
    }
  }

  return WidthBuffer[i];
}


inline double &EventData::eventWidth( long i )
{
  if ( WidthBuffer == 0 ) {
    Dummy = -HUGE_VAL;
    return Dummy;
  }

  i -= Index;

  if ( i < 0 ) {
    if ( Cyclic ) {
      i += NBuffer;
      if ( !( i >= R && i < NBuffer ) ) {
#ifndef NDEBUG
	cerr << "! error in " << Ident << "::eventWidth() const i=" << i << " < R = " << R << endl << *this << endl;
	assert( i >= R && i < NBuffer );
#endif
	Dummy = -HUGE_VAL;
	return Dummy;
      }
    }
    else {
#ifndef NDEBUG
      cerr << "! error in " << Ident << "::eventWidth() const i=" << i << " < 0" << endl << *this << endl;
      assert( i >= 0 );
#endif
      Dummy = -HUGE_VAL;
      return Dummy;
    }
  }
  else {
    if ( !( i < R ) ) {
#ifndef NDEBUG
      cerr << "! error in " << Ident << "::&eventWidth() i=" << i << " >= R = " << R << endl << *this << endl;
      assert( i < R );
#endif
      Dummy = -HUGE_VAL;
      return Dummy;
    }
  }

  return WidthBuffer[i];
}


inline double EventFrequencyIterator::operator*( void ) const
{ 
#ifndef NDEBUG
  if ( !( ED != 0 && Index > 0 ) ) {
    cerr << "! Error in EventFrequencyIterator::operator*() ED = " << ED
	 << ", Index = " << Index << endl;
    assert( ED != 0 && Index > 0 );
  }
#endif

  return 1.0/( (*ED)[Index] - (*ED)[Index-1] );
}


inline double EventFrequencyIterator::operator[]( long n ) const
{ 
#ifndef NDEBUG
  if ( !( ED != 0 && Index+n > 0 ) ) {
    cerr << "! Error in EventFrequencyIterator::operator*() ED = " << ED
	 << ", Index = " << Index
	 << ", n = " << n << endl;
    assert( ED != 0 && Index+n > 0 );
  }
#endif

  return 1.0/( (*ED)[Index+n] - (*ED)[Index+n-1] );
}


inline double EventSizeIterator::operator*( void ) const
{ 
#ifndef NDEBUG
  if ( !( ED != 0 && Index >= 0 ) ) {
    cerr << "! Error in EventSizeIterator::operator*() -> ED = " << ED
	 << ", Index = " << Index << endl;
    assert( ED != 0 && Index >= 0 );
  }
#endif

  if ( ED->sizeBuffer() )
    return ED->eventSize( Index );
  else
    return 1.0;
    //    return ED->value( ED->indices( (*ED)[ Index ] ) );
}


inline double EventSizeIterator::operator[]( long n ) const
{ 
#ifndef NDEBUG
  if ( !( ED != 0 && Index+n >= 0 ) ) {
    cerr << "! Error in EventSizeIterator::operator*() ED = " << ED
	 << ", Index = " << Index
	 << ", n = " << n << endl;
    assert( ED != 0 && Index+n >= 0 );
  }
#endif

  if ( ED->sizeBuffer() )
    return ED->eventSize( Index+n );
  else
    return 1.0;
    //    return ED->value( ED->indices( (*ED)[ Index+n ] ) );
}


}; /* namespace relacs */

#endif /* ! _RELACS_EVENTDATA_H_ */
