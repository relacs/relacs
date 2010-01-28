/*
  relacsplugin.h
  Adds specific functions for RELACS plugins to ConfigDialog

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _RELACS_RELACSPLUGIN_H_
#define _RELACS_RELACSPLUGIN_H_ 1


#include <qapplication.h>
#include <relacs/tracespec.h>
#include <relacs/indata.h>
#include <relacs/inlist.h>
#include <relacs/eventdata.h>
#include <relacs/eventlist.h>
#include <relacs/outdata.h>
#include <relacs/outlist.h>
#include <relacs/metadata.h>
#include <relacs/plugins.h>
#include <relacs/configdialog.h>

namespace relacs {


/*! 
\class RELACSPlugin
\brief Adds specific functions for RELACS plugins to ConfigDialog
\author Jan Benda
\version 2.1

Whenever the RELACSPlugin is started (a RePro whenever it is executed,
all other plugins when an acquisition is started) the current values
of the Options are saved and can be later on accesed via settings().

A warning meassage can be displayed in a popup window with warning()
An info meassage can be displayed in a popup window with info().
Both warning() and info() write the message 
together with the current time to standard error as well.
For only writing a message to standard error together with the current time
use printlog().

All input traces and events can be accessed with traces() and events(),
respectively.
Single input traces and events can be accessed with 
trace( int ), trace( const string &),
events( int ), and events( const string & ), respectively.
The index of the trace or events that was filtered to obtain an input trace
can be retrieved with the traceInputTrace() and traceInputEvent() functions.
The index of the trace or events of which some events were detected
can be retrieved with the eventInputTrace() and eventInputEvent() functions.

The gains for the input channels of the data acquisition board can be set
with setGain(), adjustGain(), adjust(), activateGains().
Use these functions with care! Make sure that you do not interfere
with other RELACS plugins.

The current mode() of RELACS can be acquisition(), simulation(), or analysis().
If the mode is changed, modeChanged() is called that you can reimplement
according to your needs.

Whenever a recording session is started or stopped,
sessionStarted() or sessionStopped() are called, respectively.
You can implement these virtual functions according to your needs.
Other functions related to sessions are sessionData(), sessionTime(),
sessionTimeStr(), sessionRunning().

The meta data sections of the current recording can be accessed by metaData(const string&).
Whenever some of the meta data are set to new values,
the virtual function notifyMetaData() is called, that you can implement
according to your needs.
With metaData( void ) you get access to a few more functions for managing meta data.

All other RELACS plugins can be accessed:
devices(), device(), attenuator(), filter(), filterOpts(),
detector(), detectorOpts(), control(), controlOpts(),
repros(), reprosDialogOpts(), repro(), reproOpts(), currentRePro(), currentReProOpts()
but should be locked with
lockFilter(), unlockFilter(),
lockDetector(), unlockDetector(),
lockControl(), unlockControl(),
lockRePro(), unlockRePro().
Note, however, that inside the RePro::main() and Control::main()
functions the input data, event data, and the own data are
already locked.

addPath() prepends the current directory for data storage to a filename.
addPath() prepends the default directory for data storage.
saving() returns \c true whenever voltage traces and eventlists
are saved to disk (via SaveFiles).

Use postCustomEvent() or - if this is not possible -
lockGUI() and unlockGUI() for thread save drawing 
and other interactions with the GUI.

Some integers for identifying the type of the RELACS plugin are defined
and are used by the addDevice, addAttenuate, addAttuator,
addAnalogInput, addAnalofOutput, addModel, addFilter, addDetector,
addControl, and addRePro macros.

Two groups of configuration files are defined as Core and Plugins.
*/

class RePro;
class RePros;
class Filter;
class Control;
class Model;
class Device;
class AllDevices;
class Devices;
class DigitalIO;
class Attenuate;
class AttInterfaces;
class RELACSWidget;


class RELACSPlugin : public ConfigDialog
{
  Q_OBJECT

public:

