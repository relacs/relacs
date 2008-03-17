/*
  plugins.h
  Dynamically load plugins (libraries) into the running program.

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

#ifndef _PLUGINS_H_
#define _PLUGINS_H_


#include <string>
#include <vector>
using namespace std;


/*!
  \class Plugins
  \brief Dynamically load plugins (libraries) into the running program.
  \author Jan Benda
  \version 1.0

  Plugins has a file list with all library files that are opened
  and a plugins list containing the plugins
  (a library can contain more than one plugin).
  With the open() functions you can add files to the file list
  and load the libraries into the program.
  The plugins in the library automatically add themselves
  to the plugins list.
  The close() function removes the libraries from the program
  but keeps the filename in the list.
  The plugins of a closed library are removed from the plugins list.
  To close the libraries and remove them from the file list
  use the erase() functions.
  The clear() function tries to erase all libraries.
  With reopen() a library can be first closed
  and than opened again.
*/


class Plugins
{

public:

    /*! Error codes for handling plugins. */
  enum ErrorCode { 
      /*! The library is already loaded. */
    AlreadyLoaded,
      /*! Error in loading the library. */
    LibraryError,
      /*! The requested file does not exist. */
    InvalidFile,
      /*! Can't read files of a directory or from file. */
    CantGetFiles,
      /*! No files found in the directory or file. */
    NoFiles,
      /*! Some plugins of the library are still in use. */
    LibraryInUse,
      /*! The requested plugin does not exist. */
    EraseFailed,
      /*! The file cannot be erased from the list. */
    InvalidPlugin
  };

    /*! Constructor. Does nothing. */
  Plugins( void );
    /*! Destructor. */
  ~Plugins( void );

    /*! Load the library with file name \a file into the program
        if it is not already loaded
	(it may however be already in the list of libraries).
        Returns the id of the library on success.
        If the library is already loaded, -AlreadyLoaded is returned.
        If the library cannot be loaded, -LibraryError is returned
        and a string explaining the reason is added to the library errors
        which can be read using libraryErrors(). */
  static int open( const string &file );
    /*! Load the library with id \a id into the program
        if it is not already loaded. 
        Returns the id of the library on success. 
        If \a id is invalid, -InvalidFile is returned.
        If the library is already loaded, -AlreadyLoaded is returned.
        If the library cannot be loaded, -LibraryError is returned
        and a string explaining the reason is added to the library errors
        which can be read using libraryErrors(). */
  static int open( int id );
    /*! Load all libraries specified by the path \a path into the program. 
        \a path may include wildcard characters. 
	If \a path starts with "[PLUGINHOME]" it is replaced by \a pluginhome.
        Returns the id of the first successfully loaded library. 
        If \a path is invalid, -CantGetFiles is returned.
        If no library was opened successfully, -NoFiles is returned. */
  static int openPath( const string &path, const string &pluginhome );
    /*! Load all libraries listed in the file \a file into the program.
        Returns the index of the first successfully loaded library.
        If \a file is invalid, -CantGetFiles is returned.
        If no library was opened successfully, -NoFiles is returned. */
  static int openFile( const string &file );

    /*! Close library specified by its id \a id.
        The library is not removed from the list.
        You can open the library again with the open() functions.
	Use erase() to close the library and remove it from the list. 
        If \a id is invalid, -InvalidFile is returned.
        If still some plugins of the libraries are used, -LibraryInUse is returned.
        On success, the id of the library is returned. */
  static int close( int id );
    /*! Close library specified by its file name \a file.
        The library is not removed from the list.
        You can open the library again with the open() functions.
	Use erase() to close the library and remove it from the list. 
        If \a file is not found in the list of opened libraries, 
        -InvalidFile is returned.
        If still some plugins of the libraries are used,
	-LibraryInUse is returned.
        On success, the id of the library is returned. */
  static int close( const string &file );
    /*! Close all libraries. 
        The libraries are not removed from the list.
        You can open the libraries again with the open() functions.
	Use clear() to close the libraries and remove them from the list.
	If all libraries were closed successfully, 0 is returned.
	If some libraries cannot be closed, since some plugins are still used,
        the negative of the number of these libraries is returned. */
  static int close( void );

    /*! Close library specified by its id \a id and remove it form the list.
        If \a id is invalid, -InvalidFile is returned.
        If still some plugins of the libraries are used,
	-LibraryInUse is returned.
        On success, the id of the library is returned. */
  static int erase( int id );
    /*! Close library specified by its file name \a file
        and remove it form the list.
        If \a file is not found in the list of opened libraries, 
        -InvalidFile is returned.
        If still some plugins of the libraries are used,
	-LibraryInUse is returned.
        On success, the id of the library is returned. */
  static int erase( const string &file );
    /*! Close all libraries and remove them from the list. 
	If all libraries were closed successfully, 0 is returned.
	If some libraries cannot be closed and removed, 
	since some plugins are still used,
        the negative of the number of these libraries is returned. */
  static int clear( void );

    /*! Load the library with id \a id into the program
        after closing it.
        Returns the id of the library. */
  static int reopen( int id ); 
    /*! Load the library with file name \a file into the program
        after closing it.
        Returns the id of the library. */
  static int reopen( const string &file ); 

    /*! Number of libraries in the list. */
  static int size( void ) { return Files.size(); };
    /*! True if no libraries are in the list. */
  static bool empty( void ) { return Files.empty(); };

