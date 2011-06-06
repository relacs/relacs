/*
  plugins.cc
  Dynamically load plugins (libraries) into the running program.

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

#include <cstdlib>
#include <dlfcn.h>
#include <fstream>
#include <iostream>
#include <QDir>
#include <QFileInfo>
#include <relacs/str.h>
#include <relacs/plugins.h>
using namespace std;

namespace relacs {


#ifdef VERSION
  string Plugins::Version = VERSION;
#else
  #error no VERSION string defined!
#endif

Plugins::PluginsType Plugins::Plugs;
Plugins::FilesType Plugins::Files;

string Plugins::LibraryErrors = "";
string Plugins::ClassErrors = "";
int Plugins::CurrentFileID = 0;


Plugins::Plugins( void )
{
}


Plugins::~Plugins( void )
{
}


int Plugins::open( const string &file )
{
  // already loaded?
  unsigned int index;
  for ( index=0; index<Files.size(); index++ )
    if ( Files[index].File == file && Files[index].Lib != 0 )
      return -AlreadyLoaded;

  // load plugin file:
  CurrentFileID = newFileID();
  void *dlib = dlopen( file.c_str(), RTLD_NOW | RTLD_GLOBAL );

  // check success:
  if ( dlib == 0 ) {
    char *es;
    es = dlerror();
    if ( es != 0 ) {
      addLibraryError( "<b>" + file + ":</b> " + es );
    }
    return -LibraryError;
  }

  // success! memorize plugin file:
  if ( index < Files.size() && Files[index].File == file ) {
    Files[index].Lib = dlib;
    Files[index].FileID = CurrentFileID;
  }
  else {
    Files.push_back( FileInfo( file, dlib, CurrentFileID ) );
  }

  return CurrentFileID;
}


int Plugins::open( int id )
{
  unsigned int index;
  for ( index=0; index<Files.size(); index++ )
    if ( Files[index].FileID == id )
      break;

  // not found:
  if ( index >= Files.size() )
    return -InvalidFile;

  // already loaded?
  if ( Files[index].Lib != 0 )
      return -AlreadyLoaded;
  
  string file = Files[index].File;

  // load plugin file:
  CurrentFileID = newFileID();
  void *dlib = dlopen( file.c_str(), RTLD_NOW | RTLD_GLOBAL );

  // check success:
  if ( dlib == 0 ) {
    char *es;
    es = dlerror();
    if ( es != 0 ) {
      addLibraryError( "<b>" + file + ":</b> " + es );
    }
    return -LibraryError;
  }

  // success!
  Files[index].Lib = dlib;
  Files[index].FileID = CurrentFileID;

  return CurrentFileID;
}


int Plugins::openPath( const string &path, const string &relativepath,
		       const StrQueue &pluginhomes )
{
  Str p( path );
  p.strip();
  if ( p.empty() )
    return 0;

  // add pattern:
  if ( p[p.size()-1] == '/' )
    p += "*";
  if ( p.extension().empty() ) {
    p.providePeriod();
    p += "so";
  }

  // possible relative pathes:
  StrQueue rps;
  if ( p[0] != '/' ) {
    // plugin homes:
    if ( p.find( '/' ) < 0 &&
	 ! pluginhomes.empty() && !pluginhomes[0].empty() )
      rps = pluginhomes;
    // or relative path:
    else if ( ! relativepath.empty() )
      rps = relativepath;
    else
      rps = "./";
  }
  else {
    // absolute path:
    rps = "";
  }

  // read all libraries specified by path:
  int n = 0;
  for ( int j=0; j<rps.size(); j++ ) {
    // add relative path:
    Str file = p;
    if ( ! rps[j].empty() ) {
      rps[j].provideSlash();
      file = rps[j] + p;
    }
    // check prefix:
    string filename = file.notdir();
    if ( filename.substr( 0, 3 ) != "lib" )
      filename = "lib" + filename;
    // look for files in directory:
    QDir files( file.dir().c_str(), filename.c_str() );
    for ( unsigned int k=0; k < files.count(); k++ ) {
      Str libfile = files.absoluteFilePath( files[k] ).toStdString();
      QFileInfo qfi( libfile.c_str() );
      if ( qfi.exists() ) {
	int r = open( libfile );
	if ( r >= 0 || r == -AlreadyLoaded )
	  n ++;
      }
    }
  }
  
  return n;
}


int Plugins::openFile( const string &file )
{
  // read libraries from file:
  ifstream pluginFile( file.c_str() );
  if ( !pluginFile ) 
    return -CantGetFiles;

  int n = 0;
  string ls, ss;
  while ( getline( pluginFile, ls ) )
    if ( ls.length() > 0 && ls[0] != '#' ) {
      unsigned int k = ls.find_last_not_of( " \t\n" );
      ls.resize( k + 1 );
      if ( ls[0] == '/' )
	ss = ls;
      else
	ss = "./" + ls;
      
      int r = open( ss );
      if ( r >= 0 )
	n++;
    }
  
  return n;
}


int Plugins::close( int id )
{
  unsigned int index;
  for ( index=0; index<Files.size(); index++ )
    if ( Files[index].FileID == id )
      break;

  // not found:
  if ( index >= Files.size() )
    return -InvalidFile;

  // check for still used plugins:
  for ( unsigned int k=0; k<Plugs.size(); k++ ) {
    if ( Plugs[k].FileID == id && Plugs[k].UseCount > 0 )
      return -LibraryInUse;
  }

  // remove plugins:
  for ( PluginsType::iterator p = Plugs.begin(); p != Plugs.end(); )
    if ( p->FileID == id )
      p = Plugs.erase( p );
    else
      ++p;

  // close file:
  if ( dlclose( Files[index].Lib ) != 0 )
    return -LibraryInUse;

  // mark as unused:
  Files[index].Lib = 0;

  return id;
}


int Plugins::close( const string &file )
{
  for ( unsigned int k=0; k<Files.size(); k++ )
    if ( Files[k].File == file )
      return close( Files[k].FileID );

  return -InvalidFile;
}


int Plugins::close( void )
{
  int r = 0;
  for ( unsigned int k=0; k<Files.size(); k++ ) {
    int c = close( Files[k].FileID );
    if ( c < 0 )
      r++;
  }
  return -r;
}


int Plugins::erase( int id )
{
  int r = close( id );
  if ( r < 0 )
    return r;
  
  FilesType::iterator p;
  for ( p = Files.begin(); p != Files.end() && p->FileID != id; ++p );

  if ( p != Files.end() && p->Lib == 0 )
    Files.erase( p );
  else
    return -EraseFailed;

  return id;
}


int Plugins::erase( const string &file )
{
  int r = close( file );
  if ( r < 0 )
    return r;

  return erase( r );  
}


int Plugins::clear( void )
{
  int r = close();

  if ( r >= 0 ) {
    Files.clear();
    return 0;
  }

  for ( FilesType::iterator p = Files.begin(); p != Files.end(); )
    if ( p->Lib == 0 )
      p = Files.erase( p );
    else
      ++p;

  return r;
}


int Plugins::reopen( const string &file )
{
  int r = close( file );
  if ( r < 0 ) {
    cerr << "! error in Plugins::reopen() -> cannot close file " << file << endl;
    return r;
  }

  // get index of the file:
  unsigned int index;
  for ( index=0; index<Files.size(); index++ )
    if ( Files[index].File == file )
      break;

  // not found (should not happen!):
  if ( index >= Files.size() ) {
    cerr << "! error in Plugins::reopen() -> didn't find FileID " << file << endl;
    return -InvalidFile;
  }

  // erase file from list:
  Files.erase( Files.begin() + index );
  
  // load plugin file:
  CurrentFileID = newFileID();
  void *dlib = dlopen( file.c_str(), RTLD_NOW | RTLD_GLOBAL );

  // check success:
  if ( dlib == 0 ) {
    char *es;
    es = dlerror();
    if ( es != 0 ) {
      addLibraryError( "<b>" + file + ":</b> " + es );
    }
    return -LibraryError;
  }

  // memorize plugin file:
  Files.push_back( FileInfo( file, dlib, CurrentFileID ) );
  return CurrentFileID;
}


int Plugins::reopen( int id )
{
  int r = close( id );
  if ( r < 0 ) {
    cerr << "! error in Plugins::reopen() -> cannot close file " << id << endl;
    return r;
  }

  // get index of the file:
  unsigned int index;
  for ( index=0; index<Files.size(); index++ )
    if ( Files[index].FileID == id )
      break;

  // not found (should not happen!):
  if ( index >= Files.size() ) {
    cerr << "! error in Plugins::reopen() -> didn't find FileID " << id << endl;
    return -InvalidFile;
  }

  // erase file from list:
  string file = Files[index].File;
  Files.erase( Files.begin() + index );
  
  // load plugin file:
  CurrentFileID = newFileID();
  void *dlib = dlopen( file.c_str(), RTLD_NOW | RTLD_GLOBAL );

  // check success:
  if ( dlib == 0 ) {
    char *es;
    es = dlerror();
    if ( es != 0 ) {
      addLibraryError( "<b>" + file + ":</b> " + es );
    }
    return -LibraryError;
  }

  // memorize plugin file:
  Files.push_back( FileInfo( file, dlib, CurrentFileID ) );
  return CurrentFileID;
}


int Plugins::size( void )
{
  return Files.size();
}


bool Plugins::empty( void )
{
  return Files.empty();
}


int Plugins::plugins( void )
{
  return Plugs.size();
}


int Plugins::plugins( int type )
{
  if ( type <= 0 )
    return Plugs.size();

  int n=0;
  for ( unsigned int k=0; k<Plugs.size(); k++ )
    if ( (Plugs[k].Type & type) == type )
      n++;

  return n;
}


string Plugins::ident( int index )
{
  if ( index >= 0 && index < (int)Plugs.size() )
    return Plugs[index].Ident;

  return "";
}


string Plugins::first( int type )
{
  for ( unsigned int index=0; index<Plugs.size(); index++ )
    if ( type <= 0 || (Plugs[index].Type & type) > 0 )
      return Plugs[index].Ident;

  return "";
}


int Plugins::index( const string &plugin, int type )
{
  for ( unsigned int k=0; k<Plugs.size(); k++ )
    if ( ( type <= 0 || (Plugs[k].Type & type) == type ) &&
	 Plugs[k].Ident == plugin )
      return k;

  return -InvalidPlugin;
}


int Plugins::type( int index )
{
  if ( index >= 0 && index < (int)Plugs.size() )
    return Plugs[index].Type;

  return -InvalidPlugin;
}


int Plugins::type( const string &plugin )
{
  return type( index( plugin, -1 ) );
}


int Plugins::fileID( int index )
{
  if ( index >= 0 && index < (int)Plugs.size() )
    return Plugs[index].FileID;

  return -InvalidPlugin;
}


int Plugins::fileID( const string &plugin )
{
  return fileID( index( plugin, -1 ) );
}


void *Plugins::create( int index )
{
  if ( index >= 0 && index < (int)Plugs.size() ) {
    void *m = Plugs[index].Create();
    if ( m != 0 )
      Plugs[index].UseCount++;
    return m;
  }

  return 0;
}


void *Plugins::create( const string &plugin, int type )
{
  return create( index( plugin, type ) );
}


int Plugins::destroy( int index )
{
  if ( index >= 0 && index < (int)Plugs.size() ) {
    if ( Plugs[index].UseCount > 0 )
      Plugs[index].UseCount--;
    return Plugs[index].UseCount;
  }

  return -InvalidPlugin;
}


int Plugins::destroy( const string &plugin, int type )
{
  return destroy( index( plugin, type ) );
}


const string &Plugins::libraryErrors( void )
{
  return LibraryErrors;
}


void Plugins::addLibraryError( const string &error )
{
  LibraryErrors += error;
  LibraryErrors += '\n';
}


void Plugins::clearLibraryErrors( void )
{
  LibraryErrors = "";
}


const string &Plugins::classErrors( void )
{
  return ClassErrors;
}


void Plugins::addClassError( const string &error )
{
  ClassErrors += error;
  ClassErrors += '\n';
}


void Plugins::clearClassErrors( void )
{
  ClassErrors = "";
}


void Plugins::add( const string &ident, int type, PluginCreator create,
		   const string &version )
{
  if ( version != Version ) {
    string es = "Version of Plugin ";
    es += ident;
    es += " is ";
    es += version;
    es += ", should be ";
    es += Version;
    addClassError( es );
    return;
  }

  for ( PluginsType::iterator p = Plugs.begin(); p != Plugs.end(); ++p )
    if ( p->Ident == ident && (p->Type & type) > 0 ) {
      string es = "Plugin ";
      es += ident;
      es += " already loaded!";
      addClassError( es );
      return;
    }

  // add plugin:
  Plugs.push_back( PluginInfo( ident, type, create, CurrentFileID ) );
}


void Plugins::erase( const string &ident, int type )
{
  for ( PluginsType::iterator p = Plugs.begin(); p != Plugs.end(); ++p )
    if ( ( type <= 0 || (p->Type & type) == type ) && p->Ident == ident ) {
      Plugs.erase( p );
      return;
    }
}


int Plugins::newFileID( void )
{
  int i = 0;
  for ( unsigned int k=0; k<Files.size(); k++ )
    if ( Files[k].FileID >= i )
      i = Files[k].FileID + 1;
  return i;
}


ostream &operator<< ( ostream &str, const Plugins &plugins )
{
  str << "Plugins:" << '\n';
  for ( unsigned int k=0; k<plugins.Plugs.size(); k++ )
    str << "  " << k << " " << plugins.Plugs[k].Ident << ":"
	<< " type=" << plugins.Plugs[k].Type
	<< " file=" << plugins.Plugs[k].FileID
	<< " use=" << plugins.Plugs[k].UseCount << '\n';

  str << "Files:" << '\n';
  for ( unsigned int k=0; k<plugins.Files.size(); k++ )
    str << "  " << plugins.Files[k].File
	<< ": id=" << plugins.Files[k].FileID << '\n';

  return str;
}


Plugins::PluginInfo::PluginInfo( const string &ident, int type, 
				 PluginCreator create, int fileid )
{
  Ident = ident;
  Type = type;
  FileID = fileid;
  UseCount = 0;
  Create = create;
}


Plugins::FileInfo::FileInfo( const string &file, void *lib, int fileid )
{
  File = file;
  Lib = lib;
  FileID = fileid;
}


}; /* namespace relacs */

