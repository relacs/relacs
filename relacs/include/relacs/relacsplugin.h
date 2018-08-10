/*
  relacsplugin.h
  Adds specific functions for RELACS plugins to ConfigDialog

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

#ifndef _RELACS_RELACSPLUGIN_H_
#define _RELACS_RELACSPLUGIN_H_ 1


#include <QKeyEvent>
#include <QApplication>
#include <QReadWriteLock>
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

The current mode() of RELACS can be acquisition(), simulation(),
analysis(), or idle().  If the mode is changed, modeChanged() is
called that you can reimplement according to your needs.

Whenever a recording session is started or stopped,
sessionStarted() or sessionStopped() are called, respectively.
You can implement these virtual functions according to your needs.
Other functions related to sessions are sessionData(), sessionTime(),
sessionTimeStr(), sessionRunning(), reproCount().

The meta data sections of the current recording can be accessed by
metaData(). Don't forget to lock every acces to metaData() with
lockMetaData() and unlockMetaData().  Whenever some of the meta data
are set to new values, the virtual function notifyMetaData() is
called, that you can implement according to your needs.

Some stimulation related information is stored in stimulusData().
Don't forget to lock every acces to stimulusData() with
lockStimulusData() and unlockStimulusData().  Whenever some of the stimulus data
are set to new values, the virtual function notifyStimulusData() is
called, that you can implement according to your needs.

All other RELACS plugins can be accessed:
devices(), device(), attenuator(), digitalIO(), trigger(), filter(), filterOpts(),
autoConfigureFilter(), detector(), detectorOpts(), autoConfigureDetector(),
control(), controlOpts(),
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

Use update() or postCustomEvent() for thread save drawing 
and other interactions with the GUI.

For thread-save changes of the mouse cursor indicating some busy action use
setWaitMouseCursor() and restoreMouseCursor().

You can handle key press and release events of a RELACSPlugin that has a widget
by reimplementing keyPressEvent() and keyReleaseEvent().
Usually, the events are only delivered, if the corresponding widget() is
visible. If you want he key event handlers to be called irrespective
of the widgets visibility, then call setGlobalKeyEvents().

Some integers for identifying the type of the RELACS plugin are defined
and are used by the addDevice, addAttenuate, addAttuator,
addAnalogInput, addAnalogOutput, addDigitalIO, addTrigger,
addModel, addFilter, addDetector, addControl, and addRePro macros.

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
class Trigger;
class Attenuate;
class AttInterfaces;
class RELACSWidget;


class RELACSPlugin : public ConfigDialog
{
  Q_OBJECT

public:

    /*! The identifier for plugins derived from the Device-class. */
  static const int DeviceId = 0x0001;
    /*! The identifier for plugins derived from the AnalogInput-class. */
  static const int AnalogInputId = 0x0002;
    /*! The identifier for plugins derived from the AnalogOutput-class. */
  static const int AnalogOutputId = 0x0004;
    /*! The identifier for plugins derived from the AnalogOutput-class. */
  static const int DigitalIOId = 0x0008;
    /*! The identifier for plugins derived from the Trigger-class. */
  static const int TriggerId = 0x0010;
    /*! The identifier for plugins derived from the Attenuator-class. */
  static const int AttenuatorId = 0x0020;
    /*! The identifier for plugins derived from the Attenuate-class. */
  static const int AttenuateId = 0x0040;
    /*! The identifier for plugins derived from the Model-class. */
  static const int ModelId = 0x0080;
    /*! The identifier for plugins derived from the Filter-class. */
  static const int FilterId = 0x0100;
    /*! The identifier for plugins derived from the RePro-class. */
  static const int ReProId = 0x0200;
    /*! The identifier for plugins derived from the Control-class. */
  static const int ControlId = 0x0400;

    /*! Config-File group for RELACS core classes (=0). */
  static const int Core;
    /*! Config-File group for alr RELACS plugins, i.e.
        RePro, Filter, Control, Model, ... classes (=1). */
  static const int Plugins;

    /*! Construct an RELACSPlugin.
        The identifier \a configident is used for identifying this class
	in the configuration file of group \a configgroup.
        \a name has to be exactly the name of the class.
	The class belongs to the set of plugins named \a pluginset.
	The implementation of a class derived from %RELACSPlugin
	has a \a version and was written by \a author on \a date.
        \sa setConfigIdent(), setConfigGroup(),
	setName(), setTitle(), setAuthor(), setDate(), setVersion() */
  RELACSPlugin( const string &configident="", int configgroup=0,
		const string &name="", 
		const string &pluginset="",
		const string &author="unknown",
		const string &version="unknown",
		const string &date=__DATE__ );
    /*! Destruct the RELACSPlugin. */
  virtual ~RELACSPlugin( void );

    /*! \return the widget of this plugin., or NULL if it does not have one.
        \sa setWidget(), setLayout() */
  QWidget *widget( void );
    /*! Declare \a widget as the main widget of this class.
        \note call this function only once in the constructor.
        \sa widget(), setLayout() */
  void setWidget( QWidget *widget );
    /*! Declare \a layout as the main layout of this class.
        A container widget for the layout is created automatically.
        \note call this function only once in the constructor.
        \sa widget(), setWidget() */
  void setLayout( QLayout *layout );
     /*! The width in pixels of the desktop. \sa desktopHeight() */
  int desktopWidth( void ) const;
     /*! The height in pixels of the desktop. \sa desktopWidth() */
  int desktopHeight( void ) const;

    /*! Set the name of the RELACSPlugin to \a name.
        Also calls updateUniqueName(). */
  virtual void setName( const string &name );
    /*! The name of the plugin set the class belongs to. */
  string pluginSet( void ) const;
    /*! A unique name of the class (either "name()" or "name()[pluginSet()]" ). */
  string uniqueName( void ) const;
    /*! Set the unique name of the class to its name() (default). */
  void setShortUniqueName( void );
    /*! Set the unique name of the class to "name()[pluginSet()]". */
  void setLongUniqueName( void );
    /*! Updat the unique name of the class to a new name(). */
  void updateUniqueName( void );

    /*! \return the plugin's options with the values at the time where
        the RELACSPlugin was started (a RePro whenever it was executed,
        all other plugins when an acquisition was started).
	The settings name is set to "Settings", its type is empty.
	\sa setSettings() */
  const Options &settings( void ) const;
    /*! \return the plugin's options with the values at the time where
        the RELACSPlugin was started (a RePro whenever it was executed,
        all other plugins when an acquisition was started).
	The settings name is set to "Settings", its type is empty.
	\sa setSettings() */
  Options &settings( void );
    /*! Copy the plugin's current Options to its settings(). */
  void setSettings( void );

    /*! Called whenever the acquisition mode is changed. 
        The new mode() of RELACS can be acquisition(), simulation(), analysis(), or idle().
	Reimplement this functions according to your needs. */
  virtual void modeChanged( void );
    /*! This function is called whenever some values of meta data have
        been changed.  Implement this function if the plugin needs to
        react to this.  The meta data mutex is already locked when
        this function is entered. */
  virtual void notifyMetaData( void );
    /*! This function is called whenever some values of the stimulus
        data have been changed.  Implement this function if the plugin
        needs to react to this.  The stimulus data mutex is already
        locked when this function is entered. */
  virtual void notifyStimulusData( void );

    /*! Pass the %RELACSWidget \a rw to the RELACSPlugin.  This is
        used for accessing Devices, AttInterfaces, Filters, Control,
        and RePros. */
  void setRELACSWidget( RELACSWidget *rw );

    /*! \return the name of the file to be displayed in the help dialog.
        This implementation returns pluginSet() + '-' + name() + \c ".html". */
  virtual string helpFileName( void ) const;

    /*! Path to relacs icons (usually in \c /usr/share/relacs). */
  string iconPath( void ) const;
    /*! Path to the relacs documentation. */
  string docPath( void ) const;


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
	Can be called directly from a non GUI thread. */
  void warning( const string &s, double timeout=0.0 );
    /*! Opens an info window with the message \a s.
        The message together with the current time and the name of the plugin
	is also written to standard error.
        If \a timeout is greater than zero, the message window is
        closed automatically after \a timeout seconds.
	Can be called directly from a non GUI thread. */
  void info( const string &s, double timeout=0.0 );

    /*! Post a custom event for thread save manipulations of the GUI.
        This is just a shortcut for
	\code
	QApplication::postEvent( this, new QCustomEvent( QEvent::Type( QEvent::User+type ) ) );
	\endcode
	For your own events use values greater than 10 for \a type.
	To handle posted events, reimplement customEvent( QEvent* ).
	Don't forget to call RELACSPlugin::customEvent() from your reimplemented customEvent().
	If, for example, you want to hide a widget, you call
        \code
	postCustomEvent( 10 );
	\endcode
	Then you reimplement customEvent() like this:
	\code
	void MyPlugin::customEvent( QEvent *qce )
	{
  	  if ( qce->type() == QEvent::User+10 ) {
	     // hide the widget here
 	  }
	  else
	    RELACSPlugin::customEvent( qce );
	}
	\endcode
        \sa customEvent() */
  void postCustomEvent( int type=0 );


 public:

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
    /*! Returns the names of all input traces (raw traces and filtered traces) separated by '|'.
        This string can be used for a text Parameter. For example, to define an option
        that selects an input trace:
	\code
MyPlugin::MyPlugin( void )
{
  addSelection( "intrace", "Input trace", "V-1" );
  ...
}

void MyPlugin::preConfig( void )
{
  setText( "intrace", traceNames() );
  setToDefault( "intrace" );
  ...
}


int MyPlugin::main( void )
{
  int intrace = index( "intrace" );
  ...
}
\endcode
    */
  string traceNames( void ) const;
    /*! Returns the names of all raw input traces (with their source() == 0) separated by '|'.
        This string can be used for a text Parameter. For example, to define an option
        that selects an input trace:
	\code
MyPlugin::MyPlugin( void )
{
  addSelection( "intrace", "Input trace", "V-1" );
  ...
}

void MyPlugin::preConfig( void )
{
  setText( "intrace", rawTraceNames() );
  setToDefault( "intrace" );
  ...
}


int MyPlugin::main( void )
{
  int intrace = traceIndex( text( "intrace" ) );
  ...
}
\endcode */
  string rawTraceNames( void ) const;

    /*! \return the list of all events.
        \sa traces(), traceIndex(), traceInputTrace(), traceInputEvent(),
	eventInputTrace(), eventInputEvent() */
  const EventList &events( void ) const;
    /*! \return the events with index \a index.
        \sa traces(), traceIndex(), traceInputTrace(), traceInputEvent(),
	eventInputTrace(), eventInputEvent() */
  const EventData &events( int index ) const;
    /*! \return the events with identifier \a ident.
        \sa traces(), traceIndex(), traceInputTrace(), traceInputEvent(),
	eventInputTrace(), eventInputEvent() */
  const EventData &events( const string &ident ) const;
    /*! \return the times where stimuli started. 
        \sa restartEvents(), recordingEvents(), events() */
  const EventData &stimulusEvents( void ) const;
    /*! \return the times where the acquisition was restarted. 
        \sa stimulusEvents(), recordingEvents(), events() */
  const EventData &restartEvents( void ) const;
    /*! \return the times where the recordings were started. 
        \sa stimulusEvents(), restartEvents(), events() */
  const EventData &recordingEvents( void ) const;
    /*! Returns the names of all event traces separated by '|'. */
  string eventNames( void ) const;

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
    /*! \return the current recording time of the input buffers. */
  double currentTime( void ) const;
    /*! \return the smallest recording time of all raw traces (source == 0). */
  double currentTimeRaw( void ) const;

    /*! Add the local copies of the data buffers to \a data and \a events. */
  void addTracesEvents( deque<InList*> &data, deque<EventList*> &events );
    /*! Copies pointers to each element of \a data and \a events to this. */
  void setTracesEvents( const InList &data, const EventList &event );
    /*! Copies \a data and \a events to this by copying a pointer to the data buffers only. */
  void assignTracesEvents( const InList &data, const EventList &events );
    /*! Copies again all settings and indices from the reference traces and events to this. */
  void assignTracesEvents( void );
    /*! Updates the indices of derived traces and events (traces with source != 0 ). */
  void updateDerivedTracesEvents( void );
    /*! Make current trace and event data available to this.
	If \a mintracetime is greater than zero getData() waits until the input
        traces of the currently running acquisition contain a minimum
        number of data elements.  Returns immediately in case of
        errors or the acquisition was stopped.
	\param[in] mintracetime If \a mintracetime is greater than zero,
	blocks until data upto \a mintracetime seconds are available.
	\param[in] prevsignal If in addition \a prevsignal is greater than zero,
	first block until the time of the last signal is greater than \a prevsignal
	and afterwards until data until the signal time plus \a mintracetime are available.
        \return \c 1 if the input traces contain the required data,
	\c 0 if interrupted, or \c -1 on error. */
  int getData( double mintracetime=0.0, double prevsignal=-1000.0 );


 protected:

    /*! The list of input ranges the DAQ board supports for the input trace \a data. 
        \param[in] data: the input trace that specifies the input device, channel,
	and unipolar/bipolar mode.
	\param[out] ranges: the list of available ranges specified as the maximum
	voltage that can beacquired. The corresponding index can be passed directly to setGain().
	On failure an empty list is returned.
	\return 0 in succes, otherwise an DAQError code. */
  int maxVoltages( const InData &data, vector<double> &ranges ) const;
    /*! The list of input ranges the DAQ board supports for the input trace \a data. 
        \param[in] data: the input trace that specifies the input device, channel,
	and unipolar/bipolar mode.
	\param[out] ranges: the list of available ranges specified as the maximum
	values measured in data.unit(). The corresponding index can be passed directly to setGain().
	On failure an empty list is returned.
	\return 0 in succes, otherwise an DAQError code. */
  int maxValues( const InData &data, vector<double> &ranges ) const;

    /*! Set the gain for input trace \a data to \a gainindex.
        The smallest possible gain is indexed with \a gainindex = 0.
	Higher values for \a gainindex correspond to increasing gains.
	The current value of the gain index ca be retrieved directly from \a data via
	data.gainIndex().
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
        activateGains() assumes the data traces and events not to be locked
	and will write-lock the raw data.
	\return -1 on failure.
        \sa adjustGain( InData, double ), adjustGain( InData, double, double ), adjust() */
  int activateGains( void );


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
        This string can be used for a text Parameter. For example, to define an option
        that selects an output trace:
	\code
