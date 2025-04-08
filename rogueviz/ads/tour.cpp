// TO DO
// * knives in Asteroids sometimes do not hit rocks
// * in Asteroids use the Relative Hell imagery
// * visualize the 'Spherical symmetry' slide

namespace hr {

namespace ads_game {

namespace ads_tour {
using namespace rogueviz::pres;

string defs = 
  "\\def\\map{m}"
  "\\def\\VofH{V}"
  "\\def\\dist{\\delta}"
  "\\def\\ra{\\rightarrow}"
  "\\def\\bbH{\\mathbb{H}}"
  "\\def\\bbE{\\mathbb{E}}"
  "\\def\\bbR{\\mathbb{R}}"
  "\\def\\bbS{\\mathbb{S}}"
  "\\def\\dS#1{d\\bbS^#1}"
  "\\def\\wadS#1{ad\\bbS^#1}"
  "\\def\\uadS#1{\\widetilde{ad\\bbS^#1}}"
  "\\renewcommand{\\rmdefault}{\\sfdefault}\\sf"
  ;

int slv_mode;

cell *slv;
transmatrix at0, at1;
int t0, t1;

void straight_line_viz(presmode mode) {
  if(mode == pmKey) slv_mode = (slv_mode == 0 ? 1 : 0);
  if(mode == pmStart) rogueviz::rv_hook(hooks_markers, 100, [] {
    println(hlog, "slv_mode = ", slv_mode, " tick = ", ticks);
    if(slv_mode == 1) {
      t0 = ticks;
      slv = shmup::pc[0]->base;
      at0 = shmup::pc[0]->at;
      slv_mode++;
      return;
      }
    if(slv_mode == 2 && ticks >= t0 + 20) {
      println(hlog, "elapsed ", ticks - t0);
      t1 = ticks;
      if(slv != shmup::pc[0]->base) { slv_mode = 0; return; }
      at1 = shmup::pc[0]->at;
      slv_mode++;
      return;
      }
    if(slv_mode == 3) {
      ld t = (ticks - t0) * 1. / (t1 - t0);
      vector<vector<hyperpoint>> pts(6);
      vector<hpcshape*> shapes = { &cgi.shSpaceshipBase, &cgi.shSpaceshipCockpit, &cgi.shSpaceshipEngine, &cgi.shSpaceshipGun, &cgi.shSpaceshipEngine, &cgi.shSpaceshipGun };
      for(int si=0; si<6; si++) {
        auto& sh = *(shapes[si]);
        for(int i=sh.s; i<sh.e; i++) {
          hyperpoint h = cgi.hpc[i];
          if(si >= 4) h = MirrorY * h;
          hyperpoint a0 = at0 * h;
          hyperpoint a1 = at1 * h;
          ld d = geo_inner(a0, a1);
          if(hyperbolic) d = -d;
          ld di = acos_auto_clamp(d);
          hyperpoint diff = (a1 - a0 / d) / tan_auto(di);
          h = a0 * cos_auto(di*t) + diff * sin_auto(di*t);
          if(hdist0(h) < 5) pts[si].push_back(h);
          }
        }
      vid.linewidth *= 3;
      for (const shiftmatrix& V : hr::span_at(current_display->all_drawn_copies, slv)) for(auto& pts1: pts) {
        for(auto h: pts1) curvepoint(h);
        queuecurve(V, 0xFFFF80FF, 0, PPR::SUPERLINE);
        }
      vid.linewidth /= 3;
      }
    });
  }

void ds_restart_scaled() {
  check_cgi();
  cgi.require_basics();
  cgi.require_shapes();
  ds_restart();
  }

void set_spacerocks_ship() {
  auto& cs = getcs();
  tour::slide_backup(cs.charid, 10);
  tour::slide_backup(cs.skincolor, 0xFFFFFFFF);
  tour::slide_backup(cs.eyecolor, 0x8080FFFF);
  tour::slide_backup(cs.dresscolor, 0xFFC0C0FF);
  tour::slide_backup(cs.haircolor, 0xC0FFC0FF);
  }

slide relhell_tour[] = {
  {"Intro", 10, LEGAL::ANY | QUICKGEO | NOTITLE, 
    "Relative Hell is a game taking place in relativistic analogs of spherical and hyperbolic geometries. "
    "Here is Space Rocks, a clone of the classic game Asteroids. It is based on Newtonian physics: "
    "if you accelerate, you move forever in that direction, unless you deaccelerate.", 
    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        set_spacerocks_ship();
        set_geometry(gEuclidSquare);
        set_variation(eVariation::pure);
        tour::slide_backup(land_structure, lsSingle);
        tour::slide_backup(specialland, laAsteroids);
        auto& ua = euc::eu_input;
        tour::slide_backup(ua, ua);
        for(int i=0; i<2; i++)
        for(int j=0; j<2; j++) ua.user_axes[i][j] = i == j ? 5 : 0;
        ua.twisted = false;
        euc::build_torus3();
        tour::slide_backup(shmup::on, true);
        tour::slide_backup(pconf.scale, 0.5);
        });
      }
    },

