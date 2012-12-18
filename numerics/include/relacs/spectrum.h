/*
  spectrum.h
  

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

#ifndef _RELACS_SPECTRUM_H_
#define _RELACS_SPECTRUM_H_ 1

#include <cmath>
#include <algorithm>

using namespace std;

namespace relacs {

  /*! \return the smalles power of two that is equal or greater than \a n. */
int nextPowerOfTwo( int n );

  /*! Compute an in-place radix-2 FFT on the range \a first, \a last
      of complex numbers.
      The size \a N = \a last - \a first of the range has to be a power of two,
      otherwise -1 is returned.
      \param[in] sign determines the sign of the exponential.
      Usually \a sign=-1 is used for forward and \a sign=1 for backward transformation.
      The backward transformation is not normalized;
      you need to multiply the real and imaginary part of each element by \a 1/N.
      \param RandomAccessIter is a random access iterator that points to a REAL number. 
      Real and imaginary parts of each complex number are placed in alternate neighboring elements,
      i.e. *(first+2*i) and *(first+2*i+1) point to the real and imaginary part
      of the \a i-th complex number of the range.
      If the input data are spaced by \a Delta,
      then the first half of the output range contains the positive frequencies
      at i/(N Delta), i=0..N/2, the negative frequencies are stored
      backwards from the end with the frequencies -(N-i)/(N Delta) at the indices
      i=N/2+1..N-1.
      Algorithm adapted from the GNU Scientific Library http://www.gnu.org/software/gsl .
      \sa rFFT(), cPower(), cMagnitude(), cPhase() */
template < typename RandomAccessIter >
int cFFT( RandomAccessIter first, RandomAccessIter last, int sign );
template < typename Container >
int cFFT( Container &c, int sign );

  /*! Return in the range \a firstp, \a lastp the power 
      of the complex fourier transform in the range \a firstc, \a lastc.
      Half the number \a N of data elements in the range \a firstc, \a lastc
      can be assigned a power in the range \a firstp, \a lastp,
      excess elements are set to zero.
      If the input data to cFFT() were spaced by \a Delta,
      then the power is computed for the frequencies i/(N Delta), i=0..N/2.
      \sa cMagnitude(), cPhase(), cFFT() */
template < typename BidirectIterC, typename ForwardIterP >
void cPower( BidirectIterC firstc, BidirectIterC lastc,
	      ForwardIterP firstp, ForwardIterP lastp );
template < typename ContainerC, typename ContainerP >
void cPower( ContainerC &c, ContainerP &p );
  /*! Return in the range \a firstm, \a lastm the magnitude (absolute value)
      of the complex fourier transform in the range \a firstc, \a lastc.
      Each of the \a N data elements in the range \a firstc, \a lastc
      can be assigned a magnitude in the range \a firstm, \a lastm,
      excess elements are set to zero.
      If the input data to cFFT() were spaced by \a Delta,
      then the magnitude is computed for the frequencies i/(N Delta), i=-N/2+1..N/2.
      \sa cPower(), cPhase(), cFFT() */
template < typename BidirectIterC, typename ForwardIterM >
void cMagnitude( BidirectIterC firstc, BidirectIterC lastc,
		  ForwardIterM firstm, ForwardIterM lastm );
template < typename ContainerC, typename ContainerM >
void cMagnitude( ContainerC &c, ContainerM &m );
  /*! Return in the range \a firstp, \a lastp the phase (argument, from -pi to pi) 
      of the complex fourier transform in the range \a firstc, \a lastc.
      Each of the \a N data elements in the range \a firstc, \a lastc
      can be assigned a phase in the range \a firstm, \a lastm,
      excess elements are set to zero.
      If the input data to rFFT() were spaced by \a Delta,
      then the phase is computed for the frequencies i/(N Delta), i=-N/2+1..N/2.
      \sa cPower(), cMagnitude(), cFFT() */
template < typename BidirectIterC, typename ForwardIterP >
void cPhase( BidirectIterC firstc, BidirectIterC lastc,
	      ForwardIterP firstp, ForwardIterP lastp );
template < typename ContainerC, typename ContainerP >
void cPhase( ContainerC &c, ContainerP &p );

  /*! Compute an in-place radix-2 FFT on the range \a first, \a last
      of real numbers.
      The size \a N = \a last - \a first of the range has to be a power of two,
      otherwise -1 is returned.
      The output is a half-complex sequence, which is stored in-place. 
      The arrangement of the half-complex terms uses the following
      scheme: for k < N/2 the real part of the k-th term is stored in
      location k, and the corresponding imaginary part is stored in
      location N-k. Terms with k > N/2 (the negative frequencies)
      can be reconstructed using the symmetry z_k = z^*_{N-k}.
      The terms for k=0 and k=N/2 are both
      purely real, and count as a special case. Their real parts are
      stored in locations 0 and N/2 respectively, while their
      imaginary parts which are zero are not stored.
      If the input data are spaced by \a Delta,
      then the first half of the output range contains the positive frequencies
      at i/(N Delta), i=0..N/2.
      \param RandomAccessIter is a random access iterator that points to a
      real number. 
      Algorithm adapted from the GNU Scientific Library http://www.gnu.org/software/gsl .
      \sa hcFFT(), cFFT(), hcPower(), hcMagnitude(), hcPhase(), hcReal(), hcImaginary() */
template < typename RandomAccessIter >
int rFFT( RandomAccessIter first, RandomAccessIter last );
template < typename Container >
int rFFT( Container &c );

  /*! Compute the inverse in-place radix-2 FFT on the half-complex
      sequence \a first, \a last stored according the output scheme used by
      rFFT(). 
      The size \a N = \a last - \a first of the range has to be a power of two,
      otherwise -1 is returned.
      The result is a real array stored in natural order that is not normalized;
      you need to multiply each element by \a 1/N.
      \param RandomAccessIter is a random access iterator that points to a
      real number. 
      Algorithm adapted from the GNU Scientific Library http://www.gnu.org/software/gsl .
      \sa rFFT(), cFFT() */
template < typename RandomAccessIter >
int hcFFT( RandomAccessIter first, RandomAccessIter last );
template < typename Container >
int hcFFT( Container &c );

  /*! Return in the range \a firstp, \a lastp the power 
      of the half-complex sequence in the range \a firsthc, \a lasthc.
      Half the number \a N of data elements in the range \a firsthc, \a lasthc
      can be assigned a power in the range \a firstp, \a lastp,
      excess elements are set to zero.
      If you want to compute the power from rFFT, you need to multiply the
      result by \[$ 2/N^2 \f$ to normalize the power such that its sum equals the
      mean squared amplitudes of the signal.
      If the input data to rFFT() were spaced by \a Delta,
      then the power is computed for the frequencies i/(N Delta), i=0..N/2.
      \sa hcMagnitude(), hcPhase(), hcReal(), hcImaginary(), rFFT() */
template < typename BidirectIterHC, typename ForwardIterP >
void hcPower( BidirectIterHC firsthc, BidirectIterHC lasthc,
	      ForwardIterP firstp, ForwardIterP lastp );
template < typename ContainerHC, typename ContainerP >
void hcPower( const ContainerHC &hc, ContainerP &p );
  /*! Return in the range \a firstm, \a lastm the magnitude
      (absolute value, square root of the power)
      of the half-complex sequence in the range \a firsthc, \a lasthc.
      Half the number \a N of data elements in the range \a firsthc, \a lasthc
      can be assigned a magnitude in the range \a firstm, \a lastm,
      excess elements are set to zero.
      If the input data to rFFT() were spaced by \a Delta,
      then the magnitude is computed for the frequencies i/(N Delta), i=0..N/2.
      \sa hcPower(), hcPhase(), hcReal(), hcImaginary(), rFFT() */
template < typename BidirectIterHC, typename ForwardIterM >
void hcMagnitude( BidirectIterHC firsthc, BidirectIterHC lasthc,
		  ForwardIterM firstm, ForwardIterM lastm );
template < typename ContainerHC, typename ContainerM >
void hcMagnitude( const ContainerHC &hc, ContainerM &m );
  /*! Return in the range \a firstp, \a lastp the phase (argument, from -pi to pi) 
      of the half-complex sequence in the range \a firsthc, \a lasthc.
      Half the number \a N of data elements in the range \a firsthc, \a lasthc
      can be assigned a phase in the range \a firstm, \a lastm,
      excess elements are set to zero.
      If the input data to rFFT() were spaced by \a Delta,
      then the phase is computed for the frequencies i/(N Delta), i=0..N/2.
      \sa hcPower(), hcMagnitude(), hcReal(), hcImaginary(), rFFT() */
template < typename BidirectIterHC, typename ForwardIterP >
void hcPhase( BidirectIterHC firsthc, BidirectIterHC lasthc,
	      ForwardIterP firstp, ForwardIterP lastp );
template < typename ContainerHC, typename ContainerP >
void hcPhase( const ContainerHC &hc, ContainerP &p );
  /*! Return in the range \a firstr, \a lastr the real parts
      of the half-complex sequence in the range \a firsthc, \a lasthc.
      Half the number \a N of data elements in the range \a firsthc, \a lasthc
      can be assigned a real part in the range \a firstm, \a lastm,
      excess elements are set to zero.
      If the input data to rFFT() were spaced by \a Delta,
      then the real parts are computed for the frequencies i/(N Delta), i=0..N/2.
      \sa hcPower(), hcPhase(), hcImaginary(), rFFT() */
template < typename BidirectIterHC, typename ForwardIterR >
void hcReal( BidirectIterHC firsthc, BidirectIterHC lasthc,
	     ForwardIterR firstr, ForwardIterR lastr );
template < typename ContainerHC, typename ContainerR >
void hcReal( const ContainerHC &hc, ContainerR &r );
  /*! Return in the range \a firsti, \a lasti the imaginary parts
      of the half-complex sequence in the range \a firsthc, \a lasthc.
      Half the number \a N of data elements in the range \a firsthc, \a lasthc
      can be assigned a imaginary part in the range \a firstm, \a lastm,
      excess elements are set to zero.
      If the input data to rFFT() were spaced by \a Delta,
      then the imaginary parts are computed for the frequencies i/(N Delta), i=0..N/2.
      \sa hcPower(), hcPhase(), hcReal(), rFFT() */
template < typename BidirectIterHC, typename ForwardIterI >
void hcImaginary( BidirectIterHC firsthc, BidirectIterHC lasthc,
		  ForwardIterI firsti, ForwardIterI lasti );
template < typename ContainerHC, typename ContainerI >
void hcImaginary( const ContainerHC &hc, ContainerI &i );

  /*! The Bartlett-window 
      \f[ w_j = 1 - \left| \frac{j-N/2}{N/2} \right| \f] 
      for estimating the power spectrum.
      See Oppenheim & Schafer, Digital Signal Processing, p. 241 (1st ed.) */
double bartlett( int j, int n );
  /*! The Blackman-window 
      \f[ w_j = 0.42 - 0.5*\cos(2 \pi j / N) + 0.08*\cos(4 \pi j / N) \f] 
      for estimating the power spectrum.
      See Oppenheim & Schafer, Digital Signal Processing, p. 242 (1st ed.) */
double blackman( int j, int n );
  /*! The Blackman-Harris-window 
      \f[ w_j = 0.35875 - 0.48829*\cos(2 \pi j / N) + 0.14128*\cos(4 \pi j / N) - 0.01168\cos(6 \pi j / N) \f] 
      for estimating the power spectrum.
      See Harris, F.J., "On the use of windows for harmonic analysis with the
      discrete Fourier transform", Proc. IEEE, Jan. 1978 */
double blackmanHarris( int j, int n );
  /*! The Hamming-window 
      \f[ w_j = 0.54 - 0.46\cos(2 \pi j / N) \f] 
      for estimating the power spectrum.
      See Oppenheim & Schafer, Digital Signal Processing, p. 242 (1st ed.) */
double hamming( int j, int n );
  /*! The Hanning-window 
      \f[ w_j = 0.5 - 0.5\cos(2 \pi j / N) \f] 
      for estimating the power spectrum.
      See Oppenheim & Schafer, Digital Signal Processing, p. 242 (1st ed.)
      The second edition of Numerical Recipes calls this the "Hann" window. */
double hanning( int j, int n );
  /*! The Parzen-window 
      \f[ w_j = 1 - \left| \frac{j-N/2}{1+N/2}\right| \f] 
      for estimating the power spectrum.
      See Press, Flannery, Teukolsky, & Vetterling, Numerical Recipes in C,
      p. 442 (1st ed.) */
double parzen( int j, int n );
  /*! The Square-window 
      \f[ w_j = 1 \f] 
      for estimating the power spectrum. */
double square( int j, int n );
  /*! The Welch-window 
      \f[ w_j = 1 - \left( \frac{j-N/2}{N/2} \right)^2 \f] 
      for estimating the power spectrum.
      See Press, Flannery, Teukolsky, & Vetterling, Numerical Recipes in C,
      p. 442 (1st ed.) or p. 554 (2nd ed.) */
