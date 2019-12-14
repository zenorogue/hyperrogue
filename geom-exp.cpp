// Hyperbolic Rogue -- the 'experiments with geometry' menu
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file geom-exp.cpp
 *  \brief The 'experiments with geometry' menu
 *
 *  Implementation of this menu, and computation of the statistics shown there
 */

#include "hyper.h"
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
  dialog::init(XLAT("field quotient"));
  fgeomextra& gxcur = fgeomextras[current_extra];
  for(int i=0; i<isize(fgeomextras); i++) {
    auto& g = fgeomextras[i];
    dialog::addBoolItem(ginf[g.base].tiling_name, g.base == gxcur.base, 'a'+i);
    }
  
  dialog::addBreak(100);
  
  nextPrimes(gxcur);
  string stars[3] = {"", "*", "**"};
  for(int i=0; i<isize(gxcur.primes); i++) {
    auto& p = gxcur.primes[i];
    
    int sides = ginf[gxcur.base].sides;
    bool sa = shapename_available(sides);
    string s = sa ? "order %1%2 (%3 %4s)" : "order %1%2 (%3 %4-gons)";
    
    s = XLAT(s, its(p.p), p.squared ? "²" : "", its(p.cells), sa ? shapename(sides) : its(sides));
    
    s += stars[gxcur.dualval[i]];
    
    dialog::addBoolItem(s, i == gxcur.current_prime_id, 'A'+i);
    }
  
  if(isize(gxcur.primes) <= 6) {
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

EX string bitruncnames[5] = {" (b)", " (n)", " (g)", " (i)", " (d)"};

void validity_info() {
  int vccolors[4] = {0xFF0000, 0xFF8000, 0xFFFF00, 0x00FF00};
  auto lv = land_validity(specialland);
  if(lv.flags & lv::display_error_message)
    dialog::addInfo(XLAT(lv.msg), vccolors[lv.quality_level]);
  else
    dialog::addBreak(100);
  }

EX bool showquotients;

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
  if(chaosUnlocked && !quotient && !euclid && !sphere && !walls_not_implemented()) {
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

#if HDR
struct geometry_filter {
  string name;
  /** test if the current geometry matches the filter */
  function<bool()> test; 
  };
#endif

EX geometry_filter *current_filter;

bool forced_quotient() { return quotient && !(cgflags & qOPTQ); }

EX geometry_filter gf_hyperbolic = {"hyperbolic", [] { return (arcm::in() || hyperbolic) && !forced_quotient(); }};
EX geometry_filter gf_spherical = {"spherical", [] { return (arcm::in() || sphere) && !forced_quotient(); }};
EX geometry_filter gf_euclidean = {"Euclidean", [] { return (arcm::in() || euclid) && !forced_quotient(); }};
EX geometry_filter gf_other = {"non-isotropic", [] { return prod || nonisotropic; }};
EX geometry_filter gf_regular_2d = {"regular 2D tesselations", [] { 
  return standard_tiling() && WDIM == 2 && !forced_quotient();
  }};
EX geometry_filter gf_regular_3d = {"regular 3D honeycombs", [] { 
  if(euclid) return geometry == gCubeTiling;
  return !binarytiling && !penrose && WDIM == 3 && !forced_quotient() && !nonisotropic && !prod;
  }};
EX geometry_filter gf_quotient = {"interesting quotient spaces", [] { 
  return forced_quotient() && !elliptic;
  }};
  
vector<geometry_filter*> available_filters = { &gf_hyperbolic, &gf_spherical, &gf_euclidean, &gf_other, &gf_regular_2d, &gf_regular_3d, &gf_quotient };

void ge_select_filter() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(2);  

  dialog::init(XLAT("geometries"));
  
  char x = 'a';
  for(auto f: available_filters) {
    if(current_filter)
      dialog::addBoolItem(XLAT(f->name), f == current_filter, x++);
    else
      dialog::addItem(XLAT(f->name), x++);
    dialog::add_action([f] { current_filter = f; popScreen(); });
    }

  dialog::addBack();
  dialog::display();
  }

void set_default_filter() {
  current_filter = hyperbolic ? &gf_hyperbolic : euclid ? &gf_euclidean : sphere ? &gf_spherical : &gf_other; 
  for(auto f: available_filters) if(f->test()) current_filter = f;
  }

void set_or_configure_geometry(eGeometry g) {
  if(0) ;
  #if CAP_CRYSTAL
  else if(g == gCrystal)
    pushScreen(crystal::show);
  #endif
  #if CAP_ARCM
  else if(g == gArchimedean)
    pushScreen(arcm::show);
  #endif
  else {
    if(among(g, gProduct, gRotSpace)) {
      if(WDIM == 3 || (g == gRotSpace && euclid)) {
        addMessage(XLAT(          
          g == gRotSpace ?
            "Only works with 2D non-Euclidean geometries"
          : "Only works with 2D geometries"
            ));
        return;
        }
      if(g == gRotSpace) {
        bool ok = true;
        if(arcm::in()) ok = PURE;
        else if(binarytiling || penrose) ok = false;
        else ok = PURE || BITRUNCATED;
        if(!ok) {
          addMessage(XLAT("Only works with (semi-)regular tilings"));
          return;
          }
        if(arcm::in()) {
          int steps, single_step;
          if(!arcm::current.get_step_values(steps, single_step)) {
            addMessage(XLAT("That would have %1/%2 levels", its(steps), its(single_step)));
            return;
            }
          }
        }
      }
    dual::may_split_or_do([g] { set_geometry(g); });
    start_game();
    }
  }

/** is g2 the same tiling as the current geometry (geometry)? */
bool same_tiling(eGeometry g2) {
  if(g2 == gCrystal)
    return S3 == 4;
  if(g2 == gFieldQuotient && geometry != gFieldQuotient) {
    int ce = 0;
    for(auto& ge: fieldpattern::fgeomextras) {
      if(ginf[ge.base].tiling_name == ginf[geometry].tiling_name) {
        fieldpattern::current_extra = ce;
        return true;
        }
      ce++;
      }
    }
  return ginf[g2].tiling_name == ginf[geometry].tiling_name;    
  }

void ge_select_tiling() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);  

  if(!current_filter) { popScreen(); return; }
  dialog::init();
  dialog::addItem(XLAT(current_filter->name), 'x');
  dialog::add_action_push(ge_select_filter);
  
  vector<eGeometry> geometries;

  dialog::addBreak(100);
  
  char letter = 'a';
  for(int i=0; i<isize(ginf); i++) {
    eGeometry g = eGeometry(i);
    if(among(g, gProduct, gRotSpace)) hybrid::configure(g);
    bool orig_el = elliptic;
    bool on = geometry == g;
    bool in_2d = WDIM == 2;
    dynamicval<eGeometry> cg(geometry, g);
    if(g == gArbitrary) continue;
    if(g == gTorus) continue;
    if(arcm::in() && !CAP_ARCM) continue;
    if(cryst && !CAP_CRYSTAL) continue;
    if(sol && !CAP_SOLV) continue;
    if(WDIM == 3 && MAXMDIM == 3) continue;
    if(geometry == gFieldQuotient && !CAP_FIELD) continue;
    if(!current_filter->test()) continue;
    if(orig_el) {
      for(int j=0; j<isize(ginf); j++) 
        if(ginf[j].tiling_name == ginf[i].tiling_name)
          geometry = g = eGeometry(j);
      }
    dialog::addBoolItem(XLAT(
      (geometry == gProduct && in_2d) ? XLAT("current geometry x E") : 
      (geometry == gRotSpace && in_2d) ? XLAT("space of rotations in current geometry") : 
      ginf[g].menu_displayed_name), on, letter++);
    dialog::lastItem().value += validclasses[land_validity(specialland).quality_level];
    dialog::add_action([g] { set_or_configure_geometry(g); });
    }
  
  dialog::addBreak(100);
  dual::add_choice();  
  dialog::addBack();
  dialog::display();
  }

