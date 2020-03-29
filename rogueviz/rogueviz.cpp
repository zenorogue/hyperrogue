// Hyperbolic Rogue
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

// Hyperbolic geometry is a good tool to visualize data,
// especially trees and scale-free networks. This file
// uses HyperRogue to browse such vizualizations.

// Since it is not of use for general HyperRogue players, 
// it is disabled by default -- compile with the ROGUEVIZ flag to enable this.

// How to use:

// hyper -embed <start of filename> -- visualize a social network
//   embedded into hyperbolic plane, see:
//   https://bitbucket.org/HaiZhung/hyperbolic-embedder/overview
//   (it uses the same format)

// hyper -tess <parameter file> -- visualize a horocyclic tesselation,

#include "../hyper.h"
#include "rogueviz.h"

namespace rogueviz {

ld fat_edges = 0;
ld ggamma = 1;

using namespace hr;

edgetype default_edgetype = { .1, .1, .1, DEFAULT_COLOR, 0xFF0000FF, "default" };

bool showlabels = false;
bool specialmark = false;
bool edge_legend = false;

bool rog3 = false;
int vertex_shape = 1;

string edgename;
string fname;

// const char *fname;
// const char *cfname;

vector<shared_ptr<edgetype>> edgetypes;

edgetype *add_edgetype(const string& name) {
  auto e = make_shared<edgetype> (default_edgetype);
  e->name = name;
  edgetypes.push_back(e);
  return &*e;
  }

map<color_t, array<color_t, 16> > next_hue;

color_t parse1(const string& s) {
  // color can be given as RRGGBB
  // or as 'Rmax,min,alpha,step,start', for rainbow Collatz
  if(s[0] == 'R') {
    color_t mh = 192, minh = 0, alpha = 255;
    int step = 50, start = 0;
    sscanf(s.c_str(), "R%x,%x,%x,%d,%d", &mh, &minh, &alpha, &step, &start);
    vector<color_t> hues;
    color_t difh = mh - minh;
    color_t base = alpha + minh * 0x1010100;

    for(unsigned y=0; y<difh; y++)
      hues.push_back(base + 0x1000000*mh + 0x10000 * y);
    for(unsigned y=0; y<difh; y++)
      hues.push_back(base + 0x1010000*mh - 0x1000000 * y);
    for(unsigned y=0; y<difh; y++)
      hues.push_back(base + 0x0010000*mh + 0x100 * y);
    for(unsigned y=0; y<difh; y++)
      hues.push_back(base + 0x0010100*mh - 0x10000 * y);
    for(unsigned y=0; y<difh; y++)
      hues.push_back(base + 0x0000100*mh + 0x1000000 * y);
    for(unsigned y=0; y<difh; y++)
      hues.push_back(base + 0x1000100*mh - 0x100 * y);

    for(int t=0; t<isize(hues); t++)
      for(int a=0; a<16; a++)
        next_hue[hues[t]][a] = hues[gmod(t + rand() % step - rand() % step, isize(hues))];
      
    return hues[gmod(start, isize(hues))];
    }
  else {
    color_t res;
    sscanf(s.c_str(), "%x", &res);
    return res;
    }
  }


color_t perturb(color_t c) {
  if(!next_hue.count(c)) return c;
  return next_hue[c][rand() % 16];
  }

colorpair perturb(colorpair cp) {
  cp.color1 = perturb(cp.color1);
  cp.color2 = perturb(cp.color2);
  return cp;
  }

colorpair parse(string s) {
  colorpair cp;
  auto pospng = s.find(":PNG:");
  if(pospng != string::npos) {
    string fname = s.substr(pospng+5);
    s = s.substr(0, pospng);
    #if CAP_TEXTURE
    cp.img = make_shared<rvimage>();
    auto& i = *cp.img;
    i.tdata.twidth = 1024;
    if(!(i.tdata.readtexture(fname) && i.tdata.loadTextureGL())) {
      println(hlog, "failed to load: ", fname);
      cp.img = NULL;
      return cp;
      }
    println(hlog, "loaded texture: ", fname);
    for(int x=0; x<16; x++)
    for(int y=0; y<16; y++) {
      auto addv = [&] (ld x, ld y) {
        x -= 8;
        y -= 8;
        x /= 16.;
        y /= 16.;
        ld r = max(abs(x), abs(y)) / hypot(x, y);
        if(x || y) {
          x *= r;
          y *= r;      
          }
        i.tinf.tvertices.push_back(glhr::makevertex(x + .5, y + .5, 0));
        i.vertices.push_back(hpxy(x * .4, y * .4));
        };
      addv(x, y);
      addv(x, y+1);
      addv(x+1, y);
      addv(x, y+1);
      addv(x+1, y);
      addv(x+1, y+1);
      }
    i.tinf.texture_id = i.tdata.textureid;
    #endif
    }
  auto pos = s.find(":");
  if(pos != string::npos) {
    cp.color1 = parse1(s.substr(0, pos));
    cp.shade = s[pos+1];
    cp.color2 = parse1(s.substr(pos+2));
    }
  else {
    cp.shade = 0; cp.color2 = 0;
    cp.color1 = parse1(s);
    }
  return cp;
  }

vector<vertexdata> vdata;

transmatrix cspin(int i, int ch) {
  return spin(M_PI + (2 * M_PI * (i+1)) / (ch+1));
  }

map<string, int> labeler;

int getid(const string& s) {
  if(labeler.count(s)) return labeler[s];
  else {
    int id = isize(vdata);
    vdata.resize(isize(vdata) + 1);
    vdata[id].name = s;
    return labeler[s] = id;
    }
  }

int getnewid(string s) {
  while(labeler.count(s)) s += "'";
  return getid(s);
  }
  
void addedge0(int i, int j, edgeinfo *ei) {
  vdata[i].edges.push_back(make_pair(j, ei));
  vdata[j].edges.push_back(make_pair(i, ei));
  }

void createViz(int id, cell *c, transmatrix at) {
  vertexdata& vd(vdata[id]);
  vd.m = new shmup::monster;
  vd.m->pid = id;
  vd.m->type = moRogueviz;
  vd.m->base = c;
  vd.m->at = at;
  vd.m->isVirtual = false;
  }

void notimpl() {
  printf("Not implemented\n"); exit(1); 
  }

hyperpoint where(int i, cell *base) {
  auto m = vdata[i].m;
  if(m->base == base) return tC0(m->at);
  else if(confusingGeometry()) {
    return calc_relative_matrix(m->base, base, C0) * tC0(m->at);
    }
  else {
    // notimpl(); // actually probably that's a buug
    return inverse(ggmatrix(currentmap->gamestart())) * (ggmatrix(m->base) * tC0(m->at));
    }
  }

void addedge(int i, int j, edgeinfo *ei) {
  cell *base = 
    confusingGeometry() ? vdata[i].m->base : currentmap->gamestart();
  hyperpoint hi = where(i, base);
  hyperpoint hj = where(j, base);
  double d = hdist(hi, hj);
  if(d >= 4) {
    hyperpoint h = mid(hi, hj);
    int id = isize(vdata);
    vdata.resize(id+1);
    vertexdata& vd(vdata[id]);
    vd.cp = colorpair(0x400000FF);
    vd.virt = ei;
    
    createViz(id, base, rgpushxto0(h));
    vd.m->no_targetting = true;
    
    addedge(i, id, ei);
    addedge(id, j, ei);
    virtualRebase(vdata[i].m);
    }
  else addedge0(i, j, ei);
  }

vector<edgeinfo*> edgeinfos;

void addedge(int i, int j, double wei, bool subdiv, edgetype *t) {
  edgeinfo *ei = new edgeinfo(t);
  edgeinfos.push_back(ei);
  ei->i = i;
  ei->j = j;
  ei->weight = wei;
  if(subdiv) addedge(i, j, ei);
  else addedge0(i, j, ei);
  }

void storeall(int from) {
  for(int i=from; i<isize(vdata); i++)
    if(vdata[i].m)
      vdata[i].m->store();
  }

colorpair dftcolor = 0x282828FF;

namespace spiral {

