/*
  ../include/relacs/rangeloop.h
  A flexible and sophisticated way to loop trough a range of values.

  RELACS - RealTime ELectrophysiological data Acquisition, Control, and Stimulation
  Copyright (C) 2002-2007 Jan Benda <j.benda@biologie.hu-berlin.de>

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

#ifndef _RANGELOOP_H_
#define _RANGELOOP_H_


#include <vector>
using namespace std;


/*!
  \class RangeLoop
  \author Jan Benda
  \version 1.2
  \brief A flexible and sophisticated way to loop trough a range of values.
  \todo set( string ): introduce control characters for controlling the addMode


  \a pos: is the index of an element
  \a index: is the index of the current sequence
 */

class RangeLoop
{
 public:

    /*! Different sequences for looping through the data. */
  enum Sequence {
      /*! Traverse trough the data values in the order they are stored. */
    Up=0,
      /*! Traverse reversed trough the data values in the order they are stored. */
    Down=1,
      /*! Traverse upwards trough the data values alternating
	  from the up and down sequence starting with the outermost values. */
    Alternate=2,
      /*! Same as Alternate */
    AlternateIn=Alternate,
      /*! Same as Alternate */
    AlternateInUp=Alternate, 
      /*! Traverse downwards trough the data values alternating
	  from the up and down sequence starting with the outermost values. */
    AlternateDown=3,
      /*! Same as AlternateDown */
    AlternateInDown=AlternateDown,
      /*! Traverse upwards trough the data values alternating
	  from the up and down sequence starting with the innermost values. */
    AlternateOut=4,
      /*! Same as AlternateOut */
    AlternateOutUp=AlternateOut, 
      /*! Traverse downwards trough the data values alternating
	  from the up and down sequence starting with the innermost values. */
    AlternateOutDown=5, 
      /*! Traverse randomly through the data values with the random seed
	  set to the current time. */
    Random=6,
      /*! Traverse randomly through the data values
 	  with always the same random seed. */
    PseudoRandom=7
  };

    /*! Different ways how addition of already existing values is handled. */
  enum AddMode {
      /*! Simply add the value, no matter whether it is already 
          contained in the range or not. */
    Add,
      /*! Don't add the value if it is already contained in the list. */
    Skip,
      /*! Don't add the value if it is already contained in the list
	  and remove the existing entries as well. */
    RemoveAll,
      /*! Add the value and remove any already existing entries. */
    AddRemove
  };

  RangeLoop( void );
  RangeLoop( double first, double last, double step,
	     int repeat=1, int blockrepeat=1, int singlerepeat=1,
	     int increment=1 );
  RangeLoop( double first, double last, int n,
	     int repeat=1, int blockrepeat=1, int singlerepeat=1,
	     int increment=1 );
  RangeLoop( double value, int size=1, 
	     int repeat=1, int blockrepeat=1, int singlerepeat=1,
	     int increment=1 );
  RangeLoop( const string &range );
  ~RangeLoop( void );

    /*! The number of elements. */
  int size( void ) const;
    /*! True if the range is empty. */
  bool empty( void ) const;
    /*! Resize the number of elements in the buffer and indices to \a newsize
        and initialize new elements with \a dflt. . */
  void resize( int newsize, double dflt=0.0 );
    /* Empty the range. */
  void clear( void );

    /*! The maximum number of elements for which memory is allocated. */
  int capacity( void ) const;
    /*! Allocate memory for \a newsize elements and indices. */
  void reserve( int newsize );