    /*! The identifier for plugins derived from the Device-class. */
  static const int DeviceId = 1;
    /*! The identifier for plugins derived from the AnalogInput-class. */
  static const int AnalogInputId = 2;
    /*! The identifier for plugins derived from the AnalogOutput-class. */
  static const int AnalogOutputId = 4;
    /*! The identifier for plugins derived from the AnalogOutput-class. */
  static const int DigitalIOId = 8;
    /*! The identifier for plugins derived from the Attenuator-class. */
  static const int AttenuatorId = 16;
    /*! The identifier for plugins derived from the Attenuate-class. */
  static const int AttenuateId = 32;
    /*! The identifier for plugins derived from the Model-class. */
  static const int ModelId = 64;
    /*! The identifier for plugins derived from the Filter-class. */
  static const int FilterId = 128;
    /*! The identifier for plugins derived from the RePro-class. */
  static const int ReProId = 256;
    /*! The identifier for plugins derived from the Control-class. */
  static const int ControlId = 512;

    /*! Config-File group for RELACS core classes. */
  static const int Core = 0;
    /*! Config-File group for alr RELACS plugins, i.e.
        RePro, Filter, Control, Model, ... classes. */
  static const int Plugins = 1;

    /*! Construct an RELACSPlugin.
        The identifier \a configident is used for identifying this class
	in the configuration file of group \a configgroup.
        \a name has to be exactly the name of the class.
	The class has a widget \a title and
	belongs to the set of plugins named \a pluginset.
	The implementation of a class derived from %RELACSPlugin
	has a \a version and was written by \a author on \a date.
        \sa setConfigIdent(), setConfigGroup(),
	setName(), setTitle(), setAuthor(), setDate(), setVersion() */
  RELACSPlugin( const string &configident="", int configgroup=0,
		const string &name="", 
		const string &title="",
		const string &pluginset="",
		const string &author="unknown",
		const string &version="unknown",
		const string &date=__DATE__ );
    /*! Destruct the RELACSPlugin. */
  virtual ~RELACSPlugin( void );

    /*! The name of the plugin set the class belongs to. */
  string pluginSet( void ) const;

    /*! \return the plugin's options with the values at the time where
        the RELACSPlugin was started (a RePro whenever it was executed,
        all other plugins when an acquisition was started).
	\sa setSettings() */
  const Options &settings( void ) const;
    /*! \return the plugin's options with the values at the time where
        the RELACSPlugin was started (a RePro whenever it was executed,
        all other plugins when an acquisition was started).
	\sa setSettings() */
  Options &settings( void );
    /*! Copy the plugin's current Options to its settings(). */
  void setSettings( void );

    /*! Called whenever the mode is changed. */
  virtual void modeChanged( void );
    /*! This function is called whenever some values of meta data
        in MetaDataSection \a section have been changed.
        Implement this function if the plugin needs to react to this.
        The meta data mutex is already locked when this function is entered. */
  virtual void notifyMetaData( const string &section );
    /*! This function is called whenever some values of the stimulus data
        have been changed.
        Implement this function if the plugin needs to react to this.
        The stimulus data mutex is already locked when this function is entered. */
  virtual void notifyStimulusData( void );

    /*! Pass the %RELACSWidget \a rw to the RELACSPlugin. 
        This is used for accessing Devices, 
	AttInterfaces, Filters, Control, and RePros. */
  void setRELACSWidget( RELACSWidget *rw );

    /*! \return the name of the file to be displayed in the help dialog.
        This implementation returns pluginSet() + '-' + name() + \c ".html". */
  virtual string helpFileName( void ) const;


protected:

    /*! Set the name of the plugin set the class belongs to to \a pluginset. */
  virtual void setPluginSet( const string &pluginset );

    /*! Write the string \a s to standard error.
        The current time and the name of the plugin
	is written in front of \a s.
        Use this function instead of writing to \c cerr directly. */
  void printlog( const string &s ) const;
    /*! Opens a warning window with the message \a s.
        The message together with the current time and the name of the plugin
	is also written to standard error as a warning.
        If \a timeout is greater than zero, the message window is
        closed automatically after \a timeout seconds.
        You do NOT need to wrap this function with lockGUI() and unlockGUI(). */
  void warning( const string &s, double timeout=0.0 );
    /*! Opens an info window with the message \a s.
        The message together with the current time and the name of the plugin
	is also written to standard error.
        If \a timeout is greater than zero, the message window is
        closed automatically after \a timeout seconds.
        You do NOT need to wrap this function with lockGUI() and unlockGUI(). */
  void info( const string &s, double timeout=0.0 );

