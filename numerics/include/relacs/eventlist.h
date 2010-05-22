/*
  eventlist.h
  A container for EventData

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

#ifndef _RELACS_EVENTLIST_H_
#define _RELACS_EVENTLIST_H_ 1

#include <cassert>
#include <vector>
#include <iostream>
#include <relacs/random.h>
#include <relacs/eventdata.h>

using namespace std;

namespace relacs {


class Kernel;
template < typename T > class Array;
typedef Array< double > ArrayD;
template < typename T > class Map;
typedef Map< double > MapD;
template < typename T > class SampleData;
typedef SampleData< double > SampleDataD;


/*!
\class EventList
\brief A container for EventData
\author Jan Benda
\version 1.1

Use push() or add() to add EventData to the list.

The range operator[] returns a reference to an EventData
specified either by index or identifier.

The index() function returns the index of an EventData specified
by its identifier.

\todo check equal size of input SampleDataD in e.g. rate, etc.
*/

class EventList
{

public:

    /*! Constructs an empty EventList. */
  EventList( void );
    /*! Copy constructor. 
        All EventData that are owened by \a el are copied,
        otherwise only pointers to EventData are copied. */
  EventList( const EventList &el );
    /*! Contruct an EventList with event times copied from each element
        of \a el between time \a tbegin and time \a tend seconds.
	In the copy, all event times and the signalTime() 
	are set relative to time \a tbegin. */
  EventList( const EventList &el, double tbegin, double tend );
    /*! Contruct an EventList with event times copied from each element
        of \a el between time \a tbegin and time \a tend seconds.
	In the copy, all event times and the signalTime() 
	are set relative to time \a tref. */
  EventList( const EventList &el, double tbegin, double tend, double tref );

    /*! Constructs an EventList containing the single EventData \a events. */
  EventList( const EventData &events );
    /*! Constructs an EventList containing the event times
        of \a events between time \a tbegin and time \a tend seconds.
	In the copy, all event times and the signalTime() 
	are set relative to time \a tbegin. */
  EventList( const EventData &events, double tbegin, double tend );
    /*! Constructs an EventList containing the event times
        of \a events between time \a tbegin and time \a tend seconds.
	In the copy, all event times and the signalTime() 
	are set relative to time \a tref. */
  EventList( const EventData &events, double tbegin, double tend, double tref );

    /*! For each event \a t in \a times copy all event times from \a events
        between the time of the event \a t and \a t plus \a tend seconds
	as a new element to the end of the EventList
	with the event times set relative to the time of the event \a t. */
  EventList( const EventData &events, const EventData &times, double tend );
    /*! For each event \a t in \a times copy all event times from \a events
        between the time of the event \a t plus \a tbegin seconds
	and \a t plus \a tend seconds
	as a new element to the end of the EventList
	with the event times set relative to the time of the event \a t. */
  EventList( const EventData &events, const EventData &times,
	     double tbegin, double tend );

    /*! Constructs an EventList containing the single pointer \a events.
        Setting \a own to \c true transfers the ownership to the EventList. */
  EventList( EventData *events, bool own=false );
    /*! Creates an EventList with \a n non-cyclic EventData, each of capacity \a m
        and additional buffer for the size and the width of events
	if \a sizebuffer or \a widthbuffer are \c true, respectively. */
  EventList( int n, int m=0, bool sizebuffer=false, bool widthbuffer=false );

    /*! Destructs an EventList. All EventData are deleted. */
  ~EventList( void );

    /*! The number of EventData in the EventList. */
  int size( void ) const;
    /*! True if there are no EventData contained in the EventList. */
  bool empty( void ) const;
    /*! Resize the EventList such that it contains \a n EventData.
        If \a n equals zero, clear() is called.
	If a larger size than the current size() is requested 
	than empty EventData are appended, each of capacity \a m
        and additional buffer for the size and the width of events
	if \a sizebuffer or \a widthbuffer are \c true, respectively.
	\sa clear(), size(), empty(), reserve(), capacity() */
  void resize( int n, int m=0, bool sizebuffer=false, bool widthbuffer=false );
    /*! Clear the EventList, i.e. remove all EventData the EventList owns. */
  void clear( void );

    /*! Maximum number of EventData the EventList can hold. */
  int capacity( void ) const;
    /*! Increase the capacity() to \a n.  */
  void reserve( int n );

    /*! Assignment. */
  EventList &operator=( const EventList &el );

    /*! Returns a const reference of the \a i -th EventData of the list. */
  inline const EventData &operator[]( int i ) const 
    { assert( i>=0 && i<(int)Events.size() ); return *Events[ i ]; };
    /*! Returns a reference of the \a i -th EventData of the list. */
  inline EventData &operator[]( int i ) 
    { assert( i>=0 && i<(int)Events.size() ); return *Events[ i ]; };

