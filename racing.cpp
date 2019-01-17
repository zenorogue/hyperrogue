// Hyperbolic Rogue

// namespaces for complex features (whirlwind, whirlpool, elec, princess, clearing, 
// mirror, hive, heat + livecaves, etc.)

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

namespace racing {

void set_race_configurer();

bool guiding = false;

bool on;
bool player_relative = false;
bool track_ready;

bool official_race = false;

int TWIDTH;

ld race_advance = 0;

static const int LENGTH = 250;
static const int DROP = 1;

int ghosts_to_show = 5;
int ghosts_to_save = 10;

struct race_cellinfo {
  cell *c;
  int from_track;
  int completion;
  int from_start, from_goal;
  };  

vector<race_cellinfo> rti;
vector<cell*> track;
map<cell*, int> rti_id;

int trophy[MAXPLAYER];

string track_code = "OFFICIAL";

transmatrix straight;

int race_try;

void apply_seed() {
  int s = race_try;
  for(char c: track_code) s = 713 * s + c;
  shrand(s);
  }

int race_start_tick, race_finish_tick[MAXPLAYER];

typedef unsigned char uchar;

uchar frac_to_uchar(ld x) { return uchar(x * 256); }
uchar angle_to_uchar(ld x) { return frac_to_uchar(x / 2 / M_PI); }

ld uchar_to_frac(uchar x) { return x / 256.; }
transmatrix spin_uchar(uchar x) { return spin(uchar_to_frac(x) * 2 * M_PI); }

static const ld distance_multiplier = 4;

struct ghostmoment {
  int step, where_id;
  uchar alpha, distance, beta, footphase;
  };

struct ghost {
  charstyle cs;
  int result;
  int checksum;
  long long timestamp;
  vector<ghostmoment> history;
  };

typedef map<eLand, vector<ghost>> raceset;
map<pair<string, int>, raceset> race_ghosts;

map<pair<string, int>, raceset> official_race_ghosts;

raceset& ghostset() { return race_ghosts[make_pair(track_code, modecode())]; }
raceset& oghostset() { return official_race_ghosts[make_pair(track_code, modecode())]; }

int get_score_in_land(eLand l) {
  auto& gh = ghostset();
  if(!gh.count(l)) return 0;
  auto& v = gh[l];
  if(!isize(v)) return 0;
  return v[0].result;
  }

array<vector<ghostmoment>, MAXPLAYER> current_history;

string ghost_prefix = "default";

string ghost_filename(string seed, int mcode) {
  if(ghost_prefix == "default") {
    #ifdef FHS
    if(getenv("HOME")) {
      string s = getenv("HOME");
      mkdir((s + "/.hyperrogue").c_str(), 0755);
      mkdir((s + "/.hyperrogue/racing").c_str(), 0755);
      ghost_prefix = s + "/.hyperrogue/racing/";
      }
    #else
    #if WINDOWS
    mkdir("racing");
    #else
    mkdir("racing", 0755);
    #endif
    ghost_prefix = "racing/";
    #endif
    }
  return ghost_prefix + seed + "-" + itsh(mcode) + ".data";
  }

void hread(hstream& hs, ghostmoment& m) {
  hread(hs, m.step, m.where_id, m.alpha, m.distance, m.beta, m.footphase);
  }

void hwrite(hstream& hs, const ghostmoment& m) {
  hwrite(hs, m.step, m.where_id, m.alpha, m.distance, m.beta, m.footphase);
  }

void hread(hstream& hs, ghost& gh) {
  hread(hs, gh.cs, gh.result, gh.timestamp, gh.checksum, gh.history);
  }

void hwrite(hstream& hs, const ghost& gh) {
  hwrite(hs, gh.cs, gh.result, gh.timestamp, gh.checksum, gh.history);
  }

bool read_ghosts(string seed, int mcode) {

  if(seed == "OFFICIAL" && mcode == 2) {
    fhstream f("officials.data", "rb");
    f.get<int>();
    hread(f, oghostset());
    }
  
  string fname = ghost_filename(seed, mcode);
  println(hlog, "trying to read ghosts from: ", fname);
  fhstream f(fname, "rb");
  if(!f.f) return false;
  int vernum = f.get<int> ();
  if(vernum <= 0xA600) return true; // scores removed due to the possibility of cheating
  hread(f, ghostset());
  return true;
  }

void write_ghosts(string seed, int mcode) {
  fhstream f;
  f.f = fopen(ghost_filename(seed, mcode).c_str(), "wb");
  if(!f.f) throw hstream_exception(); // ("failed to write the ghost file");
  hwrite(f, (const int&) VERNUM_HEX);
  hwrite(f, ghostset());
  }

transmatrix get_ghostmoment(ghostmoment& p) {
  cell *w = rti[p.where_id].c;
  transmatrix T = spin_uchar(p.alpha) * xpush(uchar_to_frac(p.distance) * distance_multiplier) * spin_uchar(p.beta);
  return gmatrix[w] * T;
  } 

void fix_cave(cell *c) {
  int v = 0;
  // if(c->wall == waCavewall) v++;
  // if(c->wall == waCavefloor) v--;
  forCellEx(c2, c) {
    if(c2->wall == waCavewall) v++;
    if(c2->wall == waCavefloor) v--;
    }
  else v--;
  if(v>0 && c->wall == waCavefloor) c->wall = waCavewall;
  if(v<0 && c->wall == waCavewall) c->wall = waCavefloor;
  }

bool bad(cell *c2, cell *c) {
  if(c2->land == laCaves) {
    forCellEx(c3, c2) fix_cave(c3);
    fix_cave(c2);
    }
  if(!passable(c2, c, P_ISPLAYER)) return true;
  if((c2->land == laCrossroads) ^ (c->land == laCrossroads)) return true;
  return false;
  }

int rcelldist(cell *c) {
  if(geometry == gCrystal) return crystal::space_distance(c, currentmap->gamestart());
  else return celldist(c);
  }

int pcelldist(cell *c) {
  if(geometry == gCrystal) return crystal::precise_distance(c, currentmap->gamestart());
  else return celldist(c);
  }

int trackval(cell *c) {
  int v = rcelldist(c);
  int bonus = 0;
  if(c->land != laCrossroads)
  forCellEx(c2, c) {
    int d = rcelldist(c2) - v;
    if(d < 0 && bad(c2, c))
      bonus += 2;
    if(d == 0 && bad(c2, c))
      bonus ++;
    }
  return v + bonus;
  }

void tie_info(cell *c, int from_track, int comp) {
  rti_id[c] = isize(rti);
  rti.emplace_back(race_cellinfo{c, from_track, comp, -1, -1});
  }

race_cellinfo& get_info(cell *c) {
  return rti[rti_id.at(c)];
  }

int race_checksum;

ld start_line_width;

void generate_track() {

  TWIDTH = getDistLimit() - 1;  

  if(ghostset().empty())
    read_ghosts(track_code, modecode());

  track.clear();

  /*
  int t = -1;
  bignum full_id;
  bool onlychild = true;
  */
  
  cell *s = currentmap->gamestart();  

  if(specialland == laCrossroads) {
    celllister cl(s, TWIDTH, 1000000, NULL);
    for(cell *c: cl.lst) c->bardir = NOBARRIERS;
    }                              

  setdist(s, 6, NULL);
  makeEmpty(s);
  
  map<cell*, cell*> parent;
  map<int, vector<cell*> > cellbydist;
  cellbydist[0].push_back(s);
  
  cell *goal;
  
  int traversed = 0;
  
  while(true) {
    traversed++;
    if(cellbydist.empty()) {
      printf("reset after traversing %d\n", traversed);
      race_try++;
      gamegen_failure = true;
      return;
      }
    auto it = cellbydist.end();
    it--;
    // if(hrand(100) < 85 && it != cellbydist.begin()) it--;
    auto& v = it->second;
    if(v.empty()) { cellbydist.erase(it); continue; }
    int id = hrand(isize(v));
    cell *c = v[id];
    v[id] = v.back(); v.pop_back();    
    if(it->first >= LENGTH) {
      goal = c;
      break;
      }
    setdist(c, 4, parent[c]);
    forCellEx(c1, c) if(!bad(c1, c) && !parent.count(c1)) {
      parent[c1] = c;
      cellbydist[trackval(c1)].push_back(c1);
      }
    }

  track = build_shortest_path(s, goal);
  
  
  /*
  transmatrix At = spin(hrand(1000));
  
  track.push_back(s);
  
  while(isize(track) < LENGTH) {
    At = At * xpush(.1);
    cell *sb = s;
    virtualRebase(s, At, true);
    fixmatrix(At);
    if(s != sb) track.push_back(s);
    }
  */
  
  /*

  cellwalker ycw(s, hrand(s->type));
  
  track.push_back(s);

  for(int i=0; i<LENGTH; i++) {

    if(i == 0) {
      t = type_in(expansion, s, celldist);
      bignum b = expansion.get_descendants(LENGTH, t);
      full_id = hrand(b);
      }
            
    if(i == 1) 
      onlychild = true;

    if(!onlychild) ycw++;
    if(VALENCE == 3) ycw++;

    onlychild = false;
          
    for(int tch: expansion.children[t]) {
      ycw++;
      if(i == 1)
        tch = type_in(expansion, ycw.cpeek(), celldist);
      auto& sub_id = expansion.get_descendants(LENGTH-i-1, tch);
      if(full_id < sub_id) { t = tch; break; }
      
      full_id.addmul(sub_id, -1);
      onlychild = true;
      }

    ycw += wstep;
    track.push_back(ycw.at);
    }

  for(int a = 0; a < 2; a++)
  for(int i=0; i<isize(track)-1; i++) {
    cell *cx = track[min(i + 10, isize(track)-1)];
    ld err = HUGE_VAL;
    forCellEx(c2, track[i]) {
      ld dist = hdist0(calc_relative_matrix(cx, c2, C0) * C0);
      if(dist < err) err = dist, track[i+1] = c2;
      }
    } */
    
  // for(cell *c:track) if(c->land != laMirror) c->bardir = NOBARRIERS;
  for(cell *c:track) setdist(c, 0, NULL);
  
  if(1) {
    manual_celllister cl;
  
    for(int i=0; i<isize(track); i++) {
      tie_info(track[i], 0, i);
      cl.add(track[i]);
      }
    
    int win = isize(track) - DROP;
    
    for(int i=0; i<isize(cl.lst); i++) {
      cell *c = cl.lst[i];
      auto p = get_info(c);
      forCellEx(c2, c) if(!rti_id.count(c2)) {
        tie_info(c2, p.from_track+1, p.completion);
        cl.add(c2);
        }
      c->item = itNone;
      if(c->wall == waMirror || c->wall == waCloud) c->wall = waNone;
      if(!isIvy(c))
        c->monst = moNone;
      if(c->monst == moIvyHead) c->monst = moIvyWait;
      if(inmirror(c->land))
        ;      
      else if(p.from_track == TWIDTH) {
        killMonster(c, moNone, 0);
        c->wall = waBarrier;
        c->land = laBarrier;
        }
      else if(p.from_track > TWIDTH) {
        killMonster(c, moNone, 0);
        c->land = laMemory;
        c->wall = waChasm;
        }
      if(p.completion >= win && p.from_track < TWIDTH) {
        c->wall = hrand(2) ? waMirror : waCloud;
        killMonster(c, moNone, 0);
        }
      }
    }
  
  int byat[256];
  for(int a=0; a<16; a++) byat[a] = 0;
  for(const auto s: rti) byat[s.from_track]++;
  for(int a=0; a<16; a++) printf("%d: %d\n", a, byat[a]);
  
  if(s->land == laCaves) {
    set<unsigned> hash;
    while(true) {
      unsigned hashval = 7;
      int id = 0;
      for(auto s: rti) {
        fix_cave(s.c);
        if(s.c->wall == waCavewall) 
          hashval = (3+2*(id++)) * hashval + 1;
        if(s.c->wall == waCavefloor) 
          hashval = (3+2*(id++)) * hashval + 2;
        }
      printf("hashval = %x id = %d\n", hashval, id);
      if(hash.count(hashval)) break;
      hash.insert(hashval);
      }
    }
  
  for(cell *sc: track) {
    straight = calc_relative_matrix(sc, track[0], C0);
    if(straight[2][2] > 1e8) break;
    }
  straight = rspintox(straight * C0);
  
  ld& a = start_line_width;
  for(a=0; a<10; a += .1) {
    hyperpoint h = straight * parabolic1(a) * C0;
    cell *at = s;
    virtualRebase(at, h,  true);
    if(!rti_id.count(at) || get_info(at).from_track >= TWIDTH) break;
    }
  
  for(ld cleaner=0; cleaner<a*.75; cleaner += .2) for(int dir=-1; dir<=1; dir+=2) {
    transmatrix T = straight * parabolic1(cleaner * dir);
    cell *at = s;
    virtualRebase(at, T,  true);
    get_info(at).from_start = 0;
    for(ld u=0; u<50; u++) {
      if(at->wall != waBarrier)
        makeEmpty(at);
      killMonster(at, moNone, 0);
      T = T * xpush(.1);
      virtualRebase(at, T,  true);      
      }
    }

  for(auto s: rti) if(s.c->monst == moIvyDead) s.c->monst = moNone;
  
  for(int i=0; i<motypes; i++) kills[i] = 0;
  
  vector<transmatrix> forbidden;
  for(auto& ghost: ghostset()[specialland])
    forbidden.push_back(get_ghostmoment(ghost.history[0]));
  for(auto& ghost: oghostset()[specialland])
    forbidden.push_back(get_ghostmoment(ghost.history[0]));

  for(int i=0; i<multi::players; i++) trophy[i] = 0;

  for(int i=0; i<multi::players; i++) {
    auto who = shmup::pc[i];
    // this is intentionally not hrand
    
    for(int j=0; j<100; j++) {
      who->at = straight * parabolic1(start_line_width * (rand() % 20000 - 10000) / 40000) * spin(rand() % 360);
      who->base = s;
      bool ok = true;
      for(const transmatrix& t: forbidden) if(hdist(t*C0, who->at * C0) < 10. / (j+10)) ok = false;
      if(ok) break;
      }
    virtualRebase(who, true);
    }

  if(1) {
    manual_celllister cl;
    cl.add(s);
    bool goal = false;
    for(int i=0; i<isize(cl.lst); i++) {
      cell *c = cl.lst[i];
      forCellEx(c2, c) {
        if(among(c2->wall, waCloud, waMirror)) goal = true;
        if(passable(c2, c, P_ISPLAYER))
          cl.add(c2);
        }
      }
    if(!goal) {
      printf("error: goal unreachable\n");
      gamegen_failure = true;
      race_try++;
      return;    
      }
    }
  
  if(1) {
    map<cell*, pair<int, int> > cdists;
    manual_celllister cl;
    cl.add(s);
    for(auto cc: rti) if(cc.from_start == 0) cl.add(cc.c);

    for(int i=0; i<isize(cl.lst); i++) {
      cell *c = cl.lst[i];
      forCellEx(c2, c) {
        if(passable(c2, c, P_ISPLAYER) && !cl.listed(c2))
          cl.add(c2), get_info(c2).from_start = get_info(c).from_start + 1;
        }
      }
    
    }

  if(1) {
    map<cell*, pair<int, int> > cdists;
    manual_celllister cl;
    for(auto cc: rti) if(among(cc.c->wall, waCloud, waMirror))
      cc.from_goal = 0, cl.add(cc.c);

    for(int i=0; i<isize(cl.lst); i++) {
      cell *c = cl.lst[i];
      forCellEx(c2, c) {
        if(passable(c2, c, P_ISPLAYER) && !cl.listed(c2))
          cl.add(c2), get_info(c2).from_goal = get_info(c).from_goal + 1;
        }
      }
    
    }
  
  int total_track = get_info(s).from_goal;
  
  auto blockoff = [&total_track] (race_cellinfo& cc) {
    if(cc.from_start < 1 || cc.from_goal < 1) return false;
    int dif = cc.from_start + cc.from_goal - 2 * cc.from_track - total_track;
    return dif > 3;
    };

  auto blockbound = [&blockoff] (cell *c) {
    forCellEx(c2, c) if(passable(c2, c, P_ISPLAYER) && !blockoff(get_info(c2))) return true;
    return false;
    };
  
  vector<cell*> to_block;
  
  for(auto cc: rti) if(blockoff(cc)) to_block.push_back(cc.c);
  
  hrandom_shuffle(&to_block[0], isize(to_block));
  
  for(cell *c: to_block) switch(specialland) {
    case laIce:
      c->wall = waIcewall;
      break;
    
    case laHell:
      c->wall = waSulphur;
      break;
    
    case laJungle: {
      vector<int> dirs;
      forCellIdEx(c2, i, c) if(among(c2->monst, moIvyRoot, moIvyWait)) dirs.push_back(i);
      if(dirs.empty()) c->monst = moIvyRoot;
      else c->monst = moIvyWait, c->mondir = dirs[hrand(isize(dirs))];
      break;
      }
    
    case laDeadCaves: 
      if(blockbound(c)) c->wall = waDeadwall;
      break;
    
    case laRedRock:
      if(blockbound(c)) c->wall = waRed3;
      break;
        
    case laDragon:
      c->wall = waChasm;
      break;

    case laDryForest:
      if(blockbound(c)) c->wall = waBigTree;
      break;
    
    case laDesert:
      if(blockbound(c)) c->wall = waDune;
      break;

    case laRuins:
      if(blockbound(c)) c->wall = waRuinWall;
      break;

    case laElementalWall:
      if(blockbound(c)) {
        if(c->land == laEFire) c->wall = waEternalFire;
        else if(c->land == laEWater) c->wall = waSea;
        else if(c->land == laEAir) c->wall = waChasm;
        else if(c->land == laEEarth) c->wall = waStone;
        }
      break;    
  
    default: break;
    }

  // for(cell *c: to_block) if(blockbound(c)) c->land = laOvergrown;

  /*
  for(cell *c: track) {
    int i = trackval(c) - celldist(c);
    if(i == 0) c->item = itDiamond;
    if(i == 1) c->item = itGold;
    if(i == 2) c->item = itEmerald;
    if(i == 3) c->item = itSapphire;
    if(i == 4) c->item = itRuby;
    if(i >= 5) c->item = itBone;
    }
  */

  track_ready = true;
  race_checksum = hrand(1000000);
  
  race_start_tick = 0;
  for(int i=0; i<MAXPLAYER; i++) race_finish_tick[i] = 0;

  official_race = (track_code == "OFFICIAL" && modecode() == 2);
  if(official_race && race_checksum != oghostset() [specialland] [0].checksum) {
    official_race = false;
    addMessage(XLAT("Race did not generate correctly for some reason -- not ranked"));
    }
  }

bool inrec = false;

ld race_angle = 90;

int current_player;

void set_view() {

  if(race_start_tick == 0) race_start_tick = ticks + 5000;

  if(subscreen_split(set_view)) return;

  shmup::monster *who = shmup::pc[current_player];
  
  if(!inrec) {
    const transmatrix T = who->at;
    ld alpha = -atan2(T * C0);
    ld distance = hdist0(T * C0);
    ld beta = -atan2(xpush(-distance) * spin(-alpha) * T * Cx1);
    current_history[current_player].emplace_back(ghostmoment{ticks - race_start_tick, rti_id[who->base], 
      angle_to_uchar(alpha),
      frac_to_uchar(distance / distance_multiplier),
      angle_to_uchar(beta),
      frac_to_uchar(who->footphase)
      });
    }

  transmatrix at = ypush(-vid.yshift) * ggmatrix(who->base) * who->at;
  
  if(racing::player_relative || quotient)
    View = spin(race_angle * degree) * inverse(at) * View;
  else {
    int z = get_info(who->base).completion;
    int steps = euclid ? 1000 : 20;
    cell *c1 = racing::track[max(z-steps, 0)];
    cell *c2 = racing::track[min(z+steps, isize(racing::track)-1)];
    transmatrix T1 = ypush(-vid.yshift) * ggmatrix(c1);
    transmatrix T2 = ypush(-vid.yshift) * ggmatrix(c2);
    transmatrix T = spintox(inverse(T1) * T2 * C0);
    hyperpoint h = T * inverse(T1) * at * C0;
    ld y = asin_auto(h[1]);
    ld x = asin_auto(h[0] / cos_auto(y));
    x += race_advance;
    // printf("%d %lf\n", z, x);
    transmatrix Z = T1 * inverse(T) * xpush(x);
    View = spin(race_angle * degree) * inverse(Z) * View;
    }
  }

#if CAP_COMMANDLINE  
void show();

int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-racing")) {
    PHASEFROM(2);
    stop_game();
    switch_game_mode(rg::racing);
    }
  else return 1;
  return 0;
  }
