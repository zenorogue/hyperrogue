// PRIVATE

#define HAVE_ACHIEVEMENTS
#define LEADERFULL "Steam Leaderboards"
#define LEADER "Steam"

#ifdef STEAMWIN
#include "sdk-w/public/steam/steam_api.h"
#else
#include "sdk/public/steam/steam_api.h"
#endif

#include "steamvisualinterface.h"

#define NOLICENSE

unsigned steam_appid;
SteamLeaderboard_t steamleader[NUMLEADER];

int lastleader;

void findLeaderboards() {
  lastleader = ticks;
  for(int i=0; i<NUMLEADER; i++) if(!steamleader[i]) if(leadernames[i][0] != '@') {
//  printf("Looking for Leaderboard '%s'\n", leadernames[i]);
    currentscore[i] = SCORE_UNKNOWN;
    SteamAPICall_t hSteamAPICall = 
      SteamUserStats()->FindLeaderboard(leadernames[i]);
    if(!hSteamAPICall) {
      printf("Failed to look for leaderboard named '%s'\n", leadernames[i]);
      }
    else {
      register_callback(hSteamAPICall);
      return;
      }
    }
  for(int i=0; i<NUMLEADER; i++) if(steamleader[i] && currentscore[i] == SCORE_UNKNOWN) {
    SteamAPICall_t hSteamAPICall = SteamUserStats()->DownloadLeaderboardEntries(
      steamleader[i], k_ELeaderboardDataRequestGlobalAroundUser, 0, 0);
    register_callback_download(hSteamAPICall);
    }
  }

CSteamID whoami;

void OnUserStatsReceived( UserStatsReceived_t *pCallback ) {
  if(steam_appid != pCallback->m_nGameID)
    return;
  if(pCallback->m_eResult == k_EResultOK) {
    printf("Received stats and achievements from Steam\n" );
    whoami = pCallback->m_steamIDUser;

/*    string s = SteamUserStats()->GetAchievementDisplayAttribute("DIAMOND1", "name");
    printf("Achievement name: '%s'\n", s.c_str());

    if(s == "Ice Explorer") {
      printf("Steam language is English\n");
      default_language = 0;
      }
    else if(s == "Eksplorator") {
      printf("Steam language is Polish\n");
      default_language = 1;
      }
    else if(s == "Buz Kâşifi") {
      printf("Steam language is Turkish\n");
      default_language = 2;
      }
    else if(s == "Průzkumník Ledu") {
      printf("Steam language is Czech\n");
      default_language = 3;
      } */

    string l = SteamApps()->GetCurrentGameLanguage();
    printf("Steam language: %s\n", l.c_str());
    if(l == "english") default_language = 0;
    if(l == "polish") default_language = 1;
    if(l == "turkish") default_language = 2;
    if(l == "czech") default_language = 3;
    if(l == "russian") default_language = 4;
      
    // SteamUserStats()->ClearAchievement("DIAMOND1");
    }
  else {
    printf("RequestStats - failed, %d\n", pCallback->m_eResult);
    }
  }

void OnFindLeaderboard( LeaderboardFindResult_t *pFindLeaderboardResult, bool bIOFailure ) {
  if ( !pFindLeaderboardResult->m_bLeaderboardFound || bIOFailure ) {
    printf("Leaderboard not found!\n");
    return;
    }

  // check to see which leaderboard handle we just retrieved
  const char *pchName = SteamUserStats()->GetLeaderboardName( pFindLeaderboardResult->m_hSteamLeaderboard );
  
  printf("Found leaderboard: '%s'\n", pchName);

  for(int i=0; i<NUMLEADER; i++) if(strcmp( pchName, leadernames[i] ) == 0 )
    steamleader[i] = pFindLeaderboardResult->m_hSteamLeaderboard;
  
  findLeaderboards();
  }

void OnScoresDownloaded( LeaderboardScoresDownloaded_t *p, bool bIOFailure ) {
  if ( bIOFailure ) {
    printf("OnScoresDownloaded failure!\n");
    return;
    }

  printf("Download count: %d\n", p->m_cEntryCount);
  if(p->m_cEntryCount) {
    LeaderboardEntry_t entry;
    SteamUserStats()->GetDownloadedLeaderboardEntry(
      p->m_hSteamLeaderboardEntries,0,&entry,NULL,0
      );

    if(entry.m_steamIDUser == whoami) for(int i=0; i<NUMLEADER; i++) 
      if(p->m_hSteamLeaderboard == steamleader[i]) {
        printf("Retrieved the current score for '%s': %d\n", leadernames[i], entry.m_nScore);
        currentscore[i] = entry.m_nScore;
        }
    }
  else {
    for(int i=0; i<NUMLEADER; i++) 
      if(p->m_hSteamLeaderboard == steamleader[i])
        currentscore[i] = NO_SCORE_YET;
    }

  findLeaderboards();
  }

