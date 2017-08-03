/*
  acoustic/loudspeaker.cc
  Direct conversion to attenuation level 

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

#include <cmath>
#include <ctime>
#include <iostream>
#include <fstream>
#include <relacs/str.h>
#include <relacs/strqueue.h>
#include <relacs/options.h>
#include <relacs/tablekey.h>
#include <relacs/relacsplugin.h>
#include <relacs/acoustic/loudspeaker.h>
using namespace std;
using namespace relacs;

namespace acoustic {


LoudSpeaker::LoudSpeaker( void )
  : Attenuate( "LoudSpeaker", "sound intensity", "dB SPL", "%5.1f",
	       "carrier frequency", "Hz", "%7.0f" ),
    ConfigClass( "LoudSpeaker", RELACSPlugin::Plugins, ConfigClass::Save )
{
  // parameter:
  DefaultGain = -1.0;
  DefaultOffset = 100.0;
  MaxVoltage = 10.0;
  SamplingRate = -1.0;
  CalibDate = "";
  CalibFile = "calib.dat";

  initOptions();
  // add some parameter as options:
  addNumber( "defaultgain", "Default gain", DefaultGain, -10000.0, 10000.0, 0.5 );
  addNumber( "defaultoffset", "Default offset", DefaultOffset, -10000.0, 10000.0, 5.0, "dB SPL" );
  addNumber( "maxvoltage", "Maximum allowed p-p voltage", MaxVoltage, 0.0, 10.0, 0.05, "V" );
}


LoudSpeaker::~LoudSpeaker( void )
{
}


int LoudSpeaker::decibel( double intensity, double frequency, double &db ) const
{
  if ( intensity == MuteIntensity )
    db = MuteAttenuationLevel;
  else {
    double g, o;
    gain( g, o, frequency );
    db = intensity*g + o;
    double ppvolt = 1.0 * ::pow( 10.0, -db/20.0 );
    if ( ppvolt > MaxVoltage ) {
      db = -20.0*log10( MaxVoltage/1.0 );
      return IntensityOverflow;
    }
  }
  return 0;
}


void LoudSpeaker::intensity( double &intens, double frequency,
			     double decibel ) const
{
  if ( decibel != MuteAttenuationLevel ) {
    double g, o;
    gain( g, o, frequency );
    intens = ( decibel - o )/g;
    // error handling! g == 0!
  }
  else
    intens = MuteIntensity;
}


double LoudSpeaker::gain( double frequency ) const
{
  double g, o;
  gain( g, o, frequency );
  return g;
}


double LoudSpeaker::offset( double frequency ) const
{
  double g, o;
  gain( g, o, frequency );
  return o;
}


void LoudSpeaker::gain( double &gain, double &offset, double &frequency ) const
{
  frequency = ::fabs( frequency );

  if ( Frequency.empty() ) {
    // no calibration table:
    gain = DefaultGain;
    offset = DefaultOffset;
    frequency = 0.0;
  }
  else if ( Frequency.size() == 1 ) {
    // return the only available calibration:
    gain = Gain[0];
    offset = Offset[0];
    frequency = Frequency[0];
  }
  else if ( frequency <= Frequency[0] ) {
    gain = Gain[0];
    offset = Offset[0];
    frequency = Frequency[0];
  }
  else if ( frequency >= Frequency.back() ) {
    gain = Gain.back();
    offset = Offset.back();
    frequency = Frequency.back();
  }
  else {
    // interpolate:
    int k=0;
    for ( k=0; k<Frequency.size() && Frequency[k] < frequency; k++ );
    gain = (frequency-Frequency[k])*(Gain[k]-Gain[k-1])/(Frequency[k]-Frequency[k-1]) + Gain[k];
    offset = (frequency-Frequency[k])*(Offset[k]-Offset[k-1])/(Frequency[k]-Frequency[k-1]) + Offset[k];
  }
}


void LoudSpeaker::setGain( double gain, double offset, double frequency )
{
  if ( frequency <= 0.0 )
    return;

  ArrayD::iterator fp = Frequency.begin();
  ArrayD::iterator gp = Gain.begin();
  ArrayD::iterator op = Offset.begin();

  // search for frequency:
  while ( fp != Frequency.end() && *fp < frequency ) {
    ++fp;
    ++gp;
    ++op;
  }

  if ( fp != Frequency.end() && fabs( *fp - frequency ) < 1.0e-8 ) {
    // frequency already exist:
    *gp = gain;
    *op = offset;
  }
  else {
    // new frequency:
    if ( fp != Frequency.end() ) {
      Frequency.insert( fp, frequency );
      Gain.insert( gp, gain );
      Offset.insert( op, offset );
    }
    else {
      Frequency.push( frequency );
      Gain.push( gain );
      Offset.push( offset );
    }
  }
}



void LoudSpeaker::reset( double frequency )
{
  setGain( DefaultGain, DefaultOffset, frequency );
}


void LoudSpeaker::clear( void )
{
  Frequency.clear();
  Gain.clear();
  Offset.clear();
}


void LoudSpeaker::calibrationTable( ArrayD &freq, ArrayD &offs, ArrayD &gain ) const
{
  freq = Frequency;
  offs = Offset;
  gain = Gain;
}


void LoudSpeaker::setSamplingRate( double rate )
{
  SamplingRate = rate;
}


void LoudSpeaker::load( void )
{
  Frequency.clear();
  Gain.clear();
  Offset.clear();

  ifstream cf( CalibFile.c_str() );
  Str line;
  while ( getline( cf, line ).good() ) {
    if ( ! line.empty( Str::WhiteSpace, "#" ) ) {
      int inx=0;
      double f, g, o;
      const double nonum = -1.13e-123;
      // frequency:
      f = line.number( nonum, inx, &inx );
      // gain and offset:
      o = line.number( nonum, inx, &inx );
      g = line.number( nonum, inx, &inx );
      if ( f > 0.0 && f != nonum && g != nonum && o != nonum ) {
	Frequency.push( f );
	Gain.push( g );
	Offset.push( o );
      }
    }
    else {
      if ( line.find( "sampling rate" ) > 0 )
	SamplingRate = 1000.0 * line.value().number();
      else if ( line.find( "date" ) > 0 )
	CalibDate = line.value();
    }
  }
}


void LoudSpeaker::saveCalibration( const string &file,
				   const string &date ) const
{
  // create file:
  ofstream df( file.c_str() );
  if ( ! df.good() )
    return;

  // write header and key:
  Options header;
  header.addNumber( "device", aoDevice() );
  header.addNumber( "channel", aoChannel() );
  if ( SamplingRate > 0.0 )
    header.addNumber( "sampling rate", 0.001*SamplingRate, "kHz" );
  header.addText( "date", date );
  header.newSection( *this, "Settings" );
  header.save( df, "# " );
  df << '\n';
  TableKey key;
  key.addNumber( frequencyName(), frequencyUnit(), frequencyFormat() );
  key.addNumber( "offset", "dB SPL", "%6.2f" );
  key.addNumber( "gain", "1", "%6.3f" );
  key.saveKey( df, true, false );

  // write data:
  for ( int k=0; k<Frequency.size(); k++ ) {
    key.save( df, Frequency[k], 0 );
    key.save( df, Offset[k], 1 );
    key.save( df, Gain[k], 2 );
    df << '\n';
  }
}


void LoudSpeaker::save( void ) const
{
  time_t dt = ::time( 0 );
  Str date = "%d.%m.%02Y";
  date.format( ::localtime( &dt ) );
  CalibDate = date;

  saveCalibration( CalibFile, date );
}


void LoudSpeaker::save( const string &path ) const
{
  saveCalibration( path+CalibFile, CalibDate );
}


void LoudSpeaker::config( void )
{
  CalibFile = "calib" + Str( aoDevice() ) + "-" + Str( aoChannel() ) + ".dat";
  DefaultGain = number( "defaultgain" );
  DefaultOffset = number( "defaultoffset" );
  MaxVoltage = number( "maxvoltage" );

  load();

  Attenuate::init();
  Info.addNumber( "maximum allowed voltage", MaxVoltage, "V" );
}


void LoudSpeaker::setDeviceIdent( const string &ident )
{
  Attenuate::setDeviceIdent( ident );
  setConfigIdent( ident );
}


addAttenuate( LoudSpeaker, acoustic );


}; /* namespace acoustic */