#endif

int tstart, tstop;
heptspin sview;

/*
void restore_time(int t) {
  tuple<int, cell*, transmatrix, ld> sf = make_tuple(t, nullptr, Id, 0);
  auto it = lower_bound(history.begin(), history.end(), sf, [] (auto a, auto b) { return get<0>(a) < get<0>(b); });
  auto& m = shmup::pc[0];
  tie(t, m->base, m->at, m->footphase) = *it;
  shmup::pc[0]->pat = ggmatrix(shmup::pc[0]->base) * shmup::pc[0]->at;
  }
*/

/*
bool akh(int sym, int uni) {
  if(uni == '1') { tstart = ticks; sview = viewctr; }
  else if(uni == '2') { tstop = ticks; }
  else if(uni == '3') { conformal::model_orientation = 90; pmodel = mdBand; player_relative = false; }
  else if(uni == '4') { conformal::model_orientation = 180; pmodel = mdHalfplane; conformal::halfplane_scale = 2; player_relative = false; }
  else if(uni == '5') { pmodel = mdDisk; player_relative = true; }
  else if(uni == '6') { vid.use_smart_range = true; vid.smart_range_detail = 2; }
  else if(uni == '7' && tstart && tstop) {
    viewctr = sview;
    nohud = true;
    restore_time(tstart);
    drawthemap();
    centerpc(0);
    int t = tstart;
    int i = 0;
    inrec = true;
    while(t < tstop) {
      ticks = t;
      char buf[1000];
      restore_time(t);
      shmup::fixStorage();
      centerpc(0);
      optimizeview();
      cwt.at = shmup::pc[0]->base;
      drawthemap();
      fullcenter();
      cmode = sm::NORMAL;
      drawthemap();
      centerpc(0);
      optimizeview();
      snprintf(buf, 1000, "animations/race/race%d-%03d.png", int(pmodel), i++);
      saveHighQualityShot(buf);
      t += 40;
      }
    inrec = false;
    }
  else return false;
  return true;
  }
*/