    /*! Returns a const reference to the first EventData in the list. */
  const EventData &front( void ) const;
    /*! Returns a reference to the first EventData in the list. */
  EventData &front( void );
    /*! Returns a const reference to the last EventData in the list. */
  const EventData &back( void ) const;
    /*! Returns a reference to the last EventData in the list. */
  EventData &back( void );

    /*! Returns a const reference of the EventData element with
        identifier \a ident.
        \warning No "range checking" is performed.
        If there is no EventData element with identifier \a ident
        a reference to the first element is returned. */
  const EventData &operator[]( const string &ident ) const;
    /*! Returns a reference of the EventData element with
        identifier \a ident.
        \warning No "range checking" is performed.
        If there is no EventData element with identifier \a ident
        a reference to the first element is returned. */
  EventData &operator[]( const string &ident );

    /*! Return the index of the event data trace with identifier \a ident.
        If there is no trace with this identifier -1 is returned. */
  int index( const string &ident ) const;

    /*! Copies \a events as a new element to the end of the EventList. */
  void push( const EventData &events );
    /*! Copy event times from \a events between time \a tbegin and 
        time \a tend seconds as a new element to the end of the EventList.
	In the copy, all event times and the signalTime() 
	are set relative to time \a tbegin. */
  void push( const EventData &events, double tbegin, double tend );
    /*! Copy event times from \a events between time \a tbegin and 
        time \a tend seconds as a new element to the end of the EventList.
	In the copy, all event times and the signalTime() 
	are set relative to time \a tref. */
  void push( const EventData &events, double tbegin, double tend, double tref );
    /*! Copies \a events as a new element to the end of the EventList. */
  void push( const ArrayD &events, double tbegin=-HUGE_VAL,
	     double tend=HUGE_VAL, double stepsize=0.0001 );

    /*! For each event \a t in \a times copy all event times from \a events
        between the time of the event \a t and \a t plus \a tend seconds
	as a new element to the end of the EventList
	with the event times set relative to the time of the event \a t. */
  void push( const EventData &events, const EventData &times, double tend );
    /*! For each event \a t in \a times copy all event times from \a events
        between the time of the event \a t plus \a tbegin seconds
	and \a t plus \a tend seconds
	as a new element to the end of the EventList
	with the event times set relative to the time of the event \a t. */
  void push( const EventData &events, const EventData &times, double tbegin, double tend );

    /*! Copy each element of \a el as new elements to the end of the EventList. */
  void push( const EventList &el );
    /*! Copy event times from each element of \a el between time \a tbegin
        and time \a tend seconds as a new element to the end of the EventList.
	In the copy, all event times and the signalTime() 
	are set relative to time \a tbegin. */
  void push( const EventList &el, double tbegin, double tend );
    /*! Copy event times from each element of \a el between time \a tbegin
        and time \a tend seconds as a new element to the end of the EventList.
	In the copy, all event times and the signalTime() 
	are set relative to time \a tref. */
  void push( const EventList &el, double tbegin, double tend, double tref );

    /*! Add an EventData as a new element to the end of the list
        and set its capacity to \a m.
        The new EventData element gets additional buffers for the size
	and the width of events if \a sizebuffer or \a widthbuffer
	are \c true, respectively. */
  void push( int m, bool sizebuffer=false, bool widthbuffer=false );
    /*! Add \a n EventData as new elements to the end of the list
        and set their capacity to \a m.
        The new EventData elements get additional buffers for the size
	and the width of events if \a sizebuffer or \a widthbuffer
	are \c true, respectively. */
  void push( int n, int m, bool sizebuffer=false, bool widthbuffer=false );

    /*! Add the pointer \a events as a new element to the end of the EventList.
        If \a own is set to \c true then the ownership of \a events
        is transfered to the EventList, i.e. the EventList might delete it. */
  void add( EventData *events, bool own=false );
    /*! Add the pointer \a events as a new element to the end of the EventList.
        If \a own is set to \c true then the ownership of \a events
        is transfered to the EventList, i.e. the EventList might delete it. */
  void add( const EventData *events, bool own=false );

    /*! Erase the EventData at index \a index. */
  void erase( int index );

    /*! Clear each of the EventData without erasing them from the EventList. */
  void clearBuffer( void );

  typedef vector< EventData* > EL;
  typedef EL::iterator iterator;
  typedef EL::const_iterator const_iterator;
  iterator begin( void ) { return Events.begin(); };
  const_iterator begin( void ) const { return Events.begin(); };
  iterator end( void ) { return Events.end(); };
  const_iterator end( void ) const { return Events.end(); };

