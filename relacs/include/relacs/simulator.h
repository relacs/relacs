/*
  simulator.h
  Simulation of Acquire

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_SIMULATOR_H_
#define _RELACS_SIMULATOR_H_ 1

#include <relacs/acquire.h>
using namespace std;

namespace relacs {


/*! 
\class Simulator
\brief Simulation of Acquire
\author Jan Benda
\version 0.8

\todo Need possibility to get information of the real devices
(limitations: maximum sampling rate, number of channels, bits,
possible gain factors, maximum attenuation).
Maybe via a single DAQ simulator that stores this information
in a simulation config file.
\todo readBuffer implementation
\todo support of multiple inputs and outputs
*/

class Model;

class Simulator : public Acquire
{

public:

    /*! Construct an empty Simulator. */
  Simulator( void );
    /*! Construct a Simulator using the Model \a sim. */
  Simulator( Model &sim );
    /*! Deconstruct a Simulator. */
  ~Simulator( void );

    /*! Return the Model. */
  Model *model( void );
    /*! Set the model to \a sim. */
  void setModel( Model *sim );
    /*! Clear the Model. */
  void clearModel( void );

    /*! Start analog input with the settings given by \a data. 
	Returns 0 on success, negative numbers otherwise.
        Possible errors are indicated by the error state of \a data. */
  virtual int read( InList &data );
    /*! Simply returns 0. */
  virtual int readData( void );
    /*! Transfer simulated data to the data from the last call of read().
	Returns 0 on success, negative numbers otherwise. */
  virtual int convertData( void );
    /*! Returns pointer to the raw data buffer in \a buffer
        of the analog input device \a ai
        and its current size in \a n (in bytes). */
  //  virtual void readBuffer( char **buffer, int *n, int ai=0 );
    /*! Stop simulation. */
  virtual int stopRead( void );

    /*! Output of a signal \a signal. */
  virtual int setupWrite( OutData &signal );
  virtual int startWrite( OutData &signal );
  virtual int setupWrite( OutList &signal );
  virtual int startWrite( OutList &signal );
    /*! Transfer data to hardware driver. */
  virtual int writeData( void );
    /*! Stop current stimulus. */
  virtual int stopWrite( void );
    /*! Check for a new signal event and add it to \a events.
        \param[out] signaltime is set to the time of the most recent signal.
	\param[out] data all traces get the current signalTime() set
	\param[out] events the stimulus events gets the current signal added
        \return \c true if there was a new signal event. */
  virtual bool readSignal( double &signaltime, InList &data, EventList &events );

    /*! Direct output of a single data value as specified by \a signal
        to the DAQ boards. */
  virtual int directWrite( OutData &signal );
    /*! Direct output of single data values as specified by \a signal
        to different channels of the DAQ boards. */
  virtual int directWrite( OutList &signal );

    /*! Write a zero to all analog output channels. 
        \param[in] channels resets all physical output channels. 
        \param[in] params resets parameter channels. */
  virtual int writeReset( bool channels=true, bool params=true );
    /*! Set the output of channel \a channel on device \a device to zero.
        Returns 0 on success or a negative number on error. */
  virtual int writeZero( int channel, int device );

    /*! Stop all activity. */
  virtual void stop( void );


 protected:

    /*! Restart data aquisition and write signals 
        pending on devices in \a aos.
        If still running, stop analog input first.
	If \a directao, then the analog output signals are scheduled for direct outut.
        If \a updategains, the input gains are updated as well. */
  virtual int restartRead( vector< AOData* > &aos, bool directao,
			   bool updategains );


private:

  Model *Sim;
  InList Data;

};

}; /* namespace relacs */

#endif /* ! _RELACS_SIMULATOR_H_ */

