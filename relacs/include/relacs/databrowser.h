/*
  databrowser.h
  Interface for browsing recorded data.

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

#ifndef _RELACS_DATABROWSER_H_
#define _RELACS_DATABROWSER_H_ 1

#include <deque>
#include <QSplitter>
#include <QTreeView>
#include <relacs/dataindex.h>
using namespace std;

namespace relacs {


class DataTreeModel;


/*!
\class DataBrowser
\brief Interface for browsing recorded data.
\author Karin Fisch, Jan Benda
*/


class DataBrowser : public QSplitter
{
  Q_OBJECT

public:

  DataBrowser( DataIndex *data, QWidget *parent=0 );
  ~DataBrowser( void );

  void display( const string &path, const deque<int> &traceindex,
		const deque<int> &eventsindex, double time );


signals:

  void displayIndex( const string &path, const deque<int> &traceindex,
		     const deque<int> &eventsindex, double time );


protected:

  virtual void keyPressEvent( QKeyEvent *qke );


private:

  QTreeView *OverviewWidget;
  QTreeView *DescriptionWidget;

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
  void setBrowser( DataBrowser *browser );

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
  void endAddChild( void );
  void beginPopChild( DataIndex::DataItem *parent );
  void endPopChild( DataIndex::DataItem *parent );


public slots:

  void setDescription( const QModelIndex &index );
  void setDescription( const QModelIndex &currrent, const QModelIndex &previous );


private slots:

  void displayIndex( const QModelIndex &index );


private:

  DataIndex *Data;
  QTreeView *View;
  DataBrowser *Browser;
  bool AutoActivate;

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

#endif /* ! _RELACS_DATABROWSER_H_ */

