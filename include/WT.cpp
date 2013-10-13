/*
  FILE: WT.cpp
  AUTHOR: Shane Neph & Scott Kuehn
  CREATE DATE: Fri Sep 21 00:38:19 PDT 2007
*/

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

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <string>
#include <sstream>

#include "Assertion.hpp"
#include "Exception.hpp"

#include "Wavelet.hpp"
#include "WTBoundaries.hpp"
#include "WTFilter.hpp"
#include "WTOps.hpp"


namespace WT {


  namespace Details {


    //=================
    // modwt_forward() : the workhorse of modwt()
    //=================
    // o passed in Vj is used to store scaling coefficient results
    // o internal Wj is a non-array for potential memory savings in
    //    some scenarios, particularly if we are not going to compute
    //    the details waveforms later which require all wavelet
    //    coefficients. One can look to 'wop' to store those results,
    //    though, or call the overloaded version below.
    //=================
    template <
              typename Container,
              typename WaveletFilter,
              typename ScalingFilter,
              typename VOp,
              typename WOp
             >
    void modwt_forward(const Container& Vi, const WaveletFilter& wavefilt, const ScalingFilter& scalefilt,
                       int j, Container& Vj, VOp& vop, WOp& wop) {

      const std::size_t N = static_cast<std::size_t>(Vi.size()); // Vi.size() == sequence-size
      const std::size_t L = static_cast<std::size_t>(wavefilt.size()); // wavefilt.size() == scalefilt.size()
      const std::size_t D = static_cast<std::size_t>(std::pow(2.0, j)); // assumed <= N ; asserted by caller
      typename Container::value_type Wj = 0;
      std::size_t k = 0;

      for ( std::size_t t = 0; t < N; k = static_cast<std::size_t>(++t) ) {
        Vj[t] = scalefilt[0] * Vi[t];
        Wj = wavefilt[0] * Vi[t];

        for ( std::size_t l = 1; l < L; ++l ) {
          if ( k >= D )
            k -= D;
          else
            k = static_cast<std::size_t>(N + k - D);
          Vj[t] += scalefilt[l] * Vi[k];
          Wj += wavefilt[l] * Vi[k];
        } // for

        vop(Vj[t]);
        wop(Wj);
      } // for
    }


    //===============
    // modwt_forward : Overload 2
    //===============
    // o passed in Vj stores results -> scaling coefficients
    // o passed in Wj stores results -> wavelet coefficients
    // o this can lead to overall advantages in terms of memory savings by
    //    allowing the caller to manage these resources
    //===============
    template <
              typename Container,
              typename WaveletFilter,
              typename ScalingFilter,
              typename VOp,
              typename WOp
             >
    void modwt_forward(const Container& Vi, const WaveletFilter& wavefilt, const ScalingFilter& scalefilt,
                       int j, Container& Vj, Container& Wj, VOp& vop, WOp& wop) {

      const std::size_t N = static_cast<std::size_t>(Vi.size()); // Vi.size() == sequence-size
      const std::size_t L = static_cast<std::size_t>(wavefilt.size()); // wavefilt.size() == scalefilt.size()
      const std::size_t D = static_cast<std::size_t>(std::pow(2.0, j)); // assumed <= N ; asserted by caller
      std::size_t k = 0;

      for ( std::size_t t = 0; t < N; k = static_cast<std::size_t>(++t) ) {
        Vj[t] = scalefilt[0] * Vi[t];
        Wj[t] = wavefilt[0] * Vi[t];

        for ( std::size_t l = 1; l < L; ++l ) {
          if ( k >= D )
            k -= D;
          else
            k = static_cast<std::size_t>(N + k - D);
          Vj[t] += scalefilt[l] * Vi[k];
          Wj[t] += wavefilt[l] * Vi[k];
        } // for

        vop(Vj[t]);
        wop(Wj[t]);
      } // for
    }


    //===================
    // imodwt_backward() : the workhorse of imodwt()
    //===================
    // o This is the general implementation used by the general imodwt() function
    // o A related function below can be used in a way to save some memory overhead
    //    when calculating smooth/details.  That one, imodwt_backward_zerophase(),
    //    works with one container of coefficients & "applies" a zero-phase filter.
    //===================
    template <
              typename Container,
              typename WaveletFilter,
              typename ScalingFilter,
              typename VOp
             >
    void imodwt_backward(const Container& Vj, const Container& Wj, const WaveletFilter& wavefilt,
                         const ScalingFilter& scalefilt, int j, Container& Vi, VOp& vop) {

      const std::size_t N = static_cast<std::size_t>(Vj.size()); // Vj.size() == sequence-size
      const std::size_t L = static_cast<std::size_t>(wavefilt.size()); // wavefilt.size() == scalefilt.size()
      const std::size_t D = static_cast<std::size_t>(std::pow(2.0, j)); // assumed <= N ; asserted by caller
      std::size_t k = 0;

      for ( std::size_t t = 0; t < N; k = static_cast<std::size_t>(++t) ) {
        Vi[t] = scalefilt[0] * Vj[t] + wavefilt[0] * Wj[t];

        for ( std::size_t l = 1; l < L; ++l ) {
          k += D;
          if ( k >= N )
            k -= N;
          Vi[t] += scalefilt[l] * Vj[k] + wavefilt[l] * Wj[k];
        } // for

        vop(Vi[t]);
      } // for
    }


