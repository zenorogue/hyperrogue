namespace rogue_unlike {

map<cell*, int> in_queue;

queue<cell*> q;

void visit(cell *c, int d) {
  if(in_queue.count(c)) return;
  in_queue[c] = d;
  q.push(c);
  }

void save_map(string fname) {
  in_queue.clear();
  q = {};
  visit(currentmap->gamestart(), -1);

  fhstream f(fname, "w");

  while(!q.empty()) {
    auto c = q.front();
    q.pop();
    auto& r = rooms[c];
    if(!r.infile) continue;

    for(int i=0; i<c->type; i++) visit(c->cmove(i), c->c.spin(i));

    if(!r.edited) continue;
    auto inq = in_queue[c];
    if(inq != -1) {
      println(f, "MOVE ", c->c.spin(inq), " ", rooms[c->move(inq)].roomname);
      }
    println(f, "ROOM ", r.roomname);

    map<int, char> code_for;
    map<char, int> code_used;
    char next_code = 'A';

    for(int y=0; y<room_y; y++)
    for(int x=0; x<room_x; x++) {
      auto i = r.block_at[y][x] >> 3;
      if(i == int(wRogueWallHidden)) i = int(wRogueWall);
      auto& c = code_for[i];
      if(c == 0 && !code_used.count(walls[i].glyph[0])) c = walls[i].glyph[0];
      if(c == 0) c = next_code++;
      code_used[c] = i;
      }

    for(auto [key, id]: code_used) println(f, format("%c", key), " ", walls[id].name);

    println(f, "MAP");
    
    for(int y=0; y<room_y; y++) {
      for(int x=0; x<room_x; x++) {
        auto v = r.block_at[y][x];
        if(v == int(wRogueWallHidden)) v = int(wRogueWall);
        print(f, format("%c", (v & 7) == 7 ? 'b' : code_for[v>>3]));
        }
      println(f);
      }

    println(f, "OK\n");
    }
  }

void err(string s, string context) {
  println(hlog, "while: ", context, " reading: ", s);
  throw hr_exception("ru read error");
  }

void create_long_rope(room& r, int step, xy w) {
  ld radius = 2.7;
  ld qty = 300;
  ld period = 5;
  ld shift = 0;
  ld max_swing = 7.5;
  for(int i=0; i<qty; i++) {
    auto b = std::make_unique<rope_platform>();
    b->ctr = w; b->radius = radius; b->period = period; b->max_swing = max_swing * 1._deg; b->shift = shift;
    b->dist = radius * (i+0.5) / qty;
    r.entities.emplace_back(std::move(b));
    }
  for(auto& p: rooms) if(&p.second == &r) {
    auto goto_rope = [&] (int i, int j) {
      create_long_rope(*get_room_at(p.first->cmove(i)), j, get<0>(get_next_room(w, &r, i)));
      };
    if(step < 3) goto_rope(3, step + 1);
    if(step == 3) goto_rope(4, 4);
    if(step == 2) goto_rope(2, 4);
    if(step == 2) goto_rope(4, 4);
    if(step == 1) goto_rope(4, 4);
    break;
    }
  }

void load_room(fhstream& f, cell *c) {
  setdist(c, 7, nullptr);
  auto& r = *get_room_at(c);
  string s = scanline_noblank(f);
  if(s.substr(0, 5) == "ROOM ") r.roomname = s.substr(5);
  else err("load_room name ", s);
  println(hlog, "loading room named: ", r.roomname);
  map<char, int> codes;
  while(true) {
    string s = scanline_noblank(f);
    if(s == "") continue;
    if(s == "MAP") break;
    string t = s.substr(2);
    if(s.size() < 3 || s[1] != ' ') err("load codes", s);
    for(int i=0; i<qwall; i++) if(walls[i].name == t) {
      if(i == int(wRogueWall)) i = int(wRogueWallHidden);
      if(i == int(wSmashedDoor)) i = int(wDoor);
      codes[s[0]] = i;
      break;
      }
    }
  if(1) {
    vector<string> bmap;
    for(int y=0; y<room_y; y++) bmap.push_back(scanline_noblank(f));
    for(int y=0; y<room_y; y++) for(int x=0; x<room_x; x++) {
      if(bmap[y][x] == 'b') ;
      else if(y < room_y-1 && bmap[y+1][x] == 'b') ;
      else if(x < room_x-1 && bmap[y][x+1] == 'b') ;
      else if(y < room_y-1 && x < room_x-1 && bmap[y+1][x+1] == 'b')
        r.place_block_full(x, y, 8 * codes.at(bmap[y][x]) + 4);
      else
        r.place_block_full(x, y, 8 * codes.at(bmap[y][x]));
      }
    }
  while(true) {
    s = scanline_noblank(f);
    if(s == "OK") break;
    println(hlog, "s = ", s);
    auto pos = s.find(" ");
    if(pos != string::npos) {
      string cap = s.substr(0, pos);
      string param = s.substr(pos+1);
      if(cap == "START") {
        fountain_room = current_room = &r;
        sscanf(param.c_str(), "%lf%lf", &m.where.x, &m.where.y);
        fountain_where = m.where;
        }
      else if(cap == "ITEM") {
        auto b = std::make_unique<item>();
        b->qty = 1;
        sscanf(param.c_str(), "%lf%lf%d", &b->where.x, &b->where.y, &b->qty);
        s = scanline_noblank(f);
        b->id = -1;
        for(int i=0; i<isize(powers); i++) if(powers[i].name == s) b->id = i;
        if(b->id == -1) println(hlog, "error: unknown item name ", s), b->id = 0;
        b->pickup_message = scanline_noblank(f);
        r.entities.emplace_back(std::move(b)); 
        }
      else if(cap == "LOOT") {
        auto b = std::make_unique<loot>();
        b->qty = 1;
        b->owner = &*r.entities.back();
        sscanf(param.c_str(), "%d", &b->qty);
        s = scanline_noblank(f);
        b->id = -1;
        b->where = xy(320, 200);
        for(int i=0; i<isize(powers); i++) if(powers[i].name == s) b->id = i;
        if(b->id == -1) println(hlog, "error: unknown loot name ", s), b->id = 0;
        b->pickup_message = scanline_noblank(f);
        b->existing = false; b->dropped = false;
        r.entities.emplace_back(std::move(b));
        println(hlog, "loot pushed");
        }
      else if(cap == "SHOPITEM") {
        auto b = std::make_unique<shopitem>();
        b->qty = 1; b->qty1 = 0;
        sscanf(param.c_str(), "%lf%lf%d%d%d", &b->where.x, &b->where.y, &b->price, &b->qty, &b->qty1);
        s = scanline_noblank(f);
        b->id = -1;
        for(int i=0; i<isize(powers); i++) if(powers[i].name == s) b->id = i;
        if(b->id == -1) println(hlog, "error: unknown item name ", s), b->id = 0;
        b->pickup_message = scanline_noblank(f);
        r.entities.emplace_back(std::move(b));
        }
      else if(cap == "NPC") {
        auto b = std::make_unique<npc>();
        sscanf(param.c_str(), "%lf%lf%08x", &b->where.x, &b->where.y, &b->col);
        s = scanline_noblank(f);
        b->sglyph = s[0];
        b->name = s.substr(1);
        b->text = scanline_noblank(f);
        r.entities.emplace_back(std::move(b));
        }
      else if(cap == "TRADER") {
        auto b = std::make_unique<trader>();
        sscanf(param.c_str(), "%lf%lf", &b->where.x, &b->where.y);
        b->name = scanline_noblank(f);
        b->text = scanline_noblank(f);
        r.entities.emplace_back(std::move(b));
        }
      else if(cap == "BOAR") {
        auto b = std::make_unique<boar>();
        sscanf(param.c_str(), "%lf%lf", &b->where.x, &b->where.y);
        b->respawn = b->where; b->postfix();
        r.entities.emplace_back(std::move(b));
        }
      else if(cap == "GIANTFROG") {
        auto b = std::make_unique<giantfrog>();
        sscanf(param.c_str(), "%lf%lf", &b->where.x, &b->where.y);
        b->respawn = b->where; b->postfix();
        r.entities.emplace_back(std::move(b));
        }
      else if(cap == "FROG") {
        auto b = std::make_unique<frog>();
        sscanf(param.c_str(), "%lf%lf", &b->where.x, &b->where.y);
        b->respawn = b->where; b->postfix();
        r.entities.emplace_back(std::move(b));
        }
      else if(cap == "TIMEORB") {
        auto b = std::make_unique<timed_orb>();
        ld dur = 0;
        sscanf(param.c_str(), "%lf%lf%lf", &b->where.x, &b->where.y, &dur);
        b->duration = dur * game_fps;
        r.entities.emplace_back(std::move(b));
        }
      else if(cap == "BAT") {
        auto b = std::make_unique<bat>();
        sscanf(param.c_str(), "%lf%lf", &b->where.x, &b->where.y);
        b->respawn = b->where; b->postfix();
        r.entities.emplace_back(std::move(b));
        }
      else if(cap == "GUINEAPIG") {
        auto b = std::make_unique<guineapig>();
        sscanf(param.c_str(), "%lf%lf%lf%d", &b->where.x, &b->where.y, &b->pigvel, &b->spindir);
        b->pigvel /= game_fps; b->falling = true; b->vel = xy{0,0};
        b->respawn = b->where; b->respawn_spindir = b->spindir; b->postfix();
        r.entities.emplace_back(std::move(b));
        }
      else if(cap == "VTRAP") {
        auto b = std::make_unique<vtrap>();
        sscanf(param.c_str(), "%lf%lf", &b->where.x, &b->where.y);
        auto dat = b->get_dat();
        b->vel.x = 0; b->vel.y = 150 * 0.005 * dat.modv * dat.d;

        b->postfix();
        r.entities.emplace_back(std::move(b));
        }
      else if(cap == "GRIDBUG") {
        auto b = std::make_unique<gridbug>();
        sscanf(param.c_str(), "%lf%lf", &b->where.x, &b->where.y);
        b->respawn = b->where; b->postfix();
        r.entities.emplace_back(std::move(b));
        }
      else if(cap == "KESTREL") {
        auto b = std::make_unique<kestrel>();
        sscanf(param.c_str(), "%lf%lf%lf%lf", &b->where.x, &b->where.y, &b->vel.x, &b->vel.y);
        b->vel *= xy(block_x, block_y) / game_fps;
        b->respawn = b->where; b->respawn_vel = b->vel; b->postfix();
        r.entities.emplace_back(std::move(b));
        }
      else if(cap == "SNAKE") {
        auto b = std::make_unique<snake>();
        sscanf(param.c_str(), "%lf%lf%d", &b->where.x, &b->where.y, &b->dir);
        b->respawn = b->where; b->respawn_dir = b->dir; b->postfix();
        r.entities.emplace_back(std::move(b));
        }
      else if(cap == "FERRIS") {
        ld cx, cy, radius; int qty;
        sscanf(param.c_str(), "%lf%lf%lf%d", &cx, &cy, &radius, &qty);

        for(int i=0; i<qty; i++) {
          auto b = std::make_unique<ferris_platform>();
          b->ctr = {cx, cy}; b->radius = radius;
          b->shift = i * TAU / qty;
          r.entities.emplace_back(std::move(b));
          }
        }
      else if(cap == "ROPE") {
        ld cx, cy, radius, period, shift, max_swing; int qty;
        sscanf(param.c_str(), "%lf%lf%lf%d%lf%lf%lf", &cx, &cy, &radius, &qty, &period, &shift, &max_swing);

        for(int i=0; i<qty; i++) {
          auto b = std::make_unique<rope_platform>();
          b->ctr = {cx, cy}; b->radius = radius; b->period = period; b->max_swing = max_swing * 1._deg; b->shift = shift;
          b->dist = radius * (i+0.5) / qty;
          r.entities.emplace_back(std::move(b));
          }
        }
      else if(cap == "LONGROPE") {
        create_long_rope(r, 0, {lerp(l_margin_at, r_margin_at, 2/3.), 0});
        }
      else if(cap == "PENDULUM") {
        auto b = std::make_unique<pendulum_platform>();
        sscanf(param.c_str(), "%lf%lf%lf%lf%lf%lf", &b->a.x, &b->a.y, &b->b.x, &b->b.y, &b->period, &b->shift);
        r.entities.emplace_back(std::move(b));
        }
      else if(cap == "HINT") {
        auto b = std::make_unique<hint>();
        sscanf(param.c_str(), "%lf%lf%lf%lf", &b->where.x, &b->where.y, &b->size.x, &b->size.y);
        b->hint_text = scanline_noblank(f);
        r.entities.emplace_back(std::move(b));
        }
      else println(hlog, "unknown mapline ", s);
      }
    else println(hlog, "unknown mapline ", s);
    }
  }

void load_map(string fname) {
  fhstream f(fname, "r");
  load_room(f, currentmap->gamestart());
  while(!feof(f.f)) {
    string s = scanline_noblank(f);
    if(s == "") continue;
    if(s.substr(0, 4) == "MOVE") {
      int i = s[5] - '0';
      string roomname = s.substr(7);
      for(auto& [c,r]: rooms) if(r.roomname == roomname) load_room(f, c->move(i));
      }
    else err("load_map", s);
    }
  }

void load_cheat(string fname) {
  fhstream f(fname, "r");
  auto power_edited = &powers[0];
  while(!feof(f.f)) {
    string s = scanline_noblank(f);
    auto pos = s.find(" ");
    if(pos != string::npos) {
      string cap = s.substr(0, pos);
      string param = s.substr(pos+1);
      if(cap == "START") {
        for(auto& [c,r]: rooms) if(r.roomname == param) current_room = &r;
        }
      else if(cap == "POS") {
        sscanf(param.c_str(), "%lf%lf", &m.where.x, &m.where.y);
        }
      else if(cap == "ITEM") {
        bool found = false;
        for(int i=0; i<isize(powers); i++) if(powers[i].name == param) found = true, power_edited = &powers[i];
        if(!found) println(hlog, "cheat item not found: ", param);
        }
      else if(cap == "GAIN") {
        int a, b;
        sscanf(param.c_str(), "%d%d", &a, &b);
        power_edited->qty_owned += a;
        power_edited->qty_filled += b;
        println(hlog, "gain ", power_edited->name, " qty ", tie(a,b));
        }
      else println(hlog, "unrecognized cheat: ", s);
      }
    else if(s == "IDENTIFY") {
      power_edited->flags |= IDENTIFIED;
      }
    else if(s == "ACTIVATE") {
      power_edited->flags |= ACTIVE;
      }
    else if(s == "") {}
    else println(hlog, "unrecognized cheat: ", s);
    }
  }


}
