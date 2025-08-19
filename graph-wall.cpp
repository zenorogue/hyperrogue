// Hyperbolic Rogue -- terrain graphics file
// Copyright (C) 2011-2025 Zeno Rogue, see 'hyper.cpp' for details

#include "hyper.h"
namespace hr {

EX bool camelotcheat;
EX bool shadingcheat;

EX colortable minecolors = {
  0xFFFFFF, 0xF0, 0xF060, 0xF00000, 
  0x60, 0x600000, 0x00C0C0, 0x000000, 0x808080, 0xFFD500
  };

EX colortable distcolors = {
  0xFFFFFF, 0xF0, 0xF060, 0xF00000, 
  0xA0A000, 0xA000A0, 0x00A0A0, 0xFFD500
  };

EX const char* minetexts[8] = {
  "No mines next to you.",
  "A mine is next to you!",
  "Two mines next to you!",
  "Three mines next to you!",
  "Four mines next to you!",
  "Five mines next to you!",
  "Six mines next to you!",
  "Seven mines next to you!"
  };

EX map<cell*, int> fake_minecount;

EX int countMinesAround(cell *c) {
  if(fake_minecount.count(c)) return fake_minecount[c];
  int mines = 0;
  for(cell *c2: adj_minefield_cells(c))
    if(c2->wall == waMineMine)
      mines++;
  return mines;
  }

EX transmatrix applyPatterndir(cell *c, const patterns::patterninfo& si) {
  if(NONSTDVAR || bt::in() || cgi.emb->is_euc_in_noniso()) return Id;
  transmatrix V = ddspin180(c, si.dir);
  if(si.reflect) V = V * lmirror();
  if(euclid) return V;
  return V * iddspin180(c, 0);
  }

EX transmatrix applyDowndir(cell *c, const cellfunction& cf) {
  return ddspin180(c, patterns::downdir(c, cf));
  }

EX color_t reptilecolor(cell *c) {
  int i;
  
  if(arcm::in())
    i = c->master->rval0 & 3;
  else {
    i = zebra40(c);
    
    if(!euclid) {
      if(i >= 4 && i < 16) i = 0; 
      else if(i >= 16 && i < 28) i = 1;
      else if(i >= 28 && i < 40) i = 2;
      else i = 3;
      }
    }

  color_t reptilecolors[4] = {0xe3bb97, 0xc2d1b0, 0xebe5cb, 0xA0A0A0};
  return reptilecolors[i];
  }

// Color components in nestcolors must be less than 0x80 (for addition in drawMonster for Rock Snakes)
// and must be divisible by 4 (for brightening of raised cells in celldrawer::setcolors)
EX colortable nestcolors = { 0x7C0000, 0x007C00, 0x00007C, 0x404040, 0x700070, 0x007070, 0x707000, 0x606060 };

color_t floorcolors[landtypes];

EX void init_floorcolors() {
  for(int i=0; i<landtypes; i++)
    floorcolors[i] = linf[i].color;

  floorcolors[laDesert] = 0xEDC9AF;
  floorcolors[laKraken] = 0x20A020;
  floorcolors[laDocks] = 0x202020;
  floorcolors[laCA] = 0x404040;
  floorcolors[laMotion] = 0xF0F000;
  floorcolors[laGraveyard] = 0x107010;
  floorcolors[laWineyard] = 0x006000;
  floorcolors[laLivefjord] = 0x306030;
    
  floorcolors[laMinefield] = 0x80A080; 
  floorcolors[laCaribbean] = 0x006000;

  floorcolors[laAlchemist] = 0x202020;

  floorcolors[laRlyeh] = 0x004080;
  floorcolors[laHell] = 0xC00000;
  floorcolors[laCrossroads] = 0xFF0000;
  floorcolors[laJungle] = 0x008000;

  floorcolors[laZebra] = 0xE0E0E0;

  floorcolors[laCaves] = 0x202020;
  floorcolors[laEmerald] = 0x202020;
  floorcolors[laDeadCaves] = 0x202020;

  floorcolors[laPalace] = 0x806020;
  
  floorcolors[laHunting] = 0x40E0D0 / 2;

  floorcolors[laBlizzard] = 0x5050C0;
  floorcolors[laCocytus] = 0x80C0FF;
  floorcolors[laIce] = 0x8080FF;
  floorcolors[laCamelot] = 0xA0A0A0;

  floorcolors[laOvergrown] = 0x00C020;
  floorcolors[laClearing] = 0x60E080;
  floorcolors[laHaunted] = 0x609F60;
  floorcolors[laCursed] = 0x481848;
  floorcolors[laDice] = 0xC0C0FF;

  floorcolors[laMirror] = floorcolors[laMirrorWall] = floorcolors[laMirrorOld] = 0x808080;  
  }

EX color_t magma_color(int id) {
  if(id == 95/4-1) return 0x200000;
  else if(id == 95/4) return 0x100000;
  else if(id < 48/4) return gradient(0xF0F000, 0xF00000, 0, id, 48/4);
  else if(id < 96/4) return gradient(0xF00000, 0x400000, 48/4, id, 95/4-2);
  else return winf[waMagma].color;
  }

bool noAdjacentChasms(cell *c) {
  forCellEx(c2, c) if(c2->wall == waChasm) return false;
  return true;
  }

#if CAP_SHAPES
EX void floorShadow(cell *c, const shiftmatrix& V, color_t col) {
  if(model_needs_depth() || noshadow) 
    return; // shadows break the depth testing
  dynamicval<color_t> p(poly_outline, OUTLINE_TRANS);
  if(qfi.shape) {
    queuepolyat(V * qfi.spin * cgi.shadowmulmatrix, *qfi.shape, col, PPR::WALLSHADOW);
    }
  else if(qfi.usershape >= 0)
    mapeditor::drawUserShape(V * qfi.spin * cgi.shadowmulmatrix, mapeditor::sgFloor, qfi.usershape, col, c, PPR::WALLSHADOW);
  else 
    draw_shapevec(c, V, qfi.fshape->shadow, col, PPR::WALLSHADOW);
  }

EX bool use_warp_graphics() {
  if(shmup::on) return false;
  if(geosupport_football() != 2) return false;
  if(ls::chaoticity() >= 75) return false;
  return true;
  }

EX void escherSidewall(cell *c, SIDE sidepar, const shiftmatrix& V, color_t col) {
  if(sidepar >= SIDE::RED1 && sidepar <= SIDE::RED3) {
    int sl = int(sidepar) - int(SIDE::RED1);
    for(int z=1; z<=4; z++) if(z == 1 || (z == 4 && detaillevel == 2))
      draw_qfi(c, orthogonal_move_fol(V, zgrad0(cgi.slev * sl, cgi.slev * (sl+1), z, 4)), col, PPR::RED1_ESCHER+3*sl);
    }
  else if(sidepar == SIDE::WALL) {
    const int layers = 2 << detaillevel;
    for(int z=1; z<layers; z++) 
      draw_qfi(c, orthogonal_move_fol(V, zgrad0(0, geom3::actual_wall_height(), z, layers)), col, PPR::WALL_ESCHER);
    }
  else if(sidepar == SIDE::FLOOR) {
    const int layers = 1 << (detaillevel-1);
    if(detaillevel) for(int z=0; z<layers; z++)
      draw_qfi(c, orthogonal_move_fol(V, zgrad0(-vid.lake_top, 0, z, layers)), col, PPR::FLOOR_ESCHER);
    }
  else if(sidepar == SIDE::WATERLEVEL) {
    const int layers = 1 << (detaillevel-1);
    if(detaillevel) for(int z=0; z<layers; z++)
      draw_qfi(c, orthogonal_move_fol(V, zgrad0(-vid.lake_shallow, -vid.lake_top, z, layers)), col, PPR::WATERLEVEL_ESCHER);
    }
  else if(sidepar == SIDE::SHALLOW) {
    const int layers = 1 << (detaillevel-1);
    if(detaillevel) for(int z=0; z<layers; z++)
      draw_qfi(c, orthogonal_move_fol(V, zgrad0(-vid.lake_bottom, -vid.lake_shallow, z, layers)), col, PPR::SHALLOW_ESCHER);
    }
  else if(sidepar == SIDE::DEEP) {
    const int layers = 1 << detaillevel;
    draw_qfi(c, orthogonal_move_fol(V, cgi.INFDEEP), col, PPR::MINUSINF);
    for(int z=layers-1; z>1; z--)
      draw_qfi(c, orthogonal_move_fol(V, zgrad0(-vid.lake_bottom, -vid.lake_top, -z, 1)), col, PPR::DEEP_ESCHER);
    }
  }

EX bool use_escher(SIDE sidepar) {
  return (!qfi.fshape || !qfi.fshape->is_plain || !cgi.validsidepar[sidepar] || qfi.usershape >= 0) && (GDIM == 2);
  }

EX bool placeSidewall(cell *c, int i, SIDE sidepar, const shiftmatrix& V, color_t col) {

  if(use_escher(sidepar)) {
    escherSidewall(c, sidepar, V, col);
    return true;
    }
  if(!qfi.fshape) return true;
  
  if(qfi.fshape == &cgi.shBigTriangle && pseudohept(c->move(i))) return false;
  if(qfi.fshape == &cgi.shTriheptaFloor && !pseudohept(c) && !pseudohept(c->move(i))) return false;

  PPR prio = side_to_prio[sidepar];

  if((col & 255) < 255) prio = PPR::TRANSPARENT_WALL;
  
  dynamicval<bool> ncor(approx_nearcorner, true);
  
  #if CAP_ARCM
  if(arcm::in() && !PURE)
    i = gmod(i + arcm::parent_index_of(c->master)/DUALMUL, c->type);
  #endif
  if(currentmap->strict_tree_rules()) {
    i = rulegen::get_arb_dir(c, i);
    }
  if(int(sidepar) >= SIDEPARS) {
    println(hlog, "ERROR: sidepar >= SIDEPARS: ", make_pair(int(sidepar), SIDEPARS));
    return false;
    }
  if(i >= isize(qfi.fshape->side[sidepar])) {
    println(hlog, "ERROR: i >= side[sidepar]", make_tuple(int(sidepar), i, isize(qfi.fshape->side[sidepar])));
    return false;
    }
  draw_shapevec(c, V, qfi.fshape->side[sidepar][i], col, prio);
  return false;
  }
#endif

EX int mine_opacity = 255;

EX bool isWall3(cell *c, color_t& wcol) {
  if(c->wall == waRose) { wcol = gradient(0, wcol, -5 - 5 * (7-rosephase), sintick(50 * (8 - rosephase)), 1); }
  if(isWall(c)) return true;
  if(c->wall == waChasm && c->land == laMemory && (anyshiftclick || !(cgflags & qFRACTAL))) { wcol = 0x606000; return true; }
  if(c->wall == waInvisibleFloor) return false;
  // if(chasmgraph(c)) return true;
  if(among(c->wall, waMirror, waCloud)) return true;
  if(among(c->wall, waMineUnknown, waMineMine) && mine_opacity == 255) return true;
  return false;
  }

EX bool isWall3(cell *c) { color_t dummy; return isWall3(c, dummy); }

EX bool isSulphuric(eWall w) { return among(w, waSulphur, waSulphurC); }

// 'land color', but a bit twisted for Alchemist Lab
color_t lcolor(cell *c) {
  if(isAlch(c->wall) && !c->item) return winf[c->wall].color;
  return floorcolors[c->land];
  }

EX color_t transcolor(cell *c, cell *c2, color_t wcol) {
  color_t dummy;
  if(isWall3(c2, dummy)) return 0;
  if(c->land != c2->land && c->land != laNone && c2->land != laNone) {
    if(c>c2) return 0;
    if(c->land == laBarrier) return darkena3(lcolor(c2), 0, 0x40);
    if(c2->land == laBarrier) return darkena3(lcolor(c), 0, 0x40);
    return darkena3(gradient(lcolor(c), lcolor(c2), 0, 1, 2), 0, 0x40);
    }
  if(sol && c->land == laWineyard && c2->master->distance < c->master->distance)
    return 0x00800040;
  if(isAlch(c) && !c->item && (c2->item || !isAlch(c2))) return darkena3(winf[c->wall].color, 0, 0x40);
  if(c->wall == c2->wall) return 0;
  if(isFire(c) && !isFire(c2)) return darkena3(wcol, 0, 0x30);
  if(c->wall == waLadder) return darkena3(wcol, 0, 0x30);
  
  if(c->land == laZebra && c2->land == laZebra && c2->wall == waTrapdoor) return 0x202020A0;

  if(c->wall == waChasm && c2->wall != waChasm) return 0x606060A0;
  if(isWateryOrBoat(c) && !isWateryOrBoat(c2)) return 0x0000C060;
  if(isSulphuric(c->wall) && !isSulphuric(c2->wall)) return darkena3(winf[c->wall].color, 0, 0x40);
  if(among(c->wall, waCanopy, waSolidBranch, waWeakBranch) && !among(c2->wall, waCanopy, waSolidBranch, waWeakBranch)) return 0x00C00060;
  if(c->wall == waFloorA && c2->wall == waFloorB && !c->item && !c2->item) return darkena3(0xFF00FF, 0, 0x80);
  if(realred(c->wall) || realred(c2->wall)) {
    int l = int(get_spatial_info(c).top) - int(get_spatial_info(c2).top);
    if(l > 0) return darkena3(floorcolors[laRedRock], 0, 0x30 * l);
    }
  if(among(c->wall, waRubble, waDeadfloor2) && !among(get_spatial_info(c2).top, SIDE::RED1, SIDE::RED2, SIDE::RED3)) return darkena3(winf[c->wall].color, 0, 0x40);
  if(c->wall == waMagma && c2->wall != waMagma) return darkena3(magma_color(lavatide(c, -1)/4), 0, 0x80);
  if(among(c->wall, waMineUnknown, waMineMine) && !among(c2->wall, waMineMine, waMineUnknown) && mine_opacity > 0 && mine_opacity < 255) 
    return 0xFFFFFF00 | mine_opacity;
  return 0;
  }

EX bool no_darken = false;

// how much should be the d-th wall darkened in 3D
EX int get_darkval(cell *c, int d) {
  if(no_darken) return 0;
  if(mhybrid) {
    return d >= c->type - 2 ? 4 : 0;
    }
  const int darkval_hbt[9] = {0,2,2,0,6,6,8,8,0};
  const int darkval_s12[12] = {0,1,2,3,4,5,0,1,2,3,4,5};
  const int darkval_e6[6] = {0,4,6,0,4,6};
  const int darkval_e12[12] = {0,4,6,0,4,6,0,4,6,0,4,6};
  const int darkval_e14[14] = {0,0,0,4,6,4,6,0,0,0,6,4,6,4};
  const int darkval_hh[14] = {0,0,0,1,1,1,2,2,2,3,3,3,1,0};
  const int darkval_hrec[7] = {0,0,2,4,2,4,0};
  const int darkval_sol[8] = {0,2,4,4,0,2,4,4};
  const int darkval_arnold[12] = {0,2,0,2,4,5,0,2,0,2,4,5};
  const int darkval_kite[12] = {0, 2, 0, 2, 4, 4, 6, 6, 6, 6, 6, 6};
  const int darkval_nil[8] = {6,6,0,3,6,6,0,3};
  const int darkval_nih[11] = {0,2,0,2,4,6,6,6,6,6,6};
  const int darkval_ot[8] = {0,1,2,3,6,5,4,3};
  #if MAXMDIM >= 4
  if(among(variation, eVariation::dual_subcubes, eVariation::bch, eVariation::bch_oct, eVariation::coxeter)) {
    int v = reg3::get_face_vertex_count(c, d);
    return v-3;
    }
  #endif
  if(sphere) return darkval_s12[d];
  if(euclid && S7 == 6) return darkval_e6[d];
  if(euclid && S7 == 12) return darkval_e12[d];
  if(euclid && S7 == 14) return darkval_e14[d];
  if(geometry == gHoroHex) return darkval_hh[d];
  if(geometry == gHoroRec) return darkval_hrec[d];
  if(geometry == gOctTet3) return darkval_ot[d + (shvid(c) == 2 ? 4 : 0)];
  if(kite::in()) return darkval_kite[d];
  if(asonov::in()) return darkval_arnold[d];
  if(sol) return darkval_sol[d];
  if(nih) return darkval_nih[d];
  if(bt::in()) return darkval_hbt[d];
  if(hyperbolic && S7 == 6) return darkval_e6[d];
  if(hyperbolic && S7 == 12) return darkval_s12[d];
  if(nil) return darkval_nil[d];
  return 0;
  }

EX subcellshape& generate_subcellshape_if_needed(cell *c, int id) {
  if(isize(cgi.subshapes) <= id) cgi.subshapes.resize(id+1);
  
  auto& ss = cgi.subshapes[id];
  if(!ss.faces.empty()) return ss;
  
  cell *c1 = mhybrid ? hybrid::get_where(c).first : c;
  
  if(mhybrid || WDIM == 2) for(int i=0; i<c1->type; i++) {
    hyperpoint w;
    auto f = [&] { 
      /* mirror image of C0 in the axis h1-h2 */
      hyperpoint h1 = get_corner_position(c1, i);
      hyperpoint h2 = get_corner_position(c1, i+1);
      transmatrix T = gpushxto0(h1);
      T = lspintox(T * h2) * T;
      w = T * C0;
      w[1] = -w[1];
      w = iso_inverse(T) * w;
      };
    if(mproduct) PIU(f());
    else f();
    ss.walltester.push_back(w);
    }

  if(mhybrid || WDIM == 2) {
    ss.walltester.push_back(C0);
    ss.walltester.push_back(C0);
    }

  for(int i=0; i<c1->type; i++)
    ss.faces.push_back({hybrid::get_corner(c1, i, 0, -1), hybrid::get_corner(c1, i, 0, +1), hybrid::get_corner(c1, i, 1, +1), hybrid::get_corner(c1, i, 1, -1)});

  ss.angle_of_zero = -PIU(atan2(currentmap->adj(c1, 0)*C0));
  for(int a: {0,1}) {
    vector<hyperpoint> l;
    int z = a ? 1 : -1;
    hyperpoint ctr = zpush0(z * cgi.plevel/2);
    int qty = (mproduct || WDIM == 2) ? 1 : 3;
    for(int i=0; i<c1->type; i++)
      if(qty == 1)
        l.push_back(hybrid::get_corner(c1, i, 0, z));
      else {
        l.push_back(ctr);
        l.push_back(hybrid::get_corner(c1, i+1, 0, z));
        l.push_back(hybrid::get_corner(c1, i, 1, z));
        l.push_back(ctr);
        l.push_back(hybrid::get_corner(c1, i, 1, z));
        l.push_back(hybrid::get_corner(c1, i, 0, z));
        }
    if(a == 0) std::reverse(l.begin()+1, l.end());
    if(a == 1) std::rotate(l.begin(), l.begin()+qty, l.end());
    ss.faces.push_back(l);
    }
  
  ss.compute_hept();
  return ss;
  }

int hrmap::wall_offset(cell *c) {
  int id = currentmap->full_shvid(c);

  if(WDIM == 3 && !mhybrid && !reg3::in() && geometry != gOctTet3) return 0;

  if(isize(cgi.walloffsets) <= id) cgi.walloffsets.resize(id+1, {-1, nullptr});
  auto &wop = cgi.walloffsets[id];
  int &wo = wop.first;
  if(!wop.second) wop.second = c;
  if(wo == -1) {
    auto& ss = generate_subcellshape_if_needed(c, id);
    wo = isize(cgi.shWall3D);

    if(!cgi.wallstart.empty()) cgi.wallstart.pop_back();
    cgi.reserve_wall3d(wo + isize(ss.faces));

    rk_shape = &ss;
    for(int i=0; i<isize(ss.faces); i++) {
      cgi.make_wall(wo, i, ss.faces[i]);
      cgi.walltester[wo + i] = ss.walltester[i];
      }
    
    cgi.wallstart.push_back(isize(cgi.raywall));
    cgi.compute_cornerbonus();
    cgi.extra_vertices();
    }
  return wo;
  }

EX void queue_transparent_wall(const shiftmatrix& V, hpcshape& sh, color_t color) {
  auto& poly = queuepolyat(V, sh, color, PPR::TRANSPARENT_WALL);
  shiftpoint h = V * sh.intester;
  if(in_perspective())
    poly.subprio = int(hdist0(h) * 100000);
  else {
    hyperpoint h2;
    applymodel(h, h2);
    poly.subprio = int(h2[2] * 100000);
    }
  }

#if MAXMDIM >= 4
EX int ceiling_category(cell *c) {
  switch(c->land) {
    case laNone:
    case laMemory:
    case laMirrorWall2:
    case laMirrored:
    case laMirrored2:
    case landtypes:
      return 0;

    /* starry levels */
    case laIce:
    case laCrossroads:
    case laCrossroads2:
    case laCrossroads3:
    case laCrossroads4:
    case laCrossroads5:
    case laJungle:    
    case laGraveyard:
    case laMotion:
    case laRedRock:
    case laZebra:
    case laHunting:
    case laEAir:
    case laStorms:
    case laMountain:
    case laHaunted:
    case laHauntedWall:
    case laHauntedBorder:
    case laWhirlwind:
    case laBurial:
    case laHalloween:
    case laReptile:
    case laVolcano:
    case laBlizzard:
    case laDual:
    case laWestWall:
    case laAsteroids:
      return 1;
    
    case laPower:
    case laWineyard:
    case laDesert:
    case laAlchemist:
    case laDryForest:
    case laCaribbean:
    case laMinefield:
    case laOcean:
    case laWhirlpool:
    case laLivefjord:
    case laEWater:
    case laOceanWall:
    case laWildWest:
    case laOvergrown:
    case laClearing:
    case laRose:
    case laWarpCoast:
    case laWarpSea:
    case laEndorian:
    case laTortoise:
    case laPrairie:
    case laDragon:
    case laSnakeNest:
    case laDocks:
    case laKraken:
    case laBrownian: 
    case laHell:
    case laVariant:    
    case laFrog:
    case laWet:
      return 2;
    
    case laBarrier: 
    case laCaves:
    case laMirror:
    case laMirrorOld:
    case laCocytus:
    case laEmerald:
    case laDeadCaves:
    case laHive:
    case laCamelot:
    case laIvoryTower:
    case laEFire:
    case laEEarth:
    case laElementalWall:
    case laTrollheim:
    case laDungeon:
    case laBull:
    case laCA:
    case laMirrorWall:
    case laTerracotta:
    case laMercuryRiver:
    case laMagnetic:
    case laSwitch:
    case laEclectic:
      return 3;

    case laCanvas:
      if(canvas_default_wall == waInvisibleFloor) return 0;
      return 3;
    
    case laPalace:
    case laPrincessQuest:
    default:
      return 4;
    
    case laRuins:
      return 6;
    
    case laTemple:
    case laRlyeh:
      return 7;
    }
  }

#endif

}