double welch( int j, int n );

  /*! Compute the power spectrum of the range \a firstx, \a lastx
      of real numbers.
      The power spectrum is returned in the range \a firstp, \a lastp.
      The spectrum is normalized such that its sum equals the
      mean squared amplitudes of the signal.
      The input range is divided into chunks of TWO times \a N,
      where \a N is the minimum power of two not less than 
      the number of data points of the power spectrum.
      The chunks may overlap by half according to \a overlap. 
      Each chunk is windowed through a \a window function.
      The final chunk may exceed the data. In that case it is
      discarded, if it contains less than 1.5*N data elements.
      Otherwise it is filled up with zeros and weighted appropriately.
      To avoid zero padding the data buffer should contain a multiple of
      N (\a overlap = \c true) or 2N (\a overlap = \c false) data points.
      If the input data were sampled with \a delta, then 
      the frequencies are sampled with 1/(2 N delta).
      \a ForwardIterX is a forward iterator that points to a real number.
      \a ForwardIterP is a forward iterator that points to a real number. */
template < typename ForwardIterX, typename ForwardIterP >
int rPSD( ForwardIterX firstx, ForwardIterX lastx,
	  ForwardIterP firstp, ForwardIterP lastp,
	  bool overlap=true, double (*window)( int j, int n )=bartlett );
template < typename ContainerX, typename ContainerP >
int rPSD( const ContainerX &x, ContainerP &p,
	  bool overlap=true, double (*window)( int j, int n )=bartlett );

  /*! Compute transfer function between the two ranges \a firstx, \a lastx
      and \a firsty, \a lasty as a half-complex sequence
      in range \a firsth, \a lasth.
      The input ranges are divided into chunks of \a N data points,
      where \a N/2 is the number of complex data points of the transfer function
      (N = lasth - firsth). N must be a power of two.
      The chunks may overlap by half according to \a overlap. 
      Each chunk is windowed through a \a window function
      before passing it to rFFT().
      The final chunk may exceed the data. In that case it is
      discarded, if it contains less than 0.75*N data elements.
      Otherwise it is filled up with zeros and weighted appropriately.
      To avoid zero padding the data buffer should contain a multiple of
      N/2 (\a overlap = \c true) or N (\a overlap = \c false) data points.
      If the input data were sampled with \a delta, then 
      the frequencies are sampled with 1/(N delta).
      The gain, phase, real parts, and imaginary parts of the transfer function can
      be computed using hcMagnitude(), hcPhase(), hcReal(), and hcImaginary(),
      respectively.
      \a ForwardIterX and \a ForwardIterY
      are forward iterators that point to real numbers. 
      \a BidirectH is a bidirectional iterator pointing to real numbers. */
template < typename ForwardIterX, typename ForwardIterY,
  typename BidirectIterH >
int transfer( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty,
	      BidirectIterH firsth, BidirectIterH lasth,
	      bool overlap=true, double (*window)( int j, int n )=bartlett );
template < typename ContainerX, typename ContainerY, typename ContainerH >
int transfer( const ContainerX &x, const ContainerY &y, ContainerH &h,
	      bool overlap=true, double (*window)( int j, int n )=bartlett );
  /*! Compute transfer function between the two ranges \a firstx, \a lastx
      and \a firsty, \a lasty as a half-complex sequence
      in range \a firsth, \a lasth and the coherence in the range
      \a firstc, \a lastc.
      The input ranges are divided into chunks of \a N data points,
      where \a N/2 is the number of complex data points of the transfer function
      (N = lasth - firsth = 2*(lastc-firstc)). N must be a power of two.
      The chunks may overlap by half according to \a overlap. 
      Each chunk is windowed through a \a window function
      before passing it to rFFT().
      The final chunk may exceed the data. In that case it is
      discarded, if it contains less than 0.75*N data elements.
      Otherwise it is filled up with zeros and weighted appropriately.
      To avoid zero padding the data buffer should contain a multiple of
      N/2 (\a overlap = \c true) or N (\a overlap = \c false) data points.
      If the input data were sampled with \a delta, then 
      the frequencies are sampled with 1/(N delta).
      The gain, phase, real parts, and imaginary parts of the transfer function can
      be computed using hcMagnitude(), hcPhase(), hcReal(), and hcImaginary(),
      respectively.
      \a ForwardIterX and \a ForwardIterY
      are forward iterators that point to real numbers. 
      \a BidirectH and \a BidirectC are a bidirectional iterators
      pointing to real numbers. */
template < typename ForwardIterX, typename ForwardIterY,
  typename BidirectIterH, typename BidirectIterC >
int transfer( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty,
	      BidirectIterH firsth, BidirectIterH lasth,
	      BidirectIterC firstc, BidirectIterC lastc,
	      bool overlap=true, double (*window)( int j, int n )=bartlett );
template < typename ContainerX, typename ContainerY,
  typename ContainerH, typename ContainerC >
int transfer( const ContainerX &x, const ContainerY &y,
	      ContainerH &h, ContainerC &c,
	      bool overlap=true, double (*window)( int j, int n )=bartlett );
  /*! Compute gain (absolute value of the transfer function)
      in range \a firstg, \a lastg
      between the two ranges \a firstx, \a lastx and \a firsty, \a lasty.
      The input ranges are divided into chunks of TWO times \a N,
      where \a N is the minimum power of two not less than 
      the number of data points in the range \a firstg, \a lastg.
      The chunks may overlap by half according to \a overlap. 
      Each chunk is windowed through a \a window function.
      The final chunk may exceed the data. In that case it is
      discarded, if it contains less than 1.5*N data elements.
      Otherwise it is filled up with zeros and weighted appropriately.
      To avoid zero padding the data buffer should contain a multiple of
      N (\a overlap = \c true) or 2N (\a overlap = \c false) data points.
      If the input data were sampled with \a delta, then 
      the frequencies are sampled with 1/(2 N delta).
      \a ForwardIterX, \a ForwardIterY, and \a ForwardG
      are forward iterators that point to real numbers. */
template < typename ForwardIterX, typename ForwardIterY,
  typename ForwardIterG >
int gain( ForwardIterX firstx, ForwardIterX lastx,
	  ForwardIterY firsty, ForwardIterY lasty,
	  ForwardIterG firstg, ForwardIterG lastg,
	  bool overlap=true, double (*window)( int j, int n )=bartlett );
template < typename ContainerX, typename ContainerY, typename ContainerG >
int gain( const ContainerX &x, const ContainerY &y, ContainerG &g,
	  bool overlap=true, double (*window)( int j, int n )=bartlett );
  /*! Compute coherence in range \a firstc, \a lastc
      of the two ranges \a firstx, \a lastx and \a firsty, \a lasty.
      The input ranges are divided into chunks of TWO times \a N,
      where \a N is the minimum power of two not less than 
      the number of data points in the range \a firstc, \a lastc.
      The chunks may overlap by half according to \a overlap. 
      Each chunk is windowed through a \a window function.
      The final chunk may exceed the data. In that case it is
      discarded, if it contains less than 1.5*N data elements.
      Otherwise it is filled up with zeros and weighted appropriately.
      To avoid zero padding the data buffer should contain a multiple of
      N (\a overlap = \c true) or 2N (\a overlap = \c false) data points.
      If the input data were sampled with \a delta, then 
      the frequencies are sampled with 1/(2 N delta).
      \a ForwardIterX, \a ForwardIterY, \a ForwardIterC
      are a forward iterators that point to real numbers. 
      \note Means are not subtracted from the data.
  */
template < typename ForwardIterX, typename ForwardIterY, 
  typename ForwardIterC >
int coherence( ForwardIterX firstx, ForwardIterX lastx,
	       ForwardIterY firsty, ForwardIterY lasty,
	       ForwardIterC firstc, ForwardIterC lastc,
	       bool overlap=true, double (*window)( int j, int n )=bartlett );
template < typename ContainerX, typename ContainerY, typename ContainerC >
int coherence( const ContainerX &x, const ContainerY &y, ContainerC &c,
	       bool overlap=true, double (*window)( int j, int n )=bartlett );
  /*! Returns a lower bound of transmitted information based on the coherence
      \f$ \gamma^2 \f$ in the range \a firstc, \a lastc computed by
      \f[ I_{\mathrm{LB}} = -\int_0^{\infty} \log_2(1-\gamma^2) \, df \f]
      The coherence is sampled with \a deltaf Hz.
      \a ForwardIterC is a forward iterator pointing to real numbers. */
template < typename ForwardIterC >
double coherenceInfo( ForwardIterC firstc, ForwardIterC lastc, double deltaf );
template < typename ContainerC >
double coherenceInfo( ContainerC &c, double deltaf );
  /*! Compute the cross power spectrum (squared magnitude of cross spectrum,
      in range \a firstc, \a lastc)
      of the two ranges \a firstx, \a lastx and \a firsty, \a lasty.
      The input ranges are divided into chunks of \a N,
      where \a N is the smallest power of two greater than 
      the number of data points in the range \a firstc, \a lastc.
      The chunks may overlap by half according to \a overlap. 
      Each chunk is windowed through a \a window function.
      The final chunk may exceed the data. In that case it is
      discarded, if it contains less than 0.75*N data elements.
      Otherwise it is filled up with zeros and weighted appropriately.
      To avoid zero padding the data buffer should contain a multiple of
      N/2 (\a overlap = \c true) or N (\a overlap = \c false) data points.
      If the input data were sampled with \a delta, then 
      the frequencies are sampled with 1/(N delta).
      \a ForwardIterX, \a ForwardIterY, \a ForwardIterC
      are a forward iterators that point to real numbers. */
template < typename ForwardIterX, typename ForwardIterY, 
  typename ForwardIterC >
int rCSD( ForwardIterX firstx, ForwardIterX lastx,
	  ForwardIterY firsty, ForwardIterY lasty,
	  ForwardIterC firstc, ForwardIterC lastc,
	  bool overlap=true, double (*window)( int j, int n )=bartlett );
template < typename ContainerX, typename ContainerY, typename ContainerC >
int rCSD( const ContainerX &x, const ContainerY &y, ContainerC &c,
	  bool overlap=true, double (*window)( int j, int n )=bartlett );
  /*! Compute gain (in range \a firstg, \a lastg),
      coherence (in range \a firstc, \a lastc), 
      and power spectrum of the response (in range \a firstyp, \a lastyp)
      between the two ranges \a firstx, \a lastx and \a firsty, \a lasty.
      \a ForwardIterX, \a ForwardIterY, \a ForwardIterG, \a ForwardIterC,
      and \a ForwardIterYP are a forward iterators that point to real numbers. */
template < typename ForwardIterX, typename ForwardIterY, 
  typename ForwardIterG, typename ForwardIterC, typename ForwardIterYP >
int spectra( ForwardIterX firstx, ForwardIterX lastx,
	     ForwardIterY firsty, ForwardIterY lasty,
	     ForwardIterG firstg, ForwardIterG lastg,
	     ForwardIterC firstc, ForwardIterC lastc,
	     ForwardIterYP firstyp, ForwardIterYP lastyp,
	     bool overlap=true, double (*window)( int j, int n )=bartlett );
template < typename ContainerX, typename ContainerY, 
  typename ContainerG, typename ContainerC, typename ContainerYP >
int spectra( const ContainerX &x, const ContainerY &y,
	     ContainerG &g, ContainerC &c, ContainerYP &yp,
	     bool overlap=true, double (*window)( int j, int n )=bartlett );
  /*! Compute gain (in range \a firstg, \a lastg),
      coherence (in range \a firstc, \a lastc), 
      auto- (in range \a firstxp, \a lastxp and \a firstyp, \a lastyp)
      and cross power spectra (magnitude squared of cross spectrum, in range \a firstcp, \a lastcp)
      between the two ranges \a firstx, \a lastx and \a firsty, \a lasty.
      \a ForwardIterX, \a ForwardIterY, \a ForwardIterG, \a ForwardIterC,
      \a ForwardIterCP, \a ForwardIterXP, and \a ForwardIterYP
      are a forward iterators that point to real numbers. */
template < typename ForwardIterX, typename ForwardIterY, 
  typename ForwardIterG, typename ForwardIterC, typename ForwardIterCP,
  typename ForwardIterXP, typename ForwardIterYP >
int spectra( ForwardIterX firstx, ForwardIterX lastx,
	     ForwardIterY firsty, ForwardIterY lasty,
	     ForwardIterG firstg, ForwardIterG lastg,
	     ForwardIterC firstc, ForwardIterC lastc,
	     ForwardIterCP firstcp, ForwardIterCP lastcp,
	     ForwardIterXP firstxp, ForwardIterXP lastxp,
	     ForwardIterYP firstyp, ForwardIterYP lastyp,
	     bool overlap=true, double (*window)( int j, int n )=bartlett );
template < typename ContainerX, typename ContainerY, 
  typename ContainerG, typename ContainerC, typename ContainerCP,
  typename ContainerXP, typename ContainerYP >
