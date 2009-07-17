/*
  multielectrode/multista.h
  Computes spike-triggered averages for many spike traces and a common to be averaged waveform

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
\brief [RePro] Computes spike-triggered averages for many spike traces and a common to be averaged waveform
\author Jan Benda

\par Options
- \c averagetrace=V-1: Input trace to be averaged (\c string)
- \c interval=1sec: Averaging interval (\c number)
- \c repeats=100: Repeats (\c integer)
- \c stamint=-100ms: Minimum STA time (\c number)
- \c stamaxt=10ms: Maximum STA time (\c number)
- \c plotsnippets=true: Plot the individual snippets (\c boolean)

\par Files
No output files.

\par Plots
The STAs. All snippets in red, the STA (blue) and the standard deviation (cyan).

\par Requirements
- One voltage trace
- At least one spike trace

\version 0.1 (Jan 21, 2009)
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
  void analyze( const InData &intrace, double interval );
    /*! Plot the results. */
  void plot( bool snippets );


protected:

    /*! Stores data for STA of a single traces. 
        SampleDataF is from numerics/include/relacs/sampledata.h
        vector is from the STL */
  struct STA {
    vector< SampleDataF > Snippets;
    SampleDataF Average;
    SampleDataF StDev;
  };
    /*! The STA data for all input traces. */
  vector< STA > STAs;

    /*! We need lots of plots for the STAs. */
  MultiPlot P;

};


}; /* namespace multielectrode */

#endif /* ! _RELACS_MULTIELECTRODE_MULTISTA_H_ */

