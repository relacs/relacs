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


const string DAQFlexCore::FirmwarePath = "/usr/lib/daqflex/";


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
  // Check if the product ID is a valid MCC product ID:
  if ( productid != USB_2001_TC &&
       productid != USB_7202 &&
       productid != USB_7204 &&
       productid != USB_1608_GX &&
       productid != USB_1608_GX_2AO &&
       productid != 0 ) {
    ErrorState = ErrorInvalidID;
    return ErrorState;
  }

  string serialno = opts.text( "serialno", "" );

  int mccdevicenum = opts.integer( "devicenum", 0, 1 );

  //Initialize USB libraries
  if ( libusb_init( NULL ) != 0 ) {
    ErrorState = ErrorUSBInit;
    return ErrorState;
  }

  // Get the list of USB devices connected to the PC:
  libusb_device ** list;
  ssize_t listsize = libusb_get_device_list( NULL, &list );
  
  // Traverse the list of USB devices to find the requested device:
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
	// Open the device:
	if ( !libusb_open( device, &DeviceHandle ) ) {
	  //Claim interface with the device
	  if ( !libusb_claim_interface( DeviceHandle, 0 ) ) {
	    //Get input and output endpoints
	    getEndpoints();
	    if ( ErrorState != Success ) {
	      libusb_release_interface( DeviceHandle, 0 );
	      continue;
	    }
	    InPacketSize = libusb_get_max_iso_packet_size(  device,
							     EndpointIn );
	    OutPacketSize = libusb_get_max_iso_packet_size(  device,
							      EndpointOut );
	    if ( ! serialno.empty() ) {
	      //get the device serial number
	      string message = sendMessage( "?DEV:MFGSER" );
	      // Erase message while keeping serial number:
	      message.erase( 0, 11 );
	      cout << "Found " << productName( ProductID ) << " with serial number " << message << "\n";
	      
	      // check that the serial numbers are the same:
	      if ( message.compare( serialno ) )
		//serial numbers are not the same, release device and continue on
		libusb_release_interface( DeviceHandle, 0 );
	      else
		// serial numbers are the same, this is the correct device:
		found = true;
	    }
	    else
	      found = true;
	  }
	}
      }
    }
  }
    
  libusb_free_device_list( list, true );
  
  if ( !found ) {
    ErrorState = ErrorNoDevice;
    DeviceHandle = NULL;
  }
  else 
    initDevice();

  return ErrorState;
}


bool DAQFlexCore::isOpen( void ) const
{
  return ( DeviceHandle != NULL );
}


void DAQFlexCore::close( void )
{
  if ( isOpen() ) {
    //Free memory and devices
    libusb_release_interface( DeviceHandle, 0 );
    libusb_close( DeviceHandle );
    libusb_exit( NULL );
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
    case LIBUSB_ERROR_IO:
      ErrorState = ErrorLibUSBIO;
    case LIBUSB_ERROR_INVALID_PARAM:
      ErrorState = ErrorLibUSBInvalidParam;
    case LIBUSB_ERROR_ACCESS: 
      ErrorState = ErrorLibUSBAccess; 
    case LIBUSB_ERROR_NO_DEVICE:
      ErrorState = ErrorLibUSBNoDevice;
    case LIBUSB_ERROR_NOT_FOUND:
      ErrorState = ErrorLibUSBNotFound;
    case LIBUSB_ERROR_BUSY:
      ErrorState = ErrorLibUSBBusy;
    case LIBUSB_ERROR_TIMEOUT: 
      ErrorState = ErrorLibUSBTimeout; 
    case LIBUSB_ERROR_OVERFLOW:
      ErrorState = ErrorLibUSBOverflow;
    case LIBUSB_ERROR_PIPE:
      ErrorState = ErrorLibUSBPipe;
    case LIBUSB_ERROR_INTERRUPTED:
      ErrorState = ErrorLibUSBInterrupted;
    case LIBUSB_ERROR_NO_MEM: 
      ErrorState = ErrorLibUSBNoMem; 
    case LIBUSB_ERROR_NOT_SUPPORTED:
      ErrorState = ErrorLibUSBNotSupported;
    case LIBUSB_ERROR_OTHER:
      ErrorState = ErrorLibUSBOther;
    default:
      ErrorState = ErrorLibUSBUnknown;
    }
  }
}


