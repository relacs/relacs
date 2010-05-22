/*
  datafile.h
  Reading Ascii Data Files.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_DATAFILE_H_
#define _RELACS_DATAFILE_H_ 1


#include <cmath>
#include <iostream>
#include <fstream>
#include <deque>
#include <relacs/str.h>
#include <relacs/strqueue.h>
#include <relacs/options.h>
#include <relacs/tablekey.h>
#include <relacs/tabledata.h>
using namespace std;

namespace relacs {


/*!
\class DataFile
\brief Reading Ascii Data Files.
\author Jan Benda
\version 1.1
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
      /*! A general description of the data file (the first block of meta data). */
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
  int levels( void ) const;
    /*! The number of of new meta data blocks. */
  int newLevels( void ) const;

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
    /*! Return a const reference to the special "File" meta data block. */
  const StrQueue &dataFile( void ) const;
    /*! Return a const reference to the special "File" meta data block converted to Options. */
  const Options &dataFileOptions( void ) const;
    /*! Return a const reference to the special "Key" meta data block. */
  const StrQueue &dataKey( void ) const;
    /*! Return a const reference to the special "Key" meta data block converted to Options. */
  const Options &dataKeyOptions( void ) const;
    /*! Return a const reference to the meta data which were read within a data block. */
  const StrQueue &dataComments( void ) const;
    /*! Return a const reference to the meta data which were read within a data block converted to Options. */
  const Options &dataCommentsOptions( void ) const;

    /*! \c True if new meta data of level \a level where read in since the
        last call of read(). */
  bool newMetaData( int level ) const;
    /*! \c True if new special "file" meta data  where read in since the
        last call of read().
        Should be \c true after the first call to read() only. */
  bool newDataFile( void ) const;
    /*! \c True if new special "key" meta data where read in since the
        last call of read(). */
  bool newDataKey( void ) const;
    /*! \c True if new meta data within the data block where read in since the
        last call of read(). */
  bool newDataComments( void ) const;

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
        The return value should be zero since the "file" meta data are always 
        the first meta data, if these meta data are present, and -1 otherwise.
        \sa metaDataIndex() */
  int dataFileIndex( void ) const;
    /*! Each block of data and meta data is numbered in the order
        they are read in from the file.
        This function returns the number of the special "key" meta data.
        \sa metaDataIndex() */
  int dataKeyIndex( void ) const;
    /*! Each block of data and meta data is numbered in the order
        they are read in from the file.
        This function returns the number of the special "data comment" meta data.
        \sa metaDataIndex() */
  int dataCommentIndex( void ) const;

    /*! Returns the number of empty line that followed 
        the block of meta data of level \a level. */
  int emptyLines( int level ) const;

    /*! The number of meta data blocks of level \a level read in after
        the last call of resetMetaDataCount(). */
  int metaDataCount( int level ) const;
    /*! The total number of meta data blocks of level \a level. */
  int metaDataTotalCount( int level ) const;
    /*! Reset the count for all meta data levels. */
  void resetMetaDataCount( void );

    /*! Add a line \a line to the meta data of level \a level. */
  void add( int level, const string &line );
    /*! Add a line \a line to the special "file" meta data. */
  void addFile( const string &line );
    /*! Add a line \a line to the special "data comments" meta data. */
  void addComment( const string &line );
    /*! Add a line \a line to the special "data comments" meta data,
        and mark them as new. */
  void addNewComment( const string &line );
    /*! Add several lines of text \a sq to the meta data of level \a level. */
  void add( int level, const StrQueue &sq );
    /*! Add several lines of text \a sq to the special "file" meta data. */
  void addFile( const StrQueue &sq );
    /*! Add several lines of text \a sq to the special "data comments" meta data. */
  void addComment( const StrQueue &sq );

    /*! Insert a line \a line at the beginning of the meta data of level \a level. */
  void insert( int level, const string &line );
    /*! Insert a line \a line at the beginning of the special "file" meta data. */
  void insertFile( const string &line );
    /*! Insert a line \a line at the beginning of the special "data comments" meta data. */
  void insertComment( const string &line );
    /*! Insert several lines of text \a sq at the beginning of the meta data of level \a level. */
  void insert( int level, const StrQueue &sq );
    /*! Insert several lines of text \a sq at the beginning of the special "file" meta data. */
  void insertFile( const StrQueue &sq );
    /*! Insert several lines of text \a sq at the beginning of the special "data comments" meta data. */
  void insertComment( const StrQueue &sq );

    /*! The current line. */
  string line( void ) const;
    /*! The current line. */
  const string &line( void );
    /*! The current line split up in its data items. */
  void splitLine( StrQueue &items, const string separators=Str::WhiteSpace ) const;
    /*! The number of read in lines. */
  int lineNum( void ) const;
    /*! The number of data lines read in by the last call of readData(). */
  int dataLines( void ) const;
    /*! The number of empty lines following the last read in block of data. */
  int emptyLines( void ) const;

    /*! The current table key. */
  const TableKey &key( void ) const;
    /*! Returns the column index whose identifier string in the key()
        matches \a pattern. The first column is 0. */
  int column( const string &pattern ) const;

  inline TableData &data( void ) { return Data; };
  inline const TableData &data( void ) const { return Data; };
  inline double data( int col, int row ) const { return Data( col, row ); };
  inline double &data( int col, int row ) { return Data( col, row ); };
  inline double data( const string &pattern, int row ) const { int c = key().column( pattern ); return c < 0 ? HUGE_VAL : Data( c, row ); };
  inline double &data( const string &pattern, int row ) { int c = key().column( pattern ); if ( c < 0 ) { Dummyd = HUGE_VAL; return Dummyd; } else return Data( c, row ); };
  inline double operator()( int col, int row ) const { return Data( col, row ); };
  inline double operator()( const string &pattern, int row ) const { int c = key().column( pattern ); return c < 0 ? HUGE_VAL : Data( c, row ); };
  inline ArrayD &col( int col ) { return Data.col( col ); };
  inline ArrayD &col( const string &pattern ) { int c = key().column( pattern ); if ( c < 0 ) { Dummya.clear(); return Dummya; } else return Data.col( c ); };
  bool newData( void ) const;

    /*! \c True if the state of the file is good for i/o operations. */
  bool good( void ) const;
    /*! \c True if the end of the file is reached. */
  bool eof( void ) const;
    /*! \c True if an error other than an End-Of-File occured. */
  bool fail( void ) const;
    /*! \c True if the state of the file is bad, 
        i.e. an unrecoverable error has occured. */
  bool bad( void ) const;
  
    /*! The string indicating a comment. */
  string comment( void ) const;
    /*! Set the string for indicating comments to \a comment. */
  void setComment( const string &comment );


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
    MetaD( void );
    MetaD( StrQueue *sq, bool n=false, int num=-1, int e=0, bool ch=false );
    MetaD( const MetaD &md );
    ~MetaD( void );
    void clear( void );
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


}; /* namespace relacs */

#endif /* ! _RELACS_DATAFILE_H_ */
