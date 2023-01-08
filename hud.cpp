// Hyperbolic Rogue -- Heads-Up Display
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file hud.cpp
 *  \brief Heads-Up display: items collected, monsters killed, radar, etc.
 */

#include "hyper.h"
namespace hr {

EX purehookset hooks_stats;

EX int monsterclass(eMonster m) {
  if(isFriendly(m) || m == moTortoise) return 1;
  else if(isMonsterPart(m)) return 2;
  else return 0;
  }

EX int glyphclass(int i) {
  if(i < ittypes) {
    eItem it = eItem(i);
    return itemclass(it) == IC_TREASURE ? 0 : 1; 
    }
  else {
    eMonster m = eMonster(i-ittypes);
    return monsterclass(m) == 0 ? 2 : 3; 
    }
  }

EX int subclass(int i) {
  if(i < ittypes) 
    return itemclass(eItem(i)); 
  else 
    return monsterclass(eMonster(i-ittypes));
  }

#define GLYPH_MARKTODO   1
#define GLYPH_MARKOVER   2
#define GLYPH_LOCAL      4
#define GLYPH_IMPORTANT  8
#define GLYPH_NONUMBER   16
#define GLYPH_DEMON      32
#define GLYPH_RUNOUT     64
#define GLYPH_INPORTRAIT 128
#define GLYPH_LOCAL2     256
#define GLYPH_TARGET     512
#define GLYPH_INSQUARE   1024
#define GLYPH_INLANDSCAPE 2048

#if HDR
enum eGlyphsortorder {
  gsoFirstTop, gsoFirstBottom,
  gsoLastTop, gsoLastBottom,
  gsoLand, gsoValue,
  gsoMAX
  };
#endif

EX eGlyphsortorder glyphsortorder;
  
int zero = 0;

int& ikmerge(int i) {
  if(i < ittypes) return items[i];
  else if(i == ittypes) return zero;
  else return kills[i-ittypes];
  }

bool ikappear(int i) {
  if(i == itInventory && inv::on) return true;
  return ikmerge(i);
  }

const int glyphs = ittypes + motypes;

int gfirsttime[glyphs], glasttime[glyphs], gcopy[glyphs], ikland[glyphs];
int glyphorder[glyphs];
int glyphphase[glyphs];
int glyph_lastticks;

void updatesort() {
  for(int i=0; i<glyphs; i++) {
    int ik = ikmerge(i);
    if(ikappear(i) && gfirsttime[i] == 0)
      gfirsttime[i] = ticks;
    if(ik != gcopy[i])
      gcopy[i] = ik, glasttime[i] = ticks;
    int& gp = glyphphase[i];
    if(ticks <= glasttime[i]+500)
      gp += (ticks - glyph_lastticks);
    else if((gp % 500) && ((i >= ittypes) || i == itTerra)) {    
      int a = gp;
      gp += (ticks - glyph_lastticks);
      if(a/500 != gp/500)
        gp = gp/500*500;
      }
    }
  glyph_lastticks = ticks;
  }

EX void preparesort() {
  for(int i=0; i<glyphs; i++) glyphorder[i] = i;
  for(int i=0; i<isize(land_over); i++) {
    eLand l = land_over[i];
    ikland[treasureType(l)] = i+1;
    for(int mi=0; mi<motypes; mi++) 
      if(isNative(l, eMonster(mi)))
        ikland[mi+ittypes] = i+1;
    }
  glyphsortorder = gsoLand; updatesort();
  glyphsortorder = gsoFirstTop;
  }

int glyphsortkey = 0;

int glyphcorner(int i) {
  if(i < ittypes)
    return itemclass(eItem(i)) == IC_ORB ? 3 : 0;
  else
    return 1;
  }

bool glyphsort(int i, int j) {
  if(subclass(i) != subclass(j))
    return subclass(i) < subclass(j);
  if(glyphsortorder == gsoFirstTop)
    return gfirsttime[i] < gfirsttime[j];
  if(glyphsortorder == gsoFirstBottom)
    return gfirsttime[i] > gfirsttime[j];
  if(glyphsortorder == gsoLastTop)
    return glasttime[i] > glasttime[j];
  if(glyphsortorder == gsoLastBottom)
    return glasttime[i] < glasttime[j];
  if(glyphsortorder == gsoValue)
    return ikmerge(i) > ikmerge(j);
  if(glyphsortorder == gsoLand)
    return ikland[i] < ikland[j];
  return 0;
  }

int glyphflags(int gid) {
  int f = 0;
  if(gid < ittypes) {
    eItem i = eItem(gid);
    if(itemclass(i) == IC_NAI && i != itFatigue) f |= GLYPH_NONUMBER;
    if(isElementalShard(i)) {
      f |= GLYPH_LOCAL | GLYPH_INSQUARE;
      if(i == localshardof(cwt.at->land)) f |= GLYPH_LOCAL2;
      }
    if(i == treasureType(cwt.at->land) || daily::on) 
      f |= GLYPH_LOCAL | GLYPH_LOCAL2 | GLYPH_IMPORTANT | GLYPH_INSQUARE | GLYPH_INPORTRAIT | GLYPH_INLANDSCAPE;
    if(i == itHolyGrail) {
      if(items[i] >= 3 && !inv::on) f |= GLYPH_MARKOVER;
      }
    else if(itemclass(i) == IC_TREASURE) {
      if(items[i] >= 25 && items[i] < 100 && !inv::on) f |= GLYPH_MARKOVER;
      else if(items[i] < 10) f |= GLYPH_MARKTODO;
      }
    else {
      f |= GLYPH_IMPORTANT | GLYPH_INSQUARE | GLYPH_INPORTRAIT | GLYPH_INLANDSCAPE;
      if(itemclass(i) == IC_ORB && items[i] < 10) f |= GLYPH_RUNOUT;
      }
    if(i == orbToTarget) f |= GLYPH_TARGET;
    if(!less_in_portrait) f |= GLYPH_INPORTRAIT;
    if(!less_in_landscape) f |= GLYPH_INLANDSCAPE;
    }
  else {
    eMonster m = eMonster(gid-ittypes);
    if(m == moLesser) f |= GLYPH_IMPORTANT | GLYPH_DEMON | GLYPH_INPORTRAIT | GLYPH_INSQUARE | GLYPH_INLANDSCAPE;
    int isnat = isNative(cwt.at->land, m);
    if(isnat) f |= GLYPH_LOCAL | GLYPH_IMPORTANT | GLYPH_INPORTRAIT | GLYPH_INSQUARE | GLYPH_INLANDSCAPE;
    if(isnat == 2) f |= GLYPH_LOCAL2;
    if(m == monsterToSummon) f |= GLYPH_TARGET;
    if(!less_in_landscape) f |= GLYPH_INLANDSCAPE;
    }
  return f;
  }

EX bool graphglyph(bool isMonster) {
  // if(GDIM == 3) return false;
  if(vrhr::active()) return false;
  return vid.graphglyph == 2 || (vid.graphglyph == 1 && (isMonster ? mmmon : mmitem));
  }

bool displayglyph(int cx, int cy, int buttonsize, char glyph, color_t color, int qty, int flags, int id) {
    
  bool b =
    mousex >= cx && mousex < cx+buttonsize && mousey >= cy-buttonsize/2 && mousey <= cy-buttonsize/2+buttonsize;

  int glsize = buttonsize;
  if(glyph == '%' || glyph == 'M' || glyph == 'W') glsize = glsize*4/5;
  
  int d = ticks - glasttime[id];
  double zoom = (d <= 250 && d >= 0) ? 1.25 - .001 * d : 1;
  glsize = int(glsize * zoom);
  bool isMonster = (id >= ittypes);
  
  if(graphglyph(isMonster)) {
    initquickqueue();
    poly_outline = OUTLINE_DEFAULT;
    if(isMonster) {
      eMonster m = eMonster(id - ittypes);
      double bsize = buttonsize * 2/3;
      if(m == moKrakenH) bsize /= 3;
      if(m == moKrakenT || m == moDragonTail) bsize /= 2;
      if(m == moSlime) bsize = (2*bsize+1)/3;
      transmatrix V = atscreenpos(cx+buttonsize/2, cy, bsize*zoom);
      if(isWorm(m) && cgi.wormscale != 1) 
        for(int i=0; i<GDIM; i++)
          V[i][i] /= cgi.wormscale;
      int mcol = color;
      mcol -= (color & 0xFCFCFC) >> 2;
      drawMonsterType(m, NULL, shiftless(V), mcol, glyphphase[id]/500.0, NOCOLOR);
      }
    else {
      eItem it = eItem(id);
      double bsize = buttonsize / 2;
      if(glyph == '*') bsize *= 2;
      if(glyph == '%') bsize *= 2;
      if(glyph == '!') bsize *= 2;
      if(glyph == '?') bsize *= 2;
      if(glyph == ';') bsize *= 2;
      if(glyph == '$') bsize = (bsize*5+2)/3;
      if(glyph == 'o') bsize = (bsize*3+1)/2;
      if(glyph == 'c') bsize = (bsize*3+1)/2;
      if(glyph == 't') bsize = bsize*5/2;
      if(glyph == '(') bsize = bsize*2.5;
      if(glyph == '/') bsize = bsize*2;
      if(glyph == ']') bsize = bsize*1.5;
      if(glyph == 'x') bsize = bsize*1.5;
      if(it == itWarning) bsize *= 2;
      if(it == itBombEgg || it == itTrollEgg || it == itDodeca) bsize = bsize*3/2;
      int icol = color;
      icol -= (color & 0xFCFCFC) >> 2;
      int ic = itemclass(it);
      bsize = bsize * zoom;
      transmatrix V = atscreenpos(cx+buttonsize/2, cy, bsize);
      double t =
        (ic == IC_ORB || ic == IC_NAI) ? ticks*2 : 
        ((glyph == 't' && qty%5) || it == itOrbYendor) ? ticks/2 : 
        it == itTerra ? glyphphase[id] * 3 * M_PI + 900 * M_PI:
        glyphphase[id] * 2;

      drawItemType(it, NULL, shiftless(V), icol, t, false);
      
      int c1 = max(color_diff(color, backcolor), color_diff(color, bordcolor));
      if(c1 < 0x80) {
        color = orb_auxiliary_color(it);
        }
      }
    int c1 = max(color_diff(color, backcolor), color_diff(color, bordcolor));
    if(c1 < 0x80)
      color = gradient(color, forecolor, 0, .4, 1);
    sortquickqueue();
    quickqueue();
    }
  else if(glyph == '*')
    displaychr(cx + buttonsize/2, cy+buttonsize/4, 0, glsize*3/2, glyph, darkenedby(color, b?0:1));
  else
    displaychr(cx + buttonsize/2, cy, 0, glsize, glyph, darkenedby(color, b?0:1));
  
  string fl = "";
  string str = its(qty);

  if(flags & GLYPH_TARGET) fl += "!";
  if(flags & GLYPH_LOCAL2) fl += "+";
  else if(flags & GLYPH_LOCAL) fl += "-";
  if(flags & GLYPH_DEMON) fl += "X";
  if(flags & GLYPH_MARKOVER) str += "!";

  if(fl != "") 
    displaystr(cx + buttonsize, cy-buttonsize/2 + buttonsize/4, 0, buttonsize/2, fl, darkenedby(color, 0), 16);

  if(flags & GLYPH_NONUMBER) str = "";
  
  int bsize = 
    (qty < 10 && (flags & (GLYPH_MARKTODO | GLYPH_RUNOUT))) ? buttonsize*3/4 :
    qty < 100 ? buttonsize / 2 :
    buttonsize / 3;

  if(id == moMutant + ittypes && clearing::imputed.nonzero()) {
    bignum bn = clearing::imputed + qty;
    str = short_form(bn);
    bsize = buttonsize / 4;
    }

  if(str != "") {
    if(textwidth(bsize, str) < buttonsize)
      displayfr(cx + buttonsize, cy + buttonsize/2 - bsize/2, 1, bsize, str, color, 16);
    else
      displayfr(cx, cy + buttonsize/2 - bsize/2, 1, bsize, str, color, 0);
    }

  return b;
  }

void displayglyph2(int cx, int cy, int buttonsize, int i) {
      
  char glyph = i < ittypes ? iinf[i].glyph : minf[i-ittypes].glyph;
  color_t color = i < ittypes ? iinf[i].color : minf[i-ittypes].color;
  int imp = glyphflags(i);

  if(displayglyph(cx, cy, buttonsize, glyph, color, ikmerge(i), imp, i)) {
    instat = true;
    getcstat = SDLK_F1;
    if(i < ittypes) {
      eItem it = eItem(i);
      int t = itemclass(it);
      if(t == IC_TREASURE)
        mouseovers = XLAT("treasure collected: %1", it);
      if(t == IC_OTHER)
        mouseovers = XLAT("objects found: %1", it);
      if(t == IC_NAI)
        mouseovers = XLATT1(it);
      if(t == IC_ORB)
        mouseovers = XLAT("orb power: %1", eItem(i));
      if(it == itGreenStone) {
        mouseovers += XLAT(" (click to drop)");
        getcstat = 'g';
        }
      if(it == itStrongWind) {
        mouseovers += XLAT(" (click to use)");
        getcstat = 't';
        }
      if(it == itInventory) {
        mouseovers += XLAT(" (click to use)");
        getcstat = 'i';
        }
      if(imp & GLYPH_LOCAL) mouseovers += XLAT(" (local treasure)");
      help = generateHelpForItem(it);
      }
    else {
      eMonster m = eMonster(i-ittypes);
      if(isMonsterPart(m))
        mouseovers = s0 + XLAT("parts destroyed: %1", m);
      else if(isFriendly(m) && isNonliving(m))
        mouseovers = s0 + XLAT("friends destroyed: %1", m);
      else if(isFriendly(m))
        mouseovers = s0 + XLAT("friends killed: %1", m);
      else if(isNonliving(m))
        mouseovers = s0 + XLAT("monsters destroyed: %1", m);
      else if(m == moTortoise)
        mouseovers = s0 + XLAT("animals killed: %1", m);
      else 
        mouseovers = s0 + XLAT("monsters killed: %1", m);
      if(imp & GLYPH_LOCAL2) mouseovers += XLAT(" (killing increases treasure spawn)");
      else if(imp & GLYPH_LOCAL) mouseovers += XLAT(" (appears here)");
      help = generateHelpForMonster(m);
      }
    }
  }

EX bool nohud, nomenukey, nomsg;

EX hookset<bool()> hooks_prestats;

#if CAP_SHAPES
void drawMobileArrow(int i) {

  int dir = i;
  cell *c = cwt.at->move(i);
  if(!c) return;

  transmatrix T = currentmap->adj(cwt.at, i);

  // color_t col = getcs().uicolor;
  // col -= (col & 0xFF) >> 1;
  
  bool invalid = !legalmoves[dir];
  
  color_t col = cellcolor(c);
  if(col == OUTLINE_NONE) col = 0xC0C0C0FF;
  col -= (col & 0xFF) >> 1;
  if(invalid) col -= (col & 0xFF) >> 1;
  if(invalid) col -= (col & 0xFF) >> 1;
  
  poly_outline = OUTLINE_DEFAULT;
  // transmatrix m2 = Id;
  ld scale = vid.mobilecompasssize * (sphere ? 7 : euclid ? 6 : 5);
  // m2[0][0] = scale; m2[1][1] = scale; m2[2][2] = 1;

  shiftmatrix U = ggmatrix(cwt.at);
  shiftpoint H = tC0(U);
  shiftmatrix Centered = rgpushxto0(H);

  hyperpoint P = inverse_shift(Centered, U * T * C0);
  double alpha = atan2(P[1], P[0]);

  using namespace shmupballs;
  
  double dx = xmove + rad*(1+SKIPFAC-.2)/2 * cos(alpha);
  double dy = yb + rad*(1+SKIPFAC-.2)/2 * sin(alpha);
  
  queuepolyat(shiftless(atscreenpos(dx, dy, scale) * spin(-alpha)), cgi.shArrow, col, PPR::MOBILE_ARROW);
  }
#endif

EX bool nofps = false;

EX color_t crosshair_color = 0xFFFFFFC0;
EX ld crosshair_size = 0;

EX bool long_kills;

/** HUD margin in pixels. In mobile devices we need this margin because the corners are hidden. side==0: top, side==1: bottom */
EX int hud_margin(int side) {
  if(ISIOS) return 24;
  if(ISANDROID) return 8;
  if(ISFAKEMOBILE) return 8;
  return 0;
  }

EX void draw_crosshair() {
  auto& cd = current_display;
  auto xc = cd->xcenter;
  auto yc = cd->ycenter;
  
  flat_model_enabler fme;

  if(crosshair_color && crosshair_size > 0) {
    initquickqueue();
    vid.linewidth = 1;
    queueline(shiftless(tC0(atscreenpos(xc - crosshair_size, yc, 1))), shiftless(tC0(atscreenpos(xc + crosshair_size, yc, 1))), crosshair_color).prio = PPR::SUPERLINE;
    queueline(shiftless(tC0(atscreenpos(xc, yc - crosshair_size, 1))), shiftless(tC0(atscreenpos(xc, yc + crosshair_size, 1))), crosshair_color).prio = PPR::SUPERLINE;
    quickqueue();
    }
  return;
  }

EX bool less_in_portrait, less_in_landscape;
  
EX void drawStats() {
  if(vid.stereo_mode == sLR) return;
  draw_crosshair();
  if(nohud) return;
  if(callhandlers(false, hooks_prestats)) return;
  if(viewdists && show_distance_lists) 
    get_expansion().view_distances_dialog();
  if(current_display->sidescreen) return;
  
  first_cell_to_draw = true;
  bool h = hide_player();
  instat = false;

  bool cornermode = (vid.xres > vid.yres * 85/100 && vid.yres > vid.xres * 85/100);
  
  #if MAXMDIM >= 4
  if(geometry == gRotSpace || geometry == gProduct) rots::draw_underlying(!cornermode);
  #endif
  
  {
  
  if(vid.radarsize > 0 && h)
  #if CAP_RACING
    if(!racing::on)
  #endif
    if(!peace::on)
    if(!(cmode & sm::MISSION))
      draw_radar(cornermode);

  flat_model_enabler fme;

  if(haveMobileCompass()) {
    initquickqueue();
    using namespace shmupballs;
    calc();
    #if CAP_QUEUE
    queuecircle(xmove, yb, rad, 0xFF0000FF);
    queuecircle(xmove, yb, rad*SKIPFAC, 
      legalmoves[cwt.at->type] ? 0xFF0000FF : 0xFF000080
      );
    #endif
    #if CAP_SHAPES
    for(int i=0; i<cwt.at->type; i++) drawMobileArrow(i);
    #endif
    if(hypot(mousex-xmove, mousey-yb) <= rad) getcstat = '-';
    quickqueue();
    }
  
  if(racing::on) 
#if CAP_RACING
    racing::drawStats();
#else
    {}
#endif
  else if(cornermode) {
    int bycorner[4];
    for(int u=0; u<4; u++) bycorner[u] = 0;
    for(int i=0; i<glyphs; i++) if(ikappear(i) && (glyphflags(i) & GLYPH_INSQUARE))
      bycorner[glyphcorner(i)]++;
    updatesort();
    stable_sort(glyphorder, glyphorder+glyphs, glyphsort);
    int rad = min(vid.xres, vid.yres) / 2;
    for(int cor=0; cor<4; cor++) {
      for(int a=5; a<41; a++) {
        int s = min(vid.xres, vid.yres) / a;
        int spots = 0;
        for(int u=vid.fsize; u<vid.xres/2-s; u += s)
        for(int v=vid.fsize; v<vid.yres/2-s; v += s)
          if(hypot(vid.xres/2-u-s, (vid.yres/2-v-s) / pconf.stretch) > rad) {
            spots++;
            }
        if(spots >= bycorner[cor] && spots >= 3) {
          int next = 0;
          vector<int> glyphstoshow;
          for(int i=0; i<glyphs; i++) {
            int g = glyphorder[i];
            if(ikappear(g) && (glyphflags(g) & GLYPH_INSQUARE) && glyphcorner(g) == cor)
              glyphstoshow.push_back(g);
            }
          for(int u=vid.fsize; u<vid.xres/2-s; u += s)
          for(int v=vid.fsize; v<vid.yres/2-s; v += s)
            if(hypot(vid.xres/2-u-s, (vid.yres/2-v-s) / pconf.stretch) > rad) {
              if(next >= isize(glyphstoshow)) break;

              int cx = u;
              int cy = v + s/2;
              if(cor&1) cx = vid.xres-1-s-cx;
              if(cor&2) cy = vid.yres-1-cy;
    
              displayglyph2(cx, cy, s, glyphstoshow[next++]);
              }
          break;
          }
        }
      }
    }
  
  else {
  
    bool portrait = vid.xres < vid.yres;
    int colspace = portrait ? (current_display->ycenter - current_display->scrsize - 3 * vid.fsize) : (vid.xres - vid.yres - 16) / 2;
    int rowspace = portrait ? vid.xres - 16 : vid.yres - vid.fsize * (vid.msgleft ? 9 : 4);
    int colid[4], rowid[4];
    int maxbyclass[4];
    for(int z=0; z<4; z++) maxbyclass[z] = 0;

    flagtype flag = portrait ? GLYPH_INPORTRAIT : GLYPH_INLANDSCAPE;

    for(int i=0; i<glyphs; i++) if(ikappear(i))
      if(glyphflags(i) & flag)
        maxbyclass[glyphclass(i)]++;
    int buttonsize;
    int columns, rows;
    bool imponly = false;
    int minsize = vid.fsize * (portrait ? 3 : 2);
    rows = 0;
    while((buttonsize = minsize - vid.killreduction)) {
      columns = colspace / buttonsize;
      rows = rowspace / buttonsize; if(!rows) return;
      int coltaken = 0;
      for(int z=0; z<4; z++) {
        if(z == 2 && !portrait) {
          if(coltaken > columns) { vid.killreduction++; continue; }
          coltaken = 0;
          }
        colid[z] = coltaken, rowid[z] = 0,
        coltaken += (maxbyclass[z] + rows-1) / rows;
        }
      if(coltaken > columns) { vid.killreduction++; continue; }
      break;
      }
  
    if(buttonsize <= vid.fsize*3/4) {
      imponly = true; buttonsize = minsize;
      rows = rowspace / buttonsize; if(!rows) return;
      colid[0] = 0; colid[2] = portrait ? 1 : 0;
      }  
    
    updatesort();
    stable_sort(glyphorder, glyphorder+glyphs, glyphsort);
    
    for(int i0=0; i0<glyphs; i0++) {
      int i = glyphorder[i0];
      if(!ikappear(i)) continue;
      int z = glyphclass(i);
      int imp = glyphflags(i);
      if(!(imp & flag)) continue;
      if(imponly) { z &=~1; if(!(imp & GLYPH_IMPORTANT)) continue; }
  
      int cx, cy;
      if(portrait)
        cx = 8 + buttonsize * rowid[z], cy = vid.fsize*2 + buttonsize * (colid[z]) + buttonsize/2 + hud_margin(0);
      else
        cx = 8 + buttonsize * (colid[z]), cy = vid.fsize * 3 + buttonsize * rowid[z] + hud_margin(0);
      
      if(!portrait && z < 2) cx = vid.xres - cx - buttonsize;
  
      rowid[z]++; if(rowid[z] >= rows) rowid[z] = 0, colid[z]++;
      
      displayglyph2(cx, cy, buttonsize, i);    
      }
    }
  }
  glflush();
  calcparam();

  int top_y = vid.fsize + hud_margin(0);
  
  string s0;
  if(racing::on) {
    #if CAP_RACING
    using namespace racing;
    color_t col;
    if(ticks >= race_start_tick)
      col = 0x00FF00;
    else if(ticks >= race_start_tick - 2000)
      col = 0xFFFF00;
    else
      col = 0xFF0000;
    for(int i=0; i<multi::players; i++) if(race_finish_tick[i])
      col = 0xFFFFFF;
    
    dynamicval<int> x(vid.fsize, vid.fsize*2);
    if(displayButtonS(vid.xres - 8, top_y, racetimeformat(ticks - race_start_tick), col, 16, vid.fsize)) getcstat = 'o';

    for(int i=0; i<multi::players; i++) {
      if(race_finish_tick[i]) {
        multi::cpid = i;
        if(displayButtonS(vid.xres - 8, top_y + vid.fsize * (2+2*i), racetimeformat(race_finish_tick[i] - race_start_tick), (getcs().uicolor >> 8), 16, vid.fsize))
          getcstat = 'o';
        }
      else {
        int comp = get_percentage(i);
        if(displayButtonS(vid.xres - 8, top_y + vid.fsize * (2+2*i), its(comp) + "%", (getcs().uicolor >> 8), 16, vid.fsize))
          getcstat = 'o';
        }
      if(displayButtonS(vid.xres - 8, top_y + vid.fsize * (3+2*i), fts_fixed(shmup::pc[i]->vel * SCALE * 1000/600, 2), (getcs().uicolor >> 8), 16, vid.fsize))
        getcstat = 'o';
      }
    #endif
    }
  else if(!peace::on) {
    string scoreline = XLAT("score: %1", its(gold()));
    if(displayButtonS(vid.xres - 8, top_y, scoreline, forecolor, 16, vid.fsize)) {
      mouseovers = XLAT("Your total wealth"),
      instat = true,
      getcstat = SDLK_F1,
      help = helptitle(XLAT("Your total wealth"), 0xFFD500) + 
      XLAT(
        "The total value of the treasure you have collected.\n\n"
        "Every world type contains a specific type of treasure, worth 1 $$$; "
        "your goal is to collect as much treasure as possible, but every treasure you find "
        "causes more enemies to hunt you in its native land.\n\n"
        "Orbs of Yendor are worth 50 $$$ each.\n\n"
        );
      }
    string s = XLAT("kills: %1", its(tkills()));
    long_kills = false;
    int siz = vid.fsize;
    if(cwt.at->land == laClearing && clearing::imputed.approx_ld() >= 100000) {
      long_kills = true;
      s = XLAT("leaves cut: %1", (bignum(kills[moMutant]) + clearing::imputed).get_str(200));
      if(mouseovers == standard_help()) mouseovers = " ";
      while(siz > 4 && textwidth(siz, s) > vid.xres - textwidth(vid.fsize, scoreline)) siz--;
      }
    
    if(displayButtonS(8, top_y, s, forecolor, 0, siz)) {
      instat = true;
      getcstat = SDLK_F1;
      if(long_kills) { mouseovers = " "; help = generateHelpForMonster(moMutant); }
      else {
        mouseovers = XLAT("Your total kills")+": " + its(tkills()),
        help = helptitle(XLAT("Your total kills") + ": " + its(tkills()), 0x404040) + 
          XLAT(
          "In most lands, more treasures are generated with each enemy native to this land you kill. "
          "Moreover, 100 kills is a requirement to enter the Graveyard and the Hive.\n\n"
          "Friendly creatures and parts of monsters (such as the Ivy) do appear in the list, "
          "but are not counted in the total kill count.");
        }
      }
    }
  string vers = VER;
  if(true) {
    if(casual) vers += " casual";
    if(autocheat) vers += " god";
    else if(cheater) vers += " cheat";
    if(yendor::on) vers += " Yendor";
    if(tactic::on) vers += " PTM";
    if(inv::on) vers += " inv";
    if(tour::on) vers += " tour";
    if(shmup::on) vers += " shmup";
    if(multi::players > 1) vers += " P" + its(multi::players);
    if(pureHardcore()) vers += " hardcore";
    else if(hardcore) vers += " partial hardcore";
    if(peace::on) vers += " peace";
    if(racing::on) vers += " racing";
    if(daily::on) vers += " strange";
    if(land_structure != default_land_structure())
      vers += land_structure_name(true);
    if(princess::challenge) vers += " Princess";
    if(randomPatternsMode) vers += " RPM";
    
    if(geometry != gNormal || !BITRUNCATED) 
      vers = vers + " " + full_geometry_name();
    }
  if(!nofps) vers += XLAT(" fps: ") + its(calcfps());
  
  #if CAP_MEMORY_RESERVE
  if(reserve_limit && reserve_count < reserve_limit) {
    vers += " " + its(reserve_count) + "/" + its(reserve_limit) + " MB";
    if(displayButtonS(4, vid.yres - 4 - vid.fsize/2 - hud_margin(1), vers, 0xFF2020, 0, vid.fsize/2)) 
      getcstat = PSEUDOKEY_MEMORY, instat = true;
    }
  else 
  #endif
  if(displayButtonS(4, vid.yres - 4 - vid.fsize/2 - hud_margin(1), vers, 0x202020, 0, vid.fsize/2)) {
    mouseovers = XLAT("frames per second"),
    getcstat = SDLK_F1,
    instat = true,
    help = 
      helptitle(XLAT("frames per second"), 0xFF4040) +
      XLAT(
      "The higher the number, the smoother the animations in the game. "
      "If you find that animations are not smooth enough, you can try "
      "to change the options "
      ) +
#if ISIOS
XLAT(
      "(in the MENU). You can reduce the sight range, this should make "
      "the animations smoother.");
#else
XLAT(
      "(press v) and change the wall/monster mode to ASCII, or change "
      "the resolution.");
#endif
    }

  glflush();
  achievement_display();

  callhooks(hooks_stats);
  }

}
