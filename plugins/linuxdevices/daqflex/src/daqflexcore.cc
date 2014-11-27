/*
  daqflex/daqflexcore.cc
  The DAQFlex interface over libusb

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
    "USB init",
    "pipe",
    "transfer failed",
    "invalid buffer size",
    "cant open FPGA file",
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
}


DAQFlexCore::DAQFlexCore( const string &device, const Options &opts  )
  : Device( "DAQFlexCore" ),
    DeviceHandle( NULL ),
    ErrorState( Success )
{
  open( device, opts );
}


DAQFlexCore::~DAQFlexCore( void )
{
  close();
}


int DAQFlexCore::open( const string &devicestr, const Options &opts )
{
  ErrorState = Success;

  Info.clear();
  Settings.clear();

  int productid = 0;
  /* set it from options:
     productid = opts.number( "productid" );
  */
  // check if the product ID is a valid MCC product ID:
  if ( productid != USB_2001_TC &&
       productid != USB_7202 &&
       productid != USB_7204 &&
       productid != USB_1608_GX &&
       productid != USB_1608_GX_2AO &&
       productid != USB_205 &&
       productid != 0 ) {
    ErrorState = ErrorInvalidID;
    return ErrorState;
  }

  string serialno = opts.text( "serialno", "" );

  int mccdevicenum = opts.integer( "devicenum", 0, 1 );

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
	// if ern ErrorLibUSBAccess then we do not have permissions for the device!
	// write an appropriate error message!
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
	else
	  cerr << "DAQFlex: open device failed: " << errorStr() << '\n';
      }
    }
  }

  libusb_free_device_list( list, true );

  if ( !found ) {
    if ( ErrorState == Success )
      ErrorState = ErrorNoDevice;
    cerr << "Failed to open DAQFlex device: " << errorStr() << '\n';
    DeviceHandle = NULL;
  }
  else {
    Str path = opts.text( "firmwarepath" );
    path.provideSlash();
    initDevice( path );
    if ( ErrorState != Success ) {
      cerr << "Error in initializing DAQFlex device: " << errorStr() << '\n';
      if ( ErrorState == ErrorLibUSBIO )
	cerr << "Check the USB cable!\n";
      close();
    }
  }

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


unsigned short DAQFlexCore::maxAIData( void ) const
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


bool DAQFlexCore::aiRanges( void ) const
{
  return AIRanges;
}


unsigned short DAQFlexCore::maxAOData( void ) const
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
  case USB_1608_GX:
    return "USB-1608GX";
  case USB_1608_GX_2AO:
    return "USB-1608GX-2AO";
  case USB_205:
    return "USB-205";
  default:
    return "Invalid Product ID";
  }
}


