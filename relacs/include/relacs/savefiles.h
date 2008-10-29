/*
  savefiles.h
  Write data to files

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _RELACS_SAVEFILES_H_
#define _RELACS_SAVEFILES_H_ 1

#include <vector>
#include <fstream>
#include <ctime>
#include <qhbox.h>
#include <qlabel.h>
#include <qmutex.h>
#include <relacs/str.h>
#include <relacs/options.h>
#include <relacs/tablekey.h>
#include <relacs/acquire.h>
#include <relacs/outdata.h>
#include <relacs/eventdata.h>
#include <relacs/repro.h>
#include <relacs/spiketrace.h>

namespace relacs {


class RELACSWidget;

  /*! Flag for the modes of traces or events, indicating that they should be saved. */
static const int SaveFilesMode = 0x0008;

/*! 
\class SaveFiles
\brief Write data to files
\author Jan Benda
\version 2.2

is owned by RELACSWidget and used by RELACSPlugin (path() and stimulusData())
Settings (defaultPath()) and MetaData (path() only)

SaveFile sets the following environment variables:
- \c RELACSDEFAULTPATH : The default base path where RELACS stores data (inbetween sessions).
- \c RELACSDATAPATH: The base path where RELACS is currently storing data.

\bug implement saving of trace data
\bug what about no longer valid pointers in StimulusToWrite and TraceToWrite?
\bug createStimulusFile: write sample rate of all traces
\bug createStimulusFile: write name of trace file
\bug writeTrace: needs to be implemented
\bug writeStimulus: multi board signal times?
\bug createTraceFile: needs to be implemented.
\bug writeTrace: works only if all traces are written.
\todo platform independent mkdir in openFiles()!
\todo writeStimulus: adaptive time for calculating the mean rate
\todo check it carefully!
\todo warning on Disk full (or even before!)
\todo Save event sizes and widths as well.

\todo File formats: .wav: enough flexibility, compression possible, but file size limited to 2GB has to be known in advance, .au: raw data with unlimited file size und minimal header information
*/

class SaveFiles : public QHBox, public Options
{
  Q_OBJECT

public:

  SaveFiles( RELACSWidget *rw, int height,
	     QWidget *parent=0, const char *name=0 );
  ~SaveFiles( void );

    /*! The current status of writing data to files.
        \return \c true if data are currently written into files.
        \sa saving(), write(bool) */
  bool writing( void ) const;
    /*! The current status of having files ready for saving data.
        \return \c true if files are open.
        \sa writing() */
  bool saving( void ) const;

    /*! \return the base path where data are currently stored.
        \sa addPath(), setPath(), defaultPath(), pathTemplate() */
  string path( void ) const;
    /*! Set the base path for all data to be stored and
        the corresponding environment variable RELACSDATAPATH
        \param[in] path the base path for storing data.
        \a path(), setPathTemplate() */
  void setPath( const string &path );
    /*! Expand file name by the current base path for storing data.
        \param[in] file the name of the file
	\return \a file added to the current path for storing data.
        \sa path() */
  string addPath( const string &file ) const;

    /*! \return the template for the base path where data are to stored.
        \sa setPathTemplate(), path() */
  string pathTemplate( void ) const;
    /*! Set the template for the base path for all data to be stored.
        Also adjusts the width of the widget accordingly.
        \param[in] path the template for the base path.
	\note The template is only set if \a path is not an empty string.
        \a pathTemplate() */
  void setPathTemplate( const string &path );

    /*! \return the default base path where data are stored
        if no session is running.
        \sa addDefaultPath(), setDefaultPath(), path() */
  string defaultPath( void ) const;
    /*! Set the default base path for all data to be stored and
        the corresponding environment variable RELACSDEFAULTPATH.
	If the current path() equals defaultPath() then
	the current base path is alse set via setPath().
        \param[in] defaultpath the base path for storing data.
	\note The pathes are only set if \a defaultpath is not an empty string.
        \a defaultPath() */
  void setDefaultPath( const string &defaultpath );
    /*! Expand file name by the default base path for storing data.
        \param[in] file the name of the file
	\return \a file added to the default base path for storing data.
        \sa defaultPath() */
  string addDefaultPath( const string &file ) const;

