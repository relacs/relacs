/*
  indata.h
  A cyclic buffer for data acquired from a data acquisition board.

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

#ifndef _RELACS_INDATA_H_
#define _RELACS_INDATA_H_ 1
 
#include <iostream>
#include <string>
#include <vector>
#include <relacs/cyclicsampledata.h>
#include <relacs/daqerror.h>
using namespace std;

namespace relacs {


/*! 
\class InData
\brief A cyclic buffer for data acquired from a data acquisition board.
\author Jan Benda


InData contains all data necessary to specify the acquisition of data from
a data-acquisition board as well as a cyclic buffer for the read in data.
The content of the data buffer can be accesed by the []-operators,
and the at() functions.

To convert indices to the buffer into the corresponding time and vice versa
use the functions pos(), interval(), index() and indices().
pos( int index ) returns the time corresponding to the data element
with index \a index.
index( double pos ) is the index of the data element 
at time \a pos given in seconds.

Since the buffer is cyclic it cannot contain all data, but only the
last accessibleSize() read in data values.
The index of the first accessible data element is returned by minIndex()
and an iterator by minBegin().
The index behind the most recent data element is returned by
currentIndex() and equals the size() of the buffer.
signalIndex() returns the index of the output of the last signal
and restartIndex() the index where the data acquisition was restarted.

The data are stored as floats in a secondary unit.
Multiplication of the voltage obtained from the data acquisition board
with scale() results in the data value stored in the InData buffer
with an unit returned by unit().

The acquisition of the data is specified by sampleRate(), delay(),
startSource(), priority(), continuous(), device(), channel(),
reference(), unipolar(), and gainIndex().
Further, the InData has an identifier string ident(),
a unique trace number trace() and mode() flags.

InputData inherits an error flag from the class DaqError.  The error
flag can be read with error() where errors of the last input operation
are coded.
With success() it can be checked whether the last input/output
operation was successful.
For more details, see DaqError.
*/

class OutData;
class InDataIterator;
class InDataDiffIterator;
class InDataTimeIterator;

class InData : public CyclicSampleDataF, public DaqError
{
  
 public:

    /*! Channel numbers larger or equal than this are additional
        traces not acquired from the daq board. */
  static const int ParamChannel = 1000;

    /*! Reference types for analog input lines. */
  enum RefType { 
      /*! Differential input. */
    RefDifferential=0,
      /*! A "common" reference (the low inputs of all the channels are
  	  tied together, but are isolated from ground).
	  Also called "nonreferenced single-ended". */
    RefCommon=1,
      /*! Referenced to ground.
  	  Also called "referenced single-ended". */
    RefGround=2,
      /*! Any reference that does not fit into the above categories. */
    RefOther=3
  };
  
    /*! Constructor. */
  InData( void );
    /*! Constructs an InData with capacity \a n,
        and sampling interval \a step. */
  InData( int n, double step );
    /*! Constructs an InData with capacity \a n,
        reserved elements for writing \a m,
        and sampling interval \a step. */
  InData( int n, int m, double step );
    /*! Constructs an InData with the same settings
        as \a d and using the same buffer as \a d. */
  InData( const InData *d );
    /*! Copy constructor. */
  InData( const InData &data );
    /*! Destructor. */
  virtual ~InData( void );

    /*! Assignment operator. */
  const InData &operator=( const InData &data );
    /*! Make \a data internal reference to an InData and copy its properties
        and a pointer to its buffer to this. */
  const InData &assign( const InData *data );
    /*! Assign all properties from the internal reference to an InData instance
        to this. \sa update() */
  const InData &assign( void );

    /*! Copy the data from element \a first to element \a last to \a data.
        No amplitude information is stored into the description of \a data.
        Set the name of the description to \a name. */
  void copy( int first, int last, OutData &data, const string &name="" ) const;
    /*! Copy the data from time \a tbegin to time \a tend seconds
        to \a data.
        No amplitude information is stored into the description of \a data.
	Set the name of the description to \a name. */
  void copy( double tbegin, double tend, OutData &data, const string &name="" ) const;
    /*! Copy the data values from \a time + \a trace.leftMargin()
        to \a time + \a trace.rightMargin() to \a trace.
        The sample interval given by \a trace is used.
        If the sample intervals differ
	the values are obtained by linear interpolation.
        If \a time + \a trace.rightMargin() is larger than length()
        \a trace is appropriately truncated. */
  void copy( double time, SampleDataF &trace ) const;
    /*! Copy the data values from \a time + \a trace.leftMargin()
        to \a time + \a trace.rightMargin() to \a trace.
        The sample interval given by \a trace is used.
        If the sample intervals differ
	the values are obtained by linear interpolation.
        If \a time + \a trace.rightMargin() is larger than length()
        \a trace is appropriately truncated. */
  void copy( double time, SampleDataD &trace ) const;
    /*! Copy the data from element \a first to element \a last to \a data. */
  void copy( int first, int last, ArrayF &data ) const;