    /*! Set the beginning of the range() containing the events to \a offset
        for all EventData in the list. */
  void setOffset( double offset );
    /*! Set the length of the range() containing the events to \a duration
        for all EventData in the list. */
  void setLength( double duration );
    /*! Set the stepsize (resolution) of the range() containing the events
        to \a stepsize for all EventData in the list. */
  void setStepsize( double stepsize );
    /*! Set the offset of the range containing the events
        to \a front without changing the last range element
	for all EventData in the list. */
  void setRangeFront( double front );
    /*! Resize the range containing the events
        such that the last range element equals \a back
	for all EventData in the list. */
  void setRangeBack( double back );
    /*! Set the time of the signal to \a s for all EventData in the list. */
  void setSignalTime( double s );

    /*! Add \a x to all event times, signalTime() and the range(). */
  const EventList &operator+=( double x );
    /*! Subtract \a x from all event times, signalTime() and the range(). */
  const EventList &operator-=( double x );
    /*! Multiply all event times, signalTime() and the range() by \a x. */
  const EventList &operator*=( double x );
    /*! Divide all event times, signalTime() and the range() by \a x. */
  const EventList &operator/=( double x );

    /*! Returns the average number of events
        between time \a tbegin and time \a tend seconds. 
        The standard deviation of the count is returned in \a sd,
	provided \a sd does not equal zero. */
  double count( double tbegin, double tend, double *sd=0 ) const; 
    /*! Returns the total number of events
        between time \a tbegin and time \a tend seconds. */
  double totalCount( double tbegin, double tend ) const; 

    /*! Mean event rate (Hz) as the number of events
        between time \a tbegin and time \a tend seconds 
	divided by the width \a tend - \a tbegin of the time window. 
        In \a sd the standard deviation of the rate is returned
	provided \a sd does not equal zero. */
  double rate( double tbegin, double tend, double *sd=0 ) const; 

    /*! The time course of the mean event rate
        between rate.rangeFront() and rate.rangeBack() seconds
	relative to time \a time seconds is returned in \a rate.
	The rate is the average number of events per bin.
	The width of the bins is given by \a width seconds.
        If \a width is less or equal to zero it is set to
	the stepsize of \a rate. */
  void rate( SampleDataD &rate, double width=0.0, double time=0.0 ) const;
    /*! The time course of the mean event rate and its standard deviation
        between rate.rangeFront() and rate.rangeBack() seconds
	relative to time \a time seconds is returned in \a rate.
	The rate is the average number of events per bin.
	The width of the bins is given by \a width seconds.
        If \a width is less or equal to zero it is set to
	the stepsize of \a rate. */
  void rate( SampleDataD &rate, SampleDataD &ratesd,
	     double width=0.0, double time=0.0 ) const;
    /*! The time course of the mean event rate
        between rate.rangeFront() and rate.rangeBack() seconds
	relative to time \a time seconds is added to \a rate.
	The rate is the average number of events per bin.
	The width of the bins is given by \a width seconds.
        If \a width is less or equal to zero it is set to
	the stepsize of \a rate. */
  void addRate( SampleDataD &rate, int &trial, double width=0.0,
		double time=0.0 ) const;

    /*! The time course of the mean cyclic event rate
        between rate.rangeFront() and rate.rangeBack() seconds
	relative to time \a time seconds is returned in \a rate.
	The rate is the number of events per bin.
	The width of the bins is given by \a width seconds.
        If \a width is less or equal to zero it is set to
	the stepsize of \a rate. */
  void cyclicRate( SampleDataD &rate, double width=0.0,
		   double time=0.0 ) const;
    /*! The time course of the mean cyclic event rate
        between rate.rangeFront() and rate.rangeBack() seconds
	relative to time \a time seconds is added to \a rate.
	The rate is the number of events per bin.
	The width of the bins is given by \a width seconds.
        If \a width is less or equal to zero it is set to
	the stepsize of \a rate. */
  void addCyclicRate( SampleDataD &rate, int &trial, double width=0.0,
		      double time=0.0 ) const;

