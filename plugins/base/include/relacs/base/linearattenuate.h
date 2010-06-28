/*
  base/linearattenuate.h
  Linear conversion of intensity to attenuation level independent of carrier frequency.

  RELACS - Relaxed ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2010 Jan Benda <benda@bio.lmu.de>

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

#ifndef _RELACS_BASE_LINEARATTENUATE_H_
#define _RELACS_BASE_LINEARATTENUATE_H_


#include <relacs/configclass.h>
#include <relacs/attenuate.h>
using namespace relacs;

namespace base {


/*!
\class LinearAttenuate
\author Jan Benda
\version 1.1
\brief [Attenuate] Linear conversion of intensity to attenuation level independent of carrier frequency.

\par Options
- \c line: the line on the attenuator
- \c aodevice: the identifier analog output device
- \c aochannel: the channel of the analog output device
- \c intensityname: name of the intensity variable
- \c intensityunit: unit of the intensity variable
- \c intensityformat: format string for the intensity variable

*/


class LinearAttenuate : public Attenuate, public ConfigClass
{

public:

    /*! Constructor. */
  LinearAttenuate( void );
    /*! Destructor. */
  ~LinearAttenuate( void );

    /*! Returns the gain factor. */
  double gain( void ) const { return Gain; };
    /*! Set the gain factor to \a gain. */
  void setGain( double gain );
    /*! Returns the offset. */
  double offset( void ) const { return Offset; };
    /*! Set the offset to \a offset. */
  void setOffset( double offset );
    /*! Set the gain factor to \a gain and the offset to \a offset. */
  void setGain( double gain, double offset );

    /*! Set the device and the config identifier string to \a ident. */
  virtual void setDeviceIdent( const string &ident );
    /*! Set the \a Gain and \a Offset variable if the config is changed. */
  virtual void notify( void );

    /*! Adds offset and gain to the info(). */
  virtual void init( void );


protected:

    /*! Transform the requested intensity \a intensity
        for the carrier frequency \a frequency of the signal into
        \a db decibel which are ued to set the attenuation level. */
  int decibel( double intensity, double frequency, double &db ) const;
    /*! Transform the attenuation level \a decibel
        for the carrier frequency \a frequency of the signal into
        the intesity \a intens. 
        This is the inverse function of decibel(). */
  void intensity( double &intens, double frequency, double decibel ) const;


private:

  double Gain;
  double Offset;

};


}; /* namespace base */

#endif /* ! _RELACS_BASE_LINEARATTENUATE_H_ */
