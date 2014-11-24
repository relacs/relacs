/*
  outdata.h
  An output signal for a data acquisition board.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2012 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_OUTDATA_H_
#define _RELACS_OUTDATA_H_ 1
 
#include <fstream>
#include <string>
#include <vector>
#include <relacs/sampledata.h>
#include <relacs/daqerror.h>
#include <relacs/options.h>

using namespace std;

namespace relacs {


  class Acquire;


/*! 
\class OutData
\brief An output signal for a data acquisition board.
\author Marco Hackenberg, Jan Benda

\bug noiseWave(), bandNoiseWave(): should set negative carrier frequency -> but: calibrate repro still does not calibrate white noise stimuli!

Before doing anything with an OutData you should set the output trace by
setTrace() or setTraceName() first.

The way the data values of the signal are interpreted by the hardware driver
interface class depends on whether an attenuator is connected to the
output line and on the type of external reference. For normal operation
without a connected attenuator, the data values are given in units \a unit.
The hardware driver interface class converts the data values
by multiplying with \a scale() to the voltage that is put out by the daq board. 
Both intensity() and level() are ignored.
Which gain is used for converting the voltage to integer values that are transferred to the DAQ board
can be controlled by request().
A constant-voltage external reference is used as an additionally 
available gain factor.
If a non-constant external reference is used by explicitely requesting
it with setExtRef() or request( min, ExtRef ),
then the values from 0 to 1 (unipolar mode) or -1 to 1 (bipolar mode)
are mapped to the full output range of the daq board.
This range can be modified by scale().

In case of an attenuator connected to the output line,
the  values from 0 to 1 (unipolar mode) or -1 to 1 (bipolar mode)
are mapped to the full output range of the daq board.
scale() might be used internally by AnalogOutput for proper scaling.
The resulting voltage is then attenuated by additional hardware
according to the requested intensity() or level().
*/

class OutData : public SampleData< float >, public DaqError
{
  
 public:

    /*! Value for intensity requesting to mute the attenuator. */
  static const double MuteIntensity;
    /*! Default value for Intensity indicating that no attenuator is used. */
  static const double NoIntensity;

    /*! Default value fur attenuator level indicating that no attenuator is used. */
  static const double NoLevel;

    /*! Indicates that the minimum or maximum value of the signal trace
        should be used for setting the appropriate gain on the daq board.
        \sa requestedMin(), requestedMax(), request() */
  static const double AutoRange;
    /*! Indicates that the external reference should be used.
        \sa requestedMin(), requestedMax(), request() */
  static const double ExtRef;
  
    /*! Create an empty signal. */
  OutData( void );
    /*! Create an OutData-object with \a n data elements sampled with
        \a stepsize seconds. */
  OutData( int n, double stepsize );
    /*! Create an OutData-object for data elements sampled with
        \a stepsize for \a duration seconds. */
  OutData( double duration, double stepsize );
   /*! Create an OutData-object with \a n data elements sampled with
       \a stepsize and initialzes them with the values given in \a a. */
  template < typename R >
  OutData( const R *a, int n, double stepsize=1.0 );
    /*! Create an OutData-object with the same size and content
        as the vector \a a, sampled with \a stepsize. */
  template < typename R >
  OutData( const vector< R > &a, double stepsize=1.0 );
    /*! Create an OutData-object with the same size and content
        as the array \a a, sampled with \a stepsize. */
  template < typename R >
  OutData( const Array< R > &a, double stepsize=1.0 );
    /*! Create an OutData-object with the same size and content
        as the array \a sa.
        sa.offset() is set to zero. */
  template < typename R >
  OutData( const SampleData< R > &sa );
    /*! Copy constructor.
        Create an OutData-object with the same size, content, and properties
        as \a od. */
  OutData( const OutData &od );
    /*! The destructor. */
  virtual ~OutData( void );

    /*! Set the size(), capacity() and content of the array to \a a
        without affecting the stepsize() and other properties.
        \sa assign() */
  template < typename R >
  const OutData &operator=( const R &a );
    /*! Set the content and stepsize of the OutData to \a sa.
        \sa assign() */
  template < typename R >
  const OutData &operator=( const SampleData< R > &a );
    /*! Set the content and properties of the OutData to \a od.
        \sa assign() */
  const OutData &operator=( const OutData &od );

  /* Generates declarations for unary operators of class OutData
     that take scalars as argument.
     \a COP is the operator name (like operator+= ). */
#define OUTDATAOPS1SCALARDEC( COP )		\
  const OutData &COP( float x );				\
  const OutData &COP( double x );				\
  const OutData &COP( long double x );			\
  const OutData &COP( signed char x );			\
  const OutData &COP( unsigned char x );			\
  const OutData &COP( signed int x );			\
  const OutData &COP( unsigned int x );			\
  const OutData &COP( signed long x );			\
  const OutData &COP( unsigned long x );                      \

    /*! Set the value of each data element to \a val. */
  OUTDATAOPS1SCALARDEC( operator= );

