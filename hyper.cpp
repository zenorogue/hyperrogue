// Hyperbolic Rogue
// Copyright (C) 2011-2019 Zeno Rogue

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

/** \file hyper.cpp
 *  \brief master file which includes everything
 *  You can either compile this file, or use mymake to compile all the *.cpp files separately.
 */

#include "hyper.h"

#include "classes.cpp"
#include "colors.cpp"
#include "glhr.cpp"
#include "shaders.cpp"
#include "raycaster.cpp"
#include "hprint.cpp"
#include "util.cpp"
#include "hyperpoint.cpp"
#include "patterns.cpp"
#include "fieldpattern.cpp"
#include "heptagon.cpp"
#include "binary-tiling.cpp"
#include "nonisotropic.cpp"
#include "asonov.cpp"
#include "kite.cpp"
#include "aperiodic-hat.cpp"
#include "archimedean.cpp"
#include "arbitrile.cpp"
#include "rulegen.cpp"
#include "rulegen3.cpp"
#include "euclid.cpp"
#include "sphere.cpp"
#include "fake.cpp"
#include "quotient.cpp"
#include "crystal.cpp"
#include "reg3.cpp"
#include "language.cpp"
#include "cell.cpp"
#include "locations.cpp"
#include "expansion.cpp"
#include "goldberg.cpp"
#include "irregular.cpp"
#include "pattern2.cpp"
#include "flags.cpp"
#include "yendor.cpp"
#include "complex.cpp"
#include "complex2.cpp"
#include "savemem.cpp"
#include "game.cpp"
#include "passable.cpp"
#include "checkmove.cpp"
#include "pcmove.cpp"
#include "environment.cpp"
#include "monstermove.cpp"
#include "mapeffects.cpp"
#include "attack.cpp"
#include "items.cpp"
#include "orbgen.cpp"
#include "monstergen.cpp"
#include "landlock.cpp"
#include "landgen.cpp"
#include "wfcgen.cpp"
#include "orbs.cpp"
#include "inventory.cpp"
#include "system.cpp"
#include "debug.cpp"
#include "geometry.cpp"
#include "embeddings.cpp"
#include "geometry2.cpp"
#include "polygons.cpp"
#include "3d-models.cpp"
#include "floorshapes.cpp"
#include "usershapes.cpp"
#include "drawing.cpp"
#include "mapeditor.cpp"
#include "netgen.cpp"
#include "nofont.cpp"
#include "basegraph.cpp"
#include "screenshot.cpp"
#include "renderbuffer.cpp"
#include "help.cpp"
#include "legacy.cpp"
#include "config.cpp"
#include "scores.cpp"
#include "dialogs.cpp"
#include "menus.cpp"
#include "geom-exp.cpp"
#include "quit.cpp"
#include "multi.cpp"
#include "shmup.cpp"
#include "racing.cpp"
#include "models.cpp"
#include "history.cpp"
#include "rug.cpp"
#include "control.cpp"
#include "hud.cpp"
#include "radar.cpp"
#include "hypgraph.cpp"
#include "textures.cpp"
#include "graph.cpp"
#include "celldrawer.cpp"
#include "sky.cpp"
#include "blizzard.cpp"
#include "sound.cpp"
#include "achievement.cpp"
#include "barriers.cpp"
#include "surface.cpp"
#include "tour.cpp"
#include "commandline.cpp"
#include "bigstuff.cpp"
#include "multigame.cpp"
#include "inforder.cpp"
#include "vr.cpp"
#include "intra.cpp"

#if CAP_ROGUEVIZ
#include "rogueviz/rogueviz-all.cpp"
#endif

#if CAP_DAILY
#include "private/daily.cpp"
#else
namespace hr { namespace daily { bool on; } }
#endif

#include "mobile.cpp"
#include "hyper-main.cpp"