MyPlugin::MyPlugin( void )
{
  addSelection( "outtrace", "Output trace", "V-1" );
  ...
}

void MyPlugin::preConfig( void )
{
  setText( "outtrace", outTraceNames() );
  setToDefault( "outtrace" );
  ...
}


int MyPlugin::main( void )
{
  int outtrace = index( "outtrace" );
  ...
}
\endcode */
  string outTraceNames( void ) const;
    /*! Return the output trace with index \a index. */
  const TraceSpec &outTrace( int index ) const;
    /*! Return the output trace with name \a name. */
  const TraceSpec &outTrace( const string &name ) const;

    /*! Set the delay in seconds it takes from starting analog output 
        to the actual signal start for all channels of the
	analog output device \a device to \a delay. */
  void setSignalDelay( int device, double delay );

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

    /*! The options that are stored with each stimulus in the trigger file.
        \note You have to lock and unlock usage of stimulusData() by 
	calling lockStimulusData() and unlockStimulusData(). 
        \sa stimulusDataTraceFlag(), stimulusDataMutex() */
  Options &stimulusData( void );
    /*! The options that are stored with each stimulus in the trigger file.
        \note You have to lock and unlock usage of stimulusData() by 
	calling lockStimulusData() and unlockStimulusData(). 
        \sa stimulusDataTraceFlag(), stimulusDataMutex() */
  const Options &stimulusData( void ) const;
    /*! Flag that marks output traces in stimulusData().
        \sa stimulusData(), lockStimulusData() */
  int stimulusDataTraceFlag( void ) const;
    /*! Lock the stimulus data mutex.
        \sa unlockStimulusData(), stimulusDataMutex(), stimulusData() */
  void lockStimulusData( void ) const;
    /*! Unlock the stimulus data mutex.
        \sa lockStimulusData(), stimulusDataMutex(), stimulusData() */
  void unlockStimulusData( void ) const;
    /*! The mutex for the stimulus data.
        \sa lockStimulusData(), stimulusData() */
  QMutex *stimulusDataMutex( void );

    /*! Return a reference to the MetaData instance that manages all
        the meta data sections. MetaData is an Options, therefore, for
        example, you can retireve values from the meta data using
        something like \c metaData().number( "xyz" );
        \note You have to lock and unlock usage of metaData() by 
	calling lockMetaData(), unlockMetaData(). 
        \sa metaDataMutex() */
  MetaData &metaData( void );
    /*! Return a const reference to the MetaData instance that manages all 
        the meta data sections. MetaData is an Options, therefore, for
        example, you can retireve values from the meta data using
        something like \c metaData().number( "xyz" );
        \note You have to lock and unlock usage of metaData() by 
	calling lockMetaData(), unlockMetaData(). 
        \sa metaDataMutex() */
  const MetaData &metaData( void ) const;
    /*! Lock the meta data mutex. \sa unlockMetaData(), metaDataMutex() */
  void lockMetaData( void ) const;
    /*! Unlock the meta data mutex. \sa lockMetaData(), metaDataMutex() */
  void unlockMetaData( void ) const;
    /*! The mutex for the meta data. \sa lockMetaData(), unlockMetaData() */
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
    /*! Return the trigger device with identifier \a ident. */
  Trigger *trigger( const string &ident );

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
    /*! Auto-configure the parameter of the filter \a filter
        on the time range from \a tbegin to \a tend. */
  void autoConfigureFilter( Filter *filter, double tbegin, double tend );
    /*! Auto-configure the parameter of the filter \a filter
        on the last \a duration seconds of data. */
  void autoConfigureFilter( Filter *filter, double duration );
    /*! Auto-configure the parameter of all filter and detectors
        on the time range from \a tbegin to \a tend.
        Same function as autoConfigureDetectors( double, double ) */
  void autoConfigureFilter( double tbegin, double tend );
    /*! Auto-configure the parameter of the all filter and detectors
        on the last \a duration seconds of data.
        Same function as autoConfigureDetectors( double ) */
  void autoConfigureFilter( double duration );

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
    /*! Auto-configure the parameter of the detector \a detector
        on the time range from \a tbegin to \a tend. */
  void autoConfigureDetector( Filter *detector, double tbegin, double tend );
    /*! Auto-configure the parameter of the detector \a detector
        on the last \a duration seconds of data. */
  void autoConfigureDetector( Filter *detector, double duration );
    /*! Auto-configure the parameter of all filter and detectors
        on the time range from \a tbegin to \a tend.
        Same function as autoConfigureFilter( double, double ) */
  void autoConfigureDetectors( double tbegin, double tend );
    /*! Auto-configure the parameter of the all filter and detectors
        on the last \a duration seconds of data.
        Same function as autoConfigureFilter( double ) */
  void autoConfigureDetectors( double duration );

    /*! The elapsed time of the current session in seconds. */
  double sessionTime( void ) const;
    /*! A string displaying the elapsed time of the current session
        as specified by \a SessionTimeFormat. */
  string sessionTimeStr( void ) const;
    /*! True if a session is currently running. */
  bool sessionRunning( void ) const;
    /*! The number of so far executed RePros of the running session. */
  int reproCount( void ) const;


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

    /*! Move the focus back to the top level window.
        Call this function, for example, for a button that wa clicked,
	by connecting removeFocus() to the button's clicked() signal. */
  void removeFocus( void );


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

    /*! Reimplement this function in case you need 
        to handle key-press events.
        \note this RELACSPlugin needs to have a widget,
	set by setWidget() or setLayout(), in order 
        to have keyPressEvent() called. */
  virtual void keyPressEvent( QKeyEvent *event );
    /*! Reimplement this function in case you need 
        to handle key-release events.
        \note this RELACSPlugin needs to have a widget,
	set by setWidget() or setLayout(), in order 
        to have keyPressEvent() called. */
  virtual void keyReleaseEvent( QKeyEvent *event );
    /*! This function calls keyPressEvent() and keyReleaseEvent()
        by listening to the events the widget() is receiving. */
  virtual bool eventFilter( QObject *obj, QEvent *event );

    /*! Set the mouse cursor of the application for indicating busy activity
        in a thread-safe way.
        \sa restoreMouseCursor() */
  void setWaitMouseCursor( void );
    /*! Restore the mouse cursor of the application
        in a thread-safe way.
        \sa setWaitMouseCursor() */
  void restoreMouseCursor( void );


