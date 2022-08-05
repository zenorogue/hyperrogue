// Hyperbolic Rogue -- multi-game features
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file multi.cpp
 *  \brief running several games at once -- used in the Tutorial and Dual Geometry mode
 */

#include "hyper.h"
namespace hr {

#if HDR
/** gamedata structure, for recording the game data in memory temporarily */
struct gamedata {
  /** important parameters should be visible */
  eGeometry geo;
  eVariation var;
  eLand specland;
  bool active;
  /** other properties are recorded here */
  vector<char> record;
  int index, mode;
  void storegame();
  void restoregame();
  template<class T> void store(T& x) {
    int ssize = sizeof(x);
    if(ssize & 7) ssize = (ssize | 7) + 1;
    if(mode == 0) {
      record.resize(index+ssize);
      ::new (&record[index]) T(std::move(x));
      }
    else {
      T& at = (T&) record[index];
      x = std::move(at);
      at.~T();
      }
    index += ssize;
    }
  template<class T> void store_ptr(T& x) {
    if(mode == 0) {
      T* copy = new T(std::move(x));
      store(copy);
      }
    else {
      T* copy = nullptr;
      store(copy);
      x = std::move(*copy);
      delete copy;
      }
    }
  };
#endif

void gamedata_all(gamedata& gd) {
  gd.index = 0;
  gd.store(firstland);
  gd.store(currentmap);
  gd.store(cwt);
  gd.store(allmaps);
  gd.store(shmup::on);
  gd.store(land_structure);
  gd.store(*current_display);
  gd.store(cgip);
  if(gd.mode == 0) cgip->use_count++;
  if(gd.mode != 0) cgip->use_count--;
  gd.store(hybrid::underlying);
  gd.store(hybrid::csteps);
  if(hybri && hybrid::underlying_cgip) {
    if(gd.mode == 0) hybrid::underlying_cgip->use_count++;
    if(gd.mode != 0) hybrid::underlying_cgip->use_count--;
    }
  gd.store(hybrid::underlying_cgip);
  gd.store_ptr(vid.projection_config);
  gd.store_ptr(vid.rug_config);
  gd.store(vid.yshift);
  gd.store(vid.plevel_factor);
  gd.store(vid.binary_width);
  gd.store(sightrange_bonus);
  gd.store(genrange_bonus);
  gd.store(gamerange_bonus);
  gd.store(targets);
  gd.store(patterns::rwalls);
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

EX hookset<void(gamedata*)> hooks_gamedata;

EX namespace gamestack {

  vector<gamedata> gd;
  
  EX bool pushed() { return isize(gd); }
  
  EX void push() {
    gd.emplace_back();
    gd.back().storegame();
    }
    
  EX void pop() {
    if(!pushed()) return;
    if(game_active) stop_game();
    gd.back().restoregame();
    gd.pop_back();
    }
  
EX }

EX namespace dual {
  /** 0 = dualmode off, 1 = in dualmode (no game chosen), 2 = in dualmode (working on one of subgames) */
  EX int state;

  /** exactly one side is Euclidean -- it should not be synchronized with the other side */
  EX bool one_euclidean;
  
  EX int currently_loaded;
  EX int main_side;
  EX bool affect_both;

  gamedata dgd[2];
  EX transmatrix player_orientation[2];
  
  hyperpoint which_dir;
  
  EX purehookset hooks_after_move;
  
  int remap_direction(int d, int cg) {
    if(WDIM == 2 || cg == currently_loaded) return d;
    
    hyperpoint h = sword::dir[0].T * which_dir;

    h = hpxy3(h[0]/10, h[1]/10, h[2]/10);
    ld b = HUGE_VAL;
    for(int i=0; i<S7; i++) {
      hyperpoint checked = tC0(currentmap->relative_matrix(cwt.at->cmove(i)->master, cwt.at->master, C0));
      ld dist = hdist(checked, h);
      if(dist < b) { b = dist; d = i; }
      }
    d = gmod(d - cwt.spin, S7);

    return d;
    }
  
  EX transmatrix get_orientation() {
    if(WDIM == 2)
      return gpushxto0(tC0(cwtV.T)) * cwtV.T;      
    else if(cwt.at) {
      transmatrix T = unshift(ggmatrix(cwt.at));
      return gpushxto0(tC0(T)) * T * sword::dir[0].T;
      }
    else
      return Id;
    }
  
  EX void switch_to(int k) {
    if(k != currently_loaded) {
      // gamedata has shmup::on because tutorial needs changing it, but dual should keep it fixed
      dynamicval<bool> smon(shmup::on);
      player_orientation[currently_loaded] = get_orientation();
      dgd[currently_loaded].storegame();
      currently_loaded = k;
      dgd[currently_loaded].restoregame();
      }
    }
  
  EX bool movepc(int d, int subdir, bool checkonly) {
    dynamicval<int> dm(dual::state, 2);
    int cg = currently_loaded;
      
    auto orbusedbak = orbused;

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
        orbused = orbusedbak;
        }
      if(ok && checkonly) {
        switch_to(cg);
        return true;
        }
      if(ok) for(int k=0; k<2; k++) {
        switch_to(k);
        movepcto(d, subdir, false);
        if(k == 0) turncount--;
        }
      if(!ok) {
        addMessage(XLAT("Impossible."));
        }
      switch_to(cg);
      return ok;
      }
    
    which_dir = inverse(sword::dir[0].T) * tC0(currentmap->relative_matrix((cwt+d).cpeek()->master, cwt.at->master, C0));
    
    bool lms[2][5];
    eLastmovetype lmt[2][5];
    for(int k=0; k<2; k++) {
      switch_to(k);
      for(eForcemovetype fm: { fmMove, fmAttack, fmInstant, fmActivate }) {
        forcedmovetype = fm; 
        lms[k][fm] = movepcto(fm == fmMove ? remap_direction(d, cg) : 0, subdir, true); 
        lmt[k][fm] = nextmovetype;
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
      if(checkonly) { switch_to(cg); return true; }
      int flash = items[itOrbFlash], lgt = items[itOrbLightning];
      switch_to(0); forcedmovetype = fm; movepcto(0, subdir, false); forcedmovetype = fmSkip;
      if(fm == fmInstant) { items[itOrbFlash] = flash, items[itOrbLightning] = lgt; }
      turncount--;
      switch_to(1); forcedmovetype = fm; movepcto(0, subdir, false); forcedmovetype = fmSkip;
      switch_to(cg);
      reduceOrbPowers();
      callhooks(hooks_after_move);
      return true;
      }
    addMessage(XLAT("Impossible."));
    flipplayer = false;
    switch_to(cg);
    return false;
    }
  
  EX void in_subscreen(reaction_t what) {
    dynamicval<ld> xmax(current_display->xmax, 0.5 * (currently_loaded+1));
    dynamicval<ld> xmin(current_display->xmin, 0.5 * (currently_loaded));
    what();
    }
  
  EX bool split(reaction_t what) {
    if(state != 1) return false;
    state = 2;
    
    for(int a=0; a<2; a++) {
      switch_to(currently_loaded ^ 1);
      what();
      }

    state = 1;
    return true;    
    }
  
  EX void enable() {
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
        #if CAP_ARCM
        geometry = s == 0 ? gEuclidSquare : gArchimedean;
        #else
        geometry = gEuclidSquare;
        #endif
        }
      firstland = specialland = laCrossroads4;
      #if CAP_ARCM
      if(geometry == gArchimedean) 
        arcm::current.parse("4,4,4,4,4");
      #endif
      check_cgi();
      cgi.require_basics();
      dgd[s].storegame();
      }
    