    /*! The time course of the mean event rate.
        between rate.rangeFront() and rate.rangeBack() seconds
	relative to time \a time seconds is returned in \a rate.
	Each event is replaced by the \a kernel,
	which then are summed up. */
  void rate( SampleDataD &rate, const Kernel &kernel, double time=0.0 ) const;
    /*! The time course of the mean event rate and its standard deviation
        between rate.rangeFront() and rate.rangeBack() seconds
	relative to time \a time seconds 
	are returned in \a rate and \a ratesd, respectively.
	Each event is replaced by the \a kernel,
	which then are summed up. */
  void rate( SampleDataD &rate, SampleDataD &ratesd, const Kernel &kernel,
	     double time=0.0 ) const;
    /*! The time course of the mean event rate 
        between rate.rangeFront() and rate.rangeBack() seconds
	relative to time \a time seconds is added to \a rate.
	Each event is replaced by the \a kernel,
	which then are summed up. */
  void addRate( SampleDataD &rate, int &trial, const Kernel &kernel,
		double time=0.0 ) const;

    /*! The time course of the mean cyclic event rate
        between rate.rangeFront() and rate.rangeBack() seconds
	relative to time \a time seconds is returned in \a rate.
	Each event is replaced by the \a kernel,
	which then are summed up. */
  void cyclicRate( SampleDataD &rate, const Kernel &kernel,
		   double time=0.0 ) const;
    /*! The time course of the mean cyclic event rate and its standard deviation.
        between rate.rangeFront() and rate.rangeBack() seconds
	relative to time \a time seconds 
	are returned in \a rate and \a ratesd, respectively.
	The width and the position of the time windows
	are given by \a rate in seconds.
	Each event is replaced by the \a kernel,
	which then are summed up. */
  void cyclicRate( SampleDataD &rate, SampleDataD &ratesd,
		   const Kernel &kernel, double time=0.0 ) const;
    /*! The time course of the mean cyclic event rate
        between rate.rangeFront() and rate.rangeBack() seconds
	relative to time \a time seconds is added to \a rate.
	Each event is replaced by the \a kernel,
	which then are summed up. */
  void addCyclicRate( SampleDataD &rate, int &trial,
		      const Kernel &kernel, double time=0.0 ) const;

    /*! Return the interevent intervals for each time 
        of the range \a intervals relative to \a time in \a intervals. */
  void intervals( SampleData< ArrayD > &intervals, double time=0.0 ) const;
    /*! Return the interevent intervals that include
        time \a time in \a intervals. */
  void intervalsAt( double time, ArrayD &intervals ) const;

    /*! Mean event interval (seconds) of all event intervals 
        between time \a tbegin and time \a tend seconds. */
  double interval( double tbegin, double tend ) const; 
    /*! Mean event interval (seconds) of all event intervals 
        between time \a tbegin and time \a tend seconds.
        In \a sd the standard deviation of the intervals is returned. */
  double interval( double tbegin, double tend, double &sd ) const; 
    /*! The time course of the trial averaged event intervals 1/<1/ISI>
        between intervals.rangeFront() and intervals.rangeBack() seconds
	relative to time \a time seconds is returned in \a intervals. */
  void interval( SampleDataD &intervals, double time=0.0 ) const;
    /*! The time course of the trial averaged event intervals 1/<1/ISI>
        between intervals.rangeFront() and intervals.rangeBack() seconds
        relative to time \a time seconds is returned in \a intervals.
	In \a sd the standard deviation of the intervals is returned. */
  void interval( SampleDataD &intervals, SampleDataD &sd,
		 double time=0.0 ) const;
    /*! The time course of the trial averaged event intervals 1/<1/ISI>
        between intervals.rangeFront() and intervals.rangeBack() seconds
        relative to time \a time seconds is returned in \a intervals.
	In \a cv the corresponding coefficient of variation is returned. */
  void intervalCV( SampleDataD &intervals, SampleDataD &cv,
		   double time=0.0 ) const;
    /*! The time course of the trial averaged cyclic event intervals 1/<1/ISI>
        between intervals.rangeFront() and intervals.rangeBack() seconds
	relative to time \a time seconds is returned in \a intervals. */
  void cyclicInterval( SampleDataD &intervals, double time=0.0 ) const;
    /*! Mean event interval (seconds) of the event intervals at 
        \a time seconds. */
  double intervalAt( double time ) const; 
    /*! Mean event interval (seconds) of the event intervals at 
        \a time seconds.
        In \a sd the standard deviation of the intervals is returned. */
  double intervalAt( double time, double &sd ) const; 