public:

    /*! \return \c true if the key event handlers should be called
        irrespective of the visibility of the corresponding widget.
	\sa setGlobalKeyEvents(), keyPressEvent(), keyReleaseEvent(), setWidget() */
  bool globalKeyEvents( void );
    /*! Set \a global to \c true in order to make the key event handlers to
        be called irrespective of the visibility of the corresponding widget().
	\sa globalKeyEvents(), keyPressEvent(), keyReleaseEvent(), setWidget() */
  void setGlobalKeyEvents( bool global=true );


public slots:

    /*! Write the plugin's options to standard output in a format that can
        directly be used for the doxygen documentation of the plugin. */
  virtual void saveDoxygenOptions( void );
    /*! Save the plugin's widget as an png file to the 
        current working directory.
        The name of the png file is the plugin's name() in lowercase. */
  virtual void saveWidget( void );


protected slots:

    /*! Mute the audio monitor. */
  void muteAudioMonitor( void );
    /*! Unmute the audio monitor. */
  void unmuteAudioMonitor( void );

    /*! Called from warning(), info(), updateDeviceMenu().
        If you reimplement this event handler,
        don't forget to call this implementation
        via RELACSPlugin::customEvent().
        \sa postCustomEvent() */
  virtual void customEvent( QEvent *qce );

    /*! Informs the plugin that the associated widget is going
        to be destroyed. */
  void widgetDestroyed( QObject *obj );


