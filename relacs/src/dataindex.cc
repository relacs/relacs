/*
  dataindex.cc
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

#include <QDir>
#include <QStringList>
#include <relacs/datafile.h>
#include <relacs/dataindex.h>
#include <relacs/outdatainfo.h>
#include <relacs/repro.h>


namespace relacs {



DataIndex::DataItem::DataItem( void )
  : Level( 0 ),
    Parent( 0 ),
    TreeModel( 0 )
{
  Data.clear();
  Children.clear();
}


DataIndex::DataItem::DataItem( const DataIndex::DataItem &data )
  : Level( data.Level ),
    Name( data.Name ),
    Data( data.Data ),
    Children( data.Children ),
    Parent( data.Parent ),
    TreeModel( data.TreeModel )
{
}


DataIndex::DataItem::DataItem( const string &name )
  : Level( 0 ),
    Name( name ),
    Parent( 0 ),
    TreeModel( 0 )
{
  Data.clear();
  Children.clear();
}


DataIndex::DataItem::DataItem( const string &name, int level,
			       DataIndex::DataItem *parent )
  : Level( level ),
    Name( name ),
    Parent( parent ),
    TreeModel( parent->treeModel() )
{
  Data.clear();
  Children.clear();
}


DataIndex::DataItem::DataItem( const string &name, const Options &data,
			       int level, DataIndex::DataItem *parent )
  : Level( level ),
    Name( name ),
    Data( data ),
    Parent( parent ),
    TreeModel( parent->treeModel() )
{
  Children.clear();
}


bool DataIndex::DataItem::empty( void ) const
{
  return Children.empty();
}


int DataIndex::DataItem::size( void ) const
{
  return Children.size();
}


void DataIndex::DataItem::clear( void )
{
  Children.clear();
}


DataIndex::DataItem &DataIndex::DataItem::back( void )
{
  return Children.back();
}


void DataIndex::DataItem::pop( void )
{
  TreeModel->beginPopChild( this );
  Children.pop_back();
  TreeModel->endPopChild( this );
}


const DataIndex::DataItem &DataIndex::DataItem::operator[]( int index ) const
{
  return Children[index];
}


DataIndex::DataItem &DataIndex::DataItem::operator[]( int index )
{
  return Children[index];
}


int DataIndex::DataItem::index( DataItem *data )
{
  for ( int k=0; k<size(); k++ ) {
    if ( &Children[k] == data )
      return k;
  }
  return -1;
}


DataIndex::DataItem *DataIndex::DataItem::child( int index )
{
  if ( index >= 0 && index < size() )
    return &Children[index];

  return 0;
}


DataIndex::DataItem *DataIndex::DataItem::parent( void ) const
{
  return Parent;
}


void DataIndex::DataItem::addChild( const string &name )
{
  TreeModel->beginAddChild( this );
  Children.push_back( DataItem( name, level()+1, this ) );
  TreeModel->endAddChild( this );
}


void DataIndex::DataItem::addChild( const string &name, const Options &data )
{
  TreeModel->beginAddChild( this );
  Children.push_back( DataItem( name, data, level()+1, this ) );
  TreeModel->endAddChild( this );
}


int DataIndex::DataItem::level( void ) const
{
  return Level;
}


string DataIndex::DataItem::name( void ) const
{
  return Name;
}


void DataIndex::DataItem::setName( const string &name )
{
  Name = name;
}


const Options &DataIndex::DataItem::data( void ) const
{
  return Data;
}


Options &DataIndex::DataItem::data( void )
{
  return Data;
}


DataTreeModel *DataIndex::DataItem::treeModel( void )
{
  return TreeModel;
}


void DataIndex::DataItem::setTreeModel( DataTreeModel *model )
{
  TreeModel = model;
}


void DataIndex::DataItem::print( void )
{
  /*
  for ( int k=0; k<level(); k++ )
    cout << "  ";
  cout << name() << '\n';
  for ( int k=0; k<size(); k++ )
    Children[k].print();
  */
}


DataIndex::DataIndex( void )
  : Cells( "Data" ),
    Session ( false )
{
  TreeModel = new DataTreeModel( 0 );
  TreeModel->setDataIndex( this );
  Cells.setTreeModel( TreeModel );

  // load current directory:
  QDir dir;
  loadDirectory( dir.currentPath().toStdString() );
}


