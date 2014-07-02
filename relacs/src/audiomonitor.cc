#include <relacs/relacsplugin.h>
#include <relacs/audiomonitor.h>

using namespace std;
using namespace relacs;


AudioMonitor::AudioMonitor( void )
  : ConfigDialog( "AudioMonitor", RELACSPlugin::Core, "AudioMonitor" ),
    Initialized( false ),
    Running( false ),
    AudioDevice( 0 ),
    Gain( 1.0 ),
    Mute( 1.0 )
{
  setDate( "" );
  setDialogHelp( false );

  // options:
  addInteger( "device", "Audio device number", 0, 0, 100 );
  addBoolean( "enable", "Enable audio monitor", true );
  addBoolean( "mute", "Mute audio monitor", false );
  addNumber( "gain", "Gain factor", 1.0, 0.0, 10000.0, 0.1 );
}


AudioMonitor::~AudioMonitor( void )
{
  terminate();
}


void AudioMonitor::notify( void )
{
  Mutex.lock();
  int audiodevice = integer( "device" );
  Gain = number( "gain" );
  Mute = boolean( "mute" ) ? 0.0f : 1.0f;
  bool enable = boolean( "enable" );
  Mutex.unlock();
  if ( enable ) {
    if ( Initialized && audiodevice != AudioDevice )
      terminate();
    AudioDevice = audiodevice;
    initialize();
  }
  else
    terminate();
  AudioDevice = audiodevice;
}

  
void AudioMonitor::initialize( void )
{
  if ( Initialized )
    return;

#ifdef HAVE_LIBPORTAUDIO
  Stream = 0;
  PaError err = Pa_Initialize();
  if ( err != paNoError ) {
    cerr << "Failed to initialize PortAudio: " << Pa_GetErrorText( err ) << '\n';
    return;
  }
  int numdev = Pa_GetDeviceCount();
  if ( AudioDevice >= numdev )
    AudioDevice = numdev-1;
  cerr << "Available audio devices for the audio monitor:\n";
  for ( int k=0; k<numdev; k++ ) {
    const PaDeviceInfo *devinfo = Pa_GetDeviceInfo( k );
    string ds = "  ";
    if ( k == AudioDevice )
      ds = "* ";
    cout << "  " << ds << k << " " << devinfo->name
	 << " with " << devinfo->maxOutputChannels << " output channels\n";
  }
  cerr << "Defaul audio device is " << Pa_GetDefaultOutputDevice() << '\n';
#endif

  Initialized = true;

  if ( Running )
    start();
}

  
void AudioMonitor::terminate( void )
{
  if ( Running ) {
    stop();
    Running = true;
  }

#ifdef HAVE_LIBPORTAUDIO
  if ( Initialized )
    Pa_Terminate();
#endif

  Initialized = false;
}

  
void AudioMonitor::start( void )
{
  Running = true;

  if ( ! Initialized )
    return;

  Trace = 0;
  Index = Data[Trace].currentIndex() - 256;
  if ( Index < Data[Trace].minIndex() )
    Index = Data[Trace].minIndex();

#ifdef HAVE_LIBPORTAUDIO

  // open default stream for output:
  PaStreamParameters params;
  params.channelCount = 1;
  params.device = AudioDevice;
  params.hostApiSpecificStreamInfo = NULL;
  params.sampleFormat = paFloat32;
  params.suggestedLatency = Pa_GetDeviceInfo( AudioDevice )->defaultLowOutputLatency ;
  params.hostApiSpecificStreamInfo = NULL;
  PaError err = Pa_OpenStream( &Stream, NULL, &params, Data[Trace].sampleRate(),
			       256, paNoFlag, audioCallback, this );
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
#endif
}


void AudioMonitor::stop( void )
{
  if ( ! Initialized ) {
    Running = false;
    return;
  }

  // stop stream:
  Mutex.lock();
  bool run = Running;
  Mutex.unlock();
  if ( run ) {
    Mutex.lock();
    Running = false;
    Mutex.unlock();
#ifdef HAVE_LIBPORTAUDIO
    PaError err = Pa_StopStream( Stream );
    if ( err != paNoError ) {
      cerr << "Failed to stop audio stream: " << Pa_GetErrorText( err ) << '\n';
    }
#endif
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

