/*
  comedi/comedinipfi.cc
  Controlls the PFI pins of a NI daq-board via comedi.

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

#include <iostream>
#include <sstream>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include <relacs/str.h>
#include <relacs/comedi/comedinipfi.h>
using namespace std;
using namespace relacs;

namespace comedi {


const string ComediNIPFI::PFISignals[PFISignalsMax] = {
  "PFI_DEFAULT", "AI_START1", "AI_START2", "AI_CONVERT", "G_SRC1", "G_GATE1", "AO_UPDATE_N", "AO_START1",
  "AI_START_PULSE", "G_SRC0", "G_GATE0", "EXT_STROBE", "AI_EXT_MUX_CLK", "GOUT0", "GOUT1", "FREQ_OUT",
  "PFI_DO", "I_ATRIG", "RTSI0", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID", "INVALID",
  "INVALID", "PXI_STAR_TRIGGER_IN", "SCXI_TRIG1", "DIO_CHANGE_DETECT_RTSI", "CDI_SAMPLE", "CDO_UPDATE" };


ComediNIPFI::ComediNIPFI( void ) 
  : ComediRouting( "ComediNIPFI" )
{
  clear();
  initOptions();
}


ComediNIPFI::ComediNIPFI( const string &device, const Options &opts ) 
  : ComediNIPFI()
{
  Options::read(opts);
  open( device );
}

  
ComediNIPFI::~ComediNIPFI( void ) 
{
}


void ComediNIPFI::initOptions()
{
  Device::initOptions();

  addInteger( "channel", "Channel", 0, 0, 100 );
  string pfisignals = "";
  for ( int k=0; k<PFISignalsMax; k++ ) {
    if ( k > 0 )
      pfisignals += "|";
    pfisignals += PFISignals[k];
   }
  addSelection( "routing", "Signal to be routed", pfisignals );
}


int ComediNIPFI::open( const string &device )
{ 
  clearError();
  Info.clear();
  Settings.clear();

  // check PFI subdevice no. 7
  int subdev = 7;

  // get channel:
  int channel = integer( "channel", 0, -1 );
  if ( channel < 0 ) {
    setErrorStr( "missing or invalid channel for device " + deviceIdent() );
    return WriteError;
  }

  // get routing:
  int routing = index( "routing" );
  if ( routing < 0 ) {
    setErrorStr( "missing or invalid routing parameter for device " + deviceIdent() );
    return WriteError;
  }
  if ( routing >= PFISignalsMax || PFISignals[routing] == "INVALID" ) {
    setErrorStr( "invalid routing parameter for device " + deviceIdent() );
    return WriteError;
  }

  return ComediRouting::open( device, subdev, channel, routing, PFISignals[routing] );
}


}; /* namespace comedi */
