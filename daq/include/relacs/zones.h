/*
  zones.h
  A collection of zones for manipulators. 

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

#ifndef _RELACS_ZONES_H_
#define _RELACS_ZONES_H_ 1

#include <relacs/device.h>
using namespace std;

namespace relacs {


/*!
\class Zones
\brief A collection of zones for manipulators.
\author Fabian Sinz
\version 1.0

*/

struct Point3D{
  double x,y,z;
  
  Point3D(){
    x = y = z = 0;
  }

  Point3D(double mx, double my, double mz){
    x = mx;
    y = my;
    z = mz;
  }


};
typedef struct Point3D Point3D;

/********************************************************/
struct InfBox{
  
  double xmin, xmax, ymin, ymax, zmin;
  InfBox(){
    xmin = xmax = ymin = ymax = zmin = 0;
  }

  InfBox(vector<Point3D> co){
    if (co.size() < 4){
      cerr << "Need 4 corners to construct Box!" << endl;
    }

    xmax = xmin = co[0].x;
    ymax = ymin = co[0].y;
    zmin = co[0].z;

    for (int i = 1; i != 4; ++i){
      xmax = (xmax < co[i].x) ? co[i].x : xmax;
      xmin = (xmin > co[i].x) ? co[i].x : xmin;
      ymax = (ymax < co[i].y) ? co[i].y : ymax;
      ymin = (ymin > co[i].y) ? co[i].y : ymin;
      zmin = (zmin > co[i].z) ? co[i].z : zmin;
    }

    cerr << "Created new InfBox " << xmin << " < x < " << xmax\
	 << ", " << ymin << " < y < " << ymax \
	 << ", " << zmin << endl;
    
  }

  bool isInside(double x, double y, double z){
    return ( (x >= xmin) && (x <= xmax)  && \
	     (y >= ymin) && (y <= ymax) && z >= zmin);
  }

  bool isInside(Point3D p){
    return ( (p.x >= xmin) && (p.x <= xmax)  && \
	     (p.y >= ymin) && (p.y <= ymax)  && p.z >= zmin);
  }
  

};
typedef struct InfBox Polyhedron;


/********************************************************/
class Zones
{

public:

    /*! Construct Zones. */
  Zones( void );

  bool insideZone(double x, double y, double z);
  int addZone(vector<Point3D> points);

 protected:

 private:
  vector<Polyhedron> zones;
  double R[3][3];
  double T[3];

  Point3D transform(const Point3D in);
  Point3D transform(const double x , const double y, const double z);

};


}; /* namespace relacs */

#endif /* ! _RELACS_ZONES_H_ */
