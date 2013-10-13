/*
  FILE: Wavelet.hpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Tue Oct  9 18:36:13 PDT 2007
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

#ifndef WT_FRAMEWORK_HPP
#define WT_FRAMEWORK_HPP

#include "WTBoundaries.hpp"
#include "WTFilter.hpp"
#include "WTOps.hpp"


namespace WT {

  /* Most everything associated with wavelet transformations came from ideas outlined here:
       Percival, D. B. and A. T. Walden (2000) Wavelet Methods for Time Series Analysis, Cambridge University Press.
  */

  //=========
  // modwt() : modified discrete wavelet transform ; calculates wavelet and scaling coefficients
  //=========
  // o Sequence should be a vector<T>, deque<T> or array of T's
  // o VOp and WOp will most often come from wavelets/WTOps.hpp
  // o 'numLevels' is the number of levels you want to calculate
  //=========
  template <
            typename Sequence,      // 'X' contains N measurement values
            typename WaveletFilter, // Filter from wavelets/WTFilter.hpp
            typename ScalingFilter, // Filter from wavelets/WTFilter.hpp
            typename VOp,           // Op called for each scaling coeff calculated (N per level)
            typename WOp            // Op called for each wavelet coeff calculated (N per level)
           >
  void modwt(Sequence& X, const WaveletFilter& wavefilt, const ScalingFilter& scalefilt, int numLevels,
             VOp& vop, WOp& wop);


  //==========
  // imodwt() : inverse modified discrete wavelet transform
  //==========
  //  o Most often used to invert modwt() results
  //  o 'Wj' should be a vector< vector<T> >, or a deque/array analog or combo
  //  o 'wavefilt' and 'scalefilt' should be the same as those used in the original modwt() call
  //==========
  template <
            typename ScalingCoefficients,     // N scaling coefficients
            typename ContWaveletCoefficients, // Container of containers of N wavelet coeff's
            typename WaveletFilter,           // Same as for modwt()
            typename ScalingFilter,           // Same as for modwt()
            typename VOp                      // Op called for each inverse value calculated
           >
  void imodwt(ScalingCoefficients Vj0, ContWaveletCoefficients& Wj, const WaveletFilter& wavefilt,
              const ScalingFilter& scalefilt, VOp& vop);




  /* Note that the smooth() and details() calculated below together sum back to the original Sequence X.
      They are the finer innerworking results of what imodwt() does.  When output as a set of waveforms,
      the smooth and details make up the multiresolution analysis (mra) described in Percival and Walden.
  */

  //==========
  // smooth() : calculates the smooth waveform
  //==========
  // o 'scalefilt' should be the same as was used in calculating 'Vj0' via modwt()
  // o 'numLevels' is the depth used to originally calculated 'Vj0' via modwt()
  //==========
  template <
            typename ScalingCoefficients, // Scaling coefficients calculated from modwt()
            typename ScalingFilter,       // As described for modwt()
            typename SmoothOp             // Op called for each of the N calculated smooth values
           >
  void smooth(ScalingCoefficients& Vj0, const ScalingFilter& scalefilt, int numLevels, SmoothOp& sop);


  //===========
  // details() : calculates the detail waveforms ; concentrating on time rather than memory
  //===========
  //  o 'wavefilt' and 'scalefilt' should be the same as those used in the original modwt() call
  //  o Each operation in 'dops' belongs to a respective container of coefficients and will be called N times
  //===========
  template <
            typename ContWaveletCoefficients, // Container of containers of N wavelet coeff's
            typename WaveletFilter,           // Same as for modwt()
            typename ScalingFilter,           // Same as for modwt()
            typename ContDetailsOps           // Container of ops, one for each wavelet coeff container
           >
  void details(ContWaveletCoefficients& Wj, const WaveletFilter& wavefilt,
               const ScalingFilter& scalefilt, ContDetailsOps& dops);


  //=======
  // mra() : multiresolution analysis ; calculates details and smooth
  //=======
  // o Efficient convenience wrapper for doAll(), specific to details and smooth
  // o Wavelet and scaling coefficients ignored after calculation & use
  // o DetailsOp should self-regulate via 'detailsOp.Reset()'.  See struct PrintLast for a good example.
  //=======
  template <
            typename Sequence,  // 'X' contains the N original values
            typename DetailsOp, // Op called for calculated detail values
            typename SmoothOp   // Op called for calculated smooth values
           >
  void mra(Sequence& X, unsigned int level, Filter::FType filterType,
           DetailsOp& detailsOp, SmoothOp& smoothOp);


  //=========
  // doAll() : calculates everything from scratch, giving opportunity for all output operations
  //=========
  // o All possible ops available: wavelet coeff's, scaling coeff's, details and smooth
  // o DetailsOp should self-regulate via 'detailsOp.Reset()'.  See struct PrintLast for a good example.
  //=========
  template <
            typename Sequence,              // 'X' contains the N original values
            typename WaveletCoefficientOps, // Op called for calculated wavelet coeff values (N per level)
            typename DetailsOp,             // Op called for calculated details values (N per level)
            typename ScalingCoefficientOp,  // Op called for calculated scaling coeff values
            typename SmoothOp               // Op called for calculated smooth values
           >
  void doAll(Sequence& X,
             unsigned int level,
             Filter::FType filterType,
             WaveletCoefficientOps& waveletOp,
             DetailsOp& detailsOp,
             ScalingCoefficientOp& scalingOp,
             SmoothOp& smoothOp);

} // namespace WT


#include "WT.cpp"

#endif // WT_FRAMEWORK_HPP
