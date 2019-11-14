#include "hyper.h"
namespace hr {

#if HDR
struct radarpoint {
  hyperpoint h;
  char glyph;
  color_t color;
  color_t line;
  };

struct radarline {
  hyperpoint h1, h2;
  color_t line;
  };
#endif

EX vector<radarpoint> radarpoints;
EX vector<radarline> radarlines;

EX transmatrix radar_transform;

pair<bool, hyperpoint> makeradar(hyperpoint h) {
  if(GDIM == 3 && WDIM == 2) h = radar_transform * h;

  ld d = hdist0(h);

  if(sol && nisot::geodesic_movement) {
    h = inverse_exp(h, iLazy);
    ld r = hypot_d(3, h);
    if(r < 1) h = h * (atanh(r) / r);
    else return {false, h};
    }
  if(prod) h = product::inverse_exp(h);
  if(nisot::local_perspective_used()) h = NLP * h;
  
  if(WDIM == 3) {
    if(d >= vid.radarrange) return {false, h};
    if(d) h = h * (d / vid.radarrange / hypot_d(3, h));
    }
  else if(hyperbolic) {
    for(int a=0; a<3; a++) h[a] = h[a] / (1 + h[3]);
    }
  else if(sphere) {
    h[2] = h[3];
    }
  else {
    if(d > vid.radarrange) return {false, h};
    if(d) h = h * (d / (vid.radarrange + cgi.scalefactor/4) / hypot_d(3, h));
    }
  if(invalid_point(h)) return {false, h};
  return {true, h};
  }

EX void addradar(const transmatrix& V, char ch, color_t col, color_t outline) {
  hyperpoint h = tC0(V);
  auto hp = makeradar(h);
  if(hp.first)
    radarpoints.emplace_back(radarpoint{hp.second, ch, col, outline});
  }

EX void addradar(const hyperpoint h1, const hyperpoint h2, color_t col) {
  auto hp1 = makeradar(h1);
  auto hp2 = makeradar(h2);
  if(hp1.first && hp2.first)
    radarlines.emplace_back(radarline{hp1.second, hp2.second, col});
  }

void celldrawer::drawcell_in_radar() {
  cell *c = cw.at;
  #if CAP_SHMUP
  if(shmup::on) {
    pair<shmup::mit, shmup::mit> p = 
      shmup::monstersAt.equal_range(c);
    for(shmup::mit it = p.first; it != p.second; it++) {
      shmup::monster* m = it->second;
      addradar(V*m->at, minf[m->type].glyph, minf[m->type].color, 0xFF0000FF);
      }
    }
  #endif
  if(c->monst) 
    addradar(V, minf[c->monst].glyph, minf[c->monst].color, isFriendly(c->monst) ? 0x00FF00FF : 0xFF0000FF);
  else if(c->item && !itemHiddenFromSight(c))
    addradar(V, iinf[c->item].glyph, iinf[c->item].color, kind_outline(c->item));
  }

void celldrawer::radar_grid() {
  cell *c = cw.at;
  for(int t=0; t<c->type; t++)
    if(c->move(t) && c->move(t) < c)
      addradar(V*get_corner_position(c, t%c->type), V*get_corner_position(c, (t+1)%c->type), gridcolor(c, c->move(t)));
  }

EX void draw_radar(bool cornermode) {

  if(dual::split([] { dual::in_subscreen([] { calcparam(); draw_radar(false); }); })) return;
  bool d3 = WDIM == 3;
  bool hyp = hyperbolic;
  bool sph = sphere;
  bool scompass = nonisotropic && !hybri;

  dynamicval<eGeometry> g(geometry, gEuclid);
  dynamicval<eModel> pm(pmodel, mdDisk);
  dynamicval<bool> ga(vid.always3, false);
  dynamicval<geometryinfo1> gi(ginf[gEuclid].g, giEuclid2);
  initquickqueue();
  int rad = vid.radarsize;
  if(dual::state) rad /= 2; 
  
  ld cx = dual::state ? (dual::currently_loaded ? vid.xres/2+rad+2 : vid.xres/2-rad-2) : cornermode ? rad+2 : vid.xres-rad-2;
  ld cy = vid.yres-rad-2 - vid.fsize;

  for(int i=0; i<360; i++)
    curvepoint(atscreenpos(cx-cos(i * degree)*rad, cy-sin(i*degree)*rad, 1) * C0);
  queuecurve(0xFFFFFFFF, 0x000000FF, PPR::ZERO);      

  ld alpha = 15 * degree;
  ld co = cos(alpha);
  ld si = sin(alpha);
  
  if(sph && !d3) {
    for(int i=0; i<360; i++)
      curvepoint(atscreenpos(cx-cos(i * degree)*rad, cy-sin(i*degree)*rad*si, 1) * C0);
    queuecurve(0, 0x200000FF, PPR::ZERO);
    }

  if(d3) {
    for(int i=0; i<360; i++)
      curvepoint(atscreenpos(cx-cos(i * degree)*rad, cy-sin(i*degree)*rad*si, 1) * C0);
    queuecurve(0xFF0000FF, 0x200000FF, PPR::ZERO);
  
    curvepoint(atscreenpos(cx-sin(vid.fov*degree/2)*rad, cy-sin(vid.fov*degree/2)*rad*si, 1) * C0);
    curvepoint(atscreenpos(cx, cy, 1) * C0);
    curvepoint(atscreenpos(cx+sin(vid.fov*degree/2)*rad, cy-sin(vid.fov*degree/2)*rad*si, 1) * C0);
    queuecurve(0xFF8000FF, 0, PPR::ZERO);
    }
  
  if(d3) for(auto& r: radarpoints) {    
    queueline(atscreenpos(cx+rad * r.h[0], cy - rad * r.h[2] * si + rad * r.h[1] * co, 0)*C0, atscreenpos(cx+rad*r.h[0], cy - rad*r.h[2] * si, 0)*C0, r.line, -1);
    }
  
  if(scompass) {
    auto compassdir = [&] (char dirname, hyperpoint h) {
      h = NLP * h * .8;
      queueline(atscreenpos(cx+rad * h[0], cy - rad * h[2] * si + rad * h[1] * co, 0)*C0, atscreenpos(cx+rad*h[0], cy - rad*h[2] * si, 0)*C0, 0xA0401040, -1);
      displaychr(int(cx+rad * h[0]), int(cy - rad * h[2] * si + rad * h[1] * co), 0, 8, dirname, 0xA04010);
      };
    compassdir('E', point3(+1, 0, 0));
    compassdir('N', point3(0, +1, 0));
    compassdir('W', point3(-1, 0, 0));
    compassdir('S', point3(0, -1, 0));
    compassdir('U', point3(0,  0,+1));
    compassdir('D', point3(0,  0,-1));
    }

  auto locate = [&] (hyperpoint h) {
    if(sph)
      return point3(cx + (rad-10) * h[0], cy + (rad-10) * h[2] * si + (rad-10) * h[1] * co, +h[1] * si > h[2] * co ? 8 : 16);
    else if(hyp) 
      return point3(cx + rad * h[0], cy + rad * h[1], 1/(1+h[3]) * cgi.scalefactor * current_display->radius / (inHighQual ? 10 : 6));
    else
      return point3(cx + rad * h[0], cy + rad * h[1], rad * cgi.scalefactor / (vid.radarrange + cgi.scalefactor/4) * 0.8);
    };
  
  for(auto& r: radarlines) {
    hyperpoint h1 = locate(r.h1);
    hyperpoint h2 = locate(r.h2);
    h1 = tC0(atscreenpos(h1[0], h1[1], 1));
    h2 = tC0(atscreenpos(h2[0], h2[1], 1));
    queueline(h1, h2, r.line, -1);
    }

  quickqueue();
  glflush();
  
  for(auto& r: radarpoints) {
    if(d3) displaychr(int(cx + rad * r.h[0]), int(cy - rad * r.h[2] * si + rad * r.h[1] * co), 0, 8, r.glyph, r.color);
    else {
      hyperpoint h = locate(r.h);
      displaychr(int(h[0]), int(h[1]), 0, int(h[2]), r.glyph, r.color);
      }
    }
  }

}
