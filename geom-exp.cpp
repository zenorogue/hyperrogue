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
int euperpage = 9;

string euchelp =
  "If you want to know how much the gameplay is affected by the "
  "hyperbolic geometry in HyperRogue, this mode is for you!\n\n"
  
  "You can try many different geometries here. We start by gluing "
  "n-gons in such a way that k of them meet in every vertex. "
  "Depending on n and k, this either folds into a sphere, unfolds into a plane, "
  "or requires a hyperbolic space. The result may be then 'bitruncated' by "
  "replacing each vertex by a 2k-gon. Furthermore, you can play "
  "with quotient geometries. For example, the elliptic geometry is "
  "obtained from the sphere by making the antipodes be the same point, "
  "so you return to the same spot (but as a mirror image) after going there. "
  "Have fun experimenting! "
  "Achievements and leaderboards do not work in geometry experiments, "
  "except some specific ones.\n\n"
  "In standard geometry (bitruncated or not), you can play the full game, but in other geometries "
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
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("field quotient"));
  fgeomextra& gxcur = fgeomextras[current_extra];
  for(int i=0; i<isize(fgeomextras); i++) {
    auto& g = fgeomextras[i];
    dialog::addBoolItem(ginf[g.base].tiling_name, g.base == gxcur.base, 'a'+i);
    dialog::add_action([i] { current_extra = i; });
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
    dialog::add_action([&gxcur, i] { gxcur.current_prime_id = i; });
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
  dialog::add_action([&gxcur] { nextPrime(gxcur); });
  
  dialog::addItem("activate", 'x');
  dialog::add_action_confirmed([&gxcur] {
    set_geometry(gxcur.base);
    enableFieldChange();
    set_geometry(gFieldQuotient);
    start_game();
    });  

  dialog::addItem("find alternate manifolds", 'y');
  dialog::add_action_confirmed([&gxcur] {
    set_geometry(gxcur.base);
    triplet_id = 0;
    enableFieldChange();
    set_geometry(gFieldQuotient);
    start_game();

    auto& cfp = currfp;
    auto triplets = cfp.find_triplets();
    pushScreen([triplets] {
      cmode = sm::SIDE | sm::MAYDARK;
      gamescreen();
      dialog::init(XLAT("alternate manifolds"));
      int id = 0;
      for(auto t: triplets) {
        dialog::addItem(XLAT("generators (%1,%2), size %3", its(t.i), its(t.j), its(t.size/S7)), 'a'+id);
        dialog::add_action([id] {
          stop_game();
          triplet_id = id;
          fieldpattern::enableFieldChange();
          start_game();
          });
        id++;
        }
      dialog::addBreak(100);
      dialog::addHelp(XLAT(
        "This option finds alternate solutions. For example, there are three {7,3} manifolds with 156 heptagons each (\"first Hurwitz triplet\").")
        );
      dialog::display();
      });
    });

  dialog::addItem("default", 'z');
  dialog::add_action_confirmed([] {
    set_geometry(gEuclid);
    fieldpattern::quotient_field_changed = false;
    set_geometry(gFieldQuotient);
    start_game();
    });
  
  dialog::addBack();
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
  
EX void ge_land_selection() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();

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

  dialog::init(XLAT("select the starting land"));
  if(dialog::infix != "") mouseovers = dialog::infix;
  
  generateLandList([] (eLand l) { 
    if(dialog::infix != "" && !dialog::hasInfix(linf[l].name)) return false;
    return !!(land_validity(l).flags & lv::appears_in_geom_exp); 
    });
  stable_sort(landlist.begin(), landlist.end(), [] (eLand l1, eLand l2) { return land_validity(l1).quality_level > land_validity(l2).quality_level; });
  
  dialog::start_list(900, 900, '1');
  
  for(auto& l: landlist) {
  
    string s = XLAT1(linf[l].name);

    if(landvisited[l]) {
      dialog::addBoolItem(s, l == specialland, dialog::list_fake_key++);
      }
    else {
      dialog::addSelItem(s, XLAT("(locked)"), dialog::list_fake_key++);
      }
    
    dialog::lastItem().color = linf[l].color;
    dialog::lastItem().value += validclasses[land_validity(l).quality_level];
    dialog::add_action([l] {
      if(landvisited[l]) dialog::do_if_confirmed(dual::mayboth([l] {
        stop_game_and_switch_mode(tactic::on ? rg::tactic : rg::nothing);
        firstland = specialland = l;
        if(l == laCanvas || l == laAsteroids || (land_validity(l).flags & lv::switch_to_single))
          land_structure = lsSingle;
        else if(among(l, laCrossroads, laCrossroads2))
          land_structure = lsNiceWalls;
        else if(among(l, laCrossroads4))
          land_structure = lsNoWalls;
        start_game();
        popScreen();
        }));
      });
    }
  dialog::end_list();

  dialog::addBreak(25);
  validity_info();    
  dialog::addBreak(25);
  
  dialog::addInfo(XLAT("press letters to search"));
  dual::add_choice();  
  dialog::addBack();
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);
    
    if(dialog::editInfix(uni)) dialog::list_skip = 0;
    else if(doexiton(sym, uni)) popScreen();
    };
  }

