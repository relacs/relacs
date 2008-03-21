/*
  spectrumanalyzer.h
  Displays the spectrum of the voltage traces.

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

#ifndef _SPECTRUMANALYZER_H_
#define _SPECTRUMANALYZER_H_

#include <relacs/config.h>
#include <relacs/control.h>
#include <relacs/plot.h>

/*! 
\class SpectrumAnalyzer
\brief Displays the spectrum of the voltage traces.
\author Jan Benda
\version 1.0 (Mar 1, 2005)
*/


class SpectrumAnalyzer : public Control
{
  Q_OBJECT

public:

  SpectrumAnalyzer( void );
  ~SpectrumAnalyzer( void );

  virtual void config( void );

  virtual void notify( void );

  virtual void main( void );


private:

  int Trace;
  double Offset;
  double Duration;
  int SpecSize;
  bool Overlap;
  double (*Window)( int j, int n );
  bool Decibel;
  double FMax;
  double PMin;

  Plot P;

};


#endif
