/*
  tabledata.cc
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

#include <relacs/tablekey.h>
#include <relacs/tabledata.h>

namespace relacs {


TableData::TableData( void )
{
  Rows = 0;
}


TableData::TableData( int cols, int rows )
{
  Rows = 0;
  resize( cols, rows );
}


TableData::TableData( const TableKey &key, int rows )
{
  Rows = 0;
  resize( key.columns(), rows );
}


TableData::TableData( const TableData &table )
{
  assign( table );
}


TableData::~TableData( void )
{
  clear();
}


void TableData::resize( int cols, int rows )
{
  Data.resize( cols, ArrayD() );
  for ( unsigned int k=0; k<Data.size(); k++ )
    Data[k].reserve( rows );
  Rows = 0;
}


void TableData::resize( int rows )
{
  if ( rows < 0 )
    rows = 0;
  for ( unsigned int k=0; k<Data.size(); k++ )
    Data[k].resize( rows );
  Rows = rows;
}


void TableData::reserve( int rows )
{
  for ( unsigned int k=0; k<Data.size(); k++ )
    Data[k].reserve( rows );
}


void TableData::clear( void )
{
  for ( unsigned int k=0; k<Data.size(); k++ )
    Data[k].clear();
  Rows = 0;
}


TableData &TableData::operator=( const TableData &table )
{
  if ( &table != this )
    assign( table );

  return *this;
}


void TableData::assign( const TableData &table )
{
  Data.resize( table.columns(), ArrayD() );
  for ( unsigned int k=0; k<Data.size(); k++ )
    Data[k] = table.col( k );
  Rows = table.rows();
}


ArrayD TableData::row( int r ) const
{
  ArrayD data( columns() );
  for ( int c=0; c<columns(); c++ )
    data[c] = Data[c][r];
  return data;
}


void TableData::erase( int row )
{
  if ( row < 0 || row >= rows() )
    return;

  for ( int c=0; c<columns(); c++ )
    Data[c].erase( row );
  Rows--;
}


TableData &TableData::sort( int scol )
{
  // get sorted index:
  ArrayI indices;
  col( scol ).sortedIndex( indices, 0, Rows );
  if ( indices.empty() )
    return *this;

  ArrayD wksp;
  wksp.reserve( Rows );
  for ( int k=0; k<columns(); k++ ) {
    wksp = col( k );
    for ( int j=0; j<Rows; j++ )
      Data[k][j] = wksp[ indices[j] ];
  }

  return *this;
}


}; /* namespace relacs */

