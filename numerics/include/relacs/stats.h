/*
  stats.h
  

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2008 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _RELACS_STATS_H_
#define _RELACS_STATS_H_ 1

#include <cmath>
#include <vector>
using namespace std;


namespace numerics
{

  /*! Returns the median of the sorted range \a first, \a last.
      \a RandomIter is a random access iterator
      that points to a number. */
template < typename RandomIter >
double median( RandomIter first, RandomIter last );
  /*! The median of the sorted container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
double median( const Container &vec );
  /*! Returns the quantile \a f of the sorted range \a first, \a last.
      \a f is a number ranging from 0 to 1.
      \a RandomIter is a random access iterator
      that points to a number. */
template < typename RandomIter >
double quantile( double f, RandomIter first, RandomIter last );
  /*! The quantile \a f of the sorted container \a vec.
      \a f is a number ranging from 0 to 1.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
double quantile( double f, const Container &vec );
  /*! Replace each element of the sorted range \a first, \a last
      by its rank, including midranking of ties,
      and returns the sum of f^3-f, 
      where f is the number of elements in each tie. */
template < typename ForwardIter >
double rank( ForwardIter first, ForwardIter last );
  /*! Replace each element of the sorted container \a vec
      by its rank, including midranking of ties,
      and returns the sum of f^3-f, 
      where f is the number of elements in each tie. */
template < typename Container >
double rank( Container &vec );

  /*! The minimum value of the range \a first, \a last.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
double min( ForwardIter first, ForwardIter last );
  /*! The minimum value of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
double min( const Container &vec );
  /*! The minimum value of the range \a first, \a last.
      In \a index the index of the element with the minimum value is returned.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
double min( int &index, ForwardIter first, ForwardIter last );
  /*! The minimum value of all elements of the container \a vec.
      In \a index the index of the element with the minimum value is returned.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
double min( int &index, const Container &vec );
  /*! The index of the element with the minimum value of the range \a first, \a last.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
int minIndex( ForwardIter first, ForwardIter last );
  /*! The index of the element with the minimum value of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
int minIndex( const Container &vec );

  /*! The maximum value of the range \a first, \a last.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
double max( ForwardIter first, ForwardIter last );
  /*! The maximum value of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
double max( const Container &vec );
  /*! The maximum value of the range \a first, \a last.
      In \a index the index of the element with the maximum value is returned.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
double max( int &index, ForwardIter first, ForwardIter last );
  /*! The maximum value of all elements of the container \a vec.
      In \a index the index of the element with the maximum value is returned.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
double max( int &index, const Container &vec );
  /*! The index of the element with the maximum value of the range \a first, \a last.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
int maxIndex( ForwardIter first, ForwardIter last );
  /*! The index of the element with the maximum value of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
int maxIndex( const Container &vec );

  /*! The minimum value \a min and maximum value \a max 
      of the range \a first, \a last.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
void minMax( double &min, double &max, ForwardIter first, ForwardIter last );
  /*! The minimum value \a min and maximum value \a max
      of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
void minMax( double &min, double &max, const Container &vec );
  /*! The minimum value \a min and maximum value \a max
      of the range \a first, \a last.
      In \a minindex and \a maxindex the indices of the elements
      with the minimum and maximum value are returned.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
void minMax( double &min, int &minindex, double &max, int &maxindex,
	     ForwardIter first, ForwardIter last );
  /*! The minimum value \a min and maximum value \a max
      of all elements of the container \a vec.
      In \a minindex and \a maxindex the indices of the elements
      with the minimum and maximum value are returned.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
void minMax( double &min, int &minindex, double &max, int &maxindex,
	     const Container &vec );
  /*! The indices \a minindex and \a maxindex of the elements
      with the minimum and maximum value of the range \a first, \a last.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
void minMaxIndex( int &minindex, int &maxindex, ForwardIter first, ForwardIter last );
  /*! The indices \a minindex and \a maxindex of the elements
      with the minimum and the maximum value of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
void minMaxIndex( int &minindex, int &maxindex, const Container &vec );

  /*! The minimum absolute value of the range \a first, \a last.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
double minAbs( ForwardIter first, ForwardIter last );
  /*! The minimum absolute value of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
double minAbs( const Container &vec );

  /*! The maximum absolute value of the range \a first, \a last.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
double maxAbs( ForwardIter first, ForwardIter last );
  /*! The maximum absolute value of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
double maxAbs( const Container &vec );

  /*! Clips the elements of the range \a first, \a last
      at \a min and \a max.
      Returns the number of clipped data elements.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
int clip( double min, double max, ForwardIter first, ForwardIter last );
  /*! Clips the elements of the container \a vec
      at \a min and \a max.
      Returns the number of clipped data elements.
      \a Container contains numbers
      \c float 's, \c double 's, or \c long \c double 's.). */
template < typename Container >
int clip( double min, double max, Container &vec );

  /*! The arithmetic mean 
      \f[ \bar x = \frac{1}{N} \sum_{i=1}^N x_i \f] 
      over the range of \a x - values \a firstx, \a lastx.
      \a ForwardIterX is a forward iterator
      that points to a number. */
template < typename ForwardIterX >
double mean( ForwardIterX firstx, ForwardIterX lastx );
  /*! The arithmetic mean 
      \f[ \bar x = \frac{1}{N} \sum_{i=1}^N x_i \f]
      of all elements \a x of the container \a vecx.
      \a ContainerX holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX >
double mean( const ContainerX &vecx );

  /*! The weighted arithmetic mean 
      \f[ \bar x = \frac{\sum_{i=1}^N x_i w_i}{\sum_{i=1}^N w_i} \f]
      over the range of \a x - values \a firstx, \a lastx
      with corresponding weigths \a w in the range \a firstw, \a lastw.
      \a ForwardIterX and \a ForwardIterW are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterW >
double wmean( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterW firstw, ForwardIterW lastw );
  /*! The weighted arithmetic mean 
      \f[ \bar x = \frac{\sum_{i=1}^N x_i w_i}{\sum_{i=1}^N w_i} \f]
      of all elements \a x of the container \a vecx
      with corresponding weights \a w in the container \a vecw.
      \a ContainerX and \a ContainerW hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerW >
double wmean( const ContainerX &vecx, const ContainerW &vecw );

  /*! The weighted arithmetic mean 
      \f[ \bar x = \frac{\sum_{i=1}^N x_i / \sigma_i^2}{\sum_{i=1}^N 1/\sigma_i^2} \f]
      over the range of \a x - values \a firstx, \a lastx
      with corresponding standard deviations \f$ \sigma_i \f$ in the range \a firsts, \a lasts.
      \a ForwardIterX and \a ForwardIterS are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterS >
double smean( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterS firsts, ForwardIterS lasts );
  /*! The weighted arithmetic mean 
      \f[ \bar x = \frac{\sum_{i=1}^N x_i / \sigma_i^2}{\sum_{i=1}^N 1/\sigma_i^2} \f]
      of all elements \a x of the container \a vecx
      with corresponding standard deviations \f$ \sigma_i \f$ in the container \a vecs.
      \a ContainerX and \a ContainerS hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerS >
double smean( const ContainerX &vecx, const ContainerS &vecs );

  /*! The arithmetic mean
      \f[ \bar x = \frac{1}{N} \sum_{i=1}^N x_i \f] 
      over the range \a firstx, \a lastx.
      \a ForwardIterX is a forward iterator
      that points to a number. 
      In \a stdev the unbiased standard deviation 
      \f[ \sigma = \sqrt{\frac{1}{N-1}\sum_{i=1}^N (x_i - \bar x)^2} \f] is returned. */
template < typename ForwardIterX >
double meanStdev( double &stdev, 
		  ForwardIterX firstx, ForwardIterX lastx );
  /*! The arithmetic mean 
      \f[ \bar x = \frac{1}{N} \sum_{i=1}^N x_i \f] 
      of all elements \a x of the container \a vecx.
      \a ContainerX holds an array of numbers
      that can be accessed via standard STL const_iterators. 
      In \a stdev the unbiased standard deviation 
      \f[ \sigma = \sqrt{\frac{1}{N-1}\sum_{i=1}^N (x_i - \bar x)^2} \f] is returned. */
template < typename ContainerX >
double meanStdev( double &stdev, const ContainerX &vecx );

  /*! The weighted arithmetic mean 
      \f[ \bar x = \frac{\sum_{i=1}^N x_i w_i}{\sum_{i=1}^N w_i} \f]
      over the range of \a x - values \a firstx, \a lastx
      with corresponding weigths in the range \a firstw, \a lastw.
      \a ForwardIterX and \a ForwardIterW are forward iterators
      that point to a number. 
      In \a stdev the standard deviation 
      \f[ \sigma = \sqrt{\frac{\sum_{i=1}^N w_i (x_i - \bar x)^2}{\sum_{i=1}^N w_i}} \f]
      is returned. */
template < typename ForwardIterX, typename ForwardIterW >
double wmeanStdev( double &stdev,
		   ForwardIterX firstx, ForwardIterX lastx,
		   ForwardIterW firstw, ForwardIterW lastw );
  /*! The weighted arithmetic mean 
      \f[ \bar x = \frac{\sum_{i=1}^N x_i w_i}{\sum_{i=1}^N w_i} \f]
      of all elements \a x of the container \a vecx
      with corresponding weights in the container \a vecw.
      \a Container1 and \a ContainerW hold an array of numbers
      that can be accessed via standard STL const_iterators. 
      In \a stdev the standard deviation 
      \f[ \sigma = \sqrt{\frac{\sum_{i=1}^N w_i (x_i - \bar x)^2}{\sum_{i=1}^N w_i}} \f]
      is returned. */
template < typename ContainerX, typename ContainerW >
double wmeanStdev( double &stdev,
		   const ContainerX &vecx, const ContainerW &vecw );

  /*! The weighted arithmetic mean 
      \f[ \bar x = \frac{\sum_{i=1}^N x_i / \sigma_i^2}{\sum_{i=1}^N 1/\sigma_i^2} \f]
      over the range of \a x values \a firstx, \a lastx
      with corresponding standard deviations \f$ \sigma_i \f$ in the range \a firsts, \a lasts.
      \a ForwardIterX and \a ForwardIterS are forward iterators
      that point to a number. 
      In \a stdev the standard deviation 
      \f[ \sigma = \sqrt{\frac{\sum_{i=1}^N (x_i - \bar x)^2/\sigma_i^2}{\sum_{i=1}^N 1/\sigma_i^2}} \f]
      is returned. */
