#include "rogueviz.h"

namespace hr {

namespace planets {

texture::texture_data earth, neptune, mars, moon, sun;

map<void*, ld> radius;
map<void*, string> pname;
map<void*, char> key;

ld longit = 0, latit = 0;

bool loaded;

void load_planets() {
  if(!loaded) {
    earth.twidth = earth.theight = 0; earth.stretched = true;
    if(file_exists("textures/earth.png"))
      earth.readtexture("textures/earth.png");
    else
      earth.readtexture("textures/earth320.png");
    earth.loadTextureGL();
    radius[&earth] = 6371;
    pname[&earth] = "Earth";
    key[&earth] = 'e';

    neptune.twidth = neptune.theight = 0; neptune.stretched = true;
    neptune.readtexture("textures/neptune.png");
    neptune.loadTextureGL();
    radius[&neptune] = 24622;
    pname[&neptune] = "Neptune";
    key[&neptune] = 'n';

    sun.twidth = sun.theight = 0; sun.stretched = true;
    sun.readtexture("textures/sun.png");
    sun.loadTextureGL();
    radius[&sun] = 696340;
    pname[&sun] = "Sun";
    key[&sun] = 's';

    moon.twidth = moon.theight = 0; moon.stretched = true;
    moon.readtexture("textures/moon.png");
    moon.loadTextureGL();
    radius[&moon] = 1371.1;
    pname[&moon] = "Moon";
    key[&moon] = 'm';

    mars.twidth = mars.theight = 0; mars.stretched = true;
    mars.readtexture("textures/mars.png");
    mars.loadTextureGL();
    radius[&mars] = 3389.5;
    pname[&mars] = "Mars";
    key[&mars] = 'x';

    loaded = true;
    }
  }

basic_textureinfo tv, tv1;

bool anim = false;

int index = 0;

string mname;

void rev_band_conformal(ld& x, ld& y) {
  x /= 2; y /= 2;
  y = asin(tanh(y));
  }

template<class T> void reverse_band(hyperpoint h, hyperpoint& h1, const T& f) {
  ld x = h[0] * M_PI;
  ld y = h[1] * M_PI;
  f(x, y);
  h1 = xpush(x) * ypush(y) * C0;
  }

void reverse_model(hyperpoint h, hyperpoint& h1, eModel md) {
  switch(pmodel) {
    case mdDisk:
      h1 = perspective_to_space(h);
      return;

    case mdEquidistant:
      h[0] *= M_PI; h[1] *= M_PI;
      h1 = direct_exp(h);
      break;

    case mdEquiarea: {

      ld d = hypot_d(2, h);
      // d = sqrt(2*(1 - cos(d0))) / 2;
      ld d0 = acos(1 - pow(2 * d, 2) / 2);

      h[0] *= d0 / d; h[1] *= d0 / d;
      h1 = direct_exp(h);
      break;
      }
    
    case mdBand: {
      reverse_band(h, h1, rev_band_conformal);
      break;
      }

    case mdBandEquiarea: {
      reverse_band(h, h1, [] (ld& x, ld& y) { y = asin_auto(y); });
      break;
      }

    case mdBandEquidistant: {
      reverse_band(h, h1, [] (ld& x, ld& y) {});
      break;
      }
    
    case mdSinusoidal: {
      reverse_band(h, h1, [] (ld& x, ld& y) { x /= cos_auto(y); });
      break;
      }

    default:
      throw "unknown";
    }
  }

bool kill_off(hyperpoint h1, hyperpoint h2, hyperpoint& h3) {
  if(pmodel == mdEquidistant) {
    if(hypot_d(2, h2) > 1) {
      h3 = point3(0, 0, -1); 
      return true;
      }
    }
  if(pmodel == mdEquiarea) {
    if(hypot_d(2, h2) > 1) {
      h3 = point3(0, 0, -1); 
      return true;
      }     
    }
  if(pmodel == mdDisk && pconf.alpha == 0) {
    if(abs(h1[2]) < 1e-6) return true;
    if(h1[2] < 0) h3 = -h3;
    }
  if(pmodel == mdDisk && pconf.alpha > 1) {
    if(hypot_d(2, h2) > 1 / pconf.alpha) {
      h2 = h2 * .999 / pconf.alpha / hypot_d(2, h2);
      return true;
      }
    if(h1[2] < 0) h3[2] = -h3[2];
    }
  if(among(pmodel, mdBand, mdBandEquidistant, mdBandEquiarea)) {
    if(abs(h1[0]) < 1e-4 && h1[2] < 0) return true;
    if(pmodel == mdBand && (h2[0] < -2 || h2[0] > 2)) return true;
    if(pmodel != mdBand && (h2[0] < -1 || h2[0] > 1)) return true;
    if(pmodel == mdBandEquidistant && (h2[1] < -.5 || h2[1] > .5)) return true;
    }
  return false;
  }

int bad;

void test_reverse() {
  int ok = 0, killed = 0;
  bad = 0;
  for(int a=0; a<100; a++) {
    hyperpoint h = random_spin3() * C0;
    hyperpoint h1;
    hyperpoint h2;
    applymodel(shiftless(h), h1);
    reverse_model(h1, h2, pmodel);
    bool ko = kill_off(h, h1, h2);
    if(hdist(h, h2) < 1e-5)
      ok++;
    else if(ko)
      killed++;
    else {
      bad++;
      println(hlog, h, " -> ", h1, " -> ", h2);
      }
    }
  // println(hlog, "ok ", ok, " bad ", bad, " killed ", killed);
  }

texture::texture_data *tgt_planet = &moon;
texture::texture_data *src_planet = &earth;

int alpha = 255;

void pick_model() {
  switch(index) {
    case 0: 
      mname = "azimuthal equidistant";
      pmodel = mdEquidistant;
      break;

    case 1: 
      mname = "azimuthal equal area";
      pmodel = mdEquiarea;
      break;
    
    case 2:
      mname = "gnomonic";
      pmodel = mdDisk;
      pconf.alpha = 0;
      break;

    case 3:
      mname = "stereographic";
      pmodel = mdDisk;
      pconf.alpha = 1;
      break;

    case 4:
      mname = "orthographic";
      pmodel = mdDisk;
      pconf.alpha = 999999;
      break;

    case 5:
      mname = "Mercator projection";
      pmodel = mdBand;
      break;

    case 6:
      mname = "equirectangular projection";
      pmodel = mdBandEquidistant;
      break;

    case 7:
      mname = "cylindrical equal area";
      pmodel = mdBandEquiarea;
      break;
    }    
  }

ld prec = 5;

void draw_earth() {
  load_planets();

  shiftmatrix S = ggmatrix(currentmap->gamestart()) * spin90();

  ld mte = radius[src_planet] / radius[tgt_planet];
  
  if(true) {
    tv1.tvertices.clear();
    if(prec<0.5) prec=1;
    for(int x=-180; x<180; x+=prec)
    for(int y=-90; y<90; y+=prec) {
    
      vector<hyperpoint> bases = {
        point31(x, y, 0), 
        point31(x+prec, y, 0), 
        point31(x, y+prec, 0), 
        point31(x+prec, y, 0), 
        point31(x, y+prec, 0), 
        point31(x+prec, y+prec, 0)
        };
      
      auto tform = [&] (hyperpoint euc) {
        return xpush(euc[0] * degree) * ypush(euc[1] * degree) * C0;
        };  
      
      for(int i=0; i<6; i++) {
  
        hyperpoint h = bases[i];
        hyperpoint h1 = tform(h);
        curvepoint(h1);
        hyperpoint vi = point31((h[0] + 180) / 360., (h[1] + 90) / 180., 0);
        tv1.tvertices.push_back(glhr::pointtogl(vi));
  
        color_t col = tgt_planet->get_texture_pixel(vi[0]*tgt_planet->twidth, vi[1]*tgt_planet->theight);
        col &= 0xFFFFFF;
        addaura(S*h1, col, 0);
        }
      }
    
    if(isize(tv1.tvertices)) {
      color_t full = 0xFFFFFFFF;
      part(full, 0) = 255;
        
      auto& poly = queuecurve(S, 0, full, PPR::LINE);
      poly.flags |= POLY_TRIANGLES;
      poly.tinf = &tv1;
      poly.offset_texture = 0;
      tv1.texture_id = tgt_planet->textureid;
      }
    }
  
  if(true) {

    tv.tvertices.clear();
    
    dynamicval<eModel> md(pmodel, pmodel);
    dynamicval<ld> ma(pconf.alpha, pconf.alpha);
    
    pick_model();
    
    test_reverse(); 
    
    bool twoside = (pmodel == mdDisk && pconf.alpha > 100);
    
    for(int x=-180; x<180; x+=prec)
    for(int y=-90; y<90; y+=prec) 
    for(int si: {-1, 1}) {
    
      if(si == 1 && !twoside) continue;
    
      vector<hyperpoint> bases = {
        point31(x, y, 0), 
        point31(x+prec, y, 0), 
        point31(x, y+prec, 0), 
        point31(x+prec, y, 0), 
        point31(x, y+prec, 0), 
        point31(x+prec, y+prec, 0)
        };
      
      int errs = 0;
    
      for(int i=0; i<6; i+=3) {
  
        array<hyperpoint, 3> tforms;
        
        errs = 0;
  
        for(int j=0; j<3; j++) {
          hyperpoint euc = bases[i+j];
          hyperpoint h = xpush(euc[0] * degree) * ypush(euc[1] * degree) * C0;

          h = cspin(1, 2, latit*degree) * cspin(0, 2, -longit*degree) * h;
          
          hyperpoint h1, h2;
          applymodel(shiftless(h), h1);
          h1[0] *= mte;
          h1[1] *= mte;
          
          if(twoside) {
            if(h[2] * si < 0) errs++;
            else if(hypot_d(2, h1) > 1 / pconf.alpha) {
              h1 = h1 * .999999 / hypot_d(2, h1) / pconf.alpha;
              errs++;
              }
            }
          
          reverse_model(h1, h2, pmodel);
          
          if(kill_off(h, h1, h2)) errs++;
          
          tforms[j] = h2;
          }
        
        if(twoside && errs && mte < 1) continue;
        
        if(errs < 3) {
          for(int j=0; j<3; j++) {
            hyperpoint h = bases[i+j];
            hyperpoint h1 = tforms[j];
            curvepoint(h1);
            hyperpoint vi = point31((h[0] + 180) / 360., (h[1] + 90) / 180., 0);
            tv.tvertices.push_back(glhr::pointtogl(vi));
  
            color_t col = src_planet->get_texture_pixel(vi[0]*src_planet->twidth, vi[1]*src_planet->theight);
            col &= 0xFFFFFF;
            addaura(S*h1, col, 0);
            }
          }
        }
      }
      
    if(isize(tv.tvertices)) {
      color_t full = 0xFFFFFFFF;
      part(full, 0) = alpha;
        
      auto& poly = queuecurve(S, 0, full, PPR::LINE);
      poly.flags |= POLY_TRIANGLES;
      poly.tinf = &tv;
      poly.offset_texture = 0;
      tv.texture_id = src_planet->textureid;
      }
    }
  }

EX bool ourStats() {
  draw_centerover = false;

  displayfr(10, 10 + 2 * vid.fsize, 2, vid.fsize * 2, mname, 0xFFFFFF, 0);
  displayfr(vid.xres - 10, vid.yres - 10 - 2 * vid.fsize, 2, vid.fsize * 2, pname[src_planet] + " to " + pname[tgt_planet], 0xFFFFFF, 16);

  // nohelp = true;
  // nomenukey = true;
  clearMessages();

  glflush();
  
  // hide_hud = false;

  return true;
  }

texture::texture_data* get_planet(char ch) {
  if(ch == 'a') return &mars;
  if(ch == 'm') return &moon;
  if(ch == 's') return &sun;
  if(ch == 'n') return &neptune;
  if(ch == 'e') return &earth;
  return nullptr;
  }

ld max_alpha = 0;

EX void compare() {
  if(max_alpha) {
    ld t = ticks * 1. / anims::period;
    t = t - floor(t);
    static ld lrot = 0;
    ld rot = 0;
    ld dark = 1;
    if(t > .1 && t < .9) {
      rot = (t - .1) / .8;
      rot = rot * rot * (3-2*rot);
      }
    else {
      dark = t < .5 ? 10 * t : 10 * (1 - t);
      dark = dark * dark * (3-2*dark);
      }
    alpha = dark * max_alpha;
    View = cspin(0, 2, (rot - lrot) * TAU) * View;
    lrot = rot;
    anims::moved();
    }
  }

void choose_projection() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("choose projection"), 0xFFFFFFFF, 150, 0);
  dynamicval<int> di(index);
  for(int i=0; i<8; i++) {
    index = i;
    dynamicval<eModel> md(pmodel, pmodel);
    dynamicval<ld> ma(pconf.alpha, pconf.alpha);
    pick_model();
    dialog::addItem(mname, 'a'+i);
    dialog::add_action([i] { index = i; popScreen(); });
    }
  dialog::addBack();
  dialog::display();
  }

