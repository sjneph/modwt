/*
  FILE: WTFilter.cpp
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

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>
#include <utility>
#include <vector>

#include "Assertion.hpp"
#include "Exception.hpp"
#include "WTFilter.hpp"


namespace WT {

  namespace Filter {

    namespace Details {

      //=============
      // IsSame<T,U>
      //=============
      template <typename T, typename U>
      struct IsSame {
        static const bool same = false;
      };

      template <typename T>
      struct IsSame<T,T> {
        static const bool same = true;
      };


      //===================
      // quadraticMirror()
      //===================
      template <typename RIter, typename Iter>
      void quadraticMirror(RIter rbeg, RIter rend, Iter out, bool invert = false) {
        /* Require a reverse_iterator<T> as RIter
            : std::copy_backwards() misses -1*T assignments
        */
        int inv = invert ? 1 : -1;
        while ( rbeg != rend ) {
          *out++ = (*rbeg++) * inv;
          inv = -inv;
        } // while
      }

      //===============
      // make_filter()
      //===============
      template <typename WT, typename T>
      std::pair< std::vector<T>, std::vector<T> > make_filter(T const* t, std::size_t sz) {
        std::vector<T> g(t, t+sz), h(g.size());
        if ( IsSame< WT, MODWT >::same )
          std::transform(g.begin(), g.end(), g.begin(), std::bind2nd(std::divides<T>(), std::sqrt(2)));
        quadraticMirror(g.rbegin(), g.rend(), h.begin(), true);
        return(std::make_pair(h, g));
      }


      //========
      // Haar()
      //========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > Haar() {
        double garr[] = { 0.707106781186547, 0.707106781186547 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //=========
      // Daub4()
      //=========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > Daub4() {
        double garr[] = { 0.482962913144534, 0.836516303737808,
                          0.224143868042013, -0.129409522551260 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //=========
      // Daub6()
      //=========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > Daub6() {
        double garr[] = { 0.332670552950083, 0.806891509311093,
                          0.459877502118491, -0.135011020010255,
                         -0.0854412738820267, 0.0352262918857096 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //=========
      // Daub8()
      //=========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > Daub8() {
        double garr[] = { 0.230377813307443, 0.714846570548406,
                          0.630880767935879, -0.0279837694166834,
                          -0.187034811717913, 0.0308413818353661,
                          0.0328830116666778, -0.0105974017850021 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //==========
      // Daub10()
      //==========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > Daub10() {
        double garr[] = { 0.160102397974193,   0.60382926979719,
                          0.724308528437773,   0.138428145901320,
                         -0.242294887066382,  -0.0322448695846381, 
                          0.0775714938400459, -0.0062414902127983,
                         -0.012580751999082,   0.0033357252854738 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //==========
      // Daub12()
      //==========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > Daub12() {
        double garr[] = { 0.111540743350109, 0.494623890398453,
                          0.751133908021095, 0.315250351709198,
                         -0.22626469396544, -0.129766867567262, 
                          0.0975016055873224, 0.0275228655303053,
                         -0.0315820393174862, 0.0005538422011614,
                          0.0047772575109455, -0.0010773010853085 };                       
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //==========
      // Daub14()
      //==========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > Daub14() {
        double garr[] = { 0.0778520540850081, 0.396539319481914,
                          0.729132090846237, 0.469782287405215,
                         -0.143906003928529, -0.224036184993854,
                          0.0713092192668312, 0.080612609151082,
                         -0.0380299369350125, -0.0165745416306664,
                          0.0125509985560993, 0.0004295779729214, 
                         -0.0018016407040474, 0.0003537137999745 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //==========
      // Daub16()
      //==========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > Daub16() {
        double garr[] = { 0.0544158422431049, 0.312871590914303,
                          0.67563073629729, 0.585354683654191,
                         -0.0158291052563816, -0.284015542961570,
                          0.0004724845739124, 0.128747426620484,
                         -0.0173693010018083, -0.0440882539307952,
                          0.0139810279173995, 0.0087460940474061,
                         -0.0048703529934518, -0.000391740373377,
                          0.0006754494064506, -0.0001174767841248 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //==========
      // Daub18()
      //==========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > Daub18() {
        double garr[] = { 0.0380779473638791, 0.243834674612594,
                          0.604823123690116, 0.657288078051296,
                          0.133197385824993, -0.293273783279176,
                         -0.0968407832229524, 0.148540749338131,
                          0.0307256814793395, -0.0676328290613302,
                          0.000250947114834, 0.0223616621236805,
                         -0.004723204757752, -0.0042815036824636,
                          0.0018476468830564, 0.0002303857635232,
                         -0.0002519631889427, 0.0000393473203163 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //==========
      // Daub20()
      //==========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > Daub20() {
        double garr[] = { 0.0266700579005546, 0.188176800077686,
                          0.52720118893172, 0.688459039453625,
                          0.281172343660649, -0.249846424327228, 
                         -0.19594627437734, 0.127369340335789,
                          0.0930573646035802, -0.0713941471663697,
                         -0.029457536821848, 0.0332126740593703, 
                          0.003606553566988, -0.0107331754833036,
                          0.0013953517470692, 0.001992405295193,
                         -0.0006858566949566, -0.0001164668551285, 
                          0.0000935886703202 -0.0000132642028945 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }


      //=======
      // LA8()
      //=======
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > LA8() {
        double garr[] = { -0.0757657147893407, -0.0296355276459541,
                           0.497618667632458, 0.803738751805216,
                           0.297857795605542, -0.0992195435769354,
                          -0.0126039672622612, 0.0322231006040713 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //========
      // LA10()
      //========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > LA10() {
        double garr[] = { 0.0195388827353869, -0.0211018340249298,
                         -0.175328089908107, 0.0166021057644243,
                          0.633978963456949, 0.723407690403808,
                          0.199397533976996, -0.0391342493025834,
                          0.0295194909260734, 0.0273330683451645 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //========
      // LA12()
      //========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > LA12() {
        double garr[] = { 0.0154041093273377, 0.0034907120843304,
                         -0.117990111148411, -0.0483117425859981,
                          0.49105594192764, 0.787641141028794, 
                          0.33792942172824, -0.0726375227866,
                         -0.0210602925126954, 0.0447249017707482,
                          0.0017677118643983, -0.007800708324765 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //========
      // LA14()
      //========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > LA14() {
        double garr[] = { 0.0102681767084968, 0.0040102448717033,
                         -0.107808237703617, -0.140047240442703,
                          0.288629631750983, 0.767764317004571,
                          0.536101917090772, 0.0174412550871099,
                         -0.049552834937041, 0.0678926935015971,
                          0.0305155131659062, -0.0126363034031526,
                         -0.0010473848889657, 0.0026818145681164 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //========
      // LA16()
      //========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > LA16() {
        double garr[] = { -0.0033824159513594, -0.0005421323316355,
                           0.0316950878103452, 0.0076074873252848,
                          -0.143294238351054, -0.0612733590679088,
                           0.481359651259201, 0.777185751699748, 
                           0.364441894835956, -0.0519458381078751,
                          -0.0272190299168137, 0.0491371796734768,
                           0.0038087520140601, -0.0149522583367926, 
                          -0.0003029205145516, 0.0018899503329007 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //========
      // LA18()
      //========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > LA18() {
        double garr[] = { 0.0010694900326538, -0.0004731544985879,
                         -0.0102640640276849, 0.0088592674935117,
                          0.0620777893027638, -0.0182337707798257, 
                         -0.191550831296487, 0.0352724880359345,
                          0.617338449141352, 0.717897082764226,
                          0.238760914607418, -0.0545689584305765, 
                          0.0005834627463312, 0.0302248788579895,
                         -0.0115282102079848, -0.0132719677815332,
                          0.0006197808890549, 0.0014009155255716 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //========
      // LA20()
      //========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > LA20() {
        double garr[] = { 0.000770159809103, 0.0000956326707837,
                         -0.0086412992759401, -0.0014653825833465,
                          0.0459272392237649, 0.0116098939129724,
                         -0.159494278857531, -0.0708805358108615,
                          0.471690666842659, 0.769510037014339,
                          0.383826761225382, -0.0355367403054689,
                         -0.0319900568281631, 0.049994972079156,
                          0.0057649120455518, -0.020354939803946,
                         -0.000804358934537, 0.0045931735836703,
                          0.000057036084339, -0.0004593294205481 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //========
      // BL14()
      //========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > BL14() {
        double garr[] = { 0.0120154192834842, 0.0172133762994439,
                         -0.0649080035533744, -0.064131289818917,
                          0.360218460898555, 0.781921593296555,
                          0.483610915693782, -0.0568044768822707,
                         -0.101010920866413, 0.0447423494687405,
                          0.0204642075778225, -0.0181266051311065,
                         -0.0032832978473081, 0.0022918339541009 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //========
      // BL18()
      //========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > BL18() {
        double garr[] = { 0.0002594576266544,  -0.0006273974067728,
                         -0.0019161070047557, 0.0059845525181721,
                          0.0040676562965785,  -0.0295361433733604,
                         -0.0002189514157348, 0.0856124017265279,
                         -0.0211480310688774, -0.143292975939652,
                          0.233778290022498, 0.737470761993369,
                          0.592655137443396, 0.0805670008868546,
                         -0.114334306961931, -0.0348460237698368,
                          0.0139636362487191, 0.0057746045512475 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //========
      // BL20()
      //========
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > BL20() {
        double garr[] = { 0.0008625782242896, 0.0007154205305517,
                         -0.0070567640909701, 0.0005956827305406,
                          0.0496861265075979, 0.0262403647054251,
                         -0.121552106157816, -0.0150192395413644,
                          0.513709872833405, 0.766954836501085,
                          0.340216013511079, -0.0878787107378667,
                         -0.0670899071680668, 0.0338423550064691,
                         -0.0008687519578684, -0.0230054612862905,
                         -0.0011404297773324, 0.0050716491945793, 
                          0.0003401492622332, -0.0004101159165852
                        };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //======
      // C6()
      //======
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > C6() {
        double garr[] = {-0.0156557285289848, -0.0727326213410511,
                          0.384864856538113, 0.85257204164239,
                          0.337897670951159, -0.0727322757411889 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //=======
      // C12()
      //=======
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > C12() {
        double garr[] = { -0.0007205494453679, -0.0018232088707116,
                           0.0056114348194211, 0.0236801719464464,
                          -0.0594344186467388, -0.0764885990786692,
                           0.417005184423671, 0.812723635449398,
                           0.386110066822994, -0.0673725547222826,
                          -0.0414649367819558, 0.0163873364635998 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //=======
      // C18()
      //=======
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > C18() {
        double garr[] = { -0.0000345997728362, -0.0000709833031381,
                           0.0004662169601129, 0.0011175187708906,
                          -0.0025745176887502, -0.0090079761366615,
                           0.0158805448636158, 0.0345550275730615,
                          -0.0823019271068856, -0.0717998216193117,
                           0.428483476377617, 0.793777222625617,
                           0.405176902409615, -0.0611233900026726,
                          -0.0657719112818552, 0.0234526961418362,
                           0.0077825964273254, -0.003793512864491 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //=======
      // C24()
      //=======
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > C24() {
        double garr[] = { -0.0000017849850031, -0.0000032596802369,
                           0.0000312298758654, 0.000062339034461,
                          -0.0002599745524878, -0.0005890207562444, 
                           0.0012665619292991, 0.003751436157279,
                          -0.0056582866866115, -0.0152117315279485,
                           0.0250822618448678, 0.0393344271233433, 
                          -0.096220442034002, -0.0666274742634348,
                           0.434386056491532, 0.782238930920613,
                           0.415308407030491, -0.056077313316763, 
                          -0.0812666996808907, 0.026682300156057,
                           0.0160689439647787, -0.0073461663276432,
                          -0.001629492012602, 0.0008923136685824 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

      //=======
      // C30()
      //=======
      template <typename WT>
      std::pair< WaveletFilter, ScalingFilter > C30() {
        double garr[] = { -0.0000000951765727, -0.0000001674428858,
                           0.0000020637618516, 0.0000037346551755,
                          -0.0000213150268122, -0.0000413404322768,
                           0.0001405411497166, 0.0003022595818445,
                          -0.0006381313431115, -0.001662863702186,
                           0.0024333732129107, 0.0067641854487565,
                          -0.0091642311634348, -0.0197617789446276,
                           0.0326835742705106, 0.0412892087544753,
                          -0.105574208714317, -0.0620359639693546, 
                           0.437991626217383, 0.774289603733474,
                           0.42156620673469, -0.0520431631816557,
                          -0.0919200105692549, 0.0281680289738655, 
                           0.0234081567882734, -0.0101311175209033,
                          -0.0041593587818186, 0.0021782363583355,
                           0.000358589687933, -0.0002120808398259 };
        return( Details::make_filter<WT>(garr, sizeof(garr) / sizeof(double)) );
      }

    } // namespace Details

  } // namespace Filter




  namespace Filter {

    //==============
    // getFilters()
    //==============
    template <typename WT>
    std::pair< WaveletFilter, ScalingFilter > getFilters(FType ft) {
      switch(ft) {
        case Haar:
          return Details::Haar<WT>();
        case D4:
          return Details::Daub4<WT>();
        case D6:
          return Details::Daub6<WT>();
        case D8:
          return Details::Daub8<WT>();
        case D10:
          return Details::Daub10<WT>();
        case D12:
          return Details::Daub12<WT>();
        case D14:
          return Details::Daub14<WT>();
        case D16:
          return Details::Daub16<WT>();
        case D18:
          return Details::Daub18<WT>();
        case D20:
          return Details::Daub20<WT>();
        case LA8:
          return Details::LA8<WT>();
        case LA10:
          return Details::LA10<WT>();
        case LA12:
          return Details::LA12<WT>();
        case LA14:
          return Details::LA14<WT>();
        case LA16:
          return Details::LA16<WT>();
        case LA18:
          return Details::LA18<WT>();
        case LA20:
          return Details::LA20<WT>();
        case BL14:
          return Details::BL14<WT>();
        case BL18:
          return Details::BL18<WT>();
        case BL20:
          return Details::BL20<WT>();
        case C6:
          return Details::C6<WT>();
        case C12:
          return Details::C12<WT>();
        case C18:
          return Details::C18<WT>();
        case C24:
          return Details::C24<WT>();
        case C30:
          return Details::C30<WT>();
        default:
          throw(Ext::ArgumentError("getFilters()", "no FType match"));
      };
      return Details::LA8<WT>(); // never reach here
    }

    //================
    // selectFilter()
    //================
    FType selectFilter(const std::string& ftype) {
      std::string nocase = ftype;
      for ( std::string::size_type sz = 0; sz < nocase.size(); ++sz )
        nocase[sz] = std::tolower(nocase[sz]);

      WT::Filter::FType ft;
      if ( nocase == "haar" )
        ft = WT::Filter::Haar;
      else if ( nocase == "d4" )
        ft = WT::Filter::D4;
      else if ( nocase == "d6" )
        ft = WT::Filter::D6;
      else if ( nocase == "d8" )
        ft = WT::Filter::D8;
      else if ( nocase == "d10" )
        ft = WT::Filter::D10;
      else if ( nocase == "d12" )
        ft = WT::Filter::D12;
      else if ( nocase == "d14" )
        ft = WT::Filter::D14;
      else if ( nocase == "d16" )
        ft = WT::Filter::D16;
      else if ( nocase == "d18" )
        ft = WT::Filter::D18;
      else if ( nocase == "d20" )
        ft = WT::Filter::D20;
      else if ( nocase == "la8" )
        ft = WT::Filter::LA8;
      else if ( nocase == "la10" )
        ft = WT::Filter::LA10;
      else if ( nocase == "la12" )
        ft = WT::Filter::LA12;
      else if ( nocase == "la14" )
        ft = WT::Filter::LA14;
      else if ( nocase == "la16" )
        ft = WT::Filter::LA16;
      else if ( nocase == "la18" )
        ft = WT::Filter::LA18;
      else if ( nocase == "la20" )
        ft = WT::Filter::LA20;
      else if ( nocase == "bl14" )
        ft = WT::Filter::BL14;
      else if ( nocase == "bl18" )
        ft = WT::Filter::BL18;
      else if ( nocase == "bl20" )
        ft = WT::Filter::BL20;
      else if ( nocase == "c6" )
        ft = WT::Filter::C6;
      else if ( nocase == "c12" )
        ft = WT::Filter::C12;
      else if ( nocase == "c18" )
        ft = WT::Filter::C18;
      else if ( nocase == "c24" )
        ft = WT::Filter::C24;
      else if ( nocase == "c30" )
        ft = WT::Filter::C30;
      else
        throw(Ext::ArgumentError("selectFilter()", "No FType match"));

      return(ft);
    }

    //===================
    // allFTypeStrings()
    //===================
    std::list< std::string > allFTypesStrings() {
      std::list< std::string > allFTypes;
      allFTypes.push_back("Haar");
      allFTypes.push_back("D4");
      allFTypes.push_back("D6");
      allFTypes.push_back("D8");
      allFTypes.push_back("D10");
      allFTypes.push_back("D12");
      allFTypes.push_back("D14");
      allFTypes.push_back("D16");
      allFTypes.push_back("D18");
      allFTypes.push_back("D20");
      allFTypes.push_back("LA8");
      allFTypes.push_back("LA10");
      allFTypes.push_back("LA12");
      allFTypes.push_back("LA14");
      allFTypes.push_back("LA16");
      allFTypes.push_back("LA18");
      allFTypes.push_back("LA20");
      allFTypes.push_back("BL14");
      allFTypes.push_back("BL18");
      allFTypes.push_back("BL20");
      allFTypes.push_back("C6");
      allFTypes.push_back("C12");
      allFTypes.push_back("C18");
      allFTypes.push_back("C24");
      allFTypes.push_back("C30");
      return(allFTypes);
    }

    //=============
    // allFTypes()
    //=============
    std::list< FType > allFTypes() {
      std::list< FType > allFTypes;
      std::list< std::string > allStrings = allFTypesStrings();
      std::list< std::string >::const_iterator i = allStrings.begin();
      while ( i != allStrings.end() )
        allFTypes.push_back(selectFilter(*i++));
      return(allFTypes);
    }

  } // namespace Filter

} // namespace WT
