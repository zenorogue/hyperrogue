namespace nilrider {

bool all(checkerparam c) { return c.t->collected_triangles == Flag(isize(c.l->triangles))-1; }

goalchecker basic_check(ld time_limit, ld rev_limit) {
  return [=] (checkerparam c) {
    if(c.t->timer > time_limit || c.rev > rev_limit) return grFailed;
    if(all(c)) return grSuccess;
    return grNone;
    };
  }

eGoalResult ski_check(checkerparam c) {
  if(c.t->on_surface && c.t->where[0] >= 8) return grSuccess;
  return grNone;
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

ld f_heisenberg0(hyperpoint h) { return nilv::convert_bonus(h, nilv::nmHeis, nilv::model_used); }

ld rot_plane(hyperpoint h) { return nilv::convert_bonus(h, nilv::nmSym, nilv::model_used); }

ld f_rot_well(hyperpoint h) {
  return rot_plane(h) + h[0] * h[0] + h[1] * h[1];
  }

ld long_x(hyperpoint h) {
  return rot_plane(h) + h[0] * h[1] / 2;
  }

ld cycloid(ld x) {
  // for x from 0 to TAU, returns y from 0 to 2
  ld alpha = binsearch(0, TAU, [x] (ld a) {
    ld ax = a - sin(a);
    return ax >= x;
    }, 20);
  return 1 - cos(alpha);
  }

ld cycloid_wave(ld x) {
  /* static bool test = true;
  if(test) {
    for(ld a=0; a<TAU; a += 0.01) printf("%5.3f : %5.3f\n", a, cycloid(a));
    exit(1);
    } */
  int i = floor(x);
  ld xf = x - i;
  return cycloid(xf * TAU) * ((i&1) ? -1 : 1) / TAU;
  }

ld brachistochrone(hyperpoint h) {
  return long_x(h) - cycloid_wave(h[0] / 63) * 63 + h[1] * h[1] / 5;
  }

ld geodesics_0(hyperpoint h) {
  ld r = hypot_d(2, h);
  ld phi = atan2(h[1], h[0]);
  
  ld z = (phi / TAU) * (M_PI * r * r + TAU);
  return z + rot_plane(h);
  }

ld geodesics_at_4(hyperpoint h) {
  ld r = 4;
  ld phi = atan2(h[1], h[0]);
  
  ld z = (phi / TAU) * (M_PI * r * r + TAU);
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
  {'!', 0xFF000000},
  {'Y', 0xFFFFFF80},
  {'y', 0xFFC0C040},
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
  {'~', {
    "WWWWWWWWWWWWWWWW",
    "6WWWWWWWWWWWWWWW",
    "6WWWWWYWWWWWWWWW",
    "6WWWWWWWWWWWWWWW",
    "6WYWWWWWWWYWWWWW",
    "WWWWWWWWWWWWWWWW",
    "6WWWWWWWWWWWWWWW",
    "6WWWWWWWWWWWWWWW",
    "6WWWWWWYWWWWWWWW",
    "6WWWWWWWWWWWWYWW",
    "WWWWWWWWWWWWWWWW",
    "6WWYWWWWWWWWWWWW",
    "6WWWWWWWWWWWWWWW",
    "6WWWWWWWWWYWWWWW",
    "6WWWWWWWWWWWWWWW",
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
  {'T', { /* trampoline */
    "WWWWWWWWWWWWWWWW",
    "WYYYYYYYYYYYYYYW",
    "WYyyyyyyyyyyyyYW",
    "WYyYYYYYYYYYYyYW",
    "WYyYyyyyyyyyYyYW",
    "WYyYyYYYYYYyYyYW",
    "WYyYyYyyyyYyYyYW",
    "WYyYyYyYYyYyYyYW",
    "WYyYyYyYYyYyYyYW",
    "WYyYyYyyyyYyYyYW",
    "WYyYyYYYYYYyYyYW",
    "WYyYyyyyyyyyYyYW",
    "WYyYYYYYYYYYYyYW",
    "WYyyyyyyyyyyyyYW",
    "WYYYYYYYYYYYYYYW",
    "WWWWWWWWWWWWWWWW",
    }},
  {'V', { /* velocity converter */
    "WWWWWWWWWWWWWWWW",
    "WrrrrrrrrrrrrrrW",
    "WrbbrbbrrbbrbbrW",
    "WrbbrbbrrbbrbbrW",
    "WrrrrrrrrrrrrrrW",
    "WrbbrbbrrbbrbbrW",
    "WrbbrbbrrbbrbbrW",
    "WrrrrrrrrrrrrrrW",
    "WrrrrrrrrrrrrrrW",
    "WrbbrbbrrbbrbbrW",
    "WrbbrbbrrbbrbbrW",
    "WrrrrrrrrrrrrrrW",
    "WrbbrbbrrbbrbbrW",
    "WrbbrbbrrbbrbbrW",
    "WrrrrrrrrrrrrrrW",
    "WWWWWWWWWWWWWWWW",
    }},
  };

auto award_stars(double mul, double t1, double t2) {
  return [=] (ld t) { t = ilerp(t1, t2, t); t *= t; t += 1; t *= mul; return t; };
  }

auto award_stars_distance(ld t) {
  return -100 * t / 8;
  }

level rotplane(
  "Trying to be horizontal", 'r', 0, 0,
  "Collect all the triangles!\n\n"
  "All the lines going through the center are horizontal.\n"
  "However, this is Nil geometry. The other lines are NOT horizontal! Clockwise ones slope upwards, and counterclockwise ones slope downwards.\n"
  "Your unicycle is powered only by the gravity. Use that to your advantage!"
  ,

  -7.5*dft_block, 7.5*dft_block, 7.5*dft_block, -7.5*dft_block,
  {
  "ggggggggggggggg",
  "ggggggfffgggggg",
  "ggggggfffgggggg",
  "gggg|ggggg|gggg",
  "ggg-*-----*-ggg",
  "gggg|ggggf|gggg",
  "ggGg|g+ggg|grgG",
  "gGgg|g|xgo|gggg",
  "ggGg|g|ggg|grgg",
  "gggg|g|ggg|gggg",
  "gg--*-+---*--gg",
  "gggg|ggggg|gggg",
  "gggggggGGgggggg",
  "ggggggggggggggg",
  "ggggggggggggggg"
  },
  6, 6, {},
  rot_plane,
  {
    // the solver[0.25] result is 36.92
    goal{0x40FF40, "Collect all the triangles in below 60 seconds", basic_check(60, 999), "ROTPLANE", "Trying to be horizontal", award_stars(100, 60, 38)},
    goal{0xFFD500, "Collect all the triangles in below 38 seconds", basic_check(38, 999), "ROTPLANE2", "", award_stars(100, 60, 38)}
    }
  );

level longtrack(
  "A Long Track", 'l', 0, 100,
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
  0, 5, {},
  long_x,
  {
    // the solver[0.25] result is 1:08.56 (reduced to 1:08.45 by removing some points)
    goal{0xFFD500, "Collect the triangle in below 1:15", basic_check(75, 999), "LONGTRACK", "", award_stars(100, 70, 75)},
    // the solver[0.25] + some manual modifications achieves 1:37.44
    goal{0xFF4040, "Stop where the triangle is in below 1:45", fullstop_check(75, 999), "LONGTRACKSTOP", "", award_stars(100, 100, 105)},
    // the solver[0.25] result is 1:45.52
    goal{0x303030, "Reach the triangle without going on the right side of the road below 2:00", yplus_check(120, 999), "LONGTRACKLEFT", "", award_stars(100, 105, 120)},

    goal{0x40FF40, "Stop where the triangle is without reversing time", basic_check(999, 0), "", "A Long Track", award_stars(100, 70, 900)},
    }
  );

level geodesical(
  "Roads are Geodesics", 'g', nrlPolar, 300,
  "Geodesics are the lines that are (locally) shortest. In the default settings, "
  "the space in Nil Rider is rendered according to the Fermat's principle, that is, "
  "light rays are assumed to be geodesics.\n\n"
  "Geodesics in Nil are horizontal, vertical, and helical. "
  "In this level, all the roads are (fragments of) helical geodesics.",
  -45._deg, 3*dft_block, 225._deg, 0,
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
  0, 6, {},
  geodesics_0,
  {
    // the solver[0.25] result is 26.10
    goal{0xFFD500, "Collect both triangles in below 30 seconds", basic_check(30, 999), "GEODESICS", "", award_stars(100, 27, 30)},
    goal{0x40FF40, "Collect both triangles without reversing time", basic_check(999, 0), "", "Roads are Geodesics", award_stars(100, 27, 30)}
    }
  );

level geodesical4(
  "Helical Geodesic", 's', nrlPolar, 500,
  "The main road here is a helical geodesic. Orthogonal lines are horizontal.",  
  -80._deg, 8.5*dft_block, 260._deg, 0.5*dft_block,
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
  0, 5, {},
  geodesics_at_4,
  {
    // the solver[0.25] result is 32.04
    goal{0xFFD500, "Collect the triangle in below 35 seconds", basic_check(35, 999), "HELICAL", "", award_stars(100, 33, 35)},
    goal{0x40FF40, "Collect the triangle without reversing time", basic_check(999, 0), "", "Helical Geodesic", award_stars(100, 35, 999)},
    }
  );

level heisenberg0(
  "Heisenberg Zero", 'z', 0, 700,
  "This is the plane z=0 in the Heisenberg group model of Nil. The roads are x=0, y=0 axes.",  
  -7.5*dft_block, 7.5*dft_block, 7.5*dft_block, -7.5*dft_block,
  {
  "ggggggg|ggggggg",
  "grggggg|gggggrg",
  "gg*gggg|gggg*gg",
  "gggffgg|ggggggg",
  "gggffgg|ggfrggg",
  "ggggggg|gggggGg",
  "ggggggg|ggggggg",
  "-------+-------",
  "ggggggg|ggggggg",
  "gggGgog|ggggggg",
  "ggggggg|ggrgrgg",
  "gggGgGg|ggggggg",
  "gg*gggg|gggg*gg",
  "grggggg|gggggrg",
  "ggggggg|ggggggg"
  },
  8, 8, {},
  f_heisenberg0,
  {
    // the solver[0.25] result is 49:15
    goal{0x40FFd0, "Collect all triangles in below 0:55", basic_check(55, 999), "HZERO", "", award_stars(100, 51, 55)},
    goal{0x40c040, "Collect all triangle without reversing time", basic_check(999, 0), "", "Heisenberg Zero", award_stars(100, 51, 999)},
    }
  );

level rotwell(
  "Deep Well", 'd', nrlOrder, 900,
  "Can you escape this well?\n\n"
  "The sculpture in the center is built around eight helical geodesics which start in a point on the floor, and all cross in a point in the sky. Try to find that point and "
  "look below!\n\n"
  "Note: you can move the camera freely (using the arrow keys and PageUp/Down/Home/End) while the game is paused."
  ,
  -7.5*dft_block, 7.5*dft_block, 7.5*dft_block, -7.5*dft_block,
  {
  "ggggggggggggggg",
  "gogggggggggggog",
  "ggggg--*--ggggg",
  "gggg*ggggg*gggg",
  "ggg*ggGfGgg*ggg",
  "gg|ggfgggfgg|gg",
  "gg|gGgggggGg|gg",
  "gg*gfggxggfg*gg",
  "gg|gGgggggGg|gg",
  "gg|ggfgggfgg|gg",
  "ggg*ggGfGgg*ggg",
  "gggg*ggggg*gggg",
  "ggggg--*--ggggg",
  "gogggggggggggog",
  "ggggggggggggggg"
  },
  8, 8, {},
  f_rot_well,
  {
    // the solver[0.5] result is 1:19.54 (obtained using get_ordered)
    goal{0xFFD500, "Collect all triangles below 1:25", basic_check(85, 999), "ROTWELL", "", award_stars(100, 80, 85)},
    goal{0x40c040, "Collect all triangle without reversing time", basic_check(999, 0), "", "Deep Well", award_stars(100, 80, 999)}
    }
  );

level labyrinth(
  "Labyrinth", 'l', 0, 1100,
  "You will have to go clockwise this time!\n\n"
  "The squares of this level have half of their usual length.",
  -7.5*dft_block/2, 7.5*dft_block/2, 7.5*dft_block/2, -7.5*dft_block/2,
  {
  "ogggrfffffffffo",
  "g*ggrgggggggggg",
  "ggggrgggggggggg",
  "ggggrgggggggggg",
  "ggggrgggrrggggg",
  "ggggrgGGGrrgggg",
  "ggggrGgggGrgggg",
  "ggggrGgxgGrgggg",
  "ggggrGgggGrgggg",
  "ggggrrGGGrrgggg",
  "gggggrrrrrggggg",
  "ggggggggggggggg",
  "ggggggggggggggg",
  "ggggggggggggggg",
  "offfffffffffffo",
  },
  8, 8, {},
  rot_plane,
  {
    // the solver[0.1] result is 1:03.53
    // the solver[0.15] result is 1:06.58
    // the solver[0.24] result is 1:08.54
    // the solver[0.25] result is 1:22.09 (it goes north for some reason)
    goal{0xFFD500, "Collect the triangle in below 1:15", basic_check(75, 999), "LABYRINTH", "", award_stars(100, 65, 75)},
    goal{0x40c040, "Collect all triangle without reversing time", basic_check(999, 0), "", "Labyrinth", award_stars(100, 75, 999)}
    }
  );

level obstacle(
  "Obstacle Course", 'o', 0, 1200,
  "The main street is horizontal, as well as the lines orthogonal to it.",
  0*dft_block, 2.5*dft_block, 64*dft_block, -2.5*dft_block,
  {
  "ggggggGrggGrgggggggggggggggggggggGrxgggggggggGrggggggggGrggggggo",
  "ggggggGrggGrgggGrgggggGrgggggggggGrgggggggggggggGrgggggGrggggggo",
  "-----------r----r------r----r-----r--r---------r---------------*",
  "ggggggGrgggggggGrgggggGrggggggggggggGrggggggGrgggggggggGrggggggo",
  "ggggggGrgggggggGrgggggggggggggggggggGrgggggggggGrggggggGrggggggo"
  },
  0, 4, {},
  long_x,
  {
    goal{0xFFFFC0, "Collect the triangle in below 1:25, reversing time at most 3 times", basic_check(85, 3), "OBSTACLE1", "", award_stars(100, 70, 85)},
    goal{0xFFD500, "Collect the triangle in below 1:10, reversing time at most 3 times", basic_check(70, 3), "OBSTACLE2", "", award_stars(100, 65, 70)},
    goal{0x40c040, "Collect the triangle without reversing time", basic_check(999, 0), "", "Obstacle Course", award_stars(100, 65, 999)}
    }
  );

level *curlev = &rotplane;

struct complex_surface {
  hyperpoint cur;
  map<pair<int, int>, surface_fun> blocks;

  static transmatrix flatpush(hyperpoint h) { return rgpushxto0(nilv::convert(point31(h[0], h[1], 0), nilv::nmSym, nilv::model_used)); }
  static transmatrix hpush(hyperpoint h) { h[1] = 0; h[2] = 0; return flatpush(h); }
  static transmatrix vpush(hyperpoint h) { h[0] = 0; h[2] = 0; return flatpush(h); }

  static hyperpoint spin_around(hyperpoint h, hyperpoint start, hyperpoint ctr, ld dir) {
    auto h1 = h - ctr;

    auto d = hypot_d(2, h1);
    ld r = 2;
    h1 = h1 * (r / d);
    ld phi = atan2(h1[1], h1[0]) + 90._deg;
    ld phis = atan2((start-ctr)[1], (start-ctr)[0]) + 90._deg;
    cyclefix(phi, phis);
    h1 += ctr;
    auto z = [&] (ld a) { return point31(r*sin(a), -r*cos(a), (r * r / 2) * (a-sin(a)*cos(a))); };

    if(0) {
      // not smooth enough ....
      transmatrix q = gpushxto0(z(phis)) * rgpushxto0(z(phi));
      hyperpoint arc = rgpushxto0(start) * q * flatpush(h-h1) * C0;
      return arc;
      }

    hyperpoint h2 = h; if(start[0] == ctr[0]) h2[1] = start[1]; else h2[0] = start[0];
    hyperpoint pre = rgpushxto0(start) * flatpush(h2-start) * flatpush(h-h2) * C0;

    hyperpoint last = rgpushxto0(start) * gpushxto0(z(phis)) * rgpushxto0(z(phis + dir * 90._deg)) * C0;
    hyperpoint h3 = h; if(start[0] != ctr[0]) h3[1] = last[1]; else h3[0] = last[0];
    hyperpoint post = rgpushxto0(last) * flatpush(h3-last) * flatpush(h-h3) * C0;

    ld p = (1+sin((phi-phis)*2 - 90._deg)) / 2.;

    pre[2] = pre[2] + (post[2] - pre[2]) * p;

    // println(hlog, "START = ", start, " LAST = ", last, " h = ", h, " h2 = ", h2, " h3 = ", h3, " p = ", p, " pre = ", pre);
    // exit(1);

    return pre;
    // flatpush(h1 - start) * flatpush(h - h1) * C0;
    }

  static hyperpoint rel(int x, int y) { return point30(x, y, 0); };

  surface_fun& at(hyperpoint h) {
    int ax = int(floor(h[0] / 4));
    int ay = int(floor(h[1] / 4));
    return blocks[{ax, ay}];
    };

  void right_block() {
    auto c = cur;
    println(hlog, "RIGHT at ", c);
    auto f = [c] (hyperpoint h) { return rgpushxto0(c) * hpush(h-c) * vpush(h-c) * C0; };
    at(c+rel(2, 0)) = [f] (hyperpoint h) { return f(h)[2]; };
    cur = f(c+rel(4, 0));
    }

  void left_block() {
    auto c = cur;
    println(hlog, "LEFT at ", c);
    auto f = [c] (hyperpoint h) { return rgpushxto0(c) * hpush(h-c) * vpush(h-c) * C0; };
    at(c+rel(-2, 0)) = [f] (hyperpoint h) { return f(h)[2]; };
    cur = f(c+rel(-4, 0));
    }

  void up_block() {
    auto c = cur;
    println(hlog, "UP at ", c);
    auto f = [c] (hyperpoint h) { return rgpushxto0(c) * vpush(h-c) * hpush(h-c) * C0; };
    at(c+rel(0, 2)) = [f] (hyperpoint h) { return f(h)[2]; };
    cur = f(c+rel(0, 4));
    }

  void down_block() {
    auto c = cur;
    println(hlog, "DOWN at ", c);
    auto f = [c] (hyperpoint h) { return rgpushxto0(c) * vpush(h-c) * hpush(h-c) * C0; };
    at(c+rel(0, -2)) = [f] (hyperpoint h) { return f(h)[2]; };
    cur = f(c+rel(0, -4));
    }

  /* counterclockwise */
  void turn_up_block() {
    auto c = cur;
    println(hlog, "TURN UP at ", c);
    auto f = [c] (hyperpoint h) { return (spin_around(h, c, c+rel(0, 2), 1)); };
    at(c+rel(2, 0)) = [f] (hyperpoint h) { return f(h)[2]; };
    cur = f(c+rel(2, 2));
    };
  void turn_left_block() {
    auto c = cur;
    auto f = [c] (hyperpoint h) { return (spin_around(h, c, c+rel(-2, 0), 1)); };
    at(c+rel(0, 2)) = [f] (hyperpoint h) { return f(h)[2]; };
    cur = f(c+rel(-2, 2));
    };
  void turn_down_block () {
    auto c = cur;
    auto f = [c] (hyperpoint h) { return (spin_around(h, c, c+rel(0, -2), 1)); };
    at(c+rel(-2, 0)) = [f] (hyperpoint h) { return f(h)[2]; };
    cur = f(c+rel(-2, -2));
    };
  void turn_right_block() {
    auto c = cur;
    auto f = [c] (hyperpoint h) { return (spin_around(h, c, c+rel(2, 0), 1)); };
    at(c+rel(0, -2)) = [f] (hyperpoint h) { return f(h)[2]; };
    cur = f(c+rel(2, -2));
    };

  /* clockwise */
  void turn_up_block2() {
    auto c = cur;
    println(hlog, "TURN UP at ", c);
    auto f = [c] (hyperpoint h) { return (spin_around(h, c, c+rel(0, 2), -1)); };
    at(c+rel(-2, 0)) = [f] (hyperpoint h) { return f(h)[2]; };
    cur = f(c+rel(-2, 2));
    };
  void turn_left_block2() {
    auto c = cur;
    auto f = [c] (hyperpoint h) { return (spin_around(h, c, c+rel(-2, 0), -1)); };
    at(c+rel(0, -2)) = [f] (hyperpoint h) { return f(h)[2]; };
    cur = f(c+rel(-2, -2));
    };
  void turn_down_block2() {
    auto c = cur;
    auto f = [c] (hyperpoint h) { return (spin_around(h, c, c+rel(0, -2), -1)); };
    at(c+rel(2, 0)) = [f] (hyperpoint h) { return f(h)[2]; };
    cur = f(c+rel(2, -2));
    };
  void turn_right_block2() {
    auto c = cur;
    auto f = [c] (hyperpoint h) { return (spin_around(h, c, c+rel(2, 0), -1)); };
    at(c+rel(0, 2)) = [f] (hyperpoint h) { return f(h)[2]; };
    cur = f(c+rel(2, 2));
    };

  ld get(hyperpoint h) {
    int ax = int(floor(h[0] / 4));
    int ay = int(floor(h[1] / 4));
    if(blocks.count({ax, ay})) return blocks[{ax, ay}] (h);
    return 0;
    }

  complex_surface(hyperpoint h) : cur(h) {}
  };

complex_surface *spiral, *hilbert;

ld spiral_level(hyperpoint h) {
  if(!spiral) {
    spiral = new complex_surface(point31(-4, 2, 0));
    spiral->right_block();
    spiral->right_block();
    spiral->right_block();
    spiral->right_block();
    spiral->turn_up_block();
    spiral->up_block();
    spiral->up_block();
    spiral->turn_left_block();
    spiral->left_block();
    spiral->left_block();
    spiral->turn_down_block();
    spiral->down_block();
    spiral->turn_right_block();
    spiral->right_block();
    spiral->turn_up_block();
    spiral->turn_left_block();
    spiral->left_block();
    }
  return spiral->get(h);
  }

ld hilbert_level(hyperpoint h) {
  if(!hilbert) {
    hilbert = new complex_surface(point31(2, 0, 0));
    hilbert->up_block();
    hilbert->turn_right_block2();
    hilbert->turn_down_block2();
    hilbert->turn_right_block();
    hilbert->right_block();
    hilbert->turn_up_block();
    hilbert->turn_left_block();
    hilbert->turn_up_block2();
    hilbert->turn_right_block2();
    hilbert->turn_up_block();
    hilbert->turn_left_block();
    hilbert->left_block();
    hilbert->turn_down_block();
    hilbert->turn_left_block2();
    hilbert->turn_up_block2();
    hilbert->up_block();
    }
  return hilbert->get(h);
  }

level spirallev(
  "Square Spiral", 's', 0, 1300, 
  "The projection of this track is shaped like a square spiral.",
  0.5*dft_block, 16.5*dft_block, 16.5*dft_block, 0.5*dft_block,

  {
  "!!!!!!!!!!!!!!!!",
  "rgggggggggggggr!",
  "g+-----------+g!",
  "g|gGgggggggGg|g!",
  "g|G!!!!!!!!!G|g!",
  "g|g!rgggggr!g|g!",
  "g|g!g*---+g!g|g!",
  "g|g!rgggg|g!g|g!",
  "g|G!!!!!x|g!g|g!",
  "g|gGgggGg|g!g|g!",
  "g+-------+g!g|g!",
  "rgggggggggr!g|g!",
  "!!!!!!!!!!!!G|g!",
  "fffggggggggGg|g!",
  "-------------+g!",
  "ggggggggggggggr!"
  },

  1, 15.4, {}, spiral_level,
  {
    // the solver result is 55.239
    goal{0xFFD500, "Collect the triangle in below 60 seconds", basic_check(60, 999), "SPIRAL2", "", award_stars(100, 56, 60)},
    goal{0xFF4040, "Collect the triangle in below 70 seconds", basic_check(70, 999), "SPIRAL1", "", award_stars(100, 56, 60)},
    goal{0x40c040, "Collect the triangle without reversing time", basic_check(999, 0), "", "Square Spiral", award_stars(100, 60, 999)}
  }
  );

level hilbertlev(
  "Hilbert's Curve", 's', 0, 1400, 
  "The projection of this track is shaped like the Hilbert curve.",
  0.5*dft_block, 16.5*dft_block, 16.5*dft_block, 0.5*dft_block,

  {
  "!!!!!!!!!!!!!!!!",
  "ggg!rgggGGGgggr!",
  "g*g!gf-------fg!",
  "g|g!g|ggGGGgg|g!",
  "g|g!g|g!!!!!g|g!",
  "g|gxg|g!rgggg|g!",
  "gf---fg!gf---fg!",
  "rgggggr!g|ggggr!",
  "!!!!!!!!g|o!!!!!",
  "rgggggr!g|ggggr!",
  "gf---fg!gf---fg!",
  "g|ggg|g!rgggg|g!",
  "g|g!x|g!!!!!g|g!",
  "g|g!g|ggGGGgg|g!",
  "g|g!gf-------fg!",
  "g|g!rgggGGGgggr!"
  },

  2.4, 15.4, {}, hilbert_level,
  {
    // the solver result is 50.94
    goal{0xFFD500, "Collect the triangle in below 55 seconds", basic_check(55, 999), "HILBERT", "", award_stars(100, 50, 55)},
    goal{0xFF4040, "Collect the triangle in below 60 seconds", basic_check(60, 999), "", "Hilbert's Curve", award_stars(100, 50, 60)},
  }
  );

level cycloid_slalom(
  "Cycloid slalom", 'c', nrlSwapTriangleOrder, 1500,
  "The main street is a brachistochrone. If you were not moving north/south, "
  "it would be the fastest path to get to the goal. Is it still the case "
  "in these physics? Definitely not if you have to collect on the way!",
  -0.5*dft_block, 2.5*dft_block, 63.5*dft_block, -2.5*dft_block,
  {
  "gggggggG*GggggrgggggG*GgggggrggggggG*GgggggrggggG*Rgggggrggggggo",
  "gggggggGGGggggggggggGGGggggggggggggGGGggggggggggGGGggggggggggggo",
  "---------------------------------------------------------------*",
  "gggggggggggggGGGgggggggggggGGGggggggggggggGGGggggggggggGGGgggggo",
  "ggggggggrggggG*GgggggrgggggG*GggggggrgggggG*GggggrgggggG*Ggggggo"
  },
  0, 2, {},
  brachistochrone,
  {
    goal{0xFFFFC0, "Collect all triangles in below 1:25, reversing time at most 3 times", basic_check(85, 3), "CYCLOID1", "", award_stars(100, 60, 85)},
    goal{0xFFD500, "Collect all triangles in below 1:10, reversing time at most 3 times", basic_check(70, 3), "CYCLOID2", "", award_stars(100, 60, 70)},
    goal{0x40c040, "Collect the triangle without reversing time", basic_check(999, 0), "", "Cycloid slalom", award_stars(100, 60, 999)}
    }
  );

level multifloor(
  "Multi-floor", 'm', 0, 1600,
  "There are triangles on the floors below. I mean, DIRECTLY below.\n\nHopefully, you can abuse how straight lines work in this geometry to reach them!"
  ,

  -1.5*dft_block, 1.5*dft_block, 1.5*dft_block, -1.5*dft_block,
  {
  "ggg",
  "ggg",
  "ggg"},
  0, 1, {},
  rot_plane,
  {
    goal{0x40FF40, "Collect all the triangles in below 300 seconds, reversing time at most 3 times", basic_check(300, 3), "MULTIFLOOR", "", award_stars(100, 100, 300)},
    goal{0xFFD500, "Collect all the triangles in below 150 seconds, reversing time at most once", basic_check(150, 1), "MULTIFLOOR2", "", award_stars(100, 100, 150)}
    }
  );

level skijump (
  "Ski Jumping", 'r', nrlJumping, 1700,
  "Try to jump far away!",
  -0.5*dft_block, 2.5*dft_block, 15.5*dft_block, -2.5*dft_block,
  {
  "!!!!!!!!!~~~~~~~",
  "!!!!!!!!!~~~~~~~",
  "-----!!!!~~~~~*~",
  "!!!!!!!!!~~~~~~~",
  "!!!!!!!!!~~~~~~~",
  },
  0, 2, {},
  [] (hyperpoint h) {
    if(h[0] > 4.6) return h[0] * h[1] / 2;
    return h[0] * h[1] / 2 + 4 * (4.5 - h[0]) + 1 / (5 - h[0]);
    },
  {
    goal{0x40c040, "Jump as far as you can", ski_check, "", "Ski Jumping", award_stars_distance}
    }
  );

ld f_bumpy(hyperpoint h) {
  ld a = h[0] * 2 + h[1];
  ld b = h[1] * 2 - h[0];
  return rot_plane(h) + h[0] * h[1] / 2 + sin(a*1.5) * sin(b*1.5) / 3;
  }

level bumpy(
  "Bumpy Ride", 'b', 0, 1800,
  "The main street is horizontal, as well as the lines orthogonal to it.",
  0*dft_block, 2.5*dft_block, 64*dft_block, -2.5*dft_block,
  {
  "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~o",
  "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~o",
  "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*",
  "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~o",
  "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~o",
  },
  0, 4, {},
  f_bumpy,
  {
    goal{0xFFFFC0, "Collect the triangle in below 1:25, reversing time at most 3 times", basic_check(85, 3), "BUMPY", "Bumpy Ride", award_stars(100, 60, 85)},
    }
  );



vector<level*> all_levels = {
  &rotplane, &longtrack, &geodesical, &geodesical4, &heisenberg0, &rotwell, &labyrinth, &obstacle, &spirallev, &hilbertlev, &cycloid_slalom,
  &multifloor, &skijump, &bumpy
  };
  
}