    /*! Set the size() and capacity() of the OutData array to \a n,
        its content to \a a, and the stepsize to \a stepsize.
        All other properties are not affected. */
  template < typename R >
  OutData &assign( const R *a, int n, const double stepsize=1 );
    /*! Set the size(), capacity(), and content of the OutData array to \a a, 
        and the stepsize to \a stepsize.
        All other properties are not affected. */
  template < typename R >
  OutData &assign( const R &a, const double stepsize=1 );
    /*! Set the size(), capacity(), stepsize(), and content 
        of the OutData array to \a sa.
        sa.offset() is set tot zero.
        All other properties are not affected. */
  template < typename R >
  OutData &assign( const SampleData< R > &sa );
    /*! Make the OutData object a copy of \a od. */
  OutData &assign( const OutData &od );

    /*! Copy the content of the OutData object to \a a. 
        If necessary remaining elements of \a a
        are set to \a val. */
  template < typename R >
  const OutData &copy( R *a, int n, const float &val=0 ) const;
    /*! Copy the content and stepsize (if present) 
        of the OutData object to \a a. */
  template < typename R >
  const OutData &copy( R &a ) const;
    /*! Make \a od a copy of the OutData object. */
  const OutData &copy( OutData &od ) const;

    /*! Append \a od to the OutData object.
        If the stepsize of \a od differs,
        then \a od is resampled with linear interpolation.
	\param[in] \a name the optional name can be used to functionally describe the resulting signal. */
  OutData &append( const OutData &od, const string &name="" );

    /*! Repeat the current content of the OutData array \a n times.
	\param[in] \a name the optional name can be used to functionally describe the resulting signal. */
  OutData &repeat( int n, const string &name="" );

    /*! Clear the data and the description(), but not any additional
        information like trace(), intenisty() ,etc. */
  void clear( void );

    /*! Return string with an error message: 
        '"ident", channel # on device #: error message'.
        If there isn't any error, an empty string is returned. */
  string errorMessage( void ) const;

    /*! The sampling rate of the signal in Hertz */
  double sampleRate( void ) const;
    /*! Set the sampling rate of the signal to \a rate Hertz. */
  void setSampleRate( double rate );
    /*! The sampling interval of the signal in seconds */
  double sampleInterval( void ) const;
    /*! Set the sampling interval of the signal to \a step seconds. */
  void setSampleInterval( double step );

    /*! The source for the start trigger of the data aquisition. */
  int startSource( void ) const;
    /*! Set the source for the start trigger of the data aquisition to \a startsource.
        See the manual of the data acquisition board for details.
        If set to zero, the output of the signal starts immediately. */
  void setStartSource( int startsource );
    /*! Delay in seconds from start trigger to start of aquisition. */
  double delay( void ) const;
    /*! Set delay to \a delay (in seconds). */
  void setDelay( double delay );
    /*! Returns true if the signal has the priority to run
        even if there is still another signal running. */
  bool priority( void ) const;
    /*! Set the priority of the signal to \a priority.
        If \a priority is \a true then the signal is processed even
	if there is still another signal running.
	Otherwise the signal is not processed and returns with an error. */
  void setPriority( const bool priority=true );
    /*! Is output performed in continuous mode? */
  bool continuous( void ) const;
    /*! Set continuous output to \a continuous. */
  void setContinuous( bool continuous=true );
    /*! Is output restarting the data acquisition? */
  bool restart( void ) const;
    /*! If \a restart is set to \c true, then the ouput of the signal
        will restart the data acquisition. */
  void setRestart( bool restart=true );

    /*! The id of the output device. 
        \sa setDevice(), channel(), trace(), traceName() */
  int device( void ) const;
    /*! Set the device id to \a device.
        You only need to specify an output device if more than one
        output devices are available and the data should be sent
        to an output device other than the default one.
	Alternatively, you can set an output trace by setTrace() or
	setTraceName() (recommended). Then the Acquire class fills in
	the device and channel information.
        \sa device(), setChannel(), setTrace(), setTraceName() */
  void setDevice( int device );
    /*! The number of the channel on the specified device
        that is used for output. 
        \sa setChannel(), device(), trace(), traceName() */
  int channel( void ) const;
    /*! Set the number of the channel on the specified device
        that should be used for output to \a channel. 
        Default is the first ('0') channel. 
        \sa channel(), setDevice(), setTrace(), setTraceName() */
  void setChannel( int channel );
    /*! Set the number of the channel to \a channel
        and the device to \a device. 
	Alternatively, you can set an output trace by setTrace() or
	setTraceName() (recommended). Then the Acquire class fills in
	the device and channel information.
        \sa channel(), setDevice(), setTrace(), setTraceName() */
  void setChannel( int channel, int device );

