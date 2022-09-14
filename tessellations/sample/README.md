# Tes file format

The tes file format is used to describe tessellations that can be loaded with HyperRogue/RogueViz.
The format is designed to be written and read by humans, so all the numerical values can be provided
as formulas.

To load a tes file in HyperRogue, do: special modes -> experiment with geometry -> geometry -> hyperbolic* ->
load from file. You can also do it with command line, e.g. `./hyperrogue -tes tessellations/sample/marjorie-rice.tes`.

* You can also pick Euclidean or spherical here -- does not matter, any file can be loaded.

## Structure

A typical tes file consists of the following parts:

* Name of the tessellation (as displayed in HyperRogue). This is given in a line starting with `##`.

* Geometry. Usually `h2.` (hyperbolic), `e2.` (Euclidean), `s2.` (spherical).

* A definition of distance and angle units. For example `angleunit(deg)` defines the angle unit to be 1 degree,
  and `distunit(arcmedge(3,3,4,4,4))` defines the distance unit to be x such that the total internal angle of
  two triangles and three quadrangles of edge x equals 360 degrees. The default angle unit is radian and the
  default distance unit is 1 (absolute unit).

* A definition of tiles. Use tile(e0, a0, e1, a1, e2, a2) to construct a tile with edge lengths e0, e1, e2 and angles
  a0, a1, a2, ... Edges are given clockwise and angles are internal. You can also use unittile(a0, a1, a2) if all
  edge lengths are 1. Tiles are indexed starting from 0.

* A definition of connections. Most tes files use the Doily notation, e.g. conway("(0 0)(1 2)"), which means that 
  the edge 0 of tile 0 always connects to itself, and edge 1 of tile 0 always connect to edge 2 of tile 0. You can
  use `i`, `i'`, `i''`, `i'''`, `i@n` to refer to edge i of tile 0, 1, 2, 3, n. Use [ ] instead of ( ) if the
  connection is to be mirrored. You can also use c(i1,n1,i2,n2,0) (the last value becomes 1 for mirrored connections).

## Symmetries

Some tessellations include tiles with rotational symmetry. For example, in the standard HyperRogue tiling 
(bitruncated {7,3}), the heptagonal tiles have 7-fold rotational symmetry, and the hexagonal tiles have 3-fold
rotational symmetry. In this case, it is convenient to not give an index to every edge of every tile, but only
to the part which repeats (i.e., heptagons have 7 edges of index 0, and hexagons have 3 edges of index 0 and
3 edges of index 1).

Such situation can be defined with e.g. `tile(e0, a0, e1, a1, *3)` (this defines a hexagon with edges e0,e1,e0,e1,e0,e1
and angles a0,a1,a0,a1,a0,a1; edges are indexed with 0,1,0,1,0,1).

You can also use `repeat(tile_index, qty)` to make every edge index of tile tile_index repeat qty times, for example,
`tile(e0, a0, e1, a1, e2, a2) repeat(0, 3)` has the same effect as tile(e0, a0, e1, a1, *3).

Some older tessellation definitions do not use this, for example `73.tes` simply lists all connections as `c(i,0,i,0,0)`.
This indexing will not be consistent: there is no way to index edges of every tile clockwise so that tile indexed i
always connects to a tile indexed i. (The engine ignores this; but for new tessellations it is recommended to use 
`*k` or `repeat` to explain the situation explicitly.)

## Formulas

As mentioned above, all the numerical values can be provided as formulas. Formulas can use complex numbers.

You can use the constants: `pi`, `deg` (pi/180), `e`, `i`, `inf` (infinity). You can also declare new constants as in `let(tau=2*pi)`.

You can use the standard math operators `+`, `-`, `*` (multiplication), `/` (division), `^` (exponentation), and math functions `sin`, `cos`, `tan`, `sinh`, `cosh`, `tanh`, `asin`, `acos`, `atan`, `asinh`, `acosh`, `atanh`, `exp`, `sqrt`, `log` (natural logarithm), `tan`,
`re` (real part), `im` (imaginary part), `conj` (conjugation), `floor`, `frac` (fractional part), `min`, `max`.