int spectra( const ContainerX &x, const ContainerY &y,
	     ContainerG &g, ContainerC &c,
	     ContainerCP &cp, ContainerXP &xp, ContainerYP &yp,
	     bool overlap=true, double (*window)( int j, int n )=bartlett );
  /*! Compute power spectra of the ranges \a firstxp, \a lastxp and \a firstyp, \a lastyp,
      and cross spectrum (in range \a firstcp, \a lastcp as a half-complex sequence)
      between the two ranges \a firstx, \a lastx and \a firsty, \a lasty.
      \a ForwardIterX, \a ForwardIterY, \a ForwardIterXP, and \a ForwardIterYP
      are a forward iterators that point to real numbers.
      \a BidirectIterCP is a bidirectional iterator that points to real numbers.
      The size of the power spectra must be a power of two.
      The size of the cross spectrum must be twice the size of the power spectra.
      \sa coherence() */
template < typename ForwardIterX, typename ForwardIterY, 
  typename BidirectIterCP, typename ForwardIterXP, typename ForwardIterYP >
int crossSpectra( ForwardIterX firstx, ForwardIterX lastx,
		  ForwardIterY firsty, ForwardIterY lasty,
		  BidirectIterCP firstcp, BidirectIterCP lastcp,
		  ForwardIterXP firstxp, ForwardIterXP lastxp,
		  ForwardIterYP firstyp, ForwardIterYP lastyp,
		  bool overlap=true, double (*window)( int j, int n )=bartlett );
template < typename ContainerX, typename ContainerY, 
  typename ContainerCP, typename ContainerXP, typename ContainerYP >
int crossSpectra( const ContainerX &x, const ContainerY &y,
		  ContainerCP &cp, ContainerXP &xp, ContainerYP &yp,
		  bool overlap=true, double (*window)( int j, int n )=bartlett );
  /*! Return in the range \a firstc, \a lastc the coherence computed from
      the cross spectrum, a half-complex sequence in the range \a firstcp, \a lastcp,
      the power spectrum of the input in the range \a firstxp, \a lastxp, and
      the power spectrum of the output in the range \a firstyp, \a lastyp.
      \sa crossSpectra() */
template < typename BidirectIterCP, typename ForwardIterXP,
  typename ForwardIterYP, typename ForwardIterC >
  void coherence( BidirectIterCP firstcp, BidirectIterCP lastcp,
		ForwardIterXP firstxp, ForwardIterXP lastxp,
		ForwardIterYP firstyp, ForwardIterYP lastyp,
		ForwardIterC firstc, ForwardIterC lastc );
template < typename ContainerCP, typename ContainerXP,
  typename ContainerYP, typename ContainerC >
void coherence( const ContainerCP &cp, const ContainerXP &xp,
		const ContainerYP &yp, ContainerC &c );


///// implementation /////////////////////////////////////////////////////////


template < typename RandomAccessIter >
int cFFT( RandomAccessIter first, RandomAccessIter last, int sign )
{
  typedef typename iterator_traits<RandomAccessIter>::value_type ValueType;

  // number of data elements:
  int n = last - first;
  n >>= 1;

  // identity operation?
  if ( n <= 1 ) {
    return 0;
  }

  // make sure that n is a power of 2:
  int logn = 0;
  for ( int k=1; k<n; k <<= 1 )
    logn++;
  if ( n != (1 << logn) )       
    return -1 ; // n is not a power of 2!

  // Goldrader bit-reversal algorithm:
  for ( int i=0, j=0; i<n-1; i++ ) {
    if ( i < j ) {
      swap( *(first+(2*i)), *(first+(2*j)) );
      swap( *(first+(2*i+1)), *(first+(2*j+1)) );
    }
    int m = n >> 1;
    while ( m <= j ) {
      j -= m ;
      m >>= 1;
    }
    j += m;
  }

  // apply fft recursion:
  int dual = 1;
  for ( int bit = 0; bit < logn; bit++ ) {
    ValueType w_real = 1.0;
    ValueType w_imag = 0.0;

    const double theta = 2.0 * sign * M_PI / (2.0 * (double) dual);
    const ValueType s = ::sin( theta );
    const ValueType t = ::sin( theta / 2.0 );
    const ValueType s2 = 2.0 * t * t;

    // a = 0:
    for ( int b = 0; b < n; b += 2 * dual ) {
      RandomAccessIter iter1 = first+(2*b);
      RandomAccessIter iter2 = iter1+(2*dual);

      const ValueType wd_real = *iter2;
      *iter2 = *iter1 - wd_real;
      *iter1 += wd_real;

      ++iter1;
      ++iter2;

      const ValueType wd_imag = *iter2;
      *iter2 = *iter1 - wd_imag;
      *iter1 += wd_imag;
    }

    // a = 1 .. (dual-1):
    for ( int a = 1; a < dual; a++ ) {
      // trignometric recurrence for w-> exp(i theta) w:
      {
	const ValueType tmp_real = w_real - s * w_imag - s2 * w_real;
	const ValueType tmp_imag = w_imag + s * w_real - s2 * w_imag;
	w_real = tmp_real;
	w_imag = tmp_imag;
      }

      for ( int b = 0; b < n; b += 2 * dual ) {
	RandomAccessIter iter1r = first+(2*(b+a));
	RandomAccessIter iter2r = iter1r+(2*dual);
	RandomAccessIter iter1i = iter1r + 1;
	RandomAccessIter iter2i = iter2r + 1;

	const ValueType z1_real = *iter2r;
	const ValueType z1_imag = *iter2i;

	const ValueType wd_real = w_real * z1_real - w_imag * z1_imag;
	const ValueType wd_imag = w_real * z1_imag + w_imag * z1_real;

	*iter2r = *iter1r - wd_real;
	*iter2i = *iter1i - wd_imag;
	*iter1r += wd_real;
	*iter1i += wd_imag;
      }
    }
    dual <<= 1;
  }

  return 0;
}


template < typename Container >
int cFFT( Container &c, int sign )
{
  return cFFT( c.begin(), c.end(), sign );
}


template < typename BidirectIterC, typename ForwardIterP >
void cPower( BidirectIterC firstc, BidirectIterC lastc,
	     ForwardIterP firstp, ForwardIterP lastp )
{
  typedef typename iterator_traits<ForwardIterP>::value_type ValueTypeP;

  if ( firstc == lastc || firstp == lastp )
    return;
  ValueTypeP invn2 = 0.5/(lastc - firstc);
  invn2 *= invn2;
  // f=0 real part:
  *firstp = (*firstc) * (*firstc);
  ++firstc;
  // f=0 imaginary part:
  *firstp += (*firstc) * (*firstc);
  ++firstc;
  *firstp *= invn2;
  ++firstp;
  --lastc;
  while ( firstp != lastp && firstc != lastc ) {
    // f_k real part squared + f_(N-k) imaginary part squared:
    *firstp = (*firstc) * (*firstc) + (*lastc) * (*lastc);
    ++firstc;
    --lastc;
    // f_k imaginary part squared + f_(N-k) real part squared:
    if ( firstp != lastp && firstc != lastc ) {
      *firstp += (*firstc) * (*firstc) + (*lastc) * (*lastc);
      ++firstc;
      --lastc;
    }
    *firstp *= invn2;
    ++firstp;
  }
  while ( firstp != lastp ) {
    *firstp = 0.0;
    ++firstp;
  }
}


template < typename ContainerC, typename ContainerP >
void cPower( ContainerC &c, ContainerP &p )
{
  cPower( c.begin(), c.end(), p.begin(), p.end() );
}


template < typename BidirectIterC, typename ForwardIterM >
void cMagnitude( BidirectIterC firstc, BidirectIterC lastc,
		 ForwardIterM firstm, ForwardIterM lastm )
{
  typedef typename iterator_traits<ForwardIterM>::value_type ValueTypeM;

  if ( firstc == lastc || firstm == lastm )
    return;
  int n = lastc - firstc;
  ValueTypeM m = 0.0;

  // negative frequencies:
  BidirectIterC iterc = firstc + n/2 + 2;
  while ( firstm != lastm && iterc != lastc ) {
    // real part squared:
    m = (*iterc) * (*iterc);
    ++iterc;
    // imaginary part squared:
    m += (*iterc) * (*iterc);
    ++iterc;
    *firstm = ::sqrt( m );
    ++firstm;
  }

  // positive frequencies:
  iterc = firstc + n/2 + 2;
  while ( firstm != lastm && firstc != iterc ) {
    // real part squared:
    m = (*firstc) * (*firstc);
    ++firstc;
    // imaginary part squared:
    m += (*firstc) * (*firstc);
    ++firstc;
    *firstm = ::sqrt( m );
    ++firstm;
  }

  while ( firstm != lastm ) {
    *firstm = 0.0;
    ++firstm;
  }
}


template < typename ContainerC, typename ContainerM >
void cMagnitude( ContainerC &c, ContainerM &m )
{
  cMagnitude( c.begin(), c.end(), m.begin(), m.end() );
}


template < typename BidirectIterC, typename ForwardIterP >
void cPhase( BidirectIterC firstc, BidirectIterC lastc,
	      ForwardIterP firstp, ForwardIterP lastp )
{
  if ( firstc == lastc || firstp == lastp )
    return;
  int n = lastc - firstc;

  // negative frequencies:
  BidirectIterC iterc = firstc + n/2 + 2;
  while ( firstp != lastp && iterc != lastc ) {
    *firstp = ::atan2( *(iterc+1), *iterc );
    iterc+=2;
    ++firstp;
  }

  // positive frequencies:
  iterc = firstc + n/2 + 2;
  while ( firstp != lastp && firstc != iterc ) {
    *firstp = ::atan2( *(firstc+1), *firstc );
    firstc+=2;
    ++firstp;
  }

  while ( firstp != lastp ) {
    *firstp = 0.0;
    ++firstp;
  }
}


template < typename ContainerC, typename ContainerP >
void cPhase( ContainerC &c, ContainerP &p )
{
  cPhase( c.begin(), c.end(), p.begin(), p.end() );
}


template < typename RandomAccessIter >
int rFFT( RandomAccessIter first, RandomAccessIter last )
{
  typedef typename iterator_traits<RandomAccessIter>::value_type ValueType;
  
  // number of data elements:
  int n = last - first;

  // identity operation?
  if ( n <= 1 ) {
    return 0;
  }

  // make sure that n is a power of 2:
  int logn = 0;
  for ( int k=1; k<n; k <<= 1 )
    logn++;
  if ( n != (1 << logn) )       
    return -1 ; // n is not a power of 2!

  // Goldrader bit-reversal algorithm:
  for ( int i=0, j=0; i<n-1; i++ ) {
    if ( i < j ) {
      swap( *(first+i), *(first+j) );
    }
    int m = n >> 1;
    while ( m <= j ) {
      j -= m ;
      m >>= 1;
    }
    j += m;
  }

  // apply fft recursion:
  int p = 1;
  int q = n;
  int p_1;

  for ( int i=1; i<=logn; i++ ) {
    p_1 = p;
    p <<= 1;
    q >>= 1;
    
    // a = 0:
    for ( int b=0; b<q; b++ ) {
      RandomAccessIter iter1 = first+(b*p);
      RandomAccessIter iter2 = iter1 + p_1;
      ValueType tmp = *iter1;
      *iter1 += *iter2;
      *iter2 = tmp - *iter2;
    }
    
    // a = 1 ... p_{i-1}/2 - 1
    {
      ValueType w_real = 1.0;
      ValueType w_imag = 0.0;
      
      const double theta = - 2.0 * M_PI / p;
      const ValueType s = ::sin( theta );
      const ValueType t = ::sin( theta / 2.0 );
      const ValueType s2 = 2.0 * t * t;
      
      for ( int a=1; a<(p_1)/2; a++ ) {
	// trignometric recurrence for w-> exp(i theta) w 
	
	{
	  const ValueType tmp_real = w_real - s * w_imag - s2 * w_real;
	  const ValueType tmp_imag = w_imag + s * w_real - s2 * w_imag;
	  w_real = tmp_real;
	  w_imag = tmp_imag;
	}
            
	for ( int b=0; b<q; b++ ) {
	  RandomAccessIter iter = first+(b*p);
	  ValueType z0_real = *(iter + a);
	  ValueType z0_imag = *(iter + (p_1 - a));
	  ValueType z1_real = *(iter + (p_1 + a));
	  ValueType z1_imag = *(iter + (p - a));
	  
	  // t0 = z0 + w * z1
	  ValueType t0_real = z0_real + w_real * z1_real - w_imag * z1_imag;
	  ValueType t0_imag = z0_imag + w_real * z1_imag + w_imag * z1_real;
	  
	  // t1 = z0 - w * z1
	  ValueType t1_real = z0_real - w_real * z1_real + w_imag * z1_imag;
	  ValueType t1_imag = z0_imag - w_real * z1_imag - w_imag * z1_real;
	  
	  *(iter + a) = t0_real;
	  *(iter + (p - a)) = t0_imag;
	  
	  *(iter + (p_1 - a)) = t1_real;
	  *(iter + (p_1 + a)) = -t1_imag;
	}
      }
    }

    if ( p_1 >  1 ) {
      for ( int b = 0; b < q; b++ ) {
	// a = p_{i-1}/2
	*(first+(b*p + p - p_1/2)) *= -1;
      }
    }
  }
  return 0;
}