    /*! Returns in \a intrvls[i].x() the position of each interevent interval
        between \a tbegin and \a tend, and in \a intrvls[i].y() the
	interevent interval for each trial \a i.
        The position of the interevent interval is the position of the left
	event (\a pos = -1, default), the position of the right event 
	(\a pos = 1), or in between the left and the right event
	(\a pos = 0).
        \return the number of interevent intervals. */
  int intervals( double tbegin, double tend,
		 vector<MapD> &intrvls, int pos=-1 ) const;
    /*! Write into stream \a os the position of each interevent interval
        between \a tbegin and \a tend multiplied by \a tfac in the first column,
	and the interevent interval in the second column.
        The position of the interevent interval is the position of the left
	event (\a pos = -1, default), the position of the right event 
	(\a pos = 1), or in between the left and the right event
	(\a pos = 0).
	Both the position and the intervals are
	formatted as specified by \a width, \a prec, and \a frmt.
        The individual EventData are separated by \a sep blank lines.
        If there aren't any intervals and \a noevents isn't empty, than
        \a noevents is printed as the only output once in each column.
        \return the number of interevent intervals. */
  ostream &saveIntervals( double tbegin, double tend, ostream &os, int pos=-1,
			  double tfac=1.0, int width=0, int prec=5,
			  char frmt='g', int sep=1,
			  const string &noevents="" ) const;

    /*! Mean event frequency (Hz) as the inverse of the mean event interval
        of all event intervals
	between time \a tbegin and time \a tend seconds. */
  double frequency( double tbegin, double tend ) const; 
    /*! Mean event frequency (Hz) as the inverse of the mean event interval
        of all event intervals
	between time \a tbegin and time \a tend seconds.
        In \a sd the standard deviation in Hz as the standard deviation
	of the intervals devided by the squared mean interval is returned. */
  double frequency( double tbegin, double tend, double &sd ) const; 
    /*! The time course of the averaged instantaneous rate <1/ISI>
        between rate.rangeFront() and rate.rangeBack() seconds
        relative to time \a time seconds is returned in \a rate. */
  void frequency( SampleDataD &rate, double time=0.0 ) const;
    /*! The time course of the averaged instantaneous rate <1/ISI>
        between rate.rangeFront() and rate.rangeBack() seconds
        relative to time \a time seconds is returned in \a rate.
	In \a sd the standard deviation of the rate is returned. */
  void frequency( SampleDataD &rate, SampleDataD &sd, double time=0.0 ) const;
    /*! The time course of the averaged instantaneous rate <1/ISI>
        between rate.rangeFront() and rate.rangeBack() seconds
        relative to time \a time seconds is returned in \a rate.
	In \a cv the corresponding coefficient of variation is returned. */
  void frequencyCV( SampleDataD &rate, SampleDataD &cv,
		    double time=0.0 ) const;
    /*! The time course of the averaged cyclic instantaneous rate <1/ISI>
        between rate.rangeFront() and rate.rangeBack() seconds
        relative to time \a time seconds is returned in \a rate. */
  void cyclicFrequency( SampleDataD &rate, double time=0.0 ) const;
    /*! The time course of the averaged cyclic instantaneous rate <1/ISI>
        between rate.rangeFront() and rate.rangeBack() seconds
        relative to time \a time seconds is returned in \a rate.
	In \a sd the standard deviation of the rate is written. */
  void cyclicFrequency( SampleDataD &rate, SampleDataD &sd,
			double time=0.0 ) const;
    /*! Mean frequency as the inverse of the mean event intervals at 
        \a time seconds. */
  double frequencyAt( double time ) const; 
    /*! Mean frequency (Hz) as the inverse of the mean event intervals at 
        \a time seconds.
        In \a sd the standard deviation in Hz is returned. */
  double frequencyAt( double time, double &sd ) const; 

    /*! Returns in \a freqs[i].x() the position of each interevent interval
        between \a tbegin and \a tend, and in \a freqs[i].y() 1 divided by
	that interevent interval for each trial \a i.
        The position of the interevent interval is the position of the left
	event (\a pos = -1, default), the position of the right event 
	(\a pos = 1), or in between the left and the right event
	(\a pos = 0).
        \return the number of interevent intervals. */
  int frequencies( double tbegin, double tend,
		   vector<MapD> &freqs, int pos=-1 ) const;
    /*! Write into stream \a os the position of each interevent interval
        between \a tbegin and \a tend multiplied by \a tfac in the first column,
	and 1 divided by that interevent interval in the second column.
        The position of the interevent interval is the position of the left
	event (\a pos = -1, default), the position of the right event 
	(\a pos = 1), or in between the left and the right event
	(\a pos = 0).
	Both the position and the intervals are
	formatted as specified by \a width, \a prec, and \a frmt.
        The individual EventData are separated by \a sep blank lines.
        If there aren't any intervals and \a noevents isn't empty, than
        \a noevents is printed as the only output once in each column.
        \return the number of interevent intervals. */
  ostream &saveFrequencies( double tbegin, double tend, ostream &os,
			    int pos=-1, double tfac=1.0, int width=0,
			    int prec=5, char frmt='g', int sep=1,
			    const string &noevents="" ) const;

