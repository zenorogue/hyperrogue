// PRIVATE

namespace anticheat {

  bool tampered;

  void save(FILE *f) {
    int cert = 0x1234;
    for(int i=0; i<boxid; i++)
      cert = 3 * cert + (761349061 + 1463190*i) * savebox[i];
    if(!tampered) fprintf(f, " C %d\n", cert);
    }

  bool load(FILE *f, score& sc, string ver) {
    int cert = 0x1234;
    for(int i=0; i<boxid; i++)
      cert = 3 * cert + (761349061 + 1463190*i) * sc.box[i];
    char cr;
    int scert;
    int err = fscanf(f, "%c%d", &cr, &scert);
    if(err != 2) return true;
    // if(cr == 'C' && cert == scert) printf("Certificate OK\n");
    return cert != scert;
    }

  int vcertify(string ver, string ss, int a, int b, int c, int d) { 
    ss = ver + ss;
    const char *s = ss.c_str();
    if(tampered) return -1;
    int cert = 0x5678;
    while(*s) cert = cert * 37113 + (*s), s++;
    cert = cert + a * 681491 + b * 681390411 + c * 581911 + d * 16406101;
    return cert;
    }
    
  int certify(string ss, int a, int b, int c, int d) {
    return vcertify(VER, ss, a, b, c, d);
    }
    
  int check(int cv, const string& ver, const string& s, int a, int b, int c, int d) { 
    if(ver < "8.0f" && cv == d) return true;
    return cv == vcertify(ver, s, a, b, c, d);
    }
  
  void nextid(int& tid, const string& ver, int cert) { 
    if(ver >= "8.0i") tid += 1 + cert + cert * cert / 7; 
    else tid++;
    }

// 8.0i: ver included in achievements
// 8.0f: fixed in Windows
  }

#ifndef MOBILE
void SDL_Delay_anticheat(int i) {
  saveBox();
  int i0 = 0; savebox[1] = savebox[65] = anticheat::tampered; // do not consider Timer
  for(int t=0; t<boxid; t++) 
    i0 = 73 * i0 + savebox[t] + savebox[t] * (savebox[t] + 7103);
  for(int t=0; t<boxid; t++) savebox[t] = 0;
  SDL_Delay(i);

  saveBox(); 
  int i1 = 0; savebox[1] = savebox[65] = anticheat::tampered; // do not consider Timer
  for(int t=0; t<boxid; t++) 
    i1 = 73 * i1 + savebox[t] + savebox[t] * (savebox[t] + 7103);
  
  if(i0 != i1) {
    anticheat::tampered = true;
    }
  }
#endif

#define SDL_Delay SDL_Delay_anticheat