template < typename Container >
int rFFT( Container &c )
{
  return rFFT( c.begin(), c.end() );
}


template < typename RandomAccessIter >
int hcFFT( RandomAccessIter first, RandomAccessIter last )
{
  typedef typename iterator_traits<RandomAccessIter>::value_type ValueType;

  // number of data elements:
  int n = last - first;

  // identity operation?
  if ( n <= 1 ) {
    return 0;
  }

  // make sure that n is a power of 2:
  int logn = 0;
  for ( int k=1; k<n; k <<= 1 )
    logn++;
  if ( n != (1 << logn) )       
    return -1 ; // n is not a power of 2!

  // apply fft recursion:
  int p = n;
  int q = 1 ; 
  int p_1 = n/2 ;

  for ( int i = 1; i <= logn; i++ ) {

    // a = 0:
    for ( int b = 0; b < q; b++ ) {
      RandomAccessIter iter1 = first+(b*p);
      RandomAccessIter iter2 = iter1 + p_1;
      ValueType tmp = *iter1;
      *iter1 += *iter2;
      *iter2 = tmp - *iter2;
    }
    
    // a = 1 ... p_{i-1}/2 - 1:
    {
      ValueType w_real = 1.0;
      ValueType w_imag = 0.0;
      
      const ValueType theta = 2.0 * M_PI / p;
      const ValueType s = ::sin( theta );
      const ValueType t = ::sin( theta / 2.0 );
      const ValueType s2 = 2.0 * t * t;
      
      for ( int a = 1; a < (p_1)/2; a++ ) {
	// trignometric recurrence for w-> exp(i theta) w:
	{
	  const ValueType tmp_real = w_real - s * w_imag - s2 * w_real;
	  const ValueType tmp_imag = w_imag + s * w_real - s2 * w_imag;
	  w_real = tmp_real;
	  w_imag = tmp_imag;
	}
	
	for ( int b = 0; b < q; b++ ) {
	  RandomAccessIter iter = first+(b*p);
	  ValueType z0_real = *(iter + a);
	  ValueType z0_imag = *(iter + (p - a));
	  ValueType z1_real = *(iter + (p_1 - a));
	  ValueType z1_imag = -(*(iter + (p_1 + a)));
	  
	  // t0 = z0 + z1:
	  ValueType t0_real = z0_real + z1_real;
	  ValueType t0_imag = z0_imag + z1_imag;
	  
	  // t1 = (z0 - z1):
	  ValueType t1_real = z0_real -  z1_real;
	  ValueType t1_imag = z0_imag -  z1_imag;

	  *(iter + a) = t0_real;
	  *(iter + (p_1 - a)) = t0_imag;
	  
	  *(iter + (p_1 + a)) = (w_real * t1_real - w_imag * t1_imag);
	  *(iter + (p - a)) = (w_real * t1_imag + w_imag * t1_real);
	}
      }
    }
    
    if ( p_1 >  1 ) {
      for ( int b = 0; b < q; b++ ) {
	RandomAccessIter iter = first+(b*p);
	*(iter+(p_1/2)) *= 2;
	*(iter+(p_1 + p_1/2)) *= -2;
      }
    }

    p_1 >>= 1;
    p >>= 1;
    q <<= 1;

  }

  // Goldrader bit-reversal algorithm:
  for ( int i=0, j=0; i<n-1; i++ ) {
    if ( i < j ) {
      swap( *(first+i), *(first+j) );
    }
    int m = n >> 1;
    while ( m <= j ) {
      j -= m ;
      m >>= 1;
    }
    j += m;
  }

  return 0;
}


template < typename Container >
int hcFFT( Container &c )
{
  return hcFFT( c.begin(), c.end() );
}


template < typename BidirectIterHC, typename ForwardIterP >
void hcPower( BidirectIterHC firsthc, BidirectIterHC lasthc,
	      ForwardIterP firstp, ForwardIterP lastp )
{
  if ( firsthc == lasthc || firstp == lastp )
    return;
  *firstp = (*firsthc) * (*firsthc) * 0.5;
  ++firsthc;
  --lasthc;
  ++firstp;
  while ( firstp != lastp && firsthc != lasthc ) {
    *firstp = (*firsthc) * (*firsthc) + (*lasthc) * (*lasthc);
    ++firsthc;
    --lasthc;
    ++firstp;
  }
  if ( firstp != lastp ) {
    *firstp = (*firsthc) * (*firsthc) * 0.5;
    ++firstp;
  }
  while ( firstp != lastp ) {
    *firstp = 0.0;
    ++firstp;
  }
}


template < typename ContainerHC, typename ContainerP >
void hcPower( const ContainerHC &hc, ContainerP &p )
{
  hcPower( hc.begin(), hc.end(), p.begin(), p.end() );
}


template < typename BidirectIterHC, typename ForwardIterM >
void hcMagnitude( BidirectIterHC firsthc, BidirectIterHC lasthc,
		  ForwardIterM firstm, ForwardIterM lastm )
{
  typedef typename iterator_traits<ForwardIterM>::value_type ValueTypeM;

  if ( firsthc == lasthc || firstm == lastm )
    return;
  *firstm = ::sqrt( (*firsthc) * (*firsthc) );
  ++firsthc;
  --lasthc;
  ++firstm;
  while ( firstm != lastm && firsthc != lasthc ) {
    *firstm = ::sqrt( (*firsthc) * (*firsthc) + (*lasthc) * (*lasthc) );
    ++firsthc;
    --lasthc;
    ++firstm;
  }
  if ( firstm != lastm ) {
    *firstm = ::sqrt( (*firsthc) * (*firsthc) );
    ++firstm;
  }
  while ( firstm != lastm ) {
    *firstm = 0.0;
    ++firstm;
  }
}


template < typename ContainerHC, typename ContainerM >
void hcMagnitude( const ContainerHC &hc, ContainerM &m )
{
  hcMagnitude( hc.begin(), hc.end(), m.begin(), m.end() );
}


template < typename BidirectIterHC, typename ForwardIterP >
void hcPhase( BidirectIterHC firsthc, BidirectIterHC lasthc,
	      ForwardIterP firstp, ForwardIterP lastp )
{
  if ( firsthc == lasthc || firstp == lastp )
    return;
  *firstp = 0.0;
  ++firsthc;
  --lasthc;
  ++firstp;
  while ( firstp != lastp && firsthc != lasthc ) {
    *firstp = ::atan2( *lasthc, *firsthc );
    ++firsthc;
    --lasthc;
    ++firstp;
  }
  while ( firstp != lastp ) {
    *firstp = 0.0;
    ++firstp;
  }
}


template < typename ContainerHC, typename ContainerP >
void hcPhase( const ContainerHC &hc, ContainerP &p )
{
  hcPhase( hc.begin(), hc.end(), p.begin(), p.end() );
}


template < typename BidirectIterHC, typename ForwardIterR >
void hcReal( BidirectIterHC firsthc, BidirectIterHC lasthc,
	     ForwardIterR firstr, ForwardIterR lastr )
{
  if ( firsthc == lasthc || firstr == lastr )
    return;
  *firstr = *firsthc;
  ++firsthc;
  --lasthc;
  ++firstr;
  while ( firstr != lastr && firsthc != lasthc ) {
    *firstr = *firsthc;
    ++firsthc;
    --lasthc;
    ++firstr;
  }
  if ( firstr != lastr ) {
    *firstr = *firsthc;
    ++firstr;
  }
  while ( firstr != lastr ) {
    *firstr = 0.0;
    ++firstr;
  }
}


template < typename ContainerHC, typename ContainerR >
void hcReal( const ContainerHC &hc, ContainerR &r )
{
  hcReal( hc.begin(), hc.end(), r.begin(), r.end() );
}


template < typename BidirectIterHC, typename ForwardIterI >
void hcImaginary( BidirectIterHC firsthc, BidirectIterHC lasthc,
		  ForwardIterI firsti, ForwardIterI lasti )
{
  if ( firsthc == lasthc || firsti == lasti )
    return;
  *firsti = 0.0;
  ++firsthc;
  --lasthc;
  ++firsti;
  while ( firsti != lasti && firsthc != lasthc ) {
    *firsti = *lasthc;
    ++firsthc;
    --lasthc;
    ++firsti;
  }
  while ( firsti != lasti ) {
    *firsti = 0.0;
    ++firsti;
  }
}


template < typename ContainerHC, typename ContainerI >
void hcImaginary( const ContainerHC &hc, ContainerI &i )
{
  hcImaginary( hc.begin(), hc.end(), i.begin(), i.end() );
}


template < typename ForwardIterX, typename ForwardIterP >
int rPSD( ForwardIterX firstx, ForwardIterX lastx,
	  ForwardIterP firstp, ForwardIterP lastp,
	  bool overlap, double (*window)( int j, int n ) )
{
  typedef typename iterator_traits<ForwardIterX>::value_type ValueTypeX;
  typedef typename iterator_traits<ForwardIterP>::value_type ValueTypeP;

  int np = lastp - firstp;  // size of power spectrum
  int nw = np*2;  // window size
  if ( nw <= 2 )
    return -1;

  // make sure that nw is a power of 2:
  nw = nextPowerOfTwo( nw );

  // clear psd:
  for ( ForwardIterP iterp=firstp; iterp != lastp; ++iterp )
    *iterp = 0.0;

  // normalization factor:
  ValueTypeP wwn = 0.0;
  for ( int k=0; k<nw; ++k ) {
    ValueTypeP w = window( k, nw );
    wwn += w*w;
  }
  ValueTypeP norm = 2.0/wwn/nw;

  // cycle through the data:
  int c = 0;
  ForwardIterX iterx = firstx;
  ForwardIterX iterx2 = iterx;
  while ( iterx != lastx && iterx2 != lastx ) {

    // copy chunk of data into buffer and apply window:
    ValueTypeX buffer[nw];
    int k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && iterx != lastx; ++k, ++iterx )
	buffer[k] = *iterx * window( k, nw );
      for ( iterx2=iterx; k<nw && iterx2 != lastx; ++k, ++iterx2 )
	buffer[k] = *iterx2 * window( k, nw );
    }
    else {
      for ( ; k<nw && iterx != lastx; ++k, ++iterx )
	buffer[k] = *iterx * window( k, nw );
    }
    if ( c >= 1 && k < 3*nw/4 )
      break;
    ValueTypeP normfac = norm;
    if ( k < nw ) {
      ValueTypeP wwz = 0.0;
      for ( ; k<nw; k++ ) {
	buffer[k] = 0.0;
	ValueTypeP w = window( k, nw );
	wwz += w*w;
      }
      normfac *= wwn / ( wwn - wwz );
    }

    // fourier transform:
    rFFT( buffer, buffer+nw );

    // add power to psd:
    c++;
    // first element:
    ForwardIterP iterp=firstp;
    ValueTypeP power = buffer[0] * buffer[0];
    *iterp += ( 0.5*power*normfac - *iterp ) / c;
    ++iterp;
    // remaining elements:
    for ( int k=1; iterp != lastp; ++iterp, ++k ) {
      power = buffer[k] * buffer[k] + buffer[nw-k] * buffer[nw-k];
      *iterp += ( power*normfac - *iterp ) / c;
    }

  }

  // last element:
  if ( np == nw/2 )
    *(firstp+nw/2) *= 0.25;

  return 0;
}


template < typename ContainerX, typename ContainerP >
int rPSD( const ContainerX &x, ContainerP &p,
	  bool overlap, double (*window)( int j, int n ) )
{
  return rPSD( x.begin(), x.end(), p.begin(), p.end(),
	       overlap, window );
}


template < typename ForwardIterX, typename ForwardIterY,
  typename BidirectIterH >
