/*
  acoustic/soundlevel.h
  Measures sound level in dB SPL

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

#ifndef _RELACS_ACOUSTIC_SOUNDLEVEL_H_
#define _RELACS_ACOUSTIC_SOUNDLEVEL_H_ 1

#include <QLabel>
#include <relacs/control.h>
using namespace relacs;

namespace acoustic {


/*!
\class SoundLevel
\brief [Control] Measures sound level in dB SPL
\author Jan Benda
\version 1.0 (Jul 28, 2016)
\par Options
- \c intrace=Sound-1: Input trace (\c string)
- \c origin=before end of data: Analysis window (\c string)
- \c offset=0ms: Offset of analysis window (\c number)
- \c duration=1000ms: Width of analysis window (\c number)
- \c scale=1.0: Scale for V/Pa (\c number)
*/


class SoundLevel : public Control
{
  Q_OBJECT

public:

  SoundLevel( void );
  virtual void main( void );
  virtual void preConfig( void );
  virtual void notify( void );

private:

  int InTrace;
  int Origin;
  double Offset;
  double Duration;
  double SoundPressureScale;
  QLabel *IW;

};


}; /* namespace acoustic */

#endif /* ! _RELACS_ACOUSTIC_SOUNDLEVEL_H_ */
