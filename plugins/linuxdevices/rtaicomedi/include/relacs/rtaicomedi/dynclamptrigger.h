/*
  comedi/dynclamptrigger.h
  Interface for setting up an analog trigger in the dynamic clamp kernel module.

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

#ifndef _COMEDI_DYNCLAMPTRIGGER_H_
#define _COMEDI_DYNCLAMPTRIGGER_H_

#include <relacs/trigger.h>
using namespace std;
using namespace relacs;

namespace comedi {


/*! 
\class DynClampTrigger
\author Jan Benda
\brief [Trigger] Interface for setting up an analog trigger in the dynamic clamp kernel module.
*/


class DynClampTrigger : public Trigger
{

public:

    /*! Create a new DynClampTrigger without opening a device. */
  DynClampTrigger( void );
    /*! Open the real time kernel module specified by its device file \a device. */
  DynClampTrigger( const string &device, const Options &opts );
    /*! Close the trigger driver. */
  virtual ~DynClampTrigger( void );

    /*! Open the trigger device on device file \a device. */
  virtual int open( const string &device, const Options &opts );
    /*! Returns true if dynamic clamp module was succesfully opened. */
  virtual bool isOpen( void ) const;
    /*! Stop all triggering activity and close the device. */
  virtual void close( void );

    /*! Call this function to transfer all settings to the trigger device
        and to activate them.
        \return 0 on success, negative numbers on complete failure,
        positive numbers if not everything is supported by the device. */
  virtual int activate( void );
    /*! Disable the trigger device, i.e. no more trigger events will be emitted.
        \return 0 on success, negative numbers on failure. */
  virtual int disable( void );


 protected:

    /*! Name of the kernel module device file. */
  string ModuleDevice;
    /*! File descriptor for the kernel module. */
  int ModuleFd;

    /*! The device file of the analog input device on which a channel is monitored. */
  string AIDevice;
    /*! The channel on the AIDevice that is monitored. */
  int AIChannel;

};


}; /* namespace comedi */

#endif /* ! _COMEDI_DYNCLAMPTRIGGER_H_ */
