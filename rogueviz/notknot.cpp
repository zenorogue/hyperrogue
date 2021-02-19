#include "../hyper.h"

/** 

* mg 2 in old video

Blocky Knot Portal: 

compile with: mymake rogueviz/notknot

the video has been created with the following options:

https://youtu.be/1TMY2U4_9Qg
nk_margin=2 -noplayer -canvas-random 20 -geo notknot -sight3 0.5 -ray-cells 600000 smooth_scrolling=1 camspd=10 panini_alpha=1 fov=150 -shot-hd ray_exp_decay_poly=30 ray_fixed_map=1 

https://youtu.be/eb2DhCcGH7U
nk_margin=4 -noplayer -canvas-random 20 -geo notknot -sight3 0.5 -ray-cells 600000 smooth_scrolling=1 camspd=10 panini_alpha=1 fov=150 -shot-hd ray_exp_decay_poly=30 ray_fixed_map=1 -ray-iter 100 ray_reflect_val=0.30

The algorithm here is as follows:

* create the map without portals (this is just the cube with the trifoil knot in it)

  This is done with a program (function create_trifoil_knot).
  The general algorithm should work with any map, and it would be possible to make it so that the user can create the map using HyperRogue map editor.

* the universal cover can be seen as the set of all paths, where two paths are identified if they are homotopic. So we generate all paths
  from the chosen starting point; when we generate extensions from path ...A, we check if we have a situation where we could go ...A->B->D or ...A->C->D --
  if so, the two paths are homotopic, so we identify them (that is, all the references to one of the paths are replaced with the references to the other path,
  possibly propagating to all known extensions of these paths)

* since the universal cover of the trifoil knot complement is infinite, we also identify two paths if they differ by three loops around the knot (around any point of the knot).
  (If we did not do that, the algorithm would never terminate, or we could terminate it at some point (terminate_at), but then we easily run into ugly unfinished areas.)

* this result in a relatively standard HyperRogue map representation (cubes connected via glued faces) which is rendered using HyperRogue's raycaster.

*/

namespace hr {

namespace notknot {

/* how many times we need to loop around the portal frame to get back to the same space */
/* the number of the worlds is: 1 (loop=1), 6 (loop=2), 24, 96, 600, infinity (loop>5) */
int loop = 3;

/* extra space around the knot */
int margin = 4;

/* the scale factor for the knot */
int knotsize = 3;

int terminate_at = 500000000;

eGeometry gNotKnot(eGeometry(-1));

/** It was easier to generate a program to design the trifoil knot than to generate it manually.
 *  This function does generate the instructions for trifoil knot, although they have been adjusted manually
 *  The generated instructions is the weird array in create_trifoil_knot()
 */

void gen_trifoil() {
  for(int len=2; len<=12; len+=2) {
    println(hlog, "trying len = ", len);
    int pos = 1;
    for(int l=0; l<len; l++) pos *= 6;
    
    for(int p=0; p<pos; p++) {

      vector<int> lst;
      int a = p;
      int bal = 0;
      for(int p=0; p<len; p++) {
        if(a%6 < 3) bal++; else bal--;
        lst.push_back(a%6);
        a /= 6;
        }
      if(bal) continue;
      
      array<int, 3> start = {0, 0, 0};
  
      array<int, 3> where = start;
  
      set<array<int, 3> > ful;
      map<array<int, 2>, int> proj;
      
      int steps = 0;
      vector<pair<int, int> > crosses;
      
      for(int i=0; i<3; i++) {
        for(auto d: lst) {
          if(ful.count(where)) goto next;
          ful.insert(where);
          auto pco = array<int,2>{{where[0]-where[2], where[1]-where[2]}};
          if(proj.count(pco)) crosses.emplace_back(proj[pco], steps);
          else proj[pco] = steps;
          where[(d+i)%3] += (d<3?1:-1);
          steps++;
          }
        }
      
      if(where != start) { println(hlog, "bad loop"); continue; }
      if(isize(ful) != 3*len) continue;

      if(isize(proj) != 3*len-3) continue;
      
      println(hlog, "len=", len, " ful=", isize(ful), " proj=", isize(proj), " for ", lst);
      
      println(hlog, "crosses = ", crosses);

      if(1) {
        set<int> crosval;
        for(auto c: crosses) crosval.insert(c.first), crosval.insert(c.second);
        vector<int> cvs;
        for(auto s: crosval) cvs.push_back(s);

        bool wrong = false;
        for(auto c: crosses) if(c.first == cvs[0] && c.second == cvs[1]) wrong = true;
        for(auto c: crosses) if(c.first == cvs[1] && c.second == cvs[2]) wrong = true;
      
        if(wrong) continue;
        }
      
      println(hlog, "result: ", lst);
      
      exit(3);

      next: ;
      }
    }
  
  exit(2);
  }

struct hrmap_notknot : hrmap {
  
