// HyperRogue for Android
// Copyright (C) 2012-2017 Zeno Rogue

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#define ISANDROID 1
#define GL_ES

#define MOBPAR_FORMAL JNIEnv *env, jobject thiz
#define MOBPAR_ACTUAL env, thiz

void gdpush(int t);

#include <jni.h>
void shareScore(MOBPAR_FORMAL);

const char *scorefile, *conffile;
#include <string>
std::string levelfile, picfile;

bool settingsChanged = false;

#include "../../../../../init.cpp"

// #define delref env->DeleteLocalRef(_thiz)

int semaphore = 0;
bool crash = false;


#include <android/log.h>

#define LOCK(s, x) \
  semaphore++; const char *xs = x; if(semaphore > 1) { crash = true; \
    __android_log_print(ANDROID_LOG_WARN, "HyperRogue", "concurrency crash in %s\n", x); semaphore--; fflush(stdout); return s; }
#define UNLOCK semaphore--; if(crash) { crash = false; __android_log_print(ANDROID_LOG_WARN, "HyperRogue", "concurrency crashed with %s\n", xs); fflush(stdout); }


extern "C" jintArray
Java_com_roguetemple_hyperroid_HyperRogue_loadMap
  ( MOBPAR_FORMAL)
{
//  if(debfile) fprintf(debfile, "loadmap started.\n"), fflush(debfile);
    LOCK(NULL, "loadMap")

    jintArray result;
    result = env->NewIntArray(size(graphdata));
    if(result == NULL) return NULL;
    
    env->SetIntArrayRegion(result, 0, size(graphdata), &*graphdata.begin());
    // delref;
//  env->DeleteLocalRef(result);
//  if(debfile) fprintf(debfile, "loadmap finished.\n"), fflush(debfile);

    UNLOCK
    return result;
}

extern "C" bool
Java_com_roguetemple_hyperroid_HyperRogue_captureBack
  ( MOBPAR_FORMAL) {
    if(cmode & sm::NORMAL) return false;
    popScreenAll();
    return true;
  }

void uploadAll(MOBPAR_FORMAL);

extern "C" bool
Java_com_roguetemple_hyperroid_HyperRogue_keepinmemory
  ( MOBPAR_FORMAL) {
  saveStats(true);
  uploadAll(MOBPAR_ACTUAL);
  if(!canmove) return false;
  if(items[itOrbSafety]) return false;
  return gold() >= 10 || tkills() >= 50;
  }

extern "C" int
Java_com_roguetemple_hyperroid_HyperRogue_getland
  ( MOBPAR_FORMAL)
{
    return getCurrentLandForMusic();
    }

extern "C" int
Java_com_roguetemple_hyperroid_HyperRogue_getLanguage
  ( MOBPAR_FORMAL)
{
    return vid.language;
    }

extern "C" int
Java_com_roguetemple_hyperroid_HyperRogue_getMusicVolume
  ( MOBPAR_FORMAL)
{
    return musicvolume;
    }

extern "C" int
Java_com_roguetemple_hyperroid_HyperRogue_getEffVolume
  ( MOBPAR_FORMAL)
{
    return effvolume;
    }

extern "C" int
Java_com_roguetemple_hyperroid_HyperRogue_getImmersive(MOBPAR_FORMAL)
{
    return vid.full;
    }

extern "C" int
Java_com_roguetemple_hyperroid_HyperRogue_getGL(MOBPAR_FORMAL)
{
    return vid.usingGL;
    }

string sscorefile, sconffile, scachefile;

#include <sys/stat.h>

extern "C" void
Java_com_roguetemple_hyperroid_HyperRogue_setFilesDir(MOBPAR_FORMAL, jstring dir)
{
  const char *nativeString = env->GetStringUTFChars(dir, 0);
  sscorefile = nativeString; sscorefile += "/hyperrogue.log";
  sconffile = nativeString; sconffile += "/hyperrogue.ini";
  scachefile = nativeString; scachefile += "/scorecache.txt";
  levelfile = nativeString; levelfile += "/hyperrogue.lev"; 
  picfile = nativeString; picfile += "/hyperrogue.pic"; 
  scorefile = sscorefile.c_str();
  conffile = sconffile.c_str();
  chmod(scorefile, 0777);
  chmod(conffile, 0777);
  chmod(nativeString, 0777);
  env->ReleaseStringUTFChars(dir, nativeString);
  }

bool gamerunning;

extern "C" 
jint
Java_com_roguetemple_hyperroid_HyperRogue_initGame(MOBPAR_FORMAL) {
                                                  
//  debfile = fopen("/sdcard/hyperdebug.txt", "wt");
//  if(!debfile) debfile = fopen("/storage/simulated/0/hyperdebug.txt", "wt");
  
//  if(debfile) fprintf(debfile, "initgame started.\n"), fflush(debfile);
  

  __android_log_print(ANDROID_LOG_VERBOSE, "HyperRogue", "Initializing game, gamerunning = %d\n", gamerunning);
  fflush(stdout);
  if(gamerunning) return 1;
  gamerunning = true;  
  initAll();
  uploadAll(MOBPAR_ACTUAL);
  return 0;
  }

