// PRIVATE

#include "sdk/public/steam/steam_api.h"
#include "steamvisualinterface.h"

class SteamContact {
  public:
  
  STEAM_CALLBACK( SteamContact, OnUserStatsReceived, UserStatsReceived_t, m_CallbackStatsReceived );
  SteamContact ();

  void OnFindLeaderboard( LeaderboardFindResult_t *pFindLearderboardResult, bool bIOFailure );
  void OnDownloadScores( LeaderboardScoresDownloaded_t *pLeaderboardScoresDownloaded, bool bIOFailure );
  CCallResult<SteamContact, LeaderboardFindResult_t> m_SteamCallResultCreateLeaderboard;
  CCallResult<SteamContact, LeaderboardScoresDownloaded_t> m_SteamCallResultScoresDownloaded;
  };

SteamContact :: SteamContact() 
: m_CallbackStatsReceived(this, &SteamContact::OnUserStatsReceived) 
  {  }

static SteamContact *steam;

callback_statsreceived sr;
callback_findleaderboard fl;
callback_scoresdownload sdl;

void init_steamvisualinterface(callback_statsreceived _sr, callback_findleaderboard _fl, callback_scoresdownload _sdl) {
  sr = _sr;
  fl = _fl;
  sdl = _sdl;
  steam = new SteamContact;
  }

void register_callback(SteamAPICall_t handle) {
  steam->m_SteamCallResultCreateLeaderboard.Set( handle, steam, &SteamContact::OnFindLeaderboard );
  }      

void register_callback_download(SteamAPICall_t handle) {
  steam->m_SteamCallResultScoresDownloaded.Set( handle, steam, &SteamContact::OnDownloadScores );
  }      

void SteamContact::OnFindLeaderboard( LeaderboardFindResult_t *pFindLeaderboardResult, bool bIOFailure ) {
  fl(pFindLeaderboardResult, bIOFailure);
  }

void SteamContact::OnDownloadScores( LeaderboardScoresDownloaded_t *pLeaderboardScoresDownloaded, bool bIOFailure ) {
  sdl(pLeaderboardScoresDownloaded, bIOFailure);
  }

void SteamContact::OnUserStatsReceived( UserStatsReceived_t *pCallback ) {
  sr(pCallback);
  }

