/*
  FILE: WTBoundaries.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Fri Sep 21 00:38:19 PDT 2007
*/

//    The Maximal Overlap Discrete Wavelet Tranform (MODWT)
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

#ifndef WTBOUNDARIES_HPP
#define WTBOUNDARIES_HPP

#include <list>
#include <string>

namespace WT {

  enum Boundary {
    Periodic, Reflected
  };

  //===================
  // selectBoundary() : convert a std::string to a Boundary value
  //===================
  Boundary selectBoundary(const std::string& btype);

  //=================
  // allBoundary() : return a list all Boundary values
  //=================
  std::list< Boundary > allBoundary();

  //=======================
  // allBoundaryStrings() : return a list of string representations of Boundary values
  //=======================
  std::list< std::string > allBoundaryStrings();

} // namespace WT

#endif // WTBOUNDARIES_HPP
