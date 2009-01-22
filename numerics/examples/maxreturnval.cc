/*
This is how to determine the return type of an iterator or container template function.

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

// this works for all iterators:
template < typename ForwardIter >
typename iterator_traits<ForwardIter>::value_type max( ForwardIter first, ForwardIter last )
{
  if ( first == last )
    return 0;

  typename iterator_traits<ForwardIter>::value_type max = *first;
  while ( ++first != last ) {
    if ( max < *first )
      max = *first;
  }
  return max;
}


/*
// this only works wit iterator that have value_type defined!
// *double iterators don't have this defined!
template < typename ForwardIter >
typename ForwardIter::value_type max( ForwardIter first, ForwardIter last )
{
  if ( first == last )
    return 0;

  typename ForwardIter::value_type max = *first;
  while ( ++first != last ) {
    if ( max < *first )
      max = *first;
  }
  return max;
}
*/



template < typename Container >
typename Container::value_type max( const Container &vec )
{
  return ::max( vec.begin(), vec.end() );
}


int main( void )
{
  double dmax = 0.0;
  int imax = 0;

  vector< double > d;
  d.push_back( 0.1 );
  d.push_back( 0.2 );
  d.push_back( 0.3 );
  d.push_back( 0.4 );
  dmax = ::max( d.begin(), d.end() );
  imax = ::max( d.begin(), d.end() );
  cout << dmax << '\n';
  cout << ::max( d ) << endl;

  vector< int > i;
  i.push_back( 1 );
  i.push_back( 2 );
  i.push_back( 3 );
  i.push_back( 4 );
  imax = ::max( i.begin(), i.end() );
  dmax = ::max( i.begin(), i.end() );
  cout << imax << '\n';
  cout << ::max( i ) << endl;


  Array< float > a;
  a.push( 0.1f );
  a.push( 0.2f );
  a.push( 0.3f );
  a.push( 0.4f );
  dmax = ::max( a.begin(), a.end() );
  imax = ::max( a.begin(), a.end() );
  cout << dmax << '\n';
  cout << ::max( a ) << endl;

  double c[4] = { 0.2, 0.4, 0.6, 0.8 };
  dmax = ::max( &c[0], &c[4] );
  imax = ::max( &c[0], &c[4] );
  cout << dmax << '\n';

  return 0;
}