template < typename ForwardIterX, typename ForwardIterS >
double smeanStdev( double &stdev,
		   ForwardIterX firstx, ForwardIterX lastx,
		   ForwardIterS firsts, ForwardIterS lasts );
  /*! The weighted arithmetic mean 
      \f[ \bar x = \frac{\sum_{i=1}^N x_i / \sigma_i^2}{\sum_{i=1}^N 1/\sigma_i^2} \f]
      of all elements \a x of the container \a vecx
      with corresponding weights in the container \a vecw.
      \a Container1 and \a ContainerS hold an array of numbers
      that can be accessed via standard STL const_iterators. 
      In \a stdev the standard deviation 
      \f[ \sigma = \sqrt{\frac{\sum_{i=1}^N w_i (x_i - \bar x)^2}{\sum_{i=1}^N w_i}} \f]
      is returned. */
template < typename ContainerX, typename ContainerS >
double smeanStdev( double &stdev,
		   const ContainerX &vecx, const ContainerS &vecs );

  /*! The unbiased variance 
      \f[ \sigma^2 = \frac{1}{N-1}\sum_{i=1}^N (x_i - \bar x)^2 \f]
      over the range of \a x - values \a firstx, \a lastx.
      \a ForwardIterX is a forward iterator
      that points to a number. */
template < typename ForwardIterX >
double variance( ForwardIterX firstx, ForwardIterX lastx );
  /*! The unbiased variance 
      \f[ \sigma^2 = \frac{1}{N-1}\sum_{i=1}^N (x_i - \bar x)^2 \f]
      of all elements \a x of the container \a vecx.
      \a ContainerX holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX >
double variance( const ContainerX &vecx );
  /*! The umbiased variance 
      \f[ \sigma^2 = \frac{1}{N-1}\sum_{i=1}^N (x_i - \mu)^2 \f]
      over the range of \a x - values \a firstx, \a lastx
      for known \a mean.
      \a ForwardIterX is a forward iterator that points to a number. */
template < typename ForwardIterX >
double variance( double mean, ForwardIterX firstx, ForwardIterX lastx );
  /*! The unbiased variance
      \f[ \sigma^2 = \frac{1}{N-1}\sum_{i=1}^N (x_i - \mu)^2 \f]
      of all elements \a x of the container \a vecx
      for known mean.
      \a ContainerX holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX >
double variance( double mean, const ContainerX &vecx );
  /*! The variance 
      \f[ \sigma^2 = \frac{1}{N}\sum_{i=1}^N (x_i - \mu)^2 \f]
      over the range of \a x - values \a firstx, \a lastx
      for fixed \a mean.
      \a ForwardIterX is a forward iterator that points to a number. */
template < typename ForwardIterX >
double varianceFixed( double fixedmean, ForwardIterX firstx, ForwardIterX lastx );
  /*! The variance
      \f[ \sigma^2 = \frac{1}{N}\sum_{i=1}^N (x_i - \mu)^2 \f]
      of all elements \a x of the container \a vecx
      for fixed mean.
      \a ContainerX holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX >
double varianceFixed( double fixedmean, const ContainerX &vecx );

  /*! The weighted variance
      \f[ \sigma^2 = \frac{\sum_{i=1}^N w_i (x_i - \bar x)^2}{\sum_{i=1}^N w_i} \f]
      over the range of \a x - values \a firstx, \a lastx
      with corresponding weigths \a w in the range \a firstw, \a lastw.
      \a ForwardIterX and \a ForwardIterW are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterW >
double wvariance( ForwardIterX firstx, ForwardIterX lastx,
		  ForwardIterW firstw, ForwardIterW lastw );
  /*! The weighted unbiased variance 
      \f[ \sigma^2 = \frac{\sum_{i=1}^N w_i (x_i - \bar x)^2}{\sum_{i=1}^N w_i} \f]
      of all elements \a x of the container \a vecx
      with corresponding weights \a w in the container \a vecw.
      \a ContainerX and \a ContainerW hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerW >
double wvariance( const ContainerX &vecx, const ContainerW &vecw );
  /*! The weighted variance 
      \f[ \sigma^2 = \frac{\sum_{i=1}^N w_i (x_i - \mu)^2}{\sum_{i=1}^N w_i} \f]
      over the range of \a x - values \a firstx, \a lastx
      for known \a mean
      with corresponding weigths \a w in the range \a firstw, \a lastw.
      \a ForwardIterX and \a ForwardIterW are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterW >
double wvariance( double mean, ForwardIterX firstx, ForwardIterX lastx, 
		  ForwardIterW firstw, ForwardIterW lastw );
  /*! The weighted variance 
      \f[ \sigma^2 = \frac{\sum_{i=1}^N w_i (x_i - \mu)^2}{\sum_{i=1}^N w_i} \f]
      of all elements \a x of the container \a vecx
      for known mean
      with corresponding weights \a w in the container \a vecw.
      \a ContainerX and \a ContainerW hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerW >
double wvariance( double mean, const ContainerX &vecx, const ContainerW &vecw );

  /*! The unbiased standard deviation
      \f[ \sigma = \sqrt{\frac{1}{N-1}\sum_{i=1}^N (x_i - \bar x)^2} \f]
      over the range of \a x - values \a firstx, \a lastx.
      \a ForwardIterX is a forward iterator
      that points to a number. */
template < typename ForwardIterX >
double stdev( ForwardIterX firstx, ForwardIterX lastx );
  /*! The unbiased standard deviation 
      \f[ \sigma = \sqrt{\frac{1}{N-1}\sum_{i=1}^N (x_i - \bar x)^2} \f]
      of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX >
double stdev( const ContainerX &vec );
  /*! The unbiased standard deviation 
      \f[ \sigma = \sqrt{\frac{1}{N-1}\sum_{i=1}^N (x_i - \mu)^2} \f]
      over the range of \a x - values \a firstx, \a lastx for known mean.
      \a ForwardIterX is a forward iterator that points to a number. */
template < typename ForwardIterX >
double stdev( double mean, ForwardIterX firstx, ForwardIterX lastx );
  /*! The unbiased standard deviation 
      \f[ \sigma = \sqrt{\frac{1}{N-1}\sum_{i=1}^N (x_i - \mu)^2} \f]
      of all elements of the container \a vec for known mean.
      \a ContainerX holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX >
double stdev( double mean, const ContainerX &vec );
  /*! The standard deviation 
      \f[ \sigma = \sqrt{\frac{1}{N}\sum_{i=1}^N (x_i - \mu)^2} \f]
      over the range of \a x - values \a firstx, \a lastx for fixed mean.
      \a ForwardIterX is a forward iterator that points to a number. */
template < typename ForwardIterX >
double stdevFixed( double fixedmean, ForwardIterX firstx, ForwardIterX lastx );
  /*! The standard deviation 
      \f[ \sigma = \sqrt{\frac{1}{N}\sum_{i=1}^N (x_i - \mu)^2} \f]
      of all elements of the container \a vec for fixed mean.
      \a ContainerX holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX >
double stdevFixed( double fixedmean, const ContainerX &vec );

  /*! The unbiased weighted standard deviation
      \f[ \sigma = \sqrt{\frac{\sum_{i=1}^N w_i (x_i - \bar x)^2}{\sum_{i=1}^N w_i}} \f]
      over the range of \a x - values \a firstx, \a lastx
      with corresponding weigths in the range \a firstw, \a lastw.
      \a ForwardIterX and \a ForwardIterW are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterW >
double wstdev( ForwardIterX firstx, ForwardIterX lastx,
	       ForwardIterW firstw, ForwardIterW lastw );
  /*! The unbiased weighted standard deviation 
      \f[ \sigma = \sqrt{\frac{\sum_{i=1}^N w_i (x_i - \bar x)^2}{\sum_{i=1}^N w_i}} \f]
      of all elements of the container \a vecx
      with corresponding weights in the container \a vecw.
      \a ContainerX and \a ContainerW hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerW >
double wstdev( const ContainerX &vecx, const ContainerW &vecw );
  /*! The weighted standard deviation 
      \f[ \sigma = \sqrt{\frac{\sum_{i=1}^N w_i (x_i - \mu)^2}{\sum_{i=1}^N w_i}} \f]
      over the range of \a x - values \a firstx, \a lastx
      for known \a mean
      with corresponding weigths in the range \a firstw, \a lastw.
      \a ForwardIterX and \a ForwardIterW are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterW >
double wstdev( double mean, ForwardIterX firstx, ForwardIterX lastx, 
	       ForwardIterW firstw, ForwardIterW lastw );
  /*! The weighted standard deviation
      \f[ \sigma = \sqrt{\frac{\sum_{i=1}^N w_i (x_i - \mu)^2}{\sum_{i=1}^N w_i}} \f]
      of all elements of the container \a vecx
      for known mean
      with corresponding weights in the container \a vecw.
      \a ContainerX and \a ContainerW hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerW >
double wstdev( double mean, const ContainerX &vecx, const ContainerW &vecw );

  /*! The unbiased standard error mean
      \f[ s.e.m. = \sqrt{\frac{1}{N-1}\sum_{i=1}^N (x_i - \bar x)^2} / \sqrt{N} \f]
      over the range of \a x - values \a firstx, \a lastx.
      \a ForwardIterX is a forward iterator
      that points to a number. */
template < typename ForwardIterX >
double sem( ForwardIterX firstx, ForwardIterX lastx );
  /*! The unbiased standard error mean 
      \f[ \sigma = \sqrt{\frac{1}{N-1}\sum_{i=1}^N (x_i - \bar x)^2} / \sqrt{N} \f]
      of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX >
double sem( const ContainerX &vec );
  /*! The unbiased standard error mean 
      \f[ \sigma = \sqrt{\frac{1}{N-1}\sum_{i=1}^N (x_i - \mu)^2} / \sqrt{N} \f]
      over the range of \a x - values \a firstx, \a lastx for known mean.
      \a ForwardIterX is a forward iterator that points to a number. */
template < typename ForwardIterX >
double sem( double mean, ForwardIterX firstx, ForwardIterX lastx );
  /*! The unbiased standard error mean 
      \f[ \sigma = \sqrt{\frac{1}{N-1}\sum_{i=1}^N (x_i - \mu)^2} / \sqrt{N} \f]
      of all elements of the container \a vec for known mean.
      \a ContainerX holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX >
double sem( double mean, const ContainerX &vec );
  /*! The standard error mean 
      \f[ \sigma = \sqrt{\frac{1}{N}\sum_{i=1}^N (x_i - \mu)^2} / \sqrt{N} \f]
      over the range of \a x - values \a firstx, \a lastx for fixed mean.
      \a ForwardIterX is a forward iterator that points to a number. */
template < typename ForwardIterX >
double semFixed( double fixedmean, ForwardIterX firstx, ForwardIterX lastx );
  /*! The standard error mean 
      \f[ \sigma = \sqrt{\frac{1}{N}\sum_{i=1}^N (x_i - \mu)^2} / \sqrt{N} \f]
      of all elements of the container \a vec for fixed mean.
      \a ContainerX holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX >
double semFixed( double fixedmean, const ContainerX &vec );

  /*! The absolute deviation from the mean 
      \f[ s = \frac{1}{N}\sum_{i=1}^N |x_i - \bar x| \f]
      over the range of \a x - values \a firstx, \a lastx.
      \a ForwardIterX is a forward iterator
      that points to a number. */
