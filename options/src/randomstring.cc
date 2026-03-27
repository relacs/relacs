//
// Created by sonnenberg on 21/06/19.
//

#include <relacs/randomstring.h>
#include <relacs/str.h>
#include <cstdlib>

namespace relacs {

std::string randomString( const int len ) {
  std::string s = "";
  static const char alphanum[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  for ( int i = 0; i < len; ++i ) {
    s = s + alphanum[rand() % (sizeof( alphanum ) - 1)];
  }
  return s;
};


}; /* namespace relacs */