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
    OverviewModel( 0 )
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
    OverviewModel( data.OverviewModel )
{
}


DataIndex::DataItem::DataItem( const string &name )
  : Level( 0 ),
    Name( name ),
    Parent( 0 ),
    OverviewModel( 0 )
{
  Data.clear();
  Children.clear();
}


DataIndex::DataItem::DataItem( const string &name, int level,
			       DataIndex::DataItem *parent )
  : Level( level ),
    Name( name ),
    Parent( parent ),
    OverviewModel( parent->overviewModel() )
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
    OverviewModel( parent->overviewModel() )
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
  OverviewModel->beginPopChild( this );
  Children.pop_back();
  OverviewModel->endPopChild( this );
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
  OverviewModel->beginAddChild( this );
  Children.push_back( DataItem( name, level()+1, this ) );
  OverviewModel->endAddChild( this );
}


void DataIndex::DataItem::addChild( const string &name, const Options &data )
{
  OverviewModel->beginAddChild( this );
  Children.push_back( DataItem( name, data, level()+1, this ) );
  OverviewModel->endAddChild( this );
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


DataOverviewModel *DataIndex::DataItem::overviewModel( void )
{
  return OverviewModel;
}


void DataIndex::DataItem::setOverviewModel( DataOverviewModel *model )
{
  OverviewModel = model;
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
  OverviewModel = new DataOverviewModel( 0 );
  OverviewModel->setDataIndex( this );
  Cells.setOverviewModel( OverviewModel );

  DescriptionModel = new DataDescriptionModel( 0 );
  DescriptionModel->setOptions( 0 );
}


DataIndex::~DataIndex( void )
{
  Cells.clear();
  delete OverviewModel;
  delete DescriptionModel;
}


void DataIndex::addStimulus( const Options &signal )
{
  if ( ! Cells.empty() && ! Cells.back().empty() && Session ) {
    string s = signal.type();
    Cells.back().back().addChild( s, signal );
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
  if ( Session && ! saved ) {
    descriptionModel()->setOptions( 0 );
    Cells.pop();
  }
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


DataOverviewModel *DataIndex::overviewModel( void )
{
  return OverviewModel;
}


void DataIndex::setOverviewView( QTreeView *view )
{
  if ( OverviewModel != 0 ) {
    OverviewModel->setTreeView( view );
  }
}


DataDescriptionModel *DataIndex::descriptionModel( void )
{
  return DescriptionModel;
}


void DataIndex::setDescriptionView( QTreeView *view )
{
  if ( DescriptionModel != 0 )
    DescriptionModel->setTreeView( view );
}


DataOverviewModel::DataOverviewModel( QObject *parent )
  : QAbstractItemModel( parent ),
    Data( 0 ),
    View( 0 )
{
}


void DataOverviewModel::setDataIndex( DataIndex *data )
{
  Data = data;
}


void DataOverviewModel::setTreeView( QTreeView *view )
{
  View = view;
  if ( View != 0 ) {
    connect( view->selectionModel(),
	     SIGNAL( currentChanged( const QModelIndex&, const QModelIndex& ) ),
	     this, SLOT( setDescription( const QModelIndex&, const QModelIndex& ) ) );
  }
}


QVariant DataOverviewModel::data( const QModelIndex &index, int role ) const
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


Qt::ItemFlags DataOverviewModel::flags( const QModelIndex &index ) const
{ 
  if ( ! index.isValid() )
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant DataOverviewModel::headerData( int section, Qt::Orientation orientation,
					int role ) const
{
  if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
    return QVariant( QString( "Name" ) );
  
  return QVariant();
}


QModelIndex DataOverviewModel::index( int row, int column,
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


QModelIndex DataOverviewModel::parent( const QModelIndex &index ) const
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


bool DataOverviewModel::hasChildren( const QModelIndex &parent ) const
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


int DataOverviewModel::rowCount( const QModelIndex &parent ) const
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


int DataOverviewModel::columnCount( const QModelIndex &parent ) const
{
  return 1;
}


bool DataOverviewModel::canFetchMore( const QModelIndex &parent ) const
{
  return false;
}


void DataOverviewModel::fetchMore( const QModelIndex &parent )
{
}


void DataOverviewModel::beginAddChild( DataIndex::DataItem *parent )
{
  DataIndex::DataItem *parentitem = parent->parent();
  if ( parentitem == 0 )
    beginInsertRows( QModelIndex(), parent->size(), parent->size() );
  else
    beginInsertRows( createIndex( parentitem->index( parent ), 0, parent ),
		     parent->size(), parent->size() );
}


void DataOverviewModel::endAddChild( DataIndex::DataItem *parent )
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


void DataOverviewModel::beginPopChild( DataIndex::DataItem *parent )
{
  DataIndex::DataItem *parentitem = parent->parent();
  if ( parentitem == 0 )
    beginRemoveRows( QModelIndex(), parent->size()-1, parent->size()-1 );
  else
    beginRemoveRows( createIndex( parentitem->index( parent ), 0, parent ),
		     parent->size()-1, parent->size()-1 );
}


void DataOverviewModel::endPopChild( DataIndex::DataItem *parent )
{
  endRemoveRows();
  if ( View != 0 ) {
    QModelIndex item = createIndex( parent->size()-1, 0,
				    parent->child( parent->size()-1 ) );
    View->scrollTo( item );
    View->setCurrentIndex( item );
  }
}


void DataOverviewModel::setDescription( const QModelIndex &index )
{
  if ( ! index.isValid() )
    return;
  
  if ( index.column() > 0 )
    return;
  
  DataIndex::DataItem *item =
    static_cast<DataIndex::DataItem*>( index.internalPointer() );

  if ( item == 0 )
    return;

  // inform descriptionModel about new selected data:
  Data->descriptionModel()->setOptions( &item->data() );
}


void DataOverviewModel::setDescription( const QModelIndex &current,
					const QModelIndex &previous )
{
  setDescription( current );
}


DataDescriptionModel::DataDescriptionModel( QObject *parent )
  : QAbstractItemModel( parent ),
    Data( 0 ),
    View( 0 )
{
}


void DataDescriptionModel::setOptions( Options *data )
{
  beginResetModel();
  Data = data;
  endResetModel();
  if ( Data != 0 )
    View->expandToDepth( 1 );
}


void DataDescriptionModel::setTreeView( QTreeView *view )
{
  View = view;
}


QVariant DataDescriptionModel::data( const QModelIndex &index, int role ) const
{
  if ( ! index.isValid() )
    return QVariant();
  
  if ( role != Qt::DisplayRole )
    return QVariant();
  
  if ( index.column() > 1 )
    return QVariant();
  
  Options *item = static_cast<Options*>( index.internalPointer() );

  string s = "";
  int i = index.row();
  if ( i < item->parameterSize() )
    s = index.column() > 0 ? (*item)[i].text() : (*item)[i].name();
  else {
    i -= item->parameterSize();
    if ( i < item->sectionsSize() )
	 s = index.column() > 0 ? item->section( i ).type() : item->section( i ).name();
  }

  return QVariant( QString( s.c_str() ) );
}


Qt::ItemFlags DataDescriptionModel::flags( const QModelIndex &index ) const
{ 
  if ( ! index.isValid() )
    return 0;

  return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QVariant DataDescriptionModel::headerData( int section, Qt::Orientation orientation,
					   int role ) const
{
  if ( orientation == Qt::Horizontal && role == Qt::DisplayRole )
    return section > 0 ? QVariant( QString( "Value" ) ) : QVariant( QString( "Name" ) );
  
  return QVariant();
}


QModelIndex DataDescriptionModel::index( int row, int column,
					 const QModelIndex &parent ) const
{
  //  if ( !hasIndex( row, column, parent ) )
  //    return QModelIndex();

  Options *parentitem = 0;
  if ( ! parent.isValid() ) {
    if ( Data != 0 )
      return createIndex( row, column, Data );
    else
      return QModelIndex();
  }

  parentitem = static_cast<Options*>( parent.internalPointer() );
  if ( parent.row() >= parentitem->parameterSize() ) {
    int r = parent.row() - parentitem->parameterSize();
    if ( r < parentitem->sectionsSize() ) {
      Options *childitem = &parentitem->section( r );
      return createIndex( row, column, childitem );
    }
  }

  return QModelIndex();
}


QModelIndex DataDescriptionModel::parent( const QModelIndex &index ) const
{
  if ( ! index.isValid() )
    return QModelIndex();

  Options* childitem = static_cast<Options*>( index.internalPointer() );
  Options* parentitem = childitem->parentSection();
  if ( parentitem == 0 )
    return QModelIndex();

  for ( int r=0; r<parentitem->sectionsSize(); r++ ) {
    if ( &parentitem->section( r ) == childitem ) {
      r += parentitem->parameterSize();
      return createIndex( r, 0, parentitem );
    }
  }
  return QModelIndex();
}


bool DataDescriptionModel::hasChildren( const QModelIndex &parent ) const
{
  if ( parent.column() > 0 )
    return false;

  if ( parent.isValid() ) {
    Options* parentitem = static_cast<Options*>( parent.internalPointer() );
    int r = parent.row() -  parentitem->parameterSize();
    if ( r >= 0 && r < parentitem->sectionsSize() ) {
      Options* childitem = &parentitem->section( r );
      return ( childitem->parameterSize() + childitem->sectionsSize() ) > 0;
    }
  }
  else if ( Data != 0 )
    return ( Data->parameterSize() + Data->sectionsSize() ) > 0;

  return false;
}


int DataDescriptionModel::rowCount( const QModelIndex &parent ) const
{
  if ( parent.column() > 0 )
    return 0;

  if ( parent.isValid() ) {
    Options* parentitem = static_cast<Options*>( parent.internalPointer() );
    int r = parent.row() -  parentitem->parameterSize();
    if ( r >= 0 && r < parentitem->sectionsSize() ) {
      Options* childitem = &parentitem->section( r );
      return childitem->parameterSize() + childitem->sectionsSize();
    }
  }
  else if ( Data != 0 )
    return Data->parameterSize() + Data->sectionsSize();

  return 0;
}


int DataDescriptionModel::columnCount( const QModelIndex &parent ) const
{
  return 2;
}


}; /* namespace relacs */


#include "moc_dataindex.cc"

