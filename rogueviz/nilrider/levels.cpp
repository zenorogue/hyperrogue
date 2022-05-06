namespace nilrider {

bool all(checkerparam c) { return c.t->collected_triangles == Flag(isize(c.l->triangles))-1; }

goalchecker basic_check(ld time_limit, ld rev_limit) {
  return [=] (checkerparam c) {
    if(c.t->timer > time_limit || c.rev > rev_limit) return grFailed;
    if(all(c)) return grSuccess;
    return grNone;
    };
  }

goalchecker get_any(ld time_limit, ld rev_limit) {
  return [=] (checkerparam c) {
    if(c.t->timer > time_limit || c.rev > rev_limit) return grFailed;
    if(c.t->collected_triangles) return grSuccess;
    return grNone;
    };
  }

goalchecker get_ordered(ld time_limit, ld rev_limit) {
  return [=] (checkerparam c) {
    if(c.t->timer > time_limit || c.rev > rev_limit) return grFailed;
    if(c.t->collected_triangles & (c.t->collected_triangles+1)) return grFailed;
    if(all(c)) return grSuccess;
    return grNone;
    };
  }

goalchecker yplus_check(ld time_limit, ld rev_limit) {
  return [=] (checkerparam c) {
    if(c.t->timer > time_limit || c.rev > rev_limit) return grFailed;
    if(c.t->where[1] < 0) return grFailed;
    if(all(c)) return grSuccess;
    return grNone;
    };
  }

goalchecker fullstop_check(ld time_limit, ld rev_limit) {
  return [=] (checkerparam c) {
    if(c.t->timer > time_limit || c.rev > rev_limit) return grFailed;
    if(all(c) && c.t->vel == 0) return grSuccess;
    return grNone;
    };
  }

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
  {'!', 0xFF000000}
  };

const int pixel_per_block = 16;

map<char, array<string, pixel_per_block> > submaps = {
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
  rot_plane,
  {
    // the solver[0.25] result is 36.92
    goal{0x40FF40, "Collect all the triangles in below 60 seconds", basic_check(60, 999)},
    goal{0xFFD500, "Collect all the triangles in below 38 seconds", basic_check(38, 999)}
    }
  );

level longtrack(
  "A Long Track", 'l', 0,
  "The main street is horizontal, as well as the lines orthogonal to it.",
  0*dft_block, +6.5*dft_block, 64*dft_block, -1.5*dft_block,
  {
  "Ggggggggr!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",
  "Ggggggggr!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",
  "Ggggggggr!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!x!",
  "Ggggxgggr!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",
  "gggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggggg",
  "ggggggggrggggggrggggggggrGggggggggGGggggGGGgggggGGGGggggggggggGG",
  "--------------------------------------------------------------*G",
  "gggggfffffggggggggggggggggggggggggggggggggggggggggggggggggggggGG"
  },
  0, 5,
  long_x,
  {
    // the solver[0.25] result is 1:08.56 (reduced to 1:08.45 by removing some points)
    goal{0xFFD500, "Collect the triangle in below 1:15", basic_check(75, 999)},
    // the solver[0.25] + some manual modifications achieves 1:37.44
    goal{0xFF4040, "Stop where the triangle is in below 1:45", fullstop_check(75, 999)},
    // the solver[0.25] result is 1:45.52
    goal{0x303030, "Reach the triangle without going on the right side of the road below 2:00", yplus_check(120, 999)},
    }
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
  "*--------------*",
  "----------------",
  "----------------",
  "----------------",
  "bbbbbbbbbbbbbbbb",
  },
  0, 6,
  geodesics_0,
  {
    // the solver[0.25] result is 26.10
    goal{0xFFD500, "Collect both triangles in below 30 seconds", basic_check(30, 999)}
    }
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
  "+--------------*",
  "gggggGggggGggggg",
  "gggGgggggGgggggg",
  "ffffffffffffffff",
  },
  0, 5,
  geodesics_at_4,
  {
    // the solver[0.25] result is 32.04
    goal{0xFFD500, "Collect the triangle in below 35 seconds", basic_check(35, 999)}
    }
  );

level heisenberg0(
  "Heisenberg Zero", 'z', 0,
  "This is the plane z=0 in the Heisenberg group model of Nil. The roads are x=0, y=0 axes.",  
  -7.5*dft_block, 7.5*dft_block, 8.5*dft_block, -8.5*dft_block,
  {
  "ggggggg|ggggggg!",
  "grggggg|gggggrg!",
  "gg*gggg|gggg*gg!",
  "gggffgg|ggggggg!",
  "gggffgg|ggfrggg!",
  "ggggggg|gggggGg!",
  "ggggggg|ggggggg!",
  "-------+-------!",
  "ggggggg|ggggggg!",
  "gggGgog|ggggggg!",
  "ggggggg|ggrgrgg!",
  "gggGgGg|ggggggg!",
  "gg*gggg|gggg*gg!",
  "grggggg|gggggrg!",
  "ggggggg|ggggggg!",
  "!!!!!!!!!!!!!!!!"
  },
  8, 8,
  f_heisenberg0,
  {
    // the solver[0.25] result is 49:15
    goal{0x40FFd0, "Collect all triangles in below 0:55", basic_check(55, 999)}
    }
  );

level rotwell(
  "Deep Well", 'd', nrlOrder,
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
  f_rot_well,
  {
    // the solver[0.5] result is 1:19.54 (obtained using get_ordered)
    goal{0xFFD500, "Collect all triangles below 1:25", basic_check(85, 999)}
    }
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
  rot_plane,
  {
    // the solver[0.1] result is 1:03.53
    // the solver[0.15] result is 1:06.58
    // the solver[0.24] result is 1:08.54
    // the solver[0.25] result is 1:22.09 (it goes north for some reason)
    goal{0xFFD500, "Collect the triangle in below 1:15", basic_check(75, 999)}
    }
  );

level *curlev = &rotplane;

vector<level*> all_levels = {
  &rotplane, &longtrack, &geodesical, &geodesical4, &heisenberg0, &rotwell, &labyrinth
  };
  
}
