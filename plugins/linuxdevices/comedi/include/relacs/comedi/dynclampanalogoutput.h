#ifndef _COMEDI_DYNCLAMPANALOGOUTPUT_H_
#define _COMEDI_DYNCLAMPANALOGOUTPUT_H_

#include <vector>
#include <comedilib.h>
#include <relacs/daqerror.h>
#include <relacs/analogoutput.h>
#include <relacs/analoginput.h>
#include <relacs/comedi/comedianalogoutput.h>
#include <relacs/comedi/moduledef.h>
using namespace std;
using namespace relacs;

namespace comedi {


/*! 
\class DynClampAnalogOutput
\author Marco Hackenberg
\version 0.1
\brief Interface for accessing analog output of a daq-board via a dynamic clamp kernel module.
\bug fix errno usage
*/


class DynClampAnalogOutput : public AnalogOutput
{

public:

    /*! Create a new DynClampAnalogOutput without opening a device. */
  DynClampAnalogOutput( void );
    /*! Open the analog output driver specified by its device file \a device. */
  DynClampAnalogOutput( const string &device, long mode=0 );
    /*! Stop analog output and close the daq driver. */
  virtual ~DynClampAnalogOutput( void );

    /*! Open the analog output device on device file \a device. */
  virtual int open( const string &device, long mode=0 );
    /*! Returns true if dynamic clamp module was succesfully opened. */
  virtual bool isOpen( void ) const;
    /*! Stop all activity and close the device. */
  virtual void close( void );

    /*! Set the name of the dynamic clamp module file. This has to be done 
        before performing prepareWrite() or  startWrite().
        Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).        
	\sa moduleName() \sa setDeviceName() \sa deviceName()  */
  int setModuleName( string modulename );
    /*! Return the name of the dynamic clamp module file.
      \sa setModuleName() \sa setDeviceName() \sa deviceName()  */
  string moduleName( void ) const;

    /*! Number of analog output channels. */
  virtual int channels( void ) const;
    /*! Resolution in bits of analog output. */
  virtual int bits( void ) const;
    /*! Maximum sampling rate in Hz of analog output. */
  virtual double maxRate( void ) const;

    /*! Maximum number of analog output ranges. */
  virtual int maxRanges( void ) const;
    /*! Voltage range \a index in Volt for unipolar mode.
        If -1 is returned this range is not supported. */
  virtual double unipolarRange( int index ) const;
    /*! Voltage range \a index in Volt for bipolar mode.
        If -1 is returned this range is not supported. */
  virtual double bipolarRange( int index ) const;

    /*! Convert data of the output signals \a sigs.
	If an error ocurred in any channel, the corresponding errorflags in the
	OutData structure are filled and a negative value is returned.
	The output signals are sorted by channel number first
        and are then multiplexed into a buffer of signed short's (2 byte).
        The buffer is attached to the first signal in \a sigs. */
  virtual int convertData( OutList &sigs );
    /*! Prepare analog output of the output signals \a sigs on the device.
	If an error ocurred in any signal, the corresponding errorflags in
	OutData are set and a negative value is returned.
	This function assumes that \a sigs successfully passed testWrite().
        The channels in \a sigs are not sorted. */
  virtual int prepareWrite( OutList &sigs );
    /*! Start analog output of the output signals \a sigs
        after they were prepared by prepareWrite().
	If an error ocurred in any signal, the corresponding errorflags in
	OutData are set and a negative value is returned.
        The channels in \a sigs are not sorted. */
  virtual int startWrite( OutList &sigs );
    /*! Write data to a running data acquisition.
        Returns the number of data values that were popped from the \a trace- 
	device-buffer (sum over all \a traces).
	If an error ocurred in any channel, the corresponding errorflags in the
	OutList structure are filled and a negative value is returned.  */
  virtual int writeData( OutList &sigs );

    /*! Stop any running ananlog output activity and reset the device.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, WriteError.
        \sa close(), open(), isOpen() */
  virtual int reset( void );
  
    /*! True if analog output is running. */
  virtual bool running( void ) const;

    /*! Get error status of the device. 
        0: no error
	-1: underrun
        other: unknown */
  virtual int error( void ) const;

    /*! Index of signal start.
        The defualt implemetation returns -1, indicating that
        no index is available.
        If the analog output driver can return
        an index into the data stream of a running analog input
        where the last analog output started,
        then this function should return the this index.
        You also need to reimplement getAISyncDevice()
        to let the user know about this property. */
  virtual long index( void ) const;
  
    /*! This function is called once after opening the device
        and before any IO operation.
        In case the analog output driver can return
        an index (via the index() function)
        into the data stream of a running analog input
        where the last analog output started,
        then this function should return the index
        of the corresponding analog input device in \a ais. */
  virtual int getAISyncDevice( const vector< AnalogInput* > &ais ) const;

    /*! If the analog output device supports outputs that
        are not physical output lines but rather writeable parameter,
        like model parameter for a dynamic clamp modul,
        then reimplement this function. 
        Add for each such parameter a TraceSpec to \a traces.
        \a deviceid is the id of the analog output device
        that you should use for initializing the TraceSpec. */
  virtual void addTraces( vector< TraceSpec > &traces, int deviceid ) const;


protected:

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
  virtual int testWriteDevice( OutList &sigs );

    /*! Comedi internal index of analog output subdevice. */
  int subdevice( void ) const;

    /* True, if configuration command for acquisition is successfully loaded
       into the registers of the hardware.
       For internal usage.
       \sa running(), reload() */
  bool loaded( void ) const;

    /*! True if analog output was prepared using testWriteDevice() and prepareWrite() */
  bool prepared( void ) const;

    /* Sets the running status and unsets the prepared status. */
  void setRunning( void );


private:

    /*! Unique analog I/O device type id for all 
        DynClamp DAQ devices. */
  static const int DynClampAnalogIOType = 2;

  ComediAnalogOutput *CAO;

  // needed by DynClamp class e.g. for assigning TraceInfo strings to channels
  int SubdeviceID;
  bool IsLoaded;
  bool IsKernelDaqOpened;

  string Modulename;
  int Modulefile;
  int FifoFd;

  unsigned int Subdevice;
  int Channels;
  int Bits;
  double MaxRate;
  int ComediBufferSize;
  unsigned int BufferElemSize;  

  unsigned int ChanList[MAXCHANLIST];

  OutList *Sigs;

  mutable int ErrorState;
  mutable bool IsRunning;
  bool IsPrepared;


};


}; /* namespace comedi */

#endif /* ! _COMEDI_DYNCLAMPANALOGOUTPUT_H_ */
