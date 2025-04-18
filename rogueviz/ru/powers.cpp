namespace rogue_unlike {

flagtype STARTING = Flag(1);
flagtype WHILE_PAUSED = Flag(2);

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
      if((d.keystate & 1) && m.on_floor) m.vel_y = -(non_hyperbolic ? 3 : 5) * d.d * d.modv;
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
      auto bb = pixel_to_block(m.get_pixel_bbox_at(m.where_x + m.attack_facing * m.dsx(), m.where_y));
      for(int y=bb.miny; y<bb.maxy; y++)
      for(int x=bb.minx; x<bb.maxx; x++) {
        int b = current_room->at(x, y);
        if(b == wDoor) {
          current_room->replace_block(x, y, wSmashedDoor);
          addMessage("You smash the door!");
          }
        }
      }),

  power('o', "Poincaré's Crystal Ball",
    "This crystal ball will not let you predict the future or see things elsewhere, but will let you easily map the "
    "parts of the world you have seen so far. This artifact is rumored to have been actually created by Beltrami, but "
    "it was bought and presented to people by the famous wizard Poincaré, and people thought it was Poincaré's creation.",
    "o", 0x00FF00FF, WHILE_PAUSED,
    [] (data& d) {
      if(d.keystate != 1) return;
      switch_mapmode_to(cmapmode == mapmode::poincare ? mapmode::standard : mapmode::poincare);
      }
    ),

  power('b', "Beltrami's Crystal Ball",
    "Created by the ancient wizard Beltrami, this crystal ball will not let you predict the future or see things elsewhere, "
    "but will let you easily map the parts of the world you have seen so far. Contrary to Poincaré's ball, straight lines are "
    "mapped faithfully.",
    "o", 0x00FFFFFF, WHILE_PAUSED,
    [] (data& d) {
      if(d.keystate != 1) return;
      switch_mapmode_to(cmapmode == mapmode::klein ? mapmode::standard : mapmode::klein);
      }
    ),

  };

void handle_powers(data& d) {
  for(auto& p: powers) {
    if(cmode == mode::paused && !(p.flags & WHILE_PAUSED)) continue;
    d.keystate = 0;
    if(keyheld(p.key)) d.keystate |= 1;
    if(keywasheld(p.key)) d.keystate |= 2;
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
