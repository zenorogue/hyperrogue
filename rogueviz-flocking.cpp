// flocking simulations
// Copyright (C) 2018 Zeno and Tehora Rogue, see 'hyper.cpp' for details

// based on Flocking by Daniel Shiffman (which in turn implements Boids by Craig Reynold)
// https://processing.org/examples/flocking.html

// Our implementation simplifies some equations a bit.

// example parameters: 

// flocking on a torus:
//    -tpar 21,4 -geo 6 -flocking 10 -rvshape 3

// flocking on the Zebra quotient:
//    -geo 4 -flocking 10 -rvshape 3 -zoom .9

// press 'o' when flocking active to change the parameters.

namespace rogueviz {

namespace flocking {

  int N;
  
  bool draw_lines = false;
  
  map<cell*, map<cell*, transmatrix>> relmatrices;

  ld ini_speed = .5;
  ld max_speed = 1;

  ld sep_factor = 1.5;
  ld sep_range = .25;

  ld align_factor = 1;
  ld align_range = .5;
  
  ld coh_factor = 1;
  ld coh_range = .5;
  
  ld check_range = 2.5;
  
  vector<tuple<hyperpoint, hyperpoint, color_t> > lines;
  
  void init() {
    if(!bounded) {
      addMessage("Flocking simulation needs a bounded space.");
      return;
      }
    stop_game();
    rogueviz::init(); kind = kFlocking;
    vdata.resize(N);
    
    const auto v = currentmap->allcells();
    
    printf("computing relmatrices...\n");
    for(cell* c1: v) {
      manual_celllister cl;
      cl.add(c1);
      for(int i=0; i<isize(cl.lst); i++) {
        cell *c2 = cl.lst[i];
        transmatrix T = calc_relative_matrix(c2, c1, C0);
        if(hdist0(tC0(T)) <= check_range) {
          relmatrices[c1][c2] = T;
          forCellEx(c3, c2) cl.add(c3);
          }
        }
      }

    printf("setting up...\n");
    for(int i=0; i<N; i++) {
      vertexdata& vd = vdata[i];
      createViz(i, v[hrand(isize(v))], spin(hrand(100)) * xpush(hrand(100) / 200.));
      vd.name = its(i+1);
      vd.cp = dftcolor;
      vd.cp.color2 = ((hrand(0x1000000) << 8) + 0xFF) | 0x808080FF;
      vd.cp.shade = 'b';
      vd.m->vel = ini_speed;
      }
  
    storeall();
    printf("done\n");
    }
  
  int precision = 10;
  
  void simulate(int delta) {
    while(delta > precision && delta < 100 * precision) { 
      simulate(precision); delta -= precision; 
      }      
    ld d = delta / 1000.;
    using namespace hyperpoint_vec;
    int N = isize(vdata);
    vector<transmatrix> pats(N);
    vector<ld> vels(N);
    using shmup::monster;
    
    map<cell*, vector<monster*>> monsat;

    for(int i=0; i<N; i++) {
      vertexdata& vd = vdata[i];
      auto m = vd.m;
      monsat[m->base].push_back(m);
      }
    
    for(cell *c: currentmap->allcells()) ggmatrix(c);
    
    lines.clear();

    for(int i=0; i<N; i++) {
      vertexdata& vd = vdata[i];
      auto m = vd.m;
      hyperpoint velvec = hpxyz(m->vel, 0, 0);
      
      transmatrix I = inverse(m->at);
      
      // if(i == 0) display(I);

      hyperpoint sep = hpxyz(0, 0, 0);
      int sep_count = 0;

      hyperpoint align = hpxyz(0, 0, 0);
      int align_count = 0;
      
      hyperpoint coh = hpxyz(0, 0, 0);
      int coh_count = 0;
      
      m->findpat();

      for(auto& p: relmatrices[m->base]) {
        for(auto m2: monsat[p.first]) if(m != m2) {
          ld vel2 = m2->vel;
          transmatrix at2 = I * p.second * m2->at;
          hyperpoint ac = tC0(at2);
          ld di = hdist0(ac);
          ld alpha = -atan2(ac);

          color_t col = 0;
            
          if(di < align_range) {
            align += gpushxto0(ac) * at2 * hpxyz(vel2, 0, 0);
            align_count++;
            col = 0xFF00FF;
            }
          
          if(di < check_range) {
            coh += spin(alpha) * hpxyz(di, 0, 0);
            coh_count++;
            }
          
          if(di < sep_range) {
            sep -= spin(alpha) * hpxyz(1 / di, 0, 0), sep_count++;
            col = 0xFF0000FF;
            }
          
          if(col && draw_lines)
            lines.emplace_back(m->pat * C0, m->pat * at2 * C0, col);          
          }
        }
      
      // a bit simpler rules than original
      
      if(sep_count) velvec += sep * (d * sep_factor / sep_count);
      if(align_count) velvec += align * (d * align_factor / align_count);
      if(coh_count) velvec += coh * (d * coh_factor / coh_count);
      
      vels[i] = hypot2(velvec);
      ld alpha = -atan2(velvec);

      if(vels[i] > max_speed) { 
        velvec = velvec * (max_speed / vels[i]);
        vels[i] = max_speed;
        }      
      
      pats[i] = m->pat * spin(alpha) * xpush(vels[i] * d);
      }
    for(int i=0; i<N; i++) {
      vertexdata& vd = vdata[i];
      auto m = vd.m;
      m->rebasePat(pats[i]);
      virtualRebase(m, true);
      m->vel = vels[i];
      }
    shmup::fixStorage();
    }