    /*! The number of plugins in the plugins list. */
  static int plugins( void ) { return Plugs.size(); };
    /*! The number of plugins in the plugins list of type \a type. */
  static int plugins( int type );
    /*! The identfier string of the plugin specified by its index \a index. */
  static string ident( int index );
    /*! The index of a plugin which is specified by its 
        identifier string \a plugin and its type \a type.
        If \a type is negative,
	the type is not used to identify the plugin.
        \a -InvalidPlugin is returned if the plugin was not found. */
  static int index( const string &plugin, int type=-1 );
    /*! The type of the plugin specified by its index \a index. */
  static int type( int index );
    /*! The type of the plugin specified by its identifier string \a plugin. */
  static int type( const string &plugin ) { return type( index( plugin, -1 ) ); };
    /*! The file id of the library which contains the plugin
        specified by its index \a index. */
  static int fileID( int index );
    /*! The file id of the library which contains the plugin
        specified by its identifier string \a plugin. */
  static int fileID( const string &plugin ) { return fileID( index( plugin, -1 ) ); };
    /*! Constructs a new instance of the plugin specified by its index \a index. 
        Returns a pointer to that plugin. */
  static void *create( int index );
    /*! Constructs a new instance of the plugin specified by its 
        identifier string \a plugin. 
        Returns a pointer to that plugin. */
  static void *create( const string &plugin, int type ) { return create( index( plugin, type ) ); };
    /*! If you delete a plugin which has been created
        by one of the create functions,
        call this function to decrement its use count.
	The number of still existing instances of the specified
	plugin is returned.
        The plugin is specified by its index \a index. */
  static int destroy( int index );
    /*! If you delete a plugin which has been created
        by one of the create functions,
        call this function to decrement its use count.
	The number of still existing instances of the specified
	plugin is returned.
        The plugin is specified by its identifier string \a plugin. */
  static int destroy( const string &plugin, int type ) { return destroy( index( plugin, type ) ); };
    /*! The identifier string of the first plugin which is of type \a type. */
  static string first( int type=0 );

    /*! Get a string containing the names of all libraries
        which could not be loaded. */
  static const string &libraryErrors( void ) { return LibraryErrors; };
    /*! Clear the string containing the library errors. */ 
  static void clearLibraryErrors( void ) { LibraryErrors = ""; };

    /*! Get a string containing the names of all plugin-classes
        which could not be loaded. */
  static const string &classErrors( void ) { return ClassErrors; };
    /*! Clear the string containing the plugin-class errors. */ 
  static void clearClassErrors( void ) { ClassErrors = ""; };

    /*! Returns \c true if the irectory is changed to the libraries directory
        before loading it. */
  static bool changeToPluginDir( void ) { return ChangeToLibDir; };
    /*! Determine whether to change to the libraries direcory before loading it. */
  static void setChangeToPluginDir( bool chdir=true ) { ChangeToLibDir = chdir; };

  friend ostream &operator<< ( ostream &str, const Plugins &plugins );


  typedef void *(*PluginCreator)( void );

    /*! Add a plugin with identifier string \a ident and plugin type \a type
        to the plugin list.
	The plugin can be created by calling create().
        The plugin was compiled with version number \a version.
        If the version number does not match the one of the Plugins class,
        an error string is appened to the class errors which
	can be read using classErrors(). */
  static void add( const string &ident, int type, PluginCreator create, 
		   const string &version );
    /*! Remove the plugin specified by its identifier string \a ident
        and plugin type \a type from the plugin list. */
  static void erase( const string &ident, int type );

    /*! Append string \a error to the list of libraries  errors.  */
  static void addLibraryError( const string &error ) { LibraryErrors += error; LibraryErrors += '\n'; };
    /*! Append string \a error to the list of class errors. */
  static void addClassError( const string &error ) { ClassErrors += error; ClassErrors += '\n'; };


private:

  class PluginInfo
  {
  public:
    PluginInfo( const string &ident, int type, PluginCreator create, int fileid );
  
      /*! Identifier string of the plugin. */  
    string Ident;
      /*! Type of the plugin. */
    int Type;
      /*! ID of the library, from which the plugin comes from. */
    int FileID;
      /*! Counts the number of created and existing instances of the plugin. */
    int UseCount;
      /*! The function to create the plugin. */
    PluginCreator Create;
  };

  typedef vector< PluginInfo > PluginsType;
    /*! The list of plugins. */
  static PluginsType Plugs;

  class FileInfo
  {
  public:
    FileInfo( const string &file, void *lib, int fileid );
  
      /*! File name of the library. */  
    string File;
      /*! Pointer to the library. */
    void *Lib;
      /*! ID of the library. */
    int FileID;
  };

  typedef vector< FileInfo > FilesType;
    /*! The list of libraries. */
  static FilesType Files;

    /*! Names of libraries which could not be loaded. */
  static string LibraryErrors;
    /*! Names of classes contained in loaded libraries which could not be loaded. */
  static string ClassErrors;

    /*! The version number of the Plugins class. */
  static string Version;

    /*! Change to the directory of the libary before loeading it. */ 
  static bool ChangeToLibDir;

    /*! Returns a new unused file id. */
  static int newFileID( void );

    /*! The file ID of the currently opened library. */
  static int CurrentFileID;
};


#define addPlugin( pluginClass, pluginType ) \
\
void* create ## pluginClass( void ) \
{ \
  return new pluginClass(); \
} \
\
class reg ## pluginClass : public Plugins \
{ \
public: \
  reg ## pluginClass() \
  { \
    add( # pluginClass, pluginType, create ## pluginClass, PLUGINVERSION ); \
  } \
\
  ~reg ## pluginClass() \
  { \
    erase( # pluginClass, pluginType ); \
  } \
}; \
\
reg ## pluginClass init ## pluginClass;


#endif
