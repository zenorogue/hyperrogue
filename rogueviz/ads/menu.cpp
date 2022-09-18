namespace hr {

namespace ads_game {

void edit_difficulty() {
  dialog::init(XLAT("AdS game parameters"), 0xC0C0FFFF, 150, 100);
  add_edit(simspeed);
  add_edit(accel);
  add_edit(how_much_invincibility);

  dialog::addBreak(100);
  edit_rsrc();
  
  dialog::addBreak(100);
  dialog::addBack();
  dialog::display();
  }

void game_menu() {
  dialog::init(XLAT("AdS game settings"), 0xC0C0FFFF, 150, 100);
  
  dialog::addItem(XLAT("set game parameters"), 'm');
  dialog::add_action_push(edit_difficulty);
  
  add_edit(pause_speed);
  add_edit(view_proper_times);
  add_edit(time_unit);
  add_edit(auto_rotate);
  add_edit(auto_angle);

  dialog::addItem(XLAT("restart game"), 'r');
  dialog::add_action([] { ads_game::restart(); popScreen(); });

  dialog::addItem(XLAT("refill cheat"), 'R');
  dialog::add_action([] { init_rsrc(); popScreen(); });
  
  dialog::addItem("configure keys", 'k');
  dialog::add_action_push(multi::get_key_configurer(1, move_names, "Nilrider keys"));

  #if CAP_AUDIO
  add_edit(effvolume);
  add_edit(musicvolume);
  #endif

  dialog::addItem("RogueViz settings", 'v');
  dialog::add_key_action('r', [] {
    pushScreen(showSettings);
    });

  #if CAP_FILES && !ISWEB
  dialog::addItem("save the current config", 's');
  dialog::add_action([] {
    dynamicval<eGeometry> g(geometry, gNormal);
    saveConfig();
    });
  #endif

  dialog::addBreak(100);
  dialog::addBack();
  dialog::display();
  }

}}