    /*! Return string with an error message: 
        "Channel # on device #: error message".
        If there isn't any error, an empty string is returned. */
  string errorMessage( void ) const;

    /*! \return the size of the part of the buffer reserved for writing new data. */
  int writeBufferCapacity( void ) const;
    /*! Set the capacity of the part of the bufer to be used for
        writing new data to \a m. If \a m is greater than capacity()
	it is set to capacity(). */
  void setWriteBufferCapacity( int m );

    /*! Empties the buffer and resets all indices. */
  void clear( void );
    /*! The number of data elements that are actually stored in the array
        and therefore are accessible. */
  virtual int accessibleSize( void ) const;

    /*! Index + 1 where data end. Equals size(). \sa currentTime() */
  int currentIndex( void ) const;
    /*! Time in seconds where data end. Equals length(). \sa currentIndex() */
  double currentTime( void ) const;
    /*! The index of the first accessible data element. \sa minTime() */
  virtual int minIndex( void ) const;
    /*! The time in seconds corresponding to 
        the first accessible data element.
        \sa minIndex() */
  double minTime( void ) const;
    /*! The time in seconds corresponding to 
        the first accessible data element.
	Same as minTime(). \sa minIndex() */
  virtual double minPos( void ) const;
    /*! Maximum possible index. \sa maxTime() */
  int maxIndex( void ) const;
    /*! Time in seconds corresponding to the maximum possible index.
        \sa maxIndex() */
  double maxTime( void ) const;
    /*! Index of start of last signal.
        If there wasn't any signal yet, -1 is returned.
        \sa signalTime(), setSignalIndex(), setSignalTime() */
  int signalIndex( void ) const;
    /*! Time in seconds of start of last signal. 
        If there wasn't any signal yet, -1.0 is returned.
        \sa signalIndex(), setSignalIndex(), setSignalTime() */
  double signalTime( void ) const;
    /*! Set index of start of last signal to \a index.
        \sa setSignalTime() */
  void setSignalIndex( int index );
    /*! Set time of start of last signal to \a time.
        \sa setSignalIndex() */
  void setSignalTime( double time );
    /*! Index where aquisition was restarted. 
        \sa restartTime(), setRestart() */
  int restartIndex( void ) const;
    /*! Time in seconds where aquisition was restarted. 
        \sa restartIndex(), setRestart() */
  double restartTime( void ) const;
    /*! Set Restart-index to current size(). 
        \sa setRestartTime(), restartIndex(), restartTime() */
  void setRestart( void );
    /*! Set Restart-index to the element at \a restarttime. 
        \sa setRestart(), restartTime() */
  void setRestartTime( double restarttime );

    /*! Copy all indices from the internal reference to an InData instance
        to this. \sa assign() */
  void update( void );

    /*! The number of data elements available to be read from the buffer. 
        \sa read(), readIndex(), size(), accessibleSize() */
  virtual int readSize( void ) const;

    /*! Minimum possible value (in the secondary unit).
        \sa maxValue(), minVoltage(), setMinValue() */
  double minValue( void ) const;
    /*! Maximum possible value (in the secondary unit).
        \sa minValue(), maxVoltage(), setMaxValue() */
  double maxValue( void ) const;
    /*! Set the minimum possible value to \a minv. */
  void setMinValue( double minv );
    /*! Set the maximum possible value to \a maxv. */
  void setMaxValue( double maxv );

    /*! Get the voltage of the \a index -th element in Volt.
        \a index must be a valid index. */
  double voltage( int index ) const;
    /*! Returns the voltage corresponding to the value \a val in Volt. */
  double getVoltage( double val ) const;
    /*! Minimum possible voltage value.
        \sa maxVoltage(), minValue(), setMinVoltage() */
  double minVoltage( void ) const;
    /*! Maximum possible voltage value.
        \sa minVoltage(), maxValue(), setMaxVoltage() */
  double maxVoltage( void ) const;
    /*! Set the minimum possible voltage value to \a minv.
        \sa setMaxVoltage(), setMinValue(), minValue() */
  void setMinVoltage( double minv );
    /*! Set the maximum possible votlage value to \a maxv.
        \sa setMinVoltage(), setMaxValue(), maxValue() */
  void setMaxVoltage( double maxv );