EX string current_proj_name() {
  bool h = hyperbolic || solnih;
  if(pmodel != mdDisk)
    return models::get_model_name(pmodel);
  else if(h && vid.alpha == 1)
    return XLAT("Poincaré model");
  else if(h && vid.alpha == 0)
    return XLAT("Klein-Beltrami model");
  else if(h && vid.alpha == -1)
    return XLAT("inverted Poincaré model");
  else if(sphere && vid.alpha == 1)
    return XLAT("stereographic projection");
  else if(sphere && vid.alpha == 0)
    return XLAT("gnomonic projection");
  else if(sphere && vid.alpha >= 999)
    return XLAT("orthographic projection");
  else if(h && vid.alpha >= 999)
    return XLAT("Gans model");
  else 
    return XLAT("general perspective");
  }

EX string dim_name() {
  return " (" + its(WDIM) + "D)";
  }

EX string geometry_name() {
  switch(ginf[geometry].cclass) {
    case gcHyperbolic:
      return XLAT("hyperbolic") + dim_name();

    case gcEuclid: 
      return XLAT("flat") + dim_name();
    
    case gcSphere:
      return XLAT("spherical") + dim_name();

    case gcSolNIH:
#if CAP_SOLV
      switch(solnihv::geom()) {
        case gSol:
          return XLAT("Sol");
        case gNIH:
          return XLAT("hyperbolic (3:2)");
        case gSolN:
          return XLAT("Sol (3:2)");
        default:
          return "unknown";
        }
#else
      return XLAT("Sol");
#endif

    case gcNil:
      return XLAT("Nil");

    case gcSL2:
      return XLAT("~SL(2,R)~");

    case gcProduct:
      return XLAT("%1 x E", PIU(geometry_name()));
    }
  return "?";
  }

