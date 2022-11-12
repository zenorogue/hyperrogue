// Hyperbolic Rogue -- Goldberg-Coxeter construction
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file goldberg.cpp
 *  \brief Goldberg-Coxeter construction
 *
 *  This is generally not used for standard pure and bitruncated tilings, even though they are technically Goldberg too.
 */

#include "hyper.h"
namespace hr { 

#if HDR
struct hrmap;
extern hrmap *currentmap;
#endif

EX namespace gp {

  #if HDR
  struct loc : pair<int, int> {
    loc() {}

    loc(int x, int y) : pair<int,int> (x,y) {}

    loc operator+(loc e2) {
      return loc(first+e2.first, second+e2.second);
      }
    
    loc operator-(loc e2) {
      return loc(first-e2.first, second-e2.second);
      }
    
    loc operator*(loc e2) {
      return loc(first*e2.first-second*e2.second, 
        first*e2.second + e2.first*second + (S3 == 3 ? second*e2.second : 0));
      }
  
    loc operator*(int i) {
      return loc(first*i, second*i);
      }  

    int operator %(int i) {
      return gmod(first, i) + gmod(second, i);
      }

    loc operator /(int i) {
      return loc(first/i, second/i);
      }
      
    loc conj() {
      if(S3 == 4) return loc(first, -second);
      return loc(first+second, -second);
      }

    };

  struct local_info {
    int last_dir;
    loc relative;
    int first_dir;
    int total_dir;
    };
  #endif

  EX local_info current_li;
  EX cell *li_for;
  
  EX loc eudir(int d) {
    if(S3 == 3) {
      d %= 6; if (d < 0) d += 6;
      switch(d) {
        case 0: return loc(1, 0);
        case 1: return loc(0, 1);
        case 2: return loc(-1, 1);
        case 3: return loc(-1, 0);
        case 4: return loc(0, -1);
        case 5: return loc(1, -1);
        default: return loc(0, 0);
        }
      }
    else switch(d&3) {
      case 0: return loc(1, 0);
      case 1: return loc(0, 1);
      case 2: return loc(-1, 0);
      case 3: return loc(0, -1);
      default: return loc(0, 0);
      }
    }
  
  EX int length(loc p) {
    return euc::dist(p.first, p.second);
    }
  
#if CAP_GP
  EX loc param = loc(1, 0);

  EX hyperpoint next;
  
  struct goldberg_mapping_t {
    cellwalker cw;
    signed char rdir;
    signed char mindir;
    loc start;
    transmatrix adjm;
    };

  EX int fixg6(int x) { return gmod(x, SG6); }
  
  const int GOLDBERG_LIMIT_HALF = GOLDBERG_LIMIT/2;
  const int GOLDBERG_MASK_HALF = GOLDBERG_MASK/2;
  
  EX int get_code(const local_info& li) {
    return 
      ((li.relative.first & GOLDBERG_MASK_HALF) << 0) +
      ((li.relative.second & GOLDBERG_MASK_HALF) << (GOLDBERG_BITS-1)) +
      ((fixg6(li.total_dir)) << (2*GOLDBERG_BITS-2)) +
      ((li.last_dir & 15) << (2*GOLDBERG_BITS+2));
    }
  
  EX local_info get_local_info(cell *c) {
    if(INVERSE) {
      c = get_mapped(c);
      return UIU(get_local_info(c));
      }
    local_info li;
    if(c == c->master->c7) {
      li.relative = loc(0,0);
      li.first_dir = -1;
      li.last_dir = -1;
      li.total_dir = -1;
      }
    else {
      vector<int> dirs;
      while(c != c->master->c7) {
        dirs.push_back(c->c.spin(0));
        c = c->move(0);
        }
      li.first_dir = dirs[0];
      li.last_dir = dirs.back();

      loc at(0,0);
      int dir = 0;
      at = at + eudir(dir);
      dirs.pop_back();
      while(dirs.size()) {
        dir += dirs.back() + SG3;
        dirs.pop_back();
        at = at + eudir(dir);
        }
      li.relative = at;
      li.total_dir = dir + SG3;
      }
    return li;
    }
    
  EX int last_dir(cell *c) {
    return get_local_info(c).last_dir;
    }

  EX loc get_coord(cell *c) {
    return get_local_info(c).relative;
    }
  
  EX int pseudohept_val(cell *c) {
    loc v = get_coord(c);
    return gmod(v.first - v.second, 3);
    }
  
  // mapping of the local equilateral triangle
  // goldberg_map[y][x].cw is the cellwalker in this triangle at position (x,y)
  // facing local direction 0  
  
  goldberg_mapping_t goldberg_map[GOLDBERG_LIMIT][GOLDBERG_LIMIT];
  void clear_mapping() {
    for(int y=0; y<GOLDBERG_LIMIT; y++) for(int x=0; x<GOLDBERG_LIMIT; x++) {
      goldberg_map[y][x].cw.at = NULL;
      goldberg_map[y][x].rdir = -1;
      goldberg_map[y][x].mindir = 0;
      }
    }
  
  goldberg_mapping_t& get_mapping(loc c) {
    return goldberg_map[c.second&GOLDBERG_MASK][c.first&GOLDBERG_MASK];
    }

  int spawn;

  cell*& peek(cellwalker cw) {
    return cw.at->move(cw.spin);
    }

  cellwalker get_localwalk(const goldberg_mapping_t& wc, int dir) {
    if(dir < wc.mindir) dir += SG6;
    if(dir >= wc.mindir + SG6) dir -= SG6;
    return wc.cw + dir;
    }

  void set_localwalk(goldberg_mapping_t& wc, int dir, const cellwalker& cw) {
    if(dir < wc.mindir) dir += SG6;
    if(dir >= wc.mindir + SG6) dir -= SG6;
    wc.cw = cw - dir;
    }

  bool pull(loc at, int dir) {
    auto& wc = get_mapping(at);
    auto at1 = at + eudir(dir);
    int dir1 = fixg6(dir+SG3);
    cellwalker wcw = get_localwalk(wc, dir);
    auto& wc1= get_mapping(at1);
    if(wc1.cw.at) {
      if(peek(wcw)) {
        auto wcw1 = get_localwalk(wc1, dir1);
        if(wcw + wstep != wcw1) {
          DEBB(DF_GP, (at1, " : ", (wcw+wstep), " / ", wcw1, " (pull error from ", at, " :: ", wcw, ")") );
          exit(1);
          }
        if(do_adjm) wc1.adjm = wc.adjm * get_adj(wcw.at, wcw.spin);
        }
      return false;
      }
    if(peek(wcw)) {
      set_localwalk(wc1, dir1, wcw + wstep);
      DEBB(DF_GP, (at1, " :", wcw+wstep, " (pulled from ", at, " :: ", wcw, ")"));
      if(do_adjm) wc1.adjm = wc.adjm * get_adj(wcw.at, wcw.spin);
      return true;
      }
    return false;
    }
  
  EX bool do_adjm;

  void conn1(loc at, int dir, int dir1) {
    auto& wc = get_mapping(at);
    auto wcw = get_localwalk(wc, dir);
    auto& wc1 = get_mapping(at + eudir(dir));
    DEBB0(DF_GP, (format("  md:%02d s:%d", wc.mindir, wc.cw.spin)); )
    DEBB0(DF_GP, ("  connection ", at, "/", dir, " ", wc.cw+dir, "=", wcw, " ~ ", at+eudir(dir), "/", dir1, " "); )
    if(!wc1.cw.at) {
      wc1.start = wc.start;
      if(peek(wcw)) {
        DEBB0(DF_GP, (" (pulled) "); )
        set_localwalk(wc1, dir1, wcw + wstep);
        if(do_adjm) wc1.adjm = wc.adjm * get_adj(wcw.at, wcw.spin);
        }
      else {
        peek(wcw) = newCell(SG6, wc.cw.at->master);
        wcw.at->c.setspin(wcw.spin, 0, false);
        set_localwalk(wc1, dir1, wcw + wstep);
        if(do_adjm) wc1.adjm = wc.adjm;
        spawn++;
        DEBB0(DF_GP, (" (created) "); )
        }
      }
    DEBB0(DF_GP, (wc1.cw+dir1, " "));
    auto wcw1 = get_localwalk(wc1, dir1);
    if(peek(wcw)) {
      if(wcw+wstep != wcw1) {
        DEBB(DF_GP, ("FAIL: ", wcw, " connected to ", wcw+wstep, " not to ", wcw1); exit(1); )
        }
      else {
        DEBB(DF_GP, ("(was there)"));
        }
      }
    else {
      DEBB(DF_GP, ("ok"));
      peek(wcw) = wcw1.at;
      wcw.at->c.setspin(wcw.spin, wcw1.spin, wcw.mirrored != wcw1.mirrored);
      if(wcw+wstep != wcw1) {
        DEBB(DF_GP | DF_ERROR, ("assertion failed"));
        exit(1);
        }
      }
    if(do_adjm) {
      get_adj(wcw.at, wcw.spin) = inverse(wc.adjm) * wc1.adjm;
      get_adj(wcw1.at, wcw1.spin) = inverse(wc1.adjm) * wc.adjm;
      }
    }

  void conn(loc at, int dir) {
    conn1(at, fixg6(dir), fixg6(dir+SG3));
    conn1(at + eudir(dir), fixg6(dir+SG3), fixg6(dir));
    }
  
  EX map<pair<cell*, int>, transmatrix> gp_adj;
  
  EX transmatrix& get_adj(cell *c, int i) { return gp_adj[make_pair(c,i)]; }

  goldberg_mapping_t& set_heptspin(loc at, heptspin hs) {
    auto& ac0 = get_mapping(at);
    ac0.cw = cellwalker(hs.at->c7, hs.spin, hs.mirrored);
    ac0.start = at;
    DEBB(DF_GP, (at, " : ", ac0.cw));
    return ac0;
    }

  EX void extend_map(cell *c, int d) {
    DEBB(DF_GP, ("EXTEND ",c, " ", d));
    indenter ind(2);
    if(c->master->c7 != c) {
      auto c1 = c;
      auto d1 = d;
      while(c->master->c7 != c) {
        DEBB(DF_GP, (c, " direction 0 corresponds to ", c->move(0), " direction ", c->c.spin(0)); )
        d = c->c.spin(0);
        c = c->move(0);
        }
      // c move 0 equals c' move spin(0)
      extend_map(c, d);
      extend_map(c, c->c.fix(d-1));
      extend_map(c, c->c.fix(d+1));
      if(S3 == 4 && !c1->move(d1)) {
        for(int i=0; i<S7; i++)
        for(int j=0; j<S7; j++)
          extend_map(createStep(c->master, i)->c7, j);
        }
      if(S3 == 4 && !c1->move(d1)) {
        for(int i=0; i<S7; i++)
        for(int i1=0; i1<S7; i1++)
        for(int j=0; j<S7; j++)
          extend_map(createStep(createStep(c->master, i), i1)->c7, j);
        }
      return;
      }

    if(S3 == 4 && param.first <= param.second) { d--; if(d<0) d += S7; }
    clear_mapping();

    // we generate a local map from an Euclidean grid to the
    // hyperbolic grid we build.
    
    // we fill the equilateral triangle with the following vertices:

    loc vc[4];
    vc[0] = loc(0,0);
    vc[1] = param;
    if(S3 == 3)
      vc[2] = param * loc(0,1);
    else 
      vc[2] = param * loc(1,1),
      vc[3] = param * loc(0,1);
    
    heptspin hs(c->master, d, false);
    
    auto& ac0 = set_heptspin(vc[0], hs);
    ac0.mindir = -1;
    auto& ac1 = set_heptspin(vc[1], hs + wstep - SG3);
    ac1.mindir = 0;
    auto& ac2 = set_heptspin(vc[S3-1], S3 == 3 ? hs + 1 + wstep - 4 : hs + 1 + wstep + 1);
    ac2.mindir = S3 == 3 ? 1 : -2;
    if(S3 == 4) {
      set_heptspin(vc[2], hs + wstep - 1 + wstep + 1).mindir = -3;
      }

    do_adjm = quotient || sphere;
    if(do_adjm) {
      auto m = (hrmap_standard*)currentmap;
      get_mapping(vc[0]).adjm = Id;
      get_mapping(vc[1]).adjm = m->adj(c->master, d);
      get_mapping(vc[S3-1]).adjm = m->adj(c->master, (d+1)%c->master->type);
      if(S3 == 4) {
        heptspin hs1 = hs + wstep - 1;
        get_mapping(vc[2]).adjm = m->adj(c->master, d) * m->adj(hs1.at, hs1.spin);
        }
      }
    
    auto fix_mirrors = [&] {
      if(ac1.cw.mirrored != hs.mirrored) ac1.cw--;
      if(ac2.cw.mirrored != hs.mirrored) ac2.cw--;
      if(S3 == 4) {
        auto& ac3 = get_mapping(vc[2]);
        if(ac3.cw.mirrored != hs.mirrored) ac3.cw--;
        }
      };

    if(S3 == 4 && param == loc(1,1)) {
      fix_mirrors();
      conn(loc(0,0), 1);
      conn(loc(0,1), 0);
      conn(loc(0,1), 1);
      conn(loc(0,1), 2);
      conn(loc(0,1), 3);
      return;
      }

    if(S3 == 4 && param.first == param.second && nonorientable) {
      fix_mirrors();

      int size = param.first;

      // go along the boundary of the 'diamond'

      for(int dir=0; dir<4; dir++) {
        int dir_orth = (dir+1) & 3;

        loc at = vc[dir];

        for(int i=0; i<size; i++) {
          if(!pull(at, dir)) break;
          at = at + eudir(dir);

          if(!pull(at, dir_orth)) break;
          at = at + eudir(dir_orth);
          }
        }

      // build the skeleton

      for(int dir=0; dir<4; dir++) {
        int dir_orth = (dir+1) & 3;
        for(int i=0; i<size; i++) {
          conn(vc[dir] + eudir(dir_orth) * i, dir_orth);
          }
        }

      // fill everything

      for(int y=0; y<2*size; y++) {
        int xdist = min(y, 2*size-y);
        for(int x=0; x<xdist; x++)
        for(int d=0; d<4; d++) {
          conn(loc(x, y), d);
          conn(loc(-x, y), d);
          }
        }

      return;
      }
    
    if(nonorientable && param.first == param.second) {
      int x = param.first;
      fix_mirrors();
      
      for(int d=0; d<3; d++) for(int k=0; k<3; k++) 
      for(int i=0; i<x; i++) {
        int dd = (2*d+k);
        loc cx = vc[d] + eudir(dd) * i;
        if(!pull(cx, dd)) break;
        }
      
      for(int i=0; i<=2*x; i++)
      for(int d=0; d<3; d++) {
        loc cx = vc[d] + eudir(1+2*d) * i;
        if(i < 2*x) conn(cx, 1+2*d);
        int jmax = x-i, drev = 2*d;
        if(jmax < 0) drev += 3, jmax = -jmax;
        for(int j=0; j<jmax; j++) {
          loc cy = cx + eudir(drev) * j;
          conn(cy, drev);
          conn(cy, drev+1);
          conn(cy, drev+2);
          }
        }
      
      return;
      }

    // then we set the edges of our big equilateral triangle (in a symmetric way)
    for(int i=0; i<S3; i++) {
      loc start = vc[i];
      loc end = vc[(i+1)%S3];
      DEBB(DF_GP, ("from ", start, " to ", end); )
      loc rel = param;
      auto build = [&] (loc& at, int dx, bool forward) {
        int dx1 = dx + SG2*i;
        DEBB(DF_GP, (at, " .. ", make_pair(at + eudir(dx1), fixg6(dx1+SG3))));
        conn(at, dx1);        
        if(forward) get_mapping(at).rdir = fixg6(dx1);
        else get_mapping(at+eudir(dx1)).rdir = fixg6(dx1+SG3);
        at = at + eudir(dx1);
        };
      while(rel.first >= 2 && (S3 == 3 ? rel.first >= 2 - rel.second : true)) {
        build(start, 0, true);
        build(end, SG3, false);
        rel.first -= 2;
        }
      while(rel.second >= 2) {
        build(start, 1, true);
        build(end, 1+SG3, false);
        rel.second -= 2;
        }
      while(rel.second <= -2 && S3 == 3) {
        build(start, 5, true);
        build(end, 2, false);
        rel.second += 2;
        rel.first -= 2;
        }
      if(S3 == 3) while((rel.first>0 && rel.second > 0) | (rel.first > 1 && rel.second < 0)) {
        build(start, 0, true);
        build(end, 3, false);
        rel.first -= 2;
        }
      if(S3 == 4 && rel == loc(1,1)) {
        if(param == loc(3,1) || param == loc(5,1)) {
          build(start, 1, true);
          build(end, 2, false);
          rel.first--;
          rel.second--;
          }
        else {
          build(start, 0, true);
          build(end, 3, false);
          rel.first--;
          rel.second--;
          }
        }
      for(int k=0; k<SG6; k++)
        if(start + eudir(k+SG2*i) == end)
          build(start, k, true);                         
      if(start != end) { DEBB(DF_GP | DF_ERROR, ("assertion failed: start ", start, " == end ", end)); exit(1); }
      }

    // now we can fill the interior of our big equilateral triangle
    loc at = vc[0];
    int maxstep = 3000;
    while(true) {
      maxstep--; if(maxstep < 0) { DEBB(DF_GP | DF_ERROR, ("maxstep exceeded")); exit(1); }
      auto& wc = get_mapping(at);
      int dx = wc.rdir;
      auto at1 = at + eudir(dx);
      auto& wc1 = get_mapping(at1);
      DEBB(DF_GP, (make_pair(at, dx), " ", make_pair(at1, wc1.rdir)));
      int df = wc1.rdir - dx;
      if(df < 0) df += SG6;
      if(df == SG3) break;
      if(S3 == 3) switch(df) {
        case 0:
        case 4:
        case 5:
          at = at1;
          continue;
        case 2: {
          conn(at, dx+1);
          wc.rdir = (dx+1) % 6;
          break;
          }
        case 1: {
          auto at2 = at + eudir(dx+1);
          auto& wc2 = get_mapping(at2);
          if(wc2.cw.at) { at = at1; continue; }
          wc.rdir = (dx+1) % 6;
          conn(at, (dx+1) % 6);
          conn(at1, (dx+2) % 6);
          conn(at2, (dx+0) % 6);
          wc1.rdir = -1;
          wc2.rdir = dx; 
          break;
          }
        default:
          println(hlog, "case unhandled ", df);
          exit(1);
        }
      else switch(df) {
        case 0: 
        case 3:
          at = at1;
          continue;
        case 1:
          auto at2 = at + eudir(dx+1);
          auto& wc2 = get_mapping(at2);
          if(wc2.cw.at) {
            auto at3 = at1 + eudir(wc1.rdir);
            auto& wc3 = get_mapping(at3);
            auto at4 = at3 + eudir(wc3.rdir);
            if(at4 == at2) {
              wc.rdir = (dx+1)%4;
              wc1.rdir = -1;
              wc3.rdir = -1;
              conn(at, (dx+1)%4);
              }
            else { 
              at = at1;
              }
            }
          else {
            wc.rdir = (dx+1)%4;
            wc1.rdir = -1;
            wc2.rdir = dx%4;
            int bdir = -1;
            int bdist = 100;
            for(int d=0; d<4; d++) {
              auto &wcm = get_mapping(at2 + eudir(d));
              if(wcm.cw.at && length(wcm.start - at2) < bdist)
                bdist = length(wcm.start - at2), bdir = d;
              }
            if(bdir != -1) conn(at2 + eudir(bdir), bdir ^ 2);
            conn(at, (dx+1)%4);
            conn(at2, dx%4);
            
            at = param * loc(1,0) + at * loc(0, 1);
            }
          break;
        }
      }

    DEBB(DF_GP, ("DONE"))
    }
  
  EX hyperpoint loctoh_ort(loc at) {
    return point3(at.first, at.second, 1);
    }

  hyperpoint corner_coords6[7] = {
    point3(2, -1, 0),
    point3(1, 1, 0),
    point3(-1, 2, 0),
    point3(-2, 1, 0),
    point3(-1, -1, 0),
    point3(1, -2, 0),
    point3(0, 0, 0) // center, not a corner
    };

  hyperpoint corner_coords4[7] = {
    point3(1.5, -1.5, 0),
//    point3(1, 0, 0),
    point3(1.5, 1.5, 0),
//    point3(0, 1, 0),
    point3(-1.5, 1.5, 0),
//    point3(-1, 0, 0),
    point3(-1.5, -1.5, 0),
//    point3(0, -1, 0),
    point3(0, 0, 0),
    point3(0, 0, 0),
    point3(0, 0, 0)
    };

  #define corner_coords (S3==3 ? corner_coords6 : corner_coords4)
  
  hyperpoint cornmul(const transmatrix& corners, const hyperpoint& c) {
    if(sphere && S3 == 3) {
      ld cmin = c[0] * c[1] * c[2] * (6 - S7);
      return corners * point3(c[0] + cmin, c[1] + cmin, c[2] + cmin);
      }
    else return corners * c;
    }
    
  hyperpoint atz(const transmatrix& T, const transmatrix& corners, loc at, int cornerid = 6, ld cf = 3) {
    int sp = 0;
    again:
    auto corner = corners * (loctoh_ort(at) + (corner_coords[cornerid] / cf));
    if(corner[1] < -1e-6 || corner[2] < -1e-6) {
      at = at * eudir(1);
      if(cornerid < SG6) cornerid = (1 + cornerid) % SG6;
      sp++;
      goto again;
      }
    if(sp>SG3) sp -= SG6;

    return normalize(spin(TAU*sp/S7) * cornmul(T, corner));
    }
  
  transmatrix dir_matrix(int i) {
    auto ddspin = [] (int d) -> transmatrix { 
      return spin(M_PI - d * TAU / S7 - cgi.hexshift);
      };
    return spin(-cgi.gpdata->alpha) * build_matrix(
      C0, 
      ddspin(i) * xpush0(cgi.tessf),
      ddspin(i+1) * xpush0(cgi.tessf),
      C03
      );
    }
  
  void prepare_matrices() {
    cgi.gpdata->corners = inverse(build_matrix(
      loctoh_ort(loc(0,0)),
      loctoh_ort(param),
      loctoh_ort(param * loc(0,1)),
      C03
      ));
    cgi.gpdata->Tf.resize(S7);
    for(int i=0; i<S7; i++) {
      transmatrix T = dir_matrix(i);
      for(int x=-GOLDBERG_LIMIT_HALF; x<GOLDBERG_LIMIT_HALF; x++)
      for(int y=-GOLDBERG_LIMIT_HALF; y<GOLDBERG_LIMIT_HALF; y++)
      for(int d=0; d<(S3==3?6:4); d++) {
        loc at = loc(x, y);
        
        hyperpoint h = atz(T, cgi.gpdata->corners, at, 6);
        hyperpoint hl = atz(T, cgi.gpdata->corners, at + eudir(d), 6);
        cgi.gpdata->Tf[i][x&GOLDBERG_MASK][y&GOLDBERG_MASK][d] = rgpushxto0(h) * rspintox(gpushxto0(h) * hl) * spin180();
        }
      }       
    }

  EX hyperpoint get_corner_position(const local_info& li, int cid, ld cf IS(3)) {
    int i = li.last_dir;
    if(i == -1) 
      return atz(dir_matrix(cid), cgi.gpdata->corners, li.relative, 0, cf);
    else {
      auto& cellmatrix = cgi.gpdata->Tf[i][li.relative.first&GOLDBERG_MASK][li.relative.second&GOLDBERG_MASK][fixg6(li.total_dir)];
      return inverse(cellmatrix) * atz(dir_matrix(i), cgi.gpdata->corners, li.relative, fixg6(cid + li.total_dir), cf);
      }
    }
  
  EX hyperpoint get_corner_position(cell *c, int cid, ld cf IS(3)) {
    return get_corner_position(get_local_info(c), cid, cf);
    }
    
  map<pair<int, int>, loc> center_locs;
  
  EX void compute_geometry(bool inv) {
    center_locs.clear();
    if(GOLDBERG_INV || inv) {
      if(!cgi.gpdata) cgi.gpdata = make_shared<geometry_information::gpdata_t>();
      gp::clear_plainshapes();
      int x = param.first;
      int y = param.second;
      if(S3 == 3)
        cgi.gpdata->area = ((2*x+y) * (2*x+y) + y*y*3) / 4;
      else
        cgi.gpdata->area = x * x + y * y;
      next = point3(x+y/2., -y * sqrt(3) / 2, 0);
      ld scale = 1 / hypot_d(2, next);
      if(!GOLDBERG) scale = 1;
      cgi.crossf *= scale;
      cgi.hepvdist *= scale;
      cgi.hexhexdist *= scale;
      cgi.hexvdist *= scale;
      cgi.rhexf *= scale;
//    spin = spintox(next);
//    ispin = rspintox(next);
      cgi.gpdata->alpha = -atan2(next[1], next[0]) * 6 / S7;
      if(S3 == 3)
        cgi.base_distlimit = (cgi.base_distlimit + log(scale) / log(2.618)) / scale;
      else
        cgi.base_distlimit = 3 * max(param.first, param.second) + 2 * min(param.first, param.second);
      if(S7 == 12)
        cgi.base_distlimit = 2 * param.first + 2 * param.second + 1;
      if(cgi.base_distlimit > SEE_ALL)
        cgi.base_distlimit = SEE_ALL;
      prepare_matrices();
      DEBB(DF_GEOM | DF_POLY, ("scale = ", scale));
      }
    }

  loc config;
  
  EX bool rotate_and_check_limits(loc& v) {
    int& x = v.first, &y = v.second;
    while(x < 0 || y < 0 || (x == 0 && y > 0))
      v = v * loc(0, 1);
    return 2*(x+y) < (1<<GOLDBERG_BITS);
    }

  EX bool check_limits(loc v) {
    return rotate_and_check_limits(v);
    }

  loc internal_representation(loc v) {
    int& x = v.first, &y = v.second;
    while(!rotate_and_check_limits(v)) {
      if(x > y) x--; else y--;
      }
    if(S3 == 3 && y > x) v = v * loc(1, -1);
    return v;
    }
  
  EX loc human_representation(loc v) {
    int& x = v.first, &y = v.second;
    if(S3 == 3) while(x < 0 || y < 0 || (x == 0 && y > 0))
      v = v * loc(0, 1);
    return v;
    }
  
  EX eVariation variation_for(loc xy) {
    if(xy.first == 1 && xy.second == 0) 
      return eVariation::pure;
    if(xy.first == 1 && xy.second == 1 && S3 == 3) 
      return eVariation::bitruncated;
    return eVariation::goldberg;
    }
  
  EX void whirl_set(loc xy) {
    xy = internal_representation(xy);
    if(xy.second && xy.second != xy.first && nonorientable) {
      addMessage(XLAT("This does not work in non-orientable geometries"));
      xy.second = 0;
      }
    config = human_representation(xy);
    auto g = screens;
    if(xy.first == 0 && xy.second == 0) xy.first = 1;
    stop_game();
    param = xy;
    if(xy.first == 1 && xy.second == 0) {
      set_variation(eVariation::pure);
      }
    else if(xy.first == 1 && xy.second == 1 && S3 == 3) {
      set_variation(eVariation::bitruncated);
      }
    else 
      set_variation(eVariation::goldberg);
    start_game();
    screens = g;
    }

  string helptext() {
    return XLAT(
      "Goldberg polyhedra are obtained by adding extra hexagons to a dodecahedron. "
      "GP(x,y) means that, to get to a nearest non-hex from any non-hex, you should move x "
      "cells in any direction, turn right 60 degrees, and move y cells. "
      "HyperRogue generalizes this to any tesselation with 3 faces per vertex. "
      "By default HyperRogue uses bitruncation, which corresponds to GP(1,1)."
      );
    }  

  void show() {
    cmode = sm::SIDE | sm::MAYDARK;
    gamescreen();
    dialog::init(XLAT("variations"));
    
    int min_quality_chess = 0;
    
    int min_quality = 0;
#if CAP_TEXTURE
    if((texture::config.tstate == texture::tsActive) && (S7 % 2 == 1)) {
      if(texture::cgroup == cpFootball || texture::cgroup == cpThree) min_quality = 1;
      }

    if((texture::config.tstate == texture::tsActive) && (S7 % 2 == 1) && (S3 == 4)) {
      if(texture::cgroup == cpChess) min_quality = 1;
      }
#endif    
    if(min_quality == 0 && min_quality_chess == 0) {
      dialog::addBoolItem(XLAT("pure"), PURE || (GOLDBERG && univ_param() == loc(1,0)), 'a');
      dialog::lastItem().value = "GP(1,0)";
      dialog::add_action_confirmed([] { whirl_set(loc(1, 0)); });
      }
    
    if(min_quality_chess == 0) {
      dialog::addBoolItem(XLAT("bitruncated"), BITRUNCATED, 'b');  
      dialog::add_action_confirmed([] { 
        if(S3 == 4) {
          if(!BITRUNCATED) {
            stop_game();
            set_variation(eVariation::bitruncated);
            start_game();
            }
          }
        else 
          whirl_set(loc(1, 1));
        });
      }

    dialog::lastItem().value = S3 == 3 ? "GP(1,1)" : ONOFF(BITRUNCATED);

    if(min_quality == 0 || min_quality_chess) {
      dialog::addBoolItem(S3 == 3 ? XLAT("chamfered") : XLAT("expanded"), univ_param() == loc(2,0) && GOLDBERG, 'c');
      dialog::lastItem().value = "GP(2,0)";
      dialog::add_action_confirmed([] { 
        whirl_set(loc(2, 0));
        });
      }

    if(S3 == 3) {
      dialog::addBoolItem(XLAT("2x bitruncated"), GOLDBERG && univ_param() == loc(3,0), 'd');
      dialog::lastItem().value = "GP(3,0)";
      dialog::add_action_confirmed([] { 
        whirl_set(loc(3, 0));
        });
      }
    else {
      dialog::addBoolItem(XLAT("rectified"), param == loc(1,1) && GOLDBERG, 'd');
      dialog::lastItem().value = "GP(1,1)";
      dialog::add_action_confirmed([] { 
        whirl_set(loc(1, 1));
        });
      }

    dialog::addBreak(100);
    dialog::addSelItem("x", its(config.first), 'x');
    dialog::add_action([] { dialog::editNumber(config.first, 0, 8, 1, 1, "x", helptext()); });
    dialog::addSelItem("y", its(config.second), 'y');
    dialog::add_action([] { dialog::editNumber(config.second, 0, 8, 1, 1, "y", helptext()); });
    
    if(!check_limits(config))
      dialog::addInfo(XLAT("Outside of the supported limits"));
    if(config.second && config.second != config.first && nonorientable) {
      dialog::addInfo(XLAT("This does not work in non-orientable geometries"));
      }
    else if((config.first-config.second)%3 && min_quality)
      dialog::addInfo(XLAT("This pattern needs x-y divisible by 3"));
    else if((config.first-config.second)%2 && min_quality_chess)
      dialog::addInfo(XLAT("This pattern needs x-y divisible by 2"));
    else {
      dialog::addBoolItem(XLAT("select"), param == internal_representation(config) && !IRREGULAR && !INVERSE, 'f');
      dialog::lastItem().value = "GP(x,y)";
      }
    dialog::add_action_confirmed([] { whirl_set(config); });

    dialog::addBreak(100);
      
    #if CAP_IRR
    if(irr::supports(geometry)) {
      dialog::addBoolItem(XLAT("irregular"), IRREGULAR, 'i');
      dialog::add_action(dialog::add_confirmation([=] () { 
        if(min_quality && !irr::bitruncations_requested) irr::bitruncations_requested++;
        if(euclid && (!closed_manifold || nonorientable)) { 
          println(hlog, XLAT("To create Euclidean irregular tesselations, first enable a torus"));
          return;
          }
        if(!IRREGULAR) irr::visual_creator(); 
        }));
      }
    #endif

    dialog::addBreak(100);
    int style = 0;
    auto v0 = variation_for(param);
    bool bad_bi = BITRUNCATED && a4;
    if(!bad_bi) {
      dynamicval<eVariation> v(variation, v0);
      if(geosupport_football() == 2) style = 3;
      if(geosupport_chessboard()) style = 2;
      }
    if(style == 2) {
      dialog::addBoolItem(XLAT("inverse rectify"), UNRECTIFIED, 'r');
      dialog::add_action_confirmed([v0] {
        param = univ_param();
        if(UNRECTIFIED) set_variation(v0);
        else set_variation(eVariation::unrectified);
        start_game();
        config = human_representation(univ_param());
        });
      }
    else if(style == 3) {
      dialog::addBoolItem(XLAT("inverse truncate"), UNTRUNCATED, 't');
      dialog::add_action_confirmed([v0] {
        param = univ_param();
        if(UNTRUNCATED) set_variation(v0);
        else set_variation(eVariation::untruncated);
        start_game();
        });
      dialog::addBoolItem(XLAT("warped version"), WARPED, 'w');
      dialog::add_action_confirmed([v0] {
        param = univ_param();
        if(WARPED) set_variation(v0);
        else set_variation(eVariation::warped);
        start_game();
        });
      }

    dialog::addBreak(100);
    dialog::addItem(XLAT("swap x and y"), 'z');
    dialog::add_action([] { swap(config.first, config.second); });

    bool have_dual = !bad_bi && !IRREGULAR && !WARPED;
    if(S3 == 3 && UNTRUNCATED && (univ_param()*loc(1,1)) % 3) have_dual = false;
    if(S3 == 4 && UNRECTIFIED && (univ_param()*loc(1,1)) % 2) have_dual = false;
    
    if(have_dual) {
      dialog::addItem(XLAT("dual of current"), 'D');
      dialog::add_action([] { 
        auto p = univ_param();
        if(S3 == 3 && !UNTRUNCATED) {
          println(hlog, "set param to ", p * loc(1,1));
          whirl_set(p * loc(1, 1));
          set_variation(eVariation::untruncated);
          start_game();
          config = human_representation(univ_param());
          }
        else if(S3 == 4 && !UNRECTIFIED) {
          whirl_set(p * loc(1, 1));
          set_variation(eVariation::unrectified);
          start_game();
          config = human_representation(univ_param());
          }
        else if(S3 == 3 && UNTRUNCATED) {
          println(hlog, "whirl_set to ", (p * loc(1,1)) / 3);
          whirl_set((p * loc(1,1)) / 3);
          config = human_representation(univ_param());
          }
        else if(S3 == 4 && UNRECTIFIED) {
          whirl_set((p * loc(1,1)) / 2);
          config = human_representation(univ_param());
          }
        });
      }
    
    dialog::addBreak(100);
    dialog::addHelp();
    dialog::add_action([] { gotoHelp(helptext()); });
    dialog::addBack();
    dialog::display();
    }
  
  EX loc univ_param() {
    if(GOLDBERG_INV) return param;
    else if(PURE) return loc(1,0);
    else return loc(1,1);
    }
  
  EX void configure() {
    auto l = univ_param();
    param = l;
    config = human_representation(l);
    pushScreen(gp::show);
    }
  
  EX void be_in_triangle(local_info& li) {
    int sp = 0;
    auto& at = li.relative;
    again:
    auto corner = cgi.gpdata->corners * loctoh_ort(at);
    if(corner[1] < -1e-6 || corner[2] < -1e-6) {
      at = at * eudir(1);
      sp++;
      goto again;
      }
    if(sp>SG3) sp -= SG6;
    li.last_dir = gmod(li.last_dir - sp, S7);
    }

  // from some point X, (0,0) is in distance dmain, param is in distance d0, and param*z is in distance d1
  // what is the distance of at from X?

  EX int solve_triangle(int dmain, int d0, int d1, loc at) {
    loc centerloc(0, 0);
    auto rel = make_pair(d0-dmain, d1-dmain);
    if(center_locs.count(rel))
      centerloc = center_locs[rel];
    else {
      bool found = false;
      for(int y=-20; y<=20; y++)
      for(int x=-20; x<=20; x++) {
        loc c(x, y);
        int cc = length(c);
        int c0 = length(c - param);
        int c1 = length(c - param*loc(0,1));
        if(c0-cc == d0-dmain && c1-cc == d1-dmain)
          found = true, centerloc = c;
        }
      if(!found && !quotient) {
        println(hlog, "Warning: centerloc not found: ", make_tuple(dmain, d0, d1));
        }
      center_locs[rel] = centerloc;
      }
    
    return dmain + length(centerloc-at) - length(centerloc);
    }

  int solve_quad(int dmain, int d0, int d1, int dx, loc at) {
    loc centerloc(0, 0);
    auto rel = make_pair(d0-dmain, (d1-dmain) + 1000 * (dx-dmain) + 1000000);
    if(center_locs.count(rel))
      centerloc = center_locs[rel];
    else {
      bool found = false;
      for(int y=-20; y<=20; y++)
      for(int x=-20; x<=20; x++) {
        loc c(x, y);
        int cc = length(c);
        int c0 = length(c - param);
        int c1 = length(c - param*loc(0,1));
        int c2 = length(c - param*loc(1,1));
        if(c0-cc == d0-dmain && c1-cc == d1-dmain && c2-cc == dx-dmain)
          found = true, centerloc = c;
        }
      if(!found && !quotient) {
        println(hlog, "Warning: centerloc not found: ", make_tuple(dmain, d0, d1, dx));
        }
      center_locs[rel] = centerloc;
      }
    
    return dmain + length(centerloc-at) - length(centerloc);
    }
  
  EX hyperpoint get_master_coordinates(cell *c) {
    auto li = get_local_info(c);
    be_in_triangle(li);
    return cgi.gpdata->corners * loctoh_ort(li.relative);
    }
  
  EX int compute_dist(cell *c, int master_function(cell*)) {
    if(!GOLDBERG) return master_function(c);
    auto li = get_local_info(c);
    be_in_triangle(li);
    
    cell *cm = c->master->c7;
    
    int i = li.last_dir;
    auto at = li.relative;

    auto dmain = master_function(cm);
    auto d0 = master_function(createStep(cm->master, i)->c7);
    auto d1 = master_function(createStep(cm->master, cm->c.fix(i+1))->c7);
    
    if(S3 == 4) {
      heptspin hs(cm->master, i);
      hs += wstep; hs+=-1; hs += wstep;
      auto d2 = master_function(hs.at->c7);
      return solve_quad(dmain, d0, d1, d2, at);
      }
    
    return solve_triangle(dmain, d0, d1, at);
    }
  
  EX int dist_2() {
    return length(univ_param());
    }

  EX int dist_3() {
    return length(univ_param() * loc(1,1));
    }
  
  EX int dist_1() {
    return dist_3() - dist_2();
    }
#else
  EX int dist_1() { return 1; }
  EX int dist_2() { return BITRUNCATED ? 2 : 1; }
  EX int dist_3() { return BITRUNCATED ? 3 : 2; }  
#endif

  EX array<heptagon*, 3> get_masters(cell *c) {
    if(0);
    #if CAP_GP
    else if(INVERSE) {
      c = get_mapped(c);
      return UIU(get_masters(c));
      }
    else if(GOLDBERG) {
      auto li = get_local_info(c);
      be_in_triangle(li);      
      auto cm = c->master;
      int i = li.last_dir;
      return make_array(cm, cm->cmove(i), cm->cmodmove(i+1));
      }
    #endif
    #if CAP_IRR
    else if(IRREGULAR)
      return irr::get_masters(c);
    #endif
    else
      return make_array(c->cmove(0)->master, c->cmove(2)->master, c->cmove(4)->master);
    }

  EX string operation_name() {
    if(0);
    #if CAP_IRR
    else if(IRREGULAR) 
      return XLAT("irregular");
    #endif
    else if(DUAL)
      return XLAT("dual");
    else if(PURE)
      return XLAT("pure");
    else if(BITRUNCATED)
      return XLAT("bitruncated");
    #if CAP_GP
    else if(GOLDBERG && param == loc(1, 0))
      return XLAT("pure");
    else if(GOLDBERG && param == loc(1, 1) && S3 == 3)
      return XLAT("bitruncated");
    else if(GOLDBERG && param == loc(1, 1) && S3 == 4)
      return XLAT("rectified");
    else if(UNRECTIFIED && param == loc(1, 1) && S3 == 4)
      return XLAT("dual");
    else if(UNTRUNCATED && param == loc(1, 1) && S3 == 3)
      return XLAT("dual");
    else if(GOLDBERG && param == loc(2, 0))
      return S3 == 3 ? XLAT("chamfered") : XLAT("expanded");
    else if(GOLDBERG && param == loc(3, 0) && S3 == 3)
      return XLAT("2x bitruncated");
    #if MAXMDIM >= 4
    else if(variation == eVariation::subcubes)
      return XLAT("subcubed") + "(" + its(reg3::subcube_count) + ")";
    else if(variation == eVariation::dual_subcubes)
      return XLAT("dual-subcubed") + "(" + its(reg3::subcube_count) + ")";
    else if(variation == eVariation::bch)
      return XLAT("bitruncated-subcubed") + "(" + its(reg3::subcube_count) + ")";
    else if(variation == eVariation::coxeter)
      return XLAT("subdivided") + "(" + its(reg3::coxeter_param) + ")";
    #endif
    else {
      auto p = human_representation(param);
      string s = "GP(" + its(p.first) + "," + its(p.second) + ")";
      if(UNRECTIFIED) return XLAT("unrectified") + " " + s;
      if(WARPED) return XLAT("warped") + " " + s;
      if(UNTRUNCATED) return XLAT("untruncated") + " " + s;
      return s;
      }
    #else
    else return "UNSUPPORTED";
    #endif
    }
  
  /* inverse map */
  
  EX hrmap *pmap;
  // EX geometry_information *underlying_cgip;
  
  struct hrmap_inverse : hrmap {
    hrmap *underlying_map;
    
    map<cell*, cell*> mapping;
    map<cell*, int> shift;
    
    template<class T> auto in_underlying(const T& t) -> decltype(t()) {
      dynamicval<hrmap*> gpm(pmap, this);
      dynamicval<eVariation> gva(variation, variation_for(param));
      dynamicval<hrmap*> gu(currentmap, underlying_map);
      // dynamicval<geometry_information*> gc(cgip, underlying_cgip);
      return t();
      }
    
    cell* get_mapped(cell *underlying_cell, int set_shift) {
      if(mapping.count(underlying_cell))
        return mapping[underlying_cell];
      int d = underlying_cell->type;
      if(UNTRUNCATED) d /= 2;
      if(WARPED && set_shift < 2) d /= 2;
      cell *c = newCell(d, underlying_cell->master);
      mapping[underlying_cell] = c;
      if(!UNRECTIFIED) shift[c] = set_shift;
      mapping[c] = underlying_cell;
      return c;
      }
    
    transmatrix relative_matrixh(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
      return in_underlying([&] { return currentmap->relative_matrix(h2, h1, hint); });
      }

    transmatrix relative_matrixc(cell *c2, cell *c1, const hyperpoint& hint) override {
      c1 = mapping[c1];
      c2 = mapping[c2];
      return in_underlying([&] { return currentmap->relative_matrix(c2, c1, hint); });
      }

    ~hrmap_inverse() {
      in_underlying([this] { delete underlying_map; });
      }

    heptagon *getOrigin() override { return in_underlying([this] { return underlying_map->getOrigin(); }); }
    
    cell *gs;
    
    cell* gamestart() override { 
      return gs; 
      }

    hrmap_inverse() {
      if(0) {
        println(hlog, "making ucgi");
        dynamicval<eVariation> gva(variation, variation_for(param));
        check_cgi();
        cgi.require_basics();
        // underlying_cgip = cgip;
        println(hlog, "done ucgi");
        }
      bool warped = WARPED;
      in_underlying([&,this] { 
        initcells(); 
        underlying_map = currentmap; 
        gs = currentmap->gamestart();
        if(!warped) gs = gs->cmove(0);
        });
      if(UNTRUNCATED) gs = get_mapped(gs, 1);
      else gs = get_mapped(gs, 2);
      for(hrmap*& m: allmaps) if(m == underlying_map) m = NULL;
      }
    
    cell *create_move(cell *parent, int d) {
      if(UNRECTIFIED) {
        cellwalker cw(mapping[parent], d);
        bool b = cw.mirrored;
        in_underlying([&] {
          cw += wstep;
          cw --;
          cw += wstep;
          cw --;
          if(cw.mirrored != b) cw++;
          });
        cw.at = get_mapped(cw.at, 0);
        parent->c.connect(d, cw.at, cw.spin, cw.mirrored);
        return cw.at;
        }
      if(UNTRUNCATED) {
        cellwalker cw(mapping[parent], 2*d+shift[parent]);
        in_underlying([&] {
          cw += wstep;
          });
        cw.at = get_mapped(cw.at, cw.spin & 1);
        parent->c.connect(d, cw.at, cw.spin / 2, cw.mirrored);
        return cw.at;
        }
      if(WARPED) {
        int sh = shift[parent];
        if(sh == 2) {
          cellwalker cw(mapping[parent], d);
          in_underlying([&] { cw += wstep; });
          cw.at = get_mapped(cw.at, cw.spin & 1);
          parent->c.connect(d, cw.at, cw.spin / 2, cw.mirrored);
          return cw.at;
          }
        else {
          cellwalker cw(mapping[parent], 2*d+sh);
          in_underlying([&] {
            cw += wstep;
            });
          cw.at = get_mapped(cw.at, 2);
          parent->c.connect(d, cw.at, cw.spin, cw.mirrored);
          return cw.at;
          }
        }
      throw hr_exception("unimplemented");
      }

    transmatrix adj(cell *c, int d) override {
      transmatrix T;
      if(UNRECTIFIED) {
        cellwalker cw(mapping[c], d);
        in_underlying([&] {
          T = currentmap->adj(cw.at, cw.spin);
          cw += wstep;
          cw --;
          T = T * currentmap->adj(cw.at, cw.spin);
          });
        }
      if(UNTRUNCATED) {
        cellwalker cw(mapping[c], 2*d+shift[c]);
        in_underlying([&] { T = currentmap->adj(cw.at, cw.spin); });
        }
      if(WARPED) {
        int sh = shift[c];
        if(sh == 2) {
          auto c1 = mapping[c];
          in_underlying([&] { T = currentmap->adj(c1, d); });
          }
        else {
          cellwalker cw(mapping[c], 2*d+shift[c]);
          in_underlying([&] { T = currentmap->adj(cw.at, cw.spin); });
          }
        }
      return T;
      }
    
    void draw_at(cell *at, const shiftmatrix& where) override {
      
      dq::clear_all();
      
      auto enqueue = (quotient ? dq::enqueue_by_matrix_c : dq::enqueue_c);
      enqueue(at, where);

      while(!dq::drawqueue_c.empty()) {
        auto& p = dq::drawqueue_c.front();
        cell *c = p.first;
        shiftmatrix V = p.second;
        auto c1 = get_mapped(c, 0);
        
        in_underlying([&] {
          if(GOLDBERG) {
            gp::current_li = gp::get_local_info(c1);
            }
          else {
            gp::current_li.relative.first = shvid(c1);
            gp::current_li.relative.second = shift[c];
            }
          });
      
       
        dq::drawqueue_c.pop();

        if(!do_draw(c, V)) continue;
        drawcell(c, V);
        
        for(int i=0; i<c->type; i++) if(c->cmove(i))
          enqueue(c->move(i), optimized_shift(V * adj(c, i)));
        }
      }

    void find_cell_connection(cell *c, int d) override {
      inverse_move(c, d);
      }

    int shvid(cell *c) override {
      return gp::get_plainshape_id(c);
      }   

    hyperpoint get_corner(cell *c, int cid, ld cf) override {
      if(UNTRUNCATED) {
        cell *c1 = gp::get_mapped(c);
        cellwalker cw(c1, cid*2);
        if(!gp::untruncated_shift(c)) cw--;
        hyperpoint h = UIU(nearcorner(c1, cw.spin));
        return mid_at_actual(h, 3/cf);
        }
      if(UNRECTIFIED) {
        cell *c1 = gp::get_mapped(c);
        hyperpoint h = UIU(nearcorner(c1, cid));
        return mid_at_actual(h, 3/cf);
        }
      if(WARPED) {
        int sh = gp::untruncated_shift(c);
        cell *c1 = gp::get_mapped(c);
        if(sh == 2) {
          cellwalker cw(c, cid);
          hyperpoint h1 = UIU(tC0(currentmap->adj(c1, cid)));
          cw--;
          hyperpoint h2 = UIU(tC0(currentmap->adj(c1, cw.spin)));
          hyperpoint h = mid(h1, h2);
          return mid_at_actual(h, 3/cf);
          }
        else {
          cellwalker cw(c1, cid*2);
          if(!gp::untruncated_shift(c)) cw--;
          hyperpoint h = UIU(nearcorner(c1, cw.spin));
          h = mid(h, C0);
          return mid_at_actual(h, 3/cf);
          }
        }
      return C0;
      }
    };
  
  EX hrmap* new_inverse() { return new hrmap_inverse; }
  
  hrmap_inverse* inv_map() { return (hrmap_inverse*)currentmap; }

  EX hrmap* get_underlying_map() { return inv_map()->underlying_map; }
  EX cell* get_mapped(cell *c) { return inv_map()->get_mapped(c, 0); }
  EX int untruncated_shift(cell *c) { return inv_map()->shift[c]; }

  EX void delete_mapped(cell *c) { 
    if(!pmap) return;
    auto i = (hrmap_inverse*) pmap;
    if(i->mapping.count(c))
      destroy_cell(i->mapping[c]);
    }
  
  EX cell *inverse_move(cell *c, int d) { return inv_map()->create_move(c, d); }

  #if HDR
  template<class T> auto in_underlying_geometry(const T& f) -> decltype(f()) {
    if(!INVERSE) return f();
    dynamicval<hrmap*> gpm(pmap, currentmap);
    dynamicval<eVariation> gva(variation, variation_for(param));
    dynamicval<hrmap*> gu(currentmap, get_underlying_map());
    // dynamicval<geometry_information*> gc(cgip, underlying_cgip);
    return f();
    }
  
  #define UIU(x) hr::gp::in_underlying_geometry([&] { return (x); })
  #endif


    
  }}
