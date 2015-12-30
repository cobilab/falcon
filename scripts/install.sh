#!/bin/bash
git clone https://github.com/pratas/falcon.git
cd falcon/src/
cmake .
make
cp FALCON ../../
cd ../../
