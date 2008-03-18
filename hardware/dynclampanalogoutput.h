#ifndef _DYNCLAMPANALOGOUTPUT_H_
#define _DYNCLAMPANALOGOUTPUT_H_

#include <vector>
#include <comedilib.h>
#include <relacs/daqerror.h>
#include <relacs/analogoutput.h>
#include <relacs/analoginput.h>
#include "comedianalogoutput.h"
#include "moduledef.h"

using namespace std;


/*! 
\class DynClampAnalogOutput
\author Marco Hackenberg
\version 0.1
\brief Interface for accessing analog output of a daq-board via a dynamic clamp kernel module.
*/


class DynClampAnalogOutput : public AnalogOutput
{

public:

    /*! Device type id for dynamic clamp analog output device. */
  static const int DynClampAnalogOutputType = 6;

    /*! Create a new DynClampAnalogOutput without opening a device. */
  DynClampAnalogOutput( void );
    /*! Constructs an DynClampAnalogOutput with device class name \a deviceclass
        and type id \a aotype. */
  DynClampAnalogOutput( const string &deviceclass );
    /*! Stop analog output and close the daq driver. */
  ~DynClampAnalogOutput( void );

    /*! Open the analog output device specified by \a device.
        Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).
        \sa isOpen(), close(), reset() */
  int open( const string &devicename, long mode=0 );
    /*! Returns true if dynamic clamp module was succesfully opened.
        \sa open(), close(), reset() */
  bool isOpen( void ) const;
    /*! Stop all activity and close the device.
        \sa open(), isOpen(), reset() */
  void close( void );


    /*! Set the name of the dynamic clamp module file. This has to be done 
        before performing prepareWrite() or  startWrite().
        Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).        
	\sa moduleName() \sa setDeviceName() \sa deviceName()  */
  int setModuleName( string modulename );

    /*! Return the name of the dynamic clamp module file.
      \sa setModuleName() \sa setDeviceName() \sa deviceName()  */
  string moduleName( void ) const;

    /*! Returns the pointer to the device file.
      \sa setDeviceName() \sa open() \sa subdevice() */
//  comedi_t* device( void ) const;

    /*! Comedi internal index of analog output subdevice. */
  int subdevice( void ) const;

  int maxBufSize ( void ) const;

    /*! Number of analog output channels. */
  int channels( void ) const;

    /*! Resolution in bits of analog output. */
  int bits( void ) const;

    /*! Maximum sampling rate in Hz of analog output. */
  double maxRate( void ) const;

    /*! Maximum number of analog output ranges. */
  int maxRanges( void ) const;
    /*! Voltage range \a index in Volt for unipolar mode.
        If -1 is returned this range is not supported. */
  double unipolarRange( int index ) const;
    /*! Voltage range \a index in Volt for bipolar mode.
        If -1 is returned this range is not supported. */
  double bipolarRange( int index ) const;

    /*! Device driver specific tests on the settings in \a sigs
        for each output signal.
	Before this function is called, the validity of the settings in 
	\a sigs was already tested by testReadData().
	This function should test whether the settings are really supported
	by the hardware.
	If an error ocurred in any trace, the corresponding errorflags in the
	OutData are set and a negative value is returned.
        The channels in \a sigs are not sorted.
        This function is called by testWrite(). */
  int testWriteDevice( OutList &sigs );

    /*! Prepare analog output of the output signals \a sigs on the device.
	If an error ocurred in any signal, the corresponding errorflags in
	OutData are set and a negative value is returned.
	This function assumes that \a sigs successfully passed testWrite().
        The channels in \a sigs are not sorted. */
  int prepareWrite( OutList &sigs );

    /*! Convert data of the output signals \a sigs.
	If an error ocurred in any channel, the corresponding errorflags in the
	OutData structure are filled and a negative value is returned.
	The output signals are sorted by channel number first
        and are then multiplexed into a buffer of signed short's (2 byte).
        The buffer is attached to the first signal in \a sigs. */
  int convertData( OutList &sigs );

    /*! Start analog output of the output signals \a sigs
        after they were prepared by prepareWrite().
	If an error ocurred in any signal, the corresponding errorflags in
	OutData are set and a negative value is returned.
        The channels in \a sigs are not sorted. */
  int startWrite( OutList &sigs );

    /*! Write data to a running data acquisition.
        Returns the number of data values that were popped from the \a trace- 
	device-buffer (sum over all \a traces).
	If an error ocurred in any channel, the corresponding errorflags in the
	OutList structure are filled and a negative value is returned.  */
  int writeData( OutList &sigs );
  
    /*! Write data to a running data acquisition.
        Returns the number of data values that were popped from the \a trace- 
	device-buffer (sum over all \a traces).
	If an error ocurred in any channel, the corresponding errorflags in the
	OutList structure are filled and a negative value is returned.  
	For internal usage!
    */
  int fillWriteBuffer( void );

    /*! A template function that is used for the implementation
        of the convertData() function.
	This function first sorts the output signals by channel number
	and then multiplexes the signals in a buffer of type \a T
	after appropriate scaling.
        Data values exceeding the range of the daq board are truncated.
        The buffer is attached to the first signal in \a sigs. */
  template < typename T >
    int convert( OutList &sigs );

    /*! Stop any running ananlog output activity on the device.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, WriteError.
        \sa close(), open(), isOpen() */
  int stop ( void );

    /*! Stop any running ananlog output activity and reset the device.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, WriteError.
        \sa close(), open(), isOpen() */
  int reset( void );

    /* True, if configuration command for acquisition is successfully loaded
       into the registers of the hardware.
       For internal usage.
       \sa running(), reload() */
  bool loaded( void ) const;

    /*! True if analog output was prepared using testWriteDevice() and prepareWrite() */
  bool prepared( void ) const;
  
    /*! True if analog output is running. */
  bool running( void ) const;

    /* Sets the running status and unsets the prepared status. */
  void setRunning( void );

    /*! Get error status of the device. 
        0: no error
	-1: underrun
        other: unknown */
  int error( void ) const;


    /*! Index of signal start.
        The defualt implemetation returns -1, indicating that
        no index is available.
        If the analog output driver can return
        an index into the data stream of a running analog input
        where the last analog output started,
        then this function should return the this index.
        You also need to reimplement getAISyncDevice()
        to let the user know about this property. */
  virtual long index( void );
  
    /*! This function is called once after opening the device
        and before any IO operation.
        In case the analog output driver can return
        an index (via the index() function)
        into the data stream of a running analog input
        where the last analog output started,
        then this function should return the index
        of the corresponding analog input device in \a ais. */
  virtual int getAISyncDevice( const vector< AnalogInput* > &ais ) const;




private:

  ComediAnalogOutput *CAO;

  // needed by DynClamp class e.g. for assigning TraceInfo strings to channels
  int SubdeviceID;
  bool IsLoaded;
  bool IsKernelDaqOpened;

  string Modulename;
  int Modulefile;

  unsigned int Subdevice;
  int Channels;
  int Bits;
  double MaxRate;
  int ComediBufferSize;
  unsigned int BufferElemSize;  

  unsigned int ChanList[MAXCHANLIST];

  OutList *Sigs;

  int ErrorState;
  mutable bool IsRunning;
  bool IsPrepared;


};


#endif