#if CAP_COMMANDLINE
auto hook = 
  addHook(hooks_args, 100, readArgs)
+ addHook(clearmemory, 0, []() {
    track_ready = false;
    track.clear();
    rti.clear();
    rti_id.clear();
    for(auto &ch: current_history) ch.clear();
    })
// + addHook(hooks_handleKey, 120, akh);
  ;
#endif

vector<eLand> race_lands = {
  laHunting,
  laCrossroads,
  laJungle,
  laDesert, 
  laRedRock,
  laDragon,
  laMirror,
  laRuins,
  laCaves,
  laWildWest,
  laIce, 
  laHell,  
  laTerracotta,
  laElementalWall,
  laDryForest,
  laDeadCaves,
  };

vector<string> playercmds_race = {
  "forward", "backward", "turn left", "turn right",
  "forward", "backward", "turn left", "turn right",
  "", "", "",
  "", "change camera", "", ""
  };

string racetimeformat(int t) {
  string times = "";
  int digits = 0;
  bool minus = (t < 0);
  if(t < 0) t = -t;
  while(t || digits < 6) {
    int mby = (digits == 4 ? 6 : 10);
    times = char('0'+(t%mby)) + times;
    t /= mby; digits++;
    if(digits == 3) times = "." + times;
    if(digits == 5) times = ":" + times;
    }
  if(minus) times = "-" + times;
  return times;
  }

