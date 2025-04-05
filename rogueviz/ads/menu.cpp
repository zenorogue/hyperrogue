namespace hr {

namespace ads_game {

void adjust_for_scale() {
  ld ads_scale = get_scale();
  if(ads_scale < 0.3) max_gen_per_frame = 1, draw_per_frame = 30;
  else if(ads_scale < 0.8) max_gen_per_frame = 2, draw_per_frame = 100;
  else max_gen_per_frame = 3, draw_per_frame = 1000;
  }

vector<void*> get_all_params() {
  vector<void*> vec = {&DS_(simspeed), &DS_(accel), &DS_(how_much_invincibility), &vid.creature_scale, &DS_(missile_rapidity)};
  if(!main_rock) {
    vec.push_back(&rock_max_rapidity);
    vec.push_back(&rock_density);
    }
  auto all = [&] (player_data& d) {
    vec.push_back(&d.hitpoints);
    vec.push_back(&d.ammo);
    vec.push_back(&d.fuel);
    vec.push_back(&d.oxygen);
    };
  all(DS_(max_pdata));
  all(DS_(tank_pdata));
  return vec;
  }

bool all_params_default() {
  for(auto v: get_all_params()) {
    for(auto& fs: params)
      if(fs.second->affects(v) && fs.second->dosave()) return false;
    }
  return true;
  }

void all_params_reset() {
  for(auto v: get_all_params())
    for(auto& fs: params)
      if(fs.second->affects(v))
        fs.second->reset();
  }

void edit_difficulty() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();

  dialog::init(XLAT("game parameters"), 0xC0C0FFFF, 150, 100);
  add_edit(DS_(simspeed));
  add_edit(DS_(accel));
  add_edit(DS_(how_much_invincibility));
  add_edit(vid.creature_scale);
  add_edit(DS_(missile_rapidity));

  if(!main_rock) {
    dialog::addBreak(100);
    add_edit(rock_max_rapidity);
    add_edit(rock_density);
  
    dialog::addBreak(100);
    add_edit(max_gen_per_frame);
    add_edit(draw_per_frame);
    }

  dialog::addBreak(100);
  edit_rsrc();
  
  if(!main_rock) {
    dialog::addBreak(100);
    dialog::addItem(XLAT("scale everything :2"), '!');
    dialog::add_action([] {
      change_scale(1/2.);
      adjust_for_scale();
      restart();
      });
    dialog::addItem(XLAT("scale everything *2"), '@');
    dialog::add_action([] {
      change_scale(2.);
      adjust_for_scale();
      restart();
      });
    }

  dialog::addBoolItem(XLAT("all default"), all_params_default(), 'D');
  dialog::add_action([] { all_params_reset(); restart(); });

  dialog::addBack();
  dialog::display();
  }

void edit_particles() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();

  dialog::init(XLAT("particle/texture settings"), 0xC0C0FFFF, 150, 100);

  add_edit(crash_particle_rapidity);
  add_edit(crash_particle_qty);
  add_edit(crash_particle_life);
  add_edit(fuel_particle_rapidity);
  add_edit(fuel_particle_qty);
  add_edit(fuel_particle_life);

  if(main_rock) {
    dialog::addBreak(100);
    add_edit(XSCALE);
    add_edit(YSCALE);
    add_edit(talpha);
    }

  dialog::addBreak(100);
  dialog::addBack();
  dialog::display();
  }

void edit_view_mode() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();

  dialog::init(XLAT("spacetime views"), 0xC0C0FFFF, 150, 100);

  dialog::addBoolItem(XLAT("view replay"), in_replay, 'r');
  dialog::add_action(switch_replay);

  dialog::addBoolItem_action(XLAT("reverse replay"), rev_replay, 'w');

  dialog::addBreak(100);

  if(!main_rock) add_edit(auto_rotate);
  if(!main_rock) add_edit(auto_angle);

  dialog::addBoolItem("view the current time", !in_spacetime() && which_cross == 0, 'z');
  dialog::add_action([] {
    switch_spacetime_to(false); which_cross = 0;
    });

  dialog::addBoolItem("view the visible state", !in_spacetime() && which_cross == -1, 'x');
  dialog::add_action([] {
    switch_spacetime_to(false); which_cross = -1;
    });

  dialog::addBoolItem("view the future", !in_spacetime() && which_cross == 1, 'c');
  dialog::add_action([] {
    switch_spacetime_to(false); which_cross = 1;
    });

  dialog::addBoolItem("view the spacetime", in_spacetime() && pmodel == mdRelPerspective && !use_duality, 'v');
  dialog::add_action([] {
    switch_spacetime_to(true); pmodel = mdRelPerspective; use_duality = 0;
    });

  if(main_rock) {
    dialog::addBoolItem("view the dual space", in_spacetime() && pmodel == mdPerspective, 'v');
    dialog::add_action([] {
      switch_spacetime_to(true);
      pmodel = mdPerspective;
      });
    }

  else {
    dialog::addBoolItem("view the dual spacetime", in_spacetime() && use_duality == 1, 'b');
    dialog::add_action([] {
      switch_spacetime_to(true);
      use_duality = 1;
      });

    dialog::addBoolItem("fake dual spacetime", in_spacetime() && use_duality == 2, 'n');
    dialog::add_action([] {
      switch_spacetime_to(true);
      use_duality = 2;
      });
    }

  if(in_spacetime()) {
    add_edit(spacetime_step);
    add_edit(spacetime_qty);

    if(main_rock) {
      dialog::addColorItem(XLAT("ultra-ideal ghost color"), ghost_color, 'X');
      dialog::add_action([] { dialog::openColorDialog(ghost_color); });
      }

    }

  dialog::display();
  }

