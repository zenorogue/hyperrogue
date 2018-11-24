// Hyperbolic Rogue

// namespaces for complex features (whirlwind, whirlpool, elec, princess, clearing, 
// mirror, hive, heat + livecaves, etc.)

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

namespace racing {

bool on;
bool player_relative = false;
bool track_ready;

static const int LENGTH = 50;
static const int TWIDTH = 6;
static const int DROP = 1;

struct race_cellinfo {
  cell *c;
  int from_track;
  int completion;
  };  

vector<race_cellinfo> rti;
vector<cell*> track;
map<cell*, int> rti_id;

string track_code = "OFFICIAL";

void apply_seed() {
  int s = 0;
  for(char c: track_code) s = 713 * s + c;
  shrand(s);
  }

int race_start_tick, race_finish_tick[MAXPLAYER];

struct ghostmoment {
  int step;
  int where_id;
  transmatrix T;
  ld footphase;
  };

struct ghost {
  charstyle cs;
  int result;
  vector<ghostmoment> history;
  };

map<pair<string, int>, map<eLand, vector<ghost> > > race_ghosts;

array<vector<ghostmoment>, MAXPLAYER> current_history;

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

int trackval(cell *c) {
  int v = celldist(c);
  int bonus = 0;
  if(c->land != laCrossroads)
  forCellEx(c2, c) {
    int d = celldist(c2) - v;
    if(d < 0 && bad(c2, c))
      bonus += 2;
    if(d == 0 && bad(c2, c))
      bonus ++;
    }
  return v + bonus;
  }

void tie_info(cell *c, int from_track, int comp) {
  rti_id[c] = isize(rti);
  rti.emplace_back(race_cellinfo{c, from_track, comp});
  }

race_cellinfo& get_info(cell *c) {
  return rti[rti_id.at(c)];
  }

void generate_track() {

  track.clear();

  /*
  int t = -1;
  bignum full_id;
  bool onlychild = true;
  */
  
  cell *s = currentmap->gamestart();  
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
      stop_game();
      start_game();
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

  if(euclid) {
    using namespace hyperpoint_vec;
    track.push_back(s);
    hyperpoint h = tC0(calc_relative_matrix(goal, s, C0)) - C0;
    cell *x = s;
    hyperpoint h1 = C0; 
    for(int i=0; i<=1000; i++) {
      h1 += h / 1000.;
      virtualRebase(x, h1,  true);
      if(x != track.back()) track.push_back(x);
      }
    }
  
  else {
    while(goal != s) {
      track.push_back(goal);
      forCellEx(c2, goal) if(celldist(c2) < celldist(goal)) { goal = c2; break; }
      }
  
    track.push_back(s);
    reverse(track.begin(), track.end());
    }
  
  
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
  
  for(int i=0; i<motypes; i++) kills[i] = 0;
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
  if(1) {
    manual_celllister cl;
    cl.add(s);
    bool goal = false;
    for(int i=0; i<isize(cl.lst); i++) {
      cell *c = cl.lst[i];
      forCellEx(c2, c) {
        if(among(c2->wall, waCloud, waMirror)) goal = true;
        if(c2->wall == waIcewall || passable(c2, c, P_ISPLAYER))
          cl.add(c2);
        }
      }
    if(!goal) {
      printf("error: goal unreachable\n");
      stop_game();
      start_game();
      return;    
      }
    }
  
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
  
  race_start_tick = 0;
  for(int i=0; i<MAXPLAYER; i++) race_finish_tick[i] = 0;
  }

bool inrec = false;

ld race_angle = 90;

int current_player;

void set_view() {

  if(race_start_tick == 0) race_start_tick = ticks + 5000;

  if(subscreen_split(set_view)) return;

  shmup::monster *who = shmup::pc[current_player];
  
  if(!inrec) current_history[current_player].emplace_back(ghostmoment{ticks - race_start_tick, rti_id[who->base], who->at, who->footphase});

  transmatrix at = ggmatrix(who->base) * who->at;
  
  if(racing::player_relative)
    View = spin(race_angle * degree) * inverse(at) * View;
  else {
    int z = get_info(who->base).completion;
    int steps = euclid ? 1000 : 20;
    cell *c1 = racing::track[max(z-steps, 0)];
    cell *c2 = racing::track[min(z+steps, isize(racing::track)-1)];
    transmatrix T1 = ggmatrix(c1);
    transmatrix T2 = ggmatrix(c2);
    transmatrix T = spintox(inverse(T1) * T2 * C0);
    hyperpoint h = T * inverse(T1) * at * C0;
    ld y = asin_auto(h[1]);
    ld x = asin_auto(h[0] / cos_auto(y));
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
    shmup::on = true;
    racing::on = true;
    timerghost = false;
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
  laIce, 
  laDesert, 
  
  laCrossroads,
  laCaves,
  laJungle,
  laMirror,
  laHell,
  laDryForest,
  laDeadCaves,
  laRedRock,
  laElementalWall,
  laWildWest,
  laDragon,
  laTerracotta,
  laRuins,
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
    int mby = (digits == 5 ? 6 : 10);
    times = char('0'+(t%mby)) + times;
    t /= mby; digits++;
    if(digits == 3) times = "." + times;
    if(digits == 5) times = ":" + times;
    }
  if(minus) times = "-" + times;
  return times;
  }

void track_chooser() {
  dialog::init(XLAT("Racing"));

  char let = 'a';
  for(eLand l: race_lands) {
    auto& gh = race_ghosts[{track_code, modecode()}] [l];
    const int LOST = 3600000;
    int best = LOST;
    for(auto& gc: gh) best = min(best, gc.result);
    string s = (best == LOST) ? "" : racetimeformat(best);
    dialog::addSelItem(XLAT1(linf[l].name), s, let++);
    dialog::add_action([l] () {
      stop_game();
      specialland = l;
      racing::on = true;
      shmup::on = true;
      start_game();
      popScreenAll();
      });
    }
  
  dialog::addBack();
  dialog::display();
  }

struct race_configurer {

