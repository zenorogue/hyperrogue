// Hyperbolic Rogue -- the 'experiments with geometry' menu
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file geom-exp.cpp
 *  \brief The 'experiments with geometry' menu
 *
 *  Implementation of this menu, and computation of the statistics shown there
 */

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

#if CAP_FIELD
void showQuotientConfig() {
  using namespace fieldpattern;
  gamescreen(2);
  dialog::init(XLAT("advanced configuration"));
  fgeomextra& gxcur = fgeomextras[current_extra];
  for(int i=0; i<isize(fgeomextras); i++) {
    auto& g = fgeomextras[i];
    dialog::addBoolItem(ginf[g.base].tiling_name, g.base == gxcur.base, 'a'+i);
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
    else if(uni == 'x' || uni == '\n') dialog::do_if_confirmed([&gxcur] {
      set_geometry(gxcur.base);
      enableFieldChange();
      set_geometry(gFieldQuotient);
      start_game();
      });
    else if(uni == 'c') dialog::do_if_confirmed([] {
      set_geometry(gEuclid);
      fieldpattern::quotient_field_changed = false;
      set_geometry(gFieldQuotient);
      start_game();
      });
    else if(doexiton(sym, uni))
      popScreen();
    };
  
  dialog::display();
  }
#endif

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
  cmode = sm::SIDE | sm::MAYDARK;
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

  if(square) dialog::addBoolItem(XLAT("bitruncated"), torus_bitrunc, 't');
  else dialog::addInfo("", 100);
  
  int valid = 2;

  int adx = torusconfig::newsdx, ady = torusconfig::newsdy;
  if(single) {  
    if(square) {
      dialog::addInfo("this mode has bad patterns", 0x808080), valid = 1;
      if(torus_bitrunc && valid == 1 && (torusconfig::newqty%2 || torusconfig::newdy % 2 == 0))
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
      if(torus_bitrunc && valid == 1)
        dialog::addInfo("incompatible with bitruncating", 0x808080), valid = 0;      
      if(klein && !torusconfig::mobius_symmetric(square, adx, ady))
        dialog::addInfo("Möbius band requires a symmetric period", 0x800000), valid = 0;
      }
    else {
      if(torusconfig::newsdy % 3)
        dialog::addInfo(XLAT("best if %1 is divisible by %2", "y", "3"), 0x808080), valid = 1;
      if(torusconfig::newsdx % 3)
        dialog::addInfo(XLAT("best if %1 is divisible by %2", "x", "3"), 0x808080), valid = 1;
      if(klein && !torusconfig::mobius_symmetric(square, adx, ady))
        dialog::addInfo("Möbius band requires a symmetric period", 0x800000), valid = 0;
      }
    }
  else {
    if(square) {
      if(torusconfig::newsdx & 1)
        dialog::addInfo(XLAT("best if %1 is divisible by %2", "x", "2"), 0x808080), valid = 1;
      if(torusconfig::newsdy & 1)
        dialog::addInfo(XLAT("best if %1 is divisible by %2", "y", "2"), 0x808080), valid = 1;
      if(torus_bitrunc && valid == 1)
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
  if(cyl) {
    if(!(square && klein)) {
      dialog::addBoolItem(XLAT("set y=-2x for Crossroads"), ady == -2 * adx, 'C');
      dialog::add_action([] () { torusconfig::newsdy = -2 * torusconfig::newsdx; });
      }
    dialog::addBoolItem(XLAT("set y=0 for Crossroads IV and Chaos Mode"), ady == 0, 'D');
    dialog::add_action([] () { torusconfig::newsdy = 0; });
    }
  
  dialog::addSelItem(XLAT("scale factor"), fts(vid.scale), 'z');

#if CAP_RUG
  if(GDIM == 2) dialog::addBoolItem(XLAT("hypersian rug mode"), (rug::rugged), 'u');
#endif

  dialog::addItem("activate", 'a');
  dialog::addItem("default", 'c');

  keyhandler = [=] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
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
    else if((uni == 'a' || uni == '\n') && torusconfig::newqty >= 3 && valid) dialog::do_if_confirmed([square] {
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
      });
    else if(uni == 'c') dialog::do_if_confirmed([] {
      set_geometry(gEuclid);
      torusconfig::torus_mode = torusconfig::tmSingle;
      torusconfig::qty = torusconfig::def_qty;
      torusconfig::dy = torusconfig::def_dy;
      set_geometry(gTorus);
      start_game();
      });
    else if(uni == 'z') editScale();
#if CAP_RUG
    else if(uni == 'u' && GDIM == 2) rug::select();
#endif
    else if(doexiton(sym, uni))
      popScreen();
    };
  
  dialog::display();
  }