extern int playercfg;

void track_chooser(string new_track) {
  cmode = 0;
  gamescreen(2);
  dialog::init(XLAT("Racing"));
  
  map<char, eLand> landmap;
  
  dynamicval<bool> so(shmup::on, true);
  dynamicval<bool> ro(racing::on, true);

  char let = 'a';
  for(eLand l: race_lands) {
    auto& ghs = race_ghosts[make_pair(new_track, modecode())];
    const int LOST = 3600000;
    int best = LOST;
    if(ghs.count(l)) {
      auto& gh = ghs[l];
      for(auto& gc: gh) best = min(best, gc.result);
      }
    string s = (best == LOST) ? "" : racetimeformat(best);
    landmap[let] = l;
    dialog::addSelItem(XLAT1(linf[l].name), s, let++);
    dialog::add_action([l, new_track] () {
      stop_game();
      multi::players = playercfg;
      if(!racing::on) switch_game_mode(rg::racing);
      track_code = new_track;
      specialland = l;
      start_game();
      popScreenAll();
      });
    }
  
  dialog::addBack();
  dialog::display();
  
  if(landmap.count(getcstat) && new_track == "OFFICIAL" && modecode() == 2)
    displayScore(landmap[getcstat]);  
  }

void race_projection() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(1);
  
  dialog::init(XLAT("racing projections"));

  dialog::addBoolItem(XLAT("Poincaré disk model"), pmodel == mdDisk && !vid.camera_angle, '1');
  dialog::add_action([] () {
    pmodel = mdDisk;
    race_advance = 0;
    vid.yshift = 0;
    vid.camera_angle = 0;
    vid.xposition = 0;
    vid.yposition = 0;
    vid.scale = 1;
    vid.use_smart_range = 0;
    vid.smart_range_detail = 3;
    });
  
  dialog::addBoolItem(XLAT("band"), pmodel == mdBand, '2');
  dialog::add_action([] () {
    pmodel = mdBand;
    conformal::model_orientation = race_angle;
    race_advance = 1;
    vid.yshift = 0;
    vid.camera_angle = 0;
    vid.xposition = 0;
    vid.yposition = 0;
    vid.scale = 1;
    vid.use_smart_range = 1;
    vid.smart_range_detail = 3;
    });
  
  dialog::addBoolItem(XLAT("half-plane"), pmodel == mdHalfplane, '3');
  dialog::add_action([] () {
    pmodel = mdHalfplane;
    conformal::model_orientation = race_angle + 90;
    race_advance = 0.5;
    vid.yshift = 0;
    vid.camera_angle = 0;
    vid.xposition = 0;
    vid.yposition = 0;
    vid.scale = 1;
    vid.use_smart_range = 1;
    vid.smart_range_detail = 3;
    });

  dialog::addBoolItem(XLAT("third-person perspective"), pmodel == mdDisk && vid.camera_angle, '4');
  dialog::add_action([] () {
    pmodel = mdDisk;
    race_advance = 0;
    vid.yshift = -0.3;
    vid.camera_angle = -45;
    vid.scale = 18/16. * vid.xres / vid.yres / multi::players;
    vid.xposition = 0;
    vid.yposition = -0.9;
    vid.use_smart_range = 1;
    vid.smart_range_detail = 3;
    });
  
  if(pmodel == mdDisk) {    
    dialog::addSelItem(XLAT("point of view"), player_relative ? "player" : "track", 'p');
    if(quotient)
      dialog::lastItem().value = XLAT("N/A");
    dialog::add_action([] () { 
      player_relative = !player_relative; 
      if(pmodel == mdBand || pmodel == mdHalfplane)
        pmodel = mdDisk;
      set_view();
      });
    }
  else dialog::addBreak(100);
        
  dialog::addSelItem(XLAT("race angle"), fts(race_angle), 'a');
  dialog::add_action([] () { 
    dialog::editNumber(race_angle, 0, 360, 15, 0, XLAT("race angle"), "");
    int q = conformal::model_orientation - race_angle;
    dialog::reaction = [q] () { conformal::model_orientation = race_angle + q; };
    });

  dialog::addSelItem(XLAT("show more in front"), fts(race_advance), 'A');
  dialog::add_action([] () {
    dialog::editNumber(race_advance, 0, 360, 0.1, 1, XLAT("show more in front"), "");
    });

  dialog::addBack();
  dialog::display();
  }

  int playercfg;
  bool editing_track;
  string new_track;
  
