#include "rogueviz.h"

/** \brief Snowball visualization
 *
 *  This visualization puts small objects ('snowballs') randomly throughout the space. 
 *  It provides a way to visualize the geometry without any tessellation.
 *
 *  Should work for tessellations where every tile is congruent.
 *
 *  The snow_lambda parameter gives the expected number of snowballs per cell.
 *  (The number in every region has Poisson distribution with mean proportional to its area.)
 *
 *  Freezes for tessellations with ideal vertices
 *
 * 
 *
 **/

namespace rogueviz {

namespace snow {

using rogueviz::objmodels::model;
using rogueviz::objmodels::tf_result;

vector<model> models;

ld snow_lambda = 0;

color_t snow_color = 0xFFFFFFFF;

bool snow_test = false;

/* intense brightness */
bool snow_intense = false;

/* a funny glitch */
bool snow_glitch = false;

/* disable textures */
bool snow_texture = true;

/* draw single objects? */
bool single_objects = true;

/* use random colors? */
bool random_colors = false;

/* just one snowball per cell, in the center */
bool just_centered = false;

int snow_shape = 0;

struct snowball {
  transmatrix T;
  int model_id;
  int object_id;
  color_t color;
  };

map<cell*, vector<snowball>> snowballs_at;

hpcshape& shapeid(int i) {
  switch(i) {
    case 0: 
      return cgi.shSnowball;
    case 1:
      return cgi.shHeptaMarker;
    case 2:
      return cgi.shDisk;
    default:
      return cgi.shDisk;
    }
  }

transmatrix random_snow_matrix(cell *c) {
  if(snow_glitch) {
    // in the standard tiling, this is incorrect but fun
    hyperpoint h = C0;
    h[0] = randd() - .5;
    h[1] = randd() - .5;
    h[2] = randd() - .5;
    h[2] = -h[2];
    return rgpushxto0(h);
    }
  else if(mproduct) {
    transmatrix T = PIU(random_snow_matrix(c));
    return orthogonal_move(T, (randd() - .5) * cgi.plevel);
    }
  else if(mhybrid && !mproduct) {
    return rots::lift_matrix(PIU(random_snow_matrix(c))); // * zpush((randd() - .5) * cgi.plevel);
    }
  else if(nonisotropic || bt::in()) {

    int co = bt::expansion_coordinate();
    ld aer = bt::area_expansion_rate();

    hyperpoint h;
    // randd() - .5;
    
    for(int a=0; a<3; a++) {
      if(a != co || aer == 1)
        h[a] = randd() * 2 - 1;
      else {
        ld r = randd();
        h[co] = log(hr::lerp(1, aer, r)) / log(aer) * 2 - 1;
        }
      }
    return bt::normalized_at(h);
    }
  else {
    while(true) {
      ld maxr = WDIM == 2 ? cgi.rhexf : cgi.corner_bonus;
      ld vol = randd() * wvolarea_auto(maxr);
      ld r = binsearch(0, maxr, [vol] (ld r) { return wvolarea_auto(r) > vol; });
      transmatrix T = random_spin();
      hyperpoint h = T * xpush0(r);
      cell* c1 = c;
      virtualRebase(c1, h);
      if(c1 == c)
        return T * xpush(r);
      }
    }
  }

bool draw_snow(cell *c, const shiftmatrix& V) {
  
  if(!snowballs_at.count(c)) {
    auto& v = snowballs_at[c];
    int cnt = 0;
    ld prob = randd();
    ld poisson = exp(-snow_lambda);
    while(cnt < 2*snow_lambda+100) {
      if(prob < poisson) break;
      prob -= poisson;
      cnt++;
      poisson *= snow_lambda / cnt;
      }
    if(snow_test) {
      if(c != cwt.at) 
        cnt = 0;
      else {
        c->wall = waFloorA;
        cnt = snow_lambda;
        }
      }      
    if(just_centered) cnt = 1;

    for(int t=0; t<cnt; t++) {
      snowball b{just_centered ? Id : random_snow_matrix(c), 0, -1, snow_color};
      if(random_colors)
        b.color = (hrand(0x1000000) << 8) | 0x808080FF;
      if(isize(models)) {
        b.model_id = hrand(isize(models));
        if(single_objects)
          b.object_id = hrand(isize(models[b.model_id].get().objindex)-1);
        }
      v.emplace_back(b);
      }
    }
  
  poly_outline = 0xFF;
  for(auto& T: snowballs_at[c]) {
    if(models.size()) {
     if(T.object_id == -1)
        models[T.model_id].render(V*T.T);
      else {
        auto& m = models[T.model_id].get();
        for(int i=m.objindex[T.object_id]; i<m.objindex[T.object_id+1]; i++) {
          auto& obj = m.objs[i];
          if(obj->color) queuepoly(V*T.T, obj->sh, obj->color);
          }
        }
      }
    else {
      auto& p = queuepoly(V * T.T, shapeid(snow_shape), T.color);
      if(!snow_texture) p.tinf = nullptr;
      if(snow_intense) p.flags |= POLY_INTENSE;
      }
    }

  return false;
  }

string cap = "non-Euclidean snowballs/";

void snow_slide(vector<tour::slide>& v, string title, string desc, reaction_t t) {
  using namespace tour;
  v.push_back(
    tour::slide{cap + title, 18, LEGAL::NONE | QUICKGEO, desc, 
   
  [t] (presmode mode) {
    setCanvas(mode, '0');

    slidecommand = "auto-movement";
    if(mode == pmKey) {
      using namespace anims;
      tour::slide_backup(ma, ma == maTranslation ? maNone : maTranslation);
      tour::slide_backup<ld>(shift_angle, 0);
      tour::slide_backup<ld>(movement_angle, 90);
      }
    
    if(mode == pmStart) {
      stop_game();
      tour::slide_backup<ld>(snow_lambda, 1);
      tour::slide_backup(snow_color, 0xC0C0C0FF);
      tour::slide_backup(snow_intense, true);
      t();
      start_game();
      }
    rogueviz::pres::non_game_slide_scroll(mode);
    }}
    );
  }

void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();
  dialog::init(XLAT("snowballs"), 0xFFFFFFFF, 150, 0);

