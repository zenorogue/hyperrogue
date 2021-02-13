set -x -e o pipefail


if [[ "$GH_OS" == "windows-latest" && "$GH_BUILDSYS" == "mymake" ]]; then

cat << ENDOFCMDS > .github/workflows/gdb_cmds.txt
  run --version
  quit 1
ENDOFCMDS

  gdb --batch -x .github/workflows/gdb_cmds.txt ./hyperrogue
else
  ./hyperrogue --version
fi