  int spiral_id;

  ld mul;
  
  transmatrix at(double d) {
    return spin(log(d) * 2 * M_PI / log(mul)) * xpush(log(d));
    }

  void place(int N, ld _mul) {
    mul = _mul;
    init(&spiral_id, RV_GRAPH | RV_HAVE_WEIGHT | RV_INVERSE_WEIGHT);
    weight_label = "extent";
    vdata.resize(N);
  
    for(int i=0; i<N; i++) {
      vertexdata& vd = vdata[i];
      
      double d = i + 1;
      
      transmatrix h = at(d);
  
      createViz(i, cwt.at, h);
      vd.name = its(i+1);
      virtualRebase(vd.m);
  
      vd.cp = dftcolor;
      }
  
    storeall();
    }
  
  void edge(ld shift, ld mul) {
    int N = isize(vdata);
    auto t = add_edgetype(fts(shift)+" " + fts(mul));
    t->visible_from = 1. / (N+.5);
    for(int i=0; i<N; i++) {
      int i0 = i+1;
      int j0 = int(i0 * mul + shift) - 1;
      if(j0 >= 0 && j0 < N) addedge(i, j0, 1/(i+1), false, t);
      }
    }
  
  void color(ld start, ld period, colorpair c) {
    int N = isize(vdata);
    int maxw = N;
    while(start >= 0 && start < N) {
      int i = int(start);
      vdata[i].cp = c;
      start += period;
      maxw--; if(maxw <= 0) break;
      }
    }
  }

int readLabel(fhstream& f) {
  string s = scan<string>(f);
  if(s == "") return -1;
  return getid(s);
  }

namespace anygraph {
  double R, alpha, T;
  vector<pair<double, double> > coords;
  
  edgetype *any;
  
  int vzid;
  
  int N;
               
  void fixedges() {
    for(int i=N; i<isize(vdata); i++) if(vdata[i].m) vdata[i].m->dead = true;
    for(int i=0; i<isize(vdata); i++) vdata[i].edges.clear();
    vdata.resize(N);
    for(auto e: edgeinfos) {
      e->orig = NULL;
      addedge(e->i, e->j, e);
      }
    }
  
  void tst() {}

