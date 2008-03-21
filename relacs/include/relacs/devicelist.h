/*
  ../include/relacs/devicelist.h
  A container template for Device-plugins.

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

#ifndef _DEVICELIST_H_
#define _DEVICELIST_H_

#include <qpopupmenu.h> 
#include <vector>
#include <relacs/str.h>
#include <relacs/configclass.h>
#include "relacsplugin.h"
using namespace std;

class AllDevices;


/*!
\class DeviceList
\author Jan Benda
\version 1.0
\brief A container template for Device-plugins.
*/

template < class T, int PluginID >
class DeviceList : public ConfigClass
{

public:

    /*! Construct an empty list of devices.
        The \a title is passed to the ConfigClass class and is 
	used to identify the device list in the configuration files.
	Single devices are called \a name
	in the error messages. */
  DeviceList( const string &name, const string &title );
    /*! Destructor. */
  ~DeviceList( void );

    /*! The number of devices in the list. */
  int size( void ) const;
    /*! True if there isn't any device in the list. */
  bool empty( void ) const;
    /*! Clear the device list. */
  void clear( void );

    /*! Close all devices. */
  void close( void );
    /*! Reset all devices. */
  void reset( void );

    /*! Return const reference to \a i-th device. */
  const T &operator[]( int i ) const;
    /*! Return reference to \a i-th device. */
  T &operator[]( int i );

    /*! Add Device \a d to the list and to the device list \a devices. */
  template < class DD >
  void add( T *d, DD &devices );

    /*! Create \a devices from plugins. */
  template < class DD >
    int create( Plugins &plugins, DD &devices, int n, const string &dflt="" );
    /*! Returns the warning messages of the last call of create(). */
  Str warnings( void ) const;
    /*! True if the last call of create() was succesfull, i.e. no warnings. */
  bool ok( void ) const;

    /*! Return the device with identifier string \a ident.
        If there is no device class with that id in the list
	and \a ident begins with a positive integer number \a n
	then the \a n -th device of the list is returned.
        Otherwise zero is returned. */
  T *device( const string &ident );
    /*! Return the \a n -th device of type \a type.
        If there is no such device zero is returned. */
  T *device( int type, int n  );

    /*! Load the configuration. */
  virtual void readConfig( StrQueue &sq );
    /*! Save it to make it look more pretty. */
  virtual void saveConfig( ofstream &str );

    /*! Add devices to the popup window. */
  virtual void addMenu( QPopupMenu *menu, int &index );
    /*! Update device infos in the menu. */
  virtual void updateMenu( void );

  template < class TT, int PPluginID >
  friend ostream &operator<<( ostream &str, const DeviceList< TT, PPluginID > &d );


protected:

    /*! Return the mode for the \a n -t device. */
  virtual int mode( int n, const Str &ns );
    /*! Initialize the device \a dv with some more parameter. */
  virtual void init( T *dv, int n, const Str &ns, AllDevices &devices ) {};

    /*! The list of Devices. */
  vector < T* > DVs;
    /*! The list of corresponding menus. */
  vector < QPopupMenu* > Menus;
    /*! Name of the device list used for error messages. */
  string Name;
    /*! Warning messages. */
  string Warnings;

};


template < class T, int PluginID >
DeviceList<T,PluginID>::DeviceList( const string &name, const string &title )
  : ConfigClass( title, RELACSPlugin::Core, ConfigClass::Save ),
    Name( name ),
    Warnings( "" )
{
  clear();
}


template < class T, int PluginID >
DeviceList<T,PluginID>::~DeviceList( void )
{
  clear();
}


template < class T, int PluginID >
int DeviceList<T,PluginID>::size( void ) const
{
  return DVs.size();
}


template < class T, int PluginID >
bool DeviceList<T,PluginID>::empty( void ) const
{
  return DVs.empty();
}


template < class T, int PluginID >
void DeviceList<T,PluginID>::clear( void )
{
  // devices are deleted by Devices:clear() !
  DVs.clear();
  for ( unsigned int k=0; k<Menus.size(); k++ ) {
    if ( Menus[k] != 0 )
      delete Menus[k];
  }
  Menus.clear();
}


template < class T, int PluginID >
void DeviceList<T,PluginID>::close( void )
{
  for ( unsigned int k=0; k<DVs.size(); k++ )
    DVs[k]->close();
}


template < class T, int PluginID >
void DeviceList<T,PluginID>::reset( void )
{
  for ( unsigned int k=0; k<DVs.size(); k++ )
    DVs[k]->reset();
}


template < class T, int PluginID >
const T &DeviceList<T,PluginID>::operator[]( int i ) const
{
  return *DVs[i];
}


template < class T, int PluginID >
T &DeviceList<T,PluginID>::operator[]( int i )
{
  return *DVs[i];
}


template < class T, int PluginID > template < class DD >
void DeviceList<T,PluginID>::add( T *d, DD &devices )
{
  DVs.push_back( d );
  if ( (void *)&devices != (void *)this )
    devices.add( d, devices );
  Menus.push_back( NULL );
}


template < class T, int PluginID >
int DeviceList<T,PluginID>::mode( int n, const Str &ns )
{
  return integer( "mode" + ns, 0, 0 );
}


