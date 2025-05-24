namespace rogue_unlike {

data fakedata;

power& power::is_starting() { qty_filled = qty_owned = 1; return self; }

power& power::while_paused() { paused_act = act; dead_act = act; return self; }

power& power::while_dead() { dead_act = act; return self; }

power& power::identified_name(string s, string desc) {
  auto gn = get_name;
  get_name = [gn, s, this] () { return (flags & IDENTIFIED) ? s : gn(); };
  auto gd = get_desc;
  get_desc = [gd, s, this] () { return (flags & IDENTIFIED) ? s : gd(); };
  return self;
  }

string addqof(string base, power *p) {
  if(p->qty_filled > 1) base = "+" + its(p->qty_filled) + " " + base;
  if(p->qty_owned > p->qty_filled) base = base + " " + "(+" + its(p->qty_owned - p->qty_filled) + ")";
  return base;
  }


power& power::be_weapon() {
  flags |= WEAPON;
  picked_up = [this] (int x) { qty_owned += x; qty_filled = max(qty_filled, x);  };
  auto gn = get_name; get_name = [gn, this] {
    string s = addqof(gn(), this);
    for(auto& [m, qty]: mods) {
      if(m == mod::burning) s = "flaming " + s;
      if(m == mod::freezing) s = "freezing " + s;
      }
    return s;
    };
  auto gc = get_color; get_color = [gc, this] {
    auto col = gc();
    for(auto& [m, qty]: mods) {
      if(m == mod::burning) col = gradient(0xFFFF00FF, 0xFF0000FF, -1, sin(ticks/100), 1);
      if(m == mod::freezing) col = 0x8080FFFF;
      }
    return col;
    };
  return self;
  }

power& power::be_resource(string s) {
  get_name = [this, s] { return its(qty_filled) + " " + s; };
  return self;
  }

using flavor = pair<string, color_t>;
int next_potion, next_jewelry;

vector<flavor> jewelry_colors = {
  {"steel", 0xA0A0C0FF},
  {"amethyst", 0xC060C0FF},
  {"ruby", 0xC06060FF},
  {"sapphire", 0x6060C0FF},
  {"emerald", 0x60C060FF},
  };

vector<flavor> potion_colors = {
  {"bubbling", 0xC0C0C0FF},
  {"golden", 0xFFD500FF},
  {"red", 0xC00000FF},
  {"blue", 0x0000C0FF},
  {"green", 0x00C000FF},
  {"white", 0xFFFFFFFF},
  {"black", 0x303030FF},
  };

power& power::be_jewelry(string jtype, string xdesc) {
  int nj = next_jewelry++;
  picked_up = [this] (int x) { qty_owned += x; qty_filled = max(qty_filled, x);  };
  get_color = [nj] { return jewelry_colors[nj].second; };
  get_name = [nj, this, jtype] {
    string fname = jewelry_colors[nj].first + " " + jtype;
    if(flags & IDENTIFIED) fname = fname + " of " + name;
    fname = addqof(fname, this);
    return fname;
    };
  get_desc = [this, xdesc] {
    return (flags & IDENTIFIED) ? desc : xdesc;
    };
  return self;
  }

power& power::be_wearable(string wear_effect, string remove_effect, string worn) {
  auto gn = get_name;
  get_name = [this, gn, worn] {
    string s = gn();
    if(flags & ACTIVE) s += worn;
    return s;
    };
  auto ac = act;
  act = [this, ac, wear_effect, remove_effect] (data& d) {
    if(d.keystate == 1) {
      d.p->flags ^= ACTIVE;
      string msg = (d.p->flags & ACTIVE) ? wear_effect : remove_effect;
      if(msg.find("%") != string::npos) msg.replace(msg.find("%"), 1, get_name());
      addMessage(msg);
      }
    ac(d);
    };
  return self;
  }

power& power::be_potion() {
  int np = next_potion++;
  picked_up = [this] (int x) { qty_owned += x; qty_filled = max(qty_filled, x);  };
  get_color = [np] { return potion_colors[np].second; };
  refill = [this] { qty_filled = qty_owned; };
  reshuffle = [this] { random_flavor = rand() % 5040; flags &=~ IDENTIFIED; };
  get_name = [np, this] {
    string fname = potion_colors[np].first + " potion";
    if(flags & (PARTIAL | IDENTIFIED)) fname = fname + " of " + name;
    int insq = 0;
    if(flags & IDENTIFIED)
      for(auto& e: randeffs) {
        if(!insq) fname += " ["; else fname += ", ";
        fname += e->name;
        insq++;
        }
    if(insq) fname += "]";
    fname += " (" + its(qty_filled) + "/" + its(qty_owned) + ")";
    if(flags & ACTIVE) fname += " [active]";
    return fname;
    };
  auto gd = get_desc;
  get_desc = [this, gd] () -> string {
    if(!(flags & (PARTIAL | IDENTIFIED)))
      return "You will need to drink this potion to identify it.";
    else {
      auto desc = gd();
      if(flags & IDENTIFIED)
        for(auto& e: randeffs)
          desc += e->desc;
      return desc;
      }
    };
  return self;
  }

void random_potion_act(data& d) {
  if(d.p->qty_filled == d.p->qty_owned) d.p->flags &=~ ACTIVE;
  if(d.keystate == 1 && !(d.p->flags & ACTIVE)) {
    if(d.p->qty_filled == 0) {
      addMessage("You have no more " + d.p->get_name());
      return;
      }
    addMessage("You drink the " + d.p->get_name());
    for(auto& e: d.p->randeffs) {
      if(e->effect != "") addMessage(e->effect);
      d.re = e; d.mode = rev::start;
      e->act(d);
      }
    d.p->flags |= (ACTIVE | PARTIAL | IDENTIFIED);
    d.p->qty_filled--;
    }
  else if(d.p->flags & ACTIVE) {
    for(auto& e: d.p->randeffs) {
      d.re = e; d.mode = rev::active;
      e->act(d);
      }
    }
  };

vector<power> powers;

void power::init() {
  id_status = 0;
  qty_filled = 0; qty_owned = 0;
  flags = 0;
  act = [this] (data& d) { pf(d); };
  paused_act = [] (data&) {};
  dead_act = [] (data&) {};
  get_name = [this] { return name; };
  get_desc = [this] { return desc; };
  get_color = [this] { return color; };
  get_glyph = [this] { return glyph; };
  picked_up = [this] (int x) { qty_filled += x; qty_owned += x; };
  refill = [this] {};
  reshuffle = [this] {};
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

power *extra_life;
int gold_id;
power *dexmode;

void power_death_revert(power& p) {
  int q0 = p.qty_filled;
  int q1 = p.qty_owned;
  add_revert(death_revert, [&p, q0, q1] { p.qty_filled = q0; p.qty_owned = q1; });
  }

void gen_powers() {
  powers.reserve(100);

  extra_life = &gen_power('1', "Extra Life",
    "You are really proud of this potion, which, after you die, will let you return to the moment of time when you drank it. "
    "Unfortunately it still requires an ingredient found only in the magical fountains of the Dungeons of Alchemy.\n\n"
    "You can only drink this potion when at a magical fountain. To protect yourself from dying permanently, when you drink it, "
    "you drink it automatically whenever you are at a magical fountain.",
    "!", 0xFFFF00FF,
    [] (data& d) {
      d.p->flags |= IDENTIFIED;
      if(d.keystate == 1) {
        if(!m.existing) {
          auto w = m.where;
          auto cr = current_room;
          int hp = m.max_hp();
          revert_all(death_revert);
          regenerate_all();
          if(!(extra_life->flags & ACTIVE)) extra_life->qty_filled = 0;
          m.existing = true;
          m.where = fountain_where;
          current_room = fountain_room;
          if(d.p->flags & ACTIVE)
            addMessage("You wake up at the Magic Fountain.");
          else
            addMessage("You wake up from a very bad nightmare. Wow, you are really stressed.");

          if(m.experience >= 50) {
            auto g = std::make_unique<ghost>();
            g->where = w; g->hp = hp; g->xp = m.experience/2; m.experience -= g->xp; g->postfix();
            cr->entities.emplace_back(std::move(g));
            }

          auto bones = std::make_unique<item>();
          bones->qty = 10;
          bones->where = stable_where;
          bones->id = gold_id;
          bones->pickup_message = "You got it back.";
          stable_room->entities.emplace_back(std::move(bones));
          }
        else if(!d.p->qty_filled)
          addMessage("You need to find a Magic Fountain to prepare this potion.");
        else if(d.p->flags & ACTIVE)
          addMessage("This potion is drank automatically whenever you visit a Magic Fountain.");
        else if(!on_fountain)
          addMessage("For safety, you can only drink " + d.p->get_name() + " at the Magic Fountain.");
        else {
          fountain_room = current_room; fountain_where = m.where;
          addMessage("You drink the " + d.p->get_name() + " and you feel that nothing will stop you now!");
          d.p->flags |= ACTIVE;
          }
        }
      }
    ).is_starting().be_potion().while_dead();
  extra_life->qty_filled = 0;

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
        if(gframeid <= m.on_floor_when + m.current.coyote_time) can_jump = true;
        if(can_jump) m.vel.y = m.zero_vel.y-(non_hyperbolic ? 3 : 5) * d.d * d.modv, m.on_floor_when = -1000;
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

  dexmode = &gen_power('c', "chill time",
    "Concentrate to make the timing of your moves perfect.\n\n"
    "From the player's point of view, this makes the game run slower.\n\nThe higher your Dexterity, the slower the game becomes.",
    "-", 0xFF0000FF,
    [] (data& d) {
      }).is_starting().while_paused().be_wearable("You concentrate.", "You calm down.", " (on)"),

  gen_power(' ', "dagger",
    "This sharp dagger is very useful during the preparation of alchemical ingredients, but it works as a basic weapon too.",
    ")", 0xFFFFFFFF,
    [] (data& d) {
      if(d.keystate != 1) return;
      m.attack_facing = m.facing; m.attack_when = gframeid;
      auto pb = m.get_pixel_bbox_at(xy{m.where.x + m.attack_facing * m.dsiz().x, m.where.y});
      auto bb = pixel_to_block(pb);
      for(auto& e: current_room->entities)
        if(e->existing && intersect(e->get_pixel_bbox(), pb)) {
          int sav = e->invinc_end;
          e->attacked((m.current.stats[stat::str] + 1) * 3 / 2);
          for(auto& [m, qty]: d.p->mods) {
            if(m == mod::burning) { e->invinc_end = sav; e->attacked(qty); }
            if(m == mod::freezing) { e->invinc_end = sav; e->attacked(qty); }
            }
          }
      for(int y=bb.miny; y<bb.maxy; y++)
      for(int x=bb.minx; x<bb.maxx; x++) {
        int b = current_room->at(x, y);
        if(b == wDoor) {
          current_room->replace_block_frev(x, y, wSmashedDoor);
          addMessage("You smash the door!");
          }
        for(auto& [m, qty]: d.p->mods) {
          if(m == mod::burning && b == wWoodWall) {
            current_room->replace_block_frev(x, y, wAir);
            addMessage("You burn the wall!");
            }
          if(m == mod::freezing && b == wWater) {
            current_room->replace_block_frev(x, y, wFrozen);
            addMessage("You freeze the water!");
            }
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
      if(d.p->flags & ACTIVE) m.next.coyote_time += 30;
      if(!(d.p->flags & IDENTIFIED) && (gframeid <= m.on_floor_when + m.current.coyote_time) && !m.on_floor && (d.p->flags & ACTIVE)) {
        d.p->flags |= IDENTIFIED;
        addMessage("You feel a strange magical force wanting to hold your foot from below.");
        }
      }
    ).identified_name("Toe Ring of the Coyote", "This ring, worn on a toe, will let you still jump after running off a platform. Just make sure that you run off with the foot that you are wearing this ring on!")
    .be_wearable("You put the % on your toe.", "You remove the % from your toe.");

  gen_power('g', "Golden Shoelaces",
    "These shoelaces might bind you into place or give you freedom... or they could just be mundane shoelaces for rich people... one way to tell.",
    "=", 0xFFD500FF,
    [] (data& d) {
      if(d.p->flags & ACTIVE) {
        m.next.jump_control++;
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
    ).identified_name("Golden Shoelaces", "Normally you cannot control your jumps while you are flying. These shoelaces allow you some control over your jumps.")
    .be_wearable("You put the Golden Shoelaces on your boots.", "You remove the Golden Shoelaces on your boots.");

  gen_power('r', "strength",
    "Wearing this ring will raise your strength.",
    "=", 0xC04040FF,
    [] (data& d) {
      if(d.p->flags & ACTIVE) {
        m.next.stats[stat::str] += d.p->qty_filled;
        d.p->flags |= IDENTIFIED;
        }
      }
    ).be_jewelry("ring", "You need to wear this ring to know what it does.")
     .be_wearable("You put the % on your finger. You feel stronger!", "You remove the %. You feel weaker..."),

  gen_power('j', "toughness",
    "Wearing this ring will raise your toughness.",
    "=", 0xC04040FF,
    [] (data& d) {
      if(d.p->flags & ACTIVE) {
        m.next.stats[stat::str] += d.p->qty_filled;
        d.p->flags |= IDENTIFIED;
        }
      }
    ).be_jewelry("ring", "You need to wear this ring to know what it does.")
     .be_wearable("You put the % on your finger. You feel tougher!", "You remove the %. You feel weaker..."),

  gen_power('y', "wisdom",
    "Wearing this ring will raise your wisdom.",
    "=", 0xC04040FF,
    [] (data& d) {
      if(d.p->flags & ACTIVE) {
        m.next.stats[stat::str] += d.p->qty_filled;
        d.p->flags |= IDENTIFIED;
        }
      }
    ).be_jewelry("ring", "You need to wear this ring to know what it does.")
     .be_wearable("You put the % on your finger. You feel wiser!", "You remove the %. You feel stupid..."),

  gen_power('x', "dexterity",
    "Wearing this ring will raise your dexterity.",
    "=", 0xC04040FF,
    [] (data& d) {
      if(d.p->flags & ACTIVE) {
        m.next.stats[stat::str] += d.p->qty_filled;
        d.p->flags |= IDENTIFIED;
        }
      }
    ).be_jewelry("ring", "You need to wear this ring to know what it does.")
     .be_wearable("You put the % on your finger. You feel better with bows!", "You remove the %. You feel worse with bows..."),

  gen_power('2', "health",
    "This will cure your wounds, in some way.",
    "!", 0xFFFF00FF,
    random_potion_act
    ).be_potion(),

  gen_power('3', "reach",
    "This will let you reach heights, in some way.",
    "!", 0xFFFF00FF,
    random_potion_act
    ).be_potion(),

  gen_power('4', "fire",
    "This will let you produce fire, in some way.",
    "!", 0xFFFF00FF,
    random_potion_act
    ).be_potion(),

  gen_power('5', "polymorph",
    "This will let you change into a small creature.",
    "!", 0xFFFF00FF,
    random_potion_act
    ).be_potion(),

  gen_power('6', "the thief",
    "This will let you bypass trapped areas, in some way.",
    "!", 0xFFFF00FF,
    random_potion_act
    ).be_potion(),

  gen_power('7', "mystery",
    "A potion with some random effect.",
    "!", 0xFFFF00FF,
    random_potion_act
    ).be_potion(),

  gold_id = isize(powers);

  gen_power('t', "gold",
    "For some weird reason, people love gold, and they will give you anything if you give them enough gold.\n\n"
    "This can be used to buy things in shops. "
    "Just stand on the item, press the hotkey, go to the shopkeeper, and press the hotkey again.\n\n"
    "If you decide not to buy, press the hotkey without going to the shopkeeper.",
    "$", 0xFFD500FF,
    [] (data& d) {
      if(d.keystate == 1) {
        trader *tr;
        for(auto& e: current_room->entities) if(auto t = e->as_trader()) tr = t;
        bool on_trader = intersect(tr->get_pixel_bbox(), m.get_pixel_bbox());
        bool done_something = false;
        for(int it: {0, 1})
        for(auto& e: current_room->entities) if(auto si = e->as_shopitem()) {
          bool on = intersect(si->get_pixel_bbox(), m.get_pixel_bbox());
          if(it == 0 && on && si->existing && !si->bought) {
            done_something = true;
            addMessage(si->pickup_message);
            power_death_revert(powers[si->id]);
            powers[si->id].qty_owned += si->qty;  powers[si->id].qty_filled += si->qty1;
            add_revert(death_revert, [si] { si->existing = true; });
            si->existing = false;
            }
          else if(it == 0 && on && si->existing && si->bought) {
            done_something = true;
            addMessage("You get some gold.");
            power_death_revert(powers[gold_id]);
            powers[gold_id].qty_owned += si->price;  powers[gold_id].qty_filled += si->price;
            add_revert(death_revert, [si] { si->existing = true; });
            si->existing = false;
            }
          else if((it ? !done_something : on) && !si->existing && !si->bought) {
            done_something = true;
            addMessage("You rethink your purchase.");
            power_death_revert(powers[si->id]);
            powers[si->id].qty_owned -= si->qty;  powers[si->id].qty_filled -= si->qty1;
            add_revert(death_revert, [si] { si->existing = false; });
            si->existing = true;
            }
          else if((it ? !done_something : on) && !si->existing && si->bought) {
            done_something = true;
            addMessage("You rethink your actions.");
            power_death_revert(powers[gold_id]);
            powers[gold_id].qty_owned -= si->price;  powers[gold_id].qty_filled -= si->price;
            add_revert(death_revert, [si] { si->existing = false; });
            si->existing = true;
            }
          else if(it == 0 && on_trader && !si->existing && d.p->qty_owned >= si->price) {
            done_something = true;
            addMessage("You buy the " + powers[si->id].get_name() + ".");
            power_death_revert(powers[gold_id]);
            powers[gold_id].qty_owned -= si->price;  powers[gold_id].qty_filled -= si->price;
            si->existing = true; si->bought = true;
            add_revert(death_revert, [si] { si->existing = false; si->bought = false; });
            }
          else if(it == 0 && on_trader && !si->existing && !si->bought) {
            done_something = true;
            addMessage("You have not enough gold to buy the " + powers[si->id].get_name() + ".");
            }
          }
        if(!done_something) addMessage("You count your gold. You have " + its(d.p->qty_owned) + " gold.");
        }
      }
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
    else if(!m.existing) p.dead_act(d);
    else p.act(d);
    }
  }

void draw_inventory_frame() {
  flat_model_enabler fme;
  initquickqueue();
  for(int a: {0, 1, 3, 2, 0})
    curvepoint(eupoint((a&1)?16:vid.xres-16, (a&2)?16:vid.yres-16));
  queuecurve(atscreenpos(0, 0), 0xFFFFFFFF, 0x000000E0, PPR::LINE);
  quickqueue();
  }

void draw_inventory() {
  render_the_map();
  draw_inventory_frame();
  dialog::init();
  int next_y = 48;
  int st = vid.fsize * 1.2;
  displaystr(32, next_y, 0, vid.fsize, "Your inventory:", 0xC0C0C0, 0);
  next_y += st * 1.5;
  for(auto& p: powers) if(p.qty_owned) {
    string key = p.key == ' ' ? "␣" : dialog::keyname(p.key);
    if(displaystr(100, next_y, 0, vid.fsize, key, p.get_color() >> 8, 16)) getcstat = p.key;
    if(displaystr(130, next_y, 0, vid.fsize, p.get_glyph(), p.get_color() >> 8, 8)) getcstat = p.key;
    if(displaystr(160, next_y, 0, vid.fsize, p.get_name(), p.get_color() >> 8, 0)) getcstat = p.key;
    next_y += st;
    dialog::add_key_action(p.key, [&p] { pushScreen([&p] {
      render_the_map();
      draw_inventory_frame();
      dialog::init(p.get_name(), p.get_color() >> 8);
      dialog::addHelp(p.get_desc());
      dialog::addItem("press a key to redefine", SDLK_ESCAPE);
      dialog::display();
      dialog::addBack();
      keyhandler = [&p] (int sym, int uni) {
        if(sym == 0) return;
        if(sym == SDLK_ESCAPE) return popScreen();
        for(auto& p1: powers) if(p1.key == sym) p1.key = p.key;
        p.key = sym;
        };
      }); });
    }
  }

void shuffle_all() {
  auto& jc = jewelry_colors;
  for(int i=1; i<isize(jc); i++) swap(jc[i], jc[rand() % (i+1)]);
  auto& pc = potion_colors;
  for(int i=1; i<isize(pc); i++) swap(pc[i], pc[rand() % (i+1)]);
  for(auto& p: powers) p.reshuffle();
  assign_potion_powers();
  }

power& find_power(string name) {
  for(auto& p: powers) if(p.name == name) return p;
  throw hr_exception("unknown power");
  }

}