//Send a message to the device
  int DAQFlexCore::sendControlTransfer( const string &message, bool display )
{
  if ( display )
    cerr << "DAQFlex Sending: " << message << "\n";
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

//Receive a message from the device. This should follow a call to sendControlTransfer.
string DAQFlexCore::getControlTransfer( bool display )
{
  unsigned char message[MaxMessageSize];
  int numbytes = libusb_control_transfer( DeviceHandle,
					  LIBUSB_REQUEST_TYPE_VENDOR + LIBUSB_ENDPOINT_IN,
					  StringMessage, 0, 0,
					  message, MaxMessageSize, 100 );
  
  setLibUSBError( numbytes );
  if ( ErrorState != Success )
    return "";

  if ( display )
    cerr << "DAQFlex Got: " << message << "\n";
  return (char *)message;
}


//Returns response if transfer successful, null if not
  string DAQFlexCore::sendMessage( const string &message, bool display )
{
  display = false;
  int r = sendControlTransfer( message, display );
  if ( r == Success )
    return getControlTransfer( display );
  else
    return "";
}

//Convert an MCC product ID to a product name
//Get the device input and output endpoints
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
 
//Find the input endpoint from an endpoint descriptor
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

//Find the output endpoint from an endpoint descriptor
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


int DAQFlexCore::initDevice( void )
{
  ErrorState = Success;

  switch ( ProductID ) {
  case USB_1608_GX: // Fall through, same init for USB-1608GX and USB-1608GX-2AO
  case USB_1608_GX_2AO: {
    MaxAIData = 0xFFFF;
    MaxAIRate = 500000.0;
    MaxAIChannels = 16;
    if ( ProductID == USB_1608_GX_2AO ) {
      MaxAOData = 0xFFFF;
      MaxAORate = 500000.0;
      MaxAOChannels = 2;
    }
    else {
      MaxAOData = 0;
      MaxAORate = 0.0;
      MaxAOChannels = 0;
    }
    // Check if the firmware has been loaded already:
    string response = sendMessage( "?DEV:FPGACFG" );
    if ( response.find( "CONFIGURED" ) == string::npos ) {
      cout << "Firmware being flashed...\n";
      // firmware hasn't been loaded yet, do so:
      stringstream firmwarefile;
      firmwarefile << FirmwarePath << "USB_1608G.rbf";
      transferFPGAfile( firmwarefile.str() );

      if ( ErrorState == Success ) {
	// Check if the firmware got loaded successfully:
	response = sendMessage( "?DEV:FPGACFG" );
	if ( response.find( "CONFIGURED" ) == string::npos )
	  ErrorState = ErrorFPGAUploadFailed;
      }
      if ( ErrorState == Success )
	cout << "Firmware successfully flashed...\n";
    } 
    else
      cout << "Firmware already flashed, skipping this time\n";
  }
    break;

  case USB_7202:
    MaxAIData = 0xFFFF;
    MaxAIRate = 50000.0;
    MaxAIChannels = 8;
    MaxAOData = 0;
    MaxAORate = 0.0;
    MaxAOChannels = 0;
    break;

  case USB_7204:
    MaxAIData = 0xFFF;
    MaxAIRate = 50000.0;
    MaxAIChannels = 8;
    MaxAOData = 4096;
    MaxAORate = 10000.0;
    MaxAOChannels = 2;
    break;

  default:
    MaxAIData = 0xFFF;
    MaxAIRate = 50000.0;
    MaxAIChannels = 8;
    MaxAOData = 0;
    MaxAORate = 0.0;
    MaxAOChannels = 0;
    break;
  }

  // set basic device infos:
  setDeviceName( productName( ProductID ) );
  setDeviceVendor( "Measurement Computing" );
  setDeviceFile( "USB" );

  return ErrorState;
}


int DAQFlexCore::transferFPGAfile( const string &path )
{
  ErrorState = Success;

  // Turn on FPGA configure mode:
  sendMessage( "DEV:FPGACFG=0XAD" );
  if ( ErrorState != Success )
    return ErrorState;
    
  // Open file for input in binary mode, cursor at end of file:
  ifstream file( path.data(), ios::in|ios::binary|ios::ate );
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


}; /* namespace daqflex */
