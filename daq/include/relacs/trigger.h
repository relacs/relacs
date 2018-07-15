/*
  trigger.h
  Virtual class for setting up an analog trigger device. 

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

#ifndef _RELACS_TRIGGER_H_
#define _RELACS_TRIGGER_H_ 1

#include <relacs/device.h>
using namespace std;

namespace relacs {


/*!
\class Trigger
\brief Virtual class for setting up an analog trigger device. 
\author Jan Benda
\version 1.0

A trigger device sets it output to high if a certain type of event was
detected on its input. On a different type of input event the output
is set back to low. The event types supported by the Trigger interface
are threshold crossings, peaks, and troughs.  By the setCrossing() and
setPeakTrough() functions, these events can be associated with the
actions "set output to high" (AboveSet, BelowSet), "set output to low"
(AboveReset, BelowReset), or "ignore" (AboveIgnore, BelowIgnore).
Always call clear() before specifying new trigger settings with
setCrossing() or setPeakTrough()!

The trigger settings need to be activated by calling activate(). The
trigger device can be stopped to emit trigger signals by calling
disable().

Only the activate() and disable() functions need to be implemented
along with the open(), isOpen(), and close() functions.
Towards the end of the open() function you should call set() for
setting trigger parameters from the supplied options, followed by
activate().

In case you want to use a trigger device within RELACS, your
Trigger implementation needs to provide a void default constructor
(i.e. with no parameters) that does not open the device.  Also,
include the header file \c <relacs/relacsplugin.h> and make the
Trigger device known to RELACS with the \c addTrigger(
ClassNameOfYourTriggerImplementation, PluginSetName ) macro.
*/


class Trigger : public Device
{

public:

  /*! The actions to be taken for a trigger event
      that falls below the corresponding threshold level
      (threshold crossing from above, peak/trough below threshold). */
  enum BelowActions {
      /*! Ignore the event. */
    BelowIgnore = 0x0000,
      /*! Set the level of the trigger output to high. */
    BelowSet = 0x0001,
      /*! Reset the level of the trigger output to low. */
    BelowReset = 0x0002,
      /*! Bitmask for the BelowActions. */
    BelowMask = BelowIgnore | BelowSet | BelowReset
  };

  /*! The actions to be taken for a trigger event
      that falls above the corresponding threshold level
      (threshold crossing from below, peak/trough above threshold). */
  enum AboveActions {
      /*! Ignore the event. */
    AboveIgnore = 0x0000,
      /*! Set the level of the trigger output to high. */
    AboveSet = 0x0100,
      /*! Reset the level of the trigger output to low. */
    AboveReset = 0x0200,
      /*! Bitmask for the AboveActions. */
    AboveMask = AboveIgnore | AboveSet | AboveReset
  };

    /*! Construct a Trigger Device. */
  Trigger( void );
    /*! Construct a Trigger Device with class \a deviceclass.
        \sa setDeviceClass() */
  Trigger( const string &deviceclass );

    /*! Setup a single threshold crossing detector.
        \param[in] level is the threshold level.
        \param[in] mode determines what to do if the signal crosses the threshold.
	It is a combination (OR) of one of the BelowActions and one of the AboveActions.
	\sa setRising(), setFalling(), activate() */
  void setCrossing( int mode, double level );
    /*! Setup the trigger output to be high as long as the signal is above the threshold
        (high mode).
	This is a shortcut for setCrossing( AboveSet | BelowReset, level ).
        \param[in] level the level of the threshold.
        \sa setCrossing(int,double). */
  void setRising( double level );
    /*! Setup the trigger output to be high as long as the signal is below the threshold
        (low mode).
	This is a shortcut for setCrossing( AboveReset | BelowSet, level ).
        \param[in] level the level of the threshold.
        \sa setCrossing(int,double). */
  void setFalling( double level );