/* struct race_configurer { */

  void set_race_configurer() { editing_track = false; new_track = track_code; playercfg = multi::players; }
  
  static string random_track_name() {
    string s = "";
    for(int a = 0; a < 4; a++)  {
      int u = rand() % 2;
      if(u == 0)
        s += "AEIOUY" [ rand() % 6];
      s += "BCDFGHJKLMNPRSTVWZ" [ rand() % 18];
      if(u == 1)
        s += "AEIOUY" [ rand() % 6];
      }
    return s;
    }
  
  static string racecheck(int sym, int uni) {
    if(uni >= 'A' && uni <= 'Z') return string("") + char(uni);
    if(uni >= 'a' && uni <= 'z') return string("") + char(uni - 32);
    return "";
    }

  void raceconfigurer() {
  
    gamescreen(1);
  
    dialog::init(XLAT("Racing"));
  
    if(bounded)
      dialog::addInfo(XLAT("Racing available only in unbounded worlds."), 0xFF0000);
    else {
      dialog::addItem(XLAT("select the track and start!"), 's');
      dialog::add_action([/*this*/] () { 
        dynamicval<bool> so(shmup::on, true);
        dynamicval<bool> ro(racing::on, true);
        if(race_ghosts[make_pair(new_track, modecode())].empty())
          read_ghosts(new_track, modecode());
        else
          println(hlog, "known ghosts: ", isize(race_ghosts[make_pair(new_track, modecode())]));
        pushScreen([/*this*/] () { track_chooser(new_track); }); 
        });
      }
    
    dialog::addBreak(100);

    dialog::addItem(XLAT("configure the projection"), 'p');
    dialog::add_action([] () { pushScreen(race_projection); });
  
    dialog::addBoolItem(XLAT("guiding line"), guiding, 'g');
    dialog::add_action([] () { guiding = !guiding; });
  
    dialog::addItem(shmup::player_count_name(playercfg), 'n');
    dialog::add_action([/*this*/] () { 
      playercfg = playercfg == 1 ? 2 : 1;
      });
  
    dialog::addItem(XLAT("configure player 1"), '1');
    dialog::add_action([] () {
      pushScreen(shmup::key_configurer(1, playercmds_race));
      });
  
    if(playercfg >= 2) {
      dialog::addItem(XLAT("configure player 2"), '2');
      dialog::add_action([] () {
        pushScreen(shmup::key_configurer(2, playercmds_race));
        });
      }
    else dialog::addBreak(100);
    
    dialog::addBreak(100);

    dialog::addSelItem("track seed", editing_track ? dialog::view_edited_string() : new_track, '/');
    dialog::add_action([/*this*/] () { 
      editing_track = !editing_track;
      if(editing_track) dialog::start_editing(new_track);
      });
    dialog::addItem("play the official seed", 'o');
    dialog::add_action([/*this*/] () { new_track = "OFFICIAL"; });
    dialog::addItem("play a random seed", 'r');
    dialog::add_action([/*this*/] () { new_track = random_track_name(); });    

    dialog::addBreak(100);
    
    dialog::addSelItem(XLAT("best scores to show as ghosts"), its(ghosts_to_show), 'g');
    dialog::add_action([]() { dialog::editNumber(ghosts_to_show, 0, 100, 1, 5, "best scores to show as ghosts", ""); });

    dialog::addSelItem(XLAT("best scores to save"), its(ghosts_to_save), 'b');
    dialog::add_action([]() { dialog::editNumber(ghosts_to_save, 0, 100, 1, 10, "best scores to save", ""); });
    
    
    if(racing::on) {
      dialog::addItem(XLAT("disable the racing mode"), 'x');
      dialog::add_action([] () { 
        stop_game();
        switch_game_mode(rg::racing);
        race_try = 0;
        start_game();
        });
      }

    dialog::addBack();
    dialog::display();

    keyhandler = [/*this*/] (int sym, int uni) {
      if(editing_track) {
        if(sym == SDLK_RETURN) sym = uni = '/';
        if(dialog::handle_edit_string(sym, uni, racecheck)) return;
        }
      dialog::handleNavigation(sym, uni);
      if(doexiton(sym, uni)) { if(editing_track) editing_track = false; else popScreen(); }
      };
    
    }
