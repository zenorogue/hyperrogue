// PRIVATE

void saveAchShot(const char *b, double scale) {

  for(int i=0; i<2; i++) {
    SDL_Surface *sav = s;
    s = SDL_CreateRGBSurface(SDL_SWSURFACE,512,512,32,0,0,0,0);
    
    videopar vid2 = vid;
    vid.xres = vid.yres = 512; vid.scale = scale;
    bool bb = vid.usingGL; vid.usingGL = false;
    calcparam();

    darken = 0;
    ptds.clear();
    drawthemap();

    SDL_FillRect(s, NULL, i ? 0xFFFFFF : 0);

    drawqueue();
    ptds.clear();
    drawthemap();

    char buf[128]; sprintf(buf, "steam/ach/%d%s.bmp", i, b);
    SDL_SaveBMP(s, buf);
    SDL_FreeSurface(s); s = sav; vid = vid2;
    vid.usingGL = bb;
    }
  }

void saveFrame(int fr) {

  fanframe = fr / 30.0;

  for(int i=0; i<1; i++) {
    SDL_Surface *sav = s;
    s = SDL_CreateRGBSurface(SDL_SWSURFACE,2048,2048,32,0,0,0,0);
    
    videopar vid2 = vid;
    vid.xres = vid.yres = 2048; vid.scale = 1;
    bool bb = vid.usingGL; vid.usingGL = false;
    calcparam();

  int ssr = sightrange; sightrange = 10; int sch = cheater; cheater = 0;

    darken = 0;
    ptds.clear();
    mapeditor::drawplayer = false;
    drawthemap();

    SDL_FillRect(s, NULL, i ? 0xFFFFFF : 0);

    drawqueue();

    char buf[128]; sprintf(buf, "fanframe/image%02d.bmp", fr);
    SDL_SaveBMP(s, buf);
    SDL_FreeSurface(s); s = sav; vid = vid2;
    vid.usingGL = bb;
    
    cheater = sch; sightrange = ssr;
    }
  }

int fadeoutat = -1;
bool before;
int tv = -1;
int textat;
string landmsg;
string landmsg2;
int landcol;
eLand nextland;

void process_local_extra() {
  if(fadeoutat != -1) {
    fadeout = 1 - (SDL_GetTicks() - fadeoutat) / 1000.0;
    if(fadeout < -1) { fadeoutat = -1; fadeout = 1; nextland = laNone; return; }
    if(fadeout < 0) {
      fadeout = -fadeout;
      if(before) {
        msgs.clear();
        textat = SDL_GetTicks();
        before = false;
        firstland = nextland;
        landmsg = linf[firstland].name;
        landmsg2 = "";
        landcol = linf[firstland].color;
        if(shmup::on) restartGame('s');
        if(euclid) restartGame('e');
        tactic::trailer = true;
        if(!tactic::on) restartGame('t');
        else restartGame();
        
        /* if(firstland == laCamelot) {
          firstland = laIce;
          landmsg = "Shmup Mode";
          landmsg2 = "you can play with your friends!";
          shmup::players = 2;
          restartGame('t');
          restartGame('s');
          }
        
        if(firstland == laMirror) {
          landmsg2 = "featuring the Euclidean mode!";
          restartGame('e');
          } */
        
        if(firstland == laWildWest)
          landmsg2 = "(bonus land)";
          
        // vid.female = hrand(2);
        
        if(firstland == laRedRock)
          landcol = 0xFF0000;

        /* if(firstland == laPalace)
          cwt.c->mov[0]->monst = moPrincessArmed,
          cwt.c->mov[0]->hitpoints = 7,
          landmsg2 = "featuring the Princess!",
          vid.female = false,
          vid.samegender = false; */
        
        if(isCrossroads(firstland)) {
          if(firstland == laCrossroads) landmsg2 = "and the Camelot!",
          tactic::on = false;
          for(int i=0; i<ittypes; i++) if(itemclass(eItem(i)) == IC_TREASURE)
            items[i] = 10;
          items[itHyperstone] = 0;
          items[itBounty] = 0;
          }
        
        if(firstland == laTortoise) {
          items[itBabyTortoise]++;
          tortoise::seekbits = rand();
          }
        }
      }
    }
  }

void process_local_stats() {
  if(ticks >= textat && ticks < textat + 2500) {
    int dt = SDL_GetTicks()-textat;
    int col = gradient(0, landcol, 0, min(dt, 1000), 1000);
    displayfr(vid.xres/2, vid.yres/4, 2, vid.fsize * 2, landmsg, col, 8);
    displayfr(vid.xres/2, vid.yres/4 + vid.fsize*2, 2, vid.fsize, landmsg2, col, 8);
    }
  }

bool nomusic = false; // true;

bool local_changemusic(eLand& id) {
  if(nomusic) return true;
  if(nextland) id = nextland;
  return false;
  }