    //============================
    // imodwt_backward_zerophase() : applies a zero-phase filter to 'Kj', results go to 'Ki'
    //============================
    // o Useful when computing smooth or details
    // o One less container here can help to reduce peak memory overhead for caller
    //============================
    template <
              typename Container,
              typename Filter,
              typename Op
             >
    void imodwt_backward_zerophase(const Container& Kj, const Filter& filt, int j,
                                   Container& Ki, Op& op) {

      // One container of values and filter is absent (compared to imodwt_backward()) -> a zero-phase filter
      const std::size_t N = static_cast<std::size_t>(Kj.size()); // Kj.size() == sequence-size
      const std::size_t L = static_cast<std::size_t>(filt.size()); // filter size
      const std::size_t D = static_cast<std::size_t>(std::pow(2.0, j)); // asserted <= N by caller
      std::size_t k = 0;

      for ( std::size_t t = 0; t < N; k = static_cast<std::size_t>(++t) ) {
        Ki[t] = filt[0] * Kj[t];

        for ( std::size_t l = 1; l < L; ++l ) {
          k += D;
          if ( k >= N )
            k -= N;
          Ki[t] += filt[l] * Kj[k];
        } // for

        op(Ki[t]);
      } // for
    }


    //===============
    // details_one() : Produces a detail waveform from a single set of wavelet coefficients
    //===============
    template <
              typename WaveletCoefficients,
              typename DetailResults,
              typename WaveletFilter,
              typename ScaleFilter,
              typename DetailsOp
             >
    void details_one(WaveletCoefficients& Wjt, DetailResults& Wit, const WaveletFilter& wavefilt,
                     const ScaleFilter& scalefilt, int level, DetailsOp& dop) {

      if ( Wjt.empty() )
        return;

      WaveletCoefficients* WjtPtr = &Wjt;
      WaveletCoefficients* WitPtr = &Wit;

      for ( int j = 0; j <= level; ++j ) {
        dop.Level(j+1);
        if ( j == level )
          Details::imodwt_backward_zerophase(*WjtPtr, wavefilt, j, *WitPtr, dop);
        else
          Details::imodwt_backward_zerophase(*WjtPtr, scalefilt, j, *WitPtr, dop);
        WjtPtr = WitPtr;
        WitPtr = (WjtPtr == &Wit) ? &Wjt : &Wit;
      } // for
    }


    //==============
    // smooth_one() : produces a smooth output in 'Vi' from 'Vj0'
    //==============
    // o not much different from smooth() other than no internal Sequence variable
    // o One less container here can help to reduce peak memory overhead for caller
    //==============
    template <
              typename Sequence,
              typename ScalingFilter,
              typename SmoothOp
             >
    void smooth_one(Sequence& Vj0, Sequence& Vi, const ScalingFilter& scalefilt, int numLevels, SmoothOp& sop) {

      double expsz = numLevels - 1;
      Ext::Assert<Ext::ArgumentError>(Vj0.size() >= std::pow(2.0, expsz), "smooth_one()", "wavelet xfm exceeds sample size");
      Sequence* VjPtr = &Vj0;
      Sequence* ViPtr = &Vi;

      ++numLevels;
      for ( int j = numLevels - 1; j >= 0; --j ) {
        sop.Level(numLevels - j); // counting backwards
        Details::imodwt_backward_zerophase(*VjPtr, scalefilt, j, *ViPtr, sop);
        VjPtr = ViPtr;
        ViPtr = (VjPtr == &Vj0) ? &Vi : &Vj0;
      } // for
    }

  } // namespace Details