    /*! Compute a histogram of the event intervals
        between time \a tbegin and time \a tend seconds.
	The resolution and maximum interval lenght is given by \a hist. */
  void intervalHistogram( double tbegin, double tend, SampleDataD &hist ) const;
    /*! Compute a normalized histogram of all the event intervals 
        that include time \a time.
	The resolution and maximum interval lenght is given by \a hist. */
  void directIntervalHistogram( double time, SampleDataD &hist ) const;
    /*! Compute a normalized histogram of all the event intervals 
        that include time \a time weighted with their length.
	The resolution and maximum interval lenght is given by \a hist. */
  void correctedIntervalHistogram( double time, SampleDataD &hist ) const;

    /*! Compute the serial correlation coefficients
        for lag zero to lag \a sc.size()-1
        of all event intervals between time \a tbegin
	and time \a tend seonds averaged over the trials. */
  void serialCorr( double tbegin, double tend, ArrayD &sc ) const;
    /*! Compute the serial correlation coefficients
        for lag zero to lag \a sc.size()-1
        of all event intervals between time \a tbegin
	and time \a tend seonds averaged over the trials.
        In \a sd the corresponding standard deviation is returned. */
  void serialCorr( double tbegin, double tend, ArrayD &sc, ArrayD &sd ) const;

    /*! Compute the phase locking as the number of spikes per period \a period
        of all events between time \a tbegin
	and time \a tend seonds.
        \a tend is reduced to \a tbegin plus 
	the closest multiple of \a period. */
  double locking( double tbegin, double tend, double period ) const
    { double sd=0.0; return locking( tbegin, tend, period, sd ); };
    /*! Compute the phase locking as the number of spikes per period \a period
        of all events since time \a time (seconds) 
	during \a duration seconds.
        \a duration is reduced to the closest multiple of \a period.
        For each EventData the locking is computed separately.
        The function returns the mean locking 
	and the standarad deviation in \a sd. */
  double locking( double tbegin, double tend, double period, double &sd ) const;
    /*! Compute the vector strength for a period \a period (seconds)
        of all events between time \a tbegin and time \a tend seconds
	according to 
        \f[ VS = \frac{1}{n} \sqrt{ \left( \sum_{i=1}^n \sin\varphi_i \right)^2 + \left( \sum_{i=1}^n \cos\varphi_i \right)^2} \f]
	with \f[ \varphi_i = 2 \pi ( t_i - time ) / period \f]
	for all \a n spike times \a t_i. */
  double vectorStrength( double tbegin, double tend, double period ) const;
    /*! Compute the vector strength for a period \a period (seconds)
        of all events between time \a tbegin and time \a tend seconds
	according to 
        \f[ VS = \frac{1}{n} \sqrt{ \left( \sum_{i=1}^n \sin\varphi_i \right)^2 + \left( \sum_{i=1}^n \cos\varphi_i \right)^2} \f]
	with \f[ \varphi_i = 2 \pi ( t_i - time ) / period \f]
	for all \a n spike times \a t_i of each trial.
        For each EventData the vector strength is computed separately.
        The function returns the mean vector strength 
	and the standarad deviation in \a sd. */
  double vectorStrength( double tbegin, double tend, double period, 
			 double &sd ) const;
    /*! Compute the vector phase for a period \a period (seconds)
        of all events between time \a tbegin and time \a tend seconds
	according to 
        \f[ VP = \mbox{atan} \frac{ \sum_{i=1}^n \sin\varphi_i }{ \sum_{i=1}^n \cos\varphi_i } \f]
	with \f[ \varphi_i = 2 \pi ( t_i - time ) / period \f]
	for all \a n spike times \a t_i.*/
  double vectorPhase( double tbegin, double tend, double period ) const;
    /*! Compute the vector phase for a period \a period (seconds)
        of all events between time \a tbegin and time \a tend seconds
	according to 
        \f[ VP = \mbox{atan} \frac{ \sum_{i=1}^n \sin\varphi_i }{ \sum_{i=1}^n \cos\varphi_i } \f]
	with \f[ \varphi_i = 2 \pi ( t_i - time ) / period \f]
	for all \a n spike times \a t_i of each trial.
        For each EventData the vector strength is computed separately.
        The function returns the mean vector strength 
	and the standarad deviation in \a sd. */
  double vectorPhase( double tbegin, double tend, double period, 
		      double &sd ) const;

