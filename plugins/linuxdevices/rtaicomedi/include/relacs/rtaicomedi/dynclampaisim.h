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

#include <vector>
#include <relacs/aisim.h>
#include <relacs/rtaicomedi/moduledef.h>

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
  virtual int open( const string &device) override;
    /*! Open the analog input device simulation. */
  virtual int open( Device &device) override;

    /*! Initialize dynamic clamp model and prepare analog input of the input traces \a traces. */
  virtual int prepareRead( InList &traces );

    /*! Compute the dynamic clamp model. */
  virtual void model( InList &data,
		      const vector< int > &aochannels, vector< float > &aovalues );

    /*! If the analog input device supports inputs that
        are not physical input lines but rather readable internal variables,
        like model outputs of a dynamic clamp modul,
        then reimplement this function. 
        Add for each such variable a TraceSpec to \a traces.
        \a deviceid is the id of the analog output device
        that you should use for initializing the TraceSpec. */
  virtual void addTraces( vector< TraceSpec > &traces, int deviceid ) const;

    /*! Match trace names with model trace names. */
  virtual int matchTraces( InList &traces ) const;


protected:

    /*! Device driver specific tests on the settings in \a traces
        for each input signal.
	Before this function is called, the validity of the settings in 
	\a traces was already tested by testReadData().
	This function should test whether the settings are really supported
	by the hardware.
	If an error ocurred in any trace, the corresponding errorflags in the
	InData are set and a negative value is returned.
        The channels in \a traces are not sorted.
        This function is called by testRead(). */
  virtual int testReadDevice( InList &traces );


private:

  vector<float> statusInput;
  vector<string> statusInputNames;
  vector<string> statusInputUnits;
#ifdef ENABLE_INTERVALS
  int intervalstatusinx;
#endif
#ifdef ENABLE_AITIME
  int aitimestatusinx;
#endif
#ifdef ENABLE_AIACQUISITIONTIME
  int aiacquisitiontimestatusinx;
#endif
#ifdef ENABLE_AICONVERSIONTIME
  int aiconversiontimestatusinx;
#endif
#ifdef ENABLE_AOTIME
  int aotimestatusinx;
#endif
#ifdef ENABLE_MODELTIME
  int modeltimestatusinx;
#endif
#ifdef ENABLE_COMPUTATION
  int outputstatusinx;
#endif


};


}; /* namespace relacs */

#endif /* ! _RELACS_DYNCLAMPAISIM_H_ */
