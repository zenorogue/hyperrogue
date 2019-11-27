// Hyperbolic Rogue -- Goldberg-Coxeter construction
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file goldberg.cpp
 *  \brief Goldberg-Coxeter construction
 *
 *  This is generally not used for standard pure and bitruncated tilings, even though they are technically Goldberg too.
 */

#include "hyper.h"
namespace hr { 

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
    };

  struct local_info {
    int last_dir;
    loc relative;
    int first_dir;
    int total_dir;
    };
  #endif

  EX local_info draw_li;
  
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
    return eudist(p.first, p.second);
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

  EX int fixg6(int x) { return (x + MODFIXER) % SG6; }
  
  EX int get_code(const local_info& li) {
    return 
      ((li.relative.first & 15) << 0) +
      ((li.relative.second & 15) << 4) +
      ((fixg6(li.total_dir)) << 8) +
      ((li.last_dir & 15) << 12);
    }
  
  EX local_info get_local_info(cell *c) {
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
    return (v.first - v.second + MODFIXER)%3;
    }
  
  // mapping of the local equilateral triangle
  // goldberg_map[y][x].cw is the cellwalker in this triangle at position (x,y)
  // facing local direction 0  
  
  goldberg_mapping_t goldberg_map[32][32];
  void clear_mapping() {
    for(int y=0; y<32; y++) for(int x=0; x<32; x++) {
      goldberg_map[y][x].cw.at = NULL;
      goldberg_map[y][x].rdir = -1;
      goldberg_map[y][x].mindir = 0;
      }
    }
  
  goldberg_mapping_t& get_mapping(loc c) {
    return goldberg_map[c.second&31][c.first&31];
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
        }
      return false;
      }
    if(peek(wcw)) {
      set_localwalk(wc1, dir1, wcw + wstep);
      DEBB(DF_GP, (at1, " :", wcw+wstep, " (pulled from ", at, " :: ", wcw, ")"));
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
    DEBB0(DF_GP, ("  connection ", at, "/", dir, " ", wc.cw+dir, "=", wcw, " ~ ", at+eudir(dir), "/", dir1); )
    if(!wc1.cw.at) {
      wc1.start = wc.start;
      if(peek(wcw)) {
        DEBB0(DF_GP, ("(pulled) "); )
        set_localwalk(wc1, dir1, wcw + wstep);
        if(do_adjm) wc1.adjm = wc.adjm * gp_adj[{wcw.at, wcw.spin}];
        }
      else {
        peek(wcw) = newCell(SG6, wc.cw.at->master);
        wcw.at->c.setspin(wcw.spin, 0, false);
        set_localwalk(wc1, dir1, wcw + wstep);
        if(do_adjm) wc1.adjm = wc.adjm;
        spawn++;
        DEBB0(DF_GP, ("(created) "); )
        }
      }
    DEBB0(DF_GP, (wc1.cw+dir1, " "));
    auto wcw1 = get_localwalk(wc1, dir1);
    if(peek(wcw)) {
      if(wcw+wstep != wcw1) {
        DEBB(DF_GP, ("FAIL: ", wcw, " / ", wcw1); exit(1); )
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
      gp_adj[{wcw.at, wcw.spin}] = inverse(wc.adjm) * wc1.adjm;
      gp_adj[{wcw1.at, wcw1.spin}] = inverse(wc1.adjm) * wc.adjm;
      }
    }

  void conn(loc at, int dir) {
    conn1(at, fixg6(dir), fixg6(dir+SG3));
    conn1(at + eudir(dir), fixg6(dir+SG3), fixg6(dir));
    }
  
  EX map<pair<cell*, int>, transmatrix> gp_adj;

  goldberg_mapping_t& set_heptspin(loc at, heptspin hs) {
    auto& ac0 = get_mapping(at);
    ac0.cw = cellwalker(hs.at->c7, hs.spin, hs.mirrored);
    ac0.start = at;
    DEBB(DF_GP, (at, " : ", ac0.cw));
    return ac0;
    }

  EX void extend_map(cell *c, int d) {
    DEBB(DF_GP, ("EXTEND ",c, " ", d));
    if(c->master->c7 != c) {
      while(c->master->c7 != c) {
        DEBB(DF_GP, (c, " direction 0 corresponds to ", c->move(0), " direction ", c->c.spin(0)); )
        d = c->c.spin(0);
        c = c->move(0);
        }
      // c move 0 equals c' move spin(0)
      extend_map(c, d);
      extend_map(c, c->c.fix(d-1));
      extend_map(c, c->c.fix(d+1));
      if(S3 == 4 && !c->move(d))
        for(int i=0; i<S7; i++)
        for(int j=0; j<S7; j++)
          extend_map(createStep(c->master, i)->c7, j);
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

    do_adjm = quotient;
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
    
    if(S3 == 4 && param == loc(1,1)) {
      conn(loc(0,0), 1);
      conn(loc(0,1), 0);
      conn(loc(0,1), 1);
      conn(loc(0,1), 2);
      conn(loc(0,1), 3);
      return;
      }
    
    if(nonorientable && param.first == param.second) {
      int x = param.first;
      if(ac1.cw.mirrored != hs.mirrored) ac1.cw--;
      if(ac2.cw.mirrored != hs.mirrored) ac2.cw--;
      
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

    return normalize(spin(2*M_PI*sp/S7) * cornmul(T, corner));
    }
  
  transmatrix dir_matrix(int i) {
    auto ddspin = [] (int d) -> transmatrix { 
      return spin(M_PI - d * 2 * M_PI / S7 - cgi.hexshift);
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
    for(int i=0; i<S7; i++) {
      transmatrix T = dir_matrix(i);
      for(int x=-16; x<16; x++)
      for(int y=-16; y<16; y++)
      for(int d=0; d<(S3==3?6:4); d++) {
        loc at = loc(x, y);
        
        hyperpoint h = atz(T, cgi.gpdata->corners, at, 6);
        hyperpoint hl = atz(T, cgi.gpdata->corners, at + eudir(d), 6);
        cgi.gpdata->Tf[i][x&31][y&31][d] = rgpushxto0(h) * rspintox(gpushxto0(h) * hl) * spin(M_PI);
        }
      }       
    }

  EX hyperpoint get_corner_position(const local_info& li, int cid, ld cf IS(3)) {
    int i = li.last_dir;
    if(i == -1) 
      return atz(dir_matrix(cid), cgi.gpdata->corners, li.relative, 0, cf);
    else {
      auto& cellmatrix = cgi.gpdata->Tf[i][li.relative.first&31][li.relative.second&31][fixg6(li.total_dir)];
      return inverse(cellmatrix) * atz(dir_matrix(i), cgi.gpdata->corners, li.relative, fixg6(cid + li.total_dir), cf);
      }
    }
  
  EX hyperpoint get_corner_position(cell *c, int cid, ld cf IS(3)) {
    return get_corner_position(get_local_info(c), cid, cf);
    }
    
  map<pair<int, int>, loc> center_locs;
  
  EX void compute_geometry() {
    center_locs.clear();
    if(GOLDBERG) {
      if(!cgi.gpdata) cgi.gpdata = make_shared<geometry_information::gpdata_t>();
      int x = param.first;
      int y = param.second;
      if(S3 == 3)
        cgi.gpdata->area = ((2*x+y) * (2*x+y) + y*y*3) / 4;
      else
        cgi.gpdata->area = x * x + y * y;
      next = point3(x+y/2., -y * sqrt(3) / 2, 0);
      ld scale = 1 / hypot_d(2, next);
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
  
  loc internal_representation(loc v) {
    int& x = v.first, &y = v.second;
    while(x < 0 || y < 0 || (x == 0 && y > 0))
      v = v * loc(0, 1);
    if(x > 8) x = 8;
    if(y > 8) y = 8;
    if(S3 == 3 && y > x) v = v * loc(1, -1);
    return v;
    }
  
  EX loc human_representation(loc v) {
    int& x = v.first, &y = v.second;
    if(S3 == 3) while(x < 0 || y < 0 || (x == 0 && y > 0))
      v = v * loc(0, 1);
    return v;
    }
  
  void whirl_set(loc xy) {
    xy = internal_representation(xy);
    if(xy.second && xy.second != xy.first && nonorientable) {
      addMessage(XLAT("This does not work in non-orientable geometries"));
      xy.second = 0;
      }
    config = human_representation(xy);
    auto g = screens;
    if(xy.first == 0 && xy.second == 0) xy.first = 1;
    if(xy.first == 1 && xy.second == 0) {
      stop_game(); set_variation(eVariation::pure);
      }
    else if(xy.first == 1 && xy.second == 1 && S3 == 3) {
      stop_game(); set_variation(eVariation::bitruncated);
      }
    else {
      param = xy;
      stop_game(); set_variation(eVariation::goldberg);
      }
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
    gamescreen(0);  
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
      dialog::addBoolItem(XLAT("pure"), univ_param() == loc(1,0) && !IRREGULAR, 'a');
      dialog::lastItem().value = "GP(1,0)";
      }
    
    if(min_quality_chess == 0)
      dialog::addBoolItem(XLAT("bitruncated"), BITRUNCATED, 'b');  
    dialog::lastItem().value = S3 == 3 ? "GP(1,1)" : XLAT(BITRUNCATED ? "ON" : "OFF");

    if(min_quality == 0 || min_quality_chess) {
      dialog::addBoolItem(XLAT(S3 == 3 ? "chamfered" : "expanded"), univ_param() == loc(2,0), 'c');
      dialog::lastItem().value = "GP(2,0)";
      }

    if(S3 == 3) {
      dialog::addBoolItem(XLAT("2x bitruncated"), univ_param() == loc(3,0), 'd');
      dialog::lastItem().value = "GP(3,0)";
      }
    else {
      dialog::addBoolItem(XLAT("rectified"), param == loc(1,1) && GOLDBERG, 'd');
      dialog::lastItem().value = "GP(1,1)";
      }

    dialog::addBreak(100);
    dialog::addSelItem("x", its(config.first), 'x');
    dialog::addSelItem("y", its(config.second), 'y');
    
    if(config.second && config.second != config.first && nonorientable) {
      dialog::addInfo(XLAT("This does not work in non-orientable geometries"));
      }
    else if((config.first-config.second)%3 && min_quality)
      dialog::addInfo(XLAT("This pattern needs x-y divisible by 3"));
    else if((config.first-config.second)%2 && min_quality_chess)
      dialog::addInfo(XLAT("This pattern needs x-y divisible by 2"));
    else    
      dialog::addBoolItem(XLAT("select"), param == internal_representation(config) && !IRREGULAR, 'f');
      
    if(irr::supports(geometry)) {
      dialog::addBoolItem(XLAT("irregular"), IRREGULAR, 'i');
      dialog::add_action(dialog::add_confirmation([=] () { 
        if(min_quality && !irr::bitruncations_requested) irr::bitruncations_requested++;
        if(!IRREGULAR) irr::visual_creator(); 
        }));
      }
    
    dialog::addBreak(100);
    dialog::addHelp();
    dialog::addBack();
    dialog::display();

    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      if(uni == 'a') dialog::do_if_confirmed([] {
        whirl_set(loc(1, 0));
        });
      else if(uni == 'b') dialog::do_if_confirmed([] {
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
      else if(uni == 'c') dialog::do_if_confirmed([] {
        whirl_set(loc(2, 0));
        });
      else if(uni == 'd') dialog::do_if_confirmed([] {
        whirl_set(S3 == 3 ? loc(3, 0) : loc(1,1));
        });
      else if(uni == 'f') dialog::do_if_confirmed([] {
        whirl_set(config);
        });
      else if(uni == 'x')
        dialog::editNumber(config.first, 0, 8, 1, 1, "x", helptext());
      else if(uni == 'y')
        dialog::editNumber(config.second, 0, 8, 1, 1, "y", helptext());
      else if(uni == 'z')
        swap(config.first, config.second);
      else if(uni == '?' || sym == SDLK_F1 || uni == 'h' || uni == '2')
        gotoHelp(helptext());
      else if(doexiton(sym, uni))
        popScreen();
      };
    }
  
  EX loc univ_param() {
    if(GOLDBERG) return param;
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
    else if(param == loc(1, 0))
      return XLAT("pure");
    else if(param == loc(1, 1) && S3 == 3)
      return XLAT("bitruncated");
    else if(param == loc(1, 1) && S3 == 4)
      return XLAT("rectified");
    else if(param == loc(2, 0))
      return S3 == 3 ? XLAT("chamfered") : XLAT("expanded");
    else if(param == loc(3, 0) && S3 == 3)
      return XLAT("2x bitruncated");
    else {
      auto p = human_representation(param);
      return "GP(" + its(p.first) + "," + its(p.second) + ")";
      }
    #else
    else return "UNSUPPORTED";
    #endif
    }
    
  }}
