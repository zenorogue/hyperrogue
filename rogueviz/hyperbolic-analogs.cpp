#include "rogueviz.h"

// used in: https://www.youtube.com/watch?v=H7NKhKTjHVE&feature=youtu.be
// run: -analogs

namespace hr {

namespace analogs {

patterns::ePattern wp;
flagtype wpf;

texture::texture_data earth;

bool loaded;

bool textured = true;

basic_textureinfo tv;

int earthpart;

string spherename = "A";
string hypername = "B";

vector<reaction_t> models_to_use = {
  [] {
    if(sphere) {
      pmodel = mdDisk;
      pconf.alpha = 1000;
      pconf.scale *= pconf.alpha;
      View = cspin(1, 2, 20._deg) * View;
      }
    else {
      pmodel = mdHyperboloid;
      pconf.top_z = 4;
      pconf.ballangle = -20;
      pconf.scale = .75;
      }
    spherename = "Euclidean sphere";
    hypername = "Minkowski hyperboloid";
    },
  [] {
    pmodel = mdDisk;
    pconf.alpha = 1;
    pconf.scale = .9;
    spherename = "stereographic projection";
    if(sphere) pconf.scale *= .75;
    hypername = "Poincaré disk model";    
    },
  [] {
    pmodel = mdDisk;
    pconf.alpha = 0;
    pconf.scale = sphere ? 0.25 : .9;
    spherename = "gnomonic projection";
    hypername = "Beltrami-Klein disk model";    
    },
  [] {
    pmodel = mdDisk;
    pconf.alpha = 1000;
    pconf.scale *= pconf.alpha;
    if(hyperbolic) pconf.scale *= .25;
    spherename = "orthographic projection";
    hypername = "Gans model";
    },
  [] {
    pmodel = mdEquidistant;
    spherename = "azimuthal equidistant";
    },
  [] {
    pmodel = mdEquiarea;
    spherename = "azimuthal equi-area";
    }, 
  [] {
    pmodel = mdBandEquidistant;
    pconf.scale = .5;
    if(sphere) pconf.scale *= 2;
    spherename = "equirectangular projection";
    hypername = "Lobachevsky coordinates";
    },
  [] {
    pmodel = mdBand;
    pconf.scale = .5;
    if(sphere) pconf.scale *= 0.9;
    spherename = "Mercator projection";
    hypername = "band model";
    },
  [] {
    pmodel = mdBandEquiarea;
    pconf.scale = .5;
    pconf.stretch = M_PI;
    spherename = "cylindrical equal-area";
    },
  [] {
    pmodel = mdCentralCyl;
    pconf.scale = .5;
    spherename = "central cylindrical";
    },
  [] {
    pmodel = mdGallStereographic;
    pconf.scale = .5;
    pconf.scale *= 1.8;
    spherename = "Gall stereographic";
    },
  [] {
    pmodel = mdMiller;
    pconf.scale = .5;
    spherename = "Miller cylindrical";
    },
  [] {
    pmodel = mdLoximuthal;
    pconf.scale = .5;
    if(sphere) pconf.scale *= 2;
    spherename = "loximuthal projection";
    pconf.loximuthal_parameter = 15._deg;
    },
  [] {
    pmodel = mdSinusoidal;
    pconf.scale = .5;
    if(sphere) pconf.scale *= 1.5;
    spherename = "(co)sinusoidal projection";
    },
  [] {
    pmodel = mdMollweide;
    pconf.scale = .5;
    if(sphere) pconf.scale *= 2;
    spherename = "Mollweide projection";
    },
  [] {
    pmodel = mdCollignon;
    pconf.scale = .5;
    if(sphere) pconf.scale *= 2;
    spherename = "Collignon projection";
    },
  [] {
    pmodel = mdTwoPoint;
    pconf.scale = .5;
    if(sphere) pconf.scale *= 2;
    spherename = "two-point equidistant";
    },
  [] {
    pmodel = mdSimulatedPerspective;
    pconf.scale = .5;
    spherename = "two-point azimuthal";
    },
  [] {
    pmodel = mdAitoff;
    pconf.scale = .5;
    if(sphere) pconf.scale *= 2;
    spherename = "Aitoff projection";
    },
  [] {
    pmodel = mdHammer;
    pconf.scale = .5;
    if(sphere) pconf.scale *= 1.3;
    spherename = "Hammer projection";
    },
  [] {
    pmodel = mdWinkelTripel;
    pconf.scale = .5;
    if(sphere) pconf.scale *= 2;
    spherename = "Winkel tripel projection";
    },
  [] {
    pmodel = mdWerner;
    pconf.scale = .3;
    spherename = "Werner projection";
    },
  };

ld prec = 5;

void draw_earth() {
  if(textured && !loaded) {
    earth.twidth = earth.theight = 0; earth.stretched = true;
    // earth.readtexture("extra/to-earth.png");
    earth.readtexture(file_exists("textures/earth.png") ? "textures/earth.png" : "textures/earth320.png");
    earth.loadTextureGL();
    loaded = true;
    }

  auto tform = [] (hyperpoint euc) {
    return xpush(euc[0] * degree) * ypush(euc[1] * degree) * C0;
    };  
  
  if(sphere && textured) {
  
  shiftmatrix S = ggmatrix(currentmap->gamestart());
  tv.tvertices.clear();
  
  if(prec < .5) prec = 1;
  
  for(ld x=-180; x<180; x+=prec)
  for(ld y=-90; y<90; y+=prec) {
  
    vector<hyperpoint> bases = {
      point31(x, y, 0), 
      point31(x+prec, y, 0), 
      point31(x, y+prec, 0), 
      point31(x+prec, y, 0), 
      point31(x, y+prec, 0), 
      point31(x+prec, y+prec, 0)
      };
    
    bool ok = true;

    if(pmodel == mdSimulatedPerspective) for(hyperpoint base: bases) {
      hyperpoint h = S.T * tform(base);
      if(h[2] <= 0.1) ok = false;
      }
      
    if(among(pmodel, mdEquidistant, mdEquiarea)) for(hyperpoint base: bases) {
      hyperpoint h = S.T * tform(base);
      if(h[2] <= -0.999) ok = false;
      }
      
    if(ok) for(auto base: bases) {
      hyperpoint h = base;
      hyperpoint h1 = tform(h);
      curvepoint(h1);
      
      hyperpoint vi = point31((h[0] + 180) / 360., (h[1] + 90) / 180., 0);
      tv.tvertices.push_back(glhr::pointtogl(vi));

      color_t col = earth.get_texture_pixel(vi[0]*earth.twidth, vi[1]*earth.theight);
      col &= 0xFFFFFF;
      addaura(S*h1, col, 0);
      }
    }
    
  if(isize(tv.tvertices)) {
    color_t full = 0xFFFFFFFF;
    part(full, 0) = earthpart;
      
    auto& poly = queuecurve(ggmatrix(currentmap->gamestart()), 0, full, PPR::LINE);
    poly.flags |= POLY_TRIANGLES;
    poly.tinf = &tv;
    poly.offset_texture = 0;
    tv.texture_id = earth.textureid;
    }
  }
  }

bool cycle_models = false;

bool animate = true;

EX void compare() {
  if(!animate) return;
  centerover = cwt.at;
  spherename = "";
  hypername = "";
  ld t = ticks * 1. / anims::period;
  t = frac(t);  
  
  if(cycle_models) {
    int mtu = isize(models_to_use);
    t *= mtu;
    if(anims::period < 100) {
      t = ticks % mtu;
      println(hlog, "t = ", t);
      }
    int index = t;
    t = frac(t);
    pconf.alpha = 1;
    pconf.scale = .9;
    pconf.stretch = 1;
    models_to_use[index]();
    }
  View = Id;
  ld t4 = t * 5;
  int at4 = t4;
  t4 -= at4;
  t4 = t4 * t4 * (3 - 2 * t4);
  earthpart = 192;
  if(at4 == 0)
    earthpart = lerp(255, earthpart, t4);
  else if(at4 == 1)
    View = spin(t4 * M_PI) * View;
  else if(at4 == 2)
    View = xpush(t4 * M_PI) * spin180() * View;
  else if(at4 == 3)
    View = ypush(t4 * M_PI) * xpush(M_PI) * spin180() * View;
  else if(at4 == 4) {
    View = ypush(M_PI) * xpush(M_PI) * spin180() * View;
    earthpart = lerp(255, earthpart, 1-t4);
    }
  anims::moved();
  no_find_player = true;
  vid.cells_drawn_limit = 20000;
  pconf.twopoint_param = 0.5;
  neon_nofill = true;
  if(hyperbolic && pmodel == mdWerner)
    neon_mode = eNeon::neon;
  else
    neon_mode = eNeon::none;
  vid.smart_area_based = (pmodel != mdHyperboloid);
  
  if(!inHighQual) {
    vid.cells_drawn_limit = 1000;
    }
  }

EX bool ourStats() {
  draw_centerover = false;

  displayfr(10, 10 + 2 * vid.fsize, 2, vid.fsize * 2, spherename, 0xFFFFFF, 0);

  displayfr(vid.xres - 10, vid.yres - (10 + 2 * vid.fsize), 2, vid.fsize * 2, hypername, 0xFFFFFF, 16);
  
  nohelp = true;
  nomenukey = true;
  clearMessages();

  glflush();
  
  hide_hud = false;

  return true;
  }

bool restrict_cell(cell *c, const shiftmatrix& V) {
  if(hyperbolic && zebra40(c) >= 40)
    c->landparam = 0x0080C0;
  if(hyperbolic && pmodel == mdWerner && hdist0(V.T * C0) > 3)
    return true;
  return false;
  }

int current_index = -1;

void choose_projection() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("choose projection"), 0xFFFFFFFF, 150, 0);
  for(int i=0; i<isize(models_to_use); i++) {
    hypername = "";
    if(1) {
      dynamicval<projection_configuration> vp(pconf, pconf);
      dynamicval<transmatrix> v(View, View);
      models_to_use[i]();
      }
    dialog::addBoolItem(hypername == "" ? spherename : spherename + " / " + hypername, i == current_index, 'a'+i);
    dialog::add_action([i] { 
      current_index = i;
      dual::switch_to(0);
      models_to_use[i]();
      dual::switch_to(1);
      models_to_use[i]();
      });
    }
  dialog::addBack();
  dialog::display();
  }

