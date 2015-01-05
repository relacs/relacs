/*
  filterdetectors.h
  Container organizing filter and event detectors.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_FILTERDETECTORS_H_
#define _RELACS_FILTERDETECTORS_H_ 1

#include <string>
#include <deque>
#include <vector>
#include <QMutex>
#include <QMenu>
#include <relacs/configclass.h>
#include <relacs/inlist.h>
#include <relacs/eventlist.h>
#include <relacs/plugintabs.h>
#include <relacs/plottrace.h>

namespace relacs {


class RePros;
class Session;
class Filter;
class Devices;
class AttInterfaces;
class RELACSWidget;

/*!
\class FilterDetectors
\author Jan Benda
\version 1.0
\brief Container organizing filter and event detectors.
*/


class FilterDetectors : public PluginTabs, public ConfigClass
{
  Q_OBJECT

public:

  FilterDetectors( RELACSWidget *rw, QWidget *parent=0 );
  ~FilterDetectors( void );

  void clear( void );
  void clearIndices( void );

    /*! Copies pointers of each element of \a il and \a el to each Filter. */
  void setTracesEvents( const InList &il, const EventList &el );

    /*! Load options from config file. */
  virtual void readConfig( StrQueue &sq );

    /*! Add filters / detectors to this according to
        the configuration. */
  string createFilters( void );

    /*! Number of output traces. */
  int traces( void );
    /*! Number of output events. */
  int events( void );

    /*! Further initialize the last event in \a events as stimulus events. */
  void createStimulusEvents( EventList &events, deque< PlotTrace::EventStyle > &eventstyles );
    /*! Further initialize the last event in \a events as restart events. */
  void createRestartEvents( EventList &events, deque< PlotTrace::EventStyle > &eventstyles );
    /*! Initialize file recording events. */
  void createRecordingEvents( InList &data, EventList &events,
			      deque< PlotTrace::EventStyle > &eventstyles );
    /*! Initialize the traces and events and assign them to the Filter. */
  string createTracesEvents( InList &data, EventList &events,
			     deque< PlotTrace::TraceStyle > &tracestyles,
			     deque< PlotTrace::EventStyle > &eventstyles );

    /*! Initialize the Filter.
        \return in case of errors (init() not implemented)
	an appropriate message. */
  string init( void );

    /*! Set the flag which is used to mark traces whose gain was changed to \a flag. 
        \sa scheduleAdjust(), adjust() */
  void setAdjustFlag( int flag );
    /*! Notice input signal gain changes so that adjust() is called
        on the next invokation of filter(). */
  void scheduleAdjust( void );
    /*! React to input signal gain changes.
        Calls the adjust() function of each filter/detector if the gain of its
        input data was adjusted. \sa scheduleAdjust() */
  void adjust( void );

    /*! Call the Filter::autoConfigure() function for all filter
        on the last \a duration seconds of data. */
  void autoConfigure( double duration );
    /*! Call the Filter::autoConfigure() function for all filter
        on the time range from \a tbegin to \a tend . */
  void autoConfigure( double tbegin, double tend );
    /*! Call the Filter::autoConfigure() function for the filter \a f
        on the last \a duration seconds of data. */
  void autoConfigure( Filter *f, double duration );
    /*! Call the Filter::autoConfigure() function for the filter \a f
        on the time range from \a tbegin to \a tend. */
  void autoConfigure( Filter *f, double tbegin, double tend );

    /*! Filter or detect events. The Filter is initialized at its first call.
        \param[in] signaltime this signaltime is set in the derived data.
        \return in case of errors (filter() not implemented)
	an appropriate message. */
  string filter( double signaltime );

    /*! Return filter of the \a index trace in an InList. */
  Filter *filter( int index );
    /*! Return filter with identifier \a ident. */
  Filter *filter( const string &ident );
    /*! Return detector of the \a index trace in an EventList. */
  Filter *detector( int index );
    /*! Return detector with identifier \a ident. */
  Filter *detector( const string &ident );
    /*! \c true if filter or event detector with identifier \a ident exist. */
  bool exist( const string &ident ) const;

    /*! Return the index of the input trace that was filtered
        to obtain input trace with index \a trace. */
  int traceInputTrace( int trace ) const;
    /*! Return the index of the input trace that was filtered
        to obtain input trace with identifier \a ident. */
  int traceInputTrace( const string &ident ) const;
    /*! Return the index of the EventData that was filtered
        to obtain input trace with index \a trace. */
  int traceInputEvent( int trace ) const;
    /*! Return the index of the EventData that was filtered
        to obtain input trace with identifier \a ident. */
  int traceInputEvent( const string &ident ) const;

    /*! Return the index of the input trace on which the
        events of the EventData with index \a event were detected. */
  int eventInputTrace( int event ) const;
    /*! Return the index of the input trace on which the
        events of the EventData with identifier \a ident were detected. */
  int eventInputTrace( const string &ident ) const;
    /*! Return the index of the EventData on which the
        events of the EventData with index \a event were detected. */
  int eventInputEvent( int event ) const;
    /*! Return the index of the EventData on which the
        events of the EventData with identifier \a ident were detected. */
  int eventInputEvent( const string &ident ) const;

    /*! Add the menu for configuring %FilterDetectors to \a menu. */
  void addMenu( QMenu *menu, bool doxydoc );

    /*! Calles modeChanged() of each Filter
        whenever the mode is changed. */
  void modeChanged( void );
    /*! Inform each Filter that some stimulus data have been changed. */
  void notifyStimulusData( void );
    /*! Inform each Filter that some meta data have been changed. */
  void notifyMetaData( void );
    /*! Inform each Filter that a new session is started. */
  void sessionStarted( void );
    /*! Inform each Filter that the session is stopped. */
  void sessionStopped( bool saved );

  friend ostream &operator<<( ostream &str, const FilterDetectors &DT );


protected:

  void keyPressEvent( QKeyEvent *event );
  void keyReleaseEvent( QKeyEvent *event );


private:

  class FilterData
  {
  public:
    FilterData( Filter *filter, const string &pluginname,
		const vector<string> &in,
		const vector<string> &other,
		long n, bool size, bool width );
    FilterData( const FilterData &fd );
    ~FilterData();

    void print( ostream &str ) const;

    Filter *FilterDetector;
    string PluginName;
    int Out;
    int NOut;
    vector<string> In;
    vector<string> Other;
    InList InTraces;
    EventList InEvents;
    InList OutTraces;
    EventList OutEvents;
    EventList OtherEvents;
    long NBuffer;
    bool SizeBuffer;
    bool WidthBuffer;
    bool Init;
  };

  typedef vector<FilterData> FilterList;

  FilterList FL;

    /*! Pointer to the events marking daq board restarts. */
  EventData *RestartEvents;

    /*! Maps each InData to an InData. */
  vector<int> TraceInputTrace;
    /*! Maps each InData to an EventData. */
  vector<int> TraceInputEvent;
    /*! Maps each EventData to an InData. */
  vector<int> EventInputTrace;
    /*! Maps each EventData to an EventData. */
  vector<int> EventInputEvent;

  QMenu *Menu;

  bool NeedAdjust;
  int AdjustFlag;
  QMutex AdjustMutex;

};


}; /* namespace relacs */

#endif /* ! _RELACS_FILTERDETECTORS_H_ */

