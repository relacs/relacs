/*
  daqflex/daqflexcore.cc
  The DAQFlex interface over libusb

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

#include <ctype.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <relacs/daqflex/daqflexcore.h>
using namespace std;
using namespace relacs;

namespace daqflex {


const string DAQFlexCore::DefaultFirmwarePath = "/usr/lib/daqflex/";

const string DAQFlexCore::DAQFlexErrorText[DAQFlexCore::DAQFlexErrorMax] =
  { "success",
    "no device",
    "invalid ID",
    "USB init failed",
    "pipe",
    "transfer failed",
    "invalid buffer size",
    "failed to open FPGA file",
    "FPGA upload failed",
    "libusb IO",
    "libusb invalid parameter",
    "libusb access",
    "libusb no device",
    "libusb not found",
    "libusb busy",
    "libusb timeout",
    "libusb overflow",
    "libusb pipe",
    "libusb interrupted",
    "libusb no memory",
    "libusb not supported",
    "libusb other",
    "libusb unknown" };


DAQFlexCore::DAQFlexCore( void )
  : Device( "DAQFlexCore" ),
    DeviceHandle( NULL ),
    ErrorState( Success )
{
  initOptions();
}


DAQFlexCore::DAQFlexCore( const string &device, const Options &opts  )
  : DAQFlexCore()
{
  Options::read(opts);
  open( device );
}


DAQFlexCore::~DAQFlexCore( void )
{
  close();
}

void DAQFlexCore::initOptions()
{
  Device::initOptions();

  addText( "serialno", "Serial number of DAQFlex device", "" );
  addInteger( "devicenum", "Take DAQFlex device number", 1 );
  addText( "firmwarepath", "Path to firmware files", "" );
}

int DAQFlexCore::open( const string &devicestr )
{
  clearError();

  Info.clear();
  Settings.clear();

  int productid = 0;
  /* set it from options:
     productid = opts.number( "productid" );
  // check if the product ID is a valid MCC product ID:
  if ( productid != USB_2001_TC &&
       productid != USB_7202 &&
       productid != USB_7204 &&
       productid != USB_1608_G &&
       productid != USB_1608_GX &&
       productid != USB_1608_GX_2AO &&
       productid != USB_201 &&
       productid != USB_204 &&
       productid != USB_202 &&
       productid != USB_205 &&
       productid != USB_2408 &&
       productid != USB_2408_2AO &&
       productid != USB_1208_FS_Plus &&
       productid != USB_1408_FS_Plus &&
       productid != USB_1608_FS_Plus &&
       productid != 0 ) {
    setErrorStr( "Product ID " + Str( productid ) + " not supported." );
    ErrorState = ErrorInvalidID;
    return ErrorState;
  }
  */

  string serialno = text( "serialno", "" );

  int mccdevicenum = integer( "devicenum", 0, 1 );

  // initialize USB libraries:
  if ( libusb_init( NULL ) != 0 ) {
    ErrorState = ErrorUSBInit;
    return ErrorState;
  }

  // get the list of USB devices connected to the PC:
  libusb_device **list;
  ssize_t listsize = libusb_get_device_list( NULL, &list );

  // traverse the list of USB devices to find the requested device:
  int mccdevicecount = 0;
  bool found = false;
  for ( int i=0; i<listsize && !found; i++ ) {
    ErrorState = Success;
    libusb_device *device = list[i];
    libusb_device_descriptor desc;
    libusb_get_device_descriptor( device, &desc );
    if ( desc.idVendor == MCCVendorID &&
	 ( desc.idProduct == productid || productid == 0 ) ) {
      mccdevicecount++;
      ProductID = desc.idProduct;
      if ( ! serialno.empty() || mccdevicenum == mccdevicecount ) {
	// open the device:
	int ern = libusb_open( device, &DeviceHandle );
	setLibUSBError( ern );
	if ( ErrorState == Success ) {
	  // claim interface with the device:
	  ern = libusb_claim_interface( DeviceHandle, 0 );
	  setLibUSBError( ern );
	  if ( ErrorState == Success ) {
	    // get input and output endpoints:
	    getEndpoints();
	    if ( ErrorState != Success ) {
	      libusb_release_interface( DeviceHandle, 0 );
	      continue;
	    }
	    InPacketSize = libusb_get_max_iso_packet_size( device, EndpointIn );
	    OutPacketSize = libusb_get_max_iso_packet_size( device, EndpointOut );
	    // get the device serial number:
	    string message = sendMessage( "?DEV:MFGSER" );
	    // erase message while keeping serial number:
	    message.erase( 0, 11 );
	    cout << "DAQFlex: found device " << productName( ProductID )
		 << " with serial number " << message << "\n";
	    if ( ! serialno.empty() ) {
	      // check that the serial numbers are the same:
	      if ( message.compare( serialno ) )
		// serial numbers are not the same, release device and continue on:
		libusb_release_interface( DeviceHandle, 0 );
	      else
		// serial numbers are the same, this is the correct device:
		found = true;
	    }
	    else
	      found = true;
	  }
	}
	else {
	  if ( ErrorState == ErrorLibUSBAccess )
	    setErrorStr( "you do not have the permissions to access the USB device." );
	}
      }
    }
  }

  libusb_free_device_list( list, true );

  if ( !found ) {
    if ( ErrorState == Success ) {
      setErrorStr( "did not find an USB device. Try to reconnect the USB DAQ board or exchange the USB cable." );
      ErrorState = ErrorNoDevice;
    }
    DeviceHandle = NULL;
  }
  else {
    Str path = text( "firmwarepath" );
    if ( path.empty() )
      path = DefaultFirmwarePath;
    path.provideSlash();
    initDevice( path );
    if ( ErrorState != Success ) {
      if ( ErrorState == ErrorLibUSBIO )
	setErrorStr( "check the USB cable/connector!" );
      close();
    }
  }

  AISampleRate = 0.0;

  return ErrorState;
}


