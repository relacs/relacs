/*
  containerops.h
  

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_CONTAINEROPS_H_
#define _RELACS_CONTAINEROPS_H_ 1

namespace relacs {


///////////// unary class member operators //////////////////////////////////

/* Generates declarations for unary class member operators
   that take scalars as argument.
   \a CONTAINERTYPE is the return type (the class) and
   \a COP is the operator name (like operator+= ). */
#define CONTAINEROPS1SCALARDEC( CONTAINERTYPE, COP )		\
  const CONTAINERTYPE &COP( float x );				\
  const CONTAINERTYPE &COP( double x );				\
  const CONTAINERTYPE &COP( long double x );			\
  const CONTAINERTYPE &COP( signed char x );			\
  const CONTAINERTYPE &COP( unsigned char x );			\
  const CONTAINERTYPE &COP( signed int x );			\
  const CONTAINERTYPE &COP( unsigned int x );			\
  const CONTAINERTYPE &COP( signed long x );			\
  const CONTAINERTYPE &COP( unsigned long x );			\


/* Generates declarations for unary class member operators
   that take other container or scalars as argument.
   \a CONTAINERTYPE is the return type (the class) and
   \a COP is the operator name (like operator+= ). */
#define CONTAINEROPS1DEC( CONTAINERTYPE, COP )		\
  template< class COT >					\
  const CONTAINERTYPE &COP( const COT &x );		\
							\
  CONTAINEROPS1SCALARDEC( CONTAINERTYPE, COP )		\


/* Used by macro CONTAINEROPS1SCALARDEF to generate
   definitions for unary class member operators 
   that take a scalar as argument. 
   \a CONTAINERTEMPL is a template definition (like template< class T > ),
   \a CONTAINERTYPE is the return type (the class),
   \a COPNAME is the operator name (like operator+= ),
   \a COP is the operator (like += ), and
   \a SCALAR is the type of the scalar argument. */
#define CONTAINEROPS1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, SCALAR ) \
  CONTAINERTEMPL							\
  const CONTAINERTYPE &CONTAINERTYPE::COPNAME( SCALAR x )		\
  {									\
    iterator iter1 = begin();						\
    iterator end1 = end();						\
    while ( iter1 != end1 ) {						\
      (*iter1) COP static_cast< value_type >( x );			\
      ++iter1;								\
    };									\
    return *this;							\
  }									\


/* Generates definitions for unary class member operators. 
   that take scalars as argument.
   \a CONTAINERTEMPL is a template definition (like template< class T > ),
   \a CONTAINERTYPE is the return type (the class),
   \a COPNAME is the operator name (like operator+= ), and
   \a COP is the operator name (like += ). */
#define CONTAINEROPS1SCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP ) \
  CONTAINEROPS1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, float ) \
    CONTAINEROPS1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, double ) \
    CONTAINEROPS1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, long double ) \
    CONTAINEROPS1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, signed char ) \
    CONTAINEROPS1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, unsigned char ) \
    CONTAINEROPS1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, signed int ) \
    CONTAINEROPS1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, unsigned int ) \
    CONTAINEROPS1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, signed long ) \
    CONTAINEROPS1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, unsigned long ) \


/* Generates definitions for unary class member operators. 
   \a CONTAINERTEMPL is a template definition (like template< class T > ),
   \a CONTAINERTYPE is the return type (the class),
   \a COPNAME is the operator name (like operator+= ), and
   \a COP is the operator name (like += ). */
#define CONTAINEROPS1DEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP ) \
  CONTAINERTEMPL template < class COT >					\
  const CONTAINERTYPE &CONTAINERTYPE::COPNAME ( const COT &x )		\
  {									\
    iterator iter1 = begin();						\
    iterator end1 = end();						\
    typename COT::const_iterator iter2 = x.begin();			\
    typename COT::const_iterator end2 = x.end();			\
    while ( iter1 != end1 && iter2 != end2 ) {				\
      (*iter1) COP static_cast< value_type >(*iter2);			\
      ++iter1;								\
      ++iter2;								\
    };									\
    return *this;							\
  }									\
									\
  CONTAINEROPS1SCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP ) \


