#!/bin/bash
# 
# AN EXAMPLE SHOWING THE DETECTION OF PATHOGENS IN THE NEANDERTHAL GENOME.
#
# INSTALL FALCON ==============================================================
git clone https://github.com/pratas/falcon.git
cd falcon/src/
cp Makefile.linux Makefile
make
cp FALCON ../../
cd ../../
cp falcon/scripts/downloadBacteria.pl .
cp falcon/scripts/GetHumanNean.sh .
rm -fr falcon/
# GET NENADERTHAL =============================================================
. GetHumanNean.sh
# GET VIRUSES =================================================================
perl downloadBacteria.pl
# RUN FALCON ==================================================================
for((x=1 ; x<=56 ; ++x));
  do
  (time ./FALCON -v -l 36 -p 1 -t 10000 -n 16 -c 50 -x TOP$x HN-C$x \
  Bacteria.fa) &> REPORT$x;
  done
#
#==============================================================================
