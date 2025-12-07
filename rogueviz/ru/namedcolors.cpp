namespace rogue_unlike {

struct namedcolor {
  string name;
  color_t col;
  };

namedcolor 
  // cat colors
  nc_red{"red", 0xC04040FF},
  nc_black{"red", 0x505060FF},
  nc_white{"white", 0xF0F0F0FF},
  // hair colors
  nc_blonde{"blonde", 0xF0F0D0FF},
  nc_brownhair{"brown", 0x706030FF},
  // eye colors
  nc_green{"green", 0x40F040FF},
  nc_browneye{"brown", 0xA07030FF};

vector<namedcolor> cat_colors = { nc_red, nc_black, nc_white };

}
