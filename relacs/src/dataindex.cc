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
#include <relacs/databrowser.h>
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


}; /* namespace relacs */


#include "moc_dataindex.cc"