int syserr;

int tab[3000][3000];

vector<int> gtab[600][600][20];

int gromovprod(int x, int y, int o) {
  return tab[o][x] + tab[o][y] - tab[x][y];
  }

void gromov1() {
  int N = size(dcal);
  
  for(int i=0; i<N; i++)
  for(int j=0; j<N; j++) 
    tab[i][j] = celldistance(dcal[i], dcal[j]);

  int deltas[100];
  for(int k=0; k<100; k++) deltas[k] = 0;
  
  int fdelta = -1;
  
  
  for(int x=0; x<N; x++) {
  for(int y=0; y<N; y++) 
  for(int z=0; z<N; z++)
  for(int o=0; o<N; o++) {
    int deltabound = 
      min(gromovprod(x,z,o), gromovprod(y,z,o)) - gromovprod(x,y,o);
    
    deltas[50+deltabound]++;
    fdelta = max(deltabound, fdelta);
    }
    printf("x=%d/%d (%d)\n", x, N, fdelta);
    }
  
  for(int t=0; t<100; t++) if(deltas[t])
    printf("%4d: %d\n", t-50, deltas[t]);  
  
  exit(0);
  }

int pdst(int a, vector<int>& v) {
  int maxd = -1;
  for(int i=0; i<size(v); i++) if(tab[a][v[i]] > maxd) maxd = tab[a][v[i]];
  return maxd;
  }

void gromov() {
  static bool done = false;
  if(done) return;
  done = true;
  
  int N = 120; // size(dcal);
  
  for(int i=0; i<N; i++)
  for(int j=0; j<N; j++) 
    tab[i][j] = celldistance(dcal[i], dcal[j]);
  
  printf("built tab\n");
  
  for(int i=0; i<N; i++)
  for(int j=0; j<N; j++) 
  for(int k=0; k<N; k++) 
    if(tab[i][k] + tab[k][j] == tab[i][j])
      gtab[i][j][tab[i][k]].push_back(k);

  printf("built geos\n");
  
  int fdelta = -1;
  
  int fi, fj, fk;

  for(int i=0; i<N; i++)
  for(int j=0; j<N; j++)
  for(int d=0; d<=tab[i][j]; d++)
    if(size(gtab[i][j][d]) == 0)
      printf("Error in celldistance!\n");
    
  for(int l=0; l<N; l++)
  for(int i=0; i<=l; i++)
  for(int j=0; j<=l; j++)
  for(int k=(i==l||j==l?0:l); k<=l; k++) {
    int dij = tab[i][j];
    int dik = tab[i][k];
    int dkj = tab[k][j];
    for(int d=0; d<=dij; d++) for(int v=0; v<size(gtab[i][j][d]); v++) {
      int chk = gtab[i][j][d][v];
      int mindelta = 100;
      for(int d1=0; d1<=dik; d1++)
        mindelta = min(mindelta, pdst(chk, gtab[i][k][d1]));
      for(int d2=0; d2<=dkj; d2++)
        mindelta = min(mindelta, pdst(chk, gtab[j][k][d2]));
      if(mindelta > fdelta) {
        fdelta = mindelta;
        printf("found delta of %d [%d %d %d]\n", fdelta, i, j, k);
        fi=i; fj=j; fk=k;
        }
      }
    }
  
  dcal[fi]->item = itRuby;
  dcal[fj]->item = itAmethyst;
  dcal[fk]->item = itCompass;

  for(int i=0; i<N; i++) {
    if(tab[i][fi] + tab[i][fj] == tab[fi][fj]) dcal[i]->wall = waFloorA;
    if(tab[i][fj] + tab[i][fk] == tab[fj][fk]) dcal[i]->wall = waFloorB;
    if(tab[i][fk] + tab[i][fi] == tab[fk][fi]) dcal[i]->wall = waFloorC;
    }
  
  gromov1();
  }

void process_local0(int sym) {  

  // gromov();
  
  if(sym == SDLK_F4) {
    fadeoutat = SDL_GetTicks();
    before = true;
    tv++;
    tv %= LAND_TAC;
    nextland = land_tac[tv].l;
    vid.axes = 0;
    if(nextland == laCamelot) process_local0(sym);
    return;
    }

/*
  if(sym == SDLK_F6 && nomusic && !items[itOrbFriend]) { nomusic = false; return; } */

  /* if(sym == SDLK_F9) {
    syserr = system("killall recordmydesktop");
    }

  if(sym == SDLK_F8) {
    syserr = system("killall -s SIGUSR1 recordmydesktop");
    }  */

  }

