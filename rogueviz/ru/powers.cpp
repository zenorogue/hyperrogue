namespace rogue_unlike {

flagtype STARTING = Flag(1);
flagtype WHILE_PAUSED = Flag(2);
flagtype TOGGLEABLE = Flag(4);
flagtype ACTIVE = Flag(8);
flagtype NEEDS_IDENTIFY = Flag(16);

vector<power> powers = {
  power('1', "Potion of Extra Life",
    "You are really proud of this potion, which, after you die, will let you return to the moment of time when you drank it. "
    "Unfortunately it still requires an ingredient found only in the magical fountains of the Dungeons of Alchemy.\n\n"
    "You can only drink this potion when at a magical fountain. To protect yourself from dying permanently, it is drank "
    "automatically whenever you are at a magical fountain.",
    "!", 0xFFFF00FF, STARTING,
    [] (data& d) { }
    ),

  power('d', "move right",
    "A special power of human beings, and most other animals, that they earn early in their life.",
    ">", 0xFF0000FF, STARTING,
    [] (data& d) { if(d.keystate & 1) d.dx += 1; }
    ),

  power('a', "move left",
    "Moving to the right was a mistake? If so, this special power can be used to ignore the consequences. In most cases, at least...",
    "<", 0xFF0000FF, STARTING,
    [] (data& d) { if(d.keystate & 1) d.dx -= 1; }
    ),

  power('w', "jump",
    "This power can be used to reach higher parts of the world. Its power is quite limited compared to move left and right, but "
    "you expect to find some ways to make it more powerful.",
    "^", 0xFF0000FF, STARTING,
    [] (data& d) {
      if(d.keystate & 1) {
        bool can_jump = m.on_floor;
        println(hlog, "on_floor_when = ", m.on_floor_when, " gframeid = ", gframeid, " coyote_time = ", m.coyote_time);
        if(gframeid <= m.on_floor_when + m.coyote_time) can_jump = true;
        if(can_jump) m.vel_y = -(non_hyperbolic ? 3 : 5) * d.d * d.modv, m.on_floor_when = -1000;
        }
      }
    ),

  power('s', "fall",
    "If you are on a platform, this ability can be used to drop down.",
    "v", 0xFF0000FF, STARTING,
    [] (data& d) {
      m.fallthru = (d.keystate & 1);
      }
    ),

  power('p', "pause",
    "Becoming an alchemist requires intelligence: thinking quickly to react to surprising effects of experiments. "  
    "To reflect this, you can use this power at any time to give yourself more time to think about the situation.",
    "-", 0xFF0000FF, STARTING | WHILE_PAUSED,
    [] (data& d) {
      if(d.keystate == 1) cmode = mode::paused;
      }),

  power(' ', "dagger",
    "This sharp dagger is very useful during the preparation of alchemical ingredients, but it works as a basic weapon too.",
    ")", 0xFFFFFFFF, 0, 
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
      }),

  power('o', "strange blue crystal ball", "You feel an urge to look into it.",
    "o", 0x00FF00FF, WHILE_PAUSED | NEEDS_IDENTIFY,
    [] (data& d) {
      if(!(d.p->flags & NEEDS_IDENTIFY))
        d.p->name = "Poincaré's Crystal Ball",
        d.p->desc = 
          "This crystal ball will not let you predict the future or see things elsewhere, but will let you easily map the "
          "parts of the world you have seen so far. This artifact is rumored to have been actually created by Beltrami, but "
          "it was bought and presented to people by the famous wizard Poincaré, and people thought it was Poincaré's creation.";
      if(d.keystate != 1) return;
      d.p->flags &=~ NEEDS_IDENTIFY;
      switch_mapmode_to(cmapmode == mapmode::poincare ? mapmode::standard : mapmode::poincare);
      }
    ),

  power('b', "strange cyan crystal ball", "You feel an urge to look into it.",
    "o", 0x00FFFFFF, WHILE_PAUSED | NEEDS_IDENTIFY,
    [] (data& d) {
      if(!(d.p->flags & NEEDS_IDENTIFY))
        d.p->name = "Beltrami's Crystal Ball",
        d.p->desc =
          "Created by the ancient wizard Beltrami, this crystal ball will not let you predict the future or see things elsewhere, "
          "but will let you easily map the parts of the world you have seen so far. Contrary to Poincaré's ball, straight lines are "
          "mapped faithfully.";
      if(d.keystate != 1) return;
      d.p->flags &=~ NEEDS_IDENTIFY;
      switch_mapmode_to(cmapmode == mapmode::klein ? mapmode::standard : mapmode::klein);
      }
    ),

  power('c', "furry ring",
    "This strange ring is too small to put on your finger, but maybe you could put it on your small toe?",
    "=", 0xe1cbbeFF, TOGGLEABLE | NEEDS_IDENTIFY,
    [] (data& d) {
      if(!(d.p->flags & NEEDS_IDENTIFY))
        d.p->name = "Toe Ring of the Coyote",
        d.p->desc =
          "This ring, worn on a toe, will let you still jump after running off a platform. Just make sure that you run off with the foot that you are wearing this ring on!";
      if(d.keystate == 1) {
        d.p->flags ^= ACTIVE;
        if(d.p->flags & ACTIVE) addMessage("You put the " + d.p->name + " on your toe.");
        else addMessage("You remove the " + d.p->name + " from your toe.");
        }
      if(d.p->flags & ACTIVE) m.next_coyote_time += 30;
      if((d.p->flags & NEEDS_IDENTIFY) && (gframeid <= m.on_floor_when + m.coyote_time) && !m.on_floor) {
        d.p->flags &=~ NEEDS_IDENTIFY;
        addMessage("You feel a strange magical force wanting to hold your foot from below.");
        }
      }
    ),

  power('g', "Golden Shoelaces",
    "These shoelaces might bind you into place or give you freedom... or they could just be mundane shoelaces for rich people... one way to tell.",
    "=", 0xFFD500FF, TOGGLEABLE,
    [] (data& d) {
      if(!(d.p->flags & NEEDS_IDENTIFY))
        d.p->desc =
         "Normally you cannot control your jumps while you are flying. These shoelaces allow you some control over your jumps.";
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
            d.p->flags |= NEEDS_IDENTIFY;
            addMessage("This control originates from your golden shoelaces.");
            }
          }
        }
      }
    ),

  power('r', "steel ring",
    "Is it safe to put this ring on?",
    "=", 0xC04040FF, TOGGLEABLE | NEEDS_IDENTIFY,
    [] (data& d) {
      if(!(d.p->flags & NEEDS_IDENTIFY))
        d.p->name = "Ring of Strength",
        d.p->desc =
          "This will raise your strength!";
      if(d.keystate == 1) {
        d.p->flags ^= ACTIVE;
        d.p->flags &=~ NEEDS_IDENTIFY;
        if(d.p->flags & ACTIVE) addMessage("You put the Ring of Strength on your finger.");
        else addMessage("You remove the Ring of Strength from your finger.");
        }
      }
    ),

  };

void handle_powers(data& d) {
  for(auto& p: powers) {
    if(cmode == mode::paused && !(p.flags & WHILE_PAUSED)) continue;
    d.keystate = 0;
    if(keyheld(p.key)) d.keystate |= 1;
    if(keywasheld(p.key)) d.keystate |= 2;
    d.p = &p;
    p.pf(d);
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
  for(auto& p: powers) {
    string s = ""; s += dialog::keyname(p.key);
    s += " ";
    s += p.glyph;
    s += " ";
    s += p.name;
    displaystr(sx, sy + st * (lineid++), 0, vid.fsize, s, p.color >> 8, 0);
    }
  }

}
