/*
  AudioMonitor.h
  Plays recordings on speakers using portaudio library.

  FishGrid
  Copyright (C) 2009 Jan Benda <benda@bio.lmu.de> & Joerg Henninger <henninger@bio.lmu.de>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  FishGrid is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _AUDIOMONITOR_H_
#define _AUDIOMONITOR_H_ 1

#ifdef HAVE_LIBPORTAUDIO
#include <portaudio.h>
#endif
#include <QMutex>
#include <relacs/inlist.h>
#include <relacs/configdialog.h>

using namespace std;

namespace relacs {


class AudioMonitor : public ConfigDialog
{

public:

  AudioMonitor( void );
  ~AudioMonitor( void );

  virtual void notify( void );

  void start( void );
  void stop( void );

  void mute( void );
  void unmute( void );
  bool muted( void ) const;

  void assignTraces( const InList &il, deque<InList*> &data );
  void assignTraces( void );
  void updateDerivedTraces( void );

protected:
	
#ifdef HAVE_LIBPORTAUDIO
    /*! This function will be called by the PortAudio engine when audio is needed.
        It may called at interrupt level on some machines so don't do anything
	that could mess up the system like calling malloc() or free(). */
  static int audioCallback( const void *input, void *output,
			    unsigned long framesperbuffer,
			    const PaStreamCallbackTimeInfo* timeinfo,
			    PaStreamCallbackFlags statusFlags, void *userdata );

  PaStream *Stream;
#endif

  bool Running;

  InList Data;
  int Trace;
  int Index;
  float Gain;
  float Mute;
  mutable QMutex Mutex;
	
};


}; /* namespace relacs */

#endif /* ! _AUDIOMONITOR_H_ */