template < typename ForwardIterX >
double absdev( ForwardIterX firstx, ForwardIterX lastx );
  /*! The absolute deviation from the mean
      \f[ s = \frac{1}{N}\sum_{i=1}^N |x_i - \bar x| \f]
      of all elements of the container \a vec.
      \a ContainerX holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX >
double absdev( const ContainerX &vec );
  /*! The absolute deviation from the mean 
      \f[ s = \frac{1}{N}\sum_{i=1}^N |x_i - \mu| \f]
      over the range of \a x - values \a firstx, \a lastx for known mean.
      \a ForwardIterX is a forward iterator that points to a number. */
template < typename ForwardIterX >
double absdev( double mean, ForwardIterX firstx, ForwardIterX lastx );
  /*! The absolute deviation from the mean
      \f[ s = \frac{1}{N}\sum_{i=1}^N |x_i - \mu| \f]
      of all elements of the container \a vec for known mean.
      \a ContainerX holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX >
double absdev( double mean, const ContainerX &vec );

  /*! The weighted absolute deviation from the mean
      \f[ s = \frac{\sum_{i=1}^N w_i |x_i - \bar x|}{\sum_{i=1}^N w_i} \f]
      over the range of \a x - values \a firstx, \a lastx
      with corresponding weigths in the range \a firstw, \a lastw.
      \a ForwardIterX and \a ForwardIterW are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterW >
double wabsdev( ForwardIterX firstx, ForwardIterX lastx,
	       ForwardIterW firstw, ForwardIterW lastw );
  /*! The weighted absolute deviation from the mean
      \f[ s = \frac{\sum_{i=1}^N w_i |x_i - \bar x|}{\sum_{i=1}^N w_i} \f]
      of all elements of the container \a vecx
      with corresponding weights in the container \a vecw.
      \a ContainerX and \a ContainerW hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerW >
double wabsdev( const ContainerX &vecx, const ContainerW &vecw );
  /*! The weighted absolute deviation from the mean
      \f[ s = \frac{\sum_{i=1}^N w_i |x_i - \mu|}{\sum_{i=1}^N w_i} \f]
      over the range of \a x - values \a firstx, \a lastx
      for known \a mean
      with corresponding weigths in the range \a firstw, \a lastw.
      \a ForwardIterX and \a ForwardIterW are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterW >
double wabsdev( double mean, ForwardIterX firstx, ForwardIterX lastx, 
	       ForwardIterW firstw, ForwardIterW lastw );
  /*! The weighted absolute deviation from the mean
      \f[ s = \frac{\sum_{i=1}^N w_i |x_i - \mu|}{\sum_{i=1}^N w_i} \f]
      of all elements of the container \a vecx
      for known mean
      with corresponding weights in the container \a vecw.
      \a ContainerX and \a ContainerW hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerW >
double wabsdev( double mean, const ContainerX &vecx, const ContainerW &vecw );

  /*! The root-mean-square
      over the range \a first, \a last.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
double rms( ForwardIter first, ForwardIter last );
  /*! The root-mean-square
      of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
double rms( const Container &vec );

  /*! The skewness over the range \a first, \a last.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
double skewness( ForwardIter first, ForwardIter last );
  /*! The skewness of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
double skewness( const Container &vec );

  /*! The kurtosis over the range \a first, \a last.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
double kurtosis( ForwardIter first, ForwardIter last );
  /*! The skewness of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
double kurtosis( const Container &vec );

  /*! The sum of all elements over the range \a first, \a last.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
double sum( ForwardIter first, ForwardIter last );
  /*! The sum of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
double sum( const Container &vec );

  /*! The sum of the square of all elements over the range \a first, \a last.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
double squaredSum( ForwardIter first, ForwardIter last );
  /*! The sum of the square of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
double squaredSum( const Container &vec );

  /*! The square root of the sum of the square of all elements over the range \a first, \a last.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
double magnitude( ForwardIter first, ForwardIter last );
  /*! The square root of the sum of the square of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
double magnitude( const Container &vec );

  /*! The power \<x^2\> of all elements over the range \a first, \a last.
      \a ForwardIter is a forward iterator
      that points to a number. */
template < typename ForwardIter >
double power( ForwardIter first, ForwardIter last );
  /*! The power \<x^2\> of all elements of the container \a vec.
      \a Container holds an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename Container >
double power( const Container &vec );

  /*! The dot product 
      \f[ \mathrm{dot}(x,y) = \sum_{i=1}^N x_i y_i \f]
      between the two ranges of \a x - values \a firstx, \a lastx
      and of \a y - values \a firsty, \a lasty.
      \a ForwardIterX and \a ForwardIterY are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterY >
double dot( ForwardIterX firstx, ForwardIterX lastx,
            ForwardIterY firsty, ForwardIterY lasty );
  /*! The dot product 
      \f[ \mathrm{dot}(x,y) = \sum_{i=1}^N x_i y_i \f]
      between the two container of \a x - values \a vecx and of \a y - values \a vecy.
      \a ContainerX and \a ContainerY each hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerY >
double dot( const ContainerX &vecx, const ContainerY &vecy );

  /*! Return in each element in \a x the average over the corresponding 
      elements in \a y. */
template < typename ContainerX, typename ContainerY >
void average( ContainerX &x, const vector< ContainerY > &y );
  /*! Return in each element in \a x the average over the corresponding 
      elements in \a y. 
      The corresponding standard deviations are returned in \a s. */
template < typename ContainerX, typename ContainerS, typename ContainerY >
void average( ContainerX &x, ContainerS &s, const vector< ContainerY > &y );

  /*! The covariance 
      \f[ cov(x,y) = \frac{1}{N-1} \sum_{i=1}^N (x_i - \bar x)(y_i - \bar y) \f]
      between the two ranges of \a x - values \a firstx, \a lastx
      and of \a y - values \a firsty, \a lasty.
      \a ForwardIterX and \a ForwardIterY are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterY >
double cov( ForwardIterX firstx, ForwardIterX lastx,
            ForwardIterY firsty, ForwardIterY lasty );
  /*! The covariance 
      \f[ cov(x,y) = \frac{1}{N-1} \sum_{i=1}^N (x_i - \bar x)(y_i - \bar y) \f]
      between the two container of \a x - values \a vecx and of \a y - values \a vecy.
      \a ContainerX and \a ContainerY each hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerY >
double cov( const ContainerX &vecx, const ContainerY &vecy );

  /*! The linear correlation coefficient 
      \f[ r = \frac{cov(x,y)}{\sigma_x \sigma_y} \f]
      (Pearson's r)
      between the two ranges \a firstx, \a lastx
      and \a firsty, \a lasty.
      \a ForwardIterX and \a ForwardIterY are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterY >
double corrCoef( ForwardIterX firstx, ForwardIterX lastx,
		 ForwardIterY firsty, ForwardIterY lasty );
  /*! The linear correlation coefficient 
      \f[ r = \frac{cov(x,y)}{\sigma_x \sigma_y} \f]
      (Pearson's r) between the two container \a vecx and \a vecy.
      \a ContainerX and \a ContainerY each hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerY >
double corrCoef( const ContainerX &vecx, const ContainerY &vecy );

  /*! The linear correlation coefficient 
      \f[ r = \frac{\sum_{i=1}^N w_i (x_i - \bar x)(y_i - \bar y)}{\sqrt{(\sum_{I=1}^N w_i (x_i - \bar x)^2 )(\sum_{I=1}^N w_i (y_i - \bar y)^2 )}} \f]
      (Pearson's r)
      between the two ranges \a firstx, \a lastx
      and \a firsty, \a lasty with weights in the range from \a firstw to \a lastw.
      \a ForwardIterX, \a ForwardIterY, and \a ForwardIterW are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterY, typename ForwardIterW >
double wcorrCoef( ForwardIterX firstx, ForwardIterX lastx,
		  ForwardIterY firsty, ForwardIterY lasty,
		  ForwardIterW firstw, ForwardIterW lastw );
  /*! The linear correlation coefficient 
      \f[ r = \frac{\sum_{i=1}^N w_i (x_i - \bar x)(y_i - \bar y)}{\sqrt{(\sum_{I=1}^N w_i (x_i - \bar x)^2 )(\sum_{I=1}^N w_i (y_i - \bar y)^2 )}} \f]
      (Pearson's r) between the two container \a vecx and \a vecy
      with weights \a vecw.
      \a ContainerX, \a ContainerY, and \a ContainerW each hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerY, typename ContainerW >
double wcorrCoef( const ContainerX &vecx, const ContainerY &vecy, 
		  const ContainerW &vecw );

  /*! The linear correlation coefficient 
      \f[ r = \frac{\sum_{i=1}^N (x_i - \bar x)(y_i - \bar y)/\sigma_i^2}{\sqrt{(\sum_{I=1}^N (x_i - \bar x)^2/\sigma_i^2 )(\sum_{I=1}^N (y_i - \bar y)^2/\sigma_i^2 )}} \f]
      (Pearson's r)
      between the two ranges \a firstx, \a lastx
      and \a firsty, \a lasty with standard deviations of \a y in the range from \a firsts to \a lasts.
      \a ForwardIterX, \a ForwardIterY, and \a ForwardIterS are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterY, typename ForwardIterS >
double scorrCoef( ForwardIterX firstx, ForwardIterX lastx,
		  ForwardIterY firsty, ForwardIterY lasty,
		  ForwardIterS firsts, ForwardIterS lasts );
  /*! The linear correlation coefficient 
      \f[ r = \frac{\sum_{i=1}^N (x_i - \bar x)(y_i - \bar y)/\sigma_i^2}{\sqrt{(\sum_{I=1}^N (x_i - \bar x)^2/\sigma_i^2 )(\sum_{I=1}^N (y_i - \bar y)^2/\sigma_i^2 )}} \f]
      (Pearson's r) between the two container \a vecx and \a vecy
      with standard deviations of \a y \a vecs.
      \a ContainerX, \a ContainerY, and \a ContainerS each hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerY, typename ContainerS >
double scorrCoef( const ContainerX &vecx, const ContainerY &vecy, 
		  const ContainerS &vecs );

  /*! The squared difference (chi squared)
      \f[ \chi^2 = \sum_{i=1}^N \left(x_i - y_i\right)^2 \f]
      between the two ranges \a firstx, \a lastx
      and \a firsty, \a lasty.
      \a ForwardIterX and \a ForwardIterY are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterY >
double chisq( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty );
  /*! The squared difference (chi squared)
      \f[ \chi^2 = \sum_{i=1}^N \left(x_i - y_i\right)^2 \f]
      between the two container \a vecx and \a vecy.
      \a ContainerX and \a ContainerY each hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerY >
double chisq( const ContainerX &vecx, const ContainerY &vecy );

  /*! The squared difference (chi squared)
      \f[ \chi^2 = \sum_{i=1}^N \left(\frac{x_i - y_i}{\sigma_i}\right)^2 \f]
      between the two ranges \a firstx, \a lastx
      and \a firsty, \a lasty with standard deviations \f$ \sigma_i \f$ in the range from \a firsts to \a lasts.
      \a ForwardIterX, \a ForwardIterY, and \a ForwardIterS are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterY, typename ForwardIterS >
double chisq( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty,
	      ForwardIterS firsts, ForwardIterS lasts );
  /*! The squared difference (chi squared)
      \f[ \chi^2 = \sum_{i=1}^N \left(\frac{x_i - y_i}{\sigma_i}\right)^2 \f]
      between the two container \a vecx and \a vecy
      with standard deviations in \a vecs.
      \a ContainerX, \a ContainerY, and \a ContainerS each hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerY, typename ContainerS >
double chisq( const ContainerX &vecx, const ContainerY &vecy, 
	      const ContainerS &vecs );

  /*! The serial correlation coefficients (autocorrelation)
      r_k = cov(x_1,x_k)/var(x) 
      of the range \a firstx, \a lastx
      for different lags \a k are returned in the range
      \a firsty, \a lasty.
      \a ForwardIterX and \a ForwardIterY are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterY >
void serialCorr( ForwardIterX firstx, ForwardIterX lastx,
		 ForwardIterY firsty, ForwardIterY lasty );
  /*! The serial correlation coefficients (autocorrelation)
      r_k = cov(x_1,x_k)/var(x) 
      of the container \a vecx
      for different lags \a k are returned in the container \a vecy.
      \a ContainerX and \a ContainerY each hold an array of numbers
      that can be accessed via standard STL iterators. */
