/*
  model.h
  Base class of all models used by Simulate.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _RELACS_MODEL_H_
#define _RELACS_MODEL_H_ 1

#include <vector>
#include <string>
#include <qpopupmenu.h>
#if QT_VERSION >= 0x030301
#define USEWAITCONDITION
#endif
#include <qmutex.h>
#ifdef USEWAITCONDITION
#include <qwaitcondition.h>
#endif
#include <qthread.h>
#include <qdatetime.h>
#include <relacs/cyclicarray.h>
#include <relacs/outdata.h>
#include <relacs/relacsplugin.h>
using namespace std;

namespace relacs {


/*! 
\class Model
\author Jan Benda
\version 0.8
\brief Base class of all models used by Simulate.
\bug Check locking of signals AND traces!
\bug restart() synchronization of real time, data buffers and signal times does not work
\todo Handling of multiple signals.
*/

class Model : public RELACSPlugin, public QThread 
{

  friend class Simulator;

public:

    /*! Construct a Model with name \a name (has to be identical
        to the class name), widget title \a title,
	that belongs to the set of plugins named \a pluginset.
	The implementation of a class derived from Model
	has the version \a version and was written
	by \a author on \a date. */
  Model( const string &name,
	 const string &title="", 
	 const string &pluginset="",
	 const string &author="unknown",
	 const string &version="unknown",
	 const string &date=__DATE__ );
    /*! Deconstruct a Model. */
  virtual ~Model( void );

    /*! Reimplement this function with your own simulation.
        \sa push(), traces(), deltat() */
  virtual void main( void );

    /*! Process a new signal.
        By reimplementing this function, any signals can be preprocessed
	before they are used by the model.
        The original signal is \a source.
	The processed signal has to be written to \a dest.
        \a dest is initialized as an empty OutData.
	By default, \a dest is simply copied to \a source. */
  virtual void process( const OutData &source, OutData &dest );

    /*! This function is called whenever some Options of the Model are 
        changed.
        The default implementation simply restarts the simulation,
        assuming that main() first reads out the Options. */
  virtual void notify( void );

    /*! Returns the signal at time \a t. 
        Specifically, this function returns the data value 
	of the current signal at or right before time \a t. */
  double signal( double t ) const;
    /*! Returns the linearly interpolated signal at time \a t. */
  double signalInterpolated( double t ) const;

    /*! Returns \c true if the simulation thread should be stopped.
        Use this from within main()
        \code
         void MyModel::main( void )
	 {
  	   ...
	   if ( interrupt() )
	     return;
	   ...
	 }
        \endcode
        */
  bool interrupt( void ) const;

    /*! Push the value \a val of trace \a trace to the data buffer.
        \sa main() */
  void push( int trace, float val );

    /*! The number of traces that need to be simulated. */
  int traces( void ) const;
    /*! The device of trace \a trace of the simulated data. */
  int device( int trace ) const;
    /*! The channel number of trace \a trace of the simulated data. */
  int channel( int trace ) const;
    /*! The time step for trace \a trace of the simulated data.
        The time step is set to the sampling interval of the data acquisition
	by default. Adjust it to your needs with setDeltat(). */
  double deltat( int trace ) const;
    /*! The current time of trace \a trace. 
        This is the number of so far pushed data elements times deltat(). */
  double time( int trace ) const;
    /*! The currently effective gain of the daq-board
        of trace \a trace of the simulated data.
        \sa scale(), offset() */
  float gain( int trace ) const;
    /*! The scale for scaling the voltage into a seconday unit
        of trace \a trace of the simulated data.
	The value in the secondary unit is \a scale times the voltage at
	the daq board plus the \a offset.
        \sa gain(), offset() */
  float scale( int trace ) const;
    /*! The offset for the seconday unit
        of trace \a trace of the simulated data.
	The value in the secondary unit is \a scale times the voltage at
	the daq board plus the \a offset.
        \sa gain(), scale() */
  float offset( int trace ) const;

