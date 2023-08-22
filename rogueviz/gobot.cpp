// Hyperbolic Rogue -- Go
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

// compile the Discord version:
// ./mymake rogueviz/gobot.cpp -std=c++17 -lssl -lz -lcrypto -I rogueviz/aegis/include/ -I rogueviz/aegis/lib/json/include/ -I rogueviz/aegis/lib/spdlog/include/ -I rogueviz/aegis/lib/websocketpp/ -I rogueviz/aegis/lib/asio/asio/include/

// to run on an interesting board, run RogueViz with parameters:
// -canvas i -fillmodel ff801080 -noscr -geo Bring -gp 5 1 -unrectified -go-local -smart 1
// (add -run before -go-local if you want to select your board manually (press F10 after selecting the board)

// run online: ./hyper-go -canvas i -fillmodel ff801080 -noscr -geo Bring -gp 5 1 -unrectified -smart 3 -shot-1000 -shotxy 500 500 -shott 1 -gobot -go-discord

/** \file gobot.cpp
 *  \brief bot to play Go via Discord
 */

#define AEGIS 0

#if AEGIS
#include <dpp/dpp.h>
#endif

#include "rogueviz.h"

namespace hr {
#if CAP_THREAD
EX namespace gobot {

eWall empty = waChasm;

bool in = false;

vector<cell*> ac;
map<cell*, int> indices;

const int Free = 2;
const int Unowned = 3;

struct boarddata {
  vector<int> taken, owner;
  array<int, 2> captures;
  string geom;
  int last_index;
  };

boarddata current;

int labels_value = 1;

vector<boarddata> history;

bool draw_go(cell *c, const shiftmatrix& V);

void init_go_board() {
  ac = currentmap->allcells();
  current.taken.clear();
  current.owner.clear();
  current.taken.resize(isize(ac), 2);
  current.owner.resize(isize(ac), 2);
  current.captures[0] = 0;
  current.captures[1] = 0;
  current.last_index = -1;
  shstream f; mapstream::save_geometry(f); current.geom = f.s;
  indices.clear();
  for(int i=0; i<isize(ac); i++)
    indices[ac[i]] = i;
  }

void init_go() {
  init_go_board();
  rogueviz::addHook(hooks_drawcell, 100, draw_go);
  }

void hwrite(hstream& hs, const boarddata& b) {
  hwrite(hs, b.captures, b.taken, b.owner, b.geom);
  }

void hread(hstream& hs, boarddata& b) {
  hread(hs, b.captures, b.taken, b.owner, b.geom);
  }

vector<int> neigh_indices(int i) {
  vector<int> res;
  forCellEx(c1, ac[i])
    if(indices.count(c1))
      res.push_back(indices[c1]);
  return res;
  }

string chars = "abcdefghijkmnopqrstuvwxyzABCDEFGHJKLMNOPQRSTUVWXYZ123456789";

bool valid_index(int q) {
  return q >= 0 && q < isize(ac);
  }

string index_to_str(int k) {
  if(k < isize(chars))
    return s0 + chars[k];
  else return chars[k % isize(chars)] + index_to_str(k / isize(chars) - 1);
  }

int str_to_index(string s) {
  int val = -1;
  for(int i=0; i<isize(chars); i++) if(s[0] == chars[i]) val = i;
  if(val == -1) return -1;
  if(isize(s) == 1) return val;
  else return val + isize(chars) * (str_to_index(s.substr(1)) + 1);
  }

color_t player_colors[4] = {0x202020FF, 0xFFFFFFFF, 0, 0xFFD500FF};

bool draw_go(cell *c, const shiftmatrix& V) {
  if(c->wall == waSea) c->wall = waChasm;
  if(!indices.count(c)) return false;
  
  int id = indices[c];
  
  int lv_needed;
  
  forCellIdCM(c1, i, c) if(indices.count(c1) && indices[c1] < id) {
    vid.linewidth *= 2;
    gridline(V, C0, currentmap->adj(c, i) * C0, 0xFF, 4);
    vid.linewidth /= 2;
    }
  
  if(current.taken[id] != 2) {
    vid.linewidth *= 2;
    poly_outline = 0xFF;
    queuepolyat(V, cgi.shHugeDisk, player_colors[current.taken[id]], PPR::SUPERLINE);
    vid.linewidth /= 2;
    lv_needed = 3;
    }

  else if(current.owner[id] == 2) {
    lv_needed = 1;
    }
  
  else {
    vid.linewidth /= 5;
    queuepoly(V, cgi.shGem[0], player_colors[current.owner[id]]);    
    vid.linewidth *= 5;
    lv_needed = 2;
    }
  
  if(id == current.last_index) {
    vid.linewidth *= 3;
    queuecircleat1(c, V, 1, 0x8080FFFF);
    vid.linewidth /= 3;
    }

  if(labels_value >= lv_needed) {
    string s = index_to_str(id);
    queuestr(V, isize(s) == 1 ? 0.8 : 0.5, s, 0xFFD500);
    }
  
  return false;
  }
  
void save_backup() {
  history.push_back(current);
  }

void undo() {
  if(current.geom != history.back().geom) {
    shstream f; f.s = history.back().geom;
    stop_game();
    mapstream::load_geometry(f);
    start_game();
    init_go();
    }
  current = history.back();
  history.pop_back();
  }

#if AEGIS
dpp::message_create_t* cur;
dpp::cluster *pbot;
#endif

int shot_state;

#if AEGIS
// std::vector<dpp::future<dpp::message> > old_shots;
#endif

void clean_old_shots() {
  /*
  std::vector<aegis::future<aegis::gateway::objects::message> > remaining;
  for(auto& sh: old_shots)
    if(sh.available()) {
       sh.get().delete_message();
      }
    else
      remaining.emplace_back(std::move(sh));
  old_shots = std::move(remaining);
  */
  }

bool menubased;

bool full_scores = true;

struct score {
  array<int, 4> owned_by, stones;
  };

score get_score() {
  score sc;
  for(int i=0; i<4; i++)
    sc.owned_by[i] = sc.stones[i] = 0;

  for(int i=0; i<isize(ac); i++)
    if(current.taken[i] != Free)
      sc.stones[current.taken[i]]++;
    else sc.owned_by[current.owner[i]]++;
  return sc;
  }

void go_screenshot_content() {
  gamescreen();
  if(!full_scores) return;

  score sc = get_score();

  flat_model_enabler fme;
  initquickqueue();

  ld rad = vid.fsize;

  if(sc.stones[0] > 0) {
    shiftmatrix V = shiftless(atscreenpos(rad, rad, rad));
    queuepolyat(V, cgi.shHugeDisk, player_colors[0], PPR::SUPERLINE);
    write_in_space(V, max_glfont_size, .8, its(sc.stones[0]), player_colors[1], 1);
    }

  if(current.captures[0] > 0) {
    shiftmatrix V = shiftless(atscreenpos(rad * 3, rad, rad));
    write_in_space(V, max_glfont_size, .8, its(current.captures[0]), player_colors[1], 1);
    }

  if(sc.owned_by[0] > 0) {
    shiftmatrix V = shiftless(atscreenpos(rad, rad * 3, rad));
    write_in_space(V, max_glfont_size, .8, its(sc.owned_by[0]), player_colors[1], 1);
    }

  if(sc.stones[1] > 0) {
    shiftmatrix V = shiftless(atscreenpos(vid.xres-rad, rad, rad));
    queuepolyat(V, cgi.shHugeDisk, player_colors[1], PPR::SUPERLINE);
    write_in_space(V, max_glfont_size, .8, its(sc.stones[1]), player_colors[0], 1);
    }

  if(current.captures[1] > 0) {
    shiftmatrix V = shiftless(atscreenpos(vid.xres - rad * 3, rad, rad));
    write_in_space(V, max_glfont_size, .8, its(current.captures[1]), player_colors[0], 1);
    }

  if(sc.owned_by[1] > 0) {
    shiftmatrix V = shiftless(atscreenpos(vid.xres - rad, rad * 3, rad));
    write_in_space(V, max_glfont_size, .8, its(sc.owned_by[1]), player_colors[1], 1);
    }

  if(sc.owned_by[2] > 0) {
    shiftmatrix V = shiftless(atscreenpos(rad, vid.yres - rad, rad));
    write_in_space(V, max_glfont_size, .8, its(sc.owned_by[2]), player_colors[2], 1);
    }

  if(sc.owned_by[3] > 0) {
    shiftmatrix V = shiftless(atscreenpos(rad, vid.yres - rad, rad));
    write_in_space(V, max_glfont_size, .8, its(sc.owned_by[3]), player_colors[3], 1);
    }


  quickqueue();
  println(hlog, "should be drawn");
  }

void take_shot() {
  #if AEGIS
  if(cur) {
    println(hlog, "taking test screenshot");
    shot_state = 1;
    while(shot_state == 1) usleep(1000);
    shot_state = 0;
    
    dpp::message msg(cur->msg.channel_id, "");
    msg.add_file("go-board.png", dpp::utility::read_file("go-temp.png"));
    pbot->message_create(msg);

    clean_old_shots();
    // old_shots.push_back();
    println(hlog, "message sent");
    }
  #else
  if(0) {}
  #endif
  else if(!menubased) {
    println(hlog, "taking test screenshot");
    shot::take("go-test.png", go_screenshot_content);
    }
  }

void go_message(string s) {
  println(hlog, s);
  addMessage(s);

  #if AEGIS
  if(cur) {
    dpp::message msg(cur->msg.channel_id, s);
    pbot->message_create(msg);
    }
  #endif
  }

struct dfs {
  vector<bool> visited;
  
  vector<int> q;
  
  dfs() {
    visited.resize(isize(ac), false);
    }

  void visit(int i) {
    if(visited[i]) return;
    visited[i] = true;
    q.push_back(i);
    };  
  };

int count_breath(int pos) {
  int who = current.taken[pos];
  
  dfs d;
  
  int result = 0;
  d.visit(pos);
  for(int i=0; i<isize(d.q); i++) {
    int at = d.q[i];
    if(current.taken[at] == Free)
      result++;
    else if(current.taken[at] == who)
      for(int j: neigh_indices(at)) d.visit(j);
    }
  
  return result;
  }

void dead_group(int pos) {
  int who = current.taken[pos];
  
  dfs d;
  
  d.visit(pos);
  for(int i=0; i<isize(d.q); i++) {
    int at = d.q[i];
    if(current.taken[at] == who) {
      current.taken[at] = Free;
      current.captures[who]++;
      for(int j: neigh_indices(at)) d.visit(j);
      }
    }
  current.last_index = pos;
  }

bool dead_group(string s) {
  int pos = str_to_index(s);
  if(!valid_index(pos)) {
    go_message("invalid index " + s);
    return false;
    }
  if(current.taken[pos] == Free) {
    go_message("this is free: " + s);
    return false;
    }
  
  dead_group(pos);
  return true;
  }

bool mark_owned(string s, int who) {
  int pos = str_to_index(s);
  if(!valid_index(pos)) {
    go_message("invalid index " + s);
    return false;
    }
  if(current.taken[pos] != Free) {
    go_message("this is not free: " + s);
    return false;
    }

  dfs d;  
  d.visit(pos);
  for(int i=0; i<isize(d.q); i++) {
    int at = d.q[i];
    if(current.taken[at] == Free) {
      current.owner[at] = who;
      for(int j: neigh_indices(at)) d.visit(j);
      }
    }
  
  current.last_index = pos;
  return true;
  }

bool set_owner_auto() {
  int N = isize(ac);
  for(int at=0; at<N; at++) if(current.taken[at] == Free) current.owner[at] = Free;

  for(int pos=0; pos<N; pos++) if(current.taken[pos] == Free && current.owner[pos] == Free) {
    dfs d;
    d.visit(pos);
    int t0 = 0, t1 = 0;
    for(int i=0; i<isize(d.q); i++) {
      int at = d.q[i];
      if(current.taken[at] == Free) {
        for(int j: neigh_indices(at)) d.visit(j);
        }
      else if(current.taken[at] == 0) t0++;
      else if(current.taken[at] == 1) t1++;
      }
    for(int i=0; i<isize(d.q); i++) {
      int at = d.q[i];
      auto& o = current.owner[at];
      if(t0 && t1) o = Unowned;
      else if(t0) o = 0;
      else if(t1) o = 1;
      else { go_message("all free!"); return true; } /* all free */
      }
    }

  current.last_index = -1;
  take_shot();
  return true;
  }

string mouse_label() {
  if(!indices.count(mouseover)) return "?";
  return index_to_str(indices[mouseover]);
  }

void try_to_play(string s, int who) {
  int pos = str_to_index(s);
  if(!valid_index(pos)) {
    go_message("invalid index: " + s);
    return;
    }
  if(who != Free && current.taken[pos] != Free) {
    go_message("this is not free: " + s);
    return;
    }

  save_backup();
  
  current.taken[pos] = who;
  for(int i: neigh_indices(pos)) {
    println(hlog, "at ", index_to_str(i), " : ", current.taken[i], " vs ", 1-who);
    if(current.taken[i] == 1-who) println(hlog, "breath = ", count_breath(i));
    if(current.taken[i] == 1-who && count_breath(i) == 0)
      dead_group(i);
    }

  int steps = 0;
  for(int it=isize(history)-1; it>=0; it--) {
    steps++;
    if(history[it].taken == current.taken) {
      go_message("This position repeated " + its(steps) + " moves back. Say 'undo' to cancel this move");
      }
    if(history[it].captures[0] == 0 && history[it].captures[1] == 0)
      break;
    }
  
  if(count_breath(pos) == 0) {
    current.taken[pos] = Free;
    go_message("suicidal move at " + s);
    undo();
    }
  
  else {
    current.last_index = pos;
    take_shot();
    }      
  }

void die_at(vector<string> tokens) {
  int t = isize(tokens);
  save_backup();
  bool ok = false;
  for(int i=1; i<t; i++)
    if(dead_group(tokens[i]))
      ok = true;
  if(!ok) undo();
  else take_shot();
  }

void set_owner(vector<string> tokens, int who) {
  int t = isize(tokens);
  save_backup();
  bool ok = false;
  for(int i=1; i<t; i++) {
    if(mark_owned(tokens[i], who)) ok = true;
    }
  if(!ok) undo();
  else {
    take_shot();
    }
  }

void clear_owner_marks() {
  save_backup();
  for(int i=0; i<isize(ac); i++) current.owner[i] = Free;
  current.last_index = -1;
  take_shot();
  }

void save_go() {
  save_backup();
  fhstream f("go.saved-game", "wb");
  f.write(f.vernum);
  f.write(history);
  undo();
  }

void accept_command(string s) {
  println(hlog, "accepting command: '", s, "'");
  vector<string> tokens;
  string ctoken;
  for(char c: s + " ")
    if(among(c, ' ', '\n', '\r', '\t')) {
      if(ctoken != "") tokens.push_back(ctoken), ctoken = "";
      }
    else ctoken += c;
  
  int t = isize(tokens);
  if(t == 0) return;
  
  if(tokens[0] == "b" && t == 2) {
    try_to_play(tokens[1], 0);
    }
  
  if(tokens[0] == "w" && t == 2) {
    try_to_play(tokens[1], 1);
    }

  if(tokens[0] == "center" && t == 2) {
    int pos = str_to_index(tokens[1]);
    if(!valid_index(pos)) go_message("invalid cell: " + tokens[1]);
    else {
      centerover = ac[pos];
      // fullcenter();
      take_shot();
      }
    }

  if(tokens[0] == "rotate" && t == 2) {
    try {
      ld angle = parseld(tokens[1]);
      View = spin(angle * degree) * View;
      take_shot();
      }
    catch(hr_parse_exception& exc) {
      go_message(exc.s);
      }
    }

  if(tokens[0] == "labels" && t == 2) {
    try {
      labels_value = parseld(tokens[1]);
      current.last_index = -1;
      take_shot();
      }
    catch(hr_parse_exception& exc) {
      go_message(exc.s);
      }
    }    

  if(tokens[0] == "hires") {
    dynamicval<int> dx(shot::shotx, 1000);
    dynamicval<int> dy(shot::shoty, 1000);
    take_shot();
    }    

  if(tokens[0] == "help")
    go_message(
      "b [where] - play as black\n"
      "w [where] - play as white\n"
      "center [where] - center the screen\n"
      "rotate [degrees] - rotate the screen\n"
      "die [where] - kill a group\n"
      "clear - clear owner marks\n"
      "labels 0..3 - show (0) no labels, (1) labels on unowned, (2) labels on empty, (3) all labels\n"
      "ob [where] - own area as black\n"
      "ow [where] - own area as white\n"
      "of [where] - the area is free\n"
      "oc [where] - the area is common\n"
      "oauto - own automatically\n"
      "score - view the score (score on/off to include the score in screenshots)\n"
      "hires - take a 1000x1000 screenshot\n"
      "restart - restart\n"
      "bring-unrectified x y -- restart on unrectified GP(x,y) Bring surface\n"
      "disk-unrectified x y size -- restart on unrectified GP(x,y) {4,5} disk of given size\n"
      "undo - undo last move\n"
      "export - export board to string (no history, owners, captures)\n"
      "import [string] - import board from string\n"
      "csc [value] - stone size, current is " + fts(vid.creature_scale) + "\n"
      );

  if(tokens[0] == "save") save_go();

  if(tokens[0] == "die") die_at(tokens);

  if(tokens[0] == "csc" && t == 2) vid.creature_scale = parseld(tokens[1].c_str());
  
  if(tokens[0] == "clear" && t == 1) 
    clear_owner_marks();

  if(tokens[0] == "ob") set_owner(tokens, 0);
  if(tokens[0] == "ow") set_owner(tokens, 1);
  if(tokens[0] == "of") set_owner(tokens, 2);
  if(tokens[0] == "oc") set_owner(tokens, 3);

  if(tokens[0] == "oauto")
    set_owner_auto();
  
  if(tokens[0] == "undo") {
    if(history.empty())
      go_message("no undo history");
    else {
      undo();
      take_shot();
      }
    }

  if(tokens[0] == "score") {
    score sc = get_score();
    
    shstream ss;
    println(ss, "black: ", sc.stones[0], " stones, ", sc.owned_by[0], " area, ", current.captures[1], " prisoners");
    println(ss, "white: ", sc.stones[1], " stones, ", sc.owned_by[1], " area, ", current.captures[0], " prisoners");
    print(ss, "board size: ", isize(ac));
    if(sc.owned_by[2]) print(ss, " free: ", sc.owned_by[2]);
    if(sc.owned_by[3]) print(ss, " common: ", sc.owned_by[3]);
    
    go_message(ss.s);

    if(t == 2 && tokens[1] == "on") { full_scores = true; take_shot(); }
    if(t == 2 && tokens[1] == "off") { full_scores = false; take_shot(); }
    }

  if(tokens[0] == "restart") {
    save_backup();
    for(int i=0; i<isize(ac); i++) current.owner[i] = current.taken[i] = Free;
    current.captures[0] = 0;
    current.captures[1] = 0;
    take_shot();
    }

  if(tokens[0] == "bring-unrectified" && t == 3) {
    int x = atoi(tokens[1].c_str());
    int y = atoi(tokens[2].c_str());
    if(x > 8 || y > 8 || x < 0 || y < 0 || x+y == 0) { go_message("illegal parameters"); return; }
    save_backup();
    stop_game();
    geometry = gBring;
    variation =eVariation::unrectified;
    gp::param = {x, y};
    start_game();
    init_go_board();
    go_message("Bring surface, size = " + its(isize(ac)));
    take_shot();
    }

  if(tokens[0] == "disk-unrectified" && t == 4) {
    int x = atoi(tokens[1].c_str());
    int y = atoi(tokens[2].c_str());
    int size = atoi(tokens[3].c_str());
    if(x > 8 || y > 8 || x < 0 || y < 0 || x+y == 0 || size > 1000 || size < 10) { go_message("illegal parameters"); return; }
    save_backup();
    stop_game();
    geometry = g45;
    req_disksize = size;
    variation =eVariation::unrectified;
    gp::param = {x, y};
    start_game();
    init_go_board();
    go_message("disk, size = " + its(isize(ac)));
    take_shot();
    }

  if(tokens[0] == "export" && t == 1) {
    string ex;
    for(int i=0; i<isize(ac); i++) ex.push_back("bw." [current.taken[i]] );
    go_message("current board: ```" + ex + "```");
    }

  if(tokens[0] == "import" && t == 2) {
    string ex = tokens[1];
    if(isize(ex) != isize(ac)) { go_message("bad length"); return; }
    save_backup();
    for(int i=0; i<isize(ac); i++) {
      auto& t = current.taken[i];
      auto ch = ex[i];
      t = Free;
      if(ch == 'b') t = 0;
      if(ch == 'w') t = 1;
      current.owner[i] = Free;
      }
    take_shot();
    }
  }

std::thread bot_thread;

void go_discord() {
#if AEGIS
  bot_thread = std::thread([] {
    println(hlog, "starting bot");
    uint64_t intents = dpp::i_default_intents | dpp::i_message_content;
    dpp::cluster bot(AEGIS_TOKEN, intents);
    pbot = &bot;
    std::mutex lock;
    println(hlog, "on_message_create");
    bot.on_message_create([&](auto obj) {
      if(obj.msg.channel_id != 820590567397261352LL) {
        println(hlog, "message '", obj.msg.content, "' on wrong channel");
        return;
        }
      std::unique_lock<std::mutex> lk(lock);
      cur = &obj;
      accept_command(obj.msg.content);
      cur = nullptr; 
      });
    println(hlog, "starting bot");
    bot.start(dpp::st_wait);
    println(hlog, "done");
    // bot.yield();
    });
#endif
  }

void go_menu() {

  getcstat = '-';
  cmode = sm::SIDE | sm::DIALOG_STRICT_X;
  gamescreen();

  dialog::init("HyperGo", iinf[itPalace].color, 150, 100);
  
  mousing = false; /* always display letters */

  array<int, 2> owned_by, stones;
  for(int i=0; i<2; i++)
    owned_by[i] = stones[i] = 0;

  for(int i=0; i<isize(ac); i++)
    if(current.taken[i] != Free)
      stones[current.taken[i]]++;
    else if(current.owner[i] != Free)
      owned_by[current.owner[i]]++;
    
  shstream ss;
  println(ss, "black: ", stones[0], " stones, ", owned_by[0], " area, ", current.captures[1], " prisoners");
  println(ss, "white: ", stones[1], " stones, ", owned_by[1], " area, ", current.captures[0], " prisoners");

  dialog::addSelItem("play black", its(stones[0]), 'b');
  dialog::add_action([] { try_to_play(mouse_label(), 0); });
  dialog::addSelItem("play white", its(stones[1]), 'w');
  dialog::add_action([] { try_to_play(mouse_label(), 1); });
  dialog::addItem("remove stone", 'r');
  dialog::add_action([] { try_to_play(mouse_label(), Free); });
  dialog::addItem("kill a group", 'k');
  dialog::add_action([] { die_at({"", mouse_label()}); });
  dialog::addItem("clear owner marks", 'c');
  dialog::add_action(clear_owner_marks);
  dialog::addSelItem("own as black", its(owned_by[0]), 'B');
  dialog::add_action([] { set_owner({"", mouse_label()}, 0); });
  dialog::addSelItem("own as white", its(owned_by[1]), 'W');
  dialog::add_action([] { set_owner({"", mouse_label()}, 1); });
  dialog::addSelItem("white prisoners", its(current.captures[1]), 0);
  dialog::addSelItem("black prisoners", its(current.captures[0]), 0);
  dialog::addItem("undo", 'u');
  dialog::add_action(undo);

  dialog::addItem("quit", 'q');
  dialog::add_action(popScreen);
  
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    handlePanning(sym, uni);
    dialog::handleNavigation(sym, uni);
    };
  }

#if CAP_COMMANDLINE
int rugArgs() {
  using namespace arg;
           
  if(0) ;

  else if(argis("-gobot")) {
    PHASEFROM(3);
    start_game();
    init_go();
    }

  else if(argis("-go-local")) {
    labels_value = 0;
    showstartmenu = false;
    mapeditor::drawplayer = false;
    menubased = true;
    PHASEFROM(3);
    start_game();
    init_go();
    pushScreen(go_menu);
    }
  
  else if(argis("-go")) {
    shift(); accept_command(args());
    }
  
  else if(argis("-go-discord"))
    go_discord();

  else if(argis("-go-load")) {
    shift(); string s = args();
    fhstream f(s, "rb");
    f.read(f.vernum);
    f.read(history);
    println(hlog, "history of ", isize(history), " read successfully");
    undo();
    }

  else if(argis("-go-save")) save_go();

  else if(argis("-go-video")) {
    save_backup();
    shift(); string s = args();
    shift(); int fpmove = argi();
    shift(); int ffinal = argi();
    int N = isize(history);
    anims::noframes = N * fpmove + ffinal;
    anims::period = anims::noframes;

    int a = addHook(anims::hooks_anim, 100, [&] {
      current = history[min(ticks / fpmove, N-1)];
      });
    anims::videofile = s;
    anims::record_video_std();
    delHook(anims::hooks_anim, a);
    undo();
    }

  else return 1;
  return 0;
  }

bool display_stats = false;

auto gobot_hook = 
  addHook(hooks_args, 100, rugArgs) +
  addHook(shmup::hooks_turn, 100, [] (int t) {
    if(shot_state == 1) {
      shot::take("go-temp.png", go_screenshot_content);
      shot_state = 2;
      }
    return false;
    })
  ;
  
#endif

EX }
#endif
EX }