EX string bitruncnames[5] = {" (b)", " (n)", " (g)", " (i)", " (d)"};

void validity_info() {
  int vccolors[4] = {0xFF0000, 0xFF8000, 0xFFFF00, 0x00FF00};
  auto lv = land_validity(specialland);
  if(lv.flags & lv::display_error_message)
    dialog::addInfo(XLAT(lv.msg), vccolors[lv.quality_level]);
  else
    dialog::addBreak(100);
  }

bool showquotients;

string validclasses[4] = {" (X)", " (½)", "", " (!)"};
  
void ge_land_selection() {
  cmode = sm::SIDE | sm::MAYDARK;
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
  landvisited[laCamelot] |= hiitemsMax(treasureType(laCamelot)) >= 1;
  landvisited[laCA] = true;
  landvisited[laAsteroids] = true;

  dialog::init(XLAT("experiment with geometry"));
  
  // dialog::addSelItem(XLAT("geometry"), XLAT(bitruncnames[int(variation)]), '5');
  // dialog::addBreak(50);
    
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
    dialog::add_action([l] {
      if(landvisited[l]) dialog::do_if_confirmed(dual::mayboth([l] {
        stop_game_and_switch_mode(tactic::on ? rg::tactic : rg::nothing);
        firstland = specialland = l;
        start_game();
        popScreen();
        }));
      });
    }
  dialog::addBreak(50);
  if(chaosUnlocked && !quotient && !euclid && !sphere) {
    dialog::addItem(XLAT("Chaos mode"), '1');
    dialog::add_action(dual::mayboth([] {
      if(chaosUnlocked) dialog::do_if_confirmed([] {
        stop_game_and_switch_mode(rg::chaos);
        start_game();
        });
      }));
    }
  dialog::addItem(XLAT("next page"), '-');

  dialog::addBreak(25);
  validity_info();    
  dialog::addBreak(25);
  
  dual::add_choice();  
  dialog::addBack();
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    
    if(uni == '-' || uni == PSEUDOKEY_WHEELUP || uni == PSEUDOKEY_WHEELDOWN) {
      eupage++;
      if(eupage * euperpage >= isize(landlist)) eupage = 0;
      }
    else if(doexiton(sym, uni)) popScreen();
    };
  }

vector<eGeometry> tilinglist = {
  gTinySphere, gSmallSphere, gSphere, gEuclid, gNormal, gOctagon,
  gOctahedron, gEuclidSquare, g45, g46, g47,
  gArchimedean, gBinary4, gBinaryTiling, gKiteDart2
  };

vector<eGeometry> quotientlist = {
  gElliptic,
  gZebraQuotient, gFieldQuotient, gTorus,
  gSmallElliptic, 
  gKleinQuartic, gBolza, gBolza2, gMinimal, 
  gMacbeath, gBring, gSchmutzM2, gSchmutzM3, gCrystal  
  };

vector<eGeometry> list3d = {
  gBinary3, gHoroTris, gHoroRec, gHoroHex,
  gSpace534, gSpace435, 
  gField534, gField435,
  gCubeTiling, gRhombic3, gBitrunc3, 
  gCell120, gECell120,
  gCell600, gECell600,
  gCell24, gECell24,
  gCell16, gECell16,
  gCell8, gECell8,
  gCell5, gKiteDart3, gSol, gNil, gProduct, gRotSpace
  };

