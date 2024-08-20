namespace nilrider {

hyperpoint get_spline(ld t);

level *layer_edited;

bool level::simulate() {
  loaded_or_planned = true;
  if(history.empty())
    history.push_back(start);
  auto at = history.back();

  if(at.t >= isize(plan) - 1.001) return false;
  
  ld goal_t;

  if(at.on_surface && isize(history) >= 2 && !history[isize(history)-2].on_surface) {
    int index = ceil(history[isize(history)-2].t)+1;
    if(index >= isize(plan)) {
      return false;
      }
    plan[index].at = at.where;
    history.back().t = at.t = index;
    if(index == isize(plan) - 1) return false;
    }
  
  if(at.on_surface) {
    int steps = 20;
    ld min_t, max_t;
    
    if(isize(history) == 1) {
      steps = 60;
      min_t = at.t;
      max_t = at.t + 0.5;
      }
    else {
      ld ldiff = history.back().t - history[history.size() - 2].t;
      min_t = at.t;
      max_t = min<ld>(at.t + ldiff + .1, isize(plan)-1);
      }
    
    auto f = [&] (ld t) {
      hyperpoint h = get_spline(t);
      auto copy = at;
      copy.heading_angle = atan2(h[1] - at.where[1], h[0] - at.where[0]);
      copy.tick(this);
      return sqhypot_d(2, copy.where-h);
      };

    string seq = "";
    
    for(int i=0; i<steps; i++) {
      ld t1 = min_t * .6 + max_t * .4;
      ld t2 = min_t * .4 + max_t * .6;
      auto e1 = f(t1);
      auto e2 = f(t2);        
      if(e1 < e2) max_t = t2, seq += "B";
      else min_t = t1, seq += "A";
      }
    
    goal_t = (min_t + max_t) / 2;

    hyperpoint h = get_spline(goal_t);
    at.heading_angle = atan2(h[1] - at.where[1], h[0] - at.where[0]);

    history.back() = at;
    at.be_consistent();
    if(!at.tick(this)) { println(hlog, "tick returns false"); return false; }
    at.t = goal_t;
    history.push_back(at);
    }
  else {
    at.be_consistent();
    if(at.where[2] <= surface(at.where) - 100) { println(hlog, "fall"); return false; }
    if(!at.tick(this)) { println(hlog, "crash"); return false; }
    history.push_back(at);
    }
  
  return true;
  }

hyperpoint level::get_spline(ld t) {
  int i = t;
  if(i == isize(plan) - 1) return plan.back().at;
  ld tf = t - i;
  return plan[i].at * (1-tf) * (1-tf) * (1+2*tf) + plan[i+1].at * (tf*tf * (3-2*tf)) + plan[i].vel * tf * (1-tf) * (1-tf) - plan[i+1].vel * tf * tf * (1-tf);
  }

hyperpoint mousept;
ld box;

char planmode = 'p';
vector<pair<char, string> > buttons = {
  {'p', "pan"}, {'a', "add"}, {'m', "move"}, {'i', "insert"}, {'d', "delete"}, {'l', "levels"}
  };

bool recompute_plan_transform = true;

void level::compute_plan_transform() {
  dynamicval<eModel> pm(pmodel, mdDisk);
  dynamicval<eGeometry> g(geometry, gEuclid);
  dynamicval<bool> ga(vid.always3, false);
  dynamicval<geometryinfo1> gi(ginf[gEuclid].g, giEuclid2);
  auto& cd = current_display;  
  auto sId = shiftless(Id);
  ld pix = 1 / (2 * cgi.hcrossf / cgi.crossf);
  ld scale_x = (vid.xres - 2 * vid.fsize) / abs(real_maxx-real_minx);
  ld scale_y = (vid.yres - 2 * vid.fsize) / abs(real_maxy-real_miny);
  ld scale = min(scale_x, scale_y);
  plan_transform = sId * atscreenpos(cd->xcenter, cd->ycenter, pix * scale) * eupush(-(real_minx+real_maxx)/2, (real_miny+real_maxy)/2) * MirrorY;
  }

bool restored = false;

int plan_precision = 50;

void level::draw_planning_screen() {
  if(just_refreshing) return;

  restored = true;

  if(inHighQual) {
    new_levellines_for = mousept = current.where;
    }
  curlev->init_textures();
  for(auto lay: gen_layer_list()) lay->init_textures();
  dynamicval<eGeometry> g(geometry, gEuclid);
  dynamicval<eModel> pm(pmodel, mdDisk);
  dynamicval<bool> ga(vid.always3, false);
  dynamicval<geometryinfo1> gi(ginf[gEuclid].g, giEuclid2);
  check_cgi();
  cgi.require_shapes();
  for(auto lay: gen_layer_list()) lay->init_shapes();
  initquickqueue();
  
  if(recompute_plan_transform) {
    compute_plan_transform();
    recompute_plan_transform = false;
    }
    
  auto& T = plan_transform;
  
  auto scr_to_map = [&] (hyperpoint h) {
    transmatrix mousef = inverse(unshift(T)) * atscreenpos(h[0], h[1], 1);
    h = mousef * C0;
    h /= h[2];
    return h;
    };
  
  if(!inHighQual) mousept = scr_to_map(hpxy(mousex, mousey));

  box = inHighQual ? scr_to_map(hpxy(10, 0))[0] - scr_to_map(hpxy(0,0))[0] : scr_to_map(hpxy(mousex + 5, mousey))[0] - mousept[0];
  
  auto draw_layer = [&] (level *l, color_t col) {
    auto& p = queuepolyat(T, l->shPlanFloor, col, PPR::FLOOR);
    p.tinf = &l->uniltinf;
    l->uniltinf.texture_id = l->unil_texture_levels->textureid;
    };

  bool layer_found = false;
  auto layers = gen_layer_list();
  for(auto l: layers) {
    if(l == layer_edited) layer_found = true;
    else draw_layer(l, 0x808080FF);
    }
  if(!layer_found) layer_edited = this;
  draw_layer(layer_edited, 0xFFFFFFFF);
  
  auto draw_sq = [&] (hyperpoint h, color_t col, PPR prio) {
    curvepoint(hpxy(h[0]+box, h[1]+box));
    curvepoint(hpxy(h[0]+box, h[1]-box));
    curvepoint(hpxy(h[0]-box, h[1]-box));
    curvepoint(hpxy(h[0]-box, h[1]+box));
    curvepoint(hpxy(h[0]+box, h[1]+box));
    queuecurve(T, 0xFF, col, prio);
    };
  
  auto draw_line = [&] (hyperpoint h1, hyperpoint h2, color_t col, PPR prio) {
    curvepoint(hpxy(h1[0], h1[1]));
    curvepoint(hpxy(h2[0], h2[1]));
    queuecurve(T, col, 0, prio);
    };
  
  if(levellines_for[3])
    draw_sq(levellines_for, 0xFFC0FFFF, PPR::ITEM);

  /* draw the plan */
  for(auto& pp: plan) {
    draw_sq(pp.at - pp.vel, 0xFF8080FF, PPR::ITEM);
    draw_sq(pp.at + pp.vel, 0x80FF80FF, PPR::ITEM);
    draw_sq(pp.at, 0xFFFF00FF, PPR::ITEM);
    draw_line(pp.at - pp.vel, pp.at + pp.vel, 0x80, PPR::BFLOOR);
    }
  
  if(history.empty()) history.push_back(start);
  
  vid.linewidth *= 3;

  int ps = isize(plan);
  for(int t=0; t<=100*(ps-1); t++) {
    hyperpoint h = get_spline(t / 100.);
    curvepoint(hpxy(h[0], h[1]));
    }
  queuecurve(T, 0xFF8080C0, 0, PPR::LIZEYE);
  vid.linewidth /= 3;

  ld closest_dist = box * 2;
  current = history.back();

  vid.linewidth *= 3;
  level *current_surface = this;
  auto surface_color = [&] () -> color_t {
    if(current_surface == layer_edited) return 0xFFFFFFFF;
    if(current_surface) return 0x101010FF;
    return 0xFF00FFFF;
    };
  for(int i=0; i<isize(history);) {
    auto& hi = history[i];
    auto& h = hi.where;
    curvepoint(hpxy(h[0], h[1]));
    if(hi.on_surface != current_surface) {
      queuecurve(T, surface_color(), 0, PPR::LIZEYE);
      current_surface = hi.on_surface;
      curvepoint(hpxy(h[0], h[1]));
      }
    i++; if(i < isize(history)) i = min(i + plan_precision - 1, isize(history)-1);
    ld dist = sqhypot_d(2, h - mousept);
    if(dist < closest_dist) closest_dist = dist, current = history[i];
    }
  queuecurve(T, surface_color(), 0, PPR::LIZEYE);
  vid.linewidth /= 3;
    
  draw_sq(current.where, 0xFF8000FF, PPR::ITEM);
  draw_sq(mousept, 0x8080FFFF, PPR::ITEM);  
  
  quickqueue();

  glflush();
  getcstat = '-';  
  }

hyperpoint mousept_drag;

int move_id = -1, move_dir = 0;

bool level::handle_planning(int sym, int uni) {
  if(sym == PSEUDOKEY_WHEELUP || sym == SDLK_PAGEUP) {
    dynamicval<eGeometry> g(geometry, gEuclid);
    plan_transform.T = atscreenpos(mousex, mousey, 1.2) * inverse(atscreenpos(mousex, mousey, 1)) * plan_transform.T;
    return true;
    }
  if(sym == PSEUDOKEY_WHEELDOWN || sym == SDLK_PAGEDOWN) {
    dynamicval<eGeometry> g(geometry, gEuclid);
    plan_transform.T = atscreenpos(mousex, mousey, 1) * inverse(atscreenpos(mousex, mousey, 1.2)) * plan_transform.T;
    return true;
    }
  for(auto& b: buttons) if(uni == b.first) {
    if(uni == 'l' && planmode == 'l') new_levellines_for[3] = 0;
    planmode = uni; return true;
    }
  auto clean_history_to = [&] (int i) {
    while(history.size() > 1 && history.back().t > i) history.pop_back();
    };
  switch(planmode) {
    case 'p':
      if(uni == '-' && !holdmouse) {
        mousept_drag = mousept;
        holdmouse = true;
        return true;
        }
      else if(uni == '-' && holdmouse) {
        dynamicval<eGeometry> g(geometry, gEuclid);
        if(restored) plan_transform.T = plan_transform.T * eupush(mousept-mousept_drag);
        restored = false;
        return true;
        }
      return false;
    case 'a':
      if(uni == '-' && !holdmouse) {
        plan.emplace_back(mousept, hpxy(0, 0));
        holdmouse = true;
        return true;
        }
      else if(uni == '-' && holdmouse) {
        plan.back().vel = mousept - plan.back().at;
        return true;
        }
      return false;
    case 'm': case 'd': {
      if(!holdmouse) {
        ld len = box * 2;
        move_id = -1;
        auto check = [&] (hyperpoint h, int id, int dir) {
          ld d = sqhypot_d(2, h - mousept);
          if(d < len) { len = d; move_id = id; move_dir = dir; }
          };
        int next_id = 0;
        for(auto p: plan) {
          check(p.at + p.vel, next_id, 1);
          check(p.at - p.vel, next_id, -1);
          check(p.at, next_id, 0);
          next_id++;
          }
        }
      if(uni == '-' && planmode == 'd' && move_id > 0) {
        plan.erase(plan.begin() + move_id);
        clean_history_to(move_id - 1);
        return true;
        }
      else if(uni == '-' && planmode == 'm' && (move_id + move_dir * move_dir > 0) && !holdmouse) {
        holdmouse = true;
        println(hlog, "moving ", tie(move_id, move_dir));
        return true;
        }
      else if(uni == '-' && planmode == 'm' && holdmouse) {
        println(hlog, "moving further ", tie(move_id, move_dir));
        if(move_dir == 0) plan[move_id].at = mousept;
        else plan[move_id].vel = move_dir * (mousept - plan[move_id].at);
        println(hlog, "set to ", tie(plan[move_id].at, plan[move_id].vel));
        clean_history_to(move_id - 1);
        return true;
        }
      return false;
      }
    case 'i': {
      if(uni == '-' && !holdmouse) {
        planpoint pt(C0, C0);
        pt.at = get_spline(current.t);
        pt.vel = hpxy(0, 0);
        plan.insert(plan.begin() + int(ceil(current.t)), pt);
        move_id = int(ceil(current.t));
        holdmouse = true;
        clean_history_to(int(current.t));
        return true;
        }
      else if(uni == '-' && holdmouse) {
        plan[move_id].vel = mousept - plan[move_id].at;
        clean_history_to(move_id - 1);
        }
      return false;
      }
    case 'l': {
      if(uni == '-') {
        new_levellines_for = mousept;
        new_levellines_for[2] = surface(new_levellines_for);
        holdmouse = true;
        return true;
        }
      return false;
      }
    default:
      return false;
    }
  }

}
