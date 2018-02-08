#include <values.h>
#include <cmath>
#include <iostream>
using namespace std;

int main( void )
{
  double a = HUGE_VAL;
  cout << "HUGE_VAL is NaN: " << ::isnan( a ) << '\n';  // 0
  cout << "HUGE_VAL is inf: " << ::isinf( a ) << '\n';  // 1
  cout << "HUGE_VAL is finite: " << ::finite( a ) << '\n';  // 1
  cout << '\n';

  a = -HUGE_VAL;
  cout << "-HUGE_VAL is NaN: " << ::isnan( a ) << '\n';  // 0
  cout << "-HUGE_VAL is inf: " << ::isinf( a ) << '\n';  // -1
  cout << "-HUGE_VAL is finite: " << ::finite( a ) << '\n';  // 1
  cout << '\n';

  a = MAXDOUBLE;
  cout << "MAXDOUBLE is NaN: " << ::isnan( a ) << '\n';  // 0
  cout << "MAXDOUBLE is inf: " << ::isinf( a ) << '\n';  // 0
  cout << "MAXDOUBLE is finite: " << ::finite( a ) << '\n';  // 1
  cout << '\n';

  a = -MAXDOUBLE;
  cout << "-MAXDOUBLE is NaN: " << ::isnan( a ) << '\n';  // 0
  cout << "-MAXDOUBLE is inf: " << ::isinf( a ) << '\n';  // 0
  cout << "-MAXDOUBLE is finite: " << ::finite( a ) << '\n';  // 1
  cout << '\n';

  a = sqrt( -1.0 );
  cout << "sqrt( -1.0 ) is NaN: " << ::isnan( a ) << '\n';  // 1
  cout << "sqrt( -1.0 ) is inf: " << ::isinf( a ) << '\n';  // 0
  cout << "sqrt( -1.0 ) is finite: " << ::finite( a ) << '\n';  // 1
  cout << '\n';

  a = -sqrt( -1.0 );
  cout << "-sqrt( -1.0 ) is NaN: " << ::isnan( a ) << '\n';  // 1
  cout << "-sqrt( -1.0 ) is inf: " << ::isinf( a ) << '\n';  // 0
  cout << "-sqrt( -1.0 ) is finite: " << ::finite( a ) << '\n';  // 1
  cout << '\n';

  return 0;
}
