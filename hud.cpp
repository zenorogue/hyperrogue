// Hyperbolic Rogue -- heads-up display
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

vector<radarpoint> radarpoints;

purehookset hooks_stats;

int monsterclass(eMonster m) {
  if(isFriendly(m) || m == moTortoise) return 1;
  else if(isMonsterPart(m)) return 2;
  else return 0;
  }

int glyphclass(int i) {
  if(i < ittypes) {
    eItem it = eItem(i);
    return itemclass(it) == IC_TREASURE ? 0 : 1; 
    }
  else {
    eMonster m = eMonster(i-ittypes);
    return monsterclass(m) == 0 ? 2 : 3; 
    }
  }

int subclass(int i) {
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

eGlyphsortorder glyphsortorder;
  
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

void preparesort() {
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
    if(itemclass(i) == IC_NAI) f |= GLYPH_NONUMBER;
    if(isElementalShard(i)) {
      f |= GLYPH_LOCAL | GLYPH_INSQUARE;
      if(i == localshardof(cwt.at->land)) f |= GLYPH_LOCAL2;
      }
    if(i == treasureType(cwt.at->land) || daily::on) 
      f |= GLYPH_LOCAL | GLYPH_LOCAL2 | GLYPH_IMPORTANT | GLYPH_INSQUARE;
    if(i == itHolyGrail) {
      if(items[i] >= 3 && !inv::on) f |= GLYPH_MARKOVER;
      }
    else if(itemclass(i) == IC_TREASURE) {
      if(items[i] >= 25 && items[i] < 100 && !inv::on) f |= GLYPH_MARKOVER;
      else if(items[i] < 10) f |= GLYPH_MARKTODO;
      }
    else {
      f |= GLYPH_IMPORTANT | GLYPH_INSQUARE;
      if(itemclass(i) == IC_ORB && items[i] < 10) f |= GLYPH_RUNOUT;
      }
    if(i == orbToTarget) f |= GLYPH_TARGET;
    f |= GLYPH_INPORTRAIT;
    }
  else {
    eMonster m = eMonster(gid-ittypes);
    if(m == moLesser) f |= GLYPH_IMPORTANT | GLYPH_DEMON | GLYPH_INPORTRAIT | GLYPH_INSQUARE;
    int isnat = isNative(cwt.at->land, m);
    if(isnat) f |= GLYPH_LOCAL | GLYPH_IMPORTANT | GLYPH_INPORTRAIT | GLYPH_INSQUARE;
    if(isnat == 2) f |= GLYPH_LOCAL2;
    if(m == monsterToSummon) f |= GLYPH_TARGET;
    }
  return f;
  }

bool graphglyph() {
  // if(DIM == 3) return false;
  return vid.graphglyph == 2 || (vid.graphglyph == 1 && vid.monmode);
  }