  void read(string fn, bool subdiv, bool doRebase, bool doStore) {
    init(&vzid, RV_GRAPH);
    any = add_edgetype("embedded edges");
    fname = fn;
    fhstream f(fn + "-coordinates.txt", "rt");
    if(!f.f) {
      printf("Missing file: %s-coordinates.txt\n", fname.c_str());
      exit(1);
      }
    printf("Reading coordinates...\n");
    string ignore;
    if(!scan(f, ignore, ignore, ignore, ignore, N, anygraph::R, anygraph::alpha, anygraph::T)) {
      printf("Error: incorrect format of the first line\n"); exit(1);
      }
    vdata.reserve(N);
    while(true) {
      string s = scan<string>(f);
      println(hlog, "s: ", s.c_str());
      if(s == "D11.11") tst();
      if(s == "" || s == "#ROGUEVIZ_ENDOFDATA") break;
      int id = getid(s);
      vertexdata& vd(vdata[id]);
      vd.name = s;
      vd.cp = colorpair(dftcolor);
      
      double r, alpha;
      if(!scan(f, r, alpha)) { printf("Error: incorrect format of r/alpha\n"); exit(1); }
      coords.push_back(make_pair(r, alpha));
  
      transmatrix h = spin(alpha * degree) * xpush(r);
      
      createViz(id, currentmap->gamestart(), h);
      }
    
    fhstream g(fn + "-links.txt", "rt");
    if(!g.f) {
      println(hlog, "Missing file: ", fname, "-links.txt");
      exit(1);
      }
    println(hlog, "Reading links...");
    int qlink = 0;
    while(true) {
      int i = readLabel(g), j = readLabel(g);
      if(i == -1 || j == -1) break;
      addedge(i, j, 1, subdiv, any);
      qlink++;
      }
  
    if(doRebase) {
      printf("Rebasing...\n");
      for(int i=0; i<isize(vdata); i++) {
        if(i % 10000 == 0) printf("%d/%d\n", i, isize(vdata));
        if(vdata[i].m) virtualRebase(vdata[i].m);
        }
      printf("Done.\n");
      }
    
    if(doStore) storeall();
    }
  
  }

ld maxweight;

bool edgecmp(edgeinfo *e1, edgeinfo *e2) {
  return e1->weight > e2->weight;
  }

bool which_weight = false;

void rogueviz_help(int id, int pagenumber) {

  vertexdata& vd = vdata[id];
  int noedges = isize(vd.edges);
  help = helptitle(vd.name, vd.cp.color1 >> 8);

  if(vd.info) {
    #if CAP_URL
    help_extension hex;
    hex.key = 'L';
    hex.text = "open link";
    hex.subtext = *vd.info;
    hex.action = [&vd] () { open_url(*vd.info); };
    help_extensions.push_back(hex);
    #else
    help += "\n\nlink: " + *vd.info;
    #endif
    }
  
  vector<edgeinfo*> alledges;
  
  for(int j=0; j<isize(vd.edges); j++) 
    alledges.push_back(vd.edges[j].second);
  
  sort(alledges.begin(), alledges.end(), edgecmp);

  for(int i=0; i<10 && i+pagenumber < noedges; i++) {
    help_extension hex;
    hex.key = 'a' + i;

    edgeinfo *ei = alledges[pagenumber + i];
    if(ei->weight < ei->type->visible_from_help) continue;
    int k = ei->i ^ ei->j ^ id;
    hex.text = vdata[k].name;
    hex.color = vdata[k].cp.color1 >> 8;
    if(vizflags & RV_WHICHWEIGHT) {
      if(which_weight)
        hex.subtext = fts(ei->weight2);
      else
        hex.subtext = fts(ei->weight);
      }

    hex.action = [k] () { help_extensions.clear(); rogueviz_help(k, 0); };
    help_extensions.push_back(hex);
    }

  if(noedges > pagenumber + 10) {
    help_extension hex;
    hex.key = 'z';
    hex.text = "next page";
    hex.subtext = its(pagenumber+10) + "/" + its(noedges) + " edges";
    hex.action = [id, pagenumber] () { help_extensions.clear(); rogueviz_help(id, pagenumber + 10); };
    help_extensions.push_back(hex);
    }
  
  if((vizflags & RV_WHICHWEIGHT) && noedges) {
    help_extension hex;
    hex.key = 'w';
    hex.text = "displayed weight";
    hex.subtext = which_weight ? "attraction force" : "weight from the data";
    hex.action = [id, pagenumber] () { which_weight = !which_weight; help_extensions.clear(); rogueviz_help(id, pagenumber); };
    help_extensions.push_back(hex);
    }
  }

bool describe_monster(shmup::monster *m, string& out) {

  if(m->type != moRogueviz) return false;
   
  int i = m->pid;
  vertexdata& vd = vdata[i];

  string o = vd.name + ", "+its(isize(vd.edges))+" edges";
  /* if(isize(vd.edges) < 10) {
    for(int i=0; i<isize(vd.edges); i++) 
      o += " " + its(snakedist(vd.snakeid, vd.edges[i]->snakeid));
    } */
  
  help = bygen([i] () { rogueviz_help(i, 0); });
  
  if(out == XLATN("Canvas")) out = o;
  else out = out + ", " + o;

  return true;
  }

bool activate(shmup::monster *m) {
  if(m->type != moRogueviz) return false;
  int i = m->pid;
  vertexdata& vd = vdata[i];

  vd.cp = colorpair(rand() & 0xFFFFFFFF);
  
  for(int i=0; i<isize(vd.edges); i++) 
      vd.edges[i].second->orig = NULL;
  
  return true;
  
  /* if(ealpha == 1) ealpha = 8;
  else if(ealpha == 8) ealpha = 32;
  else if(ealpha == 32) ealpha = 255;
  else ealpha = 1; */
  }

void storevertex(vector<glvertex>& tab, const hyperpoint& h) {
  tab.push_back(glhr::pointtogl(h));
  }

double linequality = .1;

void storelineto(vector<glvertex>& tab, const hyperpoint& h1, const hyperpoint& h2) {
  if(intval(h1, h2) < linequality)
    storevertex(tab, h2);
  else {
    hyperpoint h3 = mid(h1, h2);
    storelineto(tab, h1, h3);
    storelineto(tab, h3, h2);
    }
  }

void storeline(vector<glvertex>& tab, const hyperpoint& h1, const hyperpoint& h2) {
  storevertex(tab, h1);
  storelineto(tab, h1, h2);
  }

color_t darken_a(color_t c) {
  for(int p=0; p<3; p++)
  for(int i=0; i<darken; i++) part(c, i+1) = (part(c, i+1) + part(backcolor, i)) >> 1;
  return c;
  }

#if CAP_SVG
#define SVG_LINK(x) svg::link = (x)
#else
#define SVG_LINK(x) 
#endif

void queuedisk(const transmatrix& V, const colorpair& cp, bool legend, const string* info, int i) {
  if(legend && (int) cp.color1 == (int) 0x000000FF && backcolor == 0)
    poly_outline = 0x606060FF;
  else
    poly_outline = (bordcolor << 8) | 0xFF;
  
  if(cp.img) {
    for(hyperpoint h: cp.img->vertices)
      curvepoint(V * h);
    auto& qc = queuecurve(0, 0xFFFFFFFF, PPR::MONSTER_HEAD);
    qc.tinf = &cp.img->tinf;
    qc.flags |= POLY_TRIANGLES;
    return;
    }
    
  transmatrix V1;
  
  auto& sh = 
    vertex_shape == 2 ? cgi.shHeptaMarker :
    vertex_shape == 3 ? cgi.shSnowball :
    cgi.shDisk;
  
  if(vertex_shape == 0) ;
  else if(GDIM == 3 && among(cp.shade, 'b', 'f', 'g', 'B', 'F', 'G')) {
    V1 = V;
    }
  else if(GDIM == 3) {
    V1 = face_the_player(V);
    if(info) queueaction(PPR::MONSTER_HEAD, [info] () { SVG_LINK(*info); });
    queuepolyat(V1, sh, darken_a(cp.color1), PPR::MONSTER_HEAD);
    if(info) queueaction(PPR::MONSTER_HEAD, [] () { SVG_LINK(""); });
    V1 = V;
    }
  else if(rog3) {
    int p = poly_outline; poly_outline = OUTLINE_TRANS; 
    queuepolyat(V, sh, 0x80, PPR::MONSTER_SHADOW); 
    poly_outline = p; 
    if(info) queueaction(PPR::MONSTER_HEAD, [info] () { SVG_LINK(*info); });
    queuepolyat(V1 = mscale(V, cgi.BODY), sh, darken_a(cp.color1), PPR::MONSTER_HEAD);
    if(info) queueaction(PPR::MONSTER_HEAD, [] () { SVG_LINK(""); });
    }
  else {
    if(info) queueaction(PPR::MONSTER_HEAD, [info] () { SVG_LINK(*info); });
    queuepoly(V1 = V, sh, darken_a(cp.color1));
    if(info) queueaction(PPR::MONSTER_HEAD, [] () { SVG_LINK(""); });
    }
  switch(cp.shade) {
    case 't': queuepoly(V1, cgi.shDiskT, darken_a(cp.color2)); return;
    case 's': queuepoly(V1, cgi.shDiskS, darken_a(cp.color2)); return;
    case 'q': queuepoly(V1, cgi.shDiskSq, darken_a(cp.color2)); return;
    case 'm': queuepoly(V1, cgi.shDiskM, darken_a(cp.color2)); return;
    case 'b': queuepoly(V1, GDIM == 3 ? cgi.shAnimatedTinyEagle[wingphase(200)] : cgi.shTinyBird, darken_a(cp.color2)); return;
    case 'f': queuepoly(V1, cgi.shTinyShark, darken_a(cp.color2)); return;
    case 'g': queuepoly(V1, cgi.shMiniGhost, darken_a(cp.color2)); return;
    case 'B': queuepoly(V1, GDIM == 3 ? cgi.shAnimatedEagle[wingphase(100)] : cgi.shEagle, darken_a(cp.color2)); return;
    case 'F': queuepoly(V1, cgi.shShark, darken_a(cp.color2)); return;
    case 'G': queuepoly(V1, cgi.shGhost, darken_a(cp.color2)); return;
    }
  }

unordered_map<pair<edgeinfo*, int>, int> drawn_edges;

map<pair<cell*, cell*>, transmatrix> relmatrices;

transmatrix& memo_relative_matrix(cell *c1, cell *c2) {
  auto& p = relmatrices[make_pair(c1, c2)];
  if(p[2][2] == 0)
    p = calc_relative_matrix(c1, c2,  C0);
  return p;
  }

void queue_prec(const transmatrix& V, edgeinfo*& ei, color_t col) {
  if(!fat_edges)
    queuetable(V, ei->prec, isize(ei->prec), col, 0, PPR::STRUCT0);
  #if MAXMDIM >= 4
  else {
    auto& t = queuetable(V, ei->prec, isize(ei->prec), 0, col | 0x000000FF, PPR::STRUCT0);
    t.flags |= (1<<22), // poly triangles
    t.offset_texture = 0,
    t.tinf = &ei->tinf;
    t.tinf->texture_id = floor_textures->renderedTexture;
    }
  #endif
  }

bool drawVertex(const transmatrix &V, cell *c, shmup::monster *m) {
  if(m->dead) return true;
  if(m->type != moRogueviz) return false;
  int i = m->pid;
  vertexdata& vd = vdata[i];
  
  // bool ghilite = false;
  
  // if(vd.special && specialmark) ghilite = true;
  
  if(!gmatrix.count(m->base)) printf("base not in gmatrix\n");

  int lid = shmup::lmousetarget ? shmup::lmousetarget->pid : -2;
  
  if(!leftclick) for(int j=0; j<isize(vd.edges); j++) {
    edgeinfo *ei = vd.edges[j].second;
    vertexdata& vd1 = vdata[ei->i];
    vertexdata& vd2 = vdata[ei->j];

    int oi = ei->i, oj = ei->j;
    bool hilite = false;
    if(vdata[oi].special && vdata[oj].special && specialmark) hilite = true;
    else if(svg::in || inHighQual) hilite = false;
    else if(vd1.m == shmup::lmousetarget) hilite = true;
    else if(vd2.m == shmup::lmousetarget) hilite = true;
    else if(oi == lid || oj == lid) hilite = true;

    if(ei->weight < (hilite ? ei->type->visible_from_hi : ei->type->visible_from)) continue;

    // if(hilite) ghilite = true;
    
    bool multidraw = quotient;
        
    if(ei->lastdraw < frameid || multidraw) { 
      ei->lastdraw = frameid;
      
      color_t col = (hilite ? ei->type->color_hi : ei->type->color);
      auto& alpha = part(col, 0);
      
      if(vizflags & RV_AUTO_MAXWEIGHT) {
        if(ei->weight2 > maxweight) maxweight = ei->weight2;
        alpha *= pow(ei->weight2 / maxweight, ggamma);
        }
      // if(hilite || hiliteclick) alpha = (alpha + 256) / 2;
      
      if(svg::in && alpha < 16) continue;
      
      if(ISWEB) {
        if(alpha >= 128) alpha |= 15;
        else if(alpha >= 64) alpha |= 7;
        else if(alpha >= 32) alpha |= 3;
        else if(alpha >= 16) alpha |= 1;
        }
      
      alpha >>= darken;

      transmatrix gm1 = 
        (multidraw || elliptic) ? V * memo_relative_matrix(vd1.m->base, c) :
        ggmatrix(vd1.m->base);
      transmatrix gm2 = 
        (multidraw || elliptic) ? V * memo_relative_matrix(vd2.m->base, c) :
        ggmatrix(vd2.m->base);
                
      hyperpoint h1 = gm1 * vd1.m->at * C0;
      hyperpoint h2 = gm2 * vd2.m->at * C0;
      
      if(elliptic && intval(h1, h2) > intval(h1, centralsym * h2))
        h2 = centralsym * h2;
      
      if(multidraw) {
        int code = int(h1[0]) + int(h1[1]) * 12789117 + int(h2[0]) * 126081253 + int(h2[1]) * 126891531;
        int& lastdraw = drawn_edges[make_pair(ei, code)];
        if(lastdraw == frameid) continue;
        lastdraw = frameid;
        }

      /* if(hdist0(h1) < .001 || hdist0(h2) < .001) {
        printf("h1 = %s\n", display(h1));
        printf("h2 = %s\n", display(h2));
        display(m->at);
        display(vd2.m->at);
        display(V);
        display(gmatrix[vd2.m->base]);
        display(shmup::calc_gmatrix(vd2.m->base));
        } */
      
      if((col >> 8) == (DEFAULT_COLOR >> 8)) {
        col &= 0xFF;
        col |= (forecolor << 8);
        }

      bool onspiral = (vizid == &spiral::spiral_id) && abs(ei->i - ei->j) == 1;

      if((pmodel || onspiral) && !fat_edges) {
        if(onspiral) {
          const int prec = 20; 
          transmatrix T = ggmatrix(currentmap->gamestart());
          hyperpoint l1 = T*tC0(spiral::at(1+ei->i));
          for(int z=1; z<=prec; z++) {
            hyperpoint l2 = T*tC0(spiral::at(1+ei->i+(ei->j-ei->i) * z / (prec+.0)));
            queueline(l1, l2, col, vid.linequality).prio = PPR::STRUCT0;
            l1 = l2;
            }
          }
        else {
          queueline(h1, h2, col, 2 + vid.linequality).prio = PPR::STRUCT0;
          }
        }
      else {
      
        cell *center = multidraw ? c : centerover;
      
        if(!multidraw && ei->orig && ei->orig != center && celldistance(ei->orig, center) > 3) 
          ei->orig = NULL;
        if(!ei->orig) {
          ei->orig = center; // cwt.at;
          ei->prec.clear();
          
          transmatrix T = inverse(ggmatrix(ei->orig));
          
          if(fat_edges) {
            ei->tinf.tvertices.clear();
            transmatrix T1 = inverse(gm1 * vd1.m->at);
            hyperpoint goal = T1 * h2;
            transmatrix S = T * gm1 * vd1.m->at * rspintox(goal);
            ld d = hdist0(goal);
            for(int a=0; a<360; a+=30) {
              auto store = [&] (ld a, ld b) {
                storevertex(ei->prec, S * cpush(0, b) * hr::cspin(1, 2, a * degree) * cpush(1, fat_edges) * C0);
                ei->tinf.tvertices.push_back(glhr::makevertex(0,(3+cos(a * degree))/4,0));
                };
              store(a, 0);
              store(a+30, 0);
              store(a, d);
              store(a+30, 0);
              store(a, d);
              store(a+30, d);
              }
            }
          else if((vizid == spiral::place) && abs(ei->i - ei->j) == 1) {
            ei->orig = currentmap->gamestart();
            hyperpoint l1 = tC0(spiral::at(1+ei->i));
            storevertex(ei->prec, l1);
            const int prec = 20; 
            for(int z=1; z<=prec; z++) {
              hyperpoint l2 = tC0(spiral::at(1+ei->i+(ei->j-ei->i) * z / (prec+.0)));
              storeline(ei->prec, l1, l2);
              l1 = l2;
              }
            }
          else 
            storeline(ei->prec, T*h1, T*h2);
          }
        queue_prec(multidraw ? V : ggmatrix(ei->orig), ei, col);
        if(elliptic) queue_prec(centralsym * ggmatrix(ei->orig), ei, col);
        }
      }
/*
    */
    }

  if(!vd.virt) {
    queuedisk(V * m->at, vd.cp, false, vd.info, i);
    }
  
  
  if(showlabels) {
    bool doshow = true;
    if((vizflags & RV_COMPRESS_LABELS) && i > 0 && !vd.virt) {
      vertexdata& vdp = vdata[vd.data];
      hyperpoint h2 = ggmatrix(vdp.m->base) * vdp.m->at * C0;
      if(hdist(h2, V * m->at * C0) < 0.1) doshow = false;
      }
    
    hyperpoint h = tC0(V * m->at);
    transmatrix V2 = GDIM == 3 ? V * m->at : rgpushxto0(h) * ypush(PURE ? .3 : .2); // todo-variation
    if(doshow && !behindsphere(V2)) {
      auto info = vd.info;
      if(info) queueaction(PPR::MONSTER_HEAD, [info] () { SVG_LINK(*info); });
      queuestr(V2, (svg::in ? .28 : .2) * cgi.crossf / cgi.hcrossf, vd.name, forecolor, (svg::in || ISWEB) ? 0 : 1);
      if(info) queueaction(PPR::MONSTER_HEAD, [] () { SVG_LINK(""); });
      }
    }

  callhooks(hooks_drawvertex, vd, c, m, i);

  return true;
  }

vector<int> legend;

vector<cell*> named;

color_t chosen_legend_color = DEFAULT_COLOR;

bool rogueviz_hud() {
  color_t legend_color = chosen_legend_color == DEFAULT_COLOR ? forecolor : chosen_legend_color;
  if(!vizid) return false;
  if(cmode & sm::DRAW) return false;

  int qet = isize(edgetypes);
  if(qet == 1 || !edge_legend) qet = 0;

  int legit = qet + isize(legend);
  
  if(legit == 0) return true;

  initquickqueue();
  
  int rad = current_display->radius/10;
  ld x = vid.xres - rad;

  for(int i=0; i<isize(legend); i++) {
    int k = legend[i];
    vertexdata& vd = vdata[k];
    
    ld y = (current_display->radius * (i+.5)) / legit * 2 - current_display->radius + vid.yres/2;

    transmatrix V = atscreenpos(x, y, current_display->radius/4);
    
    poly_outline = OUTLINE_NONE;
    queuedisk(V, vd.cp, true, NULL, i);
    poly_outline = OUTLINE_DEFAULT;
    queuestr(int(x-rad), int(y), 0, rad*(svg::in?5:3)/4, vd.name, legend_color, 0, 16);
    }

  for(int i=0; i<qet; i++) {
    auto t = edgetypes[i];
        
    ld y = (current_display->radius * (i+isize(legend)+.5)) / legit * 2 - current_display->radius + vid.yres/2;

    transmatrix V = atscreenpos(x, y, current_display->radius/8);
    
    poly_outline = t->color | 0xFF;
    queuepolyat(V, cgi.shTriangle, 0, PPR::MONSTER_HEAD);
    
    poly_outline = OUTLINE_DEFAULT;
    queuestr(int(x-rad), int(y), 0, rad*(svg::in?5:3)/4, t->name, legend_color, 0, 16);
    }
  
  quickqueue();
  return true;
  }

inline hookset<bool(int&, string&, FILE*)> *hooks_readcolor;

void readcolor(const string& cfname) {
  FILE *f = fopen(cfname.c_str(), "rt");
  if(!f) { printf("color file missing\n"); exit(1); }
  while(true) {
    string lab = "";
    while(true) {
      int c = fgetc(f);
      if(c == EOF) { fclose(f); return; }
      else if(c == 10 || c == 13 || c == 32 || c == 9) ;
      else if(c == ',' || c == ';') break;
      else lab += c;
      }
    
    colorpair x;
    int c2 = fgetc(f);
    int known_id = -1;
    
    if(callhandlers(false, hooks_readcolor, c2, lab, f)) continue;

    if(c2 == '#') {
      while(c2 != 10 && c2 != 13 && c2 != -1) c2 = fgetc(f);
      continue;
      }
    else if(c2 == '=') {
      string lab2 = "";
      while(true) {
        int c = fgetc(f);
        if(c == 10 || c == 13 || c == 32 || c == 9 || c == ',' || c == ';' || c == EOF) break;
        else lab2 += c;
        }
      x = vdata[getid(lab2)].cp;
      }
    else if(c2 == '@') {
      legend.push_back(known_id == -1 ? getid(lab) : known_id);
      continue;
      }
    else if(c2 == '/') {
      char buf[600];
      int err = fscanf(f, "%500s", buf);
      if(err > 0) 
        vdata[getid(lab)].info = new string(buf); // replace with std::shared_ptr in C++111
      continue;
      }
    else {
      ungetc(c2, f);
      char buf[600];
      int err = fscanf(f, "%500s", buf);
      if(err > 0) x = parse(buf);
      }
    
    if(isize(lab) && lab[0] == '*') {
      lab = lab.substr(1);
      for(int i=0; i<isize(vdata); i++)
        if(vdata[i].name.find(lab) != string::npos) {
          vdata[i].cp = x;
          }
      }
    else if(isize(lab) && lab[0] == '!') {
      for(int i=0; i<isize(vdata); i++)
        if(vdata[i].name == lab) {
          vdata[i].cp = x;
          }
      }
    else {
      int i = getid(lab);
      again: vdata[i].cp = x;
      
      if(vizflags & RV_COLOR_TREE) {
        i = vdata[i].data;
        if(i >= 0) goto again;
        }
      }
    }
  }

void init(void *_vizid, flagtype _vizflags) {

  autocheat = true; 
  showstartmenu = false;
#if !ISWEB
  mapeditor::drawplayer = false;
  stop_game();
  firstland = specialland = laCanvas;
  restart_game(shmup::on ? rg::shmup : rg::nothing);
#else
  stop_game();
  firstland = specialland = laCanvas;
  restart_game(rg::nothing);
#endif
  autocheat = true;
  items[itOrbLife] = 0;
  timerghost = false;

  gmatrix.clear();
  calcparam();
  drawthemap();
  gmatrix0 = gmatrix;

  vizid = _vizid;
  vizflags = _vizflags;
  }

int search_for = -1;

purehookset hooks_close;

void close() { 
  search_for = -1;
  for(int i=0; i<isize(vdata); i++)
    if(vdata[i].m) vdata[i].m->dead = true;
  vdata.clear();
  labeler.clear();
  legend.clear();
  for(int i=0; i<isize(edgeinfos); i++) delete edgeinfos[i];
  edgeinfos.clear();
  anygraph::coords.clear();
  callhooks(hooks_close);
  edgetypes.clear();
  vizid = nullptr;
  relmatrices.clear();
  }

#ifndef CAP_RVSLIDES
#define CAP_RVSLIDES (CAP_TOUR && !ISWEB)
#endif

int dimid(char x) {
  if(x >= 'a' && x < 'a' + GDIM) return x - 'a';
  else if(x >= '0' && x < '0' + GDIM) return x - '0';
  else if(x >= 'x' && x < 'x' + GDIM) return x - 'x';
  else {
    println(hlog, "incorrect dimension ID");
    throw hr_exception();
    }
  }

#if CAP_COMMANDLINE
int readArgs() {
  using namespace arg;
           
// options before reading
  if(0) ;
  else if(argis("-dftcolor")) {
    shift(); dftcolor = parse(args());
    }  

// graph visualizer
//------------------

// this visualizes the data from: https://hpi.de/friedrich/research/hyperbolic

  else if(argis("-graph")) {
    PHASE(3); shift(); anygraph::read(args());
    }
  
// draw spirals 
//--------------

// example commandline:
// -spiral 2,10000 -spiraledge 0,2 -spiraledge 1,1 -lab -spiralcolor 2 FF4040FF

  else if(argis("-spiral")) {
    PHASE(3); 
    ld mul = 2;
    int N = 1000;
    shift(); sscanf(argcs(), LDF ",%d", &mul, &N);
    spiral::place(N, mul);
    }

  else if(argis("-spiraledge")) {
    PHASE(3); 
    ld shft = 1;
    ld mul = 1;
    shift(); sscanf(argcs(), LDF "," LDF, &shft, &mul);
    spiral::edge(shft, mul);
    }

  else if(argis("-spiralcolor")) {
    PHASE(3); 
    ld period = 1;
    ld start = 1;
    shift(); sscanf(argcs(), LDF "," LDF, &period, &start);
    start--;
    shift();
    spiral::color(start, period, parse(args()));
    }

// graphical parameters
//------------------

  // read the color/legend file
  else if(argis("-color")) {
    PHASE(3); shift(); readcolor(args());
    }
  else if(argis("-lab")) {
    showlabels = true;
    }
  else if(argis("-lab-off")) {
    showlabels = false;
    }
  else if(argis("-rog3")) {
    rog3 = true;
    }
  else if(argis("-rvedge")) {
    shift(); default_edgetype.color = default_edgetype.color_hi = arghex();
    }
  else if(argis("-rvedgehi")) {
    shift(); default_edgetype.color_hi = arghex();
    }
  else if(argis("-rvfat")) {
    shift(); 
    fat_edges = argf();
    }
  else if(argis("-lcol")) {
    shift(); 
    chosen_legend_color = arghex();
    }
  else if(argis("-ggamma")) {
    shift(); ggamma = argf();
    }
  else if(argis("-rvwarp")) {
    patterns::whichShape = '8';
    }
  else if(argis("-lq")) {
    shift_arg_formula(linequality);
    }
#if CAP_RVSLIDES
  else if(argis("-rvpres")) {
    tour::slides = rvtour::gen_rvtour();
    }
#endif
  else if(argis("-nolegend")) {
    legend.clear();
    }
  else if(argis("-edgelegend")) {
    edge_legend = true;
    }
  else if(argis("-rvshape")) {
    shift(); vertex_shape = argi() & 3;
    }
  
// graphical output 
//------------------

  // shmup::turn might be necessary when saving screenshots
  else if(argis("-TURN")) {
    PHASE(3); shmup::turn(100);
    }
  else return 1;
  return 0;
  }
#endif

void configure_edge_display() {
  cmode = sm::SIDE | sm::MAYDARK | sm::DIALOG_STRICT_X;
  static int mode = 0;
  gamescreen(0);  
  dialog::init(XLAT("rogueviz edges"));
  for(int i=0; i<isize(edgetypes); i++) {
    auto t = edgetypes[i];
    switch(mode) {
      case 0:
        dialog::addSelItem(t->name, itsh(t->color), 'a' + i);
        dialog::lastItem().colorv = t->color >> 8;
        dialog::add_action([t] {
          dialog::openColorDialog(t->color, NULL);
          dialog::dialogflags |= sm::MAYDARK | sm::SIDE;
          });
        break;
      case 1:
        if(!(vizflags & RV_INVERSE_WEIGHT)) {
          dialog::addSelItem(t->name, fts(t->visible_from), 'a'+i);
          dialog::add_action([t] {
            dialog::editNumber(t->visible_from, 0.001, 1000, .1, .1, "min weight", "");
            dialog::scaleLog();
            });
          }
        else {
          dialog::addSelItem(t->name, its(1 / t->visible_from), 'a'+i);
          dialog::add_action([t] {
            static int i;
            i = 1 / t->visible_from;
            dialog::editNumber(i, 1, 1000000, 1, 500, weight_label, "");
            dialog::reaction = [t] () { t->visible_from = i ? 1. / i : 5; };
            dialog::scaleLog(); dialog::ne.step = .2;
            });
          }
        break;
      default: break;
      }
    }
  dialog::addBreak(100);
  if(vizflags & RV_HAVE_WEIGHT) {
    dialog::addBoolItem_choice("color/alpha", mode, 0, '1');
    dialog::addBoolItem_choice(weight_label, mode, 1, '2');
    }
  else mode = 0;
  
  dialog::addBreak(50);
  dialog::addBack();
  dialog::display();
  }

void search_marker() {
  if(search_for >= 0 && search_for < isize(vdata)) {
    auto& vd = vdata[search_for];
    auto& m = vd.m;
    if(!m) return;
    hyperpoint H = ggmatrix(m->base) * tC0(m->at);
    queuestr(H, 2*vid.fsize, "X", 0x10101 * int(128 + 100 * sin(ticks / 150.)));
    addauraspecial(H, iinf[itOrbYendor].color, 0);
    }
  }

void showVertexSearch() {
  cmode = sm::SIDE | sm::MAYDARK | sm::DIALOG_STRICT_X;
  gamescreen(0); search_for = -1;

  dialog::init(XLAT("vertex search"));
  dialog::v.clear();
  if(dialog::infix != "") mouseovers = dialog::infix;
  
  for(int i=0; i<isize(vdata); i++) if(vdata[i].name != "") dialog::vpush(i, vdata[i].name.c_str());
  
  for(int i=0; i<9; i++) {
    if(i < isize(dialog::v)) {
      int id = dialog::v[i].second;
      dialog::addItem(dialog::v[i].first, '1'+i);
      dialog::add_action([id] () { 
        search_for = id; 
        popScreenAll(); 
        });
      }
    else dialog::addBreak(100);
    }

  dialog::addSelItem("matching items", its(isize(dialog::v)), 0);
  dialog::display();
  
  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);    
    if(dialog::editInfix(uni)) ;
    else if(doexiton(sym, uni)) popScreen();
    };

  }

