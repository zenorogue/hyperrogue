namespace rogue_unlike {

struct flavor {
  string name;
  color_t col;
  };

flavor default_flavor {"magenta", 0xFF00FFFF};

flavor 
  // cat colors
  cc_red{"red", 0xC04040FF},
  cc_black{"black", 0x505060FF},
  cc_white{"white", 0xF0F0F0FF},
  // hair colors
  hc_blonde{"blonde", 0xF0F0D0FF},
  hc_brownhair{"brown", 0x706030FF},
  hc_red{"red", 0xC04040FF},
  // eye colors
  ec_green{"green", 0x40F040FF},
  ec_blue{"green", 0x40F040FF},
  ec_browneye{"brown", 0xA07030FF},

  // potion colors
  pc_bubbling{"bubbling", 0xC0C0C0FF},
  pc_golden{"golden", 0xFFD500FF},
  pc_red{"red", 0xC00000FF},
  pc_blue{"blue", 0x0000C0FF},
  pc_green{"green", 0x00C000FF},
  pc_white{"white", 0xFFFFFFFF},
  pc_black{"black", 0x303030FF},

  // jewelry flavors
  jc_steel{"steel", 0xA0A0C0FF},
  jc_amethyst{"amethyst", 0xC060C0FF},
  jc_ruby{"ruby", 0xC06060FF},
  jc_sapphire{"sapphire", 0x6060C0FF},
  jc_emerald{"emerald", 0x60C060FF};

vector<flavor> cat_colors = { cc_red, cc_black, cc_white };

vector<flavor> potion_colors = { pc_bubbling, pc_golden, pc_red, pc_blue, pc_green, pc_white, pc_black };

vector<flavor> jewelry_colors = { jc_steel, jc_amethyst, jc_ruby, jc_sapphire, jc_emerald };

}
