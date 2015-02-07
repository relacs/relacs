/*
  dynclampaosim.h
  Implementation of AnalogOutput simulating an analog output device supporting analog ouput.

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

#ifndef _RELACS_DYNCLAMPAOSIM_H_
#define _RELACS_DYNCLAMPAOSIM_H_ 1

#include <relacs/rtaicomedi/moduledef.h>
#include <relacs/aosim.h>

namespace relacs {


/*! 
\class DynClampAOSim
\author Jan Benda
\brief Implementation of AnalogOutput simulating an analog output device supporting analog ouput.
 */

class DynClampAOSim: public AOSim
{

public:

    /*! Device type id for simulated DAQ output devices. */
  static const int SimAnalogOutputType = 1;

    /*! Create a new DynClampAOSim without opening a device. */
  DynClampAOSim( void );
    /*! Stop analog output and close the daq driver. */
  ~DynClampAOSim( void );           

    /*! Open the analog output device simulation */
  virtual int open( const string &device, const Options &opts );
    /*! Open the analog output device simulation. */
  virtual int open( Device &device, const Options &opts );

    /*! If the analog output device supports outputs that
        are not physical output lines but rather writeable parameter,
        like model parameter for a dynamic clamp modul,
        then reimplement this function. 
        Add for each such parameter a TraceSpec to \a traces.
        \a deviceid is the id of the analog output device
        that you should use for initializing the TraceSpec. */
  virtual void addTraces( vector< TraceSpec > &traces, int deviceid ) const;

    /*! Match trace names with model output trace names. */
  virtual int matchTraces( vector< TraceSpec > &traces ) const;


protected:

    /*! Device driver specific tests on the settings in \a sigs
        for each output signal.
	Before this function is called, the validity of the settings in 
	\a sigs was already tested by testReadData().
	This function should test whether the settings are really supported
	by the hardware.
	If an error ocurred in any trace, the corresponding errorflags in the
	OutData are set and a negative value is returned.
        The channels in \a sigs are not sorted.
        This function is called by testWrite(). */
  virtual int testWriteDevice( OutList &sigs );

};


}; /* namespace relacs */

#endif /* ! _RELACS_DYNCLAMPAOSIM_H_ */

