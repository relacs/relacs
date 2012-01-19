/*****************************************************************************
 *
 * array.cc
 * A template defining an one-dimensional array of data.
 *
 * RELACS
 * Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
 * Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>
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

#include <relacs/array.h>

namespace relacs {

#ifdef HAVE_LIBGSL

void gslVector( gsl_vector &a, const Array<double> &b )
{
  a.size = b.size();
  a.stride = 1;
  a.data = const_cast<double *>( b.data() );
  a.block = NULL;
  /*
  // this will not deleted after destroying the gsl_vector!
  a.block = new gsl_block;
  a.block->size = b.size();
  a.block->data = b.data();
  */
  a.owner = 0;
}


void gslVector( gsl_vector_float &a, const Array<float> &b )
{
  a.size = b.size();
  a.stride = 1;
  a.data = const_cast<float *>( b.data() );
  a.block = NULL;
  a.owner = 0;
}


void gslVector( gsl_vector_int &a, const Array<int> &b )
{
  a.size = b.size();
  a.stride = 1;
  a.data = const_cast<int *>( b.data() );
  a.block = NULL;
  a.owner = 0;
}

#endif


}; /* namespace relacs */

