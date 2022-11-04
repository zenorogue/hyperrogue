#include "rogueviz.h"

namespace rogueviz {

namespace collatz {

  double s2, s3, p2, p3;
  double cshift = -1;
  
  transmatrix T2, T3;
  
  edgetype *collatz1, *collatz2;
  
  void act(vertexdata& vd, cell *c, shmup::monster *m, int i);
  void lookup(long long reached, int bits);
  void collatz_video(const string &fname);

  void start() {
    init(RV_GRAPH);
    collatz1 = add_edgetype("1");
    collatz2 = add_edgetype("2");
    vdata.resize(1);
    vertexdata& vd = vdata[0];
    createViz(0, cwt.at, xpush(cshift));
    virtualRebase(vd.m);
    vd.cp = dftcolor;
    vd.data = 0;
    addedge(0, 0, 1, false, collatz::collatz1);
    vd.name = "1";
    storeall();

    T2 = spin(collatz::s2) * xpush(collatz::p2);
    T3 = spin(collatz::s3) * xpush(collatz::p3);

    rv_hook(hooks_drawvertex, 100, act);
    rv_hook(hooks_args, 100, [] {
      using namespace arg;

      if(0) ;

      #if CAP_SHOT
      else if(argis("-rvvideo")) {
        shift(); collatz_video(arg::args());
        }
      #endif
    
      else if(argis("-collatz-go")) {
        shift(); int i = argi(); shift(); int j = argi();
        if(i <= 0) i = 763;
        if(j < 0 || j > 61) j = 61;
        collatz::lookup(i, j);
        }
      
      else return 1;
      
      return 0;
      });
    }
  
  void lookup(long long reached, int bits) {
    while(reached < (1ll<<bits)) {
      if(reached%3 == 2 && (2*reached-1) % 9 && hrand(100) < 50)
        reached = (2*reached-1) / 3;
      else reached *= 2;
      }
    println(hlog, "reached = ", llts(reached));
    vector<string> seq;
    while(reached>1) { 
      seq.push_back(llts(reached));
      if(reached&1) reached += (reached>>1)+1;
      else reached >>= 1;
      }
    // seq.push_back("1");
    reverse(seq.begin(), seq.end());
    
    int id = 0;
    int next = 0;
    
    int steps = 0;
    while(true) {
      steps++;
      if(std::isnan(View[0][0])) exit(1);
      shmup::turn(100);
      drawthemap();
      centerpc(100); optimizeview();
      fixmatrix(View);
      bfs(); setdist(cwt.at, 7 - getDistLimit() - genrange_bonus, NULL);
      vertexdata& vd = vdata[id];
      for(int e=0; e<isize(vd.edges); e++) {
        int id2 = vd.edges[e].first;
        if(vdata[id2].name == seq[next]) {
          id = id2; next++;
          cwt.at = vdata[id2].m->base;
          if(shmup::on) shmup::pc[0]->base = cwt.at;
          if(next == isize(seq)) goto found;
          }
        }
      }
    
    found:
    printf("steps = %d\n", steps);    
    }

