bool audio;
string musiclicense;
string musfname[landtypes];
int musicvolume = 60, effvolume = 60;

eLand getCurrentLandForMusic() {
  eLand id = cwt.c->land;
  if(isHaunted(id)) id = laHaunted;
  if(id == laWarpSea) id = laWarpCoast;
  return id;
  }

void playSeenSound(cell *c) {
  if(!c->monst) return;
  bool nearme = c->cpdist <= 7;
  forCellEx(c2, c) if(c2->cpdist <= 7) nearme = true; 
  if(!nearme) return;
  if(c->monst == moEagle) 
    playSound(c, "seen-eagle");
  else if(c->monst == moEarthElemental) 
    playSound(c, "seen-earth");
  else if(c->monst == moAirElemental) 
    playSound(c, "seen-air");
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

#ifdef SDLAUDIO

bool loaded[landtypes];
Mix_Music* music[landtypes];
int musicpos[landtypes];
int musstart;
int musfadeval = 2000;

eLand cid = laNone;

void handlemusic() {
  DEBB(DF_GRAPH, (debugfile,"handle music\n"));
  if(audio && musicvolume) {
    eLand id = getCurrentLandForMusic();
#ifdef LOCAL
    extern bool local_changemusic(eLand& id);
    if(local_changemusic(id)) return;
#endif
    if(outoffocus) id = eLand(0);
    if(musfname[id] == "LAST") id = cid;
    if(!loaded[id]) {
      loaded[id] = true;
      // printf("loading (%d)> %s\n", id, musfname[id].c_str());
      if(musfname[id] != "") {
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


void resetmusic() {
  if(audio && musicvolume) {
    Mix_FadeOutMusic(3000);
    cid = laNone;
    for(int i=0; i<landtypes; i++) musicpos[i] = 0;
    musfadeval = 2000;
    }
  }

bool loadMusicInfo(string dir) {
  DEBB(DF_INIT, (debugfile,"load music info\n"));
  if(dir == "") return false;
  FILE *f = fopen(dir.c_str(), "rt");
  if(f) {
    string dir2;
    for(int i=0; i<size(dir); i++) if(dir[i] == '/' || dir[i] == '\\')
      dir2 = dir.substr(0, i+1);
    char buf[1000];
    while(fgets(buf, 800, f) > 0) {
      for(int i=0; buf[i]; i++) if(buf[i] == 10 || buf[i] == 13) buf[i] = 0;
      if(buf[0] == '[' && buf[3] == ']') {
        int id = (buf[1] - '0') * 10 + buf[2] - '0';
        if(id >= 0 && id < landtypes) {
          if(buf[5] == '*' && buf[6] == '/') musfname[id] = dir2 + (buf+7);
          else musfname[id] = buf+5;
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

bool loadMusicInfo() {
  return
    loadMusicInfo(musicfile)
    || loadMusicInfo("./hyperrogue-music.txt") 
    || loadMusicInfo("music/hyperrogue-music.txt")
// Destination set by ./configure (in the GitHub repository)
#ifdef MUSICDESTDIR
    || loadMusicInfo(MUSICDESTDIR)
#endif
#ifdef FHS
    || loadMusicInfo("/usr/share/hyperrogue/hyperrogue-music.txt") 
    || loadMusicInfo(s0 + getenv("HOME") + "/.hyperrogue-music.txt")
#endif
    ;
  }

void initAudio() {
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
string wheresounds = "sounds/";
#endif

void playSound(cell *c, const string& fname, int vol) {
  if(effvolume == 0) return;
  // printf("Play sound: %s\n", fname.c_str());
  if(!chunks.count(fname)) {
    string s = wheresounds+fname+".ogg";
    chunks[fname] = Mix_LoadWAV(s.c_str());
    // printf("Loading, as %p\n", chunks[fname]);
    }
  Mix_Chunk *chunk = chunks[fname];
  if(chunk) {
    Mix_VolumeChunk(chunk, effvolume * vol / 100);
    Mix_PlayChannel(-1, chunk, 0);
    }
  }

#else
void resetmusic() {}
#endif