void process_local(int sym) {
  if(sym == SDLK_F9) {
    showoff = true; showid = 0;
    for(int i=0; i<landtypes; i++) landcount[i] = 0;
    for(int i=0; i<ittypes; i++) items[i] = 0;
    for(int i=0; i<motypes; i++) kills[i] = 0;
    offscreen.clear();  
    clearMemory();
    initcells();
    initgame();
    restartGraph();
    int lat = 0;
    for(int i=0; i<landtypes; i++) if(landcount[i]) lat++;
    addMessage("lat = "+its(lat));
    }

//if(sym == SDLK_F8) 
//  saveHighQualityShot();

  if(sym == SDLK_F8)
    for(int i=0; i<30; i++) 
      saveFrame(i);

  if(sym == SDLK_F5) {

    int dcs = size(dcal);
    for(int i=0; i<dcs; i++) {
      cell *c = dcal[i];
      if(c->cpdist <= 4) setdist(c, 1, NULL);
      }

    time_t timer;
    timer = time(NULL);

    SDL_Surface *sav = s;
    s = SDL_CreateRGBSurface(SDL_SWSURFACE,1920,1200,32,0,0,0,0);
    
    int ssr = sightrange; sightrange = 10; int sch = cheater; cheater = 0;

    bool b = vid.usingGL;
    vid.usingGL = false;
    videopar vid2 = vid;
    vid.xres = 1920; vid.yres = 1200; vid.scale = 1.9;
    calcparam();

    darken = 0;
    ptds.clear();
    drawthemap();

    for(int i=0; i<2; i++) {
    SDL_FillRect(s, NULL, i ? 0xFFFFFF : 0);

#ifdef GFX
    aacircleColor(s, vid.xcenter, vid.ycenter, vid.radius, 0x0000FF80);
#endif
 
    drawqueue();

    char buf[128]; strftime(buf, 128, "backshot-%y%m%d-%H%M%S.bmp", localtime(&timer));
    buf[7] += i;
    SDL_SaveBMP(s, buf);
    }
    
    addMessage(s0 + "Backshot saved");
    
    SDL_FreeSurface(s); s = sav; vid = vid2; sightrange = ssr; cheater = sch;
    vid.usingGL = b;
    }

  if(sym == SDLK_F3) {
    showoff = true; showid = 0;
    for(int i=0; i<landtypes; i++) landcount[i] = 0;
    for(int i=0; i<ittypes; i++) items[i] = 0;
    for(int i=0; i<motypes; i++) kills[i] = 0;
    offscreen.clear();  
    clearMemory();
    initcells();
    initgame();
    restartGraph();
    
    vid.axes = 0;
    
    if(purehepta) {
      mapeditor::drawplayer = false;
      cwt.c->land = laCamelot;
      cwt.c->item = itHolyGrail;
      for(int t=0; t<cwt.c->type; t++)
        cwt.c->mov[t]->wall = waChasm;
      saveAchShot("grailh", 5); 
      return;
      }
    
    if(sphere && cwt.c->land == laHalloween) {
      mapeditor::drawplayer = false;
      vid.alpha = 999;
      View = spin(M_PI);
      saveAchShot("halloween", 998); 
      return;
      }

    for(int t=0; t<cwt.c->type; t++)
      cwt.c->mov[t]->wall = waChasm;
    
/*  mapeditor::drawplayer = false;
    cwt.c->land = laDryForest;
    cwt.c->item = itFernFlower;
    for(int t=0; t<cwt.c->type; t++)
      cwt.c->mov[t]->wall = waChasm;
    saveAchShot("fern", 5); 

    cwt.c->land = laPower;
    cwt.c->item = itPower;
    cwt.c->type = 6;
    cwt.c->wall = waGlass;
    saveAchShot("power", 5);

    cwt.c->item = itOrbFire;
    cwt.c->wall = waNone; 
    saveAchShot("fireorb", 5); */
    
    cwt.c->type = 7;
    
/*  cwt.c->land = laHive;
    cwt.c->item = itRoyalJelly;
    saveAchShot("jelly", 5);
    
    cwt.c->land = laDeadCaves;
    cwt.c->item = itSilver;
    saveAchShot("silver", 5);
    
    cwt.c->land = laEmerald;
    cwt.c->item = itEmerald;
    saveAchShot("emerald", 5);
    
    cwt.c->land = laWineyard;
    cwt.c->item = itWine;
    saveAchShot("wine", 5);
    
    cwt.c->land = laEmerald; 
    cwt.c->item = itNone;
    cwt.c->monst = moFlailer;
    saveAchShot("flailer", 5);
    
    cwt.c->land = laHive; 
    cwt.c->item = itNone;
    cwt.c->monst = moBug2;
    saveAchShot("bug", 5);
    
    cwt.c->land = laCaves; 
    cwt.c->item = itNone;
    cwt.c->monst = moNone;
    cwt.c->wall = waCavewall;
    saveAchShot("cavea", 5);
    
    cwt.c->land = laCrossroads; 
    cwt.c->item = itNone;
    cwt.c->monst = moNone;
    cwt.c->wall = waCavewall;
    saveAchShot("caveb", 5);
    
    cwt.c->wall = waNone;
    cwt.c->item = itNone;
    cwt.c->monst = moHedge;
    saveAchShot("hedge", 5); 
    
    cwt.c->item = itSapphire;
    cwt.c->monst = moNone;
    cwt.c->land = laCocytus;
    saveAchShot("sapphire", 5);

    cwt.c->item = itGrimoire;
    cwt.c->monst = moNone;
    cwt.c->land = laTemple;
    saveAchShot("grimoire", 5);

    cwt.c->item = itHolyGrail;
    cwt.c->monst = moNone;
    cwt.c->land = laCamelot;
    saveAchShot("grail", 5);

    cwt.c->item = itNone;
    cwt.c->monst = moKnight;
    cwt.c->land = laCamelot;
    cwt.c->wall = waChasm;
    saveAchShot("knight", 5);

    cwt.c->monst = moFlailer;
    cwt.c->land = laCamelot;
    cwt.c->wall = waChasm;
    saveAchShot("flailer", 5);

    cwt.c->monst = moNone;
    cwt.c->item = itHolyGrail;
    cwt.c->land = laCamelot;
    cwt.c->wall = waChasm;
    saveAchShot("grail-emo", 5);

    cwt.c->monst = moNone;
    cwt.c->item = itPirate;
    cwt.c->land = laCaribbean;
    cwt.c->wall = waCIsland;
    saveAchShot("pirate", 5);

    cwt.c->monst = moNone;
    cwt.c->item = itRedGem;
    cwt.c->land = laRedRock;
    cwt.c->wall = waRed3;
    saveAchShot("redgem", 5);

    cwt.c->monst = moNone;
    cwt.c->item = itWhirlpool;
    cwt.c->land = laWhirlpool;
    cwt.c->wall = waBoat;
    cwt.c->master->alt = cwt.c->master;
    cwt.c->master->distance = 0;
    saveAchShot("whirlpool", 5);

    cwt.c->monst = moNone;
    cwt.c->item = itCoast;
    cwt.c->land = laOcean;
    cwt.c->wall = waNone;
    saveAchShot("amber", 5);

    cwt.c->monst = moNone;
    cwt.c->item = itBombEgg;
    cwt.c->land = laMinefield;
    cwt.c->wall = waMineUnknown;
    saveAchShot("bombegg", 5);

    cwt.c->monst = moNone;
    cwt.c->item = itPalace;
    cwt.c->land = laPalace;
    cwt.c->wall = waNone;
    saveAchShot("hyperrug", 5);

    cwt.c->monst = moNone;
    cwt.c->item = itFjord;
    cwt.c->land = laLivefjord;
    cwt.c->wall = waNone;
    saveAchShot("garnet", 5);

    cwt.c->monst = moPrincess;
    cwt.c->item = itNone;
    cwt.c->land = laPalace;
    cwt.c->wall = waNone;
    saveAchShot("princess_palace", 5);

    cwt.c->monst = moPrincess;
    cwt.c->item = itNone;
    cwt.c->land = laCrossroads;
    cwt.c->wall = waNone;
    saveAchShot("princess_cross", 5);

    cwt.c->monst = moPrincess;
    cwt.c->item = itNone;
    cwt.c->land = laEmerald;
    cwt.c->wall = waNone;
    saveAchShot("princess_emerald", 5); */

/*    
    cwt.c->monst = moNone;
    cwt.c->item = itFireShard;
    cwt.c->land = laEFire;
    saveAchShot("elfire", 5);

    cwt.c->monst = moNone;
    cwt.c->item = itWaterShard;
    cwt.c->land = laEWater;
    saveAchShot("elwater", 5);

    cwt.c->monst = moNone;
    cwt.c->item = itAirShard;
    cwt.c->land = laEAir;
    saveAchShot("elair", 5);

    cwt.c->monst = moNone;
    cwt.c->item = itEarthShard;
    cwt.c->land = laEEarth;
    saveAchShot("elearth", 5);

    cwt.c->monst = moNone;
    cwt.c->item = itZebra;
    cwt.c->land = laZebra;
    saveAchShot("zebra", 5);

    cwt.c->monst = moNone;
    cwt.c->item = itFulgurite;
    cwt.c->land = laStorms;
    saveAchShot("fulgurite", 5);

    cwt.c->monst = moNone;
    cwt.c->item = itMutant;
    cwt.c->land = laOvergrown;
    saveAchShot("mutant", 5); */

/*  cwt.c->monst = moMetalBeast2;
    cwt.c->item = itNone;
    cwt.c->land = laStorms;
    saveAchShot("metalbeast", 5); */

/*    
    cwt.c->monst = moNone;
    cwt.c->item  = itLotus;
    cwt.c->land  = laHaunted;
    saveAchShot("lotus", 5);

    cwt.c->monst = moGhost;
    cwt.c->item  = itLotus;
    cwt.c->land  = laHaunted;
    saveAchShot("lotusghost", 5);

    cwt.c->monst = moNone;
    cwt.c->item  = itWindstone;
    cwt.c->land  = laWhirlwind;
    saveAchShot("dove", 5);

    cwt.c->monst = moNone;
    cwt.c->item  = itMutant2;
    cwt.c->land  = laClearing;
    saveAchShot("fruit", 5); */
    
/*  mapeditor::drawplayer = true;
    cwt.c->item  = itNone;
    cwt.c->land  = laIce;
    cwt.c->monst = moNone;
    saveAchShot("rogue", 5); */

/*  cwt.c->monst = moNone;
    cwt.c->land = laMinefield;
    cwt.c->wall = waMineOpen;
    cwt.c->item = itNone;
    for(int t=0; t<cwt.c->type; t++)
      cwt.c->mov[t]->land = laMinefield,      
      cwt.c->mov[t]->wall = waMineMine;
    saveAchShot("sevenmine", 5);

    cwt.c->monst = moCShark;
    cwt.c->land = laCamelot;
    cwt.c->item = itNone;
    cwt.c->wall = waCamelotMoat;
    cwt.c->mov[0]->wall = waCamelotMoat;
    cwt.c->mov[1]->wall = waCamelot;
    cwt.c->mov[2]->wall = waCamelotMoat;
    cwt.c->mov[3]->wall = waNone;
    cwt.c->mov[4]->wall = waNone;
    cwt.c->mov[5]->wall = waNone;
    cwt.c->mov[6]->wall = waNone;
    for(int t=0; t<cwt.c->type; t++)
      cwt.c->mov[t]->land = t<3? laCamelot : laCrossroads;
    saveAchShot("moatshark", 5);

    if(euclid) {
      vid.female = true;
      cwt.c->item = itNone;
      cwt.c->monst = moMirage;
      cwt.c->land = laMirror;
      cwt.c->wall = waNone;
      s veAchShot("mirage", 9);
      } */
     
    /*
    cwt.c->monst = moNone;
    cwt.c->item = itCompass;
    cwt.c->land = laCaribbean;
    cwt.c->wall = waChasm;
    saveAchShot("compass-emo", 5);

    cwt.c->monst = moPirate;
    cwt.c->item = itNone;
    cwt.c->land = laCaribbean;
    cwt.c->wall = waChasm;
    saveAchShot("pirate-emo", 5);

    cwt.c->monst = moBullet;
    cwt.c->item = itOrbYendor;
    cwt.c->land = laHell;
    cwt.c->wall = waNone;
    saveAchShot("knife", 5);*/

/*
    mapeditor::drawplayer = false;
    cwt.c->item  = itRose;
    cwt.c->land  = laRose;
    cwt.c->monst = moNone;
    cwt.c->wall = waNone;
    saveAchShot("rose", 5); */

/*
    mapeditor::drawplayer = false;
    cwt.c->item  = itCoral;
    cwt.c->land  = laWarpSea;
    cwt.c->monst = moNone;
    for(int i=0; i<cwt.c->type; i++) 
      cwt.c->mov[i]->land = laWarpSea;
    cwt.c->landparam = 1337;
    cwt.c->wall = waBoat;
    saveAchShot("coral", 5); */
/*
    bfs();
    for(int i=0; i<200; i++) {
      cell *c = dcal[i];
      c->wall = waChasm,
      c->item = itNone;
      }

    mapeditor::drawplayer = false;
    cwt.c->item  = itApple;
    cwt.c->land  = laEndorian;
    cwt.c->monst = moNone;
    cwt.c->wall = waCanopy;
    saveAchShot("apple", 5);

    mapeditor::drawplayer = false;
    cwt.c->item  = itDragon;
    cwt.c->land  = laDragon;
    cwt.c->monst = moNone;
    cwt.c->wall = waNone;
    saveAchShot("dragon", 4);

    mapeditor::drawplayer = false;
    cwt.c->item  = itBabyTortoise;
    cwt.c->land  = laTortoise; tortoise::babymap[cwt.c] = 0;
    cwt.c->monst = moTortoise; tortoise::emap[cwt.c] = cwt.c;
    cwt.c->wall = waNone;
    saveAchShot("tortoise", 4); */

/*
    mapeditor::drawplayer = false;
    cwt.c->item  = itKraken;
    cwt.c->land  = laKraken;
    cwt.c->monst = moNone;
    cwt.c->wall = waSea;
    items[itOrbInvis] = 10;
    saveAchShot("kraken", 5);
    items[itOrbInvis] = 0;

    mapeditor::drawplayer = false;
    cwt.c->item  = itBarrow;
    cwt.c->land  = laBurial;
    cwt.c->monst = moNone;
    cwt.c->wall = waBarrowDig;
    cwt.c->landparam = 2;
    saveAchShot("burial", 5);

    forCellEx(c2, cwt.c) forCellEx(c3, c2) forCellEx(c4, c3)
      c4->wall = waChasm;

    mapeditor::drawplayer = true;
    mousing = true;
    cwt.c->item  = itNone;
    cwt.c->land  = laBurial;
    cwt.c->monst = moNone;
    cwt.c->wall = waNone;
    items[itOrbSword] = 10;
    saveAchShot("swordslash", 3);
    items[itOrbSword] = 0;

    mapeditor::drawplayer = false;
    cwt.c->item  = itTrollEgg;
    cwt.c->land  = laTrollheim; 
    cwt.c->monst = moNone;
    cwt.c->wall = waNone;
    saveAchShot("troll", 5); */

/*    
    cwt.c->land = laCrossroads4;
    cwt.c->wall = waNone;
    cwt.c->item = itNone;
    cwt.c->mov[0]->wall = waCavewall;
    cwt.c->mov[0]->land = laCaves;
    cwt.c->mov[1]->wall = waColumn;
    cwt.c->mov[1]->land = laRlyeh;
    cwt.c->mov[3]->wall = waRose;
    cwt.c->mov[3]->land = laRose;
    cwt.c->mov[2]->wall = waDune;
    cwt.c->mov[2]->land = laDesert;
    cwt.c->mov[4]->wall = waPalace;
    cwt.c->mov[4]->land = laPalace;
    cwt.c->mov[5]->wall = waIcewall;
    cwt.c->mov[5]->land = laIce;
    cwt.c->mov[6]->wall = waVinePlant;
    cwt.c->mov[6]->land = laWineyard;
    mapeditor::drawplayer = true;

    saveAchShot("cr4", 2.3); */

/*  mapeditor::drawplayer = false;
    cwt.c->land  = laCaves; 
    cwt.c->monst = moGoblin;
    cwt.c->item = itNone;
    cwt.c->wall = waCavefloor;
    saveAchShot("goblin", 5); */
    
/*  mapeditor::drawplayer = false;
    cwt.c->land  = laReptile; 
    cwt.c->monst = moNone;
    cwt.c->item = itDodeca;
    cwt.c->wall = waReptile;
    cwt.c->wparam = 30;
    saveAchShot("dod", 5);

    mapeditor::drawplayer = false;
    cwt.c->land  = laDungeon; 
    cwt.c->monst = moNone;
    cwt.c->item = itSlime;
    cwt.c->wall = waNone;
    saveAchShot("slime", 5);
    
    mapeditor::drawplayer = false;
    cwt.c->land  = laMountain; 
    cwt.c->monst = moNone;
    cwt.c->item = itAmethyst;
    cwt.c->wall = waBigBush;
    saveAchShot("amethyst", 5); */

    mapeditor::drawplayer = false;
    cwt.c->land  = laDesert; 
    cwt.c->monst = moWorm;
    cwt.c->item = itNone;
    cwt.c->wall = waNone;
    saveAchShot("wormhead", 5);

    mapeditor::drawplayer = false;
    cwt.c->land  = laPrairie;
    cwt.c->monst = moNone;
    cwt.c->item = itGreenGrass;
    cwt.c->wall = waNone;
    saveAchShot("ggrass", 5);
    
    mapeditor::drawplayer = false;
    cwt.c->land  = laBull; 
    cwt.c->monst = moNone;
    cwt.c->item = itBull;
    cwt.c->wall = waNone;
    saveAchShot("bull", 5);

/*
    forCellEx(c2, cwt.c) forCellEx(c3, c2) forCellEx(c4, c3)
      c4->wall = waChasm, c4->item = itNone;

    cwt.c->land = laHell;
    cwt.c->wall = waNone;
    cwt.c->item = itOrbYendor;
    for(int i=0; i<7; i++) cwt.c->mov[i]->item = itOrbYendor;
    mapeditor::drawplayer = false;
    cwt.c->mov[0]->wall = waBoat;
    cwt.c->mov[0]->land = laOcean;

    cwt.c->mov[1]->wall = waNone;
    cwt.c->mov[1]->land = laGraveyard;
    cwt.c->mov[2]->wall = waNone;
    cwt.c->mov[2]->land = laJungle;
    cwt.c->mov[3]->wall = waNone;
    cwt.c->mov[3]->land = laOvergrown;
    cwt.c->mov[4]->wall = waFloorA;
    cwt.c->mov[4]->land = laAlchemist;
    cwt.c->mov[5]->wall = waNone;
    cwt.c->mov[5]->land = laPalace;
    cwt.c->mov[6]->wall = waNone;
    cwt.c->mov[6]->land = laWineyard;

    saveAchShot("yendc", 2.3);

    cwt.c->land = laIce;
    cwt.c->wall = waNone;
    cwt.c->item = itDiamond;
    mapeditor::drawplayer = false;
    cwt.c->mov[0]->wall = waNone;
    cwt.c->mov[0]->land = laDesert;
    cwt.c->mov[0]->item = itSpice;

    cwt.c->mov[1]->wall = waCavefloor;
    cwt.c->mov[1]->land = laCaves;
    cwt.c->mov[1]->item = itGold;

    cwt.c->mov[2]->wall = waNone;
    cwt.c->mov[2]->land = laJungle;
    cwt.c->mov[2]->item = itRuby;

    cwt.c->mov[3]->wall = waNone;
    cwt.c->mov[3]->land = laRlyeh;
    cwt.c->mov[3]->item = itStatue;

    cwt.c->mov[4]->wall = waFloorA;
    cwt.c->mov[4]->land = laAlchemist;
    cwt.c->mov[4]->item = itElixir;

    cwt.c->mov[5]->wall = waNone;
    cwt.c->mov[5]->land = laMotion;
    cwt.c->mov[5]->item = itFeather;
    
    cwt.c->mov[6]->wall = waNone;
    cwt.c->mov[6]->land = laHell;
    cwt.c->mov[6]->item = itHell;

    saveAchShot("ptm", 2.3); */
    }
  }

