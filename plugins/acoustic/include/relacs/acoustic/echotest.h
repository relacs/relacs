/*
  acoustic/echotest.h
  Check for echoes

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

#ifndef _RELACS_ACOUSTIC_ECHOTEST_H_
#define _RELACS_ACOUSTIC_ECHOTEST_H_ 1

#include <relacs/repro.h>
#include <relacs/sampledata.h>
#include <relacs/plot.h>
using namespace relacs;

namespace acoustic {


/*!
\class EchoTest
\brief [RePro] Check for echoes
\author Karin Fisch
\version 1.0 (Jun 22, 2010)
*/


class EchoTest : public RePro
{
  Q_OBJECT

public:

  EchoTest( void );
  virtual int main( void );
  
protected:
  
  void plotMeanResponse(SampleDataF &mean);
  Plot P;

};


}; /* namespace acoustic */

#endif /* ! _RELACS_ACOUSTIC_ECHOTEST_H_ */
