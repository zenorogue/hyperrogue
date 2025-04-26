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
        print(f, format("%c", (v & 7) == 7 ? 'b' : code_for[v>>3]));
        }
      println(f);
      }

    println(f, "OK\n");

    for(int i=0; i<c->type; i++) visit(c->cmove(i), c->c.spin(i));
    }
  }

void err(string s, string context) {
  println(hlog, "while: ", context, " reading: ", s);
  throw hr_exception("ru read error");
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
        current_room = &r;
        sscanf(param.c_str(), "%lf%lf", &m.where_x, &m.where_y);
        }
      else if(cap == "ITEM") {
        auto b = std::make_unique<item>();
        sscanf(param.c_str(), "%lf%lf", &b->where_x, &b->where_y);
        s = scanline_noblank(f);
        b->id = -1;
        for(int i=0; i<isize(powers); i++) if(powers[i].name == s) b->id = i;
        if(b->id == -1) println(hlog, "error: unknown item name ", s), b->id = 0;
        b->pickup_message = scanline_noblank(f);
        r.entities.emplace_back(std::move(b)); 
        }
      else if(cap == "NPC") {
        auto b = std::make_unique<npc>();
        sscanf(param.c_str(), "%lf%lf%08x", &b->where_x, &b->where_y, &b->col);
        s = scanline_noblank(f);
        b->sglyph = s[0];
        b->name = s.substr(1);
        b->text = scanline_noblank(f);
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


}