void verifyHell() {
  for(int i=0; i<50000; i++) {
    activateSafety(laHell);
    while(true) {
      setdist(cwt.c, 5, NULL);
      cwstep(cwt);
      if(cwt.c->wall != waNone) break;
      cwspin(cwt, 3);
      }
    cwstep(cwt);
    cellwalker z = cwt;
    int steps = 0;
    do {
      setdist(cwt.c, 6, NULL);
      cwspin(cwt, 1);
      cwstep(cwt);
      if(cwt.c->wall != waNone) {
        cwt.c->item = itGreenStone;
        cwstep(cwt);
        }
      else {
        if(cwt.c->item != itDiamond && cwt.c->item != itGreenStone) {
          cwt.c->item = itDiamond;
          steps++;
          }
        cwspin(cwt, 1);
        }
      }
    while(z.c != cwt.c || z.spin != cwt.spin);
    if(steps == 0) {i--; continue; }
    if(i % 20 == 0) { printf("\n"); fflush(stdout); }
    printf("%d,", steps); 
    if(steps > 100000000 || steps < 2) { fflush(stdout); break; }
    }
  }

bool sameland(eLand ll, eLand ln) {
  if(ln == laBarrier || ln == laOceanWall)
    return true;
  if(ln == ll) return true;
  if(isElemental(ln) && isElemental(ll)) return true;
  if(isHaunted(ln) && isHaunted(ll)) return true;
  return false;
  }

