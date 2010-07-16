/*
  devicelist.h
  A container template for Device-plugins.

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

#ifndef _RELACS_DEVICELIST_H_
#define _RELACS_DEVICELIST_H_ 1

#include <QMenu> 
#include <deque>
#include <vector>
#include <relacs/str.h>
#include <relacs/configclass.h>
#include <relacs/relacsplugin.h>
using namespace std;

namespace relacs {


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
    /*! Move device \a d and its menu entry to the back of the list. */
  void swapBack( T *d );

    /*! Create \a devices from plugins. */
  template < class DD >
    int create( DD &devices, int n, const string &dflt="0" );
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
  virtual void addMenu( QMenu *menu, int &index );
    /*! Update device infos in the menu. */
  virtual void updateMenu( void );

  template < class TT, int PPluginID >
  friend ostream &operator<<( ostream &str, const DeviceList< TT, PPluginID > &d );


protected:

    /*! The list of Devices. */
  vector < T* > DVs;
    /*! The list of corresponding menus. */
  vector < QMenu* > Menus;
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
  for ( int k=(int)DVs.size()-1; k >= 0; k-- )
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
void DeviceList<T,PluginID>::swapBack( T *d )
{
  for ( unsigned int k=0; k<DVs.size(); k++ ) {
    if ( DVs[k] == d ) {
      DVs.erase( DVs.begin() + k );
      DVs.push_back( d );
      QMenu *m = Menus[k];
      Menus.erase( Menus.begin() + k );
      Menus.push_back( m );
      break;
    }
  }
}


template < class T, int PluginID > template < class DD >
int DeviceList<T,PluginID>::create( DD &devices, int m, const string &dflt )
{
  Warnings = "";

  // get device entries:
  deque< Options::const_iterator > devicesecs;
  int failed = 0;
  for ( int j=1; failed<=5; j++ ) {
    // check for device entry in options:
    Options::const_iterator dp = find( "Device" + Str( j, 0 ) );
    if ( dp != Options::end() ) {
      failed = 0;
      devicesecs.push_back( dp );
    }
    else
      failed++;
  }
  devicesecs.push_back( Options::end() );

  int n = 0;
  for ( unsigned int j=0; j<devicesecs.size()-1; j++ ) {

    // get section of options:
    Options deviceopts;
    for ( Options::const_iterator dp=devicesecs[j];
	  dp !=devicesecs[j+1];
	  ++dp ) {
      deviceopts.add( *dp );
    }

    // get plugin:
    string ms = "";
    if ( m >= 0 && m < deviceopts.size( "plugin" ) )
      ms = deviceopts.text( "plugin", m );
    if ( ms.empty() )
      ms = dflt;
    if ( ms == "0" )
      continue;
    int k = -1;
    if ( !ms.empty() )
      k = Plugins::index( ms, PluginID );

    // check plugin:
    string ident = deviceopts.text( "ident" );
    if ( ident.empty() ) {
      Warnings += "You need to provide an identifier for the <b>" + ms
	+ "</b> plugin !\n";
      continue;
    }
    int deviceindex = -1;
    bool alreadyopen = false;
    for ( unsigned int i=0; i<DVs.size(); i++ ) {
      if ( DVs[i] != 0 && DVs[i]->deviceIdent() == ident ) {
	deviceindex = i;
	if ( DVs[i]->isOpen() )
	  alreadyopen = true;
	break;
      }
    }
    if ( alreadyopen )
      continue;

    // create plugin:
    if ( k >= 0 ) {
      void *mp = 0;
      if ( deviceindex >= 0 )
	mp = DVs[deviceindex];
      else
	mp = Plugins::create( k );
      if ( mp != 0 ) {
	// add plugin:
	T *dv = static_cast<T*>( mp );
	if ( deviceindex < 0 ) {
	  dv->setDeviceIdent( ident );
	  add( dv, devices );
	}
	else {
	  // swap device to the end of the lists:
	  swapBack( dv );
	  if ( (void *)&devices != (void *)this )
	    devices.swapBack( dv );
	}
	// open device:
	Str ds = deviceopts.text( "device" );
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
	  dv->open( *d, deviceopts );
	  if ( dv->isOpen() )
	    ds = "";
	}
	if ( ! ds.empty() )
	  dv->open( ds, deviceopts );
	if ( dv->isOpen() )
	  n++;
	else {
	  if ( ms.empty() )
	    ms = "-empty-";
	  if ( ds.empty() )
	    ds = "-empty-";
	  Warnings += "Cannot open " + Name + " plugin <b>" + ms
	    + "</b> with identifier <b>" + ident + "</b> on device <b>" + ds + " !\n";
	}
      }
      else {
	if ( ms.empty() )
	  ms = "-empty-";
	Warnings += "Cannot create " + Name + " plugin <b>" + ms + "</b> !\n";
      }
    }
    else {
      if ( ms.empty() )
	ms = "-empty-";
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
  Options::clear();
  Options::load( sq, ":" );
}


template < class T, int PluginID >
void DeviceList<T,PluginID>::saveConfig( ofstream &str )
{
  Options::save( str, "  ", -1, 0, false, false );
}


template < class T, int PluginID >
void DeviceList<T,PluginID>::addMenu( QMenu *menu, int &index )
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
      Menus[k] = menu->addMenu( s.c_str() );
    Menus[k]->clear();
    for ( int j=0; j<DVs[k]->info().size(); j++ )
      Menus[k]->addAction( DVs[k]->info()[j].save().c_str() );
    if ( ! DVs[k]->settings().empty() )
      Menus[k]->addSeparator();
    for ( int j=0; j<DVs[k]->settings().size(); j++ )
      Menus[k]->addAction( DVs[k]->settings()[j].save().c_str() );
  }
}


template < class T, int PluginID >
void DeviceList<T,PluginID>::updateMenu( void )
{
  for ( unsigned int k=0; k<DVs.size(); k++ ) {

    if ( Menus[k] == NULL )
      continue;

    Menus[k]->clear();
    for ( int j=0; j<DVs[k]->info().size(); j++ )
      Menus[k]->addAction( DVs[k]->info()[j].save().c_str() );
    if ( ! DVs[k]->settings().empty() )
      Menus[k]->addSeparator();
    for ( int j=0; j<DVs[k]->settings().size(); j++ )
      Menus[k]->addAction( DVs[k]->settings()[j].save().c_str() );
  }
}


template < class T, int PluginID >
ostream &operator<<( ostream &str, const DeviceList<T,PluginID> &d )
{
  for ( unsigned int k=0; k<d.DVs.size(); k++ )
    str << *d.DVs[k] << '\n';
  return str;
}


}; /* namespace relacs */

#endif /* ! _RELACS_DEVICELIST_H_ */

