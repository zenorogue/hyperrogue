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

#include "rogueviz.h"

namespace rogueviz {

string weight_label;

ld fat_edges = 0;
ld ggamma = 1;

using namespace hr;

edgetype default_edgetype = { .1, .1, DEFAULT_COLOR, 0xFF0000FF, "default" };

bool showlabels = false;
bool show_edges = false;
bool specialmark = false;
bool edge_legend = false;

int max_edges_help = 1000;

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

map<string, int> labeler;

bool id_known(const string& s) {
  return labeler.count(s);
  }

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
    return inverse_shift(ggmatrix(currentmap->gamestart()), ggmatrix(m->base) * tC0(m->at));
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

int readLabel(fhstream& f) {
  string s = scan<string>(f);
  if(s == "") return -1;
  return getid(s);
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

  int qty = 0;

  for(auto ei: alledges) {
    help_extension hex;
    hex.key = dialog::list_fake_key++;

    int k = ei->i ^ ei->j ^ id;
    hex.text = vdata[k].name;
    hex.color = vdata[k].cp.color1 >> 8;
    if(vizflags & RV_WHICHWEIGHT) {
      ld w = which_weight ? ei->weight2 : ei->weight;
      if(vizflags & RV_INVERSE_WEIGHT) w = 1 / w;
      hex.subtext = fts(w);
      }

    hex.action = [k] () { help_extensions.clear(); rogueviz_help(k, 0); };
    help_extensions.push_back(hex);

    qty++; if(qty > max_edges_help) break;
    }

  if((vizflags & RV_WHICHWEIGHT) && noedges) {
    help_extension hex;
    hex.key = '1';
    hex.text = "displayed weight";
    bool inv = vizflags & RV_INVERSE_WEIGHT;
    hex.subtext = which_weight ? (inv ? "distance requested" : "attraction force") : (inv ? "inverse value from data" : "weight from the data");
    hex.action = [id, pagenumber] () { which_weight = !which_weight; help_extensions.clear(); rogueviz_help(id, pagenumber); };
    help_extensions.push_back(hex);
    }

  if(true) {
    help_extension hex;
    hex.key = '2';
    hex.text = "find this";
    hex.action = [id] () { search_for = id; popScreen(); };
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

double min_line_step = .1;
double min_line_splits = 0;
double max_line_splits = 6;

void storelineto(vector<glvertex>& tab, const hyperpoint& h1, const hyperpoint& h2, int s) {
  if(s >= max_line_splits || (intval(h1, h2) < min_line_step && s >= min_line_splits))
    storevertex(tab, h2);
  else {
    hyperpoint h3 = mid(h1, h2);
    storelineto(tab, h1, h3, s+1);
    storelineto(tab, h3, h2, s+1);
    }
  }

void storeline(vector<glvertex>& tab, const hyperpoint& h1, const hyperpoint& h2) {
  storevertex(tab, h1);
  storelineto(tab, h1, h2, 0);
  }

color_t darken_a(color_t c) {
  for(int p=0; p<3; p++)
  for(int i=0; i<darken; i++) part(c, p+1) = (part(c, p+1) + part(backcolor, p)) >> 1;
  return c;
  }

#if CAP_SVG
#define SVG_LINK(x) svg::link = (x)
#else
#define SVG_LINK(x) 
#endif

void queuedisk(const shiftmatrix& V, const colorpair& cp, bool legend, const string* info, int i) {
  if(legend && (int) cp.color1 == (int) 0x000000FF && backcolor == 0)
    poly_outline = 0x606060FF;
  else
    poly_outline = (bordcolor << 8) | 0xFF;
  
  #if CAP_TEXTURE
  if(cp.img) {
    for(hyperpoint h: cp.img->vertices)
      curvepoint(h);
    auto& qc = queuecurve(V, 0, 0xFFFFFFFF, PPR::MONSTER_HEAD);
    qc.tinf = &cp.img->tinf;
    qc.flags |= POLY_TRIANGLES;
    return;
    }
  #endif
    
  shiftmatrix V1;
  
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
    queuepolyat(V1 = orthogonal_move_fol(V, cgi.BODY), sh, darken_a(cp.color1), PPR::MONSTER_HEAD);
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

map<pair<edgeinfo*, int>, int> drawn_edges;

map<pair<cell*, cell*>, transmatrix> relmatrices;

transmatrix& memo_relative_matrix(cell *c1, cell *c2) {
  auto& p = relmatrices[make_pair(c1, c2)];
  if(p[2][2] == 0) {
    forCellIdEx(c3, i, c2) if(c3 == c1)
      return p = currentmap->adj(c2, i);
    p = calc_relative_matrix(c1, c2,  C0);
    }
  return p;
  }

void queue_prec(const shiftmatrix& V, edgeinfo*& ei, color_t col) {
  if(!fat_edges)
    queuetable(V, ei->prec, isize(ei->prec), col, 0, PPR::STRUCT0);
  #if MAXMDIM >= 4 && CAP_TEXTURE
  else {
    auto& t = queuetable(V, ei->prec, isize(ei->prec), 0, col | 0x000000FF, PPR::STRUCT0);
    t.flags |= (1<<22), // poly triangles
    t.offset_texture = 0,
    t.tinf = &ei->tinf;
    t.tinf->texture_id = floor_textures->renderedTexture;
    }
  #endif
  }

int brm_limit = 1000;

ld labelshift = .2;
ld labelscale = .2; // .28 in SVG

ld edgewidth = 1;

bool highlight_target = true;

bool drawVertex(const shiftmatrix &V, cell *c, shmup::monster *m) {
  if(m->dead) return true;
  if(m->type != moRogueviz) return false;
  int i = m->pid;
  vertexdata& vd = vdata[i];
  
  // bool ghilite = false;
  
  // if(vd.special && specialmark) ghilite = true;
  
  if(!gmatrix.count(m->base)) printf("base not in gmatrix\n");

  int lid = shmup::lmousetarget ? shmup::lmousetarget->pid : -2;
  
  bool multidraw = quotient;
  
  bool use_brm = closed_or_bounded && isize(currentmap->allcells()) <= brm_limit;

  ld hi_weight = 0;
        
  if(!lshiftclick) for(int j=0; j<isize(vd.edges); j++) {
    edgeinfo *ei = vd.edges[j].second;
    if(multidraw && ei->i != i) continue;
    vertexdata& vd1 = vdata[ei->i];
    vertexdata& vd2 = vdata[ei->j];

    int oi = ei->i, oj = ei->j;
    bool hilite = false;
    if(vdata[oi].special && vdata[oj].special && specialmark) hilite = true;
    else if(svg::in || inHighQual || !highlight_target) hilite = false;
    else if(vd1.m == shmup::mousetarget) hilite = true;
    else if(vd2.m == shmup::mousetarget) hilite = true;
    else if(oi == lid || oj == lid) hilite = true;

    if(ei->weight < (hilite ? ei->type->visible_from_hi : ei->type->visible_from)) continue;

    if((vd1.m == shmup::mousetarget || vd2.m == shmup::mousetarget) && m != shmup::mousetarget)
      hi_weight = ei->weight;

    // if(hilite) ghilite = true;
    
    if(ei->lastdraw < frameid || multidraw) {
      ei->lastdraw = frameid;
      dynamicval<ld> w(vid.linewidth, vid.linewidth * edgewidth);
      
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

      shiftmatrix gm1, gm2;
      
      if(use_brm) {
        gm1 = V * memo_relative_matrix(vd1.m->base, c);
        gm2 = gm1 * brm_get(vd1.m->base, tC0(vd1.m->at), vd2.m->base, tC0(vd2.m->at));
        }
      else if(multidraw || elliptic) {
        gm1 = V * memo_relative_matrix(vd1.m->base, c);
        gm2 = V * memo_relative_matrix(vd2.m->base, c);
        }
      else {
        gm1 = ggmatrix(vd1.m->base);
        gm2 = ggmatrix(vd2.m->base);
        }
      
      shiftpoint h1 = gm1 * vd1.m->at * C0;
      shiftpoint h2 = gm2 * vd2.m->at * C0;
      
      if(elliptic && hdist(h1, h2) > hdist(h1.h, centralsym * h2.h))
        h2.h = centralsym * h2.h;
      
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
      
      if(callhandlers(false, hooks_alt_edges, ei, false)) ;

      else if(pmodel && !fat_edges) {
        queueline(h1, h2, col, 2 + vid.linequality).prio = PPR::STRUCT0;
        }
      else {
      
        cell *center = multidraw ? c : centerover;
      
        if(!multidraw && ei->orig && ei->orig != center && celldistance(ei->orig, center) > 3) 
          ei->orig = NULL;
        if(!ei->orig) {
          ei->orig = center; // cwt.at;
          ei->prec.clear();
          
          const shiftmatrix& T = multidraw ? V : ggmatrix(ei->orig);
          
          if(callhandlers(false, hooks_alt_edges, ei, true)) ;
          else if(fat_edges) {
            ei->tinf.tvertices.clear();
            shiftmatrix T1 = gm1 * vd1.m->at;
            hyperpoint goal = inverse_shift(T1, h2);
            transmatrix S = inverse_shift(T, gm1) * vd1.m->at * rspintox(goal);
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
          else 
            storeline(ei->prec, inverse_shift(T, h1), inverse_shift(T, h2));
          }
        
        const shiftmatrix& T = multidraw ? V : ggmatrix(ei->orig);

        queue_prec(T, ei, col);
        if(elliptic) queue_prec(ggmatrix(ei->orig) * centralsym, ei, col);
        }
      }
/*
    */
    }

  if(!vd.virt) {
    queuedisk(V * m->at, vd.cp, false, vd.info, i);
    }
  
  
  if((showlabels || (show_edges && hi_weight)) && !darken) {
    bool doshow = true;
    if((vizflags & RV_COMPRESS_LABELS) && i > 0 && !vd.virt) {
      vertexdata& vdp = vdata[vd.data];
      shiftpoint h2 = ggmatrix(vdp.m->base) * vdp.m->at * C0;
      if(hdist(h2, V * m->at * C0) < 0.1) doshow = false;
      }
    
    shiftpoint h = tC0(V * m->at);
    shiftmatrix V2 = GDIM == 3 ? V * m->at : rgpushxto0(h) * ypush(cgi.scalefactor * labelshift); // todo-variation
    if(doshow && !behindsphere(V2)) {
      auto info = vd.info;
      if(info) queueaction(PPR::MONSTER_HEAD, [info] () { SVG_LINK(*info); });
      string s;
      ld w = hi_weight;
      if(vizflags & RV_INVERSE_WEIGHT) w = 1/w;
      if(showlabels && hi_weight) s = vd.name + " : " + fts(w);
      else if(showlabels) s = vd.name;
      else if(hi_weight) s = fts(w);
      queuestr(V2, labelscale, s, forecolor, (svg::in || ISWEB) ? 0 : 1);
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
  if(cmode & sm::DRAW) return false;

  int qet = isize(edgetypes);
  if(qet == 1 || !edge_legend) qet = 0;

  int legit = qet + isize(legend);
  
  if(legit == 0) return false;
  
  initquickqueue();
  
  int rad = current_display->radius/10;
  ld x = vid.xres - rad;

  for(int i=0; i<isize(legend); i++) {
    int k = legend[i];
    vertexdata& vd = vdata[k];
    
    ld y = (current_display->radius * (i+.5)) / legit * 2 - current_display->radius + vid.yres/2;

    transmatrix V = atscreenpos(x, y, current_display->radius/4);
    
    poly_outline = OUTLINE_NONE;
    queuedisk(shiftless(V), vd.cp, true, NULL, i);
    poly_outline = OUTLINE_DEFAULT;
    queuestr(int(x-rad), int(y), 0, rad*(svg::in?5:3)/4, vd.name, legend_color, 0, 16);
    }

  for(int i=0; i<qet; i++) {
    auto t = edgetypes[i];
        
    ld y = (current_display->radius * (i+isize(legend)+.5)) / legit * 2 - current_display->radius + vid.yres/2;

    transmatrix V = atscreenpos(x, y, current_display->radius/8);
    
    poly_outline = t->color | 0xFF;
    queuepolyat(shiftless(V), cgi.shTriangle, 0, PPR::MONSTER_HEAD);
        
    poly_outline = OUTLINE_DEFAULT;
    queuestr(int(x-rad), int(y), 0, rad*(svg::in?5:3)/4, t->name, legend_color, 0, 16);
    }
  
  quickqueue();
  return true;
  }

bool rv_ignore_spaces = true;

bool rv_ignore(char c) {
  if(c == 32 && !rv_ignore_spaces) return true;
  if(c == 10 || c == 13 || c == 32 || c == 9) return true;
  return false;
  }

void readcolor(const string& cfname) {
  FILE *f = fopen(cfname.c_str(), "rt");
  if(!f) { printf("color file missing\n"); exit(1); }
  while(true) {
    string lab = "";
    while(true) {
      int c = fgetc(f);
      if(c == EOF) { fclose(f); return; }
      else if(c == ',' || c == ';') break;
      else if(rv_ignore(c)) ;
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
        if(rv_ignore(c) || c == ',' || c == ';' || c == EOF) break;
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
    else if(c2 == '>') {
      char buf[600];
      int err = fscanf(f, "%500s", buf);
      if(err > 0) {
        vdata[getid(lab)].name = buf;
        for(char& ch: vdata[getid(lab)].name) if(ch == '_') ch = ' ';
        }
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

void graph_rv_hooks();

void init(flagtype _vizflags) {

  autocheat = true; 
  showstartmenu = false;

  #if CAP_TOUR
  if(tour::on) {
    tour::slide_backup(mapeditor::drawplayer);
    tour::slide_backup(timerghost);
    }
  #endif

#if !ISWEB
  mapeditor::drawplayer = false;
  stop_game();
  enable_canvas();
  restart_game(shmup::on ? rg::shmup : rg::nothing);
#else
  stop_game();
  enable_canvas();
  restart_game(rg::nothing);
#endif
  autocheat = true;
  items[itOrbLife] = 0;
  timerghost = false;

  gmatrix.clear();
  calcparam();
  drawthemap();
  gmatrix0 = gmatrix;

  vizflags = _vizflags;
  
  graph_rv_hooks();
  }

int search_for = -1;

vector<reaction_t> cleanup;

void do_cleanup() {
  while(!cleanup.empty()) {
    cleanup.back()();
    cleanup.pop_back();
    }
  }

void close() { 
  search_for = -1;
  for(int i=0; i<isize(vdata); i++)
    if(vdata[i].m) vdata[i].m->dead = true;
  vdata.clear();
  labeler.clear();
  legend.clear();
  for(int i=0; i<isize(edgeinfos); i++) delete edgeinfos[i];
  edgeinfos.clear();
  callhooks(hooks_close);
  edgetypes.clear();
  do_cleanup();
  relmatrices.clear();
  }

#if CAP_COMMANDLINE
int readArgs() {
  using namespace arg;
           
// options before reading
  if(0) ;
  else if(argis("-dftcolor")) {
    shift(); dftcolor = parse(args());
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
  else if(argis("-rvedges")) {
    shift(); show_edges = argi();
    }
  else if(argis("-lab-off")) {
    showlabels = false;
    }
  else if(argis("-rvspaces")) {
    rv_ignore_spaces = false;
    }
  else if(argis("-rvlabelshift")) {
    shift_arg_formula(labelshift);
    }
  else if(argis("-rvlabelscale")) {
    shift_arg_formula(labelscale);
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
    shift_arg_formula(min_line_step);
    }
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
  gamescreen();
  dialog::init(XLAT("rogueviz edges"));
  for(int i=0; i<isize(edgetypes); i++) {
    auto t = edgetypes[i];
    switch(mode) {
      case 0:
        if(t->color == DEFAULT_COLOR)
          dialog::addSelItem(t->name, "default", 'a' + i);
        else
          dialog::addColorItem(t->name, t->color, 'a' + i);
        dialog::add_action([t] {
          dialog::openColorDialog(t->color, NULL);
          dialog::dialogflags |= sm::MAYDARK | sm::SIDE;
          });
        break;
      case 1: case 2: {
        auto& val = mode == 2 ? t->visible_from_hi : t->visible_from;
        if(!(vizflags & RV_INVERSE_WEIGHT)) {
          dialog::addSelItem(t->name, fts(val), 'a'+i);
          dialog::add_action([&val] {
            dialog::editNumber(val, 0.001, 1000, .1, .1, "min weight", "");
            dialog::scaleLog();
            });
          }
        else {
          dialog::addSelItem(t->name, fts(1 / val), 'a'+i);
          dialog::add_action([t, &val] {
            static ld i;
            i = 1 / val;
            dialog::editNumber(i, 1, 1000000, 1, 500, weight_label, "");
            dialog::reaction = [&val] () { val = i ? 1. / i : 5; };
            dialog::scaleLog(); dialog::ne.step = .2;
            });
          }
        break;
        }
      default: break;
      }
    }
  dialog::addBreak(100);
  if(vizflags & RV_HAVE_WEIGHT) {
    dialog::addBoolItem_choice("color/alpha", mode, 0, '1');
    dialog::addBoolItem_choice(weight_label, mode, 1, '2');
    dialog::addBoolItem_choice(weight_label + " (hi)", mode, 2, '3');
    dialog::addBoolItem("inverse weights", vizflags & RV_INVERSE_WEIGHT, '4');
    dialog::add_action([] {
      vizflags ^= RV_INVERSE_WEIGHT;
      });
    dialog::addSelItem("weight gamma", fts(ggamma), '5');
    dialog::add_action([] {
      dialog::editNumber(ggamma, 0, 2, 0.1, 1, "weight gamma", "");
      });
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
    shiftpoint H = ggmatrix(m->base) * tC0(m->at);
    queuestr(H, 2*vid.fsize, "X", 0x10101 * int(128 + 100 * sin(ticks / 150.)));
    addauraspecial(H, iinf[itOrbYendor].color, 0);
    }
  }

void showVertexSearch() {
  cmode = sm::SIDE | sm::MAYDARK | sm::DIALOG_STRICT_X;
  gamescreen(); search_for = -1;

  dialog::init(XLAT("vertex search"));
  dialog::v.clear();
  if(dialog::infix != "") mouseovers = dialog::infix;

  for(int i=0; i<isize(vdata); i++) if(vdata[i].name != "") dialog::vpush(i, vdata[i].name.c_str());

  dialog::addBreak(50);
  dialog::start_list(900, 900, '1');
  for(auto& vi: dialog::v) {
    dialog::addItem(vi.first, dialog::list_fake_key++);
    dialog::add_action([&vi] () {
      search_for = vi.second;
      popScreenAll();
      });
    }
  dialog::end_list();
  dialog::addBreak(50);

  dialog::addSelItem("matching items", its(isize(dialog::v)), 0);
  dialog::addInfo(XLAT("press letters to search"));
  dialog::display();

  keyhandler = [] (int sym, int uni) {
    dialog::handleNavigation(sym, uni);    
    if(dialog::editInfix(uni)) dialog::list_skip = 0;
    else if(doexiton(sym, uni)) popScreen();
    };

  }

void showMenu() {
  if(callhandlers(false, hooks_rvmenu_replace)) return;
  cmode = sm::SIDE | sm::MAYDARK | sm::DIALOG_STRICT_X;
  gamescreen();

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
  dialog::add_action([] { 
    vertex_shape = (1 + vertex_shape) & 3; 
    });

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

bool default_help() {

  help = 
    "This is RogueViz, a visualization engine based on HyperRogue.\n\nUse WASD to move, v for menu.\n\n"
    "Read more about RogueViz on : http://roguetemple.com/z/hyper/rogueviz.php\n\n";

  help_extensions.push_back(help_extension{'u', XLAT("RogueViz menu"), [] () { popScreen(); pushScreen(showMenu); }});    
  return true;
  }

void o_key(o_funcs& v) {
  v.push_back(named_dialog(XLAT("RogueViz graph viz settings"), rogueviz::showMenu));
  }

auto hooks  = 
#if CAP_COMMANDLINE
  addHook(hooks_args, 100, readArgs) +
#endif
  addHook(hooks_clearmemory, 0, close) +

  addHook(hooks_markers, 100, search_marker) +
  addHook(hooks_configfile, 100, [] {
    param_i(brm_limit, "brm_limit");
    param_f(edgewidth, "rvedgewidth");
    param_f(min_line_splits, "edgeminsplits");
    param_f(max_line_splits, "edgemaxsplits");
    }) +
 0;

void graph_rv_hooks() {
  rv_hook(hooks_default_help, 100, default_help);
  rv_hook(hooks_o_key, 100, o_key);
  rv_hook(hooks_prestats, 100, rogueviz_hud);
  rv_hook(shmup::hooks_draw, 100, drawVertex);
  rv_hook(shmup::hooks_describe, 100, describe_monster);
  rv_hook(shmup::hooks_kill, 100, activate);
  rv_hook(hooks_welcome_message, 100, [] () {
    addMessage(XLAT("Welcome to RogueViz!"));
    return true;
    });
  }
  
}
