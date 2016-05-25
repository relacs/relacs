/*
  efield/eodtools.h
  Functions for analyzing EODs of weakly electric fish.

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

    /*! Returns a threshold suitable for detecting EOD peaks and troughs
        in \a data in the time interval from \a tbegin to \a tend.
        \a contrast is the maximum expected contrast of an
	amplitude modulation or beat within this time interval. */
  double eodThreshold( const InData &data, double tbegin, double tend,
		       double contrast=0.2 );
    /*! Detect eod \a peaks in \a data.
        The times of the EOD peaks are the times of the maximum data values.
        The sizes are the maximum of a square function fitted to the three
        topmost data points. */
  void eodPeaks( const InData &data, double tbegin, double tend,
		 double threshold, EventData &peaks );
    /*! Detect eod \a troughs in \a data.
        The times of the EOD troughs are the times of the minimum data values.
        The sizes are the minimum of a square function fitted to the three
        lowest data points. */
  void eodTroughs( const InData &data, double tbegin, double tend,
		   double threshold, EventData &troughs );
    /*! Detect eod \a peaks and \a troughs in \a data.
        The times of the EOD peaks/troughs are the times of the maximum/minimum data values.
        The sizes are the maximum/minimum of a square function fitted to the three
        highest/lowest data points. */
  void eodPeaksTroughs( const InData &data, double tbegin, double tend,
			double threshold, EventData &peaks, EventData &troughs );
    /*! Mean absolute size of eod peaks. */
  double meanPeaks( const InData &data, double tbegin, double tend,
		    double threshold );
    /*! Mean absolute size of eod troughs. */
  double meanTroughs( const InData &data, double tbegin, double tend,
		      double threshold );
    /*! Mean eod amplitude from an EOD without beat (0.5 * p-p amplitude). */
  double eodAmplitude( const InData &data, double tbegin, double tend );

    /*! The mean EOD amplitude and beat amplitudes of the beat
        of period \a period and expected contrast \a contrast determined
        from the standard deviation of the upper and lower EOD amplitudes.
        upperampl and lowerampl are 0.5*p-p amplitudes. */
  void beatAmplitudes( const InData &eodd, double tbegin, double tend,
		       double period, double contrast,
		       double &uppermean, double &upperampl,
		       double &lowermean, double &lowerampl );
    /*! The amplitude of the beat of period \a period determined
        from the standard deviation of EOD amplitudes (0.5 * p-p amplitude). */
  double beatAmplitude( const InData &eodd, double tbegin, double tend,
			double period, double contrast=0.2 );
    /*! The contrast of the beat of period \a period determined
        from the standard deviation of EOD amplitudes. */
  double beatContrast( const InData &eodd, double tbegin, double tend,
		       double period, double contrast=0.2 );

  /* for nerve potential analysis: */
  template < typename DataIter, typename TimeIter >
    class AcceptEOD
    {
    public:

      AcceptEOD( void ) {};
  
      int checkPeak( DataIter first, DataIter last,
		     DataIter event, TimeIter eventtime,
		     DataIter index, TimeIter indextime,
		     DataIter prevevent, TimeIter prevtime,
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

      int checkTrough( DataIter first, DataIter last,
		       DataIter event, TimeIter eventtime,
		       DataIter index, TimeIter indextime,
		       DataIter prevevent, TimeIter prevtime,
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