purehookset hooks_rvmenu;

hookset<bool()> *hooks_rvmenu_replace;

void showMenu() {
  if(callhandlers(false, hooks_rvmenu_replace)) return;
  cmode = sm::SIDE | sm::MAYDARK | sm::DIALOG_STRICT_X;
  gamescreen(0);  

  dialog::init(XLAT("rogueviz configuration"));

  dialog::addBoolItem_action(XLAT("show labels"), showlabels, 'l');
  dialog::addBoolItem_action(XLAT("mark special vertices"), specialmark, 'x');
  dialog::addSelItem(XLAT("background color"), itsh(backcolor), 'b');
  dialog::add_action_push([] {backcolor ^= 0xFFFFFF, bordcolor ^= 0xFFFFFF, forecolor ^= 0xFFFFFF; });
  if(isize(edgetypes)) {
    dialog::addSelItem(XLAT("edge types"), its(isize(edgetypes)), 'g');
    dialog::add_action_push(configure_edge_display);
    }
  dialog::addBoolItem_action(XLAT("vertices in 3D"), rog3, 'v');
  dialog::addSelItem(XLAT("vertex shape"), its(vertex_shape), 'w');
  dialog::add_action_push([] { vertex_shape = (1 + vertex_shape) & 3; });

  dialog::add_key_action('z', [] {
    for(int i=0; i<isize(named)-1; i++) if(named[i] == cwt.at)
      swap(named[i], named[i+1]);
    if(!isize(named) || named[isize(named)-1] != cwt.at) named.push_back(cwt.at);
    printf("named = %d\n", isize(named));
    popScreen();
    });

  dialog::addItem(XLAT("vertex search"), '/');
  dialog::add_action_push(showVertexSearch);
  
  dialog::addBreak(50);

  callhooks(hooks_rvmenu);
  
  dialog::addBreak(50);
  dialog::addBack();

  dialog::display();
  }