bool doAutoplay;

void autoplay() {
  // drawMesh();
  // exit(0);

  doAutoplay = true;

  cheater = 1;
  eLand lland = laIce;
  eLand lland2 = laIce;
  int lcount = 0;
  int gcount = 0;
  int lastgold = 0;
  
  int lastdraw = 0;
  while(doAutoplay) {
  
    if(gold() > lastgold) { 
      lastgold = gold();
      gcount = 0;
      }
    else gcount++;
    
    if(sameland(lland, cwt.c->land)) lcount++;
    else {
      lcount = 0; lland2 = lland; lland = cwt.c->land;
      printf("%10dcc %5dt %5de %5d$ %5dK %5dgc %-30s\n", cellcount, turncount, celldist(cwt.c), gold(), tkills(), gcount, dnameof(cwt.c->land));
      if(int(SDL_GetTicks()) > lastdraw + 3000) {
        lastdraw = SDL_GetTicks();
        extern void fullcenter();
        extern void mainloop();
        fullcenter();
        msgs.clear();
        drawscreen();
        }
      //mainloop();
      }
    int i = rand() % cwt.c->type;
    cell *c2 = cwt.c->mov[i];
    if(gcount < 500) for(int i=1; i<size(dcal); i++) {
      c2 = dcal[i];
      if(lcount >= 50 && !sameland(lland, c2->land) && !sameland(lland2, c2->land)) break;
      else if(lcount < 50 && c2->item && c2->item != itOrbSafety) break;
      }
    items[rand() % ittypes] = 1 << (rand() % 11);
    kills[rand() % motypes] = 1 << (rand() % 11);
    items[itOrbYendor] &= 15;
    again:
    if(c2->cpdist > 1) {
      for(int j=0; j<c2->type; j++)
        if(c2->mov[j] && c2->mov[j]->cpdist < c2->cpdist) {
          c2 = c2->mov[j];
          goto again;
          }
      }
    killMonster(c2, moNone);
    jumpTo(c2, itNone);
    if(false) if(turncount % 5000 == 0) {
      printf("cells travelled: %d\n", celldist(cwt.c));
      
      printf("\n");
    
      for(int i=0; i<ittypes; i++) if(items[i])  
        printf("%4dx %s\n", items[i], iinf[i].name);
        
      printf("\n");
      
      for(int i=1; i<motypes; i++) if(kills[i])  
        printf("%4dx %s <%d>\n", kills[i], minf[i].name, i);
      
      printf("\n\n\n");      
      }

    if(rand() % 5000 == 0 || (isGravityLand(cwt.c->land) && coastvalEdge(cwt.c) >= 100) || gcount > 2000 || cellcount >= 20000000) {
      printf("RESET\n");
      gcount = 0;
      cellcount = 0;
      activateSafety(land_tac[rand() % LAND_TAC].l);
      }
    
    for(int i=0; i<size(dcal); i++) {
      cell *c = dcal[i];
      if(isChild(c, NULL)) 
        ;
      if(childbug) doAutoplay = false;
      if(c->land == laNone) { 
        printf("no-land found\n"); 
        kills[moPlayer] = 0;
        canmove = true;
        doAutoplay = false; 
        }
      }
    
    if(buggyGeneration) break;
    if(size(buggycells)) break;

//  if(turncount == 50000) exit(0);
    }
  }