bool DAQFlexCore::isOpen( void ) const
{
  lock();
  bool o = ( DeviceHandle != NULL );
  unlock();
  return o;
}


void DAQFlexCore::close( void )
{
  if ( isOpen() ) {
    // free memory and devices:
    libusb_release_interface( DeviceHandle, 0 );
    libusb_close( DeviceHandle );
    libusb_exit( NULL );
    DeviceHandle = NULL;
  }

  Info.clear();
}


int DAQFlexCore::reset( void )
{
  return 0;
}


unsigned int DAQFlexCore::maxAIData( void ) const
{
  return MaxAIData;
}


double DAQFlexCore::maxAIRate( void ) const
{
  return MaxAIRate;
}


int DAQFlexCore::maxAIChannels( void ) const
{
  return MaxAIChannels;
}


int DAQFlexCore::aiFIFOSize( void ) const
{
  return AIFIFOSize;
}


void DAQFlexCore::setAISampleRate( double samplerate )
{
  AISampleRate = samplerate;
}


double DAQFlexCore::aiSampleRate( void ) const
{
  return AISampleRate;
}


unsigned int DAQFlexCore::maxAOData( void ) const
{
  return MaxAOData;
}


double DAQFlexCore::maxAORate( void ) const
{
  return MaxAORate;
}


int DAQFlexCore::maxAOChannels( void ) const
{
  return MaxAOChannels;
}


int DAQFlexCore::aoFIFOSize( void ) const
{
  return AOFIFOSize;
}


int DAQFlexCore::dioLines( void ) const
{
  return DIOLines;
}


libusb_device_handle *DAQFlexCore::deviceHandle( void )
{
  return DeviceHandle;
}


unsigned char DAQFlexCore::endpointIn( void )
{
  return EndpointIn;
}


unsigned char DAQFlexCore::endpointOut( void )
{
  return EndpointOut;
}


int DAQFlexCore::inPacketSize( void ) const
{
  return InPacketSize;
}


int DAQFlexCore::outPacketSize( void ) const
{
  return OutPacketSize;
}


string DAQFlexCore::productName( int productid )
{
  switch( productid ) {
  case USB_2001_TC:
    return "USB-2001-TC";
  case USB_7202:
    return "USB-7202";
  case USB_7204:
    return "USB-7204";
  case USB_1608_G:
    return "USB-1608G";
  case USB_1608_GX:
    return "USB-1608GX";
  case USB_1608_GX_2AO:
    return "USB-1608GX-2AO";
  case USB_201:
    return "USB-201";
  case USB_204:
    return "USB-204";
  case USB_202:
    return "USB-202";
  case USB_205:
    return "USB-205";
  case USB_2408:
    return "USB-2408";
  case USB_2408_2AO:
    return "USB-2408-2AO";
  case USB_1208_FS_Plus:
    return "USB-1208-FS-Plus";
  case USB_1408_FS_Plus:
    return "USB-1408-FS-Plus";
  case USB_1608_FS_Plus:
    return "USB-1608-FS-Plus";
  default:
    return "Invalid Product ID";
  }
}


