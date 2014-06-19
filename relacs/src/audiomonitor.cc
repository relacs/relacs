#include <iostream>
#include <cmath>
#include <unistd.h>
#include <sys/stat.h>
#include <termios.h>
#include <relacs/audiomonitor.h>

using namespace std;
using namespace relacs;


AudioMonitor::AudioMonitor( void )
  : Running( false ),
    Gain( 1.0 ),
    Mute( 1.0 )
{
  // initialize:
#ifdef HAVE_LIBPORTAUDIO
  Stream = 0;
  PaError err = Pa_Initialize();
  if ( err != paNoError ) {
    cerr << "Failed to initialize PortAudio: " << Pa_GetErrorText( err ) << '\n';
    return;
  }
#endif
}


AudioMonitor::~AudioMonitor( void )
{
#ifdef HAVE_LIBPORTAUDIO
  Pa_Terminate();
#endif
}

  
void AudioMonitor::start( void )
{
  Running = false;
  Trace = 0;
  Index = Data[Trace].currentIndex() - 256;
  if ( Index < Data[Trace].minIndex() )
    Index = Data[Trace].minIndex();

#ifdef HAVE_LIBPORTAUDIO

  // open default stream for output:
  PaError err = Pa_OpenDefaultStream( &Stream, 0, 1, paFloat32, Data[Trace].sampleRate(),
				      256, audioCallback, this );
  if( err != paNoError ) {
    cerr << "Failed to open default audio output: " << Pa_GetErrorText( err ) << '\n';
    Stream = 0;
    return;
  }

  // start stream:
  err = Pa_StartStream( Stream );
  if ( err != paNoError ) {
    cerr << "Failed to start audio stream: " << Pa_GetErrorText( err ) << '\n';
    return;
  }
  Running = true;

#endif
}


void AudioMonitor::stop( void )
{
  // stop stream:
  if ( Running ) {
#ifdef HAVE_LIBPORTAUDIO
    PaError err = Pa_StopStream( Stream );
    if ( err != paNoError ) {
      cerr << "Failed to stop audio stream: " << Pa_GetErrorText( err ) << '\n';
    }
#endif
    Running = false;
  }

#ifdef HAVE_LIBPORTAUDIO
  // close stream:
  if ( Stream != 0 ) {
    PaError err = Pa_CloseStream( Stream );
    if( err != paNoError )
      cerr << "Failed to close audio stream: " << Pa_GetErrorText( err ) << '\n';
  }
#endif
}


void AudioMonitor::mute( void )
{
  Mutex.lock();
  Mute = 0.0f;
  Mutex.unlock();
}


void AudioMonitor::unmute( void )
{
  Mutex.lock();
  Mute = 1.0f;
  Mutex.unlock();
}


bool AudioMonitor::muted( void ) const
{
  Mutex.lock();
  float mute = Mute;
  Mutex.unlock();
  return ( mute < 0.1 );
}


#ifdef HAVE_LIBPORTAUDIO

int AudioMonitor::audioCallback( const void *input, void *output,
				 unsigned long framesperbuffer,
				 const PaStreamCallbackTimeInfo* timeinfo,
				 PaStreamCallbackFlags statusFlags, void *userdata )
{
  AudioMonitor *data = (AudioMonitor*)userdata;
  float *out = (float*)output;
  data->Mutex.lock();
  float fac = data->Mute * data->Gain / data->Data[data->Trace].maxValue();
  data->Mutex.unlock();
  if ( data->Data[data->Trace].size() - data->Index < (signed long)framesperbuffer ) {
    for( unsigned long i=0; i<framesperbuffer; i++ )
      *out++ = 0.0f;
  }
  else {
    for( unsigned long i=0; i<framesperbuffer; i++ )
      *out++ = data->Data[data->Trace][data->Index++]*fac;
  }
  return paContinue;
}

#endif


void AudioMonitor::assignTraces( const InList &il, deque<InList*> &data )
{
  Data.assign( &il );
  data.push_back( &Data );
}


void AudioMonitor::assignTraces( void )
{
  Data.assign();
}


void AudioMonitor::updateDerivedTraces( void )
{
  Data.updateDerived();
}

