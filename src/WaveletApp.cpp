/*
  FILE: WaveletApp.cpp
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

#include <cmath>
#include <cstddef>
#include <cstdio>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "Wavelet.hpp"
#include "Assertion.hpp"
#include "Exception.hpp"
#include "FPWrap.hpp"


namespace {

  // IMODWT is also available via library API
  enum Operation { WAVE_COEFFS, SCALE_COEFFS, WAVE_SCALE_COEFFS, SMOOTH, DETAILS, MRA, ALL };

  struct Help { /* */ };


  //=======
  // Input : just manages user inputs to the application
  //=======
  struct Input {
    Input(int argc, char** argv)
         : file_("-"), fType_("LA8"), bType_("Periodic"),
           op_(SMOOTH), maxLevel_(4), toStdout_(false), prefix_("") {

      Ext::Assert<Ext::UserError>(argc >= 2, "Expect <file-name>");
      if ( lc(argv[argc-1]) == "--help" )
        throw(Help());

      for ( int i = 1; i < argc - 1; ) {
        std::string option = lc(argv[i++]);
        std::string value = argv[i++];

        if ( option == "--help" || value == "--help" )
          throw(Help());

        if ( option == "--boundary" )
          bType_ = value;
        else if ( option == "--filter" )
          fType_ = value;
        else if ( option == "--level" )
          setLevel(value);
        else if ( option == "--operation" )
          setOperation(value);
        else if ( option == "--prefix" )
          prefix_ = value;
        else if ( option == "--to-stdout" ) {
          toStdout_ = true;          
          --i; // a flag
        }
        else
          throw(Ext::UserError("Unknown option", argv[i-2]));

        Ext::Assert<Ext::UserError>(i != argc, "Wrong number of total arguments");
      } // for

      file_ = argv[argc-1];
      bool problem = toStdout_ && op_ != SMOOTH && op_ != SCALE_COEFFS;
      Ext::Assert<Ext::UserError>(!problem,
                                  "--to-stdout not allowed for given --operation",
                                  "see --help for details");
      Ext::Assert<Ext::UserError>(!toStdout_ || prefix_.empty(),
                                  "cannot --to-stdout and add --prefix value"); 
    }

    std::string BoundaryType() const
      { return(bType_); }

    char const* File() const
      { return(file_.c_str()); }

    std::string FilterType() const
      { return(fType_); }

    int MaxLevel() const
      { return(maxLevel_); }

    Operation Op() const
      { return(op_); }

    std::string Prefix() const
      { return(prefix_); }

    bool StdOut() const
      { return(toStdout_); }

    static std::string Usage() {
      std::string expect = "wavelets";
      expect += "\n\t[--boundary <string = periodic>]";
      expect += "\n\t[--filter <string = LA8>]";
      expect += "\n\t[--help (includes lots of info)]";
      expect += "\n\t[--level <integer = 4>]";
      expect += "\n\t[--operation <string = smooth>]";
      expect += "\n\t[--prefix <string = ''>]";
      expect += "\n\t[--to-stdout]";
      expect += "\n\t<file-name>";
      expect += "\n";
      return(expect);
    }

    static std::string VerboseUsage() {
      std::string verbose = Usage();
      verbose += "\n";
      verbose += "\n\t--level is the max level to compute to\n";
      verbose += "\n\t--prefix is added to front of each output file name\n";
      verbose += "\n\t--to-stdout is applicable to --operation = scale|smooth";
      verbose += "\n";
      verbose += allowedOps();
      verbose += "\n";
      verbose += allowedFilters();
      verbose += "\n";
      verbose += allowedBoundaries();
      verbose += "\n";
      verbose += "\n\t<file-name> may be '-' to indicate reading from stdin";
      verbose += "\n";
      return(verbose);
    }

  private:
    std::string lc(const std::string& s) {
      std::string cpy(s);
      for ( std::string::size_type sz = 0; sz < s.size(); ++sz )
        cpy[sz] = std::tolower(cpy[sz]);
      return(cpy);
    }

    void setLevel(const std::string& s) {
      static const std::string plusInts = "0123456789";
      std::string::size_type sz = s.find_first_not_of(plusInts);
      std::string::size_type npos = std::string::npos;
      Ext::Assert<Ext::UserError>(sz == npos, "Not a +integer", s);
      std::stringstream converter(s);
      converter >> maxLevel_;
      Ext::Assert<Ext::UserError>(maxLevel_ > 0, "Not a +integer", s);
    }

    void setOperation(const std::string& s) {
      std::string op = lc(s);
      if ( op == "wave" )
        op_ = WAVE_COEFFS;
      else if ( op == "scale" )
        op_ = SCALE_COEFFS;
      else if ( op == "wave-scale" )
        op_ = WAVE_SCALE_COEFFS;
      else if ( op == "smooth" )
        op_ = SMOOTH;
      else if ( op == "details" )
        op_ = DETAILS;
      else if ( op == "mra" )
        op_ = MRA;
      else if ( op == "all" )
        op_ = ALL;
      else
        throw(Ext::UserError("Unknown --operation: " + s, allowedOps()));
    }

    static std::string allowedOps() {
      std::string val = "\n\tAllowed --operation list:\n";
      val += "\t\tall\n";
      val += "\t\tdetails\n";
      val += "\t\tmra\n";
      val += "\t\tscale\n";
      val += "\t\tsmooth\n";
      val += "\t\twave\n";
      val += "\t\twave-scale\n";
      return(val);
    }

    static std::string allowedFilters() {
      std::list< std::string > allFilts = WT::Filter::allFTypesStrings();
      std::string val = "\n\tAllowed --filter list:\n";
      std::list< std::string >::const_iterator i = allFilts.begin();
      const int maxPerLine = 10;
      int cnt = 0;
      while ( i != allFilts.end() ) {
        if ( ++cnt == 1 ) {
          val += "\t\t" + *i++;
        } else if ( cnt == maxPerLine ) {
          val += ", " + *i++ + "\n";
          cnt = 0;
        } else {
          val += ", " + *i++;
        }
      } // while
      return(val);
    }

    static std::string allowedBoundaries() {
      std::list< std::string > allBoundaries = WT::allBoundaryStrings();
      std::string val = "\n\tAllowed --boundary list:\n";
      std::list< std::string >::const_iterator i = allBoundaries.begin();
      while ( i != allBoundaries.end() )
        val += "\t\t" + *i++ + "\n";
      return(val);
    }

  private:
    std::string file_, fType_, bType_;
    Operation op_;
    int maxLevel_;
    bool toStdout_;
    std::string prefix_;
  };


  // forward decl
  template <typename X>
  void useAPI(std::vector<X>&, const Input&, std::size_t);

} // unnamed namespace