int DAQFlexCore::sendControlTransfer( const string &message )
{
  unsigned char data[MaxMessageSize];
  for ( unsigned int i = 0; i < MaxMessageSize; i++ )
    data[i] = i < message.size() ? toupper( message[i] ) : 0;
  int numbytes = libusb_control_transfer( DeviceHandle,
					  LIBUSB_REQUEST_TYPE_VENDOR + LIBUSB_ENDPOINT_OUT,
					  StringMessage, 0, 0, data,
					  MaxMessageSize, 100 );
  setLibUSBError( numbytes );
  return ErrorState;
}


string DAQFlexCore::getControlTransfer( void )
{
  unsigned char message[MaxMessageSize];
  int numbytes = libusb_control_transfer( DeviceHandle,
					  LIBUSB_REQUEST_TYPE_VENDOR + LIBUSB_ENDPOINT_IN,
					  StringMessage, 0, 0,
					  message, MaxMessageSize, 100 );

  setLibUSBError( numbytes );
  if ( ErrorState != Success )
    return "";

  return (char *)message;
}


string DAQFlexCore::sendMessage( const string &message )
{
  lock();
  int r = sendControlTransfer( message );
  string s = "";
  if ( r == Success )
    s = getControlTransfer();
  unlock();
  return s;
}


string DAQFlexCore::sendMessageUnlocked( const string &message )
{
  int r = sendControlTransfer( message );
  string s = "";
  if ( r == Success )
    s = getControlTransfer();
  return s;
}


int DAQFlexCore::sendCommand( const string &command )
{
  lock();
  int r = sendControlTransfer( command );
  unlock();
  return r;
}


int DAQFlexCore::sendCommands( const string &command1, const string &command2 )
{
  lock();
  int r = sendControlTransfer( command1 );
  if ( r == Success )
    r = sendControlTransfer( command2 );
  unlock();
  return r;
}


int DAQFlexCore::getEndpoints( void )
{
  unsigned char epdescriptor[MaxMessageSize];
  int numbytes = libusb_control_transfer( DeviceHandle, StringMessage,
					  LIBUSB_REQUEST_GET_DESCRIPTOR,
					  (0x02 << 8) | 0, 0,
					  epdescriptor, MaxMessageSize, 1000 );

  setLibUSBError( numbytes );
  if ( ErrorState != Success )
    return -1;

  EndpointIn = getEndpointInAddress( epdescriptor, numbytes );
  EndpointOut = getEndpointOutAddress( epdescriptor, numbytes );
  return ErrorState;
}


unsigned char DAQFlexCore::getEndpointInAddress( unsigned char* data, int n )
{
  int length = 0;
  int descriptortype = 0;

  int index = 0;
  while ( true ) {
    length = data[index];
    descriptortype = data[index + 1];
    if ( length == 0 )
      break;

    if ( descriptortype != 0x05 )
      index += length;
    else {
      if ( (data[index + 2] & 0x80) != 0 )
	return data[index + 2];
      else
	index += length;
    }

    if ( index >= n )
      break;
  }

  return 0;
}


unsigned char DAQFlexCore::getEndpointOutAddress( unsigned char* data, int n )
{
  int length;
  int descriptortype;

  int index = 0;
  while ( true ) {
    length = data[index];
    descriptortype = data[index + 1];
    if ( length == 0 )
      break;

    if ( descriptortype != 0x05 )
      index += length;
    else {
      if ( (data[index + 2] & 0x80 ) == 0 )
	return data[index + 2];
      else
	index += length;
    }

    if ( index >= n )
      break;
  }

  return 0;
}


