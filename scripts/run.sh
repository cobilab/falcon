#!/bin/bash
#==============================================================================
# RUNNING: . run.sh sequence.fq 
#==============================================================================
THREADS="4";
TOP="4";
SAMPLING="1";
CACHEHASH="20";
LEVEL="31";
#==============================================================================
. install.sh
perl download.pl
(time ./FALCON -v -l $LEVEL -c $CACHEHASH -p $SAMPLING -t $TOP -n $THREADS $1 viruses.fa ) &> REPORT