    /*! Initialize the range with first value \a first, last value \a last,
        and increment value \a step.
        The whole sequence is repeated \a repeat times.
	Each subsets of data elements for a given increment is repeated \a blockrepeat times.
        Each data element is repeated \a singlerepeat times.
        The initial increment is \a increment indices. */
  void set( double first, double last, double step,
	    int repeat=1, int blockrepeat=1, int singlerepeat=1,
	    int increment=1 );
    /*! Add the range with first value \a first, last value \a last,
        and increment value \a step to the range. */
  void add( double first, double last, double step );
    /*! Initialize the range with first value \a first, last value \a last,
        and increment factor \a fac.
        The whole sequence is repeated \a repeat times.
	Each subsets of data elements for a given increment is repeated \a blockrepeat times.
        Each data element is repeated \a singlerepeat times.
        The initial increment is \a increment indices. */
  void setLog( double first, double last, double fac,
	       int repeat=1, int blockrepeat=1, int singlerepeat=1,
	       int increment=1 );
    /*! Add the range with first value \a first, last value \a last,
        increment factor \a fac to the range. */
  void addLog( double first, double last, double fac );

    /*! Initialize the range with \a n evenly spaced values starting with
        the first value \a first and ending with the last value \a last.
        The sequence is repeated \a repeat times.
	Each subsets of data elements for a given increment is repeated \a blockrepeat times.
        Each data element is repeated \a singlerepeat times.
        The initial increment is \a increment indices. */
  void set( double first, double last, int n,
	    int repeat=1, int blockrepeat=1, int singlerepeat=1,
	    int increment=1 );
    /*! Add a range with \a n evenly spaced values starting with
        the first value \a first and ending with the last value \a last
	to the range. */
  void add( double first, double last, int n );
    /*! Initialize the range with \a n logarithmically spaced values 
        starting with the first value \a first
	and ending with the last value \a last.
        The sequence is repeated \a repeat times.
	Each subsets of data elements for a given increment is repeated \a blockrepeat times.
        Each data element is repeated \a singlerepeat times.
        The initial increment is \a increment indices. */
  void setLog( double first, double last, int n,
	       int repeat=1, int blockrepeat=1, int singlerepeat=1,
	       int increment=1 );
    /*! Add a range with \a n logarithmically spaced values starting with
        the first value \a first and ending with the last value \a last
	to the range. */
  void addLog( double first, double last, int n );

    /*! Initialize the range with a single value \a value
        and allocate memory for \a size elements.
        The sequence is repeated \a repeat times.
	Each subsets of data elements for a given increment is repeated \a blockrepeat times.
        Each data element is repeated \a singlerepeat times.
        The initial increment is \a increment indices. */
  void set( double value, int size=1,
	    int repeat=1, int blockrepeat=1, int singlerepeat=1,
	    int increment=1 );
    /*! Add the single value \a value to the range.
        If the value already exist, then \a value is removed from the range.
        Returns \a true if \a value was added to the range,
        and \a false if it was not added. */
  bool add( double value );

    /*! Initialize the range as defined by \a range. 
        \a range is a comma separated list of numbers, ranges, and specifiers.
        Numbers and ranges are added to the range in the order they appear in \a range.
        Linear ranges are defined by a minimum and a maximum value, 
        and an optional step size, separated by \c "..".
        For example, \c "-1,10..16..2" defines the following range
        of values: -1, 10, 12, 14, 16.
        Logarithmic ranges are defined by a minimum and a maximum value
	separated by \c "..", 
        and an optional increment factor, separated by \c "*".
        For example, \c "2..16..2" defines the following range
        of values: 2, 4, 8, 16.
        The order is specified by a separate string 
        ('up' (default), 'down', 'random', 'alternateinup', etc)
        and the initial increment by 'i:xx', where xx is an integer.
        For example 4..12..2,down,i:2 generates 12, 8, 4, 10, 6.
        All the values of the range are rescaled with \a scale. */
  void set( const string &range, double scale=1.0 );
    /*! Initialize the range as defined by \a range.
        \sa set(const string &range ) */
  const RangeLoop &operator=( const string &range );

    /*! Returns the current method that is used for adding 
        already existing data vales to the range. */
  AddMode addMode( void ) const;
    /*! Set the method that is used for adding 
        already existing data vales to the range to \a addmode. */
  void setAddMode( AddMode addmode );

