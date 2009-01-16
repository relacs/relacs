/*
  kernel.h
  Base class of all kernel functions.

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

#ifndef _RELACS_KERNEL_H_
#define _RELACS_KERNEL_H_ 1

namespace relacs {


/*! 
\class Kernel
\author Jan Benda
\version 1.0
\brief Base class of all kernel functions.


A Kernel is a function y=f(x) which is concentrated between
x=left() and x=right(), i.e.
f(x) is zero (or close to zero) for x less than left()
and for x greater than right().
The integral over the kernel is unity:
\f[ \int_{-\infty}^{+\infty} f(x) dx = 1 \f]
The width of the kernel can be controlled by a scale factor \a s,
\f$ f(x) \rightarrow f(x/s) \f$,
via setScale() or by setting the standard deviation of the kernel
via setStdev().
The actual scale factor is returned by scale()
and the corresponding standard deviation by stdev().
Note that the value returned by stdev() usually differs from scale().
The mean of the kernel is mean().
The maximum value of the kernel is max().
*/

class Kernel
{

public:

    /*! Constructs a default kernel with its scale factor set to one. */
  Kernel( void ) : Scale( 1.0 ) {};
    /*! Constructs a kernel with standard deviation \a stdev. */
  Kernel( double stdev ) : Scale( 1.0 ) { setStdev( stdev ); };
    /*! Destructor. */
  virtual ~Kernel( void ) {};

    /*! Returns the value of the kernel at position \a x.
        This operator is NOT virtual in order to speed up
        computation when using templates. 
        If you need a virtual function as an interface, implement value(). */
  double operator()( double x ) const { return value( x ); };
    /*! Returns the value of the kernel at position \a x.
        This function IS virtual and might slow things down. 
        If you need a NON virtual function as an interface,
	implement operator(). */
  virtual double value( double x ) const;

    /*! Return the scale factor of the kernel (the width). */
  double scale( void ) const { return Scale; };
    /*! Set the scale factor (width) of the kernel to \a scale. */
  virtual void setScale( double scale );

    /*! Return the mean of the kernel.
        This default implementation calculates the mean numerically
        from 400 points between left() and right(). */
  virtual double mean( void ) const;
    /*! Return the standard deviation of the kernel,
        which does not have to equal the scale().
        This default implementation calculates the standard deviation numerically
        from 400 points between left() and right(). */
  virtual double stdev( void ) const;
    /*! Set the standard deviation of the kernel to \a stdev. 
        This default implementation sets the scale() equal to \a stdev. */
  virtual void setStdev( double stdev );
    /*! Return the maximum value of the kernel.
        This default implementation searches for the maximum
	of 400 computed function values between left() and right(). */
  virtual double max( void ) const;

    /*! The leftmost extension of the kernel.
        For x values less than this value,
        the value of the kernel is negligible.
        Returns minus half the scale factor. */
  virtual double left( void ) const;
    /*! The rightmost extension of the kernel.
        For x values greater than this value,
        the value of the kernel is negligible.
        Returns half the scale factor. */
  virtual double right( void ) const;


private:

  double Scale;

};


/*! 
\class RectKernel
\author Jan Benda
\version 1.0
\brief A rectangular kernel.


\f[ f(x) = \left\{ \begin{array}{lcl} 1/s & ; & -s/2 \le x \le s/2 \\ 0 & ; & else \end{array} \right. \f]
\f[ \sigma = \frac{s}{2\sqrt{3}} \f]
\f[ s = 2\sqrt{3} \sigma \f]
\f[ f(x) = \left\{ \begin{array}{lcl} \frac{1}{2\sqrt{3}\sigma} & ; & -\sqrt{3}\sigma \le x \le \sqrt{3}\sigma \\ 0 & ; & else \end{array} \right. \f]
*/

class RectKernel : public Kernel
{

public:

  RectKernel( void );
  RectKernel( double stdev );
  virtual ~RectKernel( void );

  double operator()( double x ) const;
  virtual double value( double x ) const;

  virtual void setScale( double scale );

  virtual double mean( void ) const;
  virtual double stdev( void ) const;
  virtual void setStdev( double stdev );
  virtual double max( void ) const;


private:

  double Height;

};


/*! 
\class TriangularKernel
\author Jan Benda
\version 1.0
\brief A triangular kernel.


\f[ f(x) = \left\{ \begin{array}{lcl} \sqrt{\frac{1}{s}} - \left| \frac{x}{s} \right| & ; & -\sqrt{s} \le x \le \sqrt{s} \\ 0 & ; & else \end{array} \right. \f]
\f[ \sigma = \sqrt{s/6} \f]
\f[ s = 6 \sigma^2 \f]
\f[ f(x) = \left\{ \begin{array}{lcl} \frac{1}{6\sigma^2}\left( \sqrt{6}\sigma-|x| \right) & ; & -\sqrt{6}\sigma \le x \le \sqrt{6}\sigma \\ 0 & ; & else \end{array} \right. \f]
*/


