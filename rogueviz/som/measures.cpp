// test measures
// Copyright (C) 2011-2022 Tehora and Zeno Rogue, see 'hyper.cpp' for details

#include "kohonen.h"

namespace rogueviz {
namespace measures {

double kendall(const vector<pair<int, int>>& allp) {
  int maxo = 0, maxe = 0;
  for(const auto& a: allp) maxo = max(maxo, a.first), maxe = max(maxe, a.second);
  maxo++; maxe++;
  
  if(maxo >= 256 || maxe >= 256) {
    println(hlog, "more than 256!\n");
    exit(1);
    }
  int cnt[256][256];
  for(int a=0; a<maxo; a++)
  for(int b=0; b<maxe; b++)
    cnt[a][b] = 0;
  for(const auto& a: allp) cnt[a.first][a.second]++;

  // int i1 = 0, i2 = 0;
  int K = isize(allp);
  // allp.emplace_back(maxo, maxe);
  vector<int> counts(maxe, 0);
  vector<int> totals(maxe);
  double tau = 0;
  for(int i=0; i<maxo; i++) {
    totals[0] = 0;
    for(int ii=1; ii<maxe; ii++)
      totals[0] -= counts[ii];
    for(int ii=1; ii<maxe; ii++)
      totals[ii] = totals[ii-1] + counts[ii] + counts[ii-1];
    
    for(int b=0; b<maxe; b++) {
      tau += totals[b] * 1. * cnt[i][b];
      counts[b] += cnt[i][b];
      }
    }
  ld par = (K * (K-1.) / 2);
  return tau / par;
  }

vector<pair<int, int>> recreate_topology(const vector<int>& mapp, const vector<pair<int, int> >& edges) {

  auto cmapp = mapp;
  for(int i=0; i<isize(cmapp); i++) if(cmapp[i] >= 0) cmapp[i] = i;
  
  while(true) {
    vector<pair<int, int> > changes;
    for(auto e: edges) {
      if(cmapp[e.first] == -1 && cmapp[e.second] >= 0) changes.emplace_back(e.first, cmapp[e.second]);
      if(cmapp[e.second] == -1 && cmapp[e.first] >= 0) changes.emplace_back(e.second, cmapp[e.first]);
      }
    if(changes.empty()) break;
    for(auto ch: changes) cmapp[ch.first] = ch.second;
    }

  set<pair<int, int>> subedges;
  for(auto e: edges) {
      auto a = cmapp[e.first], b = cmapp[e.second];
      if(a==b) continue;
      if(a<b) swap(a, b);
      subedges.emplace(a, b);
      }

  vector<pair<int, int>> result;
  for(auto sube: subedges)
    result.emplace_back(sube.first, sube.second);

  return result;
  }

vector<vector<int>> build_distance_matrix(int N, const vector<pair<int,int>>& vedges) {
  vector<vector<int>> res;
  res.resize(N);
  for(auto& r: res) r.resize(N, 1000);
  vector<vector<int>> edges_of(N);
  for(auto [a, b]: vedges) edges_of[a].push_back(b), edges_of[b].push_back(a);  
   
  for(int i=0; i<N; i++) {
    auto& d = res[i];
    queue<int> bfs;
    auto visit = [&] (int id, int dist) {
      if(d[id] <= dist) return;
      d[id] = dist;
      bfs.push(id);
      };
    visit(i, 0);
    while(!bfs.empty()) {
      auto j = bfs.front();
      for(auto e: edges_of[j]) visit(e, d[j]+1);
      bfs.pop();
      }
    }
  
  return res;
  }

vector<string> catnames = {"energy", "rips", "kendall", "empty", "emptyx", "subedges", "emptyxx", "villman1", "villman2", "vedges", "maxvill1", "maxvill2"};

ld evaluate_measure(manidata& emb, manidata& orig, vector<int>& mapp, vector<pair<int, int> >& vor_edges, vector<pair<int, int>>& edo_recreated, int k) {

  int No = orig.size;
  int Ne = emb.size;
  auto& edo = orig.edges;
  auto& ede = emb.edges;
  auto& dise = emb.distances;
  auto& diso = orig.distances;
  ld energy = 0;

  if(k == 2) {
    vector<pair<int, int> > allp;
    allp.reserve(No * No);
    for(int i=0; i<No; i++) if(mapp[i] != -1)
    for(int j=0; j<i; j++) if(mapp[j] != -1)
      allp.emplace_back(diso[i][j], dise[mapp[i]][mapp[j]]);
    energy = kendall(allp);
    }
  else if(k == 3) {
    vector<bool> empty(Ne, true);
    for(int i=0; i<No; i++) 
      if(mapp[i] != -1) empty[mapp[i]] = false;
    for(auto b: empty) if(b) energy++;
    }
  else if(k == 4 || k == 6) {
    vector<vector<int>> adj(Ne);
    vector<vector<int>> on(Ne);
    for(int i=0; i<No; i++) if(mapp[i] != -1)
      on[mapp[i]].push_back(i);
    for(auto [a,b]: ede) adj[a].push_back(b), adj[b].push_back(a);
    for(auto p: edo_recreated)
      diso[p.first][p.second] -= 100,
      diso[p.second][p.first] -= 100;
    for(int i=0; i<Ne; i++) {
      bool empty = on[i].empty();
      if(empty) {
        for(auto i1: adj[i])
        for(auto i2: adj[i])
        for(auto oi1: on[i1])
        for(auto oi2: on[i2])
        if(dise[i1][i2] > 1 && diso[oi1][oi2] <= 1)
          empty = false;
        }
      if(empty && k == 6) {
        for(auto i1: adj[i])
        for(auto i2: adj[i])
        for(auto i11: adj[i1]) {
          for(auto i21: adj[i2])
          for(auto oi1: on[i11])
          for(auto oi2: on[i21])
          if(dise[i11][i21] == dise[i11][i] + dise[i21][i] && diso[oi1][oi2] <= 1)
            empty = false;

          for(auto oi1: on[i11])
          for(auto oi2: on[i2])
          if(dise[i11][i2] == dise[i11][i] + dise[i2][i] && diso[oi1][oi2] <= 1)
            empty = false;
          }
        }
      if(empty) energy++;
      }
    for(auto p: edo_recreated)
      diso[p.first][p.second] += 100,
      diso[p.second][p.first] += 100;
    }
  else if(k == 5) energy = isize(edo_recreated);
  else if(k == 1) {
    for(auto [a,b]: edo_recreated) {
      if(mapp[a] == -1 || mapp[b] == -1) continue;
      int d = dise[mapp[a]][mapp[b]];
      if(k == 9)
        energy += d * d;
      else if(k == 1)
        energy += (d < 2 ? 0 : d == 2 ? 1 : 100);
      }                
    }
  else if(k == 0) {
    for(auto [a,b]: edo) {
      if(mapp[a] == -1 || mapp[b] == -1) continue;
      int d = dise[mapp[a]][mapp[b]];
      energy += d * d;
      }                
    }
  else if(k == 7) {
    for(auto [a,b]: vor_edges) {
      int d = dise[a][b] - 1;
      energy += d*d;
      }
    }
  else if(k == 8) {
    auto disv = build_distance_matrix(Ne, vor_edges);
    int bugs = 0;
    for(auto [a,b]: ede) {
      int d = disv[a][b] - 1;
      if(d >= 900) bugs++;
      energy += d*d;
      }
    if(bugs) println(hlog, "bugs=", bugs);
    }
  else if(k == 10) {
    for(auto [a,b]: vor_edges)
      energy = max<ld>(energy, dise[a][b] - 1);
    }
  else if(k == 11) {
    auto disv = build_distance_matrix(Ne, vor_edges);
    for(auto [a,b]: ede) 
      energy = max<ld>(energy, disv[a][b] - 1);
    }
  else if(k == 9)
    energy = isize(vor_edges);  
  return energy;
  }

/*
void test_kendall() {
  for(string orig: origs.names) {
    int No = origs.mdata[orig].size;
    vector<pair<int, int> > allp;
    auto& diso = origs.mdata[orig].distances;
    
    for(int i=0; i<No; i++)
    for(int j=0; j<i; j++)
      allp.emplace_back(diso[i][j], diso[i][j]);
      
    allp.emplace_back(0, 0);
    allp.emplace_back(1, 1);
    allp.emplace_back(2, 2);
    allp.emplace_back(3, 3);

    println(hlog, orig, " : ", measures::kendall(allp));
    }  
  }
*/

}
}
