#!/bin/bash
. install.sh
. GetHumanCHMParse.sh
rm -f CHM;
for((x=1; x<=23 ; ++x)); do cat CHM$x >> CHM; rm -f CHM$x; done
(time ./FALCON -l 38 -c 90 -n 1 -t 200000 -x TOP-CHM CHM Bacteria.fa ) &> REPORT-CHM
