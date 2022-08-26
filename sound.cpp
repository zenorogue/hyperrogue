// Hyperbolic Rogue -- sound effects and music
// Copyright (C) 2011-2018 Zeno Rogue, see 'hyper.cpp' for details

/** \file sound.cpp
 *  \brief sound effects and music
 */

#include "hyper.h"
namespace hr {

#if HDR
void playSound(cell *c, const string& fname, int vol = 100);
void resetmusic();
#endif

#if HDR
/** RogueViz may be used for situations where music does not correspond to lands, so we allow extra IDs */
static constexpr int MUSIC_MAX = 500;
#endif

EX const char *musicfile = "";
EX bool audio;
EX string musiclicense;
EX string musfname[MUSIC_MAX];
EX int musicvolume = 60;
EX int effvolume = 60;
EX bool music_available;
EX int musiclength[MUSIC_MAX];

EX eLand getCurrentLandForMusic() {
  eLand id = ((anims::center_music()) && centerover) ? centerover->land : cwt.at->land;
  if(isHaunted(id)) id = laHaunted;
  if(id == laWarpSea) id = laWarpCoast;
  if(id == laMercuryRiver) id = laTerracotta;
  return id;
  }

EX void playSeenSound(cell *c) {
  if(!c->monst) return;
  bool nearme = c->cpdist <= 7;
  forCellEx(c2, c) if(c2->cpdist <= 7) nearme = true; 
  if(!nearme) return;
  if(among(c->monst, moEagle, moWindCrow, moAcidBird))
    playSound(c, "seen-eagle");
  else if(c->monst == moEarthElemental) 
    playSound(c, "seen-earth");
  else if(c->monst == moAirElemental) 
    playSound(c, "seen-air");
  else if(c->monst == moPhaser) 
    playSound(c, "seen-frog1");
  else if(c->monst == moFrog) 
    playSound(c, "seen-frog2");
  else if(c->monst == moVaulter) 
    playSound(c, "seen-frog3");
  else if(c->monst == moWaterElemental)
    playSound(c, "seen-water");
  else if(c->monst == moFireElemental)
    playSound(c, "fire");
  else if(c->monst == moDragonHead)
    playSound(c, "seen-dragon");
  else if(c->monst == moWorm)
    playSound(c, "seen-sandworm");
  else if(c->monst == moSkeleton && c->land != laDungeon)
    playSound(c, "seen-skeleton");
  else if(c->monst == moHexSnake)
    playSound(c, "seen-snake");
  else if(c->monst == moWolf || c->monst == moRedFox)
    playSound(c, "seen-woof");
  else if(isTroll(c->monst))
    playSound(c, "seen-troll");
  else if(c->monst == moNecromancer)
    playSound(c, "seen-necromancer");
  else if(c->monst == moGhost)
    playSound(c, "seen-ghost");
  else if(c->monst == moRoseBeauty) 
    playSound(c, princessgender() ? "seen-rosebeauty" : "seen-gardener");
  else if(c->monst == moVizier) 
    playSound(c, "seen-vizier");
  else if(c->monst == moFireFairy)
    playSound(c, "seen-fairy");
  else if(c->monst == moCultist) 
    playSound(c, "seen-cultist");
  else if(c->monst == moPyroCultist) 
    playSound(c, "seen-cultistfire");
  else if(c->monst == moCultistLeader)
    playSound(c, "seen-cultistleader");
  }

#if CAP_SDLAUDIO

bool loaded[MUSIC_MAX];
Mix_Music* music[MUSIC_MAX];
EX int musicpos[MUSIC_MAX];
EX int musstart;
int musfadeval = 2000;

eLand cid = laNone;

EX hookset<bool(eLand&)> hooks_music;
EX hookset<void(eLand&)> hooks_sync_music;

EX bool music_out_of_focus = false;

EX void handlemusic() {
  DEBBI(DF_GRAPH, ("handle music"));
  if(audio && musicvolume) {
    eLand id = getCurrentLandForMusic();
    if(callhandlers(false, hooks_music, id)) return;
    if(outoffocus && !music_out_of_focus) id = eLand(0);
    if(musfname[id] == "LAST") id = cid;
    if(!loaded[id] && !memory_issues()) {
      loaded[id] = true;
      // printf("loading (%d)> %s\n", id, musfname[id].c_str());      
      // reuse music
      if(musfname[id] != "") {
        for(int i=0; i<landtypes; i++)
          if(music[i] && musfname[i] == musfname[id])
            music[id] = music[i];
        }
      if(!music[id]) {
        memory_for_lib();
        music[id] = Mix_LoadMUS(musfname[id].c_str());
        if(!music[id]) {
           printf("Mix_LoadMUS: %s\n", Mix_GetError());
           }
        }
      }
    if(cid != id && !musfadeval) {
      musicpos[cid] = SDL_GetTicks() - musstart;
      musfadeval = musicpos[id] ? 500 : 2000;
      Mix_FadeOutMusic(musfadeval);
      // printf("fadeout %d, pos %d\n", musfadeval, musicpos[cid]);
      }
    if(music[id] && !Mix_PlayingMusic()) {
      callhooks(hooks_sync_music, id);
      cid = id;
      Mix_VolumeMusic(musicvolume);
      Mix_FadeInMusicPos(music[id], -1, musfadeval, musicpos[id] / 1000.0);
      // printf("fadein %d, pos %d\n", musfadeval, musicpos[cid]);
      musstart = SDL_GetTicks() - musicpos[id];
      musicpos[id] = 0;
      musfadeval = 0;
      }
    }
  }

hookset<bool(eLand&)> hooks_resetmusic;

EX void resetmusic() {
  if(audio && musicvolume) {
    Mix_FadeOutMusic(3000);
    cid = laNone;
    for(int i=0; i<landtypes; i++) musicpos[i] = 0;
    musfadeval = 2000;
    }
  }

EX bool loadMusicInfo(string dir) {
  DEBBI(DF_INIT, ("load music info"));
  if(dir == "") return false;
  FILE *f = fopen(dir.c_str(), "rt");
  if(f) {
    string dir2;
    for(int i=0; i<isize(dir); i++) if(dir[i] == '/' || dir[i] == '\\')
      dir2 = dir.substr(0, i+1);
    char buf[1000];
    while(fgets(buf, 800, f) != NULL) {
      for(int i=0; buf[i]; i++) if(buf[i] == 10 || buf[i] == 13) buf[i] = 0;
      if(buf[0] == '[' && buf[3] == ']') {
        int id = (buf[1] - '0') * 10 + buf[2] - '0';
        if(id >= 0 && id < MUSIC_MAX) {
          if(buf[5] == 'L' && buf[6] == '=') musiclength[id] = atoi(buf+7);
          else if(buf[5] == '*' && buf[6] == '/') musfname[id] = dir2 + (buf+7);
          else musfname[id] = buf+5;
          music_available = true;
          }
        else {
          fprintf(stderr, "warning: bad soundtrack id, use the following format:\n");
          fprintf(stderr, "[##] */filename\n");
          fprintf(stderr, "where ## are two digits, and */ is optional and replaced by path to the music\n");
          fprintf(stderr, "alternatively LAST = reuse the last track instead of having a special one");
          }
        // printf("[%2d] %s\n", id, buf);
        }
      else if(buf[0] == '#') {
        }
      else {
        musiclicense += buf;
        musiclicense += "\n";
        }
      }
    fclose(f);
    return true;
    }
  return false;
  }

EX bool loadMusicInfo() {
  return
    loadMusicInfo(musicfile)
    || loadMusicInfo(HYPERPATH "hyperrogue-music.txt") 
    || loadMusicInfo("./hyperrogue-music.txt") 
    || loadMusicInfo("music/hyperrogue-music.txt")
// Destination set by ./configure (in the GitHub repository)
#ifdef MUSICDESTDIR
    || loadMusicInfo(MUSICDESTDIR)
#endif
#ifdef FHS
    || loadMusicInfo("/usr/share/hyperrogue/hyperrogue-music.txt") 
    || (getenv("HOME") && loadMusicInfo(s0 + getenv("HOME") + "/.hyperrogue-music.txt"))
#endif
    ;
  }

EX void initAudio() {
  audio = loadMusicInfo();

  if(audio) {
    if(Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, 2, 2048) != 0) {
      fprintf(stderr, "Unable to initialize audio: %s\n", Mix_GetError());
      audio = false;
      }
    else {
      audio = true;
      Mix_AllocateChannels(16);
      }
    }
  }