void choose_planet(texture::texture_data *& t) {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("choose the planet"), 0xFFFFFFFF, 150, 0);
  for(auto opt: {&earth, &moon, &sun, &mars, &neptune}) {
    dialog::addSelItem(pname[opt], its(radius[opt]) + " km", key[opt]);
    dialog::add_action([&t, opt] { t = opt; });
    }
  dialog::addBack();
  dialog::display();  
  }

void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("projections between planets"), 0xFFFFFFFF, 150, 0);
  add_edit(alpha);
  add_edit(latit);
  add_edit(longit);
  add_edit(max_alpha);
  add_edit(prec);
  dialog::addSelItem("projection used", mname, 'p');
  dialog::add_action_push(choose_projection);
  dialog::addSelItem("source planet", pname[src_planet], 's');
  dialog::add_action_push([] { choose_planet(src_planet); });
  dialog::addSelItem("target planet", pname[tgt_planet], 't');
  dialog::add_action_push([] { choose_planet(tgt_planet); });
  
  if(bad) dialog::addInfo("BAD", 0xFF0000);
  dialog::addBack();
  dialog::display();
  }

void enable() {
  using rogueviz::rv_hook;
  rv_hook(hooks_frame, 100, draw_earth);
  rv_hook(hooks_prestats, 100, ourStats);
  rv_hook(hooks_o_key, 80, [] (o_funcs& v) { v.push_back(named_dialog("planet projections", show)); });
  }