    /*! Post a custom event for thread save manipulations of the GUI.
        This is just a shortcut for
	\code
	QApplication::postEvent( this, new QCustomEvent( QEvent::User+type ) );
	\endcode
	For your own events use values greater than 10 for \a type.
        \sa lockGUI(), unlockGUI() */
  void postCustomEvent( int type=0 );
    /*! Locks the GUI thread. 
        Use it whenever you call a function from a thread
	that directly or indirectly draws on the screen.
	Don't forget to unlock the GUI thread afterwards!
	\code
	lockGUI();
	updateWidgetContent(); // whatever this is
	unlockGUI();
	\endcode
        \sa unlockGUI(), postCustomEvent() */
  void lockGUI( void );
    /*! Unlocks the GUI thread.. 
        \sa lockGUI(), postCustomEvent() */
  void unlockGUI( void );

    /*! Lock the input data and events for reading. */
  void readLockData( void );
    /*! Lock the input data and events for writing. */
  void writeLockData( void );
    /*! Unlock the input data and events. */
  void unlockData( void );
    /*! Returns how often the input data are locked. 
        For debugging only. */
  int dataMutexCount( void );


 public:

    /*! Lock the RELACSPlugin (via lock()), the input data and events
        (via lockData()), the meta data (via lockMetaData()),
        and the stimulus data (via lockStimulusData()). */
  virtual void lockAll( void );
    /*! Unlock the RELACSPlugin (via unlock()), the input data and events
        (via unlockData()), the meta data (via unlockMetaData()),
	and the stimulus data (via unlockStimulusData()). */
  virtual void unlockAll( void );

    /*! Return the list of all input traces.
        \sa trace(), traceIndex(), events(), traceInputTrace(),
	traceInputEvent(), eventInputTrace(), eventInputEvent() */
  const InList &traces( void ) const;
    /*! Return the input trace with index \a index.
        \sa traceIndex(), events(), traceInputTrace(), traceInputEvent(),
	eventInputTrace(), eventInputEvent() */
  const InData &trace( int index ) const;
    /*! Return the input trace with identifier \a ident.
        \sa traceIndex(), events(), traceInputTrace(), traceInputEvent(),
	eventInputTrace(), eventInputEvent() */
  const InData &trace( const string &ident ) const;
    /*! Return the index of the input trace with identifier \a ident.
        \sa events(), traceInputTrace(), traceInputEvent(), eventInputTrace(),
	eventInputEvent() */
  int traceIndex( const string &ident ) const;
    /*! Returns the names of all input traces separated by '|'.
        This string can be used for a text Parameter. */
  string traceNames( void ) const;

    /*! Return the list of all events.
        \sa traces(), traceIndex(), traceInputTrace(), traceInputEvent(),
	eventInputTrace(), eventInputEvent() */
  const EventList &events( void ) const;
    /*! Return the events with index \a index.
        \sa traces(), traceIndex(), traceInputTrace(), traceInputEvent(),
	eventInputTrace(), eventInputEvent() */
  const EventData &events( int index ) const;
    /*! Return the events with identifier \a ident.
        \sa traces(), traceIndex(), traceInputTrace(), traceInputEvent(),
	eventInputTrace(), eventInputEvent() */
  const EventData &events( const string &ident ) const;

    /*! Return the index of the input trace that was filtered
        to obtain input trace with index \a trace.
        \sa traces(), events(), traceInputTrace( const string & ),
	traceInputEvent(), eventInputTrace(), eventInputEvent() */
  int traceInputTrace( int trace ) const;
    /*! Return the index of the input trace that was filtered
        to obtain input trace with identifier \a ident.
        \sa traces(), events(), traceInputTrace( int ),
	traceInputEvent(), eventInputTrace(), eventInputEvent() */
  int traceInputTrace( const string &ident ) const;
    /*! Return the index of the EventData that was filtered
        to obtain input trace with index \a trace.
        \sa traces(), events(), traceInputEvent( const string & ),
	traceInputTrace(), eventInputTrace(), eventInputEvent() */
  int traceInputEvent( int trace ) const;
    /*! Return the index of the EventData that was filtered
        to obtain input trace with identifier \a ident.
        \sa traces(), events(), traceInputEvent( int ),
	traceInputTrace(), eventInputTrace(), eventInputEvent() */
  int traceInputEvent( const string &ident ) const;