void verifyMutant(cell *c) {
  if(!c) return;
  if(c->monst == moMutant && c->mondir != NODIR && c->mov[c->mondir]->monst != moMutant) {
    raiseBuggyGeneration(c, "mutant failed!");
    childbug = true;
    }
  }

void verifyMutantAround(cell *c) {
  if(!c) return;
  verifyMutant(c);
  for(int i=0; i<c->type; i++) verifyMutant(c->mov[i]);
  if(childbug) printf("around!\n");
  }

priority_queue<pair<int, cell*> > riverqueue;

void enriver(cell *c) {
  if(c->wall != waNone) return;
  c->wall = waBigTree;
  int d = -(abs(getCdata(c, 0)-1) + abs(getCdata(c, 1)-1) + abs(getCdata(c, 2)-1));
  riverqueue.push(make_pair(d, c));
  }

/* void river() {
  enriver(cwt.c);
  for(int i=0; i<10000; i++) {
    cell *c = riverqueue.top().second;
    riverqueue.pop();
    c->wall = waFloorA;
    forCellCM(c2, c)  enriver(c2);
    }
  } */

void bandpicture();

int lcol(cell *c1, cell *c2) {
  bool h7 = c1->type == 7;
  if(!c2) return 0xFF0000FF;
  int i0 = zebra40(c1)&2;
  int i1 = zebra40(c2)&2;
  /* if(i0 == 2 && i1 == 2) return 0xFFFFFFFF;
  if(i0 == 0 && i1 == 0) return 0x404040FF;
  return 0xA0A0A0FF; */
  if(i0 == i1) return h7 ? 0x206020FF : 0x202020FF;
  return h7 ? 0x40F040FF : 0x00F000FF;
  }