template < typename ContainerX, typename ContainerY >
void serialCorr( const ContainerX &vecx, ContainerY &vecy );

  /*! Fit line y = m*x to the data of the two ranges 
      \a firstx, \a lastx and \a firsty, \a lasty.
      Returns in \a m the slope, in \a mu the slope's uncertainty,
      and in \a chisq the chi squared.
      \a ForwardIterX and \a ForwardIterY are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterY >
void propFit( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty,
	      double &m, double &mu, double &chisq );
  /*! Fit line y = m*x to the data of the two container 
      \a vecx and \a vecy.
      Returns in \a m the slope, in \a mu the slope's uncertainty,
      and in \a chisq the chi squared.
      \a ContainerX and \a ContainerY each hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerY >
void propFit( const ContainerX &vecx, const ContainerY &vecy,
	      double &m, double &mu, double &chisq );
  /*! Fit line y = m*x to the data of the three ranges 
      \a firstx, \a lastx and \a firsty, \a lasty.
      The first range is the x-vale, the second the y-value,
      and the third the standard deviations in y.
      Returns in \a m the slope, in \a mu the slope's uncertainty,
      and in \a chisq the chi squared.
      \a ForwardIterX, \a ForwardIterY, and \a ForwardIterS are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterY, typename ForwardIterS >
void propFit( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty,
	      ForwardIterS firsts, ForwardIterS lasts,
	      double &m, double &mu, double &chisq );
  /*! Fit line y = m*x to the data of the three container 
      \a vecx, \a vecy, and \a vecs,
      corresponding to the x-value, y-value and its standard deviation.
      Returns in \a m the slope, in \a mu the slope's uncertainty,
      and in \a chisq the chi squared.
      \a ContainerX, \a ContainerY, and \a ContainerS each hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerY, typename ContainerS >
void propFit( const ContainerX &vecx, const ContainerY &vecy, const ContainerY &vecs,
	      double &m, double &mu, double &chisq );

  /*! Fit line y = b+m*x to the data of the two ranges 
      \a firstx, \a lastx and \a firsty, \a lasty.
      Returns in \a bu and \a mu the uncertainty
      of the offset \a b und slope \a m,
      and in \a chisq the chi squared.
      \a ForwardIterX and \a ForwardIterY are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterY >
void lineFit( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty,
	      double &b, double &bu,
	      double &m, double &mu, double &chisq );
  /*! Fit line y = b+m*x to the data of the two container 
      \a vecx and \a vecy.
      Returns in \a bu and \a mu the uncertainty
      of the offset \a b und slope \a m,
      and in \a chisq the chi squared.
      \a ContainerX and \a ContainerY each hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerY >
void lineFit( const ContainerX &vecx, const ContainerY &vecy,
	      double &b, double &bu,
	      double &m, double &mu, double &chisq );
  /*! Fit line y = b+m*x to the data of the three ranges 
      \a firstx, \a lastx, \a firsty, \a lasty, and \a firsts, \a lasts,
      corresponding to x, y, and the standard deviation.
      Returns in \a bu and \a mu the uncertainty
      of the offset \a b und slope \a m,
      and in \a chisq the chi squared.
      q=GammaQ(0.5*(n-2),chisq/2)>0.1 good, >0.001 fit may be acceptable, <0.001 bad fit.
      \a ForwardIterX, \a ForwardIterY, and \a ForwardIterS are forward iterators
      that point to a number. */
template < typename ForwardIterX, typename ForwardIterY, typename ForwardIterS >
void lineFit( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty,
	      ForwardIterS firsts, ForwardIterS lasts,
	      double &b, double &bu,
	      double &m, double &mu, double &chisq );
  /*! Fit line y = b+m*x to the data of the three container 
      \a vecx, \a vecy, and \a vecy,
      corresponding to x, y, and the standard deviation.
      Returns in \a bu and \a mu the uncertainty
      of the offset \a b und slope \a m,
      and in \a chisq the chi squared.
      q=GammaQ(0.5*(n-2),chisq/2)>0.1 good, >0.001 fit may be acceptable, <0.001 bad fit.
      \a ContainerX, \a ContainerY, and \a ContainerS each hold an array of numbers
      that can be accessed via standard STL const_iterators. */
template < typename ContainerX, typename ContainerY, typename ContainerS >
void lineFit( const ContainerX &vecx, const ContainerY &vecy, const ContainerY &vecs,
	      double &b, double &bu,
	      double &m, double &mu, double &chisq );

  /*! Remove mean and slope of the range \a firstx, \a lastx. */
template < typename ForwardIterX >
void detrend( ForwardIterX firstx, ForwardIterX lastx );
  /*! Remove mean and slope of the container \a vecx. */
template < typename ContainerX >
void detrend( ContainerX &vecx );


template < typename RandomIter >
double median( RandomIter first, RandomIter last )
{
  if ( first == last )
    return 0.0;

  const int n = last - first;
  RandomIter iter1 = first + n/2;
  if ( n%2 == 0 ) {
    RandomIter iter0 = iter1 - 1;
    return 0.5 * ( (*iter0) + (*iter1) );
  }
  else
    return (*iter1);
}


template < typename Container >
double median( const Container &vec )
{
  return median( vec.begin(), vec.end() );
}


template < typename RandomIter >
double quantile( double f, RandomIter first, RandomIter last )
{
  if ( first == last )
    return 0.0;

  const int n = last - first;
  const double index = f * (n - 1);
  const int lindex = (int)::floor( index );
  const double delta = index - lindex;
  RandomIter iter1 = first + lindex;
  if ( lindex == n-1 )
    return *iter1;
  else {
    RandomIter iter2 = iter1 + 1;
    return (1.0 - delta) * (*iter1) + delta * (*iter2);
  }
}


template < typename Container >
double quantile( double f, const Container &vec )
{
  return quantile( f, vec.begin(), vec.end() );
}


template < typename ForwardIter >
double rank( ForwardIter first, ForwardIter last )
{
  int k = 1;
  double s = 0.0;
  while ( first+1 < last ) {
    if ( *(first+1) != *first ) {
      // not a tie:
      *first = k;
      ++first;
      ++k;
    } 
    else {
      // a tie:
      ForwardIter iter = first + 1;
      int kt = k + 1;
      while ( iter != last && *iter == *first ) {
	++iter;
	++kt;
      }
      double r = 0.5*(k+kt-1);
      while ( first != iter ) {
	*first = r;
	++first;
      }
      double t = kt - k;
      s += t*t*t - t;
      k = kt;
    }
  }
  if ( first+1 == last )
    *first = k;
  return s;
}


template < typename Container >
double rank( Container &vec )
{
  return rank( vec.begin(), vec.end() );
}


template < typename ForwardIter >
double min( ForwardIter first, ForwardIter last )
{
  if ( first == last )
    return 0.0;

  double min = *first;
  while ( ++first != last ) {
    if ( min > *first )
      min = *first;
  }
  return min;
}


template < typename Container >
double min( const Container &vec )
{
  return min( vec.begin(), vec.end() );
}


template < typename ForwardIter >
double min( int &index, ForwardIter first, ForwardIter last )
{
  index = -1;
  if ( first == last )
    return 0.0;

  double min = *first;
  index = 0;
  for ( int i=1; ++first != last; ++i ) {
    if ( min > *first ) {
      min = *first;
      index = i;
    }
  }
  return min;
}


template < typename Container >
double min( int &index, const Container &vec )
{
  return min( index, vec.begin(), vec.end() );
}


template < typename ForwardIter >
int minIndex( ForwardIter first, ForwardIter last )
{
  if ( first == last )
    return -1;

  double min = *first;
  int mini = 0;
  for ( int i=1; ++first != last; ++i ) {
    if ( min > *first ) {
      min = *first;
      mini = i;
    }
  }
  return mini;
}


template < typename Container >
int minIndex( const Container &vec )
{
  return minIndex( vec.begin(), vec.end() );
}


template < typename ForwardIter >
double max( ForwardIter first, ForwardIter last )
{
  if ( first == last )
    return 0.0;

  double max = *first;
  while ( ++first != last ) {
    if ( max < *first )
      max = *first;
  }
  return max;
}


template < typename Container >
double max( const Container &vec )
{
  return max( vec.begin(), vec.end() );
}


template < typename ForwardIter >
double max( int &index, ForwardIter first, ForwardIter last )
{
  index = -1;
  if ( first == last )
    return 0.0;

  double max = *first;
  index = 0;
  for ( int i=1; ++first != last; ++i ) {
    if ( max < *first ) {
      max = *first;
      index = i;
    }
  }
  return max;
}


template < typename Container >
double max( int &index, const Container &vec )
{
  return max( index, vec.begin(), vec.end() );
}


template < typename ForwardIter >
int maxIndex( ForwardIter first, ForwardIter last )
{
  if ( first == last )
    return -1;

  double max = *first;
  int maxi = 0;
  for ( int i=1; ++first != last; ++i ) {
    if ( max < *first ) {
      max = *first;
      maxi = i;
    }
  }
  return maxi;
}


template < typename Container >
int maxIndex( const Container &vec )
{
  return maxIndex( vec.begin(), vec.end() );
}


