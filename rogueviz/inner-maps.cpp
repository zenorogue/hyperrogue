// compile with:
// ./mymake -O3 inner-maps
// used in: https://twitter.com/zenorogue/status/1212408644941295619

// run with:
// ./hyper -innner-map -noscr -canvas r -canvas i -wsh 0 -smart 4 -shott 0 -alpha 1

#include "rogueviz.h"

namespace hr {

namespace inner {

bool out = true;

EX struct renderbuffer *buf1, *buf2;

map<cell*, basic_textureinfo> lti;

ld mdist;

bool for_klein = false;

ld alpha = -.7;

cell *rendercenter;
shiftmatrix rendercenterV;

bool auto_move = false;

ld texture_alpha = 1;
ld inner_alpha = 1;
bool dense = false;

// should do 30x -- temporarily disabled

ld iterations = 1;

ld iterations_bank;

void gentexture() {
  if(out) iterations_bank += iterations;
  while(iterations_bank >= 1) {
    iterations_bank -= 1;
    println(hlog, "# = ", isize(lti));
    dynamicval<bool> b(out, false);
    dynamicval<ld> va(pconf.alpha, texture_alpha);
    if(!buf1) {
      buf1 = new renderbuffer(rug::texturesize, rug::texturesize, true);
      buf2 = new renderbuffer(rug::texturesize, rug::texturesize, true);
      }
    resetbuffer rb;
    rug::calcparam_rug();
    models::configure();

    buf1->enable();
    buf1->clear(0);
    buf2->use_as_texture();

    draw_boundary(0);
    draw_boundary(1);

    draw_model_elements();
    drawthemap();
    drawqueue();
    calcparam();

    rb.reset();

    swap(buf1, buf2);

    rendercenter = centerover;
    rendercenterV = gmatrix[centerover];

    }
  }

void frame() {
  gentexture();
  if(auto_move) {
    color_t col = minf[moReptile].color;
    drawMonsterType(moReptile, NULL, shiftless(spin(alpha)), col, mdist, col);
    mapeditor::drawplayer = false;
    }
  }

bool done;

hpcshape edgeshape;

hpcshape circ;

ld crad = dense ? .5 : 1.82;
ld ceps = .02;

ld crad_p;

void need_redo() {
  done = false;
  }

void make_shape() {
  if(done) return;
  done = true;
  cgi.bshape(circ, PPR::WALL);
  circ.flags |= POLY_TRIANGLES;
  int k = for_klein ? 1 : 2;
  int maxz = for_klein ? 120 : 30;
  for(int z=0; z<maxz; z++) {
    ld ra = (z+0.)/maxz * crad, rb = (z+1.)/maxz * crad;
    for(int i=0; i<360; i+=k) {
      ld a = i * degree;
      ld b = (i+k) * degree;

      cgi.hpcpush(xspinpush0(a, ra));
      cgi.hpcpush(xspinpush0(a, rb));
      cgi.hpcpush(xspinpush0(b, rb));

      if(z) {
        cgi.hpcpush(xspinpush0(a, ra));
        cgi.hpcpush(xspinpush0(b, ra));
        cgi.hpcpush(xspinpush0(b, rb));
        }
      }
    }
  cgi.finishshape();
  println(hlog, "circ vertices = ", circ.e - circ.s);

  cgi.bshape(edgeshape, PPR::WALL);
  for(int i=0; i<=360; i+=k) {
    ld a = i * degree;
    cgi.hpcpush(xspinpush0(a, crad - ceps));
    }
  for(int i=0; i<=360; i+=k) {
    ld a = i * degree;
    cgi.hpcpush(xspinpush0(a, crad + ceps));
    }
  cgi.finishshape();

  cgi.extra_vertices();

  hyperpoint ph = xspinpush0(0, crad);
  ph /= (inner_alpha+ph[2]);
  crad_p = ph[0];
  }

set<cell*> gs;

bool render(cell *c, const shiftmatrix& V) {
  if(!buf1 || !buf2 || !rendercenter) return false;
  make_shape();
  if(false && !gs.count(c)) {
    gs.insert(c);
    c->mondir = hrand(c->type);
    dont_face_pc = true;
    switch(hrand(100)) {
      case 0:
        c->monst = moOrangeDog;
        break;
      case 1:
        c->item = itHell;
        break;
      case 2:
        c->monst = moSalamander;
        break;
      case 3:
        c->monst = moButterfly;
        break;
      case 4:
        c->monst = moSparrowhawk;
        break;
      }
    }
  if(dense ? pseudohept(c) : cdist50(c) == 0) {
    dynamicval<color_t> po(poly_outline, 0x000000FF);
    auto& p = lti[c];
    p.texture_id = buf2->renderedTexture;
    p.tvertices.clear();

  /*inner_alpha = anyshiftclick ? 1 : 0;
  hyperpoint ph = xspinpush0(0, crad);
  ph /= (inner_alpha+ph[2]);
  crad_p = ph[0];
  */

    for(int i=circ.s; i<circ.e; i++) {
      hyperpoint h = cgi.hpc[i];
      // hyperboloid to Poincare
      h /= (inner_alpha + h[2]);
      // scale up
      h /= crad_p;
      h *= .99999999;
      // Poincare to hyperboloid

      h = perspective_to_space(h, inner_alpha, cgclass);

      /*
      ld hr = sqhypot_d(2, h);
      ld hz = (1 + hr) / (1 - hr);
      h[0] *= (hz+1);
      h[1] *= (hz+1);
      h[2] = hz;
      */

      // move according to V
      h = rendercenterV.T * inverse(ggmatrix(rendercenter).T) * V.T * h;
      // texture coordinates
      // hyperpoint scr;
      // dynamicval<ld> b(pconf.alpha, anyshiftclick ? pconf.alpha : texture_alpha);
      // applymodel(h, scr);
      hyperpoint scr = h / (texture_alpha + h[2]);
      p.tvertices.push_back(glhr::makevertex(.5 + .5 * scr[0], .5 - .5 * scr[1], 0));
      }
    queuepoly(V, circ, 0xFFFFFFFF).tinf = &p;
    queuepoly(V, edgeshape, darkena(c->landparam, 0, 0xFF));
    }
  return false;
  }

void shift() {
  if(auto_move) {
    centerover = currentmap->gamestart();
    mdist = (ticks % 10000) * 6 / 10000.;
    View = spin(alpha) * xpush(-mdist) * spin(-alpha);
    anims::moved();
    centerover->monst = moNone;
    forCellEx(c1, centerover) c1->monst = moNone;
    }
  }

void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);
  dialog::init(XLAT("inner maps"), 0xFFFFFFFF, 150, 0);
  add_edit(texture_alpha);
  add_edit(inner_alpha);
  add_edit(dense);
  add_edit(crad);
  add_edit(ceps);
  add_edit(auto_move);
  add_edit(iterations);
  dialog::addBack();
  dialog::display();
  }

