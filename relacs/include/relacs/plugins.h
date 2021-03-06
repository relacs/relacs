/*
  plugins.h
  Dynamically load plugins (libraries) into the running program.

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

#ifndef _RELACS_PLUGINS_H_
#define _RELACS_PLUGINS_H_ 1


#include <string>
#include <vector>
#include <relacs/strqueue.h>
using namespace std;

namespace relacs {


/*!
\class Plugins
\brief Dynamically load plugins (libraries) into the running program.
\author Jan Benda

Plugins has a file list with all library files that are opened
and a plugins list containing the plugins
(a library can contain more than one plugin).

With the open(), openPath(), and OpenFile() functions
you can add files to the file list
and load the libraries into the program.
The plugins in the library automatically add themselves
to the plugins list.
The close() functions remove the libraries from the program
but keep the filenames in the list.
The plugins of a closed library are removed from the plugins list.
To close the libraries and remove them from the file list
use the erase() functions.
The clear() function tries to erase all libraries.
With reopen() a library can be first closed
and than opened again.
The number of library files in the list is returned by size(),
the list can alos be empty().

Each plugin has an identifier string ident(), an index(),
a type(), and is contained by the library file with file id fileID().
The type() of a plugin is used to group plugins of different type together,
i.e. classes with the same base class.
The identifier string of the first plugin in the list with a given type
is returned by \a first().
The number of loaded plugins is plugins().

An instance of a plugin is created by create() and destroyed
by destroy().

Errors concerning the library files are retuned by libraryErrors().
Errors concerning the plugin classes are retuned by classErrors().

In order to make a class a plugin that is managed by Plugins
addPlugin() must be "called", i.e. it must be added to the end
of the source file. A class can be added to or removed from
the plugins explicitely by calling add() and 
erase(const string&,const string&,int), respectively.
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
      /*! Some plugins of the library are still in use. */
    LibraryInUse,
      /*! The requested plugin does not exist. */
    EraseFailed,
      /*! The file cannot be erased from the list. */
    InvalidPlugin
  };

    /*! Constructor. Does nothing. 
        \note You should never construct a Plugins class.
        Plugins construct themselfes when their library file 
	is opened. */
  Plugins( void );
    /*! Destructor. Does nothing. */
  ~Plugins( void );

    /*! Load the library with file name \a file into the program
        if it is not already loaded
	(it may, however, be already in the list of library files).
	\param[in] file the file name of the library to be loaded. 
        \return the id of the library on success.
        If the library is already loaded, -AlreadyLoaded is returned.
        If the library cannot be loaded, -LibraryError is returned
        and a string explaining the reason is added to the library errors
        which can be read using libraryErrors().
        \sa openPath(), openFile(), close(), erase(), clear(), reopen() */
  static int open( const string &file );
    /*! Load the library with id \a id into the program
        if it is not already loaded. 
	\param[in] id the identification number of the library to be loaded.
        \return the id of the library on success. 
        If \a id is invalid, -InvalidFile is returned.
        If the library is already loaded, -AlreadyLoaded is returned.
        If the library cannot be loaded, -LibraryError is returned
        and a string explaining the reason is added to the library errors
        which can be read using libraryErrors().
        \sa openPath(), openFile(), close(), erase(), clear(), reopen() */
  static int open( int id );
    /*! Load all libraries specified by the path \a path into the program. 
	Relative pathes (not starting with a slash) are interpreted
	relative to \a relativepath, provided \a relativepath is not empty.
	If \a path is a pure filename (no directory, but may contain wildcards)
	then it is assumed to be found in one of the \a pluginhomes directories.
	If \a pluginhomes is an empty string, then pure filenames are also interpreted
	relative to \a relativepath, provided \a relativepath is not empty.
	\param[in] path the file pattern (can include wildcard characters)
	selecting the libraries to be loaded. 
	If \a path ends with a slash, "*" is added first.
	If \a path has no file extension, the appropriate extension (".so")
	is added.
	If the filename of \a path does not start with "lib",
	the filename is prefixed with "lib".
	\param[in] relativepath the path to be used for relative pathes. 
	\param[in] pluginhomes default pathes for the plugin files.
        \return the number of the successfully loaded libraries. 
        \sa open(), openFile(), close(), erase(), clear(), reopen() */
  static int openPath( const string &path, const string &relativepath,
		       const StrQueue &pluginhomes );
    /*! Load all libraries listed in the file \a file into the program.
        \param[in] file the name of a text file that contains in each line
	the file name of a library that is to be loaded.
        \return the number of the successfully loaded libraries. 
        If \a file is invalid, -CantGetFiles is returned.
        \sa open(), openPath(), close(), erase(), clear(), reopen() */
  static int openFile( const string &file );

    /*! Close library specified by its id \a id.
        The library is not removed from the list.
        You can open the library again with the open() functions.
	Use erase() to close the library and remove it from the list. 
	\param[in] id the identification number of the library to be closed.
        \return the id of the library on success.
	If \a id is invalid, -InvalidFile is returned.
        If still some plugins of the libraries are used,
	-LibraryInUse is returned.
        \sa open(), openPath(), openFile(), erase(), clear(), reopen() */
  static int close( int id );
    /*! Close library specified by its file name \a file.
        The library is not removed from the list.
        You can open the library again with the open() functions.
	Use erase() to close the library and remove it from the list. 
	\param[in] file the file name of the library to be closed. 
        \return the id of the library on success.
	If \a file is not found in the list of opened libraries, 
        -InvalidFile is returned.
        If still some plugins of the libraries are used,
	-LibraryInUse is returned.
        \sa open(), openPath(), openFile(), erase(), clear(), reopen() */
  static int close( const string &file );
    /*! Close all libraries. 
        The libraries are not removed from the list.
        You can open the libraries again with the open() functions.
	Use clear() to close the libraries and remove them from the list.
        \return 0 if all libraries were closed successfully.
	If some libraries cannot be closed, since some plugins are still used,
        the negative of the number of these libraries is returned.
        \sa open(), openPath(), openFile(), erase(), clear(), reopen() */
  static int close( void );

    /*! Close library specified by its id \a id and remove it form the list.
	\param[in] id the identification number of the library to be erased.
        \return the id of the library on success.
        If \a id is invalid, -InvalidFile is returned.
        If still some plugins of the libraries are used,
	-LibraryInUse is returned.
        \sa open(), openPath(), openFile(), close(), clear(), reopen() */
  static int erase( int id );
    /*! Close library specified by its file name \a file
        and remove it form the list.
	\param[in] file the file name of the library to be erased. 
        \return the id of the library on success.
        If \a file is not found in the list of opened libraries, 
        -InvalidFile is returned.
        If still some plugins of the libraries are used,
	-LibraryInUse is returned.
        \sa open(), openPath(), openFile(), close(), clear(), reopen() */
  static int erase( const string &file );
    /*! Close all libraries and remove them from the list. 
        \return 0 if all libraries were closed successfully.
	If some libraries cannot be closed and removed, 
	since some plugins are still used,
        the negative of the number of these libraries is returned.
        \sa open(), openPath(), openFile(), close(), erase(), reopen() */
  static int clear( void );

    /*! Load the library with id \a id into the program
        after closing it.
	\param[in] id the identification number of the library to be reopened.
        \return the id of the library.
        \sa open(), openPath(), openFile(), close(), erase(), clear() */
  static int reopen( int id ); 
    /*! Load the library with file name \a file into the program
        after closing it.
	\param[in] file the file name of the library to be reopened. 
        \return the id of the library.
        \sa open(), openPath(), openFile(), close(), erase(), clear() */
  static int reopen( const string &file ); 

    /*! \return the number of libraries in the list.
        \sa empty(), open(), erase() */
  static int size( void );
    /*! \return \c true if no libraries are contained in the list.
        \sa size(), open(), erase() */
  static bool empty( void );

    /*! \return the number of plugins in the plugins list. */
  static int plugins( void );
    /*! \return the number of plugins in the plugins list of type \a type.
        \param[in] type the type of the plugins. */
  static int plugins( int type );
    /*! \return the identfier string of the plugin specified by its index \a index.
        \param[in] index the index of the plugin.
        \sa first(), index() */
  static string ident( int index );
    /*! \return the identifier string of the first plugin which is of type \a type.
        \param[in] type the type of the plugin.
        \sa ident(), index() */
  static string first( int type=0 );
    /*! \return the index of a plugin which is specified by its 
        identifier string \a plugin and its type \a type.
        \a -InvalidPlugin is returned if the plugin was not found.
        \param[in] plugin the name of the plugin.
	\param[in] type the type of the plugins. If negative,
	the type is not used to identify the plugin.
        \sa ident() */
  static int index( const string &plugin, int type=-1 );
    /*! \return the type of the plugin specified by its index \a index.
        \param[in] index the index of the plugin.
        \sa ident(), fileID() */
  static int type( int index );
    /*! \return the type of the plugin specified by its identifier string \a plugin.
        \param[in] plugin the name of the plugin.
        \sa index(), fileID() */
  static int type( const string &plugin );
    /*! \return the file id of the library which contains the plugin
        specified by its index \a index.
        \param[in] index the index of the plugin.
        \sa ident(), type() */
  static int fileID( int index );
    /*! \return the file id of the library which contains the plugin
        specified by its identifier string \a plugin.
        \param[in] plugin the name of the plugin.
        \sa index(), type() */
  static int fileID( const string &plugin );

    /*! Constructs a new instance of the plugin specified by its index \a index. 
        \return a pointer to that plugin.
	\param[in] index the index of the plugin to be created.
        \sa destroy(), plugins() */
  static void *create( int index );
    /*! Constructs a new instance of the plugin specified by its 
        identifier string \a plugin and its type \a type. 
        \return a pointer to the new instance of the plugin.
	\param[in] plugin the name of the plugin to be created.
	\param[in] type the type of the plugin.
        \sa destroy(), plugins() */
  static void *create( const string &plugin, int type );
    /*! If you delete a plugin which has been created
        by one of the create functions,
        call this function to decrement its use count.
	\note the plugin is not removed from the list of plugins.
	\return the number of still existing instances of the specified	plugin.
        \param[in] index the index of the plugin to be destroyed.
        \sa create(), plugins() */
  static int destroy( int index );
    /*! If you delete a plugin which has been created
        by one of the create functions,
        call this function to decrement its use count.
	\note the plugin is not removed from the list of plugins.
	\return the number of still existing instances of the specified	plugin.
        \param[in] plugin the identifier string of the plugin to be destroyed.
	\param[in] type the type of the plugin
        \sa create(), plugins() */
  static int destroy( const string &plugin, int type );

    /*! \return a pointer to a new instance of the plugin. */
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

    /*! \return a string containing the names of all libraries
        which could not be loaded.
        \sa clearLibraryError(), classErrors() */
  static const string &libraryErrors( void );
    /*! Clear the string containing the library errors.
        \sa libraryError(), classError() */ 
  static void clearLibraryErrors( void );

    /*! Get a string containing the names of all plugin-classes
        which could not be loaded.
        \sa clearClassErrors(), libraryErrors() */
  static const string &classErrors( void );
    /*! Clear the string containing the plugin-class errors.
        \sa classErrors(), libraryErrors() */ 
  static void clearClassErrors( void );

    /*! Writes the content of the library file list and the plugin list
        to \a str. */
  friend ostream &operator<< ( ostream &str, const Plugins &plugins );


