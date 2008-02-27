#include "array.h"

#ifdef GSL

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

