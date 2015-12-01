#!/bin/bash
rm -fr master.zip falcon-master/
wget https://github.com/pratas/falcon/archive/master.zip
unzip master.zip
cd falcon-master/src
cp Makefile.linux Makefile
make
cp FALCON ../../
cd ../../
