#include "hyper.h"
namespace hr {

#if MAXMDIM >= 4
pair<bool, hyperpoint> makeradar(shiftpoint h) {
  if(embedded_plane) h.h = current_display->radar_transform * h.h;

  ld d = hdist0(h);

  hyperpoint h1;
  
  if(sol && nisot::geodesic_movement && !embedded_plane) {
    hyperpoint h1 = inverse_exp(h, pQUICK);
    ld r = hypot_d(3, h1);
    if(r < 1) h1 = h1 * (atanh(r) / r);
    else return {false, h1};
    }
  else if(mproduct) h1 = product::inverse_exp(unshift(h));
  else if(sl2) h1 = slr::get_inverse_exp(h);
  else h1 = unshift(h);
  
  if(nisot::local_perspective_used() && !embedded_plane) {
    h1 = NLP * h1;
    }
  
  if(WDIM == 3) {
    if(d >= vid.radarrange) return {false, h1};
    if(d) h1 = h1 * (d / vid.radarrange / hypot_d(3, h1));
    }
  else if(mhyperbolic) {
    if(geom3::hyp_in_solnih()) {
      geom3::light_flip(true);
      h1 = parabolic1(h1[1]) * xpush0(h1[0]);
      geom3::light_flip(false);
      h1[3] = h1[2]; h1[2] = 0;
      // h1 = current_display->radar_transform * h1;
      }
    for(int a=0; a<LDIM; a++) h1[a] = h1[a] / (1 + h1[LDIM]);
    }
  else if(msphere) {
    if(geom3::same_in_same()) h1[2] = h1[LDIM];
    if(geom3::sph_in_hyp()) h1 /= sinh(1);
    }
  else {
    if(geom3::euc_in_hyp()) {
      for(int a=0; a<3; a++) h1[a] = h1[a] / (1 + h1[3]);
      h1[2] -= 1;
      h1 *= 2 / sqhypot_d(3, h1);
      d = hypot_d(2, h1);
      if(d > vid.radarrange) return {false, h1};
      if(d) h1 = h1 / (vid.radarrange + cgi.scalefactor/4);
      }
    else if(geom3::same_in_same()) {
      if(d > vid.radarrange) return {false, h1};
      if(d) h1 = h1 * (d / (vid.radarrange + cgi.scalefactor/4) / hypot_d(3, h1));
      }
    else {
      d = hypot_d(2, h1);
      if(d > vid.radarrange) return {false, h1};
      if(d) h1 = h1 / (vid.radarrange + cgi.scalefactor/4);
      }
    }
  if(invalid_point(h1)) return {false, h1};
  return {true, h1};
  }

EX void addradar(const shiftmatrix& V, char ch, color_t col, color_t outline) {
  shiftpoint h = V * tile_center();
  auto hp = makeradar(h);
  if(hp.first)
    current_display->radarpoints.emplace_back(radarpoint{hp.second, ch, col, outline});
  }

EX void addradar(const shiftpoint h1, const shiftpoint h2, color_t col) {
  auto hp1 = makeradar(h1);
  auto hp2 = makeradar(h2);
  if(hp1.first && hp2.first)
    current_display->radarlines.emplace_back(radarline{hp1.second, hp2.second, col});
  }

void celldrawer::drawcell_in_radar() {
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
  for(int t=0; t<c->type; t++)
    if(c->move(t) && (c->move(t) < c || fake::split()))
      addradar(V*get_corner_position(c, t%c->type), V*get_corner_position(c, (t+1)%c->type), gridcolor(c, c->move(t)));
  }
#endif

EX void draw_radar(bool cornermode) {
#if MAXMDIM >= 4
  if(subscreens::split([=] () { calcparam(); draw_radar(false); })) return;
  if(dual::split([] { dual::in_subscreen([] { calcparam(); draw_radar(false); }); })) return;
  bool d3 = WDIM == 3;
  bool hyp = mhyperbolic;
  bool sph = msphere;
  bool scompass = nonisotropic && !mhybrid && !embedded_plane;

  dynamicval<eGeometry> g(geometry, gEuclid);
  dynamicval<eModel> pm(pmodel, mdDisk);
  dynamicval<bool> ga(vid.always3, false);
  dynamicval<geometryinfo1> gi(ginf[gEuclid].g, giEuclid2);
  initquickqueue();
  int rad = vid.radarsize;
  int divby = 1;
  if(dual::state) divby *= 2;
  if(subscreens::in) divby *= 2;
  rad /= divby;
  auto& cd = current_display;
  
  ld cx = dual::state ? (dual::currently_loaded ? vid.xres/2+rad+2 : vid.xres/2-rad-2) :
          subscreens::in ? cd->xtop + cd->xsize - rad - 2 :
          cornermode ? rad+2 : vid.xres-rad-2;
  ld cy = subscreens::in ? cd->ytop + cd->ysize - rad - 2 - vid.fsize :
          vid.yres-rad-2 - vid.fsize;
  
  auto sId = shiftless(Id);

  for(int i=0; i<360; i++)
    curvepoint(atscreenpos(cx-cos(i * degree)*rad, cy-sin(i*degree)*rad, 1) * C0);
  queuecurve(sId, 0xFFFFFFFF, 0x000000FF, PPR::ZERO);      

  ld alpha = 15._deg;
  ld co = cos(alpha);
  ld si = sin(alpha);
  
  if(sph && !d3) {
    for(int i=0; i<360; i++)
      curvepoint(atscreenpos(cx-cos(i * degree)*rad, cy-sin(i*degree)*rad*si, 1) * C0);
    queuecurve(sId, 0, 0x200000FF, PPR::ZERO);
    }

  if(d3) {
    for(int i=0; i<360; i++)
      curvepoint(atscreenpos(cx-cos(i * degree)*rad, cy-sin(i*degree)*rad*si, 1) * C0);
    queuecurve(sId, 0xFF0000FF, 0x200000FF, PPR::ZERO);
  
    curvepoint(atscreenpos(cx-sin(vid.fov*degree/2)*rad, cy-sin(vid.fov*degree/2)*rad*si, 1) * C0);
    curvepoint(atscreenpos(cx, cy, 1) * C0);
    curvepoint(atscreenpos(cx+sin(vid.fov*degree/2)*rad, cy-sin(vid.fov*degree/2)*rad*si, 1) * C0);
    queuecurve(sId, 0xFF8000FF, 0, PPR::ZERO);
    }
  
  if(d3) for(auto& r: cd->radarpoints) {
    queueline(sId*atscreenpos(cx+rad * r.h[0], cy - rad * r.h[2] * si + rad * r.h[1] * co, 0)*C0, sId*atscreenpos(cx+rad*r.h[0], cy - rad*r.h[2] * si, 0)*C0, r.line, -1);
    }
  
  if(scompass) {
    auto compassdir = [&] (char dirname, hyperpoint h) {
      h = NLP * h * .8;
      queueline(sId*atscreenpos(cx+rad * h[0], cy - rad * h[2] * si + rad * h[1] * co, 0)*C0, sId*atscreenpos(cx+rad*h[0], cy - rad*h[2] * si, 0)*C0, 0xA0401040, -1);
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
      return point3(cx + rad * h[0], cy + rad * h[1], 1/(1+h[LDIM]) * cgi.scalefactor * current_display->radius / (inHighQual ? 10 : 6));
    else
      return point3(cx + rad * h[0], cy + rad * h[1], rad * cgi.scalefactor / (vid.radarrange + cgi.scalefactor/4) * 0.8);
    };
  
  for(auto& r: cd->radarlines) {
    hyperpoint h1 = locate(r.h1);
    hyperpoint h2 = locate(r.h2);
    h1 = tC0(atscreenpos(h1[0], h1[1], 1));
    h2 = tC0(atscreenpos(h2[0], h2[1], 1));
    queueline(sId*h1, sId*h2, r.line, -1);
    }

  quickqueue();
  glflush();
  
  for(auto& r: cd->radarpoints) {
    if(d3) displaychr(int(cx + rad * r.h[0]), int(cy - rad * r.h[2] * si + rad * r.h[1] * co), 0, 8, r.glyph, r.color);
    else {
      hyperpoint h = locate(r.h);
      displaychr(int(h[0]), int(h[1]), 0, int(h[2]) / divby, r.glyph, r.color);
      }
    }
#endif
  }

#if MAXMDIM < 4
EX void addradar(const shiftmatrix& V, char ch, color_t col, color_t outline) { }
  void drawcell_in_radar();

void celldrawer::drawcell_in_radar() {}
void celldrawer::radar_grid() {}
#endif
}
