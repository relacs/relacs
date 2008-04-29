/*****************************************************************************
 *
 * translate.h
 * 
 *
 * RELACS
 * RealTime ELectrophysiological data Acquisition, Control, and Stimulation
 * Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * RELACS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#ifndef _RELACS_TRANSLATE_H_
#define _RELACS_TRANSLATE_H_ 1


#include <relacs/str.h>
#include <relacs/strqueue.h>
#include <relacs/options.h>
#include "tablekey.h"


  /*! Returns the string \a s with each occurence of a pattern $(xxxx) 
      substituted by the value of the identifier xxxx found in
      one of the options \a opt.
      A pattern of the form $(o xxxx) searches xxxx in the options \a opt
      that corresponds to the character o in \a ts.
      The first character in \a ts refers to the first options in \a opt,
      the second character to the second option, and so on.
      If \a tkey is not null then the last character in \a ts refers to
      the table key \a tkey.
      For example, if \a ts = "pst" and the pattern is $(s xxx)
      then xxx is searched in the second options in \a opt.
      If \a ts is not specified, then it is set to "0123456789".
      In addition a format string fff and a unit uuu can be specified
      that is separated from the search string xxx by a single white space:
      $(fffuuu xxxx) $(offfuuu xxx).
      The format string starts with '%' followed by the width, the precision
      and the conversion specifier, like for the printf function in C.
      For example: $(%.3fkHz frequency).
      See Parameter::text() for a complete list of format specifiers.
      A default string is specified with ':' directly following
      the format and unit.
      For example: $(%.3fkHz:10 frequency) would use '10' as the default
      string if 'frequency' is not found in \a opt.
      If the specified default string contains blank spaces,
      you need to enclose it with '"':
      $(:"no value found" frequency)
      If no default string is explicitly specified in the search pattern
      \a dflt is used.
      A pattern refering to a table key \a tkey returns the 
      corresponding column number (0 corresponds to the first column,
      if the format string equals '+', then 1 corresponds to the first column.
      If the format string equals "+nn", where nn is some integer,
      then nn corresponds to the first column.
      For example: ts="123k" $(k population>mean) returns the column number
      of the column labeled "population>mean" 
      (see TableKey::column() for details of column labels).
   */
Str translate( const Str &s, vector< Options > &opt, 
	       const string &ts="", const string &dflt="",
	       const TableKey *tkey=0 );
Str translate( const Str &s, Options &opt, const string &dflt="" );

  /*! Returns in \a dq the StrQueue \a sq with each occurence of a pattern $(xxxx) 
      substituted by the value of the identifier xxxx found in
      one of the options \a opt.
      A pattern of the form $(o xxxx) searches xxxx in the options \a opt
      that corresponds to the character o in \a ts.
      The first character in \a ts refers to the first options in \a opt,
      the second character to the second option, and so on.
      For example, if \a ts = "pst" and the pattern is $(s xxx)
      then xxx is searched in the second options in \a opt.
      If \a ts is not specified, then it is set to "0123456789".
      In addition a format string fff and a unit uuu can be specified
      that is separated from the search string xxx by a single white space:
      $(fffuuu xxxx) $(offfuuu xxx).
      The format string starts with '%' followed by the width, the precision
      and the conversion specifier, like for the printf function in C.
      For example: $(%.3fkHz frequency)
      A default string is specified with ':' directly following
      the format and unit.
      For example: $(%.3fkHz:10 frequency) would use '10' as the default
      string if 'frequency' is not found in \a opt.
      If no default string is explicitly specified in the search pattern
      \a dflt is used.
      \sa Str::translate()
   */
void translate( const StrQueue &sq, StrQueue &dq,
		vector< Options > &opt, const string &ts="", 
		const string &dflt="", const TableKey *tkey=0 );


#endif /* ! _RELACS_TRANSLATE_H_ */
