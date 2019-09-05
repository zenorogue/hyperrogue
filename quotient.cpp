// Hyperbolic Rogue -- quotient spaces
// Copyright (C) 2011-2019 Zeno Rogue, see 'hyper.cpp' for details

/** \file quotient.cpp
 *  \brief quotient spaces
 */

#include "hyper.h"
namespace hr {

// --- quotient geometry ---

EX namespace quotientspace {
  struct code {
    int c[MAX_EDGE+1];
    };
  
  bool operator == (const code& c1, const code &c2) {
    for(int i=0; i<=S7; i++) if(c1.c[i] != c2.c[i]) return false;
    return true;
    }

  bool operator < (const code& c1, const code &c2) {
    for(int i=0; i<=S7; i++) if(c1.c[i] != c2.c[i]) return c1.c[i] < c2.c[i];
    return false;
    }  

  int cod(heptagon *h) {
    return zebra40(h->c7);
    }
  
  code get(heptspin hs) {
    code res;
    res.c[0] = cod(hs.at);
    for(int i=1; i<=S7; i++) {
      res.c[i] = cod((hs + wstep).at);
      hs += 1;
      }
    return res;
    }
  
  int rvadd = 0, rvdir = 1;
  
  int rv(int x) { return (rvadd+x*rvdir) % S7; }
  
struct hrmap_quotient : hrmap_standard {

  hrmap_hyperbolic base;
  
  vector<cell*> celllist;
  
  cell *origin;
  
  map<quotientspace::code, int> reachable;
  vector<heptspin> bfsq;
  
  vector<int> connections;
  
  void add(const heptspin& hs) {
    code g = get(hs);
    if(!reachable.count(g)) {
      reachable[g] = bfsq.size();
      bfsq.push_back(hs);
      add(hs + 1);
      }
    }

  vector<heptagon*> allh;
  
  hrmap_quotient() {  
  
    dynamicval<hrmap*> cmap(currentmap, this);
  
    static int symmask = (1<<30);

    connections.clear();
    switch(geometry) {
      #if CAP_FIELD
      case gFieldQuotient: {
        connections = currfp.connections;
        break;
        }
      #endif
        
      case gZebraQuotient: {
        heptspin hs(base.origin);
        reachable.clear();
        bfsq.clear();
        add(hs);
  
        for(int i=0; i<(int)bfsq.size(); i++) {
          hs = bfsq[i] + wstep;
          add(hs);
          connections.push_back(reachable[get(hs)]);
          }
        break;
        }
      
      case gMinimal: {
        int altzebra[6][7] = {  
          { 16,125,111, 45, 32, 56, 20 },
          { 26,102,146,152, 35,124, 00 },
          { 06, 55,143,134,115,101, 10 },
          { 41, 50, 04, 44,123, 14,153 },
          { 51, 30,154,122, 33, 03,112 },
          { 31, 40,113,136,142, 21, 05 }
          };
               
        // int ok = 0;
        for(int a=0; a<6; a++) {
        for(int b=0; b<7; b++) {
          int s = altzebra[a][b];
          int mirr = s/100; s %= 100;
          int which = s/10; s %= 10;
          
          int shouldbe = a*10+b+mirr*100;
          
          if(altzebra[which][s] != shouldbe) {
            printf("error at %d:%d (is=%d shouldbe=%d)\n", a, b, altzebra[which][s], shouldbe);
            }
          
          connections.push_back(which * 7 + s + (mirr ? symmask : 0) );
          }
          }
        break;
        }
      
      case gKleinQuartic: {
        connections = {
          /* 000 */ 7, 14, 21, 28, 35, 42, 49,
          /* 001 */ 0, 55, 56, 63, 70, 77, 15,
          /* 002 */ 1, 13, 83, 84, 91, 98, 22,
          /* 003 */ 2, 20, 104, 105, 112, 119, 29,
          /* 004 */ 3, 27, 125, 74, 126, 133, 36,
          /* 005 */ 4, 34, 139, 95, 66, 140, 43,
          /* 006 */ 5, 41, 146, 116, 87, 147, 50,
          /* 007 */ 6, 48, 153, 130, 108, 57, 8,
          /* 008 */ 9, 54, 107, 102, 154, 142, 64,
          /* 009 */ 10, 62, 141, 39, 94, 161, 71,
          /* 010 */ 11, 69, 167, 127, 31, 124, 78,
          /* 011 */ 12, 76, 123, 158, 149, 85, 16,
          /* 012 */ 17, 82, 148, 46, 115, 163, 92,
          /* 013 */ 18, 90, 162, 67, 38, 138, 99,
          /* 014 */ 19, 97, 137, 155, 59, 106, 23,
          /* 015 */ 24, 103, 58, 53, 129, 165, 113,
          /* 016 */ 25, 111, 164, 88, 45, 145, 120,
          /* 017 */ 26, 118, 144, 159, 79, 75, 30,
          /* 018 */ 32, 73, 166, 109, 52, 152, 134,
          /* 019 */ 33, 132, 151, 156, 100, 96, 37,
          /* 020 */ 40, 65, 61, 160, 121, 117, 44,
          /* 021 */ 47, 86, 81, 157, 135, 131, 51,
          /* 022 */ 60, 101, 136, 150, 80, 122, 143,
          /* 023 */ 68, 93, 89, 114, 110, 128, 72,
          };
        break;
        }
      
      case gBolza: {
        connections = {
          /* 000 */ 8, 16, 24, 32, 12, 20, 28, 36,
          /* 001 */ 0, 35, 47, 21, 4, 39, 43, 17,
          /* 002 */ 1, 15, 42, 29, 5, 11, 46, 25,
          /* 003 */ 2, 23, 45, 37, 6, 19, 41, 33,
          /* 004 */ 3, 31, 40, 9, 7, 27, 44, 13,
          /* 005 */ 34, 30, 18, 14, 38, 26, 22, 10,
          };
        break;
        }
      
      case gBolza2: {
        connections = {
/* 000 */ 16, 32, 48, 64, 24, 40, 56, 72,
/* 001 */ 20, 44, 52, 76, 28, 36, 60, 68,
/* 002 */ 0, 79, 83, 45, 8, 67, 95, 33,
/* 003 */ 4, 71, 87, 37, 12, 75, 91, 41,
/* 004 */ 1, 23, 94, 61, 13, 27, 86, 49,
/* 005 */ 5, 31, 90, 53, 9, 19, 82, 57,
/* 006 */ 2, 39, 85, 77, 10, 43, 89, 65,
/* 007 */ 6, 47, 81, 69, 14, 35, 93, 73,
/* 008 */ 3, 55, 88, 21, 15, 59, 80, 25,
/* 009 */ 7, 63, 92, 29, 11, 51, 84, 17,
/* 010 */ 70, 58, 46, 18, 78, 50, 38, 26,
/* 011 */ 66, 54, 42, 30, 74, 62, 34, 22,
          };
        break;
        }

      case gMacbeath: {
        connections = {
/* 000 */ 359, 498, 189, 215, 424, 381, 20, 
/* 001 */ 366, 491, 217, 187, 431, 346, 27, 
/* 002 */ 380, 484, 168, 208, 438, 360, 6, 
/* 003 */ 345, 477, 196, 180, 445, 367, 13, 
/* 004 */ 352, 470, 203, 173, 396, 388, 48, 
/* 005 */ 373, 463, 175, 201, 403, 339, 55, 
/* 006 */ 387, 456, 210, 194, 410, 353, 34, 
/* 007 */ 338, 449, 182, 222, 417, 374, 41, 
/* 008 */ 254, 459, 140, 138, 414, 248, 69, 
/* 009 */ 247, 480, 112, 166, 435, 255, 62, 
/* 010 */ 233, 452, 161, 117, 407, 269, 83, 
/* 011 */ 268, 487, 133, 145, 442, 234, 76, 
/* 012 */ 261, 473, 126, 152, 400, 241, 97, 
/* 013 */ 240, 494, 154, 124, 421, 262, 90, 
/* 014 */ 226, 466, 119, 159, 393, 276, 111, 
/* 015 */ 275, 501, 147, 131, 428, 227, 104, 
/* 016 */ 65, 479, 343, 355, 408, 73, 167, 
/* 017 */ 100, 465, 371, 383, 422, 94, 160, 
/* 018 */ 86, 472, 350, 348, 429, 108, 153, 
/* 019 */ 79, 486, 378, 376, 415, 59, 146, 
/* 020 */ 58, 458, 385, 369, 443, 80, 139, 
/* 021 */ 107, 500, 357, 341, 401, 87, 132, 
/* 022 */ 93, 493, 364, 390, 394, 101, 125, 
/* 023 */ 72, 451, 336, 362, 436, 66, 118, 
/* 024 */ 16, 483, 294, 313, 397, 31, 209, 
/* 025 */ 37, 462, 322, 285, 446, 24, 202, 
/* 026 */ 51, 448, 287, 334, 432, 10, 223, 
/* 027 */ 2, 497, 315, 306, 411, 45, 216, 
/* 028 */ 23, 476, 308, 299, 404, 38, 181, 
/* 029 */ 30, 469, 280, 327, 439, 17, 174, 
/* 030 */ 44, 455, 329, 292, 425, 3, 195, 
/* 031 */ 9, 490, 301, 320, 418, 52, 188, 
/* 032 */ 324, 467, 98, 110, 427, 332, 258, 
/* 033 */ 289, 453, 70, 82, 441, 283, 265, 
/* 034 */ 303, 495, 91, 89, 399, 297, 272, 
/* 035 */ 310, 481, 63, 61, 413, 318, 279, 
/* 036 */ 331, 460, 56, 68, 434, 325, 230, 
/* 037 */ 282, 474, 84, 96, 420, 290, 237, 
/* 038 */ 296, 488, 77, 75, 406, 304, 244, 
/* 039 */ 317, 502, 105, 103, 392, 311, 251, 
/* 040 */ 205, 475, 259, 236, 447, 178, 328, 
/* 041 */ 184, 454, 231, 264, 426, 213, 335, 
/* 042 */ 170, 489, 266, 243, 405, 199, 314, 
/* 043 */ 219, 496, 238, 271, 412, 192, 321, 
/* 044 */ 198, 482, 245, 278, 398, 171, 300, 
/* 045 */ 191, 503, 273, 250, 419, 220, 307, 
/* 046 */ 177, 468, 224, 257, 440, 206, 286, 
/* 047 */ 212, 461, 252, 229, 433, 185, 293, 
/* 048 */ 163, 450, 49, 40, 402, 150, 363, 
/* 049 */ 114, 478, 21, 12, 430, 129, 356, 
/* 050 */ 128, 471, 28, 47, 409, 115, 349, 
/* 051 */ 149, 499, 0, 19, 437, 164, 342, 
/* 052 */ 156, 492, 7, 26, 444, 143, 391, 
/* 053 */ 121, 464, 35, 54, 416, 136, 384, 
/* 054 */ 135, 485, 14, 5, 423, 122, 377, 
/* 055 */ 142, 457, 42, 33, 395, 157, 370, 
/* 056 */ 277, 102, 158, 389, 32, 172, 312, 
/* 057 */ 242, 88, 151, 340, 39, 200, 298, 
/* 058 */ 270, 74, 116, 354, 46, 193, 305, 
/* 059 */ 249, 60, 137, 375, 53, 221, 319, 
/* 060 */ 263, 95, 123, 382, 4, 214, 291, 
/* 061 */ 228, 109, 130, 347, 11, 186, 333, 
/* 062 */ 256, 67, 165, 361, 18, 207, 326, 
/* 063 */ 235, 81, 144, 368, 25, 179, 284, 
/* 064 */ 183, 50, 337, 162, 71, 232, 288, 
/* 065 */ 211, 43, 386, 141, 57, 253, 330, 
/* 066 */ 176, 36, 372, 120, 99, 225, 323, 
/* 067 */ 204, 29, 351, 127, 85, 260, 281, 
/* 068 */ 197, 22, 344, 113, 64, 246, 309, 
/* 069 */ 169, 15, 379, 134, 78, 267, 295, 
/* 070 */ 218, 8, 365, 155, 92, 239, 302, 
/* 071 */ 190, 1, 358, 148, 106, 274, 316, 
          };
        break;
        }
      
      case gBring: {
        connections = {
/* 000 */ 5, 92, 46, 33, 14, 
/* 001 */ 0, 62, 56, 38, 24, 
/* 002 */ 15, 97, 76, 63, 4, 
/* 003 */ 10, 32, 86, 68, 29, 
/* 004 */ 25, 67, 106, 93, 9, 
/* 005 */ 20, 37, 116, 98, 19, 
/* 006 */ 35, 102, 16, 3, 44, 
/* 007 */ 30, 72, 26, 8, 54, 
/* 008 */ 45, 107, 66, 73, 34, 
/* 009 */ 40, 2, 81, 78, 59, 
/* 010 */ 55, 77, 96, 103, 39, 
/* 011 */ 50, 7, 111, 108, 49, 
/* 012 */ 65, 112, 6, 13, 74, 
/* 013 */ 60, 42, 21, 18, 84, 
/* 014 */ 75, 117, 36, 43, 64, 
/* 015 */ 70, 12, 51, 48, 89, 
/* 016 */ 85, 47, 91, 113, 69, 
/* 017 */ 80, 17, 101, 118, 79, 
/* 018 */ 95, 82, 1, 23, 104, 
/* 019 */ 90, 52, 11, 28, 114, 
/* 020 */ 105, 87, 31, 53, 94, 
/* 021 */ 100, 22, 41, 58, 119, 
/* 022 */ 115, 57, 61, 83, 99, 
/* 023 */ 110, 27, 71, 88, 109,         
          };
        break;
        }

      case gSchmutzM2: {
        connections = {
          23,  47,  27,  14,  38,  30,  17,  41,  33,  20,  44,  24, 
          35,  39,   3,  26,  42,   6,  29,  45,   9,  32,  36,   0, 
          11,  43,  15,   2,  46,  18,   5,  37,  21,   8,  40,  12, 
          22,  31,   4,  13,  34,   7,  16,  25,  10,  19,  28,   1, 
          };
        break;
        }
      
      case gSchmutzM3: {
        connections = {
          23,  47,  64,  28,  15,  39,  68,  32,  19,  43,  60,  24, 
          35,  52,  40,   4,  27,  56,  44,   8,  31,  48,  36,   0, 
          11,  71,  57,  16,   3,  63,  49,  20,   7,  67,  53,  12, 
          22,  59,  69,   5,  14,  51,  61,   9,  18,  55,  65,   1, 
          21,  30,  62,  41,  13,  34,  66,  45,  17,  26,  70,  37, 
          10,  42,  50,  29,   2,  46,  54,  33,   6,  38,  58,  25, 
          };
        break;
        }
        
      default: break; 
      }
    
    int TOT = connections.size() / S7;
    // printf("heptagons = %d\n", TOT);
    // printf("all cells = %d\n", TOT*(S7+S3)/S3);
    if(!TOT) exit(1);
    allh.resize(TOT);
    for(int i=0; i<TOT; i++) allh[i] = tailored_alloc<heptagon> (S7);
    // heptagon *oldorigin = origin;
    allh[0]->alt = base.origin;
  
    for(int i=0; i<TOT; i++) {
      heptagon *h = allh[i];
      if(i) {
        h->alt = NULL;
        }
      if(true) {
        h->s = hsOrigin;
        h->emeraldval = 0;
        h->zebraval = 0;
        h->fiftyval = 0;
        h->fieldval = S7*i;
        h->rval0 = h->rval1 = 0; h->cdata = NULL;
        h->distance = 0;
        if(!IRREGULAR) h->c7 = newCell(S7, h);
        else h->c7 = NULL;
        }
      for(int j=0; j<S7; j++) {
        int co = connections[i*S7+j];
        bool swapped = co & symmask;
        co &= ~symmask;
        h->move(rv(j)) = allh[co/S7];
        h->c.setspin(rv(j), rv(co%S7), swapped);
        }
      }

    vector<heptagon*> by_dist;
    by_dist.push_back(allh[0]);
    for(int i=0; i<TOT; i++) {
      if(i >= isize(by_dist)) { printf("too fast\n"); exit(1); }
      for(int a=0; a<S7; a++) if(by_dist[i]->move(a)->alt == NULL) by_dist.push_back(by_dist[i]->move(a));
      currentmap->generateAlts(by_dist[i], 0, false);
      }
  
    for(int i=0; i<TOT; i++) {
      allh[i]->emeraldval = allh[i]->alt->emeraldval;
      allh[i]->zebraval = allh[i]->alt->zebraval;
      allh[i]->fiftyval = allh[i]->alt->fiftyval;
      allh[i]->distance = allh[i]->alt->distance;
      /* for(int j=0; j<7; j++)
        allh[i]->move[j]->alt = createStep(allh[i]->alt, j); */
      }    
    
    #if CAP_IRR
    if(IRREGULAR) {
      irr::link_start(allh[0]);
      for(int i=0; i<TOT; i++)
        for(int j=0; j<S7; j++)
          irr::may_link_next(allh[i], j);
      }
    #endif

    celllister cl(gamestart(), 100, 100000000, NULL);
    celllist = cl.lst;
    }

  heptagon *getOrigin() { return allh[0]; }

  ~hrmap_quotient() {
    for(int i=0; i<isize(allh); i++) {
      clearHexes(allh[i]);
      tailored_delete(allh[i]);
      }
    }
  
  vector<cell*>& allcells() { return celllist; }
  };

EX hrmap* new_map() { return new hrmap_quotient; }

  };


}