  /* represents a path (may be partially identified) */
  struct ucover {
    /* the heptagon of the underlying map */
    heptagon *where;
    /* the heptagon of the result map */
    heptagon *result;
    /* what is has been merged into */
    ucover *merged_into;
    /* connections in every direction */
    ucover *ptr[6];
    /* used for painting walls in a single color */
    ucover *wall_merge;
    color_t wallcolor, wallcolor2;
    /* 0 = live, 1 = wall, 2 = merged, 4 = overflow */
    int state;
    /* index in the table `all` */
    int index;
    ucover(heptagon *w, int s) { where = w; for(int i=0; i<6; i++) ptr[i] = nullptr; state = s; merged_into = nullptr; result = nullptr; wall_merge = this; }
    };
  
  /* find-union algorithm for wall_merge */
  ucover *ufind(ucover *at) {
    if(at->wall_merge == at) return at;
    return at->wall_merge = ufind(at->wall_merge);
    }

  void funion(ucover *a, ucover *b) {
    ufind(b)->wall_merge = ufind(a);
    }
  
  /* the vector of all paths */
  vector<ucover*> all;
  
  /* the stack of known unifications */
  vector<pair<ucover*, ucover*>> unify;
  
  /* the underlying map */
  hrmap *euc;

  heptagon *getOrigin() override {
    // return hepts[0];
    return all[0]->result;
    }  
  
  heptagon* at(int x, int y, int z) { 
    dynamicval<eGeometry> g(geometry, gCubeTiling);
    dynamicval<hrmap*> m(currentmap, euc);
    euc::coord co = euc::basic_canonicalize({x, y, z});
    
    return euc::get_at(co);
    }
  
  /* make sure that where->move(d) and where->c.spin(d) are known */
  void cmov(heptagon *where, int d) {
    if(where->move(d)) return;
    dynamicval<eGeometry> g(geometry, gCubeTiling);
    dynamicval<hrmap*> m(currentmap, euc);
    createStep(where, d);
    }
  
  heptagon *create_trifoil_knot() {
    euc::coord cmin{99,99,99}, cmax{-99,-99,-99}, cat{0,0,0};
    heptagon *h = at(0, 0, 0);
    
    vector<heptagon*> trifoil;
    
    int step = knotsize;

    for(int i=0; i<3; i++) {
      for(auto m: {3, 3, 3, 3, 5, 5, 1, 0, 0, 0, 0, 0}) for(int rep=0; rep<step; rep++) {
        trifoil.push_back(h);
        int d = (i+m)%3 + (m<3?0:3);
        cmov(h, d);
        h = h->move(d);
        if(m<3) cat[(i+m)%3]++; else cat[(i+m)%3]--;
        for(int k=0; k<3; k++) cmin[k] = min(cmin[k], cat[k]), cmax[k] = max(cmax[k], cat[k]);
        }
      }
    
    int& mg = margin;

    for(int i=cmin[0]-mg; i<=cmax[0]+mg; i++)
    for(int j=cmin[1]-mg; j<=cmax[1]+mg; j++)
    for(int k=cmin[2]-mg; k<=cmax[2]+mg; k++)
      if(among(i,cmin[0]-mg,cmax[0]+mg) || among(j,cmin[1]-mg,cmax[1]+mg) || among(k,cmin[2]-mg,cmax[2]+mg))
        at(i,j,k)->zebraval = 1;
      else
        at(i,j,k)->zebraval = 0;
      
    for(auto h: trifoil)
        h->zebraval = 1;
    
    return at(cmax[0], cmax[1], cmax[2]);
    }

