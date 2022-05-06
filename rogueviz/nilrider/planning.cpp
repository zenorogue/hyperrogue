namespace nilrider {

hyperpoint get_spline(ld t);

bool level::simulate() {
  loaded_or_planned = true;
  if(history.empty())
    history.push_back(start);
  auto at = history.back();
  
  if(at.t >= isize(plan) - 1.001) return false;
  
  ld goal_t;
  
  if(1) {
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
    }

  hyperpoint h = get_spline(goal_t);
  at.heading_angle = atan2(h[1] - at.where[1], h[0] - at.where[0]);
  history.back() = at;
  
  at.be_consistent();
  if(!at.tick(this)) return false;
  at.t = goal_t;
  history.push_back(at);
  
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
ld closest_t;

char planmode = 'p';
vector<pair<char, string> > buttons = {
  {'p', "pan"}, {'a', "add"}, {'m', "move"}, {'i', "insert"}, {'d', "delete"}
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

void level::draw_planning_screen() {
  if(just_refreshing) return;
  curlev->init_textures();
  dynamicval<eGeometry> g(geometry, gEuclid);
  dynamicval<eModel> pm(pmodel, mdDisk);
  dynamicval<bool> ga(vid.always3, false);
  dynamicval<geometryinfo1> gi(ginf[gEuclid].g, giEuclid2);
  check_cgi();
  cgi.require_shapes();
  curlev->init_shapes();
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
  
  mousept = scr_to_map(hpxy(mousex, mousey));

  box = scr_to_map(hpxy(mousex + 5, mousey))[0] - mousept[0];
  
  /* draw the map */
  auto& p = queuepolyat(T, shPlanFloor, 0xFFFFFFFF, PPR::FLOOR);
  p.tinf = &uniltinf;
  uniltinf.texture_id = unil_texture->textureid;
  
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
  
  /* draw the plan */
  for(auto& pp: plan) {
    draw_sq(pp.at - pp.vel, 0xFF8080FF, PPR::ITEM);
    draw_sq(pp.at + pp.vel, 0x80FF80FF, PPR::ITEM);
    draw_sq(pp.at, 0xFFFF00FF, PPR::ITEM);
    draw_line(pp.at - pp.vel, pp.at + pp.vel, 0x80, PPR::BFLOOR);
    }
  
  bool after = false;
  
  if(history.empty()) history.push_back(start);
  
  closest_t = history.back().t;
  ld closest_dist = box * 2;

  vid.linewidth *= 3;
  int ps = isize(plan);
  for(int t=0; t<=100*(ps-1); t++) {
    ld tt = t / 100.;
    if(tt > history.back().t && !after) {
      queuecurve(T, 0xFFFFFFC0, 0, PPR::LIZEYE);
      after = true;
      }
    hyperpoint h = get_spline(tt);
    curvepoint(hpxy(h[0], h[1]));
    ld dist = sqhypot_d(2, h - mousept);
    if(dist < closest_dist) closest_dist = dist, closest_t = tt;
    }
  queuecurve(T, after ? 0xFF8080C0 : 0xFFFFFFC0, 0, PPR::LIZEYE);
  vid.linewidth /= 3;  
    
  if(!history.empty()) {
    int mint = 0, maxt = isize(history)-1;
    while(mint < maxt) {
      int t = (mint + maxt + 1) / 2;
      if(history[t].t > closest_t) maxt = t-1;
      else mint = t;
      }
      
    current = history[mint];
    }

  draw_sq(get_spline(closest_t), 0x8080FFFF, PPR::ITEM);
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
  for(auto& b: buttons) if(uni == b.first) { planmode = uni; return true; }
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
        plan_transform.T = plan_transform.T * eupush(mousept-mousept_drag);
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
        pt.at = get_spline(closest_t);
        pt.vel = hpxy(0, 0);
        plan.insert(plan.begin() + int(ceil(closest_t)), pt);
        move_id = int(ceil(closest_t));
        holdmouse = true;
        clean_history_to(int(closest_t));
        return true;
        }
      else if(uni == '-' && holdmouse) {
        plan[move_id].vel = mousept - plan[move_id].at;
        clean_history_to(move_id - 1);
        }
      return false;
      }
    default:
      return false;
    }
  }

}
