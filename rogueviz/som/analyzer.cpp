// analyze the SOM test results
// Copyright (C) 2011-2022 Tehora and Zeno Rogue, see 'hyper.cpp' for details

#include "kohonen.h"

namespace rogueviz { 

namespace kohonen_test {
extern string cg();
extern int data_scale, embed_scale;
extern int subdata_value;
}

double UNKNOWN_RESULT = -3;

using namespace kohonen_test;

namespace analyzer {

using measures::manidata;
using measures::MCOUNT;

struct maniset {
  vector<string> names;
  map<string, manidata> mdata;
  };

maniset origs, embs;

void load_maniset(maniset& m, int scale) {
  FILE *f = fopen((som_test_dir + "edgelists-" + its(scale) + ".txt").c_str(), "r");
  while(true) {
    char buf[200];
    if(fscanf(f, "%s", buf) <= 0) break;
    if(buf[0] != '#')
      m.names.push_back(buf);
    auto& md = m.mdata[buf];
    int N, M;
    hr::ignore(fscanf(f, "%d%d", &N, &M));
    println(hlog, "reading ", buf, " of size ", N, " and ", M, " edges");
    md.size = N;
    auto& ed = md.edges;
    for(int i=0; i<M; i++) {
      int a, b;
      hr::ignore(fscanf(f, "%d%d", &a, &b));
      ed.emplace_back(a, b);
      }
    md.distances = measures::build_distance_matrix(N, md.edges);
    }
  fclose(f);
  }

vector<pair<int, int> > vor_edges;

void analyze_test() {

  println(hlog, "loading original manifolds");
  load_maniset(origs, data_scale);
  println(hlog, "loading embedding manifolds");
  load_maniset(embs, embed_scale);

  map<string, map<int, array<double, MCOUNT>>> results;

  string tablefile = som_test_dir + "table" + cg() + ".csv";
  
  if(1) {
    
    fhstream res(tablefile, "rt");
    if(!res.f) { println(hlog, "table ", tablefile, " missing"); }
    else {
      println(hlog, "reading the old table ", tablefile);
      string s = scanline(res);
      while(true) {
        s = scanline(res);
        if(s == "") break;
        int i = 0;
        while(s[i] != ';') i++;
        i++;
        while(s[i] != ';') i++;
        i++;
        int id;
        sscanf(s.c_str()+i, "%d", &id);
        auto& res = results[s.substr(0, i-1)][id];
        for(auto& d: res) d = UNKNOWN_RESULT;
        sscanf(s.c_str()+i, "%d;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf;%lf", &id, &res[0], &res[1], &res[2], &res[3], &res[4], &res[5], &res[6], &res[7], &res[8], &res[9], &res[10], &res[11]);
        }
      }
    }
  
  for(string orig: origs.names) {
    
    string dir = som_test_dir + "pairs" + cg();
  
    for(string emb: embs.names) {
      string fname = dir + "/" + orig + "-" + emb + ".txt";
      if(!file_exists(fname)) continue;
      FILE *f = fopen(fname.c_str(), "rt");

      string vorname = dir + "/" + orig + "-" + emb + ".vor";
      if(!file_exists(vorname)) continue;
      FILE *vor = fopen(vorname.c_str(), "rt");

      int No = origs.mdata[orig].size;
      auto& edo = origs.mdata[orig].edges;
      vector<pair<int, int>> edo_recreated;
      
      int new_results = 0;

      int current_row = -1;

      auto& res1 = results[orig + ";" + emb];
      for(int it=0; it<100; it++) {

        bool know_result = res1.count(it);
        auto& res = res1[it];
        if(!know_result)
          for(int kk=0; kk<MCOUNT; kk++)
            res[kk] = UNKNOWN_RESULT;

        vector<int> mapp(No, -2);
        for(int k=0; k<MCOUNT; k++) {        
          bool recompute = false;
          ld energy = 0;
          if(res[k] == UNKNOWN_RESULT || recompute) {

            while(current_row < it) {
              current_row++;
              for(int i=0; i<No; i++) hr::ignore(fscanf(f, "%d", &mapp[i]));
              int V = 0; hr::ignore(fscanf(vor, "%d", &V));
              vor_edges.resize(V);
              for(int i=0; i<V; i++) hr::ignore(fscanf(vor, "%d%d", &vor_edges[i].first, &vor_edges[i].second));
              if(mapp.back() == -2) goto next_pair;
              if(current_row == it)
                edo_recreated = measures::recreate_topology(mapp, edo);
              }

            new_results++;
            
            energy = evaluate_measure(embs.mdata[emb], origs.mdata[orig], mapp, vor_edges, edo_recreated, k);
            
            if(recompute && res[k] != UNKNOWN_RESULT) {
              if(abs(res[k] - energy) > 1e-5) {
                println(hlog, "ERROR in ", orig, "->", emb, " in ", cg(), " index ", it, " : was ", res[k], " is ", energy);
                if(subdata_value) res[k] = energy;
                }
              }
            else {
              res[k] = energy;
              }
            }
          }
        }
      next_pair:
      fclose(f);
      
      if(new_results) println(hlog, "computed ", new_results, " new results in ", cg(), " for ", orig, " -> ", emb);
      }
    }

  fhstream o(som_test_dir + "summary" + cg() + ".html", "wt");
  println(o, "<html><body>\n");
  
  if(1) {
    println(o, "<table>\n");
    
    int manis = 0;
    
    for(string orig: origs.names) {

    print(o, "<tr><th>", orig, "</th>");
    for(string emb: embs.names) print(o, "<th>", emb, "</th>");
    print(o, "</tr>\n");
    
    for(int k: {0, 2, 4, 7, 8, 10, 11}) {
      print(o, "<tr><td>", measures::catnames[k], "</td>");
      
      for(string emb: embs.names) {
        ld tenergy = 0;
        int att = 0;
        auto& res1 = results[orig + ";" + emb];

        for(int it=0; it<100; it++) {
          if(!res1.count(it)) continue;
          auto& res = res1[it];
          if(res[k] == UNKNOWN_RESULT) continue;
          att++; tenergy += res[k];
          }
    
        if(orig == emb)
          print(o, "<td bgcolor=\"#C0FFFF\">");
        else if(manis & 1)
          print(o, "<td bgcolor=\"#FFFFC0\">");
        else
          print(o, "<td>");
        if(att == 100)
          print(o, tenergy * 1. / att);
        else if(att)
          print(o, tenergy * 1. / att, "?");
        else
          print(o, "???");
        print(o, "</td>");
        }
      print(o, "</tr>\n");
      }
      manis++;
      }
    print(o, "</table>\n");
    }
  print(o, "</body></html>\n");
  
  fhstream res(som_test_dir + "table" + cg() + ".csv", "wt");
  print(res, "orig;emb;index");
  for(int i=0; i<MCOUNT; i++) print(res, ";", measures::catnames[i]); println(res);
  
  for(auto& p: results) {
    for(auto& p2: p.second) {
      auto &p3 = p2.second;
      print(res, p.first, ";", p2.first);
      for(int i=0; i<MCOUNT; i++) print(res, ";", p3[i]); println(res);
      }
    }  
  }

auto khook = arg::add3("-analyze-test", analyze_test);

}}


 