    /*! The number of repetitions for the whole sequence. */
  inline int repeat( void ) const { return Repeat; };
    /*! Set the number of repetitions for the whole sequence to \a repeat. */
  inline void setRepeat( int repeat ) { Repeat = repeat; };
    /*! Return the number of executed repetitions for the whole sequence to \a repeat. */
  inline int currentRepetition( void ) const { return RepeatCount; };

    /*! The number of repetitons for a block of data elements
        of a single increment. */
  int blockRepeat( void ) const;
    /*! Set the number of repetitons for a block of data elements
        of a single increment to \a repeat. */
  void setBlockRepeat( int repeat );
    /*! Return the number of executed repetitions for a block of data elements
        of a single increment. */
  int currentBlockRepetition( void ) const;
    /*! Returns true if the current block repetitions are completed. */
  bool finishedBlock( void ) const;

    /*! The number of repetitons for a single data element. */
  int singleRepeat( void ) const;
    /*! Set the number of repetitons for a single data element to \a repeat. */
  void setSingleRepeat( int repeat );
    /*! Return the number of executed repetitions for a single data element. */
  int currentSingleRepetition( void ) const;
    /*! Returns true if the current single repetitions are completed,
        i.e. the next data element is to be started. */
  bool finishedSingle( void ) const;
    /*! Returns true if this is the last single repetition. */
  bool lastSingle( void ) const;

    /*! Set the initial increment to \a increment indices. */
  void setIncrement( int increment=1 );
    /*! Set the initial increment to the largest power of two
        less or equal than half the number of the data elements. */
  void setLargeIncrement( void );
    /*! Return the current increment. */
  int currentIncrement( void ) const;

    /*! The maximum possible number of repetitions of a single data element. */
  int maxCount( void ) const;
    /*! The maximum possible number of repetitions of a single data element
        for the current sequence. */
  int maxBlockCount( void ) const; 

    /*! Set the sequence for looping through the data values to \a seq. */
  void setSequence( Sequence seq );
  void up( void );
  void down( void );
  void alternateInUp( void );
  void alternateInDown( void );
  void alternateOutUp( void );
  void alternateOutDown( void );
  void random( void );
  void pseudoRandom( void );

    /*! Returns a string with the names of possible sequence types
        according to Sequence separated by '|'. */
  static const string &sequenceStrings( void );

    /*! Number of elements in the sequence. */
  int sequenceSize( void ) const;

    /*! Reset the range.
        Set index and repeat counters to zero,
        reset current increment,
        set the counts of the data elements to zero,
        and clear the skip flags.
        A new sequence of indices is generated
	starting with the one nearest to \a pos.
        If \a pos is negative (default) it is set to an appropriate value. */
  void reset( int pos=-1 );

    /*! Reset the sequence. */
  const RangeLoop &operator=( int pos );
    /*! Increment the current index of the sequence by one. */
  const RangeLoop &operator++( void );
    /*! True if the RangeLoop is currently pointing
        to an existing data element. */
  bool operator!( void ) const;
    /*! Call this function either if you set some skip flags
        to regenerate the sequence
	or if you want a different start position for the sequence.
        \a pos = -2: keep using the previously set start position.
        \a pos = -1: use the default start position.
        \a >= 0: use \a pos as the start position. */
  void update( int pos=-2 );
    /*! Number of increments since last call of reset(). */
  int loop( void ) const;

    /*! Return the value of the current data element. */
  double value( void ) const;
    /*! Return the value of the current data element. */
  double operator*( void ) const;
    /*! Return the position of the current data element. */
  int pos( void ) const;
    /*! Return the current index of the sequence. */
  int index( void ) const;
    /*! Return the count of the current data element. */
  int count( void ) const;
    /*! Decrement the count of the current data element. 
        Use it before calling operator++() if you do not
        want the count of the current data element to be incremented. */
  void noCount( void );
    /*! Set the skipping behavior of the current data element to \a skip.
        If \a skip is true (default) this data element will be skipped
        in future sequences. */
  void setSkip( bool skip=true );

