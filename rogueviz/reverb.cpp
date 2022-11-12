#include "rogueviz.h"

/** \brief Non-Euclidean reverb (and also Doppler effect)
 *
 *  Compile with HyperRogue, run with -geo [geometry] -reverb sounds/seen-eagle.ogg (e.g. -geo 534h -reverb)
 *
 *  you can also supply filename.raw in raw audio format (signed 16 bit, two channels; frequency set by -rev-freq).
 *
 *  Press oo to configure the physical parameters.
 *
 *  This https://twitter.com/ZenoRogue/status/1265297322369581057 has been created with:
 *

./hyper -debf g -geo 534h -ray-do -ray-reflect .5 -sight3 6 -smartlimit 200000 -genlimit 200000 -reverb sounds/seen-eagle.ogg -rev-abs 0.2 -shott 0 -shotxy 500 500 -animperiod 20000 -rev-anim -animvideo 1200 caw.mp4 -exit
ffmpeg -i caw.mp4 -f s16le -ar 44100 -ac 2 -i raw-audio.raw -c:v copy caw-hyperbolic.mp4

./hyper -debf g -geo beti -ray-do -ray-reflect .5 -smartlimit 200000 -genlimit 200000 -reverb sounds/seen-eagle.ogg -rev-abs 0.2 -shott 0 -shotxy 500 500 -animperiod 20000 -rev-anim -animvideo 1200 caw.mp4 -exit
ffmpeg -i caw.mp4 -f s16le -ar 44100 -ac 2 -i raw-audio.raw -c:v copy caw-euclidean.mp4

./hyper -debf g -geo 16c -ray-do -ray-reflect .5 -smartlimit 200000 -genlimit 200000 -reverb sounds/seen-eagle.ogg -rev-abs 0.2 -shott 0 -shotxy 500 500 -animperiod 20000 -rev-anim -animvideo 1200 caw.mp4 -exit
ffmpeg -i caw.mp4 -f s16le -ar 44100 -ac 2 -i raw-audio.raw -c:v copy caw-spherical.mp4

./hyper -debf g -geo 344h -ray-do -ray-reflect .6 -sight3 6 -smartlimit 200000 -genlimit 200000 -reverb sounds/seen-eagle.ogg -rev-abs 0.2 -shott 0 -shotxy 500 500 -animperiod 20000 -rev-anim -animvideo 1200 caw.mp4 -exit
ffmpeg -i caw.mp4 -f s16le -ar 44100 -ac 2 -i raw-audio.raw -c:v copy caw-hyperbolic-ideal.mp4

./hyper -debf g -geo 534h -ray-do -ray-reflect .5 -sight3 6 -smartlimit 200000 -genlimit 200000 -reverb sounds/seen-eagle.ogg -rev-abs 0.2 -rev-ss 4 -shott 0 -shotxy 500 500 -animperiod 40000 -rev-anim -animvideo 2400 caw.mp4 -exit
ffmpeg -i caw.mp4 -f s16le -ar 44100 -ac 2 -i raw-audio.raw -c:v copy caw-hyperbolic-doppler.mp4

 *
 **/

