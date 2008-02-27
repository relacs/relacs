/*
  oneclick.h
  

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _ONECLICK_H_
#define _ONECLICK_H_

#include "repro.h"
#include "rangeloop.h"
#include "multiplot.h"
#include "ephystraces.h"
#include "auditorytraces.h"


/*!
\class OneClick
\brief A single short click
\author Alexander Wolf
\version 0.2 (Jan 10, 2008)
-# removed stop() function
\version 0.1
\brief 
*/


class OneClick : public RePro, public EPhysTraces, public AuditoryTraces
{
  Q_OBJECT

public:

    /*! Constructor. */
  OneClick( void );
    /*! Destructor. */
  ~OneClick( void );

    /*! Read options, create stimulus and output of stimuli. */
  virtual int main( void );

  void saveSpikes( const string &file );

    /*! Plot data. */
  void plot( void );
    /*! Analyze data. */
  void analyze( void );


protected:

  double MinIntensity;
  double MaxIntensity;
  double IntensityStep;
  int IntRepeat;

  double Stepsize;
  double Duration;
  double Latency;
  double PreWidth;
  double Pause;
  int Side;

  double Intensity;

  MultiPlot P;  // Plots!

  Options Header; //?????????????

};

#endif
