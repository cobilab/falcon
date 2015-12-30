#!/bin/bash
git clone https://github.com/pratas/falcon.git
cd falcon/src/
cmake .
make
cd ../../
cp falcon/src/FALCON .
cp falcon/scripts/*.sh .
cp falcon/scripts/*.pl .
rm -fr falcon/
