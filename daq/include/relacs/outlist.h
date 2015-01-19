/*
  outlist.h
  A container for OutData

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

#ifndef _RELACS_OUTLIST_H_
#define _RELACS_OUTLIST_H_ 1

#include <string> 
#include <deque> 
#include <relacs/outdata.h>
using namespace std;

namespace relacs {


/*!
\class OutList
\author Jan Benda
\brief A container for OutData
*/

class OutList
{

public:

    /*! Constructs an empty OutList. */
  OutList( void );
    /*! Constructs an OutList containing the single OutData \a signal.
        \sa push( OutData& ) */
  OutList( OutData &signal );
    /*! Constructs an OutList containing the single pointer \a signal.
        Setting \a own to \c true transfers the ownership to the OutList. 
        \sa add( OutData* ) */
  OutList( OutData *signal, bool own=false );
    /*! Copy constructor. */
  OutList( const OutList &ol );
    /*! Destructor. */
  ~OutList( void );

    /*! The number of OutData signals in the OutList. */
  int size( void ) const { return OL.size(); };
    /*! True if there are no OutData signals contained in the OutList. */
  bool empty( void ) const { return OL.empty(); };
    /*! Resize the OutList such that it contains \a n OutData signals.
        If \a n equals zero, clear() is called.
	If a larger size than the current size() is requested 
	than empty OutData are appended, each of capacity \a m
	and sampling interval \a step seconds.
	\sa clear(), size(), empty() */
  void resize( int n, int m=0, double step=1.0 );
    /*! Clear the OutList, i.e. remove all OutData signals the OutList owns. */
  void clear( void );

    /*! Assignment. */
  OutList &operator=( const OutList &ol );

    /*! Returns a const reference of the \a i -th OutData signal of the list. */
  const OutData &operator[]( int i ) const { return *(OL[i].OD); };
    /*! Returns a reference of the \a i -th OutData signal of the list. */
  OutData &operator[]( int i ) { return *(OL[i].OD); };

    /*! Returns a const reference to the first OutData signal in the list. */
  const OutData &front( void ) const;
    /*! Returns a reference to the first OutData signal in the list. */
  OutData &front( void );
    /*! Returns a const reference to the last OutData signal in the list. */
  const OutData &back( void ) const;
    /*! Returns a reference to the last OutData signal in the list. */
  OutData &back( void );

    /*! Return the index of the output data trace with identifier \a ident.
        If there is no trace with this identifier -1 is returned. */
  int index( const string &ident ) const;

    /*! Copy \a signal as a new element to the end of the list. */
  void push( OutData &signal );
    /*! Copy each output signal from \a sigs to the end of the list. */
  void push( const OutList &sigs );

    /*! Add the pointer \a signal as a new element to the end of the list.
        If \a own is set to \c true then the ownership of \a signal
        is transfered to the OutList, i.e. the OutList might delete it. */
  void add( OutData *signal, bool own=false );
    /*! Add the pointer \a signal as a new element to the end of the list.
        If \a own is set to \c true then the ownership of \a signal
        is transfered to the OutList, i.e. the OutList might delete it. */
  void add( const OutData *signal, bool own=false );
    /*! Add pointers to each output signal in \a sigs to the end of the list.
        If \a own is set to \c true then the ownership of the signals
        is transfered to the OutList, i.e. the OutList might delete it. */
  void add( const OutList &sigs, bool own=false );

    /*! Erase the OutData at index \a index. */
  void erase( int index );

    /*! Sort the output signals by increasing channel number. */
  void sortByChannel( void );
    /*! Sort the output signals by increasing device and 
        by increasing channel number. */
  void sortByDeviceChannel( void );

    /*! Returns the descriptions of the output signals. */
  const Options &description( void ) const;
    /*! Returns the descriptions of the output signals. */
  Options &description( void );

    /*! Set the device id of all signals to \a device. */
  void setDevice( int device );
    /*! Set the source for the start trigger of the output
        for all signals to \a startsource. */
  void setStartSource( int startsource );
    /*! Set delay for all signals to \a delay (in seconds). */
  void setDelay( double delay );
    /*! Set the priority of all output signals to \a priority.
        If \a priority is \a true then the output signal is processed even
	if there still is a data acquisition running.
	Otherwise the output signal is not processed and returns with an error. */
  void setPriority( bool priority=true );
    /*! Set the sampling rate of all output signals to \a rate Hertz */
  void setSampleRate( double rate );
    /*! Set the sampling interval of all output signals to \a step seconds. */
  void setSampleInterval( double step );
    /*! Set continuous mode of data aquisition for all signals to \a continuous. */
  void setContinuous( bool continuous=true );
    /*! If \a restart is set to \c true, then the ouput of the signals
        will restart the data acquisition. */
  void setRestart( bool restart=true );

    /*! \return The duration of the longest OutData in the list. */
  double maxLength( void ) const;

    /*! Reset the device indices for a delay of \a delay indices. */
  void deviceReset( int delay=0 );
    /*! Returns the required size of the device buffer
        (total number of data elements include delays). */
  int deviceBufferSize( void );

    /*! Return string with an error message. */
  string errorText( void ) const;

    /*! Clear all error flags and error messages of all output signals. */
  void clearError( void );
    /*! Set error flags of all output signals to \a flags. */
  void setError( long long flags );
    /*! Add the bits specified by \a flags to the error flags
        of all output signals. */
  void addError( long long flags );
    /*! Clear the bits specified by \a flags of the error flags
        of all output signals. */
  void delError( long long flags );
    /*! Add error code \a de originating from daq board to the error flags
        of all output signals. */
  void addDaqError( int de );
    /*! Set additional error string of all output signals to \a strg. */
  void setErrorStr( const string &strg );
    /*! Add \a msg to the additional error message of all output signals. */
  void addErrorStr( const string &strg );
    /*! Set additional error string of all output signals
        to the string describing the 
        standard C error code \a errnum (from \c errno). */
  void setErrorStr( int errnum );
    /*! Add the string describing the standard C error code \a errnum 
        (from \c errno) to the additional error string of all output signals. */
  void addErrorStr( int errnum );
    /*! Returns \c true if all output traces are ok. */
  bool success( void ) const;
    /*! Returns \c true if one or more output signals failed. */
  bool failed( void ) const;

    /*! Write content of all OutData variables to stream \a str
        (for debugging only). */
  friend ostream &operator<< ( ostream &str, const OutList &ol );


 private:

  struct OLE {
    OLE( void ) : OD( NULL ), Own( false ) {};
    OLE( OutData *od, bool own ) : OD( od ), Own( own ) {};
    const OLE &operator=( const OLE &ole ) 
    { if ( &ole == this ) return *this; OD = ole.OD; Own = ole.Own; return *this; };
    OutData *OD;
    bool Own;
  };

  deque< OLE > OL;
  Options Description;

  friend bool lessChannelOLE( const OLE &a, const OLE &b );
  friend bool lessDeviceChannelOLE( const OLE &a, const OLE &b );

};


}; /* namespace relacs */

#endif /* ! _RELACS_OUTLIST_H_ */