    /*! Const iterator used to iterate through InData. */
  typedef InDataIterator const_iterator;
    /*! Returns an iterator pointing to the first data element. */
  const_iterator begin( void ) const;
    /*! Returns an iterator pointing to the element at time \a time seconds. */
  const_iterator begin( double time ) const;
    /*! Returns an iterator pointing to the first accessible element \sa minIndex(). */
  const_iterator minBegin( void ) const;
    /*! Returns an iterator pointing behind the last element. */
  const_iterator end( void ) const;
    /*! Const iterator used to iterate through InData. */
  typedef InDataTimeIterator const_range_iterator;
    /*! Returns an iterator for the time associated with the data elements 
        pointing to the first data element. */
  const_range_iterator timeBegin( void ) const;
    /*! Returns an iterator for the time associated with the data elements 
        pointing to the element at time \a time seconds. */
  const_range_iterator timeBegin( double time ) const;
    /*! Returns an iterator for the time associated with the data elements
        pointing to the first accessible element \sa minIndex(). */
  const_range_iterator minTimeBegin( void ) const;
    /*! Returns an iterator for the time associated with the data elements 
        pointing behind the last element. */
  const_range_iterator timeEnd( void ) const;

    /*! The sampling rate of the input trace in Hertz */
  double sampleRate( void ) const;
    /*! Set the sampling rate of the input trace to \a rate Hertz */
  void setSampleRate( double rate );
    /*! The sampling interval of the input trace in seconds.
        Same as stepszie(). */
  double sampleInterval( void ) const;
    /*! Set the sampling interval of the input trace to \a step seconds. 
        Same as setStepsize(). */
  void setSampleInterval( double step );

    /*! Delay in seconds from start trigger to start of aquisition. */
  double delay( void ) const;
    /*! Set delay to \a delay (in seconds). */
  void setDelay( double delay );
    /*! The source for the start trigger of the data aquisition. */
  int startSource( void ) const;
    /*! Set the source for the start trigger of the data aquisition
        to \a startsource. */
  void setStartSource( int startsource );
    /*! Returns true if the input trace has the priority to run
        even if there still is a data acquisition running. */
  bool priority( void ) const;
    /*! Set the priority of the input trace to \a priority.
        If \a priority is \a true then the input trace is processed even
	if there still is a data acquisition running.
	Otherwise the input trace is not processed and returns with an error. */
  void setPriority( bool priority=true );
    /*! Is aquisition performed in continuous mode? */
  bool continuous( void ) const;
    /*! Set continuous mode of data aquisition to \a continuous. */
  void setContinuous( bool continuous=true );

    /*! The id of the input device. 
        \sa channel(), setChannel(), setDevice() */
  int device( void ) const;
    /*! Set the device id to \a device.
        You only need to specify an input device if more than one
        output devices are available and the data should be acquired
        from an input device other than the default one. 
        \sa channel(), setChannel(), device() */
  void setDevice( int device );
    /*! The number of the channel on the specified device
        that is used for input. 
        \sa setChannel(), device(), setDevice() */
  int channel( void ) const;
    /*! Set the number of the channel on the specified device
        that should be used for input to \a channel. 
        Default is the first ('0') channel. 
        \sa channel(), device(), setDevice() */
  void setChannel( int channel );
    /*! Set the number of the channel to \a channel
        and the device to \a device. 
        \sa channel(), device(), setDevice() */
  void setChannel( int channel, int device );
    /*! \c true if this is a channel sampling from a acquisition device.
        \sa paramChannel() */
  bool rawChannel( void ) const;
    /*! \c true if this is a channel sampling from model or status variables.
        \sa rawChannel() */
  bool paramChannel( void ) const;
    /*! The trace-number. */
  int trace( void ) const;
    /*! Set the trace number to \a trace. */
  void setTrace( int trace );

    /*! The description of the input trace. */
  string ident( void ) const;
    /*! Set the description of the input trace to \a ident. */
  void setIdent( const string &ident );
    /*! The reference for the input line.
        Possible return values are RefDifferential, RefCommon, RefGround, RefOther.
	\sa referenceStr(), setReference() */
  RefType reference( void ) const;
    /*! The reference for the input line as a string.
	\sa reference(), setReference() */
  string referenceStr( void ) const;
    /*! The reference type \a ref as a string.
	\sa reference(), setReference() */
  static string referenceStr( RefType ref );
    /*! Set the reference of the input line to \a ref.
        Possible values are RefDifferential, RefCommon, RefGround, RefOther.
        Defaults to RefDifferential.
	\sa reference(), referenceStr() */
  void setReference( RefType ref );
    /*! Set the reference of the input line to \a ref.
        Possible values are "differential", "common", "ground", "other".
	\sa reference(), referenceStr() */
  void setReference( const string &ref );

