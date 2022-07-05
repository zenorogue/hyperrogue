// Banach-Tarski animation in RogueViz.
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// good parameters: -fixx 10 -W Cros -bantar_anim
// use -bantar_map to see how it works

#include "rogueviz.h"

namespace rogueviz { namespace banachtarski {

bool on;

typedef vector<int> cwpath;

cwpath invertpath(cwpath p) {
  cwpath res;
  for(int i=0; i<isize(p); i++)
    res.push_back(-p[isize(p)-1-i]);
  return res;
  }

array<cwpath, 4> gens;

cellwalker bttargets[4];

cellwalker trace(cellwalker cw, cwpath& p) {
  for(int i: p) if(i == 0) cw += wstep; else cw += i;
  return cw;
  }

set<cell*> testlist;

map<cell*, cell*> parent;

bool test_uniq(cellwalker cw, int z, int lev, cell *par) {
  if(testlist.count(cw.at)) return false;
  testlist.insert(cw.at);
  if(par) parent[cw.at] = par;
  if(celldist(cw.at) > 9) return true;

  /* if(cw.at->wall == waSea) {
    printf("test_uniq failed\n");
    cw.at->wall = waEternalFire;
    }
  cw.at->wall = waSea; */
  if(lev) for(int y=0; y<4; y++) if(y != z)
    if(!test_uniq(trace(cw, gens[y]), y^2, lev-1, cw.at))
      return false;
  
  return true;
  }

template<class T> void recursively(cell *c, cell *c1, const T& t) {
  t(c);
  for(int i=0; i<c->type; i++) 
    if(c->move(i) && c->c.spin(i) == 0 && c->move(i) != c1)
      recursively(c->move(i), c, t);
  }

vector<cell*> allcells;

set<cell*> seen;

struct cellinfo {
  cell *c;
  int gid;
  int spdist;
  eWall w;
  eItem it;
  eLand land;
  eMonster mo;
  vector<int> way;
  cwpath pinv;
  };

map<cell*, cellinfo> infos;

int cidd = 0;

int more = 5;

void debugpath(vector<int>& way, cwpath& pinv) {
  printf("pinv:"); for(int i: pinv) printf(" %d", i); printf("\n");

  cellwalker cw = cwt;
  
  cellwalker last_cw = cw;
  int lastd = 0;

  printf("way:"); for(int i: way) {
    cellwalker cw2 = trace(cw, pinv);
    printf(" [%d]", lastd = celldist(cw2.at));
    printf(" %d", i);
    last_cw = cw;
    cw = trace(cw, gens[i]);
    }

  cellwalker cw2 = trace(cw, pinv);
  int curd;
  printf(" [%d]", curd = celldist(cw2.at));
  printf("\n"); 

  if(lastd == 10 && curd == 2) {
    int b = way.back();
    way.pop_back();
    printf("CW:"); for(int w: way) for(int wp: gens[w]) printf(" %d", wp);
    printf(" {");
    for(int wp: gens[b]) printf(" %d", wp);
    printf(" }");
    for(int i: pinv) printf(" %d", i);
    way.push_back(b);
    printf("\n");
    }
  }


void recursive_paint(cwpath& pinv, vector<int>& way, int noway) {
  cellwalker cw = cwt;
  for(int i: way) cw = trace(cw, gens[i]);
  cw = trace(cw, pinv);
  cell *c = cw.at;

  /* if(cidd == 1 && way == vector<int>{1,2,3})
    c->item = itPirate; */

  if(seen.count(c)) { 
    printf("seen error [%d]\n", celldist(c)); 
    debugpath(way, pinv);
    debugpath(infos[c].way, infos[c].pinv);
    return; 
    }
  seen.insert(c);

  int hsh = 7;
  for(int w: way) hsh = 11301 * hsh + w * 37121;
  
  bool all0 = true;
  for(int w: way) if(w) all0 = false;
  
  int gid;
  
  /* if(d)
    c->landparam = 0x202020;
  else */
  if(all0 || way[0] == 2)
    gid = 0;
  else if(way[0] == 0)
    gid = 1;
  else if(way[0] == 1)
    gid = 2;
  else if(way[0] == 3)
    gid = 3;
  else
    gid = 3;
  
  infos[c] = cellinfo{c, gid, 0, waNone, itNone, laNone, moNone, way, pinv};

  // c->landparam ^= ((isize(way)&1) * 0x3F3F3F);
  // c->landparam = hsh; // d * 5 + 256 * (hsh&0xFFFF) + 0x400000;
  if(cidd>112899) c->landparam = 0x101010;
  // c->landparam = cidd * 0x1241C3;

  if(celldist(c) <= 11+more) for(int i=0; i<4; i++) if(i != noway) {
    vector<int> newway = {i};
    for(int ii: way) newway.push_back(ii);
    recursive_paint(pinv, newway, i^2);
    }
  }

bool once = true;

cwpath path_to(cell *c, int dir = 0) {
  cwpath p;
  cellwalker cw(c, dir);
  while(cw != cwt) {
    if(celldist((cw+wstep).at) < celldist(cw.at))
      p.push_back(0), cw += wstep;
    else {
      if(p.size() && p.back()) p.back()++;
      else p.push_back(1);
      cw += 1;
      }
    }
  return p;
  }

void bantar_note(cell *c) {
  if(seen.count(c)) return;
  
  cwpath pinv = invertpath(path_to(c));
  
  vector<int> way;
  recursive_paint(pinv, way, 4);
  cidd++;
  }

using bantar_config = pair<cell*, cell*>;

tuple<ld,bool,ld> quality(bantar_config cp) {
  shiftpoint h1 = tC0(ggmatrix(cp.first));
  shiftpoint h2 = tC0(ggmatrix(cp.second));
  return make_tuple(hdist0(h1) * hdist0(h2), h2[1] > 0, abs(h2[0] / h2[1]));
  }

int notry = 0;

void bantar() {
  if(!on) return;
  cwt = cellwalker(currentmap->gamestart(), 0);
  centerover = cwt.at;
  infos.clear();
  
  vector<bantar_config> genchoices;
  
  int lnotry = notry;
  
  {
  celllister clgen(cwt.at, 4, 1000000, NULL);
  for(cell *c1: clgen.lst) if(c1->type == S7)
  for(cell *c2: clgen.lst) if(c2->type == S7) 
    genchoices.emplace_back(c1, c2);
  stable_sort(genchoices.begin(), genchoices.end(), [] (const bantar_config b1, const bantar_config b2) { return quality(b1) < quality(b2); });
  
  for(bantar_config bc: genchoices) {
      if(get<0>(quality(bc)) >= 4) exit(1);
    for(int i=0; i<S7; i++)
    for(int j=0; j<S7; j++) {
      gens[0] = path_to(bc.first, i);
      gens[1] = path_to(bc.second, j);
      gens[2] = invertpath(gens[0]);
      gens[3] = invertpath(gens[1]);
      testlist.clear();
      parent.clear();
      bool tres = test_uniq(cwt, -1, 15, NULL);
      auto q = quality(bc);
      if(tres) {
        DEBB(DF_LOG, ("gens = ", gens));
        DEBB(DF_LOG, ("testing quality ", q, " ", make_pair(celldist(bc.first), celldist(bc.second)), ", result = ", tres));
        lnotry--; if(lnotry <= 0) goto picked;
        }
      // if(tres) goto picked;
      }
    }
  }
  picked:

  /*
  if(S7 == 8) {
    gens[1] = {0,4,0,4};
    gens[0] = {2,0,2};
    }
  else {
    gens[0] = {0,4,0,4};
    gens[1] = {2,0,4,0,2};
    }
  gens[2] = invertpath(gens[0]);
  gens[3] = invertpath(gens[1]);
  */
  
  for(int i=0; i<4; i++) bttargets[i] = trace(cwt, gens[i]);
  
  celllister cl(cwt.at, 8+more, 1000000, NULL);
  // recursively(cwt.at, NULL, [] (cell *c) { allcells.push_back(c); } );
  for(cell* c: cl.lst) bantar_note(c);
  
  for(cell *c: cl.lst) if(infos.count(c) && infos[c].gid == 0) 
    forCellEx(c2, c) if(infos.count(c2) && infos[c2].gid != 0)
      c->bardir = NOBARRIERS;
    
  for(int it=0; it<ittypes; it++) if(itemclass(eItem(it)) == IC_TREASURE)
    items[it] = 200;
  }

vector<int> spdist;

int curpart;

/* bool hidebad(cell *c, const transmatrix& V) {
  if(c->wparam != curpart && curpart < 4) return true;
  return false;
  } */

heptspin cth(cellwalker cw) {
  return heptspin(cw.at->master, cw.spin, cw.mirrored);
  }

ld alphaof(hyperpoint h) {
  return atan2(h[1], h[0]);
  }

#define ForInfos for(auto& cci: infos) 

void bantar_frame() {
  
  ForInfos
    cci.second.w = cci.second.c->wall,
    cci.second.it = cci.second.c->item,
    cci.second.mo = cci.second.c->monst,
    cci.second.land = cci.second.c->land,
    cci.second.c->wparam = cci.second.gid;

  calcparam();
  
  vector<unique_ptr<drawqueueitem>> subscr[4];
  
  compute_graphical_distance();

  const int tmax = 2000;
  int t = ticks % (5*tmax);
  int tphase = t / tmax, tsub = t % tmax;
  ld xdst, ydst;

  for(int i=0; i<4; i++) {

    ptds.clear();    

    cellwalker xcw;
    ld part = 1;
    if(i == 1) xcw = bttargets[0];
    else if(i == 2) xcw = bttargets[1], part = .5;
    else if(i == 3) xcw = bttargets[3], part = .5;
    else xcw = cwt;
    
    View = Id;
    
    transmatrix tView = actualV(cth(xcw), Id) * calc_relative_matrix(cwt.at, xcw.at, C0) * inverse(actualV(cth(cwt), Id));
    
    if(tphase < 2) part = 0;
    else if(tphase == 2)
      part = part * tsub / tmax;

    transmatrix itView = inverse(tView);
    transmatrix z = rspintox(itView*C0) * xpush(hdist0(itView*C0) * part) * spintox(itView*C0);
    transmatrix ful = rspintox(itView*C0) * xpush(hdist0(itView*C0) * tmax / tmax) * spintox(itView*C0); // rgpushxto0(itView*C0);
    hyperpoint C1 = xpush0(1);
    ld bof = alphaof(tView * ful * C1);
    z = z * spin(bof * part);
    View = inverse(z);
    
    if(tphase == 0 && tsub > tmax/2) {
      ld alpha = rand() % 10;
      ld d = (rand() % 1000) / 20000. * (tsub-tmax/2) / (tmax/2);
      View = spin(alpha) * xpush(d) * spin(-alpha);
      }
    
    /* int phasemask = 3;
    if(tphase == 0) phasemask = 0;
    if(tphase == 4) phasemask = 2; */

    ForInfos if(cci.second.gid == i)
      cci.second.c->wall = cci.second.w,
      cci.second.c->item = cci.second.it,
      cci.second.c->monst = cci.second.mo,
      cci.second.c->land = cci.second.land;
    else
      cci.second.c->wall = waInvisibleFloor,
      cci.second.c->item = itNone,
      cci.second.c->monst = moNone,
      cci.second.c->land = laNone;
    
    mapeditor::drawplayer = cwt.at->wparam == i;
    
    switch(tphase) {
      case 0:
        xdst = ydst = 0;
        curpart = 4;
        break;
      case 1: 
        xdst = ydst = .5 * tsub / tmax;
        break;
      case 2:
        xdst = ydst = .5;
        break;
      case 3:
        xdst = .5, ydst = .5 * (tmax-tsub) / tmax;
        break;
      case 4:
        xdst = .5, ydst = 0;
        break;
      default:
        xdst = ydst = 0;
      }
    
    /* ld xpos = (!(i&2)) ? xdst : -xdst;
    ld ypos = (!(i&1)) ? ydst : -ydst; */
    
    gmatrix.clear();
    
    drawthemap();
    if(0) for(auto p: parent) if(gmatrix.count(p.first) && gmatrix.count(p.second) && infos[p.first].gid == i && infos[p.second].gid == i)
      queueline(tC0(gmatrix[p.first]), tC0(gmatrix[p.second]), 0xFFFFFFFF, 2);
    subscr[i] = std::move(ptds);
    }
  
  map<int, map<int, vector<unique_ptr<drawqueueitem>>>> xptds;
  for(int i=0; i<4; i++) for(auto& p: subscr[i])
    xptds[int(p->prio)][i].push_back(std::move(p));

  for(auto& sm: xptds) for(auto& sm2: sm.second) {
    int i = sm2.first;
    ptds.clear();
    for(auto& p: sm2.second) ptds.push_back(std::move(p));

    pconf.scale = .5;
    pconf.xposition = (!(i&2)) ? xdst : -xdst;
    pconf.yposition = (!(i&1)) ? ydst : -ydst;
    calcparam();
    drawqueue();
    }

  ForInfos
    cci.second.c->wall = cci.second.w,
    cci.second.c->item = cci.second.it,
    cci.second.c->monst = cci.second.mo,
    cci.second.c->land = cci.second.land;
  
  ptds.clear();
  }

void bantar_anim() {

  vid.aurastr = 0;
  bool breakanim = false;
  int t = SDL_GetTicks();
  drawthemap();
  while(!breakanim) {
    ticks = SDL_GetTicks() - t;
    
    pushScreen(bantar_frame);
    drawscreen();
    popScreen();
    
    SDL_Event ev;
    while(SDL_PollEvent(&ev)) 
      if(ev.type == SDL_KEYDOWN || ev.type == SDL_MOUSEBUTTONDOWN)
        breakanim = true;      
    }
  mapeditor::drawplayer = true;
  pconf.xposition = pconf.yposition = 0;
  pconf.scale = 1;
  }

bool bmap;

void bantar_stats() {
  if(bmap) {
    vid.linewidth *= (inHighQual ? 10 : 2);
    for(auto p: parent) if(gmatrix.count(p.first) && gmatrix.count(p.second))
      queueline(tC0(gmatrix[p.first]), tC0(gmatrix[p.second]), 0x00FF00FF, 4);

    double x = cgi.hexvdist;
    for(auto gm: gmatrix) for(cell *c: {gm.first})
    if(euclid || !pseudohept(c)) for(int t=0; t<c->type; t++) if(infos.count(c) && infos.count(c->move(t)) && c->move(t) && infos[c].gid != infos[c->move(t)].gid)
      if(euclid ? c->move(t)<c : (((t^1)&1) || c->move(t) < c))
        queueline(gm.second * ddspin(c,t,-M_PI/S6) * xpush(x) * C0, 
          gm.second * ddspin(c,t,+M_PI/S6) * xpush(x) * C0, 
          0xFF0000FF, 1);
    vid.linewidth /= (inHighQual ? 10 : 2);
    drawqueue();
    }
  }

void init_bantar() {
  if(!on) {
    stop_game();
    on = true;
    start_game();
    }
  }

void init_bantar_map() {
  bmap = true;
  ForInfos {
    int hsh = 0x202047;
    for(int w: cci.second.way) hsh = (11301 * hsh + w * 37121) & 0x7F7F7F;
    cci.second.c->landparam = hsh;
    cci.second.c->land = laCanvas;
    cci.second.c->wall = waNone;
    cci.second.c->item = itNone;
    cci.second.c->monst = moNone;
    }
  }
  
// see: https://twitter.com/ZenoRogue/status/1001127253747658752
// see also: https://twitter.com/ZenoRogue/status/1000043540985057280 (older version)

void bantar_record() {
  int TSIZE = rug::texturesize; // recommended 2048
  resetbuffer rb;
  renderbuffer rbuf(TSIZE, TSIZE, true);

  int fr = 0;
  
  for(int i=0; i < 10000; i += 33) {
    if(i % 1000 == 999) i++;
    ticks = i;

    rbuf.enable();
    vid.xres = vid.yres = TSIZE;
    banachtarski::bantar_frame();
    
    IMAGESAVE(rbuf.render(), ("bantar/" + format("%05d", fr) + IMAGEEXT).c_str());
    printf("GL %5d/%5d\n", i, 10000);
    fr++;
    }
  
  rb.reset();
  }

int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-bantar_anim")) {
    PHASE(3);
    init_bantar();
    peace::on = true;
    airmap.clear();
    ForInfos if(cci.second.c->monst == moAirElemental)
      cci.second.c->monst = moFireElemental;
    bantar_anim();
    }
  else if(argis("-bantar_test")) {
    PHASE(3);
    init_bantar();
    peace::on = true;
    airmap.clear();
    ForInfos if(cci.second.c->monst == moAirElemental)
      cci.second.c->monst = moFireElemental;
    ForInfos if(cci.second.gid != 2)
      cci.second.c->wall = waInvisibleFloor,
      cci.second.c->item = itNone,
      cci.second.c->monst = moNone,
      cci.second.c->land = laNone;
    airmap.clear();
    havewhat = 0;
    }
  else if(argis("-bantar_map")) {
    init_bantar();
    init_bantar_map();
    }
  else if(argis("-btry")) {
    shift(); notry = argi();
    }
  else if(argis("-bantar_record")) {
    using namespace banachtarski;
    PHASE(3);
    peace::on = true;
    airmap.clear();
    ForInfos if(cci.second.c->monst == moAirElemental)
      cci.second.c->monst = moFireElemental;
    bantar_record();
    }
  else return 1;
  return 0;
  }