void DAQFlexCore::setLibUSBError( int libusberror )
{
  if ( libusberror >= 0 )
    ErrorState = Success;
  else {
    switch( libusberror ) {
    case LIBUSB_SUCCESS:
      ErrorState = Success; break;
    case LIBUSB_ERROR_IO:
      ErrorState = ErrorLibUSBIO; break;
    case LIBUSB_ERROR_INVALID_PARAM:
      ErrorState = ErrorLibUSBInvalidParam; break;
    case LIBUSB_ERROR_ACCESS:
      ErrorState = ErrorLibUSBAccess; break;
    case LIBUSB_ERROR_NO_DEVICE:
      ErrorState = ErrorLibUSBNoDevice; break;
    case LIBUSB_ERROR_NOT_FOUND:
      ErrorState = ErrorLibUSBNotFound; break;
    case LIBUSB_ERROR_BUSY:
      ErrorState = ErrorLibUSBBusy; break;
    case LIBUSB_ERROR_TIMEOUT:
      ErrorState = ErrorLibUSBTimeout; break;
    case LIBUSB_ERROR_OVERFLOW:
      ErrorState = ErrorLibUSBOverflow; break;
    case LIBUSB_ERROR_PIPE:
      ErrorState = ErrorLibUSBPipe; break;
    case LIBUSB_ERROR_INTERRUPTED:
      ErrorState = ErrorLibUSBInterrupted; break;
    case LIBUSB_ERROR_NO_MEM:
      ErrorState = ErrorLibUSBNoMem; break;
    case LIBUSB_ERROR_NOT_SUPPORTED:
      ErrorState = ErrorLibUSBNotSupported; break;
    case LIBUSB_ERROR_OTHER:
      ErrorState = ErrorLibUSBOther; break;
    default:
      ErrorState = ErrorLibUSBUnknown;
    }
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
  /*
  else
    cerr << "error in DAQFlexCore::sendMessage( " << message << " ): " << DAQFlexErrorText[r] << '\n';
  */
  unlock();
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
  AIRanges = true;

  switch ( ProductID ) {
  case USB_1608_GX: // Fall through, same init for USB-1608GX and USB-1608GX-2AO
  case USB_1608_GX_2AO: {
    MaxAIData = 0xFFFF;
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
    // check if the firmware has been loaded already:
    string response = sendMessage( "?DEV:FPGACFG" );
    if ( ErrorState != Success )
      return ErrorState;
    if ( response.find( "CONFIGURED" ) == string::npos ) {
      cout << "Firmware being flashed...\n";
      // firmware hasn't been loaded yet, do so:
      transferFPGAfile( path + "USB_1608G.rbf" );
      if ( ErrorState == ErrorCantOpenFPGAFile )
	transferFPGAfile( DefaultFirmwarePath + "USB_1608G.rbf" );
      if ( ErrorState == Success ) {
	// check if the firmware got loaded successfully:
	response = sendMessage( "?DEV:FPGACFG" );
	if ( ErrorState == Success && response.find( "CONFIGURED" ) == string::npos )
	  ErrorState = ErrorFPGAUploadFailed;
      }
      if ( ErrorState == Success ) {
	response = sendMessage( "?DEV:FWV" );
	if ( response.empty() )
	  cout << "DAQFlex: firmware successfully flashed\n";
	else
	  cout << "DAQFlex: firmware version " << response.erase( 0, 8 ) << " successfully flashed\n";
      }
      else
	return ErrorState;
    }
    /*
    else
      cout << "DAQFlex: firmware already flashed, skipping this time\n";
    */
  }
    break;

  case USB_205:
    MaxAIData = 0x0FFF;
    MaxAIRate = 50000.0;
    MaxAIChannels = 8;
    AIFIFOSize = 12288;
    AIRanges = false;
    MaxAOData = 0x0FFF;
    MaxAORate = 500.0;
    MaxAOChannels = 2;
    AOFIFOSize = 2048;
    AOFIFOSize = 1024;
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
    AIFIFOSize = 32; // ???
    MaxAOData = 4096;
    MaxAORate = 10000.0;
    MaxAOChannels = 2;
    AOFIFOSize = 32; // ???
    DIOLines = 16;
    break;

  default:
    MaxAIData = 0xFFF;
    MaxAIRate = 50000.0;
    MaxAIChannels = 8;
    AIFIFOSize = 32;
    MaxAOData = 0;
    MaxAORate = 0.0;
    MaxAOChannels = 0;
    AOFIFOSize = 0;
    DIOLines = 8;
    break;
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


int DAQFlexCore::readBulkTransfer( unsigned char *data, int length, int *transferred,
				   unsigned int timeout )
{
  int err = libusb_bulk_transfer( deviceHandle(), endpointIn(),
				  data, length, transferred, timeout );
  return err;
}


int DAQFlexCore::writeBulkTransfer( unsigned char *data, int length, int *transferred,
				    unsigned int timeout )
{
  int err = libusb_bulk_transfer( deviceHandle(), endpointOut(),
				  data, length, transferred, timeout );
  return err;
}


void DAQFlexCore::clearRead( void )
{
  libusb_clear_halt( deviceHandle(), endpointIn() );
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


int DAQFlexCore::error( void ) const
{
  return ErrorState;
}


bool DAQFlexCore::success( void ) const
{
  return ( ErrorState == Success );
}


string DAQFlexCore::errorStr( void ) const
{
  return DAQFlexErrorText[ ErrorState ];
}


}; /* namespace daqflex */