  dialog::addSelItem("lambda", fts(snow_lambda), 'l');
  dialog::add_action([]() {
    dialog::editNumber(snow_lambda, 0, 100, 1, 10, "lambda", "snowball density");
    dialog::reaction = [] { snowballs_at.clear(); };
    });

  dialog::addSelItem("size", fts(snow_shape), 's');
  dialog::add_action([]() {
    snow_shape = (1 + snow_shape) % 3;
    });

  dialog::addBack();
  dialog::display();    
  }

void o_key(o_funcs& v) {
  if(snow_lambda) v.push_back(named_dialog("snowballs", show));
  }

auto hchook = addHook(hooks_drawcell, 100, draw_snow)

+ addHook(hooks_clearmemory, 40, [] () {
    snowballs_at.clear();
    })

+ addHook(hooks_o_key, 80, o_key)

#if CAP_COMMANDLINE
+ addHook(hooks_args, 100, [] {
  using namespace arg;
           
  if(0) ;
  else if(argis("-snow-lambda")) {
    shift_arg_formula(snow_lambda);
    }
  else if(argis("-snow-shape")) {
    shift(); snow_shape = argi();
    }
  else if(argis("-snow-test")) {
    snow_test = true;
    }
  else if(argis("-snow-color")) {
    shift(); snow_color = arghex();
    }
  else if(argis("-snow-intense")) {
    snow_intense = true;
    }
  else if(argis("-snow-no-texture")) {
    snow_texture = false;
    }
  else if(argis("-snow-glitch")) {
    snow_test = true;
    }
  else if(argis("-use-model")) {
    shift(); string s = args();
    shift(); ld scale = argf();
    shift(); int qty = argf();
    for(int i=0; i<qty; i++) {
      transmatrix T = random_spin();
      println(hlog, "T = ", T);
      models.emplace_back("rogueviz/models/", s, [scale,T] (hyperpoint h) { tf_result res{0, direct_exp(T*h*scale)}; return res; });
      s = "/" + s;
      }
    }
  else return 1;
  return 0;
  })
#endif

+ addHook(hooks_configfile, 100, [] {
    param_b(random_colors, "snow_random_colors");
    param_b(just_centered, "snow_just_centered");
    })

+ addHook_rvslides(161, [] (string s, vector<tour::slide>& v) {
  if(s != "noniso") return;
  v.push_back(tour::slide{
    cap+"snowball visualization", 10, tour::LEGAL::NONE | tour::QUICKSKIP,
    "Non-Euclidean visualizations usually show some regular constructions. Could we visualize the geometries themselves? Let's distribute the snowballs randomly."
    "\n\n"
    "You can use mouse to look in different directions. Press 5 to turn the automatic movement on or off. Press 'o' to change density and shape."
    ,
    [] (tour::presmode mode) {
      slide_url(mode, 'y', "YouTube link", "https://www.youtube.com/watch?v=leuleS9SpiA");
      slide_url(mode, 't', "Twitter link", "https://twitter.com/ZenoRogue/status/1245367263936512001");
      }
    });
  snow_slide(v, "Euclidean geometry", "This is the Euclidean space. Looks a bit like space flight in some old video games.", [] {
    set_geometry(gCubeTiling);
    snow_lambda = 20;
    });
  snow_slide(v, "Euclidean geometry (torus)", 
    "Some gamers incorrectly call warped worlds (like Asteroids) \"non-Euclidean\"; the animation for them would look the same, just a bit more regular. When playing these games, I have always wondered why the stars move so fast. Such far objects should not move.", [] {
    auto& T0 = euc::eu_input.user_axes;
    auto bak = T0;
    for(int i=0; i<3; i++)
    for(int j=0; j<3; j++) 
      T0[i][j] = i==j? 1: 0;
    euc::build_torus3();
    set_geometry(gCubeTiling);
    snow_lambda = 20;
    static bool once; once = false;
    tour::on_restore([bak] { if(once) return; once = true; auto& T0 = euc::eu_input.user_axes; stop_game(); T0 = bak; euc::build_torus3(); start_game(); });
    });
  snow_slide(v, "Hyperbolic geometry", "To the contrary, in hyperbolic geometry, parallax works in a completely different way. Everything moves. This space is expanding everywhere. Exponentially. In every geometry, snowballs close to us behave in a similar way as in the Euclidean space.", [] {
    set_geometry(gSpace534);
    snow_lambda = 20;
    });
  snow_slide(v, "H2xE", "This geometry is non-isotropic: it is hyperbolic in horizontal direction and Euclidean in (roughly) vertical direction. Since the space expands faster horizontally, the snowballs no longer look circular.", [] {
    set_geometry(gNormal);
    set_variation(eVariation::pure);
    set_geometry(gProduct);    
    snow_lambda = 20;
    });
  snow_slide(v, "Non-isotropic hyperbolic geometry", "This geometry is hyperbolic in both directions, but the curvatures are different.", [] {
    set_geometry(gNIH);
    snow_lambda = 20;
    });
  snow_slide(v, "Spherical geometry", "Do not forget about the spherical geometry. It is weird in general. When we leave the snowballs behind us, they look as if they were in front of us. Due to geometric lensing, snowballs in the antipodal point look as if they were close to us.", [] {
    set_geometry(gCell120);
    snow_lambda = 5;
    });
  snow_slide(v, "S2xE geometry", "Snowballs which are directly above or below us will look like rings, but it is hard to catch them in exactly the right spot.", [] {
    set_geometry(gSphere);
    set_variation(eVariation::pure);
    set_geometry(gProduct);    
    snow_lambda = 20;
    });
  snow_slide(v, "Nil", "Nil geometry, used for impossible figure constructions. Euclidean plane with another dimension added. Making a loop in the Euclidean plane makes you move in this third dimension, proportionally to the area of the loop. (Using larger snowballs here.)", [] {
    set_geometry(gNil);
    tour::slide_backup<ld>(sightranges[geometry], 7);
    tour::slide_backup(snow_shape, 2);
    snow_lambda = 5;
    });
  snow_slide(v, "SL(2,R)", "Here is SL(2,R), like Nil but based on hyperbolic plane instead. Geometric lensing effects are strong in both Nil and SL(2,R). (Starting with S^2 yields spherical geometry.)", [] {
    set_geometry(gNormal);
    set_variation(eVariation::pure);
    set_geometry(gRotSpace);
    snow_lambda = 5;
    });
#if CAP_SOLV
  snow_slide(v, "Solv", "Solv geometry. Like the non-isotropic hyperbolic geometry but where the horizontal and vertical curvatures work in the other way.", [] {
    set_geometry(gSol);
    // tour::slide_backup(snow_shape, 2);
    snow_lambda = 3;
    });
#endif
  });

}
}