void ge_select_tiling(const vector<eGeometry>& lst) {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);  

  dialog::init(XLAT("experiment with geometry"));
  
  /* if(&lst == &list3d)
    dialog::addInfo("3D geometries are a work in progress", 0x800000); */

  char letter = 'a';
  for(eGeometry i: lst) {
    bool on = geometry == i;
    dynamicval<eGeometry> cg(geometry, eGeometry(i));
    if(archimedean && !CAP_ARCM) continue;
    if(cryst && !CAP_CRYSTAL) continue;
    if(geometry == gFieldQuotient && !CAP_FIELD) continue;
    dialog::addBoolItem(XLAT(
      (geometry == gProduct && !hybri) ? XLAT("current geometry x E") : 
      (geometry == gRotSpace && !hybri) ? XLAT("space of rotations in current geometry") : 
      ginf[i].menu_displayed_name), on, letter++);
    dialog::lastItem().value += validclasses[land_validity(specialland).quality_level];
    dialog::add_action(dual::mayboth([i] {
      eGeometry targetgeometry = eGeometry(i);
      if(0) ;
      #if CAP_CRYSTAL
      else if(targetgeometry == gCrystal)
        pushScreen(crystal::show);
      #endif
      #if CAP_ARCM
      else if(targetgeometry == gArchimedean)
        pushScreen(arcm::show);
      #endif
      else dialog::do_if_confirmed([targetgeometry] () {
        bool th = among(targetgeometry, gProduct, gRotSpace);
        if(th && (WDIM == 3 || euclid)) {
          addMessage(XLAT("Only works with 2D non-Euclidean geometries"));
          return;
          }
        if(targetgeometry == gRotSpace) {
          bool ok = true;
          if(archimedean) ok = PURE;
          else if(binarytiling || penrose) ok = false;
          else ok = PURE || BITRUNCATED;
          if(!ok) {
            addMessage(XLAT("Only works with (semi-)regular tilings"));
            return;
            }
          if(archimedean) {
            int steps, single_step;
            if(!arcm::current.get_step_values(steps, single_step)) {
              addMessage(XLAT("That would have %1/%2 levels", its(steps), its(single_step)));
              return;
              }
            }
          }
        set_geometry(targetgeometry);
        start_game();
        if(euwrap) {
          prepare_torusconfig();
          pushScreen(showTorusConfig);
          }
        #if CAP_FIELD
        if(geometry == gFieldQuotient) {
          pushScreen(showQuotientConfig);
          }
        #endif
        });
      }));
    }
  
  dual::add_choice();  
  dialog::addBack();
  dialog::display();
  }

EX string current_proj_name() {
  if(pmodel != mdDisk || nonisotropic)
    return models::get_model_name(pmodel);
  else if(hyperbolic && vid.alpha == 1)
    return XLAT("Poincaré model");
  else if(hyperbolic && vid.alpha == 0)
    return XLAT("Klein-Beltrami model");
  else if(hyperbolic && vid.alpha == -1)
    return XLAT("inverted Poincaré model");
  else if(sphere && vid.alpha == 1)
    return XLAT("stereographic projection");
  else if(sphere && vid.alpha == 0)
    return XLAT("gnomonic projection");
  else if(sphere && vid.alpha >= 999)
    return XLAT("orthographic projection");
  else if(hyperbolic && vid.alpha >= 999)
    return XLAT("Gans model");
  else 
    return XLAT("general perspective");
  }

