#!/bin/bash
#
# ===========----------------------------------
# | WARNING | THIS REQUIRES 1 TB OF FREE DISK |
# ===========----------------------------------
#
# DEPENDECY: none;
#
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
EVANM="http://cdna.eva.mpg.de";
EVAPT="$EVANM/denisova/raw_reads";
WGETO=" --trust-server-names -q ";
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#
echo "Downloading sequences ... (This may take a while!)";
wget $WGETO $EVAPT/B1087_SR.txt.gz -O X1.gz
wget $WGETO $EVAPT/B1088_SR.txt.gz -O X2.gz
wget $WGETO $EVAPT/B1101_SR.txt.gz -O X3.gz
wget $WGETO $EVAPT/B1102_SR.txt.gz -O X4.gz
wget $WGETO $EVAPT/B1107_SR.txt.gz -O X5.gz
wget $WGETO $EVAPT/B1108_SR.txt.gz -O X6.gz
wget $WGETO $EVAPT/B1109_SR.txt.gz -O X7.gz
wget $WGETO $EVAPT/B1110_SR.txt.gz -O X8.gz
wget $WGETO $EVAPT/B1128_SR.txt.gz -O X9.gz
wget $WGETO $EVAPT/B1130_SR.txt.gz -O X10.gz
wget $WGETO $EVAPT/B1133_SR.txt.gz -O X11.gz
wget $WGETO $EVAPT/SL3003_SR.txt.gz -O X12.gz
wget $WGETO $EVAPT/SL3004_SR.txt.gz -O X13.gz
for((x=1 ; x<14 ; ++x)); 
  do
  gunzip X$x.gz
  done
# - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
