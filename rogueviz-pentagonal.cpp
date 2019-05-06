namespace rogueviz {

#if CAP_ARCM
namespace pentagonal {
transmatrix ts[3];

using namespace hyperpoint_vec;

hyperpoint facingdir(array<hyperpoint,3>& a) {
  hyperpoint tmp = (a[1]-a[0]) ^ (a[2]-a[0]);
  tmp /= sqrt(tmp|tmp);
  return tmp;
  }

vector<pair<ld, transmatrix>> sideangles;

cell *p0, *t0, *t1, *t2, *cc;

bool snubon;

hyperpoint cor;

void kframe() {
  if(snubon) {
  queuestr(gmatrix[p0], 0.6, "P0", 0xFFFFFF, 1);
  queuestr(gmatrix[cc], 0.6, "C", 0xFFFFFF, 1);
  queuestr(gmatrix[t0], 0.6, "T0", 0xFFFFFF, 1);
  queuestr(gmatrix[t1], 0.6, "T1", 0xFFFFFF, 1);
  queuestr(gmatrix[t2], 0.6, "T2", 0xFFFFFF, 1);
  }}

hyperpoint xts0;
array<hyperpoint, 3> mts;

rug::rugpoint *pt(hyperpoint h, hyperpoint c, int id) {
  auto r = rug::addRugpoint(C0, -1);
  r->flat = h;
  r->x1 = (1 + c[0]) / 16 + (id/8) / 8.;
  r->y1 = (1 + c[1]) / 16 + (id%8) / 8.;
  r->valid = true;
  return r;
  }

hyperpoint inplane(array<hyperpoint, 3>& a, hyperpoint line) {
  hyperpoint mu = (a[1]-a[0]) ^ (a[2]-a[0]);
  // (a[0] | mu) == (line * z | mu)
  return line * (a[0] | mu) / (line | mu);
  }

transmatrix matrix2;

#if CAP_TEXTURE
bool need_texture = true;
texture::texture_data tdata; // = texture::config.data;
#endif

int global_v, global_w;

void make_texture() {
  #if CAP_TEXTURE
  rug::renderonce = true;
  need_texture = false;
  tdata.whitetexture();
  int tw = tdata.twidth;
  printf("tw = %d\n", tw);
  int fw = tw / 4;
  auto pix = [&] (int k, int x, int y) -> unsigned& {
    return tdata.texture_pixels[y * tw + x + (k&3) * fw + (k>>2) * fw * tw];
    };
  for(int y=0; y<tw; y++)
  for(int x=0; x<tw; x++) 
  for(int p=0; p<3; p++) {
    int ax = x / (tw/8);
    int ay = y / (tw/8);
    int bx = x % (tw/8);
    int by = y % (tw/8);
    int id = ax * 8 + ay;
    hyperpoint h = sideangles[id % isize(sideangles)].second * xts0;
    if(!sphere) {
      hyperpoint ehs[7] = {hpxyz(0,-1,-1), hpxyz(0,0,-1), hpxyz(-1,0,-1), hpxyz(-1,0,0), hpxyz(-1,-1,0), hpxyz(0,-1,0), hpxyz(0,-1,-1)};
      ld idx = (id % global_v) * 6. / global_v;
      h = ehs[int(idx)] * (1-(idx-int(idx))) + ehs[int(idx)+1] * (idx-int(idx));
      }
    ld hyp = hypot(bx-tw/16, by-tw/16) / (tw/16);
    if(hyp > 1) hyp = 1;
    part(pix(0,x,y), p) = 255 * (1 * hyp + (0.5 + h[p]/2) * (1-hyp));
    }
  
  tdata.loadTextureGL();

  rug::alternate_texture = tdata.textureid;
  #endif
  }

void create_model();

void run_snub(int v, int w) {
  snubon = false;
  global_v = v; global_w = w;
  printf("set geometry\n");
  stop_game(); autocheat = true; 
  int bonus;  
  if(w == 4 && v == 4) bonus = 12;
  else if(w == 4 && v == 5) bonus = 7;
  else if(w == 4 && v == 6) bonus = 4;
  else if(w == 3 && v == 6) bonus = 12;
  else if(w == 3 && v == 7) bonus = 8;
  else if(w == 3 && v == 8) bonus = 7;
  else if(w == 3 && v == 9) bonus = 6;
  else bonus = 0;
  gamerange_bonus = genrange_bonus = sightrange_bonus = bonus;
  set_geometry(gArchimedean);
  set_variation(eVariation::pure);
  arcm::current.parse("("+its(v)+",3," + its(w) + ",3,3) (2,3)(1,0)(4)");
  specialland = laCanvas;
  patterns::whichCanvas = 'A';
  // vid.wallmode = 1;
  need_reset_geometry = true;
  printf("start game\n");
  printf("distlimit = %d\n", base_distlimit);
  precalc();
  printf("distlimit = %d\n", base_distlimit);
  start_game();
  printf("ok\n");
  printf("allcells = %d\n", isize(currentmap->allcells()));
  
  sideangles.clear();
  printf("gamerange = %d\n", gamerange());
  printf("genrange = %d\n", getDistLimit() + genrange_bonus);
  setdist(cwt.at, 7 - getDistLimit() - genrange_bonus, NULL);
  bfs();
  
  drawthemap();
  
  if(euclid || sphere) for(cell *c: currentmap->allcells())
    gmatrix[c] = arcm::archimedean_gmatrix[c->master].second;

  cellwalker cw(currentmap->gamestart(), 0);
  p0 = cw.at;
  t0 = (cw - 1 + wstep).at;
  t1 = (cw + wstep).at;
  t2 = (cw + wstep + 1 + wstep - 1).at;
  // p1 = (cw + wstep + 1 + wstep -1 + wstep).at;
  cc = (cw - 1 + wstep - 1 + wstep).at;
  
  transmatrix rel = inverse(gmatrix[p0]);
  
  ts[0] = rel * gmatrix[t0] * ddspin(t0, (cw - 1 + wstep).spin);
  ts[1] = rel * gmatrix[t1];
  ts[2] = rel * gmatrix[t2] * ddspin(t2, (cw + wstep + 1 + wstep - 1).spin);
  
  matrix2 = ts[2] * inverse(ts[0]);
  
  for(int i=0; i<3; i++) mts[i] = ts[i] * C0;
  hyperpoint f = facingdir(mts);

  for(cell *c: currentmap->allcells()) {
    int id = arcm::id_of(c->master);
    if(among(id, 0, 1)) for(int d=0; d<v; d++) {
      transmatrix T = rel * ggmatrix(c) * spin(2*M_PI*d/v);
      array<hyperpoint,3> hts;
      for(int i=0; i<3; i++) 
        hts[i] = T * ts[i] * C0;
      // for(int i=0; i<3; i++) printf("%s ", display(hts[i])); 
      hyperpoint f1 = facingdir(hts);
      
      ld scalar = (f|f1);
      ld alpha = (M_PI - acos(scalar)) / degree;
      sideangles.emplace_back(alpha, T);
      }
    }
    
  vector<double> sav;
  for(auto p: sideangles) sav.push_back(p.first);
  sort(sav.begin(), sav.end());
  
  println(hlog, "sideangles ", sav);
  
  xts0 = tC0(ts[0]);
  
  println(hlog, "original ", xts0);
  
  cor = rel * gmatrix[cc] * C0;

  rug::reopen();
  for(auto p: rug::points) p->valid = true;
  rug::good_shape = true;  

  make_texture();
  
  create_model();
  printf("points = %d tris = %d side = %d\n", isize(rug::points), isize(rug::triangles), isize(sideangles));
  rug::model_distance = euclid ? 4 : 2;
  rug::rug_perspective = hyperbolic;
  showstartmenu = false;
  snubon = true;
  rug::invert_depth = hyperbolic;
  }

void create_model() {

  int v = global_v;
  rug::clear_model(); 
  ld x = (mousex - current_display->xcenter + .0) / vid.xres;
  ld y = (mousey - current_display->ycenter + .0) / vid.yres;
  
  ld alpha = atan2(y, x);
  ld h = hypot(x, y);

  hyperpoint chk = ts[0] * xspinpush0(alpha, h);

  mts[0] = chk;
  mts[1] = spin(-2*M_PI/v) * chk;
  mts[2] = matrix2 * chk;

  hyperpoint c[5];
  for(int i=0; i<5; i++)
    c[i] = hpxy(sin(2 * i * M_PI/5), cos(2 * i * M_PI/5));
    
  hyperpoint tria[5];
  tria[0] = mts[0];
  tria[1] = inplane(mts, C0);
  tria[2] = mts[1];
  tria[3] = mts[2];
  tria[4] = inplane(mts, cor);
  
  hyperpoint ctr = Hypc;
  for(int i=0; i<5; i++) ctr += tria[i];
  ctr = inplane(mts, ctr);
  
  transmatrix tester = spin(1.1) * xpush(1);
  
  int idh = 0;
  
  for(hyperpoint h: {ctr, tria[0], tria[1], tria[2], tria[3], tria[4], ctr}) {
    int good1 = 0, good2 = 0;
    // printf("%d: ", idh);
    for(int i=0; i<5; i++) {
      array<hyperpoint, 3> testplane;
      testplane[0] = tester * h;
      testplane[1] = tester * tria[i];
      testplane[2] = tester * tria[(i+1)%5];
      hyperpoint f = facingdir(testplane);
      // printf("%lf ", f[0]);
      if(f[0] > -1e-6 || std::isnan(f[0])) good1++;
      if(f[0] < +1e-6 || std::isnan(f[0])) good2++;
      }
    // printf("\n");
    if(good1 == 5 || good2 == 5) {ctr = h; break; }
    idh++;
    }
  
  // printf("idh = %d\n", idh);

  transmatrix t = hyperbolic ? rotmatrix(M_PI, 0, 2) * xpush(sin(ticks * M_PI / 8000.)) : rotmatrix(ticks * M_PI / 8000., 0, 2);
  
  hyperpoint hs = hyperbolic ? hpxyz(0,0,-1) : hpxyz(0,0,0);
  
  if(euclid) t = Id;
  
  int id = 0;
  for(auto& p: sideangles) {
    auto& T = p.second;
    array<rug::rugpoint*,5> hts;
    auto cpt = pt(hs + t * T * ctr, C0, id);

    for(int s=0; s<5; s++) 
      hts[s] = pt(hs + t * T * tria[s], c[s], id);

    for(int s=0; s<5; s++) 
      rug::addTriangle(cpt, hts[s], hts[(s+1)%5]);
      
    id++;
    if(!sphere) id %= global_v;
    }
  
  }

#if CAP_COMMANDLINE  
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-snub")) {
    PHASE(3);
    shift();
    run_snub(argi(), 3);
    }
  else if(argis("-snub4")) {
    PHASE(3);
    shift();
    run_snub(argi(), 4);
    }
  else return 1;
  return 0;
  }