private:

    /*! The local copy of all input traces. */
  InList IData;
    /*! The local copy of all event traces. */
  EventList EData;
    /*! The local copy of the current signal time. */
  double SignalTime;

  string PluginSet;
  string UniqueName;

  Options Settings;

  bool GlobalKeyEvents;
  QWidget *Widget;

    /*! Dummy trace. */
  static const TraceSpec DummyTrace;

  Options Dummy;

};


class RelacsPluginEvent : public QEvent
{

public:

 RelacsPluginEvent( int type, const string &text, double timeout=0.0 )
   : QEvent( QEvent::Type( QEvent::User+type ) ), Text( text ), TimeOut( timeout ) {};

  string text( void ) const { return Text; };
  double timeOut( void ) const { return TimeOut; };


private:

  string Text;
  double TimeOut;

};


  /*! A macro to make the class \a deviceClass
      derived from the Device-class available as a plugin. */
#define addDevice( deviceClass, pluginSet )			\
addPlugin( deviceClass, pluginSet, RELACSPlugin::DeviceId )
  /*! A macro to make the class \a aiClass
      derived from the AnalogInput-class available as a plugin. */
#define addAnalogInput( aiClass, pluginSet ) \
addPlugin( aiClass, pluginSet, RELACSPlugin::AnalogInputId )
  /*! A macro to make the class \a aoClass
      derived from the AnalogOutput-class available as a plugin. */
