/*
  savefiles.h
  Write data to files

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

#ifndef _SAVEFILES_H_
#define _SAVEFILES_H_

#include <vector>
#include <fstream>
#include <ctime>
#include <qhbox.h>
#include <qlabel.h>
#include <qmutex.h>
#include "str.h"
#include "options.h"
#include "tablekey.h"
#include "inlist.h"
#include "outdata.h"
#include "eventdata.h"
#include "repro.h"
#include "spiketrace.h"

class RELACSWidget;

// constants for open() and toggle():
  /*! create a new path (used only by open). */
static const int FPath    = 0x0001;
  /*! write the voltage trace as signed shorts into *.sw1. */
static const int FTrace   = 0x0002;
  /*! write a trigger list of stimuli to the trace and the event list into *.trg. */
static const int FTrigger = 0x0004;
  /*! write a list of events into *.spk (indexed by *.trg). */
static const int FEvents  = 0x0008;
  /*! write all file types. */
static const int FAll     = 0x00ff;

  /*! Flag for the modes of traces or events, indicating that they should be saved. */
static const int SaveFilesMode = 0x0008;

/*! 
\class SaveFiles
\author Jan Benda
\version 2.1
\brief Write data to files

\bug implement saving of trace data
\bug what about no longer valid pointers in StimulusToWrite and TraceToWrite?
\bug createTriggerFile: write sample rate of all traces
\bug createTriggerFile: write name of trace file
\bug writeTrace: needs to be implemented
\bug writeTrigger: multi board signal times?
\bug createTraceFile: needs to be implemented.
\bug writeTrace: works only if all traces are written.
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

    /*! Returns true if at least one file type is to be opened. */
  bool openState( void ) const;
    /*! Returns true if data are written into files. */
  bool writing( void ) const;
    /*! Returns true if files are open. */
  bool saving( void ) const;

    /*! The path where all data of the current session are stored. */
  string path( void ) const;
    /*! Set the path where all data of the current session are stored
        to \a path. A slash is appended if missing. */
  void setPath( const string &path );
    /*! Returns \a file added to the current path where data should be stored. */
  string addPath( const string &file ) const;
    /*! The default path where data are stored if no session is running. */
  string defaultPath( void ) const;
    /*! Returns \a file added to the default path. */
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

    /*! The base name used for the files to be written. */
  string baseName( void ) const { return FileName; }

    /*! Determines in which file should be written.
        See constants FTrace, FTrigger... for possible values for \a what. 
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
        open and initialize the data-, index- and event-files. */
  void openFiles( const InList &data, const EventList &events );
    /*! Close files, delete them and removes the directory. */
  void deleteFiles( void );
    /*! Close files and add a header. */
  void completeFiles( void );


public slots:

    /*! Doing some late initialization, i.e. setting fonts and palette. */
  void polish( void );


signals:

  void newFiles( void );


protected:

    /*! should extra directory be created? */
  bool OpenPath;
    /*! should trigger to trace files be created? */
  bool OpenTrigger;
    /*! should trace data files be created? */
  bool OpenTrace;
    /*! should list of event data file be created? */
  bool OpenEvents;

    /*! are there any files open to write in? */
  bool FilesOpen;
    /*! should be written into the files? */
  bool Writing;

    /*! The path where all data of the current session are stored. */
  string Path;

    /*! file with trigger to trace. */
  ofstream *TF;
    /*! file for voltage trace. */
  ofstream *VF;
    /*! files for complete list of events. */
  vector< ofstream* > EF;

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
  TableKey TriggerKey;
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

    /*! identification number for files used to create a filename. */
  int  FileNumber;
    /*! The time used to generate the file name. */
  time_t FileTime;
    /*! basename for files and directory. */
  Str FileName;

    /*! The file \a filename will be removed if the session is not saved. */
  void addRemoveFile( const string &filename );
    /*! Reset the list of files that have to be deleted if the session is not to be saved. */
  void clearRemoveFiles( void );
    /*! A list of files which have to be deleted if the session is not to be saved. */
  vector<string> RemoveFiles; 

  RELACSWidget *RW;

    /*! close all open files */
  void closeFiles( void );
    /*! Returns true if the file path() + filename exist. */
  bool tryFile( const string &filename );
    /*! Open the file path() + filename
        as specified by \a type (ios::out, ios::in, ios::binary, ...). */
  ofstream *openFile( const string &filename, int type );

  void createTriggerFile( const InList &data, const EventList &events );
  void createTraceFile( const InList &data );
  void createEventFiles( const EventList &events );

  QLabel *FileLabel;
  QFont NormalFont;
  QFont HighlightFont;
  QPalette NormalPalette;
  QPalette HighlightPalette;
  SpikeTrace *SaveLabel;

  mutable QMutex StimulusDataLock;

};

#endif