#if CAP_RVSLIDES
namespace rvtour {

using namespace tour;

string cname() {
  if(euclid) return "coord-6.txt";
  if(PURE) return "coord-7.txt";
  return "coord-67.txt";
  }

template<class T> function<void(presmode)> roguevizslide(char c, const T& t) {
  return [c,t] (presmode mode) {
    patterns::canvasback = 0x101010;
    setCanvas(mode, c);
    if(mode == 1 || mode == pmGeometryStart) t();
  
    if(mode == 3 || mode == pmGeometry || mode == pmGeometryReset) {
      rogueviz::close();
      shmup::clearMonsters();
      if(mode == pmGeometryReset) t();
      }
  
    slidecommand = "toggle the player";
    if(mode == 4) 
      mapeditor::drawplayer = !mapeditor::drawplayer;
    pd_from = NULL;
    };
  }

template<class T, class U>
function<void(presmode)> roguevizslide_action(char c, const T& t, const U& act) {
  return [c,t,act] (presmode mode) {
    patterns::canvasback = 0x101010;
    setCanvas(mode, c);
    if(mode == pmStart || mode == pmGeometryStart) t();
  
    act(mode);

    if(mode == pmStop || mode == pmGeometry || mode == pmGeometryReset) {
      rogueviz::close();
      shmup::clearMonsters();
      if(mode == pmGeometryReset) t();
      }
  
    };
  }

#define RVPATH HYPERPATH "rogueviz/"

vector<slide> rvslides;
extern vector<slide> rvslides_default;

slide *gen_rvtour() {
  rvslides = rvslides_default;
  callhooks(hooks_build_rvtour, rvslides);
  rvslides.emplace_back(
    slide{"THE END", 99, LEGAL::ANY | FINALSLIDE,
    "Press '5' to leave the presentation.",
    [] (presmode mode) {
      firstland = specialland = laIce;
      if(mode == 4) restart_game(rg::tour);
      }
    });
  return &rvslides[0];
  }

vector<slide> rvslides_default = {
    {"RogueViz", 999, LEGAL::ANY, 
      "This is a presentation of RogueViz, which "
      "is an adaptation of HyperRogue as a visualization tool "
      "rather than a game. Hyperbolic space is great "
      "for visualizing some kinds of data because of the vast amount "
      "of space.\n\n"
      "Press '5' to switch to the standard HyperRogue tutorial. "
      "Press ESC to look at other functions of this presentation."
      ,
      [] (presmode mode) {
        slidecommand = "the standard presentation";
        if(mode == pmStartAll) firstland = specialland = laPalace;
        if(mode == 4) {
          tour::slides = default_slides;
          while(tour::on) restart_game(rg::tour);
          firstland = specialland = laIce;
          tour::start();
          }
        }
      },
    {"straight lines in the Palace", 999, LEGAL::ANY, 
      "One simple slide about HyperRogue. Press '5' to show some hyperbolic straight lines.",
      [] (presmode mode) {
       using namespace linepatterns;
       slidecommand = "toggle the Palace lines";
       if(mode == 4) patPalace.color = 0xFFD500FF;
       if(mode == 3) patPalace.color = 0xFFD50000;
        }
      },
  };

int rvtour_hooks = 
  addHook(hooks_slide, 100, [] (int mode) {
    if(currentslide == 0 && slides == default_slides) {
      slidecommand = "RogueViz presentation";
      if(mode == 1)
        help += 
          "\n\nYour version of HyperRogue is compiled with RogueViz. "
          "Press '5' to switch to the RogueViz slides. Watching the "
          "common HyperRogue tutorial first is useful too, "
          "as an introduction to hyperbolic geometry.";         
      if(mode == 4) {
        slides = gen_rvtour();
        while(tour::on) restart_game(rg::tour);
        tour::start();
        }
      }
    }) +
  0;

}
#endif

