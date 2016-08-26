// Hyperbolic Rogue pattern generator

// Copyright (C) 2011-2016 Zeno Rogue, see 'hyper.cpp' for details

void spill50(cell *c, eWall w, int r) {
  c->wall = w;
  if(r) for(int i=0; i<c->type; i++) spill50(createMov(c, i), w, r-1);
  }

int style = 3;

struct state50 {
  bool polarity1;
  bool polarity2;
  int color, color2, wc2;
  };

void progress(cell *c, state50 s);

#include <map>

map<cell*, state50> map50;

void initPatternFifty(cell *c) {
  state50 s;
  s.polarity1 = 0;
  s.polarity2 = 0;
  s.color = 0;
  s.color2 = 1;
  s.wc2 = 0;
  map50[c] = s;
  }

int par;

int haverule[1<<24][8];

void saveCode(heptagon *H1, int d, heptagon *H2) {

/*
  if(!H1 || !H2) return;
  if(H1->move[d] != H2) exit(1);
  
  int h1 = H1->fiftyval;
  int h2 = H2->fiftyval;

  if(!h1 || !h2) return;

  if(haverule[h1][d] && haverule[h1][d] != h2) {
    printf("rule conflict: %06x, %d, %06x/%06x\n", 
      h1, d, h2, haverule[h1][d]);
    // exit(1);
    }
    
  if(!haverule[h1][d]) {
    haverule[h1][d] = h2;
 
    printf("RULE50(0x%06x, %d, 0x%06x)\n", h1, d, h2);
    } */

  fflush(stdout);
  }
  
void saveCode(cell *c) {

/*  heptagon *hept = c->master;
  
  if(hept != &origin) 
    saveCode(hept->move[0], hept->spin[0], hept); */
    
  bool allcodes = c->master->fiftyval >= 0;

  for(int i=0; i<7; i++) 
    if(c->master->move[i] && c->master->move[i]->fiftyval >= 0) ;
    else allcodes = false;
  
  if(allcodes)  {
    printf("RULE50(0x%03x", c->master->fiftyval);
    int rulebase = 0;
    for(int i=0; i<7; i++) 
      if(c->master->move[i]->fiftyval < c->master->move[rulebase]->fiftyval)
        rulebase = i;
    for(int i=0; i<7; i++) printf(", 0x%03x", c->master->move[(rulebase+i)%7]->fiftyval);
    printf(")\n");
    }
  
  // hept->fiftyused = true;
  }

void encode(cell *c, state50 s, int mod, int spn) {
/*int i = 0;
  i *= 16; i += s.color;
  i *= 16; i += s.color2;
  i *= 16; i += s.polarity1;
  i *= 16; i += s.polarity2;
  i *= 16; i += mod;
  i *= 16; i += spn;
  if(c->master->fiftyval) { printf("multiply 50val\n"); exit(1); } 
  c->master->fiftyval = i; c->master->fiftyused = false; */
  
  c->master->fiftyval = 
    s.color + s.polarity1 * 8 + s.polarity2 * 16 + mod * 32;
  // s.color * 16 + (s.polarity1 ? 128 : 0) + (s.polarity2 ? 256 :0) + mod;
  
  saveCode(c);
  for(int i=0; i<7; i++)
    saveCode(createStep(c->master, i)->c7);
    
  /* for(int i=0; i<7; i++) if(c->master->move[i]) {
    saveCode(c->master, i, c->master->move[i]);
    saveCode(c->master->move[i], c->master->spin[i], c->master);
    } */
  }

eWall colorwalls[4] = {waCIsland, waCIsland2, waMineOpen, waDeadfloor };