  hrmap_notknot() {

    if(1) {
      dynamicval<eGeometry> dg(geometry, gCubeTiling);
      initcells(); euc = currentmap;
      for(hrmap*& m: allmaps) if(m == euc) m = NULL;
      }
    
    int i = 0;

    all.emplace_back(new ucover(create_trifoil_knot(), 0));    
    
    auto gen_adj = [&] (ucover *u, int d) {
      if(u->ptr[d]) return u->ptr[d];
      cmov(u->where, d);
      auto x = u->where->move(d);
      auto d1 = u->where->c.spin(d);
      if(x->zebraval > 1) {
        println(hlog, "zebraval failure!");
        exit(3);
        x->zebraval = 0;
        }
      u->ptr[d] = new ucover(x, x->zebraval);
      u->ptr[d]->ptr[d1] = u;
      u->ptr[d]->index = isize(all);
      all.push_back(u->ptr[d]);
      return u->ptr[d];
      };
    
    while(true) {
    
      /* handle all known unifications */
      if(!unify.empty()) {
        ucover *uf, *ut;
        tie(uf, ut) = unify.back();
        unify.pop_back();
        while(uf->merged_into) uf = uf->merged_into;
        while(ut->merged_into) ut = ut->merged_into;
        if(uf == ut) continue;
        if(!uf || !ut) println(hlog, "null unified");
        /* we always keep the one with the lower index */
        if(uf->index < ut->index) swap(uf, ut);        
        uf->state |= 2; uf->merged_into = ut;
        if(uf->where != ut->where)
          println(hlog, "where confusion");
        for(int d=0; d<6; d++) {
          cmov(uf->where, d);
          auto d1 = uf->where->c.spin(d);
          if(uf->ptr[d]) {
            if(!ut->ptr[d]) {
              /* was a known connection in uf, but not in ut -- reconnect it to ut */
              uf->ptr[d]->ptr[d1] = ut;
              ut->ptr[d] = uf->ptr[d];
              uf->ptr[d] = nullptr;
              }
            else {
              /* in some direction, connections for both uf and ut are already known, so unify them too */
              unify.emplace_back(uf->ptr[d], ut->ptr[d]);
              uf->ptr[d]->ptr[d1] = nullptr;
              uf->ptr[d] = nullptr;
              }
            }
          }
        continue;
        }
    
      /* handle creation and loops */

      if(i >= isize(all)) break;
      auto u = all[i++];
      if(u->state != 0) continue;
      if(i > terminate_at) { u->state |= 4; continue; }

      for(int k=0; k<6; k++) gen_adj(u, k);

      /* unify homotopies */
      for(int k=0; k<6; k++)
      for(int l=0; l<6; l++) {
        auto uk = gen_adj(u, k);
        if(uk->state != 0) continue;
        auto ul = gen_adj(u, l);
        if(ul->state != 0) continue;
        auto ukl = gen_adj(uk, l);
        auto ulk = gen_adj(ul, k);
        if(ukl->state != 0) continue;
        if(ulk->state != 0) continue;
        if(ukl == ulk) continue; /* okay */
        if(!ukl || !ulk) println(hlog, "null returned");
        unify.emplace_back(ukl, ulk);
        }
      
      /* try to make it finite */
      auto ux = u;
      for(int iter=0; iter<loop; iter++)
        for(int w: {0, 0, 1, 1, 3, 3, 4, 4}) {
          ux = gen_adj(ux, w);
          if(ux->state != 0) goto nxt;
          }
      unify.emplace_back(u, ux);
      
      nxt: ;
      }
    
    /* make the walls single-colored */

    for(int i=0; i<isize(all); i++) {
      auto u = all[i];
      if(u->state != 0) continue;

      /* convex corners */
      for(int k=0; k<6; k++)
      for(int l=0; l<6; l++) {
        auto uk = gen_adj(u, k);
        if(uk->state != 0) continue;
        auto ul = gen_adj(u, l);
        if(ul->state != 0) continue;
        auto ukl = gen_adj(uk, l);
        auto ulk = gen_adj(ul, k);
        if(ukl->state != 0)
          funion(ukl, ulk);
        }

      /* flat areas */
      for(int k=0; k<6; k++)
      for(int l=0; l<6; l++) {
        auto uk = gen_adj(u, k);
        if(uk->state != 0) continue;
        auto ul = gen_adj(u, l);
        if(ul->state != 1) continue;
        auto ukl = gen_adj(uk, l);
        if(ukl->state != 1) continue;
        funion(ul, ukl);        
        }

      /* concave corners */
      for(int k=0; k<6; k++)
      for(int l=0; l<6; l++) {
        auto uk = gen_adj(u, k);
        if(uk->state != 1) continue;
        auto ul = gen_adj(u, l);
        if(ul->state != 1) continue;
        if(abs(k-l) != 3)
          funion(ul, uk);        
        }      
      }
    
    /* statistics */
    int lives = 0, walls = 0, overflow = 0, merged = 0;
    
    for(auto v: all) {
      if(v->state == 0) lives++;
      if(v->state == 1) walls++;
      if(v->state == 2) merged++;
      if(v->state == 3) overflow++;
      }
      
    set<heptagon*> wheres;
    
    println(hlog, "lives = ", lives);
    println(hlog, "walls = ", walls);
    println(hlog, "merged = ", merged);
    println(hlog, "overflow = ", overflow);
    println(hlog, "total = ", isize(all));

    /* create the result map */
    for(int i=0; i<isize(all); i++) {
      auto u = all[i];
      if(u->state & 2) continue;
      if(u->state == 0) wheres.insert(all[i]->where);
      u->result = tailored_alloc<heptagon> (S7);
      u->result->c7 = newCell(S7, u->result);
      }

    println(hlog, "wheres = ", isize(wheres), " : ", lives * 1. / isize(wheres));

    for(int i=0; i<isize(all); i++) {
      auto u = all[i];
      if(u->state & 2) continue;

      for(int d=0; d<S7; d++) {        
        cmov(u->where, d);
        auto d1 = u->where->c.spin(d);
          if(abs(d-d1) != 3) println(hlog, "incorrect d1");
        if(u->ptr[d] && u->ptr[d]->result == nullptr) {
          println(hlog, "connection to null in state ", u->ptr[d]->state, " from state ", u->state, " i=", i, " .. ", u->ptr[d]->index);
          exit(1);
          }
        if(u->ptr[d] && u->ptr[d]->ptr[d1] != u)
          println(hlog, "wrong connection");
        if(u->ptr[d])
          u->result->c.connect(d, u->ptr[d]->result, d1, false);          
        else
          u->result->c.connect(d, u->result, d, false);
        }
      }
    
    for(int k=0; k<23; k++) hrand(5);

    int colors_used = 0;

    for(int i=0; i<isize(all); i++) 
      all[i]->wallcolor = 0;

    for(int i=0; i<isize(all); i++) 
      if(all[i]->state == 1)
        ufind(all[i])->wallcolor++;
        
    map<int, int> sizes;

    for(int i=0; i<isize(all); i++) 
      if((all[i]->state & 1) && ufind(all[i]) == all[i] && all[i]->wallcolor)
        colors_used++,
        sizes[all[i]->wallcolor]++;
    
    for(auto p: sizes) 
      println(hlog, "size = ", p.first, " times ", p.second);

    println(hlog, "colors_used = ", colors_used);

    for(int i=0; i<isize(all); i++) 
      if((all[i]->state & 1) && ufind(all[i]) == all[i]) {
        all[i]->wallcolor = hrand(0x1000000) | 0x404040,
        all[i]->wallcolor2 = hrand(0x1000000) | 0x404040;
        }
      
    for(int i=0; i<isize(all); i++) {
      auto u = all[i];
      if(!u->result) continue;
      cell *c = u->result->c7;
      setdist(c, 7, c);
      c->land = laCanvas;
      if(u->state & 1) {
        c->wall = waWaxWall;
        c->landparam = hrand(100) < 10 ? ufind(u)->wallcolor2 : ufind(u)->wallcolor;
        if(!(ufind(u)->state & 1)) println(hlog, "connected to state ", ufind(u)->state);
        // if(!(c->landparam & 0x404040)) println(hlog, "color found ", c->landparam);
        }
      else if(u->state & 4)
        c->wall = waBigTree;
      else 
        c->wall = waNone;
      }
    }
  
  transmatrix relative_matrix(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
    return Id;
    }

  transmatrix adj(heptagon *h, int i) override {
    return euc->adj(h, i);
    }
  
  };

auto h = addHook(hooks_newmap, 0, [] {
  // gen_trifoil();
  if(geometry == gNotKnot) {
    return (hrmap*) new hrmap_notknot;
    }
  return (hrmap*) nullptr;
  });

void create_notknot() {
  if(gNotKnot != eGeometry(-1)) return;
  ginf.push_back(ginf[gCubeTiling]);
  gNotKnot = eGeometry(isize(ginf) - 1);
  // variation = eVariation::pure;
  auto& gi = ginf.back();
  gi.flags = qANYQ | qBOUNDED | qEXPERIMENTAL | qPORTALSPACE;
  gi.quotient_name = "notknot";
  gi.shortname = "notknot";
  gi.menu_displayed_name = "notknot";
  }
  
auto shot_hooks = addHook(hooks_initialize, 100, create_notknot)
  + addHook(hooks_configfile, 100, [] {
    param_i(loop, "nk_loop");
    param_i(margin, "nk_margin");
    param_i(knotsize, "nk_knotsize");
    param_i(terminate_at, "nk_terminate");
    });

}

}
