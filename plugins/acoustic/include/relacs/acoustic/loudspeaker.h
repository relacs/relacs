/*
  acoustic/loudspeaker.h
  Direct conversion to attenuation level 

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

#ifndef _RELACS_ACOUSTIC_LOUDSPEAKER_H_
#define _RELACS_ACOUSTIC_LOUDSPEAKER_H_ 1


#include <relacs/array.h>
#include <relacs/configclass.h>
#include <relacs/attenuate.h>
using namespace std;
using namespace relacs;

namespace acoustic {


/*!
\class LoudSpeaker
\brief [Attenuate] Converts intensities givenen in dB SPL to attenuation level 
       in dependence on carrier frequency.
\author Jan Benda
\version 1.4 (Jul 5, 2017)

\par Calibration
The conversion is set by the CalibSpeaker RePro which defines stimulus intensity as
dB SPL, i.e. root-mean-square amplitude of a full-amplitude sine wave.

The calibration table is saved in the files \c calibD-C.dat where \c D
is the device number of the data acquisition board and C is the analog
output channel of that board.

\par Options
- \c line: the line on the attenuator
- \c aodevice: the identifier analog output device
- \c aochannel: the channel of the analog output device
- \c maxvoltage: maximum allowed peak voltage to be put out by the attenuator.
*/


class LoudSpeaker : public Attenuate, public ConfigClass
{

public:

    /*! Constructor. */
  LoudSpeaker( void );
    /*! Destructor. */
  virtual ~LoudSpeaker( void );

    /*! Returns the gain factor used for a signal 
        with carrier frequency \a frequency. */
  double gain( double frequency ) const;
    /*! Returns the offset used for a signal 
        with carrier frequency \a frequency. */
  double offset( double frequency ) const;
    /*! Returns the gain factor in \a gain and the offset in \a offset 
        used for a signal with carrier frequency \a frequency.
        \a frequency is set to the closest value found 
	in the calibration table.
        \a gain and \a offset are linearly interpolated. */
  void gain( double &gain, double &offset, double &frequency ) const;
    /*! Set the gain factor for a signal 
        with carrier frequency \a frequency
	to \a gain and the offset to \a offset. */
  void setGain( double gain, double offset, double frequency );
    /*! Reset the gain factor and the offset for a signal 
        with carrier frequency \a frequency
	to the default values. */
  void reset( double frequency );
    /*! Clear the calibration table. */
  void clear( void );

    /*! Return the calibration table in \a freq, \a offs, \a gain */
  void calibrationTable( ArrayD &freq, ArrayD &offs, ArrayD &gain ) const;

    /*! Set the sampling rate that was used while determining the gain()
        and offset() to \a rate. 
        It is just saved in the calibration file. */
  void setSamplingRate( double rate );

    /*! Load the calibration data. */
  void load( void );
    /*! Save the calibration data to \a file, using \a date. */
  void saveCalibration( const string &file, const string &date ) const;
    /*! Save the calibration data in directory \a path. */
  virtual void save( const string &path ) const;
    /*! Save the calibration data to the default file. */
  void save( void ) const;

    /*! Set the device and the config identifier string to \a ident. */
  virtual void setDeviceIdent( const string &ident );

    /*! Load the configuration file. */
  virtual void config( void );


protected:

    /*! Transform the requested sound intensity \a intensity
        for the carrier frequency \a frequency of the signal into
        \a db decibel which are used to set the attenuation level.
	A gain and an offset value are retrieved from the calibration table
	according to the requested carrier frequency \a frequency.
	The attenuation level is then calculated as the requested
	intensity multiplied by gain plus the offset.
	If \a intensity is set to MuteIntensity, the attenuator is muted. */
  virtual int decibel( double intensity, double frequency, double &db ) const;
    /*! Transform the attenuation level \a decibel
        for the carrier frequency \a frequency of the signal into
        the intensity \a intens.
	If \a decibel is set to MuteAttenuationLevel, then
	MuteIntensity is returned in \a intens.
        This is the inverse function of decibel(). */
  virtual void intensity( double &intens, double frequency, double decibel ) const;


private:

  ArrayD Frequency;
  ArrayD Gain;
  ArrayD Offset;

  string CalibFile;
  double DefaultGain;
  double DefaultOffset;
  double MaxVoltage;
  double SamplingRate;
  mutable string CalibDate;

};


}; /* namespace acoustic */

#endif /* ! _RELACS_ACOUSTIC_LOUDSPEAKER_H_ */