    /*! Setup a two-threshold crossing detector.
        \param[in] alevel is the level of the lower of the two thresholds.
        \param[in] amode determines what to do if the signal crosses the lower threshold.
	It is a combination (OR) of one of the BelowActions and one of the AboveActions.
        \param[in] blevel is the level of the higher of the two thresholds.
        \param[in] bmode determines what to do if the signal crosses the higher threshold.
	It is a combination (OR) of one of the BelowActions and one of the AboveActions.
        \sa setRisingHysteresis(), setFallingHysteresis(), setWindow(), activate() */
  void setCrossing( int amode, double alevel, int bmode, double blevel );
    /*! Setup the trigger output to be high if the signal is above the higher 
        threshold. The trigger output is set low if the signal falls below
	the lower threshold (hysteresis high mode).
	This is a shortcut for setCrossing( BelowReset, alevel, AboveSet, blevel ).
        \param[in] alevel the level of the lower threshold.
        \param[in] blevel the level of the higher threshold.
        \sa setCrossing(int,double,int,double). */
  void setRisingHysteresis( double alevel, double blevel );
    /*! Setup the trigger output to be high if the signal is below the lower 
        threshold. The trigger output is set low if the signal rises above
	the higher threshold (hysteresis low mode).
	This is a shortcut for setCrossing( BelowSet, alevel, AboveReset, blevel ).
        \param[in] alevel the level of the lower threshold.
        \param[in] blevel the level of the higher threshold.
        \sa setCrossing(int,double,int,double). */
  void setFallingHysteresis( double alevel, double blevel );
    /*! Setup the trigger output to be high if the signal is inbetween the
        lower and the higher threshold. Above the higher threshold and below 
	the lower threshold the trigger output is set low (middle mode or window mode).
	This is a shortcut for setCrossing( AboveSet | BelowReset, alevel, AboveReset | BelowSet, blevel ).
        \param[in] alevel the level of the lower threshold.
        \param[in] blevel the level of the higher threshold.
        \sa setCrossing(int,double,int,double). */
  void setWindow( double alevel, double blevel );

    /*! Setup a peak/trough detector.
        \param[in] threshold is the threshold used for detecting peaks and troughs
	(only peaks and troughs that differ by more than \a threshold are detected.
	If \a threshold is zero, all peaks and troughs are detected.)
        \param[in] alevel is the level of the lower of the two thresholds.
        \param[in] peakamode determines what to do if a peak is above or below the lower threshold.
        \param[in] troughamode determines what to do if a trough is above or below the lower threshold.
        \param[in] blevel is the level of the higher of the two thresholds.
        \param[in] peakbmode determines what to do if a peak is above or below the higher threshold.
        \param[in] troughbmode determines what to do if a trough is above or below the higher threshold.

	\a peakamode, \a troughamode, \a peakbmode, and \a troughbmode
	are all combinations (OR) of one of the BelowActions and one of the AboveActions.
        \sa setPeak(), setTrough(), activate() */
  void setPeakTrough( double threshold,
		      int peakamode, int troughamode, double alevel,
		      int peakbmode, int troughbmode, double blevel );
    /*! Trigger at any detected peak.
	Shortcut for setPeakTrough( threshold, AboveSet | BelowSet, AboveReset | BelowReset, 0.0,
	0, 0, 0.0 ).
        \param[in] threshold is the threshold used for detecting peaks and troughs
	(only peaks and troughs that differ by more than \a threshold are detected.
        \sa setPeakTrough(). */
  void setPeak( double threshold );
    /*! Trigger at any detected trough.
        Shortcut for setPeakTrough( threshold, AboveReset | BelowReset, AboveSet | BelowSet, 0.0,
	0, 0, 0.0 ).
        \param[in] threshold is the threshold used for detecting peaks and troughs
	(only peaks and troughs that differ by more than \a threshold are detected.
        \sa setPeakTrough(). */
  void setTrough( double threshold );
    /*! Trigger at detected peaks that are above the specified level.
        Shortcut for setPeakTrough( threshold, AboveSet, AboveReset | BelowReset, level,
	0, 0, 0.0 ).
        \param[in] threshold is the threshold used for detecting peaks and troughs
	(only peaks and troughs that differ by more than \a threshold are detected.
        \param[in] level the peak must be above this level.
        \sa setPeakTrough(). */
  void setPeak( double threshold, double level );
    /*! Trigger at detected troughs that are below the specified level.
	Shortcut for setPeakTrough( threshold, AboveReset | BelowReset, BelowSet, level,
	0, 0, 0.0 ).
        \param[in] threshold is the threshold used for detecting peaks and troughs
	(only peaks and troughs that differ by more than \a threshold are detected.
        \param[in] level the trough must be below this level.
        \sa setPeakTrough(). */
  void setTrough( double threshold, double level );
    /*! Trigger at detected peaks that are above the specified level \a alevel
        and below the specified level \a blevel.
        Shortcut for setPeakTrough( threshold, AboveSet | BelowReset, AboveReset | BelowReset, alevel,
	AboveReset | BelowSet, 0, blevel ).
        \param[in] threshold is the threshold used for detecting peaks and troughs
	(only peaks and troughs that differ by more than \a threshold are detected.
        \param[in] alevel the peak must be above this level.
        \param[in] blevel the peak must be below this level (\a alevel < \a blevel).
        \sa setPeakTrough(). */
  void setPeak( double threshold, double alevel, double blevel );
    /*! Trigger at detected troughs that are above the specified level \a alevel
        and below the specified level \a blevel.
        Shortcut for setPeakTrough( threshold, AboveReset | BelowReset, AboveSet | BelowReset, alevel,
	0, AboveReset | BelowSet, blevel ).
        \param[in] threshold is the threshold used for detecting peaks and troughs
	(only peaks and troughs that differ by more than \a threshold are detected.
        \param[in] alevel the trough must be above this level.
        \param[in] blevel the trough must be below this level (\a alevel < \a blevel).
        \sa setPeakTrough(). */
  void setTrough( double threshold, double alevel, double blevel );

