/*
  multielectrode/multista.h
  Computes spike-triggered averages from all recorded input traces

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

#ifndef _RELACS_MULTIELECTRODE_MULTISTA_H_
#define _RELACS_MULTIELECTRODE_MULTISTA_H_ 1

#include <vector>
#include <relacs/multiplot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
using namespace std;
using namespace relacs;

namespace multielectrode {


/*!
\class MultiSTA
\brief [RePro] Computes spike-triggered averages from all recorded input traces
\author Jan Benda, Michael Pfizenmaier
\version 0.1 (Jan 21, 2009)

\par Options

\par Files
\arg No output files.

\par Plots
\arg The STAs. All snippets in red, the STA (blue) and the standard deviation (cyan).

\par Requirements
\arg 
*/


class MultiSTA : public RePro, public ephys::Traces
{
  Q_OBJECT

public:

    /*! Constructor. Defines all options. */
  MultiSTA( void );

    /*! This is what the RePro is doing. */
  virtual int main( void );

    /*! Provide a list of existing input traces to select from. */
  virtual void config( void );

    /*! Analyze the data. */
  void analyze( const EventData &spiketrain, double interval );
    /*! Plot the results. */
  void plot( void );


protected:

    /*! Stores data for STA of a single traces. 
        SampleDataF is from numerics/include/relacs/sampledata.h
        vector is from the STL */
  struct STA {
    vector< SampleDataF > Snippets;
    SampleDataD Average;
    SampleDataD StDev;
  };
    /*! The STA data for all input traces. */
  vector< STA > STAs;

    /*! We need lots of plots for the STAs. */
  MultiPlot P;

};


}; /* namespace multielectrode */

#endif /* ! _RELACS_MULTIELECTRODE_MULTISTA_H_ */

