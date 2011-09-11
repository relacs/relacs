/*
  trigger.h
  Virtual class for setting up an analog trigger device. 

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2011 Jan Benda <benda@bio.lmu.de>

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

#include <relacs/trigger.h>

namespace relacs {


Trigger::Trigger( void ) 
  : Device( TriggerType ),
    Hoops( 0 )
{
}


Trigger::Trigger( const string &deviceclass )
  : Device( deviceclass, TriggerType ),
    Hoops( 0 )
{
}


void Trigger::setCrossing( int mode, double level )
{
  Hoop[Hoops].ACrossing = mode;
  Hoop[Hoops].BCrossing = 0;
  Hoop[Hoops].APeak = 0;
  Hoop[Hoops].BPeak = 0;
  Hoop[Hoops].ATrough = 0;
  Hoop[Hoops].BTrough = 0;
  Hoop[Hoops].ALevel = level;
  Hoop[Hoops].BLevel = 0.0;
}


void Trigger::setRising( double level )
{
  setCrossing( AboveSet | BelowReset, level );
}


void Trigger::setFalling( double level )
{
  setCrossing( AboveReset | BelowSet, level );
}


void Trigger::setCrossing( int amode, double alevel, int bmode, double blevel )
{
  Hoop[Hoops].ACrossing = amode;
  Hoop[Hoops].BCrossing = bmode;
  Hoop[Hoops].ALevel = alevel;
  Hoop[Hoops].BLevel = blevel;
}


void Trigger::setRisingHysteresis( double alevel, double blevel )
{
  setCrossing( BelowReset, alevel, AboveSet, blevel );
}


void Trigger::setFallingHysteresis( double alevel, double blevel )
{
  setCrossing( BelowSet, alevel, AboveReset, blevel );
}


void Trigger::setWindow( double alevel, double blevel )
{
  setCrossing( AboveSet | BelowReset, alevel, AboveReset | BelowSet, blevel );
}


void Trigger::setPeakTrough( double threshold,
			     int peakamode, int troughamode, double alevel,
			     int peakbmode, int troughbmode, double blevel )
{
  Hoop[Hoops].APeak = peakamode;
  Hoop[Hoops].BPeak = peakbmode;
  Hoop[Hoops].ATrough = troughamode;
  Hoop[Hoops].BTrough = troughbmode;
  Hoop[Hoops].ALevel = alevel;
  Hoop[Hoops].BLevel = blevel;
  Hoop[Hoops].Threshold = threshold;
}


void Trigger::setPeak( double threshold )
{
  setPeakTrough( threshold,
		 AboveSet | BelowSet, AboveReset | BelowReset, 0.0,
		 0, 0, 0.0 );
}


void Trigger::setTrough( double threshold )
{
  setPeakTrough( threshold,
		 AboveReset | BelowReset, AboveSet | BelowSet, 0.0,
		 0, 0, 0.0 );
}


void Trigger::setPeak( double threshold, double level )
{
  setPeakTrough( threshold,
		 AboveSet, AboveReset | BelowReset, level,
		 0, 0, 0.0 );
}


void Trigger::setTrough( double threshold, double level )
{
  setPeakTrough( threshold,
		 AboveReset | BelowReset, BelowSet, level,
		 0, 0, 0.0 );
}


void Trigger::setPeak( double threshold, double alevel, double blevel )
{
  setPeakTrough( threshold,
		 AboveSet | BelowReset, AboveReset | BelowReset, alevel,
		 AboveReset | BelowSet, 0, blevel );
}


void Trigger::setTrough( double threshold, double alevel, double blevel )
{
  setPeakTrough( threshold,
		 AboveReset | BelowReset, AboveSet | BelowReset, alevel,
		 0, AboveReset | BelowSet, blevel );
}


int Trigger::set( const Options &opts )
{
  clear();

  // read parameter:
  string ttype = opts.text( "type" );
  double alevel = 0.0;
  if ( opts.exist( "level" ) )
    alevel = opts.number( "level" );
  else
    alevel = opts.number( "alevel" );
  double blevel = opts.number( "blevel" );
  double threshold = opts.number( "threshold" );

  // setup trigger parameter:
  if ( ttype == "rising" )
    setRising( alevel );
  else if ( ttype == "falling" )
    setFalling( alevel );
  else if ( ttype == "risinghysteresis" )
    setRisingHysteresis( alevel, blevel );
  else if ( ttype == "fallinghysteresis" )
    setFallingHysteresis( alevel, blevel );
  else if ( ttype == "window" )
    setWindow( alevel, blevel );
  else if ( ttype == "peak" )
    setPeak( threshold );
  else if ( ttype == "trough" )
    setTrough( threshold );
  else if ( ttype == "peakabove" )
    setPeak( threshold, alevel );
  else if ( ttype == "troughbelow" )
    setTrough( threshold, alevel );
  else if ( ttype == "peakwindow" )
    setPeak( threshold, alevel, blevel );
  else if ( ttype == "troughwindow" )
    setTrough( threshold, alevel, blevel );
  else
    return 0;

  return 1;
}


void Trigger::addHoop( double delay, double width )
{
  Hoops++;
  Hoop[Hoops].Delay = delay;
  Hoop[Hoops].Width = width;
  Hoop[Hoops].ACrossing = 0;
  Hoop[Hoops].BCrossing = 0;
  Hoop[Hoops].APeak = 0;
  Hoop[Hoops].BPeak = 0;
  Hoop[Hoops].ATrough = 0;
  Hoop[Hoops].BTrough = 0;
  Hoop[Hoops].ALevel = 0.0;
  Hoop[Hoops].BLevel = 0.0;
  Hoop[Hoops].Threshold = 0.0;
}


void Trigger::clear( void )
{
  Hoops = 0;
  Hoop[Hoops].ACrossing = 0;
  Hoop[Hoops].BCrossing = 0;
  Hoop[Hoops].APeak = 0;
  Hoop[Hoops].BPeak = 0;
  Hoop[Hoops].ATrough = 0;
  Hoop[Hoops].BTrough = 0;
  Hoop[Hoops].ALevel = 0.0;
  Hoop[Hoops].BLevel = 0.0;
  Hoop[Hoops].Threshold = 0.0;
}


void Trigger::setSettings( void )
{
  Settings.clear();
  Settings.addText( "status", "not yet implemented" );
}


int Trigger::reset( void )
{
  disable();
  clear();
  return 0;
}


}; /* namespace relacs */

