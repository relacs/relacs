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
#include <relacs/outdatainfo.h>
#include <relacs/options.h>
using namespace std;

namespace relacs {


  class RePro;


/*! 
\class DataIndex
\brief Index to previously recorded data, repro, and stimuli.
\author Jan Benda

This is the data model used by the DataBrowser class.
*/


class DataIndex
{

  friend class DataTreeModel;

public:


  class DataItem
  {

  public:

    DataItem( void );
    DataItem( const DataItem &data );
    DataItem( const string &name, int level=0, DataItem *parent=0 );
    DataItem( const string &name, const Options &data,
	      int level=0, DataItem *parent=0 );
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
    int level( void ) const;
    void setName( const string &name );
    string name( void ) const;
    const Options &data( void ) const;
    Options &data( void );
    void print( void );


  protected:

    int Level;
    string Name;
    Options Data;
    deque<DataItem> Children;
    DataItem *Parent;
  };


  DataIndex( void );
  ~DataIndex( void );

  DataItem *cells( void ) { return &Cells; };

  void addStimulus( const OutDataInfo &signal );
  void addStimulus( const deque< OutDataInfo > &signal );
  void addRepro( const RePro &repro );
  void addSession( const string &path, const Options &data );
  void endSession( bool saved );
  void loadDirectory( const string &dir );
  void loadCell( int index );

  bool empty( void ) const { return Cells.empty(); };
  int size( void ) const { return Cells.size(); };

  void print( void );


private:

  DataItem Cells;
  bool Session;

};


}; /* namespace relacs */

#endif /* ! _RELACS_DATAINDEX_H_ */

