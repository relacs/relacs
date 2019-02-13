/*
  rangeloop.h
  A flexible and sophisticated way to loop trough a range of values.

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

#ifndef _RELACS_RANGELOOP_H_
#define _RELACS_RANGELOOP_H_ 1


#include <vector>
#include <string>
using namespace std;

namespace relacs {


/*!
  \class RangeLoop
  \author Jan Benda
  \brief A flexible and sophisticated way to loop trough a range of values.
  \todo set( string ): introduce control characters for controlling the addMode

  RangeLoop is a one-dimensional array of doubles. The array can be filled
  by the constructors or by the set() and add() functions. All the standard
  vector functions for accessing and manipulating the data values are provided:
  operator[], back(), front(), size(), empty(), resize(), clear(),
  capacity(), reserve().

  Now, RangeLoop provides a couple of features for looping through this
  array.
  The basic usage is like this:
  \code
  RangeLoop range( 1.0, 4.0, 0.5 );
  for ( range.reset(); ! range; ++range )
    cout << *range << '\n';
  \endcode
  This will print the sequence 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0
  on the screen.

  You can specify in which order the array is traversed by setSequence()
  or by calling one of the functions
  up(), down(), alternateInUp(), alternateInDown(), alternateOutUp(),
  alternateOutDown(), random(), pseudoRandom(). They will produce the
  following sequences:
  - up(): 1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0
  - down(): 4.0, 3.5, 3.0, 2.5, 2.0, 1.5, 1.0
  - alternateInUp(): 1.0, 4.0, 1.5, 3.5, 2.0, 3.0, 2.5
  - alternateInDown(): 5.0, 1.0, 3.5, 1.5, 3.0, 2.0, 2.5
  - alternateOutUp(): 2.5, 3.0, 2.0, 3.5, 1.5, 4.0, 1.0
  - alternateOutDown(): 2.5, 2.0, 3.0, 1.5, 3.5, 1.0, 4.0
  - random(): a randomized sequence that will differ for each repetition
  - pseudoRandom(): a randomized sequence that will always be the same
  
  How often a single data value is immediately 
  repeated can be specified by setSingleRepeat(),
  the number of repetitions of the whole sequence is controlled by
  setRepeat(). The latter can be zero, indicating that the whole sequence
  should be repeated indefinitely.

  By specifying an increment by setIncrement(), you can create sub-sequences
  of reduced resolutions that are traversed first.
  The number of repetitions of each of the sub-sequences are set by setBlockRepeat().
  After a sub-sequence is finished, the increment is halfed, and a new
  sub-sequence is created.
  This is continued until all data-values have been traversed.

  Data values can be excluded form being further traversed by setSkip(),
  setSkipAbove(), setSkipBelow(), and setSkipBetween().
  How often a data value was traversed can be retrieved by count().
  You can remove data elements that have their skip flag set by calling purge().

  pos() is the index of an data element from the whole data array.
  index() is the current index into the current (sub-)sequence.
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

    /*! Construct an empty RangeLoop.
        Use the set() and add() functions to fil lthe range. */
  RangeLoop( void );
    /*! Construct a linear range.
        See set( double, double, double, int, int, int, int ) for details. */
  RangeLoop( double first, double last, double step,
	     int repeat=1, int blockrepeat=1, int singlerepeat=1,
	     int increment=1 );
    /*! Construct a linear range.
        See set( double, double, int, int, int, int, int ) for details. */
  RangeLoop( double first, double last, int n,
	     int repeat=1, int blockrepeat=1, int singlerepeat=1,
	     int increment=1 );
    /*! Construct a range with a single value \a value.
        See set( double, int, int, int, int, int ) for details. */
  RangeLoop( double value, int size=1, 
	     int repeat=1, int blockrepeat=1, int singlerepeat=1,
	     int increment=1 );
    /*! Construct a range from the string \a range.
        See set( const string& ) for details. */
  RangeLoop( const string &range, double scale=1.0 );
    /*! Destruct thre RangeLoop. */
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
	If \a repeat is set to zero, the whole sequence is repeated indefinitely.
	Each subsets of data elements for a given increment is repeated \a blockrepeat times.
        Each data element is repeated \a singlerepeat times.
        The initial increment is set according to \a increment
	via setIncrement(). */
  void set( double first, double last, double step,
	    int repeat=1, int blockrepeat=1, int singlerepeat=1,
	    int increment=1 );
    /*! Add the range with first value \a first, last value \a last,
        and increment value \a step to the range. \sa addMode() */
  void add( double first, double last, double step );
    /*! Initialize the range with first value \a first, last value \a last,
        and increment factor \a fac.
        The whole sequence is repeated \a repeat times.
	If \a repeat is set to zero, the whole sequence is repeated indefinitely.
	Each subsets of data elements for a given increment is repeated \a blockrepeat times.
        Each data element is repeated \a singlerepeat times.
        The initial increment is set according to \a increment
	via setIncrement(). */
  void setLog( double first, double last, double fac,
	       int repeat=1, int blockrepeat=1, int singlerepeat=1,
	       int increment=1 );
    /*! Add the range with first value \a first, last value \a last,
        increment factor \a fac to the range. \sa addMode() */
  void addLog( double first, double last, double fac );

    /*! Initialize the range with \a n evenly spaced values starting with
        the first value \a first and ending with the last value \a last.
        The sequence is repeated \a repeat times.
	If \a repeat is set to zero, the whole sequence is repeated indefinitely.
	Each subsets of data elements for a given increment is repeated \a blockrepeat times.
        Each data element is repeated \a singlerepeat times.
        The initial increment is set according to \a increment
	via setIncrement(). */
  void set( double first, double last, int n,
	    int repeat=1, int blockrepeat=1, int singlerepeat=1,
	    int increment=1 );
    /*! Add a range with \a n evenly spaced values starting with
        the first value \a first and ending with the last value \a last
	to the range. \sa addMode() */
  void add( double first, double last, int n );
    /*! Initialize the range with \a n logarithmically spaced values 
        starting with the first value \a first
	and ending with the last value \a last.
        The sequence is repeated \a repeat times.
	If \a repeat is set to zero, the whole sequence is repeated indefinitely.
	Each subsets of data elements for a given increment is repeated \a blockrepeat times.
        Each data element is repeated \a singlerepeat times.
        The initial increment is set according to \a increment
	via setIncrement(). */
  void setLog( double first, double last, int n,
	       int repeat=1, int blockrepeat=1, int singlerepeat=1,
	       int increment=1 );
    /*! Add a range with \a n logarithmically spaced values starting with
        the first value \a first and ending with the last value \a last
	to the range. \sa addMode() */
  void addLog( double first, double last, int n );

    /*! Initialize the range with a single value \a value
        and allocate memory for \a size elements.
        The sequence is repeated \a repeat times.
	If \a repeat is set to zero, the whole sequence is repeated indefinitely.
	Each subsets of data elements for a given increment is repeated \a blockrepeat times.
        Each data element is repeated \a singlerepeat times.
        The initial increment is set according to \a increment
	via setIncrement(). */
  void set( double value, int size=1,
	    int repeat=1, int blockrepeat=1, int singlerepeat=1,
	    int increment=1 );
    /*! Add the single value \a value to the range.
        If the value already exist, then \a value is removed from the range.
        Returns \a true if \a value was added to the range,
        and \a false if it was not added. \sa addMode() */
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
        already existing data vales to the range. \sa setAddMode() */
  AddMode addMode( void ) const;
    /*! Set the method that is used for adding 
        already existing data vales to the range to \a addmode. \sa addMode() */
  void setAddMode( AddMode addmode );

    /*! The number of repetitions for the whole sequence.
        \sa setRepeat(), currentRepetition(), blockRepeat(), singleRepeat() */
  int repeat( void ) const;
    /*! Set the number of repetitions for the whole sequence to \a repeat.
	If \a repeat is set to zero, the whole sequence is repeated indefinitely.
        \sa repeat(), setBlockRepeat(), setSingleRepeat() */
  void setRepeat( int repeat );
    /*! Return the number of executed repetitions for the whole sequence to \a repeat.
        \sa repeat(), currentBlockRepetition(), currentSingleRepetition() */
  int currentRepetition( void ) const;

    /*! The number of repetitons for a block of data elements
        of a single increment.
	\sa setBlockRepeat(), currentBlockRepetition(), finishedBlock(),
        repeat(), singleRepeat() */
  int blockRepeat( void ) const;
    /*! Set the number of repetitons for a block of data elements
        of a single increment to \a repeat. 
	\sa blockRepeat(), setRepeat(), setSingleRepeat() */
  void setBlockRepeat( int repeat );
    /*! Return the number of executed repetitions for a block of data elements
        of a single increment. \sa blockRepeat(), finishedBlock(),
	currentRepetition(), currentSingleRepetition() */
  int currentBlockRepetition( void ) const;
    /*! Returns true if the current block repetitions are completed.
        \sa blockRepeat(), currentBlockRepetition(), finishedSingle() */
  bool finishedBlock( void ) const;

    /*! The number of repetitons for a single data element.
        \sa setSingleRepeat(), currentSingleRepetition(), finishedSingle(), lastSingle(),
	repeat(), blockRepeat() */
  int singleRepeat( void ) const;
    /*! Set the number of repetitons for a single data element to \a repeat.
        \sa singleRepeat(), setRepeat(), setBlockRepeat() */
  void setSingleRepeat( int repeat );
    /*! Return the number of executed repetitions for a single data element.
        \sa singleRpeat(), finishedSingle(), lastSingle(),
	currentRepetition(), currentBlockRepetition() */
  int currentSingleRepetition( void ) const;
    /*! Returns true if the current single repetitions are completed,
        i.e. the next data element is to be started.
        \sa singleRpeat(), currentSignelRepetition(), lastSingle(), finishedBlock() */
  bool finishedSingle( void ) const;
    /*! Returns true if this is the last single repetition.
        \sa singleRpeat(), currentSignelRepetition(), finishedSingle() */
  bool lastSingle( void ) const;

    /*! Set the initial increment to \a increment indices.
        E.g. an \a increment of 2 selects every second data value.
	If \a increment equals 0, the the increment is set to
	setLargeIncrement().
	If \a increment is negative, then the increment is set to
	setLargeIncrement() / 2^|\a increment|.
        \sa setLargeIncrement(), currentIncrement() */
  void setIncrement( int increment=1 );
    /*! Set the initial increment to the largest power of two
        less or equal than half the number of the data elements.
        \sa setIncrement(), currentIncrement() */
  void setLargeIncrement( void );
    /*! Return the current increment. \sa setIncrement() */
  int currentIncrement( void ) const;
    /*! Return the value corresponding to the current increment.
        This is currentIncrement() times the difference
	of succesive values in case of a linear range,
	the factor between succesive increments to the power
	of currentIncrement() in case of logarithmic ranges,
	or zero otherwise. \sa currentIncrement() */
  double currentIncrementValue( void ) const;

    /*! The sum of all counts of alle range elements. \sa count() */
  int totalCount( void ) const;
    /*! The maximum possible number of repetitions of a single data element
        (repeat() * blockRepeat() * singleRepeat() ).
         \sa totalCount(), maxBlockCount(), remainingCount() */
  int maxCount( void ) const;
    /*! The maximum possible number of repetitions of a single data element
        for the current block sequence. \sa maxCount(), remainingCount() */
  int maxBlockCount( void ) const;
    /*! Returns the overall number of remaining counts summed over all data elements
        that do not have their skip flag set.
        If repeat() is set to zero, returns the remaining count for a single repeat.
        \sa remainingBlockCount(), maxCount() */
  int remainingCount( void ) const;
    /*! Returns the overall number of remaining counts summed over all data elements
        that do not have their skip flag set
	for the current block of data. */
  int remainingBlockCount( void ) const;

    /*! Set the sequence for looping through the data values to \a seq.
        \sa up(), down(), alternateInUp(), alternateInDown(),
        alternateOutUp(), alternateOutDown(), random(), pseudoRandom() */
  void setSequence( Sequence seq );
    /*! Loop upwards trough the data values. \sa setSequence() */
  void up( void );
    /*! Loop downwards trough the data values. \sa setSequence() */
  void down( void );
    /*! Loop alternating trough the data values starting with the highest
        value. \sa setSequence() */
  void alternateInUp( void );
    /*! Loop alternating trough the data values starting with the lowest
        value. \sa setSequence() */
  void alternateInDown( void );
    /*! Loop alternating trough the data values starting in the middle
        upwards. \sa setSequence() */
  void alternateOutUp( void );
    /*! Loop alternating trough the data values starting in the middle
        downwards. \sa setSequence() */
  void alternateOutDown( void );
    /*! Loop randomly trough the data values, i.e. always draw a
        new sequence of random number. \sa setSequence() */
  void random( void );
    /*! Loop pseudo-randomly trough the data values, i.e. repeat
        always the same random sequence. \sa setSequence() */
  void pseudoRandom( void );

    /*! Returns a string with the names of possible sequence types
        according to Sequence separated by '|'. */
  static const string &sequenceStrings( void );

    /*! Number of elements in the current sequence. */
  int sequenceSize( void ) const;

    /*! Reset the range.
        Set index and repeat counters to zero,
        reset current increment,
        set the counts of the data elements to zero.
        If \a clearskip is set to \c true (default)
	then the skip flags are also cleared.
        A new sequence of indices is generated
	starting with the one nearest to \a pos.
        If \a pos is negative (default) it is set to an appropriate value.
	\sa purge() */
  void reset( int pos=-1, bool clearskip=true );
    /*! Remove all data elements, that have their skip flag set
        and generate a new sequence.
        \sa reset() */
  void purge( void );

    /*! Reset the sequence and use \a pos as the first data element.
        \sa reset() */
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
        \a >= 0: use \a pos as the start position.
        \note the new sequence might be empty! */
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
    /*! Return the count of the current data element. \sa totalCount() */
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
    /*! Return true if the data element at position \a pos is to be skipped.
        \sa setSkip() */
  int skip( int pos ) const;
    /*! Set the skipping behavior of the data element a position \a pos
        to \a skip.
        If \a skip is true (default) this data element will be skipped
        in future sequences. \sa skip(), setSkipBelow(), setSkipAbove(),
	setSkipBetween(), setSkipNocount() */
  void setSkip( int pos, bool skip=true );
    /*! Set the skipping behavior of all data elements below position \a pos
        inclusively to \a skip.
        If \a skip is true (default) these data elements will be skipped
        in future sequences. \sa setSkipAbove(), setSkipBetween(), setSkip(), setSkipNocount() */
  void setSkipBelow( int pos, bool skip=true );
    /*! Set the skipping behavior of all data elements above position \a pos
        inclusively to \a skip.
        If \a skip is true (default) these data elements will be skipped
        in future sequences. \sa setSkipBelow(), setSkipBetween(), setSkip(), setSkipNocount() */
  void setSkipAbove( int pos, bool skip=true );
    /*! Set the skipping behavior of all data elements above position \a pos1 inclusively
        and below position \a pos2 inclusively
        to \a skip.
        If \a skip is true (default) these data elements will be skipped
        in future sequences.
        \sa setSkip(), setSkipBelow(), setSkipAbove(), setSkipNocount() */
  void setSkipBetween( int pos1, int pos2, bool skip=true );
    /*! Set the skipping behavior of all data elements that have not been used yet,
        i.e. with zero count(), to \a skip.
        If \a skip is true (default) these data elements will be skipped
        in future sequences.
        \sa setSkip(), setSkipBelow(), setSkipAbove(), setSkipBetween() */
  void setSkipNocount( bool skip=true );

    /*! Within the range of non-skipped data elements make sure that at minimum
        \a num data elements are not skipped. Keep the ones that have a
        count greater than zero, and, if necessary, select further not to be
	skipped elements while respecting the current increment. */
  void setSkipNumber( int num );

    /*! Returns the position of the data element next or equal to position \a pos
        with count larger than zero.
        If there isn't any such element, size() is returned. \sa previous() */ 
  int next( int pos ) const;
    /*! Returns the position of the data element previous or equal to position \a pos
        with count larger than zero.
        If there isn't any such element, -1 is returned. \sa next() */ 
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

  double StepFac;

};


}; /* namespace relacs */

#endif /* ! _RELACS_RANGELOOP_H_ */