//========
// main()
//========
int main(int argc, char** argv)
{
  typedef float T;
  bool isError = true;

  try {
    // Check input
    Input input(argc, argv);
    Ext::FPWrap<Ext::InvalidFile> infile(input.File());

    // Read in all data
    std::vector<T> x; // our original series
    T d;
    std::string f = Formats::Format(T()) + std::string("\n");
    char const* format = f.c_str();
    while ( !std::feof(infile) ) { // read the whole file
      if ( EOF == std::fscanf(infile, format, &d) ) {
        std::fprintf(stderr, "Unable to read numeric input");
        return(EXIT_FAILURE);
      }
      x.push_back(d);
    } // while

    // Deal with possible reflected boundary
    std::size_t outputSize = x.size();
    WT::Boundary boundaryType = WT::selectBoundary(input.BoundaryType());
    if ( boundaryType == WT::Reflected ) {
      x.resize(2 * outputSize);
      for ( std::size_t i = 2 * outputSize - 1, j = 0; i >= outputSize; )
        x[i--] = x[j++];
    }

    // Lets perform the operation
    useAPI(x, input, outputSize);

    isError = false;
  } catch(Help& h) {
    isError = false;
    std::fprintf(stdout, "%s\n", Input::VerboseUsage().c_str());
  } catch(Ext::UserError& ue) {
    std::fprintf(stderr, "%s\n", ue.what());
    std::fprintf(stderr, "%s\n", Input::Usage().c_str());
  } catch(std::exception& i) {
    std::fprintf(stderr, "%s\n", i.what());
  } catch(...) {
    std::fprintf(stderr, "unknown error");
  }
  return(isError ? EXIT_FAILURE : EXIT_SUCCESS);
}



