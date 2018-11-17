// Hyperbolic Rogue

// namespaces for complex features (whirlwind, whirlpool, elec, princess, clearing, 
// mirror, hive, heat + livecaves, etc.)

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

namespace racing {

bool on;
bool player_relative = false;

static const int LENGTH = 250;
static const int TWIDTH = 6;

vector<cell*> track;
map<cell*, pair<int, int> > trackstage;

int trackval(cell *c) {
  int v = celldist(c);
  int bonus = 0;
  forCellEx(c2, c) {
    int d = celldist(c2) - v;
    if(d < 0 && !passable(c2, c, P_ISPLAYER))
      bonus += 2;
    if(d == 0 && !passable(c2, c, P_ISPLAYER))
      bonus ++;
    }
  return v + bonus;
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

  while(true) {
    if(cellbydist.empty()) {
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
    forCellEx(c1, c) if(passable(c, c1, P_ISPLAYER) && !parent.count(c1)) {
      parent[c1] = c;
      cellbydist[trackval(c1)].push_back(c1);
      }
    }

  while(goal != s) {
    track.push_back(goal);
    forCellEx(c2, goal) if(celldist(c2) < celldist(goal)) { goal = c2; break; }
    }

  track.push_back(s);
  reverse(track.begin(), track.end());
  
  
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
  
  manual_celllister cl;

  for(int i=0; i<isize(track); i++) {    
    trackstage[track[i]] = {0, i};
    cl.add(track[i]);
    }
  
  int win = isize(track) - 10;
  
  for(int i=0; i<isize(cl.lst); i++) {
    cell *c = cl.lst[i];
    auto p = trackstage[c];
    forCellEx(c2, c) if(!trackstage.count(c2)) {
      trackstage[c2] = {p.first+1, p.second};
      cl.add(c2);
      }
    c->item = itNone;
    if(c->wall == waMirror || c->wall == waCloud) c->wall = waNone;
    if(!isIvy(c))
      c->monst = moNone;
    if(inmirror(c->land))
      ;      
    else if(p.first == TWIDTH)
      c->wall = waBarrier,
      c->land = laBarrier;
    else if(p.first > TWIDTH)
      c->land = laMemory,
      c->wall = waChasm;
    if(p.second >= win && p.first < TWIDTH) c->wall = hrand(2) ? waMirror : waCloud;
    }
  
  int byat[256];
  for(int a=0; a<16; a++) byat[a] = 0;
  for(auto s: trackstage) byat[s.second.first]++;
  for(int a=0; a<16; a++) printf("%d: %d\n", a, byat[a]);
  
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
  }

vector<tuple<int, cell*, transmatrix, ld> > history;

bool inrec = false;

ld race_angle = 90;

int current_player;

void set_view() {

  if(subscreen_split(set_view)) return;

  shmup::monster *who = shmup::pc[current_player];
  
  safety = true;
  printf("%d\n", ticks);
  if(!inrec) history.emplace_back(ticks, who->base, who->at, who->footphase);

  transmatrix at = ggmatrix(who->base) * who->at;
  
  if(racing::player_relative)
    View = spin(race_angle * degree) * inverse(at) * View;
  else {
    int z = racing::trackstage[who->base].second;
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
    tactic::on = true;
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
// + addHook(hooks_handleKey, 120, akh);
  ;
#endif

vector<eLand> race_lands = {
  laIce, laDesert, 
  
  laCrossroads, /* need editing */
  laCaves,      /* need fixing */
  laJungle,     /* need pacifying Ivy */
  laMirror,
  laHell,
  laDryForest,
  laDeadCaves,
  laRedRock,
  laElementalWall,
  laWildWest,
  laDragon,
  laHunting,
  laTerracotta, /* disable traps and warriors */
  laRuins,
  };

void show() {
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

  dialog::addSelItem(XLAT("race angle"), fts(race_angle), 'm');
  dialog::add_action([] () { 
    dialog::editNumber(race_angle, 0, 360, 15, 0, XLAT("spiral angle"), "");
    int q = conformal::model_orientation - race_angle;
    dialog::reaction = [q] () { conformal::model_orientation = race_angle + q; };
    });

  dialog::addBack();
  dialog::display();
  
  }

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