    /*! The index of the output trace.
        \sa setTrace(), channel(), device(), traceName() */
  int trace( void ) const;
    /*! Set the output trace by specifying its index \a index.
        If an Acquire class was specified by setAcquire(),
	then channel number, device, maximum sampling rate, etc.
	are set according to the trace \a index.
	Otherwise clears the channel(), device(), and the trace() index
	so that they are set later by one of the Acquire::write() functions.
	\return 
	-  0: success
	- -1: index < 0
	- -2: trace does not exist
	- -3: invalid match
        \sa trace(), setChannel(), setDevice(), setTraceName(),
	setMaxSampleRate(), setFixedSampleRate() */
  int setTrace( int index );
    /*! The name of the output trace.
        \sa setTrace(), channel(), device(), traceName() */
  string traceName( void ) const;
    /*! Set the output trace by specifying its name \a name.
        If an Acquire class was specified by setAcquire(),
	then channel number, device, maximum sampling rate, etc.
	are set according to the trace \a name.
	Otherwise clears the channel(), device(), and the trace() index
	so that they are set later by one of the Acquire::write() functions.
	\return 
	-  0: success
	- -1: index < 0
	- -2: trace does not exist
	- -3: invalid match
        \sa trace(), setChannel(), setDevice(), setTraceName(),
	setMaxSampleRate(), setFixedSampleRate() */
  int setTraceName( const string &name );
    /*! Set the index and the name of the output trace to \a index
        and \a name, respectively, without resetting or updating other information. */
  void setTrace( int index, const string &name );
    /*! Tell OutData the Acquire class \a a that manages all output traces.
        Used by setTrace() and setTraceName() to fill in further information
	about the output trace (scale(), unit(),
	maxSampleRate(), fixedSampleRate(). */
  static void setAcquire( const Acquire *a );

    /*! A string identifying the signal. */
  string ident( void ) const;
    /*! Set the identifier of the ouput signal to \a ident. */
  void setIdent( const string &ident );

    /*! Returns the description of the output signal. */
  const Options &description( void ) const;
    /*! Returns the description of the output signal. */
  Options &description( void );
    /*! Set the description of the output signal to \a description. */
  void setDescription( const Options &description );

    /*! Returns the minimum value of the
        signal trace that should be used for determining the appropriate gain
	setting on the daq board.
	If AutoRange is returned, then the minimum value of the current
	signal trace should be used.
	If ExtRef is returned, then the external reference should be used
	regardless of the signal's minimum and maximum values.
	\sa requestedMax(), request(), gainIndex() */
  double requestedMin( void ) const;
    /*! Returns the maximum value of the
        signal trace that should be used for determining the appropriate gain
	setting on the daq board.
	If AutoRange is returned, then the maximum value of the current
	signal trace should be used.
	If ExtRef is returned, then the external reference should be used
	regardless of the signal's minimum and maximum values.
	\sa requestedMin(), request(), gainIndex() */
  double requestedMax( void ) const;
    /*! Set the minimum and maximum value of the
        signal trace that should be used for determining the appropriate gain
	setting on the daq board to \a min and \a max respectively.
        Use AutoRange to indicate that the minimum or maximum value of the current
	signal trace should be used. 
	Use ExtRef for the maximum value to indicate 
	that the external reference should be used
	regardless of the signal's minimum and maximum values.
	In this case the value of \a min then still determines the polarity.
	\sa setExtRef(), requestedMin(), requestedMax(), gainIndex() */
  void request( double min, double max );
    /*! Force the hardware driver to use the external reference
        in bipolar output mode,
        regardless of the signal's minimum and maximum values. 
        Same as request( -1.0, ExtRef ). */
  void setExtRef( void );

    /*! Returns an integer encoding the output gain, polarity, and reference. 
        This function is internally used by the hardware driver class. */
  int gainIndex( void ) const;
    /*! Set the integer encoding the output gain, polarity, and reference
        to \a index. 
        This function is internally used by the hardware driver class. */
  void setGainIndex( int index );
    /*! Returns the data to be used by AnalogOutput for converting
        voltage to raw data.
        \sa setGainData(), gainIndex() */
  char *gainData( void ) const;
    /*! Set the data to be used by AnalogOutput for converting
        voltage to raw data to \a data.
        \sa gainData(), setGainIndex() */
  void setGainData( char *data );

    /*! Get the voltage of the \a index -th element in Volt.
	\a index must be a valid index. */
  double voltage( int index ) const;
    /*! Returns the voltage corresponding to the value \a val in Volt. */
  double getVoltage( double val ) const;
    /*! Minimum possible voltage value for the preset gain of the daq board.
        \sa maxVoltage(), minValue() */
  double minVoltage( void ) const;
    /*! Maximum possible voltage value for the preset gain of the daq board.
        \sa minVoltage(), maxValue() */
  double maxVoltage( void ) const;
    /*! Set the minimum possible voltage value to \a minv.
        \sa setMaxVoltage(), setMinValue(), minValue() */
  void setMinVoltage( double minv );
    /*! Set the maximum possible votlage value to \a maxv.
        \sa setMinVoltage(), setMaxValue(), maxValue() */
  void setMaxVoltage( double maxv );