class TriangularKernel : public Kernel
{

public:

  TriangularKernel( void );
  TriangularKernel( double stdev );
  virtual ~TriangularKernel( void );

  double operator()( double x ) const;
  virtual double value( double x ) const;

  virtual void setScale( double scale );

  virtual double mean( void ) const;
  virtual double stdev( void ) const;
  virtual void setStdev( double stdev );
  virtual double max( void ) const;

  virtual double left( void ) const;
  virtual double right( void ) const;


private:

  double Max;

};


/*! 
\class EpanechnikovKernel
\author Jan Benda
\version 1.0
\brief A quadratic kernel.


\f[ f(x) = \left\{ \begin{array}{lcl} \left( \frac{3}{4 s} \right)^{\frac{2}{3}} - \left( \frac{x}{s} \right)^2 & ; & - s \sqrt[3]{\frac{3}{4s}} \le x \le s \sqrt[3]{\frac{3}{4s}} \\ 0 & ; & else \end{array} \right. \f]
\f[ \sigma = s^{2/3} \sqrt{\frac{1}{5}} \sqrt[3]{\frac{3}{4}} \f]
\f[ s = \sqrt{\frac{4}{3}} 5^{3/4} \sigma^{3/2} \f]
\f[ f(x) = \left\{ \begin{array}{lcl} \frac{3}{4\sqrt{5}\sigma}\left( 1 - \frac{x^2}{5\sigma^2} \right) & ; & -\sqrt{5}\sigma \le x \le \sqrt{5}\sigma \\ 0 & ; & else \end{array} \right. \f]
*/


class EpanechnikovKernel : public Kernel
{

public:

  EpanechnikovKernel( void );
  EpanechnikovKernel( double stdev );
  virtual ~EpanechnikovKernel( void );

  double operator()( double x ) const;
  virtual double value( double x ) const;

  virtual void setScale( double scale );

  virtual double mean( void ) const;
  virtual double stdev( void ) const;
  virtual void setStdev( double stdev );
  virtual double max( void ) const;

  virtual double left( void ) const;
  virtual double right( void ) const;


private:

  double Stdev;
  double Max;
  double Fac;

};


/*! 
\class GaussKernel
\author Jan Benda
\version 1.0
\brief A gaussian kernel.


\f[ f(x) = \frac{1}{s\sqrt{2 \pi}} \mathrm{e}^{ - \frac{x^2}{2 s^2}} \f]
\f[ \sigma = s \f]
\f[ f(x) = \frac{1}{\sigma\sqrt{2 \pi}} \mathrm{e}^{ - \frac{x^2}{2 \sigma^2}} \f]
*/

class GaussKernel : public Kernel
{

public:

  GaussKernel( void );
  GaussKernel( double stdev );
  virtual ~GaussKernel( void );

  double operator()( double x ) const;
  virtual double value( double x ) const;

  virtual void setScale( double scale );

  virtual double mean( void ) const;
  virtual double stdev( void ) const;
  virtual void setStdev( double stdev );
  virtual double max( void ) const;

  virtual double left( void ) const;
  virtual double right( void ) const;


private:

  double Norm;

};


/*! 
\class GammaKernel
\author Jan Benda
\version 1.0
\brief A gamma kernel of order \a n.


\f[ f_n(x) = \left\{ \begin{array}{lcl} \frac{1}{s (n-1)!} \left( \frac{x}{s} \right)^{n-1} \mathrm{e}^{-\frac{x}{s}} & ; & 0 \le x \le 5 s\sqrt{n} \\ 0 & ; & x < 0 \end{array} \right. \f]
\f[ f_1(x) = \frac{1}{s} \mathrm{e}^{-\frac{x}{s}} \f]
\f[ \frac{x}{s} f_n(x) = n f_{n+1}(x) \f]
\f[ \Gamma(n) = (n-1)! \f]
\f[ \mu = s n \f]
\f[ \sigma = s\sqrt{n} \f]

THe order of the gamma kernel is order()
and can be set by setOrder().
*/

class GammaKernel : public Kernel
{

public:

  GammaKernel( void );
  GammaKernel( double stdev, int order=1 );
  virtual ~GammaKernel( void );

  double operator()( double x ) const;
  virtual double value( double x ) const;

  virtual void setScale( double scale );

  virtual double mean( void ) const;
  virtual double stdev( void ) const;
  virtual void setStdev( double stdev );
  virtual double max( void ) const;

  int order( void ) const { return Order; };
  void setOrder( int order ) { Order = order > 0 ? order : 1; };

  virtual double left( void ) const;
  virtual double right( void ) const;


private:

  int Order;
  double Norm;

};


}; /* namespace relacs */

#endif /* ! _RELACS_KERNEL_H_ */
