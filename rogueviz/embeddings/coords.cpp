
namespace rogueviz {

namespace embeddings {

  enum class fmt { hyperb, poincare, bhyper };
  
  string format_names[3] = { "hyperboloid trailing", "Poincare", "hyperboloid leading" };

  fmt coord_format;

  struct coord_embedding : public untiled_embedding {
    vector<hyperpoint> coords;

    hyperpoint as_hyperpoint(int id) override {
      return coords[id];
      }

    virtual string name() override { return "coordinates: " + format_names[(int) coord_format]; }

    ld zero_distance(int id) override { return hdist0(coords[id]); }

    ld distance(int i, int j) override {
      return precise_hdist(coords[i], coords[j]);
      }

    void save(fhstream& f) override {
      int i = 0;
      for(auto& co: coords) {
        println(f, vdata[i++].name);
        switch(coord_format) {
          case fmt::hyperb:
            for(int i=0; i<MDIM; i++) println(f, format("%.20lf", co[i]));
            break;
          case fmt::bhyper:
            println(f, format("%.20lf", co[MDIM-1]));
            for(int i=0; i<MDIM-1; i++) println(f, format("%.20lf", co[i]));
            break;
          case fmt::poincare:
            auto h1 = space_to_perspective(co, 1);
            for(int i=0; i<MDIM-1; i++) println(f, format("%.20lf", h1[i]));
            break;
          }
        }
      }

    };

  /** read polar coordinates, in the format returned by the BFKL embdder. */

  void read_coord(const string& fn) {

    auto pe = std::make_shared<coord_embedding> ();
    int N = isize(vdata);
    pe->coords.resize(N);

    if(fn == "-") {
      for(int i=0; i<N; i++) {
        pe->coords[i] = current->as_hyperpoint(i);
        }
      return enable_embedding(pe);
      }

    fhstream f(fn, "rt");
    if(!f.f) return file_error(fn);

    for(int i=0; i<N; i++) {
      string s = scan<string>(f);
      if(s == "") throw hr_exception("data failure");
      int id = rogueviz::labeler.at(s);
      auto& co = pe->coords[id];
      switch(coord_format) {
        case fmt::hyperb:
          for(int i=0; i<MDIM; i++) co[i] = scan<ld>(f);
          break;
        case fmt::bhyper:
          co[MDIM-1] = scan<ld>(f);
          for(int i=0; i<MDIM-1; i++) co[i] = scan<ld>(f);
          break;
        case fmt::poincare:
          hyperpoint h1;
          for(int i=0; i<MDIM-1; i++) h1[i] = scan<ld>(f);
          co = perspective_to_space(h1, 1);
          break;
        }
      co = normalize(co);
      }
    
    enable_embedding(pe);
    }

  int a_coord =
    arg::add3("-el-coord", [] { arg::shift(); read_coord(arg::args()); })
  + arg::add3("-ec-hyperb", [] { coord_format = fmt::hyperb; })
  + arg::add3("-ec-bhyper", [] { coord_format = fmt::bhyper; })
  + arg::add3("-ec-poincare", [] { coord_format = fmt::poincare; });
  
  }
}