EX void showEuclideanMenu() {
  // for(int i=2; i<lt; i++) landvisited[i] = true;

  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);  

  dialog::init(XLAT("experiment with geometry"));
  
  dialog::addSelItem(XLAT("basic tiling"), XLAT(ginf[geometry].tiling_name), 't');
  dialog::add_action_push([] { ge_select_tiling(tilinglist); });

  int ts = ginf[geometry].sides;
  int tv = ginf[geometry].vertex;
  int nom = (BITRUNCATED ? tv+ts : tv) * 4;
  int denom = (2*ts + 2*tv - ts * tv);
  
  #if CAP_GP
  if(GOLDBERG && S3)
    nom = 2 * (2*tv + ts * (cgi.gpdata->area-1));

  if(GOLDBERG && S3 == 4)
    nom = 2 * (2*tv + 2 * ts * (cgi.gpdata->area-1));
  #endif

  int worldsize;
    
  int euler = 0;
  if(euclid) euler = 0;
  else if(sphere && nonorientable) euler = 1;
  else if(sphere) euler = 2;
  else if(!bounded) euler = -2;
  else if(WDIM == 3) euler = 0;
  else switch(geometry) {
    case gFieldQuotient:
      worldsize = isize(currentmap->allcells());
      euler = 2 * worldsize * denom / nom;
      break;
    
    case gMinimal:
      euler = -1;
      break;
    
    case gZebraQuotient:
    case gBolza:
      euler = -2;
      break;
    
    case gKleinQuartic:
    case gSchmutzM2:
    case gBolza2:
      euler = -4;
      break;
    
    case gSchmutzM3:
    case gBring:
      euler = -6;
      break;
    
    case gMacbeath:
      euler = -12;
      break;
    
    default: 
      println(hlog, "warning: Euler characteristics unknown");
      break;
    }
  
  nom *= euler;
  denom *= 2;
        
  if(hybri && !prod) nom *= cgi.steps, denom *= cgi.single_step;

  int g = gcd(nom, denom);
  if(g) {
    nom /= g;
    denom /= g;
    }
  
  if(fulltorus) {
    using namespace torusconfig;
    auto& mode = tmodes[torus_mode];
    if(mode.flags & TF_SINGLE)
      worldsize = qty;
    else
      worldsize = sdx * sdy;
    }
  else worldsize = denom ? nom / denom : 0;
  
  if(euler < 0 && !bounded)
    worldsize = -worldsize;

  string spf = its(ts);
  if(0) ;
  #if CAP_ARCM
  else if(archimedean) {
    spf = "";
    for(int i: arcm::current.faces) {
      if(spf != "") spf += ",";
      spf += its(i);
      }
    if(BITRUNCATED) spf = "[" + spf + "]," + its(arcm::current.N * 2) + "," + its(arcm::current.N * 2);
    if(DUAL) spf = its(arcm::current.N) + "^[" + spf + "]";
    }
  #endif
  #if CAP_BT
  else if(binarytiling)
    spf = "6,[6,7],7";
  #endif
  else if(BITRUNCATED && !euclid6)
    spf = spf + "," + its(S6) + "," + its(S6);
  #if CAP_IRR
  else if(IRREGULAR && irr::bitruncations_performed)
    spf = "[4..8],6,6";
  else if(IRREGULAR)
    spf = "[4..8]^3";
  #endif
  #if CAP_GP
  else if(GOLDBERG && S3 == 4 && gp::param == gp::loc(1, 1))
    spf = spf + ",4," + spf + ",4";
  else if(GOLDBERG && S3 == 4 && gp::param == gp::loc(2, 0))
    spf = spf + ",4,4,4";
  else if(GOLDBERG && S3 == 4)
    spf = "[" + spf + ",4],4,4,4";
  else if(GOLDBERG && S3 == 3)
    spf = "[" + spf + ",6],6,6";
  #endif
  else {
    string spf0 = spf;
    for(int z=1; z<S3; z++) spf = spf + "," + spf0;
    }

  string qstring = ginf[geometry].quotient_name;

  if(qstring == "none")
    dialog::addBoolItem(XLAT("quotient space"), false, 'q');
  else
    dialog::addSelItem(XLAT("quotient space"), XLAT(qstring), 'q');

  dialog::add_action_push([] { ge_select_tiling(quotientlist); });

  #if MAXMDIM >= 4
  dialog::addSelItem(XLAT("dimension"), its(WDIM), 'd');
  dialog::add_action_push([] { ge_select_tiling(list3d); });
  #endif
  
  #if CAP_IRR
  if(hyperbolic && IRREGULAR) {
    nom = isize(irr::cells);
    // both Klein Quartic and Bolza2 are double the Zebra quotiennt
    denom = -2;
    if(!quotient) worldsize = nom / denom;
    }
  #endif
  
  if(prod) {
    dialog::addSelItem(XLAT("Z-level height factor"), fts(vid.plevel_factor), 'Z');
    dialog::add_action([] {
      dialog::editNumber(vid.plevel_factor, 0, 2, 0.1, 0.7, XLAT("Z-level height factor"), "");
      });
    }
  else if(hybri) {
    dialog::addSelItem(XLAT("number of levels"), its(cgi.steps / cgi.single_step), 0);
    }
  else if(ts == 6 && tv == 3)
    dialog::addSelItem(XLAT("variations"), XLAT("does not matter"), 'v');
  else if(binarytiling) {
    dialog::addSelItem(XLAT("width"), fts(vid.binary_width), 'v');
    dialog::add_action([] {
      dialog::editNumber(vid.binary_width, 0, 2, 0.1, 1, XLAT("binary tiling width"), "");
      dialog::reaction = [] () {
        #if CAP_TEXTURE
        texture::config.remap();
        #endif
        };
      });
    extern void add_edit_wall_quality(char);
    add_edit_wall_quality('W');
    }
  else if(WDIM == 3 || penrose) dialog::addBreak(100);
  else {
    dialog::addSelItem(XLAT("variations"), gp::operation_name(), 'v');    
    dialog::add_action([] {
      if(0) ;
      #if CAP_ARCM
      else if(archimedean) arcm::next_variation();
      #endif
      else if(euclid4 || !CAP_GP) dialog::do_if_confirmed([] {
        set_variation(PURE ? eVariation::bitruncated : eVariation::pure);
        start_game();
        });
      #if CAP_GP
      else // if(S3 == 3) 
        gp::configure();
      #endif
      });
    }

        
  
  if(euwrap || geometry == gFieldQuotient || cryst || archimedean || (euclid && WDIM == 3)) {
    dialog::addItem(XLAT("advanced parameters"), '4');
    dialog::add_action([] {
      if(0); 
      #if CAP_ARCM
      else if(archimedean)
        pushScreen(arcm::show);
      #endif
      #if CAP_CRYSTAL
      else if(cryst)
        pushScreen(crystal::show);
      #endif
      #if MAXMDIM == 4
      else if(euclid && WDIM == 3)
        euclid3::prepare_torus3(),
        pushScreen(euclid3::show_torus3);
      #endif
      else if(euwrap) 
        prepare_torusconfig(),
        pushScreen(showTorusConfig);
      #if CAP_FIELD
      else if(geometry == gFieldQuotient) 
        pushScreen(showQuotientConfig);
      #endif
      });
    }
  else dialog::addBreak(100);
  
  dialog::addBreak(50);
  
  dialog::addSelItem(XLAT("land"), XLAT1(linf[specialland].name), 'l');
  dialog::add_action_push(ge_land_selection);
  
  if(specialland == laMinefield && bounded) {
    dialog::addSelItem(XLAT("number of mines"), its(bounded_mine_quantity), 'm');
    dialog::add_action([] {
      dialog::editNumber(bounded_mine_quantity, 0, bounded_mine_max, 1, (bounded_mine_max+5)/10, 
        XLAT("number of mines"), "");
      dialog::reaction = [] {
        if(bounded_mine_quantity < 0) bounded_mine_quantity = 0;
        if(bounded_mine_quantity > bounded_mine_max) bounded_mine_quantity = bounded_mine_max;
        };
      dialog::reaction_final = [] {
        bounded_mine_percentage = bounded_mine_quantity * 1. / bounded_mine_max;
        stop_game();
        start_game();
        };
      });
    }

  if(specialland == laMinefield && geometry_has_alt_mine_rule()) {
    dialog::addSelItem(XLAT("mine adjacency rule"), XLAT(mine_adjacency_rule ? "vertex" : WDIM == 3 ? "face" : "edge"), 'M');
    dialog::add_action([] {
      stop_game();
      mine_adjacency_rule = !mine_adjacency_rule;
      start_game();
      });
    }
  
  dialog::addBoolItem(XLAT("pattern"), specialland == laCanvas, 'p');
  if(specialland == laCanvas) dialog::lastItem().value = patterns::whichCanvas;
  dialog::add_action_push(patterns::showPrePattern);
  validity_info();
  if(WDIM == 3) {
    dialog::addItem(XLAT("3D configuration"), '9');
    dialog::add_action_push(show3D);
    }
  dialog::addSelItem(XLAT("projection"), current_proj_name(), '1');
  dialog::add_action_push(models::model_menu);
  if(nonisotropic && !sl2)
    dialog::addBoolItem_action(XLAT("geodesic movement in Sol/Nil"), nisot::geodesic_movement, 'G');
  #if CAP_CRYSTAL && MAXMDIM >= 4
  crystal::add_crystal_transform('x');  
  #endif

  dialog::addBreak(50);
   
  #if CAP_SHOT
  dialog::addItem(XLAT("take screenshot"), 's');
  dialog::add_action_push(shot::menu);
  #endif

  dialog::addHelp();
  dialog::addBack();
  
  dialog::addBreak(150);

  string fgname = XLAT(ginf[geometry].tiling_name);
  if(qstring != "none") fgname += " " + XLAT(qstring);
  if(!euclid6) fgname = gp::operation_name() + " " + fgname;
  
  dialog::addTitle(XLAT("info about: %1", fgname), 0xFFFFFF, 150);
  
  if(WDIM == 2) dialog::addSelItem(XLAT("faces per vertex"), spf, 0);

  dialog::addSelItem(XLAT("size of the world"), 
    #if CAP_BT
    binarytiling ? fts(8 * M_PI * sqrt(2) * log(2) / pow(vid.binary_width, WDIM-1), 4) + " exp(∞)" :
    #endif
    #if CAP_ARCM
    archimedean && (WDIM == 2) ? arcm::current.world_size() :
    (archimedean && sphere) ? its(isize(currentmap->allcells())) :
    #endif
    #if CAP_CRYSTAL
    cryst ? "∞^" + its(ts/2) :
    #endif
    WDIM == 3 && bounded ? its(isize(currentmap->allcells())) :
    WDIM == 3 && euclid ? "∞" :
    worldsize < 0 ? (nom%denom ? its(nom)+"/"+its(denom) : its(-worldsize)) + " exp(∞)": 
    (euwrap && !fulltorus) ? "∞" :
    worldsize == 0 ? "∞²" :
    its(worldsize),
    '3');
  
  if(WDIM == 2) dialog::add_action([] {
    if(!viewdists) { enable_viewdists(); pushScreen(viewdist_configure_dialog); }
    else if(viewdists) viewdists = false;
    });

  if(bounded) {
    if(WDIM == 3) euler = 0;
    dialog::addSelItem(XLAT("Euler characteristics"), its(euler), 0);
    if(WDIM == 3) ;
    else if(nonorientable)
      dialog::addSelItem(XLAT("demigenus"), its(2-euler), 0);
    else
      dialog::addSelItem(XLAT("genus"), its((2-euler)/2), 0);
    }
  else dialog::addBreak(200);
  
  switch(ginf[geometry].cclass) {
    case gcHyperbolic:
      dialog::addSelItem(XLAT("Curvature"), XLAT("hyperbolic"), 0);
      break;
    
    case gcEuclid: 
      dialog::addSelItem(XLAT("Curvature"), XLAT("flat"), 0);
      break;
    
    case gcSphere:
      dialog::addSelItem(XLAT("Curvature"), XLAT("spherical"), 0);
      break;

    case gcSol:
      dialog::addSelItem(XLAT("Curvature"), XLAT("Sol"), 0);
      break;

    case gcNil:
      dialog::addSelItem(XLAT("Curvature"), XLAT("Nil"), 0);
      break;

    case gcSL2:
      dialog::addSelItem(XLAT("Curvature"), XLAT("~SL(2,R)~"), 0);
      break;

    case gcProduct:
      dialog::addSelItem(XLAT("Curvature"), XLAT("Product"), 0);
      break;
    }    
    
  dialog::display();
  }