template < class T, int PluginID > template < class DD >
int DeviceList<T,PluginID>::create( Plugins &plugins, DD &devices,
				    int m, const string &dflt )
{
  Warnings = "";
  int n = 0;
  int failed = 0;
  for ( int j=1; ; j++ ) {
    Str ns( j, 0 );
    if ( ! exist( "plugin" + ns ) ) {
      failed++;
      if ( failed > 5 )
	break;
      else
	continue;
    }
    failed = 0;
    string ms = dflt;
    if ( m >= 0 && m < Options::size( "plugin" + ns ) )
      ms = text( "plugin" + ns, m );
    if ( ms == "0" )
      continue;
    if ( ms.empty() )
      ms = dflt;
    int k = plugins.index( ms, PluginID );
    if ( !ms.empty() && k >= 0 ) {
      void *mp = plugins.create( k );
      if ( mp != 0 ) {
	T *dv = static_cast<T*>( mp );
	Str ds = text( "device" + ns );
	int m = mode( j, ns );
	/*
	for ( int i = 0; i<devices.size(); i++ ) {
	  if ( devices[i].deviceIdent() == ds ) {
	    dv->open( devices[i], m );
	    ds = "";
	    break;
	  }
	}
	*/
	Device *d = devices.device( ds );
	if ( d != 0 ) {
	  dv->open( *d, m );
	  ds = "";
	}
	if ( ! ds.empty() )
	  dv->open( ds, m );
	if ( dv->isOpen() ) {
	  dv->setDeviceIdent( text( "ident" + ns ) );
	  init( dv, j, ns, devices );
	  add( dv, devices );
	  n++;
	}
	else {
	  Warnings += "Cannot open " + Name + " Plugin <b>" + ms + "</b> !\n";
	}
      }
      else {
	Warnings += "Cannot create " + Name + " Plugin <b>" + ms + "</b> !\n";
      }
    }
    else {
      Warnings += Name + " Plugin <b>" + ms + "</b> not found!\n";
    }
  }
  return n;
}


template < class T, int PluginID >
Str DeviceList<T,PluginID>::warnings( void ) const
{
  return Warnings;
}


template < class T, int PluginID >
bool DeviceList<T,PluginID>::ok( void ) const
{
  return Warnings.empty();
}


template < class T, int PluginID >
T *DeviceList<T,PluginID>::device( const string &ident )
{
  for ( unsigned int k=0; k < DVs.size(); k++ )
    if ( DVs[k]->deviceIdent() == ident )
      return DVs[k];

  int di = (int)::rint( Str( ident ).number( -1.0, 0 ) );
  if ( di >= 0 && di < (int)DVs.size() )
    return DVs[di];

  return 0;
}


template < class T, int PluginID >
  T *DeviceList<T,PluginID>::device( int type, int n )
{
  int i = 0;
  for ( unsigned int k=0; k < DVs.size(); k++ ) {
    if ( DVs[k]->deviceType() == type ) {
      if ( i == n )
	return DVs[k];
      else
	i++;
    }
  }

  return 0;
}


template < class T, int PluginID >
void DeviceList<T,PluginID>::readConfig( StrQueue &sq )
{
  Options::load( sq );
}


template < class T, int PluginID >
void DeviceList<T,PluginID>::saveConfig( ofstream &str )
{
  Options::save( str, "  ", -1, 0, false, false );
}


template < class T, int PluginID >
  void DeviceList<T,PluginID>::addMenu( QPopupMenu *menu, int &index )
{
  for ( unsigned int k=0; k<DVs.size(); k++, index++ ) {

    string s = "&";
    if ( index == 0 )
      s += '0';
    else if ( index < 10 )
      s += ( '1' + index - 1 );
    else
      s += ( 'a' + index - 10 );
    s += " ";
    s += DVs[k]->deviceIdent();
    if ( Menus[k] == NULL )
      Menus[k] = new QPopupMenu( menu );
    Menus[k]->clear();
    StrQueue sq1( DVs[k]->info(), ";" );
    for ( int j=0; j<sq1.size(); j++ ) {
      if ( ! sq1[j].empty() )
	Menus[k]->insertItem( sq1[j].c_str() );
    }
    StrQueue sq2( DVs[k]->settings(), ";" );
    if ( ! sq2.empty() && ! sq2[0].empty() )
      Menus[k]->insertSeparator();
    for ( int j=0; j<sq2.size(); j++ ) {
      if ( ! sq2[j].empty() )
	Menus[k]->insertItem( sq2[j].c_str() );
    }
    menu->insertItem( s.c_str(), Menus[k] );
  }
}


template < class T, int PluginID >
  void DeviceList<T,PluginID>::updateMenu( void )
{
  for ( unsigned int k=0; k<DVs.size(); k++ ) {

    if ( Menus[k] == NULL )
      continue;

    Menus[k]->clear();
    StrQueue sq1( DVs[k]->info(), ";" );
    for ( int j=0; j<sq1.size(); j++ ) {
      if ( ! sq1[j].empty() )
	Menus[k]->insertItem( sq1[j].c_str() );
    }
    StrQueue sq2( DVs[k]->settings(), ";" );
    if ( ! sq2.empty() && ! sq2[0].empty() )
      Menus[k]->insertSeparator();
    for ( int j=0; j<sq2.size(); j++ ) {
      if ( ! sq2[j].empty() )
	Menus[k]->insertItem( sq2[j].c_str() );
    }
  }
}


template < class T, int PluginID >
ostream &operator<<( ostream &str, const DeviceList<T,PluginID> &d )
{
  for ( unsigned int k=0; k<d.DVs.size(); k++ )
    str << *d.DVs[k] << '\n';
  return str;
}


#endif