    /*! Returns the correlation between time \a tbegin
        and time \a tend seconds computed as the correlation between
        pairs of EventData convolved with \a kernel and averaged over pairs.
        The corresponding standard deviation is returned in \a sd.
        Performs computation using a temporal resolution of \a dt seconds. */
  double correlation( double tbegin, double tend, 
		      const Kernel &kernel, double dt, double &sd ) const;
    /*! Returns the correlation between time \a tbegin
        and time \a tend seconds computed as the correlation between
        pairs of EventData convolved with \a kernel and averaged over pairs.
        Performs computation using a temporal resolution of \a dt seconds. */
  double correlation( double tbegin, double tend, 
		      const Kernel &kernel, double dt=0.001 ) const;

    /*! Returns the reliability between time \a tbegin
        and time \a tend seconds computed as the correlation between
        pairs of EventData convolved with \a kernel
	without removing the average and averaged over pairs 
	(Schreiber et al.).
        The corresponding standard deviation is returned in \a sd.
        Performs computation using a temporal resolution of \a dt seconds. */
  double reliability( double tbegin, double tend, 
		      const Kernel &kernel, double dt, double &sd ) const;
    /*! Returns the reliability between time \a tbegin
        and time \a tend seconds computed as the correlation between
        pairs of EventData convolved with \a kernel
	without removing the average and averaged over pairs 
	(Schreiber et al.).
        Performs computation using a temporal resolution of \a dt seconds. */
  double reliability( double tbegin, double tend, 
		      const Kernel &kernel, double dt=0.001 ) const;

    /*! Convolves each spiketrain with the kernel \a kernel.
        The resulting firing rates are pairwise multiplied.
        Returns the square root of the mean over all pairs in \a rate
        and the corresponding standard deviation in \a ratesd. */
  void coincidenceRate( SampleDataD &rate,  SampleDataD &ratesd, 
			const Kernel &kernel );

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

    /*! Returns in \a psd the trial averaged powerspectrum density
        of the events between \a tbegin and \a tend.
	The size of \a psd times \a step determines
	the width of the time windows used for the fourier transformations.
	The bin width for discretizing the events is set to \a step.
        The frequency axis of the spectrum \a psd is set to the appropriate values. */
  void spectrum( double tbegin, double tend, double step,
		 SampleDataD &psd ) const;
    /*! Returns in \a psd the trial averaged powerspectrum density
        of the events between \a tbegin and \a tend.
	In \a sd the corresponding standard deviation is returned.
	The size of \a psd times \a step determines
	the width of the time windows used for the fourier transformations.
	The bin width for discretizing the events is set to \a step.
        The frequency axis of the spectrum \a psd and its standard deviation \a sd
	is set to the appropriate values. */
  void spectrum( double tbegin, double tend, double step,
		 SampleDataD &psd, SampleDataD &sd ) const;

    /*! Returns in \a c the trial-averaged stimulus-response coherence between 
        \a stimulus and each of the event trials (the S-R coherence).
	The size of \a c times stimulus.stepsize() determines
	the width of the time windows used for the fourier transformations.
	Only events during the stimulus (between stimulus.rangeFront()
	and stimulus.rangeBack() ) are considered.
	The sampling interval of the stimulus (stimulus.stepsize())
	is used as the bin width for discretizing the events.
        The frequency axis of the coherence \a c is set to the appropriate values. */
  void coherence( const SampleDataD &stimulus, SampleDataD &c ) const;
    /*! Returns in \a c the trial-averaged stimulus-response coherence between 
        \a stimulus and each of the event trials (the S-R coherence).
	In \a sd the corresponding standard deviation is returned.
	The size of \a c times stimulus.stepsize() determines
	the width of the time windows used for the fourier transformations.
	Only events during the stimulus (between stimulus.rangeFront()
	and stimulus.rangeBack() ) are considered.
	The sampling interval of the stimulus (stimulus.stepsize())
	is used as the bin width for discretizing the events.
        The frequency axis of the coherence \a c and its standard deviation \a sd
	is set to the appropriate values. */
  void coherence( const SampleDataD &stimulus,
		  SampleDataD &c, SampleDataD &sd ) const;
    /*! Returns in \a c the coherence between pairs of event trials
        averaged over all pairs	(the response-response (R-R) coherence).
	The size of \a c times \a step determines
	the width of the time windows used for the fourier transformations.
	Only events during the \a tbegin and \a tend are considered.
	The bin width for discretizing the events is set to \a step.
        The frequency axis of the coherence \a c is set to the appropriate values. */
  void coherence( double tbegin, double tend, double step,
		  SampleDataD &c ) const;
    /*! Returns in \a c the coherence between pairs of event trials
        averaged over all pairs	(the response-response (R-R) coherence).
	In \a sd the corresponding standard deviation is returned.
	The size of \a c times \a step determines
	the width of the time windows used for the fourier transformations.
	Only events during the \a tbegin and \a tend are considered.
	The bin width for discretizing the events is set to \a step.
        The frequency axis of the coherence \a c and its standard deviation \a sd
	is set to the appropriate values. */
  void coherence( double tbegin, double tend, double step,
		  SampleDataD &c, SampleDataD &sd ) const;

