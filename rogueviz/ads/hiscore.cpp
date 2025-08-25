namespace hr {

namespace ads_game {

gamedata cur;
vector<gamedata> allsaves;

void init_gamedata() {
  cur.rocks_hit = 0;
  cur.rsrc_collected = 0;
  cur.turrets_hit = 0;
  cur.deathreason = "still alive";
  char buf[128]; strftime(buf, 128, "%c", localtime(&timerstart)); cur.timerstart = buf;
  cur.myname = cur.timerstart;
  }

void fill_gamedata() {
  time_t timer = time(NULL);
  char buf[128]; strftime(buf, 128, "%c", localtime(&timer)); cur.timerend = buf;
  cur.seconds = int(timer - timerstart);
  for(int a=0; a<3; a++) cur.scores[a] = pdata.score[a];
  shstream hs;
  print(hs, main_rock ? "2 " : "1 ");
  print(hs, DS_(simspeed), " ", DS_(accel), " ", DS_(how_much_invincibility), " ", vid.creature_scale, " ", DS_(missile_rapidity));
  if(!main_rock) print(hs, " ", rock_max_rapidity, " ", rock_density, " ", max_gen_per_frame, " ", draw_per_frame);
  
  auto all = [&] (player_data& d) {
    print(hs, " ", d.hitpoints, " ", d.ammo, " ", d.fuel, " ", d.oxygen);
    };
  all(DS_(max_pdata));
  all(DS_(tank_pdata));

  cur.variant = hs.s;
  }

unsigned myhash(const string& s) {
  std::size_t seed = s.size();
  for(auto& i : s) {
    seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
  return seed;
  }

void save(const gamedata& sd) {
  #if CAP_SAVE
  fhstream f("relhell.save", "at");
  println(f, "Relative Hell ", VER_RH);
  println(f, sd.myname);
  println(f, sd.timerstart);
  println(f, sd.timerend);
  println(f, sd.variant);
  println(f, sd.deathreason);
  shstream hs;
  print(hs, sd.scores[0], " ", sd.scores[1], " ", sd.scores[2], " ", sd.seconds, " ", sd.rocks_hit, " ", sd.rsrc_collected, " ", sd.turrets_hit);
  println(f, hs.s);
  println(f, myhash(sd.variant + "#" + hs.s));
  #endif
  }

void game_over_with_message(const string& reason) {
  if(game_over) return;
  fill_gamedata();
  cur.deathreason = reason;
  if(pdata.fuel <= 0) cur.deathreason += " while out of fuel";
  if(pdata.ammo <= 0) cur.deathreason += " while out of ammo";
  game_over = true;
  #if RVCOL
  if(main_rock && no_param_change)
    rogueviz::rv_leaderboard("de Sitter", current.shift * 1000, 1, rvlc::ms);
  if(!main_rock && no_param_change) {
    auto& s = pdata.score;
    string data = lalign(0, s[0], " ", s[1], " ", s[2]);
    rogueviz::rv_leaderboard("anti de Sitter: total score", s[0] + s[1] + s[2], 1, rvlc::num, data);
    rogueviz::rv_leaderboard("anti de Sitter: platinum", s[0], 1, rvlc::num, data);
    rogueviz::rv_leaderboard("anti de Sitter: plasteel", s[1], 1, rvlc::num, data);
    rogueviz::rv_leaderboard("anti de Sitter: uranium", s[2], 1, rvlc::num, data);
    }
  #endif
  }

void save_to_hiscores() {
  if(!main_rock && (pdata.score[0] + pdata.score[1] + pdata.score[2] == 0)) return;
  if(main_rock && current.shift < 5) return;
  save(cur);
  allsaves.push_back(cur);
  }

void load_hiscores() {
  allsaves.clear();
  fhstream f("relhell.save", "rt");
  if(!f.f) return;
  string s;
  while(!feof(f.f)) {
    s = scanline_noblank(f);
    if(s == "Relative Hell 1.0" || s == "Relative Hell 1.1") {
      gamedata gd;
      gd.myname = scanline_noblank(f);
      gd.timerstart = scanline_noblank(f);
      gd.timerend = scanline_noblank(f);
      gd.variant = scanline_noblank(f);
      gd.deathreason = scanline_noblank(f);
      sscanf(scanline_noblank(f).c_str(), "%lf%lf%lf%d%d%d%d",
        &gd.scores[0], &gd.scores[1], &gd.scores[2], &gd.seconds, &gd.rocks_hit, &gd.rsrc_collected, &gd.turrets_hit);
      allsaves.push_back(std::move(gd));
      }
    }
  }

int hi_sort_by = 3;

void hiscore_menu() {
  cmode = sm::VR_MENU | sm::NOSCR; gamescreen();
  dialog::init("High scores");
  fill_gamedata();
  vector<gamedata*> v;
  for(auto& ad: allsaves)
    if(ad.variant == cur.variant)
      v.push_back(&ad);
  v.push_back(&cur);

  auto getval = [] (gamedata *g) {
    if(!main_rock) return g->scores[0];
    if(hi_sort_by == 3) return g->scores[0] + g->scores[1] + g->scores[2];
    return g->scores[hi_sort_by];
    };
  
  sort(v.begin(), v.end(), [&] (gamedata* g1, gamedata* g2) { return getval(g1) > getval(g2); });
  dialog::start_list(900, 900, '1');
  for(auto ad: v) {
    dialog::addSelItem(ad->myname + " (" + ad->deathreason + ")", main_rock ? fts(getval(ad)) : its(getval(ad)), dialog::list_fake_key++);
    dialog::add_action_push([ad] {
      cmode = sm::VR_MENU;
      emptyscreen();
      dialog::init(ad->myname);
      if(!main_rock) {
        dialog::addSelItem("platinum", its(ad->scores[0]), 'p');
        dialog::addSelItem("plasteel", its(ad->scores[1]), 'l');
        dialog::addSelItem("uranium", its(ad->scores[2]), 'u');
        }
      else {
        dialog::addSelItem("score", fts(ad->scores[0]), 's');
        }

      if(!main_rock) dialog::addSelItem("stars hit", its(ad->rocks_hit), 'h');
      if(main_rock) dialog::addSelItem("rocks hit", its(ad->rocks_hit), 'r');
      if(!main_rock) dialog::addSelItem("turrets hit", its(ad->turrets_hit), 't');
      dialog::addSelItem("resources collected", its(ad->rsrc_collected), 'c');

      dialog::addSelItem("played", ad->timerstart, 'T');
      dialog::addSelItem("seconds", its(ad->seconds), 'S');

      dialog::addBack();
      dialog::display();
      });
    }

  dialog::end_list();

  if(!main_rock) add_edit(hi_sort_by);
  dialog::addSelItem("name", cur.myname, 'n');
  dialog::add_action([] { dialog::edit_string(cur.myname, "enter your name", ""); });   
  dialog::addBack();
  dialog::display();
  }

}
}