map<string, Mix_Chunk*> chunks;

#ifdef SOUNDDESTDIR
string wheresounds = SOUNDDESTDIR;
#else
string wheresounds = HYPERPATH "sounds/";
#endif

hookset<bool(const string& s, int vol)> hooks_sound;

EX void playSound(cell *c, const string& fname, int vol) {
  LATE( hr::playSound(c, fname, vol); )
  if(effvolume == 0) return;
  if(callhandlers(false, hooks_sound, fname, vol)) return;
  // printf("Play sound: %s\n", fname.c_str());
  if(!chunks.count(fname)) {
    string s = wheresounds+fname+".ogg";
    if(memory_issues()) return;
    memory_for_lib();
    chunks[fname] = Mix_LoadWAV(s.c_str());
    // printf("Loading, as %p\n", chunks[fname]);
    }
  Mix_Chunk *chunk = chunks[fname];
  if(chunk) {
    Mix_VolumeChunk(chunk, effvolume * vol / 100);
    Mix_PlayChannel(-1, chunk, 0);
    }
  }

EX void reuse_music_memory() {
  for(int i=0; i<landtypes; i++)
    if(music[i] && music[i] != music[cid]) {
      Mix_Music *which = music[i];
      println(hlog, "freeing music for ", dnameof(eLand(i)));
      Mix_FreeMusic(which);
      for(int j=0; j<landtypes; j++) if(music[j] == which) {
        println(hlog, "... which equals ", dnameof(eLand(j)));
        music[j] = NULL;
        }
      }
  set<Mix_Chunk*> currently_played;
  for(int ch=0; ch<16; ch++) currently_played.insert(Mix_GetChunk(ch));
  set<string> to_free;
  for(auto& p: chunks) 
    if(p.second) {
      if(currently_played.count(p.second)) {
        println(hlog, p.first, ": currently played");
        }
      else {
        Mix_FreeChunk(p.second);
        to_free.insert(p.first); 
        println(hlog, p.first, ": freed");
        }
      }
  for(auto& s: to_free) chunks.erase(s);
  }

#endif

#if CAP_COMMANDLINE
int read_sound_args() {
  using namespace arg;
  if(argis("-m")) { PHASE(1); shift(); musicfile = argcs(); }
#if CAP_SDLAUDIO
  else if(argis("-se")) { PHASE(1); shift(); wheresounds = args(); }
  else if(argis("-musicfocus")) { music_out_of_focus = true; }
#endif
  else if(argis("-svol")) { PHASEFROM(2); shift(); effvolume = argi(); }
  else if(argis("-mvol")) { PHASEFROM(2); shift(); musicvolume = argi(); }
  else return 1;
  return 0;
  }

#if CAP_SDLAUDIO
auto ah_sound = addHook(hooks_args, 0, read_sound_args) + addHook(hooks_clear_cache, 0, reuse_music_memory);
#endif

#endif

#if !CAP_AUDIO
EX void playSound(cell *c, const string& fname, int vol) { }
EX void resetmusic() { }
#endif

}
