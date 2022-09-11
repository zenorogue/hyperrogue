namespace hr {

namespace ads_game {

void game_menu() {
  dialog::init(XLAT("AdS game settings"), 0xC0C0FFFF, 150, 100);
  
  add_edit(simspeed);
  add_edit(accel);
  add_edit(view_proper_times);
  add_edit(auto_rotate);
  
  dialog::addItem("configure keys", 'k');
  dialog::add_action_push(multi::get_key_configurer(1, move_names, "Nilrider keys"));

  #if CAP_AUDIO
  add_edit(effvolume);
  add_edit(musicvolume);
  #endif

  dialog::addItem("RogueViz settings", 'r');
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
