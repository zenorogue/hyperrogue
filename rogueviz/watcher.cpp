// RogueViz
// Copyright (C) 2011-2025 Zeno Rogue, see 'hyper.cpp' for details

// adds a menu item that allows to select one of multiple visualizations based on a command file
// (we use this for the visualization of SAG connectomes -- it should be made more general)

#include "rogueviz.h"

namespace rogueviz {

vector<string> breakspace(string s) {
  vector<string> parsed;
  string cur = "";
  bool inquote = false;
  for(char c: s + " ")
    if(c == 10 || c == 13) ;
    else if(c == '"') inquote = !inquote;
    else if(c == ' ' && !inquote) { parsed.push_back(cur); cur = ""; } else { cur += c; }
  if(inquote) throw hr_exception("quote did not end");
  return parsed;
  }

vector<string> concat(vector<string> a, const vector<string>& b) {
  for(auto s: b) a.push_back(s);
  return a;
  }

void read_commands(string fname) {
  fhstream f(fname, "rt");
  if(!f.f) throw hr_exception("cannot open command file");
  string title = scanline(f);
  string keystr = scanline(f);
  char key = keystr[0];

  auto pre = breakspace(scanline(f));
  auto post = breakspace(scanline(f));

  vector<pair<string, vector<string>>> commands;
  while(!feof(f.f)) {
    string head = scanline(f);
    if(head == "") continue;
    string cmds = scanline(f);
    commands.emplace_back(head, concat(concat(pre, breakspace(cmds)), post));
    }

  addHook(dialog::hooks_display_dialog, 100, [title, key, commands] () {
    if(current_screen_cfunction() == showGameMenu) {
      dialog::addItem(title, key); 
      dialog::add_action_push([title, &commands] {
        dialog::init(title);
        dialog::start_list(900, 900, '1');
        for(auto& cmd: commands) {
          dialog::addItem(cmd.first, dialog::list_fake_key++);
          dialog::add_action([&cmd] {
            arg::run_arguments(cmd.second);
            });
          }
        dialog::end_list();
        dialog::addBack();
        dialog::display();
        });
      }
    });
  }

void view_choice() {
  cmode = sm::SIDE;
  gamescreen();
  
  dialog::init("view choice");
  dialog::addItem("auto visualization", 'a');
  dialog::add_action([] {
    popScreen();
    rogueviz::rv_change(anims::ma, anims::maNone);
    #if CAP_VR
    rogueviz::rv_change(vrhr::hsm, vrhr::eHeadset::model_viewing);
    rogueviz::rv_change(vrhr::eyes, vrhr::eEyes::equidistant);
    rogueviz::rv_change(vrhr::cscr, vrhr::eCompScreen::eyes);
    rogueviz::rv_change<ld>(pconf.vr_scale_factor, pconf.scale);
    rogueviz::rv_change<ld>(pconf.vr_zshift, 1.5);
    rogueviz::rv_change<ld>(camera_speed, 10);
    rogueviz::rv_change(vrhr::enabled, true);
    #endif
    });
  dialog::addItem("immersive", 'i');
  dialog::add_action([] {
    popScreen();
    rogueviz::rv_change(anims::ma, anims::maNone);
    pmodel = default_model();
    rogueviz::rv_change<ld>(camera_speed, 5);
    rogueviz::rv_change<ld>(sightranges[geometry], 50);
    if(GDIM == 3) rogueviz::rv_change(vid.cells_drawn_limit, 1);
    rogueviz::rv_change(draw_centerover, false);
    #if CAP_VR
    rogueviz::rv_change(vrhr::hsm, vrhr::eHeadset::holonomy);
    rogueviz::rv_change(vrhr::eyes, vrhr::eEyes::equidistant);
    rogueviz::rv_change(vrhr::cscr, vrhr::eCompScreen::eyes);
    rogueviz::rv_change(vrhr::enabled, true);
    #endif
    });
  dialog::display();
  }

int runslide =
  arg::add3("-read-commands", [] {
  arg::shift(); read_commands(arg::args());
  }) +
  arg::add3("-view-choice", [] {
    pushScreen(view_choice);
    }) +
  arg::add3("-default-model", [] {
    pmodel = default_model();
    });

}
