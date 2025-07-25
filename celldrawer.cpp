#include "hyper.h"
namespace hr {

#if HDR
int coastvalEdge(cell *c);

struct spatial_info {
  SIDE top, deep;
  int levels;
  int sl; /* snake level */
  };

struct celldrawer {
  cell *c;
  shiftmatrix V;

  color_t fcol;
  color_t wcol;
  color_t wcol_star;
  color_t asciicol;
  color_t aura_color;
  int fd;
  int ct6;
  bool error;
  bool onradar;
  char asciichar;
  shiftmatrix Vboat;
  shiftmatrix Vd;
  spatial_info sha;
  color_t asciiborder;
  color_t asciicol1;
  char asciichar1;
  
  void addaura();
  void setcolors();
  void tune_colors();
  int getSnakelevColor(int i, int last);
  void draw_wall();
  void draw_boat();
  void draw_grid();
  void draw_grid_edge(int t, color_t col, int prec);
  void draw_ceiling();
  void draw_halfvine();
  void draw_mirrorwall();
  bool set_randompattern_floor();
  void draw_features();
  void draw_features_and_walls_3d();
  void check_rotations();
  void drawcell_in_radar();
  void bookkeeping();
  void draw_cellstat();
  void draw_wall_full();
  void draw_wallshadow();
  void draw_item_full();
  void draw_monster_full();
  void add_map_effects();
  void draw();
  bool cell_clipped();
  void draw_fallanims();
  bool draw_shmup_monster();
  void draw_gravity_particles();

  void set_land_floor(const shiftmatrix& Vf);
  void set_towerfloor(const cellfunction& cf = coastvalEdge);
  void set_zebrafloor();
  void set_maywarp_floor();
  void set_reptile_floor(const shiftmatrix& V, color_t col, bool nodetails = false);
  void set_emeraldfloor();
  void shmup_gravity_floor();

  void draw_reptile(color_t col);

  void radar_grid();
  
  void do_viewdist();

