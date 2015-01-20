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
    PrevMute( 0.0 ),
    MuteCount( 0 ),
    AudioRate( 44100.0 ),
    AudioSize( 0 ),
    DataStartTime( 0.0 ),
    DataRefTime( 0.0 ),
    DataCurrentTime( 0.0 ),
    DataPackageTime( 0.0 ),
    DataMean( 0.0f ),
    LastOut( 0.0f )
{
  setDate( "" );
  setDialogHelp( false );

  string audiodevs = "-1 default";
#ifdef HAVE_LIBPORTAUDIO
  // get available audio devices:
  PaError err = Pa_Initialize();
  if ( err == paNoError ) {
    int numdev = Pa_GetDeviceCount();
    int defaultdev = Pa_GetDefaultOutputDevice();
    for ( int k=0; k<numdev; k++ ) {
      const PaDeviceInfo *devinfo = Pa_GetDeviceInfo( k );
      if ( devinfo->maxOutputChannels == 0 )
	continue;
      audiodevs += "|" + Str( k ) + " " + devinfo->name + " (" + Str( devinfo->maxOutputChannels ) + " channels)";
      if ( k == defaultdev )
	audiodevs += " - default";
    }
    Pa_Terminate();
  }
#endif

  // options:
  addSelection( "device", "Audio device", audiodevs );
  addBoolean( "enable", "Enable audio monitor", true );
  addBoolean( "mute", "Mute audio monitor", false );
  addNumber( "gain", "Gain factor", 1.0, 0.0, 10000.0, 0.1 );
  addSelection( "audiorate", "Audio sampling rate", "44.1|8|16|22.05|44.1|48|96" ).setUnit( "kHz" );
}


AudioMonitor::~AudioMonitor( void )
{
  terminate();
}