int DAQFlexCore::initDevice( const string &path )
{
  ErrorState = Success;
  string fpgav = "";

  switch ( ProductID ) {
  case USB_1608_G:
  case USB_1608_GX:
  case USB_1608_GX_2AO:
    MaxAIData = 0xFFFF;
    if ( ProductID == USB_1608_G )
      MaxAIRate = 250000.0;
    else
      MaxAIRate = 500000.0;
    MaxAIChannels = 16;
    AIFIFOSize = 4096;
    if ( ProductID == USB_1608_GX_2AO ) {
      MaxAOData = 0xFFFF;
      MaxAORate = 500000.0;
      MaxAOChannels = 2;
      AOFIFOSize = 2048;
    }
    else {
      MaxAOData = 0;
      MaxAORate = 0.0;
      MaxAOChannels = 0;
      AOFIFOSize = 0;
    }
    DIOLines = 8;
    uploadFPGAFirmware( path, "USB_1608G.rbf" );
    if ( ErrorState != Success )
      return ErrorState;
    fpgav = sendMessage( "?DEV:FPGAV" );
    if ( ! fpgav.empty() )
      fpgav.erase( 0, 10 );
    if ( ErrorState != Success )
      return ErrorState;
    break;

  case USB_201:
    MaxAIData = 0x0FFF;
    MaxAIRate = 100000.0;
    MaxAIChannels = 8;
    AIFIFOSize = 12288;
    MaxAOData = 0;
    MaxAORate = 0.0;
    MaxAOChannels = 0;
    AOFIFOSize = 0;
    DIOLines = 8;
    break;

  case USB_202:
    MaxAIData = 0x0FFF;
    MaxAIRate = 100000.0;
    MaxAIChannels = 8;
    AIFIFOSize = 12288;
    MaxAOData = 0x0FFF;
    MaxAORate = 600.0;
    MaxAOChannels = 2;
    AOFIFOSize = -1;
    DIOLines = 8;
    break;

  case USB_204:
    MaxAIData = 0x0FFF;
    MaxAIRate = 500000.0;
    MaxAIChannels = 8;
    AIFIFOSize = 12288;
    MaxAOData = 0;
    MaxAORate = 0.0;
    MaxAOChannels = 0;
    AOFIFOSize = 0;
    DIOLines = 8;
    break;

  case USB_205:
    MaxAIData = 0x0FFF;
    MaxAIRate = 500000.0;
    MaxAIChannels = 8;
    AIFIFOSize = 12288;
    MaxAOData = 0x0FFF;
    MaxAORate = 600.0;
    MaxAOChannels = 2;
    AOFIFOSize = -1;
    DIOLines = 8;
    break;

  case USB_7202:
    MaxAIData = 0xFFFF;
    MaxAIRate = 50000.0;
    MaxAIChannels = 8;
    AIFIFOSize = 32768;
    MaxAOData = 0;
    MaxAORate = 0.0;
    MaxAOChannels = 0;
    AOFIFOSize = 0;
    DIOLines = 8;
    break;

  case USB_7204:
    MaxAIData = 0xFFF;
    MaxAIRate = 50000.0;
    MaxAIChannels = 8;
    AIFIFOSize = 32768;
    MaxAOData = 0xFFF;
    MaxAORate = 10000.0;
    MaxAOChannels = 2;
    AOFIFOSize = 0; // ???
    DIOLines = 8;
    break;

  case USB_1208_FS_Plus:
    MaxAIData = 0xFFF;
    MaxAIRate = 50000.0;
    MaxAIChannels = 8;
    AIFIFOSize = 0; // ???
    MaxAOData = 0xFFF;
    MaxAORate = 10000.0;
    MaxAOChannels = 2;
    AOFIFOSize = 0;  // ???
    DIOLines = 16;
    uploadFPGAFirmware( path, "USB_1208GHS.rbf" );
    if ( ErrorState != Success )
      return ErrorState;
    break;

  case USB_1408_FS_Plus:
    MaxAIData = 0xFFF;
    MaxAIRate = 48000.0;
    MaxAIChannels = 8;
    AIFIFOSize = 0; // ???
    MaxAOData = 0xFFF;
    MaxAORate = 10000.0;
    MaxAOChannels = 2;
    AOFIFOSize = 0; // ???
    DIOLines = 16;
    break;

  case USB_1608_FS_Plus:
    MaxAIData = 0xFFFF;
    MaxAIRate = 400000.0;
    MaxAIChannels = 8;
    AIFIFOSize = 32768;
    MaxAOData = 0;
    MaxAORate = 0.0;
    MaxAOChannels = 0;
    AOFIFOSize = 0;
    DIOLines = 8;
    break;

  case USB_2408:
    MaxAIData = 0xFFFFFF;
    MaxAIRate = 1000.0;
    MaxAIChannels = 16;
    AIFIFOSize = 32768;
    MaxAOData = 0;
    MaxAORate = 0.0;
    MaxAOChannels = 0;
    AOFIFOSize = 0;
    DIOLines = 8;
    break;

  case USB_2408_2AO:
    MaxAIData = 0xFFFFFF;
    MaxAIRate = 1000.0;
    MaxAIChannels = 16;
    AIFIFOSize = 32768;
    MaxAOData = 0xFFFF;
    MaxAORate = 1000.0;
    MaxAOChannels = 2;
    AOFIFOSize = 0;
    DIOLines = 8;
    break;

  default:
    ErrorState = ErrorInvalidID;
    return ErrorState;
  }

  // set basic device infos:
  setDeviceName( productName( ProductID ) );
  setDeviceVendor( "Measurement Computing" );
  setDeviceFile( "USB" );

  Device::addInfo();
  // get the device serial number:
  string serial = sendMessage( "?DEV:MFGSER" );
  // erase message while keeping serial number:
  serial.erase( 0, 11 );
  Info.addText( "SerialNumber", serial );

  // firmware version:
  string fwv = sendMessage( "?DEV:FWV" );
  if ( ! fwv.empty() ) {
    fwv.erase( 0, 8 );
    Info.addText( "Firmware version", fwv );
  }
  // fpga firmware version:
  if ( ! fpgav.empty() )
    Info.addText( "FPGA version", fpgav );

  return ErrorState;
}


