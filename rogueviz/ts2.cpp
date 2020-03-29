#include "../hyper.h"

// Twisted S2xE.

// We use a model with coordinates (r,phi,z), where (r,phi) are the polar coordinates in S2.

// Metric: ds^2 = (dr)^2 + (sin r * dphi)^2 + (dz + K * (1-cos(r)) dphi)^2

// See https://youtu.be/lZCkEuud6aU and https://youtu.be/rfu6m_xGxWY


namespace hr {

EX namespace ts2 {

  eGeometry ts2 = eGeometry(-1);
  
  EX bool in() { return geometry == gTS; }

  EX ld K = -.9;
  
  EX hyperpoint at = point3(.5, 0, 0);
  EX transmatrix camera;
  
  void init() {
    ld r = .5;
    at = point3(r, 0, 0);
    camera = build_matrix(
      point3(1, 0, 0),
      point3(0, 1/sin(r), K/sin(r)*(cos(r)-1)),
      point3(0, 0, 1),
      point31(0,0,0)
      );
    camera = camera * cspin(1, 2, 90*degree);
    }

  // a dummy map that does nothing
  struct hrmap_ts2 : hrmap {
    heptagon *origin;
    heptagon *getOrigin() override { return origin; }
    
    struct transmatrix relative_matrix(heptagon *h2, heptagon *h1, const hyperpoint& hint) override {
      return Id;
      }
    
    hrmap_ts2() {
      init();
      heptagon*& h =origin;
      h = tailored_alloc<heptagon> (S7);
      h->c7 = newCell(S7, h);
      h->distance = 0;
      h->dm4 = 0;
      h->fieldval = 0;
      h->cdata = NULL;
      h->alt = NULL;
      }

    heptagon *create_step(heptagon *parent, int d) override {
      parent->c.connect(d, parent, d, false);
      return parent;
      }

    void draw() override {
      println(hlog, "at = ", at);
      for(int i=0; i<3; i++)
        println(hlog, i, ": ", camera * point3(i==0, i==1, i==2));
      }
    };
  
  EX hrmap* new_map() { return new hrmap_ts2; };
  
  EX hyperpoint christoffel(const hyperpoint at, const hyperpoint velocity, const hyperpoint transported) {
    const ld r = at[0];
    
    hyperpoint c = point3(0,0,0);
    // const ld r2 = r * r;
    const ld K2 = K * K;
    const ld sr = sin(r);
    // const ld sr2 = sr * sr;
    
    const ld cr = cos(r) - 1;
    const ld cr2 = cr * cr;
  
    c[ 0 ] = 0
     + velocity[ 1 ] * transported[ 1 ] *  (-K2*(cr) + cos(r))*sr
     + velocity[ 1 ] * transported[ 2 ] *  K*sr/2
     + velocity[ 2 ] * transported[ 1 ] *  K*sr/2;
    c[ 1 ] = 0
     + velocity[ 0 ] * transported[ 1 ] *  (K2*cos(r) - K2 - 2*cos(r))/(2*sr)
     + velocity[ 0 ] * transported[ 2 ] *  -K/(2*sr)
     + velocity[ 1 ] * transported[ 0 ] *  (K2*cos(r) - K2 - 2*cos(r))/(2*sr)
     + velocity[ 2 ] * transported[ 0 ] *  -K/(2*sr);
    c[ 2 ] = 0
     + velocity[ 0 ] * transported[ 1 ] *  K*(K2 - 1)*cr2/(2*sr)
     + velocity[ 0 ] * transported[ 2 ] *  K2*(1 - cos(r))/(2*sr)
     + velocity[ 1 ] * transported[ 0 ] *  K*(K2 - 1)*cr2/(2*sr)
     + velocity[ 2 ] * transported[ 0 ] *  K2*(1 - cos(r))/(2*sr);
  
    return c;
    }
  
void geodesic_step(hyperpoint& at, hyperpoint& velocity) {
  auto acc = ts2::christoffel(at, velocity, velocity);
  
  auto at2 = at + velocity / 2;
  auto velocity2 = velocity + acc / 2;
  
  auto acc2 = ts2::christoffel(at2, velocity2, velocity2);
  
  at = at + velocity + acc2 / 2;
  
  velocity = velocity + acc;
  }

EX bool shift_view(hyperpoint dist) {

  if(!in()) return false;
  
  auto tPos = transpose(camera);
  hyperpoint h = camera * dist;
    
  int steps = 100;
  h /= steps;
  
  for(int i=0; i<steps; i++) {
    for(int j=0; j<3; j++)
      tPos[j] += christoffel(at, h, tPos[j]);
    geodesic_step(at, h);
    }
                                                                                  
  camera = transpose(tPos);
  return true;
  }

EX bool rotate_view(transmatrix T) {
  if(!in()) return false;
  camera = camera * inverse(T);
  return true;
  }

EX void radar() {
  hyperpoint a = at;
  hyperpoint v = camera * point3(0,0,1) / 100.;

  int it;
  for(it=0; it<1000; it++) {
    geodesic_step(a, v);  
    // if(a[0] < .1) break;

    if(a[0] > .5 && a[0] < .6 && cos(a[1]) > .9 && cos(a[2]) > .9) break;
    }
  
  println(hlog, "radar = ", it);
  }

void twist() {

  for(K = -2; K<=2; K += .1) 
  for(ld start: {0.5f, 1.f}) {
    hyperpoint at = point3(start, 0, 0);
    hyperpoint vel = point3(0, 1e-5, 0);
    
    // println(hlog, "simulating");
    
    hyperpoint at1 = at, at2 = at;
      
    int it = 0;
    for(; ; it++) {
      at2 = at1; at1 = at;
      // if(it % 1000 == 0) println(hlog, format("%6d. ", it), at, " vel = ", vel);
      geodesic_step(at, vel);

      if(at2[0] > at1[0] && at1[0] < at[0]) break;
      }
    
    println(hlog, format("%8d. ", it), lalign(40, kz(at)), " vel = ", lalign(40, kz(vel)), " K = ", K);
    }
  }
  
int readArgs() {
  using namespace arg;
           
  if(0) ;
  else if(argis("-ts2")) {
    if(ts2 == (eGeometry)(-1)) {
      ts2 = (eGeometry) isize(ginf);
      ginf.push_back(geometryinfo{
        "TS",     "none",    "TS",                                         "ts",      1, 1, qEXPERIMENTAL | qRAYONLY,    giSphere3, 0x31400, {{7, 2}}, eVariation::pure
        });
      }
    set_geometry(ts2);
    }
  else if(argis("-twist")) {
    twist();
    exit(1);
    }
  else return 1;
  return 0;
  }

auto fundamentalhook = addHook(hooks_args, 100, readArgs);

EX string fragmentshader() {
  return
    "varying mediump vec4 at;\n"
    "uniform mediump vec4 uStart;\n"
    "uniform mediump mat4 uLP;\n"

    "const float K = float(" + fts(ts2::K) + ");\n"
    "const float maxdist = 20.;\n"

    "vec4 christoffel(vec4 at, vec4 vel) {\n"
       "float r = at.x;\n"

       "float K2 = K * K;\n"
       "float sr = sin(r);\n"
       "float cr = cos(r) - 1.;\n"
       "float cr2 = cr * cr;\n"
       
       "vec4 c;\n"

"c[ 0 ] = 0.\n"
 "+ vel.y * vel.y *  (-K2*(cr) + cos(r))*sr\n"
 "+ vel.y * vel.z *  K*sr/2.\n"
 "+ vel.z * vel.y *  K*sr/2.;\n"
"c[ 1 ] = 0."
 "+ vel.x * vel.y *  (K2*cos(r) - K2 - 2.*cos(r))/(2.*sr)\n"
 "+ vel.x * vel.z *  -K/(2.*sr)\n"
 "+ vel.y * vel.x *  (K2*cos(r) - K2 - 2.*cos(r))/(2.*sr)\n"
 "+ vel.z * vel.x *  -K/(2.*sr);\n"
"c[ 2 ] = 0."
 "+ vel.x * vel.y *  K*(K2 - 1.)*cr2/(2.*sr)\n"
 "+ vel.x * vel.z *  K2*(1. - cos(r))/(2.*sr)\n"
 "+ vel.y * vel.x *  K*(K2 - 1.)*cr2/(2.*sr)\n"
 "+ vel.z * vel.x *  K2*(1. - cos(r))/(2.*sr);\n"

"return c;}\n"
    
    "void main() {\n"
    
    "  mediump vec4 at0 = at;\n"
    "  at0.y = -at.y;\n"
    "  at0.xyz = at0.xyz / length(at0.xyz);\n"
    
    "  mediump vec4 position = uStart;\n"
    "  mediump vec4 tangent = uLP * at0;\n"
    
    "  tangent = tangent;\n"
    "  float dist = 0.;\n"
    "  int iter = 0;"

    "  while(dist < maxdist && iter < 10000) {"

// we make smaller steps if we are close to the singularities at the poles
    
    "     float step = sin(position.x) * .05;"
    "     dist = dist + step;\n"
    "     iter++;\n"
    "     tangent = tangent * step;\n"

    "     vec4 acc = christoffel(position, tangent);"
    "     vec4 at2 = position + tangent / 2.;"
    "     vec4 tangent2 = tangent + acc / 2.;"
    "     vec4 acc2 = christoffel(at2, tangent2);"
    "     position = position + tangent + acc2 / 2.;"
    "     tangent = tangent + acc;\n"
    "     tangent = tangent / step;\n"
    "     if(position.x > .5 && position.x < .6 && cos(position.y) > .9 && cos(position.z/2./K) > .9) {\n"
    "       float bri = float(1. - dist / maxdist);\n"
    "       int e = 0;\n"
    "       if(position.x < .51 || position.x > .59) e++;\n"
    "       if(cos(position.y) < .91) e++;\n"
    "       if(cos(position.z/2./K) < .91) e++;\n"
    "       if(e >= 2) bri /= 2.;\n"
    "       gl_FragColor = vec4(bri, bri, bri, 1.);\n"
    "       return;"
    "       }"
    "     }\n"
    
    "  gl_FragColor = vec4(0.,0.,0.,1.);\n"
    
    "  }";
  }

int ah2 = addHook(ray::hooks_rayshader, 100, [] (string &vsh, string &fsh) { fsh = ts2::fragmentshader(); })
  + addHook(ray::hooks_rayset, 100, [] (shared_ptr<ray::raycaster> o) {
      if(!in()) return false;
      glUniformMatrix4fv(o->uLP, 1, 0, glhr::tmtogl_transpose3(ts2::camera).as_array());
      auto pg = glhr::pointtogl(ts2::at);
      glUniform4f(o->uStart, pg[0], pg[1], pg[2], pg[3]);
      return true;
      })
  + addHook(hooks_newmap, 100, [] () { if(in()) return ts2::new_map(); return (hrmap*) nullptr; })
  + addHook(hooks_rotate_view, 100, rotate_view)
  + addHook(hooks_shift_view, 100, shift_view)
  ;


EX }

}