    /*! React to settings of the stimulus options.
        This function calls notifyStimulusData() in all RELACSPlugins. */
  virtual void notify( void );
    /*! Lock the stimulus data. */
  void lock( void ) const;
    /*! Unlock the stimulus data mutex. */
  void unlock( void ) const;
    /*! The mutex of the stimulus data. */
  QMutex *mutex( void );

    /*! Switch writing data to file on or off.
        Call this only in or before the initialization of a RePro
        or in RePro::read() *before* any write(). */
  void write( bool on );

    /*! Write input data to files */
  void write( const InList &data, const EventList &events );
    /*! Write output-meta-data to files. */
  void write( const OutData &signal );
    /*! Write output-meta-data to files. */
  void write( const OutList &signal );
    /*! Write RePro meta data to files. */
  void write( const RePro &RP );

    /*! If no file is open: create a new file name, make a directory,
        open and initialize the data-, event-, and stimulus files. */
  void openFiles( const Acquire &intracs, const EventList &events );
    /*! Close files and delete them and/or remove base directory. */
  void deleteFiles( void );
    /*! Close files and keep them. */
  void completeFiles( void );


public slots:

    /*! Doing some late initialization, i.e. setting fonts and palette. */
  void polish( void );


protected:

    /*! Close all open files */
  void closeFiles( void );
    /*! Open the file path() + filename
        as specified by \a type (ios::out, ios::in, ios::binary, ...).
        Add it to the list of files to be removed 
        and print an error message if opening of the file failed. */
  ofstream *openFile( const string &filename, int type );

  void createTraceFile( const Acquire &intraces );
  void createEventFiles( const EventList &events );
  void createStimulusFile( const Acquire &intraces, const EventList &events );

    /*! are there any files open to write in? */
  bool FilesOpen;
    /*! should be written into the files? */
  bool Writing;

    /*! The path (directory or common basename)
        where all data of the current session are stored. */
  string Path;
    /*! The template from which \a Path is generated. */
  string PathTemplate;
    /*! The default path (directory or common basename)
        where all data are stored. */
  string DefaultPath;

    /*! Identification number for pathes used to create a base pathes
        from \a PathFormat. */
  int  PathNumber;
    /*! The time used to generate the previous base path. */
  time_t PathTime;

    /*! files for all voltage traces. */
  vector< ofstream* > VF;
    /*! files for complete list of events except stimulus events. */
  vector< ofstream* > EF;
    /*! file with stimuli and indices to traces and events. */
  ofstream *SF;

    /*! The trace data that have to be written into the file. */
  const InList *TraceToWrite;
    /*! Time of start of the session. */
  double SessionTime;
    /*! Current index to trace data. */
  long TraceIndex;
    /*! Number of so far written trace data. */
  long TraceOffs;
    /*! Start of stimulus as an index to the written trace data. */
  long SignalOffs;

    /*! The names of the files for the events. */
  vector<string> EventFileNames;
    /*! The event data that have to be written into the file. */
  vector<const EventData*> EventsToWrite;
    /*! Indices to each event data. */
  vector<long> EventOffs;
    /*! Already written lines of events. */
  vector<long> EventLines;
    /*! Line index to the signal start in the events files. */
  vector<long> SignalEvents;

  OutList StimulusToWrite;
  bool StimulusData;
  TableKey StimulusKey;
  void writeStimulus( void );

  string ReProName;
  string ReProAuthor;
  string ReProVersion;
  string ReProDate;
  Options ReProSettings;
  bool ReProData;
  void writeRePro( void );

  bool ToggleOn;
  bool ToggleData;
  void writeToggle( void );

    /*! The file \a filename will be removed if the session is not
        saved. */
  void addRemoveFile( const string &filename );
    /*! Reset the list of files that have to be deleted if the session
        is not to be saved. */
  void clearRemoveFiles( void );
    /*! Remove all files from the list and clear the list of files
        that have to be deleted if the session is not to be saved. */
  void removeFiles( void );
    /*! A list of files which have to be deleted if the session is not
        to be saved. */
  vector<string> RemoveFiles; 

  RELACSWidget *RW;

  QLabel *FileLabel;
  QFont NormalFont;
  QFont HighlightFont;
  QPalette NormalPalette;
  QPalette HighlightPalette;
  SpikeTrace *SaveLabel;

  mutable QMutex StimulusDataLock;

};


}; /* namespace relacs */

#endif /* ! _RELACS_SAVEFILES_H_ */