EX void select_quotient_screen() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);

  dialog::init(XLAT("quotient spaces in ") + ginf[geometry].tiling_name);
  char key = 'a';
  for(int i=0; i<isize(ginf); i++) {
    auto g = eGeometry(i);
    if(same_tiling(g)) {
      dialog::addBoolItem(
        (ginf[g].flags & qANYQ) ? 
           XLAT(ginf[g].menu_displayed_name) :
           "no quotient",
        g == geometry, key++);
      dialog::add_action([g] {
        if(g == gFieldQuotient) 
          pushScreen(showQuotientConfig);
        else {
          dual::may_split_or_do([g] { set_geometry(g); });
          start_game();
          }
        });
      }
    }
  
  dialog::addBack();
  dialog::display();
  }

EX void select_quotient() {
  if(euclid && !penrose && !arcm::in()) {
    euc::prepare_torus3();
    pushScreen(euc::show_torus3);
    }
  else if(nil) {
    nilv::prepare_niltorus3(),  
    pushScreen(nilv::show_niltorus3);
    }
  else if(asonov::in()) {
    asonov::prepare_config();
    pushScreen(asonov::show_config);
    }
  else if(prod)
    pushScreen(product::show_config);
  else {
    vector<eGeometry> choices;
    for(int i=0; i<isize(ginf); i++) if(same_tiling(eGeometry(i))) choices.push_back(eGeometry(i));
    
    println(hlog, "choices = ", choices);

    if(isize(choices) > 2) 
      pushScreen(select_quotient_screen);
    else if(isize(choices) > 1) {
      set_geometry(choices[choices[0] == geometry ? 1 : 0]);
      start_game();
      }
    else
      addMessage("No quotient spaces avialable in the current tiling.");
    }
  }

