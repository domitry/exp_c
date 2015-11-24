#!/bin/sh
src="../ex${1}.c"
dst="./ex${1}.out"

cd compiled
gcc $src -o $dst -lm -lGL -lglut -Wall
echo "builded :)"

$dst
echo "done :-)"
cd ..
