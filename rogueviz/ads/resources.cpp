namespace hr {

namespace ads_game {

enum eResourceType { rtNone, rtHull, rtGold, rtAmmo, rtFuel, rtOxygen };

color_t rock_color[6] = { 0x703800FF, 0xC0A080FF, 0xC08010FF, 0xC04000FF, 0x408000FF, 0x8040A0FF,  };
color_t rsrc_color[6] = { 0x404040FF, 0x40C0C0FF, 0xFFD500FF, 0xFF0000FF, 0x00FF00FF, 0x0000FFFF };

vector<ld>* rsrc_shape[6] = { &shape_particle, &shape_heart, &shape_gold, &shape_weapon, &shape_fuel, &shape_airtank };
string rsrc_sound[6] = {"", "pickup-potion", "pickup-gold", "pickup-scroll", "pickup-speed", "seen-air" };

void rsrc_config() {
  ads_max_pdata.hitpoints = 3;
  ads_max_pdata.score = 0;
  ads_max_pdata.ammo = 50;
  ads_max_pdata.fuel = 12 * TAU;
  ads_max_pdata.oxygen = 20 * TAU;
  
  ads_tank_pdata.hitpoints = 1;
  ads_tank_pdata.score = 1;
  ads_tank_pdata.ammo = 20;
  ads_tank_pdata.fuel = 4 * TAU;
  ads_tank_pdata.oxygen = 5 * TAU;

  ds_max_pdata.hitpoints = 5;
  ds_max_pdata.score = 0;
  ds_max_pdata.ammo = 10;
  ds_max_pdata.fuel = 12 * TAU;
  ds_max_pdata.oxygen = 20 * TAU;
  
  ds_tank_pdata.hitpoints = 1;
  ds_tank_pdata.score = 1;
  ds_tank_pdata.ammo = 2;
  ds_tank_pdata.fuel = 8 * TAU;
  ds_tank_pdata.oxygen = 15 * TAU;
  
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

  all(ads_max_pdata, "ads_max_", "AdS maximum", "These control the maximum and initial values of your resources. ");
  all(ads_tank_pdata, "ads_tank_", "AdS bonus", "These control the amount of resource in a bonus tank. ");
  all(ds_max_pdata, "ds_max_", "dS maximum", "These control the maximum and initial values of your resources. ");
  all(ds_tank_pdata, "ds_tank_", "dS bonus", "These control the amount of resource in a bonus tank. ");
  }

void edit_rsrc() {
  auto all = [] (player_data& d) {
    add_edit(d.hitpoints);
    add_edit(d.ammo);
    add_edit(d.fuel);
    add_edit(d.oxygen);
    };
  all(DS_(max_pdata));
  all(DS_(tank_pdata));
  }

void init_rsrc() {
  pdata = DS_(max_pdata);
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
  
  if(true) ctr = 20*y+10, sta = 20;
  
  for(int i=0; i<N; i+=2) curvepoint(atscreenpos(sta - 10 + shape[i+1] * 100, ctr - shape[i] * 100, 1) * C0);
  curvedata.push_back(curvedata[curvestart]);
  queuecurve(sId, 0x000000FF, col, PPR::ZERO);
  
  ld siz = 150;
  ld fen = sta + siz;
  ld top = ctr-5;
  ld bot = ctr+5;
  
  if(maxv == 0) {
    string s;
    if(main_rock) s = format(tformat, current.shift);
    else s = its(val + .5);

    queuestr(sta, ctr, 0, 20, s, col >> 8, 1, 0);
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
  
  if(unit > 0 && maxv / unit <= siz/2 + 1e-6) for(ld u=unit; u<maxv-1e-3; u+=unit) {
    ld at = sta + siz * u / maxv;
    curvepoint(atscreenpos(at, top, 1) * C0);
    curvepoint(atscreenpos(at, ctr, 1) * C0);
    queuecurve(sId, 0x80, 0, PPR::ZERO);
    }

  if(tank > 0 && maxv / tank <= siz/2 + 1e-6) for(ld u=tank; u<maxv-1e-3; u+=tank) {
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

  #define D(id, y, field, unit) display(id, y, pdata.field, DS_(max_pdata).field, DS_(tank_pdata).field, unit)
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
  auto& tank_pdata = DS_(tank_pdata);
  auto& max_pdata = DS_(max_pdata);
  #define D(id, field) if(rsrc == id) { pdata.field += tank_pdata.field; if(max_pdata.field && pdata.field > max_pdata.field) pdata.field = max_pdata.field; }
  println(hlog, "gain resource ", int(rsrc));
  D(1, hitpoints)
  D(2, score)
  D(3, ammo)
  D(4, fuel)
  D(5, oxygen)
  playSound(nullptr, rsrc_sound[rsrc]);
  }

}}