template < typename ForwardIter >
void minMax( double &min, double &max, ForwardIter first, ForwardIter last )
{
  if ( first == last ) {
    min = 0.0;
    max = 0.0;
    return;
  }

  min = *first;
  max = *first;
  while ( ++first != last ) {
    if ( min > *first )
      min = *first;
    else if ( max < *first )
      max = *first;
  }
}


template < typename Container >
void minMax( double &min, double &max, const Container &vec )
{
  minMax( min, max, vec.begin(), vec.end() );
}


template < typename ForwardIter >
void minMax( double &min, int &minindex, double &max, int &maxindex,
	     ForwardIter first, ForwardIter last )
{
  if ( first == last ) {
    minindex = -1;
    maxindex = -1;
    min = 0.0;
    max = 0.0;
    return;
  }

  min = *first;
  max = *first; 
  minindex = 0;
  maxindex = 0;
  for ( int i=1; ++first != last; ++i ) {
    if ( min > *first ) {
      min = *first;
      minindex = i;
    }
    else if ( max < *first ) {
      max = *first;
      maxindex = i;
    }
  }
}


template < typename Container >
void minMax( double &min, int &minindex, double &max, int &maxindex,
	     const Container &vec )
{
  minMax( min, minindex, max, maxindex, vec.begin(), vec.end() );
}


template < typename ForwardIter >
void minMaxIndex( int &minindex, int &maxindex,
		  ForwardIter first, ForwardIter last )
{
  if ( first == last ) {
    minindex = -1;
    maxindex = -1;
    return;
  }

  double min = *first;
  double max = *first;
  minindex = 0;
  maxindex = 0;
  for ( int i=1; ++first != last; ++i ) {
    if ( min > *first ) {
      min = *first;
      minindex = i;
    }
    else if ( max < *first ) {
      max = *first;
      maxindex = i;
    }
  }
}


template < typename Container >
void minMaxIndex( int &minindex, int &maxindex, const Container &vec )
{
  minMaxIndex( minindex, maxindex, vec.begin(), vec.end() );
}


template < typename ForwardIter >
double minAbs( ForwardIter first, ForwardIter last )
{
  if ( first == last )
    return 0.0;

  double min = ::fabs( *first );
  while ( ++first != last ) {
    if ( min > ::fabs( *first ) )
      min = ::fabs( *first );
  }
  return min;
}


template < typename Container >
double minAbs( const Container &vec )
{
  return minAbs( vec.begin(), vec.end() );
}


template < typename ForwardIter >
double maxAbs( ForwardIter first, ForwardIter last )
{
  if ( first == last )
    return 0.0;

  double max = ::fabs( *first );
  while ( ++first != last ) {
    if ( max < ::fabs( *first ) )
      max = ::fabs( *first );
  }
  return max;
}


template < typename Container >
double maxAbs( const Container &vec )
{
  return maxAbs( vec.begin(), vec.end() );
}


template < typename ForwardIter >
int clip( double min, double max,
	   ForwardIter first, ForwardIter last )
{
  int c = 0;
  for ( ; first != last; ++first ) {
    if ( *first < min ) {
      *first = min;
      c++;
    }
    else if ( *first > max ) {
      *first = max;
      c++;
    }
  }
  return c;
}


template < typename Container >
int clip( double min, double max, Container &vec  )
{
  return clip( min, max, vec.begin(), vec.end() );
}


template < typename ForwardIterX >
double mean( ForwardIterX firstx, ForwardIterX lastx )
{
  double a = 0.0;
  for ( int k=1; firstx != lastx; ++firstx, ++k ) {
    a += ( *firstx - a ) / k;
  }
  return a;
}


template < typename ContainerX >
double mean( const ContainerX &vecx )
{
  return mean( vecx.begin(), vecx.end() );
}


template < typename ForwardIterX, typename ForwardIterW >
double wmean( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterW firstw, ForwardIterW lastw )
{
  double s = 0.0;
  double w = 0.0;
  for ( ; ( firstx != lastx ) && ( firstw != lastw ); ++firstx, ++firstw ) {
    s += (*firstx) * (*firstw);
    w += (*firstw);
  }
  return w > 0.0 ? s/w : 0.0;
}


template < typename ContainerX, typename ContainerW >
double wmean( const ContainerX &vecx, const ContainerW &vecw )
{
  return wmean( vecx.begin(), vecx.end(), vecw.begin(), vecw.end() );
}


template < typename ForwardIterX, typename ForwardIterS >
double smean( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterS firsts, ForwardIterS lasts )
{
  double s = 0.0;
  double w = 0.0;
  for ( ; ( firstx != lastx ) && ( firsts != lasts ); ++firstx, ++firsts ) {
    double d = (*firsts);
    if ( d < 1.0e-10 ) 
      d = 1.0e-10;
    double i = 1.0 / (d*d);
    s += (*firstx) * i;
    w += i;
  }
  return w > 0.0 ? s/w : 0.0;
}


template < typename ContainerX, typename ContainerS >
double smean( const ContainerX &vecx, const ContainerS &vecs )
{
  return smean( vecx.begin(), vecx.end(), vecs.begin(), vecs.end() );
}


template < typename ForwardIterX >
double meanStdev( double &stdev,
		  ForwardIterX firstx, ForwardIterX lastx )
{
  double a = 0.0;
  ForwardIterX iterx = firstx;
  int n=1;
  for ( ; iterx != lastx; ++iterx, ++n ) {
    a += ( *iterx - a ) / n;
  }
  n--;

  double v = 0.0;
  if ( n > 1 ) {
    for ( int k=1; firstx != lastx; ++firstx, ++k ) {
      double s = *firstx - a;
      v += ( s*s - v ) / k;
    }
    v *= n/(n-1);
  }
  stdev = ::sqrt( v );

  return a;
}


template < typename ContainerX >
double meanStdev( double &stdev, const ContainerX &vecx )
{
  return meanStdev( stdev, vecx.begin(), vecx.end() );
}


template < typename ForwardIterX, typename ForwardIterW >
double wmeanStdev( double &stdev,
		   ForwardIterX firstx, ForwardIterX lastx,
		   ForwardIterW firstw, ForwardIterW lastw )
{
  double s = 0.0;
  double w = 0.0;
  ForwardIterX iterx = firstx;
  ForwardIterW iterw = firstw;
  int k=0;
  for ( ; ( iterx != lastx ) && ( iterw != lastw ); ++iterx, ++iterw, ++k ) {
    s += (*iterx) * (*iterw);
    w += (*iterw);
  }
  double a = w > 0.0 ? s/w : 0.0;

  double v = 0.0;
  if ( k > 1 ) {
    double vs = 0.0;
    for ( ; ( firstx != lastx ) && ( firstw != lastw ); ++firstx, ++firstw ) {
      double ss = *firstx - a;
      vs += ss * ss * (*firstw);
    }
    v = w > 0.0 ? vs/w : 0.0;
  }
  stdev = ::sqrt( v );

  return a;
}


template < typename ContainerX, typename ContainerW >
double wmeanStdev( double &stdev, const ContainerX &vecx, const ContainerW &vecw )
{
  return wmeanStdev( stdev, vecx.begin(), vecx.end(), vecw.begin(), vecw.end() );
}


template < typename ForwardIterX, typename ForwardIterS >
double smeanStdev( double &stdev,
		   ForwardIterX firstx, ForwardIterX lastx,
		   ForwardIterS firsts, ForwardIterS lasts )
{
  double s = 0.0;
  double w = 0.0;
  ForwardIterX iterx = firstx;
  ForwardIterS iters = firsts;
  int k=0;
  for ( ; ( iterx != lastx ) && ( iters != lasts ); ++iterx, ++iters, ++k ) {
    double d = (*iters);
    if ( d < 1.0e-10 ) 
      d = 1.0e-10;
    double i = 1.0 / (d*d);
    s += (*iterx) * i;
    w += i;
  }
  double a = w > 0.0 ? s/w : 0.0;

  double v = 0.0;
  if ( k > 1 ) {
    double vs = 0.0;
    for ( ; ( firstx != lastx ) && ( firsts != lasts ); ++firstx, ++firsts ) {
      double d = (*firsts);
      if ( d < 1.0e-10 ) 
	d = 1.0e-10;
      double i = 1.0 / (d*d);
      double ss = *firstx - a;
      vs += ss * ss * i;
    }
    v = w > 0.0 ? vs/w : 0.0;
  }
  stdev = ::sqrt( v );

  return a;
}


template < typename ContainerX, typename ContainerS >
double smeanStdev( double &stdev, 
		   const ContainerX &vecx, const ContainerS &vecs )
{
  return smeanStdev( stdev, vecx.begin(), vecx.end(), vecs.begin(), vecs.end() );
}


template < typename ForwardIterX >
double variance( ForwardIterX firstx, ForwardIterX lastx )
{
  double a = 0.0;
  ForwardIterX iterx = firstx;
  int k=1;
  for ( ; iterx != lastx; ++iterx, ++k ) {
    a += ( *iterx - a ) / k;
  }

  double v = 0.0;
  if ( k > 2 ) {
    for ( k=1; firstx != lastx; ++firstx, ++k ) {
      double s = *firstx - a;
      v += ( s*s - v ) / k;
    }
    k--;
    v *= k/(k-1);
  }
  return v;
}


template < typename Container >
double variance( const Container &vec )
{
  return variance( vec.begin(), vec.end() );
}


template < typename ForwardIterX >
double variance( double mean, ForwardIterX firstx, ForwardIterX lastx )
{
  double v = 0.0;
  int k=1;
  for ( ; firstx != lastx; ++firstx, ++k ) {
    double s = *firstx - mean;
    v += ( s*s - v ) / k;
  }
  if ( k > 2 ) {
    k--;
    v *= k/(k-1);
  }
  return v;
}


template < typename Container >
double variance( double mean, const Container &vec )
{
  return variance( mean, vec.begin(), vec.end() );
}


template < typename ForwardIterX >
double varianceFixed( double fixedmean, 
		      ForwardIterX firstx, ForwardIterX lastx )
{
  double v = 0.0;
  for ( int k=1; firstx != lastx; ++firstx, ++k ) {
    double s = *firstx - fixedmean;
    v += ( s*s - v ) / k;
  }
  return v;
}


template < typename Container >
double varianceFixed( double fixedmean, const Container &vec )
{
  return varianceFixed( fixedmean, vec.begin(), vec.end() );
}


template < typename ForwardIterX, typename ForwardIterW >
double wvariance( ForwardIterX firstx, ForwardIterX lastx,
		  ForwardIterW firstw, ForwardIterW lastw )
{
  double s = 0.0;
  double w = 0.0;
  ForwardIterX iterx = firstx;
  ForwardIterW iterw = firstw;
  int k=0;
  for ( ; ( iterx != lastx ) && ( iterw != lastw ); ++iterx, ++iterw, ++k ) {
    s += (*iterx);
    w += (*iterw);
  }
  double a = w > 0.0 ? s/w : 0.0;

  double v = 0.0;
  if ( k > 1 ) {
    double vs = 0.0;
    for ( ; ( firstx != lastx ) && ( firstw != lastw ); ++firstx, ++firstw ) {
      double ss = (*firstx) - a;
      vs += ss * ss * (*firstw);
    }
    v = w > 0.0 ? vs/w : 0.0;
  }
  return v;
}


