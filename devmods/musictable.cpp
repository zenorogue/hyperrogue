#include "../hyper.h"

// a tool to generate the music tables for Android and iOS

namespace hr {

auto aec = 
  arg::add3("-music-ios", [] {
    for(int i=0; i<landtypes; i++)
      if(musfname[i] == "") printf("  NULL,\n");
      else printf("  @\"%s\", // %2d : %s\n", musfname[i].c_str(), i, dnameof(eLand(i)).c_str());
    })
+ arg::add3("-music-android", [] {
    for(int i=0; i<landtypes; i++)
      if(musfname[i] != "") {
        string s = musfname[i].substr(4, isize(musfname[i])-8);
        printf("  if(curland == %2d) id = R.raw.%s; // %s\n", i, s.c_str(), dnameof(eLand(i)).c_str());
        }
    })
+ arg::add3("-music-license", [] {
    printf("musiclicense = \"");
    for(int i=0; i<isize(musiclicense); i++)
      if(musiclicense[i] == '\n') printf("\\n\"\n\"");  else printf("%c", musiclicense[i]);
    printf("\";\n");
    });

}
