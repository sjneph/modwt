/*
  FILE: WTOps.cpp
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

#include <cstddef>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "Assertion.hpp"
#include "Exception.hpp"
#include "PrintTypes.hpp"
#include "WTOps.hpp"


namespace WT {


  //=============
  // PrintValues
  //=============

  PrintValues::PrintValues(const std::string& basename, std::size_t maxPrints, int pLevel)
      : pLevel_(pLevel), currentPrints_(0), maxPrints_(maxPrints), base_(basename),
        on_(true), doReset_(false), useStdout_(basename.empty()), fptr_(0) {

    Ext::Assert<Ext::LogicError>(!useStdout_ || pLevel_ >= 0,
                                 "Cannot send each level's information to stdout",
                                 "Logic Error: PrintValues constructor");
  }

  void PrintValues::Level(int level) {
    if ( !useStdout_ && fptr_ )
      std::fclose(fptr_);

    if ( pLevel_ >= 0 && level != pLevel_ ) {
      on_ = false;
      if ( !useStdout_ )
        fptr_ = 0;
      return;
    }
    else if ( !on_ && pLevel_ < 0 ) // manually turned Off()
      return;

    // if pLevel >= 0 and this object was turned Off() manually, but
    //  level == pLevel, then it is automatically turned On.  Later,
    //  when this func is called again with a new level, this object
    //  is, again, turned off. 
    // in this way, when pLevel >= 0, pLevel acts as a trigger
    //  condition

    currentPrints_ = 0;
    on_ = true;
    if ( !useStdout_ ) {
      std::stringstream s;
      s << level;
      std::string name = base_ + "." + s.str();
      fptr_ = std::fopen(name.c_str(), "w");
      Ext::Assert<Ext::InvalidFile>(fptr_ && fptr_ != NULL,
                                    "Unable to open file for writing: " + name);
    }
    else
      fptr_ = stdout;
  }

  void PrintValues::Off()
    { on_ = false; }

  void PrintValues::On()
    { on_ = true; }

  bool PrintValues::IsOn() const
    { return(on_); }

  void PrintValues::Reset() {
    on_ = true;
    if ( !useStdout_ ) {
      if ( fptr_ )
        std::fclose(fptr_);
      fptr_ = 0;
    }
  }

  template <typename T>
  inline void PrintValues::operator()(T t) {
    if ( on_ ) {
      if ( ++currentPrints_ <= maxPrints_ )
        PrintTypes::Println(fptr_, t);
    }
  }

  PrintValues::~PrintValues() {
    if ( !useStdout_ && fptr_ )
      std::fclose(fptr_);
  }


  //===========
  // PrintLast
  //===========

  PrintLast::PrintLast(const std::string& basename, std::size_t maxPrints, int pLevel)
      : PrintValues(basename, maxPrints, pLevel) {
    typedef Ext::ArgumentError AE;
    typedef Ext::LogicError LE;
    Ext::Assert<AE>(pLevel >= 0, "Cannot create PrintLast with pLevel < 0");

    // PrintLast is used for a specific purpose - look at its Reset()
    //  implementation below --> meant for mra() and doAll()
    Ext::Assert<LE>(!basename.empty(),
                    "Cannot use a PrintLast object with stdout",
                    "PrintLast constructor");
  }

  void PrintLast::Reset() {
    if ( pLevel_ >= 0 )
      ++pLevel_, currentPrints_ = 0;
  }


  //===============
  // SaveAllValues
  //===============

  template <typename T>
  SaveAllValues<T>::SaveAllValues() : on_(true)
    { /* */ }

  template <typename T>
  void SaveAllValues<T>::Level(int level) {
    if ( on_ )
      vals_.push_back( std::vector<T>() ); // add an empty vector
  }

  template <typename T>
  void SaveAllValues<T>::Off()
    { on_ = false; }

  template <typename T>
  void SaveAllValues<T>::On()
    { on_ = true; }

  template <typename T>
  bool SaveAllValues<T>::IsOn() const
    { return(on_); }

  template <typename T>
  void SaveAllValues<T>::Reset() {
    vals_.clear();
    on_ = true;
  }

  template <typename T>
  inline void SaveAllValues<T>::operator()(const T& t) {
    if ( on_ )
      vals_[vals_.size()-1].push_back(t);
  }

  template <typename T>
  std::vector< std::vector<T> >& SaveAllValues<T>::Values()
    { return(vals_); }


  //===============
  // SaveLastLevel
  //===============

  template <typename T>
  SaveLastLevel<T>::SaveLastLevel(int last) : on_(false), last_(last)
    { /* */ }

  template <typename T>
  void SaveLastLevel<T>::Level(int level) {
    if ( last_ == level || last_ < 0 )
      on_ = true;

    if ( on_ )
      vals_.clear();
  }

  template <typename T>
  void SaveLastLevel<T>::Off()
    { on_ = false; }

  template <typename T>
  void SaveLastLevel<T>::On()
    { on_ = true; }

  template <typename T>
  bool SaveLastLevel<T>::IsOn() const
    { return(on_); }

  template <typename T>
  void SaveLastLevel<T>::Reset() {
    vals_.clear();
    on_ = true;
  }

  template <typename T>
  std::vector<T>& SaveLastLevel<T>::Values()
    { return(vals_); }

  template <typename T>
  inline void SaveLastLevel<T>::operator()(const T& t) {
    if ( on_ )
      vals_.push_back(t);
  }

} // namespace WT
