#include "rogueviz.h"

namespace rogueviz {

namespace honey {

bool alone = true;

bool in_special = false;

auto geoslide(eGeometry g, char canvas, int jhole, int jblock) {
  using namespace tour;
  return [=] (presmode mode) {
    setCanvas(mode, '0');
    if(mode == pmStart) {
      set_geometry(g);
      if(g == gSphere) {
        set_geometry(gProduct);
        }
      if(g == gNormal) {
        set_geometry(gRotSpace);
        }
      tour::slide_backup<ld>(sightranges[gProduct], 12);
      tour::slide_backup<ld>(sightranges[gNil], 7);
      tour::slide_backup<ld>(sightranges[gSol], 7);
      tour::slide_backup<ld>(sightranges[gSpace435], 7);
      vid.texture_step = 4;
      tour::slide_backup(patterns::jhole, jhole);
      tour::slide_backup(patterns::rwalls, jhole);
      tour::slide_backup(patterns::jblock, jblock);
      tour::slide_backup(patterns::whichCanvas, canvas);
      tour::slide_backup(vid.linewidth, vid.linewidth / 10);
      start_game();
      if(jblock < 0) {
        pmodel = mdDisk;
        sightranges[gSol] = 4;
        }
      }
    rogueviz::pres::non_game_slide_scroll(mode);
    if(mode == pmStop && jblock < 0)
      pmodel = mdGeodesic;
    slidecommand = "switch raycaster";

    if(in_special && among(mode, pmGeometrySpecial, pmStop)) {
      in_special = false;
      gamestack::pop();
      patterns::whichCanvas = canvas;
      vid.grid = false;
      fat_edges = false;
      sightranges[gSpace435] = 7;
      }

    else if(mode == pmGeometrySpecial && !in_special) {
      in_special = true;
      gamestack::push();
      patterns::whichCanvas = 'g';
      vid.grid = true;
      stdgridcolor = 0xFFFF00FF;
      fat_edges = true;
      start_game();
      sightranges[gSpace435] = 3;
      }

    if(mode == pmKey && jblock < 0) {
      sightranges[gSol] = 11 - sightranges[gSol];
      addMessage("Changed the sight range to ", sightranges[gSol]);
      }
    else if(mode == pmKey) {
      if(sl2) {
        addMessage("Raycaster not implemented here.");
        }
      #if CAP_RAY
      else if(ray::want_use != 2) {
        ray::want_use = 2;
        ray::max_cells = 4096;
        addMessage("Using a raycaster.");
        }
      #endif
      else {
        #if CAP_RAY
        ray::want_use = 0;
        #endif
        addMessage("Using primitives.");
        }
      }
    };
  }

string cap = "honeycombs/";

void honey(string s, vector<tour::slide>& v) {
  if(s != "noniso") return;
  using namespace tour;

  v.emplace_back(
    slide{cap+"Tessellations in 3D geometries", 999, LEGAL::NONE | QUICKSKIP, 
      "This series of slides presents the honeycombs we use. "
      "You can compare the output of primitive-based and raycaster-based rendering by pressing '5'.",
      [] (presmode mode) {}
      });

  v.emplace_back(
    slide{cap+"Hyperbolic space", 999, LEGAL::SPECIAL,
      "Traditional visualizations of non-Euclidean honeycombs "
      "show the edges of all cells. In our visualizations, we fill some of the cells. "
      "The disadvantage of the traditional visualization is visible here, on the example of {4,3,5} hyperbolic honeycomb: "
      "our Euclidean brains tend to interpret this visualization incorrectly. (Press '2' to get the traditional visualization.)",
      geoslide(gSpace435, 'r', 50, 0)
      });
  v.emplace_back(
    slide{cap+"S2xE", 999, LEGAL::NONE,
      "This is the S2xE geometry.",
      geoslide(gSphere, 'r', 10, 0)
      });
  v.emplace_back(
    slide{cap+"Solv: random", 999, LEGAL::NONE,
      "Random blocks in Solv geometry.",
      geoslide(gSol, 'r', 20, 0)
      });
  v.emplace_back(
    slide{cap+"Solv: Poincaré ball", 999, LEGAL::NONE,
      "Surfaces of constant 'z' in Solv geometry, displayed in Poincaré ball-like model. "
      "Press '5' to change the sight range (this slide is not optimized, so it will be slow).",
      geoslide(gSol, 'j', 0, -1)
      });
  v.emplace_back(
    slide{cap+"Solv: horotori", 999, LEGAL::NONE,
      "Solv geometry. Colored torus-like surfaces are surfaces of constant 'z'. "
      "Press '5' to enable the raycaster",
      geoslide(gSol, 'j', 50, 0)
      });
  v.emplace_back(
    slide{cap+"Solv: difficult region", 999, LEGAL::NONE,
      "This slide focuses on the area in Solv geometry which is difficult to render using primitives. "
      "Press '5' to enable the raycaster.",      
      geoslide(gSol, 'J', 0, 10)
      });
  v.emplace_back(
    slide{cap+"Nil geometry", 999, LEGAL::NONE,
      "Nil geometry. Colored surfaces are surfaces of constant 'x'. "
      "Press '5' to enable the raycaster",
      geoslide(gNil, 'j', 10, 10)
      });
  v.emplace_back(
    slide{cap+"SL(2,R) geometry", 999, LEGAL::NONE,
      "SL(2,R) geometry.",
      geoslide(gNormal, 'G', 90, 0)
      });

  }

#if CAP_RVSLIDES
vector<tour::slide> noniso_slides;
tour::slide *gen_noniso_demo() {
  noniso_slides.clear();
  using namespace tour;
  noniso_slides.emplace_back(
    slide{"Non-isotropic geometry demo", 999, LEGAL::NONE | QUICKSKIP | QUICKGEO, 
      "This is a presentation of non-isotropic geometries.",
      [] (presmode mode) {
        slide_url(mode, 'p', "paper about non-isotropic geometries", "https://arxiv.org/abs/2002.09533");
        setCanvas(mode, 'r');
        if(mode == pmStart) {
          set_geometry(gCubeTiling);
          start_game();
          }
        }
      });

  callhooks(pres::hooks_build_rvtour, "noniso", noniso_slides);
  pres::add_end(noniso_slides);
  return &noniso_slides[0];
  }
#endif

auto hooks  = addHook_rvslides(163, honey)
  + addHook_slideshows(120, [] (tour::ss::slideshow_callback cb) {
  
    if(noniso_slides.empty()) 
      gen_noniso_demo();

    cb(XLAT("non-isotropic geometries"), &noniso_slides[0], 'n');
    });

} }