    /*! Returns the averaged load of the simulation process. */
  double load( void ) const;

    /*! Add specific actions to the menu. */
  virtual void addActions( QPopupMenu *menu );


private:

     /*! Set the time step of trace \a trace to \a deltat. */
  void setDeltat( int trace, double deltat );

    /*! Clear the content of the data buffers and start the simulation.
        \sa clearData(), main(), restart() */
  void start( void );
    /*! Restart a previously stopped simulation.
        \sa stop(), notify(), start() */
  void restart( void );
    /*! The simulation thread. Simply calls main(). */
  virtual void run( void );
    /*! Stop the simulation. */
  void stop( void );

    /*! The number of unread data elements of the buffer of trace \a trace. */
  inline double size( int trace ) { return Data[trace].Buffer.readSize(); };
    /*! Retrieve a single data value of the buffer of trace \a trace. */
  inline float pop( int trace ) { return Data[trace].Buffer.read(); };

    /*! Add trace of device \a device, channel no. \a channel
        with sampling interval \a deltat seconds,
	offset \a offs, voltage to secondary unit gain \a gain,
	gain of the daq channel \a gain, and
	a buffer with \a nbuffer elements.
	\sa clear() */
  void add( int device, int channel, double deltat, 
	    double offs, double scale, double gain, int nbuffer );
    /*! Clear trace buffers. 
        \sa add() */
  void clear( void );
    /*! Clear content of the trace buffers. 
        \sa clear() */
  void clearData( void );

    /*! Add output signal to the simulation. 
        Returns the starting time of the signal on success,
	otherwise -1.0. */
  double add( OutData &signal );
    /*! Stop current signal. */
  void stopSignal( void );
    /*! Remove all signals. */
  void clearSignals( void );

    /*! The elapsed time of the simulation in seconds. */
  double elapsed( void ) const;

    /*! True if the simulation was restarted since the last call of restart(). */
  bool restarted( void );

  int MaxPush;
  double MaxPushTime;
  int PushCount;
  QTime SimTime;
  bool Restarted;
  double AveragedLoad;
  double AverageRatio;

  struct InTrace {
    InTrace( void ) : Device( 0 ), Channel( 0 ), DeltaT( 0.0 ), 
		      Offset( 0.0 ), Scale( 1.0 ), Gain( 1.0 ),
		      Buffer() {};
    InTrace( int device, int channel, double deltat, 
	     float offs, float scale, float gain, int nbuffer )
      : Device( device ), Channel( channel ), DeltaT( deltat ), 
	Offset( offs ), Scale( scale ), Gain( gain ), 
	Buffer( nbuffer )
      {};
    InTrace( const InTrace &td )
      : Device( td.Device ), Channel( td.Channel ), DeltaT( td.DeltaT ), 
	Offset( td.Offset ), Scale( td.Scale ), Gain( td.Gain ),
	Buffer( td.Buffer )
      {};
    void clear( void ) { Buffer.clear(); };
    int Device;
    int Channel;
    double DeltaT;
    float Offset;
    float Scale;
    float Gain;
    CyclicArrayF Buffer;
  };
  vector< InTrace > Data;

  struct OutTrace {
    OutTrace( void ) : Onset( 0.0 ), Offset( 0.0 ) {};
    OutTrace( double t, const OutData &signal ) : 
      Onset( t ), Offset( t + signal.totalDuration() - signal.delay() ), Buffer( signal ) {};
    OutTrace( const OutTrace &signal ) : Onset( signal.Onset ), Offset( signal.Offset ), Buffer( signal.Buffer ) {};
    double Onset;
    double Offset;
    OutData Buffer;
  };
  vector< OutTrace > Signals;
  double SignalEnd;
  QMutex SignalMutex;

  bool InterruptModel;
  mutable QMutex InterruptLock;
#ifdef USEWAITCONDITION
  QWaitCondition SleepWait;
#endif

};


}; /* namespace relacs */

#endif /* ! _RELACS_MODEL_H_ */

