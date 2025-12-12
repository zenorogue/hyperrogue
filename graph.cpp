// Hyperbolic Rogue -- main graphics file
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file graph.cpp
 *  \brief Drawing cells, monsters, items, etc.
 */

#include "hyper.h"
namespace hr {

EX int last_firelimit;
EX int firelimit;

EX int inmirrorcount = 0;

/** wall optimization: do not draw things beyond walls */
EX bool wallopt;

EX bool in_wallopt() { return wallopt || racing::on; }

EX bool spatial_graphics;
EX bool wmspatial, wmescher, wmplain, wmblack, wmascii, wmascii3;
EX bool mmspatial, mmhigh, mmmon, mmitem;

EX int detaillevel = 0;

EX bool first_cell_to_draw = true;

EX bool zh_ascii = false;

EX bool in_perspective() {
  return models::is_perspective(pconf.model);
  }

EX bool in_perspective_v() {
  return models::is_perspective(vpconf.model);
  }

EX bool hide_player() {
  return GDIM == 3 && playermoved && vid.yshift == 0 && vid.sspeed > -5 && in_perspective() && (first_cell_to_draw || elliptic) && (WDIM == 3 || vid.camera == 0) && !inmirrorcount
#if CAP_RACING     
   && !(racing::on && !racing::use_standard_centering() && !racing::player_relative)
#endif
     ;
  }

EX transmatrix ddspin180(cell *c, int dir) { return ddspin(c, dir, M_PI); }
EX transmatrix iddspin180(cell *c, int dir) { return iddspin(c, dir, M_PI); }

EX transmatrix lpispin() {
  return spin180();
  }

EX hookset<bool(int sym, int uni)> hooks_handleKey;
EX hookset<bool(cell *c, const shiftmatrix& V)> hooks_drawcell;
EX purehookset hooks_frame, hooks_markers;

#define WOLNIEJ 1
#define BTOFF 0x404040
#define BTON  0xC0C000

// #define PANDORA

int colorbar;

EX bool inHighQual; // taking high quality screenshot
EX bool auraNOGL;    // aura without GL

// 
int axestate;

EX int ticks;
EX int frameid;

EX bool nomap;

EX eItem orbToTarget;
EX eMonster monsterToSummon;

EX int sightrange_bonus = 0;

EX string mouseovers;

EX int darken = 0;

EX bool doHighlight() {
  return mmhigh;
  }

int dlit;

ld spina(cell *c, int dir) {
  return TAU * dir / c->type;
  }

/** @brief used to alternate colors depending on distance to something. In chessboard-patterned geometries, automatically a third step.
 *  In some cases, we want to avoid a number of colors in the table -- set @param subtract to the number of such colors.
 */
EX color_t get_color_auto3(int f, const colortable& ctab, int subtract IS(0)) {
  int size = ctab.size() - subtract;
  if(size < 1) return 0;
  if(geosupport_chessboard() && size == 2) {
    f = gmod(f, 3);
    if(f == 1)
      return gradient(ctab[0], ctab[1], 0, 1, 2);
    return ctab[f/2];
    }
  else
    return ctab[gmod(f, size)];
  }

EX ld cheilevel(ld v) {
  return cgi.FLOOR + (cgi.HEAD - cgi.FLOOR) * v;
  }

EX transmatrix chei(const transmatrix V, int a, int b) {
#if MAXMDIM >= 4
  if(GDIM == 2) return V;
  return V * lzpush(cheilevel((a+.5) / b));
#else
  return V;
#endif
  }

EX shiftmatrix chei(const shiftmatrix V, int a, int b) {
#if MAXMDIM >= 4
  if(GDIM == 2) return V;
  return V * lzpush(cheilevel((a+.5) / b));
#else
  return V;
#endif
  }

EX bool ivoryz;

/** Change the level of V. Takes ivoryz and all geometries into account */
EX transmatrix at_smart_lof(const transmatrix& V, ld lev) {
  if(!mmspatial) return V;
  if(ivoryz) return mzscale(V, lev);
  return orthogonal_move_fol(V, lev);
  }

EX shiftmatrix at_smart_lof(const shiftmatrix& V, ld lev) { return shiftless(at_smart_lof(V.T, lev), V.shift); }

EX color_t kind_outline(eItem it) {
  switch(itemclass(it)) {
    case IC_TREASURE:
      return OUTLINE_TREASURE;
    case IC_ORB:
      return OUTLINE_ORB;
    default:
      return OUTLINE_OTHER;
    }
  }

/** should objects fly slightly up and down in product/twisted product geometries */
EX bool bobbing = true;

EX void draw_ascii(const shiftmatrix& V, const string& s, color_t col, ld size, ld size2) {
  int id = isize(ptds);
  if(WDIM == 2 && GDIM == 3)
    queuestrn(V * lzpush(cgi.FLOOR - cgi.scalefactor * size / 4), size * mapfontscale / 100, s, darkenedby(col, darken), 0);
  else
    queuestrn(V, size2 * mapfontscale / 100, s, darkenedby(col, darken), GDIM == 3 ? 0 : 2);
  while(id < isize(ptds)) ptds[id++]->prio = PPR::MONSTER_BODY;
  }

EX void draw_ascii(const shiftmatrix& V, char glyph, color_t col, ld size) {
  draw_ascii(V, s0 + glyph, col, size, 1);
  }

EX void draw_ascii_or_zh(const shiftmatrix& V, char glyph, const string& name, color_t col, ld size, ld zh_size) {
#if CAP_TRANS
  if(zh_ascii) {
    auto p = XLAT1_acc(name, 8);
    if(p) {
      string chinese = p;
      chinese.resize(utfsize(chinese[0]));
      dynamicval<fontdata*> df(cfont, cfont_chinese);
      draw_ascii(V, chinese, col, size, zh_size);
      return;
      }
    }
#endif
  draw_ascii(V, glyph, col, size);
  }

// push down the queue after q-th element, `down` absolute units down,
// based on cell c and transmatrix V
// do change the zoom factor? do change the priorities?

EX int cellcolor(cell *c) {
  if(isPlayerOn(c) || isFriendly(c)) return OUTLINE_FRIEND;
  if(noHighlight(c->monst)) return OUTLINE_NONE;
  if(c->monst) return OUTLINE_ENEMY;
  
  if(c->wall == waMirror) return c->land == laMirror ? OUTLINE_TREASURE : OUTLINE_ORB;

  if(c->item && !itemHiddenFromSight(c))
    return kind_outline(c->item);

  return OUTLINE_NONE;
  } 

#define AURA 180

array<array<int,4>,AURA+1> aurac;

int haveaura_cached;

/** 0 = no aura, 1 = standard aura, 2 = Joukowsky aura */
EX int haveaura() {
  if(!(vid.aurastr>0 && !svg::in && (auraNOGL || vid.usingGL))) return 0;
  if(vrhr::active()) return 0;
  if(sphere && mdAzimuthalEqui()) return 0;
  if(among(pmodel, mdJoukowsky, mdJoukowskyInverted) && hyperbolic && pconf.model_transition < 1) 
    return 2;
  if(among(pmodel, mdFisheye, mdFisheye2)) return 1;
  return pmodel == mdDisk && (!sphere || pconf.alpha > 10) && !euclid;
  }
  
vector<pair<int, int> > auraspecials; 

int auramemo;

EX void clearaura() {
  haveaura_cached = haveaura();
  if(!haveaura_cached) return;
  for(int a=0; a<AURA; a++) for(int b=0; b<4; b++) 
    aurac[a][b] = 0;
  auraspecials.clear();
  auramemo = 128 * 128 / vid.aurastr;
  }

void apply_joukowsky_aura(shiftpoint& h) {
  if(haveaura_cached == 2)  {
    hyperpoint ret;
    applymodel(h, ret);
    h.h = ret;
    }
  if(nonisotropic) {
    h.h = lp_apply(inverse_exp(h, pfNO_DISTANCE));
    }
  }

EX void addauraspecial(shiftpoint h, color_t col, int dir) {
  if(!haveaura_cached) return;
  apply_joukowsky_aura(h);
  int r = int(2*AURA + dir + atan2(h[1], h[0]) * AURA / TAU) % AURA;
  auraspecials.emplace_back(r, col);
  }

EX void addaura(shiftpoint h, color_t col, int fd) {
  if(!haveaura_cached) return;
  apply_joukowsky_aura(h);

  int r = gmod(atan2(h[1], h[0]) * AURA / TAU, AURA);
  aurac[r][3] += auramemo << fd;
  col = darkened(col);
  aurac[r][0] += (col>>16)&255;
  aurac[r][1] += (col>>8)&255;
  aurac[r][2] += (col>>0)&255;
  }
  
void sumaura(int v) {
  int auc[AURA];
  for(int t=0; t<AURA; t++) auc[t] = aurac[t][v];
  int val = 0;
  if(vid.aurasmoothen < 1) vid.aurasmoothen = 1;
  if(vid.aurasmoothen > AURA) vid.aurasmoothen = AURA;
  int SMO = vid.aurasmoothen;
  for(int t=0; t<SMO; t++) val += auc[t];
  for(int t=0; t<AURA; t++) {
    int tt = (t + SMO/2) % AURA;
    aurac[tt][v] = val;
    val -= auc[t];
    val += auc[(t+SMO) % AURA];
    }  
  aurac[AURA][v] = aurac[0][v];
  }

#if CAP_GL
vector<glhr::colored_vertex> auravertices;
#endif

EX debugflag debug_graph = {"graph"};

EX debugflag debug_aura = {"graph_aura"};

EX void drawaura() {
  indenter_finish(debug_aura, "drawaura");
  if(!haveaura()) return;
  if(vid.stereo_mode) return;
  double rad = current_display->radius;
  if(sphere && !mdAzimuthalEqui()) rad /= sqrt(pconf.alpha*pconf.alpha - 1);
  if(hyperbolic && pmodel == mdFisheye) {
    ld h = 1;
    h /= pconf.fisheye_param;
    ld nrad = h / sqrt(2 + h*h);
    rad *= nrad;
    }
  
  for(int v=0; v<4; v++) sumaura(v);
  for(auto& p: auraspecials) {
    int r = p.first;
    aurac[r][3] = auramemo;
    for(int k=0; k<3; k++) aurac[r][k] = (p.second >> (16-8*k)) & 255;
    }

#if CAP_SDL || CAP_GL
  ld bak[3];
  bak[0] = ((backcolor>>16)&255)/255.;
  bak[1] = ((backcolor>>8)&255)/255.;
  bak[2] = ((backcolor>>0)&255)/255.;
#endif
  
#if CAP_SDL
  if(!vid.usingGL) {
    SDL_LockSurface(s);
    for(int y=0; y<vid.yres; y++)
    for(int x=0; x<vid.xres; x++) {

      ld hx = (x * 1. - current_display->xcenter) / rad;
      ld hy = (y * 1. - current_display->ycenter) / rad / pconf.stretch;
  
      if(!models::camera_straight) camrotate(hx, hy);
      
      ld fac = sqrt(hx*hx+hy*hy);
      if(fac < 1) continue;
      ld dd = log((fac - .99999) / .00001);
      ld cmul = 1 - dd/10.;
      if(cmul>1) cmul=1;
      if(cmul<0) cmul=0;
      
      ld alpha = AURA * atan2(hx,hy) / TAU;
      if(alpha<0) alpha += AURA;
      if(alpha >= AURA) alpha -= AURA;
      
      int rm = int(alpha);
      ld fr = alpha-rm;
      
      if(rm<0 || rm >= AURA) continue;
      
      color_t& p = qpixel(s, x, y);
      for(int c=0; c<3; c++) {
        ld c1 = aurac[rm][2-c] / (aurac[rm][3]+.1);
        ld c2 = aurac[rm+1][2-c] / (aurac[rm+1][3]+.1);
        const ld one = 1;
        part(p, c) = int(255 * min(one, bak[2-c] + cmul * ((c1 + fr * (c2-c1) - bak[2-c])))); 
        }
      }
    SDL_UnlockSurface(s);
    return;
    }
#endif

#if CAP_GL
  float cx[AURA+1][11][5];

  double facs[11] = {1, 1.01, 1.02, 1.04, 1.08, 1.70, 1.95, 1.5, 2, 6, 10};
  double cmul[11] = {1,   .8,  .7,  .6,  .5,  .16,  .12,  .08,  .07,  .06, 0};
  double d2[11] = {0, 2, 4, 6.5, 7, 7.5, 8, 8.5, 9, 9.5, 10};

  for(int d=0; d<11; d++) {
    double dd = d2[d];
    cmul[d] = (1- dd/10.);
    facs[d] = .99999 +  .00001 * exp(dd);
    }
  facs[10] = 10;
  cmul[1] = cmul[0];
  
  bool inversion = pconf.alpha <= -1 || pmodel == mdJoukowsky;
  bool joukowsky = among(pmodel, mdJoukowskyInverted, mdJoukowsky) && hyperbolic && pconf.model_transition < 1;

  for(int r=0; r<=AURA; r++) for(int z=0; z<11; z++) {
    float rr = (TAU * r) / AURA;
    float rad0 = inversion ? rad / facs[z] : rad * facs[z];
    int rm = r % AURA;
    ld c = cos(rr);
    ld s = sin(rr);

    if(joukowsky) {
      hyperpoint v(c, s, 0, 1);
      if(inversion)
        models::ori_to_scr(v);
      else        
        models::scr_to_ori(v);
      ld c1 = v[0], s1 = v[1];

      ld& mt = pconf.model_transition;
      ld mt2 = 1 - mt;

      ld m = sqrt(c1*c1 + s1*s1 / mt2 / mt2);
      m *= 2;
      if(inversion) rad0 /= m;
      else rad0 *= m;
      }

    ld x = rad0 * c;
    ld y = rad0 * s;
    
    if(!models::camera_straight) {
      hyperpoint p = hyperpoint(x, y, rad0, 1);
      p = rot_inverse(pconf.cam()) * p;
      x = p[0] * rad0 / p[2];
      y = p[1] * rad0 / p[2];
      }
    cx[r][z][0] = x;
    cx[r][z][1] = y * pconf.stretch;
    
    for(int u=0; u<3; u++)
      cx[r][z][u+2] = bak[u] + (aurac[rm][u] / (aurac[rm][3]+.1) - bak[u]) * cmul[z];
    }
  
  auravertices.clear();
  for(int r=0; r<AURA; r++) for(int z=0;z<10;z++) {
    for(int c=0; c<6; c++) {
      int br = (c == 1 || c == 3 || c == 5) ? r+1 : r;
      int bz = (c == 2 || c == 4 || c == 5) ? z+1 : z;
      auravertices.emplace_back(
        cx[br][bz][0], cx[br][bz][1], cx[br][bz][2], cx[br][bz][3], cx[br][bz][4]
        );
      }
    }
  glflush();
  current_display->next_shader_flags = GF_VARCOLOR;
  dynamicval<eModel> m(pmodel, mdPixel);
  current_display->set_all(0, 0);
  glhr::id_modelview();
  glhr::prepare(auravertices);
  glhr::set_depthtest(false);
  glDrawArrays(GL_TRIANGLES, 0, isize(auravertices));
#endif
  }

// int fnt[100][7];

bool bugsNearby(cell *c, int dist = 2) {
  if(!(havewhat&HF_BUG)) return false;
  if(isBug(c)) return true;
  if(dist) for(int t=0; t<c->type; t++) if(c->move(t) && bugsNearby(c->move(t), dist-1)) return true;
  return false;
  }

EX int celldistAltPlus(cell *c) { return 1000000 + celldistAlt(c); }

EX bool drawstaratvec(double dx, double dy) {
  return dx*dx+dy*dy > .05;
  }

ld wavefun(ld x) { 
  return sin(x);
  /* x /= TAU;
  x -= (int) x;
  if(x > .5) return (x-.5) * 2;
  else return 0; */
  }

// does the current geometry allow nice duals
EX bool has_nice_dual() {
  #if CAP_IRR
  if(IRREGULAR) return irr::bitruncations_performed > 0;
  #endif
  #if CAP_ARCM
  if(arcm::in()) return geosupport_football() >= 2;
  #endif
  if(bt::in()) return false;
  if(BITRUNCATED) return true;
  if(a4) return false;
  if((S7 & 1) == 0) return true;
  if(PURE) return false;
  #if CAP_GP
  return (gp::param.first + gp::param.second * 2) % 3 == 0;
  #else
  return false;
  #endif
  }

// does the current geometry allow nice duals
EX bool is_nice_dual(cell *c) {
  return c->land == laDual && has_nice_dual();
  }

EX bool use_swapped_duals() {
  return (euclid && !a4) || GOLDBERG;
  }

EX bool openorsafe(cell *c) {
  #if CAP_COMPLEX2
  return c->wall == waMineOpen || mine::marked_safe(c);
  #else
  return false;
  #endif
  }

#define Dark(x) darkena(x,0,0xFF)

EX color_t stdgridcolor = 0x202020FF;

EX int gridcolor(cell *c1, cell *c2) {
  if(cmode & sm::DRAW && !mapeditor::drawing_tool) return Dark(forecolor);
  if(!c2)
    return 0x202020 >> darken;
  int rd1 = rosedist(c1), rd2 = rosedist(c2);
  if(rd1 != rd2) {
    int r = rd1+rd2;
    if(r == 1) return Dark(0x802020);
    if(r == 3) return Dark(0xC02020);
    if(r == 2) return Dark(0xF02020);
    }
  if((get_spatial_info(c1).deep<SIDE::SHALLOW) != (get_spatial_info(c2).deep<SIDE::SHALLOW) && c1->land != laAsteroids && c2->land != laAsteroids)
    return Dark(0x808080);
  if(c1->land == laAlchemist && c2->land == laAlchemist && c1->wall != c2->wall && !c1->item && !c2->item)
    return Dark(0xC020C0);
  if((c1->land == laWhirlpool || c2->land == laWhirlpool) && (celldistAlt(c1) != celldistAlt(c2)))
    return Dark(0x2020A0);
  if(c1->land == laMinefield && c2->land == laMinefield && (openorsafe(c1) != openorsafe(c2)))
    return Dark(0xA0A0A0);
  if(!darken) return stdgridcolor;
  return Dark(0x202020);
  }

#if CAP_SHAPES
EX void pushdown(cell *c, int& q, const shiftmatrix &V, double down, bool rezoom, bool repriority) {
 
  #if MAXMDIM >= 4
  if(GDIM == 3) {
    for(int i=q; i<isize(ptds); i++) {
      auto pp = ptds[q++]->as_poly();
      if(!pp) continue;
      auto& ptd = *pp;
      ptd.V = ptd.V * lzpush(+down);
      }
    return;
    }
  #endif

  // since we might be changing priorities, we have to make sure that we are sorting correctly
  if(down > 0 && repriority) { 
    int qq = q+1;
    while(qq < isize(ptds))
      if(qq > q && ptds[qq]->prio < ptds[qq-1]->prio) {
        swap(ptds[qq], ptds[qq-1]);
        qq--;
        }
      else qq++;
    }
  
  while(q < isize(ptds)) {
    auto pp = ptds[q++]->as_poly();
    if(!pp) continue;
    auto& ptd = *pp;

    double z2;
    
    double z = zlevel(tC0(ptd.V.T));
    double lev = geom3::factor_to_lev(z);
    double nlev = lev - down;
    
    double xyscale = rezoom ? geom3::scale_at_lev(lev) / geom3::scale_at_lev(nlev) : 1;
    z2 = geom3::lev_to_factor(nlev);
    double zscale = z2 / z;
    
    // xyscale = xyscale + (zscale-xyscale) * (1+sin(ticks / 1000.0)) / 2;
    
    ptd.V.T = xyzscale( V.T, xyscale*zscale, zscale)
      * z_inverse(V.T) * unshift(ptd.V, V.shift);
      
    if(!repriority) ;
    else if(nlev < -vid.lake_bottom-1e-3) {
      ptd.prio = PPR::DEEP_FALLANIM;
      if(c->wall != waChasm)
        ptd.color = 0; // disappear!
      }
    else if(nlev < -vid.lake_top-1e-3)
      ptd.prio = PPR::SHALLOW_FALLANIM;
    else if(nlev < 0)
      ptd.prio = PPR::FLOOR_FALLANIM;
    }
  }
#endif

bool allemptynear(cell *c) {
  if(c->wall) return false;
  forCellEx(c2, c) if(c2->wall) return false;
  return true;
  }

EX bool bright;
EX int canvasdark;

// how much to darken
EX int getfd(cell *c) {
  if(bright) return 0;
  if(among(c->land, laAlchemist, laHell, laVariant, laEclectic) && WDIM == 2 && GDIM == 3) return 0;
  switch(c->land) {
    case laCanvas:
      return min(2,max(0,canvasdark));

    case laRedRock:
    case laReptile:
      return 0;
      
    case laSnakeNest:
      return realred(c->wall) ? 0 : 1;
    
    case laTerracotta:
    case laMercuryRiver:
      return (c->wall == waMercury && wmspatial) ? 0 : 1;

    case laKraken:
    case laDocks:
    case laBurial:
    case laIvoryTower:
    case laDungeon:
    case laMountain:
    case laEndorian:
    case laCaribbean:
    case laWhirlwind:
    case laRose:
    case laWarpSea:
    case laTortoise:
    case laDragon:
    case laHalloween:
    case laHunting:
    case laOcean:
    case laLivefjord:
    case laWhirlpool:
    case laAlchemist:
    case laIce:
    case laGraveyard:
    case laBlizzard:
    case laRlyeh:
    case laTemple:
    case laWineyard:
    case laDeadCaves:
    case laPalace:
    case laCA:
    case laDual:
    case laBrownian:
      return 1;
    
    case laVariant:
      if(isWateryOrBoat(c)) return 1;
      return 2;
    
    case laTrollheim:
    default:
      return 2;
    }    
  }

EX bool just_gmatrix;

EX int colorhash(color_t i) {
  return (i * 0x471211 + i*i*0x124159 + i*i*i*0x982165) & 0xFFFFFF;
  }

EX ld mousedist(shiftmatrix T) {
  if(GDIM == 2) return hdist(mouseh, tC0(T));
  shiftpoint T1 = orthogonal_move_fol(T, cgi.FLOOR) * tile_center();
  hyperpoint h1;
  applymodel(T1, h1);
  if(mouseaim_sensitivity) return sqhypot_d(2, h1) + (point_behind(T1) ? 1e10 : 0);
  h1 = h1 - point2((mousex - current_display->xcenter) / current_display->radius, (mousey - current_display->ycenter) / current_display->radius);
  return sqhypot_d(2, h1) + (point_behind(T1) ? 1e10 : 0);
  }

EX vector<vector<hyperpoint>> clipping_plane_sets;
EX int noclipped;

EX bool frustum_culling = true;

EX ld threshold, xyz_threshold;

EX bool clip_checked = false;

EX bool other_stereo_mode() {
  return vid.stereo_mode != sOFF;
  }

EX void make_clipping_planes() {
#if MAXMDIM >= 4
  clip_checked = false;
  if(!frustum_culling || PIU(sphere) || experimental || other_stereo_mode() || gproduct || embedded_plane) return;

  if(WDIM == 3 && pmodel == mdPerspective && !nonisotropic && !in_s2xe())
    threshold = sin_auto(cgi.corner_bonus), xyz_threshold = 0, clip_checked = true;
  else if(pmodel == mdGeodesic && sn::in())
    threshold = .6, xyz_threshold = 3, clip_checked = true;
  else if(pmodel == mdGeodesic && nil)
    threshold = 2, xyz_threshold = 3, clip_checked = true;
  else return;
  
  clipping_plane_sets.clear();  
    
  auto add_clipping_plane_txy = [] (transmatrix T, const transmatrix& nlp, ld x1, ld y1, ld x2, ld y2) {
    ld z1 = 1, z2 = 1;
    hyperpoint sx = point3(y1 * z2 - y2 * z1, z1 * x2 - z2 * x1, x1 * y2 - x2 * y1);
    sx /= hypot_d(3, sx);
    sx[3] = 0;
    sx = T * sx;
    if(nisot::local_perspective_used) sx = ortho_inverse(nlp) * sx;
    clipping_plane_sets.back().push_back(sx);
    };

  #if CAP_VR
  auto add_clipping_plane_proj = [&] (transmatrix T, const transmatrix& nlp, const transmatrix& iproj, ld x1, ld y1, ld x2, ld y2) {
    hyperpoint h1 = iproj * point31(x1, y1, .5);
    hyperpoint h2 = iproj * point31(x2, y2, .5);
    h1 /= h1[2]; h2 /= h2[2];
    add_clipping_plane_txy(T, nlp, h1[0], h1[1], h2[0], h2[1]);
    };
  #endif

  auto clipping_planes_screen = [&] (const transmatrix& T, const transmatrix& nlp) {
    ld tx = current_display->tanfov;
    ld ty = tx * current_display->ysize / current_display->xsize;
    clipping_plane_sets.push_back({});
    add_clipping_plane_txy(T, nlp, +tx, +ty, -tx, +ty);
    add_clipping_plane_txy(T, nlp, -tx, +ty, -tx, -ty);
    add_clipping_plane_txy(T, nlp, -tx, -ty, +tx, -ty);
    add_clipping_plane_txy(T, nlp, +tx, -ty, +tx, +ty);
    };
    
  bool stdview = true;
  
  #if CAP_VR
  if(vrhr::active()) {
    for(auto p: vrhr::frusta) {
      if(p.screen)
        clipping_planes_screen(inverse(p.pre), p.nlp);
      else {
        auto iproj = inverse(p.proj);
        auto ipre = inverse(p.pre);
        clipping_plane_sets.push_back({});
        add_clipping_plane_proj(ipre, p.nlp, iproj, 1, 1, 0, 1);
        add_clipping_plane_proj(ipre, p.nlp, iproj, 0, 1, 0, 0);
        add_clipping_plane_proj(ipre, p.nlp, iproj, 0, 0, 1, 0);
        add_clipping_plane_proj(ipre, p.nlp, iproj, 1, 0, 1, 1);
        }
      stdview = false;
      }
    }    
  #endif
  if(stdview) clipping_planes_screen(Id, NLP);
#endif
  }

EX bool clipped_by(const hyperpoint& H, const vector<hyperpoint>& v) {
  for(auto& cpoint: v) if((H|cpoint) < -threshold) return true;
  return false;
  }

EX bool clipped_by(const hyperpoint& H, const vector<vector<hyperpoint>>& vv) {
  for(auto& cps: vv) if(!clipped_by(H, cps)) return false;
  return true;
  }

bool celldrawer::cell_clipped() {

  if(!clip_checked) return false;
  
  hyperpoint H = unshift(tC0(V));
  
  if(xyz_threshold && abs(H[0]) <= xyz_threshold && abs(H[1]) <= xyz_threshold && abs(H[2]) <= xyz_threshold) {
    noclipped++;
    return false;
    }
  
  if(clipped_by(H, clipping_plane_sets)) {
    drawcell_in_radar();    
    return true;
    }

  noclipped++;
  return false;
  }

EX ld precise_width = .5;

int grid_depth = 0;

EX bool fat_edges = false;

EX bool gridbelow;

EX void gridline(const shiftmatrix& V1, const hyperpoint h1, const shiftmatrix& V2, const hyperpoint h2, color_t col, int prec) {
  transmatrix U2 = unshift(V2, V1.shift);

  int c1 = safe_classify_ideals(h1);
  int c2 = safe_classify_ideals(h2);
  ld d = (c1 <= 0 || c2 <= 0) ? 99 : hdist(V1.T*h1, U2*h2);
  
  #if MAXMDIM >= 4
  if(GDIM == 3 && fat_edges) {
    if(nonisotropic) {
      auto nV1 = V1 * rgpushxto0(h1);
      hyperpoint U2 = inverse_shift(nV1, V2*rgpushxto0(h2)) * C0;
      auto& p = cgi.get_pipe_noniso(U2, vid.linewidth, ePipeEnd::ball);
      queuepoly(nV1, p, col);
      return;
      }

    shiftmatrix T = V1 * rgpushxto0(h1);
    transmatrix S = rspintox(inverse_shift(T, V2) * h2);
    transmatrix U = rspintoc(inverse_shift(T*S, shiftless(C0)), 2, 1);
    auto& p = queuepoly(T * S * U, cgi.get_pipe_iso(d, vid.linewidth, ePipeEnd::ball), col);
    p.intester = xpush0(d/2);
    return;
    }
  #endif

  while(d > precise_width && d < 100 && grid_depth < 10) { 
    if(V1.shift != V2.shift || !eqmatrix(V1.T, V2.T, 1e-6)) { gridline(V1, h1, V1, inverse_shift(V1, V2) * h2, col, prec); return; }
    hyperpoint h;
    if(c1 <= 0 && c2 <= 0) {
      h = closest_to_zero(h1, h2);
      if(safe_classify_ideals(h) <= 0) return;
      h = normalize(h);
      }
    else if(c2 <= 0) {
      dynamicval<int> dw(grid_depth, 99);
      for(ld a=0; a<ideal_limit; a+=precise_width)
        gridline(V1, towards_inf(h1, h2, a), V1, towards_inf(h1, h2, a+precise_width), col, prec);
      return;
      }
    else if(c1 <= 0) {
      dynamicval<int> dw(grid_depth, 99);
      for(ld a=0; a<ideal_limit; a+=precise_width)
        gridline(V1, towards_inf(h2, h1, a), V1, towards_inf(h2, h1, a+precise_width), col, prec);
      return;
      }
    else h = midz(h1, h2);
    grid_depth++;
    gridline(V1, h1, V1, h, col, prec); 
    gridline(V1, h, V1, h2, col, prec); 
    grid_depth--;
    return;
    }
#if MAXMDIM >= 4
  if(WDIM == 2 && GDIM == 3) {
    ld eps = cgi.human_height/100;
    queueline(V1*orthogonal_move(h1,cgi.FLOOR+eps), V2*orthogonal_move(h2,cgi.FLOOR+eps), col, prec);
    queueline(V1*orthogonal_move(h1,cgi.WALL-eps), V2*orthogonal_move(h2,cgi.WALL-eps), col, prec);
    }
  else
#endif
    queueline(V1*h1, V2*h2, col, prec, gridbelow ? PPR::FLOORd : PPR::LINE);
  }

EX void gridline(const shiftmatrix& V, const hyperpoint h1, const hyperpoint h2, color_t col, int prec) {
  gridline(V, h1, V, h2, col, prec);
  }

EX void set_detail_level(const shiftmatrix& V) {
  ld dist0 = hdist0(tC0(V)) - 1e-6;
  if(vid.use_smart_range) detaillevel = 2;
  else if(dist0 < vid.highdetail) detaillevel = 2;
  else if(dist0 < vid.middetail) detaillevel = 1;
  else detaillevel = 0;

  if((cmode & sm::NUMBER) && (dialog::editingDetail())) {
    color_t col = 
      dist0 < vid.highdetail ? 0xFF80FF80 :
      dist0 >= vid.middetail ? 0xFFFF8080 :
      0XFFFFFF80;
    queuepoly(V, cgi.shHeptaMarker, darkena(col & 0xFFFFFF, 0, 0xFF));
    }
  }

#if CAP_QUEUE
EX void queuecircleat1(cell *c, const shiftmatrix& V, double rad, color_t col) {
  if(WDIM == 3) {
    dynamicval<color_t> p(poly_outline, col);
    int ofs = currentmap->wall_offset(c);
    for(int i=0; i<c->type; i++) {
      queuepolyat(V, cgi.shWireframe3D[ofs + i], 0, PPR::SUPERLINE);
      }
    return;
    }    
  if(spatial_graphics || GDIM == 3) {
    vector<hyperpoint> corners(c->type+1);
    for(int i=0; i<c->type; i++) corners[i] = get_corner_position(c, i, 3 / rad);
    corners[c->type] = corners[0];
    for(int i=0; i<c->type; i++) {
      queueline(V * orthogonal_move_fol(corners[i], cgi.FLOOR), V * orthogonal_move_fol(corners[i+1], cgi.FLOOR), col, 2, PPR::SUPERLINE);
      queueline(V * orthogonal_move_fol(corners[i], cgi.WALL), V * orthogonal_move_fol(corners[i+1], cgi.WALL), col, 2, PPR::SUPERLINE);
      queueline(V * orthogonal_move_fol(corners[i], cgi.FLOOR), V * orthogonal_move_fol(corners[i], cgi.WALL), col, 2, PPR::SUPERLINE);
      }
    return;
    }
  #if CAP_SHAPES
  if(vid.stereo_mode || sphere) {
    dynamicval<color_t> p(poly_outline, col);
    queuepolyat(V * spintick(100), cgi.shGem[1], 0, PPR::LINE);
    return;
    }
  #endif
  queuecircle(V, rad, col);  
  if(!wmspatial) return;
  auto si = get_spatial_info(c);
  if(si.top == SIDE::WALL)
    queuecircle(orthogonal_move_fol(V, cgi.WALL), rad, col);
  if(si.top == SIDE::RED1)
    queuecircle(orthogonal_move_fol(V, cgi.RED[1]), rad, col);
  if(si.top == SIDE::RED2)
    queuecircle(orthogonal_move_fol(V, cgi.RED[2]), rad, col);
  if(si.top == SIDE::RED3)
    queuecircle(orthogonal_move_fol(V, cgi.RED[3]), rad, col);
  if(si.top <= SIDE::WATERLEVEL)
    queuecircle(orthogonal_move_fol(V, cgi.WATERLEVEL), rad, col);
  }

EX void queuecircleat(cell *c, double rad, color_t col) {
  if(!c) return;
  for (const shiftmatrix& V : hr::span_at(current_display->all_drawn_copies, c))
    queuecircleat1(c, V, rad, col);
  }
#endif

#if ISMOBILE
#define MOBON (clicked)
#else
#define MOBON true
#endif

EX cell *forwardcell() {
  #if CAP_VR
  if(vrhr::active()) {
    return vrhr::forward_cell;
    }
  #endif
  movedir md = vectodir(move_destination_vec(6));
  cellwalker xc = cwt + md.d + wstep;
  return xc.at;
  }

EX bool draw_centerover = true;

EX bool should_draw_mouse_cursor() {
  if(!mousing || inHighQual) return false;
  if(outofmap(mouseh.h)) return false;
  if(rug::rugged && !rug::renderonce) return true;
  return false;
  }

EX void drawMarkers() {

  shmup::draw_collision_debug();

  if(!(cmode & sm::NORMAL)) return;

  if(should_draw_mouse_cursor()) {
    for(int i: player_indices()) {
      queueline(ggmatrix(playerpos(i)) * (WDIM == 2 && GDIM == 3 ? zpush0(cgi.WALL) : C0), mouseh, 0xFF00FF, grid_prec() + 1);
      }
    }
  
  callhooks(hooks_markers);
  #if CAP_SHAPES
  viewmat();
  #endif
  
  #if CAP_QUEUE
  for(cell *c1: crush_now) 
    queuecircleat(c1, .8, darkena(minf[moCrusher].color, 0, 0xFF));
  #endif

  if(!inHighQual) {

    bool ok = !ISPANDORA || mousepressed;
    
    ignore(ok);
     
    #if CAP_QUEUE
    if(haveMount())
      for (const shiftmatrix& V : hr::span_at(current_display->all_drawn_copies, dragon::target)) {
        queuestr(V, mapfontscale/100, "X",
          gradient(0, iinf[itOrbDomination].color, -1, sintick(dragon::whichturn == turncount ? 75 : 150), 1));
        }
    #endif

    /* for(int i=0; i<12; i++) if(c->type == 5 && c->master == &dodecahedron[i])
      queuestr(xc, yc, sc, 4*vid.fsize, s0+('A'+i), iinf[itOrbDomination].color); */
    
    if(1) {
      using namespace yendor;
      if(yii < isize(yi) && !yi[yii].found) {
        cell *keycell = NULL;
        int last_i = 0;
        for(int i=0; i<YDIST; i++)
          if(yi[yii].path[i]->cpdist <= get_sightrange_ambush()) {
            keycell = yi[yii].path[i]; last_i = i;
            }
        if(keycell) {
          for(int i = last_i+1; i<YDIST; i++) {
            cell *c = yi[yii].path[i];
            if(inscreenrange(c))
              keycell = c;
            }
          shiftpoint H = tC0(ggmatrix(keycell));
          #if CAP_QUEUE
          queue_goal_text(H, 2, "X", 0x10101 * int(128 + 100 * sintick(150)));
          int cd = celldistance(yi[yii].key(), cwt.at);
          if(cd == DISTANCE_UNKNOWN) for(int i2 = 0; i2<YDIST; i2++) {
            int cd2 = celldistance(cwt.at, yi[yii].path[i2]);
            if(cd2 != DISTANCE_UNKNOWN) {
              cd = cd2 + (YDIST-1-i2);
              }
            }
          queue_goal_text(H, 1, its(cd), 0x10101 * int(128 - 100 * sintick(150)));
          #endif
          addauraspecial(H, iinf[itOrbYendor].color, 0);
          addradar(ggmatrix(keycell), 'X', iinf[itKey].color, kind_outline(itKey), true);
          }
        }
      }
  
    #if CAP_RACING
    racing::markers();
    #endif
  
    #if CAP_QUEUE        
    if(lmouseover && vid.drawmousecircle && ok && DEFAULTCONTROL && MOBON && WDIM == 2) {
      cell *at = lmouseover;
      #if CAP_VR
      if(vrhr::active() && vrhr::forward_cell)
        at = vrhr::forward_cell;
      #endif
      queuecircleat(at, .8, darkena(lmouseover->cpdist > 1 ? 0x00FFFF : 0xFF0000, 0, 0xFF));
      }

    if(global_pushto && vid.drawmousecircle && ok && DEFAULTCONTROL && MOBON && WDIM == 2) {
      queuecircleat(global_pushto, .6, darkena(0xFFD500, 0, 0xFF));
      }
    #endif

#if CAP_SDLJOY && CAP_QUEUE
    if(joydir.d >= 0 && WDIM == 2) 
      queuecircleat(cwt.at->modmove(joydir.d+cwt.spin), .78 - .02 * sintick(199), 
        darkena(0x00FF00, 0, 0xFF));
#endif

    bool m = true;
    ignore(m);
#if CAP_MODEL
    m = netgen::mode == 0;
#endif

    #if CAP_QUEUE
    if(centerover && !playermoved && m && !anims::any_animation() && WDIM == 2 && draw_centerover)
      queuecircleat(centerover, .70 - .06 * sintick(200), 
        darkena(int(175 + 25 * sintick(200)), 0, 0xFF));

    if(multi::players > 1 || multi::alwaysuse) for(int i=0; i<numplayers(); i++) {
      multi::cpid = i;
      if(multi::players == 1) multi::player[i] = cwt;
      cell *ctgt = multi::multiPlayerTarget(i);
      queuecircleat(ctgt, .40 - .06 * sintick(200, i / numplayers()), getcs().uicolor);
      }
    #endif

    // process mouse
    #if CAP_SHAPES
    if((vid.axes >= 4 || (vid.axes == 1 && !mousing)) && !shmup::on && GDIM == 2) {
      if(multi::players == 1) {
        forCellIdAll(c2, d, cwt.at) if(gmatrix.count(cwt.at)) draw_movement_arrows(c2, unshift(gmatrix[cwt.at]) * currentmap->adj(cwt.at, d), d);
        }
      else if(multi::players > 1) for(int p=0; p<multi::players; p++) {
        if(multi::playerActive(p) && (vid.axes >= 4 || !drawstaratvec(multi::mdx[p], multi::mdy[p]))) 
        forCellIdAll(c2, d, multi::player[p].at) if(gmatrix.count(cwt.at)) {
          multi::cpid = p;
          dynamicval<shiftmatrix> ttm(cwtV, multi::whereis[p]);
          dynamicval<cellwalker> tcw(cwt, multi::player[p]);
          draw_movement_arrows(c2, unshift(gmatrix[cwt.at]) * currentmap->adj(cwt.at, d), d);
          }
        }
      }
    
    if(GDIM == 3 && !inHighQual && !shmup::on && vid.axes3 && playermoved) {
      cell *c = forwardcell();
      if(c) queuecircleat(c, .8, getcs().uicolor);
      }
    
    #endif

    if(mhybrid && !shmup::on) {

      using namespace sword;
      int& ang = sword::dir[multi::cpid].angle;
      ang %= sword_angles;

      int adj = 1 - ((sword_angles/cwt.at->type)&1);
      
      if(items[itOrbSword])
        for (const shiftmatrix& V : hr::span_at(current_display->all_drawn_copies, cwt.at))
          queuestr(V * spin(M_PI+(-adj-2*ang)*M_PI/sword_angles) * xpush0(cgi.sword_size), vid.fsize*2, "+", iinf[itOrbSword].color);
      if(items[itOrbSword2])
        for (const shiftmatrix& V : hr::span_at(current_display->all_drawn_copies, cwt.at))
          queuestr(V * spin((-adj-2*ang)*M_PI/sword_angles) * xpush0(-cgi.sword_size), vid.fsize*2, "+", iinf[itOrbSword2].color);
      }
    if(SWORDDIM == 3 && !shmup::on) {
      if(items[itOrbSword])
        for (const shiftmatrix& V : hr::span_at(current_display->all_drawn_copies, cwt.at))
          queuestr(V * sword::dir[multi::cpid].T * xpush0(cgi.sword_size), vid.fsize*2, "+", iinf[itOrbSword].color);
      if(items[itOrbSword2])
        for (const shiftmatrix& V : hr::span_at(current_display->all_drawn_copies, cwt.at))
          queuestr(V * sword::dir[multi::cpid].T * xpush0(-cgi.sword_size), vid.fsize*2, "+", iinf[itOrbSword2].color);
      }
    }

  monsterToSummon = moNone;
  orbToTarget = itNone;

  if(mouseover && targetclick) {
    multi::cpid = 0;
    orbToTarget = targetRangedOrb(mouseover, roCheck);
    #if CAP_QUEUE
    if(bow::fire_mode) {
      queuestr(mousex, mousey, 0, vid.fsize, "+", getcs().bowcolor >> 8);
      orbToTarget = itNone;
      }
    else if(orbToTarget == itOrbSummon) {
      monsterToSummon = summonedAt(mouseover);
      queuestr(mousex, mousey, 0, vid.fsize, s0+minf[monsterToSummon].glyph, minf[monsterToSummon].color);
      queuecircleat(mouseover, 0.6, darkena(minf[monsterToSummon].color, 0, 0xFF));
      }
    else if(orbToTarget) {
      queuestr(mousex, mousey, 0, vid.fsize, "@", iinf[orbToTarget].color);
      queuecircleat(mouseover, 0.6, darkena(iinf[orbToTarget].color, 0, 0xFF));
      }
    #endif
    #if CAP_SHAPES
    if(orbToTarget && rand() % 200 < ticks - lastt) {
      if(orbToTarget == itOrbDragon)
        drawFireParticles(mouseover, 2);
      else if(orbToTarget == itOrbSummon) {
        drawParticles(mouseover, iinf[orbToTarget].color, 1);
        drawParticles(mouseover, minf[monsterToSummon].color, 1);
        }
      else {
        drawParticles(mouseover, iinf[orbToTarget].color, 2);
        }
      }
    if(items[itOrbAir] && mouseover->cpdist > 1) {
      cell *c1 = mouseover;
      int dir = c1->monst == moVoidBeast ? -1 : 1;
      for(int it=0; it<10; it++) {
        int di;
        auto mib = blowoff_destination_dir(c1, di, dir);
        if(!mib.proper()) break;
        auto& c2 = mib.t;
        shiftmatrix T1 = ggmatrix(c1);
        shiftmatrix T2 = ggmatrix(c2);
        shiftmatrix T = T1 * lrspintox(inverse_shift(T1,T2*C0)) * xpush(hdist(T1*C0, T2*C0) * fractick(50, 0));
        color_t aircol = (orbToTarget == itOrbAir ? 0x8080FF40 : 0x8080FF20);
        queuepoly(T, cgi.shDisk, aircol);
        c1 = c2;
        }
      }
    #endif
    }  
  }

EX bool allowIncreasedSight() {
  if(cheater || autocheat) return true;
  if(peace::on) return true;
#if CAP_TOUR
  if(tour::on) return true;
#endif
  if(randomPatternsMode) return true;
  if(racing::on) return true;
  if(quotient || !hyperbolic || arcm::in() || arb::in()) return true;
  if(WDIM == 3) return true;
  if(!canmove) return true;
  return false;
  }

EX bool allowChangeRange() {
  if(cheater || peace::on || randomPatternsMode) return true;
#if CAP_TOUR
  if(tour::on) return true;
#endif
  if(racing::on) return true;
  if(arcm::in() || arb::in()) return true;
  if(WDIM == 3) return true;
  return false;
  }

EX purehookset hooks_drawmap;

EX transmatrix actual_view_transform;

EX ld wall_radar(cell *c, transmatrix T, transmatrix LPe, ld max) {
  if(!in_perspective() || !vid.use_wall_radar) return max;
  transmatrix ori;
  if(gproduct) ori = ortho_inverse(LPe);
  ld step = max / 20;
  ld fixed_yshift = 0;
  for(int i=0; i<20; i++) {
    T = shift_object(T, ori, ztangent(-step), shift_method(smaWallRadar));
    virtualRebase(c, T);
    color_t col;
    if(isWall3(c, col) || (WDIM == 2 && GDIM == 3 && tC0(T)[2] > cgi.FLOOR)) { 
      T = shift_object(T, ori, ztangent(step), shift_method(smaWallRadar));
      step /= 2; i = 17; 
      if(step < 1e-3) break; 
      }
    else fixed_yshift += step;
    }
  return fixed_yshift;
  }

/** if this is set to ON, just transform non-isotropic spaces according to View, and apply NLP to view */
EX bool nonisotropic_weird_transforms;

EX void decide_lpu() {
  nisot::local_perspective_used = gproduct;
  }

EX void make_actual_view() {
  decide_lpu();
  if(!nisot::local_perspective_used) NLP = Id;
  sphereflip = Id;
  sphere_flipped = flip_sphere();
  if(sphere_flipped) sphereflip[LDIM][LDIM] = -1;
  actual_view_transform = sphereflip;  
  if(vid.yshift && WDIM == 2) actual_view_transform = ypush(vid.yshift) * actual_view_transform;
  #if MAXMDIM >= 4
  if(GDIM == 3) {
    ld max = WDIM == 2 ? vid.camera : vid.yshift;
    if(max) {
      transmatrix Start = view_inverse(actual_view_transform * View);
      ld d = wall_radar(centerover, Start, NLP, max);
      actual_view_transform = get_shift_view_of(ztangent(d), actual_view_transform * View) * view_inverse(View); 
      }
    hyperpoint h = tC0(view_inverse(actual_view_transform * View));
    
    camera_level = cgi.emb->get_logical_z(h);

    camera_sign = cgi.FLOOR > cgi.WALL;
    }
  if((nonisotropic || (hyperbolic && bt::in() && !nisot::geodesic_movement)) && !nonisotropic_weird_transforms) {
    transmatrix T = actual_view_transform * View;
    transmatrix T2 = eupush( tC0(view_inverse(T)) );
    NLP = T * T2;
    actual_view_transform = ortho_inverse(NLP) * actual_view_transform;
    nisot::local_perspective_used = true;
    }
  #endif
  cgi.emb->set_radar_transform();
  Viewbase = View;
  }

EX shiftmatrix cview(ld base_shift IS(0)) {
  return shiftless(actual_view_transform * View, base_shift);
  }

EX int point_direction;

EX int through_wall(cell *c, hyperpoint at) {
  ld dist = hdist0(at);
  int nei = -1;
  for(int i=0; i<c->type; i++) {
    ld dist1 = hdist0(currentmap->ray_iadj(c, i) * at);
    if(dist1 < dist) nei = i, dist = dist1;
    }
  return nei;
  }

EX void precise_mouseover() {
  if(WDIM == 3 && (cmode & (sm::EDIT_INSIDE_WALLS | sm::EDIT_BEFORE_WALLS))) {
    transmatrix T = view_inverse(View);
    transmatrix ori = Id;
    if(gproduct) ori = ortho_inverse(NLP);
    ld step = 0.2;
    cell *c = centerover;
    for(int i=0; i<100; i++) {
      apply_shift_object(T, ori, ztangent(step));
      int pd = through_wall(c, T * C0);
      if(pd != -1) {
        color_t col;
        cell *c1 = c->cmove(pd);
        if(isWall3(c1, col)) {
          mouseover = c;
          mouseover2 = c1;
          point_direction = pd;
          if(cmode & sm::EDIT_INSIDE_WALLS) {
            swap(mouseover, mouseover2);
            }
          else {
            point_direction =c->c.spin(pd);
            }
          return;
          }
        else {
          T = currentmap->iadj(c, pd) * T;
          c = c1;
          }
        }
      }
    }
  if(WDIM == 3) { 
    mouseover2 = mouseover = centerover;
    ld best = HUGE_VAL;
    shiftpoint h = shiftless(direct_exp(lp_iapply(ztangent(0.01))));
    point_direction = -1;

    shiftmatrix cov = ggmatrix(mouseover2);
    forCellIdEx(c1, i, mouseover2) {
      shiftpoint h1 = tC0(cov * currentmap->adj(mouseover2, i));
      ld dist = geo_dist(h, h1) - geo_dist(shiftless(C0), h1);
      if(dist < best) mouseover = c1, best = dist, point_direction = i;
      }
    return; 
    }
  if(!mouseover) return;
  if(GDIM == 3) return;
  cell *omouseover = mouseover;
  for(int loop = 0; loop < 10; loop++) { 
    bool found = false;
    if(!gmatrix.count(mouseover)) return;
    hyperpoint r_mouseh = inverse_shift(gmatrix[mouseover], mouseh);
    for(int i=0; i<mouseover->type; i++) {
      hyperpoint h1 = get_corner_position(mouseover, gmod(i-1, mouseover->type));
      hyperpoint h2 = get_corner_position(mouseover, i);
      if(det3(build_matrix(h1, h2, C0, C0)) * det3(build_matrix(h1, h2, r_mouseh, C0)) < 0) {
        mouseover2 = mouseover;
        mouseover = mouseover->move(i);
        found = true;
        break;
        }
      }
    if(!found) return;
    }
  // probably some error... just return the original
  mouseover = omouseover;
  }

EX transmatrix Viewbase;

EX bool no_wall_rendering;

EX bool set_multi = false;
EX hyperpoint multi_point;

EX void center_multiplayer_map(const vector<hyperpoint>& hs) {
  hyperpoint h = Hypc;
  for(auto h1: hs) h += h1;
  h /= isize(hs);
  h = cgi.emb->normalize_flat(h);
  cwtV = shiftless(rgpushxto0(h));
  if(isize(hs) == 2) {
    set_multi = true;
    multi_point = hs[1];
    }
  }

EX debugflag debug_map = {"graph_map"};

EX void drawthemap() {
  indenter_finish(debug_map, "drawthemap");

  check_cgi();
  cgi.require_shapes();
  
  last_firelimit = firelimit;
  firelimit = 0;

  make_clipping_planes();
  current_display->radarpoints.clear();
  current_display->radarlines.clear();
  callhooks(hooks_drawmap);

  frameid++;
  cells_drawn = 0;
  cells_generated = 0;
  noclipped = 0;
  first_cell_to_draw = true;
  
  if(sightrange_bonus > 0 && !allowIncreasedSight()) 
    sightrange_bonus = 0;
  
  swap(gmatrix0, gmatrix);
  gmatrix.clear();
  current_display->all_drawn_copies.clear();

  wmspatial = vid.wallmode == 4 || vid.wallmode == 5;
  wmescher = vid.wallmode == 3 || vid.wallmode == 5;
  wmplain = vid.wallmode == 2 || vid.wallmode == 4;
  wmascii = vid.wallmode == 0 || vid.wallmode == 6;
  wmascii3 = vid.wallmode == 6;
  wmblack = vid.wallmode == 1;
  
  mmitem = vid.monmode >= 1;
  mmmon = vid.monmode >= 2;
  mmspatial = vid.monmode >= 3;
  
  mmhigh = vid.highlightmode >= 1;
  if(hiliteclick) mmhigh = !mmhigh;

  current_display->set_all(0, 0); /* so that non_spatial_model works correctly */
  
  spatial_graphics = wmspatial || mmspatial;
  spatial_graphics = spatial_graphics && GDIM == 2;
  #if CAP_RUG
  if(rug::rugged && !rug::spatial_rug) spatial_graphics = false;
  #endif
  if(non_spatial_model())
    spatial_graphics = false;
  if(pmodel == mdDisk && abs(pconf.alpha) < 1e-6) spatial_graphics = false;
  
  if(!spatial_graphics) wmspatial = mmspatial = false;
  if(GDIM == 3) wmspatial = mmspatial = true;

  for(int m=0; m<motypes; m++) if(isPrincess(eMonster(m))) 
    minf[m].name = princessgender() ? "Princess" : "Prince";
  
  #if CAP_RAY
  ray::in_use = ray::requested();
  #endif
  no_wall_rendering = ray::in_use;
  // ray::comparison_mode = true;
  if(ray::comparison_mode) no_wall_rendering = false;
    
  iinf[itSavedPrincess].name = minf[moPrincess].name;

  for(int i=0; i<NUM_GS; i++) {
    genderswitch_t& g = genderswitch[i];
    if(g.gender != princessgender()) continue;
    minf[g.m].help = g.desc;
    minf[g.m].name = g.name;
    }

  if(mapeditor::autochoose) mapeditor::ew = mapeditor::ewsearch;
  mapeditor::ewsearch.dist = 1e30;
  modist = 1e20; mouseover = NULL; 
  modist2 = 1e20; mouseover2 = NULL; 

  compute_graphical_distance();

  for(int i=0; i<multi::players; i++) {
    multi::ccdist[i] = 1e20; multi::ccat[i] = NULL;
    }

  downseek.reset();

  #if ISMOBILE
  mouseovers = XLAT("No info about this...");
  #endif
  if(mouseout() && !mousepan) 
    modist = -5;
  playerfound = false;
  // playerfoundL = false;
  // playerfoundR = false;
  
  arrowtraps.clear();

  make_actual_view();
  currentmap->draw_all();
  drawWormSegments();
  drawBlizzards();
  drawArrowTraps();
  
  precise_mouseover();
  
  ivoryz = false;
  
  linepatterns::drawAll();
  
  callhooks(hooks_frame);
  
  drawMarkers();
  drawFlashes();
  
  mapeditor::draw_dtshapes();
  set_multi = false;
  
  if(multi::players > 1 && !shmup::on) {
    if(multi::split_screen)
      cwtV = multi::whereis[subscreens::current_player];
    else if(multi::centerplayer != -1)
      cwtV = multi::whereis[multi::centerplayer];
    else {
      vector<hyperpoint> pts;
      for(int p=0; p<multi::players; p++) if(multi::playerActive(p))
        pts.push_back(unshift(multi::whereis[p] * tile_center()));
      center_multiplayer_map(pts);
      }
    }
  
  if(shmup::on) {
    if(multi::split_screen)
      cwtV = shmup::pc[subscreens::current_player]->pat;
    else if(multi::players == 1)
      cwtV = shmup::pc[0]->pat;
    else if(multi::centerplayer != -1)
      cwtV = shmup::pc[multi::centerplayer]->pat;
    else {
      vector<hyperpoint> pts;
      for(int p=0; p<multi::players; p++)
        pts.push_back(unshift(shmup::pc[p]->pat * tile_center()));
      center_multiplayer_map(pts);
      }
    }

  #if CAP_SDL
  const sdl_keystate_type *keystate = SDL12_GetKeyState(NULL);
  lmouseover = mouseover;
  lmouseover_distant = lmouseover;
  bool useRangedOrb = (!(vid.shifttarget & 1) && haveRangedOrb() && lmouseover && lmouseover->cpdist > 1) || (keystate[SDL12(SDLK_RSHIFT, SDL_SCANCODE_RSHIFT)] | keystate[SDL12(SDLK_LSHIFT, SDL_SCANCODE_LSHIFT)]);
  if(!useRangedOrb && !(cmode & sm::MAP) && !(cmode & sm::DRAW) && DEFAULTCONTROL && !mouseout() && !dual::state) {
    dynamicval<eGravity> gs(gravity_state, gravity_state);
    calcMousedest();
    cellwalker cw = cwt; bool f = flipplayer;
    items[itWarning]+=2;
    
    movepcto(mousedest.d, mousedest.subdir, true);
    items[itWarning] -= 2;
    if(cw.spin != cwt.spin) mirror::act(-mousedest.d, mirror::SPINSINGLE);
    cwt = cw; flipplayer = f;
    lmouseover = mousedest.d >= 0 ? cwt.at->modmove(cwt.spin + mousedest.d) : cwt.at;
    }
  #endif
  }

// old style joystick control

EX bool dronemode;

purehookset hooks_calcparam;

EX int corner_centering;

EX bool permaside;

EX bool old_center;

EX ld min_scale = 1e-6;

EX int forced_center_down = ISANDROID ? 2 : ISIOS ? 40 : 40;

EX ld get_stereo_param() {
  if(among(vid.stereo_mode, sPanini, sStereographic)) return vid.stereo_param;
  return 0;
  }

EX debugflag debug_calcparam = {"graph_param"};

EX void calcparam() {

  indenter_finish(debug_calcparam, "calcparam");
  auto cd = current_display;
  
  cd->xtop = vid.xres * cd->xmin;
  cd->ytop = vid.yres * cd->ymin;
  
  cd->xsize = vid.xres * (cd->xmax - cd->xmin);
  cd->ysize = vid.yres * (cd->ymax - cd->ymin);

  cd->xcenter = cd->xtop + cd->xsize / 2;
  cd->ycenter = cd->ytop + cd->ysize / 2;

  if(abs(pconf.scale) < min_scale) pconf.scale = 1;
  
  ld realradius = min(cd->xsize / 2, cd->ysize / 2);
  
  cd->scrsize = realradius;
  if(!inHighQual) cd->scrsize -= forced_center_down;

  current_display->sidescreen = permaside;
  
  if(vid.xres < vid.yres - 2 * vid.fsize && !inHighQual && (old_center || !in_perspective())) {
    cd->ycenter = lerp(vid.fsize + cd->scrsize, vid.yres - cd->scrsize - vid.fsize, .8);
    }
  else {
    bool ok = !vrhr::active();
    if(vid.xres > vid.yres * 4/3+16 && (cmode & sm::SIDE) && ok && !((cmode & sm::MAYDARK) && centered_menus))
      current_display->sidescreen = true;
#if CAP_TOUR
    if(tour::on && (tour::slides[tour::currentslide].flags & tour::SIDESCREEN) && ok)
      current_display->sidescreen = true;
#endif
    if((cmode & sm::DIALOG_OFFMAP) && vid.xres > vid.yres * 11/10)
      current_display->sidescreen = true;

    if(current_display->sidescreen) cd->xcenter = vid.yres/2;
    }

  cd->radius = pconf.scale * cd->scrsize;
  if(GDIM == 3 && in_perspective()) cd->radius = cd->scrsize;
  realradius = min(realradius, cd->radius);
  
  ld aradius = sphere ? cd->radius / (pconf.alpha - 1) : cd->radius;
  #if MAXMDIM >= 4
  if(euclid && hybrid::drawing_underlying) aradius *= 2.5;
  #endif
  
  if(dronemode) { cd->ycenter -= cd->radius; cd->ycenter += vid.fsize/2; cd->ycenter += vid.fsize/2; cd->radius *= 2; }
  
  if(corner_centering) {
    cd->ycenter = cd->ytop + cd->ysize - vid.fsize - aradius;
    if(corner_centering == 1)
      cd->xcenter = cd->xtop + vid.fsize + aradius;
    if(corner_centering == 2)
      cd->xcenter = cd->xtop + cd->xsize - vid.fsize - aradius;
    }

  cd->xcenter += cd->scrsize * pconf.xposition;
  cd->ycenter += cd->scrsize * pconf.yposition;
  
  ld fov = vid.fov * degree / 2;
  cd->tanfov = sin(fov) / (cos(fov) + get_stereo_param());
  
  #if CAP_SDLTTF
  set_cfont();
  #endif
  callhooks(hooks_calcparam);
  reset_projection();
  }

EX function<void()> wrap_drawfullmap = drawfullmap;

bool force_sphere_outline = false;

EX void drawfullmap() {
  indenter_finish(debug_map, "drawfullmap");

  check_cgi();
  cgi.require_shapes();

  ptds.clear();

  
  /*
  if(models::on) {
    char ch = 'A';
    for(auto& v: history::v) {
      queuepoly(ggmatrix(v->base) * v->at, cgi.shTriangle, 0x306090C0);
      queuestr(ggmatrix(v->base) * v->at * C0, 10, s0+(ch++), 0xFF0000);
      }      
    }
  */
  
  #if CAP_QUEUE
  draw_boundary(0);
  draw_boundary(1);
  
  draw_model_elements();
  #if MAXMDIM >= 4 && CAP_GL
  prepare_sky();
  #endif
  #endif
  
  /* if(vid.wallmode < 2 && !euclid && !patterns::whichShape) {
    int ls = isize(lines);
    if(ISMOBILE) ls /= 10;
    for(int t=0; t<ls; t++) queueline(View * lines[t].P1, View * lines[t].P2, lines[t].col >> (darken+1));
    } */

  clearaura();
  if(!nomap) drawthemap();
  else callhooks(hooks_frame);

  if(!inHighQual) {
    if((cmode & sm::NORMAL) && !rug::rugged) {
      if(multi::players > 1) {
        auto bcwtV = cwtV;
        for(int i=0; i<multi::players; i++) if(multi::playerActive(i))
          cwtV = multi::whereis[i], multi::cpid = i, drawmovestar(multi::mdx[i], multi::mdy[i]);
        cwtV = bcwtV;
        }
      else if(multi::alwaysuse)
        drawmovestar(multi::mdx[0], multi::mdy[0]);
      else 
        drawmovestar(0, 0);
      }
#if CAP_EDIT
    if(cmode & sm::DRAW) mapeditor::drawGrid();
#endif
    }

  drawaura();
  #if CAP_QUEUE
  drawqueue();
  #endif
  }

#if ISMOBILE
extern bool wclick;
#endif

// 0 = not refreshing, 1 = refreshing for keyboard, 2 = refreshing for mouse position analysis
EX int just_refreshing;

EX int menu_darkening = 2;
EX bool centered_menus = false;

EX string menu_format = "";

EX void gamescreen() {

  if(cmode & sm::NOSCR) {
    stillscreen = true;
    emptyscreen();
    return;
    }

  if(just_refreshing == 1) return;

  if(subscreens::split([=] () {
    calcparam();
    compute_graphical_distance();
    gamescreen();
    })) {
    if(racing::on) return;
    // create the gmatrix
    View = subscreens::player_displays[0].view_matrix;
    centerover = subscreens::player_displays[0].precise_center;
    just_gmatrix = true;
    currentmap->draw_all();
    just_gmatrix = false;
    return;
    }
  
  stillscreen = false;

  auto gx = vid.xres;
  auto gy = vid.yres;

  if(dual::split([=] () { 
    vid.xres = gx;
    vid.yres = gy;
    dual::in_subscreen([=] () { gamescreen(); });
    })) {
    calcparam(); 
    return; 
    }
  
  calcparam();
  
  darken = 0;

  if(!inHighQual && !vrhr::active()) {
    if((cmode & sm::MAYDARK) && !current_display->sidescreen)
      darken += menu_darkening;
    else if(cmode & sm::DARKEN)
      darken += menu_darkening;
    }
  if(vid.highlightmode == (hiliteclick ? 0 : 2))
    darken++;
  if(darken >= 8) {
    emptyscreen();
    return;
    }

  if(history::includeHistory) history::restore();

  festive = festive_date && festive_option;
  old_shines = std::move(shines); shines.clear();

  anims::apply();
#if CAP_RUG
  if(rug::rugged) {
    if(!nomap) rug::actDraw();
    } else
#endif
  wrap_drawfullmap();
  anims::rollback();

  if(history::includeHistory) history::restoreBack();

  poly_outline = OUTLINE_DEFAULT;
  
  #if ISMOBILE
  
  buttonclicked = false;
  
  if((cmode & sm::NORMAL) && vid.stereo_mode != sLR && !inHighQual) {
    if(andmode == 0 && shmup::on) {
      using namespace shmupballs;
      calc();
      drawCircle(xmove, yb, rad, OUTLINE_FORE);
      drawCircle(xmove, yb, rad/2, OUTLINE_FORE);
      drawCircle(xfire, yb, rad, 0xFF0000FF);
      drawCircle(xfire, yb, rad/2, 0xFF0000FF);
      }
    else {
      if(!haveMobileCompass()) displayabutton(-1, +1, andmode == 0 && useRangedOrb ? XLAT("FIRE") : andmode == 0 && WDIM == 3 && wclick ? XLAT("WAIT") : XLAT("MOVE"),  andmode == 0 ? BTON : BTOFF);
      displayabutton(+1, +1, rug::rugged ? XLAT("RUG") :andmode == 1 ?  XLAT("BACK") : GDIM == 3 ? XLAT("CAM") : XLAT("DRAG"),  andmode == 1 ? BTON : BTOFF);
      }
    displayabutton(-1, -1, XLAT("INFO"),  andmode == 12 ? BTON : BTOFF);
    displayabutton(+1, -1, XLAT("MENU"), andmode == 3 ? BTON : BTOFF);
    }
  
  #endif
  
  darken = 0;

#if CAP_TEXTURE
  if(texture::config.tstate == texture::tsAdjusting) 
    texture::config.drawRawTexture();
#endif

  #if CAP_VR
  vrhr::size_and_draw_ui_box();  
  #endif
  }

EX void emptyscreen() {
  check_cgi();
  cgi.require_shapes();
  make_actual_view();
  ptds.clear();
  ray::in_use = false;
  drawqueue();
  }

EX int nohelp;
EX bool no_find_player;

EX void show_menu_button() {
  if(menu_format != "")
    displayButton(vid.xres-8, vid.yres-vid.fsize, eval_programmable_string(menu_format), 'v', 16);
  else if(nomenukey || ISMOBILE)
    ;
#if CAP_TOUR
  else if(tour::on)
    displayButton(vid.xres-8, vid.yres-vid.fsize, XLAT("(ESC) tour menu"), SDLK_ESCAPE, 16);
#endif
  else if(dialog::display_keys == 3)
    displayButton(vid.xres-8, vid.yres-vid.fsize, XLAT("menu"), SDLK_ESCAPE, 16);
  else
    displayButton(vid.xres-8, vid.yres-vid.fsize, XLAT("(v) menu"), 'v', 16);
  }

EX void normalscreen() {
  help = "@";

  mouseovers = standard_help();

#if CAP_TOUR  
  if(tour::on) mouseovers = (tour::slides[tour::currentslide].flags & tour::NOTITLE) ? "" : tour::tourhelp;
#endif

  if(GDIM == 3 || !outofmap(mouseh.h)) getcstat = '-';
  cmode = sm::NORMAL | sm::DOTOUR | sm::CENTER;
  if(viewdists && show_distance_lists) cmode |= sm::SIDE | sm::MAYDARK;
  if(tour::on && (tour::slides[tour::currentslide].flags & tour::SIDE)) cmode |= sm::SIDE;
  gamescreen(); drawStats();

  show_menu_button();
  keyhandler = handleKeyNormal;
  dialog::key_actions.clear();

  if(!playerfound && !anims::any_on() && !sphere && !no_find_player && mapeditor::drawplayer)
    displayButton(current_display->xcenter, current_display->ycenter, (!dialog::actual_display_keys()) ? XLAT("find the player") : XLAT("press SPACE to find the player"), ' ', 8);

  if(!mapeditor::drawplayer && playermoved && !no_find_player && dialog::actual_display_keys())
    displayButton(current_display->xcenter, current_display->ycenter, XLAT("move the camera with arrow keys and Home/End"), PSEUDOKEY_NOHINT, 8);

  describeMouseover();
  }

EX vector< function<void()> > screens = { normalscreen };

#if HDR
inline void popScreen() { if(isize(screens)>1) screens.pop_back(); }
inline void popScreenAll() { while(isize(screens)>1) popScreen(); }
typedef void (*cfunction)();
#endif

EX cfunction current_screen_cfunction() {
  auto tgt = screens.back().target<cfunction>();
  if(!tgt) return nullptr;
  return *tgt;
  }

#if HDR
namespace sm {
  static constexpr flagtype NORMAL = 1;
  static constexpr flagtype MISSION = 2;
  static constexpr flagtype HELP = 4;
  static constexpr flagtype MAP = 8;
  static constexpr flagtype DRAW = 16;
  static constexpr flagtype NUMBER = 32;
  static constexpr flagtype SHMUPCONFIG = 64;
  static constexpr flagtype OVERVIEW = 128;
  static constexpr flagtype SIDE = 256;
  static constexpr flagtype DOTOUR = 512;
  static constexpr flagtype CENTER = 1024;
  static constexpr flagtype ZOOMABLE = 4096;
  static constexpr flagtype TORUSCONFIG = 8192;
  static constexpr flagtype MAYDARK = 16384; // use together with SIDE; if the screen is not wide or centered_menus is set, it will disable SIDE and instead darken the screen
  static constexpr flagtype DIALOG_STRICT_X = 32768; // do not interpret dialog clicks outside of the X region
  static constexpr flagtype EXPANSION = Flag(16);
  static constexpr flagtype HEXEDIT = Flag(17);
  static constexpr flagtype VR_MENU = Flag(18); // always show the menu in VR
  static constexpr flagtype SHOWCURSOR = Flag(19); // despite MAP/DRAW always show the cursor, no panning
  static constexpr flagtype PANNING = Flag(20); // smooth scrolling works
  static constexpr flagtype DARKEN = Flag(21); // darken the game background
  static constexpr flagtype NOSCR = Flag(22); // do not show the game background
  static constexpr flagtype AUTO_VALUES = Flag(23); // automatic place for values
  static constexpr flagtype NARROW_LINES = Flag(24); // do make the lines narrower if we needed to reduce width
  static constexpr flagtype EDIT_BEFORE_WALLS = Flag(25); // mouseover targets before walls
  static constexpr flagtype EDIT_INSIDE_WALLS = Flag(26); // mouseover targets inside walls
  static constexpr flagtype DIALOG_WIDE = Flag(27); // make dialogs wide
  static constexpr flagtype MOUSEAIM = Flag(28); // mouse aiming active here
  static constexpr flagtype DIALOG_OFFMAP = Flag(29); // try hard to keep dialogs off the map
  static constexpr flagtype NO_EXIT = Flag(30); // do not allow to exit the current dialog
  }
#endif

EX flagtype cmode;

EX bool dont_display_minecount = false;

EX color_t titlecolor;

EX void drawscreen() {

  indenter_finish(debug_map, "drawscreen");
  #if CAP_GL
  GLWRAP;
  #endif

  if(vid.xres == 0 || vid.yres == 0) return;

  calcparam();
  // rug::setVidParam();

#if CAP_GL
  if(vid.usingGL) setGLProjection();
#endif

#if CAP_XGD
  if(!vid.usingGL) {
    gdpush(5); gdpush(backcolor);
    }
#endif

#if CAP_VR
  vrhr::clear();
#endif

  
  #if CAP_SDL
  // SDL_LockSurface(s);
  // unsigned char *b = (unsigned char*) s->pixels;
  // int n = vid.xres * vid.yres * 4;
  // while(n) *b >>= 1, b++, n--;
  // memset(s->pixels, 0, vid.xres * vid.yres * 4);
  #if CAP_GL
  if(!vid.usingGL) 
  #endif
    SDL_FillSurfaceRect(s, NULL, backcolor);
  #endif
   
  // displaynum(vx,100, 0, 24, 0xc0c0c0, celldist(cwt.at), ":");
  
  lgetcstat = getcstat;
  getcstat = 0; inslider = false;
  
  mouseovers = " ";

  cmode = 0;
  reset_handlers();
  if(!isize(screens)) pushScreen(normalscreen);
  screens.back()();

#if !ISMOBILE
  color_t col = linf[cwt.at->land].color;
  if(cwt.at->land == laRedRock) col = 0xC00000;
  if(titlecolor) col = titlecolor;
  if(nohelp != 1) {
    int size = vid.fsize;
    while(size > 3 && textwidth(size, mouseovers) > vid.xres) size--;
    poly_outline = (backcolor << 8) | 0xFF;
    displayfr(vid.xres/2, vid.fsize,   2, size, mouseovers, col, 8);
    }
#endif

  drawmessages();
  
  bool normal = cmode & sm::NORMAL;
  
  if((havewhat&HF_BUG) && darken == 0 && normal) if(hive::bugcount[0] || hive::bugcount[1] || hive::bugcount[2]) for(int k=0; k<3; k++)
    displayfr(vid.xres/2 + vid.fsize * 5 * (k-1), vid.fsize*2,   2, vid.fsize, 
      its(hive::bugcount[k]), minf[moBug0+k].color, 8);
    
  bool minefieldNearby = false;
  unsigned mines[MAXPLAYER], tmines=0;
  for(int p=0; p<numplayers(); p++) {
    mines[p] = 0;
    cell *c = playerpos(p);
    if(!c) continue;
    for(cell *c2: adj_minefield_cells(c)) {
      if(c2->land == laMinefield) 
        minefieldNearby = true;
      if(c2->wall == waMineMine) {
        bool ep = false;
        if(!ep) mines[p]++, tmines++;
        }
      }
    }

  if((minefieldNearby || tmines) && !items[itOrbAether] && !last_gravity_state && darken == 0 && normal) {
    string s;
    if(tmines > 9) tmines = 9;
    color_t col = minecolors[tmines];
    
    if(tmines == 7) seenSevenMines = true;
    
    if(!dont_display_minecount) for(int p: player_indices())
      displayfr(vid.xres * (p+.5) / numplayers(),
        current_display->ycenter - current_display->radius * 3/4, 2,
        vid.fsize, 
        mines[p] >= sizeof(minetexts) / sizeof(minetexts[0]) ? its(mines[p]) : XLAT(minetexts[mines[p]]), minecolors[mines[p]], 8);

    if(minefieldNearby && !shmup::on && cwt.at->land != laMinefield && cwt.peek()->land != laMinefield && !dont_display_minecount) {
      displayfr(vid.xres/2, current_display->ycenter - current_display->radius * 3/4 - vid.fsize*3/2, 2,
        vid.fsize, 
        XLAT("WARNING: you are entering a minefield!"), 
        col, 8);
      }
    }

  // SDL_UnlockSurface(s);

  glflush();
  if(debug_map) println(hlog, "swapbuffers");

  #if CAP_VR
  vrhr::submit();
  #endif
  
  #if CAP_SDL
  present_screen();
  #endif

#if CAP_VR
  vrhr::handoff();
#endif
  
//printf("\ec");
  }

EX debugflag debug_init_graph = {"init_graph"};

EX void restartGraph() {
  indenter_finish di(debug_init_graph, "restartGraph");
  
  if(!autocheat) linepatterns::clearAll();
  if(currentmap) resetview();
  }

EX debugflag debug_graph_memory = {"graph_memory"};

auto graphcm = addHook(hooks_clearmemory, 0, [] () {
  indenter_finish di(debug_graph_memory, "graph_memory");
  mouseover = centerover = lmouseover = NULL;  
  gmatrix.clear(); gmatrix0.clear(); current_display->all_drawn_copies.clear();
  clearAnimations();
  })
+ addHook(hooks_gamedata, 0, [] (gamedata* gd) {
  gd->store(mouseover);
  gd->store(lmouseover);
  gd->store(current_display->radar_transform);
  gd->store(actual_view_transform);
  });

EX void drawBug(const cellwalker& cw, color_t col) {
#if CAP_SHAPES
  initquickqueue();
  shiftmatrix V = ggmatrix(cw.at);
  if(cw.spin) V = V * ddspin180(cw.at, cw.spin);
  queuepoly(V, cgi.shBugBody, col);
  quickqueue();
#endif
  }

EX bool inscreenrange_actual(cell *c) {
  if(GDIM == 3) return true;
  hyperpoint h1; applymodel(ggmatrix(c) * tile_center(), h1);
  if(invalid_point(h1)) return false;
  auto hscr = toscrcoord(h1);
  auto& x = hscr[0], y = hscr[1];
  if(x > current_display->xtop + current_display->xsize) return false;
  if(x < current_display->xtop) return false;
  if(y > current_display->ytop + current_display->ysize) return false;
  if(y < current_display->ytop) return false;
  return true;
  }

EX bool inscreenrange(cell *c) {
  if(sphere) return true;
  if(euclid) return celldistance(centerover, c) <= get_sightrange_ambush() && inscreenrange_actual(c);
  if(nonisotropic) return gmatrix.count(c);
  if(geometry == gCrystal344) return gmatrix.count(c) && inscreenrange_actual(c);
  auto hd = heptdistance(centerover, c);
  if(hd <= 1) return true;
  return hd <= 8 && inscreenrange_actual(c);
  }

#if MAXMDIM >= 4
auto hooksw = addHook(hooks_swapdim, 100, [] { clearAnimations(); gmatrix.clear(); gmatrix0.clear(); current_display->all_drawn_copies.clear(); });
#endif

}