  void draw_bowpath();
  };

inline void drawcell(cell *c, const shiftmatrix& V) {
  celldrawer dd;
  dd.c = c;
  dd.V = V;
  dd.draw();
  }
#endif

static constexpr int trapcol[4] = {0x904040, 0xA02020, 0xD00000, 0x303030};
static constexpr int terracol[8] = {0xD000, 0xE25050, 0xD0D0D0, 0x606060, 0x303030, 0x181818, 0x0080, 0x8080};

EX colortable prairie_colors = { 0x402000, 0x503000 };
EX colortable prairie_colors_high_cont = { 0x102030, 0x905010 };
EX colortable mountain_colors = { 0x181008*2, 0x181008*4 };
EX colortable tower_colors = { 0x202010, 0x404030 };
EX colortable westwall_colors = { 0x211F6F, 0x413F8F };
EX colortable endorian_colors = { 0x202010, 0x404030, 0x0000D0 };
EX colortable canopy_colors = { 0x60C060, 0x489048 };
EX colortable camelot_cheat_colors = { 0x606060, 0xC0C0C0 };

/** return the special colortable for the given cell -- color menu uses this to know that a colortable should be edited */
EX colortable* special_colortable_for(cell *c) {
  if(c->land == laPrairie && prairie::isriver(c)) return &prairie_colors;
  if(c->land == laMountain && !c->wall && (eubinary || sphere || c->master->alt)) return &mountain_colors;
  if(c->land == laIvoryTower && !c->wall) return &tower_colors;
  if(c->land == laWestWall) return &westwall_colors;
  if(c->land == laEndorian && !c->wall) return &endorian_colors;
  if(c->land == laEndorian && c->wall == waCanopy) return &canopy_colors;
  if(c->land == laCamelot && camelotcheat) return &camelot_cheat_colors;
  return nullptr;
  }

void celldrawer::addaura() {
  hr::addaura(tC0(V), darkened(aura_color), fd);
  }

/* Eclectic City's version of Red Rock is of slightly different color, */
/* to make it different from hot cells */
void eclectic_red(color_t& col) {
  if (!higher_contrast) {
    part(col, 0) = part(col, 2) * 3 / 4;
    } else {
    auto v = part(col, 0) + part(col, 1) + part(col, 2);
    auto t = part(col, 0);
    part(col, 0) = part(col, 1);
    part(col, 1) = part(col, 2);
    part(col, 2) = t + v/3;
    }
  }

constexpr ld spinspeed = .75 / M_PI;

EX color_t apply_mine_knowledge(color_t wcol, cell* c) {
  #if CAP_COMPLEX2
  if(mine::marked_safe(c))
    return gradient(wcol, 0x40FF40, 0, 0.2, 1);
  if(mine::marked_mine(c))
    return gradient(wcol, 0xFF4040, -1, vid.ispeed ? sintick(100) : 1, 1);
  #endif
  return wcol;
  }

void celldrawer::setcolors() {

  wcol = fcol = winf[c->wall].color;

  if(c->wall == waMineMine)
    wcol = fcol = winf[waMineUnknown].color;

  // water colors
  if(isWateryOrBoat(c) || c->wall == waReptileBridge) {
    if(c->land == laOcean)
      fcol = 
        #if CAP_FIELD
        (c->landparam > 25 && !ls::any_chaos()) ? ( 
          0x90 + 8 * sintick(1000, windmap::windcodes[windmap::getId(c)] / 256.)
          ) : 
        #endif
        0x1010C0 + int(32 * sintick(500, (ls::any_chaos() ? c->CHAOSPARAM : c->landparam)*spinspeed));
    else if(c->land == laOceanWall)
      fcol = 0x2020FF;
    else if(c->land == laVariant)
      fcol = 0x002090 + 15 * sintick(300, 0);
    else if(c->land == laKraken) {
      fcol = 0x0000A0;
      int mafcol = (kraken_pseudohept(c) ? 64 : 8);
      /* bool nearshore = false;
      for(int i=0; i<c->type; i++) 
        if(c->move(i)->wall != waSea && c->move(i)->wall != waBoat)
          nearshore = true;
      if(nearshore) mafcol += 30; */
      fcol = fcol + mafcol * (4+sintick(500, ((eubinary||c->master->alt) ? celldistAlt(c) : 0)*spinspeed))/5;
      }
    else if(c->land == laDocks) {
      fcol = 0x0000A0;
      }
    else if(c->land == laAlchemist)
      fcol = 0x900090;
    else if(c->land == laWhirlpool)
      fcol = 0x0000C0 + int(32 * sintick(200, ((eubinary||c->master->alt) ? celldistAlt(c) : 0)*spinspeed));
    else if(c->land == laLivefjord)
      fcol = 0x000080;
    else if(isWarpedType(c->land))
      fcol = 0x0000C0 + int((warptype(c)?30:-30) * sintick(600));
    else
      fcol = wcol;
    }
  
  else if(c->wall == waShallow) 
    fcol = 0x40C0C0;

  // floor colors for all the lands
  else switch(c->land) {
    case laBurial: case laTrollheim: case laBarrier: case laOceanWall:
    case laCrossroads2: case laCrossroads3: case laCrossroads4: case laCrossroads5: case laCrossroads6: case laMasterCrossroads:
    case laRose: case laPower: case laWildWest: case laHalloween: case laRedRock:
    case laDragon: case laStorms: case laTerracotta: case laMercuryRiver:
    case laDesert: case laKraken: case laDocks: 
    case laMotion: case laGraveyard: case laWineyard: case laLivefjord: 
    case laRlyeh: case laHell: case laCrossroads: case laJungle:
    case laAlchemist: case laFrog: case laCursed: case laDice:
      fcol = floorcolors[c->land]; break;

    case laCA:
      fcol = floorcolors[c->land]; 
      if(geosupport_chessboard()) {
        if(chessvalue(c)) fcol += 0x202020;
        }
      else if(geosupport_threecolor()) {
        fcol += 0x202020 * pattern_threecolor(c);
        }
      break;
    
    case laWet:
      fcol = 0x40FF40; break;
    
    #if CAP_COMPLEX2
    case laVariant: {
      int b = getBits(c);
      fcol = 0x404040;
      for(int a=0; a<21; a++)
        if((b >> a) & 1)
          fcol += variant::features[a].color_change;
      if(!higher_contrast) {
        if(c->wall == waAncientGrave)
          wcol = 0x080808;
        else if(c->wall == waFreshGrave)
          wcol = 0x202020;
        }
      break;
      }
    #endif
    
    case laRuins:
      fcol = pseudohept(c) ? 0xC0E0C0 : 0x40A040;
      break;
    
    case laDual:
      fcol = floorcolors[c->land];
      if(c->landparam == 2) fcol = 0x40FF00;
      if(c->landparam == 3) fcol = 0xC0FF00;
      break;

#if CAP_COMPLEX2
    case laBrownian: {
      if (c->wall == waNone)
        fcol = wcol = brownian::get_color(c->landparam);
      break;
      }
#endif
      
#if CAP_FIELD
    case laVolcano: {
      int id = lavatide(c, -1)/4;
      if(c->wall == waMagma) 
        fcol = wcol = magma_color(id);
      else if(c->wall == waNone) {
        fcol = wcol = 0x404040;
        if(id == 255/4) fcol = 0xA0A040;
        if(id == 255/4-1) fcol = 0x606040;
        }
      /* {
        if(id/4 == 255/4) fd = 0;
        if(id/4 == 95/4-1 || id/4 == 255/4-1) fd = 1;
        } */
      break;
      }
#endif

    case laMinefield: 
      fcol = floorcolors[c->land];
      if(c->wall == waMineMine && ((cmode & sm::MAP) || !canmove))
        fcol = wcol = 0xFF4040;
      break;
      
    case laCaribbean: 
      if(c->wall != waCIsland && c->wall != waCIsland2)
        fcol = floorcolors[c->land];
      break;

    case laReptile:
      fcol = reptilecolor(c);
      break;

    case laCaves: case laEmerald: case laDeadCaves:
      fcol = 0x202020;
      if(c->land == laEmerald) 
      if(c->wall == waCavefloor || c->wall == waCavewall) {
        fcol = wcol = gradient(winf[waCavefloor].color, 0xFF00, 0, 0.5, 1);
        if(c->wall == waCavewall) wcol = 0xC0FFC0;
        }
      break;
    case laMirror: case laMirrorWall: case laMirrorOld:
      if(c->land == laMirrorWall) fcol = floorcolors[laMirror];
      else fcol = floorcolors[c->land];
      break;
    case laDryForest:
      fcol = gradient(0x008000, 0x800000, 0, c->landparam, 10);
      break;    
    case laMountain:
      if(eubinary || sphere || c->master->alt)
        fcol = get_color_auto3(celldistAlt(c), mountain_colors);
      else fcol = 0;
      if(c->wall == waPlatform) wcol = 0xF0F0A0;
      break;
    case laCanvas:
      fcol = c->landparam;
      if(c->wall == waWaxWall) wcol = c->landparam;
      break;
    case laPalace:
      fcol = floorcolors[c->land];
      if(c->wall == waClosedGate || c->wall == waOpenGate)
        fcol = wcol;
      break;
    case laElementalWall:
      fcol = (linf[c->barleft].color>>1) + (linf[c->barright].color>>1);
      break;
    case laZebra:
      fcol = floorcolors[c->land];
      if(c->wall == waTrapdoor) fcol = 0x808080;
      break;
    case laHive:
      fcol = linf[c->land].color;
      if(c->wall == waWaxWall) wcol = c->landparam;
      if(items[itOrbInvis] && c->wall == waNone && c->landparam)
        fcol = gradient(fcol, 0xFF0000, 0, c->landparam, 100);
      if(among(int(c->bardir), NOBARRIERS, NOBARRIERS2) && c->barleft)
        fcol = minf[moBug0+c->barright].color;
      break;
    case laSwitch:
      fcol = minf[passive_switch].color;
      break;
    case laTortoise:
      fcol = tortoise::getMatchColor(getBits(c));
      if(c->wall == waBigTree) wcol = 0x709000;
      else if(c->wall == waSmallTree) wcol = 0x905000;
      break;
    case laOvergrown: case laClearing:
      fcol = floorcolors[c->land];
      if(c->wall == waSmallTree) wcol = 0x008060;
      else if(c->wall == waBigTree) wcol = 0x0080C0;
      break;
    case laTemple: {
      int d = (eubinary||c->master->alt) ? celldistAlt(c) : 99;
      if(ls::any_chaos())
        fcol = 0x405090;
      else if(d % temple_layer_size() == 0)
        fcol = gradient(0x304080, winf[waColumn].color, 0, 0.5, 1);
  //    else if(c->type == 7)
  //      wcol = 0x707070;
      else if(d% 2 == -1)
        fcol = 0x304080;
      else
        fcol = 0x405090;
      break;
      }

    case laWhirlwind:
      if(c->land == laWhirlwind) {
        color_t whirlcolors[4] = {0x404040, 0x404080, 0x2050A0, 0x5050C0};
        fcol = whirlcolors[whirlwind::fzebra3(c)];
        }
      break;

    case laHunting:
      fcol = floorcolors[c->land];
      if(pseudohept(c)) fcol = fcol * 3/4;
      break;
  
    case laIvoryTower:
      fcol = get_color_auto3(c->landparam, tower_colors);
      break;
    
    case laWestWall:
      fcol = get_color_auto3(c->landparam, westwall_colors);
      break;
    
    case laDungeon: {
      int lp = c->landparam % 5;
        // xcol = (c->landparam&1) ? 0xD00000 : 0x00D000;
      int lps[5] = { 0x402000, 0x302000, 0x202000, 0x282000, 0x382000 };
      fcol = lps[lp];
      if(c->wall == waClosedGate)
        fcol = wcol = 0xC0C0C0;
      if(c->wall == waOpenGate)
        fcol = wcol = 0x404040;
      if(c->wall == waPlatform)
        fcol = wcol = 0xDFB520;
      break;
      }
    
    case laEndorian: {
      int clev = pd_from->land == laEndorian ? edgeDepth(pd_from) : 0;
        // xcol = (c->landparam&1) ? 0xD00000 : 0x00D000;
      fcol = get_color_auto3(c->landparam, endorian_colors, 1);
      int ed = edgeDepth(c);
      int sr = get_sightrange_ambush();
      
      if(clev == UNKNOWN || ed == UNKNOWN)
        fcol = endorian_colors.back();
      else {
        while(ed > clev + sr) ed -= 2;
        while(ed < clev - sr) ed += 2;
        fcol = gradient(fcol, endorian_colors.back(), clev-sr, ed, clev+sr);
        }
      if(c->wall == waTrunk) fcol = winf[waTrunk].color;
  
      if(c->wall == waCanopy || c->wall == waSolidBranch || c->wall == waWeakBranch) {
        fcol = get_color_auto3(c->landparam, canopy_colors);
        }
      break;
      }
    
    #if CAP_FIELD
    case laPrairie:
      if(prairie::isriver(c)) {
        fcol = get_color_auto3(prairie::get_val(c),
                  higher_contrast ? prairie_colors_high_cont : prairie_colors);
        }
      else {
        fcol = 0x004000 + 0x001000 * c->LHU.fi.walldist;
        fcol += 0x10000 * (255 - 511 / (1 + max((int) c->LHU.fi.flowerdist, 1)));
        if(ls::hv_structure()) fcol = prairie::nearriver(c) ? 0x40FF00 : 0x40D000;
        // fcol += 0x1 * (511 / (1 + max((int) c->LHU.fi.walldist2, 1)));
        }
      break;
    #endif
  
    case laCamelot: {
      int d = ((eubinary||c->master->alt) ? celldistAltRelative(c) : 0);
  #if CAP_TOUR
      if(!tour::on) camelotcheat = false;
      if(camelotcheat) 
        fcol = get_color_auto3(d, camelot_cheat_colors);
      else 
  #endif
      if(d < 0) {
        fcol = floorcolors[c->land];
        }
      else {
        // a nice floor pattern
        int v = emeraldval(c);
        int v0 = (v&~3);
        bool sw = (v&1);
        if(v0 == 8 || v0 == 12 || v0 == 20 || v0 == 40 || v0 == 36 || v0 == 24)
          sw = !sw;
        if(sw)
          fcol = 0xC0C0C0;
        else
          fcol = 0xA0A0A0;
        }
      break;
      }

    case laIce: case laCocytus: case laBlizzard: case laEclectic:
      if(useHeatColoring(c)) {
        float h = HEAT(c);
        eLand l = c->land;
        
        color_t colorN04 = l == laCocytus ? 0x4080FF : 0x4040FF;
        color_t colorN10 = 0x0000FF;
        color_t color0 = floorcolors[c->land];
        color_t color02 = 0xFFFFFF;
        color_t color06 = 0xFF0000;
        color_t color08 = 0xFFFF00;
        
        if(h < -1)
          wcol = colorN10;
        else if(h < -0.4)
          wcol = gradient(colorN04, colorN10 , -0.4, h, -1);
        else if(h < 0)
          wcol = gradient(color0, colorN04, 0, h, -0.4);
        else if(h < 0.2)
          wcol = gradient(color0, color02, 0, h, 0.2);
        // else if(h < 0.4)
        //  wcol = gradient(0xFFFFFF, 0xFFFF00, 0.2, h, 0.4);
        else if(h < 0.6)
          wcol = gradient(color02, color06, 0.2, h, 0.6);
        else if(h < 0.8)
          wcol = gradient(color06, color08, 0.6, h, 0.8);
        else
          wcol = color08;
        if(c->wall == waFrozenLake) 
          fcol = wcol;
        else
          fcol = (wcol & 0xFEFEFE) >> 1;
        if(c->wall == waLake)
          fcol = wcol = (wcol & 0xFCFCFC) >> 2;
        }
      if(realred(c)) 
        eclectic_red(wcol);
      break;
    
    case laOcean:
      if(ls::any_chaos())
        fcol = gradient(0xD0A090, 0xD0D020, 0, c->CHAOSPARAM, 30);
      else
        fcol = gradient(0xD0D090, 0xD0D020, -1, sin((double) c->landparam), 1);
      break;
    
    case laSnakeNest: {
      int fv = pattern_threecolor(c);
      fcol = nestcolors[fv&7];
      if(realred(c->wall))
        wcol = fcol * (4 + snakelevel(c)) / 4;
      break;
      }
        
    default:
      if(isElemental(c->land)) fcol = linf[c->land].color;
      if(isWarpedType(c->land)) {
        fcol = warptype(c) ? 0x80C080 : 0xA06020;
        if(c->wall == waSmallTree) wcol = 0x608000;
        }  
      if(isHaunted(c->land)) {
        int itcolor = 0;
        for(int i=0; i<c->type; i++) if(c->move(i) && c->move(i)->item)
          itcolor = 1;
        if(c->item) itcolor |= 2;
        fcol = floorcolors[laHaunted] + 0x202020 * itcolor;
    
        forCellEx(c2, c) if(c2->monst == moFriendlyGhost)
          fcol = gradient(fcol, fghostcolor(c2), 0, .25, 1);
    
        if(c->monst == moFriendlyGhost) 
          fcol = gradient(fcol, fghostcolor(c), 0, .5, 1);
    
        if (!higher_contrast) {
          if(c->wall == waSmallTree) wcol = 0x004000;
          else if(c->wall == waBigTree) wcol = 0x008000;
          }
        }
    }
  
   /* if(c->land == laCaribbean && (c->wall == waCIsland || c->wall == waCIsland2))
     fcol = wcol = winf[c->wall].color; */
   
  switch(c->wall) {
    case waSulphur: case waSulphurC: case waPlatform: case waMercury: case waDock:
    case waAncientGrave: case waFreshGrave: case waThumperOn: case waThumperOff: case waBonfireOff:
    case waRoundTable: case waExplosiveBarrel:
      // floors become fcol
      fcol = wcol;
      break;
    
    case waCrateCrate: case waCrateTarget: case waCrateOnTarget:
      fcol = c->landparam;
      break;
    
    case waDeadTroll2: case waPetrifiedBridge: case waPetrified: {
      eMonster m = eMonster((unsigned char)c->wparam);
      if(c->wall == waPetrified || c->wall == waPetrifiedBridge) 
        wcol = gradient(wcol, minf[m].color, 0, .2, 1);
      if(c->wall == waPetrified || isTroll(m)) if(!(m == moForestTroll && c->land == laOvergrown))
        wcol = gradient(wcol, minf[m].color, 0, .4, 1);
      break;
      }
    
    case waFloorA: case waFloorB: // isAlch
      wcol = winf[conditional_flip_slime(det(V.T) < 0, c->wall)].color;

      if(c->item && !(history::includeHistory && history::infindhistory.count(c)))
        fcol = wcol = iinf[c->item].color;
      else
        fcol = wcol;
      break;
    
    case waBoat:
      if(wmascii) wcol = 0xC06000;
      break;
    
    case waEternalFire:
      fcol = wcol = weakfirecolor(1500);
      break;
    
    case waFire: case waPartialFire: case waBurningDock:
      fcol = wcol = firecolor(100);
      break;
    
    case waDeadfloor: case waCavefloor:
      fcol = wcol;
      break;
    
    case waNone:
      if(c->land == laNone)
        wcol = fcol = 0x101010;
      if(c->land == laHive) 
        wcol = fcol;  
      break;

    case waMineUnknown: case waMineMine: 
      #if CAP_COMPLEX2
      if(!mine_markers) fcol = wcol = apply_mine_knowledge(wcol, c);
      goto fallthrough;
      #endif

    case waMineOpen: fallthrough:
      if(wmblack || wmascii) {
        wcol &= 0xFEFEFE;
        wcol >>= 1;
        }
      fcol = wcol;
      break;
    
    case waCavewall:
      if(c->land != laEmerald) fcol = winf[waCavefloor].color;
      break;
   
    case waEditStatue:
      if(c->land == laCanvas) wcol = c->landparam;
      else wcol = (0x125628 * c->wparam) & 0xFFFFFF;
  
    default:
      break;    
    }

  /* if(false && isAlch2(c, true)) {
    int id = lavatide(c, -1);
    if(id < 96)
      wcol = gradient(0x800000, 0xFF0000, 0, id, 96);
    else 
      wcol = gradient(0x00FF00, 0xFFFF00, 96, id, 255);
    fcol = wcol;
    } */
  
  if(WDIM == 2) {
    int rd = rosedist(c);
    if(rd == 1) 
      wcol = gradient(0x804060, wcol, 0,1,3),
      fcol = gradient(0x804060, fcol, 0,1,3);
    if(rd == 2) 
      wcol = gradient(0x804060, wcol, 0,2,3),
      fcol = gradient(0x804060, fcol, 0,2,3);
    }
  
  if(items[itRevolver] && !shmup::on) {
    bool inrange = c->mpdist <= GUNRANGE;
    if(inrange) {
      inrange = false;
      for(cell *pc: player_positions())
        for(cell *c1: gun_targets(pc)) if(c1 == c) 
          inrange = true;
      }
    if(!inrange)
      fcol = gradient(fcol, 0, 0, 25, 100),
      wcol = gradient(wcol, 0, 0, 25, 100);
    }
    
  if(highwall(c) && !wmspatial)
    fcol = wcol;
  
  if(wmascii && (c->wall == waNone || isWatery(c))) wcol = fcol;
  
  if(!wmspatial && snakelevel(c) && !realred(c->wall)) fcol = wcol;
  
  if(c->wall == waGlass && !wmspatial) fcol = wcol;

  wcol_star = wcol;

  if(festive) {
    if(auto *at = at_or_null(old_shines, c)) {
      fcol = darkenedby(fcol, fd);
      wcol = darkenedby(wcol, fd);
      fd = 0;
      for(int p=0; p<3; p++) {
        part(fcol, p) = min(255, part(fcol, p) + (*at)[p] / 4);
        part(wcol, p) = min(255, part(wcol, p) + (*at)[p] / 8);
        }
      }
    }
  
  if(neon_mode == eNeon::illustration) {
    fcol = highwall(c) ? w_monochromatize(fcol, 0) : w_monochromatize(fcol, 1);
    wcol = w_monochromatize(wcol, 0);
    if(c->land == laWarpCoast && !pseudohept(c) && c->wall == waNone) fcol = 0x707070;
    }
  }

EX color_t w_monochromatize(color_t x, int d) {
  int c = part(x,2) + part(x,1) + part(x, 0) + 1;
  c /= 3;
  if(d != 0) c = (c + 2 * 255) / 3;
  return c * 0x10101;
  }


void celldrawer::tune_colors() {
  if(inmirror(c)) {
    // for debugging
    if(c->land == laMirrored) fcol = 0x008000;
    if(c->land == laMirrorWall2) fcol = 0x800000;
    if(c->land == laMirrored2) fcol = 0x000080;
    }
  
  for(int k=0; k<inmirrorcount; k++)
    wcol = gradient(wcol, 0xC0C0FF, 0, 0.2, 1),
    fcol = gradient(fcol, 0xC0C0FF, 0, 0.2, 1);

  aura_color = fcol;

  if(peace::on && peace::hint && (c->land != laTortoise || !tortoise::shading_on())) {
    int d =
      (c->land == laCamelot || (c->land == laCaribbean && celldistAlt(c) <= 0) || (c->land == laPalace && celldistAlt(c))) ? celldistAlt(c):
      celldist(c);

    int dc = 
      0x10101 * (127 + int(127 * sintick(200, d*spinspeed)));
    wcol = gradient(wcol, dc, 0, .3, 1);
    fcol = gradient(fcol, dc, 0, .3, 1);
    }
  }

int celldrawer::getSnakelevColor(int i, int last) {
  color_t col;
  if(c->wall == waTower) 
    col = 0xD0D0D0-i*0x101010;
  else if(c->land == laSnakeNest)
    return darkena(nestcolors[pattern_threecolor(c)] * (5 + i) / 4, 0, 0xFF);
  #if CAP_COMPLEX2
  else if(c->land == laBrownian)
    col = brownian::get_color(c->landparam % brownian::level + (i+1) * brownian::level);
  #endif
  else if(i == last-1)
    col = wcol;
  else {
    col = winf[waRed1+i].color;
    if(c->land == laEclectic)
      eclectic_red(col);
    }
  if (!higher_contrast)
    return darkena(col, fd, 0xFF);
  else
    return darkena(col, 0, 0xFF);
  }

void celldrawer::draw_wallshadow() {
  if(!noshadow && qfi.fshape) {
    dynamicval<color_t> p(poly_outline, OUTLINE_TRANS);
    draw_shapevec(c, V, qfi.fshape->shadow, SHADOW_WALL, GDIM == 3 ? PPR::TRANSPARENT_SHADOW : PPR::WALLSHADOW);
    }
  }

void celldrawer::draw_wall() {

  if(no_wall_rendering) return;

  if(GDIM == 3 && WDIM == 2) {
    if(!qfi.fshape) qfi.fshape = &cgi.shFullFloor;
    if(conegraph(c)) {
      draw_shapevec(c, V, qfi.fshape->cone[0], darkena(wcol_star, 0, 0xFF), PPR::WALL);
      draw_wallshadow();
      return;
      }
    if(c->wall == waClosedGate) {
      int hdir = 0;
      for(int i=0; i<c->type; i++) if(c->move(i)->wall == waClosedGate)
        hdir = i;
      queuepolyat(V * ddspin180(c, hdir), cgi.shPalaceGate, darkena(wcol, 0, 0xFF), wmspatial?PPR::WALL_DECO:PPR::WALL);
      return;
      }
    draw_wallshadow();
    return;
    }

  aura_color = wcol;
  color_t wcol0 = wcol;
  int starcol = wcol_star;
  if(c->wall == waWarpGate) starcol = 0;
  if(c->wall == waVinePlant) starcol = 0x60C000;

  if(c->wall == waClosedGate) {
    int hdir = 0;
    for(int i=0; i<c->type; i++) if(c->move(i) && c->move(i)->wall == waClosedGate)
      hdir = i;
    shiftmatrix V2 = orthogonal_move_fol(V, wmspatial?cgi.WALL:1) * ddspin180(c, hdir); // to test
    queuepolyat(V2, cgi.shPalaceGate, darkena(wcol, 0, 0xFF), wmspatial?PPR::WALL_DECO:PPR::WALL);
    starcol = 0;
    }
  
  hpcshape& shThisWall = isGrave(c->wall) ? cgi.shCross : cgi.shWall[ct6];

  shiftmatrix V1 = V;
  if(&shThisWall == &cgi.shCross) {
    auto si = patterns::getpatterninfo(c, patterns::PAT_ZEBRA, patterns::SPF_SYM0123);
    V1 = V * applyPatterndir(c, si);
    }
  
  if(conegraph(c)) {   
    const int layers = 2 << detaillevel;
    for(int z=1; z<layers; z++) {
      double zg = zgrad0(0, geom3::actual_wall_height(), z, layers);
      draw_qfi(c, xyzscale(V, zg*(layers-z)/layers, zg),
        darkena(gradient(0, wcol_star, -layers, z, layers), 0, 0xFF), PPR::WALL_DECO);
      }
    floorShadow(c, V, SHADOW_WALL);
    }
  
  else if(true) {
    if(!wmspatial) {
      if(starcol) queuepoly(V1, shThisWall, darkena(starcol, 0, 0xFF));
      }
    else {
      shiftmatrix Vdepth = orthogonal_move_fol(V1, cgi.WALL);
      int alpha = 0xFF;
      if(c->wall == waIcewall)
        alpha = 0xC0;

      if(starcol && !(wmescher && c->wall == waPlatform)) 
        queuepolyat(Vdepth, shThisWall, darkena(starcol, 0, 0xFF), PPR::WALL_DECO);

      draw_qfi(c, Vdepth, darkena(wcol0, fd, alpha), PPR::WALL_TOP);
      floorShadow(c, V, SHADOW_WALL);
      }
    }
  }

void celldrawer::draw_boat() {
  double footphase;
  if(WDIM == 3 && c == cwt.at && hide_player()) return;
  bool magical = items[itOrbWater] && (isPlayerOn(c) || (isFriendly(c) && items[itOrbEmpathy]));
  int outcol = magical ? watercolor(0) : 0xC06000FF;
  int incol = magical ? 0x0060C0FF : 0x804000FF;
  bool nospin = false;

  if(WDIM == 3) {
    Vboat = V;
    nospin = c->wall == waBoat && applyAnimation(c, Vboat, footphase, LAYER_BOAT);
    if(!nospin) Vboat = face_the_player(V);
    else Vboat = Vboat * cspin180(0, 2);
    queuepolyat(orthogonal_move(Vboat, cgi.scalefactor/2), cgi.shBoatOuter, outcol, PPR::BOATLEV2);
    queuepolyat(orthogonal_move(Vboat, cgi.scalefactor/2-0.01), cgi.shBoatInner, incol, PPR::BOATLEV2);
    return;
    }

  if(wmspatial && c->wall == waBoat) {
    nospin = c->wall == waBoat && applyAnimation(c, Vboat, footphase, LAYER_BOAT);
    if(!nospin && c->mondir != NODIR) Vboat = Vboat * ddspin180(c, c->mondir);
    queuepolyat(Vboat, cgi.shBoatOuter, outcol, PPR::BOATLEV);
    Vboat = V;
    }
  if(c->wall == waBoat) {
    nospin = applyAnimation(c, Vboat, footphase, LAYER_BOAT);
    }
  if(!nospin && c->mondir != NODIR) 
    Vboat = Vboat * ddspin180(c, c->mondir);
  else {
    shiftmatrix Vx;
    if(applyAnimation(c, Vx, footphase, LAYER_SMALL))
      animations[LAYER_SMALL][c].footphase = 0;
    }
  if(wmspatial && GDIM == 2)
    queuepolyat(orthogonal_move_fol(Vboat, (cgi.WATERLEVEL+cgi.FLOOR)/2), cgi.shBoatOuter, outcol, PPR::BOATLEV2);
  if(GDIM == 3) {
    queuepoly(orthogonal_move(Vboat, -0.004), cgi.shBoatOuter, outcol);
    queuepoly(orthogonal_move(Vboat, -0.008), cgi.shBoatInner, incol);
    }
  else {
    queuepoly(Vboat, cgi.shBoatOuter, outcol);
    queuepoly(Vboat, cgi.shBoatInner, incol);
    }
  }

EX int grid_prec() {
  int prec = sphere ? 3 : 1;
  prec += vid.linequality;
  return prec;
  }

// should we draw t-th edge of c, or the opposite edge?
EX bool pick_for_grid(cell *c, int t) {
  cell *c1 = c->move(t);
  if(!c1) return false;
  // removed: if(WDIM == 3 && bt::in() && !sn::in()) return !among(t, 5, 6, 8);
  return c < c1 || isWarped(c->move(t)) || fake::split();
  }

void celldrawer::draw_grid() {

  int prec = grid_prec();
  
  if(inmirrorcount) return;
  
  if(vid.grid || (c->land == laAsteroids && !(WDIM == 2 && GDIM == 3))) ; else return;        

  dynamicval<ld> lw(vid.linewidth, vid.linewidth);

  vid.linewidth *= vid.multiplier_grid;
  vid.linewidth *= cgi.scalefactor;

  int maxt = c->type;
  if(arb::apeirogon_hide_grid_edges && arb::is_apeirogonal(c)) maxt -= 2;

  if(isWarped(c) && has_nice_dual()) {
    if(pseudohept(c)) for(int t=0; t<c->type; t++) if(isWarped(c->move(t)))
      gridline(V, get_warp_corner(c, t%c->type), get_warp_corner(c, (t+1)%c->type), gridcolor(c, c->move(t)), prec);
    return;
    }
  
  for(int t=0; t<maxt; t++)
    if(pick_for_grid(c, t))
      draw_grid_edge(t, gridcolor(c, c->move(t)), prec);
  }

void celldrawer::draw_grid_edge(int t, color_t col, int prec) {

  #if MAXMDIM == 4
  if(WDIM == 3) {
    int ofs = currentmap->wall_offset(c);

    // if(bt::in() && !sn::in() && !among(t, 5, 6, 8)) continue;
    // if(!bt::in() && c->move(t) < c) continue;

    dynamicval<color_t> g(poly_outline, col);
    bool use_fat = fat_edges && (reg3::in() || euc::in(3));
    if(!use_fat) {
      queuepoly(V, cgi.shWireframe3D[ofs + t], 0);
      return;
      }

    auto& ss = currentmap->get_cellshape(c);
    auto& fa = ss.faces[t];
    int face = isize(fa);
    for(int j=0; j<face; j++) {
      int jj = j == face-1 ? 0 : j+1;
      int jjj = jj == face-1 ? 0 : jj+1;
      hyperpoint a = fa[j];
      hyperpoint b = fa[jj];
      if(cgflags & qIDEAL) {
        ld mm = cgi.ultra_mirror_part;
        if((cgflags & qULTRA) && !reg3::ultra_mirror_in())
          mm = lerp(1-cgi.ultra_material_part, cgi.ultra_material_part, .99);
        tie(a, b) = make_pair(normalize(lerp(a, b, mm)), normalize(lerp(b, a, mm)));
        }
      else {
        a = normalize(a);
        b = normalize(b);
        }
      gridline(V, a, b, col, prec);

      if(reg3::ultra_mirror_in()) {
        hyperpoint a = fa[j];
        hyperpoint b = fa[jj];
        hyperpoint d = fa[jjj];
        auto& mm = cgi.ultra_mirror_part;
        tie(a, d) = make_pair(normalize(lerp(a, b, mm)), normalize(lerp(d, b, mm)));
        gridline(V, a, d, stdgridcolor, prec);
        }
      }
    return;
    }
  #endif
  #if CAP_BT
  if(bt::in() && WDIM == 2) {
    auto h0 = bt::get_corner_horo_coordinates(c, t);
    auto h1 = bt::get_corner_horo_coordinates(c, t+1);
    int steps = 12 * abs(h0[1] - h1[1]);
    if(!steps) {
      gridline(V, bt::get_horopoint(h0), bt::get_horopoint(h1), col, prec);
      }
    else {
      if(vid.linequality > 0) steps <<= vid.linequality;
      if(vid.linequality < 0) steps >>= -vid.linequality;
      auto step = (h1 - h0) / steps;
      if(GDIM == 3) {
        for(int i=0; i<=steps; i++) gridline(V, bt::get_horopoint(h0 + i * step), V, bt::get_horopoint(h0 + (i+1) * step), gridcolor(c, c->move(t)), prec-2);
        }
      else {
        for(int i=0; i<=steps; i++) curvepoint(bt::get_horopoint(h0 + i * step));
        queuecurve(V, col, 0, PPR::LINE);
        }
      }
     return;
    }
  #endif
  gridline(V, get_corner_position(c, t%c->type), get_corner_position(c, (t+1)%c->type), col, prec);
  }

void celldrawer::draw_halfvine() {

  int i =-1;
  for(int t=0;t<6; t++) if(c->move(t) && c->move(t)->wall == c->wall)
    i = t;

  qfi.spin = ddspin(c, i, M_PI/S3);
  shiftmatrix V2 = V * qfi.spin;
  
  if(wmspatial && wmescher && GDIM == 2) {
    set_floor(cgi.shSemiFeatherFloor[0]);
    int dk = 1;
    int vcol = winf[waVinePlant].color;
    draw_qfi(c, orthogonal_move_fol(V2, cgi.WALL), darkena(vcol, dk, 0xFF), PPR::WALL_DECO);
    escherSidewall(c, SIDE::WALL, V2, darkena(gradient(0, vcol, 0, .8, 1), dk, 0xFF));
    queuepoly(V2, cgi.shSemiFeatherFloor[1], darkena(fcol, dk, 0xFF));
    set_floor(cgi.shFeatherFloor);
    }
  
  else if(wmspatial || GDIM == 3) {
    floorshape& shar = *(GDIM == 3 ? (floorshape*)&cgi.shFullFloor : wmplain ? (floorshape*)&cgi.shFloor : (floorshape*)&cgi.shFeatherFloor);
    
    set_floor(shar);

    int vcol = winf[waVinePlant].color;
    int vcol2 = gradient(0, vcol, 0, .8, 1);
    
    shiftmatrix Vdepth = orthogonal_move_fol(V2, cgi.WALL);

    queuepolyat(GDIM == 2 ? Vdepth : V2, cgi.shSemiFloor[0], darkena(vcol, fd, 0xFF), PPR::WALL_DECO);
    if(!noshadow) {
      dynamicval<color_t> p(poly_outline, OUTLINE_TRANS);
      queuepolyat(V2 * spin(120._deg), cgi.shSemiFloorShadow, SHADOW_WALL, GDIM == 2 ? PPR::WALLSHADOW : PPR::TRANSPARENT_SHADOW);
      }
    if(qfi.fshape) {
      auto& side = queuepolyat(V2, cgi.shSemiFloorSide[SIDE::WALL], darkena(vcol2, fd, 0xFF), PPR::WALL_SIDE);
      if(GDIM == 3) {
        side.tinf = &floor_texture_vertices[shar.id];
        ensure_vertex_number(*side.tinf, side.cnt);
        }
      }

    if(cgi.validsidepar[SIDE::WALL]) forCellIdEx(c2, j, c) {
      int dis = gmod(i-j, 6);
      if(dis != 1 && dis != 5) continue;
      if(placeSidewall(c, j, SIDE::WALL, V, darkena(vcol2, fd, 0xFF))) break;
      }
    }
  
  else {
    hpcshape *shar = cgi.shSemiFeatherFloor;
    
    if(wmblack) shar = cgi.shSemiBFloor;
    if(wmplain) shar = cgi.shSemiFloor;
    
    queuepoly(V2, shar[0], darkena(winf[waVinePlant].color, fd, 0xFF));
    
    set_floor(qfi.spin, shar[1]);
    }
  }

void celldrawer::draw_mirrorwall() {
  int d = mirror::mirrordir(c);
  bool onleft = c->type == 7;
  if(c->type == 7 && c->barleft == laMirror)
    onleft = !onleft;
  if(c->type == 6 && d != -1 && c->move(d)->barleft == laMirror)
    onleft = !onleft;
  if(PURE) onleft = !onleft;
  
  if(d == -1) {
    for(d=0; d<c->type; d++)
      if(c->move(d) && c->modmove(d+1) && c->move(d)->land == laMirrorWall && c->modmove(d+1)->land == laMirrorWall)
        break;
    qfi.spin = ddspin(c, d, 0);
    shiftmatrix V2 = V * qfi.spin;
    if(!wmblack) for(int d=0; d<c->type; d++) {
      inmirrorcount+=d;
      queuepolyat(V2 * spin(d*M_PI/S3), cgi.shHalfFloor[2], darkena(fcol, fd, 0xFF), PPR::FLOORa);
      #if MAXMDIM >= 4
      if(GDIM == 3 && camera_level > cgi.WALL && pmodel == mdPerspective)
        queuepolyat(V2 * spin(d*M_PI/S3), cgi.shHalfFloor[5], darkena(fcol, fd, 0xFF), PPR::FLOORa);
      #endif
      inmirrorcount-=d;
      }          
    if(GDIM == 3) {
      for(int d=0; d<6; d++)
        queue_transparent_wall(V2 * spin(d*M_PI/S3), cgi.shHalfMirror[2], 0xC0C0C080);
      }
    else if(wmspatial) {
      const int layers = 2 << detaillevel;
      for(int z=1; z<layers; z++) 
        queuepolyat(orthogonal_move_fol(V2, zgrad0(0, geom3::actual_wall_height(), z, layers)), cgi.shHalfMirror[2], 0xC0C0C080, PPR::WALL_SIDE);
      }
    else
      queuepolyat(V2, cgi.shHalfMirror[2], 0xC0C0C080, PPR::WALL_TOP);
    }
  else {
    qfi.spin = ddspin180(c, d);
    shiftmatrix V2 = V * qfi.spin;
    if(!wmblack) {
      inmirrorcount++;
      queuepolyat(mirrorif(V2, !onleft), cgi.shHalfFloor[ct6], darkena(fcol, fd, 0xFF), PPR::FLOORa);
      #if MAXMDIM >= 4
      if(GDIM == 3 && camera_level > cgi.WALL && pmodel == mdPerspective)
        queuepolyat(mirrorif(V2, !onleft), cgi.shHalfFloor[ct6+3], darkena(fcol, fd, 0xFF), PPR::FLOORa);
      #endif
      inmirrorcount--;
      queuepolyat(mirrorif(V2, onleft), cgi.shHalfFloor[ct6], darkena(fcol, fd, 0xFF), PPR::FLOORa);
      #if MAXMDIM >= 4
      if(GDIM == 3 && camera_level > cgi.WALL && pmodel == mdPerspective)
        queuepolyat(mirrorif(V2, onleft), cgi.shHalfFloor[ct6+3], darkena(fcol, fd, 0xFF), PPR::FLOORa);
      #endif
      }

    if(GDIM == 3) 
      queue_transparent_wall(V2, cgi.shHalfMirror[ct6], 0xC0C0C080);
    else if(wmspatial) {
      const int layers = 2 << detaillevel;
      for(int z=1; z<layers; z++) 
        queuepolyat(orthogonal_move_fol(V2, zgrad0(0, geom3::actual_wall_height(), z, layers)), cgi.shHalfMirror[ct6], 0xC0C0C080, PPR::WALL_DECO);
      }
    else 
      queuepolyat(V2, cgi.shHalfMirror[ct6], 0xC0C0C080, PPR::WALL_TOP);
    }
  }

EX int canvasfloor;

void celldrawer::set_land_floor(const shiftmatrix& Vf) {
  switch(c->land) {
    case laCanvas:
      switch(canvasfloor) {
        case caflM: set_floor(cgi.shMFloor); break;
        case caflFull: set_floor(cgi.shFullFloor); break;
//      case caflWarp is warped floor
        case caflStar: set_floor(cgi.shStarFloor); break;
        case caflCloud: set_floor(cgi.shCloudFloor); break;
        case caflCross: set_floor(cgi.shCrossFloor); break;
        case caflCharged: set_floor(cgi.shChargedFloor); break;
        case caflSStar: set_floor(cgi.shSStarFloor); break;
        case caflOver: set_floor(cgi.shOverFloor); break;
        case caflTri: set_floor(cgi.shTriFloor); break;
        case caflFeather: set_floor(cgi.shFeatherFloor); break;
        case caflBarrow: set_floor(cgi.shBarrowFloor); break;
        case caflNew: set_floor(cgi.shNewFloor); break;
        case caflTroll: set_floor(cgi.shTrollFloor); break;
        case caflButterfly: set_floor(cgi.shButterflyFloor); break;
        case caflLava: set_floor(cgi.shLavaFloor); break;
        case caflPalace: set_floor(cgi.shPalaceFloor); break;
        case caflDemon: set_floor(cgi.shDemonFloor); break;
        case caflCave: set_floor(cgi.shCaveFloor); break;
        case caflDesert: set_floor(cgi.shDesertFloor); break;
        case caflPower: set_floor(cgi.shPowerFloor); break;
        case caflRose: set_floor(cgi.shRoseFloor); break;
        case caflTurtle: set_floor(cgi.shTurtleFloor); break;
        case caflDragon: set_floor(cgi.shDragonFloor); break;
        case caflReptile: set_reptile_floor(V, fcol); break;
        case caflHive:
          if(c->wall != waFloorB && c->wall != waFloorA && c->wall != waMirror && c->wall != waCloud && sha.top == SIDE::FLOOR) {
            set_floor(cgi.shFloor);
            if(GDIM == 2) {
              draw_floorshape(c, V, cgi.shMFloor, darkena(fcol, fd + 1, 0xFF), PPR::FLOORa);
              draw_floorshape(c, V, cgi.shMFloor2, darkena(fcol, fcol==color_t(c->landparam) ? fd : fd + 1, 0xFF), PPR::FLOORb);
              }
            }
          else
            set_floor(cgi.shFloor);
          break;
        case caflSwitch:
          set_floor(cgi.shSwitchFloor);
          if(sha.top == SIDE::FLOOR && ctof(c) && STDVAR && !arcm::in() && !bt::in() && GDIM == 2) for(int i=0; i<c->type; i++)
            queuepoly(Vf * ddspin(c, i, M_PI/S7) * xpush(cgi.rhexf), cgi.shSwitchDisk, darkena(fcol, fd, 0xFF));
          break;
        case caflTower: set_towerfloor(celldist); break;
        case caflNone: default:
          set_floor(cgi.shFloor); break;
        }
      break;

    case laPrairie:
    case laAlchemist:
      set_floor(cgi.shCloudFloor);
      break;
    
    case laJungle:
    case laWineyard:
      set_floor(cgi.shFeatherFloor);
      break;

    case laFrog:
      if(c->wall == waDeepWater)
        set_floor(cgi.shFloor);
      else 
        set_floor(cgi.shFeatherFloor);
      break;
    
    case laZebra:
      set_zebrafloor();
      break;
    
    case laMountain:
      if(shmup::on || GDIM == 3)
        shmup_gravity_floor();
      else 
        set_towerfloor(euclid ? celldist : c->master->alt ? celldistAltPlus : celldist);
      break;
    
    case laEmerald:
      set_emeraldfloor();
      break;
    
    case laRlyeh:
    case laTemple:
      set_floor(cgi.shTriFloor);
      break;
    
    case laVolcano:
    case laVariant:
    case laEclectic:
      set_floor(cgi.shLavaFloor);
      break;
    
    case laRose:
      set_floor(cgi.shRoseFloor);
      break;
    
    case laTortoise:
      set_floor(cgi.shTurtleFloor);
      break;
    
    case laBurial: case laRuins:
      set_floor(cgi.shBarrowFloor);
      break;

    case laTrollheim:
      set_floor(cgi.shTrollFloor);
      break;

    /*case laMountain:
      set_floor(FEATHERFLOOR);
      break; */
    
    case laGraveyard:
      set_floor(cgi.shCrossFloor);
      break;
    
    case laMotion:
      set_floor(cgi.shMFloor);
      break;
    
    case laWhirlwind:
    case laEFire: case laEAir: case laEWater: case laEEarth: case laElementalWall:
      set_floor(cgi.shNewFloor);
      break;
    
    case laHell:
      set_floor(cgi.shDemonFloor);
      break;
          
    case laIce: case laBlizzard:
      set_floor(cgi.shStarFloor);
      break;
    
    case laSwitch:
      set_floor(cgi.shSwitchFloor);
      if(ctof(c) && STDVAR && !arcm::in() && !bt::in() && GDIM == 2) for(int i=0; i<c->type; i++)
        queuepoly(Vf * ddspin(c, i, M_PI/S7) * xpush(cgi.rhexf), cgi.shSwitchDisk, darkena(minf[active_switch()].color, fd, 0xFF));
      break;

    case laStorms:
      set_floor(cgi.shChargedFloor);
      break;
    
    case laWildWest:
      set_floor(cgi.shSStarFloor);
      break;
    
    case laPower:
      set_floor(cgi.shPowerFloor);
      break;
    
    case laCaves:
    case laLivefjord:
    case laDeadCaves:
    case laCursed:
      set_floor(cgi.shCaveFloor);
      break;
    
    case laDryForest:
      set_floor(GDIM == 3 ? cgi.shFeatherFloor : cgi.shDesertFloor);
      break;

    case laDesert:
    case laRedRock: case laSnakeNest:
    case laCocytus:
      set_floor(cgi.shDesertFloor);
      break;

    case laBull:
      set_floor(cgi.shButterflyFloor);
      break;
    
    case laCaribbean: case laOcean: case laOceanWall: case laWhirlpool:
      set_floor(cgi.shCloudFloor);
      break;
    
    case laKraken:
    case laDocks:
      set_floor(cgi.shFullFloor);
      break;
    
    case laPalace: case laTerracotta:
      set_floor(cgi.shPalaceFloor);
      break;
    
    case laDragon:
      set_floor(cgi.shDragonFloor);
      break;
    
    case laOvergrown: case laClearing: case laHauntedWall: case laHaunted: case laHauntedBorder:
      set_floor(cgi.shOverFloor);
      break;

    case laMercuryRiver: {
      if(euclid || !BITRUNCATED || GDIM == 3)
        set_floor(cgi.shFloor);
      else {
        int bridgedir = -1;
        if(c->type == 6) {
          for(int i=1; i<c->type; i+=2)
            if(pseudohept(c->modmove(i-1)) && c->modmove(i-1)->land == laMercuryRiver)
            if(pseudohept(c->modmove(i+1)) && c->modmove(i+1)->land == laMercuryRiver)
              bridgedir = i;
          }
        if(bridgedir == -1)
          set_floor(cgi.shPalaceFloor);
        else {
          transmatrix bspin = ddspin(c, bridgedir);
          set_floor(bspin, cgi.shMercuryBridge[0]);
          // only needed in one direction
          if(c < c->move(bridgedir)) {
            shiftmatrix Vbspin = Vf * bspin;
            queuepoly(Vbspin, cgi.shMercuryBridge[1], darkena(fcol, fd+1, 0xFF));
            if(wmspatial) {
              queuepolyat(orthogonal_move_fol(Vbspin, cgi.WATERLEVEL), cgi.shMercuryBridge[1], darkena(gradient(0, winf[waMercury].color, 0, 0.8,1), 0, 0x80), PPR::WATERLEVEL_TOP);
              queuepolyat(orthogonal_move_fol(Vbspin, cgi.DEEP), cgi.shMercuryBridge[1], darkena(0x202020, 0, 0xFF), PPR::DEEP_TOP);
              }
            }
          }
        }
      break;
      }
    
    case laHive:
      if(c->wall != waFloorB && c->wall != waFloorA && c->wall != waMirror && c->wall != waCloud) {
        fd = 1;
        set_floor(cgi.shFloor);
        if(c->wall != waMirror && c->wall != waCloud && GDIM == 2)
          draw_floorshape(c, V, cgi.shMFloor, darkena(fcol, 2, 0xFF), PPR::FLOORa);
        if(c->wall != waMirror && c->wall != waCloud && GDIM == 2)
          draw_floorshape(c, V, cgi.shMFloor2, darkena(fcol, fcol==wcol ? 1 : 2, 0xFF), PPR::FLOORb);
        }
      else
        set_floor(cgi.shFloor);
      break;
    
    case laEndorian:
      if(shmup::on || GDIM == 3)
        shmup_gravity_floor();

      else if(c->wall == waTrunk) 
        set_floor(cgi.shFloor);

      else if(c->wall == waCanopy || c->wall == waSolidBranch || c->wall == waWeakBranch) 
        set_floor(cgi.shFeatherFloor);
      
      else 
        set_towerfloor();
      break;
    
    case laIvoryTower: case laDungeon: case laWestWall:
      if(shmup::on || GDIM == 3)
        shmup_gravity_floor();
      else
        set_towerfloor(ls::hv_structure() ? celldistAltPlus : coastvalEdge);
      break;
    
    case laBrownian:
      if(among(c->wall, waSea, waBoat))
        set_floor(cgi.shCloudFloor);
      else
        set_floor(cgi.shTrollFloor);
      break;

    default: 
      set_floor(cgi.shFloor);
    }
  }

bool celldrawer::set_randompattern_floor() {
  if(isWarpedType(c->land)) return false;
  if(!randomPatternsMode) return false;
  if(c->land == laBarrier) return false;
  int j = (randompattern[c->land]/5) % 15;
  int k = randompattern[c->land] % RPV_MODULO;
  int k7 = randompattern[c->land] % 7;
  
  if(k == RPV_ZEBRA && k7 < 2) set_zebrafloor();
  else if(k == RPV_EMERALD && k7 == 0) set_emeraldfloor();
  else if(k == RPV_CYCLE && k7 < 4) set_towerfloor(celldist);

  else switch(j) {
    case 0:  set_floor(cgi.shCloudFloor); break;
    case 1:  set_floor(cgi.shFeatherFloor); break;
    case 2:  set_floor(cgi.shStarFloor); break;
    case 3:  set_floor(cgi.shTriFloor); break;
    case 4:  set_floor(cgi.shSStarFloor); break;
    case 5:  set_floor(cgi.shOverFloor); break;
    case 6:  set_floor(cgi.shFeatherFloor); break;
    case 7:  set_floor(cgi.shDemonFloor); break;
    case 8:  set_floor(cgi.shCrossFloor); break;
    case 9:  set_floor(cgi.shMFloor); break;
    case 10: set_floor(cgi.shCaveFloor); break;
    case 11: set_floor(cgi.shPowerFloor); break;
    case 12: set_floor(cgi.shDesertFloor); break;
    case 13: set_floor(cgi.shChargedFloor); break;
    case 14: set_floor(cgi.shLavaFloor); break;
    }
  return true;
  }

EX bool numerical_minefield;
EX int mine_zero_display = 1;
EX bool mine_hollow;
EX bool mine_markers;

EX void draw_mine_numbers(int mines, const shiftmatrix& V, int ct6) {
  auto hollow = [&] (hpcshape& sh, color_t col) {
    if(mine_hollow) {
      dynamicval<color_t> dc(poly_outline, col);
      queuepoly(V, sh, 0);
      }
    else
      queuepoly(V, sh, col);
    };
  if(mines == 0 && mine_zero_display < (WDIM == 3 ? 1 : 2)) return;
  if(numerical_minefield) {
    string label = its(mines);
    queuestr(V, (mines >= 10 ? .5 : 1) * mapfontscale / 100, label, darkened(minecolors[mines]), 8);
    }
  else {
    if(mines >= isize(minecolors)) hollow(cgi.shBigMineMark[ct6], darkena(minecolors[mines/isize(minecolors)], 0, 0xFF));
    hollow(cgi.shMineMark[ct6], darkena(minecolors[mines], 0, 0xFF));
    }
  }

#if CAP_COMPLEX2
EX void draw_mine_markers(cell *c, const shiftmatrix& V) {
  if(mine_markers && !mine::marked_safe(c)) {
    color_t col = 0xFF4040;
    if(mine::marked_mine(c))
      col = gradient(winf[waMineMine].color, col, -1, vid.ispeed ? sintick(100) : 1, 1);
    dynamicval<color_t> dc(poly_outline, (col << 8) | 0xFF);
    queuepoly(V, cgi.shJoint, 0);
    }
  }
#endif

void celldrawer::draw_features() {
  char xch = winf[c->wall].glyph;
  #if CAP_SHAPES
  switch(c->wall) {
  
    case waBigBush:
      if(detaillevel >= 2)
        queuepolyat(at_smart_lof(V, zgrad0(0, cgi.slev, 1, 2)), cgi.shHeptaMarker, darkena(wcol, 0, 0xFF), PPR::RED1_TOP);
      if(detaillevel >= 1)
        queuepolyat(at_smart_lof(V, cgi.RED[1]) * lpispin(), cgi.shWeakBranch, darkena(wcol, 0, 0xFF), PPR::RED1_TOP+1);
      if(detaillevel >= 2)
        queuepolyat(at_smart_lof(V, zgrad0(0, cgi.slev, 3, 2)), cgi.shHeptaMarker, darkena(wcol, 0, 0xFF), PPR::RED1_TOP+2);
      queuepolyat(at_smart_lof(V, cgi.RED[2]), cgi.shSolidBranch, darkena(wcol, 0, 0xFF), PPR::RED1_TOP+3);
      break;
    
    case waSmallBush:
      if(detaillevel >= 2)
        queuepolyat(at_smart_lof(V, zgrad0(0, cgi.slev, 1, 2)), cgi.shHeptaMarker, darkena(wcol, 0, 0xFF), PPR::RED1_TOP);
      if(detaillevel >= 1)
        queuepolyat(at_smart_lof(V, cgi.RED[1]) * lpispin(), cgi.shWeakBranch, darkena(wcol, 0, 0xFF), PPR::RED1_TOP+1);
      if(detaillevel >= 2)
        queuepolyat(at_smart_lof(V, zgrad0(0, cgi.slev, 3, 2)), cgi.shHeptaMarker, darkena(wcol, 0, 0xFF), PPR::RED1_TOP+2);
      queuepolyat(at_smart_lof(V, cgi.RED[2]), cgi.shWeakBranch, darkena(wcol, 0, 0xFF), PPR::RED1_TOP+3);
      break;
  
    case waSolidBranch:
      queuepoly(GDIM == 3 ? orthogonal_move_fol(V, cgi.BODY) : V, cgi.shSolidBranch, darkena(wcol, 0, 0xFF));
      break;
    
    case waWeakBranch:
      queuepoly(GDIM == 3 ? orthogonal_move_fol(V, cgi.BODY) : V, cgi.shWeakBranch, darkena(wcol, 0, 0xFF));
      break;
  
    case waLadder:
      if(GDIM == 3) {
        #if MAXMDIM >= 4
        draw_shapevec(c, V * lzpush(-cgi.human_height/20), cgi.shMFloor.levels[SIDE::FLOOR], 0x804000FF, PPR::FLOOR+1);
        #endif
        }
      else if(euclid) {
        draw_floorshape(c, V, cgi.shMFloor, 0x804000FF);
        draw_floorshape(c, V, cgi.shMFloor2, 0x000000FF);
        }
      else {
        draw_floorshape(c, V, cgi.shFloor, 0x804000FF, PPR::FLOOR+1);
        draw_floorshape(c, V, cgi.shMFloor, 0x000000FF, PPR::FLOOR+2);
        }
      break;
    
    case waReptileBridge: {
      Vboat = V;
      dynamicval<qfloorinfo> qfi2(qfi, qfi);
      color_t col = reptilecolor(c);
      set_reptile_floor(V, col);
      draw_qfi(c, V, col);
      break;
      }
    
    case waTerraWarrior:
      #if CAP_COMPLEX2
      drawTerraWarrior(V, terracotta::randterra ? (c->wparam & 7) : (5 - (c->wparam & 7)), 7, 0);
      #endif
      break;
    
    case waBoat: case waStrandedBoat: 
      draw_boat();
      break;
    
    case waBigStatue: {
      shiftmatrix V2 = V;
      double footphase;
      applyAnimation(c, V2, footphase, LAYER_BOAT);
      
      queuepolyat(V2, cgi.shStatue, 
        darkena(winf[c->wall].color, 0, 0xFF),
        PPR::BIGSTATUE
        );
      break;
      }
    
    case waSulphurC: {
      if(drawstar(c)) {
        aura_color = wcol;
        if(wmspatial) 
          queuepolyat(orthogonal_move_fol(V, cgi.HELLSPIKE), cgi.shGiantStar[ct6], darkena(wcol, 0, 0x40), PPR::HELLSPIKE);
        else
          queuepoly(V, cgi.shGiantStar[ct6], darkena(wcol, 0, 0xFF));
        }
      break;
      }
    
    case waTrapdoor:
      if(c->land == laZebra) break;
      /* fallthrough */
    
    case waClosePlate: case waOpenPlate: {
      shiftmatrix V2 = V;
      if(wmescher && geosupport_football() == 2 && pseudohept(c) && c->land == laPalace) V2 = V * spin(M_PI / c->type);
      if(GDIM == 3) {
        #if MAXMDIM >= 4
        draw_shapevec(c, V2 * lzpush(-cgi.human_height/40), cgi.shMFloor.levels[SIDE::FLOOR], darkena(winf[c->wall].color, 0, 0xFF));
        draw_shapevec(c, V2 * lzpush(-cgi.human_height/35), cgi.shMFloor2.levels[SIDE::FLOOR], (!wmblack) ? darkena(fcol, 1, 0xFF) : darkena(0,1,0xFF));
        #endif
        }
      else {
        draw_floorshape(c, V2, cgi.shMFloor, darkena(winf[c->wall].color, 0, 0xFF));
        draw_floorshape(c, V2, cgi.shMFloor2, (!wmblack) ? darkena(fcol, 1, 0xFF) : darkena(0,1,0xFF));
        }
      break;
      }
    
    case waCrateCrate: case waCrateTarget: case waCrateOnTarget: {
      shiftmatrix V2 = V;
      if(c->wall != waCrateCrate) {
        draw_floorshape(c, V2, cgi.shMFloor, darkena(winf[waCrateTarget].color, 0, 0xFF));
        draw_floorshape(c, V2, cgi.shMFloor2, c->wall == waCrateOnTarget ? darkena(0xFFFF00, 0, 0xFF) : (!wmblack) ? darkena(fcol, 1, 0xFF) : darkena(0,1,0xFF));
        }
      if(c->wall != waCrateTarget) {
        if(wmspatial) {
          const int layers = 2 << detaillevel;
          for(int z=1; z<=layers; z++) {
            double zg = zgrad0(0, geom3::actual_wall_height(), z, layers);
            queuepolyat(xyzscale(V, zg, zg), cgi.shBarrel, darkena((z&1) ? 0xFF0000 : 0xC00000, 0, 0xFF), PPR(PPR::WALL_DECO));
            }
          }
        else {
          queuepolyat(V, cgi.shBarrel, darkena(0xC00000, 0, 0xFF), PPR(PPR::RED1_TOP));
          }
        }
      break;
      }
    
    case waFrozenLake: case waLake: case waCamelotMoat:
    case waSea: case waOpenGate: case waBubble: case waDock:
    case waDeepWater: case waShallow:
    case waSulphur: case waMercury:
      break;
    
    case waNone: 
      if(c->land == laBrownian) goto wa_default;
      break;
    
    case waRose: {
      aura_color = wcol;
      wcol <<= 1;
      if(c->cpdist > 5)
        wcol = 0xC0C0C0;
      else if(rosephase == 7)
        wcol = 0xFF0000;
      else 
        wcol = gradient(wcol, 0xC00000, 0, rosephase, 6);
      queuepoly(V, cgi.shThorns, 0xC080C0FF);

      for(int u=0; u<4; u+=2)
        queuepoly(V * spin(30._deg * u), cgi.shRose, darkena(wcol, 0, 0xC0));
      break;
      }

    case waRoundTable: 
      if(wmspatial) goto wa_default;
      break;
    
    case waMirrorWall:
      break;
    
    case waGlass:
      if(wmspatial) {
        color_t col = winf[waGlass].color;
        int dcol = darkena(col, 0, 0x80);
        shiftmatrix Vdepth = orthogonal_move_fol(Vd, cgi.WALL);
        if(GDIM == 3) 
          draw_shapevec(c, V, cgi.shMFloor.levels[SIDE::WALL], dcol, PPR::WALL);
        else
          draw_floorshape(c, Vdepth, cgi.shMFloor, dcol, PPR::WALL); // GLASS
        dynamicval<qfloorinfo> dq(qfi, qfi);
        set_floor(cgi.shMFloor);
        if(cgi.validsidepar[SIDE::WALL]) forCellIdEx(c2, i, c)
          if(placeSidewall(c, i, SIDE::WALL, Vd, dcol)) break;
        }
      break;
    
    case waFan:
      #if MAXMDIM >= 4
      if(GDIM == 3)
        for(int a=0; a<10; a++)
        queuepoly(V * lzpush(cgi.FLOOR + (cgi.WALL - cgi.FLOOR) * a/10.) * spin(a *degree) * spintick(PURE ? -1000 : -500, 1/12.), cgi.shFan, darkena(wcol, 0, 0xFF));
      else
      #endif
        queuepoly(V * spintick(PURE ? -1000 : -500, 1/12.), cgi.shFan, darkena(wcol, 0, 0xFF));
      break;
    
    case waArrowTrap:
      if(c->wparam >= 1 || hat::in())
        queuepoly(orthogonal_move_fol(V, cgi.FLOOR), cgi.shDisk, darkena(trapcol[c->wparam&3], 0, 0xFF));
      if(isCentralTrap(c)) arrowtraps.push_back(c);
      break;
  
    case waFireTrap:

      if(GDIM == 3) {
        #if MAXMDIM >= 4
        draw_shapevec(c, V * lzpush(-cgi.human_height/40), cgi.shMFloor.levels[SIDE::FLOOR], darkena(0xC00000, 0, 0xFF));
        draw_shapevec(c, V * lzpush(-cgi.human_height/20), cgi.shMFloor2.levels[SIDE::FLOOR], darkena(0x600000, 0, 0xFF));
        #endif
        }
      else {
        draw_floorshape(c, V, cgi.shMFloor, darkena(0xC00000, 0, 0xFF));
        draw_floorshape(c, V, cgi.shMFloor2, darkena(0x600000, 0, 0xFF));
        }
      if(c->wparam >= 1)
        queuepoly(orthogonal_move_fol(V, cgi.FLOOR), cgi.shDisk, darkena(trapcol[c->wparam&3], 0, 0xFF));
      break;
  
    case waGiantRug:
      queuepoly(V, cgi.shBigCarpet1, darkena(GDIM == 3 ? 0 : 0xC09F00, 0, 0xFF));
      queuepoly(V, cgi.shBigCarpet2, darkena(GDIM == 3 ? 0xC09F00 : 0x600000, 0, 0xFF));
      queuepoly(V, cgi.shBigCarpet3, darkena(GDIM == 3 ? 0x600000 : 0xC09F00, 0, 0xFF));
      break;
    
    case waBarrier:
      if(c->land == laOceanWall && wmescher && wmspatial) {
       if(GDIM == 3 && qfi.fshape) {
         draw_shapevec(c, V, qfi.fshape->cone[1], darkena(wcol, 0, 0xFF), PPR::WALL);
         draw_wallshadow();
         break;
         }           
       const int layers = 2 << detaillevel;
       dynamicval<const hpcshape*> ds(qfi.shape, &cgi.shCircleFloor);
       dynamicval<transmatrix> dss(qfi.spin, Id);
       for(int z=1; z<layers; z++) {
         double zg = zgrad0(-vid.lake_top, geom3::actual_wall_height(), z, layers);
         draw_qfi(c, xyzscale(V, zg*(layers-z)/layers, zg),
           darkena(gradient(0, wcol, -layers, z, layers), 0, 0xFF), PPR::WALL_DECO);
         }
        }
      else goto wa_default;
      break;
    
    case waMineOpen: {
      int mines = countMinesAround(c);
      draw_mine_numbers(mines, V, ct6);
      break;
      }

    #if CAP_COMPLEX2
    case waMineUnknown: case waMineMine: {
      draw_mine_markers(c, V);
      break;
      }
    #endif
    
    case waEditStatue:
      if(!mapeditor::drawUserShape(V * ddspin(c, c->mondir), mapeditor::sgWall, c->wparam, darkena(wcol, fd, 0xFF), c))
        queuepoly(V, cgi.shTriangle, darkena(wcol, fd, 0xFF));
      break;
  
    default: {
      wa_default:

      if(sha.sl && wmspatial) {
        floorShadow(c, V, SHADOW_SL * sha.sl);
        }

      else if(highwall(c)) draw_wall();

      else if(xch == '%') {
        if(doHighlight())
          poly_outline = (c->land == laMirror) ? OUTLINE_TREASURE : OUTLINE_ORB;
        
        if(wmspatial) {
          color_t col = winf[c->wall].color;
          int dcol = darkena(col, 0, 0xC0);
          shiftmatrix Vdepth = orthogonal_move_fol(Vd, cgi.WALL);
          if(GDIM == 3)
            draw_shapevec(c, V, cgi.shMFloor.levels[SIDE::WALL], dcol, PPR::WALL);
          else
            draw_floorshape(c, Vdepth, cgi.shMFloor, dcol, PPR::WALL); // GLASS
          dynamicval<qfloorinfo> dq(qfi, qfi);
          set_floor(cgi.shMFloor);
          if(cgi.validsidepar[SIDE::WALL]) forCellIdEx(c2, i, c)
            if(placeSidewall(c, i, SIDE::WALL, Vd, dcol)) break;
          }
        else {
          queuepoly(V, cgi.shMirror, darkena(wcol, 0, 0xC0));
          }
        poly_outline = OUTLINE_DEFAULT;
        }

      #if CAP_COMPLEX2
      else if(isDie(c->wall)) {
        color_t col = darkena(winf[c->wall].color, 0, 0xFF);
        
        ld footphase;
        Vboat = V;
        bool animated = applyAnimation(c, Vboat, footphase, LAYER_BOAT);
        if(animated) {
          transmatrix U = inverse_shift(V, Vboat);
          U = rgpushxto0(tC0(U));
          Vboat = V * U; 
          }
        die_target = V;
        dice::draw_die(c, Vboat, 1, col);
        }
      #endif
      
      else if(c->wall == waExplosiveBarrel) {
        if(GDIM == 3 && qfi.fshape) {
          draw_shapevec(c, V, qfi.fshape->cone[1], 0xD00000FF, PPR::RED1_TOP);
          draw_wallshadow();
          break;
          }
        const int layers = 2 << detaillevel;
        for(int z=1; z<=layers; z++) {
          double zg = zgrad0(0, geom3::actual_wall_height(), z, layers);
          queuepolyat(xyzscale(V, zg, zg), cgi.shBarrel, darkena((z&1) ? 0xFF0000 : 0xC00000, 0, 0xFF), PPR(PPR::WALL_DECO));
          }
        }
      
      else if(isFire(c) || isThumper(c) || c->wall == waBonfireOff) {
        auto V2 = V;
        if(c->wall == waThumperOn) {
          ld ds = fractick(160);
          for(int u=0; u<5; u++) {
            ld rad = cgi.hexf * (.3 * (u + ds));
            int tcol = darkena(gradient(forecolor, backcolor, 0, rad, 1.5 * cgi.hexf), 0, 0xFF);
            PRING(a)
              curvepoint(xspinpush0(a * cgi.S_step, rad));
            queuecurve(V, tcol, 0, PPR::LINE);
            }
          }  
        if(hasTimeout(c)) V2 = V2 * spintick(c->land == laPower ? 5000 : 500);
        if(GDIM == 3 && qfi.fshape) {
          draw_shapevec(c, V2, qfi.fshape->cone[1], darkena(wcol, 0, 0xF0), PPR::WALL);
          draw_wallshadow();
          }
        else queuepoly(V2, cgi.shStar, darkena(wcol, 0, 0xF0));
        if(isFire(c) && rand() % 300 < ticks - lastt)
          drawParticle(c, wcol, 75);
        }
      
      else if(xch != '.' && xch != '+' && xch != '>' && xch != ':'&& xch != '-' && xch != ';' && xch != ',' && xch != '&')
        error = true;
      }
    }
  #endif
  }

void celldrawer::draw_features_and_walls_3d() {
#if MAXMDIM >= 4
  color_t dummy;
  int ofs = currentmap->wall_offset(c);

  if((cgflags & qFRACTAL) && c->wall == waChasm && c->land == laMemory) {
    for(int a=0; a<c->type; a++) if(c->move(a) && c->move(a)->land != laMemory) {
      if(anyshiftclick) {
        auto& poly = queuepoly(V, cgi.shPlainWall3D[ofs+a], 0xFFFFFFFF - 0xF0F0F00 * get_darkval(c, a));
        poly.tinf = &floor_texture_vertices[cgi.shFullFloor.id];
        ensure_vertex_number(*poly.tinf, poly.cnt);
        }
      else {
        auto& poly = queuepoly(V, cgi.shWireframe3D[ofs + a], 0);
        poly.outline = 0xFFFFFFFF;
        }
      }
    if(anyshiftclick) return;
    }

  #if CAP_COMPLEX2
  if(among(c->wall, waMineUnknown, waMineMine))
    draw_mine_markers(c, face_the_player(V));
  #endif

  if(isWall3(c, wcol)) {
    if(!no_wall_rendering) {
    if(c->wall == waChasm && c->land == laMemory && !in_perspective()) {
      for(int a=0; a<c->type; a++) if(c->move(a) && c->move(a)->land != laMemory) {
        auto& poly = queuepoly(V, cgi.shWireframe3D[ofs + a], 0);
        poly.outline = (wcol << 8) | 0x40;
        }
      goto after_walls;
      }
    color_t wcol2 = wcol;
    #if CAP_TEXTURE
    if(texture::config.tstate == texture::tsActive) wcol2 = texture::config.recolor(wcol);
    #endif

    int d = (wcol & 0xF0F0F0) >> 4;
    
    for(int a=0; a<c->type; a++) {
      bool b = true;
      if(c->move(a) && (in_perspective() || gmatrix0.count(c->move(a))))
        b = (patterns::innerwalls && (tC0(V)[2] < tC0(V * currentmap->adj(c, a))[2])) || fake::split() || !isWall3(c->move(a), dummy);
      if(b) {
        #if CAP_WRL
        /* always render */
        if(wrl::in && wrl::print) ; else
        #endif
        if(pmodel == mdPerspective && !sphere && !quotient && !aperiodic && !nonisotropic && !mhybrid && !experimental && !nih) {
          if(a < 4 && among(geometry, gHoroTris, gBinary3) && celldistAlt(c) >= celldistAlt(centerover)) continue;
          else if(a < 2 && among(geometry, gHoroRec) && celldistAlt(c) >= celldistAlt(centerover)) continue;
          // this optimization is not correct, need to fix
          // else if(c->move(a)->master->distance > c->master->distance && c->master->distance > centerover->master->distance && !quotient) continue;
          }
        else if(sol && in_perspective() && !nih && !asonov::in()) {
          ld b = vid.binary_width * log(2) / 2;
          const ld l = log(2) / 2;
          switch(a) {
            case 0: if(V[0][LDIM] >= b) continue; break;
            case 1: if(V[1][LDIM] >= b) continue; break;
            case 2: case 3: if (pmodel == mdPerspective && V[2][LDIM] >= l) continue; break;
            case 4: if(V[0][LDIM] <= -b) continue; break;
            case 5: if(V[1][LDIM] <= -b) continue; break;
            case 6: case 7: if (pmodel == mdPerspective && V[2][LDIM] <= -l) continue; break;
            }
          }
        else if(mproduct && !models::conformal_product_model()) {
          if(a < c->type-2 && !in_s2xe()) {
            ld d = in_e2xe() ? sqhypot_d(2, unshift(tC0(V))) : V[2][2];
            hyperpoint h = (unshift(V) * cgi.walltester[ofs + a]);
            ld d1 = in_e2xe() ? sqhypot_d(2, h) : h[2];
            if(d1 >= d - 1e-6) continue;
            }
          else if(a == c->type-1) {
            if(zlevel(tC0(V.T)) >= -cgi.plevel/2) continue;
            }
          else if(a == c->type-2) {
            if(zlevel(tC0(V.T)) <= +cgi.plevel/2) continue;
            }
          }
        if(qfi.fshape && wmescher) {
          auto& poly = queuepoly(V, cgi.shWall3D[ofs + a], darkena(wcol2 - d * get_darkval(c, a), 0, 0xFF));
          #if CAP_TEXTURE
          if(texture::config.tstate == texture::tsActive && isize(texture::config.tinf3.tvertices)) {
            poly.tinf = &texture::config.tinf3;
            poly.offset_texture = 0;
            }
          else
          #endif
          if(!floor_texture_vertices.empty() && neon_mode == eNeon::none) {
            poly.tinf = &floor_texture_vertices[qfi.fshape->id];
            ensure_vertex_number(*poly.tinf, poly.cnt);
            poly.offset_texture = 0;
            }
          }
        else
          queuepoly(V, cgi.shPlainWall3D[ofs + a], darkena(wcol2 - d * get_darkval(c, a), 0, 0xFF));
        }
      }
    if(WDIM == 3 && c->wall == waRose) {
      color_t col = winf[waRose].color;
      color_t col2 = (col << 8) | 0xFF;
      if(rosephase == 7) col2 = 0xFFFFFFFF;
      addradar(V, winf[waRose].glyph, col, col2);
      }
    } }
  else {
    if(!no_wall_rendering) for(int a=0; a<c->type; a++) if(c->move(a)) {
      color_t t = transcolor(c, c->move(a), wcol);
      if(t) {
        t = t - get_darkval(c, a) * ((t & 0xF0F0F000) >> 4);
        queue_transparent_wall(V, cgi.shPlainWall3D[ofs + a], t);
        }
      }
    if(among(c->wall, waBoat, waStrandedBoat)) draw_boat();
    else if(isFire(c)) {
      static int r = 0;
      r += ticks - lastt;
      int each = 5 + last_firelimit;
      while(r >= each) {
        drawParticleSpeed(c, wcol, 75 + rand() % 75);
        r -= each;
        }
      firelimit++;
      }
    else if(c->wall == waMineOpen) {
      if(pmodel == mdGeodesic && hdist0(tC0(V)) < 1e-3) {
        }
      else if(mproduct && hdist0(tC0(V)) < 1e-3) {
        }
      else {
        int mines = countMinesAround(c);
        draw_mine_numbers(mines, face_the_player(V), 0);
        }
      }
    
    else if(winf[c->wall].glyph == '.' || among(c->wall, waFloorA, waFloorB, waChasm, waLadder, waCanopy, waRed1, waRed2, waRed3, waRubble, waDeadfloor2) || isWatery(c) || isSulphuric(c->wall)) ;
    
    else if(c->wall == waBigBush || c->wall == waSolidBranch)
      queuepolyat(face_the_player(V), cgi.shSolidBranch, darkena(wcol, 0, 0xFF), PPR::RED3_TOP);

    else if(c->wall == waSmallBush || c->wall == waWeakBranch)
      queuepolyat(face_the_player(V), cgi.shWeakBranch, darkena(wcol, 0, 0xFF), PPR::RED2_TOP);
    
    else
      queuepoly(face_the_player(V), sha.deep < SIDE::SHALLOW ? cgi.shSawRing : cgi.shRing, darkena(wcol, 0, 0xFF));
    }

  after_walls:
  int rd = rosedist(c);
  if(rd == 1) 
    queuepoly(face_the_player(V), cgi.shLoveRing, darkena(0x804060, 0, 0xFF));
  if(rd == 2) 
    queuepoly(face_the_player(V), cgi.shLoveRing, darkena(0x402030, 0, 0xFF));
#endif
  }

#if HDR
struct downseek_t {
  cell *best;
  hyperpoint total;
  hyperpoint point;
  ld speed;
  ld depth;
  int qty;
  void reset() {
    qty = 0;
    total = Hypc;
    depth = 0;
    best = nullptr;
    }
  };
#endif

EX downseek_t downseek;

void celldrawer::check_rotations() {
  
  auto& ds = downseek;

  // we generally want function(ds.best) but this would fail if ds.best is not known yet,
  // so we use function(old) which will return the same value as function(c) if ds.best is not known yet
  cell *old = ds.best ? ds.best : c;

  auto use_if_less = [this] (int a, int b, ld spd, int side) {
    auto& ds = downseek;
    if(a > b) return;
    if(!ds.best || a < b) ds.reset();
    if(a <= b) {
      ds.best = c;
      ds.speed = spd;
      if(mproduct) {
        auto pd = product_decompose(unshift(tC0(V)));
        ds.total += pd.second;
        ds.depth += pd.first;
        }
      else
        ds.total += unshift(tC0(V));
      ds.qty++;
      ds.point = cgi.emb->normalize_flat(ds.total / ds.qty);
      if(mproduct) ds.point = orthogonal_move(ds.point, ds.depth / ds.qty);
      if(side == 2) for(int i=0; i<3; i++) ds.point[i] = -ds.point[i];
      if(side == 1) ds.point = spin(-90._deg) * ds.point;
      }
    };
  
  if(0);
  
  #if CAP_BT
  else if(bt::in() && models::do_rotate >= 2) {
    use_if_less(c->master->distance, old->master->distance, 1, 0);
    }
  #endif
  
  else if(isGravityLand(cwt.at->land) && cwt.at->land != laMountain) {
    int side = 0;
    if(cwt.at->land == laDungeon) side = 2;
    if(cwt.at->land == laWestWall) side = 1;
    if(cwt.at->land == laIvoryTower && ls::hv_structure()) side = 2;
    if(cwt.at->land == laDungeon && ls::hv_structure()) side = 0;
    if(models::do_rotate >= 1)
      use_if_less(edgeDepth(c), edgeDepth(old), cwt.at->landparam / 10., side);
    }
  
  else if(c->master->alt && cwt.at->master->alt &&
    ((cwt.at->land == laMountain && models::do_rotate >= 1)|| 
    (models::do_rotate >= 2 && 
      (cwt.at->land == laTemple || cwt.at->land == laWhirlpool || 
      (cheater && (cwt.at->land == laClearing || cwt.at->land == laCaribbean ||
      cwt.at->land == laCamelot || cwt.at->land == laPalace))) 
      ))
    && c->land == cwt.at->land && c->master->alt->alt == cwt.at->master->alt->alt) {
      use_if_less(celldistAlt(c), celldistAlt(old), .5, (cwt.at->land == laMountain) ? 2 : 0);
    }

  else if(models::do_rotate >= 2 && cwt.at->land == laOcean && cwt.at->landparam < 25) {
    use_if_less(coastval(c, laOcean), coastval(old, laOcean), cwt.at->landparam / 10, 0);
    }
  }

EX int debug_tiles;

void celldrawer::bookkeeping() {
  bool orig = false;
  if(!inmirrorcount) {
    auto& gm = gmatrix[c];
    orig = gm.is_null || hdist0(tC0(gm)) >= hdist0(tC0(V));
    if(orig) gm = V;
    current_display->all_drawn_copies[c].emplace_back(V);
    }
  if(just_gmatrix) return;

  set_detail_level(V);
  
  // save the player's view center
  if(isPlayerOn(c) && !shmup::on) {
    playerfound = true;

    if(multi::players > 1) {
      for(int i: player_indices()) if(playerpos(i) == c) {
        playerV = V * ddspin(c, multi::player[i].spin, 0);
        if(multi::player[i].mirrored) playerV = playerV * Mirror;
        if(orig)
          multi::whereis[i] = playerV;
        }
      }
    else {
      playerV = V * ddspin(c, cwt.spin, 0);
      if(cwt.mirrored) playerV = playerV * Mirror;
      transmatrix F = back_to_view(V);
      if((!confusingGeometry() && !fake::split() && !inmirrorcount) || eqmatrix(F, current_display->which_copy, 1e-2))
        current_display->which_copy = F;
      if(orig) cwtV = playerV;
      }
    }
  
  ld dist = mousedist(V);

  if(inmirrorcount) ;
  else if(WDIM == 3) ;
  else if(dist < modist) {
    modist2 = modist; mouseover2 = mouseover;
    modist = dist;
    mouseover = c;
    mouseoverV = V;
    }
  else if(dist < modist2) {
    modist2 = dist;
    mouseover2 = c;
    }

  int orbrange = (items[itRevolver] ? 3 : 2);
  
  if(c->cpdist <= orbrange) if(multi::players > 1 || multi::alwaysuse) 
  for(int i=0; i<multi::players; i++) if(multi::playerActive(i)) {
    double dfc = hdist(tC0(V), tC0(multi::crosscenter[i]));
    if(dfc < multi::ccdist[i] && celldistance(playerpos(i), c) <= orbrange) {
      multi::ccdist[i] = dfc;
      multi::ccat[i] = c;
      }
    }
  }

EX bool draw_plain_floors;

/** 
  Display text statistics about the cell (distances in viewdists mode, pattern codes, etc.).
  may also change the colors provided in 
  */

void celldrawer::draw_cellstat() {
  if(viewdists) do_viewdist();

  for(int i=0; i<isize(buggycells); i++)
    if(c == buggycells[i]) {
      queuepoly(V, cgi.shPirateX, 0xC000C080);
      return;
      }

  for(int i=0; i<isize(buggycells); i++) {
    cell *c1 = buggycells[i];
    cell *cf = cwt.at;
    
    while(cf != c1) {
      cf = pathTowards(cf, c1);
      if(cf == c) {
        queuepoly(V, cgi.shMineMark[1], 0xC000C0D0);
        return;
        }
      }
    }
  
  if(c->land == laMirrored || c->land == laMirrorWall2 || c->land == laMirrored2) {
    string label = its(c->landparam);
    queuestr(V, mapfontscale / 500, label, 0xFFFFFFFF, 1);
    }

  if(debug_tiles && mod_allowed()) {
    string label;
    if(arb::in() && debug_tiles == 2) {
      int id = shvid(c);
      auto& sh = arb::current.shapes[id];
      int oid = sh.orig_id;
      label = its(oid);
      if(c->master->emeraldval || sh.is_mirrored) label += 'R';
      if(arb::current.was_split_for_football) label += "F" + its(sh.football_type);
      }
#if CAP_ARCM
    else if(arcm::in() && debug_tiles == 2) {
      int id = arcm::id_of(c->master);
      int tid = arcm::current.tilegroup[id];
      int tid2 = arcm::current.tilegroup[id^1];
      bool mirrored = (id&1) && (tid != tid2);
      if(tid2 >= 0) tid = min(tid, tid2);
      label = its(tid);
      if(mirrored) label += "R";
      }
#endif
    else
      label = its(shvid(c));
    queuestr(V, mapfontscale / 200, label, 0xFFFFFFFF, 1);
    for(int i=0; i<c->type; i++) {
      queuestr(V * rgpushxto0(currentmap->get_corner(c, i, 4)), mapfontscale / 500, its(i), 0xFFFFFFFF, 1);
      queuestr(V * rgpushxto0(mid(currentmap->get_corner(c, i, 4), currentmap->get_corner(c, i+1, 5))), mapfontscale / 500, its(i), 0xFFFFFFFF, 1);
      }
    }

  if(debug_voronoi && ls::hv_structure() && mod_allowed()) {
    auto p = get_voronoi_winner(c);
    queuestr(V, mapfontscale / 500, its(p.second), index_pointer_int(p.first) * 0x7F3015, 1);
    }
    
  if(cmode & sm::TORUSCONFIG) {
    auto p = euc::coord_display(V, c);
    if(p.second != "")
      queuestr(V, (p.first ? .2 : .6) * mapfontscale / 100, p.second, p.first ? 0xFFFFFFD0 : 0xFFFF0040, 1);
    }

  #if CAP_EDIT
  if(patterns::displaycodes) {
  
    auto si = patterns::getpatterninfo0(c);
    
    for(int i= gmod(si.dir, si.symmetries); i<c->type; i += si.symmetries) {
      queuepoly(V * ddspin(c, i) * (si.reflect?Mirror:Id), cgi.shAsymmetric, darkena(0x000000, 0, 0xC0));
      si.dir += si.symmetries;
      }
    
    string label = its(si.id & 255);
    color_t col = forecolor ^ colorhash(si.id >> 8);
    queuestr(V, mapfontscale / 200, label, 0xFF000000 + col);
    }
  #endif

  if(debug_cellnames && pointer_indices.count(c)) {
    shstream ss; print(ss, c);
    queuestr(V, mapfontscale / 200, ss.s, 0xFFFFFFFF);    
    queuepoly(V * ddspin(c, 0), cgi.shAsymmetric, darkena(0x000000, 0, 0xC0));
    }
  }

EX int default_flooralpha = 255;

void celldrawer::draw_wall_full() {

  shiftmatrix Vf0;
  const shiftmatrix& Vf = (sha.top != SIDE::FLOOR) ? (Vf0=orthogonal_move_fol(V, cgi.dhi_table[sha.top])) : V;

  #if CAP_SHAPES
  int flooralpha = default_flooralpha;
  #endif

  #if CAP_EDIT && CAP_TEXTURE
  if((cmode & sm::DRAW) && mapeditor::drawcell && mapeditor::drawcellShapeGroup() == mapeditor::sgFloor && texture::config.tstate != texture::tsActive)
    flooralpha = 0xC0;
  #endif

  #if CAP_SHAPES
  set_floor(cgi.shFloor);
  #endif
  if(!wmascii) {    

#if CAP_EDIT
    auto si = patterns::getpatterninfo0(c);
    if(drawing_usershape_on(c, mapeditor::sgFloor))
      mapeditor::drawtrans = V * applyPatterndir(c, si);
#endif
      
#if CAP_SHAPES
    // floor
    if(GDIM == 2 && (c->land != laRose || ls::any_chaos())) {
      int rd = rosedist(c);
      if(rd == 1) 
        draw_floorshape(c, at_smart_lof(V, cgi.RED[2]), cgi.shRoseFloor, 0x80406040, PPR::LIZEYE);
      if(rd == 2)
        draw_floorshape(c, at_smart_lof(V, cgi.RED[2]), cgi.shRoseFloor, 0x80406080, PPR::LIZEYE);
      }

    if(c->wall == waChasm) {
      aura_color = 0;
      int rd = rosedist(c);
      if(GDIM == 2) {
        if(rd == 1) 
          draw_floorshape(c, V, cgi.shRoseFloor, 0x80406020);
        if(rd == 2)
          draw_floorshape(c, V, cgi.shRoseFloor, 0x80406040);
        }
      if(c->land == laZebra) fd++;
      if(c->land == laHalloween && !wmblack) {
        shiftmatrix Vdepth = wmspatial ? orthogonal_move_fol(V, cgi.DEEP) : V;
        if(GDIM == 3)
          draw_shapevec(c, V, cgi.shFullFloor.levels[SIDE::WATERLEVEL], darkena(firecolor(0, 10), 0, 0xDF), PPR::TRANSPARENT_LAKE);
        else
          draw_floorshape(c, Vdepth, cgi.shFullFloor, darkena(firecolor(0, 10), 0, 0xDF), PPR::DEEP_TOP);
        }
      }

#if CAP_EDIT
    else if(mapeditor::haveUserShape(mapeditor::sgFloor, si.id)) {
      qfi.usershape = si.id;
      qfi.spin = applyPatterndir(c, si);
      }
#endif

    else if(patterns::whichShape == '7')
      set_floor(cgi.shBigHepta);
    
    else if(patterns::whichShape == '8')
      set_floor(cgi.shTriheptaFloor);
    
    else if(patterns::whichShape == '6')
      set_floor(cgi.shBigTriangle);

    else if(among(patterns::whichShape, '9', '^'))
      set_floor(cgi.shFullFloor);

    else if(patterns::whichShape == '5')
      set_floor(cgi.shFloor);

    else if(patterns::whichShape == '4')
      set_floor(cgi.shMFloor);

    else if(patterns::whichShape == '3')
      set_floor(cgi.shMFloor2);

    else if(patterns::whichShape == '2')
      set_floor(cgi.shMFloor3);

#if CAP_TEXTURE
    else if(GDIM == 2 && texture::config.apply(c, Vf, darkena(fcol, fd, 0xFF))) ;
#endif
    
    else if(c->land == laMirrorWall) 
      draw_mirrorwall();
    
    else if(c->land == laWineyard && cellHalfvine(c)) 
      draw_halfvine();

    else if(c->land == laReptile || c->wall == waReptile) 
      set_reptile_floor(Vf, fcol);
    
    else if(wmblack == 1 && c->wall == waMineOpen && vid.grid) 
      ;
    
    else if(wmblack) {
      set_floor(cgi.shBFloor[ct6]);
      int rd = rosedist(c);
      if(WDIM == 2 && rd == 1)
        queuepoly(Vf, cgi.shHeptaMarker, darkena(fcol, 0, 0x80));
      else if(WDIM == 2 && rd == 2)
        queuepoly(Vf, cgi.shHeptaMarker, darkena(fcol, 0, 0x40));
      }
    
    else if(isWarped(c) || is_nice_dual(c))
      set_maywarp_floor(); 

    else if(wmplain) {
      set_floor(cgi.shFloor);
      }

    else if(set_randompattern_floor()) ;
    
    else set_land_floor(Vf);

    if(embedded_plane && qfi.fshape == &cgi.shFloor) set_floor(cgi.shFullFloor);

    // actually draw the floor

    if((sha.top != sha.deep) && wmspatial && detaillevel) {
    
      color_t col = c->land == laCocytus ? 0x080808FF : 0x101010FF;

      if(qfi.fshape == &cgi.shCloudFloor) 
        set_floor(cgi.shCloudSeabed);
      else if(qfi.fshape == &cgi.shLavaFloor) 
        set_floor(cgi.shLavaSeabed);
      else if(qfi.fshape == &cgi.shFloor)
        set_floor(cgi.shFullFloor);
      else if(qfi.fshape == &cgi.shCaveFloor)
        set_floor(cgi.shCaveSeabed);
      
      if(WDIM == 2 && GDIM == 3 && qfi.fshape)
        draw_shapevec(c, V, qfi.fshape->levels[sha.deep], col, side_to_prio_top[sha.deep]);
      else
        draw_qfi(c, orthogonal_move_fol(V, cgi.dhi_table[sha.deep]), col, side_to_prio_top[sha.deep]);

      int fd0 = fd ? fd-1 : 0;
      if(WDIM == 2 && GDIM == 3 && qfi.fshape)
        draw_shapevec(c, V, qfi.fshape->levels[sha.top], darkena3(fcol, fd0, 0x80), PPR::TRANSPARENT_LAKE);
      else
        draw_qfi(c, Vd, darkena(fcol, fd0, 0x80), PPR::WATERLEVEL_TOP);
      }

    else if(sha.top != SIDE::INFDEEP) {
      auto col = fcol;
      if(patterns::whichShape == '^') poly_outline = darkena(fcol, fd, flooralpha);
      if(sha.top == SIDE::WALL) col = wcol_star;
      else if(sha.top >= SIDE::RED1) col = wcol;

      auto sf = sha.top; if(!wmspatial) sf = SIDE::FLOOR;

      if(WDIM == 2 && GDIM == 3 && qfi.fshape && !draw_plain_floors)
        draw_shapevec(c, V, qfi.fshape->levels[sf], darkena(col, fd, 255), side_to_prio_top[sha.top]);
      else if(sf == SIDE::FLOOR)
        draw_qfi(c, V, darkena3(col, fd, flooralpha), PPR::FLOOR);
      else
        draw_qfi(c, orthogonal_move_fol(V, cgi.dhi_table[sf]), darkena3(col, fd, flooralpha), side_to_prio_top[sha.top]);
      }
    
    #if MAXMDIM >= 4
    // draw the ceiling
    if(WDIM == 2 && GDIM == 3) {
      #if CAP_GL
      draw_ceiling();
      #endif

      int rd = rosedist(c);
      if(rd) {
        dynamicval<qfloorinfo> qfi2(qfi, qfi);
        qfi.fshape = &cgi.shRoseFloor;
        int t = isize(ptds);
        color_t rcol;
        if(rd == 1) rcol = 0x80406040;
        if(rd == 2) rcol = 0x80406080;
        forCellIdEx(c2, i, c)
          if(rosedist(c2) < rd)
            placeSidewall(c, i, SIDE::WALL, V, rcol);
        for(int i=t; i<isize(ptds); i++) {
          auto p = ptds[i]->as_poly();
          if(p) p->prio = PPR::TRANSPARENT_WALL;
          }
        }
      }
    #endif

    // walls
    if(realred(c->wall) && !wmspatial) {
      int s = snakelevel(c);
      if(s >= 1) draw_floorshape(c, V, cgi.shRedRockFloor[0], getSnakelevColor(0, 7));
      if(s >= 2) draw_floorshape(c, V, cgi.shRedRockFloor[1], getSnakelevColor(1, 7));
      if(s >= 3) draw_floorshape(c, V, cgi.shRedRockFloor[2], getSnakelevColor(2, 7));
      }
    
    if(c->wall == waTower && !wmspatial) {
      fcol = 0xE8E8E8;
      set_floor(cgi.shMFloor);
      }
    
    if(WDIM == 2 && pseudohept(c) && (
      c->land == laRedRock || 
      vid.darkhepta ||
      (c->land == laClearing && !BITRUNCATED))) {
      #if MAXMDIM >= 4
      if(GDIM == 3 && WDIM == 2)
        queuepoly(Vd*lzpush(cgi.FLOOR), cgi.shHeptaMarker, wmblack ? 0x80808080 : 0x00000080);
      else
      #endif
        queuepoly(Vd, cgi.shHeptaMarker, wmblack ? 0x80808080 : 0x00000080);
      }

    if(history::includeHistory && history::inmovehistory.count(c))
      queuepoly(Vd, cgi.shHeptaMarker, 0x000000C0);

    #if MAXMDIM >= 4
    if(WDIM == 3)
      draw_features_and_walls_3d();
    #else
    if(0) ;
    #endif
    
    else draw_features();
#else
    error = true;
#endif
    }

  if(wmascii || (WDIM == 2 && GDIM == 3)) {
    if(c->wall == waNone || isWatery(c)) asciicol = fcol;
    if(c->wall == waBoat) asciicol = 0xC06000;

    if(c->wall == waArrowTrap)
      asciicol = trapcol[c->wparam & 3];
    if(c->wall == waFireTrap)
      asciicol = trapcol[c->wparam & 3];
    if(c->wall == waTerraWarrior)
      asciicol = terracol[c->wparam & 7];

    if(c->wall == waMineOpen) {
      int mines = countMinesAround(c);
      if(asciichar == '.') {
        if(mines == 0) asciichar = ' ';
        else asciichar = '0' + mines, asciicol = minecolors[mines];
        }
      else if(asciichar == '@') {
        asciicol = minecolors[mines];
        }
      }
    if(wmascii && !((c->item && !itemHiddenFromSight(c)) || c->monst || c->cpdist == 0)) error = true;
    asciiborder = bordcolor;
    }

#if CAP_SHAPES
  if(wmspatial && WDIM == 2) {

    color_t col = (highwall(c) || c->wall == waTower) ? wcol : fcol;

    auto shab = sha.levels;
    forCellIdEx(c1, i, c) {
      auto fsha1 = get_spatial_info(c1);
      auto& sha1 = fsha1.levels;
      auto sha0 = shab;

      constexpr int w = Flag((int) SIDE::WALL);
      constexpr int r = ((Flag((int) SIDE::RED1)) | (Flag((int) SIDE::RED2)) | (Flag((int) SIDE::RED3)));
      constexpr int r4 = r | (Flag((int) SIDE::RED4));

      if(fake::split()) sha1 ^= (w | r | r4);

      if((c->land == laCamelot && c->wall == waCamelot) || ((sha0 & w) && (sha1 & r))) {
        sha0 ^= (w | r | r4);
        }

      auto shad = sha0 &~ sha1;

      while(shad) {
        int lev = __builtin_ctz(shad);
        shad ^= (1<<lev);
        auto col1 = darkena(col, fd, 0xFF);

#define D(v) darkena(gradient(0, col, 0, v * (sphere ? spherity(V.T * currentmap->adj(c,i)) : 1), 1), fd, 0xFF)
        if(SIDE(lev) == SIDE::FLOOR) col1 = D(0.8);
        if(SIDE(lev) == SIDE::WATERLEVEL) col1 = D(0.75);
        if(SIDE(lev) == SIDE::SHALLOW) col1 = D(0.7);
#undef D
        if(SIDE(lev) == SIDE::DEEP) col1 = 0x101010FF;
        if(SIDE(lev) == SIDE::WALL) {
          col1 = (geometry == gEuclidSquare && (i&1)) ? gradient(0, wcol_star, 0, .9, 1) : gradient(0, wcol_star, 0, .8, 1);
          col1 = darkena(col1, fd, 255);
          }

        if(lev >= int(SIDE::RED1) && lev <= int(SIDE::RED3) && sha.sl) {
          col1 = getSnakelevColor(lev - int(SIDE::RED1), sha.sl);
          }

        if(placeSidewall(c, i, SIDE(lev), V, col1)) shab &= ~(1<<lev);
        }

      if(sha.top == SIDE::WATERLEVEL && fsha1.top == SIDE::INFDEEP) {
        dynamicval<qfloorinfo> qfib(qfi, qfi);
        set_floor(cgi.shFullFloor);
        placeSidewall(c, i, SIDE::WATERLEVEL, V, 0x202030FF);
        if(sha.deep == SIDE::DEEP) placeSidewall(c, i, SIDE::SHALLOW, V, 0x181820FF);
        }
      }
    }
  }

void celldrawer::draw_item_full() {
  eItem it = c->item;
  
  bool hidden = itemHidden(c);
  bool hiddens = itemHiddenFromSight(c);
  
  if(history::includeHistory && history::infindhistory.count(c)) {
    hidden = true;
    hiddens = false;
    }
  
  if(hiddens && !(cmode & sm::MAP))
    it = itNone;

  color_t icol = 0;
  
  if(ls::single() && specialland == laMotion && items[itOrbInvis] && c->wall == waChasm) {
    if(c->mondir < c->type) {
      vid.linewidth *= 3;
      queueline(V*C0, V*currentmap->adj(c, c->mondir) * C0, 0xFFD500FF, 4);
      vid.linewidth /= 3;
      }
    }  
  
  if(it) {
    asciichar = iinf[it].glyph, asciicol = icol = iinf[it].color;
    if(doHighlight()) asciiborder = kind_outline(it) >> 8;
    
    if(it == itCompass && isPlayerOn(c)) {
      shiftpoint dummy;
      mark_compass(c, dummy);
      }
    }
  
  if(it) {
    if(c->wall != waBoat) {
      double footphase = 0;
      applyAnimation(c, Vboat, footphase, LAYER_BOAT);
      }

    if(cellHalfvine(c)) {
      int i =-1;
      for(int t=0;t<6; t++) if(c->move(t) && c->move(t)->wall == c->wall)
        i = t;
  
      Vboat = Vboat * ddspin(c, i) * xpush(-.13);
      }
  
    if(drawItemType(it, c, Vboat, icol, ticks, hidden)) onradar = false;
    }
  }

void celldrawer::draw_monster_full() {
  color_t moncol = 0xFF00FF;
  
  if(c->monst) {
    asciichar = minf[c->monst].glyph, moncol = minf[c->monst].color;
    if(c->monst == moMimic) {
      int all = 0, mirr = 0;
      for(auto& m: mirror::mirrors) if(c == m.second.at) {
        all++;
        if(m.second.mirrored) mirr++;
        }
      if(all == 1) moncol = mirrorcolor(mirr);
      }
    if(c->monst == moMutant) {
      // root coloring
      if(c->stuntime != mutantphase)
        moncol = 
          gradient(0xC00030, 0x008000, 0, (c->stuntime-mutantphase) & 15, 15);
      }
    if(isMetalBeast(c->monst) && c->stuntime) 
      moncol >>= 1;

    if(c->monst == moSlime) {
      moncol = winf[conditional_flip_slime(det(V.T) < 0, c->wall)].color;
      moncol |= (moncol>>1);
      }
    
    asciicol = moncol;
    if(doHighlight() && !noHighlight(c->monst))
      asciiborder = (isFriendly(c) ? OUTLINE_FRIEND : OUTLINE_ENEMY) >> 8;

    if(isDragon(c->monst) || isKraken(c->monst)) if(!c->hitpoints)
      asciicol = 0x505050;
    
    if(c->monst == moTortoise)
      asciicol =  tortoise::getMatchColor(tortoise::getb(c));
    
    if(c->monst != moMutant) for(int k=0; k<c->stuntime; k++) 
      asciicol = ((asciicol & 0xFEFEFE) >> 1) + 0x101010;
    }
  
  if(c->cpdist == 0 && mapeditor::drawplayer && !shmup::on) { 
    asciichar = '@'; 
    if(!mmitem) asciicol = moncol = cheater ? 0xFF3030 : 0xD0D0D0; 
    if(doHighlight())
      asciiborder = OUTLINE_FRIEND >> 8;
    }
  
  #if CAP_SHAPES
  int q = isize(ptds);
  #endif
  bool dm = drawMonster(V, c->type, c, moncol, asciicol);
  if(dm) onradar = false; 
  #if CAP_SHAPES
  if(isize(ptds) != q && !(c == lmouseover_distant && isDie(c->monst))) {
    if(WDIM == 2 && GDIM == 3 && abs(cgi.RED[sha.sl] - cgi.FLOOR) > 1e-6)
      pushdown(c, q, V, cgi.RED[sha.sl] - cgi.FLOOR, false, false);
    if(GDIM ==2 && abs(geom3::factor_to_lev(zlevel(tC0(Vboat.T)))) > 1e-6)
      pushdown(c, q, V, -geom3::factor_to_lev(zlevel(tC0(Vboat.T))), !isMultitile(c->monst), false);
    }
  #endif
  }

void celldrawer::add_map_effects() {

  if(get_spatial_info(c).top < SIDE::FLOOR) draw_fallanims();

  if(!shmup::on && sword::at(c)) {
    queuepolyat(V, cgi.shDisk, 0xC0404040, PPR::SWORDMARK);
    }

  if(c->land == laNone && (cmode & sm::MAP)) {
    queuepoly(V, cgi.shTriangle, 0xFF0000FF);
    }

  /* Air Elemental */
  if(1) {
    int ad = airdist(c);
    if(ad == 1 || ad == 2) {
      for(int i=0; i<c->type; i++) {
        cell *c2 = c->move(i); 
        if(airdist(c2) < airdist(c)) {
          ld airdir = calcAirdir(c2); // printf("airdir = %d\n", airdir);
          transmatrix V0 = ddspin180(c, i);
          
          double ph = ptick(PURE?150:75) + airdir;
          
          int aircol = 0x8080FF00 | int(32 + 32 * -cos(ph));
          
          double ph0 = ph/2;
          ph0 -= floor(ph0/M_PI)*M_PI;
 
          poly_outline = OUTLINE_TRANS;
          queuepoly(Vd*V0*xpush(cgi.hexf*-cos(ph0)), cgi.shDisk, aircol);
          poly_outline = OUTLINE_DEFAULT;
          }
        }
      }
    }

  draw_bowpath();

  if(c->land == laBlizzard) {
    if(vid.backeffects) {
      if(c->cpdist <= getDistLimit())
        set_blizzard_frame(c, frameid);
      }
    else {
      forCellIdEx(c2, i, c) if(againstWind(c, c2))
        queuepoly(V * ddspin(c, i) * xpush(cellgfxdist(c, i)/2), cgi.shWindArrow, 0x8080FF80);
      }
    }

  if(c->ligon) {
    int tim = ticks - lightat;
    if(tim > 1000) tim = 800;
    if(elec::havecharge && tim > 400) tim = 400;
    for(int t=0; t<c->type; t++) if(c->move(t)) {
      if(c->move(t)->ligon) {
        int lcol = darkena(gradient(iinf[itOrbLightning].color, 0, 0, tim, 1100), 0, 0xFF);
        queueline(V*chei(xspinpush((vid.flasheffects ? ticks : ptick(8)) * cgi.S_step, cgi.hexf/2), rand() % 1000, 1000) * C0, V*chei(currentmap->adj(c, t), rand() % 1000, 1000) * C0, lcol, 2 + vid.linequality);
        }
      for(int u: {-1, 1}) {
        cellwalker cw = cellwalker(c, t) + wstep + u;
        if(u == -1 && valence() == 4) continue;
        cell *c2 = cw.peek();
        if(c2 && c2->ligon) {
          int lcol = darkena(gradient(iinf[itOrbLightning].color, 0, 0, tim, 1100), 0, 0xFF);
          queueline(V*chei(xspinpush((vid.flasheffects ? ticks : ptick(8)) * cgi.S_step, cgi.hexf/2), rand() % 1000, 1000) * C0, V*chei(currentmap->adj(c, t)*currentmap->adj(cw.at, cw.spin), rand() % 1000, 1000) * C0, lcol, 2 + vid.linequality);
          }
        }
      }
    }

  if(c->land == laWhirlwind) {
    whirlwind::calcdirs(c);
    
    for(int i=0; i<whirlwind::qdirs; i++) {
      ld hdir0 = currentmap->spin_angle(c, whirlwind::dfrom[i]) + M_PI;
      ld hdir1 = currentmap->spin_angle(c, whirlwind::dto[i]);
      /* todo what if no spin_angle */
  
      double ph1 = fractick(PURE ? 150 : 75);
      
      int aircol = 0xC0C0FF40;
      
      cyclefix(hdir1, hdir0);
      
      ld hdir = (hdir1*ph1+hdir0*(1-ph1));
  
      transmatrix V0 = spin(hdir);
      
      double ldist = 
        cellgfxdist(c, whirlwind::dfrom[i]) * (1-ph1)/2 + 
        cellgfxdist(c, whirlwind::dto[i]) * ph1/2; 
      // PURE ? cgi.crossf : c->type == 6 ? .2840 : 0.3399;
  
      poly_outline = OUTLINE_TRANS;
      queuepoly(Vd*V0*xpush(ldist*(2*ph1-1)), cgi.shDisk, aircol);
      poly_outline = OUTLINE_DEFAULT;
      }
    }

  if(items[itOrbGravity] && c->cpdist <= 5) 
    draw_gravity_particles();
  }

void celldrawer::draw_bowpath() {
  auto v = at_or_null(bow::bowpath_map, c);
  if(!v) return;
  for(auto& m: *v) {

    hyperpoint h0 = C0, t0 = Hypc, h1 = C0, t1 = Hypc;
    bool birth = m.flags & bow::bpFIRST;

    if(!birth) {
      ld d = cellgfxdist(c, m.prev.spin) / 2;
      h0 = ddspin(c, m.prev.spin) * xpush0(d);
      t0 = ddspin(c, m.prev.spin) * xpush(d) * xtangent(-d*2);
      }
    else birth = true;

    if(!(m.flags & bow::bpLAST)) {
      ld d = cellgfxdist(c, m.next.spin) / 2;
      h1 = ddspin(c, m.next.spin) * xpush0(d);
      t1 = ddspin(c, m.next.spin) * xpush(d) * xtangent(-d*2);
      }

    ld t = frac(ptick(PURE?500:250));

    color_t arrow_color = getcs().swordcolor;

    if(m.flags & bow::bpCOPIED) {
      arrow_color &= 0xFF;
      arrow_color |= mirrorcolor(m.next.mirrored != cwt.mirrored ? 1 : 0) << 8;
      }

    color_t arrow_color_trans = arrow_color & 0xFFFFFF00;
    if(bow::fire_mode) arrow_color = gradient(arrow_color_trans, arrow_color, 0, 0.25, 1);

    auto V1 = shmup::at_missile_level(V);

    if(birth) {
      if(t > 0.8) {
        hyperpoint h = h1 + t1 * (1-t);
        hyperpoint tg = -t1;

        poly_outline = OUTLINE_TRANS;
        queuepoly(V1 * rgpushxto0(h) * rspintox(gpushxto0(h) * tg), cgi.shTrapArrow, gradient(arrow_color_trans, arrow_color, 0.8, t, 1));
        poly_outline = OUTLINE_DEFAULT;
        }
      }

    else {
      hyperpoint h = h0 * (1-t) * (1-t) * (1 + 2 * t) + t0 * (1-t) * (1-t) * t + h1 * t * t * (3 - 2 * t) + t1 * t * t * (1-t);
      h = normalize(h);

      hyperpoint tg = (h1 - h0) * 6 * t * (1-t) + (3 * t*t - 4*t + 1) * t0 + (2*t-3*t*t) * t1;

      poly_outline = OUTLINE_TRANS;
      queuepoly(V1 * rgpushxto0(h) * rspintox(gpushxto0(h) * tg), cgi.shTrapArrow, arrow_color);
      poly_outline = OUTLINE_DEFAULT;
      }
    }
  }

void celldrawer::draw_gravity_particles() {
  unsigned int u = (unsigned int)(size_t)(c);
  u = ((u * 137) + (u % 1000) * 51) % 1000;
  int tt = ticks + u;
  ld r0 = (tt % 900) / 1100.;
  ld r1 = (tt % 900 + 200) / 1100.;
  
  const color_t grav_normal_color = 0x808080FF;
  const color_t antigrav_color = 0xF04040FF;
  const color_t levitate_color = 0x40F040FF;
  
  auto levf = [] (ld l) { 
    return GDIM == 3 ? cheilevel(l) : 1 + (1-l) * 1;
    };
  
  if(spatial_graphics || (WDIM == 2 && GDIM == 3)) {

    switch(gravity_state) {
      case gsNormal:
        for(int i=0; i<6; i++) {
          shiftmatrix T = V * spin(i*degree*60) * xpush(cgi.crossf/3);
          queueline(at_smart_lof(T, levf(r0)) * C0, at_smart_lof(T, levf(r1)) * C0, grav_normal_color);
          }
        break;
      
      case gsAnti:
        for(int i=0; i<6; i++) {
          shiftmatrix T = V * spin(i*degree*60) * xpush(cgi.crossf/3);
          queueline(at_smart_lof(T, levf(r0)) * C0, at_smart_lof(T, levf(r1)) * C0, antigrav_color);
          }
        break;
      
      case gsLevitation:
        for(int i=0; i<6; i++) {
          shiftmatrix T0 = V * spin(i*degree*60 + tt/60. * degree) * xpush(cgi.crossf/3);
          shiftmatrix T1 = V * spin(i*degree*60 + (tt/60. + 30) * degree) * xpush(cgi.crossf/3);
          ld lv = levf(GDIM == 3 ? (i+0.5)/6 : 0.5);
          queueline(at_smart_lof(T0, lv) * C0, at_smart_lof(T1, lv) * C0, levitate_color);
          }
        break;
      }
    }
  
  else {
    switch(gravity_state) {
      case gsNormal:
        for(int i=0; i<6; i++) {
          shiftmatrix T0 = V * spin(i*degree*60) * xpush(cgi.crossf/3 * (1-r0));
          shiftmatrix T1 = V * spin(i*degree*60) * xpush(cgi.crossf/3 * (1-r1));
          queueline(T0 * C0, T1 * C0, grav_normal_color);
          }
        break;
      
      case gsAnti:
        for(int i=0; i<6; i++) {
          shiftmatrix T0 = V * spin(i*degree*60) * xpush(cgi.crossf/3 * r0);
          shiftmatrix T1 = V * spin(i*degree*60) * xpush(cgi.crossf/3 * r1);
          queueline(T0 * C0, T1 * C0, antigrav_color);
          }
        break;
      
      case gsLevitation:
        for(int i=0; i<6; i++) {
          shiftmatrix T0 = V * spin(i*degree*60 + tt/60. * degree) * xpush(cgi.crossf/3);
          shiftmatrix T1 = V * spin(i*degree*60 + (tt/60. + 30) * degree) * xpush(cgi.crossf/3);
          queueline(T0 * C0, T1 * C0, levitate_color);
          }
        break;
      }
    }
  }

EX shiftmatrix ocwtV;

void celldrawer::draw() {

  cells_drawn++;

#if CAP_TEXTURE
  if(texture::saving) {
    texture::config.apply(c, V, 0xFFFFFFFF);
    draw_qfi(c, V, 0xFFFFFFFF);
    return;
    }

  if((cmode & sm::DRAW) && mapeditor::intexture && !mouseout() && c) 
    mapeditor::draw_texture_ghosts(c, V);
#endif

  bookkeeping();
  if(just_gmatrix) return;
 
  #if MAXMDIM >= 4
  if(cell_clipped()) return;  
  #endif

  ivoryz = isGravityLand(c->land);

  // if(behindsphere(V)) return;
  
  if(callhandlers(false, hooks_drawcell, c, V)) return;
  
  checkTide(c);
  
  if(1) {
  
    if(inmirror(c)) {
      if(inmirrorcount >= 10) return;
      cellwalker cw(c);
      cellwalker cw2 = mirror::reflect(cw);
      int cmc = (cw2.mirrored == cw.mirrored) ? 2 : 1;
      if(inmirrorcount == 0) ocwtV = cwtV;
      inmirrorcount += cmc;
      draw_grid();
      if(cw2.mirrored != cw.mirrored) V = V * Mirror;
      if(cw2.spin) V = V * spin(TAU*cw2.spin/cw2.at->type);
      cw2.spin = 0;
      dynamicval<shiftmatrix> dc(cwtV, cwtV);
      cwtV = V * inverse_shift(ggmatrix(cw2.at), cwtV);
      drawcell(cw2.at, V);
      inmirrorcount -= cmc;
      return;
      }                  
    
    if(!buggyGeneration && c->mpdist > 8 && !cheater && !autocheat) return; // not yet generated
    
    #if CAP_SHAPES
    ct6 = ctof(c);
    #endif
    fd = getfd(c);
    if(neon_mode == eNeon::illustration) fd = 0;
    error = false;
    
    setcolors();
    
    tune_colors();

    draw_cellstat();
    
    asciichar = winf[c->wall].glyph;
    asciicol = wcol;

    if(c->wall == waNone || isWatery(c)) asciicol = fcol;

    asciichar1 = asciichar;
    asciicol1 = asciicol;

    if(c->wall == waBoat) {
      asciicol1 = fcol;
      asciichar1 = '=';
      }    
    
    onradar = true;
    
    sha = get_spatial_info(c);
    
    if(c->wall == waMagma) fd = 0;
    
    poly_outline = OUTLINE_DEFAULT;

    Vd =
      WDIM == 3 ? V:
      !wmspatial ? V : 
      sha.sl ? orthogonal_move_fol(V, GDIM == 3 ? cgi.RED[sha.sl] - cgi.FLOOR : cgi.RED[sha.sl]) :
      (highwall(c) && GDIM == 2) ? orthogonal_move_fol(V, (1+cgi.WALL)/2) :
#if CAP_SHAPES
      (sha.top < SIDE::FLOOR) ? orthogonal_move_fol(V, GDIM == 3 ? cgi.WATERLEVEL - cgi.FLOOR : cgi.WATERLEVEL) :
#endif 
      V;
    
    Vboat = Vd;
      
    draw_shmup_monster();

    poly_outline = OUTLINE_DEFAULT;    

    draw_wall_full();
#endif    

    draw_item_full();
    draw_monster_full();
    
    if(animations[LAYER_THROW].count(c)) {
      shiftmatrix Vthrow = V;
      ld footphase;
      applyAnimation(c, Vthrow, footphase, LAYER_THROW);
      eItem it = animations[LAYER_THROW][c].thrown_item;
      if(it) drawItemType(it, c, Vthrow, iinf[it].color, 0, false);
      eMonster mo = animations[LAYER_THROW][c].thrown_monster;
      if(mo) drawMonsterType(mo, c, Vthrow, minf[mo].color, 0, minf[mo].color);
      }
    
#if CAP_TEXTURE    
    if(!texture::using_aura()) 
#endif 
      addaura();

    #if CAP_SHAPES
    add_map_effects();
    #endif
    
    #if CAP_QUEUE
    if(error && onradar) {
      int sl;
      string s = s0+asciichar;
      dynamicval<color_t> p(poly_outline, asciiborder << 8);
      if(!wmascii3)
        queuestrn(V, mapfontscale / 100, s, darkenedby(asciicol, darken), 2);
      else if(highwall(c) && conegraph(c)) {
        const int layers = 1 << detaillevel;
        string s1 = s0+asciichar1;
        poly_outline = bordcolor << 8;
        for(int z=0; z<layers; z++)
          queuestrn(orthogonal_move_fol(V, zgrad0(0, geom3::actual_wall_height(), z, layers)), 1. - z * .5 / layers, s1, darkenedby(gradient(bordcolor, asciicol1, -layers, z, layers), darken), 1);
        poly_outline = asciiborder << 8;
        queuestrn(orthogonal_move_fol(V, cgi.WALL), (asciicol == asciicol1 && asciichar == asciichar1 ? .5 : 1) * mapfontscale / 100, s, darkenedby(asciicol, darken), 2);
        }
      else if(highwall(c)) {
        const int layers = 1 << detaillevel;
        string s1 = s0+asciichar1;
        poly_outline = bordcolor << 8;
        for(int z=0; z<layers; z++)
          queuestrn(orthogonal_move_fol(V, zgrad0(0, geom3::actual_wall_height(), z, layers)), 1, s1, darkenedby(gradient(bordcolor, asciicol1, -layers, z, layers), darken), 1);
        poly_outline = asciiborder << 8;
        queuestrn(orthogonal_move_fol(V, cgi.WALL), mapfontscale / 100, s, darkenedby(asciicol, darken), 2);
        }
      else if((sl = snakelevel(c))) {
        string s1 = s0+asciichar1;
        poly_outline = bordcolor << 8;
        for(int z=0; z<sl*4; z++) if(z%4 == 0)
          queuestrn(orthogonal_move_fol(V, zgrad0(0, cgi.slev * sl, z, sl*4)), mapfontscale / 100, s1, darkenedby(gradient(bordcolor, asciicol1, -sl, z, sl*4), darken), 1);
        poly_outline = asciiborder << 8;
        queuestrn(orthogonal_move_fol(V, cgi.RED[sl]), mapfontscale / 100, s, darkenedby(asciicol, darken), 2);
        }
      else if(sha.top < SIDE::FLOOR) {
        string s1 = s0+asciichar1;
        poly_outline = bordcolor << 8;
        queuestrn(orthogonal_move_fol(V, cgi.DEEP), mapfontscale / 100, s1, darkenedby(gradient(bordcolor, asciicol1, 0, 0.3, 1), darken), 2);
        poly_outline = asciiborder << 8;
        queuestrn(V, mapfontscale / 100, s, darkenedby(asciicol, darken), 2);
        }
      else
        queuestrn(V, mapfontscale / 100, s, darkenedby(asciicol, darken), 2);
      }
    
    draw_grid();

    if(onradar && WDIM == 2 && GDIM == 3) addradar(V, asciichar, darkenedby(asciicol, darken), 0);
    
    if(WDIM == 2 && GDIM == 3) radar_grid();
    #endif
    
    check_rotations();

    #if CAP_EDIT
    if(!inHighQual) mapeditor::drawGhosts(c, V, c->type);
    #endif
    
#if CAP_MODEL
    netgen::buildVertexInfo(c, unshift(V));
#endif
    }
  }

void celldrawer::set_towerfloor(const cellfunction& cf) {
  if(weirdhyperbolic || sphere || (euc::in() && S7 == 4 && BITRUNCATED)) {
    set_floor(cgi.shFloor);
    return;
    }
  int j = -1;

  if(euclid) j = 10;
  else if(cf(c) > 1) { 
    int i = towerval(c, cf);
    if(i == 4) j = 0;
    if(i == 5) j = 1;
    if(i == 6) j = 2;
    if(i == 8) j = 3;
    if(i == 9) j = 4;
    if(i == 10) j = 5;
    if(i == 13) j = 6;
    if(PURE) {
      if(i == 7) j = 7;
      if(i == 11) j = 8;
      if(i == 15) j = 9;
      }
    }

  if(j >= 0)
    set_floor(applyDowndir(c, cf), cgi.shTower[j]);
  else if(c->wall != waLadder)
    set_floor(cgi.shMFloor);
  }

void celldrawer::set_zebrafloor() {

  if(hat::in() || kite::in()) { set_floor(cgi.shFloor); return; }

  if(euclid) { set_floor(cgi.shTower[10]); return; }
  if(weirdhyperbolic) {
    set_floor(cgi.shFloor); return;
    }
  
  auto si = patterns::getpatterninfo(c, patterns::PAT_ZEBRA, patterns::SPF_SYM0123);
  
  int j;
  if(PURE) j = 4;
  else if(si.id >=4 && si.id < 16) j = 2;
  else if(si.id >= 16 && si.id < 28) j = 1;
  else if(si.id >= 28 && si.id < 40) j = 3;
  else j = 0;

  set_floor(applyPatterndir(c, si), cgi.shZebra[j]);
  }

void celldrawer::set_maywarp_floor() {
  bool warp = isWarped(c);
  if(warp && use_warp_graphics()) {
    if(!stdhyperbolic) {
      set_floor(cgi.shTriheptaFloor);
      return;
      }
    auto si = patterns::getpatterninfo(c, patterns::PAT_TYPES, 0);
    if(si.id == 0 || si.id == 1)
      set_floor(cgi.shTriheptaFloor);
    else if(si.id >= 14)
      set_floor(cgi.shFloor);
    else
      set_floor(applyPatterndir(c, si), cgi.shTriheptaSpecial[si.id]);
    }
  else if(is_nice_dual(c)) set_floor(cgi.shBigTriangle);
  else set_floor(cgi.shFloor);
  }

void celldrawer::set_reptile_floor(const shiftmatrix& V, color_t col, bool nodetails) {

  auto si = 
    euc::in(2,6) ? 
      patterns::getpatterninfo(c, patterns::PAT_COLORING, patterns::SPF_CHANGEROT)
    :
      patterns::getpatterninfo(c, patterns::PAT_ZEBRA, patterns::SPF_SYM0123);
  
  int j;

  if(!wmescher) j = 4;
  else if(!BITRUNCATED) j = 0;
  else if(si.id < 4) j = 0;
  else if(si.id >=4 && si.id < 16) j = 1;
  else if(si.id >= 16 && si.id < 28) j = 2;
  else if(si.id >= 28 && si.id < 40) j = 3;
  else j = 4;
  
  if(euc::in(2,6)) j = 0;
  
  transmatrix D = applyPatterndir(c, si);
  if(euc::in(2,6) && GOLDBERG) D = ddspin(c, si.dir);
  if(wmescher && (stdhyperbolic || euc::in(2,6)))
    set_floor(D, cgi.shReptile[j][0]);
  else set_maywarp_floor();

  if(nodetails) return;

  int dcol = 0;

  int ecol = -1;

  if(isReptile(c->wall)) {
    unsigned char wp = c->wparam;
    if(wp == 1)        
      ecol = 0xFFFF00;
    else if(wp <= 5)
      ecol = 0xFF0000;
    else
      ecol = 0;
    if(ecol) ecol = gradient(0, ecol, -1, sintick(30), 1);
    }
  
  if(ecol == -1 || ecol == 0) dcol = darkena(col, 1, 0xFF);
  else dcol = darkena(ecol, 0, 0x80);

  dynamicval<color_t> p(poly_outline, 
    doHighlight() && ecol != -1 && ecol != 0 ? OUTLINE_ENEMY : OUTLINE_DEFAULT);

  if(sha.top != SIDE::INFDEEP) {
    if(wmescher)
      queuepoly(V*D, cgi.shReptile[j][1], dcol);
    else
      draw_floorshape(c, V, cgi.shMFloor, dcol);
    }
  
  if(ecol != -1) {
    queuepoly(V*D, cgi.shReptile[j][2], (ecol << 8) + 0xFF);
    queuepoly(V*D, cgi.shReptile[j][3], (ecol << 8) + 0xFF);
    }
  }

void celldrawer::set_emeraldfloor() {
  if(!euclid && BITRUNCATED && GDIM == 2) {
    auto si = patterns::getpatterninfo(c, patterns::PAT_EMERALD, patterns::SPF_SYM0123);
  
    int j = -1;

    if(si.id == 8) j = 0;
    else if(si.id == 12) j = 1;
    else if(si.id == 16) j = 2;
    else if(si.id == 20) j = 3;
    else if(si.id == 28) j = 4;
    else if(si.id == 36) j = 5;

    if(j >= 0) {
      set_floor(applyPatterndir(c, si), cgi.shEmeraldFloor[j]);
      return;
      }
    }
  
  set_floor(cgi.shCaveFloor);
  }

void celldrawer::shmup_gravity_floor() {
  if(GDIM == 2 && cellEdgeUnstable(c))
    set_floor(cgi.shMFloor);
  else
    set_floor(cgi.shFullFloor);
  }

void celldrawer::draw_reptile(color_t col) {
  auto qfib = qfi;
  set_reptile_floor(V, col, sha.top == SIDE::INFDEEP);
  draw_qfi(c, V, col);
  qfi = qfib;
  }      

#undef c

}
