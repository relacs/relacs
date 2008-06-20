 #ifndef _COMEDI_DYNCLAMPANALOGINPUT_H_
#define _COMEDI_DYNCLAMPANALOGINPUT_H_

#include <vector>
#include <comedilib.h>
#include <relacs/daqerror.h>
#include <relacs/analoginput.h>
#include <relacs/comedi/comedianaloginput.h>
#include <relacs/comedi/dynclampanalogoutput.h>
#include <relacs/comedi/moduledef.h>
using namespace std;
using namespace relacs;

namespace comedi {


/*! 
\class DynClampAnalogInput
\author Marco Hackenberg
\version 0.1
\brief Interface for accessing analog input of a daq-board via a dynamic clamp kernel module.
\todo: testReadDevice(): we don't get integer data!!! sigs[k].setGain( 1.0 );
\bug fix errno usage
*/


class DynClampAnalogInput : public AnalogInput
{

public:

    /*! Create a new DynClampAnalogInput without opening a device. */
  DynClampAnalogInput( void );
    /*! Open the analog input driver specified by its device file \a device. */
  DynClampAnalogInput( const string &device, long mode=0 );
    /*! Stop analog input and close the daq driver. */
  virtual ~DynClampAnalogInput( void );

    /*! Open the analog input device on device file \a device. */
  virtual int open( const string &device, long mode=0 );
    /*! Returns true if dynamic clamp module was succesfully opened. */
  virtual bool isOpen( void ) const;
    /*! Stop all activity and close the device. */
  virtual void close( void );

    /*! Set the name of the dynamic clamp module file. This has to be done 
        before performing prepareRead() or  startRead().
        Returns zero on success, or InvalidDevice (or any other negative number
	indicating the error).        
	\sa moduleName() \sa setDeviceName() \sa deviceName()  */
  int setModuleName( string modulename );
    /*! Return the name of the dynamic clamp module file.
      \sa setModuleName() \sa setDeviceName() \sa deviceName()  */
  string moduleName( void ) const;

    /*! Number of analog input channels. */
  virtual int channels( void ) const;
    /*! Resolution in bits of analog input. */
  virtual int bits( void ) const;
    /*! Maximum sampling rate in Hz of analog input. */
  virtual double maxRate( void ) const;

    /*! Maximum number of analog input ranges. */
  virtual int maxRanges( void ) const;
    /*! Voltage range \a index in Volt for unipolar mode.
        If -1 is returned this range is not supported. */
  virtual double unipolarRange( int index ) const;
    /*! Voltage range \a index in Volt for bipolar mode.
        If -1 is returned this range is not supported. */
  virtual double bipolarRange( int index ) const;

    /*! Prepare analog input of the input signals \a sigs on the device.
	If an error ocurred in any signal, the corresponding errorflags in
	InData are set and a negative value is returned.
	This function assumes that \a sigs successfully passed testRead().
        The channels in \a sigs are not sorted. */
  virtual int prepareRead( InList &sigs );
    /*! Start analog input of the input signals \a sigs
        after they were prepared by prepareRead().
	If an error ocurred in any signal, the corresponding errorflags in
	InData are set and a negative value is returned.
        The channels in \a sigs are not sorted.
	Also start possible pending acquisition on other devices
	that are known from take(). */
  virtual int startRead( InList &sigs );
    /*! Read data to a running data acquisition.
        Returns the number of data values that were popped from the \a trace- 
	device-buffer (sum over all \a traces).
	If an error ocurred in any channel, the corresponding errorflags in the
	InList structure are filled and a negative value is returned.  */
  virtual int readData( InList &sigs );

    /*! Stop any running ananlog input activity on the device.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, ReadError.
        \sa close(), open(), isOpen() */
  virtual int stop ( void );
    /*! Stop any running ananlog input activity and reset the device.
        Returns zero on success, otherwise one of the flags 
        NotOpen, InvalidDevice, ReadError.
        \sa close(), open(), isOpen() */
  virtual int reset( void );
  
    /*! True if analog input is running. */
  virtual bool running( void ) const;

    /*! Get error status of the device. 
        0: no error
	-1: underrun
        other: unknown */
  virtual int error( void ) const;

    /*! Check for every analog input and input device in \a ais and \a aos
        whether it can be simultaneously started by startRead()
	from this device (\a syncmode = 0)
	or whether the device driver can read the index of an running
	analog input at the time of starting an analog input (\a syncmode = 1).
	Add the indices of those devices to \a aiinx and \a aoinx. */
  virtual void take( vector< AnalogInput* > &ais,
		     vector< AnalogOutput* > &aos,
		     vector< int > &aiinx, vector< int > &aoinx );



protected:

    /*! Device driver specific tests on the settings in \a sigs
        for each input signal.
	Before this function is called, the validity of the settings in 
	\a sigs was already tested by testReadData().
	This function should test whether the settings are really supported
	by the hardware.
	If an error ocurred in any trace, the corresponding errorflags in the
	InData are set and a negative value is returned.
        The channels in \a sigs are not sorted.
        This function is called by testRead(). */
  virtual int testReadDevice( InList &sigs );

    /*! Comedi internal index of analog input subdevice. */
  int subdevice( void ) const;

    /* True, if configuration command for acquisition is successfully loaded
       into the registers of the hardware.
       For internal usage.
       \sa running(), reload() */
  bool loaded( void ) const;
    /*! True if analog input was prepared using testReadDevice() and prepareRead() */
  bool prepared( void ) const;

    /* Sets the running status and unsets the prepared status. */
  void setRunning( void );


private:

    /*! Unique analog I/O device type id for all 
        DynClamp DAQ devices. */
  static const int DynClampAnalogIOType = 2;

  ComediAnalogInput *CAI;
  unsigned int CAIFlags;

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

  InList *Sigs;

  int ErrorState;
  mutable bool IsRunning;
  bool IsPrepared;

  vector< DynClampAnalogInput* > DynClampAIs;
  vector< DynClampAnalogOutput* > DynClampAOs;
  vector< int > ComediAIsLink;
  vector< int > ComediAOsLink;

};


}; /* namespace comedi */

#endif /* ! _COMEDI_DYNCLAMPANALOGINPUT_H_ */
