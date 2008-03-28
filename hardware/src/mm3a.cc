/*
g++ -o mm3a mm3a.cc -I ../daq/src kleindiek.o ../daq/src/device.o
 */

#include <iostream>
#include <relacs/kleindiek.h>
using namespace std;


int main( void )
{
  Kleindiek k( "/dev/ttyS1" );

  //  cout << k << endl;

  /*
  cout << "countermode\n";
  k.countermode( 0, 0 );
  cout << "counterreset\n";
  k.counterreset();
  cout << "counterread\n";
  k.counterread();
  */
  cout << "speed\n";
  k.speed( 0, 13 );
  cout << "amplitude\n";
  k.amplitudepos( 0, 80 );
  k.amplitudeneg( 0, 80 );
  cout << "coarse\n";
  k.coarse( 0, 2000 );
  /*
  cout << "counterread\n";
  k.counterread();
  */

}
