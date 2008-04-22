/*
  ../include/relacs/repro.h
  Parent class of all research programs.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _REPRO_H_
#define _REPRO_H_

#include <qglobal.h>
#if QT_VERSION >= 0x030301
#define USEWAITCONDITION
#endif
#include <qmutex.h>
#ifdef USEWAITCONDITION
#include <qwaitcondition.h>
#endif
#include <qthread.h>
#include <qdatetime.h>
#include <relacs/outlist.h>
#include "relacsplugin.h"


/*! 
\class RePro
\author Jan Benda
\author Christian Machens
\version 3.0
\brief Parent class of all research programs.
\todo ~RePro: auto remove from RePros?
\todo addPath: something with a format string giving the RePro's name, totalRuns, etc.
\bug What is that saving(), setSaving(), noSaving() all about?
*/

class RePro : public RELACSPlugin, public QThread
{
  Q_OBJECT

public:
  
  static const int MacroFlag = Parameter::ChangedFlag >> 1;
  static const int OverwriteFlag = Parameter::ChangedFlag >> 2;
  static const int CurrentFlag = Parameter::ChangedFlag >> 3;

    /*! Different return values for the main() function. */
  enum DoneState {
      /*! The RePro completed its job and terminated normally. */
    Completed,
      /*! The RePro terminated before completing its job
	  (usually due to a request by the user). */
    Aborted,
      /*! The RePro was not able to do its job properly. */
    Failed,
      /*! Keep executing the RePro.
	  Don't use this as a return value for main(). */
    Continue
  };

    /*! Constructs a RePro with name \a name (has to be identical
        to the class name), widget title \a title,
	that belongs to the set of plugins named \a pluginset.
	The implementation of a class derived from Model
	has the version \a version and was written
	by \a author on \a date.
        The date defaults to the date of compilation. */
  RePro( const string &name,
	 const string &title="", 
	 const string &pluginset="",
	 const string &author="unknown",
	 const string &version="unknown",
	 const string &date=__DATE__ );
    /*! Destructs a RePro. */
  virtual ~RePro( void );

    /*! */
  virtual void setName( const string &name );

    /*! Read the RePro's default settings from the config file. */
  virtual void readConfig( StrQueue &sq );
    /*! Saves the RePro's default settings to the config file. */
  virtual void saveConfig( ofstream &str );

    /*! This function is called when the RePro is started.
        You need to implement this function with some useful code, 
	e.g. read options, create and play stimulus, 
	analyze the read voltage traces and events, etc.
        This function is executed as a thread.
	Within this thread the RePro, all input data, events, meta data,
	and stimulus data are already locked (via lockAll()).
	Access to those data is unlocked during sleep() and sleepOn().
	Beware that main() is executed in a thread different from the GUI thread.
	Either use lockGUI() and unlockGUI() to lock and unlock the GUI thread
	while calling some functions that eventually paint something on the screen,
	or use postCustomEvent().
	The later is to be preferred over the lockGUI().
	The function should return one of the \a DoneStates except Continue.
	After main() terminated
        a message is written ( "Repro succesfully completed",
	"RePro stopped" or "RePro interupted", depending on the return value)
        and RELACS is informed. 
	You need to return from main() if the sleep(), sleepOn(),
	or interrupt() functions return \c true.
        \sa interrupt(), sleep(), timeStamp(), sleepOn() */
  virtual int main( void ) = 0;

    /*! Returns \c true if the RePro thread should be stopped immediately.
        Use this from within main()
        \code
         int main( void )
	 {
	 ...
	 if ( interrupt() )
	   return Aborted;
	 ...
	 }
        \endcode
        */
  bool interrupt( void ) const;

    /*! Kindly ask the RePro to stop. 
        Sets interrupt() to \c true and terminates all sleeps.
        You still need to wait on the RePro thread to finish. */
  void requestStop( void );

    /*! Sleep for \a t seconds.
        Right before returning, the data and event buffers are updated.
        Returns \a true if the main() thread needs to be stopped.
        \sa sleepOn(), timeStamp(), interrupt() */
  bool sleep( double t );
    /*! Memorize the current time. 
        This time is used by sleepOn() to calculate the remaining
	time to sleep. 
        \sa sleepOn(), sleep() */
  void timeStamp( void );
    /*! Sleep for \a t seconds relative to the last call of timeStamp(). 
        Returns \a true if the main() thread needs to be stopped.
        \sa sleep(), timeStamp(), interrupt() */
  bool sleepOn( double t );