void game_menu() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();

  dialog::init(XLAT("Relative Hell"), 0xC0C0FFFF, 150, 100);
  
  dialog::addItem(XLAT("set game parameters"), 'm');
  dialog::add_action_push(edit_difficulty);
  
  add_edit(pause_speed);
  add_edit(view_proper_times);
  add_edit(DS_(time_unit));

  dialog::addItem(XLAT("set view mode"), 'v');
  dialog::add_action_push(edit_view_mode);

  dialog::addItem(XLAT("particle/texture settings"), 'p');
  dialog::add_action_push(edit_particles);

  dialog::addItem(XLAT("restart game"), 'r');
  dialog::add_action([] {
    game_over_with_message("restarted");
    save_to_hiscores();
    if(main_rock) ds_restart();
    else restart();
    popScreen(); });

  dialog::addItem(XLAT("highscores"), 'h');
  dialog::add_action_push(hiscore_menu);

  dialog::addItem(XLAT("refill cheat"), 'R');
  dialog::add_action([] { init_rsrc(); popScreen(); });
  
  dialog::addItem(XLAT("recenter cheat"), 'C');
  dialog::add_action([] { current.T = Id; vctrV = Id; });

  dialog::addItem("configure keys", 'k');
  dialog::add_action_push(multi::get_key_configurer(1, move_names, "Relative Hell keys", multi::scfg_default));

  #if CAP_AUDIO
  add_edit(effvolume);
  if(music_available) add_edit(musicvolume);
  #endif

  dialog::addItem("RogueViz settings", 'v');
  dialog::add_action([] {
    pushScreen(showSettings);
    });

  dialog::addItem("save animation/shot", 's');
  dialog::add_action([] {
    pushScreen(anims::show);
    });

  #if CAP_FILES && !ISWEB
  dialog::addItem("save the current config", 's');
  dialog::add_action([] {
    dynamicval<eGeometry> g(geometry, gNormal);
    saveConfig();
    });
  #endif

  dialog::addItem("quit the game", 'q');
  dialog::add_action([] {
    game_over_with_message("quit");
    save_to_hiscores();
    quitmainloop = true;
    });

  dialog::addBreak(100);
  dialog::addBack();
  dialog::display();
  }

void pick_the_game() {
  cmode = sm::NOSCR;
  clearMessages();
  gamescreen();

  dialog::init(XLAT("Relative Hell"), 0xC02020, 200, 0);
  dialog::addInfo(XLAT("abandon all hope of going faster than light"));
  dialog::addBreak(200);

  dialog::addBigItem("anti-de Sitter space", 'a');
  dialog::add_action([] { popScreen(); run_ads_game_std(); add_ads_cleanup(); clearMessages(); });
  dialog::addInfo(XLAT("shoot asteroids, mine resources, collect gold"));

  dialog::addBreak(100);

  dialog::addBigItem("de Sitter space", 'd');
  dialog::add_action([] { popScreen(); run_ds_game_std(); add_ds_cleanup(); clearMessages(); });
  dialog::addInfo(XLAT("avoid energy balls, but do not let the main star run away!"));

  dialog::addBreak(100);

  dialog::addBigItem("guided tour", 't');
  dialog::add_action(start_relhell_tour);
  dialog::addInfo(XLAT("but what exactly are these spaces?"));

  dialog::addBreak(100);

  dialog::addItem("not now", 'q');
  dialog::add_action([] { quitmainloop = true; });

  dialog::display();
  /* do not let the user exit this dialog */
  keyhandler = [] (int a, int b) { dialog::handleNavigation(a,b); };
  }

}}
