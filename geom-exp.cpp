// Hyperbolic Rogue -- the 'experiments with geometry' menu
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

// -- geometry menu --

namespace hr {

int eupage = 0;
int euperpage = 21;

string euchelp =
  "If you want to know how much the gameplay is affected by the "
  "hyperbolic geometry in HyperRogue, this mode is for you!\n\n"
  
  "You can try many different geometries here. We start by gluing "
  "n-gons in such a way that k of them meet in every vertex. "
  "Depending on n and k, this either folds into a sphere, unfolds into a plane, "
  "or requires a hyperbolic space. The result may be then 'bitrunc' by "
  "replacing each vertex by a 2k-gon. Furthermore, you can play "
  "with quotient geometries. For example, the elliptic geometry is "
  "obtained from the sphere by making the antipodes be the same point, "
  "so you return to the same spot (but as a mirror image) after going there. "
  "Have fun experimenting! "
  "Achievements and leaderboards do not work in geometry experiments, "
  "except some specific ones.\n\n"
  "In standard geometry (bitrunc or not), you can play the full game, but in other geometries "
  "you select a particular land. Lands are unlocked by visiting them in this "
  "session, or permanently by collecting 25 treasure. Try Crossroads in Euclidean "
  "or chaos mode in non-standard non-quotient hyperbolic to visit many lands. "
  "Highlights:\n"
  "* Crystal World and Warped Coast can be understood as extra geometries.\n"
  "* Halloween is specially designed for spherical geometry.\n"
  "* To see the difference, try Hunting Grounds in Euclidean -- it is impossible.\n";

int ewhichscreen = 2;

void showQuotientConfig() {
  using namespace fieldpattern;
  gamescreen(2);
  dialog::init(XLAT("advanced configuration"));
  fgeomextra& gxcur = fgeomextras[current_extra];
  for(int i=0; i<isize(fgeomextras); i++) {
    auto& g = fgeomextras[i];
    dialog::addBoolItem(XLAT(ginf[g.base].name), g.base == gxcur.base, 'a'+i);
    }
  nextPrimes(gxcur);
  for(int i=0; i<isize(gxcur.primes); i++) {
    auto& p = gxcur.primes[i];
    dialog::addBoolItem(XLAT("order %1%2 (non-bitruncated cells: %3)", its(p.p), p.squared ? "²" : "", its(p.cells)), i == gxcur.current_prime_id, 'A'+i);
    }
  
  if(isize(gxcur.primes) < 6) {
    dialog::addBreak(100);
    dialog::addHelp(
      "This geometry is obtained by applying the same 'generators' which "
      "lead to creating the given basic hyperbolic geometry, "
      "but using a fixed finite field instead of the field of reals. "
      "It can be also interpreted as a quotient of the given basic geometry. "
      "Warning: field patterns based on large primes might generate for a long time."
      );
    dialog::addBreak(100);
    }

  dialog::addItem("find the next prime", 'p');
  dialog::addItem("activate", 'x');
  dialog::addItem("default", 'c');

  keyhandler = [&gxcur] (int sym, int uni) {
    if(uni >= 'a' && uni < 'a' + isize(fgeomextras))
      current_extra = uni - 'a';
    else if(uni >= 'A' && uni < 'A' + isize(gxcur.primes))
      gxcur.current_prime_id = uni - 'A';
    else if(uni == 'p')
      nextPrime(gxcur);
    else if(uni == 'x' || uni == '\n') {
      set_geometry(gxcur.base);
      enableFieldChange();
      set_geometry(gFieldQuotient);
      start_game();
      }
    else if(uni == 'c') {
      set_geometry(gEuclid);
      fieldpattern::quotient_field_changed = false;
      set_geometry(gFieldQuotient);
      start_game();
      }
    else if(doexiton(sym, uni))
      popScreen();
    };
  
  dialog::display();
  }

bool torus_bitrunc;

void prepare_torusconfig() {
  torusconfig::newdy = torusconfig::dy;
  torusconfig::newqty = torusconfig::qty;
  torusconfig::newsdx = torusconfig::sdx;
  torusconfig::newsdy = torusconfig::sdy;
  torusconfig::newmode = torusconfig::torus_mode;
  torus_bitrunc = PURE;
  }

void showTorusConfig() {
  cmode = sm::SIDE;
  if(euclid) cmode |= sm::TORUSCONFIG;
  gamescreen(2);
  
  dialog::init(XLAT("advanced configuration"));
  
  auto& mode = torusconfig::tmodes[torusconfig::newmode];
  
  for(int i=0; i<isize(torusconfig::tmodes); i++) {
    char let = "0123456789!@#" [i];
    dialog::addBoolItem(torusconfig::tmodes[i].name, torusconfig::newmode == i, let);
    dialog::add_action([i] () { torusconfig::newmode = torusconfig::eTorusMode(i); });
    }
  
  bool single = (mode.flags & torusconfig::TF_SINGLE);
  bool square = (mode.flags & torusconfig::TF_SQUARE);
  bool simple = (mode.flags & torusconfig::TF_SIMPLE);
  bool cyl = (mode.flags & torusconfig::TF_CYL);
  bool klein  = (mode.flags & torusconfig::TF_KLEIN);
  
  if(single) {
    dialog::addSelItem(XLAT("number of cells (n)"), its(torusconfig::newqty), 'n');
    if(mode.flags & torusconfig::TF_HEX)
      dialog::addSelItem(XLAT("cell bottom-right from 0 (d)"), its(torusconfig::newdy), 'd');
    else
      dialog::addSelItem(XLAT("cell below 0 (d)"), its(torusconfig::newdy), 'd');
    }
  else if(cyl) {
    dialog::addSelItem(XLAT("period (x)"), its(torusconfig::newsdx), 'x');
    dialog::addSelItem(XLAT("period (y)"), its(torusconfig::newsdy), 'y');
    }
  else {
    if(torusconfig::newsdx < 1) torusconfig::newsdx = 1;
    if(torusconfig::newsdy < 1) torusconfig::newsdy = 1;
    dialog::addSelItem(XLAT("width (x)"), its(torusconfig::newsdx), 'x');
    dialog::addSelItem(XLAT("height (y)"), its(torusconfig::newsdy), 'y');
    }

  if(square) dialog::addBoolItem(XLAT("bitruncated"), !torus_bitrunc, 't');
  else dialog::addInfo("", 100);
  
  int valid = 2;

  int adx = torusconfig::newsdx, ady = torusconfig::newsdy;
  if(single) {  
    if(square) {
      dialog::addInfo("this mode has bad patterns", 0x808080), valid = 1;
      if(!torus_bitrunc && valid == 1)
        dialog::addInfo("incompatible with bitruncating", 0x808080), valid = 0;
      }
    else {
      if(torusconfig::newqty % 3)
        dialog::addInfo(XLAT("best if %1 is divisible by %2", "n", "3"), 0x808080), valid = 1;
      if((torusconfig::newdy + 999999) % 3 != 2)
        dialog::addInfo(XLAT("best if %1 is divisible by %2", "d+1", "3"), 0x808080), valid = 1;
      }
    }
  else if(cyl) {
    if(torusconfig::sdx == 0 && torusconfig::sdy == 0) 
      dialog::addInfo(XLAT("period cannot be 0"), 0x800000), valid = 0;
    else if(square) {
      if(torusconfig::newsdy & 1)
        dialog::addInfo(XLAT("best if %1 is divisible by %2", "y", "2"), 0x808080), valid = 1;
      if(torusconfig::newsdx & 1)
        dialog::addInfo(XLAT("best if %1 is divisible by %2", "x", "2"), 0x808080), valid = 1;
      if(!torus_bitrunc && valid == 1)
        dialog::addInfo("incompatible with bitruncating", 0x808080), valid = 0;      
      if(klein && abs(adx) != abs(ady) && adx != 0 && ady != 0)
        dialog::addInfo("Möbius band requires a symmetric period", 0x800000), valid = 0;

      if(klein && ady)
        dialog::addInfo("not implemented", 0x800000), valid = 0;
      }
    else {
      if(torusconfig::newsdy % 3)
        dialog::addInfo(XLAT("best if %1 is divisible by %2", "y", "3"), 0x808080), valid = 1;
      if(torusconfig::newsdx % 3)
        dialog::addInfo(XLAT("best if %1 is divisible by %2", "x", "3"), 0x808080), valid = 1;
      if(klein && adx != 0 && ady != 0 && adx != -ady)
        dialog::addInfo("Möbius band requires a symmetric period", 0x800000), valid = 0;
      if(klein)
        dialog::addInfo("not implemented", 0x800000), valid = 0;
      }
    }
  else {
    if(square) {
      if(torusconfig::newsdx & 1)
        dialog::addInfo(XLAT("best if %1 is divisible by %2", "x", "2"), 0x808080), valid = 1;
      if(torusconfig::newsdy & 1)
        dialog::addInfo(XLAT("best if %1 is divisible by %2", "y", "2"), 0x808080), valid = 1;
      if(!torus_bitrunc && valid == 1)
        dialog::addInfo("incompatible with bitruncating", 0x808080), valid = 0;
      }
    else if(simple) {
      if(torusconfig::newsdx % 3)
        dialog::addInfo(XLAT("best if %1 is divisible by %2", "x", "3"), 0x808080), valid = 1;
      if(torusconfig::newsdy % 3)
        dialog::addInfo(XLAT("best if %1 is divisible by %2", "y", "3"), 0x808080), valid = 1;
      }
    else {
      if(torusconfig::newsdx & 1)
        dialog::addInfo(XLAT("best if %1 is divisible by %2", "x", "3"), 0x808080), valid = 1;
      if(torusconfig::newsdy & 1)
        dialog::addInfo(XLAT("best if %1 is divisible by %2", "y", "2"), 0x808080), valid = 0;
      }
    }
  
  dialog::addSelItem(XLAT("scale factor"), fts(vid.scale), 'z');

#if CAP_RUG
  dialog::addBoolItem(XLAT("hypersian rug mode"), (rug::rugged), 'u');
#endif

  dialog::addItem("activate", 'a');
  dialog::addItem("default", 'c');

  keyhandler = [=] (int sym, int uni) {
    if(uni == 'n' && single)
      dialog::editNumber(torusconfig::newqty, 0, 1000, 3, torusconfig::def_qty, XLAT("number of cells (n)"), "");
    else if(uni == 'd' && single)
      dialog::editNumber(torusconfig::newdy, -1000, 1000, 3, -torusconfig::def_dy, XLAT("cell below 0 (d)"), "");
    else if(uni == 'x' && !single)
      dialog::editNumber(torusconfig::newsdx, 0, 1000, square ? 2 : 3, 12, XLAT("width (x)"), "");
    else if(uni == 'y' && !single)
      dialog::editNumber(torusconfig::newsdy, 0, 1000, square ? 2 : simple ? 3 : 2, 12, XLAT("height (y)"), "");
    else if(uni == 't')
      torus_bitrunc = !torus_bitrunc;
    else if((uni == 'a' || uni == '\n') && torusconfig::newqty >= 3 && valid) {
      set_geometry(gNormal);
      torusconfig::torus_mode = torusconfig::newmode;
      torusconfig::qty = torusconfig::newqty;
      torusconfig::dy = torusconfig::newdy;
      torusconfig::sdx = torusconfig::newsdx;
      torusconfig::sdy = torusconfig::newsdy;
      torusconfig::activate();
      set_geometry(gTorus);
      set_variation((torus_bitrunc || !square) ? eVariation::bitruncated : eVariation::pure);
      start_game();
      }
    else if(uni == 'c') {
      set_geometry(gEuclid);
      torusconfig::torus_mode = torusconfig::tmSingle;
      torusconfig::qty = torusconfig::def_qty;
      torusconfig::dy = torusconfig::def_dy;
      set_geometry(gTorus);
      start_game();
      }
    else if(uni == 'z') editScale();
#if CAP_RUG
    else if(uni == 'u') rug::select();
#endif
    else if(doexiton(sym, uni))
      popScreen();
    };
  
  dialog::display();
  }

string bitruncnames[5] = {" (b)", " (n)", " (g)", " (i)", " (d)"};

void validity_info() {
  int vccolors[4] = {0xFF0000, 0xFF8000, 0xFFFF00, 0x00FF00};
  auto lv = land_validity(specialland);
  if(lv.flags & lv::display_error_message)
    dialog::addInfo(XLAT(lv.msg), vccolors[lv.quality_level]);
  else
    dialog::addBreak(100);
  }

bool showquotients;

void showEuclideanMenu() {
  cmode = sm::SIDE;
  gamescreen(0);  
  if(cheater) for(int i=0; i<landtypes; i++) landvisited[i] = true;
  for(int i=0; i<landtypes; i++)
    if(hiitemsMax(treasureType(eLand(i))) >= 25) landvisited[i] = true;
  landvisited[laCrossroads] = true;
  landvisited[laCrossroads4] = true;
  landvisited[laIce] = true;
  landvisited[laHunting] = true;
  landvisited[laMirrorOld] = true;
  landvisited[laPrincessQuest] = cheater || princess::everSaved;
  landvisited[laWildWest] = true;
  landvisited[laHalloween] = true;
  landvisited[laWarpCoast] = true;
  landvisited[laGraveyard] = true;
  landvisited[laDual] = true;
  landvisited[laDocks] |= landvisited[laWarpCoast];
  landvisited[laSnakeNest] |= landvisited[laRedRock];
  landvisited[laCA] = true;
  // for(int i=2; i<lt; i++) landvisited[i] = true;

  string validclasses[4] = {" (X)", " (½)", "", " (!)"};
  
  if(ewhichscreen == 2) {
    dialog::init(XLAT("experiment with geometry"));
    int ts = ginf[geometry].sides;
    int tv = ginf[geometry].vertex;
    int tq = ginf[geometry].quotientstyle;
    int nom = (BITRUNCATED ? tv+ts : tv) * ((tq & qNONORIENTABLE) ? 2 : 4);
    int denom = (2*ts + 2*tv - ts * tv);
    
    if(GOLDBERG) {
      denom *= 2;
      nom = nom / tv * (2*tv + ts * (gp::area-1));
      if(nom % 2 == 0) nom /= 2, denom /= 2;
      }
    
    dialog::addSelItem(XLAT("land"), XLAT1(linf[specialland].name), '5');
    dialog::addBreak(50);

    char letter = 'a';
    for(int i=0; i<gGUARD; i++) {
      bool on = geometry == i;
      dynamicval<eGeometry> cg(geometry, eGeometry(i));
      if(!!(quotient || elliptic || euwrap) != showquotients) continue;
      dialog::addBoolItem(XLAT(ginf[i].name), on, letter++);
      dialog::lastItem().value += validclasses[land_validity(specialland).quality_level];
      dialog::add_action([i] {
        eGeometry targetgeometry = eGeometry(i);
        if(targetgeometry == gArchimedean)
          pushScreen(arcm::show);
        else {
          set_geometry(targetgeometry);
          start_game();
          }
        });
      }
    
    dialog::addBreak(50);
    dialog::addBoolItem(XLAT("show quotient spaces"), showquotients, 'z');
    dialog::addBreak(50);
  
    if(ts == 6 && tv == 3)
      dialog::addSelItem(XLAT("variations"), XLAT("does not matter"), 'v');
    else if(binarytiling)
      dialog::addSelItem(XLAT("width"), fts(vid.binary_width), 'v');
    else
      dialog::addSelItem(XLAT("variations"), gp::operation_name(), 'v');    
  
    dialog::addBreak(25);
    validity_info();    
    dialog::addBreak(25);
  
    int worldsize;
    
    int gar = 
      GOLDBERG ? gp::area - 1 :
      PURE ? 0 :
      2;    
    
    switch(geometry) {
      case gTorus: 
        worldsize = torusconfig::qty;
        break;
      
      case gZebraQuotient:
        worldsize = 12 + 14 * gar;
        break;
      
      case gFieldQuotient:
        worldsize = isize(currfp.matrices) / ts;
        worldsize = worldsize * (2*tv + ts * gar) / tv / 2;
        break;
      
      case gKleinQuartic:
        worldsize = 24 + 28 * gar;
        break;
      
      case gMacbeath:
        worldsize = (12 + 14 * gar) * 6;
        break;
      
      case gBolza:
        worldsize = 3 * (2*tv + ts * gar) / tv;
        break;
      
      case gBolza2:
        worldsize = 6 * (2*tv + ts * gar) / tv;
        break;
      
      default:
        worldsize = denom ? nom/denom : 0;
        break;
      }
    
    string spf = its(ts);
    if(archimedean) {
      spf = "";
      for(int i: arcm::current.faces) {
        if(spf != "") spf += ",";
        spf += its(i);
        }
      if(BITRUNCATED) spf = "[" + spf + "]," + its(arcm::current.N * 2) + "," + its(arcm::current.N * 2);
      if(DUAL) spf = its(arcm::current.N) + "^[" + spf + "]";
      }
    else if(binarytiling)
      spf = "6,[6,7],7";
    else if(BITRUNCATED && !euclid6)
      spf = spf + "," + its(S6) + "," + its(S6);
    else if(IRREGULAR && irr::bitruncations_performed)
      spf = "[4..8],6,6";
    else if(IRREGULAR)
      spf = "[4..8]^3";
    else if(GOLDBERG && S3 == 4 && gp::param == gp::loc(1, 1))
      spf = spf + ",4," + spf + ",4";
    else if(GOLDBERG && S3 == 4 && gp::param == gp::loc(2, 0))
      spf = spf + ",4,4,4";
    else if(GOLDBERG && S3 == 4)
      spf = "[" + spf + ",4],4,4,4";
    else if(GOLDBERG && S3 == 3)
      spf = "[" + spf + ",6],6,6";
    else {
      string spf0 = spf;
      for(int z=1; z<S3; z++) spf = spf + "," + spf0;
      }
  
    dialog::addSelItem(XLAT("faces per vertex"), spf, 0);
  
    string qstring = "none";
    if(tq & qZEBRA) qstring = "zebra";
  
    else if(tq & qFIELD) qstring = "field";
  
    else if((tq & qNONORIENTABLE) && sphere) qstring = "elliptic";
  
    else if(tq & qFULLTORUS) qstring = "torus";

    else if(tq & qEUWRAP) qstring = "cylinder";
    
    else if(tq & qSMALL) qstring = ginf[geometry].shortname;
  
    dialog::addSelItem(XLAT("quotient space"), XLAT(qstring), 0);
    
    if(hyperbolic && IRREGULAR) {
      nom = isize(irr::cells);
      // both Klein Quartic and Bolza2 are double the Zebra quotiennt
      denom = -2;
      if(!quotient) worldsize = nom / denom;
      }
    
    dialog::addSelItem(XLAT("size of the world"), 
      binarytiling ?  fts4(8 * M_PI * sqrt(2) * log(2) / vid.binary_width) + " exp(∞)" :
      archimedean ? arcm::current.world_size() :
      (archimedean && sphere) ? its(isize(currentmap->allcells())) :
      worldsize < 0 ? (nom%denom ? its(nom)+"/"+its(-denom) : its(-worldsize)) + " exp(∞)": 
      worldsize == 0 ? "∞" :
      its(worldsize),
      '3');
    
    switch(ginf[geometry].cclass) {
      case 0:
        dialog::addSelItem(XLAT("Curvature"), XLAT("hyperbolic"), 0);
        break;
      
      case 1: 
        dialog::addSelItem(XLAT("Curvature"), XLAT("flat"), 0);
        break;
      
      case 2:
        dialog::addSelItem(XLAT("Curvature"), XLAT("spherical"), 0);
        break;
      }
    
    if(sphere) 
      dialog::addBoolItem(XLAT("stereographic/orthogonal"), vid.alpha>10, '1');
    else
      dialog::addBoolItem(XLAT("Poincaré/Klein"), vid.alpha>.5, '1');
    if(euwrap || geometry == gFieldQuotient)
      dialog::addItem(XLAT("advanced parameters"), '4');  
    dialog::addHelp();
    dialog::addBack();
    dialog::display();
  
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      if(uni == 'z') 
        showquotients = !showquotients;
      else if(uni == 'v') {
        if(euclid6) ;
        else if(archimedean) arcm::next_variation();
        else if(binarytiling) {
          dialog::editNumber(vid.binary_width, 0, 2, 0.1, 1, XLAT("binary tiling width"), "");
          dialog::reaction = [] () {
            need_reset_geometry = true;
            #if CAP_TEXTURE
            texture::config.remap();
            #endif
            };
          }
        else if(euclid4) {
          set_variation(PURE ? eVariation::bitruncated : eVariation::pure);
          start_game();
          }
        else // if(S3 == 3) 
          gp::configure();
        }
      else if(uni == '2' || sym == SDLK_F1) gotoHelp(euchelp);
      else if(uni == '3' && !viewdists) { enable_viewdists(); pushScreen(viewdist_configure_dialog); }
      else if(uni == '3' && viewdists) { viewdists = false; }
      else if(uni == '1' && !euclid) {
        if(sphere) {
          if(vid.alpha < 10) { vid.alpha = 999; vid.scale = 998; }
          else {vid.alpha = 1; vid.scale = .4; }
          }
        else {
          if(vid.alpha > .5) { vid.alpha = 0; vid.scale = 1; }
          else {vid.alpha = 1; vid.scale = 1; }
          }
        }
      else if(uni == '5')
        ewhichscreen ^= 3;
      else if(uni == '4') {
        if(euwrap) 
          prepare_torusconfig(),
          pushScreen(showTorusConfig);
        else if(geometry == gFieldQuotient) 
          pushScreen(showQuotientConfig);
        }
      else if(doexiton(sym, uni))
        popScreen();
      };
    }
  else {
    dialog::init(XLAT("experiment with geometry"));
  
    dialog::addSelItem(XLAT("geometry"), XLAT(ginf[geometry].name) + XLAT(bitruncnames[int(variation)]), '5');
    dialog::addBreak(50);
    
    generateLandList([] (eLand l) { return land_validity(l).flags & lv::appears_in_geom_exp; });
    stable_sort(landlist.begin(), landlist.end(), [] (eLand l1, eLand l2) { return land_validity(l1).quality_level > land_validity(l2).quality_level; });
    
    for(int i=0; i<euperpage; i++) {
      if(euperpage * eupage + i >= isize(landlist)) { dialog::addBreak(100); break; }
      eLand l = landlist[euperpage * eupage + i];
      char ch;
      if(i < 26) ch = 'a' + i;
      else ch = 'A' + (i-26);
      string s = XLAT1(linf[l].name);

      if(landvisited[l]) {
        dialog::addBoolItem(s, l == specialland, ch);
        }
      else {
        dialog::addSelItem(s, XLAT("(locked)"), ch);
        }
      
      dialog::lastItem().color = linf[l].color;
      dialog::lastItem().value += validclasses[land_validity(l).quality_level];
      }
    dialog::addBreak(50);
    if(chaosUnlocked && !quotient && !euclid && !sphere)
      dialog::addItem(XLAT("Chaos mode"), '1');
    dialog::addItem(XLAT("next page"), '-');

    dialog::addBreak(25);
    validity_info();    
    dialog::addBreak(25);
    
    dialog::addHelp();
    dialog::addBack();
    dialog::display();
  
    keyhandler = [] (int sym, int uni) {
      dialog::handleNavigation(sym, uni);
      int lid;
      if(uni >= 'a' && uni <= 'z') lid = uni - 'a';
      else if(uni >= 'A' && uni <= 'Z') lid = 26 + uni - 'A';
      else lid = -1;
      
      if(lid >= 0) lid += euperpage * eupage;
      
      if(uni == '5') 
        ewhichscreen ^= 3;
      else if(uni == '-' || uni == PSEUDOKEY_WHEELUP || uni == PSEUDOKEY_WHEELDOWN) {
        eupage++;
        if(eupage * euperpage >= isize(landlist)) eupage = 0;
        }
      else if(uni == '1') {
        if(chaosUnlocked) {
          stop_game_and_switch_mode(rg::chaos);
          start_game();
          }
        }
      else if(lid >= 0 && lid < isize(landlist)) {
        eLand nland = landlist[lid];
        if(landvisited[nland]) {
          stop_game_and_switch_mode(tactic::on ? rg::tactic : rg::nothing);
          firstland = specialland = nland;
          start_game();
          }
        }
      else if(uni == '2' || sym == SDLK_F1) gotoHelp(euchelp);
      else if(doexiton(sym, uni)) popScreen();
      };
    }
  
  }

