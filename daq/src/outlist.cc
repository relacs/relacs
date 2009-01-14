/*
  outlist.cc
  A container for OutData

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

#include <sstream>
#include <algorithm>
#include <relacs/outlist.h>
using namespace std;

namespace relacs {


OutList::OutList( void )
  : OL()
{
}


OutList::OutList( OutData &signal )
  : OL()
{
  push( signal );
}


OutList::OutList( OutData *signal, bool own )
  : OL()
{
  add( signal, own );
}


OutList::OutList( const OutList &ol )
  : OL()
{
  OL.resize( ol.OL.size() );
  for ( unsigned int k=0; k<OL.size(); k++ ) {
    OL[k].Own = ol.OL[k].Own;
    if ( OL[k].Own )
      OL[k].OD = new OutData( *(ol.OL[k].OD) );
    else
      OL[k].OD = ol.OL[k].OD;
  }
}


OutList::~OutList( void )
{
  clear();
}


void OutList::resize( int n, int m, double step )
{
  if ( n <= 0 ) {
    clear();
    return;
  }

  int os = OL.size();
  OL.resize( n, OLE() );
  if ( n > os ) {
    for ( int k=os; k<n; k++ ) {
      OL[k].OD = new OutData( m, step );
      OL[k].Own = true;
    }
  }
}


void OutList::clear( void )
{
  for ( unsigned int k=0; k<OL.size(); k++ ) {
    if ( OL[k].Own )
      delete OL[k].OD;
  }
  OL.clear();
}


void OutList::reserve( int n )
{
  OL.reserve( n );
}


OutList &OutList::operator=( const OutList &ol )
{
  if ( &ol == this )
    return *this;

  OL.resize( ol.OL.size(), OLE() );
  for ( unsigned int k=0; k<OL.size(); k++ ) {
    OL[k].Own = ol.OL[k].Own;
    if ( OL[k].Own )
      OL[k].OD = new OutData( *(ol.OL[k].OD) );
    else
      OL[k].OD = ol.OL[k].OD;
  }

  return *this;
}


const OutData &OutList::front( void ) const
{
  return *OL.front().OD;
}


OutData &OutList::front( void )
{
  return *OL.front().OD;
}


const OutData &OutList::back( void ) const
{
  return *OL.back().OD;
}


OutData &OutList::back( void )
{
  return *OL.back().OD;
}


const OutData &OutList::operator[]( const string &ident ) const
{
  for ( unsigned int k=0; k<OL.size(); k++ ) {
    if ( OL[k].OD->ident() == ident ) 
      return *OL[k].OD;
  }
  return front();
}


OutData &OutList::operator[]( const string &ident )
{
  for ( unsigned int k=0; k<OL.size(); k++ ) {
    if ( OL[k].OD->ident() == ident ) 
      return *OL[k].OD;
  }
  return front();
}


int OutList::index( const string &ident ) const
{
  for ( unsigned int k=0; k<OL.size(); k++ ) {
    if ( OL[k].OD->ident() == ident ) 
      return k;
  }
  return -1;
}


void OutList::push( OutData &signal )
{
  OL.push_back( OLE( new OutData( signal ), true ) );
}


void OutList::push( const OutList &sigs )
{
  OL.reserve( OL.size() + sigs.size() );
  for ( int k=0; k<sigs.size(); k++ )
    OL.push_back( OLE( new OutData( sigs[k] ), true ) );
}


void OutList::add( OutData *signal, bool own )
{
  OL.push_back( OLE( signal, own ) );
}


void OutList::add( const OutData *signal, bool own )
{
  OL.push_back( OLE( const_cast<OutData*>(signal), own ) );
}


void OutList::add( const OutList &sigs, bool own )
{
  OL.reserve( OL.size() + sigs.size() );
  for ( int k=0; k<sigs.size(); k++ )
    OL.push_back( OLE( const_cast<OutData*>(&sigs[k]), own ) );
}


void OutList::erase( int index )
{
  if ( index >= 0 && index < size() ) {
    if ( OL[index].Own )
      delete OL[index].OD;
    OL.erase( OL.begin() + index );
  }
}


bool lessChannelOLE( const OutList::OLE &a, const OutList::OLE &b )
{
  return ( a.OD->channel() < b.OD->channel() );
}


void OutList::sortByChannel( void )
{
  sort( OL.begin(), OL.end(), lessChannelOLE );
}


bool lessDeviceChannelOLE( const OutList::OLE &a, const OutList::OLE &b )
{
  if ( a.OD->device() == b.OD->device() )
    return ( a.OD->channel() < b.OD->channel() );
  else
    return ( a.OD->device() < b.OD->device() );
}


void OutList::sortByDeviceChannel( void )
{
  sort( OL.begin(), OL.end(), lessDeviceChannelOLE );
}


void OutList::setDevice( int device )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setDevice( device );
}


void OutList::setReglitch( bool reglitch )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setReglitch( reglitch );
}


void OutList::setStartSource( int startsource )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setStartSource( startsource );
}


void OutList::setDelay( double delay )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setDelay( delay );
}


void OutList::setPriority( bool priority )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setPriority( priority );
}


void OutList::setSampleRate( double rate )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setSampleRate( rate );
}


void OutList::setSampleInterval( double step )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setSampleInterval( step );
}


void OutList::setContinuous( bool continuous )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setContinuous( continuous );
}


void OutList::setRestart( bool restart )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setRestart( restart );
}


void OutList::setWriteTime( double time )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setWriteTime( time );
}


void OutList::freeDeviceBuffer( void )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).freeDeviceBuffer();
}


void OutList::setAutoConvert( void )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setAutoConvert();
}


void OutList::setManualConvert( void )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setManualConvert();
}


string OutList::errorText( void ) const
{
  ostringstream ss;

  // common errors:
  long long flags = 0xffffffffffffffffLL;
  for ( int k=0; k<size(); k++ )
    flags &= operator[]( k ).error();
  if ( flags > 0 )
    ss << DaqError::errorText( flags ) << ". ";

  // individual errors:
  for ( int k=0; k<size(); k++ ) {
    long long f = operator[]( k ).error() & (~flags);
    if ( f > 0 || !operator[]( k ).errorStr().empty() ) {
      ss << "Signal " << operator[]( k ).ident()
	 << " on channel " << operator[]( k ).channel()
	 << " on device " << operator[]( k ).device() << ": ";
      if ( !operator[]( k ).errorText( f ).empty() )
	ss << operator[]( k ).errorText( f );
      if ( !operator[]( k ).errorText( f ).empty() &&
	   !operator[]( k ).errorStr().empty() )
	ss << ", ";
      if ( !operator[]( k ).errorStr().empty() )
	ss << operator[]( k ).errorStr();
      ss << ". ";
    }
  }
  return ss.str();
}


void OutList::clearError( void )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).clearError();
}


void OutList::setError( long long flags )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setError( flags );
}


void OutList::addError( long long flags )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).addError( flags );
}


void OutList::delError( long long flags )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).delError( flags );
}


void OutList::addDaqError( int de )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).addDaqError( de );
}


void OutList::setErrorStr( const string &strg )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setErrorStr( strg );
}


void OutList::addErrorStr( const string &strg )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).addErrorStr( strg );
}


void OutList::setErrorStr( int errnum )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setErrorStr( errnum );
}


void OutList::addErrorStr( int errnum )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).addErrorStr( errnum );
}


bool OutList::success( void ) const
{
  for ( int k=0; k<size(); k++ ) {
    if ( operator[]( k ).failed() )
      return false;
  }
  return true;
}


bool OutList::failed( void ) const
{
  for ( int k=0; k<size(); k++ ) {
    if ( operator[]( k ).failed() )
      return true;
  }
  return false;
}


ostream &operator<< ( ostream &str, const OutList &signal )
{
  for ( int k=0; k<signal.size(); k++ ) {
    str << "OutData " << k << ":" << '\n';
    str << signal[k] << '\n';
  }
  return str;
}


}; /* namespace relacs */

