namespace nilrider {

bool nospeed = false;
int goal_id = 0;
ld solver_unit = .25;

void level::solve() {

  ld xunit = solver_unit, yunit = solver_unit, eunit = xunit * yunit / 2;
  
  struct edge {
    int id;
    int dz;
    ld zval1;
    ld length;
    };
  
  struct vertex {
    int id;
    int x, y;
    flagtype collected;
    ld zval;
    hyperpoint where;
    bool goal;
    map<int, pair<ld, pair<int, int> > > visited;
    vector<edge> edges;
    };
  
  map<tuple<int, int, flagtype>, int> xy_to_id;
  vector<vertex> vertices;
  
  auto getpt = [&] (int x, int y) {
    hyperpoint p = point31(start.where[0] + x * xunit, start.where[1] + y * yunit, 0);
    p[2] = surface(p);
    return p;
    };

  auto get_id = [&] (int x, int y, flagtype co) {
    if(xy_to_id.count({x, y, co}))
      return xy_to_id[{x, y, co}];
    else {
      int id = isize(vertices);
      xy_to_id[{x,y, co}] = id;
      vertices.emplace_back();
      auto& b = vertices.back();
      b.where = getpt(x, y);
      b.id = id;
      b.x = x; b.y = y;
      b.collected = co;
      return id;
      }
    };
  
  get_id(0, 0, 0);
  transmatrix Rstart = gpushxto0(vertices[0].where);
  
  for(int id=0; id<isize(vertices); id++) {
    auto& v = vertices[id];

    /* we need to copy because getpt may invalidate v */
    auto x0 = v.x;
    auto y0 = v.y;
    auto point0 = v.where;

    xy_float f0 = get_xy_f(point0);

    timestamp ts;
    ts.where = point0;
    ts.collected_triangles = v.collected;
    ts.timer = 0;
    loaded_or_planned = true;
    ts.collect(this);
    checkerparam p {&ts, this, 0};
    auto res = goals[goal_id].check(p);
    if(res == grFailed) continue;

    v.goal = res == grSuccess;
    v.zval = (Rstart * point0)[2];

    for(int dx=-2; dx<=2; dx++)
    for(int dy=-2; dy<=2; dy++) if(dx%2 || dy%2) {
      int x1 = x0 + dx;
      int y1 = y0 + dy;
      edge e;
      e.dz = (x1 + x1) * (y1 - y0);

      hyperpoint point1 = getpt(x1, y1);
      e.zval1 = (Rstart * point1)[2];
      
      xy_float f1 = get_xy_f(point1);
      
      int txmin = floor(min(f0.first, f1.first) - 1e-3);
      int txmax = floor(max(f0.first, f1.first) + 1e-3);
      int tymin = floor(min(f0.second, f1.second) - 1e-3);
      int tymax = floor(max(f0.second, f1.second) + 1e-3);
      bool bad = false;
      for(int tyi=tymin; tyi<=tymax; tyi++)
      for(int txi=txmin; txi<=txmax; txi++)
        if(among(mapchar(xy_int{txi, tyi}), '!', 'r')) bad = true;
      if(bad) continue;

      hyperpoint rpoint = gpushxto0(point1) * point0;
      rpoint[2] -= rpoint[0] * rpoint[1] / 2;
      e.length = hypot_d(3, rpoint);
      e.id = get_id(x1, y1, ts.collected_triangles);
      
      vertices[id].edges.push_back(e);
      }
    }
  
  std::priority_queue<pair<ld, pair<int, int> > > dijkstra_queue;
  
  auto visit = [&] (ld nt, int id, int z, int bid, int bz) {
    auto& t = vertices[id].visited[z];
    if(t.first == 0 || t.first > nt) {
      t.first = nt;
      t.second = {bid, bz};
      pair<ld, pair<int, int> > d = {-nt, {id, z}};
      dijkstra_queue.emplace(d);
      }
    };
  
  visit(1e-15, 0, 0, 0, 0); /* more than 0 to mark it */
  
  // h[0] * h[1] / 2 yields 0
  
  int step = 0;
  
  while(!dijkstra_queue.empty()) {
    auto q = dijkstra_queue.top();
    dijkstra_queue.pop();
    ld t0 = -q.first;
    int id0 = q.second.first;
    int z0 = q.second.second;
    auto& v = vertices[id0];
    
    if(step % 10000 == 0) println(hlog, t0, " : ", tie(id0, z0), " edges = ", isize(v.edges));
    step++;
    
    if(v.goal) {
      if(nospeed && z0 * eunit - v.zval > eunit) continue;
      println(hlog, "reached the goal in time ", t0);
      vector<hyperpoint> positions;
      while(id0 || z0) {
        println(hlog, "z = ", z0);
        positions.emplace_back(vertices[id0].where);
        auto& b = vertices[id0].visited[z0];
        id0 = b.second.first;
        z0 = b.second.second;
        }
      positions.emplace_back(vertices[0].where);
      reverse(positions.begin(), positions.end());
      println(hlog, positions);
      plan.clear();
      for(auto pos: positions) {
        plan.emplace_back(pos, hpxy(0, 0));
        }
      return;
      }
    
    for(auto& e: v.edges) {
      int z1 = z0 + e.dz;

      ld energy0 = z0 * eunit - v.zval;
      ld energy1 = z1 * eunit - e.zval1;

      if(energy0 < -1e-6) continue;
      if(energy0 < 0) energy0 = 0;
      if(energy1 < -1e-6) continue;
      if(energy1 < 0) energy1 = 0; 
       
      ld t1 = t0 + e.length / (sqrt(energy0) + sqrt(energy1));
      visit(t1, e.id, z1, id0, z0);
      }
    // exit(1);
    }
  
  println(hlog, "failed to reach the goal!");
  exit(1);
  }

}

