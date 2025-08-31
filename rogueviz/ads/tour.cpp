// Relative Hell: guided tour
// Copyright (C) 2025-2025 Zeno Rogue, see '../../hyper.cpp' for details

namespace hr {

namespace ads_game {

extern purehookset hooks_pre_ads_start;
extern bool changed_structure;

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

void straight_line_viz_rocks(presmode mode) {
  if(mode == pmStart) {
    rv_hook(shmup::hooks_draw, 100, [] (const shiftmatrix&, cell*, shmup::monster*) {
      items[itOrbAether] = 0; items[itOrbShield] = 0;
      return false;
      });
    }
  if(mode == pmFrame) {
    items[itOrbLife] = 3; if(shmup::pc[0]) shmup::pc[0]->dead = false;
    }
  if(mode == pmKey) {
    for(auto [c, mo]: shmup::monstersAt) if(mo->type == moAsteroid) mo->dead = true;
    auto pc = shmup::pc[0];
    for(int r: {1, 2, 3})
    for(int i=0; i<36*r; i++) {
      transmatrix T = spin(TAU*i/r/36) * xpush(cgi.scalefactor*r) * spin(-TAU*i/r/6);

      ld r = hypot_d(WDIM, pc->inertia), eps = 1e-3;
      transmatrix In = lrspintox(pc->inertia) * lxpush(r * eps) * lspintox(pc->inertia);

      shmup::monster* child = new shmup::monster;
      child->base = pc->base;
      child->at = pc->at * T;
      child->ori = pc->ori;
      child->type = moAsteroid;
      child->pid = pc->pid;
      child->inertia = inverse(T) * In * T * C0;
      auto f = child->inertia;
      child->inertia = lrspintox(child->inertia) * eupoint(hdist0(child->inertia) / eps, 0);
      println(hlog, "inertia = ", child->inertia, " f = ", f);
      // child->inertia = eupoint(0, 0);

      child->hitpoints = 3;
      shmup::additional.push_back(child);
      }
    if(tour_value == 1) quitmainloop = true;
    }
  }

void straight_line_viz(presmode mode) {
  if(mode == pmKey) slv_mode = (slv_mode == 0 ? 1 : 0);
  if(mode == pmStart) rogueviz::rv_hook(hooks_frame, 100, [] {
    if(inHighQual && slv_mode == 0) slv_mode = 1;
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
  tour::slide_backup(stdgridcolor, 0x808080FF);
  tour::slide_backup(vid.multiplier_grid, 3);
  tour::slide_backup(hide_kills, true);
  tour::slide_backup(hide_watermark, true);
  tour::slide_backup(gridbelow, true);
  tour::slide_backup(nomap, false);
  tour::slide_backup(mapeditor::drawplayer, true);
  }

slide relhell_tour[] = {
  {"Intro", 10, LEGAL::ANY | QUICKGEO | NOTITLE, 
    "Relative Hell is a game taking place in relativistic analogs of spherical and hyperbolic geometries. "
    "Here is Space Rocks, a clone of the classic game Asteroids. It is based on Newtonian physics: "
    "if you accelerate, you move forever in that direction, unless you deaccelerate.\n\n"
    "Note: Our video 'Non-Euclidean Game + Relativity = ?' is based on this tour, or in other words, the tour "
    "is an interactive version of the video.",
    [] (presmode mode) {
      kills[moAsteroid] = 0;
      setCanvas(mode, &ccolor::plain, [] {
        set_spacerocks_ship();
        tour::slide_backup(vid.creature_scale, 0.5);
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
      slide_url(mode, 'y', "YouTube link", "https://youtu.be/PxnoSsjMrck");

      if(mode == pmKey) open_url("https://youtu.be/PxnoSsjMrck");
      }
    },

  {"Euclidean symmetry: formation", 10, LEGAL::ANY | QUICKGEO | NOTITLE,
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
      kills[moAsteroid] = 0;
      setCanvas(mode, &ccolor::plain, [] {
        set_spacerocks_ship();
        set_geometry(gEuclidSquare);
        set_variation(eVariation::pure);
        tour::slide_backup(land_structure, lsSingle);
        tour::slide_backup(specialland, laAsteroids);
        auto& ua = euc::eu_input;
        tour::slide_backup(ua, ua);
        for(int i=0; i<2; i++)
        for(int j=0; j<2; j++) ua.user_axes[i][j] = i == j ? 35 : 0;
        ua.twisted = false;
        euc::build_torus3();
        tour::slide_backup(shmup::on, true);
        tour::slide_backup(pconf.scale, 0.3);
        tour::slide_backup(dont_gen_asteroids, true);
        tour::slide_backup(stdgridcolor, 0xC0C0C0C0);
        tour::slide_backup(vid.multiplier_grid, 3);
        tour::slide_backup(gridbelow, true);
        });
      straight_line_viz_rocks(mode);
      }
    },

  {"Small Relativistic Effects", 10, LEGAL::ANY | QUICKGEO, 
    "In our real world, the universe is expanding, and the spaceship would observe relativistic effects if it started to move very fast. "
    "Such effects can be also observed in this slide, although you still need to wait for a long time or move very fast. They will be more pronounced in Relative Hell, and in the later slides.",
    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        add_ds_cleanup();
        rogueviz::on_cleanup_or_next([] { lps_enable(nullptr); });
        ads_game::run_ds_game_std();
        const ld sca = 100;
        tour::slide_backup(ds_simspeed, M_PI / 10 / sca * 5);
        tour::slide_backup(ds_missile_rapidity, 0.1);
        tour::slide_backup(vid.creature_scale, 1 / sca);
        tour::slide_backup(pconf.scale, sca);
        tour::slide_backup(texture_off, true);
        dynamicval<ld> fs(future_shown, -10);
        ds_restart_scaled();
        tour::slide_backup(invincibility_pt, HUGE_VAL);

        rockgen.cshift = 0;

        rogueviz::rv_hook(multi::hooks_handleInput, 100, [] {
          if(tour_value == 0) return;
          auto& act = multi::action_states[1];
          if(ticks >= 1000 && ticks < 4000) act[multi::pcMoveLeft].held = true;
          if(ticks >= 5500 && ticks < 8500) act[multi::pcMoveRight].held = true;
          if(ticks >= 9500 && ticks < 12500) act[multi::pcMoveRight].held = true;
          if(ticks >=14000 && ticks < 17000) act[multi::pcMoveLeft].held = true;
          });

        if(1) {

          std::mt19937 gr;
          gr.seed(617);
          auto randd = [&] { return (gr() % 1000000 + .5) / 1000000; };

          dynamicval<eGeometry> g(geometry, gSpace435);

          for(int x=-10; x<=10; x++)
          for(int y=-10; y<=10; y++) if(hypot(x+0.5, y) >= 2) {
            rockgen.add(cspin(0, 2, (x + randd() - randd()) / sca) * cspin(1, 2, (y + randd() - randd()) / sca));
            }

          for(int x: {-2.5}) {
            auto r = rockgen.add(cspin(0, 2, x / sca) * cspin(1, 2, 0));
            r->type = oResource;
            r->resource = rtFuel;
            r->shape = rsrc_shape[rtFuel];
            r->col = rsrc_color[rtFuel];
            }
          }

        rockgen.cshift = 10;
        });
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
        rogueviz::on_cleanup_or_next([] { lps_enable(nullptr); });
        add_ds_cleanup();
        ads_game::run_ds_game_std();
        const ld sca = 100;
        tour::slide_backup(ds_simspeed, M_PI / 10 / sca * 5);
        tour::slide_backup(ds_missile_rapidity, 0.5);
        tour::slide_backup(ds_accel, ds_accel * 10);
        tour::slide_backup(vid.creature_scale, 1 / sca);
        tour::slide_backup(pconf.scale, sca);
        tour::slide_backup(texture_off, true);
        tour::slide_backup(spacetime_step, spacetime_step / sca);
        tour::slide_backup(spacetime_qty, spacetime_qty * 5);
        dynamicval<ld> fs(future_shown, -10);
        ds_restart_scaled();
        tour::slide_backup(invincibility_pt, HUGE_VAL);

        rockgen.cshift = 0;

        rogueviz::rv_hook(multi::hooks_handleInput, 100, [] {
          if(tour_value == 0) return;
          auto& act = multi::action_states[1];
          if(ticks >= 16000 && ticks < 18000) act[multi::pcMoveUp].held = true;
          });

        if(1) {

          std::mt19937 gr;
          gr.seed(617);
          auto randd = [&] { return (gr() % 1000000 + .5) / 1000000; };

          dynamicval<eGeometry> g(geometry, gSpace435);
          for(int x=-6; x<=6; x++)
          for(int y=-40; y<=10; y++) if(hypot(x+0.5, y) >= 2 && (x&1) == 1) {
            rockgen.add(cspin(0, 2, (x + (randd() - randd())/3) / sca) * cspin(1, 2, (y + randd() - randd()) / sca));
            }

          for(int x=-6; x<=6; x++)
          for(int y=60; y<=200; y++) if((x&3) == 2) {
            rockgen.add(lorentz(1, 3, -2 * ds_simspeed * ds_accel) * cspin(0, 2, (x + randd() - randd()) / sca) * cspin(1, 2, (y/2 + randd() - randd()) / sca));
            }

          if(true) {
            rockgen.add(lorentz(3, 2, 12.5 * ds_simspeed) * cspin(1, 2, 1 / sca) * cspin(0, 2, -0.5 / sca) * lorentz(1, 3, -1 * ds_simspeed * ds_accel));
            }

          for(int x=0; x<=24; x++)
          for(int y=-10; y<=10; y++) if(y) {
            rockgen.cshift = (rand() % 1000) / 100. / sca;
            rockgen.add(lorentz(3, 2, 5 * ds_simspeed) * cspin(0, 1, x * 15._deg) * cspin(1, 2, y / sca) * lorentz(0, 3, 1 + randd() * 3));
            }
          }

        rockgen.cshift = 10;
        });
      }
    },

