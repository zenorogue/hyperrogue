namespace hr {

namespace ads_game {

enum eResourceType { rtNone, rtHull, rtGold, rtAmmo, rtFuel, rtOxygen };

color_t rock_color[6] = { 0x703800FF, 0xC0A080FF, 0xC08010FF, 0xC04000FF, 0x408000FF, 0x8040A0FF,  };
color_t rsrc_color[6] = { 0x404040FF, 0xC0C0C0FF, 0xFFD500FF, 0xFF0000FF, 0x00FF00FF, 0x0000FFFF };

vector<ld>* rsrc_shape[6] = { &shape_particle, &shape_heart, &shape_gold, &shape_weapon, &shape_fuel, &shape_airtank };

void rsrc_config() {
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
  
  auto all = [] (player_data& d, string s, string t, string u) {
    param_i(d.hitpoints, s+"hp")
    ->editable(0, 10, 3, t + (": hitpoints"), u + ("Hitpoints are drained whenever you are hit."), '1');
    param_i(d.ammo, s+"ammo")
    ->editable(0, 10, 3, t + (": ammo"), u + ("Ammo is drained whenever you shoot."), '1');
    param_f(d.fuel, s+"fuel")
    ->editable(0, 10, 3, t + (": fuel"), u + ("Fuel is drained whenever you thrust."), '1');
    param_f(d.oxygen, s+"oxygen")
    ->editable(0, 10, 3, t + (": oxygen"), u + ("Oxygen is drained whenever continuously."), '1');
    };

  all(max_pdata, "ads_max_", "maximum", "These control the maximum and initial values of your resources. ");
  all(tank_pdata, "ads_tank_", "bonus", "These control the amount of resource in a bonus tank. ");
  }

void edit_rsrc() {
  auto all = [] (player_data& d) {
    add_edit(d.hitpoints);
    add_edit(d.ammo);
    add_edit(d.fuel);
    add_edit(d.oxygen);
    };
  all(max_pdata);
  all(tank_pdata);
  }

void init_rsrc() {
  pdata = max_pdata;
  game_over = false;
  }

void display(int id, int y, ld val, ld maxv, ld tank, ld unit) {
  auto sId = shiftless(Id);

//  ld pix = 1 / (2 * cgi.hcrossf / cgi.crossf);
  
  color_t col = rsrc_color[id];
  
  auto& shape = rsrc_shape[id][0];
  int N = isize(shape);
  
  ld ctr = max<ld>(20*y+10, current_display->ycenter - current_display->radius + 20 * y);
  ld sta = max<ld>(20, current_display->xcenter - current_display->radius + 20);
  
  if(pmodel != mdDisk) ctr = 20*y+10, sta = 20;
  
  for(int i=0; i<N; i+=2) curvepoint(atscreenpos(sta - 10 + shape[i+1] * 100, ctr - shape[i] * 100, 1) * C0);
  curvedata.push_back(curvedata[curvestart]);
  queuecurve(sId, 0x000000FF, col, PPR::ZERO);
  
  ld siz = 150;
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
  
  if(val > 0) {
    ld end = sta + siz * val / maxv;
    curvepoint(atscreenpos(sta, top, 1) * C0);
    curvepoint(atscreenpos(end, top, 1) * C0);
    curvepoint(atscreenpos(end, bot, 1) * C0);
    curvepoint(atscreenpos(sta, bot, 1) * C0);
    curvepoint(atscreenpos(sta, top, 1) * C0);
    queuecurve(sId, col, col, PPR::ZERO);
    }
  
  if(unit) for(ld u=unit; u<maxv-1e-3; u+=unit) {
    ld at = sta + siz * u / maxv;
    curvepoint(atscreenpos(at, top, 1) * C0);
    curvepoint(atscreenpos(at, ctr, 1) * C0);
    queuecurve(sId, 0x80, 0, PPR::ZERO);
    }

  for(ld u=tank; u<maxv-1e-3; u+=tank) {
    ld at = sta + siz * (1 - u / maxv);
    curvepoint(atscreenpos(at, ctr, 1) * C0);
    curvepoint(atscreenpos(at, bot, 1) * C0);
    queuecurve(sId, 0x80, 0, PPR::ZERO);
    }  
  }

bool display_rsrc() {
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
  return true;
  }

void gain_resource(eResourceType rsrc) {
  #define D(id, field) if(rsrc == id) { pdata.field += tank_pdata.field; if(max_pdata.field && pdata.field > max_pdata.field) pdata.field = max_pdata.field; }
  println(hlog, "gain resource ", int(rsrc));
  D(1, hitpoints)
  D(2, score)
  D(3, ammo)
  D(4, fuel)
  D(5, oxygen)
  }

}}