Programming-style functions: `ifz(x,a,b)` (a if x=0, b otherwise), `ifp(x,a,b)` (a if x is positive, b otherwise), `test(x)` (returns x but also displays x while reading the file), 
`let(x=a,b)` (compute the formula a and use its value as `x` in b).

Specialized functions are very useful for defining tilings.

* `edge_angles(x,y,z)` is the edge opposite x of the triangle with angles x, y, z (angles given in angleunits). Works only in non-Euclidean geometry. Length returned in absolute units.

* `edge(a,b)` is the edge of the {a,b} tiling. Works only if the {a,b} tiling is possible in the current non-Euclidean geometry.  Length returned in absolute units.

* `regradius(a,b)` is the radius of the circumscribed circle on {a,b} tile. Works only if the {a,b} tiling is possible in the current non-Euclidean geometry. Length returned in absolute units.

* `arcmedge(3:^2,4:^3)` is the edge x such that the total internal angle of two triangles and three quadrangles of edge x equals 360 degrees. Works only if the tiling is possible in the current geometry.
Returns 1 in Euclidean geometry. Length returned in absolute units.

* `regangle(d,s)` is the internal angle of an s-sided regular tile with edge length d distunits. Angle returned in angle units.

Note: while it may seem that a and b in `edge`, `regradius`, and `arcmedge(a:^b)` are integers, the formulas actually also make sense for real numbers, which is sometimes useful (e.g. for star tilings).
Also a is allowed to be infinity (inf).

## Special features

The tes format also has some more features. Please read the tes files in `samples` subdirectory to see these in action.

* Real-valued sliders. These represent real values than can be changed in the engine. For example, you could have a real-valued slider x, that defines a Euclidean tiling with parallelograms
of angles x and 180°-x. Changing the value of a real-valued slider does not change the structure of the tiling (so it can be called without regenerating the map). See e.g. `slider.tes`.

* Integer-valued sliders. These represent integer values than can be changed in the engine. For example, a tes file can describe all {a,b} regular tessellations, where both a and b can
be changed using sliders. Changing the value of an integer-valued slider usually does change the structure of the tiling (so the map needs to be regenerated). See e.g. `regular.tes`.

* Affine tessellations. These are tessellations of the affine two-dimensional plane; their symmetries are not Euclidean isometries but rather affine transformations of R^2. See the examples in
`affine` subdirectory.

* Tiles with (ultra-)ideal angles. To achieve these, put `e1, [a1, e2, a2], e3` inside the `tile` definition; this will generate the tile as `e1, a1, e2, a2, e3` would, and then the edges `e1` and `e3`
are extended until they cross (usually, in an (ultra-)ideal point), and the edge `e2` is removed. Two convenient functions `ideal_angle` and `ideal_edge` are helpful. See e.g. `inf.tes`. 

* Apeirogonal tiles. These are achieved by giving the tile multiplier as `*inf` (e.g., `tile(e0, a0, *inf)`). See e.g. `pseudogons.tes`.

* Star polygons. Just add a line "star." so that the engine is not confused. See e.g. `star-72-7.tes`. Star polyhedra are best viewed in 3D (in settings -> 3D configuration, 
enable "configure FPP automatically", and also "make the tiles flat"; then leave the menu and move the camera using Home/End and arrow keys).

* Tessellations including a definition of a regular tree used to construct the tessellation. This allows non-periodic tessellations (in the style of binary tiling), see `horotiling.tes`. 

* If the tessellation is invalid, HyperRogue will invoke a debugger, which lets you see the tiles you have defined and move according to their connections. You can also invoke this debugger
  maually with `debug(tile_index)`.

# How to make the tessellations look good

Normally, when you choose to load a tes file, a game of HyperRogue is run on the tiling selected. Which might be not what you wanted. Here are some hints for making nice pictures.