bool displayglyph(int cx, int cy, int buttonsize, char glyph, color_t color, int qty, int flags, int id) {
    
  bool b =
    mousex >= cx && mousex < cx+buttonsize && mousey >= cy-buttonsize/2 && mousey <= cy-buttonsize/2+buttonsize;

  int glsize = buttonsize;
  if(glyph == '%' || glyph == 'M' || glyph == 'W') glsize = glsize*4/5;
  
  int d = ticks - glasttime[id];
  double zoom = (d <= 250 && d >= 0) ? 1.25 - .001 * d : 1;
  glsize = int(glsize * zoom);
  
  if(graphglyph()) {
    initquickqueue();
    if(id >= ittypes) {
      eMonster m = eMonster(id - ittypes);
      double bsize = buttonsize * 2/3;
      if(m == moKrakenH) bsize /= 3;
      if(m == moKrakenT || m == moDragonTail) bsize /= 2;
      if(m == moSlime) bsize = (2*bsize+1)/3;
      transmatrix V = atscreenpos(cx+buttonsize/2, cy, bsize*zoom);
      if(isWorm(m) && cgi.wormscale != 1) 
        for(int i=0; i<DIM; i++)
          V[i][i] /= cgi.wormscale;
      int mcol = color;
      mcol -= (color & 0xFCFCFC) >> 2;
      drawMonsterType(m, NULL, V, mcol, glyphphase[id]/500.0);
      }
    else {
      eItem it = eItem(id);
      double bsize = buttonsize / 2;
      if(glyph =='*') bsize *= 2;
      if(glyph == '$') bsize = (bsize*5+2)/3;
      if(glyph == 'o') bsize = (bsize*3+1)/2;
      if(glyph == 't') bsize = bsize*5/2;
      if(glyph == '(') bsize = bsize*2.5;
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
      drawItemType(it, NULL, V, icol, t, false);
      }
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
  if(str != "")
    displayfr(cx + buttonsize, cy + buttonsize/2 - bsize/2, 1, bsize, str, color, 16);

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

bool nohud, nomenukey;

hookset<bool()> *hooks_prestats;

#if CAP_SHAPES
void drawMobileArrow(int i) {

  int dir = i;
  cell *c = cwt.at->move(i);
  if(!c) return;

  transmatrix T;
  if(!compute_relamatrix(c, cwt.at, i, T)) return;

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

  transmatrix U = ggmatrix(cwt.at);
  hyperpoint H = sphereflip * tC0(U);
  transmatrix Centered = sphereflip * rgpushxto0(H);

  hyperpoint P = inverse(Centered) * U * T * C0;
  double alpha = atan2(P[1], P[0]);

  using namespace shmupballs;
  
  double dx = xmove + rad*(1+SKIPFAC-.2)/2 * cos(alpha);
  double dy = yb + rad*(1+SKIPFAC-.2)/2 * sin(alpha);
  
  queuepolyat(atscreenpos(dx, dy, scale) * spin(-alpha), cgi.shArrow, col, PPR::MOBILE_ARROW);
  }
#endif

bool nofps = false;
extern ld max_eu_dist;

void draw_radar(bool cornermode) {

  bool d3 = WDIM == 3;
  bool hyp = hyperbolic;
  bool sph = sphere;

  dynamicval<eGeometry> g(geometry, gEuclid);
  dynamicval<eModel> pm(pmodel, mdUnchanged);
  dynamicval<bool> ga(geom3::always3, false);
  initquickqueue();
  int rad = vid.radarsize;
  
  ld cx = cornermode ? rad+2 : vid.xres-rad-2;
  ld cy = vid.yres-rad-2 - vid.fsize;

  for(int i=0; i<360; i++)
    curvepoint(atscreenpos(cx-cos(i * degree)*rad, cy-sin(i*degree)*rad, 1) * C0);
  queuecurve(0xFFFFFFFF, 0x000000FF, PPR::ZERO);      

  ld alpha = 15 * degree;
  ld co = cos(alpha);
  ld si = sin(alpha);
  
  if(sph && !d3) {
    for(int i=0; i<360; i++)
      curvepoint(atscreenpos(cx-cos(i * degree)*rad, cy-sin(i*degree)*rad*si, 1) * C0);
    queuecurve(0, 0x200000FF, PPR::ZERO);
    }

  if(d3) {
    for(int i=0; i<360; i++)
      curvepoint(atscreenpos(cx-cos(i * degree)*rad, cy-sin(i*degree)*rad*si, 1) * C0);
    queuecurve(0xFF0000FF, 0x200000FF, PPR::ZERO);
  
    curvepoint(atscreenpos(cx-sin(vid.fov*degree/2)*rad, cy-sin(vid.fov*degree/2)*rad*si, 1) * C0);
    curvepoint(atscreenpos(cx, cy, 1) * C0);
    curvepoint(atscreenpos(cx+sin(vid.fov*degree/2)*rad, cy-sin(vid.fov*degree/2)*rad*si, 1) * C0);
    queuecurve(0xFF8000FF, 0, PPR::ZERO);
    }
  
  if(d3) for(auto& r: radarpoints) {    
    queueline(atscreenpos(cx+rad * r.h[0], cy - rad * r.h[2] * si + rad * r.h[1] * co, 0)*C0, atscreenpos(cx+rad*r.h[0], cy - rad*r.h[2] * si, 0)*C0, r.line, -1);
    }

  quickqueue();
  glflush();

  for(auto& r: radarpoints)
    if(d3) displaychr(int(cx + rad * r.h[0]), int(cy - rad * r.h[2] * si + rad * r.h[1] * co), 0, 8, r.glyph, r.color);
    else if(sph) displaychr(int(cx + (rad-10) * r.h[0]), int(cy + (rad-10) * r.h[2] * si + (rad-10) * r.h[1] * co), 0, +r.h[1] * si > r.h[2] * co ? 8 : 16, r.glyph, r.color);
    else if(hyp) {
      int siz = 1/(1+r.h[3]) * cgi.scalefactor * current_display->radius / (inHighQual ? 10 : 6);
      displaychr(int(cx + rad * r.h[0]), int(cy + rad * r.h[1]), 0, siz, r.glyph, r.color);
      }
    else {
      displaychr(int(cx + rad * r.h[0]), int(cy + rad * r.h[1]), 0, rad * cgi.scalefactor / (max_eu_dist + cgi.scalefactor/4) * 0.8, r.glyph, r.color);
      }    
  }

void drawStats() {
  if(nohud || vid.stereo_mode == sLR) return;
  if(callhandlers(false, hooks_prestats)) return;
  if(viewdists && show_distance_lists) 
    expansion.view_distances_dialog();
  if(current_display->sidescreen) return;
  
  first_cell_to_draw = true;
  bool h = hide_player();

  {
  dynamicval<eModel> pm(pmodel, DIM == 3 ? mdFlatten : mdDisk);
  // dynamicval<videopar> v(vid, vid);
  // vid.alpha = vid.scale = 1;
  dynamicval<ld> va(vid.alpha, 1);
  dynamicval<ld> vs(vid.scale, 1);
  dynamicval<ld> vc(vid.camera_angle, 0);

  calcparam();
  
  bool cornermode = (vid.xres > vid.yres * 85/100 && vid.yres > vid.xres * 85/100);
  
  if(vid.radarsize > 0 && h)
  #if CAP_RACING
    if(!racing::on && !peace::on)
  #endif
    draw_radar(cornermode);

  if(haveMobileCompass()) {
    initquickqueue();
    using namespace shmupballs;
    calc();
    #if CAP_QUEUE
    queuecircle(xmove, yb, rad, 0xFF0000FF);
    queuecircle(xmove, yb, rad*SKIPFAC, 
      legalmoves[MAX_EDGE] ? 0xFF0000FF : 0xFF000080
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
          if(hypot(vid.xres/2-u-s, (vid.yres/2-v-s) / vid.stretch) > rad) {
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
            if(hypot(vid.xres/2-u-s, (vid.yres/2-v-s) / vid.stretch) > rad) {
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
  
    instat = false;
    bool portrait = vid.xres < vid.yres;
    int colspace = portrait ? (vid.yres - vid.xres - vid.fsize*3) : (vid.xres - vid.yres - 16) / 2;
    int rowspace = portrait ? vid.xres - 16 : vid.yres - vid.fsize * (vid.msgleft ? 9 : 4);
    int colid[4], rowid[4];
    int maxbyclass[4];
    for(int z=0; z<4; z++) maxbyclass[z] = 0;
    for(int i=0; i<glyphs; i++) if(ikappear(i))
      if(!portrait || (glyphflags(i) | GLYPH_INPORTRAIT))
        maxbyclass[glyphclass(i)]++;
    int buttonsize;
    int columns, rows;
    bool imponly = false;
    int minsize = vid.fsize * (portrait ? 4 : 2);  
    rows = 0;
    while((buttonsize = minsize - vid.killreduction)) {
      columns = colspace / buttonsize;
      rows = rowspace / buttonsize;
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
      if(imponly) { z &=~1; if(!(imp & GLYPH_IMPORTANT)) continue; }
  
      int cx, cy;
      if(portrait)
        cx = 8 + buttonsize * rowid[z], cy = vid.fsize*2 + buttonsize * (colid[z]) + buttonsize/2;
      else
        cx = 8 + buttonsize * (colid[z]), cy = vid.fsize * 3 + buttonsize * rowid[z];
      
      if(!portrait && z < 2) cx = vid.xres - cx - buttonsize;
  
      rowid[z]++; if(rowid[z] >= rows) rowid[z] = 0, colid[z]++;
      
      displayglyph2(cx, cy, buttonsize, i);    
      }
    }
  }
  glflush();
  calcparam();
  
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
    if(displayButtonS(vid.xres - 8, vid.fsize, racetimeformat(ticks - race_start_tick), col, 16, vid.fsize)) getcstat = 'o';

    for(int i=0; i<multi::players; i++) {
      if(race_finish_tick[i]) {
        multi::cpid = i;
        if(displayButtonS(vid.xres - 8, vid.fsize * (3+i), racetimeformat(race_finish_tick[i] - race_start_tick), (getcs().uicolor >> 8), 16, vid.fsize))
          getcstat = 'o';
        }
      else {
        int comp = get_percentage(i);
        if(displayButtonS(vid.xres - 8, vid.fsize * (3+i), its(comp) + "%", (getcs().uicolor >> 8), 16, vid.fsize))
          getcstat = 'o';
        }
      }
    #endif
    }
  else if(!peace::on) {
    if(displayButtonS(vid.xres - 8, vid.fsize, XLAT("score: %1", its(gold())), forecolor, 16, vid.fsize)) {
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
    if(displayButtonS(8, vid.fsize, XLAT("kills: %1", its(tkills())), forecolor, 0, vid.fsize)) {
      instat = true,
      getcstat = SDLK_F1,
      mouseovers = XLAT("Your total kills")+": " + its(tkills()),
      help = helptitle(XLAT("Your total kills") + ": " + its(tkills()), 0x404040) + 
        XLAT(
        "In most lands, more treasures are generated with each enemy native to this land you kill. "
        "Moreover, 100 kills is a requirement to enter the Graveyard and the Hive.\n\n"
        "Friendly creatures and parts of monsters (such as the Ivy) do appear in the list, "
        "but are not counted in the total kill count.");
      }
    }
  string vers = VER;
  if(!nofps) vers += XLAT(" fps: ") + its(calcfps());
  if(displayButtonS(4, vid.yres - 4 - vid.fsize/2, vers, 0x202020, 0, vid.fsize/2)) {
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