  int playercfg;
  
  race_configurer() { playercfg = multi::players; }
  
  void operator() () {
  
    gamescreen(1);
  
    dialog::init(XLAT("Racing"));
  
    dialog::addBoolItem(XLAT("player relative"), player_relative, 'r');
    dialog::add_action([] () { 
      player_relative = !player_relative; 
      if(pmodel == mdBand || pmodel == mdHalfplane)
        pmodel = mdDisk;
      });
  
    dialog::addSelItem(XLAT("projection"), conformal::get_model_name(pmodel), 'm');
    dialog::add_action([] () { 
      switch(pmodel) {
        case mdDisk:
          pmodel = mdBand;
          conformal::model_orientation = race_angle;
          break;
        case mdBand:
          pmodel = mdHalfplane;
          conformal::model_orientation = race_angle + 90;
          break;
        default:
          pmodel = mdDisk;
        }
      });
  
    dialog::addSelItem(XLAT("race angle"), fts(race_angle), 'a');
    dialog::add_action([] () { 
      dialog::editNumber(race_angle, 0, 360, 15, 0, XLAT("spiral angle"), "");
      int q = conformal::model_orientation - race_angle;
      dialog::reaction = [q] () { conformal::model_orientation = race_angle + q; };
      });
  
    dialog::addItem(shmup::player_count_name(playercfg), 'n');
    dialog::add_action([this] () { 
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
  
    dialog::addItem(XLAT("select the track and start!"), 's');
    dialog::add_action([] () { pushScreen(track_chooser); });

    dialog::addBack();
    dialog::display();
    
    }
  };

auto hooks1 = 
  addHook(hooks_o_key, 90, [] { 
    if(racing::on) return named_dialog("race mode", race_configurer());
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

void race_won() {
  if(!race_finish_tick[current_player]) {
    race_finish_tick[current_player] = ticks;
    charstyle gcs = getcs();
    for(color_t *x: {&gcs.skincolor, &gcs.haircolor, &gcs.dresscolor, &gcs.swordcolor, &gcs.dresscolor2})
      part(*x, 0) >>= 1;

    race_ghosts[{track_code, modecode()}] [specialland].emplace_back(ghost{gcs, ticks - race_start_tick, current_history[current_player]});
    }
  }

void markers() {
  if(racing::player_relative) {
    using namespace racing;
    cell *goal = NULL;
    for(cell *c: track) if(inscreenrange(c)) goal = c;
    hyperpoint H = tC0(ggmatrix(goal));
    queuechr(H, 2*vid.fsize, 'X', 0x10100 * int(128 + 100 * sintick(150)));
    queuestr(H, vid.fsize, its(celldistance(cwt.at, track.back())), 0x10101 * int(128 - 100 * sintick(150)));
    addauraspecial(H, 0x10100, 0);
    }
  for(auto& ghost: race_ghosts[{track_code, modecode()}][specialland]) {
    auto p = std::find_if(ghost.history.begin(), ghost.history.end(), [] (const ghostmoment gm) { return gm.step > ticks - race_start_tick;} );
    if(p == ghost.history.end()) p--;
    cell *w = rti[p->where_id].c;
    if(!gmatrix.count(w)) continue;
    dynamicval<charstyle> x(getcs(), ghost.cs);
    drawMonsterType(moPlayer, w, gmatrix[w] * p->T, 0, p->footphase);
    }
  }

int get_percentage(int i) {
  return min(get_info(shmup::pc[i]->base).completion * 100 / (isize(track) - DROP), 100);
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