void patternFiftyAt(cell *c) {
  if(!map50.count(c)) return;
  
  state50 s = map50[c];
  
  // c->wall = waCIsland;
  
  // if(c->heat > ii) return;
  
  // printf("pfifty %p\n", c);
  
  if(style == 1 && s.polarity2) {
      spill50(c, waCamelot, 3);
      spill50(c, waNone, 2);

      for(int i=0; i<c->type; i++) {
        cellwalker cw(c, i);
        cwstep(cw); // cw.c->item = itSilver;
        cwspin(cw, 4); // 6
        cwstep(cw); // cw.c->item = itSilver;
        cwspin(cw, 3); // 6
        cwstep(cw); cw.c->wall = waFloorA; cwstep(cw);
        cwspin(cw, 1);
        cwstep(cw); cw.c->wall = waFloorA; cwstep(cw);
        }
      }
   
  /*switch(ii) {
    case 0:
      spill50(c, waNone, 3);
      break;

    case 1:
      spill50(c, waNone, 3);
      break;

    case 2:
      spill50(c, waNone, 3);
      break;

    case 3:
      spill50(c, waNone, 3);
      break;
    } */
    
  if(style == 2) {
    spill50(c, waCavefloor, 2);
    spill50(c, waFloorA, 1);
    }
  
  if(style == 3) {
    spill50(c, colorwalls[s.color], 3);
    // c->item = itGold;
//  if(s.polarity2) return;
    }

  encode(c, s, 0, 0);
  
  int sgn = s.polarity2 ? 1 : -1;
  int sgn1 = s.polarity1 ? 1 : -1;

  int col2 = s.color2, sw = s.wc2;
  
  while(col2 != 7-s.color) {
    sw += (s.polarity1?1:-1); sw %= 7;
    while(true) { col2++; col2 &= 7; if(col2 != s.color) break; }
    }  

  for(int i=0; i<c->type; i++) {
    cellwalker cw(c, sw);
    cwspin(cw, sgn1 * i);
    cwstep(cw);
    cwspin(cw, sgn*4);
    cwstep(cw); 
    if(cw.spin < 0 || cw.spin >= 7 || cw.c->type != 7) exit(1);
    encode(cw.c, s, 1+i, cw.spin);
    }
  
  for(int i=0; i<c->type; i++) {
    cellwalker cw(c, sw);
    cwspin(cw, sgn1 * i);
    cwstep(cw);
    cwspin(cw, 3);
    cwstep(cw);
    cwspin(cw, 3);
    cwstep(cw);
    if(cw.spin < 0 || cw.spin >= 7 || cw.c->type != 7) exit(1);
    encode(cw.c, s, 8+i, cw.spin);
    }
  
  // c->heat = s.color + 
    
  for(int i=0; i<c->type; i++) {
    cellwalker cw(c, s.wc2);
    cwspin(cw, sgn1 * i);
    cwstep(cw); 
    if(style == 0) cw.c->wall = waCamelot;
    // cw.c->item = itSilver;
    cwspin(cw, sgn*4); //6
    cwstep(cw); if(style == 0) cw.c->wall = waFloorA;

    // cw.c->item = itSilver;
    cwspin(cw, sgn*4); //7
    cwstep(cw); if(style == 0) cw.c->wall = waFloorA;
    // cw.c->item = itSilver;
    cwspin(cw, 3); //6
    cwstep(cw); if(style == 0) cw.c->wall = waFloorA;
    // cw.c->item = itSilver;
    cwspin(cw, 3); //6
    cwstep(cw); if(style == 0) cw.c->wall = waFloorA;
    // cw.c->item = itSilver;
    cwspin(cw, sgn*3); //7
    cwstep(cw); if(style == 0) cw.c->wall = waCamelot;
    // cw.c->item = itSilver;
    cwspin(cw, sgn*2); //6
    cwstep(cw); // cw.c->item = itGold;
    // setdist(cw.c, 8, NULL);
    state50 s2 = s; s2.polarity1 = !s.polarity1;
    s2.wc2 = (cw.spin + sgn1 * i + sgn + 42) % 7;
    progress(cw.c, s2);
    // printf("heat set %f\n", cw.c->heat);
    }
    
  int newcol = s.color2;
  
  // if(s.polarity2) return;
  
  for(int i=0; i<c->type; i++) {
    cellwalker cw(c, s.wc2);
    cwspin(cw, sgn1 * i);
    cwstep(cw); // cw.c->item = itSilver;
//  cw.c->item = itDiamond;
    cwspin(cw, 3); // 6
    cwstep(cw); // cw.c->item = itSilver;
//  cw.c->item = itDiamond;
    cwspin(cw, sgn*4); // 6
    cwstep(cw); // cw.c->item = itSilver;
//  cw.c->item = itDiamond;
    cwspin(cw, sgn*2); // 6
    cwstep(cw); // cw.c->item = itSilver;
//  cw.c->item = itDiamond;
    cwspin(cw, 3); // 6
    cwstep(cw); // cw.c->item = itSilver;
//  cw.c->item = itDiamond;
    cwspin(cw, sgn*3); // 7
    cwstep(cw); // cw.c->item = itSilver;
//  cw.c->item = itDiamond;
    cwspin(cw, sgn*4); // 6
    cwstep(cw); // cw.c->item = itSilver;
    // setdist(cw.c, 8, NULL);
    state50 s2 = s; 
    s2.polarity2 = !s.polarity2;
    if(s.polarity2) s2.polarity1 = !s.polarity1;
    s2.color2 = s2.color;
    s2.color = newcol;
    s2.wc2 = cw.spin;
    progress(cw.c, s2);
    while(true) { newcol++; newcol &= 7; if(newcol != s2.color && newcol != s.color) break; }
    // printf("heat set %f\n", cw.c->heat);
    }
  
  }

