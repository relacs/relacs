/*
  base/savetraces.h
  Saves data from  selected input traces or events for each run into files

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_BASE_SAVETRACES_H_
#define _RELACS_BASE_SAVETRACES_H_ 1

#include <QLabel>
#include <relacs/repro.h>
using namespace relacs;

namespace base {


/*!
\class SaveTraces
\brief [RePro] Saves data from selected input traces or events for each run into files
\author Jan Benda
\version 1.0 (Jan 17, 2013)

The SaveTraces-%RePro records ascii-data without writing out any stimulus
and terminates after \c repeats times \c duration seconds. If \c repeats is
set to zero, the %RePro keeps recording infinitely. Recording can be
interrupted any time by pressing the space key or starting a different
%RePro.

Data are recorded into ascii files. In addition, if \a savedata is set \c true,
data are also saved into the raw-traces files of RELACS (trace-?.raw). For only storing data
into the RELACS raw-traces files, use the Record %RePro.

\par Options
- \c General
    - \c duration=1000ms: Duration (\c number)
    - \c repeats=0: Repeats (\c integer)
    - \c savedata=false: Save raw data (\c boolean)
    - \c split=false: Save each run into a separate file (\c boolean)
- \c Analog input traces
  A list of all available analog input traces.
  Select the ones that you want to save into files.
- \c Events
  A list of all available events.
  Select the ones that you want to save into files.

\par Files
- savetraces-<tracename>-<run>.dat
- saveevents-<eventsname>-<run>.dat
*/


class SaveTraces : public RePro
{
  Q_OBJECT

public:

  SaveTraces( void );
  virtual void preConfig( void );
  virtual int main( void );


protected:

  virtual void customEvent( QEvent *qce );

  QLabel *RecordingLabel;
  QLabel *ElapsedTimeLabel;
  QLabel *CommentLabel;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_SAVETRACES_H_ */
