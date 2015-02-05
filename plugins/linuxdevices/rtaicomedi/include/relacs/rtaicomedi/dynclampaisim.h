/*
  dynclampaisim.h
  Implementation of AnalogInput simulating an analog input device supporting analog ouput.

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

#ifndef _RELACS_DYNCLAMPAISIM_H_
#define _RELACS_DYNCLAMPAISIM_H_ 1

#include <relacs/aisim.h>

namespace relacs {


/*! 
\class DynClampAISim
\author Jan Benda
\brief Implementation of AnalogInput simulating an analog input device supporting analog ouput.

\par Options:
- \c gainblacklist: List of daq board gains that should not be used. Each gain is identified by its
  maximal range value in volts.
*/


class DynClampAISim: public AISim
{

public:

    /*! Device type id for simulated DAQ input devices. */
  static const int SimAnalogInputType = 1;

    /*! Create a new DynClampAISim without opening a device. */
  DynClampAISim( void );
    /*! stop analog input and close the driver. */
  ~DynClampAISim( void );           

    /*! Open the analog input device simulation */
  virtual int open( const string &device, const Options &opts );
    /*! Open the analog input device simulation. */
  virtual int open( Device &device, const Options &opts );

};


}; /* namespace relacs */

#endif /* ! _RELACS_DYNCLAMPAISIM_H_ */