auto hook = addHook(hooks_args, 100, readArgs)
  + addHook(hooks_initgame, 100, bantar)
  + addHook(hooks_frame, 100, bantar_stats)
  + addHook_rvslides(140, [] (string s, vector<tour::slide>& v) {
    if(s != "mixed") return;
    using namespace pres;
    v.push_back(
      tour::slide{"Banach-Tarski-like", 62, LEGAL::NONE,
     "Banach-Tarski-like decomposition. Break a hyperbolic plane into two hyperbolic planes.\n\n"
     "Press '5' to show the decomposition. Press any key to stop.\n\n"
     "You will see a map of the decomposition. Press '5' again to return.",
     
    [] (presmode mode) {
      slidecommand = "Banach-Tarski switch";
      slide_url(mode, 't', "Twitter link", "https://twitter.com/ZenoRogue/status/1001127253747658752");

      if(mode == 3) {
        while(gamestack::pushed()) stop_game(), gamestack::pop();
        banachtarski::bmap = false;
        banachtarski::on = false;
        }
      if(mode == 4) {
        if(!banachtarski::on) {
          bool b = mapeditor::drawplayer;
          specialland = cwt.at->land;
          gamestack::push();
          banachtarski::init_bantar();
          airmap.clear();
          dynamicval<int> vs(sightrange_bonus, 3);
          dynamicval<int> vg(genrange_bonus, 3);
          doOvergenerate();
          banachtarski::bantar_anim();
          quitmainloop = false;
          mapeditor::drawplayer = b;
          banachtarski::init_bantar_map();
          resetview();
          }
        else if(banachtarski::on && banachtarski::bmap) {
          banachtarski::bmap = false;
          banachtarski::on = false;
          gamestack::pop();
          }
        }
      }}
      );
    });

}}
