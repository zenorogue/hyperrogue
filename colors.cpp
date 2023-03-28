// Hyperbolic Rogue -- color routines
// Copyright (C) 2011-2020 Zeno Rogue, see 'hyper.cpp' for details

/** \file colors.cpp 
 *  \brief This file implements routines related to colors
 */

#include "hyper.h"
namespace hr {

/** \brief Return a reference to i-th component of col. 
 *  \arg i For colors with alpha, A=0, R=1, G=2, B=3. For colors without alpha, R=0, G=1, B=2.
 */
EX unsigned char& part(color_t& col, int i) {
  unsigned char* c = (unsigned char*) &col;
#if ISMOBILE
  return c[i];
#else
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
  return c[sizeof(col) - 1 - i];
#else
  return c[i];
#endif
#endif
  }

#if HDR
static const color_t NOCOLOR = 0;

struct colortable: vector<color_t> {
  color_t& operator [] (int i) { i %= size(); if(i<0) i += size(); return ((vector<color_t>&)(*this)) [i]; }
  const color_t& operator [] (int i) const { i %= size(); if(i<0) i += size(); return ((vector<color_t>&)(*this)) [i]; }
  colortable(std::initializer_list<color_t> v) : vector(v) {}
  colortable() : vector({0}) {}
  void allocate(int num) {
    int q = size();
    for(int i=q; i<num; i++) push_back((*this)[i % q]);
    }
  };
#endif

/* darkening routines */

EX color_t darkena3(color_t c, int lev, int a) {
  return (darkenedby(c, lev) << 8) + a;
  }

EX color_t darkena(color_t c, int lev, int a) {
  return darkena3(c, lev, GDIM == 3 ? 255 : a);
  }

EX int darkenedby(int c, int lev) {
  for(int i=0; i<lev; i++)
    c = ((c & 0xFEFEFE) >> 1);
  return c;
  }

bool fading = false;

ld fadeout = 1;

EX color_t darkened(color_t c) {
  if(inmirrorcount&1)
    c = gradient(c, winf[waMirror].color, 0, 0.5, 1);
  else if(inmirrorcount)
    c = gradient(c, winf[waCloud].color, 0, 0.5, 1);
  if(fading) c = gradient(backcolor, c, 0, fadeout, 1);
  if(vid.desaturate) {
    ld luminance = 0.2125 * part(c,2) + 0.7154 * part(c,1) + 0.0721 * part(c, 0);
    c = gradient(c, int(luminance+.5) * 0x10101, 0, vid.desaturate, 100);
    }
  for(int i=0; i<darken; i++)
    c = ((c & 0xFEFEFE) >> 1) + ((backcolor & 0xFEFEFE) >> 1);
  return c;
  }

/* gradient interpolation */

EX color_t gradient(color_t c0, color_t c1, ld v0, ld v, ld v1) {
  int vv = int(256 * ((v-v0) / (v1-v0)));
  color_t c = 0;
  for(int a=0; a<4; a++) {
    int p0 = part(c0, a);
    int p1 = part(c1, a);
    part(c, a) = (p0*(256-vv) + p1*vv + 127) >> 8;
    }
  return c;
  }

EX color_t rcolor() {
  color_t res;
  part(res, 0) = hrand(0x80);
  part(res, 1) = hrand(256);
  part(res, 2) = hrand(0x80) + 128;
  swap(part(res, 1), part(res, rand() % 2));
  swap(part(res, 2), part(res, rand() % 3));
  return res;
  }

EX color_t rainbow_color(ld sat, ld hue) {
  hue = frac(hue);
  
  if(hue < 0) hue++;
  
  hue *= 6;
  
  color_t res = 0;
  
  if(hue<1) res = gradient(0xFF0000, 0xFFFF00, 0, hue, 1);
  else if(hue<2) res = gradient(0x00FF00, 0xFFFF00, 2, hue, 1);
  else if(hue<3) res = gradient(0x00FF00, 0x00FFFF, 2, hue, 3);
  else if(hue<4) res = gradient(0x0000FF, 0x00FFFF, 4, hue, 3);
  else if(hue<5) res = gradient(0x0000FF, 0xFF00FF, 4, hue, 5);
  else if(hue<=6) res = gradient(0xFF0000, 0xFF00FF, 6, hue, 5);
  
  return gradient(0xFFFFFF, res, 0, sat, 1);
  }

/** Adjust col to SDL_gfx functions. No adjustment is needed in SDL 1.2, but it is needed in SDL2 */
EX color_t align(color_t col) {
  #if CAP_SDL2
  swap(part(col, 0), part(col, 3));
  swap(part(col, 1), part(col, 2));
  #endif
  return col;
  }

#if HDR
enum class eNeon { none, neon, no_boundary, neon2, illustration};
#endif

EX eNeon neon_mode;
EX bool neon_nofill;

EX void apply_neon(color_t& col, int& r) {
  switch(neon_mode) {
    case eNeon::none: 
    case eNeon::illustration:
      break;
    case eNeon::neon: 
      poly_outline = col << 8; col = 0; 
      break;      
    case eNeon::no_boundary: 
      r = 0;
      break;
    case eNeon::neon2:
      poly_outline = col << 8; col &= 0xFEFEFE; col >>= 1; 
      break;
    }
  }

/** used when neon_mode is eNeon::illustration */
EX color_t magentize(color_t x) {
  if(neon_mode != eNeon::illustration) return x;
  int green = part(x,2);
  int magenta = (part(x, 1) + part(x, 3)) / 2;
  int nm = max(magenta, green);
  int gm = (magenta + green)/2;
  nm = (nm + 255) / 2;
  gm = gm / 2;
  
  return (nm * 0x1000100) | (gm * 0x10000) | (part(x, 0));
  }

EX color_t monochromatize(color_t x) {
  int c = part(x,2) + part(x,1) + part(x, 3);
  c ++;
  c /= 3;
  return c * 0x1010100 | (part(x, 0));
  }

/** colorblind mode */
EX bool cblind;

/** apply neon_mode and cblind */
EX void apply_neon_color(color_t col, color_t& pcolor, color_t& poutline, flagtype flags) {
  if(cblind) {
    // protanopia
    /* int r = (56 * part(col,3) + 43 * part(col,2)) / 100;
    int g = (58 * part(col,3) + 42 * part(col,2)) / 100;
    int b = (24 * part(col,2) + 75 * part(col,1)) / 100; */
    // deuteranopia
    /* int r = (625 * part(col,3) + 375 * part(col,2)) / 1000;
    int g = (700 * part(col,3) + 300 * part(col,2)) / 1000;
    int b = (300 * part(col,2) + 700 * part(col,1)) / 1000; 
    part(col,3) = r;
    part(col,2) = g;
    part(col,1) = b; */
    part(col,2) = part(col,3) = (part(col,2) * 2 + part(col,3) + 1)/3;
    }
  if(neon_mode == eNeon::none) {
    pcolor = (darkened(col >> 8) << 8) + (col & 0xFF);
    poutline = poly_outline;
    if(flags & POLY_TRIANGLES) poutline = 0;
    }
  else switch(neon_mode) {
    case eNeon::neon:
      pcolor = (poly_outline & 0xFFFFFF00) | (col & 0xFF);
      poutline = (darkened(col >> 8) << 8) | (col & 0xFF);
      if(col == 0xFF) poutline = 0xFFFFFFFF;
      if(neon_nofill && pcolor == 0xFF) pcolor = 0; 
      break;
    case eNeon::no_boundary:
      pcolor = (darkened(col >> 8) << 8) + (col & 0xFF);
      poutline = 0;
      break;
    case eNeon::neon2:
      pcolor = (darkened(col >> 8) << 8) + (col & 0xFF) + ((col & 0xFF) >> 2);
      poutline = (darkened(col >> 8) << 8) + (col & 0xFF);
      if(col == 0xFF) poutline = 0xFFFFFFFF;
      if(poly_outline != 0xFF) poutline = poly_outline;
      if(neon_nofill && pcolor == 0xFF) pcolor = 0; 
      break;
    case eNeon::illustration: {
      if(poly_outline && (poly_outline>>8) != bordcolor) {
        pcolor = magentize(col);
        poutline = 0xFF;
        }
      else {
        poutline = poly_outline;
        pcolor = monochromatize(col);
        }
      if(pcolor & 0xFF) pcolor |= 0xFF;
      if(poutline & 0xFF) poutline |= 0xFF;
      break;
      }
    case eNeon::none: ;
    }
  }

/* color of various stuff */

/** cloak color for the given table radius */
EX int cloakcolor(int rtr) {
  rtr -= 28;
  rtr /= 2;
  rtr %= 10;
  if(rtr < 0) rtr += 10;
  // rtr = time(NULL) % 10;
  int cc[10] = {
    0x8080FF, 0x80FFFF, 0x80FF80, 0xFF8080, 0xFF80FF, 0xFFFF80, 
    0xFFFFC0, 0xFFD500, 0x421C52, 0
    };
  return cc[rtr];
  }

EX int firegradient(double p) {
  return gradient(0xFFFF00, 0xFF0000, 0, p, 1);
  }

constexpr ld PI1000 = 0.001 / A_PI;

EX int firecolor(int phase IS(0), int mul IS(1)) {
  return gradient(0xFFFF00, 0xFF0000, -1, sintick(100*mul, phase * 5 * PI1000), 1);
  }

EX int watercolor(int phase) {
  return 0x0080C0FF + 256 * int(63 * sintick(50, phase * 10 * PI1000));
  }

EX int aircolor(int phase) {
  return 0x8080FF00 | int(32 + 32 * sintick(200, phase * 1. / cgi.S21));
  }

EX int fghostcolor(cell *c) {
  int phase = int(fractick(650, (int)(size_t)c) * 4000);
  if(phase < 1000)      return gradient(0xFFFF80, 0xA0C0FF,    0, phase, 1000);
  else if(phase < 2000) return gradient(0xA0C0FF, 0xFF80FF, 1000, phase, 2000);
  else if(phase < 3000) return gradient(0xFF80FF, 0xFF8080, 2000, phase, 3000);
  else if(phase < 4000) return gradient(0xFF8080, 0xFFFF80, 3000, phase, 4000);
  return 0xFFD500;
  }

EX int weakfirecolor(int phase) {
  return gradient(0xFF8000, 0xFF0000, -1, sintick(500, phase * PI1000), 1);
  }

/* HTML color names */

EX vector<pair<const char*, color_t>> color_names = {
{"aliceblue", 0xf0f8ff}, {"antiquewhite", 0xfaebd7}, {"aqua", 0x00ffff}, {"aquamarine", 0x7fffd4}, {"azure", 0xf0ffff},
{"beige", 0xf5f5dc}, {"bisque", 0xffe4c4}, {"black", 0x000000}, {"blanchedalmond", 0xffebcd}, {"blue", 0x0000ff}, {"blueviolet", 0x8a2be2}, {"brown", 0xa52a2a}, {"burlywood", 0xdeb887},
{"cadetblue", 0x5f9ea0}, {"chartreuse", 0x7fff00}, {"chocolate", 0xd2691e}, {"coral", 0xff7f50}, {"cornflowerblue", 0x6495ed}, {"cornsilk", 0xfff8dc}, {"crimson", 0xdc143c}, {"cyan", 0x00ffff},
{"darkblue", 0x00008b}, {"darkcyan", 0x008b8b}, {"darkgoldenrod", 0xb8860b}, {"darkgray", 0xa9a9a9}, {"darkgreen", 0x006400}, {"darkkhaki", 0xbdb76b}, {"darkmagenta", 0x8b008b}, {"darkolivegreen", 0x556b2f},
{"darkorange", 0xff8c00}, {"darkorchid", 0x9932cc}, {"darkred", 0x8b0000}, {"darksalmon", 0xe9967a}, {"darkseagreen", 0x8fbc8f}, {"darkslateblue", 0x483d8b}, {"darkslategray", 0x2f4f4f}, {"darkturquoise", 0x00ced1},
{"darkviolet", 0x9400d3}, {"deeppink", 0xff1493}, {"deepskyblue", 0x00bfff}, {"dimgray", 0x696969}, {"dodgerblue", 0x1e90ff},
{"firebrick", 0xb22222}, {"floralwhite", 0xfffaf0}, {"forestgreen", 0x228b22}, {"fuchsia", 0xff00ff},
{"gainsboro", 0xdcdcdc}, {"ghostwhite", 0xf8f8ff}, {"gold", 0xffd700}, {"goldenrod", 0xdaa520}, {"gray", 0x808080}, {"green", 0x008000}, {"greenyellow", 0xadff2f},
{"honeydew", 0xf0fff0}, {"hotpink", 0xff69b4},
{"indianred ", 0xcd5c5c}, {"indigo", 0x4b0082}, {"ivory", 0xfffff0}, {"khaki", 0xf0e68c},
{"lavender", 0xe6e6fa}, {"lavenderblush", 0xfff0f5}, {"lawngreen", 0x7cfc00}, {"lemonchiffon", 0xfffacd}, {"lightblue", 0xadd8e6}, {"lightcoral", 0xf08080}, {"lightcyan", 0xe0ffff}, {"lightgoldenrodyellow", 0xfafad2},
{"lightgrey", 0xd3d3d3}, {"lightgreen", 0x90ee90}, {"lightpink", 0xffb6c1}, {"lightsalmon", 0xffa07a}, {"lightseagreen", 0x20b2aa}, {"lightskyblue", 0x87cefa}, {"lightslategray", 0x778899}, {"lightsteelblue", 0xb0c4de},
{"lightyellow", 0xffffe0}, {"lime", 0x00ff00}, {"limegreen", 0x32cd32}, {"linen", 0xfaf0e6},
{"magenta", 0xff00ff}, {"maroon", 0x800000}, {"mediumaquamarine", 0x66cdaa}, {"mediumblue", 0x0000cd}, {"mediumorchid", 0xba55d3}, {"mediumpurple", 0x9370d8}, {"mediumseagreen", 0x3cb371}, {"mediumslateblue", 0x7b68ee},
{"mediumspringgreen", 0x00fa9a}, {"mediumturquoise", 0x48d1cc}, {"mediumvioletred", 0xc71585}, {"midnightblue", 0x191970}, {"mintcream", 0xf5fffa}, {"mistyrose", 0xffe4e1}, {"moccasin", 0xffe4b5},
{"navajowhite", 0xffdead}, {"navy", 0x000080},
{"oldlace", 0xfdf5e6}, {"olive", 0x808000}, {"olivedrab", 0x6b8e23}, {"orange", 0xffa500}, {"orangered", 0xff4500}, {"orchid", 0xda70d6},
{"palegoldenrod", 0xeee8aa}, {"palegreen", 0x98fb98}, {"paleturquoise", 0xafeeee}, {"palevioletred", 0xd87093}, {"papayawhip", 0xffefd5}, {"peachpuff", 0xffdab9}, {"peru", 0xcd853f}, {"pink", 0xffc0cb}, {"plum", 0xdda0dd}, {"powderblue", 0xb0e0e6}, {"purple", 0x800080},
{"rebeccapurple", 0x663399}, {"red", 0xff0000}, {"rosybrown", 0xbc8f8f}, {"royalblue", 0x4169e1},
{"saddlebrown", 0x8b4513}, {"salmon", 0xfa8072}, {"sandybrown", 0xf4a460}, {"seagreen", 0x2e8b57}, {"seashell", 0xfff5ee}, {"sienna", 0xa0522d}, {"silver", 0xc0c0c0}, {"skyblue", 0x87ceeb}, {"slateblue", 0x6a5acd}, {"slategray", 0x708090}, {"snow", 0xfffafa}, {"springgreen", 0x00ff7f}, {"steelblue", 0x4682b4},
{"tan", 0xd2b48c}, {"teal", 0x008080}, {"thistle", 0xd8bfd8}, {"tomato", 0xff6347}, {"turquoise", 0x40e0d0},
{"violet", 0xee82ee},
{"wheat", 0xf5deb3}, {"white", 0xffffff}, {"whitesmoke", 0xf5f5f5},
{"yellow", 0xffff00}, {"yellowgreen", 0x9acd32}
};

EX pair<const char*, color_t>* find_color_by_name(const string& s) {
  for(auto& c: color_names) if(c.first == s) return &c;
  return nullptr;
  }

}
