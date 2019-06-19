/*
  inlist.cc
  A container for InData

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

#include <sstream>
#include <relacs/inlist.h>
using namespace std;

namespace relacs {


InList::InList( void )
  : IL()
{
}


InList::InList( InData &data )
  : IL()
{
  push( data );
}


InList::InList( InData *data, bool own )
  : IL()
{
  add( data, own );
}


InList::InList( const InList *il )
  : IL()
{
  IL.resize( il->IL.size() );
  for ( unsigned int k=0; k<IL.size(); k++ ) {
    IL[k].ID->assign( &(*il)[k] );
  }
}


InList::InList( const InList &il )
  : IL()
{
  IL.resize( il.IL.size() );
  for ( unsigned int k=0; k<IL.size(); k++ ) {
    IL[k].Own = il.IL[k].Own;
    if ( IL[k].Own )
      IL[k].ID = new InData( *(il.IL[k].ID) );
    else
      IL[k].ID = il.IL[k].ID;
  }
}


InList::~InList( void )
{
  clear();
}


void InList::resize( int n, int m, int w, double step )
{
  if ( n <= 0 ) {
    clear();
    return;
  }

  int os = IL.size();
  IL.resize( n, ILE() );
  if ( n > os ) {
    for ( int k=os; k<n; k++ ) {
      IL[k].ID = new InData( m, w, step );
      IL[k].Own = true;
    }
  }
}


void InList::clear( void )
{
  for ( unsigned int k=0; k<IL.size(); k++ ) {
    if ( IL[k].Own )
      delete IL[k].ID;
  }
  IL.clear();
}


InList &InList::operator=( const InList &il )
{
  if ( &il == this )
    return *this;

  IL.resize( il.IL.size() );
  for ( unsigned int k=0; k<IL.size(); k++ ) {
    IL[k].Own = il.IL[k].Own;
    if ( IL[k].Own )
      IL[k].ID = new InData( *(il.IL[k].ID) );
    else
      IL[k].ID = il.IL[k].ID;
  }

  return *this;
}


InList &InList::assign( const InList *il )
{
  if ( il == this )
    return *this;

  IL.resize( il->IL.size() );
  for ( unsigned int k=0; k<IL.size(); k++ ) {
    assign( k, &(*il)[k] );
  }

  return *this;
}


InList &InList::assign( void )
{
  for ( unsigned int k=0; k<IL.size(); k++ )
    IL[k].ID->assign();
  return *this;
}


const InData &InList::front( void ) const
{
  return *IL.front().ID;
}


InData &InList::front( void )
{
  return *IL.front().ID;
}


const InData &InList::back( void ) const
{
  return *IL.back().ID;
}


InData &InList::back( void )
{
  return *IL.back().ID;
}


const InData &InList::operator[]( const string &ident ) const
{
  for ( unsigned int k=0; k<IL.size(); k++ ) {
    if ( IL[k].ID->ident() == ident ) 
      return *IL[k].ID;
  }
  return front();
}


InData &InList::operator[]( const string &ident )
{
  for ( unsigned int k=0; k<IL.size(); k++ ) {
    if ( IL[k].ID->ident() == ident ) 
      return *IL[k].ID;
  }
  return front();
}


int InList::index( const string &ident ) const
{
  for ( unsigned int k=0; k<IL.size(); k++ ) {
    if ( IL[k].ID->ident() == ident ) 
      return k;
  }
  return -1;
}


void InList::push( InData &data )
{
  IL.push_back( ILE( new InData( data ), true ) );
}


void InList::push( const InList &traces )
{
  for ( int k=0; k<traces.size(); k++ )
    IL.push_back( ILE( new InData( traces[k] ), true ) );
}


void InList::add( InData *data, bool own )
{
  IL.push_back( ILE( data, own ) );
}


void InList::add( const InData *data, bool own )
{
  IL.push_back( ILE( const_cast<InData*>(data), own ) );
}


void InList::add( const InList &traces, bool own )
{
  for ( int k=0; k<traces.size(); k++ )
    IL.push_back( ILE( const_cast<InData*>(&traces[k]), own ) );
}


void InList::set( int index, const InData *data, bool own )
{
  if ( index >= 0 && index < size() ) {
    if ( IL[index].Own )
      delete IL[index].ID;
    IL[index].Own = own;
    IL[index].ID = const_cast<InData*>(data);
  }
}


void InList::assign( int index, const InData *data )
{
  if ( index >= 0 && index < size() ) {
    if ( IL[index].Own )
      delete IL[index].ID;
    IL[index].Own = true;
    IL[index].ID = new InData( data );
  }
}


void InList::erase( int index )
{
  if ( index >= 0 && index < size() ) {
    if ( IL[index].Own )
      delete IL[index].ID;
    IL.erase( IL.begin() + index );
  }
}


bool lessChannelILE( const InList::ILE &a, const InList::ILE &b )
{
  return ( a.ID->channel() < b.ID->channel() );
}


void InList::sortByChannel( void )
{
  sort( IL.begin(), IL.end(), lessChannelILE );
}


bool lessDeviceChannelILE( const InList::ILE &a, const InList::ILE &b )
{
  if ( a.ID->device() == b.ID->device() )
    return ( a.ID->channel() < b.ID->channel() );
  else
    return ( a.ID->device() < b.ID->device() );
}


void InList::sortByDeviceChannel( void )
{
  sort( IL.begin(), IL.end(), lessDeviceChannelILE );
}


void InList::update( void )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).update();
}


void InList::updateRaw( void )
{
  for ( int k=0; k<size(); k++ ) {
    if ( operator[]( k ).source() == 0 )
      operator[]( k ).update();
  }
}


void InList::updateDerived( void )
{
  for ( int k=0; k<size(); k++ ) {
    if ( operator[]( k ).source() != 0 )
      operator[]( k ).update();
  }
}


void InList::clearBuffer( void )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).clear();
}


double InList::currentTime( void ) const
{
  double t = -1.0;
  for ( int k=0; k<size(); k++ ) {
    // skip empty traces in case a model does not simulate some traces:
    if ( ! operator[]( k ).empty() ) {
      double ct = operator[]( k ).currentTime();
      if ( t < 0.0 || t > ct )  
	t = ct;
    }
  }
  return t < 0.0 ? 0.0 : t;
}


double InList::currentTimeRaw( void ) const
{
  double t = -1.0;
  for ( int k=0; k<size(); k++ ) {
    if ( operator[]( k ).source() == 0 ) {
      // skip empty traces in case a model does not simulate some traces:
      if ( ! operator[]( k ).empty() ) {
	double ct = operator[]( k ).currentTime();
	if ( t < 0.0 || t > ct )  
	  t = ct;
      }
    }
  }
  return t < 0.0 ? 0.0 : t;
}


void InList::setDevice( int device )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setDevice( device );
}


void InList::setReference( InData::RefType ref )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setReference( ref );
}


void InList::setUnipolar( bool unipolar )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setUnipolar( unipolar );
}


void InList::setStartSource( int startsource )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setStartSource( startsource );
}


void InList::setDelay( double delay )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setDelay( delay );
}


void InList::setPriority( bool priority )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setPriority( priority );
}


void InList::setSampleRate( double rate )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setSampleRate( rate );
}


void InList::setSampleInterval( double step )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setSampleInterval( step );
}


void InList::setContinuous( bool continuous )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setContinuous( continuous );
}


void InList::setScale( double scale )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setScale( scale );
}


void InList::setUnit( const string &unit )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setUnit( unit );
}


void InList::setUnit( double scale, const string &unit )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setUnit( scale, unit );
}


void InList::clearMode( void )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).clearMode();
}


void InList::setMode( int flags )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setMode( flags );
}


void InList::addMode( int flags )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).addMode( flags );
}


void InList::delMode( int flags )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).delMode( flags );
}


void InList::setSignalIndex( int index )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setSignalIndex( index );
}


void InList::setSignalTime( double time )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setSignalTime( time );
}


void InList::setRestart( void )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setRestart();
}


void InList::setRestartTime( double restarttime )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setRestartTime( restarttime );
}


string InList::errorText( void ) const
{
  ostringstream ss;

  // common errors:
  bool common = false;
  long long flags = 0xffffffffffffffffLL;
  for ( int k=0; k<size(); k++ )
    flags &= operator[]( k ).error();
  if ( flags > 0 ) {
    ss << DaqError::errorText( flags );
    common = true;
  }

  // common error strings:
  bool commonstr = true;
  for ( int k=1; k<size(); k++ ) {
    if ( operator[]( k ).errorStr() != operator[]( 0 ).errorStr() )
      commonstr = false;
  }
  if ( commonstr && ! operator[]( 0 ).errorStr().empty() ) {
    if ( common )
      ss << ", ";
    ss << operator[]( 0 ).errorStr();
    common = true;
  }

  if ( common )
    ss << " (all channels). ";

  // individual errors:
  for ( int k=0; k<size(); k++ ) {
    long long f = operator[]( k ).error() & (~flags);
    string es = operator[]( k ).errorStr();
    if ( commonstr )
      es = "";
    if ( f > 0 || !es.empty() ) {
      ss << "Channel " << operator[]( k ).channel()
	 << " on device " << operator[]( k ).device() << ": ";
      string ef = operator[]( k ).errorText( f );
      if ( !ef.empty() ) {
	ss << ef;
	if ( !es.empty() )
	  ss << ", ";
      }
      if ( !es.empty() )
	ss << es;
      ss << ". ";
    }
  }
  return ss.str();
}


void InList::clearError( void )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).clearError();
}


void InList::setError( long long flags )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setError( flags );
}


void InList::addError( long long flags )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).addError( flags );
}


void InList::delError( long long flags )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).delError( flags );
}


void InList::addDaqError( int de )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).addDaqError( de );
}


void InList::setErrorStr( const string &strg )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setErrorStr( strg );
}


void InList::addErrorStr( const string &strg )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).addErrorStr( strg );
}


void InList::setErrorStr( int errnum )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setErrorStr( errnum );
}


void InList::addErrorStr( int errnum )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).addErrorStr( errnum );
}


bool InList::success( void ) const
{
  for ( int k=0; k<size(); k++ ) {
    if ( operator[]( k ).failed() )
      return false;
  }
  return true;
}


bool InList::failed( void ) const
{
  for ( int k=0; k<size(); k++ ) {
    if ( operator[]( k ).failed() )
      return true;
  }
  return false;
}


ostream &operator<< ( ostream &str, const InList &data )
{
  for ( int k=0; k<data.size(); k++ ) {
    str << "InData " << k << ":" << '\n';
    str << data[k] << '\n';
  }
  return str;
}


}; /* namespace relacs */

