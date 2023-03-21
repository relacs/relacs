/*
  voltageclamp/summary.h
  summary over other repros to plot steady-states and time constants

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

#ifndef _RELACS_VOLTAGECLAMP_SUMMARY_H_
#define _RELACS_VOLTAGECLAMP_SUMMARY_H_ 1

#include <relacs/multiplot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
#include <relacs/voltageclamp/activation.h>
#include <relacs/voltageclamp/inactivation.h>
#include <relacs/voltageclamp/recovery.h>
using namespace relacs;

namespace voltageclamp {


/*!
\class Summary
\brief [RePro] summary over other repros to plot steady-states and time constants
\author Lukas Sonnenberg
\version 1.0 (Sep 07, 2018)
*/


class Summary : public RePro
{
  Q_OBJECT

friend class Activation;
friend class Inactivation;
friend class Recovery;

public:

  Summary( void );
  virtual int main( void );

private:

    void plotactivation( RePro* rp_ac );
    void plotinactivation( RePro* rp_inac );
    void plotrecovery( RePro* rp_rec );

protected:

    MultiPlot P;

};


}; /* namespace voltageclamp */

#endif /* ! _RELACS_VOLTAGECLAMP_SUMMARY_H_ */