int transfer( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty,
	      BidirectIterH firsth, BidirectIterH lasth,
	      bool overlap, double (*window)( int j, int n ) )
{
  typedef typename iterator_traits<ForwardIterX>::value_type ValueTypeX;
  typedef typename iterator_traits<ForwardIterY>::value_type ValueTypeY;
  typedef typename iterator_traits<BidirectIterH>::value_type ValueTypeH;

  // clear transfer:
  for ( BidirectIterH iterh=firsth; iterh != lasth; ++iterh )
    *iterh = 0.0;

  // check input ranges:
  if ( lastx - firstx != lasty - firsty )
    return -2;

  // number of points for fft window:
  int nw = lasth - firsth;

  // make sure that nw is a power of 2:
  if ( nw != nextPowerOfTwo( nw ) )
    return -3;

  int np = nw/2;
  if ( np <= 1 )
    return -1;

  // working buffers:
  ValueTypeH re[np];
  ValueTypeH im[np];
  for ( int k=0; k<np; ++k ) {
    re[k] = 0.0;
    im[k] = 0.0;
  }

  // normalization factor:
  ValueTypeH wwn = 0.0;
  for ( int k=0; k<nw; ++k ) {
    ValueTypeH w = window( k, nw );
    wwn += w*w;
  }

  // cycle through the data:
  int c = 0;
  ForwardIterX iterx = firstx;
  ForwardIterX iterx2 = iterx;
  ForwardIterY itery = firsty;
  while ( iterx != lastx && iterx2 != lastx ) {

    // copy chunk of x data into buffer and apply window:
    ValueTypeX bufferx[nw];
    int k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && iterx != lastx; ++k, ++iterx )
	bufferx[k] = *iterx * window( k, nw );
      for ( iterx2=iterx; k<nw && iterx2 != lastx; ++k, ++iterx2 )
	bufferx[k] = *iterx2 * window( k, nw );
    }
    else {
      for ( ; k<nw && iterx != lastx; ++k, ++iterx )
	bufferx[k] = *iterx * window( k, nw );
    }
    if ( c >= 1 && k < 3*nw/4 )
      break;
    for ( ; k<nw; k++ )
      bufferx[k] = 0.0;

    // fourier transform x data:
    rFFT( bufferx, bufferx+nw );

    // copy chunk of y data into buffer and apply window:
    ValueTypeY buffery[nw];
    k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && itery != lasty; ++k, ++itery )
	buffery[k] = *itery * window( k, nw );
      ForwardIterY itery2 = itery;
      for ( ; k<nw && itery2 != lasty; ++k, ++itery2 )
	buffery[k] = *itery2 * window( k, nw );
    }
    else {
      for ( ; k<nw && itery != lasty; ++k, ++itery )
	buffery[k] = *itery * window( k, nw );
    }
    ValueTypeH normfac = 2.0;
    if ( k < nw ) {
      ValueTypeH wwz = 0.0;
      for ( ; k<nw; k++ ) {
	buffery[k] = 0.0;
	ValueTypeH w = window( k, nw );
	wwz += w*w;
      }
      normfac = wwn / ( wwn - wwz );
    }

    // fourier transform y data:
    rFFT( buffery, buffery+nw );

    // compute spectra:
    c++;
    // first element xx:
    BidirectIterH iterxp = firsth;
    ValueTypeH powerxp = bufferx[0] * bufferx[0];
    *iterxp += ( powerxp*normfac - *iterxp ) / c;
    ++iterxp;
    // first element Re xy:
    ValueTypeH* iterre = re;
    ValueTypeH powerre = bufferx[0] * buffery[0];
    *iterre += ( powerre*normfac - *iterre ) / c;
    ++iterre;
    // first element Im xy:
    ValueTypeH* iterim = im;
    ValueTypeH powerim = 0.0;
    *iterim += ( powerim*normfac - *iterim ) / c;
    ++iterim;
    // remaining elements:
    for ( int k=1; k<np; ++k ) {
      ValueTypeX xr = bufferx[k];
      ValueTypeX xi = bufferx[nw-k];
      ValueTypeY yr = buffery[k];
      ValueTypeY yi = buffery[nw-k];
      powerxp = xr*xr + xi*xi;
      *iterxp += ( powerxp*normfac - *iterxp ) / c;
      ++iterxp;
      powerre = xr*yr + xi*yi;
      *iterre += ( powerre*normfac - *iterre ) / c;
      ++iterre;
      powerim = xr*yi - xi*yr;
      *iterim += ( powerim*normfac - *iterim ) / c;
      ++iterim;
    }

  }

  // compute transfer function:
  BidirectIterH iterhre = firsth;
  BidirectIterH iterhim = lasth;
  ValueTypeH* iterre = re;
  ValueTypeH* iterim = im;
  *iterhre = (*iterre) / (*iterhre);
  ++iterhre;
  --iterhim;
  ++iterim;
  ++iterre;
  for ( int k=1; k<np; ++k ) {
    *iterhim = (*iterim) / (*iterhre);
    *iterhre = (*iterre) / (*iterhre);
    ++iterhre;
    --iterhim;
    ++iterim;
    ++iterre;
  }
  *iterhre = (*iterre) / (*iterhre);

  return 0;
}


template < typename ContainerX, typename ContainerY, typename ContainerH >
int transfer( const ContainerX &x, const ContainerY &y, ContainerH &h,
	      bool overlap, double (*window)( int j, int n ) )
{
  return transfer( x.begin(), x.end(),
		   y.begin(), y.end(),
		   h.begin(), h.end(),
		   overlap, window );
}


template < typename ForwardIterX, typename ForwardIterY,
  typename BidirectIterH, typename BidirectIterC >
int transfer( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty,
	      BidirectIterH firsth, BidirectIterH lasth,
	      BidirectIterC firstc, BidirectIterC lastc,
	      bool overlap, double (*window)( int j, int n ) )
{
  typedef typename iterator_traits<ForwardIterX>::value_type ValueTypeX;
  typedef typename iterator_traits<ForwardIterY>::value_type ValueTypeY;
  typedef typename iterator_traits<BidirectIterH>::value_type ValueTypeH;
  typedef typename iterator_traits<BidirectIterC>::value_type ValueTypeC;

  // clear transfer:
  for ( BidirectIterH iterh=firsth; iterh != lasth; ++iterh )
    *iterh = 0.0;

  // clear coherence:
  for ( BidirectIterC iterc=firstc; iterc != lastc; ++iterc )
    *iterc = 0.0;

  // check input ranges:
  if ( lastx - firstx != lasty - firsty )
    return -2;

  // number of points for fft window:
  int nw = lasth - firsth;

  // make sure that nw is a power of 2:
  if ( nw != nextPowerOfTwo( nw ) )
    return -3;

  int np = nw/2;
  if ( np <= 1 )
    return -1;

  // check coherence size:
  if ( lastc - firstc != np )
    return -3;

  // working buffers:
  ValueTypeH re[np];
  ValueTypeH im[np];
  for ( int k=0; k<np; ++k ) {
    re[k] = 0.0;
    im[k] = 0.0;
  }

  // normalization factor:
  ValueTypeC wwn = 0.0;
  for ( int k=0; k<nw; ++k ) {
    ValueTypeC w = window( k, nw );
    wwn += w*w;
  }

  // cycle through the data:
  int c = 0;
  ForwardIterX iterx = firstx;
  ForwardIterX iterx2 = iterx;
  ForwardIterY itery = firsty;
  while ( iterx != lastx && iterx2 != lastx ) {

    // copy chunk of x data into buffer and apply window:
    ValueTypeX bufferx[nw];
    int k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && iterx != lastx; ++k, ++iterx )
	bufferx[k] = *iterx * window( k, nw );
      for ( iterx2=iterx; k<nw && iterx2 != lastx; ++k, ++iterx2 )
	bufferx[k] = *iterx2 * window( k, nw );
    }
    else {
      for ( ; k<nw && iterx != lastx; ++k, ++iterx )
	bufferx[k] = *iterx * window( k, nw );
    }
    if ( c >= 1 && k < 3*nw/4 )
      break;
    for ( ; k<nw; k++ )
      bufferx[k] = 0.0;

    // fourier transform x data:
    rFFT( bufferx, bufferx+nw );

    // copy chunk of y data into buffer and apply window:
    ValueTypeY buffery[nw];
    k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && itery != lasty; ++k, ++itery )
	buffery[k] = *itery * window( k, nw );
      ForwardIterY itery2 = itery;
      for ( ; k<nw && itery2 != lasty; ++k, ++itery2 )
	buffery[k] = *itery2 * window( k, nw );
    }
    else {
      for ( ; k<nw && itery != lasty; ++k, ++itery )
	buffery[k] = *itery * window( k, nw );
    }
    ValueTypeC normfac = 1.0;
    if ( k < nw ) {
      ValueTypeC wwz = 0.0;
      for ( ; k<nw; k++ ) {
	buffery[k] = 0.0;
	ValueTypeC w = window( k, nw );
	wwz += w*w;
      }
      normfac = wwn / ( wwn - wwz );
    }

    // fourier transform y data:
    rFFT( buffery, buffery+nw );

    // compute spectra:
    c++;
    // first element xx:
    BidirectIterH iterxp = firsth;
    ValueTypeH powerxp = bufferx[0] * bufferx[0];
    *iterxp += ( powerxp*normfac - *iterxp ) / c;
    ++iterxp;
    // first element yy:
    BidirectIterC iteryp = firstc;
    ValueTypeC poweryp = buffery[0] * buffery[0];
    *iteryp += ( poweryp*normfac - *iteryp ) / c;
    ++iteryp;
    // first element Re xy:
    ValueTypeH* iterre = re;
    ValueTypeH powerre = bufferx[0] * buffery[0];
    *iterre += ( powerre*normfac - *iterre ) / c;
    ++iterre;
    // first element Im xy:
    ValueTypeH* iterim = im;
    ValueTypeH powerim = 0.0;
    *iterim += ( powerim*normfac - *iterim ) / c;
    ++iterim;
    // remaining elements:
    for ( int k=1; k<np; ++k ) {
      ValueTypeX xr = bufferx[k];
      ValueTypeX xi = bufferx[nw-k];
      ValueTypeY yr = buffery[k];
      ValueTypeY yi = buffery[nw-k];
      powerxp = xr*xr + xi*xi;
      *iterxp += ( powerxp*normfac - *iterxp ) / c;
      ++iterxp;
      poweryp = yr*yr + yi*yi;
      *iteryp += ( poweryp*normfac - *iteryp ) / c;
      ++iteryp;
      powerre = xr*yr + xi*yi;
      *iterre += ( powerre*normfac - *iterre ) / c;
      ++iterre;
      powerim = xr*yi - xi*yr;
      *iterim += ( powerim*normfac - *iterim ) / c;
      ++iterim;
    }

  }

  // compute transfer function and coherence:
  BidirectIterH iterhre = firsth;   // powerx
  BidirectIterH iterhim = lasth;    // nothing
  ValueTypeH* iterre = re;          // re crossspectrum xy
  ValueTypeH* iterim = im;          // im crossspectrum xy
  // first element coherence:
  if ( *iterhre == 0.0 || *firstc == 0.0 ) 
    *firstc = 0.0;
  else 
    *firstc = ( (*iterre) * (*iterre) + (*iterim) * (*iterim) ) / ( (*iterhre) * (*firstc) );
  ++firstc;
  // first element transfer function:
  *iterhre = (*iterre) / (*iterhre);
  ++iterhre;
  --iterhim;
  ++iterim;
  ++iterre;
  for ( int k=1; k<np; ++k ) {
    // coherence:
    if ( *iterhre == 0.0 || *firstc == 0.0 ) 
      *firstc = 0.0;
    else 
      *firstc = ( (*iterre) * (*iterre) + (*iterim) * (*iterim) ) / ( (*iterhre) * (*firstc) );
    ++firstc;
    // transfer function:
    *iterhim = (*iterim) / (*iterhre);
    *iterhre = (*iterre) / (*iterhre);
    ++iterhre;
    --iterhim;
    ++iterim;
    ++iterre;
  }
  *iterhre = (*iterre) / (*iterhre);

  return 0;
}


template < typename ContainerX, typename ContainerY,
  typename ContainerH, typename ContainerC >
int transfer( const ContainerX &x, const ContainerY &y,
	      ContainerH &h, ContainerC &c,
	      bool overlap, double (*window)( int j, int n ) )
{
  return transfer( x.begin(), x.end(),
		   y.begin(), y.end(),
		   h.begin(), h.end(),
		   c.begin(), c.end(),
		   overlap, window );
}


template < typename ForwardIterX, typename ForwardIterY, 
  typename ForwardIterG >
