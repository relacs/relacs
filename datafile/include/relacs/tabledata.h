/*
  tabledata.h
  Table for number and strings

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _TABLEDATA_H_
#define _TABLEDATA_H_


#include <fstream>
#include <vector>
#include <relacs/array.h>
using namespace std;

class TableKey;

/*!
\class TableData
\author Jan Benda
\version 1.0
\brief Table for number and strings
\bug This is just a quick shot! Feel free to make it better!
*/


class TableData
{

public:

  TableData( void );
  /*! Create a table with \a cols columns and 
      reserve space for \a rows rows. */
  TableData( int cols, int rows=1000 );
  /*! Create a table with tha same number of columns as \a key and 
      reserve space for \a rows rows. */
  TableData( const TableKey &key, int rows=1000 );
    /*! Copy constructor. */
  TableData( const TableData &table );
  ~TableData( void );

  int size( void ) const { return columns(); };
  int rows( void ) const { return Rows; };
  int maxRows( void ) const { return Data.empty() ? 0 : Data[0].capacity(); };
  int columns( void ) const { return Data.size(); };

  void resize( int cols, int rows );
  void resize( int rows );
  void reserve( int rows );
    /*! Clear the rows, but keep the number of columns. */
  void clear( void );

    /*! Copy \a table to \a *this. */
  TableData &operator=( const TableData &table );
    /*! Make \a *this a copy of \a table. */
  void assign( const TableData &table );

  inline double &operator()( int col, int row ) { return Data[col][row]; };
  inline const double &operator()( int col, int row ) const { return Data[col][row]; };

  inline ArrayD &col( int col ) { return Data[col]; };
  inline const ArrayD &col( int col ) const { return Data[col]; };
  inline ArrayD &operator[]( int col ) { return Data[col]; };
  inline const ArrayD &operator[]( int col ) const { return Data[col]; };
  ArrayD row( int row ) const;

  inline void push( int col, double val ) { Data[col].push( val ); };

  const TableData &operator++( void ) { Rows++; return *this; };
  const TableData &operator+=( int r ) { Rows+=r; return *this; };

    /*! Sort rows by column \a scol. */
  TableData &sort( int scol );


private:

  vector< ArrayD > Data;
  int Rows;

};


#endif