/*  }; */

void configure_race() { 
  set_race_configurer();

  pushScreen(raceconfigurer);
  }

auto hooks1 = 
  addHook(hooks_o_key, 90, [] { 
    if(racing::on) return named_dialog(XLAT("racing menu"), raceconfigurer);
    else return named_functionality();
    });

vector<display_data> player_displays;
bool in_subscreen;

void prepare_subscreens() {
  int N = multi::players;
  if(N > 1) {
    player_displays.resize(N, *current_display);
    int qrows[10] = {1, 1, 1, 1, 2, 2, 2, 3, 3, 3};
    int rows = qrows[N];
    int cols = (N + rows - 1) / rows;
    for(int i=0; i<N; i++) {
      auto& pd = player_displays[i];
      pd.xmin = (i % cols) * 1. / cols;
      pd.xmax = ((i % cols) + 1.) / cols;
      pd.ymin = (i / cols) * 1. / rows;
      pd.ymax = ((i / cols) + 1.) / rows;
      }
    }
  else {
    player_displays.clear();
    }
  }

map<string, map<eLand, int> > scoreboard;

void uploadScore() {
  int tscore = 0;
  for(eLand l: race_lands) {
    int i = get_score_in_land(l);
    if(!i) continue;
    int score = 60000000 / i; // 1000 points for minute, 2000 points for 30 sec
    tscore += score;
    }
    
  achievement_score(LB_RACING, tscore);
  }

