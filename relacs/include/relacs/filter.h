/*
  ../include/relacs/filter.h
  Basic virtual class for filtering or detecting events (e.g. spikes) 

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

#ifndef _FILTER_H_
#define _FILTER_H_

#include <limits.h>
#include <relacs/relacsplugin.h>

/*! 
\class Filter
\brief Basic virtual class for filtering or detecting events (e.g. spikes) 
       in an InData or EventData
\author Jan Benda
\version 1.2
*/

class Filter : public RELACSPlugin
{
  Q_OBJECT

public:

    /*! The different types of filters and event detectors. */
  enum FilterType {
      /*! Filters a single analog input trace. */
    SingleAnalogFilter=0,
      /*! Filters multiple input traces. */
    MultipleAnalogFilter=1, 
      /*! Filters a single event trace. */
    SingleEventFilter=2,
      /*! Filters multiple event traces. */
    MultipleEventFilter=3,
      /*! Detects events in a single analog input trace. */
    SingleAnalogDetector=4,
      /*! Detects multiple events in multiple input traces. */
    MultipleAnalogDetector=5, 
      /*! Detects events in a single event trace. */
    SingleEventDetector=6,
      /*! Detects multiple events in multiple event traces. */
    MultipleEventDetector=7,
      /*! This bit indicates multiple input and output traces. */
    MultipleTraces=1,
      /*! This bit indicates events as input to the filter or detector. */
    EventInput=2,
      /*! This bit indicates an event detector. */
    EventDetector=4
  };

    /*! The constructor.
        A Filter is a widget, i.e. you can display parameters 
        or plot something on the screen. 
	\a mode is passed to the resulting InData or EventData.
	\a type determines the type of the filter or detector.
        \a outtraces is the number of produced output traces.
        If \a outtraces == 0, then the number of output traces
        equals the number of input traces.
        The %Filter was written by \a author.
        The \a version is from \a date. */
  Filter( const string &ident, int mode, FilterType type, int outtraces=0, 
	  const string &name="", const string &title="",
	  const string &author="unknown", const string &version="unknown",
	  const string &date=__DATE__ );
    /*! The destructor. */
  virtual ~Filter( void );

    /*! Reimplement this function to initialize your filter.
        This function is for initializing filters 
	that filter a single trace of the analog data 
	given in \a indata.
	The filter and the data are already locked during this function 
	by lock() and writeLockData(), respectively.
        Returns zero on success or one at failure. */
  virtual int init( const InData &indata, InData &outdata ) { return INT_MIN; };
    /*! Reimplement this function to initialize your filter.
        This function is for initializing filters 
	that filter multiple traces of the analog data 
	given in \a indata.
	The filter and the data are already locked during this function 
	by lock() and writeLockData(), respectively.
        Returns zero on success or one at failure. */
  virtual int init( const InList &indata, InList &outdata ) { return INT_MIN; };
    /*! Reimplement this function to initialize your filter.
        This function is for initializing filters 
	that filter the events \a inevents.
	The filter and the data are already locked during this function 
	by lock() and writeLockData(), respectively.
        Returns zero on success or one at failure. */
  virtual int init( const EventData &inevents, InData &outdata ) { return INT_MIN; };
    /*! Reimplement this function to initialize your filter.
        This function is for initializing filters 
	that filter multiple events \a inevents.
	The filter and the data are already locked during this function 
	by lock() and writeLockData(), respectively.
        Returns zero on success or one at failure. */
  virtual int init( const EventList &inevents, InList &outdata ) { return INT_MIN; };

    /*! Reimplement this function to initialize your event detector.
        This function is for initializing detectors 
	that detect events in a single trace of the analog data 
	given in \a indata.
	The detector and the data are already locked during this function 
	by lock() and writeLockData(), respectively.
        Returns zero on success or one at failure. */
  virtual int init( const InData &indata, EventData &outevents,
		    const EventList &other, const EventData &stimuli ) { return INT_MIN; };
    /*! Reimplement this function to initialize your event detector.
        This function is for initializing detectors 
	that detect events in multiple traces of the analog data 
	given in \a indata.
	The detector and the data are already locked during this function 
	by lock() and writeLockData(), respectively.
        Returns zero on success or one at failure. */
  virtual int init( const InList &indata, EventList &outevents,
		    const EventList &other, const EventData &stimuli ) { return INT_MIN; };
    /*! Reimplement this function to initialize your event detector.
        This function is for initializing detectors 
	that detect events in the events \a inevents.
	The detector and the data are already locked during this function 
	by lock() and writeLockData(), respectively.
        Returns zero on success or one at failure. */
  virtual int init( const EventData &inevents, EventData &outevents,
		    const EventList &other, const EventData &stimuli ) { return INT_MIN; };
    /*! Reimplement this function to initialize your event detector.
        This function is for initializing detectors 
	that detect events in the multiple events \a inevents.
	The detector and the data are already locked during this function 
	by lock() and writeLockData(), respectively.
        Returns zero on success or one at failure. */
  virtual int init( const EventList &inevents, EventList &outevents, 
		    const EventList &other, const EventData &stimuli ) { return INT_MIN; };