EX void runGeometryExperiments() {
  if(!geometry && specialland == laIce)
    specialland = getLandForList(cwt.at);
  pushScreen(showEuclideanMenu);
  }

#if CAP_COMMANDLINE

eGeometry readGeo(const string& ss) {
  bool numeric = true;
  for(char c: ss) if(c < '0' || c > '9') numeric = false;
  if(numeric) return eGeometry(atoi(ss.c_str()));
  for(int i=0; i<isize(ginf); i++) if(appears(ginf[i].menu_displayed_name, ss)) {
    return eGeometry(i);
    break;
    }
  return gNormal;
  }

int read_geom_args() {
  using namespace arg;
  if(0) ;
  #if CAP_FIELD
  else if(argis("-qpar")) { 
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
  #endif
  else if(argis("-mineadj")) {
    shift(); mine_adjacency_rule = argi();
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
    int tmode;
    sscanf(argcs(), "%d,%d,%d", &tmode,
      &torusconfig::sdx,
      &torusconfig::sdy
      );
    if(tmode < 0 || tmode >= isize(torusconfig::tmodes)) {
      println(hlog, "bad tmode");
      exit(1);
      }
    torusconfig::torus_mode = torusconfig::eTorusMode(tmode);
    if(torusconfig::torus_mode == torusconfig::tmSingle)
      torusconfig::qty = torusconfig::sdx,
      torusconfig::dy = torusconfig::sdy;
    torusconfig::activate();
    }
  TOGGLE('7', PURE, set_variation(PURE ? eVariation::bitruncated : eVariation::pure))
  else if(argis("-geo")) { 
    PHASEFROM(2);
    shift(); 
    set_geometry(readGeo(args()));
    }
  #if CAP_GP
  else if(argis("-gp")) {
    PHASEFROM(2);
    shift(); gp::param.first = argi();
    shift(); gp::param.second = argi();
    set_variation(eVariation::goldberg);
    }
  #endif
  #if CAP_FIELD
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
  #endif
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
