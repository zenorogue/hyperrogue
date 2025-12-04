
namespace rogueviz {

namespace embeddings {

  double graph_R, graph_alpha, graph_T;

  struct polar_point {
    ld r, theta;
    };

  struct polar_embedding : public untiled_embedding {
    vector<polar_point> coords;

    hyperpoint as_hyperpoint(int id) override {
      return spin(coords[id].theta) * xpush0(coords[id].r);
      }

    ld zero_distance(int id) { return coords[id].r; }

    ld distance(int i, int j) {
      ld da = coords[i].r;
      ld db = coords[j].r;
      
      ld phi = coords[i].theta - coords[j].theta;

      ld co = sinh(da) * sinh(db) * (1 - cos(phi));
      
      ld v = cosh(da - db) + co;
      if(v < 1) return 0;
      
      return acosh(v);
      }

    };

  /** read polar coordinates, in the format returned by the BFKL embdder. */

  void read_polar(const string& fn) {

    fhstream f(fn, "rt");
    if(!f.f) return file_error(fn);

    int N;

    printf("Reading coordinates...\n");
    string ignore;
    if(!scan(f, ignore, ignore, ignore, ignore, N, graph_R, graph_alpha, graph_T)) {
      printf("Error: incorrect format of the first line\n"); exit(1);
      }

    if(graph_R && graph_T) cont_logistic.setRT(graph_R, graph_T);

    if(N == 0) N = isize(vdata);
    if(N != isize(vdata)) throw hr_exception("incorrect N");

    auto pe = std::make_shared<polar_embedding> ();
    pe->coords.resize(N);

    for(int i=0; i<N; i++) {
      string s = scan<string>(f);
      if(s == "" || s == "#ROGUEVIZ_ENDOFDATA") throw hr_exception("data failure");
      int id = rogueviz::labeler.at(s);

      double r, theta;
      if(!scan(f, r, theta)) { printf("Error: incorrect format of r/alpha\n"); exit(1); }
      pe->coords[id] = polar_point{.r = r, .theta = theta * degree};
      }
    
    enable_embedding(std::move(pe));
    }

  int a_polar =
    arg::add3("-el-polar", [] { arg::shift(); read_polar(arg::args()); })
  + arg::add3("-el-bfkl", [] { arg::shift(); read_edgelist(arg::args() + "-links.txt"); read_polar(arg::args() + "-coordinates.txt"); });
  
  }
}