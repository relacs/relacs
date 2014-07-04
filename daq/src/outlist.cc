/*
  outlist.cc
  A container for OutData

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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
{
  Description.setType( "stimulus" );
}


OutList::OutList( OutData &signal )
{
  Description.setType( "stimulus" );
  push( signal );
}


OutList::OutList( OutData *signal, bool own )
{
  Description.setType( "stimulus" );
  add( signal, own );
}


OutList::OutList( const OutList &ol )
{
  Description.setType( "stimulus" );
  OL.clear();
  for ( unsigned int k=0; k<OL.size(); k++ ) {
    OutData *od = ol.OL[k].OD;
    if ( ol.OL[k].Own )
      od = new OutData( *(ol.OL[k].OD) );
    add( od, ol.OL[k].Own );
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
  if ( n <= os ) {
    for ( unsigned int k=n; n<os; k++ ) {
      if ( OL[k].Own )
	delete OL[k].OD;
      Description.erase( Description.sectionsBegin() + n );
    }
    OL.resize( n, OLE() );
  }
  else {
    for ( int k=os; k<n; k++ )
      push( new OutData( m, step ) );
  }
}


void OutList::clear( void )
{
  Description.clear( false );
  Description.setType( "stimulus" );
  for ( unsigned int k=0; k<OL.size(); k++ ) {
    if ( OL[k].Own )
      delete OL[k].OD;
  }
  OL.clear();
}


OutList &OutList::operator=( const OutList &ol )
{
  if ( &ol == this )
    return *this;

  OL.clear();
  Description.clear();
  Description.setName( ol.Description.name() );
  Description.setType( ol.Description.type() );
  Description.setInclude( ol.Description.include() );
  for ( unsigned int k=0; k<ol.OL.size(); k++ ) {
    OutData *od = ol.OL[k].OD;
    if ( ol.OL[k].Own )
      od = new OutData( *(ol.OL[k].OD) );
    add( od, ol.OL[k].Own );
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
  Description.newSection( &OL.back().OD->description() );
}


void OutList::push( const OutList &sigs )
{
  for ( int k=0; k<sigs.size(); k++ ) {
    OL.push_back( OLE( new OutData( sigs[k] ), true ) );
    Description.newSection( &OL.back().OD->description() );
  }
}


void OutList::add( OutData *signal, bool own )
{
  OL.push_back( OLE( signal, own ) );
  Description.newSection( &OL.back().OD->description() );
}


void OutList::add( const OutData *signal, bool own )
{
  OL.push_back( OLE( const_cast<OutData*>(signal), own ) );
  Description.newSection( &OL.back().OD->description() );
}


void OutList::add( const OutList &sigs, bool own )
{
  for ( int k=0; k<sigs.size(); k++ ) {
    OL.push_back( OLE( const_cast<OutData*>(&sigs[k]), own ) );
    Description.newSection( &OL.back().OD->description() );
  }
}


void OutList::erase( int index )
{
  if ( index >= 0 && index < size() ) {
    if ( OL[index].Own )
      delete OL[index].OD;
    OL.erase( OL.begin() + index );
    Description.erase( Description.sectionsBegin() + index );
  }
}


bool lessChannelOLE( const OutList::OLE &a, const OutList::OLE &b )
{
  return ( a.OD->channel() < b.OD->channel() );
}


void OutList::sortByChannel( void )
{
  sort( OL.begin(), OL.end(), lessChannelOLE );
  Description.clear();
  Description.setType( "stimulus" );
  for ( unsigned int k=0; k<OL.size(); k++ )
    Description.newSection( &OL[k].OD->description() );
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
  Description.clear();
  Description.setType( "stimulus" );
  for ( unsigned int k=0; k<OL.size(); k++ )
    Description.newSection( &OL[k].OD->description() );
}


const Options &OutList::description( void ) const
{
  return Description;
}


Options &OutList::description( void )
{
  return Description;
}


void OutList::setDevice( int device )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).setDevice( device );
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


double OutList::maxLength( void ) const
{
  double ml = 0.0;
  for ( int k=0; k<size(); k++ ) {
    if ( ml < operator[]( k ).length() )
      ml = operator[]( k ).length();
  }
  return ml;
}


void OutList::deviceReset( void )
{
  for ( int k=0; k<size(); k++ )
    operator[]( k ).deviceReset();
}


int OutList::deviceBufferSize( void )
{
  int n = 0;
  for ( int k=0; k<size(); k++ )
    n += operator[]( k ).deviceDelay() + operator[]( k ).size();
  return n;
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

  // common error strings:
  bool commonstr = true;
  for ( int k=1; k<size(); k++ ) {
    if ( operator[]( k ).errorStr() != operator[]( 0 ).errorStr() )
      commonstr = false;
  }
  if ( size() > 0 && commonstr )
    ss << operator[]( 0 ).errorStr() << ". ";

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

