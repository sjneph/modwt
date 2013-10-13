/*
  FILE: Formats.cpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Mon Aug 20 10:22:26 PDT 2007
*/

//
//    The Maximal Overlap Discrete Wavelet Tranform (MODWT)
//    Copyright (C) 2007-2013 Shane Neph and Scott Kuehn
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

#include <cstdio>
#include <cstring>

#include "Formats.hpp"

namespace Formats {

  char const* Format(char const*) { return("%s"); }
  char const* Format(char) { return("%c"); }
  char const* Format(double) { return("%lf"); }
  char const* Format(float) { return("%f"); }
  char const* Format(int) { return("%d"); }
  char const* Format(unsigned int) { return("%u"); }
  char const* Format(long int) { return ("%ld"); }
  char const* Format(unsigned long int) { return("%lu"); }
  char const* Format(short) { return ("%hd"); }
  char const* Format(unsigned short) { return ("%hu"); }

  char const* Format(double, int precision, bool scientific) {
    static char prec[20];
    if ( scientific )
      std::sprintf(prec, "%%.%de", precision);
    else
      std::sprintf(prec, "%%.%dlf", precision);
    return(prec);
  }

} // namespace Formats