    /*! Return the index of the input trace on which the 
        events of the EventData with index \a event were detected.
        \sa traces(), events(), eventInputTrace( const string & ),
	eventInputEvent(), traceInputTrace(), traceInputEvent() */
  int eventInputTrace( int event ) const;
    /*! Return the index of the input trace on which the 
        events of the EventData with identifier \a ident were detected.
        \sa traces(), events(), eventInputTrace( int ),
	eventInputEvent(), traceInputTrace(), traceInputEvent() */
  int eventInputTrace( const string &ident ) const;
    /*! Return the index of the EventData on which the 
        events of the EventData with index \a event were detected.
        \sa traces(), events(), eventInputEvent( const string & ),
	eventInputTrace(), traceInputTrace(), traceInputEvent() */
  int eventInputEvent( int event ) const;
    /*! Return the index of the EventData on which the 
        events of the EventData with identifier \a ident were detected.
        \sa traces(), events(), eventInputEvent( int ),
	eventInputTrace(), traceInputTrace(), traceInputEvent() */
  int eventInputEvent( const string &ident ) const;

    /*! \return the time of the last signal. */
  double signalTime( void ) const;


 protected:

    /*! Set the gain for input trace \a data to \a gainindex.
        It depends on your hardware what gain is corresponding to \a gainindex.
        In order to activate the new gain you have to call activateGains().
        \sa adjustGain( InData, double, double ), 
	adjustGain( InData, double ), activateGains() */
  void setGain( const InData &data, int gainindex );
    /*! Adjust the gain for input trace \a data 
        such that the maximum value of \a maxvalue is within the
        input range.
        In order to activate the new gain you have to call activateGains().
        \sa adjustGain( InData, double, double ), adjust(), activateGains() */
  void adjustGain( const InData &data, double maxvalue );
    /*! Increases or decreases the gain for input trace \a data
        by one step.
        If \a maxvalue is larger than the maximum possible value
        of the current input range times, then the gain is decreased.
        If \a minvalue is smaller than the maximum possible value
        of the following input range, then the gain is increased.
        In order to activate the new gain you have to call activateGains().
        \sa adjustGain( InData, double ), adjust(), activateGains() */
  void adjustGain( const InData &data, double minvalue, double maxvalue );
    /*! Adjust gain of analog input trace \a data by one step.
        The maximum absolute value of the trace \a data between
        time \a tbegin and time \a tend seconds is determined.
        If this value is greater than \a threshold times the
        maximum possible value of the current range the gain is decreased.
        If this value is smaller than \a threshold squared times the
        maximum possible value of the following range,
        then the gain is increased.
        In order to activate the new gain you have to call activateGains().
        \sa adjustGain( InData, double ), adjustGain( InData, double, double ), activateGains() */
  void adjust( const InData &data, double tbegin, double tend,
	       double threshold );
    /*! Adjust the gain of trace \a data for the last \a duration seconds.
        \sa adjust() */
  void adjust( const InData &data, double duration, double threshold );
    /*! Activates the new gain settings for analog input traces
        set by adjustGain().
        \sa adjustGain( InData, double ), adjustGain( InData, double, double ), adjust() */
  void activateGains( void );


 public:

    /*! Return the number of output traces. */
  int outTracesSize( void ) const;
    /*! Return the index of the output trace with name \a name.
        If there isn't any trace with name \a name, -1 is returned. */
  int outTraceIndex( const string &name ) const;
    /*! Return the name of the output trace with index \a index.
        If \a index is invalid, an empty string is returned. */
  string outTraceName( int index ) const;
    /*! Returns the names of all output traces separated by '|'.
        This string can be used for a text Parameter. */
  string outTraceNames( void ) const;
    /*! Return the output trace with index \a index. */
  const TraceSpec &outTrace( int index ) const;
    /*! Return the output trace with name \a name. */
  const TraceSpec &outTrace( const string &name ) const;
    /*! If \a signal has an output trace specified (via OutData::setTrace())
        then the corresponding settings (channel, device, ...)
        are applied to \a signal.
	\return 
	-  0: success (or empty trace name)
	- -1: invalid trace name */
  int applyOutTrace( OutData &signal ) const;
    /*! Applies the trace settings to each of the OutData in \a signals.
	\return
        -  0: success (or empty trace name)
	- -1: invalid trace name in at least one of the signals. */
  int applyOutTrace( OutList &signal ) const;

