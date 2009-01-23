/*
This is how to determine the return type of an iterator or container template function
for mean functions.

See for example also the discussion at
http://ubiety.uwaterloo.ca/~tveldhui/papers/pepm99/
section "Traits".
*/
#include <iostream>
#include <vector>
#include <iterator>
#include <relacs/array.h>

using namespace std;
using relacs::Array;


template < typename T >
struct numerical_traits
{
  typedef T mean_type;
};


template <>
struct numerical_traits< int >
{
  typedef double mean_type;
};


template < typename Iterator >
struct numerical_iterator_traits
{
  typedef typename numerical_traits< typename iterator_traits< Iterator >::value_type >::mean_type mean_type;
};


template < typename Container >
struct numerical_container_traits
{
  typedef typename numerical_traits< typename Container::value_type >::mean_type mean_type;
};


template < typename ForwardIterX >
typename numerical_iterator_traits<ForwardIterX>::mean_type
mean( ForwardIterX firstx, ForwardIterX lastx )
{
  typename numerical_iterator_traits<ForwardIterX>::mean_type a = 0;
  for ( int k=1; firstx != lastx; ++firstx, ++k ) {
    a += ( *firstx - a ) / k;
  }
  return a;
}


template < typename ContainerX >
typename numerical_container_traits<ContainerX>::mean_type
mean( const ContainerX &vecx )
{
  return mean( vecx.begin(), vecx.end() );
}


int main( void )
{
  double dmean = 0.0;
  int imean = 0;

  vector< double > d;
  d.push_back( 0.1 );
  d.push_back( 0.2 );
  d.push_back( 0.3 );
  d.push_back( 0.4 );
  dmean = ::mean( d.begin(), d.end() );
  imean = ::mean( d.begin(), d.end() );
  cout << dmean << '\n';
  cout << ::mean( d ) << endl;

  vector< int > i;
  i.push_back( 1 );
  i.push_back( 2 );
  i.push_back( 3 );
  i.push_back( 4 );
  imean = ::mean( i.begin(), i.end() );
  dmean = ::mean( i.begin(), i.end() );
  cout << dmean << '\n';
  cout << ::mean( i ) << endl;


  Array< float > a;
  a.push( 0.1f );
  a.push( 0.2f );
  a.push( 0.3f );
  a.push( 0.4f );
  dmean = ::mean( a.begin(), a.end() );
  imean = ::mean( a.begin(), a.end() );
  cout << dmean << '\n';
  cout << ::mean( a ) << endl;

  double c[4] = { 0.2, 0.4, 0.6, 0.8 };
  dmean = ::mean( &c[0], &c[4] );
  imean = ::mean( &c[0], &c[4] );
  cout << dmean << '\n';

  return 0;
}
