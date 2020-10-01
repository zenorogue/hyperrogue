set -x -e o pipefail


GH_DEPS_UBUNTU="$GH_COMPILER libsdl1.2-dev libsdl-ttf2.0-dev libsdl-gfx1.2-dev libsdl-mixer1.2-dev"
GH_DEPS_MACOS="sdl sdl_gfx sdl_mixer sdl_ttf" # macos-latest already has both gcc and clang
GH_DEPS_MINGW64="$GH_COMPILER:x SDL:x SDL_ttf:x SDL_gfx:x SDL_mixer:x make gdb:x"

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
  # As of 2020-09-30, brew update sometimes times out on Github CI, so we don't do it here
  # it's also arguably not necessary because VM images already contain recent-ish homebrew database
  brew install $GH_DEPS_MACOS
  # work around https://stackoverflow.com/questions/51034399/ for now
  (cd /usr/local/include && ln -sf SDL/SDL.h)
elif [[ "$GH_OS" == "windows-latest" ]]; then
  pacboy -Sy --noconfirm --needed $GH_DEPS_MINGW64
  sed -i'.orig' 's/<SDL.h>/"SDL.h"/' /mingw64/include/SDL/SDL_gfxPrimitives.h
else
  echo 'unknown OS'
  exit 1
fi