    /*! Reimplement this function to reset or initialize your RePro
        when a new session is started.
        The current implementation resets the counters completeRuns(),
	totalRuns(), allRuns(), failedRuns(). */
  virtual void sessionStarted( void );
    /*! Reimplement this function to reset your RePro 
        after a session is stopped.
        If the session was saved \a saved is set to \c true.
        The current implementation resets the counters completeRuns(),
	totalRuns(), allRuns(), failedRuns(). */
  virtual void sessionStopped( bool saved );

    /*! In main(): true if previous call of this RePro was
        completed, i.e. main() returned Completed.
        \sa aborted(), failed(), completeRuns() */
  bool completed( void ) const;
    /*! In main(): true if previous call of this RePro was
        aborted by the user, i.e. main() returned Aborted.
        \sa completed(), failed(), totalRuns() */
  bool aborted( void ) const;
    /*! In main(): true if previous call of this RePro failed,
	i.e. main() returned Failed.
        \sa completed(), aborted(), failedRuns() */
  bool failed( void ) const;

    /*! Number of completed runs of this RePro (main() returned Completed)
        since last call of init() or reset().
        \sa completed(), totalRuns(), allRuns(), failedRuns() */
  int completeRuns( void ) const;
    /*! Number of runs of this RePro which did not fail
        since last call of init() or reset(),
	i.e. where main() returned Aborted or Completed.
        \sa completeRuns(), allRuns(), failedRuns() */
  int totalRuns( void ) const;
    /*! Number of runs of this RePro
        since last call of init() or reset().
        \sa completeRuns(), totalRuns(), failedRuns() */
  int allRuns( void ) const;
    /*! Number of runs of this RePro which failed
        (main() returned Failed)
        since last call of init() or reset().
        \sa completeRuns(), totalRuns(), allRuns() */
  int failedRuns( void ) const;

    /*! Time in seconds this RePro is currently running.
        \sa reproTimeStr() */
  double reproTime( void ) const;
    /*! A string displaying the elapsed time of the current RePro.
        \sa reproTime() */
  string reproTimeStr( void ) const;

    /*! Test of a single output signal \a signal for validity.
        Returns 0 on success or a negative number if \a signal
        is not valid.
        The error state of \a signal is set appropriately. */
  int testWrite( OutData &signal );
    /*! Test of a multiple output signals \a signal for validity.
        Returns 0 on success or a negative number if \a signal
        is not valid.
        The error state of \a signal is set appropriately. */
  int testWrite( OutList &signal );

    /*! Convert the data of \a signal into a device dependent format.
        Subsequent calls of write( signal ) will not
	convert the data again.
        Once convert() is called, you have to maintain the internal
        data buffer of \a signal, i.e. if you change the data
	you have to call convert() again before you are able to write out 
        the changed signal with write().
	You may also clear the internal buffer by calling signal.clearBuffer(). */
  int convert( OutData &signal );
    /*! Convert the data of \a signal into a device dependent format.
        Subsequent calls of write( signal ) will not
	convert the data again.
        Once convert() is called, you have to maintain the internal
        data buffers of \a signal, i.e. if you change the data
	you have to call convert() again before you are able to write out 
        the changed signals with write().
	You may also clear the internal buffers by calling signal.clearBuffer(). */
  int convert( OutList &signal );

    /*! Output of a signal \a signal.
        See OutData about how to specify output channel, sampling rate, 
	intensity, delay, etc. 
	Returns 0 on success.
        If the output of the signal failed, a negative number is returned and
        the reason is specified in the error state of \a signal. 
        After writing the signal to the daq-board, the signal wrote()
        is emitted, which can be used to check the success of 
	the output operation. */
  int write( OutData &signal );
    /*! Output of multiple signals \a signal.
        See OutList about how to specify output channel, sampling rate, 
	intensity, delay, etc. 
	Returns 0 on success.
        If the output of the signal failed, a negative number is returned and
        the reason is specified in the error state of \a signal. 
        After writing the signal to the daq-board, the signal wrote()
        is emitted, which can be used to check the success of 
	the output operation. */
  int write( OutList &signal );

