/*
  eventlist.cc
  A container for EventData

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

#include <cmath>
#include <iomanip>
#include <algorithm>
#include <relacs/array.h>
#include <relacs/sampledata.h>
#include <relacs/stats.h>
#include <relacs/kernel.h>
#include <relacs/eventlist.h>

using namespace std;

namespace relacs {


EventList::EventList( void )
  : Events(), Own()
{
}


EventList::EventList( const EventList &el )
{
  Own = el.Own;
  Events.resize( el.Events.size() );
  for ( unsigned int k=0; k<Events.size(); k++ ) {
    if ( Own[k] ) {
      Events[k] = new EventData( *(el.Events[k]) );
    }
    else {
      Events[k] = el.Events[k];
    }   
  }
}


EventList::EventList( const EventList &el, double tbegin, double tend )
{
  push( el, tbegin, tend );
}


EventList::EventList( const EventList &el, double tbegin, double tend, double tref )
{
  push( el, tbegin, tend, tref );
}


EventList::EventList( const EventData &events )
  : Events(), Own()
{
  push( events );
}


EventList::EventList( const EventData &events, double tbegin, double tend )
  : Events(), Own()
{
  push( events, tbegin, tend );
}


EventList::EventList( const EventData &events, 
		      double tbegin, double tend, double tref )
  : Events(), Own()
{
  push( events, tbegin, tend, tref );
}


EventList::EventList( const EventData &events, const EventData &times, 
		      double tend )
{
  push( events, times, tend );
}


EventList::EventList( const EventData &events, const EventData &times,
		      double tbegin, double tend )
{
  push( events, times, tbegin, tend );
}


EventList::EventList( EventData *events, bool own )
  : Events(), Own()
{
  add( events, own );
}


EventList::EventList( int n, int m, bool sizebuffer, bool widthbuffer )
  : Events(), Own()
{
  push( n, m, sizebuffer, widthbuffer );
}


EventList::~EventList( void )
{
  clear();
}


int EventList::size( void ) const
{
  return Events.size();
}


bool EventList::empty( void ) const
{
  return Events.empty();
}


void EventList::resize( int n, int m, bool sizebuffer, bool widthbuffer )
{
  if ( n <= 0 ) {
    clear();
    return;
  }

  int os = Events.size();
  Events.resize( n, 0 );
  Own.resize( n, true );
  if ( n > os ) {
    for ( int k=os; k<n; k++ ) {
      Events[k] = new EventData( m, sizebuffer, widthbuffer );
      Own[k] = true;
    }
  }
}


void EventList::clear( void )
{
  vector< bool >::iterator o = Own.begin();
  for ( iterator i = begin(); i != end(); ++i, ++o ) {
    if ( *o )
      delete *i;
  }
  Events.clear();
  Own.clear();
}


int EventList::capacity( void ) const
{
  return Events.capacity();
}


void EventList::reserve( int n )
{
  Events.reserve( n );
  Own.reserve( n );
}


EventList &EventList::operator=( const EventList &el )
{
  if ( &el == this )
    return *this;

  Own = el.Own;
  Events.resize( el.Events.size() );
  for ( unsigned int k=0; k<Events.size(); k++ ) {
    if ( Own[k] ) {
      Events[k] = new EventData( *(el.Events[k]) );
    }
    else {
      Events[k] = el.Events[k];
    }   
  }

  return *this;
}


const EventData &EventList::front( void ) const
{
  return *Events.front();
}


EventData &EventList::front( void )
{
  return *Events.front();
}


const EventData &EventList::back( void ) const
{
  return *Events.back();
}


EventData &EventList::back( void )
{
  return *Events.back();
}


const EventData &EventList::operator[]( const string &ident ) const
{
  for ( const_iterator i = begin(); i != end(); ++i )
    if ( (*i)->ident() == ident ) 
      return *(*i);

  return operator[]( 0 );
}


EventData &EventList::operator[]( const string &ident )
{
  for ( const_iterator i = begin(); i != end(); ++i )
    if ( (*i)->ident() == ident ) 
      return *(*i);

  return operator[]( 0 );
}


int EventList::index( const string &ident ) const
{
  int k=0;
  for ( const_iterator i = begin(); i != end(); ++i, ++k ) {
    if ( (*i)->ident() == ident ) 
      return k;
  }

  return -1;
}


void EventList::push( const EventData &events )
{
  Events.push_back( new EventData( events ) );
  Own.push_back( true );
}


void EventList::push( const EventData &events, double tbegin, double tend )
{
  Events.push_back( new EventData( events, tbegin, tend ) );
  Own.push_back( true );
}


void EventList::push( const EventData &events, double tbegin, double tend,
		      double tref )
{
  Events.push_back( new EventData( events, tbegin, tend, tref ) );
  Own.push_back( true );
}


void EventList::push( const ArrayD &events,
		      double tbegin, double tend, double stepsize )
{
  Events.push_back( new EventData( events, tbegin, tend, stepsize ) );
  Own.push_back( true );
}


void EventList::push( const EventData &events, const EventData &times,
		      double tend )
{
  reserve( size() + times.size() );
  for ( int k=0; k<times.size(); k++ ) {
    Events.push_back( new EventData( events, times[k], times[k]+tend ) );
    Own.push_back( true );
  }
}


void EventList::push( const EventData &events, const EventData &times,
		      double tbegin, double tend )
{
  reserve( size() + events.size() );
  for ( int k=0; k<times.size(); k++ ) {
    Events.push_back( new EventData( events, times[k]+tbegin, times[k]+tend,
				     times[k] ) );
    Own.push_back( true );
  }
}


void EventList::push( const EventList &el )
{
  reserve( size() + el.size() );
  for ( int k=0; k<el.size(); k++ ) {
    push( el[k] );
  }
}


void EventList::push( const EventList &el, double tbegin, double tend )
{
  reserve( size() + el.size() );
  for ( int k=0; k<el.size(); k++ ) {
    push( el[k], tbegin, tend );
  }
}


void EventList::push( const EventList &el, double tbegin, double tend, double tref )
{
  reserve( size() + el.size() );
  for ( int k=0; k<el.size(); k++ ) {
    push( el[k], tbegin, tend, tref );
  }
}


void EventList::push( int m, bool sizebuffer, bool widthbuffer )
{
  reserve( size() + 1 );
  Events.push_back( new EventData( m, sizebuffer, widthbuffer ) );
  Events.back()->setCyclic( false );
  Own.push_back( true );
}


void EventList::push( int n, int m, bool sizebuffer, bool widthbuffer )
{
  reserve( size() + n );
  for ( int k=0; k<n; k++ ) {
    Events.push_back( new EventData( m, sizebuffer, widthbuffer ) );
    Events.back()->setCyclic( false );
    Own.push_back( true );
  }
}


void EventList::add( EventData *events, bool own )
{
  Events.push_back( events );
  Own.push_back( own );
}


void EventList::add( const EventData *events, bool own )
{
  Events.push_back( const_cast<EventData*>(events) );
  Own.push_back( own );
}


void EventList::erase( int index )
{
  if ( index >= 0 && index < size() ) {
    if ( Own[index] ) {
      delete Events[index];
    }
    Events.erase( Events.begin() + index );
    Own.erase( Own.begin() + index );
  }
}


void EventList::clearBuffer( void )
{
  for ( iterator i = begin(); i != end(); ++i )
    (*i)->clear();
}


void EventList::setOffset( double offset )
{
  for ( iterator i = begin(); i != end(); ++i )
    (*i)->setOffset( offset );
}


void EventList::setLength( double duration )
{
  for ( iterator i = begin(); i != end(); ++i )
    (*i)->setLength( duration );
}


void EventList::setStepsize( double stepsize )
{
  for ( iterator i = begin(); i != end(); ++i )
    (*i)->setStepsize( stepsize );
}


void EventList::setRangeFront( double front )
{
  for ( iterator i = begin(); i != end(); ++i )
    (*i)->setRangeFront( front );
}


void EventList::setRangeBack( double back )
{
  for ( iterator i = begin(); i != end(); ++i )
    (*i)->setRangeBack( back );
}


void EventList::setSignalTime( double s )
{
  for ( iterator i = begin(); i != end(); ++i )
    (*i)->setSignalTime( s );
}


const EventList &EventList::operator+=( double x )
{
  for ( iterator i = begin(); i != end(); ++i )
    *(*i) += x;
  return *this;
}


const EventList &EventList::operator-=( double x )
{
  for ( iterator i = begin(); i != end(); ++i )
    *(*i) -= x;
  return *this;
}


const EventList &EventList::operator*=( double x )
{
  for ( iterator i = begin(); i != end(); ++i )
    *(*i) *= x;
  return *this;
}


const EventList &EventList::operator/=( double x )
{
  for ( iterator i = begin(); i != end(); ++i )
    *(*i) /= x;
  return *this;
}


double EventList::count( double tbegin, double tend, double *sd ) const
{
  vector<double> counts;
  counts.reserve( size() );

  double mean = 0.0;

  int j = 0;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    double c = (*i)->count( tbegin, tend );
    counts.push_back( c );
    mean += ( c - mean )/(++j);
  }

  if ( sd != 0 ) {
    double var = 0.0;
    for ( unsigned int k=0; k<counts.size(); k++ ) {
      double s = counts[k] - mean;
      var += ( s*s - var )/(k+1);
    }
    *sd = ::sqrt( var );
  }

  return mean;
}


double EventList::totalCount( double tbegin, double tend ) const
{
  double counts = 0.0;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    counts += (*i)->count( tbegin, tend );
  }
  return counts;
}


double EventList::rate( double tbegin, double tend, double *sd ) const
{
  vector<double> rates;
  rates.reserve( size() );

  double meanrate = 0.0;

  int j = 0;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    double r = (*i)->rate( tbegin, tend );
    rates.push_back( r );
    meanrate += ( r - meanrate )/(++j);
  }

  if ( sd != 0 ) {
    double var = 0.0;
    for ( unsigned int k=0; k<rates.size(); k++ ) {
      double s = rates[k] - meanrate;
      var += ( s*s - var )/(k+1);
    }
    *sd = ::sqrt( var );
  }

  return meanrate;
}


void EventList::rate( SampleDataD &rate, double width, double time ) const
{
  rate = 0.0;
  int trials = 0;
  addRate( rate, trials, width, time );
}


void EventList::addRate( SampleDataD &rate, int &trials, double width,
			 double time ) const
{
  for ( const_iterator i = begin(); i != end(); ++i )
    (*i)->addRate( rate, trials, width, time );
}


void EventList::cyclicRate( SampleDataD &rate, double width,
			    double time ) const
{
  rate = 0.0;
  int trials = 0;
  addCyclicRate( rate, trials, width, time );
}


void EventList::addCyclicRate( SampleDataD &rate, int &trials,
			       double width, double time ) const
{
  for ( const_iterator i = begin(); i != end(); ++i )
    (*i)->addCyclicRate( rate, trials, width, time );
}


void EventList::rate( SampleDataD &rate, const Kernel &kernel,
		      double time ) const
{
  rate = 0.0;
  int trials = 0;
  addRate( rate, trials, kernel, time );
}


void EventList::rate( SampleDataD &rate, SampleDataD &ratesd,
		      const Kernel &kernel, double time ) const
{
  rate = 0.0;
  ratesd = 0.0;

  vector< ArrayD > rates( rate.size(), ArrayD() );
  for ( unsigned int k=0; k<rates.size(); k++ )
    rates[k].reserve( size() );

  for ( const_iterator i = begin(); i != end(); ++i ) {
    SampleDataD s( rate.range() );
    (*i)->rate( s, kernel, time );
    for ( int k=0; k<s.size(); k++ )
      rates[k].push( s[k] );
  }

  for ( int k=0; k<rate.size(); k++ ) {
    double sd = 0.0;
    rate[k] = meanStdev( sd, rates[k] );
    ratesd[k] = sd;
  }

}


void EventList::addRate( SampleDataD &rate, int &trials,
			 const Kernel &kernel, double time ) const
{
  for ( const_iterator i = begin(); i != end(); ++i )
    (*i)->addRate( rate, trials, kernel, time );
}


void EventList::cyclicRate( SampleDataD &rate, const Kernel &kernel,
			    double time ) const
{
  rate = 0.0;
  int trials = 0;
  addCyclicRate( rate, trials, kernel, time );
}


void EventList::cyclicRate( SampleDataD &rate, SampleDataD &ratesd,
			    const Kernel &kernel, double time ) const
{
  rate = 0.0;
  ratesd = 0.0;

  vector< ArrayD > rates( rate.size(), ArrayD() );
  for ( int k=0; k<rate.size(); k++ )
    rates[k].reserve( size() );

  for ( const_iterator i = begin(); i != end(); ++i ) {
    SampleDataD s( rate.range() );
    (*i)->cyclicRate( s, kernel, time );
    for ( int k=0; k<s.size(); k++ )
      rates[k].push( s[k] );
  }

  for ( int k=0; k<rate.size(); k++ ) {
    double sd = 0.0;
    rate[k] = meanStdev( sd, rates[k] );
    ratesd[k] = sd;
  }
}


void EventList::addCyclicRate( SampleDataD &rate, int &trials,
			       const Kernel &kernel, double time ) const
{
  for ( const_iterator i = begin(); i != end(); ++i )
    (*i)->addCyclicRate( rate, trials, kernel, time );
}


void EventList::intervals( SampleData< ArrayD > &intervals,
			   double time ) const
{
  long k[ size() ];
  for ( int j=0; j<size(); j++ )
    k[j] = Events[j]->next( intervals.rangeFront() + time );
  for ( int i=0; i<intervals.size(); i++ ) {
    intervals[i].reserve( size() );
    intervals[i].clear();
    for ( int j=0; j<size(); j++ ) {
      for ( ; 
	    k[j] < Events[j]->size() && 
	      (*Events[j])[k[j]] < intervals.pos( i ) + time; 
	    k[j]++ );
      if ( k[j] < Events[j]->size() && k[j] > Events[j]->minEvent() ) {
	intervals[i].push( (*Events[j])[k[j]] - (*Events[j])[k[j]-1] );
      }
    }
  }
}


void EventList::intervalsAt( double time, ArrayD &intervals ) const
{
  intervals.reserve( size() );
  intervals.clear();
  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( time );
    if ( n < (*i)->size() && n > (*i)->minEvent() )
      intervals.push( (**i)[n] - (**i)[n-1] );
  }
}


double EventList::interval( double tbegin, double tend ) const
{
  double sumisi = 0.0;
  long nisi = 0;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( tbegin );
    long p = (*i)->previous( tend );
    
    if ( p > n ) {
      nisi += p-n;
      sumisi += (*(*i))[p] - (*(*i))[n];
    }
  }
  return nisi > 0 ? sumisi/nisi : 0.0;
}


double EventList::interval( double tbegin, double tend, double &sd ) const
{
  double sumisi = 0.0;
  long nisi = 0;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( tbegin );
    long p = (*i)->previous( tend );
    
    if ( p > n ) {
      nisi += p-n;
      sumisi += (*(*i))[p] - (*(*i))[n];
    }
  }
  double meanisi = nisi > 0 ? sumisi/nisi : 0.0;

  nisi = 0;
  double var = 0.0;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( tbegin );
    long p = (*i)->previous( tend );
    
    for ( n++; n <= p; n++ ) {
      nisi++;
      double s = ( (*(*i))[n] - (*(*i))[n-1] ) - meanisi;
      var += ( s*s - var )/nisi;
    }
  }
  sd = ::sqrt( var );

  return meanisi;
}


void EventList::interval( SampleDataD &intervals, double time ) const
{
  intervals = 0.0;
  long k[ size() ];
  for ( int j=0; j<size(); j++ )
    k[j] = Events[j]->next( intervals.rangeFront() + time );
  for ( int i=0; i<intervals.size(); i++ ) {
    int ij = 0;
    for ( int j=0; j<size(); j++ ) {
      for ( ; 
	    k[j] < Events[j]->size() && 
	      (*Events[j])[k[j]] < intervals.pos( i ) + time; 
	    k[j]++ );
      if ( k[j] < Events[j]->size() && k[j] > Events[j]->minEvent() ) {
	double f = 1.0 / ( (*Events[j])[k[j]] - (*Events[j])[k[j]-1] );
	intervals[i] += ( f - intervals[i] ) / (++ij);
      }
    }
    intervals[i] = 1.0 / intervals[i];
  }

}


void EventList::interval( SampleDataD &intervals, SampleDataD &sd,
			  double time ) const
{
  intervals = 0.0;
  sd = 0.0;
  long k[ size() ];
  for ( int j=0; j<size(); j++ )
    k[j] = Events[j]->next( intervals.rangeFront() + time );
  for ( int i=0; i<intervals.size(); i++ ) {
    int rj = 0;
    for ( int j=0; j<size(); j++ ) {
      for ( ; 
	    k[j] < Events[j]->size() && 
	      (*Events[j])[k[j]] < intervals.pos( i ) + time; 
	    k[j]++ );
      if ( k[j] < Events[j]->size() && k[j] > Events[j]->minEvent() ) {
	double T = (*Events[j])[k[j]] - (*Events[j])[k[j]-1];
	double f = 1.0 / T;
	sd[i] += ( T - sd[i] ) / (++rj);
	intervals[i] += ( f - intervals[i] ) / rj;
      }
    }
    double x = intervals[i]*sd[i] - 1.0;
    if ( x <= 0.0 )
      x = 0.0;
    intervals[i] = 1.0 / intervals[i];
    sd[i] = intervals[i] * ::sqrt( x );
  }
}


void EventList::intervalCV( SampleDataD &intervals, SampleDataD &cv,
			    double time ) const
{
  intervals = 0.0;
  cv = 0.0;
  long k[ size() ];
  for ( int j=0; j<size(); j++ )
    k[j] = Events[j]->next( intervals.rangeFront() + time );
  for ( int i=0; i<intervals.size(); i++ ) {
    int rj = 0;
    for ( int j=0; j<size(); j++ ) {
      for ( ; 
	    k[j] < Events[j]->size() && 
	      (*Events[j])[k[j]] < intervals.pos( i ) + time; 
	    k[j]++ );
      if ( k[j] < Events[j]->size() && k[j] > Events[j]->minEvent() ) {
	double T = (*Events[j])[k[j]] - (*Events[j])[k[j]-1];
	double f = 1.0 / T;
	cv[i] += ( T - cv[i] ) / (++rj);
	intervals[i] += ( f - intervals[i] ) / rj;
      }
    }
    double x = intervals[i]*cv[i] - 1.0;
    if ( x <= 0.0 )
      x = 0.0;
    intervals[i] = 1.0 / intervals[i];
    cv[i] = ::sqrt( x );
  }
}


void EventList::cyclicInterval( SampleDataD &intervals, double time ) const
{
  intervals = 0.0;
  int trials = 0;
  for ( const_iterator i = begin(); i != end(); ++i )
    (*i)->addCyclicInterval( intervals, trials, time );
}


double EventList::intervalAt( double time ) const
{
  double meanf = 0.0;
  int nf = 0;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( time );
    if ( n < (*i)->size() && n > (*i)->minEvent() ) {
      double f = 1.0 / ( (*(*i))[n] - (*(*i))[n-1] );
      meanf += ( f - meanf )/(++nf);
    }
  }
  return 1.0 / meanf;
}


double EventList::intervalAt( double time, double &sd ) const
{
  double meanisi = 0.0;
  double meanf = 0.0;
  int nf = 0;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( time );
    if ( n < (*i)->size() && n > (*i)->minEvent() ) {
      double T = (*(*i))[n] - (*(*i))[n-1];
      double f = 1.0 / T;
      meanisi += ( T - meanisi )/(++nf);
      meanf += ( f - meanf )/nf;
    }
  }
  double meaninterval = 1.0 / meanf;

  double x = meanf*meanisi - 1.0;
  if ( x <= 0.0 )
    x = 0.0;
  sd = meaninterval * ::sqrt( x );

  return meaninterval;
}


double EventList::frequency( double tbegin, double tend ) const
{
  double sumisi = 0.0;
  long nisi = 0;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( tbegin );
    long p = (*i)->previous( tend );
    
    if ( p > n ) {
      nisi += p - n;
      sumisi += (*(*i))[p] - (*(*i))[n];
    }
  }
  return sumisi > 0 ? nisi/sumisi : 0.0;
}


double EventList::frequency( double tbegin, double tend, double &sd ) const
{
  double sumisi = 0.0;
  long nisi = 0;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( tbegin );
    long p = (*i)->previous( tend );
    
    if ( p > n ) {
      nisi += p - n;
      sumisi += (*(*i))[p] - (*(*i))[n];
    }
  }
  double meanisi = nisi > 0 ? sumisi/nisi : 0.0;

  nisi = 0;
  double var = 0.0;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( tbegin );
    long p = (*i)->previous( tend );
    
    for ( n++; n <= p; n++ ) {
      nisi++;
      double s = ( (*(*i))[n] - (*(*i))[n-1] ) - meanisi;
      var += ( s*s - var )/nisi;
    }
  }
  sd = meanisi > 0.0 ? ::sqrt( var )/meanisi/meanisi : 0.0;

  return meanisi > 0.0 ? 1.0/meanisi : 0.0;
}


void EventList::frequency( SampleDataD &rate, double time ) const
{
  rate = 0.0;
  long k[ size() ];
  for ( int j=0; j<size(); j++ )
    k[j] = Events[j]->next( rate.rangeFront() + time );
  for ( int i=0; i<rate.size(); i++ ) {
    int rj = 0;
    for ( int j=0; j<size(); j++ ) {
      for ( ; 
	    k[j] < Events[j]->size() && 
	      (*Events[j])[k[j]] < rate.pos( i ) + time; 
	    k[j]++ );
      if ( k[j] < Events[j]->size() && k[j] > Events[j]->minEvent() ) {
	double f = 1.0 / ( (*Events[j])[k[j]] - (*Events[j])[k[j]-1] );
	rate[i] += ( f - rate[i] ) / (++rj);
      }
    }
  }
}


void EventList::frequency( SampleDataD &rate, SampleDataD &sd,
			   double time ) const
{
  rate = 0.0;
  sd = 0.0;
  long k[ size() ];
  for ( int j=0; j<size(); j++ )
    k[j] = Events[j]->next( rate.rangeFront() + time );
  for ( int i=0; i<rate.size(); i++ ) {
    int rj = 0;
    for ( int j=0; j<size(); j++ ) {
      for ( ; 
	    k[j] < Events[j]->size() && 
	      (*Events[j])[k[j]] < rate.pos( i ) + time; 
	    k[j]++ );
      if ( k[j] < Events[j]->size() && k[j] > Events[j]->minEvent() ) {
	double T = (*Events[j])[k[j]] - (*Events[j])[k[j]-1];
	double f = 1.0 / T;
	sd[i] += ( T - sd[i] ) / (++rj);
	rate[i] += ( f - rate[i] ) / rj;
      }
    }
    double x = rate[i]*sd[i] - 1.0;
    if ( x <= 0.0 )
      x = 0.0;
    sd[i] = rate[i] * ::sqrt( x );
  }
}


void EventList::frequencyCV( SampleDataD &rate, SampleDataD &cv,
			     double time ) const
{
  rate = 0.0;
  cv = 0.0;
  long k[ size() ];
  for ( int j=0; j<size(); j++ )
    k[j] = Events[j]->next( rate.rangeFront() + time );
  for ( int i=0; i<rate.size(); i++ ) {
    int rj = 0;
    for ( int j=0; j<size(); j++ ) {
      for ( ; 
	    k[j] < Events[j]->size() && 
	      (*Events[j])[k[j]] < rate.pos( i ) + time; 
	    k[j]++ );
      if ( k[j] < Events[j]->size() && k[j] > Events[j]->minEvent() ) {
	double T = (*Events[j])[k[j]] - (*Events[j])[k[j]-1];
	double f = 1.0 / T;
	cv[i] += ( T - cv[i] ) / (++rj);
	rate[i] += ( f - rate[i] ) / rj;
      }
    }
    double x = rate[i]*cv[i] - 1.0;
    if ( x <= 0.0 )
      x = 0.0;
    cv[i] = ::sqrt( x );
  }
}


void EventList::cyclicFrequency( SampleDataD &rate, double time ) const
{
  rate = 0.0;
  int trials = 0;
  for ( const_iterator i = begin(); i != end(); ++i )
    (*i)->addCyclicFrequency( rate, trials, time );
}


void EventList::cyclicFrequency( SampleDataD &rate, SampleDataD &sd,
				 double time ) const
{
  rate = 0.0;
  sd = 0.0;
  int trials = 0;
  for ( const_iterator i = begin(); i != end(); ++i )
    (*i)->addCyclicFrequency( rate, sd, trials, time );
  for ( int k=0; k<sd.size(); k++ ) {
    sd[k] = rate[k] * sqrt( rate[k]*sd[k] - 1.0 );
  }
}


double EventList::frequencyAt( double time ) const
{
  double meanf = 0.0;
  int nf = 0;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( time );
    if ( n < (*i)->size() && n > (*i)->minEvent() ) {
      double f = 1.0 / ( (*(*i))[n] - (*(*i))[n-1] );
      meanf += ( f - meanf )/(++nf);
    }
  }
  return meanf;
}


double EventList::frequencyAt( double time, double &sd ) const
{
  double meanisi = 0.0;
  double meanf = 0.0;
  int nf = 0;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( time );
    if ( n < (*i)->size() && n > (*i)->minEvent() ) {
      double T = (*(*i))[n] - (*(*i))[n-1];
      double f = 1.0 / T;
      meanisi += ( T - meanisi )/(++nf);
      meanf += ( f - meanf )/nf;
    }
  }

  double x = meanf*meanisi - 1.0;
  if ( x <= 0.0 )
    x = 0.0;
  sd = meanf * ::sqrt( x );

  return meanf;
}


void EventList::intervalHistogram( double tbegin, double tend, 
				   SampleDataD &hist ) const
{
  hist = 0.0;
  for ( const_iterator i = begin(); i != end(); ++i )
    (*i)->addIntervalHistogram( tbegin, tend, hist );
}


void EventList::directIntervalHistogram( double time,
					 SampleDataD &hist ) const
{
  hist = 0.0;

  // get intervals:
  ArrayD intervals;
  intervals.reserve( size() );
  for ( const_iterator i = begin(); i != end(); ++i ) {
    double isi = (*i)->intervalAt( time );
    if ( isi > 0.0 )
      intervals.push( isi );
  }

  // histogram:
  hist.hist( intervals );

  // normalize:
  double s = ::relacs::sum( hist );
  double norm = s > 0.0 ? 1.0 / s / hist.stepsize() : 1.0;
  hist *= norm;
}


void EventList::correctedIntervalHistogram( double time,
					    SampleDataD &hist ) const
{
  hist = 0.0;

  // get intervals:
  ArrayD intervals;
  intervals.reserve( size() );
  for ( const_iterator i = begin(); i != end(); ++i ) {
    double isi = (*i)->intervalAt( time );
    if ( isi > 0.0 )
      intervals.push( isi );
  }

  // histogram:
  hist.hist( intervals );

  // correct histogram:
  double sum = 0.0;
  for ( int k=0; k<hist.size(); k++ ) {
    hist[k] /= hist.pos( k ) + 0.5*hist.stepsize();
    sum += hist[k];
  }

  // normalize:
  double norm = sum > 0.0 ? 1.0 / sum / hist.stepsize() : 1.0;
  hist *= norm;
}


void EventList::serialCorr( double tbegin, double tend, ArrayD &sc ) const
{
  vector< ArrayD > scs( sc.size() );
  unsigned int k=0;
  for ( k=0; k<scs.size(); k++ )
    scs[k].reserve( size() );

  for ( const_iterator i = begin(); i != end(); ++i ) {
    (*i)->serialCorr( tbegin, tend, sc );
    for ( k=0; k<scs.size(); k++ )
      scs[k].push( sc[k] );
  }

  for ( k=0; k<scs.size(); k++ ) {
    sc[k] = scs[k].mean();
  }
}


void EventList::serialCorr( double tbegin, double tend, 
			    ArrayD &sc, ArrayD &sd ) const
{
  vector< ArrayD > scs( sc.size() );
  unsigned int k=0;
  for ( k=0; k<scs.size(); k++ )
    scs[k].reserve( size() );

  for ( const_iterator i = begin(); i != end(); ++i ) {
    (*i)->serialCorr( tbegin, tend, sc );
    for ( k=0; k<scs.size(); k++ )
      scs[k].push( sc[k] );
  }

  sd.resize( sc.size(), 0.0 );
  for ( k=0; k<scs.size(); k++ ) {
    sc[k] = scs[k].mean( sd[k] );
  }
}


double EventList::locking( double tbegin, double tend, 
			   double period, double &sd ) const
{
  sd = 0.0;
  ArrayD vl;
  vl.reserve( size() );
  for ( const_iterator i = begin(); i != end(); ++i ) {
     vl.push( (*i)->locking( tbegin, tend, period ) );
  }
  double mean = vl.mean( sd );
  return mean;
}


double EventList::vectorStrength( double tbegin, double tend, 
				  double period ) const
{
  double sc = 0.0;
  double ss = 0.0;
  int c = 0;

  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( tbegin );
    long p = (*i)->previous( tend );
    if ( p <= n || p < 0 )
      continue;
    
    for ( int k=n; k<=p; k++, c++ ) {
      double phi = 6.28318530717959 * ( (*(*i))[k] - tbegin ) / period;
      sc += ::cos( phi );
      ss += ::sin( phi );
    }
  }

  return ::sqrt( sc*sc + ss*ss )/c;
}


double EventList::vectorStrength( double tbegin, double tend, 
				  double period, double &sd ) const
{
  sd = 0.0;
  ArrayD vs;
  vs.reserve( size() );
  for ( const_iterator i = begin(); i != end(); ++i ) {
     vs.push( (*i)->vectorStrength( tbegin, tend, period ) );
  }
  double mean = vs.mean( sd );
  return mean;
}


double EventList::vectorPhase( double tbegin, double tend, 
			       double period ) const
{
  double sc = 0.0;
  double ss = 0.0;

  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( tbegin );
    long p = (*i)->previous( tend );
    if ( p <= n || p < 0 )
      continue;
    
    for ( int k=n; k<=p; k++ ) {
      double phi = 6.28318530717959 * ( (*(*i))[k] - tbegin ) / period;
      sc += ::cos( phi );
      ss += ::sin( phi );
    }
  }

  return ::atan2( ss, sc );
}


double EventList::vectorPhase( double tbegin, double tend, 
			       double period, double &sd ) const
{
  sd = 0.0;
  ArrayD vp;
  vp.reserve( size() );
  for ( const_iterator i = begin(); i != end(); ++i ) {
     vp.push( (*i)->vectorPhase( tbegin, tend, period ) );
  }
  double mean = vp.mean( sd );
  return mean;
}


double EventList::correlation( double tbegin, double tend, 
			       const Kernel &kernel, double dt,
			       double &sd ) const
{
  // convolve events with kernel:
  vector< SampleDataD > s( size(), SampleDataD( tbegin, tend, dt, 0 ) );
  int k=0;
  for ( const_iterator i = begin(); i != end(); ++i, ++k ) {
    (*i)->rate( s[k], kernel );
  }

  // pairwise correlations:
  vector< double > c;
  c.reserve( s.size() * ( s.size() - 1 ) / 2 );
  for ( unsigned int i=0; i<s.size(); i++ ) {
    for ( unsigned int j=i+1; j<s.size(); j++ ) {
      double rr = corrCoef( s[i], s[j] );
      c.push_back( rr );
    }
  }

  // return mean and standard deviation:
  double r = meanStdev( sd, c );
  return r;
}


double EventList::correlation( double tbegin, double tend, 
			       const Kernel &kernel, double dt ) const
{
  double sd = 0.0;
  return correlation( tbegin, tend, kernel, dt, sd );
}


double EventList::reliability( double tbegin, double tend, 
			       const Kernel &kernel, double dt,
			       double &sd ) const
{
  // convolve events with kernel:
  vector< SampleDataD > s( size(), SampleDataD( tbegin, tend, dt, 0 ) );
  int k=0;
  for ( const_iterator i = begin(); i != end(); ++i, ++k ) {
    (*i)->rate( s[k], kernel );
  }

  // magnitudes:
  vector< double > m( size() );
  for ( unsigned int i=0; i<s.size(); i++ ) {
    m[i] = magnitude( s[i] );
  }

  // pairwise correlations:
  vector< double > c;
  c.reserve( s.size() * ( s.size() - 1 ) / 2 );
  for ( unsigned int i=0; i<s.size(); i++ ) {
    for ( unsigned int j=i+1; j<s.size(); j++ ) {
      double rr = m[i] > 0.0 && m[j] > 0.0 ? dot( s[i], s[j] ) / m[i] / m[j] : 0.0;
      c.push_back( rr );
    }
  }

  // return mean and standard deviation:
  double r = meanStdev( sd, c );
  return r;
}


double EventList::reliability( double tbegin, double tend, 
			       const Kernel &kernel, double dt ) const
{
  double sd = 0.0;
  return reliability( tbegin, tend, kernel, dt, sd );
}


void EventList::coincidenceRate( SampleDataD &rate,  SampleDataD &ratesd, 
				 const Kernel &kernel )
{
  rate = 0.0;
  ratesd = 0.0;

  // convolve events with kernel:
  vector< SampleDataD > s( size(), rate.range() );
  int k=0;
  for ( const_iterator i = begin(); i != end(); ++i, ++k ) {
    (*i)->rate( s[k], kernel );
  }

  // pairwise products:
  vector< ArrayD > rate12( rate.size(), ArrayD() );
  for ( unsigned int k=0; k<rate12.size(); k++ )
    rate12[k].reserve( s.size() * ( s.size() - 1 ) / 2 );
  for ( unsigned int i=0; i<s.size(); i++ ) {
    for ( unsigned int j=i+1; j<s.size(); j++ ) {
      for ( int k=0; k<s[i].size(); k++ )
	rate12[k].push( s[i][k] * s[j][k] );
    }
  }

  for ( int k=0; k<rate.size(); k++ ) {
    double sd;
    //    rate[k] = meanStdev( sd, rate12[k] );
    //    ratesd[k] = sd;
    rate[k] = ::sqrt( meanStdev( sd, rate12[k] ) );
    ratesd[k] = 0.5*sd/rate[k];
  }
  
}


void EventList::average( double tbegin, double tend, const SampleDataD &trace,
			 SampleDataD &ave ) const
{
  ave = 0.0;

  // adjust range to trace:
  if ( trace.rangeFront() > tbegin )
    tbegin = trace.rangeFront();
  if ( trace.rangeBack() < tend )
    tend = trace.rangeBack();

  // adjust range to ave:
  if ( ave.rangeFront() < 0.0 )
    tbegin -= ave.rangeFront();
  if ( ave.rangeBack() > 0.0 )
    tend -= ave.rangeBack();

  if ( tend <= tbegin )
    return;

  int c = 0;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( tbegin );
    long p = (*i)->previous( tend );
    if ( p <= n || p < 0 )
      continue;
    
    for ( int k=n; k<=p; k++ ) {
      c++;
      for ( int j=0; j<ave.size(); j++ )
	ave[j] += ( trace[(*(*i))[k] + ave.pos(j)] - ave[j] ) / c;
    }  
  }
}


void EventList::average( double tbegin, double tend, const SampleDataD &trace,
			 SampleDataD &ave, SampleDataD &sd ) const
{
  ave = 0.0;
  sd = ave;

  // adjust range to trace:
  if ( trace.rangeFront() > tbegin )
    tbegin = trace.rangeFront();
  if ( trace.rangeBack() < tend )
    tend = trace.rangeBack();

  // adjust range to ave:
  if ( ave.rangeFront() < 0.0 )
    tbegin -= ave.rangeFront();
  if ( ave.rangeBack() > 0.0 )
    tend -= ave.rangeBack();

  if ( tend <= tbegin )
    return;

  int c = 0;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( tbegin );
    long p = (*i)->previous( tend );
    if ( p <= n || p < 0 )
      continue;
    
    for ( int k=n; k<=p; k++ ) {
      c++;
      for ( int j=0; j<ave.size(); j++ )
	ave[j] += ( trace[(*(*i))[k] + ave.pos(j)] - ave[j] ) / c;
    }
  }

  c = 0;
  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( tbegin );
    long p = (*i)->previous( tend );
    if ( p <= n || p < 0 )
      continue;
    
    for ( int k=n; k<=p; k++ ) {
      c++;
      for ( int j=0; j<sd.size(); j++ ) {
	double s = trace[(*(*i))[k] + ave.pos(j)] - ave[j];
	sd[j] += ( s*s - sd[j] ) / c;
      }
    }
  }

  for ( int j=0; j<sd.size(); j++ )
    sd[j] = ::sqrt( sd[j] );
}


void EventList::spectrum( double tbegin, double tend, double step,
			  SampleDataD &psd ) const
{
  psd = 0.0;
  SampleDataD p( psd.size(), 0.0 );
  SampleDataD rr( tbegin, tend, step );
  for ( int i=0; i<size(); i++ ) {
    (*this)[i].rate( rr );
    rr -= mean( rr );
    rPSD( rr, p );
    for ( int k=0; k<psd.size(); k++ )
      psd[k] += ( p[k] - psd[k] ) / (k+1);
  }
}


void EventList::spectrum( double tbegin, double tend, double step,
			  SampleDataD &psd, SampleDataD &sd ) const
{
  SampleDataD p( psd.size(), 0.0 );
  SampleDataD psq( psd.size(), 0.0 );
  SampleDataD rr( tbegin, tend, step );

  int n = 0;
  for ( int i=0; i<size(); i++ ) {
    (*this)[i].rate( rr );
    rr -= mean( rr );
    rPSD( rr, p );
    n++;
    for ( int k=0; k<psd.size(); k++ ) {
      psd[k] += ( p[k] - psd[k] ) / n;
      psq[k] += ( p[k]*p[k] - psq[k] ) / n;
    }
  }

  // standard deviation:
  for ( int k=0; k<psd.size(); k++ )
    sd[k] = ::sqrt( ::fabs( psq[k] - psd[k]*psd[k] ) );
}


void EventList::coherence( const SampleDataD &stimulus, SampleDataD &c ) const
{
  c = 0.0;
  SampleDataD cohere( c.size(), 0.0 );
  SampleDataD rr( stimulus.range() );
  for ( int i=0; i<size(); i++ ) {
    (*this)[i].rate( rr );
    rr -= mean( rr );
    ::relacs::coherence( stimulus, rr, cohere );
    for ( int k=0; k<c.size(); k++ )
      c[k] += ( cohere[k] - c[k] ) / (k+1);
  }
}


void EventList::coherence( const SampleDataD &stimulus,
			   SampleDataD &c, SampleDataD &sd ) const
{
  SampleDataD cohere( c.size(), 0.0 );
  SampleDataD csq( c.size(), 0.0 );
  SampleDataD rr( stimulus.range() );

  int n=0;
  for ( int i=0; i<size(); i++ ) {
    (*this)[i].rate( rr );
    rr -= mean( rr );
    ::relacs::coherence( stimulus, rr, cohere );
    n++;
    for ( int k=0; k<c.size(); k++ ) {
      c[k] += ( cohere[k] - c[k] ) / n;
      csq[k] += ( cohere[k]*cohere[k] - csq[k] ) / n;
    }
  }

  // standard deviation:
  for ( int k=0; k<c.size(); k++ )
    sd[k] = ::sqrt( ::fabs( csq[k] - c[k]*c[k] ) );
}


void EventList::coherence( double tbegin, double tend, double step,
			   SampleDataD &c ) const
{
  c = 0.0;
  // convolve events with kernel:
  vector< SampleDataD > rr( size(), SampleDataD( tbegin, tend, step, 0 ) );
  int k=0;
  for ( const_iterator i = begin(); i != end(); ++i, ++k ) {
    (*i)->rate( rr[k] );
  }

  // pairwise coherence:
  SampleDataD cohere( c.size(), 0.0 );
  int n=0;
  for ( unsigned int i=0; i<rr.size(); i++ ) {
    for ( unsigned int j=i+1; j<rr.size(); j++ ) {
      ::relacs::coherence( rr[i], rr[j], cohere );
      n++;
      for ( int k=0; k<c.size(); k++ )
	c[k] += ( cohere[k] - c[k] ) / n;
    }
  }
}


void EventList::coherence( double tbegin, double tend, double step,
			   SampleDataD &c, SampleDataD &sd ) const
{
  // convolve events with kernel:
  vector< SampleDataD > rr( size(), SampleDataD( tbegin, tend, step, 0 ) );
  int k=0;
  for ( const_iterator i = begin(); i != end(); ++i, ++k ) {
    (*i)->rate( rr[k] );
  }

  // pairwise coherence:
  SampleDataD cohere( c.size(), 0.0 );
  SampleDataD csq( c.size(), 0.0 );
  int n=0;
  for ( unsigned int i=0; i<rr.size(); i++ ) {
    for ( unsigned int j=i+1; j<rr.size(); j++ ) {
      ::relacs::coherence( rr[i], rr[j], cohere );
      n++;
      for ( int k=0; k<c.size(); k++ ) {
	c[k] += ( cohere[k] - c[k] ) / n;
	csq[k] += ( cohere[k]*cohere[k] - csq[k] ) / n;
      }
    }
  }

  // standard deviation:
  for ( int k=0; k<c.size(); k++ )
    sd[k] = ::sqrt( ::fabs( csq[k] - c[k]*c[k] ) );
}


double EventList::latency( double time, double *sd ) const
{
  double mean = 0.0;
  int nn = 0;

  for ( const_iterator i = begin(); i != end(); ++i ) {
    long n = (*i)->next( time );
    if ( n < (*i)->size() && n >= (*i)->minEvent() ) {
      double l = (*(*i))[n] - time;
      mean += ( l - mean )/(++nn);
    }
  }

  if ( sd != 0 ) {
    double var = 0.0;
    nn = 0;
    for ( const_iterator i = begin(); i != end(); ++i ) {
      long n = (*i)->next( time );
      if ( n < (*i)->size() && n >= (*i)->minEvent() ) {
	double l = (*(*i))[n] - time;
	double s = l - mean;
	var += ( s*s - var )/(++nn);
      }
    }
    *sd = sqrt( var );
  }

  return mean;
}


void EventList::sum( EventData &all )
{
  all.clear();
  if ( empty() )
    return;

  // init:
  double offs = front().offset();
  double step = front().stepsize();
  double back = front().rangeBack();
  int n = front().size();
  for ( int i=1; i<size(); i++ ) {
    if ( (*this)[i].offset() < offs )
      offs = (*this)[i].offset();
    if ( (*this)[i].rangeBack() > back )
      back = (*this)[i].rangeBack();
    if ( (*this)[i].stepsize() < step )
      step = (*this)[i].stepsize();
    n += (*this)[i].size();
  }

  // sum up:
  ArrayD a;
  a.reserve( n );
  for ( int i=0; i<size(); ++i ) {
    for ( int k=0; k<(*this)[i].size(); k++ ) {
      a.push( (*this)[i][k] );
    }
  }
  sort( a.begin(), a.end() );

  all.assign( a, offs, back, step );
}


void EventList::sync( EventData &s, double bin, double p, bool keep )
{
  s.clear();
  if ( empty() )
    return;

  // init:
  double offs = front().offset();
  double step = front().stepsize();
  double rback = front().rangeBack();
  int n = front().size();
  for ( int i=1; i<size(); i++ ) {
    if ( (*this)[i].offset() < offs )
      offs = (*this)[i].offset();
    if ( (*this)[i].rangeBack() > rback )
      rback = (*this)[i].rangeBack();
    if ( (*this)[i].stepsize() < step )
      step = (*this)[i].stepsize();
    if ( n > (*this)[i].size() )
      n = (*this)[i].size();
  }
  s.setOffset( offs );
  s.setStepsize( step );
  s.setRangeBack( rback );
  s.reserve( n );
  
  double tstart = offs;
  double tend = rback;
  for ( int i=1; i<size(); i++ ) {
    if ( (*this)[i].size() > 0 ) {
      if ( (*this)[i].front() < tstart )
	tstart = (*this)[i].front();
      if ( (*this)[i].back() > tend )
	tend = (*this)[i].back();
    }
  }

  // init iterators:
  vector< EventData::const_iterator > inx( size() );
  vector< EventData::const_iterator > last( size() );
  for ( int k=0; k<size(); k++ ) {
    inx[k] = (*this)[k].begin();
    last[k] = (*this)[k].end();
  }

  // select synchronous events:
  int min = (int)::rint( p*size() );
  if ( min < 1 )
    min = 1;
  double t0 = ::floor( tstart/bin ) * bin;
  double t1 = t0;
  for ( int i=1; t1<=tend; i++ ) {
    double t2=t0+i*bin;
    int c=0;
    double ts = t1;
    for ( int k=0; k<size(); k++ ) {
      while ( inx[k] < last[k] &&
	      *inx[k] < t1 )
	++inx[k];
      if ( inx[k] < last[k] &&
	   *inx[k] < t2 ) {
	if ( c==0 )
	  ts = *inx[k];
	++c;
      }
    }
    if ( c >= min )
      s.push( keep ? ts : t1 );
    t1 = t2;
  }

}


void EventList::poisson( int trials, double rate, double refract,
			 double duration, RandomBase &random )
{
  clear();
  resize( trials );
  for ( iterator iter=begin(); iter != end(); ++iter )
    (*iter)->poisson( rate, refract, duration, random );
}


void EventList::saveText( ostream &os, double tfac,
			  int width, int prec, char frmt,
			  int sep, const string &noevents ) const
{
  for ( const_iterator i = begin(); i != end(); ++i ) {
    (*i)->saveText( os, tfac, width, prec, frmt, noevents );
    for ( int k=0; k<sep; k++ )
      os << '\n';
  }
}


void EventList::savePoint( ostream &os, double y, double tfac,
			   int width, int prec, char frmt,
			   int sep, const string &noevents, double noy ) const
{
  for ( const_iterator i = begin(); i != end(); ++i ) {
    (*i)->savePoint( os, y, tfac, width, prec, frmt, noevents, noy );
    for ( int k=0; k<sep; k++ )
      os << '\n';
  }
}


void EventList::saveStroke( ostream &os, int offs, double tfac, 
			    int width, int prec, char frmt, int sep,
			    double lower, double upper,
			    const string &noevents, double noy ) const
{
  for ( const_iterator i = begin(); i != end(); ++i ) {
    (*i)->saveStroke( os, offs, tfac, width, prec, frmt,
		      lower, upper, noevents, noy );
    for ( int k=0; k<sep; k++ )
      os << '\n';
    offs++;
  }
}


void EventList::saveBox( ostream &os, double bin, int offs, double tfac, 
			 int width, int prec, char frmt, int sep,
			 double lower, double upper, 
			 const string &noevents, double noy ) const
{
  for ( const_iterator i = begin(); i != end(); ++i ) {
    (*i)->saveBox( os, bin, offs, tfac, width, prec, frmt,
		   lower, upper, noevents, noy );
    for ( int k=0; k<sep; k++ )
      os << '\n';
    offs++;
  }
}


ostream &operator<< ( ostream &str, const EventList &events )
{
  for ( int k=0; k<events.size(); k++ ) {
    str << "EventData " << k << ":" << '\n';
    str << events[ k ] << '\n';
  }

  return str;
}


}; /* namespace relacs */

