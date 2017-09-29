/*
  auditoryprojects/mothasongs.h
  Artificial songs of Arctiid moths

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

#ifndef _RELACS_AUDITORYPROJECTS_MOTHASONGS_H_
#define _RELACS_AUDITORYPROJECTS_MOTHASONGS_H_ 1

#include <relacs/multiplot.h>
#include <relacs/repro.h>
#include <relacs/ephys/traces.h>
#include <relacs/acoustic/traces.h>
using namespace relacs;

namespace auditoryprojects {


/*!
\class MothASongs
\brief [RePro] Artificial songs of Arctiid moths
\author Nils Brehm
\version 1.0 (Sep 27, 2017)
*/


class MothASongs : public RePro, public ephys::Traces, public acoustic::Traces
{
  Q_OBJECT

public:

  MothASongs( void );
  virtual int main( void );

  MultiPlot P;

};


}; /* namespace auditoryprojects */

#endif /* ! _RELACS_AUDITORYPROJECTS_MOTHASONGS_H_ */