    /*! The scale factor used for scaling the output signal to the voltage
        that is put out by the analog output device.
        \sa setScale(), unit(), setUnit() */
  double scale( void ) const;
    /*! Set the scale factor to \a scale.
	The scale factor \a scale is used to scale the output signal
	to the voltage that is put out by the analog output device.
        \sa multiplyScale(), scale(), unit(), setUnit() */
  void setScale( double scale );
    /*! Multiply the scale factor by \a fac.
	The scale factor \a scale is used to scale the output signal
	to the voltage that is put out by the analog output device.
        \sa scale(), setScale(), unit(), setUnit() */
  void multiplyScale( double fac );
    /*! The unit of the signal.
        \sa setUnit(), scale(), setScale() */
  string unit( void ) const;
    /*! Set the unit of the signal to \a unit.
        \sa unit(), scale(), setScale() */
  void setUnit( const string &unit );
    /*! Set the specifications for the output signal.
	The signal given in unit \a unit
	is scaled by \a scale  to the voltage 
	that is put out by the analog output device.
        \sa unit(), scale(), setScale() */
  void setUnit( double scale, const string &unit );
    /*! Minimum possible value of the signal, given the preset gain
        value of the daq board.
        \sa maxValue(), minVoltage() */
  double minValue( void ) const;
    /*! Maximum possible value of the signal, given the preset gain
        value of the daq board.
        \sa minValue(), maxVoltage() */
  double maxValue( void ) const;

    /*! Peak intensity of the signal.
        If MuteIntensity is returned, the attenuator is muted.
	If NoIntensity is returned, no attenuator is connected
	to the output channel.
        \sa setIntensity(), setNoIntensity(), noIntensity(), level(), muted() */
  double intensity( void ) const;
    /*! Set output peak intensity to \a intensity.
        The intensity is used in conjunction with the carrier frequency
	to set an attenuator or amplifier.
	Passing MuteIntensity requests to mute the attenuator
	connected to the output channel.
	Setting the intensity overrides a possible set level().
        If no such hardware is used,
        then the intensity as well the carrier frequency settings 
	have no effect.
        \sa mute(), setCarrierFreq(), intensity(), setNoIntensity(), noIntensity(), setLevel() */
  void setIntensity( double intensity );
    /*! Set the intensity to its default value, indicating that
        the intensity is not used for computing the output signal.
	This will cause an error, if the requested output channel
	is connected to an attenuator.
        \sa setIntensity(), setNoLevel() */
  void setNoIntensity( void );
    /*! Returns \c true if no intensity is set. \sa intensity(), noLevel() */
  bool noIntensity( void ) const;
    /*! Request to mute the attenuator connected to the output channel. 
        Same as setIntensity( MuteIntensity ). 
        \sa muted(), intensity(), setIntensity(), setNoIntensity() */
  void mute( void );
    /*! Returns \c true if attenuator should be muted. \sa mute(), setIntensity() */
  bool muted( void ) const;
    /*! Carrier frequency of the signal in Hertz. */
  double carrierFreq( void ) const;
    /*! Set carrier frequency of the signal to \a carrierfreq Hertz.
        The carrier frequency is used in conjunction with the intensity
        to set an attenuator or amplifier.
        If no such hardware is used,
        then the intensity as well the carrier frequency settings 
	have no effect.
        \sa setIntensity() */
  void setCarrierFreq( double carrierfreq );

    /*! The level that was set for the attenuator.
        \sa setLevel(), intensity() */
  double level( void ) const;
    /*! Set attenuator level directly to \a level.
        The specified level is only used if no intensity()
	was specified.
        If no atenuator is connected,
        then setting the level has no effect.
        \sa level(), setIntensity() */
  void setLevel( double level );
    /*! Set the attenuation level to its default value, indicating that
        the level is not used for computing the output signal.
	This will cause an error, if the requested output channel
	is connected to an attenuator and also no intensity() was specified.
        \sa setLevel(), setNoIntensity() */
  void setNoLevel( void );
    /*! Returns \c true if no level is set. \sa level(), noIntensity() */
  bool noLevel( void ) const;

    /*! The duration of the output signal. Equals length(). */
  double duration( void ) const;
    /*! Total duration of the output signal in seconds 
        ( delay() + duration() ). */
  double totalDuration( void ) const;

    /*! The maximum or fixed sampling rate to be used in Hertz.
        If no maximum sampling rate was specified
	(via setMaxSampleRate(), setMinSampleInterval())
	defaultMaxSampleRate() is returned.
        Same as 1/minSampleInterval().
        \sa fixedSampleRate() */
  double maxSampleRate( void );
    /*! Set the maximum sampling rate to \a maxrate Hertz.
        \sa setMinSampleInterval(), setFixedSampleRate(), setDefaultMaxSampleRate() */
  void setMaxSampleRate( double maxrate );
    /*! The minimum or fixed sampling interval to be used in seconds.
        If no minimum sampling interval was specified
	(via setMinSampleInterval(), setMaxSampleRate())
	defaultMinSampleInterval() is returned.
        Same as 1/maxSampleRate().
        \sa fixedSampleRate() */
  double minSampleInterval( void );
    /*! Set the minimum sampling interval to \a minsample seconds.
        \sa setMaxSampleRate(), setFixedSampleInterval(), setDefaultMinSampleInterval() */
  void setMinSampleInterval( double minsample );