EX void activate_ge_land_selection() {
  dialog::infix = "";
  eupage = 0;
  pushScreen(ge_land_selection);
  }

#if HDR
struct geometry_filter {
  string name;
  int hotkey;
  /** test if the current geometry matches the filter */
  function<bool()> test; 
  };
#endif

EX geometry_filter *current_filter;

bool forced_quotient() { return quotient && !(cgflags & qOPTQ); }

EX geometry_filter gf_hyperbolic = {"hyperbolic", 'h', [] { return (arcm::in() || arb::in() || hyperbolic) && !forced_quotient(); }};
EX geometry_filter gf_spherical = {"spherical", 's', [] { return (arcm::in() || arb::in() || sphere) && !forced_quotient(); }};
EX geometry_filter gf_euclidean = {"Euclidean", 'e', [] { return (arcm::in() || arb::in() || euclid) && !forced_quotient(); }};
EX geometry_filter gf_other = {"non-isotropic", 'n', [] { return prod || nonisotropic; }};
EX geometry_filter gf_regular_2d = {"regular 2D tesselations", 'r', [] { 
  return standard_tiling() && WDIM == 2 && !forced_quotient();
  }};
EX geometry_filter gf_regular_3d = {"regular 3D honeycombs", '3', [] { 
  if(euclid) return geometry == gCubeTiling;
  return !bt::in() && !kite::in() && WDIM == 3 && !forced_quotient() && !nonisotropic && !prod;
  }};
EX geometry_filter gf_quotient = {"interesting quotient spaces", 'q', [] { 
  return forced_quotient() && !elliptic;
  }};
EX geometry_filter gf_tes_file = {"load from file", 'f', [] { return arb::in(); }};
EX geometry_filter gf_no_filters = {"no filters", '-', [] { return true; }};
  
EX vector<geometry_filter*> available_filters = { &gf_hyperbolic, &gf_spherical, &gf_euclidean, &gf_other, &gf_regular_2d, &gf_regular_3d, &gf_quotient, &gf_tes_file, &gf_no_filters };