  //=========
  // modwt() : Modified Discrete Wavelet Transform
  //=========
  template <
            typename Sequence,
            typename WaveletFilter,
            typename ScalingFilter,
            typename VOp,
            typename WOp
           >
  void modwt(Sequence& X, const WaveletFilter& wavefilt, const ScalingFilter& scalefilt, int numLevels,
             VOp& vop, WOp& wop) {

    double expsz = numLevels - 1;
    Ext::Assert<Ext::ArgumentError>(X.size() >= std::pow(2.0, expsz), "modwt()", "wavelet xfm exceeds sample size");

    Sequence const *Vi = &X;
    Sequence Vk(X.size(), 0);
    Sequence* Vj = &Vk;

    for ( int j = 0; j < numLevels; ++j ) {
      vop.Level(j+1);
      wop.Level(j+1);
      Details::modwt_forward(*Vi, wavefilt, scalefilt, j, *Vj, vop, wop);
      Vi = Vj;
      Vj = (Vi == &Vk) ? &X : &Vk;
    } // for
  }


  //==========
  // imodwt() : Inverse Modified Discrete Wavelet Transform
  //==========
  template <
            typename ScalingCoefficients,
            typename ContWaveletCoefficients,
            typename WaveletFilter,
            typename ScalingFilter,
            typename VOp
           >
  void imodwt(ScalingCoefficients& Vj0, ContWaveletCoefficients& Wj, const WaveletFilter& wavefilt,
              const ScalingFilter& scalefilt, VOp& vop) {

    double expsz = Wj.size() - 1;
    Ext::Assert<Ext::ArgumentError>(Vj0.size() >= std::pow(2.0, expsz), "imodwt()", "wavelet xfm exceeds sample size");
    ScalingCoefficients *Vj = &Vj0;
    ScalingCoefficients Vk(Vj0.size(), 0);
    ScalingCoefficients* Vi = &Vk;

    typename ContWaveletCoefficients::reverse_iterator iterStart = Wj.rbegin(), iterEnd = Wj.rend();
    std::size_t j = Wj.size();

    while ( iterStart != iterEnd ) {
      vop.Level(--j);
      Details::imodwt_backward(*Vj, *iterStart, wavefilt, scalefilt, j, *Vi, vop);
      Vj = Vi;
      Vi = (Vj == &Vk) ? &Vj0 : &Vk;
      ++iterStart;
    } // while
  }


  //==========
  // smooth() : calculates the smooth from the scaling coefficients
  //==========
  template <
            typename ScalingCoefficients,
            typename ScalingFilter,
            typename SmoothOp
           >
  void smooth(ScalingCoefficients& Vj0, const ScalingFilter& scalefilt, int numLevels, SmoothOp& sop) {

    double expsz = numLevels - 1;
    Ext::Assert<Ext::ArgumentError>(Vj0.size() >= std::pow(2.0, expsz), "smooth()", "wavelet xfm exceeds sample size");
    ScalingCoefficients* VjPtr = &Vj0;
    ScalingCoefficients Vi(Vj0);
    ScalingCoefficients* ViPtr = &Vi;

    for ( int j = numLevels - 1; j >= 0; --j ) {
      sop.Level(numLevels - j); // counting backwards
      Details::imodwt_backward_zerophase(*VjPtr, scalefilt, j, *ViPtr, sop);
      VjPtr = ViPtr;
      ViPtr = (VjPtr == &Vj0) ? &Vi : &Vj0;
    } // for
  }


  //===========
  // details() : calculates all detail waveforms from given wavelet coefficient sets
  //===========
  template <
            typename ContWaveletCoefficients,
            typename WaveletFilter,
            typename ScaleFilter,
            typename ContDetailsOps
           >
  void details(ContWaveletCoefficients& Wj, const WaveletFilter& wavefilt, const ScaleFilter& scalefilt,
               ContDetailsOps& dops) {

    if ( Wj.empty() )
      return;

    typedef typename ContWaveletCoefficients::value_type VT;
    double expsz = Wj.size() - 1;
    Ext::Assert<Ext::ArgumentError>(Wj[0].size() >= std::pow(2.0, expsz), "details()", "wavelet xfm exceeds sample size");
    VT Wit(Wj.begin()->size());

    // The main trick here is to switch between the wavefilt and scalefilt depending on whether you
    //  are at the scale of current interest or less.
    typename ContWaveletCoefficients::iterator Wjt = Wj.begin(), end = Wj.end(), alwaysBeg = Wj.begin();
    int sz = 0;
    while ( Wjt != end ) {
      Details::details_one(*Wjt, Wit, wavefilt, scalefilt, sz, dops[sz]);
      ++Wjt;
      ++sz;
    } // while
  }


