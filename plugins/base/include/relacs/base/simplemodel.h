/*
  base/simplemodel.h
  A toy model for testing.

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

#ifndef _RELACS_BASE_SIMPLEMODEL_H_
#define _RELACS_BASE_SIMPLEMODEL_H_ 1

#include <relacs/model.h>
using namespace relacs;

namespace base {


/*! 
\class SimpleModel
\brief [Model] A toy model for testing.
\author Jan Benda
\version 1.1 (Jan 31, 2008)
-# added model option
\version 1.0 (Jan 5, 2006)
*/


class SimpleModel : public Model
{
  Q_OBJECT

public:

  SimpleModel( void );
  ~SimpleModel( void );

  virtual void main( void );

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_SIMPLEMODEL_H_ */
