set -x -e o pipefail


GH_MYMAKE_ARGS="-fPIC"

HYPERROGUE_USE_GLEW=$GH_HYP_GLEW
export HYPERROGUE_USE_GLEW=${HYPERROGUE_USE_GLEW: -1}

HYPERROGUE_USE_PNG=$GH_HYP_PNG
export HYPERROGUE_USE_PNG=${HYPERROGUE_USE_PNG: -1}

HYPERROGUE_USE_ROGUEVIZ=$GH_HYP_RVIZ
export HYPERROGUE_USE_ROGUEVIZ=${HYPERROGUE_USE_ROGUEVIZ: -1}
if [[ "$GH_HYP_RVIZ" == "rviz_1" ]]; then
  GH_MYMAKE_ARGS+=" -std=c++17 -rv"
fi

export CC=$GH_COMPILER
export CXX=${CC%cc}++
export CXXFLAGS_EARLY=-Werror

if [[ "$GH_BUILDSYS" == "makefile" ]]; then
  make
elif [[ "$GH_BUILDSYS" == "mymake" ]]; then
  make mymake
  ./mymake $GH_MYMAKE_ARGS
  mv hyper hyperrogue
else
  echo 'unknown build system'
  exit 1
fi
