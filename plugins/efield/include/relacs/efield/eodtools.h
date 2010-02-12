/*
  efield/eodtools.h
  Functions for analyzing EODs of weakly electric fish.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _RELACS_EFIELD_EODTOOLS_H_
#define _RELACS_EFIELD_EODTOOLS_H_ 1

#include <relacs/indata.h>
#include <relacs/eventdata.h>
using namespace relacs;

namespace efield {


/*!
\class EODTools
\brief [lib] Functions for analyzing EODs of weakly electric fish.
\author Jan Benda
\version 1.0 (Nov 27, 2009)
*/


class EODTools
{

public:

  EODTools( void );

    /*! Detect eod peaks in \a data. */
  void eodPeaks( const InData &data, double tbegin, double tend,
		 double threshold, EventData &peaks );
    /*! Detect eod troughs in \a data. */
  void eodTroughs( const InData &data, double tbegin, double tend,
		   double threshold, EventData &troughs );
    /*! Mean size of eod troughs. */
  double meanTroughs( const InData &data, double tbegin, double tend,
		      double threshold );
    /*! Mean eod amplitude. */
  double eodAmplitude( const InData &data, const EventData &eode,
		       double tbegin, double tend );

    /*! Mean values of the upper and lower peaks and troughs of a beat. */
  void beatPeakTroughs( const InData &eodd, const EventData &bpe,
			const EventData &bte,
			double tbegin, double tend, double offset,
			double &upperpeak, double &uppertrough,
			double &lowerpeak, double &lowertrough );
    /*! Maximum and minimum amplitude of a beat. */
  void beatAmplitudes( const InData &eodd, const EventData &bpe,
		       const EventData &bte,
		       double tbegin, double tend, double offset,
		       double &min, double &max );
    /*! The contrast of the beat. */
  double beatContrast( const InData &eodd, const EventData &bpe,
		       const EventData &bte,
		       double tbegin, double tend, double offset );

  /* for nerve potential analysis: */
  template < typename DataIter, typename TimeIter >
    class AcceptEOD
    {
    public:

      AcceptEOD( void ) {};
  
      int checkPeak( const DataIter &first, const DataIter &last,
		     DataIter &event, TimeIter &eventtime,
		     DataIter &index, TimeIter &indextime,
		     DataIter &prevevent, TimeIter &prevtime,
		     EventList &outevents,
		     double &threshold,
		     double &minthresh, double &maxthresh,
		     double &time, double &size, double &width )
      { 
	// time:
	time = *eventtime;

	// peak:
	double y2 = *event;
	double y3 = *(event+1);
	--event;
	double y1 = *event;
	double a = y3 - 4.0*y2 + 3.0*y1;
	double b = 2.0*y3 - 4.0*y2 + 2.0*y1;
	size = y1 - 0.25*a*a/b;
    
	// width:
	width = 0.0;
    
	// accept:
	return 1; 
      }

      int checkTrough( const DataIter &first, const DataIter &last,
		       DataIter &event, TimeIter &eventtime,
		       DataIter &index, TimeIter &indextime,
		       DataIter &prevevent, TimeIter &prevtime,
		       EventList &outevents,
		       double &threshold,
		       double &minthresh, double &maxthresh,
		       double &time, double &size, double &width )
      { 
	// time:
	time = *eventtime;

	// peak:
	double y2 = *event;
	double y3 = *(event+1);
	--event;
	double y1 = *event;
	double a = y3 - 4.0*y2 + 3.0*y1;
	double b = 2.0*y3 - 4.0*y2 + 2.0*y1;
	size = y1 - 0.25*a*a/b;
    
	// width:
	width = 0.0;
    
	// accept:
	return 1; 
      }

    };

};


}; /* namespace efield */

#endif /* ! _RELACS_EFIELD_EODTOOLS_H_ */
