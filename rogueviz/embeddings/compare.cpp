#include "../som/kohonen.h"

namespace rogueviz {

namespace embeddings {

int count_directed_edges() {
  int M = 0;
  for(auto& v: directed_edges) M += isize(v);
  return M;
  }

void full_evaluation() {

  println(hlog, "evaluating embedding: " + current->name());

  try {
    ld maxradius = 0;  
    for(int i=0; i<isize(rogueviz::vdata); i++) maxradius = max(maxradius, current->zero_distance(i));
    println(hlog, "maximum radius = ", maxradius);
    }
  catch(hr_exception&) {}

  bool symmetric = count_directed_edges() == 2 * isize(edgeinfos);
  if(symmetric) full_routing();
  continuous_ranks();
  analyze_mdl_symmetric(symmetric);
  }

struct abstract_embedding : embedding {
  hyperpoint as_hyperpoint(int id) override { throw hr_exception("abstract embedding"); }
  pair<cell*, hyperpoint>  as_location(int id) override { throw hr_exception("abstract embedding"); }
  };

/* convert a hyperbolic embedding into a landscape embedding */

using rogueviz::kohonen::kohvec;
using rogueviz::kohonen::columns;

int landscape_dim;

vector<kohvec> landscape_weights;

ld dist(const kohvec& a, const kohvec& b) {
  ld res = 0;
  for(int i=0; i<columns; i++) res += (a[i]-b[i]) * (a[i] - b[i]);
  return res;
  }

struct landscape_embedding : abstract_embedding {
  virtual string name() override { return "landscape"; }
  eGeometryClass get_geometry() override { return gcEuclid; }
  int get_dimension() override { return isize(landscape_weights[0]); }
  ld distance(int i, int j) override {
    return dist(landscape_weights[i], landscape_weights[j]);
    }
  };

void landscape_build(int dim) {
  rogueviz::embeddings::init_landscape(columns = landscape_dim = dim);
  int N = isize(rogueviz::vdata);
  landscape_weights.resize(N);
  if(1) {
    progressbar pb(N, "compute landscape");
    for(int i=0; i<N; i++) {
      landscape_weights[i] = rogueviz::embeddings::get_landscape_at(current->as_location(i).first);
      pb++;
      }
    }
  println(hlog, "delta size = ", isize(rogueviz::embeddings::delta_at));
  enable_embedding(make_shared<landscape_embedding>());
  }

void landscape_output(string shape, ld target_dist, string fname) {
  ld max_dist = 0;
  int N = isize(rogueviz::vdata);
  for(int i=0; i<N; i++) {
    ld tot = 0;
    for(int d=0; d<columns; d++) tot += pow(landscape_weights[i][d], 2);
    tot = sqrt(tot);
    max_dist = max(max_dist, tot);
    }
  println(hlog, "max_dist = ", max_dist, " to ", target_dist);
  fhstream f(fname, "w");
  for(int i=0; i<N; i++) {
    ld tot = 0;
    println(f, rogueviz::vdata[i].name);
    for(int d=0; d<columns; d++) {
      auto w = landscape_weights[i][d] * target_dist / max_dist;
      if(d) print(f, " ");
      print(f, w);
      tot += w * w;
      }
    if(shape == "euclid") println(f);
    else if(shape == "poincare") println(f);
    else if(shape == "lorentz") println(f, " ", sqrt(tot+1));
    else if(shape == "horosphere") {
      tot /= 2;
      ld shift = -asinh(target_dist)/2;
      println(f, " ", cosh(shift) * tot + sinh(shift) * (1+tot), " ", cosh(shift) * (1+tot) + sinh(shift) * tot);
      }
    }
  }

/* read a distance table */

struct disttable_embedding : abstract_embedding {
  vector<vector<ld>> disttable;
  virtual string name() override { return "disttable"; }
  eGeometryClass get_geometry() override { return gcHyperbolic; }
  int get_dimension() override { return 1; }
  ld zero_distance(int id) override { throw hr_exception("unknown"); }
  ld distance(int i, int j) override { return disttable[i][j]; }
  };

struct zdisttable_embedding : disttable_embedding {
  vector<ld> dist0;
  virtual string name() override { return "zdisttable"; }
  ld zero_distance(int id) override { return dist0[id]; }
  };

void read_disttable(string nodelist_fname, string disttable_fname) {
  auto de = make_shared<disttable_embedding> ();
  int N = isize(rogueviz::vdata);

  fhstream g(nodelist_fname, "rt");
  vector<int> our_index;
  while(true) {
    int i = rogueviz::readLabel(g);
    if(i == -1) break;
    our_index.push_back(i);
    }
  println(hlog, "read ", isize(our_index), " node labels from ", nodelist_fname);
  if(isize(our_index) != N) { println(hlog, "wrong number of labels, ", N, " expected"); return; }

  ld maxdist = 0;

  de->disttable.resize(N);
  for(int i=0; i<N; i++) de->disttable[i].resize(N, 0);
  fhstream g1(disttable_fname, "rt");
  println(hlog, "reading distance table from ", disttable_fname);
  for(int i=0; i<N; i++)
  for(int j=0; j<i; j++) {
    ld dist = scan<ld>(g1);
    maxdist = max(dist, maxdist);
    de->disttable[our_index[i]][our_index[j]] = dist;
    de->disttable[our_index[j]][our_index[i]] = dist;
    }

  println(hlog, "maximum distance = ", maxdist);

  enable_embedding(de);
  }

/* read a 3D Mercator embeding */

struct mercator_coord {
  ld distance = -1;
  array<ld, 3> h;
  };

struct dmercator_embedding : abstract_embedding {
  vector<mercator_coord> mcs;
  virtual string name() override { return "dmercator"; }
  eGeometryClass get_geometry() override { return gcHyperbolic; }
  int get_dimension() override { return 3; }
  hyperpoint as_hyperpoint(int id) {
    auto& mc = mcs[id];
    hyperpoint h;
    for(int i=0; i<3; i++) 
      h[i] = sinh(mc.distance) * mc.h[i];
    h[3] = cosh(mc.distance);
    return h;
    }