namespace rogueviz {

namespace embed {

int freq = 44100;

bool auto_anim = false;

bool in = false;

bool started = false;

struct sample {
  Sint16 left, right;
  Sint16& operator [] (int i) { return (&left) [i]; }
  };

/** @brief original audio data */
vector<sample> orig;

/** @brief original size of orig */
int orig_size;

int current_sample = 0, prevt = 0, curt = 0;

std::mutex lock;

/** @brief controls the volume */
ld maxsnd = 1;

/** @brief 0 = no absorption on walls, 1 = full absorption */
ld absorption = .1;

/** @brief how much time does it take to go 1 absolute unit, in seconds */
ld speed_of_sound = .25;

/** @brief inter-aural distance */
ld iad = .05;

vector<sample> to_play;

void myAudio(void *userdata, Uint8* stream, int len) {
  if(isize(to_play) < current_sample + len) return;
  if(inHighQual) return;
  sample* samples = (sample*) stream;
  len /= sizeof(sample);

  lock.lock();
  for(int i=0; i<len; i++) {
    samples[i] = to_play[current_sample++];
    }
  
  lock.unlock();
  }

void start_audio() {
  SDL_AudioSpec spec;
  spec.freq = freq;
  spec.format = AUDIO_S16SYS;
  spec.channels = 2;
  spec.samples = 4096;
  spec.callback = myAudio;
  
  SDL_CloseAudio();

  if(SDL_OpenAudio(&spec, NULL) != 0) {
    println(hlog, "OpenAudio: ", SDL_GetError());
    }
  else {
    println(hlog, "Initialized audio ", tie(spec.freq, spec.channels, spec.samples));
    SDL_PauseAudio(0);
    }
  started = true;
  }

int frameid = 10;

struct cellinfo {
  int lastframe;
  int curframe;
  array<ld, 2> lastdist;
  array<ld, 2> curdist;
  };

map<cell*, cellinfo> infos;

vector<array<double, 2> > sndbuffer;

/** after each frame, write the simulated sound to sndbuffer and to_play */

void reverb_queue() {
  prevt = curt;
  int& used_ticks = inHighQual ? ticks : sc_ticks;
  
  curt = (used_ticks * (long long)(freq)) / 1000;
  if(prevt > curt) prevt = curt;
  if(curt - prevt > freq) return;

  sndbuffer.resize(curt, {0, 0});
  
  for(auto& ps: infos) {
    auto& p = ps.second;
    if(p.curframe != frameid) continue;
    if(p.lastframe != p.curframe-1)
      p.lastdist = p.curdist;
    int dist = celldistance(ps.first, cwt.at);
    // if(ps.first == cwt.at) println(hlog, (p.curdist - p.lastdist) / (curt - prevt));

    if(dist > (sphere?3:2) && !inHighQual) continue;
    
    for(int s=0; s<(sphere?10:1); s++) {
    
    ld dist1 = dist + 3 * s;

    ld base = pow(1-absorption, dist1);
    
    ld att0[2];
    ld att1[2];
    
    /* no need to add abs or pi*s to sin */

    for(int ch=0; ch<2; ch++) {
      att0[ch] = base / sin_auto(p.lastdist[ch]);
      att1[ch] = base / sin_auto(p.curdist[ch]);
      if(att0[ch] > 5) println(hlog, att0[ch], "capped to 5");
      if(att0[ch] > 5) att0[ch] = 5;
      if(att1[ch] > 5) att1[ch] = 5;
      }
    
    for(int ch: {0,1})
      for(int i=prevt; i<curt; i++) {
        ld a = ilerp(prevt, curt, i);
        ld d = lerp(p.lastdist[ch], p.curdist[ch], a) + M_PI * s;
        int tim = (i - d * freq * speed_of_sound);
        tim %= isize(orig);
        if(tim < 0) tim += isize(orig);
          sndbuffer[i][ch] += orig[tim][ch] * lerp(att0[ch], att1[ch], a);
        }
    p.lastframe = p.curframe;
    p.lastdist = p.curdist;
    }
    }

  for(int i=prevt; i<curt; i++) for(int ch: {0,1})
    if(sndbuffer[i][ch] > maxsnd) maxsnd = sndbuffer[i][ch];

  frameid++;
  
  lock.lock();
  to_play.resize(curt);
  for(int i=prevt; i<curt; i++) for(int ch: {0,1})
    to_play[i][ch] = sndbuffer[i][ch] / maxsnd * 30000;
  lock.unlock();
  }

int maxvol = 1;

/** draw bird, and also record the distance data about cell c */
bool draw_bird(cell *c, const transmatrix& V) {

  if(!in) return false;

  if(!started) start_audio();
  
  if(c == cwt.at) {
    int& used_ticks = inHighQual ? ticks : sc_ticks;
    int nextt = (used_ticks * (long long)(freq)) / 1000;
    ld tot = 0;
    nextt %= isize(orig);
    int id = curt % isize(orig);
    while(id != nextt) {
      tot = max<int>(tot, max<int>(abs<int>(orig[id][0]), abs<int>(orig[id][1]))); 
      id++; if(id == isize(orig)) id = 0;
      }
    id = tot * WINGS / maxvol / 2;
    queuepoly(rgpushxto0(tC0(V)) * cspin90(0, 2) * cspin90(1, 2) * cspin(0, 2, 45._deg),
       GDIM == 3 ? cgi.shAnimatedTinyEagle[id] : cgi.shTinyBird, 0xFFFFFFFF
       ); 
    }
  
  auto& ci = infos[c];
  ci.curframe = frameid;
  ci.curdist[0] = hdist0(xpush(-iad) * tC0(V));
  ci.curdist[1] = hdist0(xpush(+iad) * tC0(V));  

  return false;
  }

static int isor;

void show() {
  cmode = sm::SIDE | sm::MAYDARK;
  gamescreen(0);
  dialog::init(XLAT("reverb"), 0xFFFFFFFF, 150, 0);

  dialog::addSelItem("speed of sound", "1/" + fts(speed_of_sound), 's');
  dialog::add_action([]() {
    dialog::editNumber(speed_of_sound, 0, 1, .1, .01, "time to travel 1 absolute unit", "");
    });

  dialog::addSelItem("absorption", fts(absorption), 'a');
  dialog::add_action([]() {
    dialog::editNumber(absorption, 0, 1, .1, .01, "absorption", "");
    });

  dialog::addSelItem("resynchronize", fts((current_sample - curt) * 1. / freq), 'r');
  dialog::add_action([]() {
    current_sample = curt;
    });

  dialog::addSelItem("inter-aural distance", fts(iad), 'i');
  dialog::add_action([]() {
    dialog::editNumber(iad, 0, 1, .1, .01, "inter-aural distance", "");
    });

  dialog::addSelItem("adjust volume", fts(maxsnd), 'v');
  dialog::add_action([]() {
    dialog::editNumber(maxsnd, 1, 1e6, .1, 1, "max volume", "large number -> more silent; will increase automatically if too loud");
    });

  isor = isize(orig);
  dialog::addSelItem("sample length", its(isize(orig)) + "/" + its(freq), 'l');
  
  dialog::add_action([]() {
    dialog::editNumber(isor, orig_size, orig_size*2, orig_size / 10, orig_size, "sample length", "warning: sample is cut off if you shorten it and then lengthen it again");
    dialog::reaction = [] {
      orig.resize(isor);
      };
    });

  dialog::addBoolItem_action("auto-loop", auto_anim, 'o');

  dialog::addBack();
  dialog::display();    
  }

void o_key(o_funcs& v) {
  v.push_back(named_dialog("reverb", show));
  }

void save_raw_audio() {
  if(in) {
    /* save the output as raw audio file */
    /* (it can be added to the video using ffmpeg */
    FILE *f = fopen("raw-audio.raw", "wb");

    for(int i=0; i<curt; i++) for(int ch: {0,1})
      to_play[i][ch] = sndbuffer[i][ch] / maxsnd * 30000;

    fwrite(&to_play[0], 4, to_play.size(), f);
    fclose(f);
    }
  }

auto hchook = addHook(hooks_drawcell, 100, draw_bird)

+ addHook(hooks_frame, 100, reverb_queue)

+ addHook(hooks_o_key, 80, o_key)

+ addHook(anims::hooks_after_video, 80, save_raw_audio)

+ addHook(anims::hooks_anim, 100, [] {
  if(!auto_anim) return;
  if(cgi.cellshape.empty()) return;
  hyperpoint h1 = cgi.cellshape[0];
  hyperpoint h2 = normalize(cgi.cellshape[0] + cgi.cellshape[1]);
  hyperpoint wc = Hypc;
  for(int i=0; i<cgi.face; i++) wc += cgi.cellshape[i];
  hyperpoint h3 = normalize(wc);
  if(cgflags & qIDEAL) {  
    println(hlog, "h1 was: ", h1);
    h1 = rspintox(h1) * xpush0(hdist0(h2)*4);
    println(hlog, "h1 is: ", h1);
    }
  hyperpoint h4 = mid(h3, C0);
  println(hlog, "distances: ", make_tuple(hdist0(h1), hdist0(h2), hdist0(h3), hdist0(h4)));
  vector<hyperpoint> all = {h1, h2, h3, h4, h1, h1};
  ld id = parseld("0../0..1../0..|1../0..2../0..|2../0..3../0..|3../0..4../0");
  // println(hlog, "d = ", id);
  hyperpoint h = all[int(id)] * (1-id+int(id)) + all[int(id+1)] * (id-int(id));
  h = normalize(h);
  centerover = currentmap->gamestart();
  View = /* cspin90(2, 0) * rspintox(gpushxto0(h) * C0) * */ gpushxto0(h);
  View = spintox(View * C0) * View;
  View = cspin90(2, 0) * View;

  shift_view(point3(0, 0, -1e-2));

  anims::moved();
  })

+ addHook(hooks_args, 100, [] {
  using namespace arg;
           
  if(0) ;
  else if(argis("-rev-abs")) {
    shift_arg_formula(absorption);
    }
  else if(argis("-rev-ss")) {
    shift_arg_formula(speed_of_sound);
    }
  else if(argis("-rev-iad")) {
    shift_arg_formula(iad);
    }
  else if(argis("-rev-freq")) {
    shift(); freq = argi();
    }
  else if(argis("-rev-anim")) {
    auto_anim = true;
    }
  else if(argis("-reverb")) {
    shift();
    string fname = args();
    
    if(fname.substr(isize(fname) - 4) == ".raw") {
      FILE *f = fopen(fname.c_str(), "rb");
      if(!f) {
        printf("failed to load\n");
        return 1;
        }
      fseek(f, 0, SEEK_END);
      orig.resize(ftell(f)/sizeof(sample));
      fseek(f, 0, SEEK_SET);
      fread(&orig[0], 4, orig.size(), f);
      fclose(f);
      }
    
    else {
      Mix_CloseAudio();
      Mix_OpenAudio(freq, AUDIO_S16LSB, 2, 4096);
      auto chunk = Mix_LoadWAV(fname.c_str());
      if(!chunk) {
        printf("failed to load\n");
        return 1;
        }
      orig.resize(chunk->alen / 4);
      memcpy(&orig[0], chunk->abuf, chunk->alen);
      Mix_FreeChunk(chunk);
      }
    
    orig_size = isize(orig);

    for(auto& o: orig) {
      maxvol = max(maxvol, abs<int>(o[0]));
      maxvol = max(maxvol, abs<int>(o[1]));
      }
    
    in = true;
    enable_canvas();
    patterns::whichCanvas = 'r';
    patterns::rwalls = 100;
    mapeditor::drawplayer = false;
    start_game();
    if(!cgi.cellshape.empty())
      println(hlog, "edge = ", hdist(cgi.cellshape[0], cgi.cellshape[1]));
    /* Doppler effect is weird if scrolling if not smooth */
    smooth_scrolling = true;
    /* disable the frustum culling (we need sound from every direction) */
    frustum_culling = false;
    }

  /* auto-sync sample length with animation period */
  else if(argis("-rev-length-auto")) {
    int ap = anims::period / 1000. * freq;
    ld d = ap / isize(orig);
    println(hlog, "d = ", d);
    int di = d;
    if(di) {
      int size_to = ap / di;
      orig.resize(size_to);
      }
    }
  
  /* sample length in seconds */
  else if(argis("-rev-length")) {
    shift();
    orig.resize(argi() * freq);
    }
  else return 1;
  return 0;
  });


}
}