    /*! Set the fixed sampling rate to \a rate Hertz.
        \sa setFixedSampleInterval(), setMaxSampleRate(), fixedSampleRate() */
  void setFixedSampleRate( double rate );
    /*! Set the minimum sampling interval to \a interval seconds.
        \sa setFixedSampleRate(), setMinSampleInterval(), fixedSampleRate() */
  void setFixedSampleInterval( double interval );
    /*! \c True if the sampling rate is fixed. */
  bool fixedSampleRate( void ) const;

    /*! The default maximum sampling rate to be used in Hertz. 
        Same as 1/defaultMinSampleInterval(). 
        \sa maxSampleRate() */
  static double defaultMaxSampleRate( void );
    /*! Set the default maximum sampling rate to \a maxrate Hertz.
        \sa setDefaultMinSampleInterval() */
  static void setDefaultMaxSampleRate( double maxrate );
    /*! The default minimum sampling interval to be used in seconds.
        Same as 1/defaultMaxSampleRate().
        \sa maxSampleRate() */
  static double defaultMinSampleInterval( void );
    /*! Set the default minimum sampling interval to \a minsample seconds.
        \sa setDefaultMaxSampleRate() */
  static void setDefaultMinSampleInterval( double minsample );

    /*! Returns the optimal sampling rate (in Hertz) that should be used 
        for a signal with carrier frequency \a carrierfreq Hz.
	The optimal sampling rate is the largest possible rate that is
	a multiple of four times the carrier frequency.
	This ensures that a sine wave with frequency \a carrierfreq is sampled
	exactly at the zero crossings, minima, and maxima,
	which is important for a stable output of the amplitude of the sine wave.
        The returned sampling rate is always smaller or equal to maxSampleRate().
	If a fixed sampling rate has to be used, maxSampleRate() is returned.
        If \a carrierfreq <= 0, then maxSampleRate() is returned.
        \note specify an output trace using setTrace() or setTraceName()
	before calling bestSampleInterval()!        
        \sa bestSampleInterval(), setBestSample(), fixedRate()  */
  double bestSampleRate( double carrierfreq );
    /*! Returns the optimal sampling interval (in seconds) that should be used 
        for a signal with carrier frequency \a carrierfreq Hz.
	The optimal sampling interval is the smallest possible interval 
	that, multiplied by four, results in the period of the carrier frequency.
	This ensures that a sine wave with frequency \a carrierfreq is sampled
	exactly at the zero crossings, minima, and maxima,
	which is important for a stable output of the amplitude of the sine wave.
        The returned sampling interval is always larger or equal to minSampleInterval().
	If a fixed sampling interval has to be used, minSampleInterval() is returned.
        If \a carrierfreq <= 0, then minSampleInterval() is returned.
        \note specify an output trace using setTrace() or setTraceName()
	before calling bestSampleInterval()!        
	\sa bestSampleRate(), setBestSample()  */
  double bestSampleInterval( double carrierfreq );
    /*! Set the sampling rate to be the optimal one that should be used 
        for a signal with carrier frequency \a carrierfreq Hz.
	See bestSampleRate() for details. 
        The carrier frequency is set to \a carrierfreq.
        \note specify an output trace using setTrace() or setTraceName()
	before calling bestSampleInterval()!        
        \sa setSampleRate(), setSampleInterval(), setCarrierFreq(), 
	bestSampleRate(), bestSampleInterval() */
  void setBestSample( double carrierfreq );

    /*! In case of fixedSampleRate() and different sample interval
        of the data and the required minSampleInterval(),
	interpolates the data for minSampleInterval(). */
  void fixSample( void );

    /*! Load stimulus from input stream \a str with description \a ident.
	The file has to contain at least two colums of ascii-numbers.
	The first column is the time in seconds, 
	if the unit is not specified as ms in the key. 
	The second column is the stimulus amplitude.
        All metadata in front of the data (marked with a '#')
        is loaded as the description of the stimulus.
        \a filename is added to the stimulus description. */
  istream &load( istream &str, const string &filename );
    /*! Load stimulus from file \a file.
	The file has to contain at least two colums of ascii-numbers.
	The first column is the time in seconds, 
	if the unit is not specified as ms in the key. 
	The second column is the stimulus amplitude.
        All metadata in front of the data (marked with a '#')
        is loaded as the description of the stimulus.
        If \a filename is not empty, it is added to the stimulus description.
        Otherwise, \a file is added. */
  OutData &load( const string &file, const string &filename="" );