void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("hyperbolic analogs"), 0xFFFFFFFF, 150, 0);
  add_edit(prec);
  dialog::addItem("choose a projection", 'p');
  dialog::add_action_push(choose_projection);
  add_edit(earthpart);
  add_edit(textured);
  add_edit(animate);
  add_edit(cycle_models);
  
  dialog::addBack();
  dialog::display();
  }

void enable() {
  using rogueviz::rv_hook;
  
  vid.linequality = 4;
  enable_canvas();
  patterns::whichCanvas = 'F';

  colortables['F'][0] = 0x80C080;
  colortables['F'][1] = 0x80A080;
  pconf.scale = .3;
  
  vid.use_smart_range = 2;
  vid.smart_range_detail = 2;
  
  mapeditor::drawplayer = false;
  showstartmenu = false;
  
  dual::enable();

  dual::switch_to(0);
  set_geometry(gSphere);
  set_variation(eVariation::pure);
  enable_canvas();

  dual::switch_to(1);
  set_geometry(gNormal);
  set_variation(eVariation::pure);
  enable_canvas();

  rv_hook(hooks_frame, 100, draw_earth);
  rv_hook(hooks_drawcell, 100, restrict_cell);
  rv_hook(anims::hooks_anim, 100, compare);
  rv_hook(hooks_prestats, 100, ourStats);
  rv_hook(hooks_o_key, 80, [] (o_funcs& v) { v.push_back(named_dialog("hyperbolic analogs", show)); });
  }