  {"Small Relativistic Effects", 10, LEGAL::ANY | QUICKGEO, 
    "In our real world, the universe is expanding, and the spaceship would observe relativistic effects if it started to move very fast. "
    "Such effects can be also observed in this slide, although you still need to wait for a long time or move very fast. They will be more pronounced in Relative Hell, and in the later slides.",
    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        ads_game::run_ds_game_std();
        const ld sca = 100;
        tour::slide_backup(ds_simspeed, M_PI / 10 / sca * 5);
        tour::slide_backup(ds_missile_rapidity, 0.1);
        tour::slide_backup(vid.creature_scale, vid.creature_scale / sca);
        tour::slide_backup(pconf.scale, sca);
        tour::slide_backup(texture_off, true);
        dynamicval<ld> fs(future_shown, -10);
        ds_restart_scaled();
        tour::slide_backup(invincibility_pt, HUGE_VAL);

        rockgen.cshift = 0;

        if(1) {
          dynamicval<eGeometry> g(geometry, gSpace435);
          for(int x=-10; x<=10; x++)
          for(int y=-10; y<=10; y++) if(hypot(x+0.5, y) >= 2) {
            rockgen.add(cspin(0, 2, (x + randd() - randd()) / sca) * cspin(1, 2, (y + randd() - randd()) / sca));
            }
          }

        rockgen.cshift = 10;
        });
      if(mode == pmStart) {
        add_ds_cleanup();
        rogueviz::on_cleanup_or_next([] { lps_enable(nullptr); });
        }
      }
    },

  {"Lorentz Contraction", 10, LEGAL::ANY | QUICKGEO, 
    "Here we make the relativistic effects easier to observe. According to the principles of special relativity, fast moving objects are contracted. The closer their speed is to "
    "the speed of light, the more contracted they are. This can be "
    "seen when you look at the moving objects here.\n\n"
    "We mean objects moving fast relative to you -- if you accelerate, previously stationary objects will start moving fast relative to you. Your ship is able to accelerate much faster than in "
    "the previous slide.",
    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        ads_game::run_ds_game_std();
        const ld sca = 100;
        tour::slide_backup(ds_simspeed, M_PI / 10 / sca * 5);
        tour::slide_backup(ds_missile_rapidity, 0.5);
        tour::slide_backup(ds_accel, ds_accel * 10);
        tour::slide_backup(vid.creature_scale, vid.creature_scale / sca);
        tour::slide_backup(pconf.scale, sca);
        tour::slide_backup(texture_off, true);
        dynamicval<ld> fs(future_shown, -10);
        ds_restart_scaled();
        tour::slide_backup(invincibility_pt, HUGE_VAL);

        rockgen.cshift = 0;

        if(1) {
          dynamicval<eGeometry> g(geometry, gSpace435);
          for(int x=-10; x<=10; x++)
          for(int y=-10; y<=10; y++) if(hypot(x+0.5, y) >= 2) {
            rockgen.add(cspin(0, 2, (x + randd() - randd()) / sca) * cspin(1, 2, (y + randd() - randd()) / sca));
            }

          for(int x=0; x<=24; x++)
          for(int y=-10; y<=10; y++) if(y) {
            rockgen.cshift = (rand() % 1000) / 100. / sca;
            rockgen.add(cspin(0, 1, x * 15._deg) * cspin(1, 2, y / sca) * lorentz(0, 3, 1 + randd() * 3));
            }
          }

        rockgen.cshift = 10;
        });
      if(mode == pmStart) {
        add_ds_cleanup();
        rogueviz::on_cleanup_or_next([] { lps_enable(nullptr); });
        }
      }
    },

  {"Time Dilation", 10, LEGAL::ANY | QUICKGEO, 
    "Another well-known relativistic effect is time dilation. Time passes differently for different objects.\n\n"
    "Try to accelerate, then return to the yellow star. Your clock will be different than the clock of the star.",
    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        ads_game::run_ds_game_std();
        const ld sca = 100;
        tour::slide_backup(ds_simspeed, M_PI / 10 / sca * 5);
        tour::slide_backup(ds_missile_rapidity, 0.5);
        tour::slide_backup(ds_accel, ds_accel * 10);
        tour::slide_backup(vid.creature_scale, vid.creature_scale * 5 / sca);
        tour::slide_backup(pconf.scale, sca);
        tour::slide_backup(texture_off, true);
        tour::slide_backup(view_proper_times, true);
        dynamicval<ld> fs(future_shown, -10);
        ds_restart_scaled();
        tour::slide_backup(invincibility_pt, HUGE_VAL);

        rockgen.cshift = 10;
        });
      if(mode == pmStart) {
        add_ds_cleanup();
        rogueviz::on_cleanup_or_next([] { lps_enable(nullptr); });
        }
      }
    },

  {"Spherical geometry", 10, LEGAL::ANY | QUICKGEO | NOTITLE, 
    "Relative Hell combines relativity with non-Euclidean geometry. "
    "Here is Space Rocks played in spherical geometry. It uses "
    "stereographic projection so that a big part of the sphere can be seen. (You can press '5' to switch to and from the orthogonal projection.)",
    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        set_spacerocks_ship();
        set_geometry(gSphere);
        set_variation(eVariation::bitruncated);
        tour::slide_backup(land_structure, lsSingle);
        tour::slide_backup(specialland, laAsteroids);
        tour::slide_backup(shmup::on, true);
        tour::slide_backup(pconf.scale, 0.5);
        tour::slide_backup(pconf.alpha, 1);
        tour::slide_backup(vid.monmode, 2);
        });
     if(mode == pmKey) {
        if(pconf.alpha == 1) pconf.alpha = 1000, pconf.scale = 950;
        else pconf.alpha = 1, pconf.scale = 0.5;
        }
      }
    },

  {"Spherical symmetry", 10, LEGAL::ANY | QUICKGEO | NOTITLE, 
    "In the previous slide, time was implemented as in most games, and "
    "how Newton imagined it. It is assumed that objects move on geodesics "
    "(great circles) if no force is acting on them.\n\n"
    "Note that, in the world of Newton and Galileo, and also in the world of Einstein's special relativity, the spacetime is perfectly symmetric. "
    "You cannot really tell that you are moving (except by looking at landmarks); you can create a frame of reference and a system of coordinates "
    "in which the ship is not moving and the physics are the same.\n\n"
    "While the spherical space is perfectly symmetric, the spacetime as shown in this slide is not. "
    "The wings of our ship do not move in straight lines (instead they move in smaller circles, which are curved). "
    "If we had unchained items there, they would move towards the center of the ship, allowing the "
    "captain to tell that they are moving.\n\n"
    "Press 5 to see a visualization of how various parts of the ships would move if they actually moved in straight lines."
    ,

    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        set_spacerocks_ship();
        set_geometry(gSphere);
        set_variation(eVariation::bitruncated);
        tour::slide_backup(land_structure, lsSingle);
        tour::slide_backup(specialland, laAsteroids);
        tour::slide_backup(shmup::on, true);
        tour::slide_backup(pconf.scale, 0.5);
        tour::slide_backup(pconf.alpha, 1);
        tour::slide_backup(vid.monmode, 2);
        tour::slide_backup(vid.creature_scale, 3);
        tour::slide_backup(dont_gen_asteroids, true);
        });
      straight_line_viz(mode);
      }
    },

  {"de Sitter spacetime", 10, LEGAL::ANY | QUICKGEO, 
    "The de Sitter spacetime is a way to add time to spherical geometry in a symmetric way. "
    "The space here feels to expand exponentially as the time passes, as in, nearby objects get farther and farther away. "
    "Still, the spacetime is symmetric -- if we are using an appropriate frame of reference, the 'totally geodesic' slice of spacetime at t=0 is "
    "always a sphere of the same size.\n\n"
    "If we fly too far away from the yellow star, we can never fly back to it, due to "
    "the expansion. For the same reason, we can also never actually reach the other side of the sphere.",

    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        ads_game::run_ds_game_std();
        tour::slide_backup(ds_simspeed, M_PI / 10);
        // tour::slide_backup(ds_scale, 1);
        tour::slide_backup(pconf.scale, 1);
        dynamicval<ld> fs(future_shown, -10);
        ds_restart();

        rockgen.cshift = 10;
        });
      if(mode == pmStart) {
        add_ds_cleanup();
        rogueviz::on_cleanup_or_next([] { lps_enable(nullptr); });
        }
      }
    },

  {"de Sitter game", 10, LEGAL::ANY | QUICKGEO, 
    "The de Sitter part of the Relative Hell game takes part in this spacetime. "
    "Try to stay close to the yellow star as long as possible! If required, you can "
    "shoot down stars with a limited number of missiles. For high score, you will also need to replenish your "
    "resources by capturing free-flying fuel, oxygen, health, and missiles.",

    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        ads_game::run_ds_game_std();
        });
      if(mode == pmStart) {
        add_ds_cleanup();
        rogueviz::on_cleanup_or_next([] { lps_enable(nullptr); });
        }
      }
    },

  {"Hyperbolic geometry", 10, LEGAL::ANY | QUICKGEO | NOTITLE, 
    "Hyperbolic geometry is the opposite of spherical geometry. "
    "Here is Space Rocks played in it. We use the Poincaré model to display the hyperbolic plane; "
    "you can press 5 to switch to the Beltrami-Klein model.\n\n",

    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        set_spacerocks_ship();
        set_geometry(gKleinQuartic);
        set_variation(eVariation::bitruncated);
        tour::slide_backup(land_structure, lsSingle);
        tour::slide_backup(specialland, laAsteroids);
        tour::slide_backup(shmup::on, true);
        tour::slide_backup(pconf.scale, 0.95);
        tour::slide_backup(pconf.alpha, 1);
        tour::slide_backup(vid.monmode, 2);
        });
     if(mode == pmKey) {
        if(pconf.alpha == 1) pconf.alpha = 0;
        else pconf.alpha = 1;
        }
      }
    },

  {"Hyperbolic symmetry", 10, LEGAL::ANY | QUICKGEO | NOTITLE, 
    "Of course, just like in spherical space, this is not a symmetric spacetime.\n\n"
    "Press 5 to see a visualization of how various parts of the ships would move if they actually moved in straight lines.",

    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        set_spacerocks_ship();
        set_geometry(gKleinQuartic);
        set_variation(eVariation::bitruncated);
        tour::slide_backup(land_structure, lsSingle);
        tour::slide_backup(specialland, laAsteroids);
        tour::slide_backup(shmup::on, true);
        tour::slide_backup(pconf.scale, 0.95);
        tour::slide_backup(pconf.alpha, 1);
        tour::slide_backup(vid.monmode, 2);
        tour::slide_backup(dont_gen_asteroids, true);
        tour::slide_backup(vid.linequality, 3);
        });
     straight_line_viz(mode);
     }
    },

  {"anti-de Sitter spacetime", 10, LEGAL::ANY | QUICKGEO | NOTITLE, 
    "anti-de Sitter spacetime is a way to add time to this space in a symmetric way.\n\n"
    "Because of how the anti-de Sitter spacetime works, faraway objects are 'pulled' towards us. "
    "You can see this effect by shooting a missile -- it will eventually return to us!\n\n"
    "In the world of Relative Hell, this pull is countered by making the static objects rotate in a specific way -- this creates a centrifugal "
    "force which counterbalances this effect. As you can see, the heptagons further away are "
    "squashed -- this is again the Lorentz contraction\n\n."
    "You can also press key '5' to switch to the Beltrami-Klein projection -- "
    "this counterbalances the squashing, making all the heptagons normal."
    ,

    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        ads_game::run_ads_game_std();
        /* disable everything */
        tour::slide_backup(pconf.alpha, 1);
       });
     if(mode == pmKey) {
        if(pconf.alpha == 1) pconf.alpha = 0;
        else pconf.alpha = 1;
        }
      }
    },

  {"auto-rotation", 10, LEGAL::ANY | QUICKGEO | NOTITLE, 
    "If the constantly spinning screen makes you feel dizzy, we can "
    "also automatically counter-rotate it. This makes the geometry harder to "
    "understand, but is also cool.\n\n."

    "You can also press key '5' to see how the spacetime behaves with auto-rotation on and off."
    ,

    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        ads_game::run_ads_game_std();
        /* disable everything */
        tour::slide_backup(pconf.alpha, 0);
        tour::slide_backup(auto_rotate, true);
       });
     if(mode == pmKey) {
        auto_rotate = !auto_rotate;
        }
      }
    },

  {"what you would see", 10, LEGAL::ANY | QUICKGEO | NOTITLE, 
    "One aspect we have not discussed so far: the game computed the "
    "coordinates of all objects in the ship's frame of reference "
    "(which puts the ship at the center and the current time at t=0), "
    "and displayed the slice t=0 of that spacetime.\n\n"
    "Due to the limited speed of light, this is not what the ship would "
    "actually see.\n\n"
    "In this slide, you can see the 'visible state' -- everything is seen at "
    "the moment that the ship would actually see.\n\n"

    "You can press key '5' to see how the spacetime behaves with the 'visible state' and default.\n\n"
    "During the game, see the 'view mode' menu to change many options discussed in this tour, as well as some extra visualizations."
    ,

    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        ads_game::run_ads_game_std();
        /* disable everything */
        tour::slide_backup(pconf.alpha, 0);
        tour::slide_backup(auto_rotate, false);
        tour::slide_backup(which_cross, -1);
       });
     if(mode == pmKey) {
        if(which_cross == -1) which_cross = 0;
        else which_cross = -1;
        }
      }
    },

  {"turrets", 10, LEGAL::ANY | QUICKGEO | NOTITLE, 
    "Let us place some turrets in our anti-de Sitter world.\n\n"

    "These turrets are as accurate as they could possibly be -- they see our ship, and compute the shooting angle so that the ship would be hit "
    "if it did not accelerate in the meantime. If you do not accelerate for some time, you should see that they indeed hit you.\n\n"

    "As you can imagine from the previous parts, their information is rather outdated...\n\n"

    "The world here is still displayed in the 'slice t=0' mode, rather than 'visible state'. The turrets are totally deterministic so let us assume the "
    "ship's AI helps us by computing the current state based on the visible past. The enemy bullets move at speed close to the speed of light, so it "
    "would hard to see them otherwise.\n\n"

    "You may notice the \"wobbling\" of turrets, this is caused by the Lorentz transformations as the spaceship accelerates.",

    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        ads_game::run_ads_game_std();
        tour::slide_backup(pconf.alpha, 1);
       });
      }
    },

  {"anti-de Sitter game", 10, LEGAL::ANY | QUICKGEO | NOTITLE, 
    "So this is our anti-de Sitter game.\n\n"

    "Shoot down the rocks to get gold and replenish resources. "
    "Similar to HyperRogue, collecting gold will allow you to find other parts of the spacetime, "
    "where you can find other treasures and challenges. Have fun!",

    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        ads_game::run_ads_game_std();
       });
      }
    },

  {"MATH PART!", 123, LEGAL::ANY | NOTITLE, "",
  
    [] (presmode mode) {
      empty_screen(mode);
      white_screen(mode);
      add_stat(mode, [] {
        dialog::init();        
        dialog::addTitle("MATH PART!", 0x0, 200);
        dialog::addBreak(100);
        dialog::addHelp(
          "The rest of this guided tour is a lecture on mathematics of the things we have seen so far. "
          "If you just wanted an intuitive explanation of what is going on, read no further. "
          "But if math is fun for you, please go on!");
        dialog::display();
        return true;
        });
      }
    },

  {"Euclidean geometry", 999, LEGAL::NONE | QUICKGEO | USE_SLIDE_NAME | NOTITLE,
    "explanation",
    [] (presmode mode) {
      setCanvas(mode, &ccolor::chessboard, [] { set_geometry(gEuclidSquare); set_variation(eVariation::pure); });
      latex_slide(mode, defs+R"=(
   {\color{remph}3-dimensional Euclidean space:}
   \begin{itemize}
   \item $\bbE^3 = \{(x,y,z): x,y,z \in \bbR\}$
   \item squared distance between \\ points $(x_1,y_1,z_1)$ and $(x_2, y_2, z_2)$ is \[(x_1-x_2)^2+(y_1-y_2)^2+(z_1-z_2)^2\]
   \item {\color{remph} isometries} (rotations, etc.) preserve this squared distance
   \end{itemize}
   )=", sm::SIDE, 90);
      if(mode == pmStart) {
        tour::slide_backup(mapeditor::drawplayer, false);
        tour::slide_backup(vid.axes, 0);
        rogueviz::rv_hook(hooks_latex_slide, 100, [] { dialog::dwidth += 500; menu_darkening++; dialog::draw_side_shade(); dialog::dwidth -= 500; menu_darkening --; });
        }
      }},

  {"Minkowski geometry", 999, LEGAL::NONE | QUICKGEO | USE_SLIDE_NAME | NOTITLE, 
    "explanation",
    [] (presmode mode) {
      latex_slide(mode, defs+R"=(
   {\color{remph}Minkowski spacetime with 2 space and 1 time dimension:}
   \begin{itemize}
   \item $\bbE^{2,1} = \{(x,y,t): x,y,t \in \bbR\}$
   \item spacetime interval between \\ points $(x_1,y_1,t_1)$ and $(x_2, y_2, t_2)$ is \[(x_1-x_2)^2+(y_1-y_2)^2-(t_1-t_2)^2\]
   \item {\color{remph} Lorentz transformations} preserve this
   \end{itemize}
   )=", sm::SIDE, 90);
      setCanvas(mode, &ccolor::chessboard, [] { set_geometry(gEuclidSquare); set_variation(eVariation::pure); tour::slide_backup(vid.axes, 0); });
      static int start = -1;
      if(mode == pmKey) start = (start == -1) ? ticks : -1;
      if(mode == pmStart) {
        tour::slide_backup(anims::ma, anims::maTranslation);
        tour::slide_backup(pconf.stretch, 1);
        tour::slide_backup(anims::movement_angle.get(), spin(-90._deg));
        tour::slide_backup(anims::cycle_length, 0);
        tour::slide_backup(mapeditor::drawplayer, false);
        tour::slide_backup(vid.axes, 0);
        tour::slide_backup(vid.use_smart_range, 2);
        tour::slide_backup(vid.smart_range_detail, 1);
        rogueviz::rv_hook(hooks_frame, 101, [] {
          if(start == -1) { anims::cycle_length = 0; pconf.stretch = 1; return; }
          ld t = asinh((ticks - start) / 5000.);
          anims::cycle_length = sinh(t) * 10;
          pconf.stretch = sqrt(1 - tanh(t) * tanh(t));
          println(hlog, "t=", t, "sinh = ", anims::cycle_length, " stretch = ", pconf.stretch);
          });
        rogueviz::rv_hook(hooks_latex_slide, 100, [] { 
          initquickqueue();
          dynamicval<ld> s(pconf.stretch, 1);
          drawMonsterType(moRunDog, nullptr, shiftless(spin(90._deg)), 0xFFFFFFFF, start >= 0 ? (ticks-start) / 500. : 0, 0xFFFFFFFF);
          sortquickqueue();
          quickqueue();
          dialog::dwidth += 500; menu_darkening++; dialog::draw_side_shade(); dialog::dwidth -= 500; menu_darkening --;
          });
        }
      }},

  {"spherical geometry", 999, LEGAL::NONE | QUICKGEO | USE_SLIDE_NAME | NOTITLE, 
    "explanation",
    [] (presmode mode) {
      setCanvas(mode, &ccolor::football, [] { set_geometry(gSphere); });
      if(mode == pmStart) {
        tour::slide_backup(pconf.scale, 500);
        tour::slide_backup(pconf.alpha, 1000);
        tour::slide_backup(mapeditor::drawplayer, false);
        tour::slide_backup(vid.axes, 0);
        }
      latex_slide(mode, defs+R"=(
   {\color{remph}2-dimensional sphere:}
   \begin{itemize}
   \item $\bbS^2 = \{(x,y,z) \in \bbE^3: x^2+y^2+z^2=1\}$
   \item distances measured as \\ the lengths of curves in Euclidean space
   \item {\color{remph} isometries} (rotations, etc.) keep this distance
   \end{itemize}
   )=", sm::SIDE, 90);
      }},    

  {"hyperbolic geometry", 999, LEGAL::NONE | QUICKGEO | USE_SLIDE_NAME | NOTITLE,
    "explanation",
    [] (presmode mode) {
      latex_slide(mode, defs+R"=(
   {\color{remph}2-dimensional hyperbolic space (Minkowski hyperboloid model):}
   \begin{itemize} 
   \item $\bbH^2 = \{(x,y,t) \in \bbE^{2,1}: x^2+y^2-t^2=-1, t>0\}$
   \item distances measured as \\ the lengths of curves in $\bbE^{2,1}$
   \item {\color{remph} isometries} (rotations, etc.) keep this distance
   \item we get the Poincaré model by projecting \\ $(x,y,t) \mapsto (\frac{x}{t+1}, \frac{y}{t+1})$
   \end{itemize}
   )=", sm::SIDE, 90);
      setCanvas(mode, &ccolor::football, [] {
        tour::slide_backup(pconf.model, mdHyperboloid);
        tour::slide_backup(pconf.scale, pconf.scale * 0.5);
        tour::slide_backup(pconf.ball(), cspin(1, 2, -20._deg));
        tour::slide_backup(mapeditor::drawplayer, false);
        tour::slide_backup(vid.axes, 0);
        rogueviz::rv_hook(hooks_latex_slide, 100, [] { dialog::dwidth += 500; menu_darkening++; dialog::draw_side_shade(); dialog::dwidth -= 500; menu_darkening --; });
        });
      }},

  {"anti-de Sitter spacetime", 999, LEGAL::NONE | QUICKGEO | USE_SLIDE_NAME | NOTITLE,
    "explanation",
    [] (presmode mode) {
      latex_slide(mode, defs+R"=(
   {\color{remph}anti-de Sitter spacetime:}
   \begin{itemize} 
   \item $\wadS{2} = \{(x,y,t,u) \in \bbE^{2,2}: \\ x^2+y^2-t^2-u^2=-1\}$
   \item take $u=0, t>0$ -- we get $\bbH^2$
   \item rotation in the $(t,u)$ plane \\ corresponds to the pass of time
   \item $\uadS{2}$ -- the {\color{remph}universal cover}: \\
         not a time loop of length $2\pi$, \\ but we ``unwrap'' it
   \end{itemize}
   )=", sm::SIDE | sm::NOSCR, 90);
      // if(mode == pmStart) slide_backup(nomap, true);
      static int phase = 0;
      static ld ctick;
      if(mode == pmStart) phase = 0;
      if(mode == pmKey) { phase = (1 + phase) % 3; ctick = ticks; }
      if(mode == pmStart) rogueviz::rv_hook(hooks_latex_slide, 100, [] {
        dynamicval<eGeometry> g(geometry, gCubeTiling);
        initquickqueue();
        dynamicval<ld> dw(vid.linewidth, 4);
        dynamicval<eModel> dm(pmodel, mdDisk);
        dynamicval<ld> dcmin(pconf.clip_min, -1000);
        dynamicval<ld> dcmax(pconf.clip_max, +100);
        transmatrix Rot = Id * cspin(0, 2, 5._deg) * cspin(1, 2, -15._deg);
        curvepoint(hyperpoint(2,0,0,1)); curvepoint(hyperpoint(-2,0,0,1)); queuecurve(shiftless(Rot), 0xFF, 0, PPR::LINE);
        curvepoint(hyperpoint(0,2,0,1)); curvepoint(hyperpoint(0,-2,0,1)); queuecurve(shiftless(Rot), 0xFF, 0, PPR::LINE);
        curvepoint(hyperpoint(0,0,2,1)); curvepoint(hyperpoint(0,0,-2,1)); queuecurve(shiftless(Rot), 0xFF, 0, PPR::LINE);
        // queuestr(shiftless(Rot * eupush(hyperpoint(1.75, 0.1, 0, 1))), 0.5, "t", 0);
        latex_in_space(shiftless(Rot * eupush(hyperpoint(1.75, 0.1, 0, 1))), 0.001, "$t$", 0xFF, 0);
        latex_in_space(shiftless(Rot * eupush(hyperpoint(0.15, 1.75, 0, 1))), 0.001, "$xy$", 0xFF, 0);
        latex_in_space(shiftless(Rot * eupush(hyperpoint(-0.1, 0, -1.75, 1))), 0.001, "$u$", 0xFF, 0);
        for(int y=0; y<=360; y+=15) {
          if(phase == 0 && y) continue;
          if(phase == 1 && y > (ticks - ctick) / 10.) continue;
          ld helix = min<ld>((ticks-ctick)/1000., 1); println(hlog, "helix = ", helix); helix = helix * helix * (3 - 2 * helix);
          for(int z=0; z<=360; z+=5) curvepoint(hyperpoint(1 + 0.5 * sin(z*1._deg), (phase == 2 ? -y/240. * helix :0 ) + 0.5 * cos(z*1._deg), 0, 1));
          queuecurve(shiftless(Rot * cspin(0, 2, y*1._deg)), 0xFF, 0xFFD500FF, PPR::LINE);
          }
        quickqueue();
        });
      }},

  {"de Sitter spacetime", 999, LEGAL::NONE | QUICKGEO | USE_SLIDE_NAME | NOTITLE,
    "explanation",
    [] (presmode mode) {
      latex_slide(mode, defs+R"=(
   {\color{remph}de Sitter spacetime:}
   \begin{itemize} 
   \item $\dS{2} = \{(x,y,z,t) \in \bbE^{3,1}: \\ x^2+y^2+z^2-t^2=-1\}$
   \item take $t=0$ -- we get $\bbS^2$
   \item the universe is expanding with $t$ \\ (not if we apply Lorentz transformation)
   \end{itemize}
   )=", sm::NOSCR | sm::SIDE, 90);
      static int phase = 0;
      static ld ctick;
      if(mode == pmStart) phase = 0;
      if(mode == pmKey) { phase = (1 + phase) % 3; ctick = ticks; }
      if(mode == pmStart) rogueviz::rv_hook(hooks_latex_slide, 100, [] {
        dynamicval<eGeometry> g(geometry, gCubeTiling);
        initquickqueue();
        dynamicval<ld> dw(vid.linewidth, 4);
        dynamicval<eModel> dm(pmodel, mdDisk);
        dynamicval<ld> dcmin(pconf.clip_min, -1000);
        dynamicval<ld> dcmax(pconf.clip_max, +100);
        transmatrix Rot = Id * cspin(1, 2, -120._deg) * cspin(0, 1, 30._deg);
        curvepoint(hyperpoint(2,0,0,1)); curvepoint(hyperpoint(-2,0,0,1)); queuecurve(shiftless(Rot), 0xFF, 0, PPR::LINE);
        curvepoint(hyperpoint(0,2,0,1)); curvepoint(hyperpoint(0,-2,0,1)); queuecurve(shiftless(Rot), 0xFF, 0, PPR::LINE);
        curvepoint(hyperpoint(0,0,2,1)); curvepoint(hyperpoint(0,0,-2,1)); queuecurve(shiftless(Rot), 0xFF, 0, PPR::LINE);
        // queuestr(shiftless(Rot * eupush(hyperpoint(1.75, 0.1, 0, 1))), 0.5, "t", 0);
        latex_in_space(shiftless(Rot * eupush(hyperpoint(1.75, 0.1, 0, 1)) * inverse(Rot)), 0.001, "$x$", 0xFF, 0);
        latex_in_space(shiftless(Rot * eupush(hyperpoint(0.15, -1.75, 0, 1)) * inverse(Rot)), 0.001, "$y,z$", 0xFF, 0);
        latex_in_space(shiftless(Rot * eupush(hyperpoint(-0.1, 0, -1.75, 1)) * inverse(Rot)), 0.001, "$t$", 0xFF, 0);
        for(int y=0; y<=6; y+=1) {
          ld ay = y / 3.;
          if(phase == 0 && y) continue;
          if(phase == 1 && y > (ticks - ctick) / 250.) continue;
          for(int z=0; z<=360; z+=5) curvepoint(hyperpoint(cos(z*1._deg) * cosh(ay), sin(z*1._deg) * cosh(ay), sinh(ay), 1));
          queuecurve(shiftless(Rot), 0xFF, 0xFFD500FF, PPR::LINE);
          }
        quickqueue();
        if(phase > 0) {
          glClear(GL_DEPTH_BUFFER_BIT);
          initquickqueue();
          for(int s=-5; s<=5; s++) {
            for(ld y=0; y<=2; y+=0.01) curvepoint(hyperpoint(sin(s*18._deg)*cosh(y), -cos(s*18._deg)*cosh(y), sinh(y), 1));
            queuecurve(shiftless(Rot), 0xFF8080FF, 0, PPR::LINE);
            }
          quickqueue();
          }
        if(phase == 2) {
          glClear(GL_DEPTH_BUFFER_BIT);
          initquickqueue();
          for(int y=0; y<=6; y+=1) {
            ld ay = y / 3.;
            if(phase == 2 && y > (ticks - ctick) / 250.) continue;
            for(int z=0; z<=360; z+=5) curvepoint(hyperpoint(cos(z*1._deg) * cosh(ay), sin(z*1._deg) * cosh(ay), cos(z*1._deg)*sinh(ay), 1));
            }
          queuecurve(shiftless(Rot), 0x80FF80FF, 0, PPR::LINE);
          quickqueue();
          }
        });
      }},

  {"THE END", 123, LEGAL::ANY | QUICKSKIP | NOTITLE | FINALSLIDE, "",
  
    [] (presmode mode) {
      empty_screen(mode);
      white_screen(mode);
      add_stat(mode, [] {
        dialog::init();        
        dialog::addTitle("THE END", 0x0, 200);
        dialog::addBreak(100);
        dialog::addInfo("That is all in the tour. Please play the game now!");
        dialog::display();
        return true;
        });
      }
    }  
  };  

int pohooks = 
  0 +
  addHook_slideshows(100, [] (tour::ss::slideshow_callback cb) {
    cb(XLAT("Relative Hell guided tour"), &relhell_tour[0], 'S');
    });

}

void start_relhell_tour() {
  popScreenAll();
  tour::slides = &ads_tour::relhell_tour[0];
  tour::start();
  if(!tour::on) tour::start();
  }

}
}