int gain( ForwardIterX firstx, ForwardIterX lastx,
	  ForwardIterY firsty, ForwardIterY lasty,
	  ForwardIterG firstg, ForwardIterG lastg,
	  bool overlap, double (*window)( int j, int n ) )
{
  typedef typename iterator_traits<ForwardIterX>::value_type ValueTypeX;
  typedef typename iterator_traits<ForwardIterY>::value_type ValueTypeY;
  typedef typename iterator_traits<ForwardIterG>::value_type ValueTypeG;

  // clear gain:
  for ( ForwardIterG iterg=firstg; iterg != lastg; ++iterg )
    *iterg = 0.0;

  // check input ranges:
  if ( lastx - firstx != lasty - firsty )
    return -2;

  // number of points for fft window:
  int np = lastg - firstg;
  int nw = np*2;  // window size
  if ( nw <= 2 )
    return -1;

  // make sure that nw is a power of 2:
  nw = nextPowerOfTwo( nw );
  np = nw/2;

  // working buffers:
  ValueTypeG re[ np ];
  ValueTypeG im[ np ];
  for ( int k=0; k<np; ++k ) {
    re[k] = 0.0;
    im[k] = 0.0;
  }

  // normalization factor:
  ValueTypeG wwn = 0.0;
  for ( int k=0; k<nw; ++k ) {
    ValueTypeG w = window( k, nw );
    wwn += w*w;
  }

  // cycle through the data:
  int c = 0;
  ForwardIterX iterx = firstx;
  ForwardIterX iterx2 = iterx;
  ForwardIterY itery = firsty;
  while ( iterx != lastx && iterx2 != lastx ) {

    // copy chunk of x data into buffer and apply window:
    ValueTypeX bufferx[nw];
    int k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && iterx != lastx; ++k, ++iterx )
	bufferx[k] = *iterx * window( k, nw );
      for ( iterx2=iterx; k<nw && iterx2 != lastx; ++k, ++iterx2 )
	bufferx[k] = *iterx2 * window( k, nw );
    }
    else {
      for ( ; k<nw && iterx != lastx; ++k, ++iterx )
	bufferx[k] = *iterx * window( k, nw );
    }
    if ( c >= 1 && k < 3*nw/4 )
      break;
    for ( ; k<nw; k++ )
      bufferx[k] = 0.0;

    // fourier transform x data:
    rFFT( bufferx, bufferx+nw );

    // copy chunk of y data into buffer and apply window:
    ValueTypeY buffery[nw];
    k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && itery != lasty; ++k, ++itery )
	buffery[k] = *itery * window( k, nw );
      ForwardIterY itery2 = itery;
      for ( ; k<nw && itery2 != lasty; ++k, ++itery2 )
	buffery[k] = *itery2 * window( k, nw );
    }
    else {
      for ( ; k<nw && itery != lasty; ++k, ++itery )
	buffery[k] = *itery * window( k, nw );
    }
    ValueTypeG normfac = 1.0;
    if ( k < nw ) {
      ValueTypeG wwz = 0.0;
      for ( ; k<nw; k++ ) {
	buffery[k] = 0.0;
	ValueTypeG w = window( k, nw );
	wwz += w*w;
      }
      normfac = wwn / ( wwn - wwz );
    }

    // fourier transform y data:
    rFFT( buffery, buffery+nw );

    // compute auto- and cross spectra:
    c++;
    // first element xx:
    ForwardIterG iterxp = firstg;
    ValueTypeG powerxp = bufferx[0] * bufferx[0];
    *iterxp += ( powerxp*normfac - *iterxp ) / c;
    ++iterxp;
    // first element Re xy:
    ValueTypeG* iterre = re;
    ValueTypeG powerre = bufferx[0] * buffery[0];
    *iterre += ( powerre*normfac - *iterre ) / c;
    ++iterre;
    // first element Im xy:
    ValueTypeG* iterim = im;
    ValueTypeG powerim = 0.0;
    *iterim += ( powerim*normfac - *iterim ) / c;
    ++iterim;
    // remaining elements:
    for ( int k=1; iterxp != lastg; ++k ) {
      ValueTypeX xr = bufferx[k];
      ValueTypeX xi = bufferx[nw-k];
      ValueTypeY yr = buffery[k];
      ValueTypeY yi = buffery[nw-k];
      powerxp = xr*xr + xi*xi;
      *iterxp += ( powerxp*normfac - *iterxp ) / c;
      ++iterxp;
      powerre = xr*yr + xi*yi;
      *iterre += ( powerre*normfac - *iterre ) / c;
      ++iterre;
      powerim = xr*yi - xi*yr;
      *iterim += ( powerim*normfac - *iterim ) / c;
      ++iterim;
    }

  }

  // compute gain:
  ValueTypeG* iterre = re;
  ValueTypeG* iterim = im;
  while ( firstg != lastg ) {
    ValueTypeG gr = (*iterre) / (*firstg);
    ValueTypeG gi = (*iterim) / (*firstg);
    *firstg = ::sqrt( gr*gr + gi*gi );
    ++firstg;
    ++iterre;
    ++iterim;
  }

  return 0;
}


template < typename ContainerX, typename ContainerY, typename ContainerG >
int gain( const ContainerX &x, const ContainerY &y, ContainerG &g,
	  bool overlap, double (*window)( int j, int n ) )
{
  return gain( x.begin(), x.end(),
	       y.begin(), y.end(),
	       g.begin(), g.end(),
	       overlap, window );
}


template < typename ForwardIterX, typename ForwardIterY,
  typename ForwardIterC >
int coherence( ForwardIterX firstx, ForwardIterX lastx,
	       ForwardIterY firsty, ForwardIterY lasty,
	       ForwardIterC firstc, ForwardIterC lastc,
	       bool overlap, double (*window)( int j, int n ) )
{
  typedef typename iterator_traits<ForwardIterX>::value_type ValueTypeX;
  typedef typename iterator_traits<ForwardIterY>::value_type ValueTypeY;
  typedef typename iterator_traits<ForwardIterC>::value_type ValueTypeC;

  // clear coherence:
  for ( ForwardIterC iterc=firstc; iterc != lastc; ++iterc )
    *iterc = 0.0;

  // check input ranges:
  if ( lastx - firstx != lasty - firsty )
    return -2;

  // number of points for fft window:
  int np = lastc - firstc;
  int nw = np*2;  // window size
  if ( nw <= 2 )
    return -1;

  // make sure that nw is a power of 2:
  nw = nextPowerOfTwo( nw );
  np = nw/2;

  // working buffers:
  ValueTypeC xp[ np ];
  ValueTypeC yp[ np ];
  ValueTypeC cp[ np ];
  for ( int k=0; k<np; ++k ) {
    xp[k] = 0.0;
    yp[k] = 0.0;
    cp[k] = 0.0;
  }

  // normalization factor:
  ValueTypeC wwn = 0.0;
  for ( int k=0; k<nw; ++k ) {
    ValueTypeC w = window( k, nw );
    wwn += w*w;
  }

  // cycle through the data:
  int c = 0;
  ForwardIterX iterx = firstx;
  ForwardIterX iterx2 = iterx;
  ForwardIterY itery = firsty;
  while ( iterx != lastx && iterx2 != lastx ) {

    // copy chunk of x data into buffer and apply window:
    ValueTypeX bufferx[nw];
    int k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && iterx != lastx; ++k, ++iterx )
	bufferx[k] = *iterx * window( k, nw );
      for ( iterx2=iterx; k<nw && iterx2 != lastx; ++k, ++iterx2 )
	bufferx[k] = *iterx2 * window( k, nw );
    }
    else {
      for ( ; k<nw && iterx != lastx; ++k, ++iterx )
	bufferx[k] = *iterx * window( k, nw );
    }
    if ( c >= 1 && k < 3*nw/4 )
      break;
    for ( ; k<nw; k++ )
      bufferx[k] = 0.0;

    // fourier transform x data:
    rFFT( bufferx, bufferx+nw );

    // copy chunk of y data into buffer and apply window:
    ValueTypeY buffery[nw];
    k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && itery != lasty; ++k, ++itery )
	buffery[k] = *itery * window( k, nw );
      ForwardIterY itery2 = itery;
      for ( ; k<nw && itery2 != lasty; ++k, ++itery2 )
	buffery[k] = *itery2 * window( k, nw );
    }
    else {
      for ( ; k<nw && itery != lasty; ++k, ++itery )
	buffery[k] = *itery * window( k, nw );
    }
    ValueTypeC normfac = 1.0;
    if ( k < nw ) {
      ValueTypeC wwz = 0.0;
      for ( ; k<nw; k++ ) {
	buffery[k] = 0.0;
	ValueTypeC w = window( k, nw );
	wwz += w*w;
      }
      normfac = wwn / ( wwn - wwz );
    }

    // fourier transform y data:
    rFFT( buffery, buffery+nw );

    // compute auto- and cross spectra:
    c++;
    // first element xx:
    ValueTypeC* iterxp = xp;
    ValueTypeC powerxp = bufferx[0] * bufferx[0];
    *iterxp += ( powerxp*normfac - *iterxp ) / c;
    ++iterxp;
    // first element yy:
    ValueTypeC* iteryp = yp;
    ValueTypeC poweryp = buffery[0] * buffery[0];
    *iteryp += ( poweryp*normfac - *iteryp ) / c;
    ++iteryp;
    // first element Re xy:
    ForwardIterC iterc = firstc;
    ValueTypeC powerc = bufferx[0] * buffery[0];
    *iterc += ( powerc*normfac - *iterc ) / c;
    ++iterc;
    // first element Im xy:
    ValueTypeC* itercp = cp;
    ValueTypeC powercp = 0.0;
    *itercp += ( powercp*normfac - *itercp ) / c;
    ++itercp;
    // remaining elements:
    for ( int k=1; iterc != lastc; ++k ) {
      ValueTypeX xr = bufferx[k];
      ValueTypeX xi = bufferx[nw-k];
      ValueTypeY yr = buffery[k];
      ValueTypeY yi = buffery[nw-k];
      powerxp = xr*xr + xi*xi;
      *iterxp += ( powerxp*normfac - *iterxp ) / c;
      ++iterxp;
      poweryp = yr*yr + yi*yi;
      *iteryp += ( poweryp*normfac - *iteryp ) / c;
      ++iteryp;
      powerc = xr*yr + xi*yi;
      *iterc += ( powerc*normfac - *iterc ) / c;
      ++iterc;
      powercp = xr*yi - xi*yr;
      *itercp += ( powercp*normfac - *itercp ) / c;
      ++itercp;
    }

  }

  // compute auto, cross spectra, and coherence:
  ValueTypeC* firstxp = xp;
  ValueTypeC* firstyp = yp;
  ValueTypeC* firstcp = cp;
  while ( firstc != lastc ) {
    *firstcp = (*firstc) * (*firstc) + (*firstcp) * (*firstcp);
    if ( *firstxp == 0.0 || *firstyp == 0.0 ) 
      *firstc = 0.0;
    else 
      *firstc = (*firstcp) / ( (*firstxp) * (*firstyp) );
    ++firstxp;
    ++firstyp;
    ++firstc;
    ++firstcp;
  }

  return 0;
}


template < typename ContainerX, typename ContainerY, typename ContainerC >
int coherence( const ContainerX &x, const ContainerY &y, ContainerC &c,
	       bool overlap, double (*window)( int j, int n ) )
{
  return coherence( x.begin(), x.end(),
		    y.begin(), y.end(),
		    c.begin(), c.end(),
		    overlap, window );
}


template < typename ForwardIterC >
double coherenceInfo( ForwardIterC firstc, ForwardIterC lastc, double deltaf )
{
  double sum = 0;
  while ( firstc != lastc ) {
    sum += ::log( 1.0 - (*firstc) );
    ++firstc;
  }
  return -sum*deltaf/::log( 2.0 );
}


template < typename ContainerC >
double coherenceInfo( ContainerC &c, double deltaf )
{
  return coherenceInfo( c.begin(), c.end(), deltaf );
}


template < typename ForwardIterX, typename ForwardIterY,
  typename ForwardIterC >
int rCSD( ForwardIterX firstx, ForwardIterX lastx,
	  ForwardIterY firsty, ForwardIterY lasty,
	  ForwardIterC firstc, ForwardIterC lastc,
	  bool overlap, double (*window)( int j, int n ) )
{
  typedef typename iterator_traits<ForwardIterX>::value_type ValueTypeX;
  typedef typename iterator_traits<ForwardIterY>::value_type ValueTypeY;
  typedef typename iterator_traits<ForwardIterC>::value_type ValueTypeC;

  // clear cross spectrum:
  for ( ForwardIterC iterc=firstc; iterc != lastc; ++iterc )
    *iterc = 0.0;

  // check input ranges:
  if ( lastx - firstx != lasty - firsty )
    return -2;

  // number of points for fft window:
  int nw = lastc - firstc;

  // make sure that nw is a power of 2:
  if ( nw != nextPowerOfTwo( nw ) )
    return -3;
  if ( nw <= 3 )
    return -1;

  // working buffer:
  ValueTypeC cp[nw];
  for ( int k=0; k<nw; ++k )
    cp[k] = 0.0;

  // normalization factor:
  nw *= 2;
  ValueTypeC wwn = 0.0;
  for ( int k=0; k<nw; ++k ) {
    ValueTypeC w = window( k, nw );
    wwn += w*w;
  }
  ValueTypeC norm = 2.0/wwn/nw;

  // cycle through the data:
  int c = 0;
  ForwardIterX iterx = firstx;
  ForwardIterX iterx2 = iterx;
  ForwardIterY itery = firsty;
  while ( iterx != lastx && iterx2 != lastx ) {

    // copy chunk of x data into buffer and apply window:
    ValueTypeX bufferx[nw];
    int k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && iterx != lastx; ++k, ++iterx )
	bufferx[k] = *iterx * window( k, nw );
      for ( iterx2=iterx; k<nw && iterx2 != lastx; ++k, ++iterx2 )
	bufferx[k] = *iterx2 * window( k, nw );
    }
    else {
      for ( ; k<nw && iterx != lastx; ++k, ++iterx )
	bufferx[k] = *iterx * window( k, nw );
    }
    if ( c >= 1 && k < 3*nw/4 )
      break;
    for ( ; k<nw; k++ )
      bufferx[k] = 0.0;

    // fourier transform x data:
    rFFT( bufferx, bufferx+nw );

    // copy chunk of y data into buffer and apply window:
    ValueTypeY buffery[nw];
    k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && itery != lasty; ++k, ++itery )
	buffery[k] = *itery * window( k, nw );
      ForwardIterY itery2 = itery;
      for ( ; k<nw && itery2 != lasty; ++k, ++itery2 )
	buffery[k] = *itery2 * window( k, nw );
    }
    else {
      for ( ; k<nw && itery != lasty; ++k, ++itery )
	buffery[k] = *itery * window( k, nw );
    }
    ValueTypeC normfac = norm;
    if ( k < nw ) {
      ValueTypeC wwz = 0.0;
      for ( ; k<nw; k++ ) {
	buffery[k] = 0.0;
	ValueTypeC w = window( k, nw );
	wwz += w*w;
      }
      normfac *= wwn / ( wwn - wwz );
    }

    // fourier transform y data:
    rFFT( buffery, buffery+nw );

    // compute spectra:
    c++;
    // first element Re xy:
    ForwardIterC iterc = firstc;
    ValueTypeC powerc = bufferx[0] * buffery[0];
    *iterc += ( 0.5*powerc*normfac - *iterc ) / c;
    ++iterc;
    // first element Im xy:
    ValueTypeC* itercp = cp;
    ValueTypeC powercp = 0.0;
    *itercp += ( 0.5*powercp*normfac - *itercp ) / c;
    ++itercp;
    // remaining elements:
    for ( int k=1; iterc != lastc; ++k ) {
      ValueTypeX xr = bufferx[k];
      ValueTypeX xi = bufferx[nw-k];
      ValueTypeY yr = buffery[k];
      ValueTypeY yi = buffery[nw-k];
      powerc = xr*yr + xi*yi;
      *iterc += ( powerc*normfac - *iterc ) / c;
      ++iterc;
      powercp = xr*yi - xi*yr;
      *itercp += ( powercp*normfac - *itercp ) / c;
      ++itercp;
    }

  }

  // compute cross spectrum:
  ValueTypeC* firstcp = cp;
  while ( firstc != lastc ) {
    *firstc = (*firstc) * (*firstc) + (*firstcp) * (*firstcp);
    ++firstc;
    ++firstcp;
  }
  --firstc;
  *firstc *= 0.25;

  return 0;
}


