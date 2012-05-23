/*
  zones.cc
  A collection of zones for manipulators. 

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#include <relacs/zones.h>

using namespace std;
namespace relacs {



Zones::Zones( void ) {

  R[0][0] = 1; R[0][1] = 0; R[0][2] = 0;
  R[1][0] = 0; R[1][1] = 1; R[1][2] = 0;
  R[2][0] = 0; R[2][1] = 0; R[2][2] = 1;

  T[0] = 0; T[1] = 0; T[2] = 0;

}

int Zones::addZone(vector<Point3D> points){
  vector<Point3D> tmp;
  for (vector<Point3D>::size_type i = 0; i != points.size(); ++i){
    tmp.push_back(transform(points[i]));
  }
  zones.push_back(Polyhedron(tmp));
  return 0;
}

bool Zones::insideZone(double x, double y, double z){

  vector<Polyhedron>::iterator it;
  for (it = zones.begin(); it < zones.end(); ++it){
    if (it->isInside(transform(x,y,z))) return true; 
  }
  return false;
}

Point3D Zones::transform(const Point3D tmp){
  return Point3D( (R[0][0]*tmp.x + R[0][1]*tmp.y + R[0][2]*tmp.z) + T[0],
		  (R[1][0]*tmp.x + R[1][1]*tmp.y + R[1][2]*tmp.z) + T[1],
		  (R[2][0]*tmp.x + R[2][1]*tmp.y + R[2][2]*tmp.z) + T[2]);
}

Point3D Zones::transform(const double x , const double y, const double z){
  return Point3D( (R[0][0]*x + R[0][1]*y + R[0][2]*z) + T[0],
		  (R[1][0]*x + R[1][1]*y + R[1][2]*z) + T[1],
		  (R[2][0]*x + R[2][1]*y + R[2][2]*z) + T[2]);
}



}; /* namespace relacs */

