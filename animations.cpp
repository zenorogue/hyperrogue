// Hyperbolic Rogue -- animations (movement, attack, hug, fall animations, flashes, particles, etc.)
// Copyright (C) 2011-2025 Zeno Rogue, see 'hyper.cpp' for details

#include "hyper.h"
namespace hr {

//=== animation

#if HDR
struct animation {
  int ltick;
  double footphase;
  transmatrix wherenow;
  int attacking; /** 0 = no attack animation, 1 = first phase, 2 = second phase, 3 = hugging */
  transmatrix attackat;
  bool mirrored;
  eItem thrown_item; /** for thrown items */
  eMonster thrown_monster; /** for thrown monsters */
  };

// we need separate animation layers for Orb of Domination and Tentacle+Ghost,
// and also to mark Boats
#define ANIMLAYERS 4
#define LAYER_BIG   0 // for worms and krakens
#define LAYER_SMALL 1 // for others
#define LAYER_BOAT  2 // mark that a boat has moved
#define LAYER_THROW 3 // for thrown items
#endif

EX array<map<cell*, animation>, ANIMLAYERS> animations;

EX int revhint(cell *c, int hint) {
  if(hint >= 0 && hint < c->type) return c->c.spin(hint);
  else return hint;
  }

EX transmatrix adj(const movei& m) {
  if(m.proper()) return currentmap->adj(m.s, m.d);
  else return currentmap->relative_matrix(m.t, m.s, C0);
  }

EX transmatrix iadj(const movei& m) {
  if(m.proper()) return currentmap->iadj(m.s, m.d);
  else return currentmap->relative_matrix(m.s, m.t, C0);
  }

EX void animateMovement(const movei& m, int layer) {
  if(vid.mspeed >= 5) return; // no animations!
  LATE ( animateMovement(m, layer); )
  transmatrix T = iadj(m);
  bool found_s = animations[layer].count(m.s);
  animation& a = animations[layer][m.t];
  if(found_s) {
    a = animations[layer][m.s];
    a.wherenow = T * a.wherenow;
    if(m.s != m.t)
      animations[layer].erase(m.s);
    a.attacking = 0;
    }
  else {
    a.ltick = ticks;
    a.wherenow = T;
    a.footphase = 0;
    a.mirrored = false;
    }
  if(m.proper() && m.s->c.mirror(m.d))
    a.mirrored = !a.mirrored;
  }

EX void animate_item_throw(cell *from, cell *to, eItem it, eMonster mo IS(moNone)) {

  bool steps = false;
  again:
  if(from != to) {
    forCellIdEx(c1, i, from) if(celldistance(c1, to) < celldistance(from, to)) {
      animateMovement(movei(from, i), LAYER_THROW);
      from = c1;
      steps = true;
      goto again;
      }    
    }
  
  if(steps) {
    animation& a = animations[LAYER_THROW][to];
    a.thrown_item = it;
    a.thrown_monster = mo;
    }
  }

EX void animateAttackOrHug(const movei& m, int layer, int phase, ld ratio, ld delta) {
  LATE( animateAttack(m, layer); )
  if(vid.mspeed >= 5) return; // no animations!
  transmatrix T = iadj(m);
  bool newanim = !animations[layer].count(m.s);
  animation& a = animations[layer][m.s];
  a.attacking = phase;
  auto TC0 = tile_center();
  a.attackat = lrspintox(iso_inverse(T) * TC0) * lxpush(hdist(TC0, T*TC0) * ratio + delta);
  if(newanim) a.wherenow = Id, a.ltick = ticks, a.footphase = 0;
  }

EX void animateAttack(const movei& m, int layer) {
  animateAttackOrHug(m, layer, 1, 1/3., 0);
  }

EX void animateCorrectAttack(const movei& m, int layer, eMonster who) {
  if(among(who, moPlayer, moMimic, moIllusion, moShadow) && (getcs().charid/2) == pshSpaceship) {
    animate_item_throw(m.s, m.t, itNone, moBullet);
    return;
    }
  animateAttackOrHug(m, layer, 1, 1/3., 0);
  }

EX void animateHug(const movei& m, int layer) {
  animateAttackOrHug(m, layer, 3, 0.5, ((getcs().charid/2) == pshRatling ? -0.15 : -0.0713828) * cgi.scalefactor);
  }

vector<pair<cell*, animation> > animstack;

EX void indAnimateMovement(const movei& m, int layer) {
  if(vid.mspeed >= 5) return; // no animations!
  LATE( indAnimateMovement(m, layer); )
  if(animations[layer].count(m.t)) {
    animation res = animations[layer][m.t];
    animations[layer].erase(m.t);
    animateMovement(m, layer);
    if(animations[layer].count(m.t)) 
      animstack.push_back(make_pair(m.t, animations[layer][m.t]));
    animations[layer][m.t] = res;
    }
  else {
    animateMovement(m, layer);
    if(animations[layer].count(m.t)) {
      animstack.push_back(make_pair(m.t, animations[layer][m.t]));
      animations[layer].erase(m.t);
      }
    }
  }

EX void commitAnimations(int layer) {
  LATE( commitAnimations(layer); )
  for(int i=0; i<isize(animstack); i++)
    animations[layer][animstack[i].first] = animstack[i].second;
  animstack.clear();
  }

EX bool applyAnimation(cell *c, shiftmatrix& V, double& footphase, int layer) {
  if(!animations[layer].count(c)) return false;
  animation& a = animations[layer][c];

  int td = ticks - a.ltick;
  ld aspd = td / 1000.0 * exp(vid.mspeed);
  ld R;
  again:
  auto TC0 = cgi.emb->anim_tile_center();

  if(among(a.attacking, 1, 3))
    R = hdist(a.attackat * TC0, a.wherenow * TC0);
  else
    R = hdist(a.wherenow * TC0, TC0);
  aspd *= (1+R+(shmup::on?1:0));

  if(a.attacking == 3 && aspd > R) aspd = R;

  if((R < aspd || std::isnan(R) || std::isnan(aspd) || R > 10) && a.attacking != 3) {
    if(a.attacking == 1) { a.attacking = 2; goto again; }
    animations[layer].erase(c);
    return false;
    }
  else {
    transmatrix T = inverse(a.wherenow);
    ld z = cgi.emb->anim_center_z();
    if(z) T = lzpush(-z) * T;

    hyperpoint wnow;
    if(a.attacking == 1 || a.attacking == 3)
      wnow = T * a.attackat * TC0;
    else
      wnow = T * TC0;
    
    shift_v_towards(T, shiftless(wnow), aspd, shift_method(smaAnimation));
    if(z) T = lzpush(z) * T;
    a.wherenow = inverse(T);
    fixmatrix(a.wherenow);

    if(cgflags & qAFFINE) {
      transmatrix T = a.wherenow;
      fixmatrix_euclid(T);
      a.wherenow = inverse(T) * a.wherenow;
      for(int i=0; i<MDIM; i++)
        a.wherenow[i] = lerp(a.wherenow[i], Id[i], aspd / R);
      a.wherenow = T * a.wherenow;
      }

    a.footphase += a.attacking == 2 ? -aspd : aspd;
    if(a.attacking == 3 && aspd >= R) {
      a.footphase = 0;
      hyperpoint h1 = a.wherenow * C0;
      a.wherenow = rgpushxto0(h1) * lrspintox(h1);
      }
    footphase = a.footphase;
    V = V * a.wherenow * lrspintox(wnow);
    if(cgi.emb->is_cylinder() && !gproduct) {
      if(nil) {
        V = V * lzpush(1);
        V = V * spin270();
        V = V * lzpush(-1);
        }
      else
        V = V * cspin90(1, 0);
      }
    if(a.mirrored) V = V * lmirror();
    if(a.attacking == 2) V = V * lpispin();
    a.ltick = ticks;
    return true;
    }
  }

EX double chainAngle(cell *c, shiftmatrix& V, cell *c2, double dft, const shiftmatrix &Vwhere) {
  if(cgi.emb->no_spin()) return 0;
  if(!gmatrix0.count(c2)) return dft;
  hyperpoint h = cgi.emb->anim_tile_center();
  if(animations[LAYER_BIG].count(c2)) h = animations[LAYER_BIG][c2].wherenow * h;
  h = inverse_shift(V, Vwhere) * calc_relative_matrix(c2, c, C0) * h;
  ld z = cgi.emb->anim_center_z();
  if(z) h = lzpush(-z) * h;
  return atan2(h[1], h[0]);
  }

// equivalent to V = V * spin(-chainAngle(c,V,c2,dft));
EX bool chainAnimation(cell *c, cell *c2, shiftmatrix& V, const shiftmatrix &Vwhere, ld& length) {
  if(cgi.emb->no_spin()) return false;
  hyperpoint h = cgi.emb->anim_tile_center();
  if(animations[LAYER_BIG].count(c2)) h = animations[LAYER_BIG][c2].wherenow * h;
  h = inverse_shift(V, Vwhere) * h;
  length = hdist(h, tile_center());
  ld z = cgi.emb->center_z();
  if(z) h = lzpush(-z) * h;
  V = V * rspintox(h);
  return true;  
  }

struct fallanim {
  int t_mon, t_floor, pid;
  eWall walltype;
  eMonster m;
  fallanim() { t_floor = 0; t_mon = 0; pid = 0; walltype = waNone; }
  };

map<cell*, fallanim> fallanims;

#if HDR
struct flashdata {
  int t;
  int size;
  cell *where;
  transmatrix angle_matrix;
  ld bubblesize;
  int spd; // 0 for flashes, >0 for particles
  color_t color;
  string text;
  flashdata(int _t, int _s, cell *_w, color_t col, int sped) { 
    t=_t; size=_s; where=_w; color = col;
    spd = sped;
    angle_matrix = random_spin();
    }
  };
#endif

EX vector<flashdata> flashes;

auto ahgf = addHook(hooks_removecells, 1, [] () {
  eliminate_if(flashes, [] (flashdata& f) { return is_cell_removed(f.where); });
  });

EX void drawBubble(cell *c, color_t col, string s, ld size) {
  LATE( drawBubble(c, col, s, size); )
  auto fd = flashdata(ticks, 1000, c, col, 0);
  fd.text = s;
  fd.bubblesize = size;
  flashes.push_back(fd);
  }

EX void drawFlash(cell *c) {
  flashes.push_back(flashdata(ticks, 1000, c, iinf[itOrbFlash].color, 0)); 
  }
EX void drawBigFlash(cell *c) { 
  flashes.push_back(flashdata(ticks, 2000, c, 0xC0FF00, 0)); 
  }

EX void drawParticleSpeed(cell *c, color_t col, int speed) {
  LATE( drawParticleSpeed(c, col, speed); )
  if(vid.particles && !confusingGeometry())
    flashes.push_back(flashdata(ticks, rand() % 16, c, col, speed)); 
  }
EX void drawParticle(cell *c, color_t col, int maxspeed IS(100)) {
  drawParticleSpeed(c, col, 1 + rand() % maxspeed);
  }

EX void drawDirectionalParticle(cell *c, int dir, color_t col, int maxspeed IS(100)) {
  LATE( drawDirectionalParticle(c, dir, col, maxspeed); )
  if(vid.particles && !confusingGeometry()) {
    int speed = 1 + rand() % maxspeed;
    auto fd = flashdata(ticks, rand() % 16, c, col, speed);
    ld angle = -atan2(tC0(currentmap->adj(c, dir)));
    angle += TAU * (rand() % 100 - rand() % 100) / 100 / c->type;
    fd.angle_matrix = spin(angle);
    flashes.push_back(fd); 
    }
  }

EX void drawParticles(cell *c, color_t col, int qty, int maxspeed IS(100)) { 
  if(vid.particles)
    while(qty--) drawParticle(c,col, maxspeed); 
  }
EX void drawFireParticles(cell *c, int qty, int maxspeed IS(100)) { 
  if(vid.particles)
    for(int i=0; i<qty; i++)
      drawParticle(c, firegradient(i / (qty-1.)), maxspeed); 
  }
EX void fallingFloorAnimation(cell *c, eWall w IS(waNone), eMonster m IS(moNone)) {
  if(!wmspatial) return;
  LATE( fallingFloorAnimation(c, w, m); )
  fallanim& fa = fallanims[c];
  fa.t_floor = ticks;
  fa.walltype = w; fa.m = m;
  // drawParticles(c, darkenedby(linf[c->land].color, 1), 4, 50);
  }
EX void fallingMonsterAnimation(cell *c, eMonster m, int id IS(multi::cpid)) {
  if(!mmspatial) return;
  LATE( fallingMonsterAnimation(c, m, id); )
  fallanim& fa = fallanims[c];
  fa.t_mon = ticks;
  fa.m = m;
  fa.pid = id;
  // drawParticles(c, darkenedby(linf[c->land].color, 1), 4, 50);
  }

void celldrawer::draw_fallanims() {
  poly_outline = OUTLINE_NONE;
  if(fallanims.count(c)) {
     int q = isize(ptds);
     int maxtime = euclid || sphere ? 20000 : 1500;
     fallanim& fa = fallanims[c];
     bool erase = true;
     if(fa.t_floor) {
       int t = (ticks - fa.t_floor);
       if(t <= maxtime) {
         erase = false;
         if(GDIM == 3)
           draw_shapevec(c, V, qfi.fshape->levels[SIDE::FLOOR], darkena(fcol, fd, 0xFF), PPR::WALL);
         else if(fa.walltype == waNone) {
           draw_qfi(c, V, darkena(fcol, fd, 0xFF), PPR::FLOOR);
           }
         else {
           celldrawer ddalt;
           eWall w = c->wall; int p = c->wparam;
           c->wall = fa.walltype; c->wparam = fa.m;
           ddalt.c = c;
           ddalt.setcolors();
           int starcol = c->wall == waVinePlant ? 0x60C000 : ddalt.wcol;
           c->wall = w; c->wparam = p;
           draw_qfi(c, orthogonal_move_fol(V, cgi.WALL), darkena(starcol, fd, 0xFF), PPR::WALL_TOP);
           queuepolyat(orthogonal_move_fol(V, cgi.WALL), cgi.shWall[ct6], darkena(ddalt.wcol, 0, 0xFF), PPR::WALL_DECO);
           forCellIdEx(c2, i, c)
             if(placeSidewall(c, i, SIDE::WALL, V, darkena(ddalt.wcol, 1, 0xFF))) break;
           }
         pushdown(c, q, V, t*t / 1000000. + t / 1000., true, true);
         }
       }
     if(fa.t_mon) {
       dynamicval<int> d(multi::cpid, fa.pid);
       int t = (ticks - fa.t_mon);
       if(t <= maxtime) {
         erase = false;
         c->stuntime = 0;
         shiftmatrix V2 = V;
         double footphase = t / 200.0;
         applyAnimation(c, V2, footphase, LAYER_SMALL);
         drawMonsterType(fa.m, c, V2, minf[fa.m].color, footphase, NOCOLOR);
         pushdown(c, q, V2, t*t / 1000000. + t / 1000., true, true);
         }
       }
     if(erase) fallanims.erase(c);
     }
   }

#if CAP_QUEUE
always_false static_bubbles;

EX void draw_flash(struct flashdata& f, const shiftmatrix& V, bool& kill) {  
  int tim = ticks - f.t;
 
  if(tim <= f.size && !f.spd) kill = false;

  if(f.text != "") {
    if(static_bubbles) {
      tim = 0; kill = false;
      }
    color_t col = f.color;
    dynamicval<color_t> p(poly_outline, poly_outline);
    int r = 2;
    apply_neon(col, r);
    if(GDIM == 3 || sphere)
      queuestr(V, (1 - tim * 1. / f.size) * f.bubblesize * mapfontscale / 100, f.text, col, r);
    else if(!kill) {
      shiftpoint h = tC0(V);
      if(hdist0(h) > .1) {
        transmatrix V2 = rspintox(h.h) * xpush(hdist0(h.h) * (1 / (1 - tim * 1. / f.size)));
        queuestr(shiftless(V2, h.shift), f.bubblesize * mapfontscale / 100, f.text, col, r);
        }
      }
    if(static_bubbles) {
      ld rad[25];
      for(int a=0; a<24; a++) rad[a] = (0.5 + randd() * .3 + 0.5 * (a&1)) / (2.8 + celldistance(f.where, cwt.at) * .2);
      rad[24] = rad[0];
      for(int a=0; a<24; a++) curvepoint(xspinpush0(TAU * a / 24, rad[a]));
      queuecurve(V, 0xFF, 0xFF0000FF, PPR::SUPERLINE);
      }
    }
    
  else if(f.spd) {
    #if CAP_SHAPES
    if(tim <= 300) kill = false;
    int partcol = darkena(f.color, 0, GDIM == 3 ? 255 : max(255 - tim*255/300, 0));
    poly_outline = OUTLINE_DEFAULT;
    ld t = f.spd * tim * cgi.scalefactor / 50000.;
    shiftmatrix T = V * f.angle_matrix * (GDIM == 2 ? xpush(t) : cpush(2, t));
    queuepoly(T, cgi.shParticle[f.size], partcol);
    #endif
    }
  
  else if(f.size == 1000) {
    for(int u=0; u<=tim; u++) {
      if((u-tim)%50) continue;
      if(u < tim-150) continue;
      ld rad = u * 3 / 1000.;
      rad = rad * (5-rad) / 2;
      rad *= cgi.hexf;
      int flashcol = f.color;
      if(u > 500) flashcol = gradient(flashcol, 0, 500, u, 1100);
      flashcol = darkena(flashcol, 0, 0xFF);
#if MAXMDIM >= 4
      if(GDIM == 3)
        queueball(V * lzpush(cgi.GROIN1), rad, flashcol, itDiamond);
      else 
#endif
      {
        PRING(a) curvepoint(xspinpush0(a * cgi.S_step, rad));
        queuecurve(V, flashcol, 0x8080808, PPR::LINE);
        }
      }
    }
  else if(f.size == 2000) {
    for(int u=0; u<=tim; u++) {
      if((u-tim)%50) continue;
      if(u < tim-250) continue;
      ld rad = u * 3 / 2000.;
      rad = rad * (5-rad) * 1.25;
      rad *= cgi.hexf;
      int flashcol = f.color;
      if(u > 1000) flashcol = gradient(flashcol, 0, 1000, u, 2200);
      flashcol = darkena(flashcol, 0, 0xFF);
#if MAXMDIM >= 4
      if(GDIM == 3)
        queueball(V * lzpush(cgi.GROIN1), rad, flashcol, itRuby);
      else 
#endif
      {
        PRING(a) curvepoint(xspinpush0(a * cgi.S_step, rad));
        queuecurve(V, flashcol, 0x8080808, PPR::LINE);
        }
      }
    }
  }
#endif

EX void drawFlashes() {
  #if CAP_QUEUE
  for(int k=0; k<isize(flashes); k++) {
    bool kill = true;
    flashdata& f = flashes[k];
    bool copies = false;
    for (const shiftmatrix& V : hr::span_at(current_display->all_drawn_copies, f.where)) {
      copies = true;
      draw_flash(f, V, kill);
      }
    forCellIdEx(c2, id, f.where) {
      if(!copies) {
        for (const shiftmatrix& V : hr::span_at(current_display->all_drawn_copies, c2)) {
          draw_flash(f, V * currentmap->iadj(f.where, id), kill);
          copies = true;
          }
        }
      }
    if(f.t > ticks - 800 && !copies) {
      kill = false;
      }
    if(kill) {
      f = flashes[isize(flashes)-1];
      flashes.pop_back(); k--;
      }
    }
  #endif
  }

EX void clearAnimations() {
  for(int i=0; i<ANIMLAYERS; i++) animations[i].clear();
  flashes.clear();
  fallanims.clear();
  }
  
EX ld animation_factor = 1;
EX int animation_lcm = 0;

EX ld ptick(int period, ld phase IS(0)) {
  if(animation_lcm) animation_lcm = animation_lcm * (period / gcd(animation_lcm, period));
  return (ticks * animation_factor * vid.ispeed) / period + phase * TAU;
  }

EX ld fractick(int period, ld phase IS(0)) {
  ld t = ptick(period, phase) / TAU;
  t -= floor(t);
  if(t<0) t++;
  return t;
  }
  
EX ld sintick(int period, ld phase IS(0)) {
  return sin(ptick(period, phase));
  }

EX transmatrix spintick(int period, ld phase IS(0)) {
  return spin(ptick(period, phase));
  }

auto animcm = addHook(hooks_gamedata, 0, [] (gamedata* gd) {
  gd->store(animations);
  gd->store(flashes);
  gd->store(fallanims);
  });

}
