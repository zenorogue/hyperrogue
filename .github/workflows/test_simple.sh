set -x -e o pipefail


if [[ "$GH_OS" == "windows-latest" && "$GH_BUILDSYS" == "mymake" ]]; then

cat << ENDOFCMDS > .github/workflows/gdb_cmds.txt
  run --version
  backtrace
  exit 1
ENDOFCMDS

  echo not running gdb -- not working currently
  echo gdb --batch -x .github/workflows/gdb_cmds.txt ./hyperrogue

else
  ./hyperrogue --version
fi