    /*! True if only positive values are acquired. */
  bool unipolar( void ) const;
    /*! Set the polarity of the input trace to \a unipolar. 
        If \a unipolar is true only positive values are acquired,
	if it is false positive and negative values are acquired.
        By default acquisition is bipolar. */
  void setUnipolar( bool unipolar );
    /*! Returns the gain index that is used to select the gain of 
        the input trace on the data acquisition board.
        \sa setGainIndex(), scale(), setScale(), unit(), setUnit() */
  int gainIndex( void ) const;
    /*! Set the gain index to \a gainindex.
	The gain index selects the gain of the input trace on the
	data acquisition board.
        \sa gainIndex(), scale(), setScale(), unit(), setUnit() */
  void setGainIndex( int gainindex );
    /*! Returns the data to be used by AnalogInput for converting
        raw data to voltage.
        \sa setGainData(), gainIndex() */
  char *gainData( void ) const;
    /*! Set the data to be used by AnalogInput for converting
        raw data to voltage to \a data.
        \sa gainData(), setGainIndex() */
  void setGainData( char *data );

    /*! The scale factor used for scaling the voltage data 
        to a secondary unit.
        \sa setScale(), unit(), setUnit() */
  float scale( void ) const;
    /*! Set the scale factor to \a scale.
	The scale factor \a scale is used to scale the voltage data to
	a secondary unit.
        \sa scale(), unit(), setUnit() */
  void setScale( float scale );
    /*! The secondary unit.
        \sa setUnit(), scale(), setScale() */
  string unit( void ) const;
    /*! Set the secondary unit to \a unit.
        \sa unit(), scale(), setScale() */
  void setUnit( const string &unit );
    /*! Set the specifications of a secondary unit.
	The voltage data are scaled by \a scale
	to get the data in the secondary unit \a unit.
        \sa unit(), scale(), setScale() */
  void setUnit( float scale, const string &unit );

    /*! Return a format string that is appropriate for writing out the data values. */
  string format( void ) const;

    /*! Returns 0 if the data are acquired,
        1 if the data are filtered from an InData, 
        2 if the data are filtered from an EventData. 
        \sa setSource() */ 
  int source( void ) const;
    /*! Set the source of the data to \a source. 
        \sa source() */
  void setSource( int source );


    /*! Return mode flags.
        The mode flags can be used to label the input trace.
        They do not effect the data aquisition. 
        \sa clearMode(), setMode(), addMode(), delMode() */
  int mode( void ) const;
    /*! Clear all mode flags. 
        \sa mode(), setMode(), addMode(), delMode() */
  void clearMode( void );
    /*! Set mode flags to \a flags. 
        \sa mode(), clearMode(), addMode(), delMode() */
  void setMode( int flags );
    /*! Add the bits specified by \a flags to the mode flags. 
        \sa mode(), clearMode(), setMode(), delMode() */
  void addMode( int flags );
    /*! Clear the bits specified by \a flags of the mode flags. 
        \sa mode(), clearMode(), setMode(), addMode() */
  void delMode( int flags );

    /*! Set the id of the default device to \a deviceid. */
  static void setDefaultDevice( int deviceid );
    /*! The id of the default device. */
  static int defaultDevice( void );

    /*! Write the internal variables to \a str. */
  friend ostream &operator<<( ostream &str, const InData &id );


 private:

  void construct( void );

    /*! Plain text description of RefType. */
  static const string RefStr[4];

    /*! Pointer to the source InData. */
  const InData *ID;
    /*! Number of data elements of the buffer reserved for the writing process
        and thus not accessible forthe reading process. */
  int NWrite;

    /*! Index of last restart of data acquisition. */
  int RestartIndex;
    /*! Index of last signal output. */
  int SignalIndex;

    /*! Delay in seconds from start trigger to start of aquisition. */
  double Delay;
    /*! Source of start pulse for data aquisition. */
  int StartSource;
    /*! True: Stop running data acquisition to process this one. */
  bool Priority;
    /*! Continuous aquisition mode? */
  bool Continuous;
    /*! Device identifier. */
  int Device;
    /*! Default device identifier. */
  static int DefaultDevice;
    /*! Channel number. */
  int Channel;
    /*! Trace number. */
  int Trace;
    /*! Identifier. */
  string Ident;
    /*! Reference mode. */
  RefType Reference;
    /*! True if the data are only positive. */
  bool Unipolar;
    /*! Index determining the gain on the daq board. */
  int GainIndex;
    /*! Some data used by AnalogInput to convert raw data from the data acquisition board to
        voltage */
  char *GainData;
    /*! Scale from voltage to a secondary unit. */
  float Scale;
    /*! The secondary unit. */
  string Unit;
    /*! The minimum possible value. */
  double MinValue;
    /*! The maximum possible value. */
  double MaxValue;
    /*! Freely usable mode integer. */
  int Mode;
    /*! The source of the data: 0: acquisition, 1: InData, 2: events. */
  int Source;

};

 
/*! 
  \class InDataIterator
  \author Jan Benda
  \version 1.2
  \brief Input iterator for the data elements of an InData.
*/