#endif

bool frame() { 
  if(snubon && rug::rugged) {
    create_model();
    nomenukey = true;
    displaychr(current_display->xcenter, current_display->ycenter, 0, 10, 'X', 0xFFFFFF);
    clearMessages();
    nohelp = true;
    playerfound = true;
    return true;
    }
  return false;
  }

bool handleKey(int sym, int uni) {
  if(snubon) {
    if(among(uni, '3', '4', '5', '6', '7', '8', '9'))  {
      run_snub(uni - '0', 3);
      return true;
      }
    if(among(uni, '#', '$', '%', '&', '^'))  {
      if(uni == '^') uni = '&';
      run_snub(uni - 32, 4);
      return true;
      }
    if(uni == 'a') {
      rug::close();
      mapeditor::drawplayer = false;
      return true;
      }
    if(uni == 's' && rug::rugged) {
      vid.fov += 30;
      if(vid.fov >= 180) vid.fov = 60;
      }
    if(uni == 'i' && rug::rugged) {
      rug::invert_depth = !rug::invert_depth;
      return true;
      }
    }
  return false;
  }


auto xhook = addHook(hooks_args, 100, readArgs)
+ addHook(hooks_handleKey, 0, handleKey)
+ addHook(hooks_prestats, 0, frame)
+ addHook(clearmemory, 40, [] () { snubon = false; } );
 
}
#endif
}
