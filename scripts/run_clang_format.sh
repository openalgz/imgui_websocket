#!/usr/bin/env bash
#!/bin/bash

##############################################################################
# Runs clang-format on the entire codebase or some files/directories
##############################################################################

# USAGE:
#
# Format entire codebase (.cpp, .c, and .h files):
# $ ./run_clang_format.sh
#
# Format specific directory (recursively finds .cpp, .c, and .h files):
# $ ./run_clang_format.sh ../cpp/src/math
#
# Format specific files:
# $ ./run_clang_format.sh ../cpp/src/math/*.cpp
#
# INSTALLATION:
#
# - MacOS: install Homebrew (https://brew.sh/), then `brew install clang-format`.
#
# N.B. If this script fails because of its usage of `xargs` or `find`, contact
# MFT authors for recommendations on how to upgrade these Unix commands.
#
# This script will apply clang-format to .cpp, .c, and .h files. It uses four
# cores to speed up the process.

# Get root of codebase
ROOT="$( cd "$( dirname "${BASH_SOURCE[0]}" )"/.. && pwd )"

# Helper function
function recursivelyFormat {
  find $1 \( -name "*.cpp" -o -name "*.h" -o -name "*.c" -o -name "*.hpp" \) -not -path "*/build/*" -not -path "*/CMakeFiles/*"  -not -path "*/.git/*" -not -path "*/protos/*" -print0 | xargs -P 4 -n 10 -0 clang-format -style=file -i
}

# If no arguments, run on the entire codebase
if [[ "$#" -eq 0 ]]; then
  echo "Clang-formatting entire codebase in $ROOT"
  recursivelyFormat $ROOT
else
  echo "Clang-formatting specified inputs"
  for filename in "$@"
  do
    if [[ -d $filename ]]; then
      recursivelyFormat $filename
    elif [[ -f $filename ]]; then
      clang-format -style=file -i $filename
    fi
  done
fi