void progress(cell *c, state50 s) {
  
  while(s.wc2) {
    s.wc2 += (s.polarity1?1:-1); s.wc2 %= 7;
    while(true) { s.color2++; s.color2 &= 7; if(s.color2 != s.color) break; }
    }
  
  if(map50.count(c)) {
    state50 s2 = map50[c];
    if(s2.polarity1 != s.polarity1 || s2.polarity2 != s.polarity2) {
      printf("Polarity mismatch!\n");
      exit(1);
      }
    else {
      if(s2.color != s.color || s2.color2 != s.color2 || s2.wc2 != s.wc2)
        printf("%d:%d color= %dv%d color2= %dv%d wc2= %dv%d\n", 
          s.polarity1, s.polarity2,
          s.color, s2.color,
          s.color2, s2.color2,
          s.wc2, s2.wc2);
      }
    return;
    }
  map50[c] = s;
  if(c->mpdist <= 7) 
    patternFiftyAt(c);
  }

long long Q; int qconflict;

string allconflict;

void setzebra(cellwalker cwb, int it, int type, string pathcode, int xmods) {
  // printf("spin=%d type=%d\n", cwb.spin, type);
  
  if(cwb.spin != 1 && cwb.spin != 3 && cwb.spin != 5) {
    printf("S WRONG SPIN %d\n", cwb.spin);
    exit(1);
    }
  if(type < 0 || type > 3) {
    printf("S WRONG TYPE %d\n", type);
    exit(1);
    }

  cwb.c->tmp = cwb.spin + 16 * type;
  }

void zebra(cellwalker cwb, int it, int type, string pathcode, int xmods) {
  if(!it) return;
  if(cwb.spin != 1 && cwb.spin != 3 && cwb.spin != 5) {
    printf("WRONG SPIN %d\n", cwb.spin);
    exit(1);
    }
  if(type < 0 || type > 3) {
    printf("WRONG TYPE %d\n", type);
    exit(1);
    }
  // printf("%p+%d = 0%s\n", cwb.c, cwb.spin, pathcode.c_str());
  bool redraw = false;
  // int qval = Q + 99;
  // redraw = cwb.c->heat == qval;
  // cwb.c->heat = qval;
  eWall w = colorwalls[type];
  cwb.c->wall = w;
  for(int i=0; i<6; i+=2) {
    cellwalker cw = cwb;
    cwspin(cw, i);
    cw.c->heat = 4 + type + 4 * 9;
    cwstep(cw); cwspin(cw, 3); cw.c->wall = w;
    int i0 = i; if(type&2 && i0) i0 = 6-i0; i0 /= 2;
    cw.c->heat = 4 + type + 4 * (3+i0);
    cwstep(cw); cwspin(cw, 3); cw.c->wall = w;
    cw.c->heat = 4 + type + 4 * i0;
    cwstep(cw); cwspin(cw, 3); cw.c->wall = w;
    cw.c->heat = 4 + type + 4 * (6+i0);
    cwstep(cw); cwspin(cw, -3); 
    cwstep(cw); cwspin(cw, -3); 
    cwstep(cw); cwspin(cw, -3); 
    cwstep(cw); cwspin(cw, -i);
    cwspin(cw, 0);
    setzebra(cw, it-1, type ^ 1, pathcode +'+'+char('A'+i)+char('0'+type), xmods*2); 
    }

  for(int i=0; i<6; i+=2) {
    cellwalker cw = cwb;
    cwspin(cw, (type&2)?-i:i);
    cwstep(cw); cwspin(cw, 3); 
    cwstep(cw); cwspin(cw, 5); 
    if(xmods < 2) {
      if(cw.c->item && cw.c->item != (1+i) && redraw && i==0) {
        qconflict++;
        // allconflict += pathcode; allconflict += "-";
        // cwb.c->item = itPalace;
        // printf("Conflict at %p\n", cwb.c);
        }
      // cw.c->item = eItem(1 + i);
      // cw.c->heat = 4 + type + 4 * (i/2);
      }
    cwstep(cw); cwspin(cw, 1); 
    if(type < 2) {
      // cwspin(cw, i);
      cwspin(cw, i); 
      }
    else {
      cwspin(cw, -i); 
      }
    // cwspin(cw, ((Q >> (4*type)) & 12));
    setzebra(cw, it-1, 2^type, pathcode + '+'+char('a'+i)+char('0'+type), xmods+1);
    }
  }

