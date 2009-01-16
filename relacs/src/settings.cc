/*****************************************************************************
 *
 * settings.cc
 * Includes some general Settings into the configure mechanism.
 *
 * RELACS
 * Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
 * Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * RELACS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include <cstdlib>
#include <relacs/optwidget.h>
#include <relacs/relacswidget.h>
#include <relacs/settings.h>

using namespace std;

namespace relacs {


Settings::Settings( RELACSWidget* rw )
  : ConfigDialog( "Settings", RELACSPlugin::Core, "Settings" ),
    RW( rw )
{
  addLabel( "General" );
  addNumber( "readinterval", "Interval for periodic acquisition of data", 0.01, 0.001, 1000.0, 0.001, "seconds", "ms" );
  addNumber( "processinterval", "Interval for periodic processing of data", 0.10, 0.001, 1000.0, 0.001, "seconds", "ms" );
  addLabel( "Plugins" );
  addText( "pluginpathes", "Plugin pathes", "" );
  addText( "pluginhelppathes", "Pathes to plugin help files", "" );
  addText( "controlplugin", "Control plugin", "" );
  addText( "modelplugin", "Model plugin", "" );
  addLabel( "Pathes" );
    /* A string specifying the format of the file name. 
       In addition to the time format specifiers 
       %y year
       %m month
       %d day
       %H hour
       %M minute 
       %S second
       there is 
       %n for the filenumber (1,2,...), 
       %a for the filenumber expressed as a, b, ... z, ba, bb, ... zz, baa, ....
       %A for the filenumber expressed as A, B, ... Z, BA, ... */
  addText( "pathformat", "Format for data path", "%02y-%02m-%02d-%a2a" );
  addText( "defaultpath", "Default path", "dry/" ).setStyle( OptWidget::BrowseDirectory );
  addText( "repropath", "Base directory for RePros to store some general stuff", "reprodata" ).setStyle( OptWidget::BrowseDirectory );
  addText( "infofile", "Name of info file", "info.dat", 1 );
  addLabel( "Save" );
  addBoolean( "saverelacscore", "Save core configuration of RELACS to session", true );
  addBoolean( "saverelacsplugins", "Save configuration of RELACS-plugins to session", true );
  addBoolean( "saverelacslog", "Save log of RELACS to session", true );
  addBoolean( "saveattenuators", "Save calibration files for attenuators to session", true );
  addLabel( "Date/time formats" );
  addText( "dateformat", "Format for date", "%d.%m.%02y" );
  addText( "timeformat", "Format for time", "%H:%02M" );
  addText( "elapsedformat", "Format for elapsed time", "%02H:%02M" );
  addText( "sessiontimeformat", "Format for session runtime", "%Hh%02Mmin%02Ssec" );
  addText( "reprotimeformat", "Format for repro runtime", "%Mmin%02Ssec" );

  addTypeStyle( OptWidget::Bold, Parameter::Label );

  setDialogReadOnlyMask( 1 );
  setDialogHelp( false );
  setDialogHeader( false );
  setDialogCaption( "RELACS Settings" );
}


Settings::~Settings( void )
{
}


void Settings::configure( void )
{
}


void Settings::notify( void )
{
  if ( RW->AQ != 0 ) {
    RW->AQ->setUpdateTime( number( "updateinterval" ) );
  }

  if ( RW->SF != 0 ) {

    Str pathformat = text( "pathformat" );
    pathformat.provideSlash();
    RW->SF->setPathTemplate( pathformat );
    
    Str defaultpath = text( "defaultpath" );
    defaultpath.provideSlash();
    RW->SF->setDefaultPath( defaultpath );

  }

  Str rp = text( "repropath" );
  rp.provideSlash();
  setenv( "RELACSREPROPATH", rp.c_str(), 1 );
}


}; /* namespace relacs */

#include "moc_settings.cc"

