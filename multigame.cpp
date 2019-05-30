// Hyperbolic Rogue

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// gamedata structure, for recording the game data in memory temporarily
// namespace dual (dual mode)

namespace hr {

void gamedata_all(gamedata& gd) {
  gd.index = 0;
  gd.store(firstland);
  gd.store(currentmap);
  gd.store(cwt);
  gd.store(allmaps);
  gd.store(shmup::on);
  gd.store(chaosmode);
  gd.store(*current_display);
  gd.store(cgip);
  gd.store(vid);
  gd.store(sightrange_bonus);
  gd.store(genrange_bonus);
  gd.store(gamerange_bonus);
  gd.store(targets);
  if(GDIM == 3) {
    gd.store(radarlines);
    gd.store(radarpoints);
    }
  if(GOLDBERG) gd.store(gp::param);
  callhooks(hooks_gamedata, &gd);
  }

void gamedata::storegame() {
  geo = geometry;
  var = variation;
  specland = specialland;
  active = game_active;
  record.clear();
  mode = 0;
  gamedata_all(*this);
  game_active = false;
  }

void gamedata::restoregame() {
  geometry = geo;
  variation = var;
  specialland = specland;
  game_active = active;
  mode = 1;
  gamedata_all(*this);
  }

hookset<void(gamedata*)> *hooks_gamedata;

namespace gamestack {

  vector<gamedata> gd;
  
  bool pushed() { return isize(gd); }
  
  void push() {
    gd.emplace_back();
    gd.back().storegame();
    }
    
  void pop() {
    if(!pushed()) return;
    if(game_active) stop_game();
    gd.back().restoregame();
    gd.pop_back();
    }
  
  };

namespace dual {
  int state;

  int currently_loaded;
  int main_side;

  gamedata dgd[2];
  transmatrix player_orientation[2];
  
  void switch_to(int k) {
    if(k != currently_loaded) {
      // gamedata has shmup::on because tutorial needs changing it, but dual should keep it fixed
      dynamicval<bool> smon(shmup::on);
      player_orientation[currently_loaded] = gpushxto0(tC0(cwtV)) * cwtV;
      dgd[currently_loaded].storegame();
      currently_loaded = k;
      dgd[currently_loaded].restoregame();
      }
    }
  
  bool movepc(int d, int subdir, bool checkonly) {
    dynamicval<int> dm(dual::state, 2);
    int cg = currently_loaded;
      
    bool orbusedbak[ittypes];
    for(int i=0; i<ittypes; i++) orbusedbak[i] = orbused[i];

    if(d < 0) {
      if(d == -2 && items[itGreenStone] < 2) {
        switch_to(cg);
        glance_message();
        return false;
        }
      bool ok = true;
  
      for(int k=0; k<2; k++) {
        switch_to(k);
        ok = ok && movepcto(d, subdir, true);
        for(int i=0; i<ittypes; i++) orbused[i] = orbusedbak[i];
        }
      if(ok && checkonly) {
        switch_to(cg);
        return true;
        }
      if(ok) for(int k=0; k<2; k++) {
        switch_to(k);
        movepcto(d, subdir, false);
        }
      switch_to(cg);
      return true;
      }
    
    bool lms[2][5];
    eLastmovetype lmt[2][5];
    for(int k=0; k<2; k++) {
      switch_to(k);
      for(eForcemovetype fm: { fmMove, fmAttack, fmInstant, fmActivate }) {
        forcedmovetype = fm; 
        lms[k][fm] = movepcto(fm == fmMove ? d : 0, subdir, true); 
        lmt[k][fm] = nextmovetype;
        println(hlog, k, int(fm), " -> ", lms[k][fm]);
        forcedmovetype = fmSkip;
        for(int i=0; i<ittypes; i++) orbused[i] = orbusedbak[i];
        }
      }
    
    if(lms[0][fmActivate]) {
      if(checkonly) { switch_to(cg); return true; }
      switch_to(0); forcedmovetype = fmActivate; movepcto(0, subdir, false); forcedmovetype = fmSkip;
      if(!lms[1][fmActivate]) return true;
      }
    if(lms[1][fmActivate]) {
      if(checkonly) { switch_to(cg); return true; }
      switch_to(1); forcedmovetype = fmActivate; movepcto(0, subdir, false); forcedmovetype = fmSkip;
      switch_to(cg);
      return true;
      }
    for(auto fm: {fmMove, fmInstant, fmAttack}) if(lms[0][fm] && lms[1][fm]) {
      if(lmt[0][fm] == lmSkip && lmt[1][fm] == lmSkip)
        continue;
      println(hlog, "apply ", int(fm));
      if(checkonly) { switch_to(cg); return true; }
      int flash = items[itOrbFlash], lgt = items[itOrbLightning];
      switch_to(0); forcedmovetype = fm; movepcto(0, subdir, false); forcedmovetype = fmSkip;
      if(fm == fmInstant) { items[itOrbFlash] = flash, items[itOrbLightning] = lgt; }
      switch_to(1); forcedmovetype = fm; movepcto(0, subdir, false); forcedmovetype = fmSkip;
      switch_to(cg);
      reduceOrbPowers();
      return true;
      }
    addMessage(XLAT("Impossible."));
    flipplayer = false;
    switch_to(cg);
    return false;
    }
  
  void in_subscreen(reaction_t what) {
    dynamicval<ld> xmax(current_display->xmax, 0.5 * (currently_loaded+1));
    dynamicval<ld> xmin(current_display->xmin, 0.5 * (currently_loaded));
    what();
    }
  