void runGeometryExperiments() {
  if(!geometry)
    specialland = getLandForList(cwt.at);
  pushScreen(showEuclideanMenu);
  }

#if CAP_COMMANDLINE
int read_geom_args() {
  using namespace arg;
  if(argis("-qpar")) { 
    int p;
    shift(); sscanf(argcs(), "%d,%d,%d", 
      &p, &quotientspace::rvadd, &quotientspace::rvdir
      );
    autocheat = true;
    currfp.init(p); 
    }
  else if(argis("-qpar2")) {
    stop_game_and_switch_mode(rg::nothing);
    int a, b;
    shift(); sscanf(argcs(), "%d,%d", &a, &b);
    using namespace fieldpattern;
    current_extra = a;

    auto& gxcur = fgeomextras[current_extra];
    while(b >= isize(gxcur.primes)) nextPrime(gxcur);

    fgeomextras[current_extra].current_prime_id = b;
    enableFieldChange();
    set_geometry(gFieldQuotient);
    }
  else if(argis("-cs")) {
    shift(); cheat();
    fieldpattern::matrix M = currfp.strtomatrix(args());
    fieldpattern::subpathid = currfp.matcode[M];
    fieldpattern::subpathorder = currfp.order(M);
    }
  else if(argis("-csp")) {
    cheat();
    currfp.findsubpath();
    }
  else if(argis("-tpar")) { 
    torusconfig::torus_mode = torusconfig::tmSingle;
    shift(); sscanf(argcs(), "%d,%d,%d", 
      &torusconfig::qty, 
      &torusconfig::dx,
      &torusconfig::dy
      );
    }
  else if(argis("-tparx")) {
    shift(); 
    sscanf(argcs(), "%d,%d,%d", 
      (int*) &torusconfig::torus_mode,
      &torusconfig::sdx,
      &torusconfig::sdy
      );
    if(torusconfig::torus_mode == torusconfig::tmSingle)
      torusconfig::qty = torusconfig::sdx,
      torusconfig::dy = torusconfig::sdy;
    torusconfig::activate();
    }
  TOGGLE('7', PURE, set_variation(PURE ? eVariation::bitruncated : eVariation::pure))
  else if(argis("-geo")) { 
    PHASEFROM(2);
    shift(); 
    set_geometry((eGeometry) argi());
    }
  else if(argis("-gp")) {
    PHASEFROM(2);
    shift(); gp::param.first = argi();
    shift(); gp::param.second = argi();
    set_variation(eVariation::goldberg);
    }
  else if(argis("-fi")) {
    fieldpattern::info();
    exit(0);
    } 
  else if(argis("-qs")) {
    cheat();
    shift(); currfp.qpaths.push_back(args());
    }
  else if(argis("-d:quotient")) 
    launch_dialog(showQuotientConfig);
  else if(argis("-d:torus")) {
    launch_dialog(showTorusConfig);
    prepare_torusconfig();
    }
  else if(argis("-d:geom")) 
    launch_dialog(showEuclideanMenu);
  else return 1;
  return 0;
  }

auto ah_geom = addHook(hooks_args, 0, read_geom_args);
#endif

}