template < typename ContainerX, typename ContainerW >
double wvariance( const ContainerX &vecx, const ContainerW &vecw )
{
  return wvariance( vecx.begin(), vecx.end(), vecw.begin(), vecw.end() );
}


template < typename ForwardIterX, typename ForwardIterW >
double wvariance( double mean, ForwardIterX firstx, ForwardIterX lastx,
		  ForwardIterW firstw, ForwardIterW lastw )
{
  double vs = 0.0;
  double w = 0.0;
  for ( ; ( firstx != lastx ) && ( firstw != lastw ); ++firstx, ++firstw ) {
    double ss = *firstx - mean;
    vs += ss * ss * (*firstw);
    w += (*firstw);
  }
  return w > 0.0 ? vs/w : 0.0;
}


template < typename ContainerX, typename ContainerW >
double wvariance( double mean, const ContainerX &vecx, const ContainerW &vecw )
{
  return wvariance( mean, vecx.begin(), vecx.end(), vecw.begin(), vecw.end() );
}


template < typename ForwardIterX >
double stdev( ForwardIterX firstx, ForwardIterX lastx )
{
  double a = 0.0;
  ForwardIterX iterx = firstx;
  int k=1;
  for ( ; iterx != lastx; ++iterx, ++k ) {
    a += ( *iterx - a ) / k;
  }

  double v = 0.0;
  if ( k > 2 ) {
    for ( k=1; firstx != lastx; ++firstx, ++k ) {
      double s = *firstx - a;
      v += ( s*s - v ) / k;
    }
    k--;
    v *= k/(k-1);
  }
  return ::sqrt( v );
}


template < typename ContainerX >
double stdev( const ContainerX &vecx )
{
  return stdev( vecx.begin(), vecx.end() );
}


template < typename ForwardIterX >
double stdev( double mean, ForwardIterX firstx, ForwardIterX lastx )
{
  double v = 0.0;
  int k=1;
  for ( ; firstx != lastx; ++firstx, ++k ) {
    double s = *firstx - mean;
    v += ( s*s - v ) / k;
  }
  if ( k > 2 ) {
    k--;
    v *= k/(k-1);
  }
  return ::sqrt( v );
}


template < typename ContainerX >
double stdev( double mean, const ContainerX &vecx )
{
  return stdev( mean, vecx.begin(), vecx.end() );
}


template < typename ForwardIterX >
double stdevFixed( double fixedmean, ForwardIterX firstx, ForwardIterX lastx )
{
  double v = 0.0;
  for ( int k=1; firstx != lastx; ++firstx, ++k ) {
    double s = *firstx - fixedmean;
    v += ( s*s - v ) / k;
  }
  return ::sqrt( v );
}


template < typename ContainerX >
double stdevFixed( double fixedmean, const ContainerX &vecx )
{
  return stdevFixed( fixedmean, vecx.begin(), vecx.end() );
}


template < typename ForwardIterX, typename ForwardIterW >
double wstdev( ForwardIterX firstx, ForwardIterX lastx,
	       ForwardIterW firstw, ForwardIterW lastw )
{
  double s = 0.0;
  double w = 0.0;
  ForwardIterX iterx = firstx;
  ForwardIterW iterw = firstw;
  int k=0;
  for ( ; ( iterx != lastx ) && ( iterw != lastw ); ++iterx, ++iterw, ++k ) {
    s += (*iterx);
    w += (*iterw);
  }
  double a = w > 0.0 ? s/w : 0.0;

  double v = 0.0;
  if ( k > 1 ) {
    double vs = 0.0;
    for ( ; ( firstx != lastx ) && ( firstw != lastw ); ++firstx, ++firstw ) {
      double ss = (*firstx) - a;
      vs += ss * ss * (*firstw);
    }
    v = w > 0.0 ? vs/w : 0.0;
  }
  return ::sqrt( v );
}


template < typename ContainerX, typename ContainerW >
double wstdev( const ContainerX &vecx, const ContainerW &vecw )
{
  return wstdev( vecx.begin(), vecx.end(), vecw.begin(), vecw.end() );
}


template < typename ForwardIterX, typename ForwardIterW >
double wstdev( double mean, ForwardIterX firstx, ForwardIterX lastx,
	       ForwardIterW firstw, ForwardIterW lastw )
{
  double vs = 0.0;
  double w = 0.0;
  for ( ; ( firstx != lastx ) && ( firstw != lastw ); ++firstx, ++firstw ) {
    double ss = *firstx - mean;
    vs += ss * ss * (*firstw);
    w += (*firstw);
  }
  return w > 0.0 ? ::sqrt( vs/w ) : 0.0;
}


template < typename ContainerX, typename ContainerW >
double wstdev( double mean, const ContainerX &vecx, const ContainerW &vecw )
{
  return wstdev( mean, vecx.begin(), vecx.end(), vecw.begin(), vecw.end() );
}


template < typename ForwardIterX >
double sem( ForwardIterX firstx, ForwardIterX lastx )
{
  double a = 0.0;
  ForwardIterX iterx = firstx;
  int k=1;
  for ( ; iterx != lastx; ++iterx, ++k ) {
    a += ( *iterx - a ) / k;
  }

  double v = 0.0;
  if ( k > 2 ) {
    for ( k=1; firstx != lastx; ++firstx, ++k ) {
      double s = *firstx - a;
      v += ( s*s - v ) / k;
    }
    k--;
    v *= 1.0/(k-1);
  }
  return ::sqrt( v );
}


template < typename ContainerX >
double sem( const ContainerX &vecx )
{
  return sem( vecx.begin(), vecx.end() );
}


template < typename ForwardIterX >
double sem( double mean, ForwardIterX firstx, ForwardIterX lastx )
{
  double v = 0.0;
  int k=1;
  for ( ; firstx != lastx; ++firstx, ++k ) {
    double s = *firstx - mean;
    v += ( s*s - v ) / k;
  }
  if ( k > 2 ) {
    k--;
    v *= 1.0/(k-1);
  }
  return ::sqrt( v );
}


template < typename ContainerX >
double sem( double mean, const ContainerX &vecx )
{
  return sem( mean, vecx.begin(), vecx.end() );
}


template < typename ForwardIterX >
double semFixed( double fixedmean, ForwardIterX firstx, ForwardIterX lastx )
{
  double v = 0.0;
  int k=1;
  for ( ; firstx != lastx; ++firstx, ++k ) {
    double s = *firstx - fixedmean;
    v += ( s*s - v ) / k;
  }
  if ( k > 1 ) {
    k--;
    v *= 1.0 / k;
  }
  return ::sqrt( v );
}


template < typename ContainerX >
double semFixed( double fixedmean, const ContainerX &vecx )
{
  return semFixed( fixedmean, vecx.begin(), vecx.end() );
}


template < typename ForwardIterX >
double absdev( ForwardIterX firstx, ForwardIterX lastx )
{
  double a = 0.0;
  ForwardIterX iterx = firstx;
  int k=1;
  for ( ; iterx != lastx; ++iterx, ++k ) {
    a += ( *iterx - a ) / k;
  }

  double v = 0.0;
  for ( k=1; firstx != lastx; ++firstx, ++k ) {
    double s = *firstx - a;
    v += ( ::fabs( s ) - v ) / k;
  }
  return v;
}


template < typename ContainerX >
double absdev( const ContainerX &vecx )
{
  return absdev( vecx.begin(), vecx.end() );
}


template < typename ForwardIterX >
double absdev( double mean, ForwardIterX firstx, ForwardIterX lastx )
{
  double v = 0.0;
  for ( int k=1; firstx != lastx; ++firstx, ++k ) {
    double s = *firstx - mean;
    v += ( ::fabs( s ) - v ) / k;
  }
  return v;
}


template < typename ContainerX >
double absdev( double mean, const ContainerX &vecx )
{
  return absdev( mean, vecx.begin(), vecx.end() );
}


template < typename ForwardIterX, typename ForwardIterW >
double wabsdev( ForwardIterX firstx, ForwardIterX lastx,
	       ForwardIterW firstw, ForwardIterW lastw )
{
  double s = 0.0;
  double w = 0.0;
  ForwardIterX iterx = firstx;
  ForwardIterW iterw = firstw;
  int k=0;
  for ( ; ( iterx != lastx ) && ( iterw != lastw ); ++iterx, ++iterw, ++k ) {
    s += (*iterx);
    w += (*iterw);
  }
  double a = w > 0.0 ? s/w : 0.0;

  double vs = 0.0;
  for ( ; ( firstx != lastx ) && ( firstw != lastw ); ++firstx, ++firstw ) {
    double ss = (*firstx) - a;
    vs += ::fabs( ss ) * (*firstw);
  }
  return w > 0.0 ? vs/w : 0.0;
}


template < typename ContainerX, typename ContainerW >
double wabsdev( const ContainerX &vecx, const ContainerW &vecw )
{
  return wabsdev( vecx.begin(), vecx.end(), vecw.begin(), vecw.end() );
}


template < typename ForwardIterX, typename ForwardIterW >
double wabsdev( double mean, ForwardIterX firstx, ForwardIterX lastx,
	       ForwardIterW firstw, ForwardIterW lastw )
{
  double vs = 0.0;
  double w = 0.0;
  for ( ; ( firstx != lastx ) && ( firstw != lastw ); ++firstx, ++firstw ) {
    double ss = *firstx - mean;
    vs += ::fabs( ss ) * (*firstw);
    w += (*firstw);
  }
  return w > 0.0 ? vs/w : 0.0;
}


template < typename ContainerX, typename ContainerW >
double wabsdev( double mean, const ContainerX &vecx, const ContainerW &vecw )
{
  return wabsdev( mean, vecx.begin(), vecx.end(), vecw.begin(), vecw.end() );
}


template < typename ForwardIterX >
double rms( ForwardIterX first, ForwardIterX last )
{
  double v = 0.0;
  for ( int k=1; first != last; ++first, ++k ) {
    double s = *first;
    v += ( s*s - v ) / k;
  }
  return ::sqrt( v );
}


template < typename Container >
double rms( const Container &vec )
{
  return rms( vec.begin(), vec.end() );
}


template < typename ForwardIterX >
double skewness( ForwardIterX first, ForwardIterX last )
{
  double a = 0.0;
  ForwardIterX iter = first;
  for ( int k=1; iter != last; ++iter, ++k ) {
    a += ( *iter - a ) / k;
  }

  double v = 0.0;
  double sk = 0.0;
  for ( int k=1; first != last; ++first, ++k ) {
    double s = *first - a;
    v += ( s*s - v ) / k;
    sk += ( s*s*s - sk ) / k;
  }
  v = ::sqrt( v );
  sk /= v*v*v;

  return sk;
}


