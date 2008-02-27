/*
  datafile.h
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

#ifndef _DATAFILE_H_
#define _DATAFILE_H_


#include <cmath>
#include <iostream>
#include <fstream>
#include <deque>
#include "str.h"
#include "strqueue.h"
#include "options.h"
#include "tablekey.h"
#include "tabledata.h"
using namespace std;

/*!
\class DataFile
\brief Reading Ascii Data Files.
\author Jan Benda
\version 1.1
\todo make the special file meta data block a copy of the first (or topmost) meta data block!
*/


class DataFile : public istream
{

public:

  typedef void (DataFile::*ScanDataFunc)( void );

    /*! Special meta data levels. */
  enum Levels {
      /*! A comment within a block of data. */
    DataCommentLevel=-3,
      /*! A key describing the data. */
    DataKeyLevel=-2,
      /*! A generla description of the data file. */
    DataFileLevel=-1
  };

  DataFile( void );
  DataFile( const istream &is );
  DataFile( const string &file );
  ~DataFile( void );

    /*! Open stream \a is for reading.
        \return \c true on success. */
  bool open( const istream &is );
    /*! Open file \a file for reading.
        \return \c true on success. */
  bool open( const string &file );
    /*! Close file and clear all data buffers. */
  void close( void );

    /*! Read in successive empty line.
        \return number of read in empty lines. Zero if there wasn't any empty line. */
  int readEmptyLines( void );

    /*! Read a single block of meta data and the following empty lines. 
        \return \c true on success, 
	i.e. a block of meta data was at the current position in the file.*/
  bool readBlock( void );
    /*! Read all blocks of meta data until the data block is reached. 
        \return the number of read in meta data blocks. */
  int readMetaData( void );

    /*! Init member variables for a new block of data.
        Use this function before using readDataLine(). */
  bool initData( void );
    /*! Read lines until the next data line. 
        Comment lines are stored as Data Comments.
        Empty lines are skipped.
        If \a stopempty or more empty lines are encountered
        \c false is returned. 
	Use this function together with initData() in the following way:
        \code
	DataFile sf( "test.dat" );
	while ( sf.readMetaData() ) {
          // ... process meta data ...
          sf.initData();
          do {
            // ... process data line ...
            cout << sf.line() << endl;  // just print it out
          } while ( sf.readDataLine( 1 ) );
          // ... process data block ...
          cout << "read " << sf.dataLines() << " lines of data" << endl;
        }
        sf.close();
        \endcode */
  bool readDataLine( int stopempty );
    /*! Extracts the numbers of the current line. */
  void scanDataLine( void );
    /*! Read in a block of data,
        until \a stopempty empty lines are encountered.
        \return the number of data lines that have been read.
        Uses the function \a rdf for processing the data. */
  int readData( int stopempty, ScanDataFunc rdf=&DataFile::scanDataLine );

    /*! Read all metadata and the following data, 
        until \a stopempty empty lines are encountered.
        \return the number of data lines that have been read. */
  int read( int stopempty=1, ScanDataFunc rdf=&DataFile::scanDataLine );

    /*! Read a single line.
        The content of the line is accesible by line(). */
  bool getline( void );

    /*! \c True if the current line is an empty line. */
  bool emptyLine( void ) const;
    /*! \c True if the current line contains meta data, 
        i.e. a line that starts with the comment character. 
        \sa comment(), setComment() */
  bool metaLine( void ) const;
    /*! \c True if the current line contains data,
        i.e. a line that is not empty and does not start with
        the comment character. 
        \sa emptyLine(), metaLine() */
  bool dataLine( void ) const;

    /*! The number of meta data blocks in the buffer. */
  inline int levels( void ) const { return MetaData.size() - LevelOffset; };
    /*! The number of of new meta data blocks. */
  inline int newLevels( void ) const { return Level - LevelOffset; };

