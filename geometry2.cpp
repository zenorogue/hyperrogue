// Hyperbolic Rogue
// advanced geometry

// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

namespace hr {

transmatrix &ggmatrix(cell *c);

void fixelliptic(transmatrix& at) {
  if(elliptic && at[2][2] < 0) {
    for(int i=0; i<3; i++) for(int j=0; j<3; j++)
      at[i][j] = -at[i][j];
    }
  }

void fixelliptic(hyperpoint& h) {
  if(elliptic && h[2] < 0)
    for(int i=0; i<3; i++) h[i] = -h[i];
  }

transmatrix master_relative(cell *c, bool get_inverse) {
  if(IRREGULAR) {
    int id = irr::cellindex[c];
    ld alpha = 2 * M_PI / S7 * irr::periodmap[c->master].base.spin;
    return get_inverse ? irr::cells[id].rpusher * spin(-alpha-master_to_c7_angle()): spin(alpha + master_to_c7_angle()) * irr::cells[id].pusher;
    }
  else if(GOLDBERG) {
    if(c == c->master->c7) {
      return spin((get_inverse?-1:1) * master_to_c7_angle());
      }
    else {
      auto li = gp::get_local_info(c);
      transmatrix T = spin(master_to_c7_angle()) * gp::Tf[li.last_dir][li.relative.first&31][li.relative.second&31][gp::fixg6(li.total_dir)];
      if(get_inverse) T = inverse(T);
      return T;
      }
    }
  else if(BITRUNCATED && !euclid) {
    for(int d=0; d<S7; d++) if(c->master->c7->move(d) == c)
      return (get_inverse?invhexmove:hexmove)[d];
    return Id;
    }
  else
    return pispin * Id;
  }

transmatrix calc_relative_matrix(cell *c2, cell *c1, int direction_hint) {
  return calc_relative_matrix(c2, c1, ddspin(c1, direction_hint) * xpush0(1e-2));
  }

// target, source, direction from source to target

namespace gp { extern gp::local_info draw_li; }

transmatrix calc_relative_matrix(cell *c2, cell *c1, const hyperpoint& point_hint) {

  if(sphere_narcm) {
    if(!gmatrix0.count(c2) || !gmatrix0.count(c1)) {
      printf("building gmatrix0 (size=%d)\n", isize(gmatrix0));
      auto bak = gp::draw_li;
      swap(gmatrix, gmatrix0);
      just_gmatrix = true;
      drawrec(viewctr, hsOrigin, Id);
      just_gmatrix = false;
      swap(gmatrix, gmatrix0);
      gp::draw_li = bak;
      }
    if(gmatrix0.count(c2) && gmatrix0.count(c1)) {
      transmatrix T = inverse(gmatrix0[c1]) * gmatrix0[c2];
      if(elliptic && T[2][2] < 0)
        T = centralsym * T;
      return T;
      }
    else {
      printf("error: gmatrix0 not known\n");
      return Id;
      }
    }
  
  if(binarytiling) return binary::relative_matrix(c2->master, c1->master);
  if(archimedean) return arcm::relative_matrix(c2->master, c1->master);
  
  if(torus) {
    transmatrix t = Id;
    if(whateveri) printf("[%p,%d] ", c2, celldistance(c2, c1));
    int mirrors = 0;
    approach:
    int d = celldistance(c2, c1);
    forCellIdEx(c3, i, c2) {
      if(celldistance(c3, c1) < d) {
        if(whateveri) printf(" %d [%p,%d]", i, c3, celldistance(c3, c1));
        if(c2->type < 8)
          t = eumovedir(i+(euclid6?3:2)) * t;
        else if(i&1)
          t = eumovedir(2+i/2) * eumovedir(2+(i+1)/2) * t;
        else
          t = eumovedir(2+i/2) * t;
        if(c2->c.mirror(i)) mirrors++;
        c2 = c3;
        goto approach;
        }
      }
    if(d != 0) printf("ERROR not reached\n");
    if(mirrors&1) t = Mirror * t * Mirror;
    if(whateveri) printf(" => %p\n", c1);
    return t;
    }
  
  if(euclid) return inverse(gmatrix0[c1]) * gmatrix0[c2];

  heptagon *h1 = c1->master;
  transmatrix gm = master_relative(c1, true);
  heptagon *h2 = c2->master;
  transmatrix where = master_relative(c2);

  // always add to last!
//bool hsol = false;
//transmatrix sol;

  while(h1 != h2) {
    if(quotient & qSMALL) {
      transmatrix T;
      ld bestdist = 1e9;
      for(int d=0; d<S7; d++) if(h2->move(d)) {
        int sp = h2->c.spin(d);
        transmatrix S = heptmove[sp] * spin(2*M_PI*d/S7);
        if(h2->c.mirror(d)) S = heptmove[sp] * Mirror * spin(2*M_PI*d/S7);
        if(h2->move(d) == h1) {
          transmatrix T1 = gm * S * where;
          auto curdist = hdist(tC0(T1), point_hint);
          if(curdist < bestdist) T = T1, bestdist = curdist;
          }
        if(geometry != gMinimal) for(int e=0; e<S7; e++) if(h2->move(d)->move(e) == h1) {
          int sp2 = h2->move(d)->c.spin(e);
          transmatrix T1 = gm * heptmove[sp2] * spin(2*M_PI*e/S7) * S * where;
          auto curdist = hdist(tC0(T1), point_hint);
          if(curdist < bestdist) T = T1, bestdist = curdist;
          }
        }
      if(bestdist < 1e8) return T;
      }
    for(int d=0; d<S7; d++) if(h2->move(d) == h1) {
      int sp = h2->c.spin(d);
      return gm * heptmove[sp] * spin(2*M_PI*d/S7) * where;
      }
    if(geometry == gFieldQuotient) {
      int bestdist = 1000, bestd = 0;
      for(int d=0; d<S7; d++) {
        int dist = celldistance(h2->move(d)->c7, c1);
        if(dist < bestdist) bestdist = dist, bestd = d;
        }
      int sp = h2->c.spin(bestd);
      where = heptmove[sp] * spin(2*M_PI*bestd/S7) * where;
      h2 = h2->move(bestd);
      }
    else if(h1->distance < h2->distance) {
      int sp = h2->c.spin(0);
      h2 = h2->move(0);
      where = heptmove[sp] * where;
      }
    else {
      int sp = h1->c.spin(0);
      h1 = h1->move(0);
      gm = gm * invheptmove[sp];
      }
    }
/*if(hsol) {
    transmatrix sol2 = gm * where;
    for(int i=0; i<3; i++) for(int j=0; j<3; j++)
      if(fabs(sol2[i][j]-sol[i][j] > 1e-3)) {
        printf("ERROR\n");
        display(sol);
        display(sol2);
        exit(1);
        }
    } */
  return gm * where;
  }

transmatrix &ggmatrix(cell *c) {
  transmatrix& t = gmatrix[c];
  if(t[2][2] == 0) {
    if(torus && centerover.at) 
      t = calc_relative_matrix(c, centerover.at, C0);
    else if(euclid) {
      if(!centerover.at) centerover = cwt;
      t = View * eumove(cell_to_vec(c) - cellwalker_to_vec(centerover));
      }
    else 
      t = actualV(viewctr, cview()) * calc_relative_matrix(c, viewctr.at->c7, C0);
    }
  return t;
  }

transmatrix calc_relative_matrix_help(cell *c, heptagon *h1) {
  transmatrix gm = Id;
  heptagon *h2 = c->master;
  transmatrix where = Id;
  if(GOLDBERG && c != c->master->c7) {
    auto li = gp::get_local_info(c);
    where = gp::Tf[li.last_dir][li.relative.first&31][li.relative.second&31][fix6(li.total_dir)];
    }
  else if(BITRUNCATED) for(int d=0; d<S7; d++) if(h2->c7->move(d) == c)
    where = hexmove[d];
  // always add to last!
  while(h1 != h2) {
    for(int d=0; d<S7; d++) if(h1->move(d) == h2) printf("(adj) ");
    if(h1->distance < h2->distance) {
      int sp = h2->c.spin(0);
      printf("A%d ", sp);
      h2 = h2->move(0);
      where = heptmove[sp] * where;
      }
    else {
      int sp = h1->c.spin(0);
      printf("B%d ", sp);
      h1 = h1->move(0);
      gm = gm * invheptmove[sp];
      }
    }
  printf("OK\n");
  display(gm * where);
  return gm * where;
  }

template<class T, class U> 
void virtualRebase(cell*& base, T& at, bool tohex, const U& check) {
  if(euclid || sphere) {
    again:
    if(torus) for(int i=0; i<6; i++) {
      auto newat = eumovedir(3+i) * at;
      if(hdist0(check(newat)) < hdist0(check(at))) {
        at = newat;
        base = createMov(base, i);
        goto again;
        }
      }
    else forCellCM(c2, base) {
      auto newat = inverse(ggmatrix(c2)) * ggmatrix(base) * at;
      if(hypot(check(newat)[0], check(newat)[1])
        < hypot(check(at)[0], check(at)[1])) {
        at = newat;
        base = c2;
        goto again;
        }
      }
    fixelliptic(at);
    return;
    }

  at = master_relative(base) * at;
  base = base->master->c7;
    
  while(true) {
  
    double currz = check(at)[2];
    
    heptagon *h = base->master;
    
    cell *newbase = NULL;
    
    transmatrix bestV;
    
    if(!binarytiling) for(int d=0; d<S7; d++) {
      heptspin hs(h, d, false);
      heptspin hs2 = hs + wstep;
      transmatrix V2 = spin(-hs2.spin*2*M_PI/S7) * invheptmove[d];
      double newz = check(V2 * at) [2];
      if(newz < currz) {
        currz = newz;
        bestV = V2;
        newbase = hs2.at->c7;
        }
      }

    if(newbase) {
      base = newbase;
      at = bestV * at;
      }
    else {
      if(tohex && BITRUNCATED) for(int d=0; d<S7; d++) {
        cell *c = createMov(base, d);
        transmatrix V2 = spin(-base->c.spin(d)*2*M_PI/S6) * invhexmove[d];
        double newz = check(V2 *at) [2];
        if(newz < currz) {
          currz = newz;
          bestV = V2;
          newbase = c;
          }
        }
      if(newbase) {
        base = newbase;
        at = bestV * at;
        }
      else at = master_relative(base, true) * at;
      if(binarytiling || (tohex && (GOLDBERG || IRREGULAR))) {
        while(true) {
          newbase = NULL;
          forCellCM(c2, base) {
            transmatrix V2 = calc_relative_matrix(base, c2, C0);
            double newz = check(V2 * at) [2];
            if(newz < currz) {
              currz = newz;
              bestV = V2;
              newbase = c2;
              }
            }
          if(!newbase) break;
          base = newbase;
          at = bestV * at;
          }
        }
      break;
      }
    }

  }

void virtualRebase(cell*& base, transmatrix& at, bool tohex) {
  virtualRebase(base, at, tohex, tC0);
  }

void virtualRebase(cell*& base, hyperpoint& h, bool tohex) {
  virtualRebase(base, h, tohex, [] (const hyperpoint& h) { return h; });
  }

// works only in geometries similar to the standard one, and only on heptagons
void virtualRebaseSimple(heptagon*& base, transmatrix& at) {

  while(true) {
  
    double currz = at[2][2];
    
    heptagon *h = base;
    
    heptagon *newbase = NULL;
    
    transmatrix bestV;
    
    for(int d=0; d<S7; d++) {
      heptspin hs(h, d, false);
      heptspin hs2 = hs + wstep;
      transmatrix V2 = spin(-hs2.spin*2*M_PI/S7) * invheptmove[d] * at;
      double newz = V2[2][2];
      if(newz < currz) {
        currz = newz;
        bestV = V2;
        newbase = hs2.at;
        }
      }

    if(newbase) {
      base = newbase;
      at = bestV;
      continue;
      }

    return;
    }
  }

double cellgfxdist(cell *c, int i) {
  if(NONSTDVAR || archimedean) return hdist0(tC0(calc_relative_matrix(c->move(i), c, i)));
  return !BITRUNCATED ? tessf : (c->type == 6 && (i&1)) ? hexhexdist : crossf;
  }

transmatrix cellrelmatrix(cell *c, int i) {
  if(NONSTDVAR || archimedean) return calc_relative_matrix(c->move(i), c, i);
  double d = cellgfxdist(c, i);
  return ddspin(c, i) * xpush(d) * iddspin(c->move(i), c->c.spin(i), euclid ? 0 : M_PI);
  }

double randd() { return (rand() + .5) / (RAND_MAX + 1.); }

hyperpoint randomPointIn(int t) {
  if(NONSTDVAR || archimedean) {
    // Let these geometries be less confusing.
    // Also easier to implement ;)
    return xspinpush0(2 * M_PI * randd(), asinh(randd() / 20));
    }
  while(true) {
    hyperpoint h = xspinpush0(2*M_PI*(randd()-.5)/t, asinh(randd()));
    double d =
      PURE ? tessf : t == 6 ? hexhexdist : crossf;
    if(hdist0(h) < hdist0(xpush(-d) * h))
      return spin(2*M_PI/t * (rand() % t)) * h;
    }
  }

hyperpoint get_horopoint(ld y, ld x) {
  return xpush(-y) * binary::parabolic(x) * C0;
  }

hyperpoint get_corner_position(cell *c, int cid, ld cf) {
  if(GOLDBERG) return gp::get_corner_position(c, cid, cf);
  if(IRREGULAR) {
    auto& vs = irr::cells[irr::cellindex[c]];
    return mid_at_actual(vs.vertices[cid], 3/cf);
    }
  if(binarytiling) {
    ld yx = log(2) / 2;
    ld yy = yx;
    ld xx = 1 / sqrt(2)/2;
    hyperpoint vertices[7];
    vertices[0] = get_horopoint(-yy, xx);
    vertices[1] = get_horopoint(yy, 2*xx);
    vertices[2] = get_horopoint(yy, xx);
    vertices[3] = get_horopoint(yy, -xx);
    vertices[4] = get_horopoint(yy, -2*xx);
    vertices[5] = get_horopoint(-yy, -xx);
    vertices[6] = get_horopoint(-yy, 0);
    return mid_at_actual(vertices[cid], 3/cf);
    }
  if(archimedean) {
    auto &ac = arcm::current;
    if(PURE) {
      if(arcm::id_of(c->master) >= ac.N*2) return C0;
      auto& t = ac.get_triangle(c->master, cid-1);
      return xspinpush0(-t.first, t.second * 3 / cf * (ac.real_faces == 0 ? 0.999 : 1));
      }
    if(BITRUNCATED) {
      auto& t0 = ac.get_triangle(c->master, cid-1);
      auto& t1 = ac.get_triangle(c->master, cid);
      hyperpoint h0 = xspinpush0(-t0.first, t0.second * 3 / cf * (ac.real_faces == 0 ? 0.999 : 1));
      hyperpoint h1 = xspinpush0(-t1.first, t1.second * 3 / cf * (ac.real_faces == 0 ? 0.999 : 1));
      return mid3(C0, h0, h1);
      }
    if(DUAL) {
      auto& t0 = ac.get_triangle(c->master, 2*cid-1);
      return xspinpush0(-t0.first, t0.second * 3 / cf * (ac.real_faces == 0 ? 0.999 : 1));
      }
    }
  if(PURE) {
    return ddspin(c,cid,M_PI/S7) * xpush0(hcrossf * 3 / cf);
    }
  if(BITRUNCATED) {
    if(!ishept(c))
      return ddspin(c,cid,M_PI/S6) * xpush0(hexvdist * 3 / cf);
    else
      return ddspin(c,cid,M_PI/S7) * xpush0(rhexf * 3 / cf);
    }
  return C0;
  }

hyperpoint nearcorner(cell *c, int i) {
  if(GOLDBERG) {
    cellwalker cw(c, i);
    cw += wstep;
    transmatrix cwm = calc_relative_matrix(cw.at, c, i);
    if(elliptic && cwm[2][2] < 0) cwm = centralsym * cwm;
    return cwm * C0;
    }
  if(IRREGULAR) {
    auto& vs = irr::cells[irr::cellindex[c]];
    hyperpoint nc = vs.jpoints[vs.neid[i]];
    return mid_at(C0, nc, .94);
    }
  if(archimedean) {
    if(PURE) { 
      auto &ac = arcm::current;
      auto& t = ac.get_triangle(c->master, i-1);
      int id = arcm::id_of(c->master);
      int id1 = ac.get_adj(ac.get_adj(c->master, i-1), -2).first;
      return xspinpush0(-t.first - M_PI / c->type, ac.inradius[id/2] + ac.inradius[id1/2] + (ac.real_faces == 0 ? 2 * M_PI / (ac.N == 2 ? 2.1 : ac.N) : 0));
      }
    if(BITRUNCATED) {
      auto &ac = arcm::current;
      auto& t = ac.get_triangle(c->master, i);
      return xspinpush0(-t.first, t.second);
      }
    if(DUAL) {
      auto &ac = arcm::current;
      auto& t = ac.get_triangle(c->master, i * 2);
      return xspinpush0(-t.first, t.second);
      }
    }
  if(binarytiling) {
    ld yx = log(2) / 2;
    ld yy = yx;
    // ld xx = 1 / sqrt(2)/2;
    hyperpoint neis[7];
    neis[0] = get_horopoint(0, 1);
    neis[1] = get_horopoint(yy*2, 1);
    neis[2] = get_horopoint(yy*2, 0);
    neis[3] = get_horopoint(yy*2, -1);
    neis[4] = get_horopoint(0, -1);
    if(c->type == 7)
      neis[5] = get_horopoint(-yy*2, -.5),
      neis[6] = get_horopoint(-yy*2, +.5);
    else
      neis[5] = get_horopoint(-yy*2, 0);
    return neis[i];
    }
  double d = cellgfxdist(c, i);
  return ddspin(c, i) * xpush0(d);
  }

hyperpoint farcorner(cell *c, int i, int which) {
  if(GOLDBERG) {
    cellwalker cw(c, i);
    int hint = cw.spin;
    cw += wstep;
    transmatrix cwm = calc_relative_matrix(cw.at, c, hint);
    if(elliptic && cwm[2][2] < 0) cwm = centralsym * cwm;
    // hyperpoint nfar = cwm*C0;
    auto li1 = gp::get_local_info(cw.at);
    if(which == 0)
      return cwm * get_corner_position(li1, (cw+2).spin);
    if(which == 1)
      return cwm * get_corner_position(li1, (cw-1).spin);          
    }
  if(IRREGULAR) {
    auto& vs = irr::cells[irr::cellindex[c]];
    int neid = vs.neid[i];
    int spin = vs.spin[i];
    auto &vs2 = irr::cells[neid];
    int cor2 = isize(vs2.vertices);
    transmatrix rel = vs.rpusher * vs.relmatrices[vs2.owner] * vs2.pusher;

    if(which == 0) return rel * vs2.vertices[(spin+2)%cor2];
    if(which == 1) return rel * vs2.vertices[(spin+cor2-1)%cor2];
    }
  if(binarytiling)
    return nearcorner(c, (i+which) % c->type); // lazy
  if(archimedean) {
    if(PURE) {
      auto &ac = arcm::current;
      auto& t = ac.get_triangle(c->master, i-1);
      int id = arcm::id_of(c->master);
      auto id1 = ac.get_adj(ac.get_adj(c->master, i-1), -2).first;
      int n1 = isize(ac.adjacent[id1]);
      return spin(-t.first - M_PI / c->type) * xpush(ac.inradius[id/2] + ac.inradius[id1/2]) * xspinpush0(M_PI + M_PI/n1*(which?3:-3), ac.circumradius[id1/2]);
      }
    if(BITRUNCATED || DUAL) {
      int mul = DUALMUL;
      auto &ac = arcm::current;
      auto adj = ac.get_adj(c->master, i * mul);
      heptagon h; cell cx; cx.master = &h;
      arcm::id_of(&h) = adj.first;
      arcm::parent_index_of(&h) = adj.second;

      auto& t1 = arcm::current.get_triangle(c->master, i);
    
      auto& t2 = arcm::current.get_triangle(adj);
      
      return spin(-t1.first) * xpush(t1.second) * spin(M_PI + t2.first) * get_corner_position(&cx, which ? -mul : 2*mul);
      }
    }
  
  return cellrelmatrix(c, i) * get_corner_position(c->move(i), (cellwalker(c, i) + wstep + (which?-1:2)).spin);
  }

hyperpoint midcorner(cell *c, int i, ld v) {
  auto hcor = farcorner(c, i, 0);
  auto tcor = get_corner_position(c, i, 3);
  return mid_at(tcor, hcor, v);
  }

hyperpoint get_warp_corner(cell *c, int cid) {
  // midcorner(c, cid, .5) but sometimes easier versions exist
  if(GOLDBERG) return gp::get_corner_position(c, cid, 2);
  if(IRREGULAR || archimedean) return midcorner(c, cid, .5);
  return ddspin(c,cid,M_PI/S7) * xpush0(tessf/2);
  }
  
  }

