#!/bin/bash
# 
# AN EXAMPLE SHOWING THE DETECTION OF PATHOGENS IN THE DENISOVAN.
#
# WARNING: THIS MODE USES: ~40 GB OF RAM, 16 THREADS, ~1.1 TB OF DISK.
#
# INSTALL FALCON ==============================================================
git clone https://github.com/pratas/falcon.git
cd falcon/src/
cmake .
make
cp FALCON ../../
cd ../../
cp falcon/scripts/downloadBacteria.pl .
cp falcon/scripts/GetHumanDeni.sh .
rm -fr falcon/
# GET DENISOVAN ===============================================================
. GetHumanDeni.sh
# GET VIRUSES =================================================================
perl downloadBacteria.pl
# RUN FALCON ==================================================================
for((x=1 ; x<=13 ; ++x));
  do
  (time ./FALCON -v -l 38 -p 1 -t 40000 -n 16 -c 70 -x TOP$x DX$x \
  Bacteria.fa) &> REPORT$x;
  done
#
#==============================================================================
