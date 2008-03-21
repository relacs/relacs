/*
  fifield.h
  Optimized measuring of f-I curves.

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

#ifndef _FIFIELD_H_
#define _FIFIELD_H_

#include <vector>
#include <relacs/map.h>
#include <relacs/rangeloop.h>
#include <relacs/multiplot.h>
#include <relacs/ephystraces.h>
#include "auditorytraces.h"
#include "ficurve.h"

using namespace std;


/*!
\class FIField
\brief Optimized measuring of f-I curves.
\author Jan Benda
\version 1.3 (Jan 10, 2008)
-# removed stop() function
\version 1.2 (Jan 10, 2004)

Features:
- Shorter pauses when there is no response.

\par Options
\arg \b duration (\c number, \e ms): Duration of calibration stimulus.
\arg \b repeats (\c integer): Maximum repetitions of the calibration protocol.

\par Files
\arg \b calibrate.dat : the calibration data (measured versus requested stimulus intensity).

\par Plots
The plot shows the measured versus the requested stimulus intensity (red circles).
The yellow line is a fit of a straight line to the data.
This line should for a successful calibration coincide with the blue 1:1 line.

\par Requirements
\arg Transdermal EOD recording (\c EODTrace2) and events (\c EODEvents2).
*/


class FIField : public FICurve
{
  Q_OBJECT

public:

    /*! Constructor. */
  FIField( void );
    /*! Destructor. */
  ~FIField( void );

    /*! Read options, create stimulus and start output of stimuli. */
  virtual int main( void );

  void saveHeader( const string &file );
  void saveThreshold( const string &file );
  void save( void );

    /*! Plot data. */
  virtual void plot( const vector< FIData > &results );
    /*! Set next stimulus. */
  virtual DoneState next( vector< FIData > &results, bool msg=true );


protected:

  double MinFreq;
  double MaxFreq;
  double FreqStep;
  double FreqFac;
  string FreqStr;
  RangeLoop::Sequence FreqShuffle;
  int FreqIncrement;
  RangeLoop FrequencyRange;
  int ReducedFreqIncrement;
  int ReducedFreqNumber;

  vector < ThreshData > FieldData;
  int BestIndex;
  MapD BestFICurve;
  MapD BestOnFICurve;
  MapD BestSSFICurve;

  double MinRateFrac;
  double MaxRate;

  Options FIFieldHeader;

};

#endif