    currently_loaded = 0;
    dgd[0].restoregame();
    state = 1;
    }
  
  EX void disable() {
    if(!dual::state) return;
    stop_game();
    state = 0;
    }
  
  #if CAP_COMMANDLINE
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
  #endif
  
  vector<int> landsides;
  
  EX bool check_side(eLand l) {
    return landsides[l] == currently_loaded || landsides[l] == 2;
    }

  EX void assign_landsides() {
    switch_to(!currently_loaded);
    one_euclidean = euclid;
    switch_to(!currently_loaded);
    one_euclidean ^= euclid;
    
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
      else if(l == laPower)
        v = which_hyperbolic;
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
      // println(hlog, dnameof(l), ": ", lv[0].msg, " vs ", lv[1].msg, " verdict = ", v);
      }
    }

  
  EX void add_choice() {
    if(!state) return;
    dialog::addSelItem(XLAT("subgame affected"), 
      affect_both ? XLAT("both") : main_side == 0 ? XLAT("left") : XLAT("right"), '`');
    dialog::add_action([] () {
      affect_both = !affect_both;
      if(!affect_both) {
        main_side = !main_side;
        switch_to(main_side);
        }
      });
    }
  
  EX void split_or_do(reaction_t what) {
    if(split(what)) return;
    else what();
    }

  EX bool may_split(reaction_t what) {
    if(state == 1 && affect_both) {
      split(what);
      return true;
      }
    return false;
    }

  EX void may_split_or_do(reaction_t what) {
    if(state == 1 && affect_both) {
      split(what);
      }
    else what();
    }
  }

  #if HDR
  inline reaction_t mayboth(reaction_t what) { return [=] { may_split_or_do(what); }; }
  #endif


}
