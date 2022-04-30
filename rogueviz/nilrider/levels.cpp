namespace nilrider {

ld f_heisenberg0(hyperpoint h) { return 0; }

ld rot_plane(hyperpoint h) {
  return h[0] * h[1] / 2;
  }

ld f_rot_well(hyperpoint h) {
  return h[0] * h[1] / 2 + h[0] * h[0] + h[1] * h[1];
  }

ld long_x(hyperpoint h) {
  return h[0] * h[1];
  }

ld geodesics_0(hyperpoint h) {
  ld r = hypot_d(2, h);
  ld phi = atan2(h[1], h[0]);
  
  ld z = (phi / 2 / M_PI) * (M_PI * r * r + 2 * M_PI);
  return z + rot_plane(h);
  }

ld geodesics_at_4(hyperpoint h) {
  ld r = 4;
  ld phi = atan2(h[1], h[0]);
  
  ld z = (phi / 2 / M_PI) * (M_PI * r * r + 2 * M_PI);
  return z + rot_plane(h);
  }

map<char, color_t> bcols = {
  {' ', 0xFF101010},
  {'W', 0xFFFFFFFF},
  {'g', 0xFF008000},
  {'h', 0xFF20A020},
  {'r', 0xFFFF4040},
  {'u', 0xFF4040FF},
  {'b', 0xFF804000},
  {'l', 0xFF0000C0},
  {'f', 0xFF603000},
  {'F', 0xFF804000},
  {'2', 0xFF404040},
  {'4', 0xFF808080},
  {'6', 0xFFC0C0C0},
  };

map<char, array<string, 16> > submaps = {
  {'o', {
    "WWWWWWWWWWWWWWWW",
    "W22222222222222W",
    "W22222666622222W",
    "W22266222266222W",
    "W22622222222622W",
    "W22622222222622W",
    "W26222222222262W",
    "W262222WW222262W",
    "W262222WW222262W",
    "W26222222222262W",
    "W22622222222622W",
    "W22622222222622W",
    "W22266222266222W",
    "W22222666622222W",
    "W22222222222222W",
    "WWWWWWWWWWWWWWWW"
    }},
  {'x', {
    "WWWWWWWWWWWWWWWW",
    "W22222222222222W",
    "W22222222222222W",
    "W22222222222222W",
    "W22222222222222W",
    "W22222222222222W",
    "W22222622622222W",
    "W222222rW222222W",
    "W222222Wr222222W",
    "W22222622622222W",
    "W22222222222222W",
    "W22222222222222W",
    "W22222222222222W",
    "W22222222222222W",
    "W22222222222222W",
    "WWWWWWWWWWWWWWWW"
    }},
  {'b', {
    "                ",
    " rrr rrr rrr rrr",
    "                ",
    "rr rrr rrr rrr r",
    "                ",
    " rrr rrr rrr rrr",
    "                ",
    "rr rrr rrr rrr r",
    "                ",
    " rrr rrr rrr rrr",
    "                ",
    "rr rrr rrr rrr r",
    "                ",
    " rrr rrr rrr rrr",
    "                ",
    "rr rrr rrr rrr r",
    }},
  {'f', {
    "FfFfFfFfFfFfFfFf",
    "fFfFfFfFfFfFfFfF",
    "FfFfFfFfFfFfFfFf",
    "fFfFfFfFfFfFfFfF",
    "FfFfFfFfFfFfFfFf",
    "fFfFfFfFfFfFfFfF",
    "FfFfFfFfFfFfFfFf",
    "fFfFfFfFfFfFfFfF",
    "FfFfFfFfFfFfFfFf",
    "fFfFfFfFfFfFfFfF",
    "FfFfFfFfFfFfFfFf",
    "fFfFfFfFfFfFfFfF",
    "FfFfFfFfFfFfFfFf",
    "fFfFfFfFfFfFfFfF",
    "FfFfFfFfFfFfFfFf",
    "fFfFfFfFfFfFfFfF",
    }},
  {'l', {
    "llllllllllllllll",
    "llllllllllllllll",
    "llllllllllllllll",
    "llllllllllllllll",
    "llllllllllllllll",
    "llllllllllllllll",
    "llllllllllllllll",
    "llllllllllllllll",
    "llllllllllllllll",
    "llllllllllllllll",
    "llllllllllllllll",
    "llllllllllllllll",
    "llllllllllllllll",
    "llllllllllllllll",
    "llllllllllllllll",
    "llllllllllllllll",
    }},
  {'g', {
    "ghghghghghghghgh",
    "hghghghghghghghg",
    "ghghghghghghghgh",
    "hghghghghghghghg",
    "ghghghghghghghgh",
    "hghghghghghghghg",
    "ghghghghghghghgh",
    "hghghghghghghghg",
    "ghghghghghghghgh",
    "hghghghghghghghg",
    "ghghghghghghghgh",
    "hghghghghghghghg",
    "ghghghghghghghgh",
    "hghghghghghghghg",
    "ghghghghghghghgh",
    "hghghghghghghghg",
    }},
  {'G', {
    "ghghghghghghghgh",
    "hghghghghghWhghg",
    "ghghrhghghWlWhgh",
    "hghrWrhghghWhghg",
    "ghghrhghghghghgh",
    "hghghghghghghghg",
    "ghghghghghghghgh",
    "hghghghlhghghghg",
    "ghghghlWlhghghgh",
    "hghghghlhghghghg",
    "ghghghghghghgrgh",
    "hghglghghghgrWrg",
    "ghglWlghghghgrgh",
    "hghglghghghghghg",
    "ghghghghghghghgh",
    "hghghghghghghghg",
    }},
  {'r', {
    "rrrrrrrrrrrrrrru",
    "ubbbbbbbbbbbbbbu",
    "ubbbbbbbbbbbbbbu",
    "ubbbbbbbbbbbbbbu",
    "ubbbbbbbbbbbbbbu",
    "ubbbbbbbbbbbbbbu",
    "ubbbbbbbbbbbbbbu",
    "ubbbbbbbbbbbbbbu",
    "ubbbbbbbbbbbbbbu",
    "ubbbbbbbbbbbbbbu",
    "ubbbbbbbbbbbbbbu",
    "ubbbbbbbbbbbbbbu",
    "ubbbbbbbbbbbbbbu",
    "ubbbbbbbbbbbbbbu",
    "ubbbbbbbbbbbbbbu",
    "urrrrrrrrrrrrrrr",
    }},
  {'*', {
    "WWWWWW WW WWWWWW",
    "W              W",
    "W              W",
    "W              W",
    "W              W",
    "W      rr       W",
    "       rr       ",
    "W     r  r     W",
    "W     r  r     W",
    "     r    r     ",
    "W    r    r    W",
    "W   rrrrrrrr   W",
    "W              W",
    "W              W",
    "W              W",
    "WWWWWW WW WWWWWW",
    }},
  {'+', {
    "gh     WW     gh",
    "hg     WW     hg",
    "       WW       ",
    "                ",
    "                ",
    "       WW       ",
    "       WW       ",
    "WWW  WWWWWW  WWW",
    "WWW  WWWWWW  WWW",
    "       WW       ",
    "       WW       ",
    "                ",
    "                ",
    "       WW       ",
    "gh     WW     gh",
    "hg     WW     hg",
    }},
  {'-', {
    "ghghghghghghghgh",
    "hghghghghghghghg",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "WWW  WWWWWW  WWW",
    "WWW  WWWWWW  WWW",
    "                ",
    "                ",
    "                ",
    "                ",
    "                ",
    "ghghghghghghghgh",
    "hghghghghghghghg",
    }},
  {'|', {
    "gh     WW     gh",
    "hg     WW     hg",
    "gh     WW     gh",
    "hg            hg",
    "gh            gh",
    "hg     WW     hg",
    "gh     WW     gh",
    "hg     WW     hg",
    "gh     WW     gh",
    "hg     WW     hg",
    "gh     WW     gh",
    "hg            hg",
    "gh            gh",
    "hg     WW     hg",
    "gh     WW     gh",
    "hg     WW     hg",
    }},
  };

level rotplane(
  "Trying to be horizontal", 'r', 0, 
  "All the lines going through the center are horizontal.",
  -7.5*dft_block, 7.5*dft_block, 8.5*dft_block, -8.5*dft_block,
  {
  "ggggggggggggggg!",
  "ggggggfffgggggg!",
  "ggggggfffgggggg!",
  "gggg|ggggg|gggg!",
  "ggg-*-----*-ggg!",
  "gggg|ggggf|gggg!",
  "ggGg|g+ggg|grgG!",
  "gGgg|g|xgo|gggg!",
  "ggGg|g|ggg|grgg!",
  "gggg|g|ggg|gggg!",
  "gg--*-+---*--gg!",
  "gggg|ggggg|gggg!",
  "gggggggGGgggggg!",
  "ggggggggggggggg!",
  "ggggggggggggggg!",
  "!!!!!!!!!!!!!!!!"
  },
  6, 6,
  rot_plane
  );

level longtrack(
  "A Long Track", 'l', 0,
  "The main street is horizontal, as well as the lines orthogonal to it.",
  0*dft_block, +2.5*dft_block, 64*dft_block, -1.5*dft_block,
  {
  "gggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg",
  "ggggggggrggggggrggggggggrGggggggggGGggggGGGgggggGGGGggggggggggGG",
  "g+------------------------------------------------------------*G",
  "gggggfffffggggggggggggggggggggggggggggggggggggggggggggggggggggGG"
  },
  0, 1,
  long_x
  );

level geodesical(
  "Roads are Geodesics", 'g', nrlPolar,
  "All the roads here are helical geodesics.",  
  -45*degree, 3*dft_block, 225*degree, 0,
  // -8*dft_block, +8*dft_block, +8*dft_block, 0, 
  {
  "ffffffffffffffff",
  "----------------",
  "----------------",
  "----------------",
  "----------------",
  "----------------",
  "----------------",
  "bbbbbbbbbbbbbbbb",
  },
  0, 6,
  geodesics_0
  );

level geodesical4(
  "Helical Geodesic", 's', nrlPolar,
  "The main road here is a helical geodesic. Orthogonal lines are horizontal.",  
  -80*degree, 8.5*dft_block, 260*degree, 0.5*dft_block,
  // -8*dft_block, +8*dft_block, +8*dft_block, 0, 
  {
  "!!!!!!!!!!!!!!!!",
  "ffffffffffffffff",
  "gggggggggggggggg",
  "ggGggggggggGgggg",
  "+--------------+",
  "gggggGggggGggggg",
  "gggGgggggGgggggg",
  "ffffffffffffffff",
  },
  0, 5,
  geodesics_at_4
  );

level heisenberg0(
  "Heisenberg Zero", 'z', 0,
  "This is the plane z=0 in the Heisenberg group model of Nil. The roads are x=0, y=0 axes.",  
  -7.5*dft_block, 7.5*dft_block, 8.5*dft_block, -8.5*dft_block,
  {
  "ggggggg|ggggggg!",
  "grggggg|gggggrg!",
  "ggggggg|ggggggg!",
  "gggffgg|ggggggg!",
  "gggffgg|ggfrggg!",
  "ggggggg|gggggGg!",
  "ggggggg|ggggggg!",
  "-------+-------!",
  "ggggggg|ggggggg!",
  "gggGgog|ggggggg!",
  "ggggggg|ggrgrgg!",
  "gggGgGg|ggggggg!",
  "ggggggg|ggggggg!",
  "grggggg|gggggrg!",
  "ggggggg|ggggggg!",
  "!!!!!!!!!!!!!!!!"
  },
  8, 8,
  f_heisenberg0
  );

level rotwell(
  "Deep Well", 'd', 0,
  "Can you escape this well?",
  -7.5*dft_block, 7.5*dft_block, 8.5*dft_block, -8.5*dft_block,
  {
  "ggggggggggggggg!",
  "gogggggggggggog!",
  "ggggg--*--ggggg!",
  "gggg*ggggg*gggg!",
  "ggg*ggGfGgg*ggg!",
  "gg|ggfgggfgg|gg!",
  "gg|gGgggggGg|gg!",
  "gg*gfggxggfg*gg!",
  "gg|gGgggggGg|gg!",
  "gg|ggfgggfgg|gg!",
  "ggg*ggGfGgg*ggg!",
  "gggg*ggggg*gggg!",
  "ggggg--*--ggggg!",
  "gogggggggggggog!",
  "ggggggggggggggg!",
  "!!!!!!!!!!!!!!!!"
  },
  8, 8,
  f_rot_well
  );

level labyrinth(
  "Labyrinth", 'l', 0,
  "Go clockwise. The squares of this level have half of their usual length.",
  -7.5*dft_block/2, 7.5*dft_block/2, 8.5*dft_block/2, -8.5*dft_block/2,
  {
  "ogggrfffffffffo!",
  "g*ggrgggggggggg!",
  "ggggrgggggggggg!",
  "ggggrgggggggggg!",
  "ggggrgggrrggggg!",
  "ggggrgGGGrrgggg!",
  "ggggrGgggGrgggg!",
  "ggggrGgxgGrgggg!",
  "ggggrGgggGrgggg!",
  "ggggrrGGGrrgggg!",
  "gggggrrrrrggggg!",
  "ggggggggggggggg!",
  "ggggggggggggggg!",
  "ggggggggggggggg!",
  "offfffffffffffo!",
  "!!!!!!!!!!!!!!!!"
  },
  8, 8,
  rot_plane
  );

level *curlev = &rotplane;

vector<level*> all_levels = {
  &rotplane, &longtrack, &geodesical, &geodesical4, &heisenberg0, &rotwell, &labyrinth
  };
  
}