JNIEnv *tw_env; jobject tw_thiz;

int textwidth(int siz, const string &str) {
  jclass cls = tw_env->GetObjectClass(tw_thiz);
  jmethodID mid = tw_env->GetMethodID(cls, "getTextWidth", "(Ljava/lang/String;I)I");
  jobject jstr = tw_env->NewStringUTF(str.c_str());
  int res = tw_env->CallIntMethod(tw_thiz, mid, jstr, siz);
  tw_env->DeleteLocalRef(jstr);
  tw_env->DeleteLocalRef(cls);
  return res;
  }

bool doOpenURL;

bool currentlyConnecting() { return false; }
bool currentlyConnected() { return false; }

vector<pair<string, int> > soundsToPlay;

void openURL() {
  doOpenURL = true;
  }
  
void shareScore(MOBPAR_FORMAL) {
  string s = buildScoreDescription();
  jclass cls = env->GetObjectClass(thiz);
  jmethodID mid = env->GetMethodID(cls, "shareScore", "(Ljava/lang/String;)V");
  jobject str = env->NewStringUTF(s.c_str());
  env->CallVoidMethod(thiz, mid, str);
  env->DeleteLocalRef(str);
  env->DeleteLocalRef(cls);
  }

int nticks; int getticks() { return nticks; }

extern "C" void Java_com_roguetemple_hyperroid_HyperRogue_draw(MOBPAR_FORMAL) {
//  if(debfile) fprintf(debfile, "draw started.\n"), fflush(debfile);

  LOCK(, "draw")
  /* static int infoticks;
  if(getticks() - infoticks > 10000 && !timerghost)  {
    addMessage("ticks: " + its(getticks()));
    infoticks = getticks();
    } */
  tw_thiz = thiz; tw_env = env;
  mobile_draw(MOBPAR_ACTUAL);
  uploadAll(MOBPAR_ACTUAL);
  UNLOCK
  }

extern "C" void Java_com_roguetemple_hyperroid_HyperRogue_drawScreenshot(MOBPAR_FORMAL) {
  dynamicval<bool> d(vid.usingGL, false);
  Java_com_roguetemple_hyperroid_HyperRogue_draw(MOBPAR_ACTUAL);
  }

extern "C" void Java_com_roguetemple_hyperroid_HyperRogue_handleKey(MOBPAR_FORMAL, jint keycode) {
  flashMessages(); mousing = false;
  handlekey(keycode, keycode);
  }

extern "C" void Java_com_roguetemple_hyperroid_HyperRogue_update
  ( MOBPAR_FORMAL, 
    jint xres, jint yres, jint _ticks,
    jint _mousex, jint _mousey, jboolean _clicked) {

  LOCK(, "update")

//  if(debfile) fprintf(debfile, "update started.\n"), fflush(debfile);
  
  if(xres != vid.xres || yres != vid.yres)
    vid.killreduction = 0;
    
  vid.xres = xres;
  vid.yres = yres;
  vid.fsize = (min(vid.xres, vid.yres) * fontscale + 50) / 3200;

  mousex = _mousex;
  mousey = _mousey;
  clicked = _clicked;
  nticks = _ticks;
  uploadAll(MOBPAR_ACTUAL);
  UNLOCK
  // delref;
//  if(debfile) fprintf(debfile, "update stopped.\n"), fflush(debfile);
  }
    
void playSound(cell *c, const string& fname, int vol) {
  soundsToPlay.push_back(make_pair(fname, vol));
  }

void uploadAll(JNIEnv *env, jobject thiz) {

  jclass cls = env->GetObjectClass(thiz);
  
  for(int i=0; i<size(soundsToPlay); i++) {
    jmethodID mid = env->GetMethodID(cls, "playSound", "(Ljava/lang/String;I)V");
    jobject str = env->NewStringUTF(soundsToPlay[i].first.c_str());
    env->CallVoidMethod(thiz, mid, str, soundsToPlay[i].second);
    env->DeleteLocalRef(str);
    }
  soundsToPlay.clear();

  if(settingsChanged) {
    jmethodID mid = env->GetMethodID(cls, "applyUserSettings", "()V");
    env->CallVoidMethod(thiz, mid);
    settingsChanged = false;
    }

  if(doOpenURL) {
    jmethodID mid = env->GetMethodID(cls, "openWebsite", "()V");
    env->CallVoidMethod(thiz, mid);
    doOpenURL = false;
    }
    
  env->DeleteLocalRef(cls);
  }

void achievement_init() {}
void achievement_close() {}
void achievement_gain(const char* s, char flags) {}

#include <unistd.h>

