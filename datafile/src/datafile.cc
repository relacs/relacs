/*
  datafile.cc
  Reading Ascii Data Files.

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

#include <relacs/datafile.h>


int DataFile::LevelOffset = 3;


DataFile::DataFile( void )
  : istream( 0 ), MetaData( 0 )
{
  Comment = "#";
  initialize();
}


DataFile::DataFile( const istream &is ) 
  : istream( 0 ), MetaData( 0 )
{
  Comment = "#";
  open( is );
}


DataFile::DataFile( const string &file ) 
  : istream( 0 ), MetaData( 0 )
{
  Comment = "#";
  open( file );
}


DataFile::~DataFile( void )
{
  close();
}


void DataFile::initialize( void )
{
  for ( unsigned int k=0; k<MetaData.size(); k++ ) {
    if ( MetaData[k].Data != 0 )
      delete MetaData[k].Data;
    if ( MetaData[k].Opt != 0 )
      delete MetaData[k].Opt;
  }
  MetaData.clear();
  for ( int k=0; k<LevelOffset; k++ ) {
    MetaData.push_back( MetaD( new StrQueue ) );
  }
  Level = LevelOffset;
  Count.resize( LevelOffset, 0 );
  TotalCount.resize( LevelOffset, 0 );
  Line = "";
  LineNum = 0;
  EmptyLines = 0;
  BlockNum = 0;
  KeyChanged = true;
  Data.clear();
  DataLines = 0;
}


bool DataFile::open( const istream &is )
{
  initialize();

  streambuf *sb = is.rdbuf();
  istream::rdbuf( sb );
  istream::clear( is.rdstate() );

  return ( istream::good() );
}


bool DataFile::open( const string &file )
{
  initialize();

  File.open( file.c_str() );
  streambuf *sb = File.rdbuf();
  istream::rdbuf( sb );
  istream::clear( File.rdstate() );

  return ( ! istream::fail() );
}


void DataFile::close( void )
{
  if ( File.is_open() )
    File.close();

  Line = "";
  LineNum = 0;
  EmptyLines = 0;
  BlockNum = 0;

  for ( unsigned int k=0; k<MetaData.size(); k++ ) {
    if ( MetaData[k].Data != 0 )
      delete MetaData[k].Data;
    if ( MetaData[k].Opt != 0 )
      delete MetaData[k].Opt;
  }
  MetaData.clear();
  Count.clear();
  TotalCount.clear();
  Level = LevelOffset;

  Key.clear();
  Data.clear();
  DataLines = 0;
  Dummya.clear();
  EmptyData.clear();
  EmptyOptions.clear();
}


int DataFile::readEmptyLines( void )
{
  if ( ! good() ) {
    EmptyLines = 0;
    return 0;
  }

  int n = 0;
  do {
    if ( !emptyLine() ) {
      EmptyLines = n;
      return n;
    }
    n++;
  } while ( std::getline( *this, Line ).good() &&
	    ++LineNum );

  EmptyLines = n;
  return n;
}


bool DataFile::readBlock( void )
{
  if ( ! good() )
    return false;

  StrQueue *sq = new StrQueue();

  do {

    if ( !metaLine() )
      break;
 
    // load string:
    sq->add( Line );

  } while ( std::getline( *this, Line ).good() &&
	    ++LineNum );

  readEmptyLines();

  if ( sq->empty() )
    return false;

  string key = "Key";
  int p = sq->front().first() + Comment.size();
  if ( sq->front().substr( p, key.size() ) == key ) {
    delete MetaData[ LevelOffset + DataKeyLevel ].Data;
    MetaData[ LevelOffset + DataKeyLevel ].Data = sq;
    MetaData[ LevelOffset + DataKeyLevel ].New = true;
    MetaData[ LevelOffset + DataKeyLevel ].Num = BlockNum;
    MetaData[ LevelOffset + DataKeyLevel ].Empty = EmptyLines;
    MetaData[ LevelOffset + DataKeyLevel ].Changed = true;
    Count[ LevelOffset + DataKeyLevel ]++;
    TotalCount[ LevelOffset + DataKeyLevel ]++;
    KeyChanged = true;
  }
  else if ( BlockNum == 0 && sq->front().find( "file" ) >= 0 ) {
    MetaData[ LevelOffset + DataFileLevel ].Data->add( *sq );
    MetaData[ LevelOffset + DataFileLevel ].New = true;
    MetaData[ LevelOffset + DataFileLevel ].Num = BlockNum;
    MetaData[ LevelOffset + DataFileLevel ].Empty = EmptyLines;
    MetaData[ LevelOffset + DataFileLevel ].Changed = true;
    delete sq;
    Count[ LevelOffset + DataFileLevel ]++;
    TotalCount[ LevelOffset + DataFileLevel ]++;
  }
  else {
    // erase same level meta data:
    if ( Level < (int)MetaData.size() ) {
      if ( MetaData[Level].Data != 0 )
	delete MetaData[Level].Data;
      if ( MetaData[Level].Opt != 0 )
	delete MetaData[Level].Opt;
      MetaData.erase( MetaData.begin() + Level );
    }
    // store meta data at bottom of stack:
    MetaData.insert( MetaData.begin() + 
		     LevelOffset, MetaD( sq, true, BlockNum, EmptyLines, true ) );
    // update counts:
    if ( Count.size() < MetaData.size() ) {
      Count.resize( MetaData.size(), 0 );
      TotalCount.resize( MetaData.size(), 0 );
    }
    Count[LevelOffset]++;
    TotalCount[LevelOffset]++;
    for ( int k=Level; k>LevelOffset; k-- ) {
      Count[k]++;
      Count[k-1]--;
      TotalCount[k]++;
      TotalCount[k-1]--;
    }
    // next level:
    Level++;
  }

  BlockNum++;

  return true;
}


int DataFile::readMetaData( void )
{
  for ( deque< MetaD >::iterator p = MetaData.begin();
	p != MetaData.end();
	++p )
    p->New = false;
  resetMetaDataCount();
  Level = LevelOffset;

  readEmptyLines();
  int n = 0;
  do {
    if ( readBlock() )
      n++;
  } while ( good() && metaLine() );

  return n;
}


void DataFile::resetMetaDataCount( void )
{
  for ( int k=Level-1; k >= LevelOffset; k-- )
    Count[k] = 0;
  Count[ LevelOffset + DataCommentLevel ] = 0;
  Count[ LevelOffset + DataKeyLevel ] = 0;
}


void DataFile::scanDataLine( void )
{
  if ( Data.maxRows() == 0 ) {
    Data.resize( Line.words( Str::WhiteSpace, Comment ), 50000 );
  }

  if ( Data.rows() >= Data.maxRows() ) {
    Data.reserve( 3*Data.maxRows()/2 );
  }

  int index = 0, word, k;
  for ( k=0; k<Data.columns() && index>=0; k++ ) {
    word = Line.nextWord( index, Str::WhiteSpace, Comment );

    if ( word >= 0 )
      Data.push( k, Line.number( -1.0, word ) );
  }
  for ( ; k<Data.columns(); k++ )
    Data.push( k, 0.0 );

  ++Data;
}


bool DataFile::initData( void )
{
  MetaData[ LevelOffset + DataCommentLevel ].clear();
  Data.clear();
  DataLines = 0;

  return ( good() && dataLine() );
}


bool DataFile::readDataLine( int stopempty )
{
  if ( ! good() || ! dataLine() )
    return false;

  if ( DataLines == 0 ) {
    MetaData[ LevelOffset + DataCommentLevel ].Num = BlockNum;
    BlockNum++;
  }
  DataLines++;

  std::getline( *this, Line );
  if ( ! good() )
    return false;
  ++LineNum;

  EmptyLines = 0;

  while ( metaLine() || emptyLine() ) {
    if ( emptyLine() ) {
      if ( readEmptyLines() >= stopempty )
	return false;
      if ( ! good() ) {
	EmptyLines = 1000;
	return false;
      }
    }
    else {
      addNewComment( Line );
      std::getline( *this, Line );
      if ( ! good() )
	return false;
      ++LineNum;
    }
  }

  return true;
}


int DataFile::readData( int stopempty, ScanDataFunc rf )
{
  if ( ! initData() )
    return 0;

  do {
    if ( rf != 0 )
      ((*this).*rf)();
  } while ( readDataLine( stopempty ) );

  if ( MetaData[ LevelOffset + DataCommentLevel ].New ) {
    Count[ LevelOffset + DataCommentLevel ]++;
    TotalCount[ LevelOffset + DataCommentLevel ]++;
  }

  return DataLines;
}


int DataFile::read( int stopempty, ScanDataFunc rf )
{
  readMetaData();

  return readData( stopempty, rf );
}


bool DataFile::getline( void )
{
  std::getline( *this, Line );

  if ( good() )
    ++LineNum;

  return good();
}


const StrQueue &DataFile::metaData( int level ) const
{
  level += LevelOffset;
  if ( level < 0 || level >= int( MetaData.size() ) ) {
    EmptyData.clear();
    return EmptyData;
  }
  
  return *MetaData[ level ].Data;
}


StrQueue &DataFile::metaData( int level )
{
  level += LevelOffset;
  if ( level < 0 || level >= int( MetaData.size() ) ) {
    EmptyData.clear();
    return EmptyData;
  }
  
  return *MetaData[ level ].Data;
}


const Options &DataFile::metaDataOptions( int level ) const
{
  level += LevelOffset;
  if ( level < 0 || level >= int( MetaData.size() ) ) {
    EmptyOptions.clear();
    return EmptyOptions;
  }
  
  if ( MetaData[ level ].Opt == 0 || 
       MetaData[ level ].Changed ) {
    if ( MetaData[ level ].Opt != 0 )
      delete MetaData[ level ].Opt;
    MetaData[ level ].Opt = new Options( metaData( level-LevelOffset ).strippedComments( "-" + Comment ) );
    MetaData[ level ].Changed = false;
  }

  return *MetaData[ level ].Opt;
}


Options &DataFile::metaDataOptions( int level )
{
  level += LevelOffset;
  if ( level < 0 || level >= int( MetaData.size() ) ) {
    EmptyOptions.clear();
    return EmptyOptions;
  }
  
  if ( MetaData[ level ].Opt == 0 || 
       MetaData[ level ].Changed ) {
    if ( MetaData[ level ].Opt != 0 )
      delete MetaData[ level ].Opt;
    MetaData[ level ].Opt = new Options( metaData( level-LevelOffset ).strippedComments( "-" + Comment ) );
    MetaData[ level ].Changed = false;
  }

  return *MetaData[ level ].Opt;
}


bool DataFile::newMetaData( int level ) const
{
  level += LevelOffset;
  if ( level < 0 || level >= int( MetaData.size() ) )
    return false;
  
  return MetaData[ level ].New;
}


int DataFile::metaDataIndex( int level ) const
{
  level += LevelOffset;
  if ( level < 0 || level >= int( MetaData.size() ) )
    return -1;
  
  return MetaData[ level ].Num;
}


int DataFile::emptyLines( int level ) const
{
  level += LevelOffset;
  if ( level < 0 || level >= int( MetaData.size() ) )
    return 0;
  
  return MetaData[ level ].Empty;
}


void DataFile::add( int level, const string &line )
{
  level += LevelOffset;
  if ( level < 0 || level >= int( MetaData.size() ) )
    return;
  
  MetaData[ level ].Data->add( line );
  MetaData[ level ].Changed = true;
}


void DataFile::add( int level, const StrQueue &sq )
{
  level += LevelOffset;
  if ( level < 0 || level >= int( MetaData.size() ) )
    return;
  
  MetaData[ level ].Data->add( sq );
  MetaData[ level ].Changed = true;
}


void DataFile::insert( int level, const string &line )
{
  level += LevelOffset;
  if ( level < 0 || level >= int( MetaData.size() ) )
    return;
  
  MetaData[ level ].Data->insert( line );
  MetaData[ level ].Changed = true;
}


void DataFile::insert( int level, const StrQueue &sq )
{
  level += LevelOffset;
  if ( level < 0 || level >= int( MetaData.size() ) )
    return;
  
  MetaData[ level ].Data->insert( sq );
  MetaData[ level ].Changed = true;
}


bool DataFile::emptyLine( void ) const
{
  return Line.empty();
}


bool DataFile::metaLine( void ) const
{
  int p = Line.first();
  return ( p >= 0 && Line.substr( p, Comment.size() ) == Comment );
}


bool DataFile::dataLine( void ) const
{
  int p = Line.first();
  return ( p >= 0 && Line.substr( p, Comment.size() ) != Comment );
}


const TableKey &DataFile::key( void ) const
{
  if ( KeyChanged ) {
    if ( dataKey().empty() )
      Key.clear();
    else
      Key.loadKey( dataKey() );
    KeyChanged = false;
  }
  return Key;
}

