#!/bin/sh
dst="./ex${1}.out"

cd compiled
$dst
echo "done :-)"
cd ..
