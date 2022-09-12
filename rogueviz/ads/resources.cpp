namespace hr {

namespace ads_game {

void init_rsrc() {
  max_pdata.hitpoints = 3;
  max_pdata.score = 0;
  max_pdata.ammo = 50;
  max_pdata.fuel = 12 * TAU;
  max_pdata.oxygen = 20 * TAU;
  
  tank_pdata.hitpoints = 1;
  tank_pdata.score = 1;
  tank_pdata.ammo = 20;
  tank_pdata.fuel = 4 * TAU;
  tank_pdata.oxygen = 5 * TAU;
  
  pdata = max_pdata;
  }

void display(int id, int y, ld val, ld maxv, ld tank, ld unit) {
  auto sId = shiftless(Id);

//  ld pix = 1 / (2 * cgi.hcrossf / cgi.crossf);
  
  color_t col = rsrc_color[id];
  
  auto& shape = rsrc_shape[id][0];
  int N = isize(shape);
  
  ld ctr = 20*y+10;
  
  for(int i=0; i<N; i+=2) curvepoint(atscreenpos(10 + shape[i+1] * 100, ctr - shape[i] * 100, 1) * C0);
  curvedata.push_back(curvedata[curvestart]);
  queuecurve(sId, 0x000000FF, col, PPR::ZERO);
  
  ld sta = 20;
  ld siz = 200;
  ld fen = sta + siz;
  ld top = ctr-5;
  ld bot = ctr+5;
  
  if(maxv == 0) {
    queuestr(sta, ctr, 0, 20, its(val+.05), col >> 8, 1, 0);
    return;
    }
  
  curvepoint(atscreenpos(sta, top, 1) * C0);
  curvepoint(atscreenpos(fen, top, 1) * C0);
  curvepoint(atscreenpos(fen, bot, 1) * C0);
  curvepoint(atscreenpos(sta, bot, 1) * C0);
  curvepoint(atscreenpos(sta, top, 1) * C0);
  queuecurve(sId, col, col - 128, PPR::ZERO);
  
  ld end = sta + siz * val / maxv;
  curvepoint(atscreenpos(sta, top, 1) * C0);
  curvepoint(atscreenpos(end, top, 1) * C0);
  curvepoint(atscreenpos(end, bot, 1) * C0);
  curvepoint(atscreenpos(sta, bot, 1) * C0);
  curvepoint(atscreenpos(sta, top, 1) * C0);
  queuecurve(sId, col, col, PPR::ZERO);
  
  if(unit) for(ld u=unit; u<maxv-1e-3; u+=unit) {
    ld at = sta + siz * u / maxv;
    curvepoint(atscreenpos(at, top, 1) * C0);
    curvepoint(atscreenpos(at, ctr, 1) * C0);
    queuecurve(sId, 0x80, 0, PPR::ZERO);
    }

  for(ld u=tank; u<maxv-1e-3; u+=tank) {
    ld at = sta + siz * u / maxv;
    curvepoint(atscreenpos(at, ctr, 1) * C0);
    curvepoint(atscreenpos(at, bot, 1) * C0);
    queuecurve(sId, 0x80, 0, PPR::ZERO);
    }  
  }

void display_rsrc() {
  dynamicval<eGeometry> g(geometry, gEuclid);
  dynamicval<eModel> pm(pmodel, mdDisk);
  dynamicval<bool> ga(vid.always3, false);
  dynamicval<color_t> ou(poly_outline);
  dynamicval<geometryinfo1> gi(ginf[gEuclid].g, giEuclid2);
  initquickqueue();
  check_cgi(); cgi.require_shapes();

  #define D(id, y, field, unit) display(id, y, pdata.field, max_pdata.field, tank_pdata.field, unit)
  D(1, 1, hitpoints, 1);
  D(3, 2, ammo, 1);
  D(4, 3, fuel, TAU);
  D(5, 4, oxygen, TAU);
  D(2, 5, score, 10);
  #undef D
  
  quickqueue();
  }

}}
