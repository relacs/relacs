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

#include <iostream>
#include <fstream>
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
    Time( 0.0 ),
    Parent( 0 ),
    OverviewModel( 0 )
{
  TraceIndex.clear();
  EventsIndex.clear();
  Data.clear();
  Children.clear();
}


DataIndex::DataItem::DataItem( const DataIndex::DataItem &data )
  : Level( data.Level ),
    Name( data.Name ),
    Data( data.Data ),
    TraceIndex( data.TraceIndex ),
    EventsIndex( data.EventsIndex ),
    Time( data.Time ),
    Children( data.Children ),
    Parent( data.Parent ),
    OverviewModel( data.OverviewModel )
{
}


DataIndex::DataItem::DataItem( const string &name )
  : Level( 0 ),
    Name( name ),
    Time( 0.0 ),
    Parent( 0 ),
    OverviewModel( 0 )
{
  Data.clear();
  TraceIndex.clear();
  EventsIndex.clear();
  Children.clear();
}


DataIndex::DataItem::DataItem( const string &name, int level,
			       DataIndex::DataItem *parent )
  : Level( level ),
    Name( name ),
    Time( 0.0 ),
    Parent( parent ),
    OverviewModel( parent->overviewModel() )
{
  Data.clear();
  TraceIndex.clear();
  EventsIndex.clear();
  Children.clear();
}


DataIndex::DataItem::DataItem( const string &name, const Options &data,
			       int level, DataIndex::DataItem *parent )
  : Level( level ),
    Name( name ),
    Data( data ),
    Time( 0.0 ),
    Parent( parent ),
    OverviewModel( parent->overviewModel() )
{
  TraceIndex.clear();
  EventsIndex.clear();
  Children.clear();
}


DataIndex::DataItem::DataItem( const string &name, const Options &data,
			       const deque<int> &traceindex, const deque<int> &eventsindex,
			       double time, int level, DataIndex::DataItem *parent )
  : Level( level ),
    Name( name ),
    Data( data ),
    TraceIndex( traceindex ),
    EventsIndex( eventsindex ),
    Time( time ),
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


void DataIndex::DataItem::addChild( const string &name, const Options &data,
				    const deque<int> &traceindex,
				    const deque<int> &eventsindex,
				    double time )
{
  OverviewModel->beginAddChild( this );
  Children.push_back( DataItem( name, data, traceindex, eventsindex, time,
				level()+1, this ) );
  OverviewModel->endAddChild( this );
}



