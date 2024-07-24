#include "rogueviz.h"

#if CAP_VIDEO
namespace rogueviz {

namespace statshot {

void fadeout(ld val) {
  flat_model_enabler fme;
  initquickqueue();
  ld pix = 1 / (2 * cgi.hcrossf / cgi.crossf);
  curvepoint(hyperpoint(-10, -10, 1, 1));
  curvepoint(hyperpoint(vid.xres + 10, -10, 1, 1));
  curvepoint(hyperpoint(vid.xres + 10, vid.yres + 10, 1, 1));
  curvepoint(hyperpoint(-10, vid.yres + 10, 1, 1));
  curvepoint(hyperpoint(-10, -10, 1, 1));
  shiftmatrix V = shiftless(atscreenpos(0, 0, pix));
  queuecurve(V, 0, (backcolor << 8) | int(val * 255), PPR::LINE);
  quickqueue();
  }

void shot_with_stats(int fade, vector<string> desc) {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen();

  dialog::init();
  for(const string& s: desc) {
    if(s[0] == '*') dialog::addTitle(s.substr(1), 0xFFD500, 150);
    if(s[0] == '-') dialog::addInfo(s.substr(1));
    if(s[0] == '=') dialog::addBreak(100);
    if(s[0] == ':') {
      int pos = s.find("=");
      string cap = s.substr(1, pos-1);
      string val = s.substr(pos+1);
      dialog::addSelItem(cap, val, 0);
      }
    if(s[0] == '@') {
      rogueviz::pres::dialog_add_latex(s.substr(1), (dialog::dialogcolor << 8) | 0xFF, 100, 0);
      }
    if(s[0] == '$') {
      rogueviz::pres::dialog_add_latex(s.substr(1), 0xFFD500FF, 150, 0);
      }
    }
  dialog::display();
  glflush();
  auto& f = anims::record_frame_id;
  auto ease = [] (ld x) { return x * x * (3-2*x); };
  if(f < fade) fadeout(ease(1 - (f+.5) / fade));

  if(f >= anims::noframes - fade) fadeout(ease(1 - (anims::noframes - f - 0.5) / fade));
  }

int a = arg::add3("-label-video", [] {
  string fname;
  arg::shift(); fname = arg::args();

  vector<string> desc;
  fhstream f(fname, "rt");
  int i = atoi(scanline_noblank(f).c_str()); if(i) anims::noframes = i;
  anims::videofile = scanline_noblank(f);
  int fade = atoi(scanline_noblank(f).c_str());
  string s;
  while(true) {
    s = scanline_noblank(f);
    if(s.empty()) break;
    desc.push_back(s);
    }
  hide_hud = false;
  anims::record_video(anims::videofile, [fade, desc] { return anims::record_animation_of([=] { shot_with_stats(fade, desc); }); });
  });

}}
#endif
