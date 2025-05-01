namespace rogue_unlike {

flagtype IDENTIFIED = Flag(1);
flagtype ACTIVE = Flag(2);

data fakedata;

power& power::is_starting() { qty_filled = qty_owned = 1; return self; }

power& power::while_paused() { paused_act = act; return self; }

power& power::identified_name(string s, string desc) {
  auto gn = get_name;
  get_name = [gn, s, this] () { return (flags & IDENTIFIED) ? s : gn(); };
  auto gd = get_desc;
  get_desc = [gd, s, this] () { return (flags & IDENTIFIED) ? s : gd(); };
  return self;
  }

power& power::be_weapon() {
  picked_up = [this] (int x) { qty_owned += x; qty_filled = max(qty_filled, x);  };
  auto gn = get_name; get_name = [gn, this] { return "+" + its(qty_filled-1) + " " + gn() + " (+" + its(qty_owned-qty_filled) + ")"; };
  return self;
  }

power& power::be_resource(string s) {
  get_name = [this, s] { return its(qty_filled) + " " + s; };
  return self;
  }

vector<power> powers;

void power::init() {
  id_status = 0;
  qty_filled = 0; qty_owned = 0;
  flags = 0;
  act = [this] (data& d) { pf(d); };
  paused_act = [] (data&) {};
  get_name = [this] { return name; };
  get_desc = [this] { return desc; };
  get_color = [this] { return color; };
  get_glyph = [this] { return glyph; };
  picked_up = [this] (int x) { qty_filled += x; qty_owned += x; };
  }

power& gen_power(int key, string name, string desc, string glyph, color_t color, powerfun pf) {
  powers.emplace_back();
  auto& p = powers.back();
  p.key = key;
  p.name = name;
  p.desc = desc;
  p.glyph = glyph;
  p.color = color;
  p.pf = pf;
  p.init();
  return p;
  }

void gen_powers() {
  powers.reserve(100);

  gen_power('1', "Potion of Extra Life",
    "You are really proud of this potion, which, after you die, will let you return to the moment of time when you drank it. "
    "Unfortunately it still requires an ingredient found only in the magical fountains of the Dungeons of Alchemy.\n\n"
    "You can only drink this potion when at a magical fountain. To protect yourself from dying permanently, it is drank "
    "automatically whenever you are at a magical fountain.",
    "!", 0xFFFF00FF,
    [] (data& d) { }
    ).is_starting(),

  gen_power('d', "move right",
    "A special power of human beings, and most other animals, that they earn early in their life.",
    ">", 0xFF0000FF,
    [] (data& d) { if(d.keystate & 1) d.dx += 1; }
    ).is_starting(),

  gen_power('a', "move left",
    "Moving to the right was a mistake? If so, this special power can be used to ignore the consequences. In most cases, at least...",
    "<", 0xFF0000FF,
    [] (data& d) { if(d.keystate & 1) d.dx -= 1; }
    ).is_starting(),

  gen_power('w', "jump",
    "This power can be used to reach higher parts of the world. Its power is quite limited compared to move left and right, but "
    "you expect to find some ways to make it more powerful.",
    "^", 0xFF0000FF,
    [] (data& d) {
      if(d.keystate & 1) {
        bool can_jump = m.on_floor;
        if(gframeid <= m.on_floor_when + m.coyote_time) can_jump = true;
        if(can_jump) m.vel_y = -(non_hyperbolic ? 3 : 5) * d.d * d.modv, m.on_floor_when = -1000;
        }
      }
    ).is_starting(),

  gen_power('s', "fall",
    "If you are on a platform, this ability can be used to drop down.",
    "v", 0xFF0000FF,
    [] (data& d) {
      m.fallthru = (d.keystate & 1);
      }
    ).is_starting(),

  gen_power('p', "pause",
    "Becoming an alchemist requires intelligence: thinking quickly to react to surprising effects of experiments. "  
    "To reflect this, you can use this power at any time to give yourself more time to think about the situation.",
    "-", 0xFF0000FF,
    [] (data& d) {
      if(d.keystate == 1) cmode = (cmode == mode::paused ? mode::playing : mode::paused);
      }).is_starting().while_paused(),

  gen_power(' ', "dagger",
    "This sharp dagger is very useful during the preparation of alchemical ingredients, but it works as a basic weapon too.",
    ")", 0xFFFFFFFF,
    [] (data& d) {
      if(d.keystate != 1) return;
      m.attack_facing = m.facing; m.attack_when = gframeid;
      auto pb = m.get_pixel_bbox_at(m.where_x + m.attack_facing * m.dsx(), m.where_y);
      auto bb = pixel_to_block(pb);
      for(auto& e: current_room->entities) if(intersect(e->get_pixel_bbox(), pb)) e->attacked(15);
      for(int y=bb.miny; y<bb.maxy; y++)
      for(int x=bb.minx; x<bb.maxx; x++) {
        int b = current_room->at(x, y);
        if(b == wDoor) {
          current_room->replace_block(x, y, wSmashedDoor);
          addMessage("You smash the door!");
          }
        }
      }).be_weapon(),

  gen_power('o', "strange blue crystal ball", "You feel an urge to look into it.",
    "o", 0x00FF00FF,
    [] (data& d) {
      if(d.keystate != 1) return;
      d.p->flags |= IDENTIFIED;
      switch_mapmode_to(cmapmode == mapmode::poincare ? mapmode::standard : mapmode::poincare);
      }
    ).while_paused().identified_name("Poincaré's Crystal Ball", "This crystal ball will not let you predict the future or see things elsewhere, but will let you easily map the "
          "parts of the world you have seen so far. This artifact is rumored to have been actually created by Beltrami, but "
          "it was bought and presented to people by the famous wizard Poincaré, and people thought it was Poincaré's creation."),

  gen_power('b', "strange cyan crystal ball", "You feel an urge to look into it.",
    "o", 0x00FFFFFF,
    [] (data& d) {
      if(d.keystate != 1) return;
      d.p->flags |= IDENTIFIED;
      switch_mapmode_to(cmapmode == mapmode::klein ? mapmode::standard : mapmode::klein);
      }
    ).while_paused().identified_name("Beltrami's Crystal Ball","Created by the ancient wizard Beltrami, this crystal ball will not let you predict the future or see things elsewhere, "
          "but will let you easily map the parts of the world you have seen so far. Contrary to Poincaré's ball, straight lines are "
          "mapped faithfully."),

  gen_power('c', "furry ring",
    "This strange ring is too small to put on your finger, but maybe you could put it on your small toe?",
    "=", 0xe1cbbeFF,
    [] (data& d) {
      if(d.keystate == 1) {
        d.p->flags ^= ACTIVE;
        if(d.p->flags & ACTIVE) addMessage("You put the " + d.p->get_name() + " on your toe.");
        else addMessage("You remove the " + d.p->get_name() + " from your toe.");
        }
      if(d.p->flags & ACTIVE) m.next_coyote_time += 30;
      if(!(d.p->flags & IDENTIFIED) && (gframeid <= m.on_floor_when + m.coyote_time) && !m.on_floor) {
        d.p->flags |= IDENTIFIED;
        addMessage("You feel a strange magical force wanting to hold your foot from below.");
        }
      }
    ).identified_name("Toe Ring of the Coyote", "This ring, worn on a toe, will let you still jump after running off a platform. Just make sure that you run off with the foot that you are wearing this ring on!"),

  gen_power('g', "Golden Shoelaces",
    "These shoelaces might bind you into place or give you freedom... or they could just be mundane shoelaces for rich people... one way to tell.",
    "=", 0xFFD500FF,
    [] (data& d) {
      if(d.keystate == 1) {
        d.p->flags ^= ACTIVE;
        if(d.p->flags & ACTIVE) addMessage("You put the Golden Shoelaces on your boots.");
        else addMessage("You remove the Golden Shoelaces from your boots.");
        }
      if(d.p->flags & ACTIVE) {
        m.next_jump_control++;
        auto& ids = d.p->id_status;
        bool id_up = (!!m.on_floor) == !!(ids & 1);
        if(id_up) {
          ids++;
          if(ids == 2) {
            addMessage("Something felt strange during this jump.");
            }
          if(ids == 4) {
            addMessage("This jump was strange too.");
            }
          if(ids == 6) {
            addMessage("You feel in control of the laws of physics.");
            }
          if(ids == 8) {
            addMessage("You feel able to control your jumps while flying.");
            }
          if(ids == 10) {
            d.p->flags |= IDENTIFIED;
            addMessage("This control originates from your golden shoelaces.");
            }
          }
        }
      }
    ).identified_name("Golden Shoelaces", "Normally you cannot control your jumps while you are flying. These shoelaces allow you some control over your jumps."),

  gen_power('r', "steel ring",
    "Is it safe to put this ring on?",
    "=", 0xC04040FF,
    [] (data& d) {
      if(d.keystate == 1) {
        d.p->flags ^= ACTIVE;
        d.p->flags |= IDENTIFIED;
        if(d.p->flags & ACTIVE) addMessage("You put the Ring of Strength on your finger.");
        else addMessage("You remove the Ring of Strength from your finger.");
        }
      }
    ).identified_name("Ring of Strength", "This will raise your strength!"),

  gen_power('g', "gold",
    "For some weird reason, people love gold, and they will give you anything if you give them enough gold.\n\n"
    "This can be used to buy things in shops. "
    "Just stand on the item, press the hotkey, go to the shopkeeper, and press the hotkey again.\n\n"
    "If you decide not to buy, press the hotkey without going to the shopkeeper.",
    "$", 0xFFD500FF,
    [] (data& d) {}
    ).be_resource("pieces of gold");

  };

void handle_powers(data& d) {
  for(auto& p: powers) {
    if(!p.qty_owned) continue;
    d.keystate = 0;
    if(keyheld(p.key)) d.keystate |= 1;
    if(keywasheld(p.key)) d.keystate |= 2;
    d.p = &p;
    if(cmode == mode::paused) p.paused_act(d);
    else p.act(d);
    }
  }

void draw_inventory() {
  initquickqueue();
  for(int a: {0, 1, 3, 2, 0})
    curvepoint(hyperpoint((a&1)?16:screen_x-16, (a&2)?16:screen_y-16, 1, 0));
  queuecurve(scrm, 0xFFFFFFFF, 0x000000E0, PPR::LINE);
  quickqueue();
  int sy = 200 * vid.yres / 1440;
  int sx = 100 * vid.xres / 2560;
  auto st = vid.fsize * 1.2;
  displaystr(sx, sy, 0, vid.fsize, "Your inventory:", 0xC0C0C0, 0);
  int lineid = 2;
  for(auto& p: powers) if(p.qty_owned) {
    string s = ""; s += dialog::keyname(p.key);
    s += " ";
    s += p.get_glyph();
    s += " ";
    s += p.get_name();
    displaystr(sx, sy + st * (lineid++), 0, vid.fsize, s, p.color >> 8, 0);
    }
  }

}
