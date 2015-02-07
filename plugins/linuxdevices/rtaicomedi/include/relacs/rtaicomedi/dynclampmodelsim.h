/*
  dynclampmodelsim.h
  Interface to the dynamic clamp model.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2015 Jan Benda <jan.benda@uni-tuebingen.de>

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

#ifndef _RELACS_DYNCLAMPMODELSIM_H_
#define _RELACS_DYNCLAMPMODELSIM_H_ 1

#include <vector>
#include <relacs/inlist.h>
#include <relacs/tracespec.h>
using namespace std;


namespace relacs {


/*! 
\namespace dynclampmodelsim
\author Jan Benda
\brief Interface to the dynamic clamp model.
 */

namespace dynclampmodelsim {

  void initModel( void );

  void computeModel( void );

  void addAITraces( vector< TraceSpec > &traces, int deviceid );
  void addAOTraces( vector< TraceSpec > &traces, int deviceid );

  int matchAITraces( InList &traces );
  int matchAOTraces( vector< TraceSpec > &traces );


#ifdef ENABLE_LOOKUPTABLES

  /*! This function is called from DynClampAnalogOutput in user
      space/C++ context and can be used to create some lookuptables for
      nonlinear functions to be used by computeModel(). The implementation of this
      functions has to allocate an \a x and \a y array of floats of a sensible size \a n.
      \param[in] \a k : the index for the lookup table to be generated.
      \param[out] \a n : the size of the lookup table (the number of elements in \a x and \a y).
      \param[out] \a x : the x-values.
      \param[out] \a y : the corresponding y-values.
      \return: 0 if a lookuptable was generated, -1 otherwise.
  */
  int generateLookupTable( int k, float **x, float **y, int *n );

#endif


}; /* namespace dynclampmodelsim */


}; /* namespace relacs */

#endif /* ! _RELACS_DYNCLAMPMODELSIM_H_ */

