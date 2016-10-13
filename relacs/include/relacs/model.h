/*
  model.h
  Base class of all models used by Simulate.

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

#ifndef _RELACS_MODEL_H_
#define _RELACS_MODEL_H_ 1

#include <deque>
#include <string>
#include <QMenu>
#include <QMutex>
#include <QReadWriteLock>
#include <QSemaphore>
#include <QWaitCondition>
#include <QThread>
#include <QDateTime>
#include <relacs/inlist.h>
#include <relacs/outdata.h>
#include <relacs/analoginput.h>
#include <relacs/relacsplugin.h>
using namespace std;

namespace relacs {


class ModelThread;


/*! 
\class Model
\author Jan Benda
\brief Base class of all models used by Simulate.
*/

class Model : public RELACSPlugin 
{

  friend class Simulator;

public:

    /*! Construct a Model with name \a name (has to be identical
        to the class name) that belongs to the set of plugins
	named \a pluginset.
	The implementation of a class derived from Model
	has the version \a version and was written
	by \a author on \a date. */
  Model( const string &name,
	 const string &pluginset="",
	 const string &author="unknown",
	 const string &version="unknown",
	 const string &date=__DATE__ );
    /*! Deconstruct a Model. */
  virtual ~Model( void );

    /*! Reimplement this function with your own simulation.
        You can check the expected input traces using
	traces(), traceName(), deltat(), and scale().
	Eventually you should fill all input traces
	using push() and next() in an infinite loop that only terminates
	if interrupt() returns \c true..
        With signal() the current stimulus can be retrieved.
	\note When parameter of the simulation are changed
	the simulation thread is terminated by requesting main()
	to terminate via interrupt()
	and restarted by calling main() again. The input traces
	are, however, not cleared. Use time() to keep your simulation
	time in sync with the buffer time (and thus the stimulus times
	used by signal()). */
  virtual void main( void );

    /*! Process a new signal.
        By reimplementing this function, any signals can be preprocessed
	before they are used by the model via signal() in main().
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

    /*! This function is called at the end of a recording session
        and should return any metadata describing the model and its parameter.
	The metadata are then saved to the sessions info file.
        The default implementation simply returns the %Model's Options. */
  virtual Options metaData( void );

    /*! Returns the signal of output trace \a trace at time \a t. 
        Specifically, this function returns the data value 
	of the current signal at or right before time \a t.
        Time \a t is measured in seconds,
	relative to the time of the recorded traces.*/
  double signal( double t, int trace=0 ) const;

    /*! Returns \c true if the simulation thread should be stopped.
        Use this within main() to terminate the simulation properly.
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
    /*! Returns \c true if the %Model thread is running. */
  bool isRunning( void ) const;

    /*! Push the value \a val of trace \a trace to the data buffer.
        \sa main(), next() */
  void push( int trace, float val );
    /*! Tell relacs that one cycle of model calculations is finished
        and that the values for all traces have been pushed. 
        This function computes the model of a dynamic clamp task
        and waits if necessary to ensure real time behavior. */
  void next( void );

    /*! The number of traces that need to be simulated. */
  int traces( void ) const;
    /*! The name of trace \a trace of the simulated data. */
  string traceName( int trace ) const;
    /*! The time step for trace \a trace of the simulated data.
        The time step is set to the sampling interval of the data acquisition
	by default. Adjust it to your needs with setDeltat(). */
  double deltat( int trace ) const;
    /*! The current time of trace \a trace. 
        This is the number of so far pushed data elements times deltat(). */
  double time( int trace ) const;
    /*! The scale for scaling the voltage into a secondary unit
        of trace \a trace of the simulated data.
	The value in the secondary unit is \a scale times the voltage at
	the daq board. */
  float scale( int trace ) const;

    /*! Wait until signals are finished. */
  void waitOnSignals( void );

    /*! Returns the averaged load of the simulation process. */
  double load( void ) const;

    /*! Add specific actions to the menu. */
  virtual void addActions( QMenu *menu, bool doxydoc );


private:

  friend class ModelThread;

    /*! Clear the content of the data buffers and start the simulation.
        \sa clearData(), main(), restart() */
  void start( InList &data, AnalogInput *aidevice,
	      QReadWriteLock *datamutex, QWaitCondition *datawait );
    /*! Restart a previously stopped simulation.
        \sa stop(), notify(), start() */
  void restart( void );
    /*! The simulation thread. Simply calls main(). */
  void run( void );
    /*! Stop the simulation. */
  void stop( void );

    /*! Add output signal to the simulation. 
        Returns the starting time of the signal on success,
	otherwise -1.0.
        If \a wait is set \c true, then a thread waiting on the end of the signal was started. */
  double add( OutData &signal, bool wait );
    /*! Add output signals to the simulation. 
        Returns the starting time of the signals on success,
	otherwise -1.0.
        If \a wait is set \c true, then a thread waiting on the end of the signal was started. */
  double add( OutList &sigs, bool wait );
    /*! Stop current signals. */
  void stopSignals( void );
    /*! Remove all signals. */
  void clearSignals( void );

    /*! The elapsed time of the simulation in seconds. */
  double elapsed( void ) const;

  ModelThread *Thread;

  AnalogInput *AIDevice;

  int MaxPush;
  double MaxPushTime;
  int PushCount;
  QTime SimTime;
  double AveragedLoad;
  double AverageRatio;

  InList Data;
  QReadWriteLock *DataMutex;
  QWaitCondition *DataWait;

  struct OutTrace {
    OutTrace( void ) :
    Onset( 0.0 ), Offset( 0.0 ), LastSignal( 0.0 ), ModelValue( 0.0 ), Finished( false )
    {};
    OutTrace( double t, const OutData &signal ) : 
    Onset( t ), Offset( t + signal.totalDuration() - signal.delay() ),
      Buffer( signal ), LastSignal( 0.0 ), ModelValue( 0.0 ), Finished( false )
    {};
    OutTrace( const OutTrace &signal ) :
    Onset( signal.Onset ), Offset( signal.Offset ),
      Buffer( signal.Buffer ), LastSignal( signal.LastSignal ),
      ModelValue( signal.ModelValue ), Finished( signal.Finished )
    {};
    double Onset;
    double Offset;
    OutData Buffer;
    mutable double LastSignal;
    double ModelValue;
    bool Finished;
  };
  deque< OutTrace > Signals;
  vector< int > SignalChannels;
  vector< float > SignalValues;
  QMutex SignalMutex;
  QSemaphore SignalsWait;

  bool InterruptModel;
  mutable QMutex InterruptLock;
  QWaitCondition InputWait;

};


class ModelThread : public QThread
{

public:
  
  ModelThread( Model *m );
  virtual void run( void );


private:

  Model *M;

};


}; /* namespace relacs */

#endif /* ! _RELACS_MODEL_H_ */

