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
  "Collect all the triangles!\n\n"
  "All the lines going through the center are horizontal.\n"
  "However, this is Nil geometry. The other lines are NOT horizontal! Clockwise ones slope upwards, and counterclockwise ones slop edownwards.\n"
  "Your unicycle is powered only by the gravity. Use that to your advantage!"
  ,

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
  "Geodesics are the lines that are (locally) shortest. In the default settings, "
  "the space in Nil Rider is rendered according to the Fermat's principle, that is, "
  "light rays are assumed to be geodesics.\n\n"
  "Geodesics in Nil are horizontal, vertical, and helical. "
  "In this level, all the roads are (fragments of) helical geodesics.",
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
  "Can you escape this well?\n\n"
  "The sculpture in the center is built around eight helical geodesics which start in a point on the floor, and all cross in a point in the sky. Try to find that point and "
  "look below!\n\n"
  "Note: you can move the camera freely (using the arrow keys and PageUp/Down/Home/End) while the game is paused."
  ,
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
  "You will have to go clockwise this time!\n\n"
  "The squares of this level have half of their usual length.",
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

level obstacle(
  "Obstacle Course", 'o', 0,
  "The main street is horizontal, as well as the lines orthogonal to it.",
  0*dft_block, 2.5*dft_block, 64*dft_block, -5.5*dft_block,
  {
  "ggggggGrggGrgggggggggggggggggggggGrxgggggggggGrggggggggGrggggggo",
  "ggggggGrggGrgggGrgggggGrgggggggggGrgggggggggggggGrgggggGrggggggo",
  "-----------r----r------r----r-----r--r---------r---------------*",
  "ggggggGrgggggggGrgggggGrggggggggggggGrggggggGrgggggggggGrggggggo",
  "ggggggGrgggggggGrgggggggggggggggggggGrgggggggggGrggggggGrggggggo",
  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",
  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",
  "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!",
  },
  0, 4,
  long_x,
  {
    goal{0xFFFFC0, "Collect the triangle in below 1:25, reversing time at most 3 times", basic_check(85, 3)},
    goal{0xFFD500, "Collect the triangle in below 1:10, reversing time at most 3 times", basic_check(70, 3)},
    }
  );

level *curlev = &rotplane;

struct complex_surface {
  hyperpoint cur;
  map<pair<int, int>, surface_fun> blocks;

  static transmatrix flatpush(hyperpoint h) { return rgpushxto0(point31(h[0], h[1], rot_plane(h))); }
  static transmatrix hpush(hyperpoint h) { h[1] = 0; h[2] = 0; return flatpush(h); }
  static transmatrix vpush(hyperpoint h) { h[0] = 0; h[2] = 0; return flatpush(h); }

  static hyperpoint spin_around(hyperpoint h, hyperpoint start, hyperpoint ctr, ld dir) {
    auto h1 = h - ctr;

    auto d = hypot_d(2, h1);
    ld r = 2;
    h1 = h1 * (r / d);
    ld phi = atan2(h1[1], h1[0]) + 90*degree;
    ld phis = atan2((start-ctr)[1], (start-ctr)[0]) + 90 * degree;
    if(phi < phis-M_PI) phi += 2 * M_PI;
    if(phi > phis+M_PI) phi -= 2 * M_PI;
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

    hyperpoint last = rgpushxto0(start) * gpushxto0(z(phis)) * rgpushxto0(z(phis + dir * 90*degree)) * C0;
    hyperpoint h3 = h; if(start[0] != ctr[0]) h3[1] = last[1]; else h3[0] = last[0];
    hyperpoint post = rgpushxto0(last) * flatpush(h3-last) * flatpush(h-h3) * C0;

    ld p = (1+sin((phi-phis)*2 - 90*degree)) / 2.;

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
  "Square Spiral", 's', 0,
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

  1, 15.4, spiral_level,
  {
    // the solver result is 55.239
    goal{0xFFD500, "Collect the triangle in below 60 seconds", basic_check(60, 999)},
    goal{0xFF4040, "Collect the triangle in below 70 seconds", basic_check(70, 999)},
  }
  );

level hilbertlev(
  "Hilbert's Curve", 's', 0,
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

  2.4, 15.4, hilbert_level,
  {
    // the solver result is 50.94
    goal{0xFFD500, "Collect the triangle in below 55 seconds", basic_check(55, 999)},
    goal{0xFF4040, "Collect the triangle in below 60 seconds", basic_check(60, 999)},
  }
  );

vector<level*> all_levels = {
  &rotplane, &longtrack, &geodesical, &geodesical4, &heisenberg0, &rotwell, &labyrinth, &obstacle, &spirallev, &hilbertlev
  };
  
}