template < typename ContainerX, typename ContainerY, typename ContainerC >
int rCSD( const ContainerX &x, const ContainerY &y, ContainerC &c,
	  bool overlap, double (*window)( int j, int n ) )
{
  return rCSD( x.begin(), x.end(),
	       y.begin(), y.end(),
	       c.begin(), c.end(),
	       overlap, window );
}


template < typename ForwardIterX, typename ForwardIterY, 
  typename ForwardIterG, typename ForwardIterC, typename ForwardIterYP >
int spectra( ForwardIterX firstx, ForwardIterX lastx,
	     ForwardIterY firsty, ForwardIterY lasty,
	     ForwardIterG firstg, ForwardIterG lastg,
	     ForwardIterC firstc, ForwardIterC lastc,
	     ForwardIterYP firstyp, ForwardIterYP lastyp,
	     bool overlap, double (*window)( int j, int n ) )
{
  typedef typename iterator_traits<ForwardIterX>::value_type ValueTypeX;
  typedef typename iterator_traits<ForwardIterY>::value_type ValueTypeY;
  typedef typename iterator_traits<ForwardIterYP>::value_type ValueTypeYP;
  typedef typename iterator_traits<ForwardIterG>::value_type ValueTypeG;
  typedef typename iterator_traits<ForwardIterC>::value_type ValueTypeC;

  // clear gain, coherence, and auto spectra:
  for ( ForwardIterYP iteryp=firstyp; iteryp != lastyp; ++iteryp )
    *iteryp = 0.0;
  for ( ForwardIterG iterg=firstg; iterg != lastg; ++iterg )
    *iterg = 0.0;
  for ( ForwardIterC iterc=firstc; iterc != lastc; ++iterc )
    *iterc = 0.0;

  // check input ranges:
  if ( lastx - firstx != lasty - firsty )
    return -2;

  // number of points for fft window:
  int np = lastg - firstg;
  if ( lastc - firstc != np ||
       lastyp - firstyp != np  )
    return -3;

  int nw = np*2;  // window size
  if ( nw <= 2 )
    return -1;

  // make sure that nw is a power of 2:
  nw = nextPowerOfTwo( nw );

  // working buffer:
  ValueTypeYP xp[nw/2];
  for ( int k=0; k<nw/2; ++k )
    xp[k] = 0.0;

  // normalization factor:
  ValueTypeYP wwn = 0.0;
  for ( int k=0; k<nw; ++k ) {
    ValueTypeYP w = window( k, nw );
    wwn += w*w;
  }
  ValueTypeYP norm = 2.0/wwn/nw;

  // cycle through the data:
  int c = 0;
  ForwardIterX iterx = firstx;
  ForwardIterX iterx2 = iterx;
  ForwardIterY itery = firsty;
  while ( iterx != lastx && iterx2 != lastx ) {

    // copy chunk of x data into buffer and apply window:
    ValueTypeX bufferx[nw];
    int k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && iterx != lastx; ++k, ++iterx )
	bufferx[k] = *iterx * window( k, nw );
      for ( iterx2=iterx; k<nw && iterx2 != lastx; ++k, ++iterx2 )
	bufferx[k] = *iterx2 * window( k, nw );
    }
    else {
      for ( ; k<nw && iterx != lastx; ++k, ++iterx )
	bufferx[k] = *iterx * window( k, nw );
    }
    if ( c >= 1 && k < 3*nw/4 )
      break;
    for ( ; k<nw; k++ )
      bufferx[k] = 0.0;

    // fourier transform x data:
    rFFT( bufferx, bufferx+nw );

    // copy chunk of y data into buffer and apply window:
    ValueTypeY buffery[nw];
    k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && itery != lasty; ++k, ++itery )
	buffery[k] = *itery * window( k, nw );
      ForwardIterY itery2 = itery;
      for ( ; k<nw && itery2 != lasty; ++k, ++itery2 )
	buffery[k] = *itery2 * window( k, nw );
    }
    else {
      for ( ; k<nw && itery != lasty; ++k, ++itery )
	buffery[k] = *itery * window( k, nw );
    }
    ValueTypeYP normfac = norm;
    if ( k < nw ) {
      ValueTypeYP wwz = 0.0;
      for ( ; k<nw; k++ ) {
	buffery[k] = 0.0;
	ValueTypeYP w = window( k, nw );
	wwz += w*w;
      }
      normfac *= wwn / ( wwn - wwz );
    }

    // fourier transform y data:
    rFFT( buffery, buffery+nw );

    // compute auto- and cross spectra:
    c++;
    // first element xx:
    ForwardIterYP iterxp = xp;
    ValueTypeYP powerxp = bufferx[0] * bufferx[0];
    *iterxp += ( 0.5*powerxp*normfac - *iterxp ) / c;
    ++iterxp;
    // first element yy:
    ForwardIterYP iteryp = firstyp;
    ValueTypeYP poweryp = buffery[0] * buffery[0];
    *iteryp += ( 0.5*poweryp*normfac - *iteryp ) / c;
    ++iteryp;
    // first element Re xy:
    ForwardIterC iterc = firstc;
    ValueTypeC powerc = bufferx[0] * buffery[0];
    *iterc += ( 0.5*powerc*normfac - *iterc ) / c;
    ++iterc;
    // first element Im xy:
    ForwardIterG iterg = firstg;
    ValueTypeG powerg = 0.0;
    *iterg += ( 0.5*powerg*normfac - *iterg ) / c;
    ++iterg;
    // remaining elements:
    for ( int k=1; iterc != lastc; ++k ) {
      ValueTypeX xr = bufferx[k];
      ValueTypeX xi = bufferx[nw-k];
      ValueTypeY yr = buffery[k];
      ValueTypeY yi = buffery[nw-k];
      powerxp = xr*xr + xi*xi;
      *iterxp += ( powerxp*normfac - *iterxp ) / c;
      ++iterxp;
      poweryp = yr*yr + yi*yi;
      *iteryp += ( poweryp*normfac - *iteryp ) / c;
      ++iteryp;
      powerc = xr*yr + xi*yi;
      *iterc += ( powerc*normfac - *iterc ) / c;
      ++iterc;
      powerg = xr*yi - xi*yr;
      *iterg += ( powerg*normfac - *iterg ) / c;
      ++iterg;
    }

  }

  // compute auto spectra and coherence:
  ValueTypeYP* firstxp = xp;
  while ( firstc != lastc ) {
    ValueTypeG gr = (*firstc) / (*firstxp);
    ValueTypeG gi = (*firstg) / (*firstxp);
    ValueTypeG g = ::sqrt( gr*gr + gi*gi );

    ValueTypeC cp = (*firstc) * (*firstc) + (*firstg) * (*firstg);
    if ( *firstxp == 0.0 || *firstyp == 0.0 ) 
      *firstc = 0.0;
    else 
      *firstc = cp / ( (*firstxp) * (*firstyp) );
    *firstg = g;

    ++firstxp;
    ++firstyp;
    ++firstg;
    ++firstc;
  }

  // last element:
  if ( np == nw/2 ) {
    firstyp--;
    *firstyp *= 0.25;
  }

  return 0;
}


template < typename ContainerX, typename ContainerY, 
  typename ContainerG, typename ContainerC, typename ContainerYP >
int spectra( const ContainerX &x, const ContainerY &y,
	     ContainerG &g, ContainerC &c, ContainerYP &yp,
	     bool overlap, double (*window)( int j, int n ) )
{
  return spectra( x.begin(), x.end(),
		  y.begin(), y.end(),
		  g.begin(), g.end(),
		  c.begin(), c.end(),
		  yp.begin(), yp.end(),
		  overlap, window );
}


template < typename ForwardIterX, typename ForwardIterY, 
  typename ForwardIterG, typename ForwardIterC, typename ForwardIterCP,
  typename ForwardIterXP, typename ForwardIterYP >
int spectra( ForwardIterX firstx, ForwardIterX lastx,
	     ForwardIterY firsty, ForwardIterY lasty,
	     ForwardIterG firstg, ForwardIterG lastg,
	     ForwardIterC firstc, ForwardIterC lastc,
	     ForwardIterCP firstcp, ForwardIterCP lastcp,
	     ForwardIterXP firstxp, ForwardIterXP lastxp,
	     ForwardIterYP firstyp, ForwardIterYP lastyp,
	     bool overlap, double (*window)( int j, int n ) )
{
  typedef typename iterator_traits<ForwardIterX>::value_type ValueTypeX;
  typedef typename iterator_traits<ForwardIterY>::value_type ValueTypeY;
  typedef typename iterator_traits<ForwardIterXP>::value_type ValueTypeXP;
  typedef typename iterator_traits<ForwardIterYP>::value_type ValueTypeYP;
  typedef typename iterator_traits<ForwardIterG>::value_type ValueTypeG;
  typedef typename iterator_traits<ForwardIterC>::value_type ValueTypeC;
  typedef typename iterator_traits<ForwardIterCP>::value_type ValueTypeCP;

  // clear gain, coherence, auto- and cross spectra:
  for ( ForwardIterXP iterxp=firstxp; iterxp != lastxp; ++iterxp )
    *iterxp = 0.0;
  for ( ForwardIterYP iteryp=firstyp; iteryp != lastyp; ++iteryp )
    *iteryp = 0.0;
  for ( ForwardIterG iterg=firstg; iterg != lastg; ++iterg )
    *iterg = 0.0;
  for ( ForwardIterC iterc=firstc; iterc != lastc; ++iterc )
    *iterc = 0.0;
  for ( ForwardIterCP itercp=firstcp; itercp != lastcp; ++itercp )
    *itercp = 0.0;

  // check input ranges:
  if ( lastx - firstx != lasty - firsty )
    return -2;

  // number of points for fft window:
  int np = lastg - firstg;
  if ( lastc - firstc != np ||
       lastcp - firstcp != np ||
       lastxp - firstxp != np ||
       lastyp - firstyp != np  )
    return -3;

  int nw = np*2;  // window size
  if ( nw <= 2 )
    return -1;

  // make sure that nw is a power of 2:
  nw = nextPowerOfTwo( nw );

  // normalization factor:
  ValueTypeYP wwn = 0.0;
  for ( int k=0; k<nw; ++k ) {
    ValueTypeYP w = window( k, nw );
    wwn += w*w;
  }
  ValueTypeYP norm = 2.0/wwn/nw;

  // cycle through the data:
  int c = 0;
  ForwardIterX iterx = firstx;
  ForwardIterX iterx2 = iterx;
  ForwardIterY itery = firsty;
  while ( iterx != lastx && iterx2 != lastx ) {

    // copy chunk of x data into buffer and apply window:
    ValueTypeX bufferx[nw];
    int k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && iterx != lastx; ++k, ++iterx )
	bufferx[k] = *iterx * window( k, nw );
      for ( iterx2=iterx; k<nw && iterx2 != lastx; ++k, ++iterx2 )
	bufferx[k] = *iterx2 * window( k, nw );
    }
    else {
      for ( ; k<nw && iterx != lastx; ++k, ++iterx )
	bufferx[k] = *iterx * window( k, nw );
    }
    if ( c >= 1 && k < 3*nw/4 )
      break;
    for ( ; k<nw; k++ )
      bufferx[k] = 0.0;

    // fourier transform x data:
    rFFT( bufferx, bufferx+nw );

    // copy chunk of y data into buffer and apply window:
    ValueTypeY buffery[nw];
    k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && itery != lasty; ++k, ++itery )
	buffery[k] = *itery * window( k, nw );
      ForwardIterY itery2 = itery;
      for ( ; k<nw && itery2 != lasty; ++k, ++itery2 )
	buffery[k] = *itery2 * window( k, nw );
    }
    else {
      for ( ; k<nw && itery != lasty; ++k, ++itery )
	buffery[k] = *itery * window( k, nw );
    }
    ValueTypeYP normfac = norm;
    if ( k < nw ) {
      ValueTypeYP wwz = 0.0;
      for ( ; k<nw; k++ ) {
	buffery[k] = 0.0;
	ValueTypeYP w = window( k, nw );
	wwz += w*w;
      }
      normfac *= wwn / ( wwn - wwz );
    }

    // fourier transform y data:
    rFFT( buffery, buffery+nw );

    // compute auto- and cross spectra:
    c++;
    // first element xx:
    ForwardIterXP iterxp = firstxp;
    ValueTypeXP powerxp = bufferx[0] * bufferx[0];
    *iterxp += ( 0.5*powerxp*normfac - *iterxp ) / c;
    ++iterxp;
    // first element yy:
    ForwardIterYP iteryp = firstyp;
    ValueTypeYP poweryp = buffery[0] * buffery[0];
    *iteryp += ( 0.5*poweryp*normfac - *iteryp ) / c;
    ++iteryp;
    // first element Re xy:
    ForwardIterC iterc = firstc;
    ValueTypeC powerc = bufferx[0] * buffery[0];
    *iterc += ( 0.5*powerc*normfac - *iterc ) / c;
    ++iterc;
    // first element Im xy:
    ForwardIterCP itercp = firstcp;
    ValueTypeCP powercp = 0.0;
    *itercp += ( 0.5*powercp*normfac - *itercp ) / c;
    ++itercp;
    // remaining elements:
    for ( int k=1; iterc != lastc; ++k ) {
      ValueTypeX xr = bufferx[k];
      ValueTypeX xi = bufferx[nw-k];
      ValueTypeY yr = buffery[k];
      ValueTypeY yi = buffery[nw-k];
      powerxp = xr*xr + xi*xi;
      *iterxp += ( powerxp*normfac - *iterxp ) / c;
      ++iterxp;
      poweryp = yr*yr + yi*yi;
      *iteryp += ( poweryp*normfac - *iteryp ) / c;
      ++iteryp;
      powerc = xr*yr + xi*yi;
      *iterc += ( powerc*normfac - *iterc ) / c;
      ++iterc;
      powercp = xr*yi - xi*yr;
      *itercp += ( powercp*normfac - *itercp ) / c;
      ++itercp;
    }

  }

  // compute auto, cross spectra and coherence:
  while ( firstc != lastc ) {
    ValueTypeG gr = (*firstc) / (*firstxp);
    ValueTypeG gi = (*firstcp) / (*firstxp);
    *firstg = ::sqrt( gr*gr + gi*gi );

    *firstcp = (*firstc) * (*firstc) + (*firstcp) * (*firstcp);
    if ( *firstxp == 0.0 || *firstyp == 0.0 ) 
      *firstc = 0.0;
    else 
      *firstc = (*firstcp) / ( (*firstxp) * (*firstyp) );

    ++firstxp;
    ++firstyp;
    ++firstg;
    ++firstc;
    ++firstcp;
  }

  // last element:
  if ( np == nw/2 ) {
    *(firstxp+nw/2) *= 0.25;
    *(firstyp+nw/2) *= 0.25;
    *(firstcp+nw/2) *= 0.25;
  }

  return 0;
}