DataIndex::~DataIndex( void )
{
  Cells.clear();
  delete TreeModel;
}


void DataIndex::addStimulus( const OutDataInfo &signal )
{
  if ( ! Cells.empty() && ! Cells.back().empty() && Session ) {
    Cells.back().back().addChild( "stimulus" );
  }
  print();
}


void DataIndex::addStimulus( const deque< OutDataInfo > &signal )
{
  if ( ! Cells.empty() && ! Cells.back().empty() && Session ) {
    Cells.back().back().addChild( "stimulus" );
  }
  print();
}


void DataIndex::addRepro( const RePro &repro )
{
  if ( ! Cells.empty() && Session )
    Cells.back().addChild( repro.name(), repro );
  print();
}


void DataIndex::addSession( const string &path, const Options &data )
{
  Cells.addChild( path, data );
  Session = true;
  print();
}


void DataIndex::endSession( bool saved )
{
  if ( Session && ! saved )
    Cells.pop();
  Session = false;
  print();
}


void DataIndex::loadDirectory( const string &path )
{
  QDir hdir( path.c_str() );

  if ( hdir.exists() ) {

    QStringList list = hdir.entryList( QDir::Dirs, QDir::Name );
    bool first = true;

    for ( int i=0; i < list.size(); ++i ) {

      string file = path+"/"+list[i].toStdString()+"/stimuli.dat";
      if ( ! hdir.exists( file.c_str() ) ) {
        file = path+"/"+list[i].toStdString()+"/trigger.dat";
        if ( ! hdir.exists( file.c_str() ) )
          file = "";
      }

      if ( ! file.empty() ) {
        // clear cells:
        if ( first ) {
          Cells.clear();
          first = false;
        }
        // load data:
        DataFile sf( file );
        sf.read( 1 );
        Cells.addChild( file,
      		  sf.metaDataOptions( sf.levels()>0 ? sf.levels()-1 : 0 ) );
      }

    }

    Cells.setName( path );

  }
  print();

}


void DataIndex::loadCell( int index )
{
  if ( index < 0 || index >= (int)Cells.size() )
    return;

  Cells[index].clear();
  DataFile sf( Cells[index].name() );
  while ( sf.read( 1 ) ) {
    // add RePro:
    Options &repro = sf.metaDataOptions( 0 );
    string name = repro.text( "repro" );
    Cells[index].addChild( name, repro );
    Cells[index].back().clear();
    for ( int k=0; k<sf.data().rows(); k++ ) {
      // whatever:	Cells[index].Protocolls.back().Stimuli.push_back();
    }
  }
}


void DataIndex::print( void )
{
  Cells.print();
}


DataTreeModel *DataIndex::treeModel( void )
{
  return TreeModel;
}


void DataIndex::setTreeView( QTreeView *view )
{
  if ( TreeModel != 0 )
    TreeModel->setTreeView( view );
}


DataTreeModel::DataTreeModel( QObject *parent )
  : QAbstractItemModel( parent ),
    Data( 0 ),
    View( 0 )
{
}


void DataTreeModel::setDataIndex( DataIndex *data )
{
  Data = data;
}


void DataTreeModel::setTreeView( QTreeView *view )
{
  View = view;
  if ( View != 0 ) {
    QModelIndex item = createIndex( Data->cells()->size()-1, 0,
				    Data->cells()->child( Data->cells()->size()-1 ) );
    View->scrollTo( item );
    View->setCurrentIndex( item );
  }
}


QVariant DataTreeModel::data( const QModelIndex &index, int role ) const
{
  if ( ! index.isValid() )
    return QVariant();
  
  if ( role != Qt::DisplayRole )
    return QVariant();
  
  if ( index.column() > 0 )
    return QVariant();
  
  DataIndex::DataItem *item =
    static_cast<DataIndex::DataItem*>( index.internalPointer() );

  if ( item->level() == 1 ) {
    Str file = item->name();
    return QVariant( QString( file.dir().preventedSlash().name().c_str() ) );
  }
  return QVariant( QString( item->name().c_str() ) );
}


