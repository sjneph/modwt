/*
  FILE: WTOps.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Fri Sep 21 00:38:19 PDT 2007
*/

//    The Maximal Overlap Discrete Wavelet Transform (MODWT)
//    Copyright (C) 2007-2013 Shane Neph & Scott Kuehn
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
//

#ifndef WT_OPS_FRAMEWORK_HPP
#define WT_OPS_FRAMEWORK_HPP

#include <cstddef>
#include <cstdio>
#include <limits>
#include <string>
#include <vector>

namespace WT {

  //=============
  // DoNothing()
  //=============
  struct DoNothing {
    void Level(int level) { /* */ }
    void Off() { /* */ }
    void On() { /* */ }
    bool IsOn() { return(false); }
    void Reset() { /* */ }

    template <typename T>
    inline void operator()(const T& t) { /* */ }
  };

  //===============
  // PrintValues()
  //===============
  /*
    The meaning for maxPrints constructor argument is to allow the end user to
    configure a maximum number of items to print for a given pLevel.  An exact
    example is to provide a work around when the user chooses a 'Reflected'
    boundary condition for a wavelet option.  The output will have 2*N items
    for an N input items.  The maxPrints parameter allows one to print the first
    N computed values instead.
  */
  struct PrintValues : public DoNothing {
    explicit PrintValues(const std::string& basename,
                         std::size_t maxPrints = std::numeric_limits<std::size_t>::max(),
                         int pLevel = -1);

    void Level(int level);
    void Off();
    void On();
    bool IsOn() const;
    void Reset();
    template <typename T>
    inline void operator()(T t);
    ~PrintValues();

  protected:
    int pLevel_;
    std::size_t currentPrints_;
    std::size_t maxPrints_;
    std::string base_;
    bool on_, doReset_, useStdout_;
    FILE* fptr_;    
  };


  //=============
  // PrintLast()
  //=============
  /*
    This is essentially PrintValues aimed to work with outputting
    details waveforms via the functions doAll() or mra().  In these cases,
    we run EACH set of N wavelet coefficients, one set per level, separately
    through calculations involving multiple levels (cascading zero-phase
    filters).  The real output for that single set of coefficients is the final
    output of the cascaded filters.  PrintLast provides a mechanism for
    indicating that last level.  Further, the Reset() function allows one to
    dynamically change what is the "last level", thereby allowing us to "start
    over" and sweep through another set of cascaded filters for the next set of
    N wavelet coefficients.  In the end, you receive an output of N values for
    each set of N input wavelet coefficients.
    The Reset() feature has a hardcoded increment so that the "last level" is
    dynamically set 1 larger after each such call, meaning there is one more
    level of zero-phase filtering to go through than the previous "last level"
  */
  struct PrintLast : public PrintValues {
    PrintLast(const std::string& basename, std::size_t maxPrints, int pLevel);
    void Reset();
  };


  //=================
  // SaveAllValues()
  //=================
  /*
    Realize that Values() returns a reference, and an external
    user could modify vals_ even when the object of this type
    has been turned Off().  This is unsafe, but the potential
    for speedup and memory improvements by not making extra copies
    is simply enormous.  We give the end user the benefit of the
    doubt in terms of being responsible.  This class is obviously
    meant for use with modwt(), imodwt() and similar algorithms
    where overall performance is greatly impacted by the number
    of copies made.

    Level() is used to partition inputs (that come via operator())
    into respective levels/scales.  If an end user turns an object
    of this type Off(), then operator() does nothing, but so does
    Level().  An end user needs to make sure the object is turned
    On() if they want the next partition when Level() is called.
  */
  template <typename T>
  struct SaveAllValues : public DoNothing {
    SaveAllValues();
    void Level(int level);
    void Off();
    void On();
    bool IsOn() const;
    void Reset();
    std::vector< std::vector<T> >& Values();

    inline void operator()(const T& t);

  protected:
    bool on_;
    std::vector< std::vector<T> > vals_;
  };

  //=================
  // SaveLastLevel()
  //=================
  /*
    Realize that Values() returns a reference, and an external
    user could modify vals_ even when the object of this type
    has been turned Off().  This is unsafe, but the potential
    for speedup and memory improvements by not making extra copies
    is simply enormous.  We give the end user the benefit of the
    doubt in terms of being responsible.  This class is obviously
    meant for use with modwt(), imodwt() and similar algorithms
    where overall performance is greatly impacted by the number
    of copies made.

    Level() clears the data retained by an object of this class,
    unless the object is Off() in which case nothing changes.
  */
  template <typename T>
  struct SaveLastLevel : public DoNothing {
    explicit SaveLastLevel(int level = -1);
    void Level(int level);
    void Off();
    void On();
    bool IsOn() const;
    void Reset();
    std::vector<T>& Values();

    inline void operator()(const T& t);

  protected:
    bool on_;
    int last_;
    std::vector<T> vals_;
  };

} // namespace WT

#include "WTOps.cpp"

#endif // WT_OPS_FRAMEWORK_HPP