template < typename Container >
double skewness( const Container &vec )
{
  return skewness( vec.begin(), vec.end() );
}


template < typename ForwardIterX >
double kurtosis( ForwardIterX first, ForwardIterX last )
{
  double a = 0.0;
  ForwardIterX iter = first;
  for ( int k=1; iter != last; ++iter, ++k ) {
    a += ( *iter - a ) / k;
  }

  double v = 0.0;
  double kt = 0.0;
  for ( int k=1; first != last; ++first, ++k ) {
    double s = *first - a;
    v += ( s*s - v ) / k;
    kt += ( s*s*s*s - kt ) / k;
  }
  kt /= v*v;
  kt -= 3.0;

  return kt;
}


template < typename Container >
double kurtosis( const Container &vec )
{
  return kurtosis( vec.begin(), vec.end() );
}


template < typename ForwardIterX >
double sum( ForwardIterX first, ForwardIterX last )
{
  if ( first == last )
    return 0.0;

  double sum = *first;
  while ( ++first != last ) {
    sum += *first;
  }
  return sum;
}


template < typename Container >
double sum( const Container &vec )
{
  return sum( vec.begin(), vec.end() );
}


template < typename ForwardIterX >
double squaredSum( ForwardIterX first, ForwardIterX last )
{
  if ( first == last )
    return 0.0;

  double sum = (*first)*(*first);
  while ( ++first != last ) {
    sum += (*first)*(*first);
  }
  return sum;
}


template < typename Container >
double squaredSum( const Container &vec )
{
  return squaredSum( vec.begin(), vec.end() );
}


template < typename ForwardIterX >
double magnitude( ForwardIterX first, ForwardIterX last )
{
  if ( first == last )
    return 0.0;

  double sum = (*first)*(*first);
  while ( ++first != last ) {
    sum += (*first)*(*first);
  }
  return ::sqrt( sum );
}


template < typename Container >
double magnitude( const Container &vec )
{
  return magnitude( vec.begin(), vec.end() );
}


template < typename ForwardIterX >
double power( ForwardIterX first, ForwardIterX last )
{
  double a = 0.0;
  for ( int k=1; first != last; ++first, ++k ) {
    a += ( (*first)*(*first) - a ) / k;
  }
  return a;
}


template < typename Container >
double power( const Container &vec )
{
  return power( vec.begin(), vec.end() );
}


template < typename ForwardIterX, typename ForwardIterY >
double dot( ForwardIterX firstx, ForwardIterX lastx,
            ForwardIterY firsty, ForwardIterY lasty )
{
  ForwardIterX iterx = firstx;
  ForwardIterY itery = firsty;
  double d = 0.0;
  while ( iterx != lastx && itery != lasty ) {
    d += (*iterx) * (*itery);
    ++iterx;
    ++itery;
  }

  return d;
}


template < typename ContainerX, typename ContainerY >
double dot( const ContainerX &vecx, const ContainerY &vecy )
{
  return dot( vecx.begin(), vecx.end(), vecy.begin(), vecy.end() );
}


template < typename ContainerX, typename ContainerY >
void average( ContainerX &x, const vector< ContainerY > &y )
{
  vector< typename ContainerY::value_type > buf( y.size() );
  for ( int k=0; k<x.size(); k++ ) {
    for ( unsigned int j=0; j<y.size(); j++ )
      buf[j] = y[j][k];
    x[k] = mean( buf );
  }
}


template < typename ContainerX, typename ContainerS, typename ContainerY >
void average( ContainerX &x, ContainerS &s, const vector< ContainerY > &y )
{
  vector< typename ContainerY::value_type > buf( y.size() );
  for ( int k=0; k<x.size(); k++ ) {
    for ( unsigned int j=0; j<y.size(); j++ )
      buf[j] = y[j][k];
    x[k] = meanStdev( s[k], buf );
  }
}


template < typename ForwardIterX, typename ForwardIterY >
double cov( ForwardIterX firstx, ForwardIterX lastx,
            ForwardIterY firsty, ForwardIterY lasty )
{
  ForwardIterX iterx = firstx;
  ForwardIterY itery = firsty;
  double a1 = 0.0;
  double a2 = 0.0;
  int k = 1;
  for ( ; iterx != lastx && itery != lasty; k++ ) {
    a1 += ( *iterx - a1 )/k;
    a2 += ( *itery - a2 )/k;
    ++iterx;
    ++itery;
  }

  iterx = firstx;
  itery = firsty;
  double cv = 0.0;
  for ( k=1; iterx != lastx && itery != lasty; k++ ) {
    cv += ( (*iterx - a1)*(*itery - a2) - cv )/k;
    ++iterx;
    ++itery;
  }

  return cv * (k/(k-1));
}


template < typename ContainerX, typename ContainerY >
double cov( const ContainerX &vecx, const ContainerY &vecy )
{
  return cov( vecx.begin(), vecx.end(), vecy.begin(), vecy.end() );
}


template < typename ForwardIterX, typename ForwardIterY >
double corrCoef( ForwardIterX firstx, ForwardIterX lastx,
		 ForwardIterY firsty, ForwardIterY lasty )
{
  ForwardIterX iterx = firstx;
  ForwardIterY itery = firsty;
  double a1 = 0.0;
  double a2 = 0.0;
  for ( int k=1; iterx != lastx && itery != lasty; k++ ) {
    a1 += ( *iterx - a1 )/k;
    a2 += ( *itery - a2 )/k;
    ++iterx;
    ++itery;
  }

  iterx = firstx;
  itery = firsty;
  double v1 = 0.0;
  double v2 = 0.0;
  double cv = 0.0;
  for ( int k=1; iterx != lastx && itery != lasty; k++ ) {
    double s1 = *iterx - a1;
    double s2 = *itery - a2;
    v1 += ( s1*s1 - v1 )/k;
    v2 += ( s2*s2 - v2 )/k;
    cv += ( s1*s2 - cv )/k;
    ++iterx;
    ++itery;
  }

  double s12 = ::sqrt(v1*v2);

  return s12 > 0.0 ? cv / s12 : 0.0;
}


template < typename ContainerX, typename ContainerY >
double corrCoef( const ContainerX &vecx, const ContainerY &vecy )
{
  return corrCoef( vecx.begin(), vecx.end(), vecy.begin(), vecy.end() );
}


template < typename ForwardIterX, typename ForwardIterY, typename ForwardIterW >
double wcorrCoef( ForwardIterX firstx, ForwardIterX lastx,
		  ForwardIterY firsty, ForwardIterY lasty,
		  ForwardIterW firstw, ForwardIterW lastw )
{
  ForwardIterX iterx = firstx;
  ForwardIterY itery = firsty;
  ForwardIterW iterw = firstw;
  double s1 = 0.0;
  double s2 = 0.0;
  double w = 0.0;
  for ( int k=1; (iterx != lastx) && (itery != lasty) && (iterw != lastw); k++ ) {
    s1 += (*iterx) * (*iterw);
    s2 += (*itery) * (*iterw);
    w += (*iterw);
    ++iterx;
    ++itery;
    ++iterw;
  }
  double a1 = w > 0.0 ? s1/w : 0.0;
  double a2 = w > 0.0 ? s2/w : 0.0;

  iterx = firstx;
  itery = firsty;
  iterw = firstw;
  double v1 = 0.0;
  double v2 = 0.0;
  double cv = 0.0;
  for ( int k=1; (iterx != lastx) && (itery != lasty) && (iterw != lastw); k++ ) {
    double ss1 = *iterx - a1;
    double ss2 = *itery - a2;
    v1 += ss1 * ss1 * (*iterw);
    v2 += ss2 * ss2 * (*iterw);
    cv += ss1 * ss2 * (*iterw);
    ++iterx;
    ++itery;
    ++iterw;
  }

  double s12 = ::sqrt(v1*v2);

  return s12 > 0.0 ? cv / s12 : 0.0;
}


template < typename ContainerX, typename ContainerY, typename ContainerW >
double wcorrCoef( const ContainerX &vecx, const ContainerY &vecy, 
		  const ContainerW &vecw )
{
  return wcorrCoef( vecx.begin(), vecx.end(), vecy.begin(), vecy.end(),
		    vecw.begin(), vecw.end() );
}


template < typename ForwardIterX, typename ForwardIterY, typename ForwardIterS >
double scorrCoef( ForwardIterX firstx, ForwardIterX lastx,
		  ForwardIterY firsty, ForwardIterY lasty,
		  ForwardIterS firsts, ForwardIterS lasts )
{
  ForwardIterX iterx = firstx;
  ForwardIterY itery = firsty;
  ForwardIterS iters = firsts;
  double s1 = 0.0;
  double s2 = 0.0;
  double w = 0.0;
  for ( int k=1; (iterx != lastx) && (itery != lasty) && (iters != lasts); k++ ) {
    double d = (*iters);
    if ( d < 1.0e-10 ) 
      d = 1.0e-10;
    double i = 1.0/(d*d);
    s1 += (*iterx) * i;
    s2 += (*itery) * i;
    w += i;
    ++iterx;
    ++itery;
    ++iters;
  }
  double a1 = w > 0.0 ? s1/w : 0.0;
  double a2 = w > 0.0 ? s2/w : 0.0;

  iterx = firstx;
  itery = firsty;
  iters = firsts;
  double v1 = 0.0;
  double v2 = 0.0;
  double cv = 0.0;
  for ( int k=1; (iterx != lastx) && (itery != lasty) && (iters != lasts); k++ ) {
    double d = (*iters);
    if ( d < 1.0e-10 ) 
      d = 1.0e-10;
    double i = 1.0/(d*d);
    double ss1 = *iterx - a1;
    double ss2 = *itery - a2;
    v1 += ss1 * ss1 * i;
    v2 += ss2 * ss2 * i;
    cv += ss1 * ss2 * i;
    ++iterx;
    ++itery;
    ++iters;
  }

  double s12 = ::sqrt(v1*v2);

  return s12 > 0.0 ? cv / s12 : 0.0;
}


template < typename ContainerX, typename ContainerY, typename ContainerS >
double scorrCoef( const ContainerX &vecx, const ContainerY &vecy, 
		  const ContainerS &vecs )
{
  return scorrCoef( vecx.begin(), vecx.end(), vecy.begin(), vecy.end(),
		    vecs.begin(), vecs.end() );
}


template < typename ForwardIterX, typename ForwardIterY >
double chisq( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty )
{
  double ch = 0.0;
  while ( (firstx != lastx) && (firsty != lasty) ) {
    double s = (*firstx) - (*firsty);
    ch += s*s;
    ++firstx;
    ++firsty;
  }
  return ch;
}


template < typename ContainerX, typename ContainerY >
double chisq( const ContainerX &vecx, const ContainerY &vecy )
{
  return chisq( vecx.begin(), vecx.end(), vecy.begin(), vecy.end() );
}