void achievement_init() {
  if(SteamAPI_RestartAppIfNecessary(342610)) 
    exit(0);
  if(SteamAPI_Init()) {
    steam_appid = SteamUtils()->GetAppID();  
    for(int i=0; i<NUMLEADER; i++) 
      steamleader[i] = 0;
    if(!SteamUserStats()->RequestCurrentStats()) {
      printf("Request failed\n");
      }

    init_steamvisualinterface(OnUserStatsReceived, OnFindLeaderboard, OnScoresDownloaded);
    findLeaderboards();
    }
  else {
    printf("Steam init failed\n");
    return;
    }
  }

void achievement_gain(const char *name, bool euclideanAchievement) {
  achievement_log(name, euclideanAchievement);
  if(!steam_appid) return;
  if(cheater) return;
  if(euclid != euclideanAchievement) return;

  bool achieved;
  SteamUserStats()->GetAchievement(name, &achieved);  
  SteamUserStats()->SetAchievement(name);
  SteamUserStats()->StoreStats();

  if(!achieved) {
//  printf("NEW ACHIEVEMENT\n");
    achievementTimer = ticks;
    achievementMessage[0] = XLAT("New Achievement:");
    achievementMessage[1] = SteamUserStats()->GetAchievementDisplayAttribute(name, "name");
    achievementMessage[2] = SteamUserStats()->GetAchievementDisplayAttribute(name, "desc");
    }
  /* 
  for(int i=0; i<int(sizeof(achievements)/sizeof(char*)); i++)
    if(name == achievements[i]) {
      return;
      }
  printf("Unknown achievement '%s'\n", name); */
  }

unsigned char i8(int v) {
  if(v < 0) return 255;
  int b = 0;
  if(v < 160) return v;
  while(v > 160) {
    b++; v -= (v/3);
    }
  return 160+b;
  }

typedef unsigned char uchar;

uchar ugc[640], qugc;

void ugc_init() {
  for(int x=0; x<640; x++) ugc[x] = 0;
  qugc = 0;
  }

void ugc_char(uchar& x) {
  ugc[qugc++] = x;
  }

void ugc_compress(int& x) {
  ugc[qugc++] = i8(x);
  }

template<class T> void ugc_nocompress(T& x) {
  uchar *z = (uchar*) &x;
  for(int i=0; i<(int) sizeof(T); i++) ugc_char(z[i]);
  }

void upload_score(int id, int v) {
  
  if(!steam_appid) return;
  if(!steamleader[id]) return;
  
  ugc_init();
  short vs; int vi;
  vs = VERNUM_HEX; ugc_nocompress(vs);
  vi = timerstart; ugc_nocompress(vi);
  vi = countMyGolems(); ugc_compress(vi);
  ugc_compress(turncount);
  ugc_compress(cellcount);
  ugc_compress(savecount);
  uchar l = lastland; if(tampered) l |= 64; ugc_char(l);
  vi = celldist(cwt.c); ugc_compress(vi);                      
  vi = savetime + (timerstopped ? 0 : (time(NULL) - timerstart)); ugc_compress(vi);
  for(int i=0; i<40; i++) ugc_compress(items[i]);
  for(int i=0; i<67; i++) ugc_compress(kills[i]);
  ugc_compress(explore[0]); ugc_compress(explore[5]);
  for(int i=0; i<23; i++) ugc_compress(exploreland[0][i]);

  SteamUserStats()->UploadLeaderboardScore( steamleader[id], k_ELeaderboardUploadScoreMethodKeepBest, v, (int32*)&ugc, (qugc+3) / sizeof(int32));
  }

bool haveLeaderboard(int id) {
  return steamleader[id];
  }

void achievement_close() {
  if(!steam_appid) return;
  SteamAPI_Shutdown();
  }

void achievement_pump() {
  SteamAPI_RunCallbacks();
  if(ticks > lastleader + 30000) 
    findLeaderboards();
  }