    /*! Read parameter from \a opts and call the corresponding
        setCrossing() or setPeakTrough() function. The following
        parameter are used:
	\arg \c type: the type of the trigger event ("rising", "falling",
  	  "risinghysteresis", "fallinghysteresis", "window", "peak",
	  "trough", "peakabove", "troughbelow", "peakwindow",
	  "troughwindow", corresponding to the setRising(),
	  setFalling(), setRisingHysteresis(), setFallingHysteresis(),
	  setWindow(), setPeak( double ), setTrough( double ), setPeak(
	  double, double ), setTrough( double, double ), setPeak(
	  double, double, double ), setTrough( double, double, double ),
	  respectively.
        \arg \c level: the level for the trigger events that take one
          level into account.
        \arg \c alevel: the lower level for the trigger events that take
          two levels into account.
        \arg \c blevel: the higher level for the trigger events that take
          two levels into account.
        \arg \c threshold: the threshold parameter for detecting peak and
          troughs.
        \return 0 if there aren't any parameter specified by \a opts, 
	\return 1 if some trigger operation was defined by \a opts.
        This function should be used in open() like
        \code
        if ( set() > 0 )
          activate();
        \endcode */
  int set( void );

    /*! Add a new hoop to the trigger chain.
        All subsequent calls to the set* functions configure the new hoop.
	\param[in] delay the time relative to the first trigger event
	after which the trigger of this hoop has to occur.
	\param[in] width the time interval during which the trigger of
	this hoop has to occur.
	\note Since most trigger devices do not have the concept of
	several hoops, they might simply ignore any of the hoops
	except for the first one.
	\return 0 on success, -1 if there are already too many hoops defined
	\sa setCrossing(), setPeakTrough() */
  int addHoop( double delay, double width );

    /*! Clear all settings.
        The next call of a set* function will specify the first hoop.
	\note You always should call clear() before specifying new
	trigger settings.
	\note The settings of the trigger device are not affected.
	\sa setCrossing(), setPeakTrough() */
  void clear( void );

    /*! Update the settings() of the device with the current trigger
        parameter. */
  void setSettings( void );

    /*! Call this function to transfer all settings to the trigger device
        and to activate them.

	The implementation of this function has to read out the
	\a Hoop array and configure the device accordingly.
	Note that the number of configured hoops is \a Hoops plus one.
	Any functionality of this interface that is not supported by
	the device should be gracefully ignored.
	Also the settings of the device should be updated,
	e.g. by calling setSettings().
        \return 0 on success, negative numbers on complete failure,
        positive numbers if not everything is supported by the device. */
  virtual int activate( void ) = 0;
    /*! Disable the trigger device, i.e. no more trigger events will
        be emitted.
	\return 0 on success, negative numbers on failure. */
  virtual int disable( void ) = 0;

    /*! Disables the trigger device and clears all hoops.
        \sa disable(), clear(). */
  virtual int reset( void );


protected:

    /*! Intializes available options */
  virtual void initOptions( void );

    /*! All parameter for a single trigger hoop. */
  struct HoopParams {
    int ACrossing;
    int BCrossing;
    int APeak;
    int BPeak;
    int ATrough;
    int BTrough;
    double ALevel;
    double BLevel;
    double Delay;
    double Width;
    double Threshold;
  };
    /*! Maximum number of supported trigger hoops. */
  static const int MaxHoops = 5;
    /*! The parameter of the trigger hoops. */
  HoopParams Hoop[MaxHoops];
    /*! The number of hoops minus one. */
  int Hoops;

};


}; /* namespace relacs */

#endif /* ! _RELACS_TRIGGER_H_ */
