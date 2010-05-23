/*
  filter.cc
  Basic virtual class for filtering or detecting events (e.g. spikes) 

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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

#include <relacs/filter.h>

namespace relacs {


Filter::Filter( const string &ident, int mode,
		FilterType type, int outtraces, 
		const string &name, const string &pluginset,
		const string &author, 
		const string &version, const string &date )
  : RELACSPlugin( "Filter: " + ident, RELACSPlugin::Plugins,
		  name, pluginset, author, version, date )
{
  if ( type == SingleAnalogDetector ||
       type == MultipleAnalogDetector ||
       type == SingleEventDetector ||
       type == MultipleEventDetector )
    setConfigIdent( "Event Detector: " + ident );
  Mode = mode;
  Type = type;
  OutTraces = outtraces;
  setName( name.empty() ? ident : name );
  Ident = ident;
}


Filter::~Filter( void )
{
}


const string &Filter::ident( void ) const
{
  return Ident;
}


void Filter::setIdent( const string &ident )
{
  Ident = ident;
  if ( Type == SingleAnalogDetector ||
       Type == MultipleAnalogDetector ||
       Type == SingleEventDetector ||
       Type == MultipleEventDetector )
    setConfigIdent( "Event Detector: " + ident );
  else
    setConfigIdent( "Filter: " + ident );
}


void Filter::setName( const string &name )
{
  ConfigDialog::setName( name );
  if ( Type == SingleAnalogDetector ||
       Type == MultipleAnalogDetector ||
       Type == SingleEventDetector ||
       Type == MultipleEventDetector )
    setConfigIdent( "Event Detector: " + Ident );
  else
    setConfigIdent( "Filter: " + Ident );
}



}; /* namespace relacs */

#include "moc_filter.cc"

