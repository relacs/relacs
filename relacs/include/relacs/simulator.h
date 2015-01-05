/*
  simulator.h
  Simulation of Acquire

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

#ifndef _RELACS_SIMULATOR_H_
#define _RELACS_SIMULATOR_H_ 1

#include <relacs/acquire.h>
using namespace std;

namespace relacs {


/*! 
\class Simulator
\brief Simulation of Acquire
\author Jan Benda

\todo Need possibility to get information of the real devices
(limitations: maximum sampling rate, number of channels, bits,
possible gain factors, maximum attenuation).
Maybe via a single DAQ simulator that stores this information
in a simulation config file.
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
    /*! Stop simulation. */
  virtual int stopRead( void );

    /*! Output of a signal \a signal. */
  virtual int write( OutData &signal, bool setsignaltime=true );
  virtual int write( OutList &signal, bool setsignaltime=true );
    /*! Wait for analog output to be finished. */
  virtual int waitForWrite( void );
    /*! Stop current stimulus. */
  virtual int stopWrite( void );
    /*! Check for a new signal time and return it.
        \return the new signal time, -1.0 if there is no new signal. */
  virtual double getSignal( void );

    /*! Direct output of a single data value as specified by \a signal
        to the DAQ boards. */
  virtual int directWrite( OutData &signal, bool setsignaltime=true );
    /*! Direct output of single data values as specified by \a signal
        to different channels of the DAQ boards. */
  virtual int directWrite( OutList &signal, bool setsignaltime=true );

    /*! Set the output of channel \a channel on device \a device to zero.
        Returns 0 on success or a negative number on error. 
        \sa testWrite(), write(), writeData(), stopWrite() */
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

};

}; /* namespace relacs */

#endif /* ! _RELACS_SIMULATOR_H_ */