void DataIndex::DataItem::loadCell( void )
{
  if ( level() != 1 )
    return;
  DataFile sf( name() );

  // read in meta data:
  sf.readMetaData();
  // load stimulus descriptions:
  string sd = sf.metaDataOptions( sf.levels()-1 ).text( "stimulus descriptions file", "stimulus-descriptions.dat" );
  // fix for the first stimulus descriptions implementation until 2014-06-21:
  if ( sd == "stimlus-descriptions.dat" )
    sd = "stimulus-descriptions.dat"; 
  Options stimuli;
  {
    string filename = Str( name() ).dir() + sd;
    ifstream sf( filename.c_str() );
    stimuli.load( sf );
  }
  // get columns for stimulus names:
  int k1 = sf.key().column( "stimulus>" );
  deque< int > stimcols;
  for ( int k=k1; sf.key().sectionName( k, 2 ) == "stimulus"; k++ ) {
    if ( sf.key().sectionName( k, 0 ) == "signal" )
      stimcols.push_back( k );
  }
  deque< deque< string > > stimnames( stimcols.size() );

  do {
    // add RePro:
    Options &repro = sf.metaDataOptions( 0 );
    OverviewModel->beginAddChild( this );
    Children.push_back( DataItem( repro.text( "RePro" ), repro, level()+1, this ) );
    DataItem *parent = &Children.back();
    OverviewModel->endAddChild();
    // read in stimuli:
    sf.initData();
    for ( unsigned int k=0; k<stimnames.size(); k++ )
      stimnames[k].clear();
    do {
      sf.scanDataLine();
      int index = 0;
      int word = 0;
      unsigned int j = 0;
      for ( int k=0; k<sf.data().columns() && index>=0; k++ ) {
	word = Str( sf.line() ).nextWord( index, Str::WhiteSpace, "#" );
	if ( word >= 0 && k == stimcols[j] ) {
	  stimnames[j].push_back( sf.line().substr( word, index-word ) );
	  j++;
	  if ( j >= stimcols.size() )
	    break;
	}
      }
    } while ( sf.readDataLine( 1 ) );
    // add stimuli:
    for ( int j=0; j<sf.data().rows(); j++ ) {
      // trace indices:
      k1 = sf.key().column( "traces>" );
      deque<int> traceindex;
      for ( int k=k1; sf.key().sectionName( k, 2 ) == "traces"; k++ )
	traceindex.push_back( sf.data( k, j ) );
      // event indices:
      k1 = sf.key().column( "events>" );
      deque<int> eventsindex;
      for ( int k=k1; sf.key().sectionName( k, 2 ) == "events"; k++ ) {
	if ( sf.key().sectionName( k, 0 ) == "index" )
	  eventsindex.push_back( sf.data( k, j ) );
      }
      // signal time:
      double deltat = data().number( "sample interval1", "s" );
      double signaltime = traceindex[0]*deltat;
      // signal description:
      int nstimuli = 0;
      int firststimulus = -1;
      for ( unsigned int k=0; k<stimnames.size(); k++ ) {
	if ( stimnames[k][j] != "-" ) {
	  nstimuli++;
	  if ( firststimulus < 0 )
	    firststimulus = k;
	}
      }
      Options description;
      for ( unsigned int k=0; k<stimnames.size(); k++ ) {
	if ( stimnames[k][j] != "-" ) {
	  Options::const_section_iterator sp = stimuli.findSection( stimnames[k][j] );
	  if ( sp != stimuli.sectionsEnd() ) {
	    if ( nstimuli == 1 ) {
	      description = **sp;
	      break;
	    }
	    else
	      description.newSection( **sp );
	  }
	}
      }
      if ( nstimuli > 1 )
	description.setType( "stimulus" );
      // add stimulus to tree:
      OverviewModel->beginAddChild( parent );
      parent->Children.push_back( DataItem( description.type(), description,
					    traceindex, eventsindex,
					    signaltime, level()+2, parent ) );
      OverviewModel->endAddChild();

    }
  } while ( sf.readMetaData() );
  sf.close();
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


string DataIndex::DataItem::fileName( void ) const
{
  if ( level() == 3 )
    return Parent->Parent->name();
  return "";
}


deque<int> DataIndex::DataItem::traceIndex( void ) const
{
  return TraceIndex;
}


deque<int> DataIndex::DataItem::eventsIndex( void ) const
{
  return EventsIndex;
}


double DataIndex::DataItem::time( void ) const
{
  return Time;
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


void DataIndex::addStimulus( const Options &signal, const deque<int> &traceindex,
			     const deque<int> &eventsindex, double time )
{
  if ( ! Cells.empty() && ! Cells.back().empty() && Session ) {
    string s = signal.type();
    Cells.back().back().addChild( s, signal, traceindex, eventsindex, time );
  }
  print();
}


void DataIndex::addRepro( const Options &repro )
{
  if ( ! Cells.empty() && Session )
    Cells.back().addChild( repro.text( "RePro" ), repro );
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


bool DataIndex::empty( void ) const
{
  return Cells.empty();
}


int DataIndex::size( void ) const
{
  return Cells.size();
}


void DataIndex::print( void )
{
  Cells.print();
}


DataOverviewModel *DataIndex::overviewModel( void )
{
  return OverviewModel;
}


void DataIndex::setOverviewView( QTreeView *view, DataBrowser *browser )
{
  if ( OverviewModel != 0 ) {
    OverviewModel->setTreeView( view );
    OverviewModel->setBrowser( browser );
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


}; /* namespace relacs */


#include "moc_dataindex.cc"

