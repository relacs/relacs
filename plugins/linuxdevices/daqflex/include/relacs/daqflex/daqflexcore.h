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

#ifndef _RELACS_DAQFLEX_DAQFLEXCORE_H_
#define _RELACS_DAQFLEX_DAQFLEXCORE_H_ 1

#include <string>
#include <libusb-1.0/libusb.h>
#include <relacs/device.h>
using namespace std;
using namespace relacs;

namespace daqflex {


/*!
\class DAQFlexCore
\author Jan Benda
\version 1.2
\brief [Device] The DAQFlex interface over libusb
\par Options
- \c firmwarepath=/usr/lib/daqflex/: Path to the *.rbf firmware files

\par Supported devices: 
In principle all DAQFlex Devices are supported. However, it might be
necessary to adjust some properties for some of the devices.

Currently tested and working are the following devices:
- USB_1608_GX_2AO
- USB_205

Therefore, the following devices should work as well:
- USB_1608_G
- USB_1608_GX
- USB_201
- USB_202
- USB_204
*/

class DAQFlexCore : public Device
{

public:

  enum DAQFlexError {
    Success,
    ErrorNoDevice,
    ErrorInvalidID,
    ErrorUSBInit,
    ErrorPipe,
    ErrorTransferFailed,
    ErrorInvalidBufferSize,
    ErrorCantOpenFPGAFile,
    ErrorFPGAUploadFailed,
    ErrorLibUSBIO,
    ErrorLibUSBInvalidParam,
    ErrorLibUSBAccess,
    ErrorLibUSBNoDevice,
    ErrorLibUSBNotFound,
    ErrorLibUSBBusy,
    ErrorLibUSBTimeout,
    ErrorLibUSBOverflow,
    ErrorLibUSBPipe,
    ErrorLibUSBInterrupted,
    ErrorLibUSBNoMem,
    ErrorLibUSBNotSupported,
    ErrorLibUSBOther,
    ErrorLibUSBUnknown
  };
  static const int DAQFlexErrorMax = 23;
  static const string DAQFlexErrorText[DAQFlexErrorMax];

    /*! vendor ID of MCC USB DAQ boards. */
  static const int MCCVendorID = 0x09db;

  // device product IDs:
  static const int USB_2001_TC = 0x00F9;
  static const int USB_7202 = 0x00F2;
  static const int USB_7204 = 0x00F0;
  static const int USB_1608_G = 0x0110;
  static const int USB_1608_GX = 0x0111;
  static const int USB_1608_GX_2AO = 0x0112;
  static const int USB_201 = 0x0113;
  static const int USB_204 = 0x0114;
  static const int USB_202 = 0x012B;
  static const int USB_205 = 0x012C;
  static const int USB_2408 = 0x00FD;
  static const int USB_2408_2AO = 0x00FE;
  static const int USB_1208_FS_Plus = 0x00E8;
  static const int USB_1408_FS_Plus = 0x00E9;
  static const int USB_1608_FS_Plus = 0x00EA;

  DAQFlexCore( void );
  DAQFlexCore( const string &device, const Options &opts );
  ~DAQFlexCore( void );

  virtual int open( const string &device ) override;
  virtual bool isOpen( void ) const;
  virtual void close( void );
  virtual int reset( void );

    /*! Send a message to the device without locking it. */
  int sendControlTransfer( const string &message );
    /*! Receive a message from the device without locking it. This
        should follow a call to sendControlTransfer. */
  string getControlTransfer( void );
    /*! Send message to device while locking it. 
        \return response if transfer successful, empty string if not. */
  string sendMessage( const string &message );
    /*! Send message to device without locking it. 
        \return response if transfer successful, empty string if not. */
  string sendMessageUnlocked( const string &message );
    /*! Send command to the device while locking it. 
        Use sendControlTransfer() for not locking the device.
        \return error code */
  int sendCommand( const string &command );
    /*! Send commands to the device while locking it.
        \return error code  */
  int sendCommands( const string &command1, const string &command2 );

    /*! \return the resolution of the A/D converter. */
  unsigned int maxAIData( void ) const;
    /*! \return the maximum scan rate of the A/D converter. */
  double maxAIRate( void ) const;
    /*! \return the number of analog input channels. */
  int maxAIChannels( void ) const;
    /*! \return the number of samples the AI FIFO can hold.
        0: no FIFO present but hardware paced AI supported.
        -1: no FIFO and no hardware paced AI. */
  int aiFIFOSize( void ) const;