template < typename ContainerX, typename ContainerY, 
  typename ContainerG, typename ContainerC, typename ContainerCP,
  typename ContainerXP, typename ContainerYP >
int spectra( const ContainerX &x, const ContainerY &y,
	     ContainerG &g, ContainerC &c,
	     ContainerCP &cp, ContainerXP &xp, ContainerYP &yp,
	     bool overlap, double (*window)( int j, int n ) )
{
  return spectra( x.begin(), x.end(),
		  y.begin(), y.end(),
		  g.begin(), g.end(),
		  c.begin(), c.end(),
		  cp.begin(), cp.end(),
		  xp.begin(), xp.end(),
		  yp.begin(), yp.end(),
		  overlap, window );
}


template < typename ForwardIterX, typename ForwardIterY, 
  typename BidirectIterCP, typename ForwardIterXP, typename ForwardIterYP >
int crossSpectra( ForwardIterX firstx, ForwardIterX lastx,
		  ForwardIterY firsty, ForwardIterY lasty,
		  BidirectIterCP firstcp, BidirectIterCP lastcp,
		  ForwardIterXP firstxp, ForwardIterXP lastxp,
		  ForwardIterYP firstyp, ForwardIterYP lastyp,
		  bool overlap, double (*window)( int j, int n ) )
{
  typedef typename iterator_traits<ForwardIterX>::value_type ValueTypeX;
  typedef typename iterator_traits<ForwardIterY>::value_type ValueTypeY;
  typedef typename iterator_traits<ForwardIterXP>::value_type ValueTypeXP;
  typedef typename iterator_traits<ForwardIterYP>::value_type ValueTypeYP;
  typedef typename iterator_traits<BidirectIterCP>::value_type ValueTypeCP;

  // clear auto- and cross spectra:
  for ( ForwardIterXP iterxp=firstxp; iterxp != lastxp; ++iterxp )
    *iterxp = 0.0;
  for ( ForwardIterYP iteryp=firstyp; iteryp != lastyp; ++iteryp )
    *iteryp = 0.0;
  for ( BidirectIterCP itercp=firstcp; itercp != lastcp; ++itercp )
    *itercp = 0.0;

  // check input ranges:
  if ( lastx - firstx != lasty - firsty )
    return -2;

  // number of points for fft window:
  int np = lastxp - firstxp;
  if ( lastyp - firstyp != np  )
    return -3;

  int nw = np*2;  // window size
  if ( nw <= 2 )
    return -1;

  // make sure that nw is a power of 2:
  nw = nextPowerOfTwo( nw );
  np = nw/2;

  if ( lastxp - firstxp != np )
    return -3;
  if ( lastcp - firstcp != nw )
    return -4;

  // normalization factor:
  ValueTypeYP wwn = 0.0;
  for ( int k=0; k<nw; ++k ) {
    ValueTypeYP w = window( k, nw );
    wwn += w*w;
  }
  ValueTypeYP norm = 2.0/wwn/nw;

  // cycle through the data:
  int c = 0;
  ForwardIterX iterx = firstx;
  ForwardIterX iterx2 = iterx;
  ForwardIterY itery = firsty;
  while ( iterx != lastx && iterx2 != lastx ) {

    // copy chunk of x data into buffer and apply window:
    ValueTypeX bufferx[nw];
    int k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && iterx != lastx; ++k, ++iterx )
	bufferx[k] = *iterx * window( k, nw );
      for ( iterx2=iterx; k<nw && iterx2 != lastx; ++k, ++iterx2 )
	bufferx[k] = *iterx2 * window( k, nw );
    }
    else {
      for ( ; k<nw && iterx != lastx; ++k, ++iterx )
	bufferx[k] = *iterx * window( k, nw );
    }
    if ( c >= 1 && k < 3*nw/4 )
      break;
    for ( ; k<nw; k++ )
      bufferx[k] = 0.0;

    // fourier transform x data:
    rFFT( bufferx, bufferx+nw );

    // copy chunk of y data into buffer and apply window:
    ValueTypeY buffery[nw];
    k=0;
    if ( overlap ) {
      for ( ; k<nw/2 && itery != lasty; ++k, ++itery )
	buffery[k] = *itery * window( k, nw );
      ForwardIterY itery2 = itery;
      for ( ; k<nw && itery2 != lasty; ++k, ++itery2 )
	buffery[k] = *itery2 * window( k, nw );
    }
    else {
      for ( ; k<nw && itery != lasty; ++k, ++itery )
	buffery[k] = *itery * window( k, nw );
    }
    ValueTypeYP normfac = norm;
    if ( k < nw ) {
      ValueTypeYP wwz = 0.0;
      for ( ; k<nw; k++ ) {
	buffery[k] = 0.0;
	ValueTypeYP w = window( k, nw );
	wwz += w*w;
      }
      normfac *= wwn / ( wwn - wwz );
    }

    // fourier transform y data:
    rFFT( buffery, buffery+nw );

    // compute auto- and cross spectra:
    c++;
    // first element xx:
    ForwardIterXP iterxp = firstxp;
    ValueTypeXP powerxp = bufferx[0] * bufferx[0];
    *iterxp += ( 0.5*powerxp*normfac - *iterxp ) / c;
    ++iterxp;
    // first element yy:
    ForwardIterYP iteryp = firstyp;
    ValueTypeYP poweryp = buffery[0] * buffery[0];
    *iteryp += ( 0.5*poweryp*normfac - *iteryp ) / c;
    ++iteryp;
    // first element Re xy:
    BidirectIterCP itercpr = firstcp;
    ValueTypeCP powercpr = bufferx[0] * buffery[0];
    *itercpr += ( 0.5*powercpr*normfac - *itercpr ) / c;
    ++itercpr;
    // first element Im xy:
    BidirectIterCP itercpi = lastcp;
    ValueTypeCP powercpi = 0.0;
    --itercpi;
    // remaining elements:
    for ( int k=1; iterxp != lastxp; ++k ) {
      ValueTypeX xr = bufferx[k];
      ValueTypeX xi = bufferx[nw-k];
      ValueTypeY yr = buffery[k];
      ValueTypeY yi = buffery[nw-k];
      powerxp = xr*xr + xi*xi;
      *iterxp += ( powerxp*normfac - *iterxp ) / c;
      ++iterxp;
      poweryp = yr*yr + yi*yi;
      *iteryp += ( poweryp*normfac - *iteryp ) / c;
      ++iteryp;
      powercpr = xr*yr + xi*yi;
      *itercpr += ( powercpr*normfac - *itercpr ) / c;
      ++itercpr;
      powercpi = - xr*yi + xi*yr;
      *itercpi += ( powercpi*normfac - *itercpi ) / c;
      --itercpi;
    }

  }

  // last element:
  *(firstxp+np) *= 0.25;
  *(firstyp+np) *= 0.25;
  *(firstcp+np) *= 0.25;

  return 0;
}


template < typename ContainerX, typename ContainerY, 
  typename ContainerCP, typename ContainerXP, typename ContainerYP >
int crossSpectra( const ContainerX &x, const ContainerY &y,
		  ContainerCP &cp, ContainerXP &xp, ContainerYP &yp,
		  bool overlap, double (*window)( int j, int n ) )
{
  return crossSpectra( x.begin(), x.end(),
		       y.begin(), y.end(),
		       cp.begin(), cp.end(),
		       xp.begin(), xp.end(),
		       yp.begin(), yp.end(),
		       overlap, window );
}


template < typename BidirectIterCP, typename ForwardIterXP,
  typename ForwardIterYP, typename ForwardIterC >
void coherence( BidirectIterCP firstcp, BidirectIterCP lastcp,
		ForwardIterXP firstxp, ForwardIterXP lastxp,
		ForwardIterYP firstyp, ForwardIterYP lastyp,
		ForwardIterC firstc, ForwardIterC lastc )
{
  int n = lastxp - firstxp;
  if ( n <= 0 )
    return;
  if ( lastyp - firstyp != n ||
       lastcp - firstcp != 2*n )
    return;
  if ( firstc == lastc )
    return;
       
  if ( *firstxp == 0.0 || *firstyp == 0.0 ) 
    *firstc = 0.0;
  else 
    *firstc = (*firstcp) * (*firstcp) * 0.5 / *firstxp / *firstyp;
  ++firstcp;
  --lastcp;
  ++firstxp;
  ++firstyp;
  ++firstc;
  while ( firstc != lastc && firstcp != lastcp ) {
    if ( *firstxp == 0.0 || *firstyp == 0.0 ) 
      *firstc = 0.0;
    else 
      *firstc = ( (*firstcp) * (*firstcp) + (*lastcp) * (*lastcp) ) / *firstxp / *firstyp;
    ++firstcp;
    --lastcp;
    ++firstxp;
    ++firstyp;
    ++firstc;
  }
  if ( firstc != lastc ) {
    if ( *firstxp == 0.0 || *firstyp == 0.0 ) 
      *firstc = 0.0;
    else 
      *firstc = (*firstcp) * (*firstcp) * 0.5 / *firstxp / *firstyp;
    ++firstc;
  }

  // fill up:
  while ( firstc != lastc ) {
    *firstc = 0.0;
    ++firstc;
  }
}


template < typename ContainerCP, typename ContainerXP,
  typename ContainerYP, typename ContainerC >
void coherence( const ContainerCP &cp, const ContainerXP &xp,
		const ContainerYP &yp, ContainerC &c )
{
  coherence( cp.begin(), cp.end(),
	     xp.begin(), xp.end(),
	     yp.begin(), yp.end(),
	     c.begin(), c.end() );
}


}; /* namespace relacs */

#endif /* ! _RELACS_SPECTRUM_H_ */

