// Voronoi used to measure the quality of the embedding (Villman's measure)
// Copyright (C) 2011-2022 Tehora and Zeno Rogue, see 'hyper.cpp' for details

#include "kohonen.h"

namespace rogueviz {

namespace voronoi {

void manifold::generate_data() {
  T = isize(triangles);
  triangles_of_tile.resize(N);

  for(int i=0; i<T; i++) {
    for(auto v: triangles[i])
      triangles_of_tile[v].push_back(i);
    for(int j=0; j<3; j++) {
      int e0 = triangles[i][j%3];
      int e1 = triangles[i][(j+1)%3];
      if(e1<e0) swap(e0, e1);
      auto p = make_pair(e0, e1);
      triangles_of_edge[p].push_back(i);
      }
    }  
  }

manifold build_manifold(const vector<cell*>& cells) {
  map<cell*, int> neuron_id;
  int N = isize(cells);
  for(int i=0; i<N; i++)
    neuron_id[cells[i]] = i;
  
  set<vector<int> > faces_seen;
  
  for(auto c: cells) {
    for(int i=0; i<c->type; i++) {
      cellwalker cw1(c, i);
      cellwalker cw2 = cw1;
      vector<int> tlist;
      do {
        cw2 += wstep;
        cw2++;
        auto p = at_or_null(neuron_id, cw2.at);
        if(!p) goto next;
        tlist.push_back(*p);
        }
      while(cw2 != cw1);
      if(1) {
        int minv = 0;
        for(int i=0; i<isize(tlist); i++)
          if(tlist[i] < tlist[minv])
            minv = i;
        vector<int> tlist_sorted;
        for(int i=minv; i<isize(tlist); i++)
          tlist_sorted.push_back(tlist[i]);
        for(int i=0; i<minv; i++)
          tlist_sorted.push_back(tlist[i]);
        if(tlist_sorted[1] > tlist_sorted.back())
          reverse(tlist_sorted.begin()+1, tlist_sorted.end());
        faces_seen.insert(tlist_sorted);
        }
      next: ;
      }
    }

  manifold m;
  m.N = N;
  for(const auto& v: faces_seen)
    for(int i=2; i<isize(v); i++)
      m.triangles.emplace_back(make_array(v[0], v[i-1], v[i]));

  m.generate_data();
  return m;
  }

vector<pair<int, int> > compute_voronoi_edges(manifold& m) {

  using kohonen::net;
  using kohonen::vnorm;
  using kohonen::vshift;
  using kohonen::data;
  using kohonen::kohvec;
  using kohonen::samples;
  
  vector<int> best_tile;
  /* for every neuron, compute its best tile */
  int N = isize(net);
  for(int i=0; i<N; i++) {
    ld bestval = HUGE_VAL, best_id = -1;
    for(int j=0; j<samples; j++) {
      ld val = vnorm(net[i].net, data[j].val);
      if(val < bestval) bestval = val, best_id = j;
      }
    best_tile.push_back(best_id);
    }
  
  constexpr int SUBD = 8;
  using neuron_triangle_pair = pair<int, int>;
  set< neuron_triangle_pair > visited;
  queue<neuron_triangle_pair> q;
  
  vector<kohvec> projected(N);
  
  auto visit = [&] (neuron_triangle_pair p) {
    if(visited.count(p)) return;
    visited.insert(p);
    q.push(p);
    };
  
  kohvec at;
  kohonen::alloc(at);

  auto project = [&] (kohvec& at, const array<int, 3>& tri, int i, int j) {
    int k = SUBD-i-j;
    for(auto& x: at) x = 0;
    vshift(at, data[tri[0]].val, i * 1. / SUBD);
    vshift(at, data[tri[1]].val, j * 1. / SUBD);      
    vshift(at, data[tri[2]].val, k * 1. / SUBD);
    };
  
  set<kohvec> already_picked;
  
  map<int, string> which_best;
  
  /* project all the net[ni].net on the manifold */
  for(int ni=0; ni<N; ni++) {
    kohvec best;
    int best_tri;
    ld best_dist = HUGE_VAL;
    reaction_t better = [] {};
    set<int> triangles_to_visit;
    queue<int> triangles_queue;
    
    auto visit1 = [&] (int tri) {
      if(triangles_to_visit.count(tri)) return;
      triangles_to_visit.insert(tri);
      triangles_queue.push(tri);
      };
    
    for(int tr: m.triangles_of_tile[best_tile[ni]]) 
      visit1(tr);
    
    auto& bes = which_best[ni];
    
    while(!triangles_queue.empty()) {
      int tri = triangles_queue.front();
      triangles_queue.pop();
      
      for(int i=0; i<=SUBD; i++)
      for(int j=0; j<=SUBD-i; j++) {
        project(at, m.triangles[tri], i, j);
        ld dist = vnorm(at, net[ni].net);
        if(dist < best_dist && !already_picked.count(at)) {
          best_dist = dist, best = at, best_tri = tri;
          bes = lalign(0, tie(tri, i, j));
          better = [&tri, i, j, &m, &visit1] () {
            auto flip_edge = [&] (int t1, int t2) {
              if(t2 < t1) swap(t1, t2);
              for(auto tri1: m.triangles_of_edge[{t1, t2}])
                visit1(tri1);
              };
            auto& tria = m.triangles[tri];
            if(i == 0) flip_edge(tria[1], tria[2]);
            if(j == 0) flip_edge(tria[0], tria[2]);
            if(i+j == SUBD) flip_edge(tria[0], tria[1]);
            };
          }
        }
      
      better();
      }
    projected[ni] = best;
    already_picked.insert(best);
    visit({ni, best_tri});
    }
  
  struct triangle_data {
    double dist[SUBD+1][SUBD+1];
    int which[SUBD+1][SUBD+1];
    triangle_data() {
      for(int i=0; i<=SUBD; i++)
      for(int j=0; j<=SUBD; j++)
        dist[i][j] = HUGE_VAL, which[i][j] = -1;
      }
    };
  
  vector<triangle_data> tdatas(m.T);

  while(!q.empty()) {
    auto ntp = q.front();
    q.pop();
    auto ni = ntp.first;
    auto& tri = m.triangles[ntp.second];
    auto& td = tdatas[ntp.second];
    
    for(int i=0; i<=SUBD; i++)
    for(int j=0; j<=SUBD-i; j++) {
      project(at, tri, i, j);
      ld dist = vnorm(at, projected[ni]);
      auto& odist = td.dist[i][j];
      bool tie = abs(dist - odist) < 1e-6;
      if(tie ? ni < td.which[i][j] : dist < odist) {
        td.dist[i][j] = dist, 
        td.which[i][j] = ni;
        auto flip_edge = [&] (int t1, int t2) {
          if(t2 < t1) swap(t1, t2);
          for(auto tr: m.triangles_of_edge[{t1, t2}]) {
            visit({ni, tr});
            }
          };
        if(i == 0) flip_edge(tri[1], tri[2]);
        if(j == 0) flip_edge(tri[0], tri[2]);
        if(i+j == SUBD) flip_edge(tri[0], tri[1]);
        }
      }      
    }
  
  set<pair<int, int> > voronoi_edges;
  
  auto add_edge = [&] (int i, int j) {
    if(i>j) swap(i, j);
    if(i==j) return;
    voronoi_edges.insert({i, j});
    };

  for(auto& td: tdatas) {
    for(int i=0; i<=SUBD; i++)
    for(int j=0; j<=SUBD-i; j++) {
      if(i>0) add_edge(td.which[i][j], td.which[i-1][j]);
      if(j>0) add_edge(td.which[i][j], td.which[i][j-1]);
      if(j>0) add_edge(td.which[i][j], td.which[i+1][j-1]);
      }
    }

  if(1) {
    vector<int> degs(N, 0);
    for(auto e: voronoi_edges) degs[e.first]++, degs[e.second]++;
    for(int v=0; v<N; v++) if(degs[v] == 0) {
      fhstream vorerr("voronoi-error.txt", "at");
      println(vorerr, "error: degree 0 vertex ", v, " in ", debug_str);
      println(vorerr, "best is: ", which_best[v]);
      int id = 0;
      for(auto& td: tdatas) {
        for(int i=0; i<=SUBD; i++)
        for(int j=0; j<=SUBD-i; j++) 
          if(td.which[i][j] == v) println(vorerr, "Found at ", tie(id, i, j));
        id++;
        }
      }
    }

  vector<pair<int, int> > result;
  for(auto ve: voronoi_edges) result.push_back(ve);
  return result;  
  }

}
}
