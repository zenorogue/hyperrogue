namespace dhrg {
  double graph_R, graph_alpha, graph_T;
  vector<pair<double, double> > coords;
  
  rogueviz::edgetype *any;
  
  int N;
               
  void tst() {}

  void read_graph(string fn) {

    any = rogueviz::add_edgetype("embedded edges");
    rogueviz::fname = fn;
    fhstream f(fn + "-coordinates.txt", "rt");
    if(!f.f) {
      printf("Missing file: %s-coordinates.txt\n", rogueviz::fname.c_str());
      exit(1);
      }
    printf("Reading coordinates...\n");
    string ignore;
    if(!scan(f, ignore, ignore, ignore, ignore, N, graph_R, graph_alpha, graph_T)) {
      printf("Error: incorrect format of the first line\n"); exit(1);
      }
    rogueviz::vdata.reserve(N);
    while(true) {
      string s = scan<string>(f);
      if(s == "D11.11") tst();
      if(s == "" || s == "#ROGUEVIZ_ENDOFDATA") break;
      int id = rogueviz::getid(s);
      rogueviz::vertexdata& vd(rogueviz::vdata[id]);
      vd.name = s;
      vd.cp = rogueviz::colorpair(rogueviz::dftcolor);
      
      double r, alpha;
      if(!scan(f, r, alpha)) { printf("Error: incorrect format of r/alpha\n"); exit(1); }
      coords.push_back(make_pair(r, alpha));
  
      transmatrix h = spin(alpha * degree) * xpush(r);

      vd.be(currentmap->gamestart(), h);
      }
    
    fhstream g(fn + "-links.txt", "rt");
    if(!g.f) {
      println(hlog, "Missing file: ", rogueviz::fname, "-links.txt");
      exit(1);
      }
    println(hlog, "Reading links...");
    int qlink = 0;
    while(true) {
      int i = rogueviz::readLabel(g), j = rogueviz::readLabel(g);
      if(i == -1 || j == -1) break;
      addedge(i, j, 1, any);
      qlink++;
      }
    }
  
  void unsnap() {
    for(int i=0; i<N; i++) {
      using rogueviz::vdata;
      transmatrix h = spin(coords[i].second * degree) * xpush(coords[i].first);
      vdata[i].be(currentmap->gamestart(), h);
      }
    }
  }