    /*! Multiplies each element of the signal
        with some factor such that the largest
        element takes the value \a max.
        Returns the used multiplication factor. */
  double maximize( double max=1.0 );

    /*! Create a stimulus with description \a ident from the given amplitude 
        modulation \a am (in seconds) filled with a sine wave carrier
	with frequency \a carrierfreq Hz.
	In case the carrier frequency is negative, the aamplitude modulation is filled
	with a Gaussian white noise from 0 to \a -carrierfreq Hz.
	The sampling rate is set using bestSampleRate( \a carrierfreq ).
	The carrier frequency of the signal is set to \a carrierfreq.
        \a am must have values ranging from 0...1 and 
	must contain at least 2 elements. 
        \return 1.0 for positive carrier frequencies (sine waves)
        or the factor that was used to scale the signal down to accomodate
        a noise carrier. */
  double fill( const OutData &am, double carrierfreq, 
	       const string &ident = "" );

    /*! Creates a constant stimulus consisting of a single data point
        with value \a value.
	\param[in] \a name the optional name can be used to functionally describe the signal. */
  void constWave( double value, const string &name="" );
    /*! Creates a constant stimulus that assumes \a value for a
        duration of \a duration seconds sampled with \a stepsize.
	\param[in] \a name the optional name can be used to functionally describe the signal.
	\sa pulseWave() */
  void constWave( double duration, double stepsize, double value, const string &name="" );
    /*! Creates a pulse stimulus that assumes \a value for a duration
        of \a duration seconds sampled with \a stepsize. A final value
        after the pulse sets the amplitude of the signal back to \a
        base.  If \a stepsize is negative or if fixedSampleRate(), the
        sampling rate is set using minSampleInterval().
	\param[in] \a name the optional name can be used to functionally describe the signal. */
  void pulseWave( double duration, double stepsize, double value, double base,
		  const string &name="" );
    /*! Creates a rectangle pulse pattern with period \a period,
        duration of the rectangle \a width and constant amplitude \a
	ampl. The up- and downstrokes have a width of \a ramp.
	If \a stepsize is negative or if fixedSampleRate(),
	the sampling rate is set using minSampleInterval().
	\param[in] \a name the optional name can be used to functionally describe the signal. */
  void rectangleWave( double duration, double stepsize,
		      double period, double width, double ramp, double ampl=1.0,
		      const string &name="" );
    /*! Create a sine wave of constant amplitude \a ampl (1.0 = maximum amplitude)
        with freqency \a freq Hz, \a duration seconds and ramps of \a ramp seconds length.
	If fixedSampleRate() the \a stepsize is set to minSampleInterval().
	If \a stepsize is negative, the sampling rate is set using bestSampleRate( \a freq ).
	The carrier frequency of the signal is set to \a freq.
	\param[in] \a name the optional name can be used to functionally describe the signal.
        \note specify an output trace using setTrace() or setTraceName()
	before calling sineWave()! */
  void sineWave( double duration, double stepsize,
		 double freq, double ampl=1.0, double ramp=0.0, 
		 const string &name="" );
    /*! Create Gaussian white noise with cut-off freqency \a cutofffreq in Hz,
        \a duration seconds, and ramps of \a ramp seconds length.
	The noise signal has zero mean and standard deviation \a stdev.
	The carrier frequency of the signal is set to \a cutofffreq.
	If fixedSampleRate() the \a stepsize is set to minSampleInterval().
	If \a stepsize is negative, the sampling rate is set using bestSampleRate( \a cutofffreq ).
	If \a seed is not 0 the value it is pointing to is used as the seed for initializing a random number generator.
	If \a *seed is 0, then the system time is used to generate a seed
        to imitate real randomness.
	The actually used seed is returned in \a *seed.
	\param[in] \a name the optional name can be used to functionally describe the signal.
        \note specify an output trace using setTrace() or setTraceName()
	before calling sineWave()! */
  void noiseWave( double duration, double stepsize, double cutofffreq,
		  double stdev=1.0, unsigned long *seed=0, double ramp=0.0, 
		  const string &name="" );
    /*! Create Gaussian white noise between frequencies
        \a cutofffreqlow and \a cutofffreqhigh in Hz,
        \a duration seconds, and ramps of \a ramp seconds length.
	The noise signal has zero mean and standard deviation \a stdev.
	If fixedSampleRate() the \a stepsize is set to minSampleInterval().
	If \a stepsize is negative, the sampling rate is set using bestSampleRate( \a cutofffreqhigh ).
	The carrier frequency of the signal is set to \a cutofffreqhigh.
	If \a seed is not 0 the value it is pointing to is used as the seed for initializing a random number generator.
	If \a *seed is 0, then the system time is used to generate a seed
        to imitate real randomness.
	The actually used seed is returned in \a *seed.
	\param[in] \a name the optional name can be used to functionally describe the signal.
        \note specify an output trace using setTrace() or setTraceName()
	before calling sineWave()! */
  void bandNoiseWave( double duration, double stepsize,
		      double cutofffreqlow, double cutofffreqhigh,
		      double stdev=1.0, unsigned long *seed=0, double ramp=0.0,
		      const string &name="" );
    /*! Create Ohrnstein-Uhlenbeck noise with time-constant \a tau in sec,
        \a duration seconds, and ramps of \a ramp seconds length.
	The noise signal has zero mean and standard deviation \a stdev.
	The carrier frequency of the signal is set to 1/\a tau.
 	If \a stepsize is negative or if fixedSampleRate(),
	the sampling rate is set using minSampleInterval().
	If \a seed is not 0 the value it is pointing to is used as the seed for initializing a random number generator.
	If \a *seed is 0, then the system time is used to generate a seed
        to imitate real randomness.
	The actually used seed is returned in \a *seed.
	\param[in] \a name the optional name can be used to functionally describe the signal. */
  void ouNoiseWave( double duration, double stepsize,
		    double tau, double stdev=1.0, unsigned long *seed=0, double ramp=0.0, 
		    const string &name="" );
    /*! Creates a frequency sweep from \a startfreq f_1 to \a endfreq
        f_2 of constant amplitude \a ampl and with \a duration seconds. 
	If \a stepsize is negative or if fixedSampleRate(),
	the sampling rate is set using minSampleInterval().
	\param[in] \a name the optional name can be used to functionally describe the signal. */
  void sweepWave( double duration, double stepsize,
		  double startfreq, double endfreq,
		  double ampl=1.0, double ramp=0.0, 
		  const string &name="" );
    /*! Creates a ramp stimulus that starts at \a first and linearly
        ramps up to \a last for a duration of \a duration seconds
        sampled with \a stepsize. 
	\param[in] \a name the optional name can be used to functionally describe the signal. */
  void rampWave( double duration, double stepsize, double first, double last,
		 const string &name="" );
    /*! Creates a sawtooth with period \a period and constant amplitude
        \a ampl with \a duration seconds. The downstroke has a width of
	\a ramp.
	If \a stepsize is negative or if fixedSampleRate(),
	the sampling rate is set using minSampleInterval().
	\param[in] \a name the optional name can be used to functionally describe the signal. */
  void sawUpWave( double duration, double stepsize,
		  double period, double ramp, double ampl=1.0,
		  const string &name="" );
    /*! Creates a sawtooth with period \a period and constant amplitude
        \a ampl with \a duration seconds. The upstroke has a width of \a
	ramp.
	If \a stepsize is negative or if fixedSampleRate(),
	the sampling rate is set using minSampleInterval().
	\param[in] \a name the optional name can be used to functionally describe the signal. */
  void sawDownWave( double duration, double stepsize,
		    double period, double ramp, double ampl=1.0,
		    const string &name="" );
    /*! Creates a triangle with period \a period and constant amplitude
        \a ampl with \a duration seconds. The upstroke and downstroke
	have a width of \a 0.5*period.
	If \a stepsize is negative or if fixedSampleRate(),
	the sampling rate is set using minSampleInterval().
	\param[in] \a name the optional name can be used to functionally describe the signal. */
  void triangleWave( double duration, double stepsize, double period,
		     double ampl=1.0, const string &name="" );
    /*! Creates a train of alpha functions with period \a period seconds and
        of \a duration seconds total duration sampled with \a stepsize. 
	The time constant of the alpha functions is \a tau and
	the alpha functions start at time \a delay in each period.
	The duration is extended such that the stimulus ends five times \a tau
	after the last alpha-function in the train.
	The maximum amplitude of each single aplha-function is \a ampl.
	Note, however, that by summation the resulting signal can have much larger amplitudes.
	If \a stepsize is negative or if fixedSampleRate(),
	the sampling rate is set using minSampleInterval().
        \param[in] \a name the optional name can be used to functionally describe the signal. */
  void alphaWave( double &duration, double stepsize, double period,
		  double tau, double ampl=1.0, double delay=0.0,
		  const string &name="" );