auto msc =   
  arg::add3("-moon", enable)
  + arg::add3("-ebody", [] {
    string s = arg::shift_args();
    src_planet = get_planet(s[0]);
    tgt_planet = get_planet(s[1]);
    if(!src_planet) src_planet = &earth;
    if(!tgt_planet) tgt_planet = &moon;
    })
  + addHook(anims::hooks_anim, 100, compare)
  + addHook(hooks_configfile, 100, [] {
    param_i(alpha, "moon_alpha")
    ->editable(0, 255, 15, "alpha (transparency)", "", 'a');
    param_i(index, "moon_index")
    ->editable(0, 7, 1, "projection index", "", 'i');
    param_f(latit, "moon_lat")
    ->editable(0, 7, 1, "latitude of the reference point", "", 'l');
    param_f(longit, "moon_long")
    ->editable(0, 7, 1, "longitude of the reference point", "", 'L');
    param_f(max_alpha, "moon_max_alpha")
    ->editable(0, 255, 15, "animation alpha max", "", 'm');
    param_f(prec, "moon_precision")
    ->editable(0, 30, .5, "precision", "larger values are less precise", 'p');
    })
  + addHook_rvslides(51, [] (string s, vector<tour::slide>& v) {
      if(s != "projections") return;
      using namespace tour;

      v.push_back(slide{
        "projections/sphere to sphere", 10, LEGAL::NONE | QUICKGEO,

        "We can also project a sphere to a sphere of different curvature. For example, what about the azimuthal equidistant projection from Earth to Moon? "
        "This projection correctly maps the angles and distances from a chosen point at Earth. "
        "Press '5' to use the place on Earth you are in as the chosen point, try other projections, or change the other settings!"
        ,
        [] (presmode mode) {
          slide_url(mode, 't', "Twitter link (with description)", "https://twitter.com/ZenoRogue/status/1339946298460483589");
          setCanvas(mode, '0');
          
          if(mode == pmStart) {
            enable();
            set_geometry(gSphere);
            slide_backup(canvas_default_wall, waInvisibleFloor);
            slide_backup(pmodel, mdDisk);
            slide_backup(pconf.scale, 1000);
            slide_backup(pconf.alpha, 1000);
            slide_backup(mapeditor::drawplayer, false);
            start_game();
            slide_backup(max_alpha, 192);
            }
          slidecommand = "options";
          if(mode == tour::pmKey) pushScreen(show);
          }});
      });

}
}

