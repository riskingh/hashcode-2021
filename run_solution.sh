#!/usr/bin/env bash

set -euo pipefail

mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ../cpp
make -j4

mkdir -p  ../output/$2
./hashcode ../input/a.txt ../output/$2/a.txt $1
./hashcode ../input/b.txt ../output/$2/b.txt $1
./hashcode ../input/c.txt ../output/$2/c.txt $1
./hashcode ../input/d.txt ../output/$2/d.txt $1
./hashcode ../input/e.txt ../output/$2/e.txt $1
./hashcode ../input/f.txt ../output/$2/f.txt $1

echo "Done in output/$2"
ls ../output/$2
rm -rf build
