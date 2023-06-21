#include <boost/multiprecision/mpfr.hpp>

#include "../../hyper.h"

#define TD template<class D>
#undef sl2

namespace reps {

using namespace boost::multiprecision;
using big = mpfr_float_50;
}

namespace hr {
  void print(hr::hstream& hs, ::reps::big b) {
    std::stringstream ss; 
    ss << std::setprecision(10);
    ss << b; string u; ss >> u; print(hs, u);
    }
}

namespace reps {
using std::array;
using std::vector;

using hr::cell;
using hr::print;
using hr::hlog;
using hr::celldistance;
using hr::ld;
using hr::ginf;
using hr::geometry;
using hr::gcHyperbolic;
using hr::gcSphere;
using hr::C02;
using hr::C03;
using hr::qANYQ;

template <class N> N get_deg(int deg);
template<> ld get_deg<ld> (int deg) { return M_PI*deg/180; }
template<> big get_deg<big> (int deg) { return atan(big(1))*deg/45; }

enum eNormalizeMode { 
  nmInvariant,   // if the input was normalized, the output will be normalized too
  nmForced,      // normalize the output
  nmWeak,        // weakly normalize the output
  nmCareless,    // do not try to keep the output normalized
  nmFlatten,     // flatten the representation
  nmBinary       // try to avoid overflow
  };

eNormalizeMode nm;

}

#include "counter.cpp"
#include "multivector.cpp"
#include "rep-hr.cpp"
#include "rep-multi.cpp"
#include "rep-halfplane.cpp"
#include "rep-polar.cpp"
#include "tests.cpp"

namespace reps {

// -- tests ---

void test_systems() {
  run_all_tests();
  fflush(stdout);
  exit(1);
  }

void set_repgeo() {
  if(test_dim == 3) { hr::set_geometry(hr::gNormal); hr::set_variation(hr::eVariation::pure); }
  if(test_dim == 4) { hr::set_geometry(hr::gSpace435); }
  }

int a = hr::arg::add1("-test-reps", test_systems) + hr::arg::add1("-repgeo", set_repgeo);

}