void zebraPattern() {
  // int bqc = 99;
  
  /* for(Q=0; Q<65536; Q++) {
    if((Q & (Q>>1)) & 0x5555) continue;
    qconflict = false;
    cellwalker cw(cwt);
    cwstep(cw); cwspin(cw, 1);
    qconflict = 0;
    allconflict = "";
    zebra(cw, 3, 0, "");
    if(qconflict < bqc) bqc = qconflict;
    // if(qconflict == bqc) 
    printf("%X - X-%sX\n", Q, allconflict.c_str());
    }
    
  Q = 0xFFFB; */
  
  cellwalker cw(cwt);
  cwstep(cw); cwspin(cw, 1);
  setzebra(cw, 7, 0, "", -999);
  // cw.c->
  // printf("Conflicts: %d\n", qconflict);
  }

int bspin(heptagon *h) {
  vector<int> xv;
  xv.push_back(999);
  int besti = -1;
  for(int i=0; i<7; i++) {
    vector<int> yv;
    for(int j=0; j<7; j++) yv.push_back(int(h->move[(i+j)%7]->c7->heat+.1));
    if(yv < xv) xv = yv, besti = i;
    }
  return besti;
  }

void buildAutomatonRule(heptagon *h) {
  if(!h->c7->heat) return;
  for(int i=0; i<7; i++) if(!h->move[i]) return;
  for(int i=0; i<7; i++) for(int j=0; j<7; j++) 
    if(!h->move[i]->move[j] || !h->move[i]->move[j]->c7->heat) 
      return;
  int bi = bspin(h);
  printf("RULEZEBRA(%2d", int(h->c7->heat+.1));
  for(int j=0; j<7; j++) {
    heptagon *h2 = h->move[(bi+j)%7];
    int bi2 = bspin(h2);
    printf(", %2d%d", int(h2->c7->heat+.1), fix7(bi2 - h->spin[(bi+j)%7]));
    }
  printf(")\n");
  }

void buildAutomatonRule(cell *c) {
  if(c->type == 7)
    buildAutomatonRule(c->master);
  else {
    int ii[3];
    for(int i=0; i<6; i+=2) if(!c->mov[i] || !c->mov[i]->heat || c->mov[i]->type != 7) return;
    for(int i=0; i<6; i+=2) ii[i/2] = int(c->mov[i]->heat);
    int z;
    for(int r=0; r<2; r++) 
      if(ii[1] < ii[0] || ii[2] < ii[0]) 
        z = ii[0], ii[0] = ii[1], ii[1] = ii[2], ii[2] = z;
    printf("RULEZEBRA6(%d,%d,%d,%d)\n", int(c->heat), int(ii[0]), int(ii[1]), int(ii[2]));
    }
  }

// #define BUILDZEBRA
