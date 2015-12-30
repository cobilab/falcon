#!/bin/bash
# 
# AN EXAMPLE SHOWING THE DETECTION OF PATHOGENS IN THE NEANDERTHAL GENOME.
#
# WARNING: IT USES ~22 GB OF RAM, 16 THREADS and at least 300 GB OF DISK.
#
# INSTALL FALCON ==============================================================
git clone https://github.com/pratas/falcon.git
cd falcon/src/
cp Makefile.linux Makefile
make
cp FALCON ../../
cd ../../
cp falcon/scripts/DownloadBacteria.pl .
cp falcon/scripts/GetHumanNean.sh .
rm -fr falcon/
# GET NENADERTHAL =============================================================
. GetHumanNean.sh
# GET VIRUSES =================================================================
perl DownloadBacteria.pl
# RUN FALCON ==================================================================
for((x=1 ; x<=56 ; ++x));
  do
  (time ./FALCON -v -l 36 -F -p 1 -t 20000 -n 16 -c 50 -x TOP$x HN-C$x \
  Bacteria.fa) &> REPORT$x;
  done
#
#==============================================================================
