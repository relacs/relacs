/*
  acoustic/calibmicrophone.h
  Determine calibration factor for a microphone using a calibration sound source.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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
\brief [RePro] Determine calibration factor for a microphone using a calibration sound source.
\author Jan Benda

\par Options
- \c intensity=114dB SPL: Intensity of the sound source (\c number)
- \c frequency=1000Hz: Frequency of the sound source (\c number)
- \c intrace=Sound-1: Input trace (\c string)
- \c duration=1seconds: Duration of a single measurement (\c number)
- \c repeat=10: Number of measurements (\c integer)
- \c amplitude=0V: rms amplitude (\c number)
- \c amplitudestdev=0V: rms amplitude stdev (\c number)
- \c scale=0V/Pa: Scale factor (\c number)

\version 1.0 (Aug 11 2008)
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