    /*! Set the currently used sampling rate of analog input to \a samplerate. */
  void setAISampleRate( double samplerate );
    /*! Return the currently used sampling rate of analog input. */
  double aiSampleRate( void ) const;

    /*! \return the resolution of the D/A converter. */
  unsigned int maxAOData( void ) const;
    /*! \return the maximum scan rate of the D/A converter. */
  double maxAORate( void ) const;
    /*! \return the number of analog output channels. */
  int maxAOChannels( void ) const;
    /*! \return the number of samples the AI FIFO can hold.
        0: no FIFO present but hardware paced AO supported.
        -1: no FIFO and no hardware paced AO. */
  int aoFIFOSize( void ) const;

    /*! \return the number of digital I/O lines. */
  int dioLines( void ) const;

    /*! The size of a single incoming packet in bytes. */
  int inPacketSize( void ) const;
    /*! The size of a single outgoing packet in bytes. */
  int outPacketSize( void ) const;

    /*! Transfer data from the device to a buffer.
        \param[in] data a buffer for the received data
        \param[in] length number of bytes \a data can receive
        \param[in] transferred number of bytes actually transferred
        \param[in] timeout in milliseconds
        \return an eror code. */
  DAQFlexError readBulkTransfer( unsigned char *data, int length, int *transferred,
				 unsigned int timeout );
    /*! Transfer data from a buffer to the device.
        \param[in] data to be sent
        \param[in] length number of bytes in \a data to be sent
        \param[in] transferred number of bytes actually transferred
        \param[in] timeout in milliseconds
        \return an eror code. */
  DAQFlexError writeBulkTransfer( unsigned char *data, int length, int *transferred,
				  unsigned int timeout );

    /*! Clear the reading endpoint. */
  void clearRead( void );
    /*! Clear the writing endpoint. */
  void clearWrite( void );

    /*! Clear the error state and the error string. */
  virtual void clearError( void );
    /*! \return the current error state. */
  int error( void ) const;
    /*! Return the translated error state and the error string. */
  virtual string errorStr( void ) const;
    /*! \return \c true if there is no error. */
  virtual bool success( void ) const;
    /*! \return \c true if there is an error. */
  virtual bool failed( void ) const;
    /*! \return the current error state as a descriptive string. */
  string daqflexErrorStr( void ) const;
    /*! \return a descriptive string for \a error. */
  string daqflexErrorStr( DAQFlexError error ) const;

  using Device::lock;
  using Device::unlock;
  using Device::mutex;

  
protected:

  void initOptions( void ) override;

  
private:

    /*! A handle to the USB device. */
  libusb_device_handle *deviceHandle( void );
    /*! The endpoint for reading data. */
  unsigned char endpointIn( void );
    /*! The endpoint for writing data. */
  unsigned char endpointOut( void );

  string productName( int productid );
  void setLibUSBError( int libusberror );

  int getEndpoints( void );
  unsigned char getEndpointInAddress( unsigned char* data, int n );
  unsigned char getEndpointOutAddress( unsigned char* data, int n );

  int initDevice( const string &path );
  int uploadFPGAFirmware( const string &path, const string &filename );
  int transferFPGAfile( const string &path );

    /*! Convert the \a libusberror to an \a DAQFlexError. */
  static DAQFlexError getLibUSBError( int libusberror );

  libusb_device_handle *DeviceHandle;
  unsigned char EndpointIn;
  unsigned char EndpointOut;
  int InPacketSize;
  int OutPacketSize;
  int ProductID;
  unsigned int MaxAIData;
  double MaxAIRate;
  int MaxAIChannels;
  int AIFIFOSize;  // 0: no FIFO but AISCAN, -1: no FIFO and no AISCAN
  double AISampleRate;
  unsigned int MaxAOData;
  double MaxAORate;
  int MaxAOChannels;
  int AOFIFOSize;  // 0: no FIFO but AOSCAN, -1: no FIFO and no AOSCAN
  int DIOLines;
  DAQFlexError ErrorState;
  static const uint16_t MaxMessageSize = 64;
  static const uint8_t StringMessage = 0x80;
  static const string DefaultFirmwarePath;
  static const int FPGADATAREQUEST = 0x51;

};


}; /* namespace daqflex */

#endif /* ! _RELACS_DAQFLEX_DAQFLEXCORE_H_ */