void displayScore(eLand l) {
  int vf = min((vid.yres-64) / 70, vid.xres/80);
  int x = vid.xres / 4;
  
  set_priority_board(LB_RACING);
  
  if(get_sync_status() == 1) {
    displayfr(x, 56, 1, vf, "(syncing)", 0xC0C0C0, 0);
    }
  else {
    vector<pair<int, string> > scores;
    for(auto p: scoreboard) if(p.second.count(l)) scores.emplace_back(p.second[l], p.first);
    sort(scores.begin(), scores.end());
    int i = 0;
    for(auto& sc: scores) {
      int i0 = 56 + (i++) * vf;
      displayfr(x, i0, 1, vf, racetimeformat(sc.first), 0xC0C0C0, 16);
      displayfr(x+8, i0, 1, vf, sc.second, 0xC0C0C0, 0);
      }
    }
  }

void race_won() {
  if(!race_finish_tick[current_player]) {
    int result = ticks - race_start_tick;
    int losers = 0;
    int place = 1;
    for(int i=0; i<multi::players; i++) if(race_finish_tick[i]) place++; else losers = 0;
    for(auto& ghost: ghostset()[specialland]) if(ghost.result < result) place++; else losers++;
    for(auto& ghost: oghostset()[specialland]) if(ghost.result < result) place++; else losers++;

    if(place == 1 && losers) trophy[current_player] = 0xFFD500FF;
    if(place == 2) trophy[current_player] = 0xFFFFC0FF;
    if(place == 3) trophy[current_player] = 0x967444FF;
    
    if(place == 1 && losers && official_race)
      achievement_gain("RACEWON", rg::racing);
    
    race_finish_tick[current_player] = ticks;
    charstyle gcs = getcs();
    for(color_t *x: {&gcs.skincolor, &gcs.haircolor, &gcs.dresscolor, &gcs.swordcolor, &gcs.dresscolor2}) {
      for(int a=1; a<4; a++)
        part(*x, a) = (part(*x, a) + part(gcs.uicolor, a)) / 2;
      part(*x, 0) >>= 2;
      }
      
    auto &subtrack = ghostset() [specialland];

    int ngh = 0;
    for(int i=0; i<isize(subtrack); i++) {
      if(subtrack[i].checksum != race_checksum) {
        println(hlog, format("wrong checksum: %x, should be %x", subtrack[i].checksum, race_checksum));
        }
      else {
        if(i != ngh)
          subtrack[ngh] = move(subtrack[i]);
        ngh++;
        }
      }
    subtrack.resize(ngh);    

    subtrack.emplace_back(ghost{gcs, result, race_checksum, time(NULL), current_history[current_player]});
    sort(subtrack.begin(), subtrack.end(), [] (const ghost &g1, const ghost &g2) { return g1.result < g2.result; });
    if(isize(subtrack) > ghosts_to_save && ghosts_to_save > 0) 
      subtrack.resize(ghosts_to_save);
    if(ghosts_to_save > 0)
      write_ghosts(track_code, modecode());
      
    if(official_race) uploadScore();
    }
  }

