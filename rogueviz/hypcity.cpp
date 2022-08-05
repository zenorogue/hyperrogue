#include "rogueviz.h"

// 'hyperbolic city' demo
// download the model from https://sketchfab.com/3d-models/night-city-p2-82637933a7cb4fafadb0e2a79415c438 as rogueviz/models/emilejohansson_p2.obj

// see the results posted here:

// https://twitter.com/ZenoRogue/status/1375750351391981570
// -hypcity -noplayer -geo 4x5 -gp 1 1 -unrectified -switch-fpp -canvas 303030 camera=0 depth=0 -sr 3 -PM 0 -alpha 1 -zoom .95

// https://twitter.com/ZenoRogue/status/1375748835046215682
// -hypcity -noplayer -geo nil -canvas 303030 -back 44e4 -sight3 3

// https://twitter.com/ZenoRogue/status/1375754422752575488
//  add -PM 0 -alpha 1

namespace hr {
namespace hypcity {

using namespace rogueviz::objmodels;

void prepare_tf();

model city("rogueviz/models/", "emilejohansson_p2.obj", default_transformer, prepare_tf);

hyperpoint low, high;

void prepare_tf() {
  
  for(int i=0; i<4; i++) low[i] = 100, high[i] = -100;

  cgi.require_basics();
  hyperpoint corner = get_corner_position(cwt.at, 0);
  
  ld t = abs(corner[0] / corner[3]);

  city.tf = [=] (hyperpoint h) -> pair<int, hyperpoint> {
    swap(h[1], h[2]);
    h[2] = -h[2];
    h[2] += 0.063;
    h[0] -= 0.063;
    h[1] += 0.063;
    h *= 6;
    // h[0] -= .5;
    // h[1] += .5;

    for(int i=0; i<4; i++)
      low[i] = min(low[i], h[i]),
      high[i] = max(high[i], h[i]);
      
    if(hyperbolic || sphere) {
    
      hyperpoint hx;
      hx[0] = h[0] * t * 2;
      hx[1] = h[1] * t * 2;
      hx[2] = 0;
      hx[3] = 1;
      if(hyperbolic) hx = spin(45 * degree) * hx;
      hx = normalize(hx);
      hx = zshift(hx, h[2]*(t*(sphere ? 3 : 7)));

      return {0, hx}; 
      }
    
    if(nil || sol) {
      if(nil) swap(h[1], h[2]);
      h *= 0.5 / 0.378;
      if(sol) h *= vid.binary_width;
      if(nil) h *= nilv::nilwidth;
      h[3] = 1;
      return {0, h};
      }
    
    return {0, h};
    };
  println(hlog, "low = ", low);
  println(hlog, "high = ", high);
  }

bool draw_city_at(cell *c, const shiftmatrix& V) {
  if(nil) {
    auto co = nilv::get_coord(c->master);
    if(co[1]) return false;
    }

  if(sol) {
    auto co = c->master->distance;
    if(co) return false;
    }
  
  if(c == cwt.at || true) 
    city.render(V);

  return false;
  }

void enable() { 
  rogueviz::rv_hook(hooks_drawcell, 100, draw_city_at); 
  add_model_settings();
  }

auto hypcity_ah = arg::add3("-hypcity", enable)
+ addHook_slideshows(120, [] (tour::ss::slideshow_callback cb) {

    using namespace rogueviz::pres;
    static vector<slide> hypcity_slides;

    if(hypcity_slides.empty()) {
      hypcity_slides.emplace_back(
        slide{"Introduction", 999, LEGAL::NONE, 
          "Here we put a 3D model of a city into various geometries. Don't forget to try changing RogueViz projection and view range settings!\n\n"
          "Remember to try different projections (press '1')!"
          ,
          [] (presmode mode) {
          slide_url(mode, 'm', "original model by Emile Johansson", "https://sketchfab.com/3d-models/night-city-p2-82637933a7cb4fafadb0e2a79415c438");
          slide_url(mode, 't', "original Tweets by Nico Belmonte", "https://twitter.com/philogb/status/1375147728389476356");
          }});
      
      auto add = [&] (string s, string text, int dim, reaction_t setter) {
        hypcity_slides.emplace_back(
          tour::slide{s, 100, LEGAL::NONE | QUICKGEO, text,
            [=] (presmode mode) {
              if(mode == pmStart && dim == 2 && !vid.always3) {
                geom3::switch_fpp();
                }
              setCanvas(mode, '0');
              if(mode == pmStart) {
                slide_backup(canvas_default_wall, waInvisibleFloor);
                if(dim == 2) slide_backup(vid.camera, 0);
                if(dim == 2) slide_backup(vid.depth, 0);
                slide_backup(context_fog, false);
                setter();
                start_game();
                enable();
                }
              non_game_slide_scroll(mode);
              }});
        };
      
      add("hyperbolic", "Hyperbolic geometry.", 2, [] {
        set_geometry(g45);
        gp::param = {1, 1};
        set_variation(eVariation::unrectified);
        });
      add("spherical", "Spherical geometry.", 2, [] {
        set_geometry(gSmallSphere);
        set_variation(eVariation::pure);
        });
      add("Nil", "Nil geometry.", 3, [] {
        set_geometry(gNil);
        });
      add("Solv", "Solv geometry.", 3, [] {
        set_geometry(gSol);
        });
      add_end(hypcity_slides);
      }

    cb(XLAT("non-Euclidean city"), &hypcity_slides[0], 'c');
    });

}}