namespace {

  //==========
  // useAPI()
  //==========
  template <typename X>
  void useAPI(std::vector<X>& x, const Input& input, std::size_t outputSize) {
    // Create the requested wavelet filter and scaling filter
    WT::Filter::FType filterType = WT::Filter::selectFilter(input.FilterType());
    WT::Filter::WaveletFilter wavefilt = WT::Filter::getFilters<WT::MODWT>(filterType).first;
    WT::Filter::ScalingFilter scalefilt = WT::Filter::getFilters<WT::MODWT>(filterType).second;

    // Locals
    Operation op = input.Op();
    int maxLevel = input.MaxLevel();
    std::string prefix = input.Prefix();
    bool useStdout = input.StdOut();


    // All needed operations are defined here -> switch doesn't allow local
    //  variables within a case.

    // Scaling coefficient related operations
    std::string scaleName = prefix + "scaling-coefficients";
    WT::DoNothing vop0;
    WT::PrintValues vop1((useStdout ? "" : scaleName), outputSize, maxLevel);
    WT::SaveLastLevel<X> vop2(maxLevel); // retain values of last level

    // Wavelet coefficient related operations
    std::string waveletName = prefix + "wavelet-coefficients";
    WT::DoNothing wop0;
    WT::PrintValues wop1(waveletName, outputSize);
    // (currently unused) WT::SaveLastLevel<X> wop2(maxLevel);
    WT::SaveAllValues<X> wop3;

    // Operations related to the smooth
    std::string smoothName = prefix + "smoothing";
    // (currently unused) WT::DoNothing sop0;
    WT::PrintValues sop1((useStdout ? "" : smoothName), outputSize, maxLevel);

    // Operations related to the details
    std::string detailsName = prefix + "details";
    // (currently unused) WT::DoNothing dop0;
    WT::PrintLast dop1(detailsName, outputSize, 0); // special op for doAll() & mra()
    std::vector< WT::PrintValues > dops; // container of ops for details()
    for ( int i = 0; i < maxLevel; ++i )
      dops.push_back(WT::PrintValues(detailsName, outputSize, i+1));



    // I didn't implement anything for the library's imodwt() here
    // The library API and its usage below are meant to maximize runtime performance
    switch (op) {
      case WAVE_COEFFS:
        WT::modwt(x, wavefilt, scalefilt, maxLevel, vop0, wop1);
        break;
      case SCALE_COEFFS:
        WT::modwt(x, wavefilt, scalefilt, maxLevel, vop1, wop0);
        break;
      case WAVE_SCALE_COEFFS:
        WT::modwt(x, wavefilt, scalefilt, maxLevel, vop1, wop1);
        break;
      case SMOOTH:
        WT::modwt(x, wavefilt, scalefilt, maxLevel, vop2, wop0);
        WT::smooth(vop2.Values(), scalefilt, maxLevel, sop1);
        break;
      case DETAILS:
        WT::modwt(x, wavefilt, scalefilt, maxLevel, vop0, wop3);
        WT::details(wop3.Values(), wavefilt, scalefilt, dops);
        break;
      case MRA:
        WT::mra(x, maxLevel, filterType, dop1, sop1);
        break;
      default: // ALL
        WT::doAll(x, maxLevel, filterType, wop1, dop1, vop1, sop1);
    };
  }

} // unnamed