class InDataIterator // : public iterator< random_access_iterator_tag, double, int > 
{

  friend class InDataDiffIterator;
  friend class InDataTimeIterator;
    
public:
    
    /*! Constructs an empty invalid iterator for an InData. */
  InDataIterator( void ) 
    : ID( 0 ), Index( 0 ) {};
    /*! Constructs a valid iterator for an InData \a id
        pointing to element \a index. */
  InDataIterator( const InData &id, int index ) 
    : ID( &id ), Index( index ) {};
    /*! Copy constructor. */
  InDataIterator( const InDataIterator &p )
    : ID( p.ID ), Index( p.Index ) {};
    /*! Destructor. */
  ~InDataIterator( void ) {};
    
    /*! Assigns \a p to this. */
  InDataIterator &operator=( const InDataIterator &p );
    
    /*! Returns true if both iterators point to the same element 
        of the same instance of an InData. */
  inline bool operator==( const InDataIterator &p ) const
    { return ( ID == p.ID && Index == p.Index ); };
    /*! Returns true if the iterators do not point to the same element 
        of the same instance of an InData. */
  inline bool operator!=( const InDataIterator &p ) const
    { return ( ID != p.ID || Index != p.Index ); };
    /*! Returns true if \a this points to an element preceeding the 
        element where \a p points to. */
  inline bool operator<( const InDataIterator &p ) const
    { return ( ID == p.ID && Index < p.Index ); };
    /*! Returns true if \a this points to an element succeeding the 
        element where \a p points to. */
  inline bool operator>( const InDataIterator &p ) const
    { return ( ID == p.ID && Index > p.Index ); };
    /*! Returns true if \a this points to an element preceeding
        or equaling the element where \a p points to. */
  inline bool operator<=( const InDataIterator &p ) const
    { return ( ID == p.ID && Index <= p.Index ); };
    /*! Returns true if \a this points to an element succeeding
        or equaling the element where \a p points to. */
  inline bool operator>=( const InDataIterator &p ) const
    { return ( ID == p.ID && Index >= p.Index ); };
    /*! Returns true if this is a valid iterator, i.e.
        it points to an existing element. */
  inline bool operator!( void ) const
    { return ( ID != NULL && Index >= ID->minIndex() && Index < ID->currentIndex() ); };

