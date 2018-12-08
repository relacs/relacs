/*
  daqerror.cc
  Error codes for analog input and output

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

#include <cstring>
#include <relacs/attenuate.h>
#include <relacs/daqerror.h>

namespace relacs {


string DaqError::ErrorText[LastState] =
  { "no device", "device not open", "multiple devices",
    "invalid start source", "multiple start sources",
    "invalid delay", "multiple delays", "multiple priorities",
    "invalid sampling rate", "multiple sampling rates",
    "continuous mode not supported", "multiple continuous modes",
    "multiple restart requests", "no data", "multiple buffer sizes",
    "invalid size for the driver's buffer", "multiple sizes for the driver's buffer",
    "invalid size for the update buffer", "multiple sizes for the update buffer",
    "invalid trace specification", "invalid channel", "multiple channels",
    "invalid reference", "multiple references",
    "dither not supported", "multiple dither settings",
    "reglitch not supported", "multiple reglitch settings",
    "invalid gain", "multiple gains", "signal underflow", "signal overflow",
    "calibration failed", "invalid channel type", "invalid channel sequence",
    "busy", "buffer overflow", "overflow/underrun", "unknown",
    "Intensity not set",
    "attenuator not open", "invalid attenuator device",
    "attenuator failed", "attenuator underflow", "attenuator overflow",
    "attenuator intensity overflow", "attenuator intensity underflow",
    "attenuator intensity failed" };


DaqError::DaqError( void )
  : State( 0 ),
    ErrorString( "" )
{
}


void DaqError::clearError( void )
{
  State = 0;
  ErrorString = "";
}


long long DaqError::error( void ) const
{
  return State;
}


void DaqError::setError( long long flags )
{
  State = flags;
}


void DaqError::addError( long long flags )
{
  State |= flags;
}


void DaqError::delError( long long flags )
{
  State &= ~flags;
}


void DaqError::addDaqError( int de )
{
  // no error:
  if ( de == 0 )
    return;

  // map error code:
  if ( de == 1 )
    addError( OverflowUnderrun );
  else
    addError( Unknown );
}


void DaqError::addAttError( int ae )
{
  // no error:
  if ( ae == 0 )
    return;

  // map error code:
  if ( ae == Attenuate::NotOpen )
    addError( AttNotOpen );
  else if ( ae == Attenuate::InvalidDevice )
    addError( AttInvalidDevice );
  else if ( ae == Attenuate::ReadError || ae == Attenuate::WriteError )
    addError( AttFailed );
  else if ( ae >= Attenuate::IntensityError )
    addError( 1LL<<(36 - ae) );
  else
    addError( AttIntensityFailed );
}


string DaqError::errorStr( void ) const
{
  return ErrorString;
}


void DaqError::setErrorStr( const string &strg )
{
  ErrorString = strg;
}


void DaqError::addErrorStr( const string &strg )
{
  if ( strg == ErrorString )
    return;
  if ( !ErrorString.empty() || strg.empty() )
    ErrorString += ", ";
  ErrorString += strg;
}


void DaqError::setErrorStr( int errnum )
{
  char buf[1000];
  strerror_r( errnum, buf, 1000 );
  if ( strlen( buf ) > 0 )
    ErrorString = buf;
  else
    ErrorString = "unknown error";
}


void DaqError::addErrorStr( int errnum )
{
  if ( !ErrorString.empty() )
    ErrorString += ", ";

  char buf[1000];
  char *ep = strerror_r( errnum, buf, 1000 );
  if ( strlen( ep ) > 0 )
    ErrorString += ep;
  else
    ErrorString += "unknown error";
}


string DaqError::errorText( long long flags )
{
  string errorstr = "";
  for( int i=0; i<LastState; i++ ) {
    if( flags & (1LL<<i) ) {
      if ( !errorstr.empty() )
	errorstr += ", ";
      errorstr += ErrorText[i];
    }
  }
  return errorstr;
}


string DaqError::errorText( void ) const
{
  string errorstr = "";
  for( int i=0; i<LastState; i++ ) {
    if( State & (1LL<<i) ) {
      if ( !errorstr.empty() )
	errorstr += ", ";
      errorstr += ErrorText[i];
    }
  }
  if ( ErrorString.size() > 0 ) {
    if ( !errorstr.empty() )
      errorstr += ", ";
    errorstr += ErrorString;
  }
  return errorstr;
}


bool DaqError::success( void ) const
{
  return ( State == 0 && ErrorString.empty() );
}


bool DaqError::failed( void ) const
{
  return ( State != 0 || ErrorString.size() > 0 );
}


bool DaqError::busy( void ) const
{
  return ( State & Busy );
}


bool DaqError::overflow( void ) const
{
  return ( ( State & Overflow ) > 0 ||
	   ( State & AttOverflow ) > 0 ||
	   ( State & AttIntensityOverflow ) > 0 );
}


bool DaqError::underflow( void ) const
{
  return ( ( State & Underflow ) > 0 ||
	   ( State & AttUnderflow ) > 0 ||
	   ( State & AttIntensityUnderflow ) > 0 );
}


bool DaqError::attenuatorFailed( void ) const
{
  return ( ( State & AttFailed ) > 0 || ( State & AttIntensityFailed ) > 0 );
}


void DaqError::update( const DaqError *de )
{
  State = de->State;
  ErrorString = de->ErrorString;
}


ostream &operator<<( ostream &str, const DaqError &de )
{
  str << "Error: " << de.State << '\n';
  str << "Error string: " << de.ErrorString << '\n';
  str << "Error text: " << de.errorText() << '\n';
  return str;
}


}; /* namespace relacs */