bool default_help() {
  if(!vizid) return false;

  help = 
    "This is RogueViz, a visualization engine based on HyperRogue.\n\nUse WASD to move, v for menu.\n\n"
    "Read more about RogueViz on : http://roguetemple.com/z/hyper/rogueviz.php\n\n";

  help_extensions.push_back(help_extension{'u', XLAT("RogueViz menu"), [] () { popScreen(); pushScreen(showMenu); }});    
  return true;
  }

named_functionality o_key() {
  if(vizid) return named_dialog(XLAT("rogueviz menu"), rogueviz::showMenu);
  return named_functionality();
  }

auto hooks  = 
#if CAP_COMMANDLINE
  addHook(hooks_args, 100, readArgs) +
#endif
  addHook(clearmemory, 0, close) +
  addHook(hooks_prestats, 100, rogueviz_hud) +
  addHook(shmup::hooks_draw, 100, drawVertex) +
  addHook(shmup::hooks_describe, 100, describe_monster) +
  addHook(shmup::hooks_kill, 100, activate) +
  addHook(hooks_o_key, 100, o_key) +
  
  addHook(tour::ss::extra_slideshows, 100, [] (bool view) {
    if(!view) return 1;
    dialog::addBoolItem(XLAT("RogueViz Tour"), tour::ss::wts == &rvtour::rvslides[0], 'r');
    dialog::add_action([] { tour::ss::wts = rvtour::gen_rvtour(); popScreen(); });    
    return 0;
    }) +
  
  addHook(dialog::hooks_display_dialog, 100, [] () {
    if(current_screen_cfunction() == showMainMenu) {
      dialog::addItem(XLAT("rogueviz menu"), 'u'); 
      dialog::add_action_push(rogueviz::showMenu);
      }
    #if CAP_RVSLIDES
    if(current_screen_cfunction() == showStartMenu) {
      dialog::addBreak(100);
      dialog::addBigItem(XLAT("RogueViz"), 'r');
      dialog::add_action([] () {        
        tour::slides = rogueviz::rvtour::gen_rvtour();
        popScreenAll();
        tour::start();
        printf("tour start\n");
        });
      dialog::addInfo(XLAT("see the visualizations"));
      }
    #endif
    }) +
  addHook(hooks_welcome_message, 100, [] () {
    if(vizid) addMessage(XLAT("Welcome to RogueViz!"));
    return bool(vizid);
    }) +
  addHook(hooks_default_help, 100, default_help) +
  addHook(hooks_markers, 100, search_marker) +
 0;

}

#include "kohonen.cpp"
#include "staircase.cpp"
#include "banachtarski.cpp"
#include "pentagonal.cpp"
#include "functions.cpp"
#include "fundamental.cpp"
#include "sunflower.cpp"
#include "flocking.cpp"
#include "magiccube.cpp"
#include "cvl.cpp"
#include "newconf.cpp"
#include "grigorchuk.cpp"
#include "qtm.cpp"
#include "rewriting.cpp"
#include "sag.cpp"
#include "collatz.cpp"
#include "tree.cpp"
#include "fullnet.cpp"
