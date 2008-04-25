/*
  reproexample.h
  A simple example showing how to program your own RePro.

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

#ifndef _REPROEXAMPLE_H_
#define _REPROEXAMPLE_H_


#include <relacs/repro.h>

/*!
\class ReProExample
\author Jan Benda
\brief A simple example showing how to program your own RePro.
*/


class ReProExample : public RePro
{
  Q_OBJECT

public:

    /*! Constructs a ReProExample-RePro: intialize widgets and create options. */
  ReProExample( void );
    /*! Deconstructs a ReProExample-RePro. */
  ~ReProExample( void );

    /*! Run the RePro */
  virtual void main( void );

};


#endif