    /*! Returns a const reference to the metaData of level \a level.
        If the requested level does not exist,
        an empty StrQueue is returned. */
  const StrQueue &metaData( int level ) const;
    /*! Returns a reference to the metaData of level \a level.
        If the requested level does not exist,
        an empty StrQueue is returned. */
  StrQueue &metaData( int level );
    /*! Returns a const reference to the metaData of level \a level
        converted to Options.
        If the requested level does not exist,
        an empty Options is returned. */
  const Options &metaDataOptions( int level ) const;
    /*! Returns a reference to the metaData of level \a level
        converted to Options.
        If the requested level does not exist,
        an empty Options is returned. */
  Options &metaDataOptions( int level );
    /*! Return a const reference to the special "file" meta data block. */
  inline const StrQueue &dataFile( void ) const { return metaData( DataFileLevel ); };
    /*! Return a const reference to the special "file" meta data block converted to Options. */
  inline const Options &dataFileOptions( void ) const { return metaDataOptions( DataFileLevel ); };
    /*! Return a const reference to the special "Key" meta data block. */
  inline const StrQueue &dataKey( void ) const { return metaData( DataKeyLevel ); };
    /*! Return a const reference to the special "Key" meta data block converted to Options. */
  inline const Options &dataKeyOptions( void ) const { return metaDataOptions( DataKeyLevel ); };
    /*! Return a const reference to the meta data which were read within a data block. */
  inline const StrQueue &dataComments( void ) const { return metaData( DataCommentLevel ); };
    /*! Return a const reference to the meta data which were read within a data block converted to Options. */
  inline const Options &dataCommentsOptions( void ) const { return metaDataOptions( DataCommentLevel ); };

    /*! \c True if new meta data of level \a level where read in since the
        last call of read(). */
  bool newMetaData( int level ) const;
    /*! \c True if new special "file" meta data  where read in since the
        last call of read().
        Should be \c true after the first call to read() only. */
  inline bool newDataFile( void ) const { return newMetaData( DataFileLevel ); };
    /*! \c True if new special "key" meta data where read in since the
        last call of read(). */
  inline bool newDataKey( void ) const { return newMetaData( DataKeyLevel ); };
    /*! \c True if new meta data within the data block where read in since the
        last call of read(). */
  inline bool newDataComments( void ) const { return newMetaData( DataCommentLevel ); };

    /*! Each block of data and meta data is numbered in the order
        they are read in from the file.
        This function returns the number of the current block of meta data
        of level \a level.
        If the requested meta data do not exist, -1 is returned.
        \sa dataFileIndex(), dataKeyIndex(), dataCommentIndex() */
  int metaDataIndex( int level ) const;
    /*! Each block of data and meta data is numbered in the order
        they are read in from the file.
        This function returns the number of the special "file" meta data.
        The return value should be zero, if these meta data are present,
        and -1 otherwise.
        \sa metaDataIndex() */
  inline int dataFileIndex( void ) const { return metaDataIndex( DataFileLevel ); };
    /*! Each block of data and meta data is numbered in the order
        they are read in from the file.
        This function returns the number of the special "key" meta data.
        \sa metaDataIndex() */
  inline int dataKeyIndex( void ) const { return metaDataIndex( DataKeyLevel ); };
    /*! Each block of data and meta data is numbered in the order
        they are read in from the file.
        This function returns the number of the special "data comment" meta data.
        \sa metaDataIndex() */
  inline int dataCommentIndex( void ) const { return metaDataIndex( DataCommentLevel ); };

    /*! Returns the number of empty line that followed 
        the block of meta data of level \a level. */
  int emptyLines( int level ) const;

    /*! The number of meta data blocks of level \a level read in after
        the last call of resetMetaDataCount(). */
  int metaDataCount( int level ) const { return Count[LevelOffset+level]; };
    /*! The total number of meta data blocks of level \a level. */
  int metaDataTotalCount( int level ) const { return TotalCount[LevelOffset+level]; };
    /*! Reset the count for all meta data levels. */
  void resetMetaDataCount( void );

    /*! Add a line \a line to the meta data of level \a level. */
  void add( int level, const string &line );
    /*! Add a line \a line to the special "file" meta data. */
  inline void addFile( const string &line ) { add( DataFileLevel, line ); };
    /*! Add a line \a line to the special "data comments" meta data. */
  inline void addComment( const string &line ) { add( DataCommentLevel, line ); };
    /*! Add a line \a line to the special "data comments" meta data,
        and mark them as new. */
  inline void addNewComment( const string &line ) { add( DataCommentLevel, line ); MetaData[ 0 ].New = true; MetaData[ 0 ].Changed = true; };
    /*! Add several lines of text \a sq to the meta data of level \a level. */
  void add( int level, const StrQueue &sq );
    /*! Add several lines of text \a sq to the special "file" meta data. */
  inline void addFile( const StrQueue &sq ) { add( DataFileLevel, sq ); };
    /*! Add several lines of text \a sq to the special "data comments" meta data. */
  inline void addComment( const StrQueue &sq ) { add( DataCommentLevel, sq ); };

