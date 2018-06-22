// RogueViz -- source code for creating videos and animations
// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

namespace rogueviz {

#if CAP_SDL

// see: https://www.youtube.com/watch?v=4Vu3F95jpQ4&t=6s (Collatz)
// see: https://www.youtube.com/watch?v=mDG3_f8R2Ns (SAG boardgames)
// see: https://www.youtube.com/watch?v=WSyygk_3j9o (SAG roguelikes)
// see: https://www.youtube.com/watch?v=HWQkDkeEUeM (SAG programming languages)

void rvvideo(const string &fname) {
  if(kind == kCollatz) {
    pngformat = 2;
    sightrange_bonus = 3;
    genrange_bonus = 3;
    dronemode = true; vid.camera_angle = -45; rog3 = true; patterns::whichShape = '8';
    vid.aurastr = 512;
    long long reached = 763ll;
    while(reached < (1ll<<60)) {
      if(reached%3 == 2 && (2*reached-1) % 9 && hrand(100) < 50)
        reached = (2*reached-1) / 3;
      else reached *= 2;
      }
    printf("reached = %lld\n", reached);
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
      bfs(); setdist(cwt.c, 7 - getDistLimit() - genrange_bonus, NULL);
      vertexdata& vd = vdata[id];
      for(int e=0; e<isize(vd.edges); e++) {
        int id2 = vd.edges[e].first;
        if(vdata[id2].name == seq[next]) {
          id = id2; next++;
          cwt.c = shmup::pc[0]->base = vdata[id2].m->base;
          if(next == isize(seq)) goto found;
          }
        }
      }
    
    found:
    printf("steps = %d\n", steps);
    conformal::create(), conformal::rotation = 1;
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

      linepatterns::setColor(linepatterns::patZebraTriangles, 0x40FF4000 + drawtris);
      
      if(dn && drawnet < 255) drawnet++;
      else if(drawnet && !dn) drawnet--;

      linepatterns::setColor(linepatterns::patZebraLines, 0xFF000000 + drawnet);
      
      vid.grid = drawnet;
      
      conformal::phase = 1 + (isize(conformal::v)-3) * i * .95 / FRAMECOUNT;
      conformal::movetophase();

      char buf[500];
      snprintf(buf, 500, fname.c_str(), i);
      
      if(i == 0) drawthemap();
      shmup::turn(100);
      printf("%s\n", buf);
      pngres = 1080;
      saveHighQualityShot(buf, caption, fade);
      }
  
    return;
    }
  for(int i=0; i<1800; i++) {
    char buf[500];
    snprintf(buf, 500, fname.c_str(), i);
    shmup::pc[0]->base = currentmap->gamestart();
    shmup::pc[0]->at = spin(i * 2 * M_PI / (58*30.)) * xpush(1.7);
    if(i == 0) drawthemap();
    shmup::turn(100);
    if(i == 0) drawthemap();
    centerpc(100);
    printf("%s\n", buf);
    saveHighQualityShot(buf);
    }
  }

string its05(int i) { char buf[64]; sprintf(buf, "%05d", i); return buf; }

#define TSIZE 4096

// see: https://www.youtube.com/watch?v=HZNRo6mr5pk

void staircase_video(int from, int num, int step) {
  resetbuffer rb;
  renderbuffer rbuf(TSIZE, TSIZE, true);
  stereo::mode = stereo::sODS;

  for(int i=from; i<num; i+=step) { 
    ld t = i * 1. / num;
    t = pow(t, .3);
    staircase::scurvature = t * t * (t-.95) * 4;
    staircase::progress = i / 30.;
    
    staircase::strafex = (sin(i / 240.) - sin(i / 501.)) / 2.5;
    staircase::strafey = (cos(i / 240.) - cos(i / 501.)) / 2.5;
    
    staircase::make_staircase();

    rbuf.enable();
    dynamicval<int> vx(vid.xres, TSIZE);
    dynamicval<int> vy(vid.yres, TSIZE);
    dynamicval<int> vxc(vid.xcenter, TSIZE/2);
    dynamicval<int> vyc(vid.ycenter, TSIZE/2);
    stereo::set_viewport(0);
    printf("draw scene\n");
    rug::drawRugScene();
    
    IMAGESAVE(rbuf.render(), ("staircase/" + its05(i) + IMAGEEXT).c_str());
    printf("GL %5d/%5d\n", i, num);
    }
  
  rb.reset();
  }

#undef TSIZE

#define TSIZE 2048

// see: https://twitter.com/ZenoRogue/status/1001127253747658752
// see also: https://twitter.com/ZenoRogue/status/1000043540985057280 (older version)

void bantar_record() {
  resetbuffer rb;
  renderbuffer rbuf(TSIZE, TSIZE, true);

  int fr = 0;
  
  for(int i=0; i < 10000; i += 33) {
    if(i % 1000 == 999) i++;
    ticks = i;

    rbuf.enable();
    vid.xres = vid.yres = TSIZE;
    stereo::set_viewport(0);
    banachtarski::bantar_frame();
    
    IMAGESAVE(rbuf.render(), ("bantar/" + its05(fr) + IMAGEEXT).c_str());
    printf("GL %5d/%5d\n", i, 10000);
    fr++;
    }
  
  rb.reset();
  }
#undef TSIZE

#if CAP_COMMANDLINE
int videoArgs() {
  using namespace arg;
  if(argis("-rvvideo")) {
    shift(); rvvideo(arg::args());
    }
  else if(argis("-staircase_video")) {
    staircase_video(0, 128*30, 1); // goal: 168*30
    }
  else if(argis("-bantar_record")) {
    using namespace banachtarski;
    PHASE(3);
    peace::on = true;
    airmap.clear();
    ForInfos if(cci.second.c->monst == moAirElemental)
      cci.second.c->monst = moFireElemental;
    bantar_record();
    }
  else return 1;
  return 0;
  }
#endif
#endif

auto rv_hooks = addHook(hooks_args, 100, videoArgs);

}