int DAQFlexCore::uploadFPGAFirmware( const string &path, const string &filename )
{
  // check if the firmware has been loaded already:
  string response = sendMessage( "?DEV:FPGACFG" );
  if ( ErrorState != Success )
    return ErrorState;
  if ( response.find( "CONFIGURED" ) == string::npos ) {
    // firmware hasn't been loaded yet, do so:
    transferFPGAfile( path + filename );
    if ( ErrorState == ErrorCantOpenFPGAFile )
      transferFPGAfile( DefaultFirmwarePath + filename );
    if ( ErrorState == Success ) {
      // check if the firmware got loaded successfully:
      response = sendMessage( "?DEV:FPGACFG" );
      if ( ErrorState == Success && response.find( "CONFIGURED" ) == string::npos )
	ErrorState = ErrorFPGAUploadFailed;
    }
    else
      setErrorStr( "FPGA path: " + path + filename + " or " + DefaultFirmwarePath + filename );
  }
  return ErrorState;
}


int DAQFlexCore::transferFPGAfile( const string &path )
{
  ErrorState = Success;

  // turn on FPGA configure mode:
  sendMessage( "DEV:FPGACFG=0XAD" );
  if ( ErrorState != Success )
    return ErrorState;

  // open file for input in binary mode, cursor at end of file:
  ifstream file( path.c_str(), ios::in|ios::binary|ios::ate );
  if ( file.is_open() ) {
    // read file into memblock:
    int size = (int)file.tellg();
    file.seekg( 0, ios::beg );
    unsigned char * memblock = new unsigned char[size];
    file.read( (char*)memblock, size );
    file.close();

    int totalbytes = 0;
    while( totalbytes < size ) {

      uint16_t length = MaxMessageSize;
      if ( size - totalbytes < (int)MaxMessageSize )
	length = size - totalbytes;
      int numbytes = libusb_control_transfer( DeviceHandle,
					      LIBUSB_REQUEST_TYPE_VENDOR + LIBUSB_ENDPOINT_OUT,
					      FPGADATAREQUEST, 0, 0,
					      &memblock[totalbytes],
					      length, 1000 );

      if ( numbytes < 0 ) {
	setLibUSBError( numbytes );
	break;
      }

      totalbytes += numbytes;
    }

    delete[] memblock;
  }
  else
    ErrorState = ErrorCantOpenFPGAFile;

  return ErrorState;
}


DAQFlexCore::DAQFlexError DAQFlexCore::readBulkTransfer( unsigned char *data, int length,
							 int *transferred,
							 unsigned int timeout )
{
  int err = libusb_bulk_transfer( deviceHandle(), endpointIn(),
				  data, length, transferred, timeout );
  return getLibUSBError( err );
}


