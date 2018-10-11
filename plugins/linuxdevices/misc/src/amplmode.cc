/*
  misc/amplmode.cc
  Control the mode of an amplifier via DigitalIO

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
#include <relacs/misc/amplmode.h>
using namespace relacs;

namespace misc {


AmplMode::AmplMode( DigitalIO &dio, const Options &opts )
  : AmplMode()
{
  Options::read(opts);
  open( dio );
}


AmplMode::AmplMode( void )
  : Device( "AmplMode" ),
    DIO( 0 )
{
  BridgePin = 0;
  CurrentClampPin = 1;
  VoltageClampPin = 2;
  DynamicClampPin = 5;
  SyncPin = 6;
  ResistancePin = 3;
  BuzzerPin = 4;

  BridgeMask = 0;
  CurrentClampMask = 0;
  VoltageClampMask = 0;
  DynamicClampMask = 0;
  SyncMask = 0;
  ResistanceMask = 0;
  BuzzerMask = 0;

  ModeMask = 0;
  Mask = 0;

  CurrentMode = 0;

  initOptions();
}


AmplMode::~AmplMode( void )
{
  close();
}

void AmplMode::initOptions()
{
  Device::initOptions();

  addInteger( "bridgepin", "DIO line for activating bridge mode", BridgePin );
  addInteger( "cclamppin", "DIO line for activating current clamp mode", CurrentClampPin );
  addInteger( "vclamppin", "DIO line for activating voltage clamp mode", VoltageClampPin );
  addInteger( "dclamppin", "DIO line for activating dynamic clamp mode", DynamicClampPin );
  addInteger( "syncpin", "DIO line for activating external synchronization", SyncPin );
  addInteger( "resistancepin", "DIO line for activating resistance measurement", ResistancePin );
  addInteger( "buzzerpin", "DIO line for activating buzzer", BuzzerPin );
}


int AmplMode::open( DigitalIO &dio )
{
  clearError();
  Info.clear();
  Settings.clear();

  DIO = &dio;

  if ( isOpen() ) {
    BridgePin = integer( "bridgepin", 0, BridgePin );
    CurrentClampPin = integer( "cclamppin", 0, CurrentClampPin );
    VoltageClampPin = integer( "vclamppin", 0, VoltageClampPin );
    DynamicClampPin = integer( "dclamppin", 0, DynamicClampPin );
    SyncPin = integer( "syncpin", 0, SyncPin );
    if ( DynamicClampPin < 0 )
      SyncPin = -1;
    ResistancePin = integer( "resistancepin", 0, ResistancePin );
    BuzzerPin = integer( "buzzerpin", 0, BuzzerPin );

    BridgeMask = BridgePin >= 0 ? 1 << BridgePin : 0;
    CurrentClampMask = CurrentClampPin >= 0 ? 1 << CurrentClampPin : 0;
    VoltageClampMask = VoltageClampPin >= 0 ? 1 << VoltageClampPin : 0;
    DynamicClampMask = DynamicClampPin >= 0 ? 1 << DynamicClampPin : 0;
    SyncMask = SyncPin >= 0 ? 1 << SyncPin : 0;
    ResistanceMask = ResistancePin >= 0 ? 1 << ResistancePin : 0;
    BuzzerMask = BuzzerPin >= 0 ? 1 << BuzzerPin : 0;

    ModeMask =  BridgeMask + CurrentClampMask + VoltageClampMask + DynamicClampMask + ResistanceMask;
    Mask = ModeMask + SyncMask + BuzzerMask;
    DIOId = DIO->allocateLines( Mask );
    if ( DIOId < 0 ) {
      setErrorStr( "cannot allocate pins on lines #" + Str( -DIOId, "%04x" ) + '.' );
      addErrorStr( "bridgepin=" + Str( BridgePin ) + '.' );
      addErrorStr( "cclamppin=" + Str( CurrentClampPin ) + '.' );
      addErrorStr( "vclamppin=" + Str( VoltageClampPin ) + '.' );
      addErrorStr( "dclamppin=" + Str( DynamicClampPin ) + '.' );
      addErrorStr( "syncpin=" + Str( SyncPin ) + '.' );
      addErrorStr( "resistancepin=" + Str( ResistancePin ) + '.' );
      addErrorStr( "buzzerpin=" + Str( BuzzerPin ) + '.' );
      DIO = 0;
      return InvalidDevice;
    }
    else {
      setDeviceFile( DIO->deviceIdent() );
      open();
      return 0;
    }
  }
  else {
    DIO = 0;
    if ( DIO != 0 && ! DIO->isOpen() )
      return NotOpen;
    else
      return InvalidDevice;
  }
}


int AmplMode::open( Device &device )
{
  return open( dynamic_cast<DigitalIO&>( device ) );
}


void AmplMode::open()
{
  if ( isOpen() ) {

    Info.clear();
    Settings.clear();

    // configure for parallel output: 
    DIO->configureLines( Mask, Mask );
    // manual mode selection, no buzz:
    CurrentMode = 0x00;
    DIO->writeLines( Mask, CurrentMode );

    // check for sync support:
    if ( DynamicClampMask == 0 )
      SyncMask = 0;
    else {
      if ( DIO->clearSyncPulse( ModeMask, CurrentMode ) != 0 ) {
	setErrorStr( "You may want to set dclamppin=-1 in the configuration for the AmplMode plugin to suppress this warning." );
	DynamicClampMask = 0;
	SyncMask = 0;
      }
    }

    setDeviceVendor( "npi electronic GmbH (Tamm, Germany)" );
    setDeviceName( "SEC-05LX" );
    addInfo();
    if ( BridgeMask > 0 )
      Info.addInteger( "bridgepin", BridgePin );
    if ( CurrentClampMask > 0 )
      Info.addInteger( "cclamppin", CurrentClampPin );
    if ( VoltageClampMask > 0 )
      Info.addInteger( "vclamppin", VoltageClampPin );
    if ( DynamicClampMask > 0 )
      Info.addInteger( "dynclamppin", DynamicClampPin );
    if ( SyncMask > 0 )
      Info.addInteger( "syncpin", SyncPin );
    if ( ResistanceMask > 0 )
      Info.addInteger( "resistancepin", ResistancePin );
    if ( BuzzerMask > 0 )
      Info.addInteger( "buzzerpin", BuzzerPin );
  }
}


bool AmplMode::isOpen( void ) const
{
  return ( DIO != 0 && DIO->isOpen() );
}


void AmplMode::close( void )
{
  if ( isOpen() ) {
    // manual mode selection, no buzz:
    DIO->writeLines( Mask, 0x00 );

    DIO->freeLines( DIOId );
  }

  DIO = 0;

  Info.clear();
  Settings.clear();
}


string AmplMode::errorStr( void ) const
{
  string es = "";
  if ( DIO != 0 )
    es = DIO->errorStr();
  if ( ! Device::errorStr().empty() ) {
    if ( ! es.empty() ) {
      if ( es.back() != '.' )
	es += '.';
      es += ' ';
    }
    es += Device::errorStr();
  }
  return es;
}


bool AmplMode::supportsBridgeMode( void ) const
{
  return ( BridgeMask > 0 );
}


bool AmplMode::supportsCurrentClampMode( void ) const
{
  return ( CurrentClampMask > 0 );
}


bool AmplMode::supportsVoltageClampMode( void ) const
{
  return ( VoltageClampMask > 0 );
}


bool AmplMode::supportsDynamicClampMode( void ) const
{
  return ( CurrentClampMask > 0 && DynamicClampMask > 0 && SyncMask > 0 );
}


int AmplMode::setBridgeMode( void )
{
  if ( ! isOpen() )
    return NotOpen;

  if ( BridgeMask == 0 )
    return InvalidParam;

  CurrentMode = BridgeMask;
  if ( supportsDynamicClampMode() )
    return DIO->clearSyncPulse( ModeMask, CurrentMode );
  else
    return DIO->writeLines( ModeMask, CurrentMode );
}


int AmplMode::setCurrentClampMode( void )
{
  if ( ! isOpen() )
    return NotOpen;

  if ( CurrentClampMask == 0 )
    return InvalidParam;

  CurrentMode = CurrentClampMask;
  if ( supportsDynamicClampMode() )
    return DIO->clearSyncPulse( ModeMask, CurrentMode );
  else
    return DIO->writeLines( ModeMask, CurrentMode );
}


int AmplMode::setDynamicClampMode( double duration, double mode )
{
  if ( ! isOpen() )
    return NotOpen;

  if ( ! supportsDynamicClampMode() )
    return InvalidParam;

  CurrentMode = CurrentClampMask | DynamicClampMask;
  return DIO->setSyncPulse( ModeMask, CurrentMode, SyncPin, duration, mode );
}


int AmplMode::setVoltageClampMode( void )
{
  if ( ! isOpen() )
    return NotOpen;

  if ( VoltageClampMask == 0 )
    return InvalidParam;

  CurrentMode = VoltageClampMask;
  if ( supportsDynamicClampMode() )
    return DIO->clearSyncPulse( ModeMask, CurrentMode );
  else
    return DIO->writeLines( ModeMask, CurrentMode );
}


int AmplMode::setManualSelection( void )
{
  if ( ! isOpen() )
    return NotOpen;

  CurrentMode = 0x00;
  if ( supportsDynamicClampMode() )
    return DIO->clearSyncPulse( ModeMask, CurrentMode );
  else
    return DIO->writeLines( ModeMask, CurrentMode );
}


int AmplMode::startResistance( void )
{
  if ( ! isOpen() )
    return NotOpen;

  if ( ResistanceMask == 0 )
    return InvalidParam;

  return DIO->writeLines( ModeMask, ResistanceMask );
}


int AmplMode::stopResistance( void )
{
  if ( ! isOpen() )
    return NotOpen;

  return DIO->writeLines( ModeMask, CurrentMode );
}


int AmplMode::startBuzz( void )
{
  if ( ! isOpen() )
    return NotOpen;

  if ( BuzzerPin < 0 )
    return InvalidParam;
  
  return DIO->write( BuzzerPin, true );
}


int AmplMode::stopBuzz( void )
{
  if ( ! isOpen() )
    return NotOpen;

  if ( BuzzerPin < 0 )
    return InvalidParam;

  return DIO->write( BuzzerPin, false );
}


}; /* namespace misc */