auto msc = arg::add3("-analogs", enable)
  + addHook(hooks_configfile, 100, [] {
    param_f(prec, "analogs_precision")
    ->editable(0, 30, .5, "precision", "larger values are less precise", 'p');
    param_b(animate, "analogs_animate")
    ->editable("animate", 'a');
    param_b(cycle_models, "analogs_cycle")
    ->editable("cycle models in the animation", 'm');
    param_i(earthpart, "earthpart")
    ->editable(0, 255, 15, "Earth transparency", "", 't');
    param_b(textured, "analogs_texture")
    ->editable("draw Earth", 'T');
    })
  + addHook_rvslides(131, [] (string s, vector<tour::slide>& v) {
      if(s != "mixed") return;
      using namespace tour;

      v.push_back(slide{
        "projections/Earth and the hyperbolic plane", 10, LEGAL::NONE | QUICKGEO,

        "Cartographers need to project the surface of Earth to a flat paper. However, since the surface of Earth is curved, there is no perfect way to do this. "
        "Some projections will be conformal (map angles and small shapes faithfully), equidistant (map distances along SOME lines faithfully), equal-area (map areas proportionally), etc., "
        "but no map will be all at once. Cartographers use many projections.\n\n"
        
        "We need these projections because the Earth has positive curvature, while the paper has no curvature. Interestingly, "
        "most of the popular projections can be generalized, as projections from surface of curvature any K1 to surfaces of any curvature K2!\n\n"
        
        "This slide focuses on the hyperbolic analogs of popular spherical projections (projecting H² to E²). "
        "Press '5' to enable cycling between different projections, or 'o' for more options."
        ,
        [] (presmode mode) {
          slide_url(mode, 'y', "YouTube link (with description)", "https://youtu.be/H7NKhKTjHVE");
          slide_url(mode, 'm', "HyperRogue page about projections", "http://www.roguetemple.com/z/hyper/models.php");
          setCanvas(mode, '0');
          if(mode == pmStart) {
            slide_backup(mapeditor::drawplayer);
            slide_backup(vid.use_smart_range);
            slide_backup(vid.smart_range_detail);
            slide_backup(vid.linequality);
            enable();
            start_game();
            slide_backup(cycle_models);
            slide_backup(anims::period);
            }
          if(mode == pmStop) {
            dual::disable();
            start_game();
            }
          if(mode == pmKey) {
            cycle_models = !cycle_models;
            if(cycle_models) anims::period *= 12;
            else anims::period /= 12;
            }
          }});
      
      callhooks(rogueviz::pres::hooks_build_rvtour, "projections", v);
      });

}
}