    /*! The index of the next element to be written to the data buffer.
        \sa incrDeviceIndex(), devieValue(), incrDeviceCount(), deviceReset() */
  int deviceIndex( void ) const { return DeviceIndex; };
    /*! Increment deviceIndex(). \sa deviceIndex(), deviceValue() */
  void incrDeviceIndex( void ) { DeviceIndex++; };
    /*! Return the value of the next element to be written to the data buffer
        and increment deviceIndex(). \sa deviceIndex() */
  float deviceValue( void ) { return (*this)[ DeviceIndex++ ]; };
    /*! The number of delay elements. \sa setDeviceDelay() */
  int deviceDelay( void ) const { return DeviceDelay; };
    /*! Set the number of delay elements to \a delay. \sa deviceDelay() */
  void setDeviceDelay( int delay ) { DeviceDelay = delay; };
    /*! The device buffer counter.
        \sa setDeviceCount(), incrDeviceCount(), deviceReset() */
  int deviceCount( void ) const { return DeviceCount; };
    /*! Set the device counter to \a count. \sa deviceCount() */
  void setDeviceCount( int count ) { DeviceCount = count; };
    /*! Increment the device counter and reset deviceIndex(). \sa deviceCount() */
  void incrDeviceCount( void ) { DeviceCount++; DeviceIndex = 0; };
    /*! Returns \c true as long data need to be transferred to the device. */
  bool deviceWriting( void ) const { return ( DeviceCount <= 0 ); };
    /*! Reset the device index and counter. */
  void deviceReset( void ) { DeviceIndex = 0; DeviceCount = 0; };
    /*! Reset the device index, counter, and delay. */
  void deviceReset( int delay )
    { DeviceIndex = 0; DeviceDelay = delay; DeviceCount = delay > 0 ? -1 : 0; };