EX void showEuclideanMenu() {
  // for(int i=2; i<lt; i++) landvisited[i] = true;

  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);  

  dialog::init(XLAT("experiment with geometry"));

  dialog::addSelItem(XLAT("geometry"), geometry_name(), 'd');
  dialog::add_action([] { pushScreen(ge_select_tiling); pushScreen(ge_select_filter); });
  
  dialog::addSelItem(XLAT("basic tiling"), XLAT(ginf[geometry].tiling_name), 't');
  dialog::add_action([] {  
    if(!current_filter || !current_filter->test()) set_default_filter();
    pushScreen(ge_select_tiling);
    });

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
  
  if(euclid && bounded) {
    worldsize = euc::eu.det;
    if(BITRUNCATED) worldsize *= (a4 ? 2 : 3);
    if(GOLDBERG) worldsize *= cgi.gpdata->area;
    if(IRREGULAR) worldsize *= isize(irr::cells) / isize(irr::cells_of_heptagon);
    }
  else 
  worldsize = denom ? nom / denom : 0;
  
  if(euler < 0 && !bounded)
    worldsize = -worldsize;

  string spf = its(ts);
  if(0) ;
  #if CAP_ARCM
  else if(arcm::in()) {
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
  else if(BITRUNCATED)
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

  dialog::add_action(select_quotient);
  
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
  else if(binarytiling) {
    dialog::addSelItem(XLAT("width"), fts(vid.binary_width), 'v');
    dialog::add_action([] {
      dialog::editNumber(vid.binary_width, 0, 2, 0.1, 1, XLAT("binary tiling width"), "");
      dialog::reaction = [] () {
        #if CAP_TEXTURE
        texture::config.remap();
        #endif
        if(asonov::in()) asonov::prepare();
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
      else if(arcm::in()) arcm::next_variation();
      #endif
      else if(euc::in(2,4) || !CAP_GP) dialog::do_if_confirmed([] {
        set_variation(PURE ? eVariation::bitruncated : eVariation::pure);
        start_game();
        });
      #if CAP_GP
      else // if(S3 == 3) 
        gp::configure();
      #endif
      });
    }

  if(in_s2xe()) {
    dialog::addSelItem(XLAT("precision of S2xE rings"), its(s2xe::qrings), '5');
    dialog::add_action([] {
      dialog::editNumber(s2xe::qrings, 1, 256, 4, 32, XLAT("precision of S2xE rings"),
        XLAT(
          "In S2xE, objects at spherical distances which are multiples of π will look like "
          "rings, and objects close to these will look like crescents. "
          "This setting controls the quality of rendering these rings and crescents.")
        );
      dialog::bound_low(1);
      dialog::bound_up(256);
      });
    }
  
  if(hybri) {
    auto r = rots::underlying_scale;
    dialog::addSelItem(XLAT("view the underlying geometry"), r > 0 ? fts(r)+"x" : ONOFF(false), '6');
    dialog::add_action([] {
      dialog::editNumber(rots::underlying_scale, 0, 1, 0.05, 0.25, XLAT("view the underlying geometry"),
        XLAT(
          geometry == gRotSpace ? "The space you are currently in the space of rotations of the underlying hyperbolic or spherical geometry. "
            : "You are currently in a product space.") +
        XLAT(
          "This option lets you see the underlying space. Lands and some walls (e.g. in the Graveyard) are based on "
          "the respective features in the underlying world, but details such as monsters or items are ignored."
          )
        );
      dialog::bound_low(0);
      dialog::bound_up(1);
      dialog::extra_options = [] () { rots::draw_underlying(true); };
      });
    }
  
  dialog::addBreak(100);
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

  if(among(specialland, laMinefield, laCA) && geometry_has_alt_mine_rule()) {
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
  if(!euc::in(2,6)) fgname = gp::operation_name() + " " + fgname;
  
  dialog::addTitle(XLAT("info about: %1", fgname), 0xFFFFFF, 150);
  
  if(WDIM == 2) dialog::addSelItem(XLAT("faces per vertex"), spf, 0);

  dialog::addSelItem(XLAT("size of the world"), 
    #if CAP_BT
    binarytiling ? fts(8 * M_PI * sqrt(2) * log(2) / pow(vid.binary_width, WDIM-1), 4) + " exp(∞)" :
    #endif
    #if CAP_ARCM
    arcm::in() && (WDIM == 2) ? arcm::current.world_size() :
    (arcm::in() && sphere) ? its(isize(currentmap->allcells())) :
    #endif
    #if CAP_CRYSTAL
    cryst ? "∞^" + its(ts/2) :
    #endif
    WDIM == 3 && bounded ? its(isize(currentmap->allcells())) :
    WDIM == 3 && euclid ? "∞" :
    worldsize < 0 ? (nom%denom ? its(nom)+"/"+its(denom) : its(-worldsize)) + " exp(∞)": 
    (euclid && quotient && !bounded) ? "∞" :
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
  
  dialog::display();
  }

EX void runGeometryExperiments() {
  if(!geometry && specialland == laIce)
    specialland = getLandForList(cwt.at);
  pushScreen(showEuclideanMenu);
  }

#if CAP_COMMANDLINE

eGeometry readGeo(const string& ss) {
  for(int i=0; i<isize(ginf); i++) if(ginf[i].shortname == ss) return eGeometry(i);
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
  else if(argis("-fi-at")) {
    geometry = gNormal;
    shift(); dynamicval<int> s7(S7, argi());
    shift(); dynamicval<int> s3(S3, argi());
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
  else if(argis("-d:geom")) 
    launch_dialog(showEuclideanMenu);
  else return 1;
  return 0;
  }

auto ah_geom = addHook(hooks_args, 0, read_geom_args);
#endif

}
