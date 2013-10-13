/*
  FILE: WTFilter.hpp
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

#ifndef WTFILTER_HPP
#define WTFILTER_HPP

#include <list>
#include <string>
#include <utility>
#include <vector>

namespace WT {

  struct MODWT { /* */ };
  struct DWT   { /* */ };

  namespace Filter {

    typedef std::vector<double> WaveletFilter;
    typedef std::vector<double> ScalingFilter;

    enum FType {
      Haar,
      D4, D6, D8, D10, D12, D14, D16, D18, D20,
      LA8, LA10, LA12, LA14, LA16, LA18, LA20,
      BL14, BL18, BL20,
      C6, C12, C18, C24, C30
    };

    //=================
    // selectFilter() : convert a std::string to a FType
    //=================
    FType selectFilterType(const std::string& ftype);

    //================
    // getFilters() : convert an FType into a WaveletFilter & ScalingFilter
    //================
    std::pair< WaveletFilter, ScalingFilter > getFilters(FType ft);

    //=============
    // allFTypes() : return a list of all FType values
    //=============
    std::list< FType > allFTypes();

    //====================
    // allFTypesStrings() : return a list of all 
    //====================
    std::list< std::string > allFTypesStrings();

  } // namespace Filter

} // namespace WT


#include "WTFilter.cpp"

#endif // WTFILTER_HPP