  bool split(reaction_t what) {
    if(state != 1) return false;
    state = 2;
    
    for(int a=0; a<2; a++) {
      switch_to(currently_loaded ^ 1);
      what();
      }

    state = 1;
    return true;    
    }
  
  void enable() {
    if(dual::state) return;
    stop_game();
    eGeometry b = geometry;
    eVariation v = variation;
    for(int s=0; s<2; s++) {
      // dynamicval<display_data*> pds(current_display, &subscreens::player_displays[s]);
      if(WDIM == 3) {
        variation = eVariation::pure;
        geometry = s == 0 ? gCubeTiling : gSpace435;
        }
      else if(shmup::on) {
        geometry = b;
        variation = v;
        // 'do what I mean'
        if(euclid)
          geometry = s == 0 ? b : (ginf[geometry].vertex == 3 ? gNormal : g45);
        else
          geometry = s == 0 ? (ginf[geometry].vertex == 3 ? gEuclid : gEuclidSquare) : b;
        if(geometry == gEuclid) variation = eVariation::bitruncated;
        }
      else {
        variation = eVariation::pure;
        geometry = s == 0 ? gEuclidSquare : gArchimedean;
        }
      firstland = specialland = laCrossroads4;
      if(geometry == gArchimedean) 
        arcm::current.parse("4,4,4,4,4");
      check_cgi();
      cgi.require_basics();
      dgd[s].storegame();
      }
    
    currently_loaded = 0;
    dgd[0].restoregame();
    state = 1;
    }
  
  void disable() {
    if(!dual::state) return;
    stop_game();
    state = 0;
    }
  
  int args() {
    using namespace arg;
             
    if(0) ;
    else if(argis("-dual0")) {
      PHASEFROM(2);
      enable();
      switch_to(0);
      }
    else if(argis("-dual1")) {
      PHASEFROM(2);
      enable();
      switch_to(1);
      }
    else if(argis("-dualoff")) {
      PHASEFROM(2);
      disable();
      }
    else return 1;
    return 0;
    }
  
  auto hook = addHook(hooks_args, 100, args);
  
  vector<int> landsides;
  
  bool check_side(eLand l) {
    return landsides[l] == currently_loaded || landsides[l] == 2;
    }

  void assign_landsides() {
    landsides.resize(landtypes);
    int which_hyperbolic = -1;
    if(ginf[dgd[0].geo].cclass == gcHyperbolic && ginf[dgd[1].geo].cclass != gcHyperbolic)
      which_hyperbolic = 0;
    else if(ginf[dgd[1].geo].cclass == gcHyperbolic && ginf[dgd[0].geo].cclass != gcHyperbolic)
      which_hyperbolic = 1;
    int nxt = 0;
    for(int i=0; i<landtypes; i++) {
      eLand l = eLand(i);
      auto& v = landsides[i];
      land_validity_t lv[2];
      for(int s=0; s<2; s++) {
        switch_to(s);
        lv[s] = land_validity(l);
        }
      if(!(lv[0].flags & lv::appears_in_full) && !(lv[1].flags & lv::appears_in_full)) {
        v = -1;
        continue;
        }
      else if(isCrossroads(l)) 
        v = -1; /* simply boring */
      else if(isGravityLand(l)) 
        v = -1; /* too confusing */
      else if(among(l, laTortoise)) 
        v = -1; /* does not work in hyperbolic geos available, and better not do it in Euclidean ones either */
      else if(among(l, laHaunted)) 
        v = -1; /* graveyard prefers Euclidean, while Haunted prefers hyperbolic */
      else if(l == dgd[0].specland && l == dgd[1].specland)
        v = 2;
      else if(l == dgd[0].specland)
        v = 0;
      else if(l == dgd[1].specland)
        v = 1;
      else if(isElemental(l))
        v = 1;
      else if(!(lv[0].flags & lv::appears_in_full))
        v = 1;
      else if(!(lv[1].flags & lv::appears_in_full))
        v = 0;
      else if(lv[0].quality_level > lv[1].quality_level)
        v = 0;
      else if(lv[1].quality_level > lv[0].quality_level)
        v = 1;
      else if(isEquidLand(l) && which_hyperbolic >= 0)
        v = which_hyperbolic;
      else if(among(l, laHunting, laMotion, laCaves, laAlchemist) && which_hyperbolic >= 0)
        v = which_hyperbolic;
      else if(among(l, laMirrorOld, laIce, laJungle, laDesert, laDryForest, laStorms) && which_hyperbolic >= 0)
        v = 1 - which_hyperbolic;
      else if(which_hyperbolic >= 0) 
        v = which_hyperbolic;
      else {      
        println(hlog, "equivalent");
        v = nxt, nxt = 1 - nxt;
        }
      println(hlog, dnameof(l), ": ", lv[0].msg, " vs ", lv[1].msg, " verdict = ", v);
      }
    }

  
  void add_choice() {
    if(!state) return;
    dialog::addSelItem(XLAT("subgame affected"), 
      XLAT(affect_both ? "both" : main_side == 0 ? "left" : "right"), '`');
    dialog::add_action([] () {
      affect_both = !affect_both;
      if(!affect_both) {
        main_side = !main_side;
        switch_to(main_side);
        }
      });
    }
  
  void split_or_do(reaction_t what) {
    if(split(what)) return;
    else what();
    }

  bool may_split(reaction_t what) {
    if(state == 1 && affect_both) {
      split(what);
      return true;
      }
    return false;
    }

  void may_split_or_do(reaction_t what) {
    if(state == 1 && affect_both) {
      split(what);
      }
    else what();
    }
  }

}