    /*! True if the current working mode is to
        acquiring real data from a data acquisition board.
        \sa simulation(), analysis(), idle(), modeStr() */
  bool acquisition( void ) const;
    /*! True if the current working mode is to
        simulate data using a Model. 
	\sa acquisition(), analysis(), idle(), modeStr() */
  bool simulation( void ) const;
    /*! True if the current working mode is to
        reanalyse previously recorded or simulated data.
	\sa acquisition(), simulation(), idle(), modeStr() */
  bool analysis( void ) const;
    /*! True if the current working mode is to
        nothing, i.e. to wait for a selection from the user. 
	\sa acquisition(), simulation(), analysis(), modeStr() */
  bool idle( void ) const;
    /*! Returns a string describing the current mode.
        \sa acquisition(), simulation(), analysis(), idle() */
  string modeStr( void ) const;

    /*! Return general settings of RELACS. \sa lockRelacsSettings() */
  Options &relacsSettings( void );
    /*! Return general settings of RELACS. \sa lockRelacsSettings() */
  const Options &relacsSettings( void ) const;
    /*! Lock the mutex for the general settings of RELACS.
        \sa unlockRelacsSettings(), relacsSettings() */
  void lockRelacsSettings( void ) const;
    /*! Unlock the mutex for the general settings of RELACS.
        \sa lockRelacsSettings(), relacsSettings() */
  void unlockRelacsSettings( void ) const;
    /*! The mutex for the general settings of RELACS. */
  QMutex *relacsSettingsMutex( void );

    /*! The path where all data of the current session are stored.
        \sa addPath(), defaultPath() */
  string path( void ) const;
    /*! Returns \a file added to the base path for the current session.
        \sa path(), defaultPath() */
  string addPath( const string &file ) const;

    /*! The default path where data are stored if no session is running.
        \sa path(), addDefaultPath() */
  string defaultPath( void ) const;
    /*! Returns \a file added to the default path.
        \sa defaultPath(), path() */
  string addDefaultPath( const string &file ) const;

    /*! \return \c true whenever voltage traes and eventlists are saved
        to disk via SaveFiles. */
  bool saving( void ) const;

    /*! The options that are stored with each stimulus in the trigger file. */
  Options &stimulusData( void );
    /*! The options that are stored with each stimulus in the trigger file. */
  const Options &stimulusData( void ) const;
    /*! Lock the stimulus data mutex. */
  void lockStimulusData( void ) const;
    /*! Unlock the stimulus data mutex. */
  void unlockStimulusData( void ) const;
    /*! The mutex for the stimulus data. */
  QMutex *stimulusDataMutex( void );

    /*! Return the MetaData options from section \a section.
        These options are stored in the info file of the session. */
  Options &metaData( const string &section );
    /*! Return the MetaData options from section \a section.
        These options are stored in the info file of the session. */
  const Options &metaData( const string &section ) const;
    /*! Return a reference to the MetaData instance that manages all 
        the meta data sections. */
  MetaData &metaData( void );
    /*! Return a const reference to the MetaData instance that manages all 
        the meta data sections. */
  const MetaData &metaData( void ) const;
    /*! Lock the meta data mutex. */
  void lockMetaData( void ) const;
    /*! Unlock the meta data mutex. */
  void unlockMetaData( void ) const;
    /*! The mutex for the meta data. */
  QMutex *metaDataMutex( void );

    /*! Return the complete device list. */
  AllDevices *devices( void ) const;
    /*! Return the device with identifier \a ident. */
  Device *device( const string &ident );
    /*! If you changed the settings of an ordinary device
        (not an AnalogInput, AnanlogOutput, Attenuator, Attenuate)
	call this function in order to update the settings displayed in
	the device menu. */
  void updateDeviceMenu( void );

    /*! Return the digital I/O device with identifier \a ident. */
  DigitalIO *digitalIO( const string &ident );

    /*! Return the attenuator for output trace \a index. */
  Attenuate *attenuator( int index );
    /*! Return the attenuator for output trace \a name. */
  Attenuate *attenuator( const string &name );