//////////// binary class friend operators ///////////////////////////////////


/* Generates declarations for binary class friend operators
   that take the class and a scalar as argument.
   \a CONTAINERTEMPL is a template definition (like template< class T > ),
   \a CONTAINERTYPE is the return type and one of the arguments (like Array<T>),
   and \a COP is the operator name (like operator+ ). */
#define CONTAINEROPS2SCALARDEC( CONTAINERTEMPL, CONTAINERTYPE, COP )	\
  CONTAINERTEMPL friend CONTAINERTYPE COP( float x, const CONTAINERTYPE &y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( const CONTAINERTYPE &x, float y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( double x, const CONTAINERTYPE &y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( const CONTAINERTYPE &x, double y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( long double x, const CONTAINERTYPE &y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( const CONTAINERTYPE &x, long double y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( signed char x, const CONTAINERTYPE &y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( const CONTAINERTYPE &x, signed char y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( unsigned char x, const CONTAINERTYPE &y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( const CONTAINERTYPE &x, unsigned char y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( signed int x, const CONTAINERTYPE &y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( const CONTAINERTYPE &x, signed int y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( unsigned int x, const CONTAINERTYPE &y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( const CONTAINERTYPE &x, unsigned int y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( signed long x, const CONTAINERTYPE &y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( const CONTAINERTYPE &x, signed long y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( unsigned long x, const CONTAINERTYPE &y ); \
  CONTAINERTEMPL friend CONTAINERTYPE COP( const CONTAINERTYPE &x, unsigned long y ); \


/* Generates declarations for binary class friend operators
   that take other containers as the second argument or a scalar as either argument.
   \a CONTAINERTEMPL is a template type definition needed for CONTAINERTYPE (like class TT ),
   \a CONTAINERTYPE is the type of first argument and the return type (like Array<TT>) and
   \a COP is the operator name (like operator+ ). */
#define CONTAINEROPS2DEC( CONTAINERTEMPL, CONTAINERTYPE, COP )		    \
  template < CONTAINERTEMPL, typename COT >				    \
    friend CONTAINERTYPE COP( const CONTAINERTYPE &x,  const COT &y );	    \
									    \
  CONTAINEROPS2SCALARDEC( template < CONTAINERTEMPL >, CONTAINERTYPE, COP ) \


/* Used by macro CONTAINEROPS2SCALARDEF to generate
   definitions for binary class friend operators 
   that take the class and a scalar as argument. 
   \a CONTAINERTEMPL is a template definition (like template< class T > ),
   \a CONTAINERTYPE is the return type (the class),
   \a COPNAME is the operator name (like operator+ ),
   \a COP is the operator (like + ), and
   \a SCALAR is the type of the scalar argument. */
#define CONTAINEROPS2SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, SCALAR ) \
  CONTAINERTEMPL							\
  CONTAINERTYPE COPNAME( SCALAR x, const CONTAINERTYPE &y )		\
  {									\
    CONTAINERTYPE z( y );						\
    typename CONTAINERTYPE::iterator iter1 = z.begin();			\
    typename CONTAINERTYPE::iterator end1 = z.end();			\
    while ( iter1 != end1 ) {						\
      (*iter1) = static_cast< typename CONTAINERTYPE::value_type >( x COP (*iter1) ); \
      ++iter1;								\
    };									\
    return z;								\
  }									\
									\
  CONTAINERTEMPL							\
  CONTAINERTYPE COPNAME( const CONTAINERTYPE &x, SCALAR y )		\
  {									\
    CONTAINERTYPE z( x );						\
    typename CONTAINERTYPE::iterator iter1 = z.begin();			\
    typename CONTAINERTYPE::iterator end1 = z.end();			\
    while ( iter1 != end1 ) {						\
      (*iter1) = static_cast< typename CONTAINERTYPE::value_type >( (*iter1) COP y ); \
      ++iter1;								\
    };									\
    return z;								\
  }									\


/* Generates definitions for binary class member operators. 
   that take scalars as argument.
   \a CONTAINERTEMPL is a template definition (like template< class T > ),
   \a CONTAINERTYPE is the return type (the class),
   \a COPNAME is the operator name (like operator+ ), and
   \a COP is the operator name (like + ). */
#define CONTAINEROPS2SCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP ) \
  CONTAINEROPS2SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, float ) \
    CONTAINEROPS2SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, double ) \
    CONTAINEROPS2SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, long double ) \
    CONTAINEROPS2SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, signed char ) \
    CONTAINEROPS2SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, unsigned char ) \
    CONTAINEROPS2SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, signed int ) \
    CONTAINEROPS2SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, unsigned int ) \
    CONTAINEROPS2SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, signed long ) \
    CONTAINEROPS2SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP, unsigned long ) \


/* Generates definitions for binary class friend operators
   that take other containers as the second argument or a scalar as either argument.
   \a CONTAINERTEMPL is a template type definition needed for CONTAINERTYPE (like class TT ),
   \a CONTAINERTYPE is the type of first argument and the return type (like Array<TT>) and
   \a COPNAME is the operator name (like operator+ ), and
   \a COP is the operator name (like += ). */
#define CONTAINEROPS2DEF( CONTAINERTEMPL, CONTAINERTYPE, COPNAME, COP ) \
  template < CONTAINERTEMPL, typename COT >				\
    CONTAINERTYPE COPNAME( const CONTAINERTYPE &x, const COT &y )	\
  {									\
    CONTAINERTYPE z( x );						\
    typename CONTAINERTYPE::iterator iter1 = z.begin();			\
    typename CONTAINERTYPE::iterator end1 = z.end();			\
    typename COT::const_iterator iter2 = y.begin();			\
    typename COT::const_iterator end2 = y.end();			\
    while ( iter1 != end1 && iter2 != end2 ) {				\
      (*iter1) = (*iter1) COP (*iter2);					\
      ++iter1;								\
      ++iter2;								\
    };									\
    return z;								\
  }									\
									\
  CONTAINEROPS2SCALARDEF( template< CONTAINERTEMPL >, CONTAINERTYPE, COPNAME, COP ) \



/////// one argument container function definition //////////////////////////

/* Generates declaration for a container member function that applies
   a function to each element of the container.
   \a CONTAINERTEMPL is a template type definition needed for CONTAINERTYPE (like template< class TT >),
   \a CONTAINERTYPE is the container class (like Array<TT>) and
   \a FUNC is the function name (like sin ), and
   \a CFUNC is the name of the C function for the scalar (like sin ). */
#define CONTAINERFUNC0DEF( CONTAINERTEMPL, CONTAINERTYPE, FUNC, CFUNC )	\
  CONTAINERTEMPL							\
  CONTAINERTYPE &CONTAINERTYPE::FUNC( void )				\
  {									\
    iterator iter1 = begin();						\
    iterator end1 = end();						\
    while ( iter1 != end1 ) {						\
      (*iter1) = CFUNC(*iter1);						\
      ++iter1;								\
    };									\
    return *this;							\
  }									\



///////////// unary class member functions //////////////////////////////////


/* Generates declarations for unary class member functions
   that take scalars as argument.
   \a CONTAINERTYPE is the return type (the class) and
   \a CFUNC is the operator name (like pow ). */
#define CONTAINERFUNC1SCALARDEC( CONTAINERTYPE, CFUNC )		\
  const CONTAINERTYPE &CFUNC( float x );				\
  const CONTAINERTYPE &CFUNC( double x );				\
  const CONTAINERTYPE &CFUNC( long double x );			\
  const CONTAINERTYPE &CFUNC( signed char x );			\
  const CONTAINERTYPE &CFUNC( unsigned char x );			\
  const CONTAINERTYPE &CFUNC( signed int x );			\
  const CONTAINERTYPE &CFUNC( unsigned int x );			\
  const CONTAINERTYPE &CFUNC( signed long x );			\
  const CONTAINERTYPE &CFUNC( unsigned long x );			\


/* Generates declarations for unary class member functions
   that take other container or scalars as argument.
   \a CONTAINERTYPE is the return type (the class) and
   \a CFUNC is the operator name (like pow ). */
#define CONTAINERFUNC1DEC( CONTAINERTYPE, CFUNC )		\
  template< class COT >					\
  const CONTAINERTYPE &CFUNC( const COT &x );		\
							\
  CONTAINERFUNC1SCALARDEC( CONTAINERTYPE, CFUNC )		\


/* Used by macro CONTAINERFUNC1SCALARDEF to generate
   definitions for unary class member functions 
   that take a scalar as argument. 
   \a CONTAINERTEMPL is a template definition (like template< class T > ),
   \a CONTAINERTYPE is the return type (the class),
   \a CFUNCNAME is the operator name (like pow ),
   \a CFUNC is the operator (like += ), and
   \a SCALAR is the type of the scalar argument. */
#define CONTAINERFUNC1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, CFUNCNAME, CFUNC, SCALAR ) \
  CONTAINERTEMPL							\
  const CONTAINERTYPE &CONTAINERTYPE::CFUNCNAME( SCALAR x )		\
  {									\
    iterator iter1 = begin();						\
    iterator end1 = end();						\
    while ( iter1 != end1 ) {						\
      (*iter1) = static_cast< value_type >( CFUNC( *iter1, x ) );			\
      ++iter1;								\
    };									\
    return *this;							\
  }									\


/* Generates definitions for unary class member functions. 
   that take scalars as argument.
   \a CONTAINERTEMPL is a template definition (like template< class T > ),
   \a CONTAINERTYPE is the return type (the class),
   \a CFUNCNAME is the operator name (like pow ), and
   \a CFUNC is the operator name (like += ). */
#define CONTAINERFUNC1SCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, CFUNCNAME, CFUNC ) \
  CONTAINERFUNC1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, CFUNCNAME, CFUNC, float ) \
    CONTAINERFUNC1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, CFUNCNAME, CFUNC, double ) \
    CONTAINERFUNC1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, CFUNCNAME, CFUNC, long double ) \
    CONTAINERFUNC1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, CFUNCNAME, CFUNC, signed char ) \
    CONTAINERFUNC1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, CFUNCNAME, CFUNC, unsigned char ) \
    CONTAINERFUNC1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, CFUNCNAME, CFUNC, signed int ) \
    CONTAINERFUNC1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, CFUNCNAME, CFUNC, unsigned int ) \
    CONTAINERFUNC1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, CFUNCNAME, CFUNC, signed long ) \
    CONTAINERFUNC1SINGLESCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, CFUNCNAME, CFUNC, unsigned long ) \