void ge_select_filter() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();

  dialog::init(XLAT("geometries"));
  
  for(auto f: available_filters) {
    if(current_filter)
      dialog::addBoolItem(XLAT(f->name), f == current_filter, f->hotkey);
    else
      dialog::addItem(XLAT(f->name), f->hotkey);
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
  else if(g == gArbitrary)
    arb::choose();
  else {
    if(among(g, gProduct, gRotSpace)) {
      if(WDIM == 3 || (g == gRotSpace && euclid)) {
        addMessage(
          g == gRotSpace ?
            XLAT("Only works with 2D non-Euclidean geometries")
          : XLAT("Only works with 2D geometries")
            );
        return;
        }
      if(g == gRotSpace) {
        bool ok = true;
        if(arcm::in()) ok = PURE;
        else if(bt::in() || kite::in()) ok = false;
        else ok = PURE || BITRUNCATED;
        if(!ok) {
          addMessage(XLAT("Only works with (semi-)regular tilings"));
          return;
          }
        #if CAP_ARCM
        if(arcm::in()) {
          int steps, single_step;
          if(!arcm::current.get_step_values(steps, single_step)) {
            addMessage(XLAT("That would have %1/%2 levels", its(steps), its(single_step)));
            return;
            }
          }
        #endif
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
  if(g2 == gFieldQuotient && (hyperbolic || (geometry == gCubeTiling && reg3::cubes_reg3)) && standard_tiling())
    return true;
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
  if(current_filter == &gf_tes_file) { popScreen(); set_or_configure_geometry(gArbitrary); }
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();

  if(!current_filter) { popScreen(); return; }
  dialog::init();
  dialog::addItem(XLAT(current_filter->name), 'x');
  dialog::add_action_push(ge_select_filter);
  
  vector<eGeometry> geometries;

  dialog::addBreak(100);
  
  dialog::start_list(500, 1500, 'a');
  
  for(int i=0; i<isize(ginf); i++) {
    eGeometry g = eGeometry(i);
    if(among(g, gProduct, gRotSpace)) hybrid::configure(g);
    bool orig_el = elliptic;
    bool on = geometry == g;
    bool in_2d = WDIM == 2;
    dynamicval<eGeometry> cg(geometry, g);
    if(cgflags & qDEPRECATED) continue;
    if(arcm::in() && !CAP_ARCM) continue;
    if(cryst && !CAP_CRYSTAL) continue;
    if(sol && !CAP_SOLV) continue;
    if(arb::in() && (ISMOBILE || ISWEB)) continue;
    if(WDIM == 3 && MAXMDIM == 3) continue;
    if(geometry == gFieldQuotient && !CAP_FIELD) continue;
    if(geometry == gFake) continue;
    if(!current_filter->test()) continue;
    if(orig_el) {
      for(int j=0; j<isize(ginf); j++) 
        if(ginf[j].tiling_name == ginf[i].tiling_name)
          geometry = g = eGeometry(j);
      }
    
    bool is_product = (geometry == gProduct && in_2d);
    bool is_rotspace = (geometry == gRotSpace && in_2d);
    dialog::addBoolItem(
      is_product ? XLAT("current geometry x E") : 
      is_rotspace ? XLAT("space of rotations in current geometry") : 
      XLAT(ginf[g].menu_displayed_name), on, dialog::list_fake_key++);
    dialog::lastItem().value += validclasses[land_validity(specialland).quality_level];
    dialog::add_action([g] { set_or_configure_geometry(g); });
    }
  
  dialog::end_list();
  dialog::addBreak(100);
  dual::add_choice();  
  dialog::addBack();
  dialog::display();
  }

EX string current_proj_name() {
  bool h = hyperbolic || sn::in();
  if(vpconf.model == mdPanini && vpconf.alpha == 1)
    return XLAT("stereographic Panini");
  else if(vpconf.model != mdDisk)
    return models::get_model_name(vpconf.model);
  else if(h && vpconf.alpha == 1)
    return XLAT("Poincaré model");
  else if(h && vpconf.alpha == 0)
    return XLAT("Klein-Beltrami model");
  else if(h && vpconf.alpha == -1)
    return XLAT("inverted Poincaré model");
  else if(sphere && vpconf.alpha == 1)
    return XLAT("stereographic projection");
  else if(sphere && vpconf.alpha == 0)
    return XLAT("gnomonic projection");
  else if(sphere && vpconf.alpha >= 999)
    return XLAT("orthographic projection");
  else if(h && vpconf.alpha >= 999)
    return XLAT("Gans model");
  else 
    return XLAT("general perspective");
  }

EX string dim_name() {
  return " (" + its(WDIM) + "D)";
  }

#if CAP_THREAD && MAXMDIM >= 4
EX void showQuotientConfig3() {

  using namespace fieldpattern;
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("field quotient"));
  
  auto& ds = discoveries[cginf.tiling_name];
  
  if(!ds.discoverer) {
    dialog::addItem("start discovery", 's');
    dialog::add_action([&ds] { ds.activate(); });
    }
  else if(ds.is_suspended) {
    dialog::addItem("resume discovery", 's');
    dialog::add_action([&ds] { ds.activate(); });
    }
  else {
    dialog::addItem("suspend discovery", 's');
    dialog::add_action([&ds] { ds.suspend(); });
    }

  auto& e = ds.experiment;
  if(!e.Prime)
    dialog::addBreak(100);
  else {
    string s = its(e.Prime);
    if(e.wsquare) s += "²";
    dialog::addInfo(s);
    }
    
  dialog::addBreak(100);

  if(1) {
    std::unique_lock<std::mutex> lk(ds.lock);
    auto&l = ds.hashes_found;
    for(auto& v: l) {
      char x = 'a';
      string s = XLAT("#%1, cells: %2, p=%3", itsh(v.first), its(get<5>(v.second)), its(get<0>(v.second)) + (get<1>(v.second) ? "²" : ""));
      dialog::addItem(s, x++);
      dialog::add_action([&v] {
        stop_game();
        int tmp;
        tie(currfp.Prime, currfp.wsquare, currfp.R, currfp.P, currfp.X, tmp) = v.second;
        currfp.Field = currfp.wsquare ? currfp.Prime * currfp.Prime : currfp.Prime;
        currfp.generate_all3();
        currfp.analyze();
        start_game();
        });
      }
    }
    
  dialog::addBreak(100);
  
  dialog::addBack();
  
  dialog::display();
  }
#endif

EX string geometry_name() {
  switch(ginf[geometry].cclass) {
    case gcHyperbolic:
      return XLAT("hyperbolic") + dim_name();

    case gcEuclid: 
      if(cgflags & qAFFINE)
        return XLAT("affine") + dim_name();
      return XLAT("flat") + dim_name();
    
    case gcSphere:
      return XLAT("spherical") + dim_name();

    case gcSolNIH:
#if CAP_SOLV
      switch(sn::geom()) {
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
  gamescreen();

  dialog::init(XLAT("quotient spaces in ") + ginf[geometry].tiling_name);
  char key = 'a';
  for(int i=0; i<isize(ginf); i++) {
    auto g = eGeometry(i);
    if(ginf[g].flags & qDEPRECATED) continue;
    if(same_tiling(g)) {
      dialog::addBoolItem(
        (ginf[g].flags & qANYQ) ? 
           XLAT(ginf[g].menu_displayed_name) :
           "no quotient",
        g == geometry, key++);
      dialog::add_action([g] {
        if(g == gFieldQuotient && WDIM == 3) {
          if(geometry != gFieldQuotient) {
            stop_game();
            fieldpattern::field_from_current();
            set_geometry(gFieldQuotient);
            for(int p=2;; p++) { currfp.Prime = p; currfp.force_hash = 0; if(!currfp.solve()) break; }
            println(hlog, "set prime = ", currfp.Prime);
            start_game();
            }
          #if CAP_THREAD && MAXMDIM >= 4
          pushScreen(showQuotientConfig3);
          #endif
          }
        else if(g == gFieldQuotient) 
          pushScreen(showQuotientConfig);
        #if CAP_CRYSTAL
        else if(g == gCrystal)
          pushScreen(crystal::show);
        #endif
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
  if(euclid && !kite::in() && !arcm::in() && !reg3::cubes_reg3) {
    euc::prepare_torus3();
    pushScreen(euc::show_torus3);
    }
  else if(nil) {
    nilv::prepare_niltorus3(),  
    pushScreen(nilv::show_niltorus3);
    }
  #if CAP_SOLV
  else if(asonov::in()) {
    asonov::prepare_config();
    pushScreen(asonov::show_config);
    }
  #endif
  else if(prod)
    pushScreen(product::show_config);
  else if(rotspace)
    hybrid::configure_period();
  else {
    vector<eGeometry> choices;
    for(int i=0; i<isize(ginf); i++) if(same_tiling(eGeometry(i))) choices.push_back(eGeometry(i));
    
    println(hlog, "choices = ", choices);

    if(isize(choices) > 1) 
      pushScreen(select_quotient_screen);
    /* else if(isize(choices) > 1) {
      set_geometry(choices[choices[0] == geometry ? 1 : 0]);
      start_game();
      } */
    else
      addMessage("No quotient spaces available in the current tiling.");
    }
  }

EX string full_geometry_name() {
  string qstring = ginf[geometry].quotient_name;
  bool variable =
    !(prod || hybri || bt::in() || (WDIM == 3 && !reg3::in()) || kite::in() || arb::in());
  
  string fgname = XLAT(ginf[geometry].tiling_name);
  if(qstring != "none") fgname += " " + XLAT(qstring);
  #if CAP_ARCM
  if(arcm::in()) fgname = arcm::current.symbol;
  #endif
  if(variable) fgname = gp::operation_name() + " " + fgname;
  return fgname;
  }

void action_change_variation() {
  if(0) ;
  #if CAP_ARCM
  else if(arcm::in()) arcm::next_variation();
  #endif
  #if MAXMDIM >= 4
  else if(reg3::in() || geometry == gCubeTiling) reg3::configure_variation();
  #endif
  else if(euc::in(2,4) || !CAP_GP) dialog::do_if_confirmed([] {
    set_variation(PURE ? eVariation::bitruncated : eVariation::pure);
    start_game();
    });
  #if CAP_GP
  else // if(S3 == 3) 
    gp::configure();
  #endif
  }

EX void menuitem_change_variation(char key) {
  dialog::addSelItem(XLAT("variations"), gp::operation_name(), key);    
  dialog::add_action(action_change_variation);
  }

EX void menuitem_change_geometry(char key) {
  dialog::addSelItem(XLAT("geometry/topology/tiling"), full_geometry_name(), key);
  dialog::add_action_push(current_filter ? ge_select_tiling : ge_select_filter);
  }

EX void menuitem_projection(char key) {
  dialog::addSelItem(XLAT("projection"), current_proj_name(), key);
  dialog::add_action_push(models::model_menu);
  }

EX void menuitem_binary_width(char key) {
  dialog::addSelItem(XLAT("binary tiling width"), fts(vid.binary_width), key);
  dialog::add_action([] {
    dialog::editNumber(vid.binary_width, 0, 2, 0.1, 1, XLAT("binary tiling width"), "");
    dialog::reaction = [] () {
      #if CAP_TEXTURE
      texture::config.remap();
      #endif
      #if CAP_SOLV
      if(asonov::in()) asonov::prepare();
      #endif
      };
    });
  }

EX void menuitem_nilwidth(char key) {
  dialog::addSelItem(XLAT("Nil width"), fts(nilv::nilwidth), key);
  dialog::add_action([] {
    dialog::editNumber(nilv::nilwidth, 0.01, 2, 0.1, 1, XLAT("Nil width"), "");
    dialog::reaction = ray::reset_raycaster;
    dialog::bound_low(0.01);
    });
  }

EX void edit_stretch() {
  stretch::mstretch = false;
  ray::reset_raycaster();
  dialog::editNumber(stretch::factor, -1, 9, 0.1, 0, XLAT("stretched geometry"),
    XLAT(
      "Stretch the metric along the fibers. This can currently be done in rotation spaces and in 8-cell, 24-cell and 120-cell. "
      "Value of 0 means not stretched, -1 means S2xE or H2xE (works only in the limit). (Must be > -1)"
      )
    );
  dialog::reaction = [] { if(abs(stretch::factor+1) < 1e-3) stretch::factor = -.9; ray::reset_raycaster(); };
  }

#if HDR
struct geometry_data {
  int ts, tv, nom, denom, euler, demigenus, worldsize;
  int area;
  string spf;
  string size_str;
  };
#endif

EX geometry_data compute_geometry_data() {
  geometry_data gd;
  auto& ts = gd.ts;
  auto& tv = gd.tv;
  ts = ginf[geometry].sides;
  tv = ginf[geometry].vertex;
  gd.nom = (BITRUNCATED ? tv+ts : tv) * 4;
  gd.denom = (2*ts + 2*tv - ts * tv);
  
  #if CAP_GP
  if(GOLDBERG || INVERSE) {
    gd.area = PIU(cgi.gpdata->area);

    if(GOLDBERG || WARPED) {
      gd.nom = 2 * (2*tv + (S3-2) * ts * (gd.area-1));
      }
    else if(UNRECTIFIED) {
      if((gp::param.first + gp::param.second) % 2 == 0)
        gd.nom = ts * 2 * gd.area;
      else
        gd.nom = (2*tv + (S3-2) * ts * (gd.area-1));
      }
    else if(UNTRUNCATED) {
      if((gp::param.first - gp::param.second) % 3 == 0) {
        gd.nom = ts * 4 * gd.area;
        gd.denom *= 3;
        }
      else {
        gd.nom = 2 * (2*tv + (S3-2) * ts * (gd.area-1));
        gd.denom *= 3;
        }
      }
    }
  else
  #endif
  gd.area = PURE ? 1 : 3;

  gd.euler = 0;
  if(euclid) gd.euler = 0;
  else if(sphere && nonorientable) gd.euler = 1;
  else if(sphere) gd.euler = 2;
  else if(!closed_manifold) gd.euler = -2;
  else if(WDIM == 3) gd.euler = 0;
  else switch(geometry) {
    case gFieldQuotient:
      gd.worldsize = isize(currentmap->allcells());
      gd.euler = 2 * gd.worldsize * gd.denom / gd.nom;
      break;
    
    case gMinimal:
      gd.euler = -1;
      break;
    
    case gZebraQuotient:
    case gBolza:
      gd.euler = -2;
      break;
    
    case gKleinQuartic:
    case gSchmutzM2:
    case gBolza2:
      gd.euler = -4;
      break;
    
    case gSchmutzM3:
    case gBring:
      gd.euler = -6;
      break;
    
    case gMacbeath:
      gd.euler = -12;
      break;
    
    default: 
      gd.worldsize = isize(currentmap->allcells());
      println(hlog, "warning: Euler characteristics unknown, worldsize = ", gd.worldsize);
      gd.euler = 2 * gd.worldsize * gd.denom / gd.nom;
      break;
    }
  
  gd.nom *= gd.euler;
  gd.denom *= 2;
        
  if(hybri) gd.nom *= hybrid::csteps, gd.denom *= cgi.single_step;

  int g = gcd(gd.nom, gd.denom);
  if(g) {
    gd.nom /= g;
    gd.denom /= g;
    }
  
  if(euclid && closed_manifold) {
    gd.worldsize = euc::eu.det;
    if(BITRUNCATED) gd.worldsize *= (a4 ? 2 : 3);
    if(GOLDBERG) gd.worldsize *= cgi.gpdata->area;
    #if CAP_IRR
    if(IRREGULAR) gd.worldsize *= isize(irr::cells) / isize(irr::cells_of_heptagon);
    #endif
    }
  else 
  gd.worldsize = gd.denom ? gd.nom / gd.denom : 0;
  
  if(gd.euler < 0 && !closed_manifold)
    gd.worldsize = -gd.worldsize;

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
  else if(bt::in()) switch(geometry) {
    case gBinaryTiling: spf = "6,[6,7],7"; break;
    case gBinary4: spf = "5,5,5[,5]"; break;
    case gTernary: spf = "6,6,6[,6]"; break;
    default: spf = "?"; break;
    }
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
  else if(UNRECTIFIED || UNTRUNCATED) {
    if(UNRECTIFIED && (gp::param.first-gp::param.second) % 2 != 0)
      spf = "(?)";
    else if(UNTRUNCATED && (gp::param.first-gp::param.second) % 3 != 0)
      spf = "(?)";
    else {      
      string each = UNRECTIFIED ? "4" : "3";
      int first = UNRECTIFIED ? 4 : 6;
      int second = S7;
      if(gp::param == gp::loc(1, 1))
        first = second;
      else if(second < first)
        swap(first, second);
      spf = each;
      for(int z=1; z<first; z++) spf += "," + each;
      if(first != second) {
        spf += "[";
        for(int z=first; z<second; z++) spf += "," + each;
        spf += "]";
        }
      }
    }
  else if(GOLDBERG && S3 == 4 && gp::param == gp::loc(1, 1))
    spf = spf + ",4," + spf + ",4";
  else if(GOLDBERG && S3 == 4 && gp::param == gp::loc(2, 0))
    spf = spf + ",4,4,4";
  else if(GOLDBERG && S3 == 4)
    spf = "[" + spf + ",4],4,4,4";
  else if(WARPED && S3 == 3 && gp::param == gp::loc(1,1))
    spf = spf + ",3,3";
  else if(WARPED && S3 == 3)
    spf = "[" + spf + ",6],3,3";
  else if(GOLDBERG && S3 == 3)
    spf = "[" + spf + ",6],6,6";
  #endif
  else {
    string spf0 = spf;
    for(int z=1; z<S3; z++) spf = spf + "," + spf0;
    }

  gd.size_str =
    disksize ? its(isize(currentmap->allcells())) :
    #if CAP_BT
    bt::in() ? fts(8 * M_PI * sqrt(2) * log(2) / pow(vid.binary_width, WDIM-1), 4) + " exp(∞)" :
    #endif
    #if CAP_ARCM
    arcm::in() && (WDIM == 2) ? arcm::current.world_size() :
    (arcm::in() && sphere) ? its(isize(currentmap->allcells())) :
    #endif
    #if CAP_CRYSTAL
    cryst ? "∞^" + its(ts/2) :
    #endif
    WDIM == 3 && closed_manifold ? its(isize(currentmap->allcells())) :
    WDIM == 3 && euclid ? "∞" :
    gd.worldsize < 0 ? (gd.nom%gd.denom ? its(gd.nom)+"/"+its(gd.denom) : its(-gd.worldsize)) + " exp(∞)": 
    (euclid && quotient && !closed_manifold) ? "∞" :
    gd.worldsize == 0 ? "∞²" :
    its(gd.worldsize);

  #if CAP_IRR
  if(hyperbolic && IRREGULAR) {
    gd.nom = isize(irr::cells);
    // both Klein Quartic and Bolza2 are double the Zebra quotiennt
    gd.denom = -2;
    if(!quotient) gd.worldsize = gd.nom / gd.denom;
    }
  #endif

  if(WDIM == 3) gd.euler = 0;
  gd.demigenus = 2 - gd.euler;

  return gd;
  }

EX void showEuclideanMenu() {
  // for(int i=2; i<lt; i++) landvisited[i] = true;

  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();

  dialog::init(XLAT("experiment with geometry"));

  dialog::addSelItem(XLAT("geometry"), geometry_name(), 'd');
  dialog::add_action([] { pushScreen(ge_select_tiling); pushScreen(ge_select_filter); });

  dialog::addSelItem(XLAT("basic tiling"), XLAT(ginf[geometry].tiling_name), 't');
  dialog::add_action([] {
    if(!current_filter || !current_filter->test()) set_default_filter();
    pushScreen(ge_select_tiling);
    });

  string qstring = ginf[geometry].quotient_name;

  if(qstring == "none")
    dialog::addBoolItem(XLAT("quotient space"), false, 'q');
  else
    dialog::addSelItem(XLAT("quotient space"), XLAT(qstring), 'q');

  dialog::add_action(select_quotient);
  
  #if CAP_ARCM
  if(arcm::in()) {
    dialog::addItem(XLAT("advanced parameters"), '4');
    dialog::add_action_push(arcm::show);
    }
  #endif

  #if CAP_CRYSTAL
  if(cryst) {
    dialog::addItem(XLAT("advanced parameters"), '4');
    dialog::add_action_push(crystal::show);
    }
  #endif
  
  if(fake::available()) {
    dialog::addItem(XLAT("fake curvature"), '4');
    
    dialog::add_action([] {
      if(fake::in()) fake::configure();
      else dialog::cheat_if_confirmed(
        fake::configure
        );
      });
    }
  
  if(arb::in() && !(arb::current.sliders.empty() && arb::current.intsliders.empty())) {
    dialog::addItem(XLAT("tessellation sliders"), '4');
    dialog::add_action_push(arb::set_sliders);
    }
  if(arb::is_apeirogonal()) add_edit(arb::apeirogon_simplified_display);
  if(arb::in()) add_edit(debug_tiles);
  
  #if MAXMDIM >= 4
  if(cgflags & qULTRA) {
    dialog::addBoolItem(XLAT("truncate ultra-vertices with mirrors"), reg3::ultra_mirror_on, 'Z');
    dialog::add_action([] { 
      reg3::ultra_mirror_on = !reg3::ultra_mirror_on;
      ray::reset_raycaster();
      });
    }
  #endif
  
  if(prod) {
    dialog::addSelItem(XLAT("Z-level height factor"), fts(vid.plevel_factor), 'Z');
    dialog::add_action([] {
      dialog::editNumber(vid.plevel_factor, 0, 2, 0.1, 0.7, XLAT("Z-level height factor"), "");
      dialog::reaction = ray::reset_raycaster;
      });
    }
  else if(hybri) {
    dialog::addSelItem(XLAT("number of levels"), its(hybrid::csteps / cgi.single_step), 'L');
    dialog::add_action(hybrid::configure_period);
    }
  else if(bt::in()) {
    menuitem_binary_width('v');
    add_edit_wall_quality('W');
    }
  else if(nil) {
    menuitem_nilwidth('v');
    }
  else if((WDIM == 3 || kite::in() || arb::in()) && !reg3::in() && geometry != gCubeTiling) dialog::addBreak(100);
  else 
    menuitem_change_variation('v');

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
  
  #if MAXMDIM >= 4
  if(hybri) {
    auto r = rots::underlying_scale;
    dialog::addSelItem(XLAT("view the underlying geometry"), r > 0 ? fts(r)+"x" : ONOFF(false), '6');
    dialog::add_action([] {
      dialog::editNumber(rots::underlying_scale, 0, 1, 0.05, 0.25, XLAT("view the underlying geometry"),        
        geometry == gRotSpace ? 
          XLAT("The space you are currently in is the space of rotations of the underlying hyperbolic or spherical geometry. ")
        : XLAT("You are currently in a product space.") +
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
  #endif
  
  if(stretch::applicable()) {
    dialog::addSelItem(XLAT("stretched geometry"), fts(stretch::factor), 'S');
    dialog::add_action(edit_stretch);
    }
  
  dialog::addBreak(100);
  menuitem_land_structure('l');
  add_edit(req_disksize);
  
  if(specialland == laMinefield && closed_or_bounded) {
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

  if(geometry_has_alt_mine_rule()) {
    dialog::addSelItem(XLAT("adjacency rule"), mine_adjacency_rule ? XLAT("vertex") : WDIM == 3 ? XLAT("face") : XLAT("edge"), 'M');
    dialog::add_action([] {
      stop_game();
      mine_adjacency_rule = !mine_adjacency_rule;
      start_game();
      addMessage(XLAT("Note: adjacency rule affects environmental effects, but not movement."));
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
  menuitem_projection('1');
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

  dialog::addTitle(XLAT("info about: %1", full_geometry_name()), 0xFFFFFF, 150);
  
  auto gd = compute_geometry_data();
  if(WDIM == 2 && !arb::in() && !kite::in()) dialog::addSelItem(XLAT("faces per vertex"), gd.spf, 0);
  
  if(arb::in() && arb::current.comment != "") {
    dialog::addBreak(100);
    dialog::addHelp(arb::current.comment);
    }

  dialog::addSelItem(XLAT("size of the world"), gd.size_str, '3');
  
  if(WDIM == 2 || reg3::exact_rules()) dialog::add_action([] {
    if(!viewdists) { enable_viewdists(); pushScreen(viewdist_configure_dialog); }
    else if(viewdists) viewdists = false;
    });

  if(closed_manifold) {
    dialog::addSelItem(XLAT("Euler characteristics"), its(gd.euler), 0);
    if(WDIM == 3) ;
    else if(nonorientable)
      dialog::addSelItem(XLAT("demigenus"), its(gd.demigenus), 0);
    else
      dialog::addSelItem(XLAT("genus"), its(gd.demigenus/2), 0);
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

EX eGeometry readGeo(const string& ss) {
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

EX void field_quotient_3d(int p, unsigned hash) {
  check_cgi();
  cgi.require_basics();
  stop_game_and_switch_mode(rg::nothing);
  fieldpattern::field_from_current();
  set_geometry(gFieldQuotient);
  for(;; p++) { 
    println(hlog, "trying p = ", p);
    currfp.Prime = p; currfp.force_hash = hash; if(!currfp.solve()) break; 
    }
  println(hlog, "set prime = ", currfp.Prime);
  }

EX void field_quotient_2d(int group, int id, int triplet) {
  using namespace fieldpattern;
  current_extra = group;

  auto& gxcur = fgeomextras[current_extra];
  while(id >= isize(gxcur.primes)) nextPrime(gxcur);

  fgeomextras[current_extra].current_prime_id = id;
  if(triplet != -1)
    triplet_id = triplet;
  enableFieldChange();
  set_geometry(gFieldQuotient);
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
    field_quotient_2d(a, b, -1);
    }
  else if(argis("-triplet")) {
    stop_game();
    shift(); fieldpattern::triplet_id = argi();
    fieldpattern::enableFieldChange();
    }
  else if(argis("-to-fq")) {
    int p = 2;
    shift();
    if(args() == "p") {
      shift(); p = argi();
      shift(); 
      }
    unsigned hash = arghex();
    field_quotient_3d(p, hash);
    }
  else if(argis("-cs")) {
    shift(); cheat();
    fieldpattern::matrix M = currfp.strtomatrix(args());
    fieldpattern::subpathid = currfp.matcode[M];
    fieldpattern::subpathorder = currfp.order(M);
    }
  else if(argis("-fwrite")) {
    shstream hs;
    hwrite_fpattern(hs, currfp);
    println(hlog, "current fieldpattern hash: ", currfp.hashv);
    println(hlog, "current fieldpattern: ", as_cstring(hs.s));
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
  else if(argis("-gph")) {
    PHASEFROM(2);
    shift(); int x = argi();
    shift(); int y = argi();
    gp::whirl_set({x, y});
    }
  else if(argis("-unrectified")) {
    PHASEFROM(2);
    set_variation(eVariation::unrectified);
    }
  else if(argis("-untruncated")) {
    PHASEFROM(2);
    set_variation(eVariation::untruncated);
    }
  else if(argis("-warped")) {
    PHASEFROM(2);
    set_variation(eVariation::warped);
    }
  #if MAXMDIM >= 4
  else if(argis("-subcubes")) {
    PHASEFROM(2);
    stop_game();
    set_variation(eVariation::subcubes);
    shift(); reg3::subcube_count = argi();
    }
  else if(argis("-dual-subcubes")) {
    PHASEFROM(2);
    stop_game();
    set_variation(eVariation::dual_subcubes);
    shift(); reg3::subcube_count = argi();
    }
  else if(argis("-bch-subcubes")) {
    PHASEFROM(2);
    stop_game();
    set_variation(eVariation::bch);
    shift(); reg3::subcube_count = argi();
    }
  else if(argis("-bch-oct")) {
    PHASEFROM(2);
    stop_game();
    set_variation(eVariation::bch_oct);
    shift(); reg3::subcube_count = argi();
    }
  else if(argis("-coxeter")) {
    PHASEFROM(2);
    stop_game();
    set_variation(eVariation::coxeter);
    shift(); reg3::coxeter_param = argi();
    }
  #endif
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
  else if(argis("-fi-geo")) {
    fieldpattern::info();
    exit(0);
    } 
  else if(argis("-qs")) {
    cheat();
    shift(); currfp.qpaths.push_back(args());
    }
  #if MAXMDIM >= 4
  else if(argis("-truncate-ultra")) {
    shift(); reg3::ultra_mirror_on = argi();
    }
  #endif
  else if(argis("-d:quotient")) 
    launch_dialog(showQuotientConfig);
  else if(argis("-uqf")) 
    fieldpattern::use_quotient_fp = true;
  #endif
  else if(argis("-d:geom")) 
    launch_dialog(showEuclideanMenu);
  else return 1;
  return 0;
  }

auto ah_geom = addHook(hooks_args, 0, read_geom_args);
#endif

}
