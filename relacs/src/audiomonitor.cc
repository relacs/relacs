#include <relacs/relacsplugin.h>
#include <relacs/audiomonitor.h>

using namespace std;
using namespace relacs;


AudioMonitor::AudioMonitor( void )
  : ConfigDialog( "AudioMonitor", RELACSPlugin::Core, "AudioMonitor" ),
    Initialized( false ),
    Running( false ),
    AudioDevice( -1 ),
    Gain( 1.0 ),
    Mute( 1.0 ),
    MaxSkipTime( 0.2 ),
    MaxSkip( 0 ),
    NSkip( 0 ),
    FullCount( 0 ),
    LastOut( 0.0f )
{
  setDate( "" );
  setDialogHelp( false );

  // options:
  addInteger( "device", "Audio device number", -1, -1, 100 );
  addBoolean( "enable", "Enable audio monitor", true );
  addBoolean( "mute", "Mute audio monitor", false );
  addNumber( "gain", "Gain factor", 1.0, 0.0, 10000.0, 0.1 );
  addNumber( "maxskip", "Maximum playback delay", MaxSkip, 0.0, 100.0, 0.02, "s", "ms" );
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
  double maxskiptime = number( "maxskip" );
  bool initialized = Initialized;
  Mutex.unlock();
  if ( enable ) {
    if ( initialized && 
	 ( AudioDevice != audiodevice || maxskiptime != MaxSkipTime ) )
      terminate();
    AudioDevice = audiodevice;
    MaxSkipTime = maxskiptime;
    initialize();
  }
  else
    terminate();
  AudioDevice = audiodevice;
}

  
void AudioMonitor::initialize( void )
{
  Mutex.lock();
  bool initialized = Initialized;
  Mutex.unlock();
  if ( initialized )
    return;

#ifdef HAVE_LIBPORTAUDIO
  Stream = 0;
  PaError err = Pa_Initialize();
  if ( err != paNoError ) {
    cerr << "Failed to initialize PortAudio: " << Pa_GetErrorText( err ) << '\n';
    return;
  }
  int numdev = Pa_GetDeviceCount();
  int audiodev = AudioDevice;
  if ( audiodev >= numdev )
    audiodev = numdev-1;
  if ( audiodev < 0 )
    audiodev = Pa_GetDefaultOutputDevice();
  cerr << "Available audio devices for the audio monitor:\n";
  for ( int k=0; k<numdev; k++ ) {
    const PaDeviceInfo *devinfo = Pa_GetDeviceInfo( k );
    if ( devinfo->maxOutputChannels == 0 )
      continue;
    string ds = "  ";
    if ( k == audiodev )
      ds = "* ";
    cout << "  " << ds << k << " " << devinfo->name
	 << " with " << devinfo->maxOutputChannels << " output channels\n";
  }
  cerr << "Defaul audio device is " << Pa_GetDefaultOutputDevice() << '\n';
#endif

  Mutex.lock();
  Initialized = true;
  bool run = Running;
  Mutex.unlock();
  if ( run )
    start();
}

  
void AudioMonitor::terminate( void )
{
  Mutex.lock();
  bool run = Running;
  Mutex.unlock();
  if ( run ) {
    stop();
    Mutex.lock();
    Running = true;
    Mutex.unlock();
  }

#ifdef HAVE_LIBPORTAUDIO
  Mutex.lock();
  bool initialized = Initialized;
  Mutex.unlock();
  if ( initialized )
    Pa_Terminate();
#endif

  Mutex.lock();
  Initialized = false;
  Mutex.unlock();
}

  
void AudioMonitor::start( void )
{
  Mutex.lock();
  Running = true;
  bool initialized = Initialized;
  Mutex.unlock();

  if ( ! initialized )
    return;

  const int nbuffer = 256;

  Trace = 0;
  Index = Data[Trace].currentIndex() - nbuffer;
  if ( Index < Data[Trace].minIndex() )
    Index = Data[Trace].minIndex();
  MaxSkip = Data[Trace].indices( MaxSkipTime );
  NSkip = 0;
  FullCount = 0;
  if ( Index < Data[Trace].size() )
    LastOut = Data[Trace][Index];
  else
    LastOut = 0.0f;

#ifdef HAVE_LIBPORTAUDIO

  // open default stream for output:
  int audiodev = AudioDevice;
  if ( audiodev >= Pa_GetDeviceCount() )
    audiodev = Pa_GetDeviceCount()-1;
  if ( audiodev < 0 )
    audiodev = Pa_GetDefaultOutputDevice();
  PaStreamParameters params;
  params.channelCount = 1;
  params.device = audiodev;
  params.hostApiSpecificStreamInfo = NULL;
  params.sampleFormat = paFloat32;
  params.suggestedLatency = Pa_GetDeviceInfo( audiodev )->defaultHighOutputLatency;
  params.hostApiSpecificStreamInfo = NULL;
  PaError err = Pa_OpenStream( &Stream, NULL, &params, Data[Trace].sampleRate(),
			       nbuffer, paNoFlag, audioCallback, this );
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
  Mutex.lock();
  bool initialized = Initialized;
  Mutex.unlock();
  if ( ! initialized ) {
    Mutex.lock();
    Running = false;
    Mutex.unlock();
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


bool AudioMonitor::mute( void )
{
  Mutex.lock();
  float mute = Mute;
  Mute = 0.0f;
  Mutex.unlock();
  return ( mute < 0.1 );
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
  if ( data->Index < data->Data[data->Trace].minIndex() )
    data->Index = data->Data[data->Trace].minIndex();
  if ( data->Data[data->Trace].size() - data->Index < (signed long)framesperbuffer ) {
    // write all that is available:
    unsigned long i=0;
    for( i=0; i<framesperbuffer && data->Index < data->Data[data->Trace].size(); i++ )
      *out++ = data->Data[data->Trace][data->Index++]*fac;
    if ( data->Index-1 < data->Data[data->Trace].size() )
      data->LastOut = data->Data[data->Trace][data->Index-1]*fac;
    // fill up with zeros:
    int nzeros = 0;
    for( ; i<framesperbuffer; i++ ) {
      *out++ = data->LastOut;
      nzeros ++;
    }
    if ( data->NSkip < data->MaxSkip ) {
      int nskip = nzeros;
      if ( data->NSkip + nskip > data->MaxSkip )
	nskip = data->MaxSkip - data->NSkip;
      data->NSkip += nskip;
      data->Index -= nskip; // skip!
    }
    else
      data->Index += nzeros;  // do not skip
    data->FullCount = 0;
  }
  else {
    // write out full buffer:
    for( unsigned long i=0; i<framesperbuffer; i++ )
      *out++ = data->Data[data->Trace][data->Index++]*fac;
    data->LastOut = data->Data[data->Trace][data->Index-1]*fac;
    // advance buffer:
    data->FullCount++;
    if ( data->FullCount % 10 == 0 && data->NSkip > 0 ) {
      data->Index++;
      data->NSkip--;
    }
  }
  data->Mutex.unlock();
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

