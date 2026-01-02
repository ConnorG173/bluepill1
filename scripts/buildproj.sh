#!/usr/bin/env bash
set -euo pipefail

# arg 1 = source file dir
# arg 2 = .o file dir

./compco.sh $1 $2
./newbuild.sh $2

echo "Project Built"
