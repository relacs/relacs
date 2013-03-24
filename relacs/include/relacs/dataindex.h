/*
  dataindex.h
  Index to previously recorded data, repro, and stimuli.

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

#ifndef _RELACS_DATAINDEX_H_
#define _RELACS_DATAINDEX_H_ 1

#include <deque>
#include <string>
#include <QTreeView>
#include <QAbstractItemModel>
#include <relacs/outdatainfo.h>
#include <relacs/options.h>
using namespace std;

namespace relacs {


  class RePro;
  class DataOverviewModel;
  class DataDescriptionModel;
  class DataBrowser;


/*!
\class DataIndex
\brief Index to previously recorded data, repro, and stimuli.
\author Jan Benda

This is the data model used by the DataBrowser class.
*/


class DataIndex
{

public:


  class DataItem
  {

  public:

    DataItem( void );
    DataItem( const DataItem &data );
    DataItem( const string &name );
    DataItem( const string &name, int level, DataItem *parent );
    DataItem( const string &name, const Options &data,
	      int level, DataItem *parent );
    DataItem( const string &name, const Options &data,
	      const deque<int> &traceindex, const deque<int> &eventsindex,
	      double time, int level, DataItem *parent );
    bool empty( void ) const;
    int size( void ) const;
    void clear( void );
    DataItem &back( void );
    void pop( void );
    const DataItem &operator[]( int index ) const;
    DataItem &operator[]( int index );
    int index( DataItem *data );
    DataItem *child( int index );
    DataItem *parent( void ) const;
    void addChild( const string &name );
    void addChild( const string &name, const Options &data );
    void addChild( const string &name, const Options &data,
		   const deque<int> &traceindex, const deque<int> &eventsindex,
		   double time );
    int level( void ) const;
    void setName( const string &name );
    string name( void ) const;
    const Options &data( void ) const;
    Options &data( void );
    deque<int> traceIndex( void ) const;
    deque<int> eventsIndex( void ) const;
    double time( void ) const;
    DataOverviewModel *overviewModel( void );
    void setOverviewModel( DataOverviewModel *model );
    void print( void );


  protected:

    int Level;
    string Name;
    Options Data;
    deque<int> TraceIndex;
    deque<int> EventsIndex;
    double Time;
    deque<DataItem> Children;
    DataItem *Parent;
    DataOverviewModel *OverviewModel;
  };


  DataIndex( void );
  ~DataIndex( void );

  DataItem *cells( void ) { return &Cells; };

  void addStimulus( const Options &signal, const deque<int> &traceindex,
		    const deque<int> &eventsindex, double time );
  void addRepro( const Options &repro );
  void addSession( const string &path, const Options &data );
  void endSession( bool saved );
  void loadDirectory( const string &dir );
  void loadCell( int index );

    /*! \return \c true if no recording session is in the DataIndex. */
  bool empty( void ) const;
    /*! \return the number of recording sessions hold in the DataIndex. */
  int size( void ) const;

  void print( void );

  DataOverviewModel *overviewModel( void );
    /*! Passes \a view and \a browser to the overviewModel(). */
  void setOverviewView( QTreeView *view, DataBrowser *browser );

  DataDescriptionModel *descriptionModel( void );
    /*! Passes \a view to the descriptionModel(). */
  void setDescriptionView( QTreeView *view );


private:

  DataItem Cells;
  bool Session;

  DataOverviewModel *OverviewModel;
  DataDescriptionModel *DescriptionModel;

};


}; /* namespace relacs */

#endif /* ! _RELACS_DATAINDEX_H_ */