void AudioMonitor::notify( void )
{
  Mutex.lock();
  Str audiodevs = text( "device" );
  int audiodevice = (int)::round( audiodevs.number( -1.0 ) );
  Gain = number( "gain" );
  PrevMute = Mute;
  Mute = boolean( "mute" ) ? 0.0f : 1.0f;
  if ( Mute > 0.1 )
    MuteCount = 0;
  else
    MuteCount++;
  bool enable = boolean( "enable" );
  bool initialized = Initialized;
  Str ars = text( "audiorate" );
  double audiorate = 1000.0* ars.number( 44.1 );
  Mutex.unlock();
  if ( enable ) {
    if ( initialized &&
	 ( AudioDevice != audiodevice || AudioRate != audiorate ) )
      terminate();
    AudioDevice = audiodevice;
    AudioRate = audiorate;
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

  int nbuffer = 256;
  Trace = 0;
  AudioSize = 0;
  DataStartTime = 0.0;
  DataRefTime = 0.0;
  DataCurrentTime = 0.0;
  DataPackageTime = 0.0;
  DataMean = 0.0;
  LastOut = 0.0f;
  if ( Mute < 0.1 )
    PrevMute = 0.0;

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
  const PaDeviceInfo *di = Pa_GetDeviceInfo( audiodev );
  if ( di == NULL ) {
    Stream = 0;
    return;
  }
  params.suggestedLatency = di->defaultHighOutputLatency;
  params.hostApiSpecificStreamInfo = NULL;
  PaError err = Pa_IsFormatSupported( NULL, &params, AudioRate );
  if ( err == paInvalidSampleRate ) {
    const int nrates = 6;
    const double defaultrates[nrates] = { 44100.0, 48000.0, 22050.0, 16000.0, 8000.0, 96000.0 }; 
    for ( int k=0; k<nrates; k++ ) {
      AudioRate = defaultrates[k];
      err = Pa_IsFormatSupported( NULL, &params, AudioRate );
      if ( err != paInvalidSampleRate )
	break;
    }
    if( err != paNoError ) {
      cerr << "Failed to find appropriate sampling rate and format for audio output: "
	   << Pa_GetErrorText( err ) << '\n';
      Stream = 0;
      return;
    }
  }
  while ( nbuffer < 0.02*AudioRate )
    nbuffer *= 2;
  err = Pa_OpenStream( &Stream, NULL, &params, AudioRate,
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
  else
    cerr << "Started audio stream at " << AudioRate << " Hz\n";
#endif

  DataStartTime = Data[Trace].currentTime() - Data[Trace].interval( nbuffer );
  DataRefTime = DataStartTime;
  DataCurrentTime = Data[Trace].currentTime();
  if ( Data[Trace].size() > 0 )
    DataMean = Data[Trace].back();
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


void AudioMonitor::mute( void )
{
  Mutex.lock();
  if ( Mute > 0.0 )
    MuteCount = 0;
  PrevMute = Mute;
  Mute = 0.0f;
  MuteCount++;
  bool cn = unsetNotify();
  setBoolean( "mute", true );
  setNotify( cn );
  Mutex.unlock();
}


void AudioMonitor::unmute( void )
{
  Mutex.lock();
  MuteCount--;
  if ( MuteCount <= 0 ) {
    MuteCount = 0;
    PrevMute = Mute;
    Mute = 1.0f;
    bool cn = unsetNotify();
    setBoolean( "mute", false );
    setNotify( cn );
  }
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
  const InData &trace = data->Data[data->Trace];
  float fac = data->Gain / trace.maxValue();
  double rate = 1.0/trace.stepsize();
  int datasize = trace.size();
  int dataminsize = trace.minIndex();
  int index = datasize;
  float audiofilter = 1.0/data->AudioRate;
  bool tuneaudiorate = ( trace.currentTime()-data->DataStartTime > 3.0 );
  float mute = data->PrevMute;
  float muteincr = (data->Mute - data->PrevMute)/(float)framesperbuffer;

  // write out data:
  unsigned long i=0;
  for( ; i<framesperbuffer; i++, data->AudioSize++ ) {
    double time = data->AudioSize/data->AudioRate + data->DataRefTime;
    index = trace.index( time );
    if ( index + 1 >= datasize )
      break;
    if ( index >= dataminsize ) {
      // linear interpolation:
      double m = (trace[index+1]-trace[index])*rate;
      data->LastOut = (m*(time-trace.pos(index))+trace[index])*fac;
    }
    // subtract mean:
    mute += muteincr;
    data->DataMean += ( data->LastOut - data->DataMean )*audiofilter;
    *out++ = mute * (data->LastOut - data->DataMean);
  }

  // data mising (e.g. because of restart of analog input!):
  if ( i < framesperbuffer ) {
    // cerr << "Audio skipped - missed " << framesperbuffer - i << " data points\n";
    data->AudioSize -= framesperbuffer - i;
    // data->AudioSize -= framesperbuffer;
    // data->AudioSize -= data->DataPackageTime*rate;
    tuneaudiorate = false;
  }
  // fill up the audio buffer:
  for( ; i<framesperbuffer; i++, data->AudioSize++ ) {
    mute += muteincr;
    data->DataMean += ( data->LastOut - data->DataMean )*audiofilter;
    *out++ = mute * (data->LastOut - data->DataMean);
  }

  // measure size increments of input data:
  if ( trace.currentTime() > data->DataCurrentTime ) {
    double packagetime = 1.5*(trace.currentTime() - data->DataCurrentTime);
    if ( data->DataPackageTime == 0.0 )
      data->DataPackageTime = packagetime;
    else
      data->DataPackageTime += ( packagetime - data->DataPackageTime )*0.01;
  }
  else
    tuneaudiorate = false;

  // adjust rate in order to align the ends of the data buffers:
  if ( tuneaudiorate ) {
    double newrate = data->AudioSize/(trace.currentTime()-data->DataStartTime-data->DataPackageTime);
    data->AudioRate += ( newrate - data->AudioRate )*0.01;
    data->DataRefTime = trace.pos( index+1 ) - data->AudioSize/data->AudioRate;
    //    cerr << data->AudioRate << "  " << data->DataPackageTime << '\n';
  }

  data->DataCurrentTime = trace.currentTime();
  data->PrevMute = data->Mute;

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