#define addAnalogOutput( aoClass, pluginSet ) \
addPlugin( aoClass, pluginSet, RELACSPlugin::AnalogOutputId )
  /*! A macro to make the class \a dioClass
      derived from the DigitalIO-class available as a plugin. */
#define addDigitalIO( dioClass, pluginSet ) \
addPlugin( dioClass, pluginSet, RELACSPlugin::DigitalIOId )
  /*! A macro to make the class \a triggerClass
      derived from the Trigger-class available as a plugin. */
#define addTrigger( triggerClass, pluginSet ) \
addPlugin( triggerClass, pluginSet, RELACSPlugin::TriggerId )
  /*! A macro to make the class \a attenuatorClass
      derived from the Attenuator-class available as a plugin. */
#define addAttenuator( attenuatorClass, pluginSet ) \
addPlugin( attenuatorClass, pluginSet, RELACSPlugin::AttenuatorId )
  /*! A macro to make the class \a attenuateClass
      derived from the Attenuate-class available as a plugin. */
#define addAttenuate( attenuateClass, pluginSet ) \
addPlugin( attenuateClass, pluginSet, RELACSPlugin::AttenuateId )
  /*! A macro to make the class \a modelClass
      derived from the Model-class available as a plugin. */
#define addModel( modelClass, pluginSet ) \
addPlugin( modelClass, pluginSet, RELACSPlugin::ModelId )
  /*! A macro to make the class \a detectorClass
      derived from the Filter-class available as a plugin. */
#define addDetector( detectorClass, pluginSet ) \
addPlugin( detectorClass, pluginSet, RELACSPlugin::FilterId )
  /*! A macro to make the class \a filterClass
      derived from the Filter-class available as a plugin. */
#define addFilter( filterClass, pluginSet ) \
addPlugin( filterClass, pluginSet, RELACSPlugin::FilterId )
  /*! A macro to make the class \a controlClass
      derived from the Control-class available as a plugin. */
#define addControl( controlClass, pluginSet ) \
addPlugin( controlClass, pluginSet, RELACSPlugin::ControlId )
  /*! A macro to make the class \a reproClass
      derived from the RePro-class available as a plugin. */
#define addRePro( reproClass, pluginSet ) \
addPlugin( reproClass, pluginSet, RELACSPlugin::ReProId )


}; /* namespace relacs */

#endif /* ! _RELACS_RELACSPLUGIN_H_ */