  friend ostream &operator<<( ostream &str, const OutData &od );


 private:

  void construct( void );
    /*! We do not want an offset! */
  void setRange( const double &offset, const double &stepsize ) {};
  void shift( const double &val ) {};

    /*! Delay in seconds from start trigger to start of aquisition. */
  double Delay;
    /*! Source of start pulse for data aquisition. */
  int StartSource;
    /*! True: Stop running signal to process this one. */
  bool Priority;
    /*! Continuous aquisition mode? */
  bool Continuous;
    /*! True: Restart data acquisition with this signal. */
  bool Restart;
    /*! Maximum alowed sampling rate. */
  double MaxRate;
    /*! Sampling rate has to equal MaxRate. */
  bool FixedRate;
    /*! Device identifier. */
  int Device;
    /*! Channel number. */
  int Channel;
    /*! Trace index that later specifies channel and device. */
  int Trace;
    /*! Trace name that later specifies channel and device. */
  string TraceName;
    /*! Identifier for the output signal. */
  string Ident;
    /*! Description of the output signal. */
  Options Description;
    /*! Minimum value for which a gain should be chosen. */
  double RequestMinValue;
    /*! Maximum value for which a gain should be chosen. */
  double RequestMaxValue;
    /*! Encodes gain, polarity, and reference. */
  int GainIndex;
    /*! Some data used by AnalogOutput to convert voltage to
        raw integer data for the data acquisition board. */
  char *GainData;
    /*! Scale from signal to voltage. */
  double Scale;
    /*! The unit of the signal. */
  string Unit;
    /*! The minimum possible voltage for the preset gain of the daq board. */
  double MinVoltage;
    /*! The maximum possible voltage for the preset gain of the daq board. */
  double MaxVoltage;
    /*! Output intensity. */
  double Intensity;
    /*! Carrier frequency of signal, important for attenuator. */
  double CarrierFreq;
    /*! Attenuation level. */
  double Level;

    /*! Index to the element that should be written to the device next. */
  int DeviceIndex;
    /*! Number of zeros that should be written to emulate the delay. */
  int DeviceDelay;
    /*! Counts repetitions of outputs. -1: delay emulation, 0: first time. */
  int DeviceCount;

    /*! Default minimum possible sampling interval in seconds. */
  static double DefaultMinSampleInterval;

    /*! Pointer to the Acquire class managing all output traces. */
  static const Acquire *A;

};


template < typename R >
OutData::OutData( const R *a, int n, const double stepsize )
  : SampleDataF( a, n, 0.0, stepsize )
{
  construct();
}


template < typename R >
OutData::OutData( const vector< R > &a, const double stepsize ) 
  : SampleDataF( a, 0, stepsize )
{
  construct();
}


template < typename R >
OutData::OutData( const Array< R > &a, const double stepsize ) 
  : SampleDataF( a, 0.0, stepsize )
{
  construct();
}


template < typename R >
OutData::OutData( const SampleData< R > &sa ) 
  : SampleDataF( sa )
{
  SampleDataF::setOffset( 0.0 );
  construct();
}


template < typename R >
const OutData &OutData::operator=( const R &a )
{
  return assign( a );
}


template < typename R >
const OutData &OutData::operator=( const SampleData< R > &a )
{
  return assign( a );
}


template < typename R >
OutData &OutData::assign( const R *a, int n, const double stepsize )
{
  SampleDataF::assign( a, n, 0, stepsize);
  return *this;
}


template < typename R >
OutData &OutData::assign( const R &a, const double stepsize )
{
  SampleDataF::assign( a, 0.0, stepsize);
  return *this;
}


template < typename R >
OutData &OutData::assign( const SampleData< R > &sa )
{
  SampleDataF::assign( sa );
  SampleDataF::setOffset( 0.0 );
  return *this;
}


template < typename R >
const OutData &OutData::copy( R &a ) const
{
  SampleDataF::copy( a );
  return *this;
}


template < typename R >
const OutData &OutData::copy( R *a, int n, const float &val ) const
{
  SampleDataF::copy( a, n, val );
  return *this;
}


}; /* namespace relacs */

#endif /* ! _RELACS_OUTDATA_H_ */
