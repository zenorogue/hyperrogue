# HyperRogue
<p align="right">
<a href="https://travis-ci.org/zenorogue/hyperrogue/builds">
<img align="right" src="https://badges.herokuapp.com/travis/zenorogue/hyperrogue?branch=master&env=TRAVIS_BUILD_SYSTEM=autotools&label=autotools" alt="TravisCI badge">
<img align="right" src="https://badges.herokuapp.com/travis/zenorogue/hyperrogue?branch=master&env=TRAVIS_BUILD_SYSTEM=Makefile&label=make" alt="TravisCI badge">
<img align="right" src="https://badges.herokuapp.com/travis/zenorogue/hyperrogue?branch=master&env=TRAVIS_BUILD_SYSTEM=emscripten&label=web" alt="TravisCI badge">
<br>
<img align="right" src="https://ci.appveyor.com/api/projects/status/github/zenorogue/hyperrogue?svg=true" alt="AppVeyor badge">
</a>
</p>
<p align="left">
<b>Current version: 11.0a</b>
</p>

A puzzle roguelike in the hyperbolic plane. See the [HyperRogue website](http://roguetemple.com/z/hyper.php) for detailed and most up-to-date information.

Released under [GNU General Public License, version 2](http://www.gnu.org/licenses/gpl-2.0.html). As such, it comes with without any warranty.

If you would like to thank me for HyperRogue, you can support the development by buying paid versions (with more up-to-date content and some social features), or [in other ways](http://www.roguetemple.com/z/donate.php?id=NotEye). 
You can contact me at zeno@attnam.com, or at [my blog](http://zenorogue.blogspot.com/), or via [Twitter](https://twitter.com/ZenoRogue).

## The game ##
You are a lone outsider in a strange, non-Euclidean world. You can move with the numpad, vi keys (hjklyubn), or mouse. You can also skip turns by pressing ".".

As a Rogue, your goal is to collect as many treasures as possible. However, collecting treasures attracts dangerous monsters (on the other hand, killing the monsters allows more treasures to be generated).

You can kill most monsters by moving into them. Similarly, if the monster was next to you at the end of your turn, it would kill you. The game protects you from getting yourself killed accidentally by ignoring moves which lead to instant death (similar to the check rule from Chess).

Ultimately, you will probably run into a situation where monsters surround you. That means that your adventure is over, and you will have to teleport back to the Euclidean world to survive by pressing Escape (quit).

Collecting enough treasure also allows you to find rare magical orbs, which grant you one-time or time-limited special abilities.

The world is a combination of thirteen types of lands. Each land type has specific style (monsters, treasure, magical orbs, terrain features). Collecting treasures only attracts more monsters in lands of the same type, so you can try to collect lots of treasure in several different lands!

Some of these lands are available from the beginning, others can be found only if you have proven yourself (press ESC to know what you should do to access more lands). Ultimately, you will be able to access Hell, and look for the fabulous Orbs of Yendor which are hidden there. Get at least one of them to prove that you are a true winner! Only the best players will be able to face the challenges of Hell, but both the monsters and the challenges associated with the Orbs of Yendor are different than in the rest of the game, so it should be worth it.

You can see tooltips by placing the mouse over stuff. You can also right click to get more information about game objects.

Press v to configure the game. You can also rotate the world by pressing arrow keys, PageUp and PageDn (not numpad). You can center on the PC by pressing Home.

Your scores and other stats about your games are recorded to file hyperrogue.log (in Linux, *~/.hyperrogue.log*). You can save your configation to file hyperrogue.ini (in Linux, *~/.hyperrogue.ini*).

The surface the game is played on is called a hyperbolic plane. It seems there is just a very small amount of games and other works of art which use hyperbolic geometry (the most well known are some works of M.C.Escher).

The game dynamically generates new parts of the world as you move. Due to nature of the hyperbolic plane, the chances that you get back to a place where you have been before are very low (unless you go back exactly the same way). See more information about the geometry used [on the blog](http://zenorogue.blogspot.com/2012/03/hyperbolic-geometry-in-hyperbolic-rogue.html).

You can aim for the Orb of Yendor, or you can simply aim for as high score as possible. My best so far is $175 but it should be beatable (I mean, I have won fairly only once, and that was my end score in the winning game).

## Technical information ##

You should be able to compile on Ubuntu (or similar) with something like this (you need SDL, SDL_ttf, and SDL_gfx):

```
sudo apt-get install gcc libsdl1.2-dev libsdl-ttf2.0-dev libsdl-gfx1.2-dev
git clone https://github.com/zenorogue/hyperrogue.git hyperrogue
cd hyperrogue
autoreconf -vi
./configure
make
```

There is also a standalone Makefile for Linux, OSX, and MinGW.

```
git clone https://github.com/zenorogue/hyperrogue.git
cd hyperrogue
make -f Makefile.simple
```