void enable() {
  using rogueviz::rv_hook;
  rv_hook(hooks_frame, 100, frame);
  rv_hook(hooks_drawcell, 100, render);
  rv_hook(anims::hooks_anim, 100, shift);
  rv_hook(hooks_o_key, 80, [] (o_funcs& v) { v.push_back(named_dialog("inner maps", show)); });
  }

auto hook = arg::add3("-inner-map", enable)
  + addHook(hooks_configfile, 100, [] {
    param_f(texture_alpha, "inner_talpha")
    ->editable(0, 5, .1, "texture projection distance", "", 't');
    param_f(inner_alpha, "inner_ialpha")
    ->editable(0, 5, .1, "inner projection distance", "", 'i')
    ->set_reaction(need_redo);
    param_b(dense, "inner_dense")
    ->editable("densely packed maps", 'd');
    param_f(crad, "inner_crad")
    ->editable(0, 10, .1, "radii of the inner maps", "", 'r')
    ->set_reaction(need_redo);
    param_f(ceps, "inner_ceps")
    ->editable(0, 0.1, .001, "map frame size", "", 'f')
    ->set_reaction(need_redo);
    param_b(auto_move, "auto_move")
    ->editable("animate", 'a');
    param_f(iterations, "inner_iterations")
    ->editable(0, 30, 0.2, "iterations per frame",
      "How many times per frame should we re-render the map",
      'i')
    ->set_reaction(need_redo);
    })
  + addHook_rvslides(52, [] (string s, vector<tour::slide>& v) {
      if(s != "projections") return;
      using namespace tour;

      v.push_back(slide{
        "projections/hyperbolic to hyperbolic", 10, LEGAL::NONE | QUICKGEO,

        "We can also project a hyperbolic plane to a hyperbolic plane of different curvature.\n\n"
        "Creatures living in the hyperbolic world may use the native Poincaré model to get conformal, circular, and totally useless maps of their whole world.\n\n"

        "Press 'o' to change the settings."
        ,
        [] (presmode mode) {
          slide_url(mode, 't', "Twitter link (with description)", "https://twitter.com/zenorogue/status/1212408644941295619");
          setCanvas(mode, 'r');

          if(mode == pmStart) {
            if(!shmup::on) restart_game(rg::shmup);
            slide_backup(mapeditor::drawplayer, true);
            enable();
            start_game();
            }
          }});
      });

}
}