* If you just wanted to draw the tessellation, do 'experiment with geometry' -> 'pattern'. Most useful patterns are: single color, random, types, sides.

* By default, HyperRogue tiles have wide borders, which might be not what you want. Also in the 'pattern' menu, you can enable 'display full floors' to prevent this.

* HyperRogue approximates the edges using polylines. If the current approximation is not sufficient, change main menu -> settings -> general graphics -> vector settings -> line quality to a higher value.
  From the same menu you can also change the line width.

* For tessellations including apeirogons you probably want the setting "simplified display of apeirogons" in 'experiment with geometry'. (This way you avoid the semicircular artifacts in the parts of the
  apeirogons which are close to the boundary.)

* The screenshot menu 'experiment with geometry' -> 'take screenshot' has many useful options, e.g., to hide the player, change the pattern colors (for tile/type patterns), center the screenshot on a tile
  center/vertex/edge, increase the range of tiles rendered, etc.

* To post screenshots of hyperbolic tessellations online, we recommend the following screenshot settings. In the 'projection' menu (experiment with geometry -> projection) change the scale factor to something
  slightly less than 1 (otherwise the Poincaré disk fills the whole image which does not look great). Make the screenshot transparent (screenshot menu). Change the projection background (screenshot ->
  colors & aura) to solid black (by setting the alpha to max value). This will make the background of the Poincaré disk black, while the rest of the image will be transparent, so it will use the background
  color of the website you post it on.

* To print the screenshots of tessellations (e.g. to use them in math papers), it is recommended to use a vector-based format rather than PNG. Change the screenshot format (in screenshot menu) to SVG.
  SVG files can be converted to PDF using e.g. Inkscape. Unfortunately SVG/PDF files of hyperbolic tessellations tend to be large (lots of details), so you might want to control the quality (using sight
  range settings). Options such as line width, transparency, etc. do affect SVG screenshots.

* You can also try menu -> creative mode -> map editor -> map settings -> canvas floor shape. This applies some HyperRogue floor shape tessellations to the current tiling. (You need to select the 'pattern'
  first as explained above).
  
* Most options can be also set from the commandline. For example `hyperrogue -tes tessellations/sample/hr-standard-tiling.tes -canvas B -wsh 9 -shot-1000 -shott 1 -fillmodel 000000ff -zoom .95 -smart 1 -noplayer -vlq 3 -lw 5`
  will load the named tessellation file, color the tiles with "sides" pattern (B), use full floors, make screenshots of size 1000x1000, make screenshot transparent with the Poincaré disk black, set scaling to
  0.95, and draw tiles if their size is at least 1 pixel, hide the player character, line quality to 3, and line width to 5. Add `-genlimit 999999 -pngshot x.png` or `-svgshot x.svg` to take a screenshot
  (genlimit here makes sure that tiles are generated, the default is at most 250 tiles generated per call). -rulegen-play -palrgba gtree ffffffff -palw gtree 3 -expansion 1 2 7` to generate and display a tree (explained below).

# How is this implemented

Every tile on screen is represented in the memory. Tiles are generated as needed.

By default, Archimedean/tes files are generated using a simple method: when we check for a tile across an edge, we see whether a tile already exists at that coordinates -- if yes we connect to that tile,
if no we generate a new tile. However, coordinate-based computations in hyperbolic geometry are prone to numerical precision errors, so occassionally this may result in errors.

HyperRogue also implements the algorithm described [here](https://arxiv.org/abs/2111.12040) to create tree structures which avoid this problem. This is not used by default because occassionally (very rarely)
generating a tree structure can take some time (also it currently does not support affine, star, or tessellations with (ultra-)ideal-vertices). To run the algorithm, in 'experiment with geometry' pick 'size of the world' -> 'strict tree maps' -> 'strict tree based'. In the 'size of the world' menu
you can also get various information regarding the tree structure obtained.
