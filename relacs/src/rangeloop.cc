/*
  rangeloop.cc
  A flexible and sophisticated way to loop trough a range of values.

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

#include <cstdlib>
#include <ctime>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <relacs/random.h>
#include <relacs/strqueue.h>
#include <relacs/rangeloop.h>

namespace relacs {


string RangeLoop::SequenceStrings = "Up|Down|AlternateInUp|AlternateInDown|AlternateOutUp|AlternateOutDown|Random|PseudoRandom";


RangeLoop::RangeLoop( void )
  : Index( 0 ), StartPos( 0 ), 
    Repeat( 1 ), RepeatCount( 0 ),
    BlockRepeat( 1 ), BlockRepeatCount( 0 ),
    SingleRepeat( 1 ), SingleRepeatCount( 0 ),
    Increment( 1 ), CurrentIncrement( 1 ),
    Seq( Up ), AddMethod( Add ), StepFac( 0.0 )
{
  Elements.clear();
  Indices.clear();
}


RangeLoop::RangeLoop( double first, double last, double step,
		      int repeat, int blockrepeat, int singlerepeat,
		      int increment )
{
  set( first, last, step, repeat, blockrepeat, singlerepeat, increment );
}


RangeLoop::RangeLoop( double first, double last, int n,
		      int repeat, int blockrepeat, int singlerepeat,
		      int increment )
{
  set( first, last, n, repeat, blockrepeat, singlerepeat, increment );
}


RangeLoop::RangeLoop( double value, int size,
		      int repeat, int blockrepeat, int singlerepeat,
		      int increment )
{
  set( value, size, repeat, blockrepeat, singlerepeat, increment );
}


RangeLoop::RangeLoop( const string &range, double scale )
{
  set( range, scale );
}


RangeLoop::~RangeLoop( void )
{
}


int RangeLoop::size( void ) const
{ 
  return Elements.size();
}


bool RangeLoop::empty( void ) const
{ 
  return ( Elements.size() == 0 );
}


void RangeLoop::resize( int newsize, double dflt )
{
  Elements.resize( newsize, ElementType( dflt ) );
  StepFac = 0.0;
}


void RangeLoop::clear( void )
{ 
  resize( 0 );
}


int RangeLoop::capacity( void ) const
{ 
  return Elements.capacity();
}


void RangeLoop::reserve( int newsize )
{
  Elements.reserve( newsize );
}


void RangeLoop::set( double first, double last, double step,
		     int repeat, int blockrepeat, int singlerepeat,
		     int increment )
{
  Elements.clear();

  add( first, last, step );

  Index = 0;

  Repeat = repeat;
  RepeatCount = 0;

  BlockRepeat = blockrepeat;
  BlockRepeatCount = 0;

  SingleRepeat = singlerepeat;
  SingleRepeatCount = 0;

  setIncrement( increment );
  CurrentIncrement = Increment;

  Seq = Up;
  AddMethod = Add;
}


void RangeLoop::add( double first, double last, double step )
{
  int oldsize = Elements.size();

  if ( first == last )
    add( first );
  else if ( first < last && step > 0.0 ) {
    int n = (int)floor( ( last - first ) / step ) + 1;
    Elements.reserve( Elements.size() + n );
    double v = first;
    int k=0;
    do {
      add( v );
      k++;
      v = first + k*step;
    } while ( v <= last );
  }
  else if ( first > last && step < 0.0 ) {
    int n = (int)floor( ( last - first ) / step ) + 1;
    Elements.reserve( Elements.size() + n );
    double v = first;
    int k=0;
    do {
      add( v );
      k++;
      v = first + k*step;
    } while ( v >= last );
  }

  if ( oldsize == 0 )
    StepFac = step;
  else if ( StepFac <= 0.0 || fabs( StepFac - step ) > 1.0e-8 )
    StepFac = 0.0;
}


void RangeLoop::setLog( double first, double last, double fac,
			int repeat, int blockrepeat, int singlerepeat,
			int increment )
{
  Elements.clear();

  addLog( first, last, fac );

  Index = 0;

  Repeat = repeat;
  RepeatCount = 0;

  BlockRepeat = blockrepeat;
  BlockRepeatCount = 0;

  SingleRepeat = singlerepeat;
  SingleRepeatCount = 0;

  setIncrement( increment );
  CurrentIncrement = Increment;

  Seq = Up;
  AddMethod = Add;
}


void RangeLoop::addLog( double first, double last, double fac )
{
  int oldsize = Elements.size();

  if ( first == last )
    add( first );
  else if ( first < last && fac > 1.0 ) {
    int n = (int)( 1 + log(last/first)/log(fac) );
    Elements.reserve( Elements.size() + n );
    double v = first;
    do {
      add( v );
      v *= fac;
    } while ( v <= last );
  }    
  else if ( first > last && fac > 0.0 && fac < 1.0 ) {
    int n = (int)( 1 + log(last/first)/log(fac) );
    Elements.reserve( Elements.size() + n );
    double v = first;
    do {
      add( v );
      v *= fac;
    } while ( v >= last );
  }    

  if ( oldsize == 0 )
    StepFac = -fac;
  else if ( StepFac >= 0.0 || fabs( -StepFac - fac ) > 1.0e-8 )
    StepFac = 0.0;
}


void RangeLoop::set( double first, double last, int n,
		     int repeat, int blockrepeat, int singlerepeat,
		     int increment )
{
  Elements.clear();

  add( first, last, n );

  Index = 0;

  Repeat = repeat;
  RepeatCount = 0;

  BlockRepeat = blockrepeat;
  BlockRepeatCount = 0;

  SingleRepeat = singlerepeat;
  SingleRepeatCount = 0;

  setIncrement( increment );
  CurrentIncrement = Increment;

  Seq = Up;
  AddMethod = Add;
}


void RangeLoop::add( double first, double last, int n )
{
  int oldsize = Elements.size();

  if ( n == 1 )
    add( first );
  else if ( n > 1 ) {
    Elements.reserve( Elements.size() + n );
    double step = ( last - first ) / ( n - 1 );
    for ( int k=0; k<n; k++ )
      add( first + k*step );
    if ( oldsize == 0 )
      StepFac = step;
    else if ( StepFac <= 0.0 || fabs( StepFac - step ) > 1.0e-8 )
      StepFac = 0.0;
  }
}


void RangeLoop::setLog( double first, double last, int n,
			int repeat, int blockrepeat, int singlerepeat,
			int increment )
{
  Elements.clear();

  addLog( first, last, n );

  Index = 0;

  Repeat = repeat;
  RepeatCount = 0;

  BlockRepeat = blockrepeat;
  BlockRepeatCount = 0;

  SingleRepeat = singlerepeat;
  SingleRepeatCount = 0;

  setIncrement( increment );
  CurrentIncrement = Increment;

  Seq = Up;
  AddMethod = Add;
}


void RangeLoop::addLog( double first, double last, int n )
{
  int oldsize = Elements.size();

  if ( n == 1 )
    add( first );
  else if ( n > 1 ) {
    Elements.reserve( Elements.size() + n );
    double fac = exp( log(last/first) / (n-1) );
    double v = first;
    for ( int k=0; k<n; k++ ) {
      add( v );
      v *= fac;
    }
    if ( oldsize == 0 )
      StepFac = -fac;
    else if ( StepFac >= 0.0 || fabs( -StepFac - fac ) > 1.0e-8 )
      StepFac = 0.0;
  }
}


void RangeLoop::set( double value, int size,
		     int repeat, int blockrepeat, int singlerepeat,
		     int increment )
{
  if ( size < 1 )
    size = 1;
  Elements.clear();
  Elements.reserve( size );
  Elements.push_back( ElementType( value ) );

  Index = 0;

  Repeat = repeat;
  RepeatCount = 0;

  BlockRepeat = blockrepeat;
  BlockRepeatCount = 0;

  SingleRepeat = singlerepeat;
  SingleRepeatCount = 0;

  setIncrement( increment );
  CurrentIncrement = Increment;

  Seq = Up;
  AddMethod = Add;
  StepFac = 0.0;
}


bool RangeLoop::add( double value )
{
  StepFac = 0.0;
  if ( AddMethod != Add ) {
    // check whether the element already exist:
    bool elemexist = false;
    for ( unsigned int k=0; k < Elements.size(); ) {
      if ( Elements[k].Value == value ) {
	if ( AddMethod == RemoveAll || AddMethod == AddRemove )
	  Elements.erase( Elements.begin() + k );
	else
	  k++;
	elemexist = true;
      }
      else
	k++;
    }
    if ( elemexist && AddMethod != AddRemove )
      return false;
  }

  Elements.push_back( ElementType( value ) );
  return true;
}


void RangeLoop::set( const string &range, double scale )
{
  Elements.clear();

  Index = 0;

  Repeat = 1;
  RepeatCount = 0;

  BlockRepeat = 1;
  BlockRepeatCount = 0;

  SingleRepeat = 1;
  SingleRepeatCount = 0;

  Increment = 1;
  CurrentIncrement = 1;

  Seq = Up;
  AddMethod = Add;

  // loop through comma separated elements:
  StrQueue se( range, "," );
  Elements.reserve( se.size() );
  for ( StrQueue::iterator sp = se.begin(); sp != se.end(); ++sp ) {
    (*sp).strip();
    if ( (*sp).empty() )
      continue;

    if ( Str::FirstNumber.findFirst( (*sp)[0] ) >= 0 ) {
      // get range elements:
      StrQueue sv;
      bool log = false;
      int index = 0;
      for ( int k=0; k<3; k++ ) {
	int ri = (*sp).find( "..", index );
	int incr = 2;
	if ( k == 1 ) {
	  int rif = (*sp).find( "*", index );
	  if ( rif >= 0 )
	    log = true;
	  if ( log && ( ri < 0 || rif < ri ) ) {
	    ri = rif;
	    incr = 1;
	  }
	}
	if ( ri >= 0 ) {
	  sv.add( (*sp).mid( index, ri-1 ) );
	  index = ri += incr;
	}
	else {
	  sv.add( (*sp).mid( index ) );
	  break;
	}
      }

      if ( sv.size() == 1 ) {
	// single element:
	add( sv[0].number() );
      }
      else if ( sv.size() > 1 ) {
	// range of elements:
	double first = sv[0].number();
	double last = sv[1].number();
	double step = 1.0;
	if ( sv.size() > 2 )
	  step = sv[2].number();
	if ( log )
	  addLog( first, last, step );
	else 
	  add( first, last, step );
      }
    }

    // modifiers:
    else {
      if ( (*sp).size() > 2 && (*sp)[0] == 'i' && (*sp)[1] == ':' ) {
	setIncrement( int( (*sp).number( 1.0, 2 ) ) );
	CurrentIncrement = Increment;
      }
      else {
	static const string seqstrs[8] = 
	  { "up", "down",
	    "alternateinup", 
	    "alternateindown",
	    "alternateoutup", "alternateoutdown", 
	    "random", "pseudorandom" };
	(*sp).lower();
	for ( int k=0; k<8; k++ ) {
	  if ( *sp == seqstrs[k] ) {
	    Seq = Sequence( k );
	    break;
	  }
	}
      }
    }

  }

  for ( unsigned int k=0; k<Elements.size(); k++ )
    Elements[k].Value *= scale;
}


const RangeLoop &RangeLoop::operator=( const string &range )
{
  set( range );
  return *this;
}


RangeLoop::AddMode RangeLoop::addMode( void ) const
{
  return AddMethod;
}


void RangeLoop::setAddMode( AddMode addmode )
{
  AddMethod = addmode;
}


int RangeLoop::repeat( void ) const
{
  return Repeat;
}


void RangeLoop::setRepeat( int repeat )
{
  Repeat = repeat;
}


int RangeLoop::currentRepetition( void ) const
{
  return RepeatCount;
}


int RangeLoop::blockRepeat( void ) const
{ 
  return BlockRepeat;
}


void RangeLoop::setBlockRepeat( int repeat )
{ 
  BlockRepeat = repeat;
}


int RangeLoop::currentBlockRepetition( void ) const
{ 
  return BlockRepeatCount;
}


bool RangeLoop::finishedBlock( void ) const
{ 
  return ( BlockRepeatCount == 0 && Index == 0 && SingleRepeatCount == 0 );
}


int RangeLoop::singleRepeat( void ) const
{ 
  return SingleRepeat;
}


void RangeLoop::setSingleRepeat( int repeat )
{ 
  SingleRepeat = repeat;
}


int RangeLoop::currentSingleRepetition( void ) const
{ 
  return SingleRepeatCount;
}


bool RangeLoop::finishedSingle( void ) const
{ 
  return ( SingleRepeatCount == 0 );
}


bool RangeLoop::lastSingle( void ) const
{ 
  return ( SingleRepeatCount >= SingleRepeat-1 );
}


void RangeLoop::setIncrement( int increment )
{ 
  if ( increment > 0 )
    Increment = increment;
  else {
    setLargeIncrement();
    for ( int k=0; k<abs( increment ); k++ ) {
      Increment /= 2;
      if ( Increment < 1 ) {
	Increment = 1;
	break;
      }
    }
  }
}


void RangeLoop::setLargeIncrement( void )
{
  for ( Increment=1; Increment*4 <= (int)Elements.size(); Increment *= 2 ); 
}


int RangeLoop::currentIncrement( void ) const
{ 
  return CurrentIncrement;
}


double RangeLoop::currentIncrementValue( void ) const
{ 
  if ( StepFac > 1.0e-8 )
    return CurrentIncrement*StepFac;
  else if ( StepFac < -1.0e-8 ) {
    double v = 1.0;
    for ( int k=0; k<CurrentIncrement; k++ )
      v *= StepFac;
    return v;
  }
  else
    return 0.0;
}


int RangeLoop::totalCount( void ) const
{ 
  int c = 0;
  for ( unsigned int k=0; k < Elements.size(); k++ )
    c += Elements[k].Count;
  return c;
}


int RangeLoop::maxCount( void ) const
{ 
  return Repeat*BlockRepeat*SingleRepeat;
}


int RangeLoop::maxBlockCount( void ) const
{ 
  return RepeatCount*BlockRepeat*SingleRepeat + BlockRepeat*SingleRepeat;
}


int RangeLoop::remainingCount( void ) const
{
  int mc = Repeat*BlockRepeat*SingleRepeat;
  if ( Repeat == 0 )
    mc = BlockRepeat*SingleRepeat;
  int c = 0;
  for ( unsigned int k = 0; k < Elements.size(); k++ ) {
    if ( ! Elements[k].Skip ) {
      int r = mc - Elements[k].Count;
      if ( r > 0 )
	c += r;
    }
  }
  return c;
}


int RangeLoop::remainingBlockCount( void ) const
{
  int mc = BlockRepeat*SingleRepeat;
  int c = 0;
  for ( unsigned int k = 0; k < Elements.size(); k++ ) {
    if ( ! Elements[k].Skip ) {
      int r = mc - Elements[k].Count;
      if ( r > 0 )
	c += r;
    }
  }
  return c;
}


void RangeLoop::setSequence( Sequence seq )
{ 
  Seq = seq;
}


void RangeLoop::up( void )
{ 
  setSequence( Up );
}


void RangeLoop::down( void )
{ 
  setSequence( Down );
}


void RangeLoop::alternateInUp( void )
{ 
  setSequence( AlternateInUp );
}


void RangeLoop::alternateInDown( void )
{ 
  setSequence( AlternateInDown );
}


void RangeLoop::alternateOutUp( void )
{ 
  setSequence( AlternateOutUp );
}


void RangeLoop::alternateOutDown( void )
{ 
  setSequence( AlternateOutDown );
}


void RangeLoop::random( void )
{ 
  setSequence( Random );
}


void RangeLoop::pseudoRandom( void )
{ 
  setSequence( PseudoRandom );
}


const string &RangeLoop::sequenceStrings( void )
{ 
  return SequenceStrings;
}


int RangeLoop::sequenceSize( void ) const
{ 
  return Indices.size();
}


void RangeLoop::initSequence( int pos )
{
  // indices:
  Indices.clear();
  Index = 0;
  StartPos = pos;

  // indices for sequence:
  int count = maxBlockCount();
  vector < int > einx;
  einx.reserve( Elements.size()/CurrentIncrement + 1 );
  for ( unsigned int k=0; k<Elements.size(); k+=CurrentIncrement )
    if ( !Elements[k].Skip &&
	 Elements[k].Count < count ) {
      einx.push_back( k );
    }

  if ( einx.empty() )
    return;

  // default start position:
  if ( pos < 0 ) {

    switch ( Seq ) {

    case AlternateInUp:
    case AlternateInDown:
    case AlternateOutUp:
    case AlternateOutDown:
      pos = einx[einx.size() / 2];
      break;

    case Random:
    case PseudoRandom:
    case Down:
    case Up:
    default:
      pos = 0;

    }
  }

  // start index:
  int si = 0;
  int min = Elements.size();
  for ( unsigned int j=0; j<einx.size(); j++ ) {
    if ( abs( long( einx[j] - pos ) ) < min ) {
      si = j;
      min = abs( long( einx[j] - pos ) );
    }
  }

  // new sequence:
  Indices.reserve( einx.size() );

  switch ( Seq ) {

  case Down: {
    int j = si - 1;
    for ( unsigned int k=0; k<einx.size(); k++, j-- ) {
      if ( j < 0 )
	j = einx.size() - 1;
      Indices.push_back( einx[ j ] );
    }
    break;
  }

  case AlternateInUp: {
    unsigned int j=0;
    for ( unsigned int k=0; k<einx.size(); k++, j++ ) {
      int i = ( j%2 == 0 ? j/2 : einx.size() - 1 - j/2 );
      if ( ( j%2 == 0 && i > si ) ||
	   ( j%2 == 1 && i < si ) ) {
	k--;
	continue;
      }
      Indices.push_back( einx[ i ] );
    }
    break;
  }

  case AlternateInDown: {
    unsigned int j=0;
    for ( unsigned int k=0; k<einx.size(); k++, j++ ) {
      int i = ( j%2 == 1 ? j/2 : einx.size() - 1 - j/2 );
      if ( ( j%2 == 1 && i > si ) ||
	   ( j%2 == 0 && i < si ) ) {
	k--;
	continue;
      }
      Indices.push_back( einx[ i ] );
    }
    break;
  }

  case AlternateOutUp: {
    unsigned int j=0;
    for ( unsigned int k=0; k<einx.size(); k++, j++ ) {
      int i = si + ( j%2 == 1 ? (j+1)/2 : -int(j+1)/2 );
      if ( i < 0 || i >= (int)einx.size() ) {
	k--;
	continue;
      }
      Indices.push_back( einx[ i ] );
    }
    break;
  }

  case AlternateOutDown: {
    unsigned int j=0;
    for ( unsigned int k=0; k<einx.size(); k++, j++ ) {
      int i = si + ( j%2 == 0 ? (j+1)/2 : -int(j+1)/2 );
      if ( i < 0 || i >= (int)einx.size() ) {
	k--;
	continue;
      }
      Indices.push_back( einx[ i ] );
    }
    break;
  }

  case Random: {
    ::relacs::Random r( (unsigned long)0 );
    for ( unsigned int k=0; k<einx.size(); k++ ) {
      int inx = 0;
      unsigned int j = 0;
      do {
	inx = (int)floor( einx.size() * r() );
	if ( inx >= 0 && inx < (int)einx.size() )
	  for ( j=0; j<k && Indices[j] != einx[ inx ]; j++ );
      } while ( j < k ||
		inx >= (int)einx.size() );
      Indices.push_back( einx[ inx ] );
    }
    break;
  }

  case PseudoRandom: {
    ::relacs::Random r( (unsigned long)87342 );
    for ( unsigned int k=0; k<einx.size(); k++ ) {
      int inx = 0;
      unsigned int j = 0;
      do {
	inx = (int)floor( einx.size() * r() );
	if ( inx >= 0 && inx < (int)einx.size() )
	  for ( j=0; j<k && Indices[j] != einx[ inx ]; j++ );
      } while ( j < k ||
		inx >= (int)einx.size() );
      Indices.push_back( einx[ inx ] );
    }
    break;
  }

  case Up:
  default: {
    unsigned int j = si;
    for ( unsigned int k=0; k<einx.size(); k++, j++ ) {
      if ( j >= einx.size() )
	j = 0;
      Indices.push_back( einx[ j ] );
    }
  }
    
  }
}


  void RangeLoop::reset( int pos, bool clearskip )
{
  Loop = 0;
  for ( unsigned int k=0; k<Elements.size(); k++ ) {
    Elements[k].Count = 0;
    if ( clearskip )
      Elements[k].Skip = false;
  }
  RepeatCount = 0;
  BlockRepeatCount = 0;
  SingleRepeatCount = 0;
  CurrentIncrement = Increment;
  initSequence( pos );
}


void RangeLoop::purge( void )
{
  vector < ElementType >::iterator ep=Elements.begin();
  while ( ep != Elements.end() ) {
    if ( ep->Skip )
      ep = Elements.erase( ep );
    else
      ++ep;
  }
  initSequence( StartPos );
}


const RangeLoop &RangeLoop::operator=( int pos )
{
  reset( pos );
  return *this;
}


const RangeLoop &RangeLoop::operator++( void )
{
  Loop++;

  // update element:
  if ( pos() >= 0 && pos() < (int)Elements.size() )
    Elements[pos()].Count++;

  // increment singlerepeat:
  SingleRepeatCount++;
  if ( SingleRepeatCount >= SingleRepeat ||
       Index < 0 ||
       Index >= (int)Indices.size() ||
       Elements[Indices[Index]].Skip ||
       Elements[Indices[Index]].Count >= maxBlockCount() ) {

    SingleRepeatCount = 0;

    // increment sequence:
    do {
      // increment:
      Index++;
      
      // next block loop:
      if ( Index == 0 || Index >= (int)Indices.size() ) {
	Index = 0;
	BlockRepeatCount++;

	if ( BlockRepeatCount >= BlockRepeat ) {
	  BlockRepeatCount = 0;

	  do {

	    CurrentIncrement /= 2;

	    if ( CurrentIncrement <= 0 ) {
	      CurrentIncrement = Increment;
	      RepeatCount++;
	      
	      if ( Repeat > 0 && RepeatCount >= Repeat )
		return *this;

	      // skip all?
	      bool skipall = true;
	      for ( unsigned int k=0; k<Elements.size(); k++ ) {
		if ( ! Elements[k].Skip ) {
		  skipall = false;
		  break;
		}
	      }
	      if ( skipall )
		return *this;
	    }
	    
	    initSequence( StartPos );

	  } while ( Indices.size() == 0 );
	}
      }
      
    } while ( Elements[Indices[Index]].Skip ||
	      Elements[Indices[Index]].Count >= maxBlockCount() );
  }

  return *this;
}


bool RangeLoop::operator!( void ) const
{
  return ( Index >= 0 && Index < (int)Indices.size() &&
	   RepeatCount >= 0 && ( Repeat <= 0 || RepeatCount < Repeat ) &&
	   BlockRepeatCount >= 0 && BlockRepeatCount < BlockRepeat &&
	   SingleRepeatCount >= 0 && SingleRepeatCount < SingleRepeat &&
	   CurrentIncrement > 0 );
}


void RangeLoop::update( int pos )
{
  // generate new sequence:
  if ( pos > -2 )
    StartPos = pos;
  initSequence( StartPos );

  if ( Indices.empty() ) {
    Loop--;
    operator++();
  }

  if ( Indices.empty() )
    Index = -1;
}


int RangeLoop::loop( void ) const
{ 
  return Loop;
}


double RangeLoop::value( void ) const
{ 
  return Elements[pos()].Value; 
}

 
double RangeLoop::operator*( void ) const
{ 
  return Elements[pos()].Value; 
}

 
int RangeLoop::pos( void ) const
{ 
  return Indices[Index]; 
}

 
int RangeLoop::index( void ) const
{ 
  return Index; 
}

 
int RangeLoop::count( void ) const
{ 
  return Elements[pos()].Count; 
}

 
void RangeLoop::noCount( void )
{ 
  Elements[pos()].Count--; 
}

 
void RangeLoop::setSkip( bool skip )
{ 
  Elements[pos()].Skip = skip; 
}


double RangeLoop::value( int pos ) const
{ 
  return Elements[pos].Value; 
}

 
double RangeLoop::operator[]( int pos ) const
{ 
  return Elements[pos].Value; 
}

 
double &RangeLoop::operator[]( int pos )
{ 
  return Elements[pos].Value; 
}

 
double RangeLoop::front( void ) const
{ 
  return Elements[0].Value; 
}

 
double &RangeLoop::front( void )
{ 
  return Elements[0].Value; 
}

 
double RangeLoop::back( void ) const
{ 
  return Elements.back().Value; 
}

 
double &RangeLoop::back( void )
{ 
  return Elements.back().Value;
}


double RangeLoop::minValue( void ) const
{
  if ( Elements.empty() )
    return 0.0;

  double min = Elements[0].Value;
  for ( unsigned int k = 1; k < Elements.size(); k++ )
    if ( min > Elements[k].Value )
      min = Elements[k].Value;
  return min;
}


double RangeLoop::maxValue( void ) const
{
  if ( Elements.empty() )
    return 0.0;

  double max = Elements[0].Value;
  for ( unsigned int k = 1; k < Elements.size(); k++ )
    if ( max < Elements[k].Value )
      max = Elements[k].Value;
  return max;
}


double RangeLoop::step( void ) const
{
  if ( size() <= 1 )
    return 0.0;

  return (maxValue()-minValue())/(size()-1);
}


int RangeLoop::count( int pos ) const
{ 
  return Elements[pos].Count; 
}

 
int RangeLoop::skip( int pos ) const
{ 
  return Elements[pos].Skip; 
}

 
void RangeLoop::setSkip( int pos, bool skip )
{ 
  Elements[pos].Skip = skip; 
}


void RangeLoop::setSkipBelow( int pos, bool skip )
{
  for ( int k=0; k<=pos; k++ )
    Elements[k].Skip = skip; 
}


void RangeLoop::setSkipAbove( int pos, bool skip )
{
  for ( unsigned int k=pos; k<Elements.size(); k++ )
    Elements[k].Skip = skip; 
}


void RangeLoop::setSkipBetween( int pos1, int pos2, bool skip )
{
  for ( int k=pos1; k<int(Elements.size()) && k<=pos2; k++ )
    Elements[k].Skip = skip; 
}


void RangeLoop::setSkipNocount( bool skip )
{
  for ( int k=0; k<int(Elements.size()); k++ ) {
    if ( Elements[k].Count <= 0 )
      Elements[k].Skip = skip; 
  }
}


void RangeLoop::setSkipNumber( int num )
{
  // first and last non-skipped data element:
  int first = -1;
  int last = -1;
  for ( int k=0; k<int(Elements.size()); k++ ) {
    if ( ! Elements[k].Skip ) {
      if ( first < 0 )
	first = k;
      last = k+1;
    }
  }
  if ( num > last - first )
    num = last - first;

  // mark used elements as non-skip:
  int n = 0;
  int firstnonskip = -1;
  for ( int i=first; i<last; i++ ) {
    Elements[i].Skip = ( Elements[i].Count == 0 );
    if ( ! Elements[i].Skip ) {
      n++;
      if ( firstnonskip < 0 )
	firstnonskip = i;
    }
  }

  // fill up:
  int incr = CurrentIncrement;
  while ( n < num ) {
    incr /= 2;
    if ( incr == 0 )
      break;
    int f = firstnonskip;
    while ( f - incr > first )
      f -= incr;
    for ( int k=f; k<last; k+=incr ) {
      if ( Elements[k].Skip ) {
	Elements[k].Skip = false;
	n++;
      }
    }
  }
}


int RangeLoop::next( int pos ) const
{
  for ( ; pos<size() && count( pos ) <= 0; pos++ );
  return pos;
}


int RangeLoop::previous( int pos ) const
{
  for ( ; pos >= 0 && count( pos ) <= 0; pos-- );
  return pos;
}


int RangeLoop::pos( int index ) const
{ 
  return Indices[index]; 
}


int RangeLoop::pos( double value ) const
{
  if ( Elements.empty() )
    return 0;

  int p = 0;
  double dist = fabs( Elements[0].Value - value );
  for ( unsigned int k = 1; k < Elements.size(); k++ )
    if ( dist > fabs( Elements[k].Value - value ) ) {
      dist = fabs( Elements[k].Value - value );
      p = k;
    }

  return p;
}


int RangeLoop::index( int pos ) const
{
  for ( unsigned int k=0; k<Indices.size(); k++ )
    if ( Indices[k] == pos )
      return k;

  return -1;
}


bool RangeLoop::active( int p ) const
{ 
  return ( pos() == p );
}


bool RangeLoop::near( int pos, double value ) const
{
  double d = fabs( value - Elements[pos].Value );

  for ( int k = 0; k < (int)Elements.size(); k++ )
    if ( k != pos &&
	 fabs( value - Elements[k].Value ) < d )
      return false;

  return true;
}


ostream &operator<< ( ostream &str, const RangeLoop &rl )
{
  str << "RangeLoop Elements: " << '\n';
  for ( int k=0; k<rl.size(); k++ ) {
    str << k << ": C=";
    str << rl.Elements[k].Count << " S=";
    str << rl.Elements[k].Skip << " V=";
    str << rl.Elements[k].Value;
    str << '\n';
  }

  str << "RangeLoop Indices: " << '\n';
  for ( unsigned int k=0; k<rl.Indices.size(); k++ ) {
    str << k << " -> " 
	<< rl.Indices[k] << ": " << rl.Elements[rl.Indices[k]].Value << '\n';
  }

  str << "RangeLoop Variables: " << '\n';
  str << "  Index=" << rl.Index << '\n';
  str << "  StartPos=" << rl.StartPos << '\n';
  str << "  Loop=" << rl.Loop << '\n';
  str << "  Repeat=" << rl.Repeat << '\n';
  str << "  RepeatCount=" << rl.RepeatCount << '\n';
  str << "  BlockRepeat=" << rl.BlockRepeat << '\n';
  str << "  BlockRepeatCount=" << rl.BlockRepeatCount << '\n';
  str << "  SingleRepeat=" << rl.SingleRepeat << '\n';
  str << "  SingleRepeatCount=" << rl.SingleRepeatCount << '\n';
  str << "  Increment=" << rl.Increment << '\n';
  str << "  CurrentIncrement=" << rl.CurrentIncrement << '\n';
  str << "  Seq=" << rl.Seq << '\n';
  str << "  AddMethod=" << rl.AddMethod << '\n';

  return str;
}


}; /* namespace relacs */