Qt::ItemFlags DataTreeModel::flags( const QModelIndex &index ) const
{ 
  if ( ! index.isValid() )
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant DataTreeModel::headerData( int section, Qt::Orientation orientation,
				    int role ) const
{
  if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
    return QVariant( QString( "Name" ) );
  
  return QVariant();
}


QModelIndex DataTreeModel::index( int row, int column,
				  const QModelIndex &parent ) const
{
  //  if ( !hasIndex( row, column, parent ) )
  //    return QModelIndex();

  DataIndex::DataItem *parentitem = 0;
  if ( ! parent.isValid() )
    parentitem = Data->cells();
  else
    parentitem = static_cast<DataIndex::DataItem*>( parent.internalPointer() );
  
  DataIndex::DataItem *childitem = parentitem->child( row );
  if ( childitem )
    return createIndex( row, column, childitem );

  return QModelIndex();
}


QModelIndex DataTreeModel::parent( const QModelIndex &index ) const
{
  if ( ! index.isValid() )
    return QModelIndex();

  DataIndex::DataItem* childitem =
    static_cast<DataIndex::DataItem*>( index.internalPointer() );
  DataIndex::DataItem* parentitem = childitem->parent();
  if ( parentitem == Data->cells() )
    return QModelIndex();
  return createIndex( parentitem->parent()->index( parentitem ), 0, parentitem );
}


bool DataTreeModel::hasChildren( const QModelIndex & parent ) const
{
  if ( ! parent.isValid() )
    return ! Data->cells()->empty();

  DataIndex::DataItem* parentitem =
    static_cast<DataIndex::DataItem*>( parent.internalPointer() );
  if ( parentitem == 0 )
    return false;

  if ( parentitem->level() == 1 )
    return true;
  else
    return ! parentitem->empty();
}


int DataTreeModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.column() > 0 )
    return 0;

  if ( parent.isValid() ) {
    DataIndex::DataItem* parentitem =
      static_cast<DataIndex::DataItem*>( parent.internalPointer() );
    if ( parentitem->level() == 1 ) {
      //      cerr << "we need to load cell " << parentitem->name() << '\n';
    }
    return parentitem->size();
  }
  else
    return Data->size();

  return 0;
}


int DataTreeModel::columnCount( const QModelIndex &parent ) const
{
  return 1;
}


bool DataTreeModel::canFetchMore( const QModelIndex &parent ) const
{
  return false;
}


void DataTreeModel::fetchMore( const QModelIndex &parent )
{
}


void DataTreeModel::beginAddChild( DataIndex::DataItem *parent )
{
  DataIndex::DataItem *parentitem = parent->parent();
  if ( parentitem == 0 )
    beginInsertRows( QModelIndex(), parent->size(), parent->size() );
  else
    beginInsertRows( createIndex( parentitem->index( parent ), 0, parent ),
		     parent->size(), parent->size() );
}


void DataTreeModel::endAddChild( DataIndex::DataItem *parent )
{
  endInsertRows();
  if ( View != 0 ) {
    if ( parent->size() > 1 ) {
      DataIndex::DataItem *child = parent->child( parent->size()-2 );
      View->collapse( createIndex( parent->size()-2, 0, child ) );
      while ( child->level() < 3 && ! child->empty() ) {
	DataIndex::DataItem *item = child;
	child = item->child( item->size()-1 );
	View->collapse( createIndex( item->size()-1, 0, child ) );
      }
    }
    QModelIndex item = createIndex( parent->size()-1, 0,
				    parent->child( parent->size()-1 ) );
    View->expand( item );
    View->scrollTo( item );
    View->setCurrentIndex( item );
  }
}


void DataTreeModel::beginPopChild( DataIndex::DataItem *parent )
{
  DataIndex::DataItem *parentitem = parent->parent();
  if ( parentitem == 0 )
    beginRemoveRows( QModelIndex(), parent->size()-1, parent->size()-1 );
  else
    beginRemoveRows( createIndex( parentitem->index( parent ), 0, parent ),
		     parent->size()-1, parent->size()-1 );
}


void DataTreeModel::endPopChild( DataIndex::DataItem *parent )
{
  endRemoveRows();
  if ( View != 0 ) {
    QModelIndex item = createIndex( parent->size()-1, 0,
				    parent->child( parent->size()-1 ) );
    View->scrollTo( item );
    View->setCurrentIndex( item );
  }
}


}; /* namespace relacs */


#include "moc_dataindex.cc"