    /*! Insert a line \a line at the beginning of the meta data of level \a level. */
  void insert( int level, const string &line );
    /*! Insert a line \a line at the beginning of the special "file" meta data. */
  inline void insertFile( const string &line ) { insert( DataFileLevel, line ); };
    /*! Insert a line \a line at the beginning of the special "data comments" meta data. */
  inline void insertComment( const string &line ) { insert( DataCommentLevel, line ); };
    /*! Insert several lines of text \a sq at the beginning of the meta data of level \a level. */
  void insert( int level, const StrQueue &sq );
    /*! Insert several lines of text \a sq at the beginning of the special "file" meta data. */
  inline void insertFile( const StrQueue &sq ) { insert( DataFileLevel, sq ); };
    /*! Insert several lines of text \a sq at the beginning of the special "data comments" meta data. */
  inline void insertComment( const StrQueue &sq ) { insert( DataCommentLevel, sq ); };

    /*! The current line. */
  string line( void ) const { return Line; };
    /*! The current line. */
  const string &line( void ) { return Line; };
    /*! The number of read in lines. */
  int lineNum( void ) const { return LineNum; };
    /*! The number of data lines read in by the last call of readData(). */
  int dataLines( void ) const { return DataLines; };
    /*! The number of lines of the last read in block of empty lines. */
  int emptyLines( void ) const { return EmptyLines; };

    /*! The current table key. */
  const TableKey &key( void ) const;
    /*! Returns the column index whose identifier string in the key()
        matches \a pattern. The first column is 0. */
  inline int column( const string &pattern ) const { return key().column( pattern ); };

  TableData &data( void ) { return Data; };
  const TableData &data( void ) const { return Data; };
  inline double data( int col, int row ) const { return Data( col, row ); };
  inline double &data( int col, int row ) { return Data( col, row ); };
  inline double data( const string &pattern, int row ) const { int c = key().column( pattern ); return c < 0 ? HUGE_VAL : Data( c, row ); };
  inline double &data( const string &pattern, int row ) { int c = key().column( pattern ); if ( c < 0 ) { Dummyd = HUGE_VAL; return Dummyd; } else return Data( c, row ); };
  inline double operator()( int col, int row ) const { return Data( col, row ); };
  inline double operator()( const string &pattern, int row ) const { int c = key().column( pattern ); return c < 0 ? HUGE_VAL : Data( c, row ); };
  inline ArrayD &col( int col ) { return Data.col( col ); };
  inline ArrayD &col( const string &pattern ) { int c = key().column( pattern ); if ( c < 0 ) { Dummya.clear(); return Dummya; } else return Data.col( c ); };
  bool newData( void ) const { return ( DataLines > 0 ); };

    /*! \c True if the state of the file is good for i/o operations. */
  inline bool good( void ) const { return istream::good(); };
    /*! \c True if the end of the file is reached. */
  inline bool eof( void ) const { return istream::eof(); };
    /*! \c True if an error other than an End-Of-File occured. */
  inline bool fail( void ) const { return istream::fail(); };
    /*! \c True if the state of the file is bad, 
        i.e. an unrecoverable error has occured. */
  inline bool bad( void ) const { return istream::bad(); };
  
    /*! The string indicating a comment. */
  string comment( void ) const { return Comment; };
    /*! Set the string for indicating comments to \a comment. */
  void setComment( const string &comment ) { Comment = comment; };


private:

  void initialize( void );

  ifstream File;

  Str Line;
  int LineNum;
  int DataLines;
  int BlockNum;
  string Comment;

  struct MetaD
  {
    MetaD( void ) : Data( 0 ), New( false ), Num( -1 ), Empty( 0 ), 
		    Opt( 0 ), Changed( false ) {};
    MetaD( StrQueue *sq, bool n=false, int num=-1, int e=0, bool ch=false ) 
      : Data( sq ), New( n ), Num( num ), Empty( e ), 
	Opt( 0 ), Changed( ch ) {};
    MetaD( const MetaD &md ) 
      : Data( md.Data ), New( md.New ), Num( md.Num ), Empty( md.Empty ), 
	Opt( md.Opt ), Changed( md.Changed ) {};
    ~MetaD( void ) {};
    void clear( void )
    { if ( Data != 0 ) Data->clear(); if ( Opt != 0 ) { Opt->clear(); Changed = true; } };
    StrQueue *Data;
    bool New;
    int Num;
    int Empty;
    mutable Options *Opt;
    mutable bool Changed;
  };
  deque< MetaD > MetaData;
  int Level;
  static int LevelOffset;
  vector< int > Count;
  vector< int > TotalCount;
  int EmptyLines;

  mutable TableKey Key;
  mutable bool KeyChanged;

  TableData Data;
  mutable double Dummyd;
  mutable ArrayD Dummya;

  mutable StrQueue EmptyData;
  mutable Options EmptyOptions;
 
};


#endif
