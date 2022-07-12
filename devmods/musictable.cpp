#include "../hyper.h"

// a tool to generate the music tables for Android and iOS

namespace hr {

string crop(string s) {
  string res;
  for(char c: s) if(c == '-') res = ""; else res += c;
  return res;
  }

string cropios(string s) {
  string res;
  for(char c: s) if(c == '.') return res; else res += c;
  return res;
  }

auto aec = 
  arg::add3("-music-ios", [] {
    for(int i=0; i<landtypes; i++)
      if(musfname[i] == "") printf("  NULL,\n");
      else printf("  @\"%s\", // %2d : %s\n", cropios(musfname[i]).c_str(), i, dnameof(eLand(i)).c_str());
    })
+ arg::add3("-music-android", [] {
    for(int i=0; i<landtypes; i++)
      if(musfname[i] != "") {
        string s = musfname[i].substr(4, isize(musfname[i])-8);
        printf("  if(curland == %2d) id = R.raw.%s; // %s\n", i, crop(s).c_str(), dnameof(eLand(i)).c_str());
        }
    })
+ arg::add3("-music-license", [] {
    printf("musiclicense = \"");
    for(int i=0; i<isize(musiclicense); i++)
      if(musiclicense[i] == '\n') printf("\\n\"\n\"");  else printf("%c", musiclicense[i]);
    printf("\";\n");
    });

}