    /*! Increments the iterator to the next element. */
  inline const InDataIterator &operator++( void )
    { Index++; return *this; };
    /*! Decrements the iterator to the previous element. */
  inline const InDataIterator &operator--( void )
    { Index--; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const InDataIterator &operator+=( unsigned int incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const InDataIterator &operator+=( signed int incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const InDataIterator &operator+=( unsigned long incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const InDataIterator &operator+=( signed long incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by an equivalent of time \a time seconds. */
  inline const InDataIterator &operator+=( double time )
    { assert( ID != 0 ); Index += ID->indices( time ); return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const InDataIterator &operator-=( unsigned int decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const InDataIterator &operator-=( signed int decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const InDataIterator &operator-=( unsigned long decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const InDataIterator &operator-=( signed long decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by an equivalent of time \a time seconds. */
  inline const InDataIterator &operator-=( double time )
    { assert( ID != 0 ); Index -= ID->indices( time ); return *this; };
    /*! Adds \a incr to the iterator. */
  inline InDataIterator operator+( unsigned int incr ) const
    { InDataIterator p( *this ); p.Index += incr; return p; };
    /*! Adds \a incr to the iterator. */
  inline InDataIterator operator+( signed int incr ) const
    { InDataIterator p( *this ); p.Index += incr; return p; };
    /*! Adds \a incr to the iterator. */
  inline InDataIterator operator+( unsigned long incr ) const
    { InDataIterator p( *this ); p.Index += incr; return p; };
    /*! Adds \a incr to the iterator. */
  inline InDataIterator operator+( signed long incr ) const
    { InDataIterator p( *this ); p.Index += incr; return p; };
    /*! Adds the equivalent of time \a time secondsr to the iterator. */
  inline InDataIterator operator+( double time ) const
    { InDataIterator p( *this ); assert( ID != 0 ); p.Index += ID->indices( time ); return p; };
    /*! Subtracts \a decr from the iterator. */
  inline InDataIterator operator-( unsigned int decr ) const
    { InDataIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts \a decr from the iterator. */
  inline InDataIterator operator-( signed int decr ) const
    { InDataIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts \a decr from the iterator. */
  inline InDataIterator operator-( unsigned long decr ) const
    { InDataIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts \a decr from the iterator. */
  inline InDataIterator operator-( signed long decr ) const
    { InDataIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts the equivalent of time \a time seconds from the iterator. */
  inline InDataIterator operator-( double time ) const
    { InDataIterator p( *this ); assert( ID != 0 ); p.Index -= ID->indices( time ); return p; };
    /*! Returns the number of elements between the two iterators. */
  inline int operator-( const InDataIterator &p ) const
    { if ( ID == p.ID ) return Index - p.Index; return 0; };
    
    /*! Returns the value of the data element where the iterator points to. */
  inline double operator*( void ) const
    { assert( ID != 0 && Index >= ID->minIndex() && Index < ID->size() ); return (*ID)[Index]; };
    /*! Returns the value of the data element where the iterator + \a n points to. */
  inline double operator[]( int n ) const
    { assert( ID != 0 && Index+n >= ID->minIndex() && Index+n < ID->size() ); return (*ID)[Index+n]; };
    
    
protected:

  const InData *ID;
  int Index;    
    
};

/*! 
  \class InDataDiffIterator
  \author Jan Benda
  \version 1.2
  \brief Input iterator for InData returning the difference of succeeding data elements.
*/

class InDataDiffIterator
{
    
public:
    
    /*! Constructs an empty invalid iterator for an InData. */
  InDataDiffIterator( void ) 
    : ID( 0 ), Index( 0 ), DiffWidth( 1 ) {};
    /*! Constructs an empty invalid iterator with width \a dw. */
  InDataDiffIterator( int dw ) 
    : ID( 0 ), Index( 0 ), DiffWidth( dw ) {};
    /*! Constructs a valid iterator for an InData \a id
        pointing to element \a index. */
  InDataDiffIterator( const InData &id, int index, int dw ) 
    : ID( &id ), Index( index ), DiffWidth( dw ) {};
    /*! Constructs an iterator from an InDataIterator. */
  InDataDiffIterator( const InDataIterator &p, int dw )
    : ID( p.ID ), Index( p.Index ), DiffWidth( dw ) {};
    /*! Copy constructor. */
  InDataDiffIterator( const InDataDiffIterator &p )
    : ID( p.ID ), Index( p.Index ), DiffWidth( p.DiffWidth ) {};
    /*! Destructor. */
  ~InDataDiffIterator( void ) {};
    
    /*! Assigns \a p to this. */
  InDataDiffIterator &operator=( const InDataDiffIterator &p );
    /*! Assigns \a p to this. */
  InDataDiffIterator &operator=( const InDataIterator &p );
    
    /*! Returns true if both iterators point to the same element 
        of the same instance of an InData. */
  inline bool operator==( const InDataDiffIterator &p ) const
    { return ( ID == p.ID && Index == p.Index ); };
    /*! Returns true if the iterators do not point to the same element 
        of the same instance of an InData. */
  inline bool operator!=( const InDataDiffIterator &p ) const
    { return ( ID != p.ID || Index != p.Index ); };
    /*! Returns true if \a this points to an element preceeding the 
        element where \a p points to. */
  inline bool operator<( const InDataDiffIterator &p ) const
    { return ( ID == p.ID && Index < p.Index ); };
    /*! Returns true if \a this points to an element succeeding the 
        element where \a p points to. */
  inline bool operator>( const InDataDiffIterator &p ) const
    { return ( ID == p.ID && Index > p.Index ); };
    /*! Returns true if \a this points to an element preceeding
        or equaling the element where \a p points to. */
  inline bool operator<=( const InDataDiffIterator &p ) const
    { return ( ID == p.ID && Index <= p.Index ); };
    /*! Returns true if \a this points to an element succeeding
        or equaling the element where \a p points to. */
  inline bool operator>=( const InDataDiffIterator &p ) const
    { return ( ID == p.ID && Index >= p.Index ); };
    /*! Returns true if this is a valid iterator, i.e.
        it points to an existing element. */
  inline bool operator!( void ) const
    { return ( ID != NULL && Index >= ID->minIndex() + DiffWidth && Index < ID->currentIndex() ); };
    
    /*! Increments the iterator to the next element. */
  inline const InDataDiffIterator &operator++( void )
    { Index++; return *this; };
    /*! Decrements the iterator to the previous element. */
  inline const InDataDiffIterator &operator--( void )
    { Index--; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const InDataDiffIterator &operator+=( unsigned int incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const InDataDiffIterator &operator+=( signed int incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const InDataDiffIterator &operator+=( unsigned long incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const InDataDiffIterator &operator+=( signed long incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by an equivalent of time \a time seconds. */
  inline const InDataDiffIterator &operator+=( double time )
    { assert( ID != 0 ); Index += ID->indices( time ); return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const InDataDiffIterator &operator-=( unsigned int decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const InDataDiffIterator &operator-=( signed int decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const InDataDiffIterator &operator-=( unsigned long decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const InDataDiffIterator &operator-=( signed long decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by an equivalent of time \a time seconds. */
  inline const InDataDiffIterator &operator-=( double time )
    { assert( ID != 0 ); Index -= ID->indices( time ); return *this; };
    /*! Adds \a incr to the iterator. */
  inline InDataDiffIterator operator+( unsigned int incr ) const
    { InDataDiffIterator p( *this ); p.Index += incr; return p; };
    /*! Adds \a incr to the iterator. */
  inline InDataDiffIterator operator+( signed int incr ) const
    { InDataDiffIterator p( *this ); p.Index += incr; return p; };
    /*! Adds \a incr to the iterator. */
  inline InDataDiffIterator operator+( unsigned long incr ) const
    { InDataDiffIterator p( *this ); p.Index += incr; return p; };
    /*! Adds \a incr to the iterator. */
  inline InDataDiffIterator operator+( signed long incr ) const
    { InDataDiffIterator p( *this ); p.Index += incr; return p; };
    /*! Adds the equivalent of time \a time secondsr to the iterator. */
  inline InDataDiffIterator operator+( double time ) const
    { InDataDiffIterator p( *this ); assert( ID != 0 ); p.Index += ID->indices( time ); return p; };
    /*! Subtracts \a decr from the iterator. */
  inline InDataDiffIterator operator-( unsigned int decr ) const
    { InDataDiffIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts \a decr from the iterator. */
  inline InDataDiffIterator operator-( signed int decr ) const
    { InDataDiffIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts \a decr from the iterator. */
  inline InDataDiffIterator operator-( unsigned long decr ) const
    { InDataDiffIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts \a decr from the iterator. */
  inline InDataDiffIterator operator-( signed long decr ) const
    { InDataDiffIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts the equivalent of time \a time seconds from the iterator. */
  inline InDataDiffIterator operator-( double time ) const
    { InDataDiffIterator p( *this ); assert( ID != 0 ); p.Index -= ID->indices( time ); return p; };
    /*! Returns the number of elements between the two iterators. */
  inline int operator-( const InDataDiffIterator &p ) const
    { if ( ID == p.ID ) return Index - p.Index; return 0; };
    
    /*! Returns the difference of the data element where the iterator points to
        and the by \a DiffWidth preceeding data element. */
  inline double operator*( void ) const;
    /*! Returns the difference of the data element where the iterator + \a n points to
        and the by \a DiffWidth preceeding data element. */
  inline double operator[]( int n ) const;
    
    
protected:

  const InData *ID;
  int Index;    
  int DiffWidth;
    
};


/*! 
  \class InDataTimeIterator
  \author Jan Benda
  \version 1.0
  \brief Input iterator for the time of an InData.
*/

class InDataTimeIterator
{

 public:
    
    /*! Constructs an empty invalid iterator for an InData. */
  InDataTimeIterator( void ) 
    : ID( 0 ), Index( 0 ) {};
    /*! Constructs a valid iterator for an InData \a id
        pointing to element \a index. */
  InDataTimeIterator( const InData &id, int index ) 
    : ID( &id ), Index( index ) {};
    /*! Constructs a valid iterator from \a p. */
  InDataTimeIterator( const InDataIterator &p )
    : ID( p.ID ), Index( p.Index ) {};
    /*! Copy constructor. */
  InDataTimeIterator( const InDataTimeIterator &p )
    : ID( p.ID ), Index( p.Index ) {};
    /*! Destructor. */
  ~InDataTimeIterator( void ) {};
    
    /*! Assigns \a p to this. */
  InDataTimeIterator &operator=( const InDataTimeIterator &p );
    /*! Assigns \a p to this. */
  InDataTimeIterator &operator=( const InDataIterator &p );
    
    /*! Returns true if both iterators point to the same element 
        of the same instance of an InData. */
  inline bool operator==( const InDataTimeIterator &p ) const
    { return ( ID == p.ID && Index == p.Index ); };
    /*! Returns true if the iterators do not point to the same element 
        of the same instance of an InData. */
  inline bool operator!=( const InDataTimeIterator &p ) const
    { return ( ID != p.ID || Index != p.Index ); };
    /*! Returns true if \a this points to an element preceeding the 
        element where \a p points to. */
  inline bool operator<( const InDataTimeIterator &p ) const
    { return ( ID == p.ID && Index < p.Index ); };
    /*! Returns true if \a this points to an element succeeding the 
        element where \a p points to. */
  inline bool operator>( const InDataTimeIterator &p ) const
    { return ( ID == p.ID && Index > p.Index ); };
    /*! Returns true if \a this points to an element preceeding
        or equaling the element where \a p points to. */
  inline bool operator<=( const InDataTimeIterator &p ) const
    { return ( ID == p.ID && Index <= p.Index ); };
    /*! Returns true if \a this points to an element succeeding
        or equaling the element where \a p points to. */
  inline bool operator>=( const InDataTimeIterator &p ) const
    { return ( ID == p.ID && Index >= p.Index ); };
    /*! Returns true if this is a valid iterator, i.e.
        it points to an existing element. */
  inline bool operator!( void ) const
    { return ( ID != NULL && Index >= ID->minIndex() && Index < ID->currentIndex() ); };
    
    /*! Increments the iterator to the next element. */
  inline const InDataTimeIterator &operator++( void )
    { Index++; return *this; };
    /*! Decrements the iterator to the previous element. */
  inline const InDataTimeIterator &operator--( void )
    { Index--; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const InDataTimeIterator &operator+=( unsigned int incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const InDataTimeIterator &operator+=( signed int incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const InDataTimeIterator &operator+=( unsigned long incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by \a incr. */
  inline const InDataTimeIterator &operator+=( signed long incr )
    { Index += incr; return *this; };
    /*! Increments the iterator by an equivalent of time \a time seconds. */
  inline const InDataTimeIterator &operator+=( double time )
    { assert( ID != 0 ); Index += ID->indices( time ); return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const InDataTimeIterator &operator-=( unsigned int decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const InDataTimeIterator &operator-=( signed int decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const InDataTimeIterator &operator-=( unsigned long decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by \a decr. */
  inline const InDataTimeIterator &operator-=( signed long decr )
    { Index -= decr; return *this; };
    /*! Decrements the iterator by an equivalent of time \a time seconds. */
  inline const InDataTimeIterator &operator-=( double time )
    { assert( ID != 0 ); Index -= ID->indices( time ); return *this; };
    /*! Adds \a incr to the iterator. */
  inline InDataTimeIterator operator+( unsigned int incr ) const
    { InDataTimeIterator p( *this ); p.Index += incr; return p; };
    /*! Adds \a incr to the iterator. */
  inline InDataTimeIterator operator+( signed int incr ) const
    { InDataTimeIterator p( *this ); p.Index += incr; return p; };
    /*! Adds \a incr to the iterator. */
  inline InDataTimeIterator operator+( unsigned long incr ) const
    { InDataTimeIterator p( *this ); p.Index += incr; return p; };
    /*! Adds \a incr to the iterator. */
  inline InDataTimeIterator operator+( signed long incr ) const
    { InDataTimeIterator p( *this ); p.Index += incr; return p; };
    /*! Adds the equivalent of time \a time secondsr to the iterator. */
  inline InDataTimeIterator operator+( double time ) const
    { InDataTimeIterator p( *this ); assert( ID != 0 ); p.Index += ID->indices( time ); return p; };
    /*! Subtracts \a decr from the iterator. */
  inline InDataTimeIterator operator-( unsigned int decr ) const
    { InDataTimeIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts \a decr from the iterator. */
  inline InDataTimeIterator operator-( signed int decr ) const
    { InDataTimeIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts \a decr from the iterator. */
  inline InDataTimeIterator operator-( unsigned long decr ) const
    { InDataTimeIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts \a decr from the iterator. */
  inline InDataTimeIterator operator-( signed long decr ) const
    { InDataTimeIterator p( *this ); p.Index -= decr; return p; };
    /*! Subtracts the equivalent of time \a time seconds from the iterator. */
  inline InDataTimeIterator operator-( double time ) const
    { InDataTimeIterator p( *this ); assert( ID != 0 ); p.Index -= ID->indices( time ); return p; };
    /*! Returns the number of elements between the two iterators. */
  inline int operator-( const InDataTimeIterator &p ) const
    { if ( ID == p.ID ) return Index - p.Index; return 0; };
    
    /*! Returns the time associated with the data element where 
        the iterator points to. */
  inline double operator*( void ) const
    { assert( ID != 0 && Index >= ID->minIndex() && Index < ID->size() ); return ID->pos( Index ); };
    /*! Returns the time associated with the data element where 
        the iterator + \a n points to. */
  inline double operator[]( int n ) const
    { assert( ID != 0 && Index+n >= ID->minIndex() && Index+n < ID->size() ); return ID->pos( Index+n ); };
    
    
protected:

  const InData *ID;
  int Index;    

};


inline double InDataDiffIterator::operator*( void ) const
{
  assert( ID != 0 ); 
  long j = Index - DiffWidth;
  assert( Index < ID->size() && j>=ID->minIndex() );
  return (*ID)[Index] - (*ID)[j];
}


inline double InDataDiffIterator::operator[]( int n ) const
{
  assert( ID != 0 ); 
  long i = Index + n;
  long j = i - DiffWidth;
  assert( i < ID->size() && j>=ID->minIndex() );
  return (*ID)[i] - (*ID)[j];
}


}; /* namespace relacs */

#endif /* ! _RELACS_INDATA_H_ */