private:

    /*! Append string \a error to the list of libraries  errors.
        \param[in] error a string that should be appended 
	to the list of library errors.
        \sa libraryErrors(), clearLibraryError(), classErrors() */
  static void addLibraryError( const string &error );
    /*! Append string \a error to the list of class errors.
        \param[in] error a string that should be appended 
	to the list of plugin-class errors.
        \sa classErrors(), clearClassError(), libraryErrors() */
  static void addClassError( const string &error );

  struct PluginInfo
  {
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

  struct FileInfo
  {
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

    /*! Returns a new unused file id. */
  static int newFileID( void );

    /*! The file ID of the currently opened library. */
  static int CurrentFileID;
};


  /*! \relates Plugins
      Makes a class a plugin that is managed by Plugins.
      \param[in] pluginClass the class name
      \param[in] pluginSet the name of the plugin set
      \param[out] pluginType the type of the plugin */
#define addPlugin( pluginClass, pluginSet, pluginType )	\
\
void* create ## pluginClass( void ) \
{ \
  return new pluginClass(); \
} \
\
class Reg ## pluginClass : public Plugins \
{ \
public: \
  Reg ## pluginClass() \
  { \
    add( string( # pluginClass ) + "[" + # pluginSet  + "]",	\
	 pluginType, create ## pluginClass, VERSION );	   \
  } \
\
  ~Reg ## pluginClass() \
  { \
    erase( string( # pluginClass ) + "[" + # pluginSet  + "]", pluginType ); \
  } \
}; \
\
Reg ## pluginClass init ## pluginClass;


}; /* namespace relacs */

#endif /* ! _RELACS_PLUGINS_H_ */