DAQFlexCore::DAQFlexError DAQFlexCore::writeBulkTransfer( unsigned char *data, int length,
							  int *transferred,
							  unsigned int timeout )
{
  int err = libusb_bulk_transfer( deviceHandle(), endpointOut(),
				  data, length, transferred, timeout );
  return getLibUSBError( err );
}


void DAQFlexCore::clearRead( void )
{
  libusb_clear_halt( deviceHandle(), endpointIn() );
  /* from the docu: Clear the halt/stall condition for an endpoint.
     Endpoints with halt status are unable to receive or transmit data
     until the halt condition is stalled.  YOU SHOULD CANCEL ALL
     PENDING TRANSFERS BEFORE ATTEMPTING TO CLEAR THE HALT CONDITION
     (is this really given when stopping relacs?).  This is a BLOCKING
     FUNCTION. */
}


void DAQFlexCore::clearWrite( void )
{
  // this blocks at high rates:
  libusb_clear_halt( deviceHandle(), endpointOut() );
  /* from the docu: Clear the halt/stall condition for an endpoint.
     Endpoints with halt status are unable to receive or transmit data
     until the halt condition is stalled.  YOU SHOULD CANCEL ALL
     PENDING TRANSFERS BEFORE ATTEMPTING TO CLEAR THE HALT CONDITION
     (is this really given when stopping relacs?).  This is a BLOCKING
     FUNCTION. */
}


DAQFlexCore::DAQFlexError DAQFlexCore::getLibUSBError( int libusberror )
{
  DAQFlexError error = Success;

  if ( libusberror >= 0 )
    error = Success;
  else {
    switch( libusberror ) {
    case LIBUSB_SUCCESS:
      error = Success; break;
    case LIBUSB_ERROR_IO:
      error = ErrorLibUSBIO; break;
    case LIBUSB_ERROR_INVALID_PARAM:
      error = ErrorLibUSBInvalidParam; break;
    case LIBUSB_ERROR_ACCESS:
      error = ErrorLibUSBAccess; break;
    case LIBUSB_ERROR_NO_DEVICE:
      error = ErrorLibUSBNoDevice; break;
    case LIBUSB_ERROR_NOT_FOUND:
      error = ErrorLibUSBNotFound; break;
    case LIBUSB_ERROR_BUSY:
      error = ErrorLibUSBBusy; break;
    case LIBUSB_ERROR_TIMEOUT:
      error = ErrorLibUSBTimeout; break;
    case LIBUSB_ERROR_OVERFLOW:
      error = ErrorLibUSBOverflow; break;
    case LIBUSB_ERROR_PIPE:
      error = ErrorLibUSBPipe; break;
    case LIBUSB_ERROR_INTERRUPTED:
      error = ErrorLibUSBInterrupted; break;
    case LIBUSB_ERROR_NO_MEM:
      error = ErrorLibUSBNoMem; break;
    case LIBUSB_ERROR_NOT_SUPPORTED:
      error = ErrorLibUSBNotSupported; break;
    case LIBUSB_ERROR_OTHER:
      error = ErrorLibUSBOther; break;
    default:
      error = ErrorLibUSBUnknown;
    }
  }
  return error;
}


void DAQFlexCore::setLibUSBError( int libusberror )
{
  ErrorState = getLibUSBError( libusberror );
}


void DAQFlexCore::clearError( void )
{
  ErrorState = Success;
  Device::clearError();
}


string DAQFlexCore::errorStr( void ) const
{
  string es = "";
  if ( ErrorState != Success )
    es = daqflexErrorStr();
  if ( ! Device::errorStr().empty() ) {
    if ( ! es.empty() )
      es += ", ";
    es += Device::errorStr();
  }
  return es;
}


int DAQFlexCore::error( void ) const
{
  return ErrorState;
}


bool DAQFlexCore::success( void ) const
{
  return ( ErrorState == Success && Device::success() );
}


bool DAQFlexCore::failed( void ) const
{
  return ( ErrorState != Success || Device::failed() );
}


string DAQFlexCore::daqflexErrorStr( void ) const
{
  return DAQFlexErrorText[ ErrorState ];
}


string DAQFlexCore::daqflexErrorStr( DAQFlexError error ) const
{
  return DAQFlexErrorText[ error ];
}


}; /* namespace daqflex */