    /*! Return the filter with name \a name. */ 
  Filter *filter( const string &name );
    /*! Return the filter for trace \a index. */ 
  Filter *filterTrace( int index );
    /*! Return the filter for the trace \a name. */ 
  Filter *filterTrace( const string &name );
    /*! Return the options of the filter with name \a name. */
  Options &filterOpts( const string &name );
    /*! Return the options of the filter for trace \a index. */
  Options &filterTraceOpts( int index );
    /*! Return the options of the filter for trace \a name. */
  Options &filterTraceOpts( const string &name );
    /*! Lock the filter with name \a name. */ 
  void lockFilter( const string &name );
    /*! Lock the filter for trace \a index. */ 
  void lockFilterTrace( int index );
    /*! Lock the filter for trace \a name. */ 
  void lockFilterTrace( const string &name );
    /*! Unlock the filter with name \a name. */ 
  void unlockFilter( const string &name );
    /*! Unlock the filter for trace \a index. */ 
  void unlockFilterTrace( int index );
    /*! Unlock the filter for trace \a name. */ 
  void unlockFilterTrace( const string &name );

    /*! Return the detector with name \a name. */ 
  Filter *detector( const string &name );
    /*! Return the detector for event data \a index. */ 
  Filter *detectorEvents( int index );
    /*! Return the detector for event data \a name. */ 
  Filter *detectorEvents( const string &name );
    /*! Return the options of the detector with name \a name. */
  Options &detectorOpts( const string &name );
    /*! Return the options of the detector for event data \a index. */
  Options &detectorEventsOpts( int index );
    /*! Return the options of the detector for event data \a name. */
  Options &detectorEventsOpts( const string &name );
    /*! Lock the event detector with name \a name. */ 
  void lockDetector( const string &name );
    /*! Lock the event detector for event data \a index. */ 
  void lockDetectorEvents( int index );
    /*! Lock the event detector for event data \a name. */ 
  void lockDetectorEvents( const string &name );
    /*! Unlock the event detector with name \a name. */ 
  void unlockDetector( const string &name );
    /*! Unlock the event detector for event data \a index. */ 
  void unlockDetectorEvents( int index );
    /*! Unlock the event detector for event data \a name. */ 
  void unlockDetectorEvents( const string &name );

    /*! The elapsed time of the current session in seconds. */
  double sessionTime( void ) const;
    /*! A string displaying the elapsed time of the current session
        as specified by \a SessionTimeFormat. */
  string sessionTimeStr( void ) const;
    /*! True if a session is currently running. */
  bool sessionRunning( void ) const;


protected slots:

    /*! Start a new session, if it is not already running,
        and calls the startsession - macro.
	\note This function must not be called from a non GUI thread! */
  void startTheSession( void );
    /*! Stop a running session.
	\note This function must not be called from a non GUI thread! */
  void stopTheSession( void );
    /*! Toggles the status of the session:
        stops a running session or start a new session
	if no session is running.
	\note This function must not be called from a non GUI thread! */
  void toggleSession( void );


public:

    /*! This function is called whenever a new recording session is started.
        Implement this function to do, for example, some initialization. */
  virtual void sessionStarted( void );
    /*! This function is called whenever a recording session is stopped.
        Implement this function to do, for example,
	some initialization or reset.
        If the session was saved \a saved is set to \c true. */
  virtual void sessionStopped( bool saved );


protected:

    /*! Return the control with index \a index. */
  Control *control( int index );
    /*! Return the control with name \a name. */
  Control *control( const string &name );
    /*! Return the options of the control widget with index \a index. */
  Options &controlOpts( int index );
    /*! Return the options of the control widget with name \a name. */
  Options &controlOpts( const string &name );
    /*! Lock the control with index \a index. */ 
  void lockControl( int index );
    /*! Lock the control with name \a name. */ 
  void lockControl( const string &name );
    /*! Unlock the control with index \a index. */ 
  void unlockControl( int index );
    /*! Unlock the control with name \a name. */ 
  void unlockControl( const string &name );

    /*! Return the model. */
  Model *model( void );
    /*! Return the options of the model. */
  Options &modelOpts( void );
    /*! Lock the model mutex. */ 
  void lockModel( void );
    /*! Unlock the model mutex. */ 
  void unlockModel( void );