    /*! Set the output of channel \a channel on device \a device to zero.
        Returns 0 on success or a negative number on error. 
        If \a device is negative, the default device is used. */
  int writeZero( int channel, int device );
    /*! Set the output of the trace with index \a index to zero.
        Returns 0 on success or a negative number on error. */
  int writeZero( int index );
    /*! Set the output of the trace with name \a trace to zero.
        Returns 0 on success or a negative number on error. */
  int writeZero( const string &trace );

    /*! Stop analog output of all analog output devices. */
  int stopWrite( void );

    /*! Print the default messages if \a message is \c true.
        This is the default.
        \sa noMessage(), message() */
  void setMessage( bool message=true );
    /*! Do not print any of the default messages.
        \sa setMessage(), message()*/
  void noMessage( void );
    /*! Write a message \a msg in the right half of the status line.
        \sa setMessage(), noMessage() */
  void message( const string &msg );

    /*! Reimplement this function to handle key events. */
  virtual void keyPressEvent( QKeyEvent *e );
    /*! Reimplement this function to handle key events. */
  virtual void keyReleaseEvent( QKeyEvent *e );

    /*! How many requests to stop the repro are there
        since the RePro was started?
        \sa setSoftStop(), clearSoftStop(), setSoftStopKey() */
  int softStop( void );
    /*! Set the number of requested stops for the repro to \a s.
        \sa softStop(), clearSoftStop(), setSoftStopKey() */
  void setSoftStop( int s=1 );
    /*! Set the number of requested stops for the repro to zero.
        \sa softStop(), setSoftStop(), setSoftStopKey() */
  void clearSoftStop( void );
    /*! Set the keycode initiating a soft stop to \a keycode.
        \sa softStop(), setSoftStop(), clearSoftStop() */
  void setSoftStopKey( int keycode );

    /*! */
  void plotToggle( bool on, bool fixed, double length, double offs );

    /*! Return the name of the currently running macro.
        If the %RePro was called from the %RePro menu
        then "RePro" is returned. */
  string macroName( void );
    /*! Return the parameters of the currently running macro. */
  string macroParam( void );

    /*! Return a path where the RePro can store some global stuff. 
        This is the \c repropath from the RELACS settings plus
        the name of the %RePro.
        If \a version is \c true, a subdirectory is created for
        each version of the %RePro.
        \sa addReProPath() */
  string reproPath( bool version=true );
    /*! Returns \a file added to the reproPath().
        \sa reProPath() */
  string addReProPath( const string &file, bool version=true );

    /*! This function is called to set the options of the RePro. 
        A standard dialog (OptDialog) is launched to set the repro's options. 
        Reimplement it, if you want another interface. */
  virtual void dialog( void );
    /*! Options of the RePro that overwrite macro options. */
  Options &overwriteOptions( void );

    /*! This function is evoked to check for valid options.
	If \a opttxt is empty,
	then all options are set to their default values.
	Otherwise \a opttxt is read in.
        An error message is returned. */
  string setOptions( const string &opttxt );

  bool saving( void ) const;
  virtual void setSaving( bool saving );
    /*! Indicate that the data of this RePro don't have to be saved. */
  void noSaving( void );


signals:

  /*
  // *! Request stop of data acquisition.
        Do not use this function! It will in future be removed from the RePro methods! * //
  void stopRW( );
    // *! Request restart of data acquisition.
        Do not use this function! It will in future be removed from the RePro methods! * //
  void restartRW( );
  */


private:

    /*! The thread that runs the RePro.
        It calls main(). */
  virtual void run( void );

  bool Interrupt;
  mutable QMutex InterruptLock;
#ifdef USEWAITCONDITION
  QWaitCondition SleepWait;
#endif
  QTime SleepTime;

  int LastState;
  int CompleteRuns;
  int TotalRuns;
  int AllRuns;
  int FailedRuns;

  bool Saving;

  Options OverwriteOpt;

  int SoftStop;
  int SoftStopKey;

  bool PrintMessage;

  mutable QTime ReProTime;


private slots:

  virtual void dClosed( int r );


};


#endif