template < typename ForwardIterX, typename ForwardIterY, typename ForwardIterS >
double chisq( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty,
	      ForwardIterS firsts, ForwardIterS lasts )
{
  double ch = 0.0;
  while ( (firstx != lastx) && (firsty != lasty) && (firsts != lasts) ) {
    double s = ( (*firstx) - (*firsty) ) / (*firsts);
    ch += s*s;
    ++firstx;
    ++firsty;
    ++firsts;
  }
  return ch;
}


template < typename ContainerX, typename ContainerY, typename ContainerS >
double chisq( const ContainerX &vecx, const ContainerY &vecy, 
	      const ContainerS &vecs )
{
  return chisq( vecx.begin(), vecx.end(), vecy.begin(), vecy.end(),
		vecs.begin(), vecs.end() );
}


template < typename ForwardIterX, typename ForwardIterY >
void serialCorr( ForwardIterX firstx, ForwardIterX lastx,
		 ForwardIterY firsty, ForwardIterY lasty )
{
  double stdev = 0.0;
  double a = meanStdev( stdev, firstx, lastx );
  double var = stdev * stdev;

  ForwardIterX firstx2 = firstx;
  ForwardIterY itery = firsty;

  if ( stdev / a < 1.0e-8 ) {
    *itery = 1.0;
    ++itery;
    while ( itery != lasty ) {
      *itery = 0.0;
      ++itery;
    }
    return;
  }

  while ( itery != lasty && firstx2 != lastx ) {

    ForwardIterX iterx1 = firstx;
    ForwardIterX iterx2 = firstx2;
    double covar = 0.0;
    int k = 1;
    for ( k=1; iterx1 != lastx && iterx2 != lastx; k++ ) {
      double s1 = *iterx1 - a;
      double s2 = *iterx2 - a;
      covar += ( s1*s2 - covar )/k;
      ++iterx1;
      ++iterx2;
    }

    *itery = var > 0.0 ? covar / var : 0.0;
    ++itery;

    ++firstx2;
  }

  while ( itery != lasty ) {
    *itery = 0.0;
    ++itery;
  }

}


template < typename ContainerX, typename ContainerY >
void serialCorr( const ContainerX &vecx, ContainerY &vecy )
{
  serialCorr( vecx.begin(), vecx.end(), vecy.begin(), vecy.end() );
}


template < typename ForwardIterX, typename ForwardIterY >
void propFit( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty,
	      double &m, double &mu, double &chisq )
{
  m = 0.0;
  mu = 0.0;
  chisq = -1.0;

  int nn = 0;
  double sxx=0.0, sxy=0.0;
  ForwardIterX iterx = firstx;
  ForwardIterY itery = firsty;
  while ( iterx != lastx && itery != lasty ) {
    ++nn;
    double x = *iterx;
    double y = *itery;
    sxx += ( x*x - sxx ) / nn;
    sxy += ( x*y - sxy ) / nn;
    ++iterx;
    ++itery;
  }

  if ( nn < 1 ||
       sxx < 1.0e-8 )
    return;

  m = sxy / sxx;
  mu = sqrt( 1.0 / sxx / nn );
  chisq = 0.0;
  iterx = firstx;
  itery = firsty;
  while ( iterx != lastx && itery != lasty ) {
    double v = *itery - m * *iterx; 
    chisq += v*v;
    ++iterx;
    ++itery;
  }
  if ( nn > 1 )
    mu *= sqrt( chisq / (nn-1) );
  else 
    mu = 0.0;
}


template < typename ContainerX, typename ContainerY >
void propFit( const ContainerX &vecx, const ContainerY &vecy,
	      double &m, double &mu, double &chisq )
{
  propFit( vecx.begin(), vecx.end(), vecy.begin(), vecy.end(),
	   m, mu, chisq );
}


template < typename ForwardIterX, typename ForwardIterY, typename ForwardIterS >
void propFit( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty,
	      ForwardIterS firsts, ForwardIterS lasts,
	      double &m, double &mu, double &chisq )
{
  m = 0.0;
  mu = 0.0;
  chisq = -1.0;

  int nn = 0;
  double sxx=0.0, sxy=0.0;
  ForwardIterX iterx = firstx;
  ForwardIterY itery = firsty;
  ForwardIterS iters = firsts;
  while ( iterx != lastx && itery != lasty && iters != lasts ) {
    ++nn;
    double x = *iterx;
    double y = *itery;
    double s = *iters;
    double ss = s*s;
    sxx += ( x*x/ss - sxx ) / nn;
    sxy += ( x*y/ss - sxy ) / nn;
    ++iterx;
    ++itery;
    ++iters;
  }

  if ( nn < 1 ||
       sxx < 1.0e-8 )
    return;

  m = sxy / sxx;
  mu = sqrt( 1.0 / sxx / nn );
  chisq = 0.0;
  iterx = firstx;
  itery = firsty;
  iters = firsts;
  while ( iterx != lastx && itery != lasty && iters != lasts ) {
    double v = ( *itery - m * *iterx ) / *iters; 
    chisq += v*v;
    ++iterx;
    ++itery;
    ++iters;
  }
}


template < typename ContainerX, typename ContainerY, typename ContainerS >
void propFit( const ContainerX &vecx, const ContainerY &vecy, const ContainerS &vecs,
	      double &m, double &mu, double &chisq )
{
  propFit( vecx.begin(), vecx.end(), vecy.begin(), vecy.end(),
	   vecs.begin(), vecs.end(), m, mu, chisq );
}


template < typename ForwardIterX, typename ForwardIterY >
void lineFit( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty,
	      double &b, double &bu, 
	      double &m, double &mu, double &chisq )
{
  // init values:
  b = 0.0;
  bu = 0.0;
  m = 0.0;
  mu = 0.0;
  chisq = -1.0;

  int nn = 0;
  double sx=0.0, sy=0.0;
  ForwardIterX iterx = firstx;
  ForwardIterY itery = firsty;
  while ( iterx != lastx && itery != lasty ) {
    ++nn;
    sx += ( *iterx - sx )/nn;
    sy += ( *itery - sy )/nn;
    ++iterx;
    ++itery;
  }

  // not enough data points:
  if ( nn < 2 )
    return;

  double st2=0.0;
  iterx = firstx;
  itery = firsty;
  while ( iterx != lastx && itery != lasty ) {
    double t = (*iterx) - sx;
    st2 += t * t;
    m += t * (*itery);
    ++iterx;
    ++itery;
  }
  if ( st2 < 1.0e-8 ) {
    b = 0.0;
    m = 0.0;
    return;
  }
  m /= st2;
  b = sy - sx*m;
  bu = sqrt( 1.0/nn + sx*sx/st2 );
  mu = sqrt( 1.0/st2 );
  chisq = 0.0;
  iterx = firstx;
  itery = firsty;
  while ( iterx != lastx && itery != lasty ) {
    double v = *itery - b - m * (*iterx); 
    chisq += v*v;
    ++iterx;
    ++itery;
  }
  
  double sigdat = nn > 2 ? sqrt( chisq / (nn-2) ) : 0.0;
  bu *= sigdat;
  mu *= sigdat;
}


template < typename ContainerX, typename ContainerY >
void lineFit( const ContainerX &vecx, const ContainerY &vecy,
	      double &b, double &bu, 
	      double &m, double &mu, double &chisq )
{
  lineFit( vecx.begin(), vecx.end(), vecy.begin(), vecy.end(),
	   b, bu, m, mu, chisq );
}


template < typename ForwardIterX, typename ForwardIterY, typename ForwardIterS >
void lineFit( ForwardIterX firstx, ForwardIterX lastx,
	      ForwardIterY firsty, ForwardIterY lasty,
	      ForwardIterS firsts, ForwardIterS lasts,
	      double &b, double &bu, 
	      double &m, double &mu, double &chisq )
{
  // init values:
  b = 0.0;
  bu = 0.0;
  m = 0.0;
  mu = 0.0;
  chisq = -1.0;

  int nn = 0;
  double ss=0.0, sx=0.0, sy=0.0;
  ForwardIterX iterx = firstx;
  ForwardIterY itery = firsty;
  ForwardIterS iters = firsts;
  while ( iterx != lastx && itery != lasty && iters != lasts ) {
    ++nn;
    double wt = 1.0 / ( (*iters) * (*iters) );
    ss += wt;
    sx += (*iterx)*wt;
    sy += (*itery)*wt;
    ++iterx;
    ++itery;
    ++iters;
  }

  // not enough data points:
  if ( nn < 2 )
    return;

  double sxdss = sx/ss;
  double st2=0.0;
  iterx = firstx;
  itery = firsty;
  iters = firsts;
  while ( iterx != lastx && itery != lasty && iters != lasts ) {
    double t = ( *iterx - sxdss ) / (*iters);
    st2 += t*t;
    m += t * (*itery) / (*iters);
    ++iterx;
    ++itery;
    ++iters;
  }

  if ( st2 < 1.0e-8 ) {
    b = 0.0;
    m = 0.0;
    return;
  }
  m /= st2;
  b = (sy - sx*m)/ss;
  bu = sqrt( (1.0 + (sx/ss)*(sx/st2))/ss );
  mu = sqrt( 1.0/st2 );

  chisq = 0.0;
  iterx = firstx;
  itery = firsty;
  iters = firsts;
  while ( iterx != lastx && itery != lasty && iters != lasts ) {
    double v = ( *itery - b - m * *iterx ) / *iters; 
    chisq += v*v;
    ++iterx;
    ++itery;
    ++iters;
  }
}


template < typename ContainerX, typename ContainerY, typename ContainerS >
void lineFit( const ContainerX &vecx, const ContainerY &vecy, const ContainerS &vecs,
	      double &b, double &bu, 
	      double &m, double &mu, double &chisq )
{
  lineFit( vecx.begin(), vecx.end(), vecy.begin(), vecy.end(),
	   vecs.begin(), vecs.end(), b, bu, m, mu, chisq );
}


};


template < typename ForwardIterX >
void detrend( ForwardIterX firstx, ForwardIterX lastx )
{
  int nn = 0;
  double dc = 0.0;
  double slope = 0.0;
  ForwardIterX iterx = firstx;
  while ( iterx != lastx ) {
    ++nn;
    dc += (*iterx);
    slope += (*iterx) * nn;
    ++iterx;
  }
  dc /= (double)nn;
  slope *= 12.0 / ( nn * ( nn * (double)nn - 1.0 ) );
  slope -= 6.0 * dc / ( nn - 1.0 );
  double fln = dc - 0.5 * ( nn + 1.0 ) * slope;

  nn = 0;
  while ( firstx != lastx ) {
    ++nn;
    (*firstx) -= nn*slope + fln;
    ++firstx;
  }
}


template < typename ContainerX >
void detrend( ContainerX &vecx )
{
  detrend( vecx.begin(), vecx.end() );
}

#endif /* ! _RELACS_STATS_H_ */