  //=========
  // doAll() : calculates everything from scratch ('X') giving opportunities for all output
  //=========
  template <
            typename Sequence,
            typename WaveletCoefficientOps,
            typename DetailsOp,
            typename ScalingCoefficientOp,
            typename SmoothOp
           >
  void doAll(Sequence& X,
             unsigned int level,
             Filter::FType filterType,
             WaveletCoefficientOps& waveletOp,
             DetailsOp& detailsOp,
             ScalingCoefficientOp& scalingOp,
             SmoothOp& smoothOp) {

    Ext::Assert<Ext::ArgumentError>(!X.empty(), "doAll()", "empty input");
    double expsz = level - 1;
    Ext::Assert<Ext::ArgumentError>(X.size() >= std::pow(2.0, expsz), "doAll()", "wavelet xfm exceeds sample size");

    Sequence Y(X.size());
    Sequence* xPtr = &X;
    Sequence* yPtr = &Y;
    Sequence* zPtr = static_cast<Sequence*>(0);

    typedef std::pair<WT::Filter::WaveletFilter, WT::Filter::ScalingFilter> Filts;
    Filts filters = WT::Filter::getFilters<WT::MODWT>(filterType);

    bool scalingWasOn = scalingOp.IsOn();
    scalingOp.Off(); // need off during modwt call except on last level

    for ( unsigned int idx = 1; idx <= level; ++idx ) {
      waveletOp.Level(idx);

      if ( idx == level && scalingWasOn ) {
        scalingOp.On();
        scalingOp.Level(idx);
      }


      // Perform modwt on input to obtain next set of scaling and/or wavelet coefficients
      //  yPtr will point to Vj[t] information after call; idx is 1-based.  If detail
      //  waveforms are desired, then also calculate the next one.  In that case,
      //  xPtr will point to the detail waveform when done.
      detailsOp.Reset();
      detailsOp.Level(idx); // level setting must come before IsOn() checks
      if ( detailsOp.IsOn() ) { // do we care 'bout details? ; zPtr will contain Wj[t] information
        if ( !zPtr )
          zPtr = new Sequence(Y.size());
        Details::modwt_forward(*xPtr, filters.first, filters.second, idx-1, *yPtr, *zPtr, scalingOp, waveletOp);
        Details::details_one(*zPtr, *xPtr, filters.first, filters.second, idx-1, detailsOp);
      }
      else // no Wj[t] retained
        Details::modwt_forward(*xPtr, filters.first, filters.second, idx-1, *yPtr, scalingOp, waveletOp);


      // If at last level and applicable, get smooth waveform
      // If at last level -> can write over yPtr's information in-place
      //  idx is 1 based -> 1 less
      smoothOp.Level(idx); // level setting must come before IsOn() checks
      if ( idx == level && smoothOp.IsOn() )
        Details::smooth_one(*yPtr, *xPtr, filters.second, idx-1, smoothOp);


      // Do some pointer swapping
      xPtr = yPtr;
      yPtr = (xPtr == &X) ? &Y : &X;

    } // for


    // Clean up
    if ( zPtr )
      delete zPtr;
  }


  //=======
  // mra() : multiresolution analysis; convenience function wrapper for doAll()
  // o Only passes on user-specified operators for the details and smooth
  // o Wavelet and scaling coefficients ignored after calculation & use
  //=======
  template <
            typename Sequence,
            typename DetailsOp,
            typename SmoothOp
           >
  void mra(Sequence& X,
           unsigned int level,
           Filter::FType filterType,
           DetailsOp& detailsOp,
           SmoothOp& smoothOp) {

    DoNothing waveletOp, scalingOp;
    doAll(X, level, filterType, waveletOp, detailsOp, scalingOp, smoothOp);
  }


  //==================
  // selectBoundary()
  //==================
  Boundary selectBoundary(const std::string& btype) {
    std::string nocase = btype;
    for ( std::string::size_type sz = 0; sz < nocase.size(); ++sz )
      nocase[sz] = std::tolower(nocase[sz]);

    Boundary b;
    if ( nocase == "reflected" )
      b = Reflected;
    else if ( nocase == "periodic" )
      b = Periodic;
    else
      throw(Ext::ArgumentError("selectBoundary()", "No Boundary type match"));

    return(b);
  }


  //======================
  // allBoundaryStrings()
  //======================
  std::list< std::string > allBoundaryStrings() {
    std::list< std::string > allBounds;
    allBounds.push_back("Reflected");
    allBounds.push_back("Periodic");
    return(allBounds);
  }

  //==============
  // allBoundary()
  //==============
  std::list< Boundary > allBoundary() {
    std::list< Boundary > allBounds;
    std::list< std::string > allBoundsStrings = allBoundaryStrings();
    std::list< std::string >::const_iterator i = allBoundsStrings.begin();
    while ( i != allBoundsStrings.end() )
      allBounds.push_back(selectBoundary(*i++));
    return(allBounds);
  }


} // namespace WT
