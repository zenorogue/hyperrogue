/* underlying/spacetime views, and also replaying */

namespace hr {

namespace ads_game {

hrmap *map_hyp;

void switch_pause() {
  paused = !paused;
  if(paused) {
    current_ship = current;
    vctr_ship = vctr;
    vctrV_ship = vctrV;
    view_pt = 0;
    }
  else {
    current = current_ship;
    vctr = new_vctr = vctr_ship;
    vctrV = new_vctrV = vctrV_ship;
    }
  }

bool hv_klein = false;

int use_duality = 2;

transmatrix Duality;

void switch_spacetime_to(bool b) {
  if(in_spacetime() != b) switch_spacetime();
  }

bool in_spacetime() {
  return main_rock ? hyperbolic : rotspace;
  }

void switch_spacetime() {
  clearMessages();

  if(main_rock) {
  
    if(sphere) {
      cgi.use_count++;
      geometry = gSpace435;
      variation = eVariation::pure;
      swap(currentmap, map_hyp);
      pmodel = hv_klein ? mdDisk : mdPerspective;
      if(hv_klein) pconf.alpha = 0;
      check_cgi();
      cgi.require_basics();
      cgi.use_count++;
      vid.grid = true;
      stdgridcolor = 0xFFFFFFFF;

      initcells();
      initgame();
      vid.fov = 150;
      models::desitter_projections = true;
      }

    else if(hyperbolic) {
      geometry = gSphere;
      variation = eVariation::bitruncated;
      swap(currentmap, map_hyp);
      pmodel = mdDisk;
      pconf.scale = .95;
      check_cgi();
      }

    }
  
  else {
    check_cgi();
    cgi.use_count++;
    if(hyperbolic) {
      hybrid::switch_to_actual();
      pmodel = mdRelPerspective;
      hyperpoint res;
      nonisotropic_weird_transforms = true;
      NLP = Id;
      Duality = Id;
      for(int a=0; a<4; a++) for(int b=0; b<4; b++) Duality[a][b] = (a^2) == b;
      vid.fov = 150;
      vid.grid = false;
      slr::range_xy = 2;
      slr::range_z = 2;
      }

    else if(hybri) {
      hybrid::switch_to_underlying();
      pmodel = mdDisk;
      pconf.scale = .95;
      }
    cgi.use_count++;
    }

  View = Id;
  cwt = centerover = currentmap->gamestart();
  }

bool ads_draw_cell(cell *c, const shiftmatrix& V) {
  auto cur_w = hybrid::get_where(c);
  auto& ci = ci_at[cur_w.first];
  if(ci.type) {
    c->wall = waWaxWall;
    c->landparam =
      ci.type == wtSolid ? 0x603000 :
      ci.type == wtDestructible ? 0x301800 :
      0x080828;
    }
  else {
    c->wall = waNone;
    }
  return false;
  }

void replay_animation() {
  nomap = main_rock ? (!hyperbolic || among(pmodel, mdRelPerspective, mdRelOrthogonal)) : !sl2;

  if(in_replay) {
    view_pt = (ticks / 1000.) * DS_(simspeed);
    ld maxt = history.back().start + 0.001;
    view_pt -= maxt * floor(view_pt / maxt);
    for(auto& ss: history)
      if(ss.start + ss.duration > view_pt) {
        current = ss.current;
        if(main_rock) {
          dynamicval<eGeometry> g(geometry, gSpace435);
          current.T = inverse(ss.at.T * spin(-(ss.ang)*degree));
          current.T = lorentz(3, 2, view_pt - ss.start) * current.T;
          }
        else PIA({
          vctr = new_vctr = ss.vctr;
          vctrV = new_vctrV = ss.vctrV;
          current.T = cspin(3, 2, view_pt - ss.start) * current.T;
          if(auto_rotate)
            current.T = cspin(1, 0, view_pt - ss.start) * current.T;
          });
        break;
        }
    }
  
  if(main_rock && hyperbolic) {
    View = Id;
    centerover = currentmap->gamestart();
    ld s = current.shift;
    while(s > +1) { View = lorentz(2, 3, -1) * View; optimizeview(); s--; }
    while(s < -1) { View = lorentz(2, 3, 1) * View; optimizeview(); s++; }
    View = current.T * lorentz(2, 3, -s) * View; optimizeview();
    centerover->wall = waNone;
    }
  
  if(!main_rock && rotspace) {
    check_cgi();

    ads_matrix CV = current * vctrV;
    centerover = hybrid::get_at(vctr, 0);
    
    View = Id;

    // chg_shift(CV.shift) * CV.T
    
    auto sub_optimizeview = [&] {
      /* the important difference from optimizeview() is that we do not try to fix */
      transmatrix iView = inverse(View);
      virtualRebase(centerover, iView);
      View = inverse(iView);
      };

    ld s = CV.shift;
    View = CV.T * View; optimizeview();
    while(s > 1) { View = chg_shift(1) * View; sub_optimizeview(); s--; }
    while(s < -1) { View = chg_shift(-1) * View; sub_optimizeview(); s++; }
    View = chg_shift(s) * View; sub_optimizeview();

    if(use_duality == 1) nomap = true;
    if(use_duality == 2) View = spin(90*degree) * View;

    centerover->wall = waNone;
    }
  }

void switch_replay() {
  if(!paused) switch_pause();
  in_replay = !in_replay;
  if(in_replay) {
    anims::period = 1000. * history.back().start / DS_(simspeed);    
    anims::noframes = anims::period * 60 / 1000;
    }
  }

auto view_hooks =
+ arg::add3("-ads-recenter", [] { current = Id; })
+ arg::add3("-ads-replay", [] { arg::shift(); int i = arg::argi(); if(i != in_replay) switch_replay(); })
+ arg::add3("-ads-duality", [] { arg::shift(); use_duality = arg::argi(); })
+ arg::add3("-ads-cone", [] { arg::shift(); which_cross = arg::argi(); })
+ arg::add3("-ads-spacetime", [] { arg::shift(); switch_spacetime_to(arg::argi()); });

}}
