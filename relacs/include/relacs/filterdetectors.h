/*
  ../include/relacs/filterdetectors.h
  Container organizing filter and event detectors.

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

#ifndef _FILTERDETECTORS_H_
#define _FILTERDETECTORS_H_

#include <string>
#include <vector>
#include <qtabwidget.h>
#include <qpopupmenu.h> 
#include <relacs/configclass.h>
#include <relacs/inlist.h>
#include <relacs/eventlist.h>
#include "plottrace.h"

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


class FilterDetectors : public QTabWidget, public ConfigClass
{
  Q_OBJECT

public:

  FilterDetectors( RELACSWidget *rw, QWidget *parent=0, const char *name=0 );
  ~FilterDetectors( void );

  void clear( void );
  void clearIndices( void );

    /*! Load options from config file. */
  virtual void readConfig( StrQueue &sq );

    /*! Add filters / detectors to the ist according to 
        the configuration. */
  string createFilters( void );

    /*! Number of output traces. */
  int traces( void );
    /*! Number of output events. */
  int events( void );

    /*! Initialize stimulus events. */
  void createStimulusEvents( InList &data, EventList &events,
			     vector< PlotTrace::EventStyle > &eventstyles );
    /*! Initialize restart  events. */
  void createRestartEvents( InList &data, EventList &events,
			    vector< PlotTrace::EventStyle > &eventstyles );
    /*! Initialize the traces and events and assign them to the Filter. */
  string createTracesEvents( InList &data, EventList &events,
			     vector< PlotTrace::TraceStyle > &tracestyles,
			     vector< PlotTrace::EventStyle > &eventstyles );

    /*! Initialize the Filter. */
  void init( const InList &data, EventList &events );
    /*! Notice input signal gain changes so that adjust() is called
        on the next invokation of filter(). */
  void adjust( const InList &data, const EventList &events, int flag );
    /*! React to input signal gain changes.
        Calls the adjust() function of each filter/detector if the gain of its
        input data was adjusted. */
  void adjust( const InList &data, const EventList &events );
    /*! Filter or detect events. The Filter is initialized at its first call. */
  void filter( const InList &data, EventList &events );

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

    /*! The popup menu for configuring %FilterDetectors. */
  QPopupMenu* menu( void );

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

  void keyPressEvent( QKeyEvent *e );


private:

  class FilterData
  {
  public:
    FilterData( Filter *filter, const vector<string> &in, 
		const vector<string> &other,
		long n, bool size, bool width );
    FilterData( const FilterData &fd );
    ~FilterData();

    void print( ostream &str ) const;

    Filter *FilterDetector;
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

    /*! Maps each InData to an InData. */
  vector<int> TraceInputTrace;
    /*! Maps each InData to an EventData. */
  vector<int> TraceInputEvent;
    /*! Maps each EventData to an InData. */
  vector<int> EventInputTrace;
    /*! Maps each EventData to an EventData. */
  vector<int> EventInputEvent;

  EventData *StimulusEvents;

  QPopupMenu *Menu;

  bool NeedAdjust;
  int AdjustFlag;

  RELACSWidget *RW;


private slots:

    /*! Launches the option dialog of the event detector
        as specified by \a id. 
        This function is called from the Filterdetectors menu. */
  void select( int id );

};

#endif
