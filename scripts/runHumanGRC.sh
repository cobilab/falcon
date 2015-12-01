#!/bin/bash
#==============================================================================
# RUNNING: . runHumanGRC.sh 
#==============================================================================
THREADS="4";
TOP="500";
SAMPLING="1";
CACHEHASH="25";
LEVEL="4"; #"31";
#==============================================================================
#. install.sh
#. GetHumanGRCParse.sh
#perl download.pl
for((x=1 ; x<28; ++x));
  do
  echo "Running $x ... ";
  (time ./FALCON -v -l $LEVEL -c $CACHEHASH -p $SAMPLING -t $TOP -n $THREADS \
  -x TOP$x GRC$x viruses.fa ) &> REPORT$x ;
  done
#==============================================================================
