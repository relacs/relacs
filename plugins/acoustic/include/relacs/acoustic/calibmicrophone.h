/*
  acoustic/calibmicrophone.h
  Determine calibration factor for a microphone using a calibration sound source.

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

#ifndef _CALIBMICROPHONE_H_
#define _CALIBMICROPHONE_H_

#include <relacs/repro.h>
#include <relacs/acoustic/traces.h>
#include <relacs/array.h>
#include <relacs/optwidget.h>
using namespace relacs;

namespace acoustic {


/*!
  \class CalibMicrophone
  \brief Determine calibration factor for a microphone using a calibration sound source.
  \author Jan Benda
  \version 1.0

  \par Options

  \par Files

  \par Plots

  \par Requirements
*/


class CalibMicrophone : public RePro, public Traces
{
  Q_OBJECT

public:

    /*! Constructor. */
  CalibMicrophone( void );
    /*! Destructor. */
  virtual ~CalibMicrophone( void );

  virtual void config( void );

  virtual int main( void );
  virtual void stop( void );
  void save( void );
  void analyze( const InData &intrace, double frequency, double duration,
		ArrayD &amplitudes );

 private:

  OptWidget MW;


};


}; /* namespace acoustic */

#endif /* ! _RELACS_ACOUSTIC_CALIBMICROPHONE_H_ */