  {"Time Dilation", 10, LEGAL::ANY | QUICKGEO, 
    "Another well-known relativistic effect is time dilation. Time passes differently for different objects.\n\n"
    "Try to accelerate, then return to the yellow star. Your clock will be different than the clock of the star.",
    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        rogueviz::on_cleanup_or_next([] { lps_enable(nullptr); });
        add_ds_cleanup();
        ads_game::run_ds_game_std();
        const ld sca = 100;
        tour::slide_backup(ds_simspeed, M_PI / 10 / sca * 5);
        tour::slide_backup(ds_missile_rapidity, 0.5);
        tour::slide_backup(ds_accel, ds_accel * 10);
        tour::slide_backup(vid.creature_scale, 5 / sca);
        tour::slide_backup(pconf.scale, sca * 2);
        tour::slide_backup(texture_off, true);
        tour::slide_backup(view_proper_times, true);
        tour::slide_backup(time_scale, 0.15);
        tour::slide_backup(disable_ds_gen, true);
        tour::slide_backup(spacetime_step, spacetime_step / sca * 3);
        tour::slide_backup(spacetime_qty, spacetime_qty * 500);
        tour::slide_backup(ship_history_period, spacetime_step);
        dynamicval<ld> fs(future_shown, -10);
        ds_restart_scaled();
        tour::slide_backup(invincibility_pt, HUGE_VAL);

        rogueviz::rv_hook(multi::hooks_handleInput, 100, [] {
          if(tour_value == 0) return;
          auto& act = multi::action_states[1];
          if(ticks >= 11000 && ticks < 13000) act[multi::pcMoveUp].held = true;
          if(ticks >= 13000 && ticks < 17000) act[multi::pcMoveDown].held = true;
          if(ticks >= 17000 && ticks < 19000) act[multi::pcMoveUp].held = true;
          });

        rockgen.cshift = 10;
        });

      static ld alpha = 0;

      add_temporary_hook(mode, hook_alter_replay, 100, [] {
        if(tour_value == 0) return;
        alpha = 0;
        if(ticks >= 9000 && ticks <= 10000) {
          ld t = (ticks - 9000) / 1000.;
          t = t * t * (3 - 2 * t);
          alpha = sin(t * TAU) * 0.5;
          println(hlog, "alpha = ", alpha);
          current.T = cspin(1, 0, alpha) * current.T;
          }

        // if(ticks >= 11000 && ticks < 13000) act[multi::pcMoveUp].held = true;
        });

      add_temporary_hook(mode, hooks_prestats, 50, [] {
        if(tour_value == 0) return false;
        dynamicval<eGeometry> g(geometry, gCubeTiling); dynamicval<eVariation> gv(variation, eVariation::pure);
        dynamicval<eModel> gm(pmodel, mdPerspective); dynamicval<ld> gs(sightranges[geometry], 50);
        dynamicval<ld> gw(vid.linewidth, 3 * vid.linewidth);
        initquickqueue();
        calcparam();
        for(int w: {0, 1}) {
          shiftmatrix T = shiftless(MirrorY * rgpushxto0(hyperpoint(-7, -4, 2.2, 1)));
          if(w == 1) {
            T = T * cspin(1, 0, -alpha);
            ld rapid_gained = 0;
            if(ticks >= 11000) rapid_gained += min(ticks - 11000, 2000);
            if(ticks >= 13000) rapid_gained -= min(ticks - 13000, 4000);
            if(ticks >= 17000) rapid_gained += min(ticks - 17000, 2000);
            rapid_gained /= 1000;
            auto rg = rapid_gained;
            rapid_gained *= ds_simspeed;
            rapid_gained *= ds_accel;
            println(hlog, "rapid_gained = ", rapid_gained, " from ", rg);
            T = T * lorentz(1, 2, -rapid_gained);
            }
          color_t col = w == 1 ? 0xFFFFFFFF : 0x8000FF;
          queueline(T * hyperpoint(-0.2, 0, 0, 1), T * hyperpoint(1.2, 0, 0, 1), col);
          curvepoint(hyperpoint(1.2, 0, 0, 1));
          curvepoint(hyperpoint(1.1, 0.05, 0, 1));
          curvepoint(hyperpoint(1.1, -0.05, 0, 1));
          curvepoint(hyperpoint(1.2, 0, 0, 1));
          queuecurve(T, 0, col, PPR::LINE);
          latex_in_space(T * eupush(hyperpoint(1.2, 0.2, 0, 1)) * MirrorY, 0.002, "$x$", col, 0);
          queueline(T * hyperpoint(0,-0.2, 0, 1), T * hyperpoint(0, 1.2, 0, 1), col);
          curvepoint(hyperpoint(0, 1.2, 0, 1));
          curvepoint(hyperpoint(+0.05, 1.1, 0, 1));
          curvepoint(hyperpoint(-0.05, 1.1, 0, 1));
          curvepoint(hyperpoint(0, 1.2, 0, 1));
          queuecurve(T, 0, col, PPR::LINE);
          latex_in_space(T * eupush(hyperpoint(0.2, 1.2, 0, 1)) * MirrorY, 0.002, "$y$", col, 0);
          queueline(T * hyperpoint(0,0,-0.2, 1), T * hyperpoint(0, 0, 1.2, 1), col);
          curvepoint(hyperpoint(0, 0, 1.2, 1));
          curvepoint(hyperpoint(+0.05, -0.05, 1.1, 1));
          curvepoint(hyperpoint(-0.05, 0.05, 1.1, 1));
          curvepoint(hyperpoint(0, 0, 1.2, 1));
          queuecurve(T, 0, col, PPR::LINE);
          latex_in_space(T * eupush(hyperpoint(0, -0.2, 1.2, 1)) * MirrorY, 0.002, "$t$", col, 0);
          }
        quickqueue();
        return true;
        });
      }
    },

  {"Spherical geometry", 10, LEGAL::ANY | QUICKGEO | NOTITLE, 
    "Relative Hell combines relativity with non-Euclidean geometry. "
    "Here is Space Rocks played in spherical geometry. It uses "
    "stereographic projection so that a big part of the sphere can be seen. (You can press '5' to switch to and from the orthogonal projection.)",
    [] (presmode mode) {
      kills[moAsteroid] = 0;
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
        tour::slide_backup(vid.aurasmoothen, 90);
        });
     if(mode == pmKey) {
        if(pconf.alpha == 1) pconf.alpha = 1000, pconf.scale = 950;
        else pconf.alpha = 1, pconf.scale = 0.5;
        }
      }
    },

  {"Spherical symmetry: formation", 10, LEGAL::ANY | QUICKGEO | NOTITLE,
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
      kills[moAsteroid] = 0;
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
        tour::slide_backup(vid.creature_scale, 0.5);
        tour::slide_backup(dont_gen_asteroids, true);
        // tour::slide_backup(stdgridcolor, 0x808080FF);
        tour::slide_backup(stdgridcolor, 0xC0C0C0C0);
        tour::slide_backup(vid.multiplier_grid, 3);
        tour::slide_backup(gridbelow, true);
        });
      straight_line_viz_rocks(mode);
      }
    },

  {"Spherical symmetry: forces", 10, LEGAL::ANY | QUICKGEO | NOTITLE,
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
      kills[moAsteroid] = 0;
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

  {"empty de Sitter spacetime", 10, LEGAL::ANY | QUICKGEO,
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
        tour::slide_backup(disable_ds_gen, true);
        dynamicval<ld> fs(future_shown, -20);
        ds_restart();

        rockgen.cshift = 20;
        rsrcgen.cshift = 20;
        });
      if(mode == pmStart) {
        add_ds_cleanup();
        rogueviz::on_cleanup_or_next([] { lps_enable(nullptr); });
        }
      }
    },

  {"full de Sitter game", 10, LEGAL::ANY | QUICKGEO,
    "And here is the full game. If required, you can "
    "shoot down stars with a limited number of missiles. For high score, you will also need to replenish your "
    "resources by capturing free-flying fuel, oxygen, health, and missiles.",

    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        add_ds_cleanup();
        rogueviz::on_cleanup_or_next([] { lps_enable(nullptr); });
        ads_game::run_ds_game_std();
        });
      }
    },

  {"Hyperbolic geometry", 10, LEGAL::ANY | QUICKGEO | NOTITLE, 
    "Hyperbolic geometry is the opposite of spherical geometry. "
    "Here is Space Rocks played in it. We use the Poincaré model to display the hyperbolic plane; "
    "you can press 5 to switch to the Beltrami-Klein model.\n\n",

    [] (presmode mode) {
      kills[moAsteroid] = 0;
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

  {"Hyperbolic symmetry: formation", 10, LEGAL::ANY | QUICKGEO | NOTITLE,
    "Of course, just like in spherical space, this is not a symmetric spacetime.\n\n"
    "Press 5 to see a visualization of how various parts of the ships would move if they actually moved in straight lines.",

    [] (presmode mode) {
      kills[moAsteroid] = 0;
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
        tour::slide_backup(vid.creature_scale, 0.3);
        // tour::slide_backup(stdgridcolor, 0x808080FF);
        tour::slide_backup(stdgridcolor, 0xC0C0C0C0);
        tour::slide_backup(vid.multiplier_grid, 3);
        tour::slide_backup(gridbelow, true);
        });
     straight_line_viz_rocks(mode);
     }
    },

  {"Hyperbolic symmetry: forces", 10, LEGAL::ANY | QUICKGEO | NOTITLE,
    "Of course, just like in spherical space, this is not a symmetric spacetime.\n\n"
    "Press 5 to see a visualization of how various parts of the ships would move if they actually moved in straight lines.",

    [] (presmode mode) {
      kills[moAsteroid] = 0;
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

  {"anti-de Sitter spacetime: rocks", 10, LEGAL::ANY | QUICKGEO | NOTITLE,
    "anti-de Sitter spacetime is a way to add time to this space in a symmetric way.\n\n"
    "Because of how the anti-de Sitter spacetime works, faraway objects are 'pulled' towards us. "
    "Here is an almost empty anti-de Sitter space, with some rocks. See how they are pulled towards "
    "the center."
    ,

    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        rogueviz::on_cleanup_or_next([] { lps_enable(nullptr); });
        ads_game::run_ads_game_std();
        stop_game();
        run_ads_game();
        tour::slide_backup(changed_structure, true);
        tour::slide_backup(specialland, laAsteroids);
        // tour::slide_backup(specialland, laCrossroads);
        tour::slide_backup(land_structure, lsSingle);
        run_ads_game();
        /* disable everything */
        tour::slide_backup(pconf.alpha, 1);
        tour::slide_backup(keep_ship_angle, true);

        rogueviz::rv_hook(hooks_frame, 100, [] {
          for(int i=0; i<360; i++) addaura(shiftless(xspinpush0(i*1._deg, 1)), 0xFFFFFF, 0);
          });

        for(int i=0; i<50; i++) hybrid::in_actual([&] {
          add_rock(cwt.at, ci_at[cwt.at],
            // ads_matrix(Id) * spin(rand() % 100) * twist::uxpush(0.5 + (rand() % 100)/50.) * spin(rand() % 2 ? -90._deg : 90._deg) * lorentz(0, 3, 0.2 + (rand() % 100) / 100.)
            ads_matrix(Id) * spin(rand() % 100) * lorentz(0, 3, 0.5 + (rand() % 100)/40.) * spin(rand() % 2 ? -90._deg : 90._deg) * lorentz(0, 2, 0.2 + (rand() % 100) / 50.)
            );
          });

       });
     }
    },

  {"anti-de Sitter spacetime: missiles", 10, LEGAL::ANY | QUICKGEO | NOTITLE,
    "You can also see this effect by shooting a missile -- it will eventually return to us!\n\n"
    "Then, press 5 to see a replay from the missile's point of view."
    ,

    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        rogueviz::on_cleanup_or_next([] { lps_enable(nullptr); });
        ads_game::run_ads_game_std();
        stop_game();
        run_ads_game();
        tour::slide_backup(changed_structure, true);
        tour::slide_backup(specialland, laAsteroids);
        // tour::slide_backup(specialland, laCrossroads);
        tour::slide_backup(land_structure, lsSingle);
        run_ads_game();
        /* disable everything */
        tour::slide_backup(pconf.alpha, 1);
        tour::slide_backup(keep_ship_angle, true);

        rogueviz::rv_hook(hooks_frame, 100, [] {
          for(int i=0; i<360; i++) addaura(shiftless(xspinpush0(i*1._deg, 1)), 0xFFFFFF, 0);
          });

        tour::slide_backup(time_shift, 0.2);
        tour::slide_backup(ads_time_unit, 1);
        tour::slide_backup(view_proper_times, true);
        });
     if(mode == pmStart) tour::slide_backup(missile_replay, missile_replay);
     if(mode == pmKey) {
       switch_replay();
       missile_replay = !missile_replay;
       }
     if(mode == pmStop && in_replay) switch_replay();
     }
    },

  {"anti-de Sitter spacetime: world", 10, LEGAL::ANY | QUICKGEO | NOTITLE,
    "In the world of Relative Hell, this pull is countered by making the static objects rotate in a specific way -- this creates a centrifugal "
    "force which counterbalances this effect. As you can see, the heptagons further away are "
    "squashed -- this is again the Lorentz contraction\n\n."
    "You can also press key '5' to switch to the Beltrami-Klein projection -- "
    "this counterbalances the squashing, making all the heptagons normal."
    ,

    [] (presmode mode) {
      setCanvas(mode, &ccolor::plain, [] {
        rogueviz::on_cleanup_or_next([] { lps_enable(nullptr); });
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
        rogueviz::on_cleanup_or_next([] { lps_enable(nullptr); });
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
        rogueviz::on_cleanup_or_next([] { lps_enable(nullptr); });
        tour::slide_backup(turret_limit, 1);
        ads_game::run_ads_game_std();
        tour::slide_backup(pconf.alpha, 1);
        rv_hook(hooks_pre_ads_start, 100, [] {
          tour::slide_backup(specialland, laHunting);
          tour::slide_backup(firstland, laHunting);
          tour::slide_backup(land_structure, lsSingle);
          });
        ads_game::ads_restart();
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
        rogueviz::on_cleanup_or_next([] { lps_enable(nullptr); });
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
    "OK, so let us think what the Euclidean geometry is.\n\n"
    "Let us focus on three-dimensional Euclidean geometry. "
    "We need to define what points are in our space, and how to compute distances between them. "
    "This, in turns, let us define 'isometries' (rotations, etc.) which are basically transformations of "
    "the space that keep the distance.\n\nThis template will be also used in other geometries\n\n"
    "Press 5 to animate a rotation. (Note: the further slides also have similar animations.)",
    [] (presmode mode) {
      setCanvas(mode, &ccolor::chessboard, [] { set_geometry(gEuclidSquare); set_variation(eVariation::pure); });
      latex_slide(mode, defs+R"=(
   {\color{remph}3-dimensional Euclidean space:}
   \begin{itemize}
   \item $\bbE^3 = \{(x,y,z): x,y,z \in \bbR\}$
   \item squared distance between \\ points $(x_1,y_1,z_1)$ and $(x_2, y_2, z_2)$ is \[(x_1-x_2)^2+(y_1-y_2)^2+(z_1-z_2)^2\rule{3cm}{0cm}\]
   \item {\color{remph} isometries} (rotations, etc.) \\ preserve this squared distance
   \end{itemize}
   )=", sm::SIDE, 90);
      static int start = -1;
      static int anim_start = -1;
      if(mode == pmKey) start = (start == -1) ? ticks : -1;
      if(mode == pmKeyAlt) anim_start = ticks;
      if(mode == pmStart) {
        tour::slide_backup(mapeditor::drawplayer, false);
        tour::slide_backup(vid.axes, 0);
        tour::slide_backup(pconf.scale, 0.5);
        tour::slide_backup(vid.use_smart_range, 2);
        tour::slide_backup(vid.smart_range_detail, 1);
        tour::slide_backup(pconf.xposition, -0.4);
        rogueviz::rv_hook(hooks_latex_slide, 100, [] { dialog::dwidth += 500; menu_darkening++; dialog::draw_side_shade(); dialog::dwidth -= 500; menu_darkening --; });
        View = Id;
        static ld t;
        rogueviz::rv_hook(anims::hooks_anim, 101, [] {

          if(anim_start >= 0) {
            ld t = (ticks - anim_start) / 4000.;
            if(t >= 1) t = 1;
            centerover = currentmap->gamestart();
            View = ypush(t * t * (3 - 2 * t));
            }

          if(start != -1) {
            t = (ticks - start) / 5000.;
            if(t < 1) t *= t;
            else t = 2 * t - 1;
            }
          View = spin(t) * View;
          anims::moved();
          });

        rogueviz::rv_hook(hooks_frame, 101, [] {

          if(!nomap) for(int s: {0, 1}) {
            color_t axecolor = s == 0 ? 0x80C080FF : 0x008000FF;
            shiftmatrix S = shiftless(s == 0 ? View : Id);
            vid.linewidth *= 3;

            if(s == 0 && (tour_value != 1 || ticks >= 13000)) {
              vid.linewidth *= 3;
              for(ld v=0; v<=TAU; v+=0.01) curvepoint(hyperpoint(sin(v), cos(v), 1, 1)); queuecurve(S, 0xFFC0C0FF, 0, PPR::LINE);
              vid.linewidth /= 3;
              }

            curvepoint(hyperpoint(2.1,0,1,1)); curvepoint(hyperpoint(-2.1,0,1,1)); queuecurve(S, axecolor, 0, PPR::LINE);
            curvepoint(hyperpoint(0,2.1,1,1)); curvepoint(hyperpoint(0,-2.1,1,1)); queuecurve(S, axecolor, 0, PPR::LINE);
            latex_in_space(S * eupush(hyperpoint(2.2, 0.2, 1, 1)), 0.002, "$x$", axecolor, 0);
            latex_in_space(S * eupush(hyperpoint(0.2, -2.2, 1, 1)), 0.002, "$y$", axecolor, 0);
            for(int x: {-2, -1, 1, 2}) {
              curvepoint(hyperpoint(x,0.1,1,1)); curvepoint(hyperpoint(x,-0.1,1,1)); queuecurve(S, axecolor, 0, PPR::LINE);
              curvepoint(hyperpoint(0.1,x,1,1)); curvepoint(hyperpoint(-0.1,x,1,1)); queuecurve(S, axecolor, 0, PPR::LINE);
              }
            vid.linewidth /= 3;
            curvepoint(hyperpoint(2.1,0.1,1,1)); curvepoint(hyperpoint(2.3,0,1,1)); curvepoint(hyperpoint(2.1,-0.1,1,1));
            queuecurve(S, 0, axecolor, PPR::LINE);
            curvepoint(hyperpoint(0.1,-2.1,1,1)); curvepoint(hyperpoint(0,-2.3,1,1)); curvepoint(hyperpoint(-0.1,-2.1,1,1));
            queuecurve(S, 0, axecolor, PPR::LINE);
            }
          });
        }
      }},

  {"Minkowski geometry", 999, LEGAL::NONE | QUICKGEO | USE_SLIDE_NAME | NOTITLE, 
    "The Minkowski geometry is similar to Euclidean geometry, except that in the squared distance formula, "
    "the square of the time difference has a different sign. Thus, we have different isometries, which "
    "can turn space to time and vice versa, just like Euclidean rotations turned X to Y and vice versa. "
    "Because of the different sign, these 'Lorentz transformations' work different -- for example, they are not based on sin and cos, "
    "but sinh and cosh.\n\n"
    "Just like Euclidean geometry, Minkowski geometry is maximally symmetric: spacetime directions can be classified as space-like (squared distance > 0), "
    "light-like (squared distance = 0) and time-like (squared distance < 0), but if we have a point and direction, we have an isometry that "
    "takes it into any other point and direction of the same type.",
    [] (presmode mode) {
      string s = "";
      if(!tour_value || ticks >= 9000) {
        s = "\\definecolor{rcsl}{rgb}{0,0.75,0}\n\\definecolor{rtsl}{rgb}{0,0,1}\n\\definecolor{rsign}{rgb}{1,0.25,0.25}\n";
        if(ticks >= 19000 || !tour_value)
          s += "\\def\\csl#1{{\\color{rcsl}#1}}\n\\def\\tsl#1{{\\color{rtsl}#1}}\n\\def\\hlsign#1{{\\color{rsign}#1}}\n";
       else if(ticks >= 11000)
          s += "\\def\\csl#1{{\\color{rcsl}#1}}\n\\def\\tsl#1{{\\color{rtsl}#1}}\n\\def\\hlsign#1{{#1}}\n";
       else if(ticks >= 9000)
          s += "\\def\\csl#1{{\\color{rcsl}#1}}\n\\def\\tsl#1{{#1}}\n\\def\\hlsign#1{{#1}}\n";
        }
      else {
        s =  "\\def\\csl#1{{#1}}\n\\def\\tsl#1{{#1}}\n\\def\\hlsign#1{{#1}}\n";
        }
      latex_slide(mode, defs+s+R"=(
   {\color{remph}Minkowski spacetime with 2 space and 1 time dimension:}
   \begin{itemize}
   \item $\bbE^{2,1} = \{(\csl{x,y},\tsl{t}): \csl{x,y},\tsl{t} \in \bbR\}$
   \item spacetime interval (``squared distance'') between \\ points $(\csl{x_1,y_1},\tsl{t_1})$ and $(\csl{x_2, y_2}, \tsl{t_2})$ is \[\csl{(x_1-x_2)^2\hlsign{+}(y_1-y_2)^2}\tsl{\hlsign{-}(t_1-t_2)^2}\rule{3cm}{0cm}\]
   \item {\color{remph} Lorentz transformations} preserve this
   \end{itemize}
   \rule{0cm}{3cm}
   )=", sm::SIDE, 90);
      setCanvas(mode, &ccolor::chessboard, [] { set_geometry(gEuclidSquare); set_variation(eVariation::pure); tour::slide_backup(vid.axes, 0); });
      static int start = -1;
      static int hilite = 0;
      if(mode == pmKey) start = (start == -1) ? ticks : -1;
      if(mode == pmKeyAlt) hilite++;
      if(mode == pmStart) {
        tour::slide_backup(anims::ma, anims::maTranslation);
        tour::slide_backup(pconf.stretch, 1);
        tour::slide_backup(anims::movement_angle.get(), spin(-90._deg));
        tour::slide_backup(anims::cycle_length, 0);
        tour::slide_backup(mapeditor::drawplayer, false);
        tour::slide_backup(vid.axes, 0);
        tour::slide_backup(vid.use_smart_range, 2);
        tour::slide_backup(vid.smart_range_detail, 1);
        static ld t;
        rogueviz::rv_hook(hooks_frame, 101, [] {
          if(start == -1) { anims::cycle_length = 0; pconf.stretch = 1; t = 0; return; }
          t = (ticks - start) / 5000.;
          if(tour_value && t > 1) t = 1;
          if(tour_value && ticks - start > 45000)
            t = t - (ticks - start - 45000) / 5000.;
          t = asinh(t);
          anims::cycle_length = sinh(t) * 10 * anims::period / 10000;
          pconf.stretch = sqrt(1 - tanh(t) * tanh(t));
          println(hlog, "t=", t, "sinh = ", anims::cycle_length, " stretch = ", pconf.stretch);
          });
        rogueviz::rv_hook(hooks_latex_slide, 100, [] { 
          initquickqueue();
          dynamicval<ld> s(pconf.stretch, 1);
          drawMonsterType(moRunDog, nullptr, shiftless(spin(t >= 0 ? 90._deg : -90._deg)), 0xFFFFFFFF, start >= 0 ? (ticks-start) / 500. : 0, 0xFFFFFFFF);

          sortquickqueue();
          quickqueue();
          dialog::dwidth += 500; menu_darkening++; dialog::draw_side_shade(); dialog::dwidth -= 500; menu_darkening --;
          });
        rogueviz::rv_hook(hooks_post_latex_slide, 100, [] {
          dynamicval<ld> s(pconf.stretch, 1);
          if(true) for(int s: {0, 1}) {
            vid.linewidth *= 2;
            initquickqueue();
            color_t axecolor = s == 0 ? 0x80C080FF : 0x00C000FF;
            color_t axecolor2 = s == 0 ? 0x8080C0FF : 0x0000FFFF;
            shiftmatrix S = shiftless(eupush(3, 1.1) * euscale(0.4, 0.4));

            ld gs = 2.45;
            if(s == 0 && (!tour_value || ticks >= 32000)) for(int a=0; a<4; a++) {
              vid.linewidth *= 3;
              for(ld v=-1.5; v<=1.5; v+=0.01) curvepoint(hyperpoint(sinh(v), cosh(v), 1, 1)); queuecurve(S * spin(90._deg*a), 0xFFC0C0FF, 0, PPR::LINE);
              vid.linewidth /= 3;
              }
            if(s==0) if(hilite == 3 || hilite == 4 || !tour_value) {
              curvepoint(hyperpoint(gs,gs,1,1)); curvepoint(hyperpoint(-gs,gs,1,1)); curvepoint(hyperpoint(0,0,1,1)); queuecurve(S, 0, axecolor2 & 0xFFFFFF80, PPR::LINE);
              curvepoint(hyperpoint(gs,-gs,1,1)); curvepoint(hyperpoint(-gs,-gs,1,1)); curvepoint(hyperpoint(0,0,1,1)); queuecurve(S, 0, axecolor2 & 0xFFFFFF80, PPR::LINE);
              }
            if(s==0) if(hilite == 1 || hilite == 4 || !tour_value) {
              curvepoint(hyperpoint(gs,gs,1,1)); curvepoint(hyperpoint(gs,-gs,1,1)); curvepoint(hyperpoint(0,0,1,1)); queuecurve(S, 0, axecolor & 0xFFFFFF80, PPR::LINE);
              curvepoint(hyperpoint(-gs,-gs,1,1)); curvepoint(hyperpoint(-gs,gs,1,1)); curvepoint(hyperpoint(0,0,1,1)); queuecurve(S, 0, axecolor & 0xFFFFFF80, PPR::LINE);
              }
            if(s==0) if(hilite == 2 || hilite == 4 || !tour_value) {
              vid.linewidth *= 3;
              curvepoint(hyperpoint(gs,gs,1,1)); curvepoint(hyperpoint(-gs,-gs,1,1)); curvepoint(hyperpoint(0,0,1,1)); queuecurve(S, 0x00C0C0FF, 0, PPR::LINE);
              curvepoint(hyperpoint(gs,-gs,1,1)); curvepoint(hyperpoint(-gs,gs,1,1)); curvepoint(hyperpoint(0,0,1,1)); queuecurve(S, 0x00C0C0FF, 0, PPR::LINE);
              vid.linewidth /= 3;
              }

            if(s == 0) S = S * lorentz(0, 1, t);
            vid.linewidth *= 3;
            curvepoint(hyperpoint(2.1,0,1,1)); curvepoint(hyperpoint(-2.1,0,1,1)); queuecurve(S, axecolor, 0, PPR::LINE);
            curvepoint(hyperpoint(0,2.1,1,1)); curvepoint(hyperpoint(0,-2.1,1,1)); queuecurve(S, axecolor2, 0, PPR::LINE);
            latex_in_space(S * eupush(hyperpoint(2.2, 0.2, 0, 1)), 0.002, "$y$", axecolor, 0);
            latex_in_space(S * eupush(hyperpoint(0.2, -2.2, 0, 1)), 0.002, "$t$", axecolor2, 0);
            for(int x: {-2, -1, 1, 2}) {
              curvepoint(hyperpoint(x,0.1,1,1)); curvepoint(hyperpoint(x,-0.1,1,1)); queuecurve(S, axecolor, 0, PPR::LINE);
              curvepoint(hyperpoint(0.1,x,1,1)); curvepoint(hyperpoint(-0.1,x,1,1)); queuecurve(S, axecolor2, 0, PPR::LINE);
              }
            vid.linewidth /= 3;
            curvepoint(hyperpoint(2.1,0.1,1,1)); curvepoint(hyperpoint(2.3,0,1,1)); curvepoint(hyperpoint(2.1,-0.1,1,1));
            queuecurve(S, 0, axecolor, PPR::LINE);
            curvepoint(hyperpoint(0.1,-2.1,1,1)); curvepoint(hyperpoint(0,-2.3,1,1)); curvepoint(hyperpoint(-0.1,-2.1,1,1));
            queuecurve(S, 0, axecolor2, PPR::LINE);
            quickqueue();
            vid.linewidth /= 2;
            }

          });
        }
      }},

  {"spherical geometry", 999, LEGAL::NONE | QUICKGEO | USE_SLIDE_NAME | NOTITLE, 
    "Now, let us discuss how spherical and hyperbolic geometries are obtained. Spherical "
    "is quite straightforward: we get the spherical geometry by restricting to the set of points "
    "in distance 1 from the chosen center, and also distances are the arc lengths. Just like "
    "Euclidean and Minkowski geometry, spherical geometry is maximally symmetric: every point and "
    "every direction works the same.\n\n"
    "The next slide gives a similar description of hyperbolic geometry.",
    [] (presmode mode) {
      setCanvas(mode, &ccolor::football, [] { set_geometry(gSphere); });
      if(mode == pmStart) {
        tour::slide_backup(pconf.scale, 500);
        tour::slide_backup(pconf.alpha, 1000);
        tour::slide_backup(mapeditor::drawplayer, false);
        tour::slide_backup(vid.axes, 0);
        tour::slide_backup(pconf.xposition, -0.25);
        tour::slide_backup(anims::ma, anims::maTranslation);
        tour::slide_backup(anims::cycle_length, 0);
        }
      if(mode == pmKey) anims::cycle_length = 10 - anims::cycle_length;
      latex_slide(mode, defs+R"=(
   {\color{remph}2-dimensional sphere:}
   \begin{itemize}
   \item $\bbS^2 = \{(x,y,z) \in \bbE^3: x^2+y^2+z^2=1\}$
   \item distances measured as \\ the lengths of curves in Euclidean space
   \item {\color{remph} isometries} (rotations, etc.) keep this distance
   \end{itemize}
   )=", sm::SIDE, 90);
      static bool arcs = false;
      if(mode == pmKeyAlt) arcs = !arcs;
      if(mode == pmStart) rogueviz::rv_hook(hooks_post_latex_slide, 100, [] {
        if(!arcs) return;
        initquickqueue();
        vid.linewidth *= 15;
        for(int a=0; a<=6; a++) {
          for(ld v=-1; v<=1; v+=0.01) curvepoint(cspin(0, 2, 15._deg * a) * hyperpoint(-cos(v), sin(v), 0, 1));
          queuecurve(shiftless(Id), 0xFF8080FF, 0, PPR::LINE);
          }
        vid.linewidth /= 15;
        quickqueue();
        });
      }},    

  {"hyperbolic geometry", 999, LEGAL::NONE | QUICKGEO | USE_SLIDE_NAME | NOTITLE,
    "To get hyperbolic geometry, we also restrict to the set of points in the same squared distance, "
    "but now we start with Minkowski geometry, and the 'squared radius' is negative (time-like). "
    "The obtained maximally symmetric manifold thus loses its time-like dimension and is purely a space.\n\n"
    "Therefore, in this model, every point in two-dimensional hyperbolic space is described with three "
    "coordinates. This may look scary, but actually is very similar to how spherical geometry works, "
    "we just need to use sinh and cosh, not sin and cos. The usual 3D graphics "
    "also employ an extra coordinate, and it is straightforward to apply 3D engines to work with "
    "spherical and hyperbolic geometry too, using these models.",
    [] (presmode mode) {
      string s = "";
      if(!tour_value || ticks >= 13000) {
        s = "\\definecolor{rcsl}{rgb}{0,0.75,0}\n\\definecolor{rtsl}{rgb}{0,0,1}\n\\definecolor{rsign}{rgb}{1,0.25,0.25}\n";
        s += "\\def\\csl#1{{\\color{rcsl}#1}}\n\\def\\tsl#1{{\\color{rtsl}#1}}\n\\def\\hlsign#1{{\\color{rsign}#1}}\n";
        }
      else {
        s =  "\\def\\csl#1{{#1}}\n\\def\\tsl#1{{#1}}\n\\def\\hlsign#1{{#1}}\n";
        }
      latex_slide(mode, defs+s+R"=(
   {\color{remph}2-dimensional hyperbolic space \\ (Minkowski hyperboloid model):}
   \begin{itemize} 
   \item $\bbH^2 = \{(\csl{x,y},\tsl{t}) \in \bbE^{2,1}: \csl{x^2\hlsign{+}y^2}\hlsign{-}\tsl{t^2}=-1, t>0\}$
   \item distances measured as \\ the lengths of curves in $\bbE^{2,1}$
   \item {\color{remph} isometries} (rotations, etc.) keep this distance
   \item we get the Poincaré model by projecting \\ $(x,y,t) \mapsto (\frac{x}{t+1}, \frac{y}{t+1})$
   \end{itemize}
   )=", sm::SIDE, 90);
      setCanvas(mode, &ccolor::football, [] {
        tour::slide_backup(pconf.model, mdHyperboloid);
        tour::slide_backup(pconf.scale, pconf.scale * 1);
        tour::slide_backup(vid.multiplier_ring, 3);
        tour::slide_backup(pconf.ball(), cspin(1, 2, -20._deg));
        tour::slide_backup(mapeditor::drawplayer, false);
        tour::slide_backup(vid.axes, 0);
        rogueviz::rv_hook(hooks_latex_slide, 100, [] { dialog::dwidth += 500; menu_darkening++; dialog::draw_side_shade(); dialog::dwidth -= 500; menu_darkening --; });
        tour::slide_backup(anims::ma, anims::maTranslation);
        tour::slide_backup(anims::cycle_length, 0);
        });
      if(mode == pmKey) anims::cycle_length = 10 - anims::cycle_length;
      }},

  {"anti-de Sitter geometry", 999, LEGAL::NONE | QUICKGEO | USE_SLIDE_NAME | NOTITLE,
    "Here is how we add a time coordinate to the hyperbolic plane, in order to get 2+1D anti-de Sitter spacetime. "
    "As you can see, the construction is quite similar, and again, we get a maximally symmetric spacetime.\n\n"
    "Press 5 for an animated visualization of this construction. Initially you see the hyperbolic plane at time 0 (u=0, t>0). "
    "First '5' adds the different time slices to the visualization, and the second '5' unwraps it into the universal cover.\n\n"
    "Note: the construction is quite similar to that of the Thurston geometry 'universal cover of SL(2,R)' -- in fact, Relative Hell "
    "uses the RogueViz implementation of that space. However, the angular coordinate becomes time-like, making our spacetime to be "
    "much more symmetric, and the geodesics work in a much more intuitive way.",
    [] (presmode mode) {
      string s = "\\definecolor{rcsl}{rgb}{0,0.75,0}\n\\definecolor{rtsl}{rgb}{0,0,1}\n\\definecolor{rsign}{rgb}{1,0.25,0.25}\n";
      s += "\\def\\csl#1{{\\color{rcsl}#1}}\n\\def\\tsl#1{{\\color{rtsl}#1}}\n\\def\\hlsign#1{{\\color{rsign}#1}}\n";
      latex_slide(mode, defs+s+R"=(
   {\color{remph}anti-de Sitter spacetime:}
   \begin{itemize} 
   \item $\wadS{2} = \{(\csl{x,y},\tsl{t,u}) \in \bbE^{2,2}: \\ \csl{x^2\hlsign{+}y^2}\hlsign{-}\tsl{t^2\hlsign{-}u^2}=-1\}$
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

  {"de Sitter spacetime geometry", 999, LEGAL::NONE | QUICKGEO | USE_SLIDE_NAME | NOTITLE,
    "And here is how we add a time coordinate to 2D spherical geometry, to get 2+1D de Sitter spacetime. "
    "The construction is actually very similar to three-dimensional hyperbolic plane, but now the "
    "'squared radius' is space-like. So we get a maximally symmetric spacetime again.\n\n"
    "Again, you see the slice t=0 -- press '5' to see how the universe expands, and '5' again to see how "
    "it looks from the point of view of an inhabitant -- the whole 'sphere' does not expand.",

    [] (presmode mode) {
      string s = "\\definecolor{rcsl}{rgb}{0,0.75,0}\n\\definecolor{rtsl}{rgb}{0,0,1}\n\\definecolor{rsign}{rgb}{1,0.25,0.25}\n";
      s += "\\def\\csl#1{{\\color{rcsl}#1}}\n\\def\\tsl#1{{\\color{rtsl}#1}}\n\\def\\hlsign#1{{\\color{rsign}#1}}\n";
      latex_slide(mode, defs+s+R"=(
   {\color{remph}de Sitter spacetime:}
   \begin{itemize} 
   \item $\dS{2} = \{(\csl{x,y,z},\tsl{t}) \in \bbE^{3,1}: \\ \csl{x^2\hlsign{+}y^2\hlsign{+}z^2}\hlsign{-}\tsl{t^2}=1\}$
   \item take $t=0$ -- we get $\bbS^2$
   \item the universe is expanding with $t$ \\ (not if we apply \\ the Lorentz transformation)
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
  0 + arg::add3("-contrep", [] {
    cmode = sm::NORMAL; for(int a=0; a<3000; a++) println(hlog, a), paused = false, game_over = false, have_crashes = false, ds_turn(10), ticks += 10;
    switch_replay();
    switch_spacetime_to(true);
    pmodel = mdRelPerspective; use_duality = false;
    })
  + arg::add3("-ads-missile-replay", [] { missile_replay = true; })
  + arg::add3("-stereo-transition", [] {
    arg::shift(); int len = arg::argi();
    static int tstart = 999000;

    rogueviz::rv_hook(hooks_handleKey, 101, [] (int sym, int uni) {
      println(hlog, "uni = ", uni);
      if((cmode & sm::NORMAL) && uni == 'y') {
        tstart = tstart > ticks ? ticks : 999000;
        return true;
        }
      return false;
      });
    rv_hook(anims::hooks_anim, 102, [len] {
      ld t = (ticks - tstart) * 1. / len; flip_limit = 0.9;
      if(t < 0) { pconf.alpha = 1000; pconf.scale = 950; vid.yshift = 0; pconf.stretch = 1; backbrightness = 0.25; }
      else if(t > 1) { pconf.alpha = 1; pconf.scale = 0.95; pconf.stretch = 1; vid.yshift = M_PI; backbrightness = 1; }
      else {
        ld t1 = t; t1 = t1 * t1 * (3 - 2 * t1);
        pconf.alpha = exp(log(1000) * (1 - t1));
        pconf.scale = 0.95 * pconf.alpha;
        pconf.stretch = 1;
        vid.yshift = M_PI * t1;
        backbrightness = lerp(0.25, 1, t1);
        }
      });
    })
  + arg::add3("-ads-resource", [] {
    static int howmany = -2;
    if(howmany == -2) rogueviz::rv_hook(hooks_prestats, 101, [] {
      if(howmany <= 0) return false;
      vector<eResourceType> res = {rtFuel, rtOxygen, rtHull, rtAmmo};
      flat_model_enabler fm;
      dynamicval<eGeometry> g(geometry, gEuclid);
      initquickqueue();
      int index = 0;
      for(auto which: res) {
        auto sh = *rsrc_shape[which];
        for(int s: {0, 1}) for(int dx=-5; dx<=5; dx++) for(int dy=-5; dy<=5; dy++) {
          if(s == 1 && (dx || dy)) continue;
          if(s == 0 && (dx*dx+dy*dy >= 20)) continue;
          for(int i=0; i<isize(sh); i+=2)
            curvepoint(hyperpoint(sh[i], sh[i+1], 1, 1));
          queuecurve(atscreenpos(vid.xres * (.63+.1 * index) + dx, vid.yres * .8 + dy) * euscalexx(1000) * spin90(), 0, s == 0 ? 0xFF : rsrc_color[which], PPR::LINE);
          }
        index++; if(index == howmany) break;
        }
      quickqueue();
      return true;
      });
    arg::shift(); howmany = arg::argi();
    })
  + addHook_slideshows(80, [] (tour::ss::slideshow_callback cb) {
    cb(XLAT("Relative Hell guided tour"), &relhell_tour[0], 'S');
    });

}

void start_relhell_tour() {
  lps_enable(nullptr);
  popScreenAll();
  tour::slides = &ads_tour::relhell_tour[0];
  tour::start();
  if(!tour::on) tour::start();
  }

}
}
