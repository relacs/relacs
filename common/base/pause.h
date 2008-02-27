/*
  pause.h
  Does nothing

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

#ifndef _PAUSE_H_
#define _PAUSE_H_

#include "repro.h"

/*!
\class Pause
\brief Does nothing
\author Jan Benda
\version 1.2 (Oct 1, 2004)
-# updated to new RePro interface

\par Options
\arg \b duration (\c number, \e ms): Width set for the PlotTrace widget.
\arg \b repeats (\c integer): Number of repetitions (\c repeats=0: infinite). 
The Pause-RePro terminates after \c repeats times \c duration ms.

\par Files
\arg No output files.

\par Plots
\arg No plot.

\par Requirements
\arg No requirements.
*/


class Pause : public RePro
{
  Q_OBJECT

public:

  Pause( void );
  virtual int main( void );

};


#endif