    /*! The mean latenceny and standard deviation \a sd of the events
        following time \a time. */
  double latency( double time, double *sd=0 ) const;

    /*! Merge (sum up) all event data into a single EventData \a all.
        \note Only event times are considered.
        Additional sizes and widths are ignored. */
  void sum( EventData &all ) const;
    /*! For each time bin of width \a bin 
        count the number of trials that contain at least one event.
        If the event count divided by the number of trials (size())
	equals or exceeds \a p, an event with the time of the time bin
        is added to \a s.
        If \a keep is set true, then the event times of the first trial
        are added to \a s instead of the time bins. */
  void sync( EventData &s, double bin, double p=1.0, bool keep=false ) const;

    /*! Generate \a trials of independent
        poisson spike trains each with rate \a rate Hertz
        and absoulte refractory period \a refract
        and duration \a duration seconds.
        Use the random number gnerator \a random. */
  void poisson( int trials, double rate, double refract, double duration,
		RandomBase &random=rnd );

    /*! Write event times as a single column of text in stream \a s.
        Each event time is multiplied by \a tfac,
	formatted as specified by \a width, \a prec, and \a frmt,
	and is written in a separate line. 
        The individual EventData are separated by \a sep blank lines.
        If there aren't any events in a trial and \a noevents isn't empty, than
        \a noevents is printed as the only output for that trial. */
  void saveText( ostream &os, double tfac=1.0,
		 int width=0, int prec=5, char frmt='g',
		 int sep=1, const string &noevents="" ) const;
    /*! Write event times as text in stream \a s.
        Two columns are written.
	The first column is the event time is multiplied by \a tfac, and is
	formatted as specified by \a width, \a prec, and \a frmt.
	The second column is an y-value as specified by \a y.
        The individual EventData are separated by \a sep blank lines.
        If there aren't any events in a trial and \a noevents isn't empty, than
        a single line with \a noevents as the time of an event 
	followed by \a noy as the corresponding y-value
	is printed as the only output for that trial. */
  void savePoint( ostream &os, double y=0.0, double tfac=1.0,
		  int width=0, int prec=5, char frmt='g',
		  int sep=1, const string &noevents="", double noy=-1.0 ) const;
    /*! Write event times as text in stream \a s
        such that each event gets two data points.
	The y-value of the data points are set to \a n + \a lower and \a n + \a upper
	where \a n is the index of the EventData + \a offs.
        Each event time is multiplied by \a tfac, and is
	formatted as specified by \a width, \a prec, and \a frmt.
	Events are separated by a blank line. 
        The individual EventData are separated by \a sep blank lines
	(in addition to the one blank line separating the events).
        If there aren't any events in a trial and \a noevents isn't empty, than
        a single line with \a noevents as the time of an event 
	followed by \a noy as the corresponding y-value
	is printed as the only output for that trial. */
  void saveStroke( ostream &os, int offs=0, double tfac=1.0, 
		   int width=0, int prec=5, char frmt='g', int sep=0,
		   double lower=0.1, double upper=0.9,
		   const string &noevents="", double noy=-1.0 ) const;
    /*! Write event times as text in stream \a s
        such that each event gets four data points.
	Each such box has width \a bin and contains the corresponding event.
        The sides of the box are at multiples of \a bin.
	The y-values of the box are set to 
	\a n + \a lower and \a n + \a upper
	where \a n is the index of the EventData + \a offs.
        All times are multiplied by \a tfac, and are
	formatted as specified by \a width, \a prec, and \a frmt.
	Events are separated by a blank line.
        If there aren't any events in a trial and \a noevents isn't empty, than
        a single line with \a noevents as the time of an event 
	followed by \a noy as the corresponding y-value
	is printed as the only output for that trial. */
  void saveBox( ostream &os, double bin, int offs=0, double tfac=1.0,
		int width=0, int prec=5, char frmt='g', int sep=0,
		double lower=0.1, double upper=0.9, 
		const string &noevents="", double noy=-1.0 ) const;

    /*! Write content of all EventData variables to stream \a str
        (for debugging only). */
  friend ostream &operator<< ( ostream &str, const EventList &events );


 private:

  EL Events;
  vector< bool > Own;

};


}; /* namespace relacs */

#endif /* ! _RELACS_EVENTLIST_H_ */
