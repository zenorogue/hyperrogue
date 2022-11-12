#include "rogueviz.h"

/* 

used for https://youtu.be/KxjnibOkuLs

you need the models from https://renderpeople.com/free-3d-people/ (and convert them to the Wavefront OBJ format)

-geo 120c -canvas 1 -noplayer camspd=.1

*/

namespace hr {

namespace backhead {

int view_model_id = 0;
int camera_model = 1;

using namespace rogueviz::objmodels;

map<int, model> models;

tf_result person_tf(hyperpoint h) { return {0, direct_exp(h*5)}; }

ld smoothen(ld x) { return x*x*(3-2*x); }

ld cm = 0.08 / 160;

bool use_camera;

transmatrix eyematrix(int id) {
  transmatrix S = Id;
  
  int i = 0;
  if(id == 1) for(ld val: {-0.997647,0.00073308,-0.0685486,7.63928e-05,0.0147183,-0.971198,-0.224516,0.0784166,-0.0669523,-0.224937,0.972022,0.00970247,-0.000429702,0.0785862,0.00820562,0.996873})
    S[0][i++] = val;
  if(id == 0) for(ld val: {-0.976869,0.119464,-0.17719,-0.00766071,-0.0534002,-0.935386,-0.339797,0.0820953,-0.207062,-0.322171,0.923596,0.0173391,0.000492418,0.0835894,0.0105615,0.996444})
    S[0][i++] = val;
  
  return S;
  }

ld back = 0;

bool do_anim = false;

int tf;

vector<string> captions = {
  "as seen by another person",
  "changing the view...",
  "L/R",
  "downward",
  "upwards",
  "back"
  };

EX bool ourStats() {

  /*
  if(tf < 0 || tf >= 5) { println(hlog, "tf=", tf); tf = gmod(tf, 5); }
  
  println(hlog, "displaying tf = ", tf);

  displayfr(10, 10 + 7 * vid.fsize, 2, vid.fsize * 7, captions[tf], 0xFFFFFF, 0);

  nohelp = true;
  nomenukey = true;
  clearMessages();

  glflush();

  println(hlog, "done?");
  */
  
  return true;
  }

void face_animation() {

  if(!do_anim) return;

  ld t = ticks / anims::period;
  
  t = t - floor(t);
  
  t *= 2;
  view_model_id = (t >= 1 ? 0 : 1);
  camera_model = 1 - view_model_id;
  if(t>=1) t -= 1;
  
  t *= 4.5;
  
  if(t < 1) {
    if(t < .7) t /= .7;
    else t = (t-.7) / .3 + 1;
    }
  else
    t++;

  // t *= 5;
  
  tf = t;
  t -= tf;
  
  View = Id;
  
  ld up = view_model_id == 0 ? 82 : 90;
  ld down = view_model_id == 1 ? 65 : 70;
  
  up -= 15; down -= 15;

  tie(up, down) = make_pair(-down, -up);
  
  if(tf == 0) {
    View = cspin(0, 2, 360 * degree * smoothen(t)) * View;
    View = zpush(-0.1) * View;
    View = cspin180(0, 2) * View;
    back = 0;
    }
  else if(tf == 1) {
    View = zpush(-0.1 * (1-smoothen(t))) * View;
    if(t > .9)
    back = -0.1 * smoothen(10*t-9);
    View = cspin180(0, 2) * View;
    }
  else if(tf == 2) {
    View = cspin(0, 2, 75._deg*sin(TAU*smoothen(t))) * View;
    }
  else if(tf == 3) {
    View = cspin(1, 2, up*degree*smoothen(t)) * View;
    }
  else if(tf == 4) {
    View = cspin(1, 2, degree*(up-(up+down)*smoothen(t))) * View;
    }
  else if(tf == 5) {
    View = cspin(1, 2, degree*-down*(1-smoothen(t*2))) * View;
    }
    
  use_camera = tf <= 1;

  sightranges[geometry] = use_camera ? 30 : 100;
  if(tf == 5) sightranges[geometry] = 100 - 70 * smoothen(t*2);
  
  anims::moved();

  hide_hud = false;
  }
  
bool draw_ply() {

  if(models.empty()) {
    models[0] = model("rogueviz/models/", "dennis.obj", person_tf);
    models[1] = model("rogueviz/models/", "mei.obj", person_tf);
    }
  
  shiftmatrix Zero = ggmatrix(currentmap->gamestart());
  
  Zero = Zero * eyematrix(view_model_id);
  println(hlog, Zero);
  
  models[view_model_id].render(Zero);

  if(use_camera) models[camera_model].render(shiftless(eyematrix(camera_model) * zpush(back)));
  
  return false;
  }

auto plyhook = addHook(hooks_frame, 100, draw_ply)
  + addHook(anims::hooks_anim, 100, face_animation)
+ addHook(hooks_args, 100, [] {
  using namespace arg;
           
  if(0) ;
  else if(argis("-head-swap")) {
    swap(view_model_id, camera_model);
    }
  else if(argis("-head-anim")) {
    do_anim = !do_anim;
    }
  else return 1;
  return 0;
  })
  + addHook(hooks_prestats, 100, ourStats)
    ;

}
}