/* Generates definitions for unary class member functions. 
   \a CONTAINERTEMPL is a template definition (like template< class T > ),
   \a CONTAINERTYPE is the return type (the class),
   \a CFUNCNAME is the operator name (like pow ), and
   \a CFUNC is the operator name (like += ). */
#define CONTAINERFUNC1DEF( CONTAINERTEMPL, CONTAINERTYPE, CFUNCNAME, CFUNC ) \
  CONTAINERTEMPL template < class COT >					\
  const CONTAINERTYPE &CONTAINERTYPE::CFUNCNAME ( const COT &x )	\
  {									\
    iterator iter1 = begin();						\
    iterator end1 = end();						\
    typename COT::const_iterator iter2 = x.begin();			\
    typename COT::const_iterator end2 = x.end();			\
    while ( iter1 != end1 && iter2 != end2 ) {				\
      (*iter1) = static_cast< value_type >( CFUNC( *iter1, *iter2 ) );	\
      ++iter1;								\
      ++iter2;								\
    };									\
    return *this;							\
  }									\
									\
  CONTAINERFUNC1SCALARDEF( CONTAINERTEMPL, CONTAINERTYPE, CFUNCNAME, CFUNC ) \


}; /* namespace relacs */

#endif /* ! _RELACS_CONTAINEROPS_H_ */
