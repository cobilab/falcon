#!/bin/bash
#==============================================================================
# RUNNING: . runHumanCHM.sh 
#==============================================================================
THREADS="4";
TOP="500";
SAMPLING="1";
CACHEHASH="25";
LEVEL="31";
#==============================================================================
#. install.sh
#. GetHumanCHMParse.sh
#perl download.pl
for((x=1 ; x<=24; ++x));
  do
  echo "Running $x ... ";
  (time ./FALCON -v -l $LEVEL -c $CACHEHASH -p $SAMPLING -t $TOP -n $THREADS \
  -x TOP$x CHM$x viruses.fa ) &> REPORT$x ;
  done
#==============================================================================