    /*! Return the value of the data element at position \a pos. */
  double value( int pos ) const;
    /*! Return the value of the data element at position \a pos. */
  double operator[]( int pos ) const;
    /*! Return a reference to the value of the data element at position \a pos. */
  double &operator[]( int pos );
    /*! Return the value of the first data element. */
  double front( void ) const;
    /*! Return a reference to the value of the first data element. */
  double &front( void );
    /*! Return the value of the last data element. */
  double back( void ) const;
    /*! Return a reference to the value of the last data element. */
  double &back( void );

    /*! Return the minimum value of the range. */
  double minValue( void ) const;
    /*! Return the maximum value of the range. */
  double maxValue( void ) const;
    /*! Return the step size which is computed as
        \c (maxValue()-minValue())/(size()-1) */
  double step( void ) const;

    /*! Return the count of the data element at position \a pos. */
  int count( int pos ) const;
    /*! Return true if the data element at position \a pos is to be skipped. */
  int skip( int pos ) const;
    /*! Set the skipping behavior of the data element a position \a pos
        to \a skip.
        If \a skip is true (default) this data element will be skipped
        in future sequences. */
  void setSkip( int pos, bool skip=true );
    /*! Set the skipping behavior of all data elements below position \a pos inclusively
        to \a skip.
        If \a skip is true (default) these data elements will be skipped
        in future sequences. */
  void setSkipBelow( int pos, bool skip=true );
    /*! Set the skipping behavior of all data elements above position \a pos inclusively
        to \a skip.
        If \a skip is true (default) these data elements will be skipped
        in future sequences. */
  void setSkipAbove( int pos, bool skip=true );
    /*! Set the skipping behavior of all data elements above position \a pos1 inclusively
        and below position \a pos2 inclusively
        to \a skip.
        If \a skip is true (default) these data elements will be skipped
        in future sequences. */
  void setSkipBetween( int pos1, int pos2, bool skip=true );

    /*! Returns the position of the data element next or equal to position \a pos
        with count larger than zero.
        If there isn't any such element, size() is returned. */ 
  int next( int pos ) const;
    /*! Returns the position of the data element previous or equal to position \a pos
        with count larger than zero.
        If there isn't any such element, -1 is returned. */ 
  int previous( int pos ) const;

    /*! Return the position of the data element at index \a index. */
  int pos( int index ) const;
    /*! Return the position of the data element whose value is closest to \a value. */
  int pos( double value ) const;
    /*! Return the index of the data element at position \a pos.
        If there is no index for that data element, 
        -1 is retunred. */
  int index( int pos ) const; 

    /*! Returns true if the data element at position \a p is the active one. */
  bool active( int p ) const;
    /*! Returns true if \a value is closest to the value of the
        data element with position \a pos. */
  bool near( int pos, double value ) const;

    /*! Write the content of the range to \a str. */
  friend ostream &operator<< ( ostream &str, const RangeLoop &rl );

 
 private:

  void initSequence( int pos );

  static string SequenceStrings;

  struct ElementType
  {
    ElementType( void ) : Value( 0.0 ), Count( 0 ), Skip( false ) {};
    ElementType( double value, int count=0, bool skip=false ) 
      : Value( value ), Count( count ), Skip( skip ) {};
    ElementType( const ElementType &e ) 
      : Value( e.Value ), Count( e.Count ), Skip( e.Skip ) {};
    ~ElementType( void ) {};

    double Value;
    int Count;
    bool Skip;
  };

  vector < ElementType > Elements;

  vector < int > Indices;

  int Index;
  int StartPos;
  int Loop;

  int Repeat;
  int RepeatCount;

  int BlockRepeat;
  int BlockRepeatCount;
  
  int SingleRepeat;
  int SingleRepeatCount;

  int Increment;
  int CurrentIncrement;

  Sequence Seq;

  AddMode AddMethod;

};


#endif
