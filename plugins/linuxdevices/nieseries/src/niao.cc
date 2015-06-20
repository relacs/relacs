/*
  nieseries/niao.cc
  Implementation of AnalogOutput for National Instruments E-Series

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
#include <cerrno>
#include <vector>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>   
#include <relacs/stats.h>
#include <relacs/nieseries/niai.h>
#include <relacs/nieseries/niao.h>
using namespace std;
using namespace relacs;

namespace nieseries {


NIAO::NIAO( void )
  : AnalogOutput( "NI E-Series Analog Output", NIAnalogIOType ),
    Handle( -1 )
{
  Sigs = 0;

  initOptions();
}


NIAO::NIAO( const string &device, const Options &opts )
  : NIAO()
{
  Options::read(opts);
  open( device );
}


NIAO::~NIAO( void )
{
  close();
}

void NIAO::initOptions()
{
  AnalogInput::initOptions();

  addNumber( "extref", "Voltage of external reference", -1.0, -1.0, 100.0, 0.1, "V" );
}

int NIAO::open( const string &device )
{
  clearError();
  Info.clear();
  Settings.clear();

  if ( device.empty() )
    Handle = -1;
  else
    Handle = ::open( device.c_str(), O_WRONLY | O_NONBLOCK );

  if ( Handle >= 0 ) {
    ::ioctl( Handle, NIDAQINFO, &Board );
    setDeviceName( Board.name );
    setDeviceVendor( "National Instruments" );
    setDeviceFile( device );
    // external reference:
    double extr = number( "extref", -1.0, "V" );
    setExternalReference( extr );
  }

  setInfo();

  return 0;
}


bool NIAO::isOpen( void ) const
{
  return ( Handle >= 0 );
}


void NIAO::close( void )
{
  ::close( Handle );
  Handle = -1;
  Info.clear();
}


int NIAO::channels( void ) const
{
  return Board.aoch;
}


int NIAO::bits( void ) const
{
  return Board.aobits;
}


double NIAO::maxRate( void ) const
{
  return double( Board.aomaxspl );
}


int NIAO::directWrite( OutList &sigs )
{
  cerr << "NIAO::directWrite() is not yet implemented!\n";
  return 0;
}


int NIAO::testWriteDevice( OutList &sigs )
{
  // check channel ordering:
  if ( sigs.size() > 1 ) {
    vector< unsigned int > chs( sigs.size() );
    for ( int k=0; k<sigs.size(); k++ )
      chs[k] = sigs[k].channel();
    sort( chs.begin(), chs.end() );
    for ( unsigned int k=0; k<chs.size(); k++ ) {
      if ( chs[k] != k ) {
	sigs.addError( DaqError::InvalidChannelSequence );
	break;
      }
    }
  }

  // ranges:
  int maxrange = 1 << bits();
  for ( int k=0; k<sigs.size(); k++ ) {
    // minimum and maximum values:
    double min = sigs[k].requestedMin();
    double max = sigs[k].requestedMax();
    if ( min == OutData::AutoRange || max == OutData::AutoRange ) {
      float smin = 0.0;
      float smax = 0.0;
      relacs::minMax( smin, smax, sigs[k] );
      if ( min == OutData::AutoRange )
	min = smin;
      if ( max == OutData::AutoRange )
	max = smax;
    }
    // reference and polarity:
    bool unipolar = false;
    if ( min >= 0.0 && Board.type != NI_PCI_6035E )
      unipolar = true;
    double extref = false;
    if ( max == OutData::ExtRef )
      extref = true;
    else {
      // maximum value:
      min = ::fabs( min );
      if ( min > max )
	max = min;
    }

    // allocate gain factor:
    char *gaindata = sigs[k].gainData();
    if ( gaindata != NULL )
      delete [] gaindata;
    gaindata = new char[sizeof(double)];
    sigs[k].setGainData( gaindata );
    double *gainp = (double *)gaindata;

    // set range:
    double maxboardvolt = 10.0;
    if ( sigs[k].noIntensity() ) {
      if ( ! extref ) {
	double maxvolt = sigs[k].getVoltage( max );
	if ( externalReference() < maxboardvolt ) {
	  if ( maxvolt < externalReference() )
	    extref = true;
	}
	else {
	  if ( maxvolt > maxboardvolt )
	    extref = true;
	}
      }
      if ( extref ) {
	if ( externalReference() < 0.0 ) {
	  sigs[k].addError( DaqError::InvalidReference );
	  extref = false;
	}
	else {
	  if ( externalReference() == 0.0 )
	    maxboardvolt = 1.0;
	  else
	    maxboardvolt = externalReference();
	}
      }
      *gainp = unipolar ? maxrange/maxboardvolt : maxrange/2/maxboardvolt;
    }
    else {
      if ( extref && externalReference() < 0.0 ) {
	sigs[k].addError( DaqError::InvalidReference );
	extref = false;
      }
      *gainp = unipolar ? maxrange : maxrange/2;
    }
    int index = 0;
    if ( unipolar )
      index |= 1;
    if ( extref )
      index |= 2;
    sigs[k].setGainIndex( index );
    sigs[k].setMinVoltage( unipolar ? 0.0 : -maxboardvolt );
    sigs[k].setMaxVoltage( maxboardvolt );
  }
    
  // continuous analog output not suppported:
  if ( sigs[0].continuous() ) {
    sigs.addError( DaqError::InvalidContinuous );
    sigs.setContinuous( false );
  }

  // multiple references?
  for ( int k=1; k<sigs.size(); k++ ) {
    if ( ( sigs[k].gainIndex() & 1 ) != ( sigs[0].gainIndex() & 1 ) ) {
      sigs[k].addError( DaqError::MultipleReferences );
    }
  }

  // timebase:
  long f = ::ioctl( Handle, NIDAQTIMEBASE );
  if ( f <= 0 ) {
    sigs.addErrorStr( errno );
    return -1;
  }
  double bf = double( f );

  // set delay
  long dc = long( ::rint( sigs[0].delay() * bf ) );
  sigs.setDelay( double( dc ) / bf );

  // set rate:
  long sc = long( ::rint( bf / sigs[0].sampleRate() ) );
  sigs.setSampleRate( bf / double( sc ) );

  return sigs.failed() ? -1 : 0;
}


int NIAO::convertData( OutList &sigs )
{
  // copy and sort signal pointers:
  OutList ol;
  ol.add( sigs );
  ol.sortByChannel();

  // set scaling factors:
  double scale[ ol.size() ];
  double minval[ ol.size() ];
  double maxval[ ol.size() ];
  for ( int k=0; k<ol.size(); k++ ) {
    double *gainp = (double *)ol[k].gainData();
    scale[k] = (*gainp) * ol[k].scale();
    minval[k] = ol[k].minValue();
    maxval[k] = ol[k].maxValue();
  }

  // allocate buffer:
  int nbuffer = ol.size()*ol[0].size();
  signed short *buffer = new signed short[nbuffer];

  // convert data and multiplex into buffer:
  signed short *bp = buffer;
  for ( int i=0; i<ol[0].size(); i++ ) {
    for ( int k=0; k<ol.size(); k++ ) {
      float v = ol[k][i];
      if ( v > maxval[k] )
	v = maxval[k];
      else if ( v < minval[k] ) 
	v = minval[k];
      *bp = (signed short) ::rint( v * scale[k] );
      ++bp;
    }
  }

  sigs[0].setDeviceBuffer( (char *)buffer, nbuffer, sizeof( signed short ) );

  return 0;
}


int NIAO::prepareWrite( OutList &sigs )
{
  // copy and sort signal pointers:
  OutList ol;
  ol.add( sigs );
  ol.sortByChannel();

  // reset analog output device:
  if ( ::ioctl( Handle, NIDAQAORESETBUFFER, 0 ) < 0 ) {
    ol.addErrorStr( "reset buffer" );
    ol.addErrorStr( errno );
    return -1;
  }

  // initialize channels:
  int maxrange = 1 << bits();
  if ( ::ioctl( Handle, NIDAQAOCLEARCONFIG, 0 ) != 0 ) {
    ol.addErrorStr( "clear AO configuration" );
    ol.addErrorStr( errno );
  }
  else {
    for ( int k=0; k<ol.size(); k++ ) {
      unsigned int u = 0;

      // polarity:
      bool unipolar = true;
      if ( ( ol[k].gainIndex() & 1 ) == 0 ) {
	unipolar = false;
	u |= 0x0001;
      }
      u |= 0x0001;

      // reglitch:
      if ( ol[k].reglitch() )
	u |= 0x0002;

      // channel number:
      int channel = ol[k].channel();
      channel &= 0x1;
      u |= channel << 8;

      // reference:
      bool extref = false;
      if ( ( ol[k].gainIndex() & 2 ) == 2 ) {
	extref = true;
	u |= 0x0004;
      }

      // allocate gain factor:
      char *gaindata = ol[k].gainData();
      if ( gaindata != NULL )
	delete [] gaindata;
      gaindata = new char[sizeof(double)];
      ol[k].setGainData( gaindata );
      double *gainp = (double *)gaindata;

      // range:
      double maxboardvolt = 10.0;
      if ( ol[k].noIntensity() ) {
	if ( extref )
	  maxboardvolt = externalReference();
	*gainp = unipolar ? maxrange/maxboardvolt : maxrange/2/maxboardvolt;
      }
      else
	*gainp = unipolar ? maxrange : maxrange/2;
      ol[k].setMinVoltage( unipolar ? 0.0 : -maxboardvolt );
      ol[k].setMaxVoltage( maxboardvolt );

      // add channel:
      if ( ::ioctl( Handle, NIDAQAOADDCHANNEL, u ) != 0 ) {
	ol[k].addError( DaqError::InvalidChannelType );
	::ioctl( Handle, NIDAQAOCLEARCONFIG, 0 );
	break;
      }

    }
  }

  // start source:
  if ( ::ioctl( Handle, NIDAQAOSTART1SOURCE, ol[0].startSource() ) != 0 ) {
    int ern = errno;
    if ( ern == EINVAL ) {
      // set valid start source?
      ol.addError( DaqError::InvalidStartSource );
    }
    else {
      ol.addErrorStr( "set start source" );
      ol.addErrorStr( ern );
    }
  }

  // timebase:
  long f = ::ioctl( Handle, NIDAQTIMEBASE );
  if ( f <= 0 ) {
    ol.addErrorStr( "get timebase" );
    ol.addErrorStr( errno );
    return -1;
  }
  double bf = double( f );

  // set delay
  long dc = long( ::rint( ol[0].delay() * bf ) );
  if ( ::ioctl( Handle, NIDAQAOSTART, dc ) == 0 )
    ol.setDelay( double( dc ) / bf );
  else {
    int ern = errno;
    if ( ern == EINVAL )
      ol.addError( DaqError::InvalidDelay );
    else {
      ol.addErrorStr( "set delay" );
      ol.addErrorStr( ern );
    }
  }

  // set rate:
  long sc = long( rint( bf / ol[0].sampleRate() ) );
  if ( ::ioctl( Handle, NIDAQAOINTERVAL, sc ) == 0 )
    ol.setSampleRate( bf / double( sc ) );
  else {
    int ern = errno;
    if ( ern == EINVAL )
      ol.addError( DaqError::InvalidSampleRate );
    else {
      ol.addErrorStr( "set sampling rate" );
      ol.addErrorStr( ern );
    }
  }

  if ( ol.success() ) {
    setSettings( ol, sigs[0].deviceBufferSize() );
    Sigs = &sigs;
  }

  return ol.failed() ? -1 : 0;
}


int NIAO::startWrite( void )
{
  if ( Sigs == 0 )
    return -1;

  // XXX use buffer for 5ms, this can be improved:
  int bufsize = Sigs->size()*sizeof( signed short )*(*Sigs)[0].indices( 0.005 );
  int r = write( Handle, (*Sigs)[0].deviceBuffer(), bufsize );

  if ( r < 0 ) {
    int ern = errno;
    if ( ern == EBUSY )
      Sigs->addError( DaqError::Busy );
    else
      Sigs->addErrorStr( ern );
  }
  else if ( r < (*Sigs)[0].deviceBufferSize()*(*Sigs)[0].deviceDataSize() )
    Sigs->addError( DaqError::Unknown );
  // add errno! XXX

  return Sigs->failed() ? -1 : 0;
}


int NIAO::writeData( void )
{
  return 0;
}


int NIAO::reset( void )
{
  int r = ::ioctl( Handle, NIDAQAORESETALL, 0 );

  Sigs = 0;

  Settings.clear();

  return r;
}


bool NIAO::running( void ) const
{
  return ( ::ioctl( Handle, NIDAQAORUNNING, 0 ) > 0 );
}


int NIAO::error( void ) const
{
  int err = 0;
  ::ioctl( Handle, NIDAQAOERROR, &err );
  /*
    bit 1: AO_Overrun_St
    bit 2: AO_BC_TC_Error
    bit 3: AO_BC_TC_Trigger_Error
   */
  return err;
  /*
    0: ok
    1: OverflowUnderrun
    other
  */
}


long NIAO::index( void ) const
{
  long index=0;
  ::ioctl( Handle, NIDAQAOBUFFERSTART, &index );
  return index;
}


int NIAO::getAISyncDevice( const vector< AnalogInput* > &ais ) const
{
  for ( unsigned int k=0; k<ais.size(); k++ ) {
    if ( ais[k]->analogInputType() == NIAnalogIOType &&
	 ais[k]->deviceFile().size() > 0 && deviceFile().size() > 0 &&
	 ais[k]->deviceFile()[ais[k]->deviceFile().size()-1] == deviceFile()[deviceFile().size()-1] ) {
      return k;
    }
  }
  return -1;
}


}; /* namespace nieseries */
