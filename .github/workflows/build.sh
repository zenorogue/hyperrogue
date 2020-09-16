set -e o pipefail
source .github/workflows/gh_ci_envvars.sh


GH_MYMAKE_ARGS=""
GH_AUTOTOOLS_CXXFLAGS="-W -Wall -Wextra -Wno-unused-parameter -Wno-maybe-uninitialized"

HYPERROGUE_USE_GLEW=$GH_HYP_GLEW
export HYPERROGUE_USE_GLEW=${HYPERROGUE_USE_GLEW: -1}

HYPERROGUE_USE_PNG=$GH_HYP_PNG
export HYPERROGUE_USE_PNG=${HYPERROGUE_USE_PNG: -1}

HYPERROGUE_USE_ROGUEVIZ=$GH_HYP_RVIZ
export HYPERROGUE_USE_ROGUEVIZ=${HYPERROGUE_USE_ROGUEVIZ: -1}
if [[ "$GH_HYP_RVIZ" == "rviz_1" ]]; then
  GH_MYMAKE_ARGS+=" -rv"
  GH_AUTOTOOLS_CXXFLAGS+=" -DCAP_ROGUEVIZ=1"
fi

export CC=$GH_COMPILER
export CXX=${CC%cc}++

if [[ "$GH_BUILDSYS" == "makefile" ]]; then
  make -f Makefile.simple
elif [[ "$GH_BUILDSYS" == "autotools" ]]; then
  autoreconf -vfi
  ./configure CXXFLAGS="${GH_AUTOTOOLS_CXXFLAGS}"
  make
elif [[ "$GH_BUILDSYS" == "mymake" ]]; then
  make -f Makefile.simple mymake
  ./mymake $GH_MYMAKE_ARGS
  mv hyper hyperrogue
else
  exit 'unknown build system'
fi