  void act(vertexdata& vd, cell *c, shmup::monster *m, int i) {
    if(c->cpdist > 7 && euclid) ;
    else if(vd.data == 2) {
      // doubler vertex
      string s = vd.name;
      colorpair cp = vd.cp;
      vd.data = 20;
      int i0 = isize(vdata);
      vdata.resize(i0+1);
      vertexdata& vdn = vdata[i0];
      createViz(i0, m->base, m->at * collatz::T2);
      
      virtualRebase(vdn.m);
      vdn.cp = perturb(cp);
      vdn.data = 0;
      addedge(i, i0, 1, false, collatz::collatz1);
      vdn.m->store();
      int carry = 0;
      string s2 = s;
      for(int i=isize(s2)-1; i>=0; i--) {
        int x = 2*(s2[i] - '0') + carry;
        carry = x>=10;
        if(carry) x-=10;
        s2[i] = '0'+x;
        }
      if(carry) s2 = "1" + s2;
      vdn.name = s2;
      
      int m3 = 0;
      for(int i=0; i<isize(s); i++) m3 += s[i] - '0';
      
      if(m3 % 3 == 2 && s != "2" && s != "1") {
        vdata.resize(i0+2);
        vertexdata& vdn = vdata[i0+1];
        createViz(i0+1, m->base, m->at * collatz::T3);
        virtualRebase(vdn.m);
        vdn.cp = perturb(cp);
        vdn.data = 0;
        addedge(i, i0+1, 1, false, collatz::collatz2);
        vdn.m->store();
        int carry = -1;
        string s2 = s;
        for(int i=isize(s2)-1; i>=0; i--) {
          carry += 2 * (s2[i] - '0');
          int ncarry = 0;
          while(carry % 3) carry += 10, ncarry--;
          if(carry >= 30) carry -= 30, ncarry += 3;
          s2[i] = '0'+carry/3;
          carry = ncarry;
          }
        if(s2[0] == '0') s2 = s2.substr(1);
        vdn.name = s2;
        vdn.cp = perturb(vdn.cp);
        }
      }
    else if(vd.data < 2) {
      vd.data++;
      fixmatrix(vd.m->at);
      }
    }

#if CAP_SHOT

// see: https://www.youtube.com/watch?v=4Vu3F95jpQ4&t=6s (Collatz)
void collatz_video(const string &fname) {
  if(true) {
    sightrange_bonus = 3;
    genrange_bonus = 3;
    dronemode = true; pconf.camera_angle = -45; rog3 = true; patterns::whichShape = '8';
    vid.aurastr = 512;
    
    collatz::lookup(763, 60);

    history::create_playerpath(), models::rotation = 1;
    // pmodel = mdBand;

#define STORYCOUNT 24
#define T(m,ss) (60*24*(m)+24*(ss))
#define FRAMECOUNT T(4,55)

printf("framecount = %d\n", FRAMECOUNT);

struct storydata { int s; int e; const char *text; } story[] = {
  {T(0,14), T(0,17), "I am flying above a tree of numbers."},
  {T(0,17), T(0,20), "It starts with the number 1."},
  {T(0,20), T(0,23), "Each number n branches left to 2n."},
  {T(0,23), T(0,28), "And it branches right to (2n-1)/3 if possible."},

  {T(1, 8), T(1,11), "What I am flying above is not a plane."},
  {T(1,11), T(1,14), "It is not a sphere either."},
  {T(1,14), T(1,17), "To be honest, the space I live in..."},
  {T(1,17), T(1,20), "...is not even Euclidean."},
  
  {T(2,12), T(2,15), "Look, angles of a triangle add up to..."},
  {T(2,15), T(2,18), "...less than 180 degrees in this world."},
  {T(2,18), T(2,21), "6/7 of 180 degrees, to be exact."},
  {T(2,21), T(2,24), "Do you see the regular heptagons?"},
  {T(2,36), T(2,42), "And all these lines are straight."},
  
  {T(3, 8), T(3,11), "Lots of space in my world."},
  {T(3,11), T(3,14), "In 105 steps from the root..."},
  {T(3,14), T(3,17), "...there are trillions of numbers."},
  {T(3,17), T(3,20), "That would not fit in your world."},

  {T(4,0),  T(4,3),  "Is every positive number somewhere in the tree?"},
  {T(4,3),  T(4,6),  "Your mathematicians do not know this yet."},
  {T(4,6),  T(4,10), "Will you find the answer?"},
  
  {T(4,44), T(4,54), "music: Ambient Flow, by Indjenuity"},
  
  {T(2,6),  T(2,27), "@triangles"},
  {T(2,27), T(2,42), "@network"},
  
  {0, T(0,7), "@fi"},
  {T(4,48), T(4,55), "@fo"},
  
  {0,0,NULL}
  };

    int drawtris=0, drawnet=0;
        
    for(int i=0; i<FRAMECOUNT; i++) {
      const char *caption = NULL;
      int fade = 255;
      
      bool dt = false, dn = false;
      
      for(int j=0; story[j].text; j++) if(i >= story[j].s && i <= story[j].e) {
        if(story[j].text[0] != '@')
          caption = story[j].text;
        else if(story[j].text[1] == 't')
          dt = true;
        else if(story[j].text[1] == 'n')
          dn = true;
        else if(story[j].text[2] == 'i')
          fade = 255 * (i - story[j].s) / (story[j].e-story[j].s);
        else if(story[j].text[2] == 'o')
          fade = 255 * (story[j].e - i) / (story[j].e-story[j].s);
        }
      
      if(dt && drawtris < 255) drawtris++;
      else if(drawtris && !dt) drawtris--;

      linepatterns::patZebraTriangles.color = 0x40FF4000 + drawtris;
      
      if(dn && drawnet < 255) drawnet++;
      else if(drawnet && !dn) drawnet--;

      linepatterns::patZebraLines.color = 0xFF000000 + drawnet;
      
      vid.grid = drawnet;
      
      history::phase = 1 + (isize(history::v)-3) * i * .95 / FRAMECOUNT;
      history::movetophase();

      char buf[500];
      snprintf(buf, 500, fname.c_str(), i);
      
      if(i == 0) drawthemap();
      shmup::turn(100);
      printf("%s\n", buf);
      shot::shoty = 1080; shot::shotx = 1920;
      shot::caption = caption;
      shot::fade = fade;
      shot::take(buf);
      }
  
    return;
    }
  }
#undef T
#endif

string its05(int i) { char buf[64]; sprintf(buf, "%05d", i); return buf; }

int dimid(char x) {
  if(x >= 'a' && x < 'a' + GDIM) return x - 'a';
  else if(x >= '0' && x < '0' + GDIM) return x - '0';
  else if(x >= 'x' && x < 'x' + GDIM) return x - 'x';
  else {
    println(hlog, "incorrect dimension ID");
    throw hr_exception();
    }
  }

int readArgs() {
#if CAP_COMMANDLINE
  using namespace arg;

  if(0) ;

  else if(argis("-collatz")) {
    PHASE(3); 
    using namespace collatz; 
    shift(); sscanf(argcs(), "%lf,%lf,%lf,%lf", &s2, &p2, &s3, &p3);
    start();
    }

  else if(argis("-collatz3")) {
    PHASE(3); 
    using namespace collatz; 
    s2 = p2 = s3 = p3 = 0;
    start();
    transmatrix *T = &T2;
    while(true) {
      lshift();
      if(arg::nomore()) break;
      else if(argis("fd")) { shift(); *T = *T * xpush(argf()); }
      else if(argcs()[0] == 't') { int x = dimid(argcs()[1]); int y = dimid(argcs()[2]); shift(); *T = *T * hr::cspin(x, y, argf()); }
      else if(argis("/")) { if(T == &T2) T = &T3; else break; }
      else break;
      }
    unshift();
    }

  else if(argis("-cshift")) {
    shift_arg_formula(collatz::cshift);
    }
  else return 1;
#endif
  return 0;
  }

int ah = addHook(hooks_args, 100, readArgs) +
  addHook_rvslides(42, [] (string s, vector<tour::slide>& v) {
    if(s != "data") return;
    using namespace tour;
    v.push_back(
      tour::slide{"Collatz conjecture", 51, LEGAL::UNLIMITED | QUICKGEO,
    "The following slide is a visualization of the Collatz conjecture. "
    "Press '5' for a spiral rendering of the Collatz conjecture visualization.\n\n"
    "Note that this, and many other RogueViz visualizations, have "
    "Euclidean versions (press ESC).\n",
    pres::roguevizslide('d', [] () {
      rogueviz::dftcolor = 0x206020FF;
      
      int fac = euclid ? 2 : 1;

      rogueviz::collatz::s2 = .3;
      rogueviz::collatz::p2 = .5 * fac;
      rogueviz::collatz::s3 = -.4;
      rogueviz::collatz::p3 = .4 * fac;

      rogueviz::showlabels = true;
      
      gmatrix.clear();
      drawthemap();
      gmatrix0 = gmatrix;

      rogueviz::collatz::start();
      }, [] (presmode m) { slide_url(m, 'y', "YouTube link", "https://www.youtube.com/watch?v=NqPUwA_A0_k"); })
    });
    });
  

EX }

}