  ld zero_distance(int id) override { return mcs[id].distance; }

  ld distance(int a, int b) {
    if(a == b) return ld(0);
    ld da = mcs[a].distance;
    ld db = mcs[b].distance;

    ld cosphi = 0;
    for(int i=0; i<3; i++) cosphi += mcs[a].h[i] * mcs[b].h[i];
  
    ld co = sinh(da) * sinh(db) * (1 - cosphi);  
    ld v = cosh(da - db) + co;
    if(v < 1) return ld(0);
  
    return acosh(v);
    };

  };


void read_dmercator(string fname, bool only_read = false) {
  auto me = make_shared<dmercator_embedding> ();
  int N = isize(rogueviz::vdata);

  me->mcs.resize(N);

  fhstream g(fname, "rt");

  while(true) {
    string s = scan<string>(g);
    if(s == "") break;
    if(s == "#") { scanline_noblank(g); continue; }
    int id = rogueviz::getid(s);
    if(id == -1) { printf("got -1\n"); exit(1); }
    ld kappa = scan<ld> (g); hr::ignore(kappa);
    auto& mc = me->mcs[id];
    mc.distance = scan<ld> (g);
    for(int i=0; i<3; i++) mc.h[i] = scan<ld> (g);
    ld radius = mc.h[0] * mc.h[0] + mc.h[1] * mc.h[1] + mc.h[2] * mc.h[2];
    radius = sqrt(radius);
    for(int i=0; i<3; i++) mc.h[i] /= radius;
    }

  for(auto& p: me->mcs) if(p.distance < 0) throw hr_exception("mercator read incorrectly");

  enable_embedding(me);
  }

/* read a high-dimensional Euclidean embeding */

struct euclid_embedding : zdisttable_embedding {
  vector<vector<ld>> coords;
  virtual string name() override { return "euclid"; }
  eGeometryClass get_geometry() override { return gcEuclid; }
  int get_dimension() override { return isize(coords[0]); }
  };

void read_euclid(string fname, int dim) {
  auto ee = make_shared<euclid_embedding> ();
  int N = isize(rogueviz::vdata);

  println(hlog, "Open file ", fname);
  fhstream g(fname, "rt");
  if(!g.f) return file_error(fname);
  println(hlog, "Reading Euclid (dim = ", dim, ")...");
  ee->coords.resize(N);
  ee->dist0.resize(N);
  while(true) {
    int i = rogueviz::readLabel(g);
    if(i == -1) break;
    vector<ld> co(dim);
    for(int i=0; i<dim; i++) co[i] = scan<ld>(g);

    ld d = 0; for(int i=0; i<dim; i++) d += co[i] * co[i];
    ee->dist0[i] = sqrt(d);

    ee->coords[i] = std::move(co);
    }

  ee->disttable.resize(N);
  for(int i=0; i<N; i++) ee->disttable[i].resize(N, 0);
  for(int i=0; i<N; i++)
  for(int j=0; j<=i; j++) {
    ld d = 0; for(int k=0; k<dim; k++) d += pow(ee->coords[i][k]-ee->coords[j][k], 2);
    ee->disttable[i][j] = ee->disttable[j][i] = sqrt(d);
    }

  enable_embedding(ee);
  }

/* read a high-dimensional Poincare embeding */

struct poincare_embedding : zdisttable_embedding {
  vector<vector<ld>> coords;
  virtual string name() override { return "poincare"; }
  eGeometryClass get_geometry() override { return gcHyperbolic; }
  int get_dimension() override { return isize(coords[0])-1; }
  };

void read_poincare(string fname, int dim) {
  auto pe = make_shared<poincare_embedding>();
  int N = isize(rogueviz::vdata);

  println(hlog, "Open file ", fname);
  fhstream g(fname, "rt");
  if(!g.f) return file_error(fname);
  println(hlog, "Reading Poincare (dim = ", dim, ")...");
  pe->coords.resize(N);
  pe->dist0.resize(N);

  while(true) {
    int i = rogueviz::readLabel(g);
    if(i == -1) break;
    vector<ld> co(dim+1);
    for(int i=0; i<dim; i++) co[i] = scan<ld>(g);

    ld d = 0; for(int i=0; i<dim; i++) d += co[i] * co[i];
    hyperpoint h; h[0] = sqrt(d); h[1] = 0; auto h1 = perspective_to_space(h, 1);

    for(int i=0; i<dim; i++) co[i] = co[i] * h1[0] / d; co[dim] = h1[2];

    pe->dist0[i] = acos_auto_clamp(h1[2]);
    pe->coords[i] = std::move(co);
    }

  pe->disttable.resize(N);
  for(int i=0; i<N; i++) pe->disttable[i].resize(N, 0);
  for(int i=0; i<N; i++)
  for(int j=0; j<=i; j++) {
    ld d = 0; for(int k=0; k<dim; k++) d += pow(pe->coords[i][k]-pe->coords[j][k], 2);
    d -= pow(pe->coords[i][dim]-pe->coords[j][dim], 2);
    pe->disttable[i][j] = pe->disttable[j][i] = acos_auto_clamp(d);
    }

  enable_embedding(pe);
  }

int compare_args() {
  using namespace arg;

  if(argis("-fulleval")) {
    full_evaluation();
    }
  else if(argis("-el-disttable")) {
    shift(); string nodelist_fname = args();
    shift(); string disttable_fname = args();
    read_disttable(nodelist_fname, disttable_fname);
    }
  else if(argis("-el-dmercator")) {
    shift(); read_dmercator(args());
    }
  else if(argis("-el-poincare")) {
    shift(); string fname = args();
    shift(); int dim = argi();
    read_poincare(fname, dim);
    }
  else if(argis("-el-euclid")) {
    shift(); string fname = args();
    shift(); int dim = argi();
    read_euclid(fname, dim);
    }
  else if(argis("-build-landscape")) {
    shift(); landscape_build(argi());
    }
  else return 1;

  return 0;
  }

auto acmp = addHook(hooks_args, 50, compare_args);

}
}