    /*! Return the list of RePros. */
  RePros *repros( void );
    /*! Return the additional options for the RePros dialog. */
  Options &reprosDialogOpts( void );

    /*! Return the RePro with index \a index. */
  RePro *repro( int index );
    /*! Return the RePro with name \a name. */
  RePro *repro( const string &name );
    /*! Return the options of the RePro with index \a index. */
  Options &reproOpts( int index );
    /*! Return the options of the RePro with name \a name. */
  Options &reproOpts( const string &name );
    /*! Lock the RePro with index \a index. */ 
  void lockRePro( int index );
    /*! Lock the RePro with name \a name. */ 
  void lockRePro( const string &name );
    /*! Unlock the RePro with index \a index. */ 
  void unlockRePro( int index );
    /*! Unlock the RePro with name \a name. */ 
  void unlockRePro( const string &name );
    /*! Return the currently running RePro. */
  RePro *currentRePro( void );
    /*! Return the options of the currently running RePro. */
  Options &currentReProOpts( void );
    /*! Lock the current RePro. */ 
  void lockCurrentRePro( void );
    /*! Unlock the current RePro. */ 
  void unlockCurrentRePro( void );

    /*! Pointer to the main RELACSWidget. */
  RELACSWidget *RW;


protected slots:

    /*! Called from warning(), info(), updateDeviceMenu().
        If you reimplement this event handler,
        don't forget to call this implementation
        via RELACSPlugin::customEvent(). */
  virtual void customEvent( QCustomEvent *qce );


private:

  string PluginSet;

  Options Settings;

    /*! Dummy trace. */
  static const TraceSpec DummyTrace;

  string WarningStr;
  double WarningTimeout;
  string InfoStr;
  double InfoTimeout;
  Options Dummy;

};


  /*! A macro to make the class \a deviceClass
      derived from the Device-class available as a plugin. */
#define addDevice( deviceClass ) \
addPlugin( deviceClass, RELACSPlugin::DeviceId )
  /*! A macro to make the class \a aiClass
      derived from the AnalogInput-class available as a plugin. */
#define addAnalogInput( aiClass ) \
addPlugin( aiClass, RELACSPlugin::AnalogInputId )
  /*! A macro to make the class \a aoClass
      derived from the AnalogOutput-class available as a plugin. */
#define addAnalogOutput( aoClass ) \
addPlugin( aoClass, RELACSPlugin::AnalogOutputId )
  /*! A macro to make the class \a dioClass
      derived from the DigitalIO-class available as a plugin. */
#define addDigitalIO( dioClass ) \
addPlugin( dioClass, RELACSPlugin::DigitalIOId )
  /*! A macro to make the class \a attenuatorClass
      derived from the Attenuator-class available as a plugin. */
#define addAttenuator( attenuatorClass ) \
addPlugin( attenuatorClass, RELACSPlugin::AttenuatorId )
  /*! A macro to make the class \a attenuateClass
      derived from the Attenuate-class available as a plugin. */
#define addAttenuate( attenuateClass ) \
addPlugin( attenuateClass, RELACSPlugin::AttenuateId )
  /*! A macro to make the class \a modelClass
      derived from the Model-class available as a plugin. */
#define addModel( modelClass ) \
addPlugin( modelClass, RELACSPlugin::ModelId )
  /*! A macro to make the class \a detectorClass
      derived from the Filter-class available as a plugin. */
#define addDetector( detectorClass ) \
addPlugin( detectorClass, RELACSPlugin::FilterId )
  /*! A macro to make the class \a filterClass
      derived from the Filter-class available as a plugin. */
#define addFilter( filterClass ) \
addPlugin( filterClass, RELACSPlugin::FilterId )
  /*! A macro to make the class \a controlClass
      derived from the Control-class available as a plugin. */
#define addControl( controlClass ) \
addPlugin( controlClass, RELACSPlugin::ControlId )
  /*! A macro to make the class \a reproClass
      derived from the RePro-class available as a plugin. */
#define addRePro( reproClass ) \
addPlugin( reproClass, RELACSPlugin::ReProId )


}; /* namespace relacs */

#endif /* ! _RELACS_RELACSPLUGIN_H_ */

