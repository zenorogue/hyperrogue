set -e o pipefail
source .github/workflows/gh_ci_envvars.sh


GH_DEPS_UBUNTU="$GH_COMPILER libsdl1.2-dev libsdl-ttf2.0-dev libsdl-gfx1.2-dev libsdl-mixer1.2-dev"
GH_DEPS_MACOS="sdl sdl_gfx sdl_mixer sdl_ttf"
GH_DEPS_MINGW64="$GH_COMPILER:x SDL:x SDL_ttf:x SDL_gfx:x SDL_mixer:x"

if [[ "$GH_HYP_GLEW" == "glew_1" ]]; then
  GH_DEPS_UBUNTU+=" libglew-dev"
  GH_DEPS_MACOS+=" glew"
  GH_DEPS_MINGW64+=" glew:x"
fi

if [[ "$GH_HYP_PNG" == "png_1" ]]; then
  GH_DEPS_UBUNTU+=" libpng-dev"
  GH_DEPS_MACOS+=" libpng"
  GH_DEPS_MINGW64+=" libpng:x"
fi

if [[ "$GH_BUILDSYS" == "autotools" ]]; then
  GH_DEPS_UBUNTU+=" autoconf"
  GH_DEPS_MACOS+=" automake"
  GH_DEPS_MINGW64+=" automake-wrapper autoconf"
fi

if [[ "$GH_OS" == "ubuntu-latest" ]]; then
  sudo apt-get -y install $GH_DEPS_UBUNTU
elif [[ "$GH_OS" == "macos-latest" ]]; then
  brew update
  # macos-latest already has both gcc and clang
  brew install $GH_DEPS_MACOS
  # work around https://stackoverflow.com/questions/51034399/ for now
  (cd /usr/local/include && ln -sf SDL/SDL.h)
elif [[ "$GH_OS" == "windows-latest" ]]; then
  pacboy -Sy --noconfirm --needed $GH_DEPS_MINGW64
  sed -i'.orig' 's/<SDL.h>/"SDL.h"/' /mingw64/include/SDL/SDL_gfxPrimitives.h
else
  exit 'unknown OS'
fi