void localdraw(const transmatrix &V, cell *c) {
  return;
  int ct = c->type;
    if(ct == 7) for(int i=0; i<7; i++) 
    queuepoly(V * spin(-2*M_PI*i/7), shBead1, lcol(c, c->mov[i]));
    
  if(ct == 6) for(int i=1; i<6; i+=2) if(c < c->mov[i])
    queuepoly(V * spin(-2*M_PI*i/6 + M_PI), shBead0, lcol(c, c->mov[i]));
  }

bool doCross;
extern bool fixseed;

namespace arg {
  int readLocal() {
    if(argis("-auto")) { PHASE(3); autoplay(); }
    else if(argis("-bmt")) buildmodetable();
    else if(argis("-cross")) { PHASE(2); fixseed = doCross = true, firstland = laCrossroads2; }
    else if(argis("-musictable")) {
      loadMusicInfo();
      for(int i=0; i<landtypes; i++)
        if(musfname[i] == "") printf("  NULL,\n");
        else printf("  @\"%s\", // %2d : %s\n", musfname[i].c_str(), i, dnameof(eLand(i)));
  
      printf("\n\n");
      
      for(int i=0; i<landtypes; i++)
        if(musfname[i] != "") {
          string s = musfname[i].substr(6, size(musfname[i])-10);
          printf("  if(curland == %2d) id = R.raw.%s; // %s\n", i, s.c_str(), dnameof(eLand(i)));
          }
      
      printf("\n\n");
  
      printf("musiclicense = \"");
      for(int i=0; i<size(musiclicense); i++)
        if(musiclicense[i] == '\n') printf("\\n");  else printf("%c", musiclicense[i]);
      printf("\";\n");
      exit(0);
      }
    else return 1;
    return 0;
    }
  }

hyperpoint checkoptim(transmatrix T) {
  return T*C0;
  }
