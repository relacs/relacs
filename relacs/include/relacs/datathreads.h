/*
  datathreads.h
  The threads for reading and writing to the data acquisition boards. 

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2009 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _RELACS_DATATHREADS_H_
#define _RELACS_DATATHREADS_H_ 1

#include <qthread.h>
#include <qmutex.h>

using namespace std;

namespace relacs {


class RELACSWidget;


/*! 
\class ReadThread
\brief Thread for reading data from the daq boards
\author Jan Benda
\version 1.0
*/

class ReadThread : public QThread
{

public:

  ReadThread( RELACSWidget *rw );
  void start( void );
  void stop( void );


protected:

  virtual void run( void );


private:

  bool Run;
  QMutex RunMutex;
  RELACSWidget *RW;

};


/*! 
\class WriteThread
\brief Thread for writing data to the daq boards
\author Jan Benda
\version 1.0
*/

class WriteThread : public QThread
{

public:

  WriteThread( RELACSWidget *rw );
  void start( double writeinterval );
  void stop( void );


protected:

  virtual void run( void );


private:

  bool Run;
  QMutex RunMutex;
  RELACSWidget *RW;
  double WriteInterval;

};


}; /* namespace relacs */

#endif /* ! _RELACS_DATATHREADS_H_ */