void draw_ghost(ghost& ghost) {
  auto p = std::find_if(ghost.history.begin(), ghost.history.end(), [] (const ghostmoment gm) { return gm.step > ticks - race_start_tick;} );
  if(p == ghost.history.end()) p--, p->footphase = 0;
  cell *w = rti[p->where_id].c;
  if(!gmatrix.count(w)) return;
  dynamicval<charstyle> x(getcs(), ghost.cs);
  
  if(ghost.cs.charid == -1) {
    dynamicval<bool> pc(peace::on, true);
    drawMonsterType(eMonster(ghost.cs.uicolor), w, get_ghostmoment(*p), ghost.cs.dresscolor, uchar_to_frac(p->footphase));
    return;
    }

  drawMonsterType(moPlayer, w, get_ghostmoment(*p), 0, uchar_to_frac(p->footphase));
  }

void markers() {
  if(!racing::on) return;
  if(guiding) for(int i=0; i<multi::players; i++) {
    shmup::monster *m = shmup::pc[i];
    if(!m) continue;
    for(int j=0; j<5; j++)
      queueline(m->pat * xpush0(j), m->pat * xpush0(j+1), multi::scs[i].uicolor, 2);
    }
  if(racing::player_relative) {
    using namespace racing;
    cell *goal = NULL;
    for(cell *c: track) if(inscreenrange(c)) goal = c;
    hyperpoint H = tC0(ggmatrix(goal));
    if(invalid_point(H)) return;
    queuechr(H, 2*vid.fsize, 'X', 0x10100 * int(128 + 100 * sintick(150)));
    queuestr(H, vid.fsize, 
      (geometry == gCrystal && !crystal::pure()) ? fts(crystal::space_distance(cwt.at, track.back())) :
      its(celldistance(cwt.at, track.back())), 0x10101 * int(128 - 100 * sintick(150)));
    addauraspecial(H, 0xFFD500, 0);
    }
  int ghosts_left = ghosts_to_show;
  for(auto& ghost: ghostset()[specialland]) {
    if(!ghosts_left) break;
    ghosts_left--;
    draw_ghost(ghost);
    }
  
  for(auto& ghost: oghostset()[specialland])
    draw_ghost(ghost);
  
  if(gmatrix.count(track[0])) {
    for(ld z=-start_line_width; z<=start_line_width; z+=0.1) 
      curvepoint(ggmatrix(track[0]) * straight * parabolic1(z) * C0);
    queuecurve(0xFFFFFFFF, 0, PPR::BFLOOR);
    }
  }

int get_percentage(int i) {
  return min(get_info(shmup::pc[i]->base).completion * 100 / (isize(track) - DROP), 100);
  }
  
void add_debug(cell *c) { 
  if(racing::on && racing::rti_id[c]) {
    auto& r = racing::get_info(c);
    dialog::addSelItem("from_track", its(r.from_track), 0);
    dialog::addSelItem("from_start", its(r.from_start), 0);
    dialog::addSelItem("from_goal", its(r.from_goal), 0);
    dialog::addSelItem("completion", its(r.completion), 0);
    }
  }

}

bool subscreen_split(reaction_t what) {
  using namespace racing;
  if(in_subscreen) return false;
  if(!player_displays.empty()) {
    in_subscreen = true;
    int& p = current_player;
    for(p = 0; p < multi::players; p++) {
      dynamicval<display_data*> c(current_display, &player_displays[p]);
      what();
      }
    in_subscreen = false;
    return true;
    }
  return false;
  }

}