    /*! Reimplement this function to react to changes in the input gain.
        This function is for filters and detectors 
	that detect events in a single trace of the analog data 
	given in \a indata.
	The filter and the data are already locked during this function 
	by lock() and writeLockData(), respectively. */
  virtual int adjust( const InData &indata ) { return INT_MIN; };
    /*! Reimplement this function to react to changes in the input gain.
        This function is for filters and detectors 
        that detect events in multiple traces of the analog data 
	given in \a indata.
	The filter and the data are already locked during this function 
	by lock() and writeLockData(), respectively. */
  virtual int adjust( const InList &indata ) { return INT_MIN; };
    /*! Reimplement this function to react to changes in the input gain.
        This function is for filters and detectors 
	that detect events in the events \a inevents.
	The filter and the data are already locked during this function 
	by lock() and writeLockData(), respectively. */
  virtual int adjust( const EventData &inevents ) { return INT_MIN; };
    /*! Reimplement this function to react to changes in the input gain.
        This function is for filters and detectors 
	that detect events in the multiple events \a inevents.
	The filter and the data are already locked during this function 
	by lock() and writeLockData(), respectively. */
  virtual int adjust( const EventList &inevents ) { return INT_MIN; };

    /*! Reimplement this function with an appropriate filter.
        This function filters a single trace of
	the analog data given in \a indata.
	The filtered trace is stored in \a outdata.
	The filter and the data are already locked during this function 
	by lock() and writeLockData(), respectively. */
  virtual int filter( const InData &indata, InData &outdata ) { return INT_MIN; };
    /*! Reimplement this function with an appropriate filter.
        This function filters multiple traces of
	the analog data given in \a indata.
	The filtered traces are stored in \a outdata.
	The filter and the data are already locked during this function 
	by lock() and writeLockData(), respectively. */
  virtual int filter( const InList &indata, InList &outdata ) { return INT_MIN; };
    /*! Reimplement this function with an appropriate filter.
        This function filters the events \a inevents.
	The filtered trace is stored in \a outdata.
	The filter and the data are already locked during this function 
	by lock() and writeLockData(), respectively. */
  virtual int filter( const EventData &inevents, InData &outdata ) { return INT_MIN; };
    /*! Reimplement this function with an appropriate filter.
        This function filters multiple events \a inevents.
	The filtered traces are stored in \a outdata.
	The filter and the data are already locked during this function 
	by lock() and writeLockData(), respectively. */
  virtual int filter( const EventList &inevents, InList &outdata ) { return INT_MIN; };
    /*! Reimplement this function with an appropriate event detector.
        This function detects events in a single trace of
	the analog data given in \a data.
	The trace number is specified by \a events->trace().
	The events are stored in \a outevents.
	The detector and the data are already locked during this function 
	by lock() and writeLockData(), respectively. */
  virtual int detect( const InData &data, EventData &outevents, 
		      const EventList &other, const EventData &stimuli ) { return INT_MIN; };
    /*! Reimplement this function with an appropriate event detector.
        This function detects events in multiple traces of
	the analog data given in \a data.
	The trace numbers are specified by \a events->trace() and \a event->traceNum().
	The events are stored in \a outevents.
	The detector and the data are already locked during this function 
	by lock() and writeLockData(), respectively. */
  virtual int detect( const InList &data, EventList &outevents, 
		      const EventList &other, const EventData &stimuli ) { return INT_MIN; };
    /*! Reimplement this function with an appropriate event detector.
        This function detects events in the events \a inevents.
	The detected events are stored in \a outevents.
	The detector and the data are already locked during this function 
	by lock() and writeLockData(), respectively. */
  virtual int detect( const EventData &inevents, EventData &outevents, 
		      const EventList &other, const EventData &stimuli ) { return INT_MIN; };
    /*! Reimplement this function with an appropriate event detector.
        This function detects events in multiple events \a inevents.
	The events are stored in \a outevents.
	The detector and the data are already locked during this function 
	by lock() and writeLockData(), respectively. */
  virtual int detect( const EventList &inevents, 
		      EventList &outevents, 
		      const EventList &other, const EventData &stimuli ) { return INT_MIN; };

    /*! Reimplement this function if you want to save some
        data of your filter/detector.
	This function might be called by a RePro or Session
	or from a Macro with some arbitrary string \a param.
        \note The filter is NOT locked during the call of save()! */
  virtual void save( const string &param ) {};
    /*! Calls the save() function with an empty \a param string. */
  void save( void ) { save( "" ); };

    /*! The identifier string for this instance of the %Filter. */
  const string &ident( void ) const;
    /*! Set the identifier string for this instance of the %Filter to \a ident. */
  virtual void setIdent( const string &ident );
    /*! Set the name for this type of %Filter to \a name. */
  virtual void setName( const string &name );

    /*! Indicates which of the different types of filter or detector functions
        are supported by the event detector. */
  FilterType type( void ) const { return Type; };
    /*! The number of output traces (of InData or EventData)
        the detector produces. */
  int outTraces( void ) const { return OutTraces; };
    /*! Mode which is passed to the corresponding output InData or EventData. */
  int mode( void ) const { return Mode; };
    /*! Set the mode which is passed to the corresponding output InData 
        or EventData to \a mode. */
  void setMode( int mode ) { Mode = mode; };


private:

    /*! An identifier string for this instance of the %Filter. */
  string Ident;

    /*! Indicates which of the different types of filter/detector functions
        are supported by the %Filter. */
  FilterType Type;
    /*! The number of output traces (of output InData or EventData)
        the detector produces. */
  int OutTraces;
    /*! Mode for the output InData or EventData. */
  int Mode;

};


#endif
