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
    DataOverviewModel *overviewModel( void );
    void setOverviewModel( DataOverviewModel *model );
    void print( void );


  protected:

    int Level;
    string Name;
    Options Data;
    deque<DataItem> Children;
    DataItem *Parent;
    DataOverviewModel *OverviewModel;
  };


  DataIndex( void );
  ~DataIndex( void );

  DataItem *cells( void ) { return &Cells; };

  void addStimulus( const Options &signal );
  void addRepro( const RePro &repro );
  void addSession( const string &path, const Options &data );
  void endSession( bool saved );
  void loadDirectory( const string &dir );
  void loadCell( int index );

  bool empty( void ) const { return Cells.empty(); };
  int size( void ) const { return Cells.size(); };

  void print( void );

  DataOverviewModel *overviewModel( void );
  void setOverviewView( QTreeView *view );

  DataDescriptionModel *descriptionModel( void );
  void setDescriptionView( QTreeView *view );


private:

  DataItem Cells;
  bool Session;

  DataOverviewModel *OverviewModel;
  DataDescriptionModel *DescriptionModel;

};


/*! 
\class DataOverviewModel
\brief The model for viewing an overview of the data of an DataIndex.
\author Jan Benda
*/


class DataOverviewModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  DataOverviewModel( QObject *parent = 0 );

  void setDataIndex( DataIndex *data );
  void setTreeView( QTreeView *view );

  QVariant data( const QModelIndex &index, int role ) const;
  Qt::ItemFlags flags( const QModelIndex &index ) const;
  QVariant headerData( int section, Qt::Orientation orientation,
		       int role = Qt::DisplayRole ) const;
  QModelIndex index( int row, int column,
		     const QModelIndex &parent = QModelIndex() ) const;
  QModelIndex parent( const QModelIndex &index ) const;
  bool hasChildren( const QModelIndex &parent = QModelIndex() ) const;
  int rowCount( const QModelIndex &parent = QModelIndex() ) const;
  int columnCount( const QModelIndex &parent = QModelIndex() ) const;
  bool canFetchMore( const QModelIndex &parent ) const;
  void fetchMore( const QModelIndex &parent );
  void beginAddChild( DataIndex::DataItem *parent );
  void endAddChild( DataIndex::DataItem *parent );
  void beginPopChild( DataIndex::DataItem *parent );
  void endPopChild( DataIndex::DataItem *parent );


public slots:

  void setDescription( const QModelIndex &index );
  void setDescription( const QModelIndex &currrent, const QModelIndex &previous );


private:

  DataIndex *Data;
  QTreeView *View;

};


/*! 
\class DataDescriptionModel
\brief The model for viewing a description of the currently displayed data of an DataIndex.
\author Jan Benda
*/


class DataDescriptionModel : public QAbstractItemModel
{
  Q_OBJECT

public:
  DataDescriptionModel( QObject *parent = 0 );

  void setOptions( Options *data );
  void setTreeView( QTreeView *view );

  QVariant data( const QModelIndex &index, int role ) const;
  Qt::ItemFlags flags( const QModelIndex &index ) const;
  QVariant headerData( int section, Qt::Orientation orientation,
		       int role = Qt::DisplayRole ) const;
  QModelIndex index( int row, int column,
		     const QModelIndex &parent = QModelIndex() ) const;
  QModelIndex parent( const QModelIndex &index ) const;
  bool hasChildren( const QModelIndex &parent = QModelIndex() ) const;
  int rowCount( const QModelIndex &parent = QModelIndex() ) const;
  int columnCount( const QModelIndex &parent = QModelIndex() ) const;

private:

  Options *Data;
  QTreeView *View;

};


}; /* namespace relacs */

#endif /* ! _RELACS_DATAINDEX_H_ */

