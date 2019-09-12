Welcome to the HyperRogue documentation! {#mainpage}
========================================

Documentation is in progress. Most functions are not explained yet.

Guided Tour
-----------

HyperRogue started as a very simple project: one geometry, one map structure, one
projection, one mechanics, one way to play the game (OK, there were two monster
types). Today, HyperRogue has over 60 lands with unique mechanics, about 40
geometries (including 2D, 3D, and non-isotropic ones) displayed in over 20
projections, many modes which completely change the gameplay, and even non-game
modes for mathematical or data visualization. As a result, it is quite easy to 
get lost -- not only because it is a very big project, but also because some of 
the early design decisions which were more appropriate for small projects, or
may make some things somewhat illogical now.

Where should one start when reading the source code, then?

The main files which hold everything together are:

* sysconfig.h -- loads libraries, sets various `CAP_*` (capability) compiler flags.
  These can be used to enable or disable parts of HyperRogue, due to missing
  libraries/hardware capabilities, or to save on compilation time / executable size.
  (Not all combinations work.)

* classes.h -- defines enums related to content (lands, walls, monsters, items,
  geometries, projections).

* hyper.h -- this defines the basic general templates used everywhere in the game,
  as well as macro definitions which change depending on game mode or geometry
  (such as `S7` which is the number of edges in the pure geometry, `R10` which is
  the number of treasure to collect to advance, or `sphere` which tells us whether
  we are playing in spherical geometry), and some chaotic stuff which has not been
  moved elsewhere.

* autohdr.h -- this file is generated automatically by `makeh`. It contains the
  headers for all the functions and structs defined anywhere in the game (EX and
  `#if HDR` tell makeh to put a header for the given entity in autohdr.h), so usually 
  it is better to read those other files directly.

Interesting CPP files, arranged from the most basic ones:

* hyperpoint.cpp -- computations which make non-Euclidean geometry work. An uniform
  system is used for continuous hyperbolic, Euclidean, and spherical geometries,
  in 2D and 3D (for nonisotropic geometries see nonisotropic.cpp). The most important
  types are hr::hyperpoint (a vector in homogeneous coordinates) and hr::transmatrix
  (a matrix).

* locations.cpp -- this defines the abstract classes used for representing maps. 
  Important types are hr::cell (actual cells used by the game), hr::heptagon
  (coarser structure used in the default and some other geometries, this corresponds
  to heptagons in the standard geometry, but may have nothing to do with heptagons
  in general), and hr::walker (which handle navigation on the map).

* heptagon.cpp -- this builds the structure of heptagons (see [this guide](http://roguetemple.com/z/hyper/dev.php))
  in the standard geometries on the hyperbolic planes. Other geometries are built
  in different ways, so they do not use this much. This graph is built in an abstract
  way, i.e., not using the computations from hyperpoint.cpp.

* cell.cpp -- in the standard map, this builds the actual game graph, by bitruncating
  the graph of heptagons (pure geometries are also handled here, but Goldberg and
  irregular variations are defined in goldberg.cpp and irregular.cpp).
  It also defines the basic class hr::hrmap (used in all kinds of geometries),
  hr::hrmap_hyperbolic (used in 2D hyperbolic geometries), and many functions dealing
  with the game graph, such as computing the distances.

* classes.cpp and content.cpp -- content.cpp is actual game content (lands, walls,
  items, monsters), so if you want to add more, start here. classes.cpp defines
  the available geometries and projections, and some extra definitions for game content.

* geometry.cpp -- this specifies how the discrete tessellation from heptagon.cpp and 
  cell.cpp connects to the underlying continuous geometry (hyperpoint.cpp). More advanced
  routines are in geometry2.cpp.

* Low level graphic routines can be found between hypgraph.cpp (implementation of various projections
  -- the major projections use GLSL shaders though), shaders.cpp (GLSL shaders and other
  low-level OpenGL routines), basegraph.cpp (various basic stuff), drawing.cpp (queue of
  objects to draw, special handling for spherical geometries), control.cpp (low-level control).

* graph.cpp deals mostly with drawing high-level objects, such as tiles, monsters, and items.
  The shapes themselves are programmed in polygons.cpp, floor shapes are adjusted to various
  geometries in floorshapes.cpp.

* game.cpp implements most of the game mechanics. Some mechanics which are more unique to
  specific lands are implemented in complex.cpp and complex2.cpp.

* landgen.cpp is basic (local) land generation, while barriers (i.e., Great Walls) are handled
  in barriers.cpp, and equidistant surfaces, horocycles etc. are handled in bigstuff.cpp.

* boring utilities include util.cpp (other basic maths and parsing expressions), hprint.cpp
  (dealing with files and streams), dialogs.cpp (dialog screens), hyper.cpp and init.cpp
  (initialization), system.cpp (starting new games, changing modes etc.).

How to use the HyperRogue engine for making visualizations
----------------------------------------------------------

The HyperRogue engine includes various "hooks" which can be used to extend the game.
For example, a hook can be redefined to add extra commandline parameters, generate
new kinds of hr::hrmap, replace the map-drawing or cell-drawing routines, and so on.
This is used thorough the source code to organize things such as commandline parameters,
and also can be used to experiment with adding new things. Write a cpp file which 
includes the file init.cpp (to load all the headers), redefines some hooks (for the
extra functionality), and compile it with the compiler flag `-DCU=1` (so just the headers
are included) and link it to `hyper.o`. Rogueviz works this way.

How to add game content: lands, items, monsters, walls
------------------------------------------------------

The hook system currently does not allow adding new content easily, but you can simply add the new content to the end of the file content.cpp. You also need to do the following:

For Lands:

* Add new entries to hyperrogue-music.txt, these are indexed by integer IDs.
* Create the new monsters (as described below), and add entries to isNative in landlock.cpp.
* Implement the land generation in void giantLandSwitch(cell *c, int d, cell *from) (hint: this function is called for each cell whenever the minimum distance from the player (d) decreases; d=7 are visible and thus they are fully generated)
* You probably want to add some wandering monsters at high treasure counts -- see wandering() in monstergen.cpp
* Add new entries to graph.cpp to draw the new land correctly
* To make the land reachable, you should do the following things: (land unlocking is not factored very well)
* * add entries to getNewLand in landlock.cpp, to make the land actually reachable
* * add entries to land_over in landlock.cpp, which specifies the order in the Land Overview
* * add rules to land_validity if there are any restrictions (note: the default is that there are no restrictions -- the land could appear in Chaos Yendor Challenge) 
* You might want to add special cases in functions: earthFloor(), earthWall(), summonedAt(), or add a new Yendor Challenge
* For the Steam version, achievements and leaderboards should be added 

For Monsters:

* If it is an enemy monster, you probably want to add an entry to int* killtable[] in games.cpp to make the kills count in total kills
* Add a case to drawMonsterType in graph.cpp -- at first you can just change the colors of an existing monster, adding new graphics is described later
* To save the kill count correctly, add an applyBoxM line to system.cpp, and increase POSSCORE by 1. This has to be added last, to keep compatibility. 

Adding Items

* To save the treasure count correctly, add a line applyBoxI(eItem) in system.cpp, and increase POSSCORE by 1
* To save the Orbs correctly, add lines applyBoxOrb(eItem) and list_invorb(); in system.cpp, and increase POSSCORE by 2 (this saves both the current number of charges, and the OSM inventory) This has to be added last, to keep compatibility
* For Orbs, you have to add entries to reduceOrbPowers() (maximum charges) and orbcharges() (charges per Orb) in orbs.cpp
* Treasure pictures are based on the ASCII glyphs by default -- if you want something special, add a case to drawItemType in graph.cpp 

Adding Wall Types (i.e., any static objects)

* You need to define how the new wall type reacts to many different effects: boatGoesThrough(), passable(), prespill(), makeflame(), explodeAround(), flashCell(), castLightningBolt(), explodeMine(), summonedAt(), beastcrash() 

How to add new graphics
-----------------------

* Draw a picture in the Vector Graphics Editor, and press Shift+S to get the code
* Paste the code into polydata[] in polygons.cpp, but replace ID by NEWSHAPE, (number identifier)
* Add a new shape member (sh*) in hr::geometry_information
* For floor shapes, add a new member of type escher_floorshape in hr::geometry_information, and init_escher() call in init_floorshapes()
  (the parameters are the number identifiers from polydata)
* For non-floor shapes, link the new shape global variable to the polydata[] entry in buildpolys() (by calling function bshape)
* Add the code using the new shape variable to graph.cpp 

How to add configuration
------------------------

* Add a new variable in struct videopar in hyper.h, or some other place if you prefer
* Provide a default value and config file line in void initConfig() in config.cpp
* Add a menu entry in the appropriate show* function (hint: old code implements the reaction to selecting the menu option as a case in keyhandler -- the new code just calls the add_action function just after showing the menu option, which is more convenient)

Adding new text (including names and descriptions of lands/items/walls/monsters)
--------------------------------------------------------------------------------

* Remember to call XLAT and similar functions instead of writing the text directly, so that it is translated
* It is good to list all the added texts (i.e., possible parameters to XLAT) somewhere, so it can be translated 

How to add new projections
--------------------------

This is simple (well, in the easy cases) -- add new entry to classes.h (enum eModel), classes.cpp (hr::mdinf), and implement your projection in applymodel() in hypgraph.cpp.

How to add new geometries (i.e., map structures)
------------------------------------------

Well, this is ambitious.

* Add new entry to classes.h (enum eGeometry), classes.cpp (hr::ginf) marking your geometry as "EXPERIMENTAL".
* Implement a new subclass of hr::hrmap. hr::hrmap_binary in binary-tiling.cpp is a good guide.
* If you use the standard bitruncation, or cells correspond 1-1 to heptagons, you do not have to do anything (well, you may need to automatically disable bitruncation in 
  \link hr::set_geometry() \endlink).
  Otherwise modifying \link hr::createMov() \endlink may be necessary.
* Modify initcells() (or define a hook) to actually create a map in your subclass.
* For 3D geometries you need to tell HyperRogue the shape of the faces. See geometry_information::create_wall3d() in polygons.cpp.
* For 2D geometries you need to tell HyperRogue how to draw the cells (see floorshapes.cpp) and various geometry details in geometry2.cpp.
* Add an entry in geom-exp.cpp to make your geometry available (otherwise it can be activated with the command line option `-geo NameOfMyGeometry`).
* Once the basic geometry works, you can disable the experimental status of your geometry, and try making things in cell.cpp (such as cell distances) and bigstuff.cpp
  (large scale land generation) work.
