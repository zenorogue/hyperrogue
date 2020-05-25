#include "rogueviz.h"

/** \brief Non-Euclidean reverb (and also Doppler effect)
 *
 *  Compile with HyperRogue, run with -geo [geometry] -reverb filename.raw (e.g. -geo 534h -reverb)
 *
 *  filename.raw should be in raw audio format (44100Hz, signed 16 bit, two channels).
 *
 *  Press oo to configure the physical parameters.
 *
 **/

namespace rogueviz {

namespace embed {

bool in = false;

bool started = false;

struct sample {
  Sint16 left, right;
  Sint16& operator [] (int i) { return (&left) [i]; }
  };

/** original audio data */
vector<sample> orig;

int current_sample = 0, prevt = 0, curt = 0;

std::mutex lock;

/** controls the volume */
ld maxsnd = 1;

/** 0 = no absorption on walls, 1 = full absorption */
ld absorption = .1;

/** how much time does it take to go 1 absolute unit, in seconds */
ld speed_of_sound = .25;

/** inter-aural distance */
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
  spec.freq = 44100;
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
  
  curt = (used_ticks * (long long)(44100)) / 1000;
  if(prevt > curt) prevt = curt;
  if(curt - prevt > 44100) return;

  sndbuffer.resize(curt, {0, 0});
  
  for(auto& ps: infos) {
    auto& p = ps.second;
    if(p.curframe != frameid) continue;
    if(p.lastframe != p.curframe-1)
      p.lastdist = p.curdist;
    int dist = celldistance(ps.first, cwt.at);
    ld base = pow(1-absorption, dist);
    if(dist > 2 && !inHighQual) continue;
    // if(ps.first == cwt.at) println(hlog, (p.curdist - p.lastdist) / (curt - prevt));
    
    ld att0[2];
    ld att1[2];
    
    for(int ch=0; ch<2; ch++)
      att0[ch] = min<ld>(base / sin_auto(p.lastdist[ch]), 5);

    for(int ch=0; ch<2; ch++)
      att1[ch] = min<ld>(base / sin_auto(p.curdist[ch]), 5);

    for(int ch: {0,1})
      for(int i=prevt; i<curt; i++) {
        ld a = ilerp(prevt, curt, i);
        ld d = lerp(p.lastdist[ch], p.curdist[ch], a);
        int tim = (i - d * 44100 * speed_of_sound);
        if(tim < 0) continue;
        tim %= isize(orig);
          sndbuffer[i][ch] += orig[tim][ch] * lerp(att0[ch], att1[ch], a);
        }
    p.lastframe = p.curframe;
    p.lastdist = p.curdist;
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

/** draw bird, and also record the distance data about cell c */
bool draw_bird(cell *c, const transmatrix& V) {

  if(!in) return false;

  if(!started) start_audio();

  if(c == cwt.at) 
    queuepoly(V * cspin(1, 2, M_PI/2) * cspin(0, 2, M_PI/2), GDIM == 3 ? cgi.shAnimatedTinyEagle[wingphase(200)] : cgi.shTinyBird, 0xFFFFFFFF); 
  
  auto& ci = infos[c];
  ci.curframe = frameid;
  ci.curdist[0] = hdist0(xpush(-iad) * tC0(V));
  ci.curdist[1] = hdist0(xpush(+iad) * tC0(V));  

  return false;
  }

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

  dialog::addSelItem("resynchronize", fts((current_sample - curt) / 44100.), 'r');
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
    fwrite(&to_play[0], 4, to_play.size(), f);
    fclose(f);
    }
  }

auto hchook = addHook(hooks_drawcell, 100, draw_bird)

+ addHook(hooks_frame, 100, reverb_queue)

+ addHook(hooks_o_key, 80, o_key)

+ addHook(anims::hooks_after_video, 80, save_raw_audio)

+ addHook(hooks_args, 100, [] {
  using namespace arg;
           
  if(0) ;
  else if(argis("-reverb")) {
    shift();
    string fname = args();
    FILE *f = fopen(fname.c_str(), "rb");
    fseek(f, 0, SEEK_END);
    orig.resize(ftell(f)/sizeof(sample));
    fseek(f, 0, SEEK_SET);
    fread(&orig[0], 4, orig.size(), f);
    fclose(f);
    println(hlog, "original size = ", isize(orig));
    
    in = true;
    firstland = specialland = laCanvas;
    patterns::whichCanvas = 'r';
    patterns::rwalls = 100;
    mapeditor::drawplayer = false;
    start_game();
    if(!euclid)
      println(hlog, "edge = ", hdist(cgi.vertices_only[0], cgi.vertices_only[1]) * 10000 / 44100);
    /* Doppler effect is weird if scrolling if not smooth */
    smooth_scrolling = true;
    /* disable the frustum culling (we need sound from every direction) */
    frustum_culling = false;
    }
  else return 1;
  return 0;
  });


}
}
