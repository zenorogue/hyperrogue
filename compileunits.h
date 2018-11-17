// Usage:
// * compile hyper.cpp with CU == 0
// * compile init.cpp with CU == 1
// * link them.
// Only the parts defined in #if IN_CU(1) will be included in the second compiling.


#ifndef CU
#define IN_CU(x) 1
#else
#define IN_CU(x) (CU == x)
#endif

#include "sysconfig.h"
#include "classes.h"
#include "hyper.h"

#define CU_INIT IN_CU(0)
#define CU_HYPER IN_CU(0)

#if IN_CU(0)
#include "classes.cpp"
#endif

#if IN_CU(0)
#include "shaders.cpp"
#include "util.cpp"
#include "hyperpoint.cpp"
#include "patterns.cpp"
#include "fieldpattern.cpp"
#include "heptagon.cpp"
#include "binary-tiling.cpp"
#include "archimedean.cpp"
#include "crystal.cpp"
#include "language.cpp"
#include "cell.cpp"
#include "expansion.cpp"
#include "goldberg.cpp"
#include "irregular.cpp"
#include "pattern2.cpp"
#include "flags.cpp"
#include "yendor.cpp"
#include "complex.cpp"
#if CAP_COMPLEX2
#include "complex2.cpp"
#endif
#include "savemem.cpp"
#include "game.cpp"
#include "orbgen.cpp"
#include "monstergen.cpp"
#include "landlock.cpp"
#include "landgen.cpp"
#include "orbs.cpp"
#if CAP_INV
#include "inventory.cpp"
#else
namespace hr { namespace inv { bool on, activating; } }
#endif
#include "system.cpp"
#include "debug.cpp"
#include "geometry.cpp"
#include "geometry2.cpp"
#include "polygons.cpp"
#include "floorshapes.cpp"
#include "mapeditor.cpp"
#if CAP_MODEL
#include "netgen.cpp"
#endif
#if CAP_TABFONT || CAP_CREATEFONT
#include "nofont.cpp"
#endif
#include "basegraph.cpp"
#include "screenshot.cpp"
#include "renderbuffer.cpp"
#include "help.cpp"
#include "config.cpp"
#include "scores.cpp"
#include "dialogs.cpp"
#include "menus.cpp"
#include "geom-exp.cpp"
#include "quit.cpp"
#include "shmup.cpp"

#if CAP_RACING
#include "racing.cpp"
#endif

#include "conformal.cpp"
#include "rug.cpp"
#include "control.cpp"
#include "hud.cpp"
#include "hypgraph.cpp"
#include "textures.cpp"
#include "graph.cpp"
#include "blizzard.cpp"
#include "sound.cpp"
#include "achievement.cpp"
#include "barriers.cpp"
#include "surface.cpp"
#if CAP_TOUR
#include "tour.cpp"
#endif
#include "commandline.cpp"
#include "bigstuff.cpp"

#if CAP_ROGUEVIZ
#include "rogueviz.cpp"
#endif

#if CAP_DAILY
#include "private/daily.cpp"
#else
namespace hr { namespace daily { bool on; } }
#endif
#endif

#if IN_CU(1)
#endif