  bool turn(int delta) {
    if(!on) return false;
    if(kind == kFlocking) simulate(delta), timetowait = 0;
    return false;
    // shmup::pc[0]->rebase();
    }
  
  #if CAP_COMMANDLINE
  int readArgs() {
    using namespace arg;
             
  // options before reading
    if(0) ;
    else if(argis("-flocking")) {
      shift(); N = argi();
      init();
      }
    else if(argis("-cohf")) {
      shift(); coh_factor = argf();
      }
    else if(argis("-alignf")) {
      shift(); align_factor = argf();
      }
    else if(argis("-sepf")) {
      shift(); sep_factor = argf();
      }
    else return 1;
    return 0;
    }
  
  void flock_marker() {
    if(draw_lines)
      for(auto p: lines) queueline(get<0>(p), get<1>(p), get<2>(p), 0);
    }

  void show() {
    cmode = sm::SIDE;
    gamescreen(0);
    dialog::init(XLAT("flocking"), iinf[itPalace].color, 150, 0);
    
    dialog::addSelItem("initial speed", fts(ini_speed), 'i');
    dialog::add_action([]() {
      dialog::editNumber(ini_speed, 0, 2, .1, .5, "", "");
      });
  
    dialog::addSelItem("max speed", fts(max_speed), 'm');
    dialog::add_action([]() {
      dialog::editNumber(max_speed, 0, 2, .1, .5, "", "");
      });

    dialog::addSelItem("separation factor", fts(sep_factor), 's');
    dialog::add_action([]() {
      dialog::editNumber(sep_factor, 0, 2, .1, 1.5, "", "");
      });
  
    dialog::addSelItem("separation range", fts(sep_range), 'S');
    dialog::add_action([]() {
      dialog::editNumber(sep_range, 0, 2, .1, .5, "", "");
      });
  
    dialog::addSelItem("alignment factor", fts(align_factor), 'a');
    dialog::add_action([]() {
      dialog::editNumber(align_factor, 0, 2, .1, 1.5, "", "");
      });
  
    dialog::addSelItem("alignment range", fts(align_range), 'A');
    dialog::add_action([]() {
      dialog::editNumber(align_range, 0, 2, .1, .5, "", "");
      });
  
    dialog::addSelItem("cohesion factor", fts(coh_factor), 'c');
    dialog::add_action([]() {
      dialog::editNumber(coh_factor, 0, 2, .1, 1.5, "", "");
      });
  
    dialog::addSelItem("cohesion range", fts(coh_range), 'C');
    dialog::add_action([]() {
      dialog::editNumber(coh_range, 0, 2, .1, .5, "", "");
      });
  
    dialog::addSelItem("check range", fts(check_range), 't');
    dialog::add_action([]() {
      ld radius = 0;
      for(cell *c: currentmap->allcells())
      for(int i=0; i<c->degree(); i++) {
        hyperpoint h = nearcorner(c, i);
        radius = max(radius, hdist0(h));
        }
      dialog::editNumber(check_range, 0, 2, .1, .5, "", 
        "Value used in the algorithm: "
        "only other boids in cells whose centers are at most 'check range' from the center of the current cell are considered. "
        "Should be more than the other ranges by at least double the cell radius (in the current geometry, double the radius is " + fts(radius*2) + "); "
        "but too large values slow the simulation down."
        );
      });
  
    dialog::addSelItem("number of boids", its(N), 'n');
    dialog::add_action([]() {
      dialog::editNumber(N, 0, 1000, 1, 20, "", "");
      });

    dialog::addSelItem("precision", its(precision), 'p');
    dialog::add_action([]() {
      dialog::editNumber(N, 0, 1000, 1, 10, "", "smaller number = more precise simulation");
      });

    dialog::addSelItem("change geometry", XLAT(ginf[geometry].name), 'g');
    hr::showquotients = true;
    dialog::add_action(runGeometryExperiments);

    dialog::addBoolItem("draw forces", draw_lines, 'l');
    dialog::add_action([] () { draw_lines = !draw_lines; });
  
    dialog::addBreak(100);

    dialog::addItem("restart", 'r');
    dialog::add_action(init);

    dialog::addBack();
    dialog::display();
    }
    
  named_functionality o_key() {
    if(kind == kFlocking) return named_dialog("flocking", show);
    return named_functionality();
    }

  auto hooks  = 
    addHook(hooks_args, 100, readArgs) +
    addHook(shmup::hooks_turn, 100, turn) + 
    addHook(hooks_frame, 100, flock_marker) +
    addHook(hooks_o_key, 80, o_key) +
    0;
  #endif

  }

}
